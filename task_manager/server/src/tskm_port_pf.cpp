/*
 * @copyright Copyright (c) 2016-2020 TOYOTA MOTOR CORPORATION.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "tskm_port_pf.h"
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <pwd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

#include <native_service/cl_process.h>
#include <native_service/cl_monitor.h>
#include <system_service/ss_services.h>
#include <system_service/ss_sm_client_if.h>

#include "system_service/tskm_svc.h"
#include "tskm_debug.h"

#define TSKM_PORTPF_IN() TSKM_PRINTF(TSKM_LOG_PORTPF, "%s:IN", __FUNCTION__)
#define TSKM_PORTPF_IN_ARG(format, ...) TSKM_PRINTF(TSKM_LOG_PORTPF, "%s:IN:" format, __FUNCTION__, __VA_ARGS__)
#define TSKM_PORTPF_OUT() TSKM_PRINTF(TSKM_LOG_PORTPF, "%s:OUT", __FUNCTION__)

#define TSKM_PRIO_MIN            1
#define TSKM_PRIO_MAX           99
#define TSKM_PRIO_MIN_TSS      -20
#define TSKM_PRIO_MAX_TSS       19

#define TSKM_TMP_DIRECTORY         "/tmp"
#define TSKM_TMP_TSKM_DIRECTORY    TSKM_TMP_DIRECTORY "/tskm"
#define TSKM_TMP_NV_INFO_FILEPATH  TSKM_TMP_TSKM_DIRECTORY "/nvInfo"

#define TSKM_NPP_NV_FILE_TAG       SS_TASK_MANAGER "/nvInfo"

/*********************************************************
 *        user->uid/gid conversion
 *********************************************************/
TSKM_STATIC int cnvUserName2UidGid(const char *user, uid_t *uid, gid_t *gid) {
  int ret = -1;

  if (0 == strcmp(user, "")) {
    *uid = geteuid();
    *gid = getegid();
  } else {
    static __thread size_t bufSize = 0;
    static __thread char *buf = NULL;
    struct passwd pwd;
    struct passwd *result;

    if (0 == bufSize) {
      struct stat statInfo;

      bufSize = -1;

      if (0 != stat("/etc/passwd", &statInfo) || 0 >= statInfo.st_size) {  // LCOV_EXCL_START 8: Because buf is a static variable and cannot be passed test
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        TSKM_ASSERT_ERRNO(0);
        goto ERROR;
      }
      // LCOV_EXCL_STOP

      bufSize = statInfo.st_size * 2;

      // Since TaskManager is a resident service, the area allocated here is expected to be released at process termination and is not explicitly released by free()
      buf = (char *) malloc(sizeof(char) * bufSize);  // NOLINT (runtime/printf)
      if (NULL == buf) {  // LCOV_EXCL_BR_LINE 5: Error branching of the standard-function malloc
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        TSKM_ASSERT_ERRNO(0);  // LCOV_EXCL_LINE 5: Error branching of the standard-function malloc
        goto ERROR;  // LCOV_EXCL_LINE 5: Error branching of the standard-function malloc
      }
    }

    if (NULL == buf) {  // LCOV_EXCL_BR_LINE 8: Because buf is a static variable and cannot be passed test
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      TSKM_ASSERT(0);  // LCOV_EXCL_LINE 8: Because buf is a static variable and cannot be passed test
      goto ERROR;  // LCOV_EXCL_LINE 8: Because buf is a static variable and cannot be passed test
    }

    ret = getpwnam_r(user, &pwd, buf, bufSize, &result);
    if (ret != 0 || result == NULL) {
      TSKM_ASSERT_PRINT(0, "ret = %d[%s]", ret, user);
      goto ERROR;
    }

    *uid = pwd.pw_uid;
    *gid = pwd.pw_gid;
  }

  ret = 0;

  ERROR:

  return ret;
}

/*********************************************************
 *        Convert Scheduling Policy Definitions Values (TSKM -> CL)
 *********************************************************/
TSKM_STATIC CL_ProcessSchedPolicy_t cnvTskmPolicy2ClPolicy(
    TSKM_SVC_POLICY_t policy) {
  switch (policy) {
    case TSKM_SVC_POLICY_FIFO:
      return CL_PROCESS_SCHED_POLICY_FIFO;
    case TSKM_SVC_POLICY_TSS:
      return CL_PROCESS_SCHED_POLICY_OTHER;
    case TSKM_SVC_POLICY_RR:
      return CL_PROCESS_SCHED_POLICY_RR;
    default:
      TSKM_ASSERT(0);
      break;
  }
  return CL_PROCESS_SCHED_POLICY_OTHER;
}

/*********************************************************
 *        Process priority valid value determination
 *********************************************************/
TSKM_STATIC int chkPrioValue(TSKM_SVC_POLICY_t policy, int prio) {
  switch (policy) {
    case TSKM_SVC_POLICY_FIFO:
    case TSKM_SVC_POLICY_RR:
      if ((TSKM_PRIO_MIN > prio) || (prio > TSKM_PRIO_MAX)) {
        TSKM_ASSERT(0);
      }
      break;
    case TSKM_SVC_POLICY_TSS:
    default:
      if ((TSKM_PRIO_MIN_TSS > prio) || (prio > TSKM_PRIO_MAX_TSS)) {
        TSKM_ASSERT(0);
      }
      break;
  }

  return prio;
}

/*********************************************************
 *        COMMONLIB initialization (at system startup)
 *********************************************************/
int tskm_pf_sysInit() {
  int fd = 0;
  int ret = 0;

  TSKM_PORTPF_IN();

  fd = CL_ProcessInit();
  if (fd == -1) {  // LCOV_EXCL_BR_LINE 6: Error branching during process initialization
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT_ERRNO(0);  // LCOV_EXCL_LINE 6: Error branching during process initialization
    goto ERROR;  // LCOV_EXCL_LINE 6: Error branching during process initialization
  }

  ret = CL_MonitorInit(CL_MONITOR_INIT_USER);
  if (ret != 0) {   // LCOV_EXCL_BR_LINE 4: NSFW error case.
    TSKM_ASSERT_ERRNO(0);
  }

  TSKM_PORTPF_OUT();
  return fd;
  // LCOV_EXCL_START 6: Error branching during process initialization
  ERROR:
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  return -1;
  // LCOV_EXCL_STOP
}

/*********************************************************
 *        COMMONLIB initialization (at process startup)
 *********************************************************/
int tskm_pf_procInit() {
  TSKM_PORTPF_IN();
  // Originally called the _sys_Setup_CWORD64_API (NULL), but changed to empty function along to derete "_sys"

  TSKM_PORTPF_OUT();
  return 0;
}

/*********************************************************
 *        Process startup
 *********************************************************/
pid_t tskm_pf_createProc(TSKM_SVC_ATTR_t* p_svcAttr) {
  TSKM_PORTPF_IN();
  pid_t retPid = -1;
  pid_t pid = -1;

  CL_ProcessAttr_t attr;
  char procPath[255];
  char* procName;
  uid_t setUid = 0;
  gid_t setGid = 0;

  if (0 != CL_ProcessCreateAttrInit(&attr)) {  // LCOV_EXCL_BR_LINE 6: Error branching during process initialization
    // LCOV_EXCL_START 6: Error branching during process initialization
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT(0);
    goto ERROR;
    // LCOV_EXCL_STOP
  }

  // Process name setting(from basename)
  if (strlen(p_svcAttr->path) < sizeof(procPath)) {
    strcpy(procPath, p_svcAttr->path);  // NOLINT (runtime/printf)
  } else {
    TSKM_ASSERT(0);
    goto ERROR;
  }

  procName = basename(procPath);
  if (strlen(procName) >= 16) {
    TSKM_ASSERT(0);
    procName[16] = '\0';
  }

  if (0 != CL_ProcessCreateAttrSetName(&attr, procName)) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT_ERRNO(0);
    goto ERROR;
    // LCOV_EXCL_STOP
  }

  if (0 != cnvUserName2UidGid(p_svcAttr->user, &setUid, &setGid)) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT(0);
    setUid = geteuid();
    setGid = getegid();
    // LCOV_EXCL_STOP
  }

  if (0 != CL_ProcessCreateAttrSetUid(&attr, setUid)) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT_ERRNO(0);
    goto ERROR;
    // LCOV_EXCL_STOP
  }

  if (0 != CL_ProcessCreateAttrSetGid(&attr, setGid)) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT_ERRNO(0);
    goto ERROR;
    // LCOV_EXCL_STOP
  }

  // All are group leaders in order to recover to the child processes of the service
  if (0 != CL_ProcessCreateAttrSetGroup(&attr, 1)) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT_ERRNO(0);
    goto ERROR;
    // LCOV_EXCL_STOP
  }
  if (0 != CL_ProcessCreateAttrSetCpuAssign(&attr, p_svcAttr->cpuAssign)) {
    TSKM_ASSERT_ERRNO(0);
    goto ERROR;
  }

  if (0
      != CL_ProcessCreateAttrSetSchedule(
          &attr, cnvTskmPolicy2ClPolicy(p_svcAttr->policy),
          chkPrioValue(p_svcAttr->policy, p_svcAttr->prio))) {
    TSKM_ASSERT_ERRNO(0);
    goto ERROR;
  }

  pid = CL_ProcessCreate(p_svcAttr->path, p_svcAttr->args, NULL, &attr);
  if (pid == -1) {
    TSKM_ASSERT_ERRNO(0);
    goto ERROR;
  }

  retPid = pid;
  ERROR:

  TSKM_PORTPF_OUT();
  return retPid;
}

/*********************************************************
 *        Recover termination processes
 *********************************************************/
int tskm_pf_cleanupProc(int sigFd, pid_t* p_pid, TSKM_ERR_t* p_err) {
  int ret;
  CL_ProcessCleanupInfo_t info;

  ret = CL_ProcessCleanup(sigFd, &info);
  if (ret != 0 && ret != 1) {
    TSKM_ASSERT_PRINT(0, "ret = %d", ret);
    TSKM_ASSERT_ERRNO(0);
    goto ERROR;
  }

  TSKM_PRINTF(TSKM_LOG_STATE, "SVC TERM:pid:%d code:%d status:%d", info.pid,
              info.code, info.status);

  switch (info.code) {
    case CLD_STOPPED:
      // Not comes here usually
      TSKM_PRINTF(TSKM_LOG_STATE, "svc stoped\n");
      goto ERROR;
    case CLD_TRAPPED:
      // Comes here only during debugging
      TSKM_PRINTF(TSKM_LOG_STATE, "svc traped\n");
      goto ERROR;
    case CLD_CONTINUED:
      // Not comes here usually
      TSKM_PRINTF(TSKM_LOG_STATE, "svc continue\n");
      goto ERROR;
    default:
      break;
  }

  *p_pid = info.pid;
  *p_err =
      (info.code != CLD_EXITED) ? TSKM_E_NG :
      (info.code == CLD_EXITED && info.status != EXIT_SUCCESS) ?
          TSKM_E_NG : TSKM_E_OK;

  if (TSKM_E_OK != *p_err) {
    TSKM_PRINTF(TSKM_LOG_SYSTEMDATA, "SVC ERR TERM:pid:%d code:%d status:%d",
                info.pid, info.code, info.status);
  }

  return ret;
  ERROR: return -1;
}

/*********************************************************
 *        Force Terminating a Process Group
 *********************************************************/
int tskm_pf_terminateProcGroup(uint16_t pid) {
  int ret;
  pid_t pgid;

  pgid = getpgid(pid);
  if (pgid < 0) {
    TSKM_ASSERT(0);
    goto ERROR;
  }

  TSKM_PRINTF(TSKM_LOG_STATE, "TERM SVC GROUP:pid:%dpgid:%d", pid, pgid);

  ret = CL_ProcessEuthanizeGroup(pgid);
  if (ret != 0) {
    TSKM_ASSERT_ERRNO(0);
    goto ERROR;
  }

  return 0;
  ERROR: return -1;
}

/*********************************************************
 *        Opening shared memory and trancating
 *********************************************************/
int tskm_pf_shmCreate_Open_Ftrn(const char* name, int32_t size, void** p_addr) {
  int ret = 0;
  int fd = 0;

  fd = shm_open(name, O_CREAT | O_RDWR | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
  if (fd == -1) {  // LCOV_EXCL_BR_LINE 5: Checked in Death testing, but it is not reflected in the coverage and excluded.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT_ERRNO(0);  // LCOV_EXCL_LINE 5: Checked in Death testing, but it is not reflected in the coverage and excluded.
    ret = -1;  // LCOV_EXCL_LINE 5: Checked in Death testing, but it is not reflected in the coverage and excluded.
    if (fd > 0) {  // LCOV_EXCL_LINE 5: Checked in Death testing, but it is not reflected in the coverage and excluded.
      TSKM_ASSERT_ERRNO(close(fd) == 0);  // LCOV_EXCL_LINE 5: Checked in Death testing, but it is not reflected in the coverage and excluded.
    }  // LCOV_EXCL_LINE 5: Checked in Death testing, but it is not reflected in the coverage and excluded.
    return ret;  // LCOV_EXCL_LINE 5: Checked in Death testing, but it is not reflected in the coverage and excluded.
  }

  ret = ftruncate(fd, size);
  if (ret != 0) {  // LCOV_EXCL_BR_LINE 5: Checked in Death testing, but it is not reflected in the coverage and excluded.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT_ERRNO(0);  // LCOV_EXCL_LINE 5: Checked in Death testing, but it is not reflected in the coverage and excluded.
    if (fd > 0) {  // LCOV_EXCL_LINE 5: Checked in Death testing, but it is not reflected in the coverage and excluded.
      TSKM_ASSERT_ERRNO(close(fd) == 0);  // LCOV_EXCL_LINE 5: Checked in Death testing, but it is not reflected in the coverage and excluded.
    }  // LCOV_EXCL_LINE 5: Checked in Death testing, but it is not reflected in the coverage and excluded.
    return ret;  // LCOV_EXCL_LINE 5: Checked in Death testing, but it is not reflected in the coverage and excluded.
  }

  if (p_addr) {  // LCOV_EXCL_BR_LINE 8: Because p_addr is set to NULL only
    // LCOV_EXCL_START 8: Because p_addr is set to NULL only
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    // Not supported
    TSKM_ASSERT_ERRNO(0);
    ret = -1;
    if (fd > 0) {
      TSKM_ASSERT_ERRNO(close(fd) == 0);
    }
    return ret;
    // LCOV_EXCL_STOP
  }

  ret = 0;
  if (fd > 0) {
    TSKM_ASSERT_ERRNO(close(fd) == 0);
  }
  return ret;
}

/*********************************************************
 *        Creating shared memory
 *********************************************************/
int tskm_pf_shmCreate(const char* name, int32_t size, void** p_addr) {
  TSKM_PORTPF_IN_ARG("%s,%d", name, size);
  struct stat statInfo;
  int ret = -1;

  if (stat(name, &statInfo) == 0) {
    TSKM_ASSERT_PRINT(0, "Already Exist %s", name);
    ret = 0;  // To be Succeeded
    TSKM_PORTPF_OUT();
    return ret;
  }

  ret = tskm_pf_shmCreate_Open_Ftrn(name, size, p_addr);

  TSKM_PORTPF_OUT();
  return ret;
}

/*********************************************************
 *        Deleting shared memory
 *********************************************************/
int tskm_pf_shmDelete(const char* name) {
  TSKM_PORTPF_IN_ARG("%s", name);
  int ret = 0;

  if (0 != shm_unlink(name)) {
    TSKM_ASSERT_ERRNO(0);
  }

  TSKM_PORTPF_OUT();
  return ret;
}

/*********************************************************
 *        Starting thread
 *********************************************************/
int tskm_pf_createThread(void* (*threadFunc)(void*), void* prm,
                         uint32_t priority, const char* threadName,
                         pthread_t* p_thId) {
  TSKM_PORTPF_IN();

  int ret;
  CL_ThreadAttr_t cl_attr;

  ret = CL_ThreadCreateAttrInit(&cl_attr);
  if (ret != 0) {  // LCOV_EXCL_BR_LINE 6: For processing initializing process
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT_ERRNO(0);  // LCOV_EXCL_LINE 6: For processing initializing process
    goto ERROR;  // LCOV_EXCL_LINE 6:  For processing initializing process
  }

  if (threadName) {
    ret = CL_ThreadCreateAttrSetName(&cl_attr, threadName);
    TSKM_ASSERT_ERRNO(ret == 0);
  }

  ret = CL_ThreadCreate(p_thId, NULL, &cl_attr, threadFunc, prm);
  if (ret != 0) {  // LCOV_EXCL_BR_LINE 6: For processing initializing process
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT_ERRNO(0);  // LCOV_EXCL_LINE 6:  For processing initializing process
    goto ERROR;  // LCOV_EXCL_LINE 6:  For processing initializing process
  }
  int sched_policy;
  struct sched_param sparam;

  pthread_getschedparam(*p_thId, &sched_policy, &sparam);

  sparam.sched_priority = priority;
  if ((priority != 0) && (sched_policy == SCHED_OTHER)) {
    sched_policy = SCHED_FIFO;
  } else if ((priority == 0) && (sched_policy != SCHED_OTHER)) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    sched_policy = SCHED_OTHER;  // LCOV_EXCL_LINE 6:  For processing initializing process
  }

  ret = pthread_setschedparam(*p_thId, sched_policy, &sparam);
  TSKM_ASSERT(ret == 0);
  TSKM_PORTPF_OUT();
  return 0;
  ERROR: return -1;
}
/*********************************************************
 *        Send Stop Complete Response
 *********************************************************/
int tskm_pf_sendStopCompResp() {
  EFrameworkunifiedStatus l_eStatus;
  TSKM_PORTPF_IN();

  l_eStatus = SendInterfaceunifiedOnStopResponseToSystemManager(eFrameworkunifiedStatusOK);
  if (l_eStatus != eFrameworkunifiedStatusOK) {
    TSKM_ASSERT(0);
  }

  TSKM_PORTPF_OUT();
  return 0;
}

/*********************************************************
 *        ABORT
 *********************************************************/
int tskm_pf_abort() {
  TSKM_PORTPF_IN();

  TSKM_ASSERT(0);
  sleep(1);   // Sleep a little so that the logging at the time of abnormality may be interrupted

  // Issure SIGSEG
  printf("%d\n", *(int *) 0);  // NOLINT (readability/casting)

  TSKM_PORTPF_OUT();
  return 0;
}

/*********************************************************
 *        EXIT
 *********************************************************/
int tskm_pf_exit(int status) {  //LCOV_EXCL_START 5: Checked in Death testing, but it is not reflected in the coverage and excluded
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  TSKM_PORTPF_IN();

  exit(status);

  TSKM_PORTPF_OUT();
  return 0;
}
// LCOV_EXCL_STOP

/*********************************************************
 *        TOUCH
 *********************************************************/
int tskm_pf_touch(char* path) {
  TSKM_PORTPF_IN();
  int fd;
  fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
  if (fd < 0) {
    TSKM_ASSERT_ERRNO(0);
    goto ERROR;
  }

  TSKM_ASSERT_ERRNO(close(fd) == 0);
  TSKM_PORTPF_OUT();
  return 0;
  ERROR:
  return -1;
}

/*********************************************************
 *        Touch
 *********************************************************/
int tskm_pf_mkTouchFileName(pid_t pid, char name[32]) {
  TSKM_PORTPF_IN();

  sprintf(name, "/tmp/tskm_touch%d", pid);  // NOLINT (runtime/printf)

  TSKM_PORTPF_OUT();
  return 0;
}

/*********************************************************
 *     Read file
 *********************************************************/
static ssize_t readFile(const char* filePath, void* l_buf, ssize_t bufSize) {
  int fd = -1;
  ssize_t readSize, totalReadSize = 0;
  char *p_buf = (char*) l_buf;  // NOLINT (readability/casting)
  fd = open(filePath, O_RDONLY);
  if (fd == -1) {
    TSKM_ASSERT_ERRNO(0);
    goto ERROR;
  }

  do {
    readSize = read(fd, p_buf, bufSize);
    if (readSize == -1) {
      if (errno == EINTR) {
        continue;
      } else {
        TSKM_ASSERT_ERRNO(0);
        goto ERROR;
      }
    } else if (readSize == 0) {
      break;
    }

    p_buf += readSize;
    bufSize -= readSize;
    totalReadSize += readSize;
  } while (bufSize > 0);

  ERROR: if (fd != -1) {
    close(fd);
  }

  return totalReadSize;
}

/*********************************************************
 *     Write to file
 *********************************************************/
static ssize_t writeFile(const char *filePath, const void* l_buf,
                         ssize_t bufSize) {
  int ret = -1;
  int fd = -1;

  const char *p_buf = (const char*) l_buf;

  ssize_t writeSize, totalWriteSize = 0;

  fd = open(filePath, O_RDWR | O_TRUNC | O_CREAT, S_IRWXU);
  if (fd == -1) {  // LCOV_EXCL_BR_LINE 5: Error branching of the standard-function open()
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT_ERRNO(0);  // LCOV_EXCL_LINE 5: Error branching of the standard-function open()
    goto ERROR;  // LCOV_EXCL_LINE 5: Error branching of the standard-function open()
  }

  do {
    writeSize = write(fd, p_buf, bufSize);
    if (writeSize == -1) {  // LCOV_EXCL_BR_LINE 5: Error branching of the standard-function write()
      // LCOV_EXCL_START 5: Error branching of the standard-function write()
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      if (errno == EINTR) {
        continue;
      } else {
        TSKM_ASSERT_ERRNO(0);
        goto ERROR;
      }
      // LCOV_EXCL_STOP
    } else if (writeSize == 0) {  // LCOV_EXCL_BR_LINE 5: Error branching of the standard-function write()
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      break;  // LCOV_EXCL_LINE 5: Error branching of the standard-function write()
    }
    p_buf += writeSize;
    bufSize -= writeSize;
    totalWriteSize += writeSize;
  } while (bufSize > 0);  // LCOV_EXCL_BR_LINE 5: Error branching of the standard-function write()

  ret = fsync(fd);
  if (ret == -1) {  // LCOV_EXCL_BR_LINE 5: Error branching of the standard-function fsync()
    // LCOV_EXCL_START 5: Error branching of the standard-function fsync()
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT_ERRNO(0);
    goto ERROR;
    // LCOV_EXCL_STOP
  }

  ERROR: if (fd != -1) {  // LCOV_EXCL_BR_LINE 5: Error branching of the standard-function open()
    close(fd);
  }

  return totalWriteSize;
}

/*********************************************************
 *     Checksum calculation
 *********************************************************/
static uint32_t calcCheckSum(const void* p_buf, size_t bufSize) {
  int ii;
  int blockNum;
  uint32_t sum = 0;
  uint32_t *p_calcBuf = (uint32_t*) p_buf;  // NOLINT (readability/casting)

  blockNum = static_cast<int>(bufSize) / static_cast<int>(sizeof(uint32_t));

  for (ii = 0; ii < blockNum; ii++) {
    sum += p_calcBuf[ii];
  }

  return sum;
}

/*********************************************************
 *     Checking files
 *********************************************************/
static TSKM_BOOL_t checkFile(const char *filePath) {  // LCOV_EXCL_START 6: Because the condition cannot be set
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  uint32_t checkSum;
  struct stat st;
  ssize_t readSize;
  TSKM_NV_INFO_t nvInfo;
  TSKM_BOOL_t isFileOk = TSKM_FALSE;

  if (stat(filePath, &st) != 0) {
    goto ERROR;
  }

  readSize = readFile(filePath, &nvInfo, sizeof(TSKM_NV_INFO_t));
  if (readSize != sizeof(TSKM_NV_INFO_t)) {
    TSKM_ASSERT(0);
    goto ERROR;
  }

  checkSum = calcCheckSum(&nvInfo, offsetof(TSKM_NV_INFO_t, footer.checkSum));

  if (checkSum != nvInfo.footer.checkSum) {
    TSKM_ASSERT(0);
    goto ERROR;
  }

  if (strncmp(nvInfo.header.version, TSKM_NV_STRUCT_VERSION,
              sizeof(TSKM_NV_STRUCT_VERSION)) != 0) {
    TSKM_ASSERT(0);
    goto ERROR;
  }

  isFileOk = TSKM_TRUE;
  ERROR: return isFileOk;
}
// LCOV_EXCL_STOP
/*********************************************************
 *     Non-volatile file initialization (Including checksum write)
 *********************************************************/
static int initNvFile() {
  int ii;
  ssize_t writeSize;
  TSKM_NV_INFO_t nvInfo;

  TSKM_STATIC_ASSERT(sizeof(TSKM_NV_INFO_t) == TSKM_NV_SIZE_ALL);
  TSKM_STATIC_ASSERT(
      offsetof(TSKM_NV_INFO_t, footer.checkSum) % sizeof(uint32_t) == 0);

  memset(&nvInfo, 0, sizeof(TSKM_NV_INFO_t));
  sprintf(nvInfo.header.version, TSKM_NV_STRUCT_VERSION);  // NOLINT (readability/casting)
  nvInfo.body.rsvSvcNum = 0;
  for (ii = 0; ii < TSKM_SVC_RESERVE_MAX; ii++) {
    nvInfo.body.rsvSvcs[ii] = TSKM_SVCID_NONE;
  }
  nvInfo.footer.checkSum = calcCheckSum(
      &nvInfo, offsetof(TSKM_NV_INFO_t, footer.checkSum));

  writeSize = writeFile(TSKM_TMP_NV_INFO_FILEPATH, &nvInfo,
                        sizeof(TSKM_NV_INFO_t));
  if (writeSize != sizeof(TSKM_NV_INFO_t)) {  // LCOV_EXCL_BR_LINE 6: writeSize must be the size of TSKM_NV_INFO_t
    // LCOV_EXCL_START 6: writeSize must be the size of TSKM_NV_INFO_t
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT(0);
    goto ERROR;
    // LCOV_EXCL_STOP
  }

  return 0;
  ERROR: return -1;
}

/*********************************************************
 *     Non-volatile file initialization
 *********************************************************/
int tskm_pf_nvFileInit(HANDLE hApp) {
  int ret = -1;
  struct stat st;
  EFrameworkunifiedStatus l_eStatus;

  if (stat(TSKM_TMP_TSKM_DIRECTORY, &st) != 0) {  // LCOV_EXCL_BR_LINE 5:Standard C function return value
    // LCOV_EXCL_START 5:Standard C function return value
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    ret = mkdir(TSKM_TMP_TSKM_DIRECTORY, S_IRWXU);
    if (ret == -1) {
      TSKM_ASSERT_ERRNO(0);
      goto ERROR;
    }
    // LCOV_EXCL_STOP
  }

  l_eStatus = FrameworkunifiedNPRegisterPersistentFile(hApp, TSKM_NPP_NV_FILE_TAG);
  if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 6: For processing initializing process
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT(0);  // LCOV_EXCL_LINE 6: For processing initializing process
    goto ERROR;  // LCOV_EXCL_LINE 6: For processing initializing process
  }

  l_eStatus = FrameworkunifiedNPLoadPersistentFile(hApp, TSKM_TMP_NV_INFO_FILEPATH, TSKM_NPP_NV_FILE_TAG);
  if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 6: For processing initializing process
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT(0);  // LCOV_EXCL_LINE 6: For processing initializing process
    goto ERROR;  // LCOV_EXCL_LINE 6: For processing initializing process
  }

  return 0;
  ERROR: return -1;
}

/*********************************************************
 *     Non-volatile file read
 *********************************************************/
int tskm_pf_nvFileRead(HANDLE hApp, TSKM_NV_INFO_t* p_nvInfo) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  NC_LoadPersistedAck l_tMsgAck;
  ssize_t readSize;
  TSKM_BOOL_t isTmpFileOk = TSKM_FALSE;

  if (sizeof(l_tMsgAck) != FrameworkunifiedGetMsgLength(hApp)) {
    TSKM_ASSERT(0);
    goto ERROR;
  } else if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedGetMsgDataOfSize(hApp, &l_tMsgAck, sizeof(l_tMsgAck)))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 4: NSFW error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT(0);
    goto ERROR;
    // LCOV_EXCL_STOP
  }

  if ((strncmp(l_tMsgAck.cTag, TSKM_NPP_NV_FILE_TAG, sizeof(TSKM_NPP_NV_FILE_TAG)) == 0)
      && (eFrameworkunifiedStatusOK == l_tMsgAck.eStatus)) {
    if (checkFile(TSKM_TMP_NV_INFO_FILEPATH) == TSKM_TRUE) {
      isTmpFileOk = TSKM_TRUE;
    }
  }

  if (isTmpFileOk == TSKM_FALSE) {
    int ret;

    ret = initNvFile();
    if (ret == -1) {  // LCOV_EXCL_BR_LINE 6: must return ok
      // LCOV_EXCL_START 6: must return ok
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      TSKM_ASSERT(0);
      goto ERROR;
      // LCOV_EXCL_STOP
    }
  }

  readSize = readFile(TSKM_TMP_NV_INFO_FILEPATH, p_nvInfo,
                      sizeof(TSKM_NV_INFO_t));
  if (readSize != sizeof(TSKM_NV_INFO_t)) {  // LCOV_EXCL_BR_LINE 6: readSize must be the size of TSKM_NV_INFO_t
    // LCOV_EXCL_START 6: readSize must be the size of TSKM_NV_INFO_t
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT(0);
    goto ERROR;
    // LCOV_EXCL_STOP
  }

  l_eStatus = FrameworkunifiedNPReleasePersistentFile(hApp, eFrameworkunifiedPersistOnShutdown, TSKM_NPP_NV_FILE_TAG, TSKM_TMP_NV_INFO_FILEPATH);
  if (eFrameworkunifiedStatusOK != l_eStatus) {
    TSKM_ASSERT(0);
  }

  return 0;
  ERROR: return -1;
}

/*********************************************************
 *     Write non-volatile file (Including checksum)
 *********************************************************/
int tskm_pf_nvFileWrite(const TSKM_NV_INFO_t* p_nvInfo) {
  ssize_t writeSize;
  TSKM_NV_INFO_t nvInfo;

  memcpy(&nvInfo, p_nvInfo, sizeof(TSKM_NV_INFO_t));

  nvInfo.footer.checkSum = calcCheckSum(
      &nvInfo, offsetof(TSKM_NV_INFO_t, footer.checkSum));

  writeSize = writeFile(TSKM_TMP_NV_INFO_FILEPATH, &nvInfo,
                        sizeof(TSKM_NV_INFO_t));
  if (writeSize != sizeof(TSKM_NV_INFO_t)) {  // LCOV_EXCL_BR_LINE 6: writeSize must be the size of TSKM_NV_INFO_t
    // LCOV_EXCL_START 6: writeSize must be the size of TSKM_NV_INFO_t
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT(0);
    return -1;
    // LCOV_EXCL_STOP
  }
  return 0;
}  // LCOV_EXCL_BR_LINE 10: Final line
