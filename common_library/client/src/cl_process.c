/*
 * @copyright Copyright (c) 2016-2019 TOYOTA MOTOR CORPORATION.
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <sched.h>
#include <semaphore.h>
#include <pthread.h>
#include <limits.h>
#include <sys/prctl.h>
#include <sys/signalfd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <native_service/cl_process.h>
#include "cl_process_internal.h"
#include "cl_error.h"
#include "cl_cgroup.h"
#include "cl_monitor_internal.h"

#define assert_static(e) \
  do { \
    enum { assert_static__ = 1/(e) }; \
  } while (0)

struct linux_dirent64 {
  ino64_t        d_ino;
  off64_t        d_off;
  unsigned short d_reclen;
  unsigned char  d_type;
  char           d_name[];
};

/*
 * Initialize the process
 */
int CL_ProcessInit(void) {
  int sfd = -1;
  char *name;
  sigset_t mask;

  assert_static(sizeof(CL_ProcessAttr_t) == sizeof(CL_ProcessAttrInternal_t));

  name = getenv(CL_PROCESS_NAME_ENV);
  if (name != NULL) { // LCOV_EXCL_BR_LINE 5: fail safe for libc getenv
    if (prctl(PR_SET_NAME, name) < 0) { // LCOV_EXCL_BR_LINE 5: fail safe for libc getenv
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

      CL_PERROR("prctl"); // LCOV_EXCL_LINE 5: fail safe for libc getenv
      goto exit; // LCOV_EXCL_LINE 5: fail safe for libc getenv
    }
  }

  if (sigemptyset(&mask) < 0) { // LCOV_EXCL_BR_LINE 5: fail safe for libc sigemptyset
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

    CL_PERROR("sigemptyset"); // LCOV_EXCL_LINE 5: fail safe for libc sigemptyset
    goto exit;                // LCOV_EXCL_LINE 5: fail safe for libc sigemptyset
  }
  if (sigaddset(&mask, SIGCHLD) < 0) { // LCOV_EXCL_BR_LINE 5: fail safe for libc sigaddset
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

    CL_PERROR("sigaddset");   // LCOV_EXCL_LINE 5: fail safe for libc sigaddset
    goto exit;                // LCOV_EXCL_LINE 5: fail safe for libc sigaddset
  }
  if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) { // LCOV_EXCL_BR_LINE 5: fail safe for libc sigprocmask
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

    CL_PERROR("sigprocmask"); // LCOV_EXCL_LINE 5: fail safe for libc sigprocmask
    goto exit;                // LCOV_EXCL_LINE 5: fail safe for libc sigprocmask
  }

  if ((sfd = signalfd(-1, &mask, SFD_NONBLOCK | SFD_CLOEXEC)) < 0) { // LCOV_EXCL_BR_LINE 5: fail safe for libc signalfd
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

    CL_PERROR("signalfd");    // LCOV_EXCL_LINE 5: fail safe for libc signalfd
    goto exit;                // LCOV_EXCL_LINE 5: fail safe for libc signalfd
  }

exit:
  return sfd;
}

/*
 * start of the functions to be executed from fork to exec
 */
static void cl_process_pf_err(char no) {
  char errorstr[17] = "err:post_fork:0\n";

  errorstr[14] = no;
  while (1) {
    if (write(STDERR_FILENO, errorstr, 16) == -1 && errno == EINTR) { // LCOV_EXCL_BR_LINE 5: fail safe for libc write
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      continue; // LCOV_EXCL_LINE 5: fail safe for libc write
    }
    break;
  }
}

static inline int cl_process_pf_set_schedule(const CL_ProcessAttrInternal_t *ia) {
  int ret = -1;
  struct sched_param param;
  int policy;

  switch (ia->sched_policy) {
    case CL_PROCESS_SCHED_POLICY_RR:
      policy = SCHED_RR;
      param.sched_priority = ia->sched_priority;
      break;
    case CL_PROCESS_SCHED_POLICY_FIFO:
      param.sched_priority = ia->sched_priority;
      policy = SCHED_FIFO;
      break;
    default:
      param.sched_priority = 0;
      policy = SCHED_OTHER;
      break;
  }
  if (sched_setscheduler(getpid(), policy, &param) < 0) { // LCOV_EXCL_BR_LINE 5: fail safe for glibc function sched_setscheduler
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    cl_process_pf_err('3');// LCOV_EXCL_LINE 5: fail safe for glibc function sched_setscheduler
    goto exit;// LCOV_EXCL_LINE 5: fail safe for glibc function sched_setscheduler
  }
  if (ia->sched_policy == CL_PROCESS_SCHED_POLICY_OTHER) {
    if (setpriority(PRIO_PROCESS, (__id_t)getpid(), ia->sched_priority) < 0) { // LCOV_EXCL_BR_LINE 5: fail safe for glibc function setpriority
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

      cl_process_pf_err('4');// LCOV_EXCL_LINE 5: fail safe for glibc function setpriority
      goto exit;// LCOV_EXCL_LINE 5: fail safe for glibc function setpriority
    }
  }
  ret = 0;

exit:
  return ret;
}

static inline int cl_process_pf_check_close_fd(int check_fd, int dirfd, int hold_fds_num, const int *hold_fds) {
  int i;

  if (check_fd < 3 || check_fd == dirfd) {
    return 0;
  }

  for (i = 0; i < hold_fds_num; i++) {
    if (check_fd == *(hold_fds + i)) {
      return 0;
    }
  }

  return 1;
}

static inline int cl_process_pf_fname2fd(const char *s) {
  int result = 0;
  for (; *s != '\0'; s++) {
    result = result * 10 + (*s - '0');
  }
  return result;
}

static inline int cl_process_pf_cleanup_fds(const CL_ProcessAttrInternal_t *ia) {
  int ret = -1;
  int dfd;
#define BUF_SIZE 128
  char buf[BUF_SIZE];
  struct linux_dirent64 *d;
  int hold_fds_num, i;

  for (i = 0; i < CL_PROCESSS_ATTR_HOLD_FDS_NUM; i++) {
    if (ia->hold_fds[i] == 0) {
      break;
    }
  }
  hold_fds_num = i;

  if ((dfd = open("/proc/self/fd/", O_RDONLY | O_DIRECTORY | O_CLOEXEC)) < 0) {// LCOV_EXCL_BR_LINE 5: fail safe for glibc function open
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

    cl_process_pf_err('8');// LCOV_EXCL_LINE 5: fail safe for glibc function open
    goto exit;// LCOV_EXCL_LINE 5: fail safe for glibc function open
  }

  while (1) {
    long nread;
    int fd;
    int bpos;

    nread = syscall(SYS_getdents64, dfd, buf, sizeof(buf));
    if (nread == -1) {// LCOV_EXCL_BR_LINE 5: fail safe for glibc function syscall
      // LCOV_EXCL_START 5: fail safe for glibc function syscall

      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

      cl_process_pf_err('9');
      close(dfd);
      goto exit;
    }
    // LCOV_EXCL_STOP

    if (nread == 0) {  // LCOV_EXCL_BR_LINE 5: fail safe for glibc function syscall
      break;
    }

    for (bpos = 0; bpos < nread;) {
      d = (struct linux_dirent64 *)(buf + bpos);
      fd = cl_process_pf_fname2fd(d->d_name);
      if (cl_process_pf_check_close_fd(fd, dfd, hold_fds_num, ia->hold_fds)) {
        close(fd);
      }
      bpos += d->d_reclen;
    }
  }
  close(dfd);
  ret = 0;

exit:
  return ret;
}

static void cl_process_post_fork(const char *file, char *const argv[], char *const envp[],
                                 const CL_ProcessAttrInternal_t *ia) {
  int intfy_fd;
  char intfy_fname[32] = "/tmp/intfy_00000";
  pid_t pid = getpid();

  intfy_fname[11] = (char)(intfy_fname[11] + (pid / 10000));
  intfy_fname[12] = (char)(intfy_fname[12] + ((pid / 1000) % 10));
  intfy_fname[13] = (char)(intfy_fname[13] + ((pid / 100) % 10));
  intfy_fname[14] = (char)(intfy_fname[14] + ((pid / 10) % 10));
  intfy_fname[15] = (char)(intfy_fname[15] + (pid % 10));

  unlink(intfy_fname);
  if ((intfy_fd = open(intfy_fname, O_CREAT | O_EXCL | O_WRONLY | O_CLOEXEC, 0666)) < 0) {// LCOV_EXCL_BR_LINE 5: fail safe for glibc function open
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    cl_process_pf_err('0'); // LCOV_EXCL_LINE 5:libc fail safe for open
    goto exit;              // LCOV_EXCL_LINE 5:libc fail safe for open
  }
  if (close(intfy_fd) < 0) {  // LCOV_EXCL_BR_LINE 5: fail safe for glibc function close
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    cl_process_pf_err('1');  // LCOV_EXCL_LINE 5:libc fail safe for close
    goto exit;  // LCOV_EXCL_LINE 5:libc fail safe for close
  }

  // Process group
  if (ia->create_group) {
    if (setpgid(getpid(), getpid()) < 0) {  // LCOV_EXCL_BR_LINE 5: fail safe for glibc function setpgid
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      cl_process_pf_err('2');  // LCOV_EXCL_LINE 5: fail safe for glibc function setpgid
      goto exit;  // LCOV_EXCL_LINE 5: fail safe for glibc function setpgid
    }
  }

  // Scheduling Policy/Priority
  if (cl_process_pf_set_schedule(ia) < 0) {  // LCOV_EXCL_BR_LINE 5: fail safe for glibc function sched_setscheduler
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    goto exit;  // LCOV_EXCL_LINE 5: fail safe for glibc function sched_setscheduler
  }

  // UID/GID
  if (ia->gid) {
    if (setgid(ia->gid) < 0) {  // LCOV_EXCL_BR_LINE 5: fail safe for glibc function setgid
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      cl_process_pf_err('5');  // LCOV_EXCL_LINE 5: fail safe for glibc function setgid
      goto exit;  // LCOV_EXCL_LINE 5: fail safe for glibc function setgid
    }
  }

  if (ia->uid) {
    uid_t uid = (ia->uid == UINT_MAX) ? 0 : ia->uid;
    if (setuid(uid) < 0) {// LCOV_EXCL_BR_LINE 5: fail safe for glibc function setuid
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      cl_process_pf_err('6');// LCOV_EXCL_LINE 5: fail safe for glibc function setuid
      goto exit;// LCOV_EXCL_LINE 5: fail safe for glibc function setuid
    }
  }

  // stack size
  if (ia->stack_size) {
    struct rlimit rlim;
    getrlimit(RLIMIT_STACK, &rlim);
    rlim.rlim_cur = (rlim_t)ia->stack_size;
    if (setrlimit(RLIMIT_STACK, &rlim) < 0) {// LCOV_EXCL_BR_LINE 5: fail safe for glibc function setrlimit
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

      cl_process_pf_err('7');// LCOV_EXCL_LINE 5: fail safe for glibc function setrlimit
    }
  }

  // cleanup fds
  if (ia->disable_close_fds == 0) {
    if (cl_process_pf_cleanup_fds(ia) < 0) {
      goto exit;
    }
  }

  // exec
  if (execve(file, argv, envp) < 0) {// LCOV_EXCL_BR_LINE 5: fail safe for glibc function execve
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    cl_process_pf_err('A');// LCOV_EXCL_LINE 5: fail safe for glibc function execve
  }

exit:
  return;
}
/*
 * end of the functions to be executed from fork to exec
 */

static int cl_process_pre_fork(char *const envp[], char ***new_env, const CL_ProcessAttrInternal_t *ia) {
  int i;
  size_t envlen = 0;
  char **e;
  char *b;
  int new_env_count;
  size_t name_env_len = strlen(CL_PROCESS_NAME_ENV);
  int last_name_env_pos = -1;

  if (envp == NULL) {
    e = environ;
  } else {
    e = (char **)envp;
  }

  for (i = 0; e[i] != NULL; i++) {
    if (strncmp(e[i], CL_PROCESS_NAME_ENV, name_env_len) == 0) {
      last_name_env_pos = i;
    } else {
      envlen += strlen(e[i]) + 1;
    }
  }

  if (ia->name[0] != 0) {
    envlen += strlen(CL_PROCESS_NAME_ENV) + 1 + strlen(ia->name) + 1;
    i++;
  }

  if ((b = malloc(envlen)) == NULL) { // LCOV_EXCL_BR_LINE 5: fail safe for libc malloc
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    CL_ERR_PRINT("malloc fail"); // LCOV_EXCL_LINE 5: fail safe for libc malloc
    errno = EFAULT;              // LCOV_EXCL_LINE 5: fail safe for libc malloc
    goto error;                  // LCOV_EXCL_LINE 5: fail safe for libc malloc
  }
  memset(b, 0, envlen);

  if ((*new_env = malloc(sizeof(char *) * (size_t)(i + 1))) == NULL) {    // LCOV_EXCL_BR_LINE 5: fail safe for libc malloc
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    free(b);                      // LCOV_EXCL_LINE 5: fail safe for libc malloc
    CL_ERR_PRINT("malloc fail");  // LCOV_EXCL_LINE 5: fail safe for libc malloc
    errno = EFAULT;               // LCOV_EXCL_LINE 5: fail safe for libc malloc
    goto error;                   // LCOV_EXCL_LINE 5: fail safe for libc malloc
  }
  memset(*new_env, 0, sizeof(char *) * (size_t)(i + 1));

  new_env_count = 0;
  for (i = 0; e[i] != NULL; i++) {
    if (i == last_name_env_pos) {
      continue;
    }
    strcpy(b, e[i]);
    *(*new_env + i) = b;
    b += strlen(e[i]) + 1;
    new_env_count++;
  }

  if (ia->name[0] != 0) {
    *(*new_env + new_env_count) = b;
    b += sprintf(b, "%s=%s", CL_PROCESS_NAME_ENV, ia->name);
    b++;
    new_env_count++;
  }

  return 0;

error:
  return -1;
}

/*
 * Create process
 */
int CL_ProcessCreate(const char *file, char *const argv[], char *const envp[], const CL_ProcessAttr_t *attr) {
  const CL_ProcessAttrInternal_t *ia = (const CL_ProcessAttrInternal_t *)attr;
  pid_t childpid;
  char intfy_fname[32];
  char **new_env;
  int ret;
  int retry;

  if (file == NULL || argv == NULL || attr == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (cl_process_pre_fork(envp, &new_env, ia) < 0) { // LCOV_EXCL_BR_LINE 5: fail safe for libc function malloc
    return -1;
  }

  childpid = fork();
  switch (childpid) {  // LCOV_EXCL_BR_LINE 5: fail safe for glibc function fork
    case 0: /* child process */
      cl_process_post_fork(file, argv, new_env, ia);
      _exit(CL_PROCESS_EXIT_INTERNAL);
      break;

    case -1: /* error */
      goto exit;

    default: /* parent process */
      break;
  }

  if (ia->cpu_assign != 0) {
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET((size_t)ia->cpu_assign - 1, &set); // LCOV_EXCL_BR_LINE 15:CPU_SET define at sched.h
    // LCOV_EXCL_BR_START 5: fail safe for libc sched_setaffinity
    if (sched_setaffinity(childpid, sizeof(set), &set) < 0) {
      // LCOV_EXCL_BR_STOP
      int last_errno = errno;
      CL_PERROR("sched_setaffinity");
      errno = last_errno;
      childpid = -1;
      goto exit;
    }
  }

  ret = snprintf(intfy_fname, sizeof(intfy_fname), CL_INTFY_FILENAME_FORMAT, childpid);
  if (ret < 0 || ret > sizeof(intfy_fname)) {// LCOV_EXCL_BR_LINE 5: fail safe for libc snprintf
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    CL_ERR_PRINT("snprintf fail:%d", ret);  // LCOV_EXCL_LINE 5: fail safe for libc snprintf
    childpid = -1;                          // LCOV_EXCL_LINE 5: fail safe for libc snprintf
    goto exit;                              // LCOV_EXCL_LINE 5: fail safe for libc snprintf
  }

  if (ia->cgroup_name[0] != '\0') {
    if (cl_cgroup_exist(CL_CGROUP_CPU, ia->cgroup_name) == 0) {
      if (cl_cgroup_set_num(CL_CGROUP_CPU, ia->cgroup_name, "tasks", childpid) < 0) {
        int last_errno = errno;
        CL_PERROR("cl_cgroup_set_num");
        errno = last_errno;
        childpid = -1;
        goto exit;
      }
    }
    // LCOV_EXCL_BR_START 5: fail safe for glibc function access
    if (cl_cgroup_exist(CL_CGROUP_MEMORY, ia->cgroup_name) == 0) {
      // LCOV_EXCL_BR_STOP
      if (cl_cgroup_set_num(CL_CGROUP_MEMORY, ia->cgroup_name, "tasks", childpid) < 0) {
        int last_errno = errno;
        CL_PERROR("cl_cgroup_set_num");
        errno = last_errno;
        childpid = -1;
        goto exit;
      }
    }
  }

  for (retry = 100; retry >= 0; retry--) {
    if (access(intfy_fname, F_OK) == 0) {  // LCOV_EXCL_BR_LINE 5: fail safe for glibc function access
      break;
    }
    usleep(100);
  }

exit:
  free(new_env[0]);
  free(new_env);

  return childpid;
}

/*
 * Initialize process attribute
 */
int CL_ProcessCreateAttrInit(CL_ProcessAttr_t *attr) {
  if (attr == NULL) {
    errno = EINVAL;
    return -1;
  }

  memset(attr, 0, sizeof(CL_ProcessAttr_t));
  return 0;
}

/*
 * Set process attribute (process name)
 */
int CL_ProcessCreateAttrSetName(CL_ProcessAttr_t *attr, const char *name) {
  CL_ProcessAttrInternal_t *ia = (CL_ProcessAttrInternal_t *)attr;

  if (attr == NULL || name == NULL) {
    errno = EINVAL;
    return -1;
  }

  strncpy(ia->name, name, 16);
  ia->name[15] = 0;

  return 0;
}

/*
 * Set process attribute (user ID)
 */
int CL_ProcessCreateAttrSetUid(CL_ProcessAttr_t *attr, uid_t uid) {
  CL_ProcessAttrInternal_t *ia = (CL_ProcessAttrInternal_t *)attr;

  if (attr == NULL) {
    errno = EINVAL;
    return -1;
  }

  ia->uid = uid;

  return 0;
}

/*
 * Set process attribute (group ID)
 */
int CL_ProcessCreateAttrSetGid(CL_ProcessAttr_t *attr, gid_t gid) {
  CL_ProcessAttrInternal_t *ia = (CL_ProcessAttrInternal_t *)attr;

  if (attr == NULL) {
    errno = EINVAL;
    return -1;
  }

  ia->gid = gid;

  return 0;
}

/*
 * Set process attribute (schedule policy and priority)
 */
int CL_ProcessCreateAttrSetSchedule(CL_ProcessAttr_t *attr, CL_ProcessSchedPolicy_t policy, int priority) {
  CL_ProcessAttrInternal_t *ia = (CL_ProcessAttrInternal_t *)attr;

  if (attr == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (policy == CL_PROCESS_SCHED_POLICY_RR || policy == CL_PROCESS_SCHED_POLICY_FIFO) {
    if (priority < 1 || priority > 99) {
      errno = EINVAL;
      return -1;
    }
  } else if (policy == CL_PROCESS_SCHED_POLICY_OTHER) {
    if (priority < -20 || priority > 19) {
      CL_ERR_PRINT("Invlid SCHED_OTHER priority:%d", priority);
    }
  } else {
    errno = EINVAL;
    return -1;
  }

  ia->sched_policy = policy;
  ia->sched_priority = priority;

  return 0;
}

/*
 * Set process attribute (process group)
 */
int CL_ProcessCreateAttrSetGroup(CL_ProcessAttr_t *attr, int create) {
  CL_ProcessAttrInternal_t *ia = (CL_ProcessAttrInternal_t *)attr;

  if (attr == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (create < 0 || create > 1) {
    errno = EINVAL;
    return -1;
  }

  ia->create_group = create;

  return 0;
}

/**
 * Set process attribute (CPU Assign)
 */
int CL_ProcessCreateAttrSetCpuAssign(CL_ProcessAttr_t *attr, int cpu_assign) {
  CL_ProcessAttrInternal_t *ia = (CL_ProcessAttrInternal_t *)attr;

  if (attr == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (cpu_assign < 0 || cpu_assign > 2) {
    errno = EINVAL;
    return -1;
  }

  ia->cpu_assign = cpu_assign;

  return 0;
}

/*
 * Set process attribute (Stack Size)
 */
int CL_ProcessCreateAttrSetStackSize(CL_ProcessAttr_t *attr, int stack_size) {
  CL_ProcessAttrInternal_t *ia = (CL_ProcessAttrInternal_t *)attr;

  if (attr == NULL) {
    errno = EINVAL;
    return -1;
  }

  ia->stack_size = stack_size;

  return 0;
}

/*
 * Set process attribute (FD to maintain)
 */
int CL_ProcessCreateAttrSetHoldFds(CL_ProcessAttr_t *attr, int hold_fds[]) {
  CL_ProcessAttrInternal_t *ia = (CL_ProcessAttrInternal_t *)attr;

  if (attr == NULL || hold_fds == NULL) {
    errno = EINVAL;
    return -1;
  }

  memcpy(ia->hold_fds, hold_fds, sizeof(ia->hold_fds));

  return 0;
}

/*
 * Set process attribute (to suspend forced FD close)
 */
int CL_ProcessCreateAttrSetDisableCloseFds(CL_ProcessAttr_t *attr) {
  CL_ProcessAttrInternal_t *ia = (CL_ProcessAttrInternal_t *)attr;

  if (attr == NULL) {
    errno = EINVAL;
    return -1;
  }

  ia->disable_close_fds = 1;

  return 0;
}

/*
 * Set process attribute (Cgroup)
 */
int CL_ProcessCreateAttrSetCgroup(CL_ProcessAttr_t *attr, const char *cgroup_name) {
  CL_ProcessAttrInternal_t *ia = (CL_ProcessAttrInternal_t *)attr;

  if (attr == NULL || cgroup_name  == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (strlen(cgroup_name) >= sizeof(ia->cgroup_name)) {
    errno = EINVAL;
    return -1;
  }

  strcpy(ia->cgroup_name, cgroup_name);

  return 0;
}

/*
 * Kill process
 */
int CL_ProcessTerminate(pid_t pid) {
  return kill(pid, SIGKILL);
}

/*
 * Kill process group
 */
int CL_ProcessTerminateGroup(pid_t pid) {
  return killpg(pid, SIGKILL);
}

/**
 * Forced process termination
 */
int CL_ProcessAbort(pid_t pid) {
  return kill(pid, SIGABRT);
}

/**
 * Forced process group termination
 */
int CL_ProcessAbortGroup(pid_t pid) {
  return killpg(pid, SIGABRT);
}

/**
 * Euthanize process group
 */
int CL_ProcessEuthanizeGroup(pid_t pid) {
  usleep(10 * 1000);
  return killpg(pid, SIGKILL);
}

/*
 * Collect child process
 */
int CL_ProcessCleanup(int sigchld_fd, CL_ProcessCleanupInfo_t *cleanup_info) {
  struct signalfd_siginfo fdsi;
  ssize_t s;
  siginfo_t info;
  int ret;
  char intfy_fname[32];

  if (cleanup_info == NULL) {
    errno = EINVAL;
    return -1;
  }

  while (1) {
    s = read(sigchld_fd, &fdsi, sizeof(struct signalfd_siginfo));
    if (s !=  sizeof(struct signalfd_siginfo)) {
      if (s == -1 && errno == EINTR) {
        continue;
      }
      break;
    }
  }

  info.si_pid = 0;
  while (1) {
    ret = waitid(P_ALL, 0, &info, WEXITED | WNOHANG);
    if (ret == -1) {
      return -1;
    } else if (ret == 0 && info.si_pid == 0) {
      errno = ECHILD;
      return -1;
    }
    break;
  }

  cleanup_info->pid = info.si_pid;
  cleanup_info->code = info.si_code;
  cleanup_info->status = info.si_status;

  cl_monitor_cleanup(info.si_pid);

  ret = snprintf(intfy_fname, sizeof(intfy_fname), CL_INTFY_FILENAME_FORMAT, info.si_pid);

  if (ret < 0 || ret > sizeof(intfy_fname)) {// LCOV_EXCL_BR_LINE 5: fail safe for glibc function snprintf
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    CL_ERR_PRINT("snprintf fail:%d\n", ret);// LCOV_EXCL_LINE 5: fail safe for glibc function snprintf
    goto exit;// LCOV_EXCL_LINE 5: fail safe for glibc function snprintf
  }
  if (unlink(intfy_fname) < 0) {
    CL_PERROR("unlink");
  }

exit:
  info.si_pid = 0;
  ret = waitid(P_ALL, 0, &info, WEXITED | WNOWAIT | WNOHANG);
  if (ret == 0 && info.si_pid != 0) {
    return 1;
  } else if (ret == -1 && errno != ECHILD) {
    return -1;
  }
  return 0;
}

typedef struct {
  sem_t sem;
  CL_ThreadAttrInternal_t *ia;
  void *(*start_routine)(void *);
  void *start_arg;
} create_thread_arg_t;

static void *thread_start_func(void *arg) {
  create_thread_arg_t *p = (create_thread_arg_t *)arg;
  void *(*start_routine)(void *) = p->start_routine;
  void *start_arg = p->start_arg;

  prctl(PR_SET_NAME, p->ia->name != 0 ? p->ia->name : NULL);

  if (sem_post(&p->sem) < 0) {// LCOV_EXCL_BR_LINE 5: fail safe for libc sem_post
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    CL_PERROR("sem_post"); // LCOV_EXCL_LINE 5: fail safe for libc sem_post
  }

  return (*start_routine)(start_arg);
}

/*
 * Create thread
 */
int CL_ThreadCreate(pthread_t *thread, pthread_attr_t *attr, CL_ThreadAttr_t *cl_attr, void *(*start_routine)(void *),
                    void *arg) {
  int ret;
  create_thread_arg_t cr_arg = { .ia = (CL_ThreadAttrInternal_t *)cl_attr,
                                 .start_routine = start_routine,
                                 .start_arg = arg
                               };

  if (thread == NULL || cl_attr == NULL || start_routine == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (sem_init(&cr_arg.sem, 0, 0) < 0) { // LCOV_EXCL_BR_LINE 5: fail safe for libc sem_init
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

    return -1;  // LCOV_EXCL_LINE 5: fail safe for libc sem_init
  }

  // LCOV_EXCL_BR_START 5: fail safe for libc pthread_create
  if ((ret = pthread_create(thread, attr, thread_start_func, &cr_arg)) != 0) {
    // LCOV_EXCL_BR_STOP

    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

    errno = ret; // LCOV_EXCL_LINE 5: fail safe for libc pthread_create
    return -1;   // LCOV_EXCL_LINE 5: fail safe for libc pthread_create
  }

  if (sem_wait(&cr_arg.sem) < 0) {// LCOV_EXCL_BR_LINE 5: fail safe for libc sem_wait
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

    return -1;  // LCOV_EXCL_LINE 5: fail safe for libc sem_wait
  }

  return 0;
}

/*
 * Initialize CommonLibrary extension thread attribute
 */
int CL_ThreadCreateAttrInit(CL_ThreadAttr_t *attr) {
  assert_static(sizeof(CL_ThreadAttr_t) == sizeof(CL_ThreadAttrInternal_t));

  if (attr == NULL) {
    errno = EINVAL;
    return -1;
  }

  memset(attr, 0, sizeof(CL_ThreadAttr_t));
  return 0;
}

/*
 * Set CommonLibrary extension thread attribute (thread name)
 */
int CL_ThreadCreateAttrSetName(CL_ThreadAttr_t *attr, const char *name) {
  CL_ThreadAttrInternal_t *ia = (CL_ThreadAttrInternal_t *)attr;

  if (attr == NULL || name == NULL) {
    errno = EINVAL;
    return -1;
  }

  strncpy(ia->name, name, 16);
  ia->name[15] = 0;

  return 0;
}

/*
 * Create Cgroup
 */
int CL_ProcessCreateCgroupCreate(const char *cgroup_name, CL_ProcessCreateCgroupAttr_t *attr) {
  CL_ProcessCreateCgroupAttrInternal_t *ia = (CL_ProcessCreateCgroupAttrInternal_t *)attr;
  CL_ProcessCreateCgroupAttrInternal_t cmp;

  if (cgroup_name == NULL || attr == NULL) {
    errno = EINVAL;
    return -1;
  }

  memset(&cmp, 0, sizeof(CL_ProcessCreateCgroupAttrInternal_t));
  if (memcmp(ia, &cmp, sizeof(CL_ProcessCreateCgroupAttrInternal_t)) == 0) {
    errno = EINVAL;
    return -1;
  }

  if (ia->rt_runtime_us || ia->cfs_quota_us || ia->cpu_shares) {
    // LCOV_EXCL_BR_START 5: fail safe for glibc function mkdir
    if (cl_cgroup_make(CL_CGROUP_CPU, cgroup_name) < 0) {
      // LCOV_EXCL_BR_STOP
      return -1;
    }

    if (ia->rt_runtime_us) {
      // LCOV_EXCL_BR_START 5: fail safe for glibc function write
      if (cl_cgroup_set_num(CL_CGROUP_CPU, cgroup_name, "cpu.rt_runtime_us", ia->rt_runtime_us) < 0) {
        // LCOV_EXCL_BR_STOP

        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

        return -1;// LCOV_EXCL_LINE 5: fail safe for glibc function write
      }
    }

    if (ia->cfs_quota_us) {
      // LCOV_EXCL_BR_START 5: fail safe for glibc function write
      if (cl_cgroup_set_num(CL_CGROUP_CPU, cgroup_name, "cpu.cfs_quota_us", ia->cfs_quota_us) < 0) {
        // LCOV_EXCL_BR_STOP

        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

        return -1;// LCOV_EXCL_LINE 5: fail safe for glibc function write
      }
    }

    if (ia->cpu_shares) {
      // LCOV_EXCL_BR_START 5: fail safe for glibc function write
      if (cl_cgroup_set_num(CL_CGROUP_CPU, cgroup_name, "cpu.shares", ia->cpu_shares) < 0) {
        // LCOV_EXCL_BR_STOP
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        return -1;// LCOV_EXCL_LINE 5: fail safe for glibc function write
      }
    }
  }

  if (ia->memory_limit || ia->usage_in_bytes || ia->event_fd) {
    // LCOV_EXCL_BR_START 5: fail safe for glibc function mkdir
    if (cl_cgroup_make(CL_CGROUP_MEMORY, cgroup_name) < 0) {
      // LCOV_EXCL_BR_STOP
      return -1;
    }

    if (ia->memory_limit) {
      // LCOV_EXCL_BR_START 5: fail safe for glibc function write
      if (cl_cgroup_set_num(CL_CGROUP_MEMORY, cgroup_name, "memory.limit_in_bytes", ia->memory_limit) < 0) {
        // LCOV_EXCL_BR_STOP
        return -1;
      }

      if (cl_cgroup_set_num(CL_CGROUP_MEMORY, cgroup_name, "memory.memsw.limit_in_bytes", ia->memory_limit) < 0) {
        return -1;
      }

    }

    if (ia->usage_in_bytes || ia->event_fd) {
      int checkfd;
      char setstr[64];

      // LCOV_EXCL_BR_START 5: fail safe for glibc function open
      if ( (checkfd = cl_cgroup_open(CL_CGROUP_MEMORY, cgroup_name, "memory.memsw.usage_in_bytes", O_RDONLY)) < 0) {
        // LCOV_EXCL_BR_STOP
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        return -1;  // LCOV_EXCL_LINE 5: fail safe for glibc function open
      }

      snprintf(setstr, sizeof(setstr), "%d %d %d", ia->event_fd, checkfd, ia->usage_in_bytes);
      // LCOV_EXCL_BR_START 5: fail safe for glibc function write
      if (cl_cgroup_set_string(CL_CGROUP_MEMORY, cgroup_name, "cgroup.event_control", setstr) < 0) {
        // LCOV_EXCL_BR_STOP

        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

        return -1;// LCOV_EXCL_LINE 5: fail safe for glibc function write
      }
    }
  }

  return 0;
}

/*
 * Initialize Cgroup attribute
 */
int CL_ProcessCreateCgroupAttrInit(CL_ProcessCreateCgroupAttr_t *attr) {
  assert_static(sizeof(CL_ProcessCreateCgroupAttr_t) == sizeof(CL_ProcessCreateCgroupAttrInternal_t));

  if (attr == NULL) {
    errno = EINVAL;
    return -1;
  }

  memset(attr, 0, sizeof(CL_ProcessCreateCgroupAttr_t));
  return 0;
}

/*
 * Set Cgroup attribute (RT Throttling)
 */
int CL_ProcessCreateCgroupAttrSetRtThrottling(CL_ProcessCreateCgroupAttr_t *attr, int runtime_us) {
  CL_ProcessCreateCgroupAttrInternal_t *ia = (CL_ProcessCreateCgroupAttrInternal_t *)attr;

  if (attr == NULL) {
    errno = EINVAL;
    return -1;
  }

  ia->rt_runtime_us = runtime_us;

  return 0;
}

/*
 * Set Cgroup attribute (CFS Bandwidth Control)
 */
int CL_ProcessCreateCgroupAttrSetCfsBandwidthControl(CL_ProcessCreateCgroupAttr_t *attr, int cfs_quota_us) {
  CL_ProcessCreateCgroupAttrInternal_t *ia = (CL_ProcessCreateCgroupAttrInternal_t *)attr;

  if (attr == NULL) {
    errno = EINVAL;
    return -1;
  }

  ia->cfs_quota_us = cfs_quota_us;

  return 0;
}

/*
 * Set Cgroup attribute (CPU Shares)
 */
int CL_ProcessCreateCgroupAttrSetCpuShares(CL_ProcessCreateCgroupAttr_t *attr, int cpu_shares) {
  CL_ProcessCreateCgroupAttrInternal_t *ia = (CL_ProcessCreateCgroupAttrInternal_t *)attr;

  if (attr == NULL) {
    errno = EINVAL;
    return -1;
  }

  ia->cpu_shares = cpu_shares;

  return 0;
}

/*
 * Set Cgroup attribute (Memory Limit)
 */
int CL_ProcessCreateCgroupAttrSetMemoryLimit(CL_ProcessCreateCgroupAttr_t *attr, int memory_limit) {
  CL_ProcessCreateCgroupAttrInternal_t *ia = (CL_ProcessCreateCgroupAttrInternal_t *)attr;

  if (attr == NULL) {
    errno = EINVAL;
    return -1;
  }

  ia->memory_limit = memory_limit;

  return 0;
}

/*
 * Set Cgroup attribute (Memory Usage Notification)
 */
int CL_ProcessCreateCgroupAttrSetMemoryUsageNotification(CL_ProcessCreateCgroupAttr_t *attr, int usage_in_bytes,
                                                         int event_fd) {
  CL_ProcessCreateCgroupAttrInternal_t *ia = (CL_ProcessCreateCgroupAttrInternal_t *)attr;

  if (attr == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (event_fd < 0) {
    errno = EINVAL;
    return -1;
  }

  ia->usage_in_bytes = usage_in_bytes;
  ia->event_fd = event_fd;

  return 0;
}

/*
 * Delete Cgroup
 */
int CL_ProcessCreateCgroupDelete(const char *cgroup_name) {
  int mem_ret = 1, mem_err;
  int cpu_ret = 1, cpu_err;

  if (cgroup_name == NULL) {
    errno = EINVAL;
    return -1;
  }
  // LCOV_EXCL_BR_START 5: fail safe for glibc function access
  if (cl_cgroup_exist(CL_CGROUP_MEMORY, cgroup_name) == 0) {
    // LCOV_EXCL_BR_STOP
    mem_ret = cl_cgroup_remove(CL_CGROUP_MEMORY, cgroup_name);
    mem_err = errno;
  }

  // LCOV_EXCL_BR_START 5: fail safe for glibc function access
  if (cl_cgroup_exist(CL_CGROUP_CPU, cgroup_name) == 0) {
    // LCOV_EXCL_BR_STOP
    cpu_ret = cl_cgroup_remove(CL_CGROUP_CPU, cgroup_name);
    cpu_err = errno;
  }

  if (mem_ret == 1 && cpu_ret == 1) {
    errno = ENOENT;
    return -1;
  } else if (mem_ret < 0) {
    errno = mem_err;
    return -1;
  } else if (cpu_ret < 0) {
    errno = cpu_err;
    return -1;
  }

  return 0;
}

/*
 * Move process to the Cgroup
 */
int CL_ProcessCreateCgroupClassify(const char *cgroup_name, pid_t pid) {
  int mem_ret = 1, mem_err;
  int cpu_ret = 1, cpu_err;

  if (cgroup_name == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (cl_cgroup_exist(CL_CGROUP_MEMORY, cgroup_name) == 0) {// LCOV_EXCL_BR_LINE 5: fail safe for glibc function access
    mem_ret = cl_cgroup_set_num(CL_CGROUP_MEMORY, cgroup_name, "tasks", pid);
    mem_err = errno;
  }

  if (cl_cgroup_exist(CL_CGROUP_CPU, cgroup_name) == 0) {// LCOV_EXCL_BR_LINE 5: fail safe for glibc function access
    cpu_ret = cl_cgroup_set_num(CL_CGROUP_CPU, cgroup_name, "tasks", pid);
    cpu_err = errno;
  }

  if (mem_ret == 1 && cpu_ret == 1) {
    errno = ENOENT;
    return -1;
  } else if (mem_ret < 0) {
    errno = mem_err;
    return -1;
  } else if (cpu_ret < 0) {
    errno = cpu_err;
    return -1;
  }

  return 0;
}

/* vim:set ts=8 sts=2 sw=2: */
