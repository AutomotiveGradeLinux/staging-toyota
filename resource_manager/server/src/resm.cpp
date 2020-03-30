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
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/timerfd.h>
#include <sys/wait.h>
#include <sched.h>
//#include <stub/ss_diag.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>

// NSFW
#include <native_service/frameworkunified_dispatcher.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/ns_version_if.h>
#include <system_service/ss_services.h>
#include <native_service/frameworkunified_application.h>
#include <system_service/ss_system_if.h>
#include <system_service/ss_version.h>
#include <native_service/cl_process.h>
#include <system_service/ss_system_process.h>
#include <other_service/VP_GetEnv.h>
#include <other_service/rpc.h>
#include "resmgr_srvr.h"
#include "resm_internal.h"
#include "resmgr_api.h"
#include "ss_resm_resourcemanagerlog.h"
#include "resm_cfg.h"
#include "proc_watch.h"


// NSFW
CFrameworkunifiedVersion g_FrameworkunifiedVersion(MAJORNO, MINORNO, REVISION);
/**********************************************
 * Constant definitions
 **********************************************/
#define Resm_Flag_ID_Base EV_Flag_ID_Base(RESMGR_MID)
#define RESM_SUB_PRIORITY (0)

// #define WTC_CPU_INTERVAL (3)

#define CPULOAD_INVALID (-2)
#define CPULOAD_READY (-1)
#define CPULOG_TIMER_CLEAR (1)
#define CPULOG_LOGGING (0)
#define CPULOG_NO_LOGGING (-1)

#define PROCNET_DEV_FILE "/proc/net/dev"
#define MEMINFO_FILE "/proc/meminfo"
#define PROC_STAT_FILE "/proc/stat"

#define BYTE_TO_KIBIBYTE (1024)
#define KIBIBYTE_TO_BYTE (1024)
#define PERF_PNAME_MAX  128  // Max path name of process
#define PERF_PATH  "/usr/bin/perf"
#define PERF_FILE  "/tmp/perf"
#define PERF_DUMP  "/tmp/perf_dump"
#define PERF_REPORT_DELAY  1   // 1 sec
#define PERF_REPORT_RETRY  3   // retry 3 times
#define CPULOAD_NICEVAL   10   // value of nice() to lower priority

#define DEBUG_INFO_DIRPATH  "/tmp/diag_analysis"
#define DEBUG_INFO_FPATH DEBUG_INFO_DIRPATH"/dispinfo_resource.dbg"
#define DEBUG_INFO_TMPPATH DEBUG_INFO_DIRPATH"/dispinfo_resource.tmp"
#define DEBUG_INFO_MEM_LETTERS  (25)
#define DEBUG_INFO_CPU_TOP_LINES  (10)
#define DEBUG_INFO_CMA_MIN  (160000)  // in KB
#define DEBUG_INFO_CMA_LETTERS  (20)
#define DEBUG_INFO_DSP_PG_PATH  "/usr/agl/bin/bs_analysis_dispinfo_debug"

#define DROP_CACHES_PG_PATH "/usr/agl/bin/drop_caches"

#define READLINE_MAX_SIZE       512
#define READ_MAX_SIZE          4096
#define LF (0x0A)

/**********************************************
 * Structure definitions
 **********************************************/
// App session information
typedef struct {
  BOOL useFlag;
  RESM_REQ_EVENT_t reqEv;     // event request
} SSN_INFO_t;

// CPU usage information
typedef struct {
  char cpuname[8];
  int32_t user;
  int32_t nice;
  int32_t system;
  int32_t idle;
  int32_t iowait;
  int32_t irq;
  int32_t softirq;
  int32_t steal;
  int32_t guest;
  int32_t guest_nice;
} CPU_INFO_t;

//Context
typedef struct {
  char procName[128];
  HANDLE hApp;      // DispatcherHandle
  int32_t nsFd;     // For receiving from the NSFW

  // Session information
  SSN_INFO_t ssnInfo[EV_MAX_IDS_IN_THREAD];

  // Memory information
  uint32_t restMem;     // Remaining memory information
  BOOL restMemFlag;

  // CMA information
  uint32_t restCma;
  BOOL restCmaFlag;


  // CPU load information
//  int32_t cpuloadRate;
} SVC_COMMON_t;

// meminfo table
typedef struct {
  const char* name;
  uint32_t* value;
} meminfo_tbl;

/**********************************************
 * External variable definitions
 **********************************************/
// FRAMEWORKUNIFIEDLOG
FRAMEWORKUNIFIEDLOGPARAM g_FrameworkunifiedLogParams = {
  FRAMEWORKUNIFIEDLOGOPTIONS, {
    ZONE_TEXT_10, ZONE_TEXT_11, ZONE_TEXT_12,
    ZONE_TEXT_13, ZONE_TEXT_14, ZONE_TEXT_15,
    ZONE_TEXT_16, ZONE_TEXT_17, ZONE_TEXT_18,
    ZONE_TEXT_19, ZONE_TEXT_20, ZONE_TEXT_21,
    ZONE_TEXT_22, ZONE_TEXT_23, ZONE_TEXT_24,
    ZONE_TEXT_25, ZONE_TEXT_26, ZONE_TEXT_27,
    ZONE_TEXT_28, ZONE_TEXT_29, ZONE_TEXT_30,
    ZONE_TEXT_31
  }, FRAMEWORKUNIFIEDLOGZONES
};

int isNfs;                     // NFS env : 1

static SVC_COMMON_t g_resmgr;  // NOLINT (readability/nolint)
static int32_t g_sock = -1;
static uint32_t inactFile_kib;
static uint32_t mainFree_kib;
static uint32_t memTotal_kib;
static uint32_t cmaFree_kib;
static uint32_t cmaTotal_kib;
static uint32_t minRestMem;    // Minimum memory available
static uint32_t minRestCma;    // Minimum CMA available
static int32_t g_cpuloadRate1000;
static int g_cpu_num;
static CPU_INFO_t *g_cpuload_pre;
static CPU_INFO_t *g_cpuload;

static int32_t g_fifo_status = STATUS_IDOL;
static int32_t g_tss_status = STATUS_IDOL;
static int32_t g_fifo_timer = 0;
static int32_t g_tss_timer = 0;


/**********************************************
 * Local function definition
 **********************************************/
static void ctxCreate(SVC_COMMON_t* p_ctx, int32_t argc, char* argv[]);

// Session related
static int32_t get_new_id(uint32_t* ssnId);

// Memory monitoring
void watchMem(void);
static int32_t comp_meminfo_tbl(const void* a, const void* b);
static int32_t get_meminfo(void);

// Network monitoring
static int32_t getInetStatus(RESM_INET_STATUS_t* p_iStatus);
static char* get_aliasName(char* name, char* line);
static int32_t get_hwaddr(char* name, char* hwaddr);

// CPU monitoring

static void watchCPUStatus(void);
static void watchCPU(void);
static int32_t get_cpuload(CPU_INFO_t* p_cpu);
static int32_t calc_cpuloadRate(void);
static int32_t calc_cpuloadRate_each(int num);

// static int32_t chk_logging(int32_t cpuload, int32_t timer);
// static void logging_cpuload(void);

static void trim_end(char* buf);

// static void escape_percent(char* in, char* out);
static void init_cpuload(void);

// extern void logging_cpuload_custom(void);
static void exec_perf(int32_t t_pid);
extern unsigned long logging_cpuload_custom(int32_t tmode);


// Debug information output
void outputResouceInfo(void);
static int write_meminfo_work(FILE *wfp);
static int write_cpuinfo_work(FILE *wfp);
static int write_cmainfo_work(FILE *wfp);
static int write_resourcemanagerloginfo_work(void);

/*******************************************************************************
 * RPC public API
 *******************************************************************************/
/* Event issuance request */
RESM_ERR_t RESM_ReqEvent(uint32_t ssnId, const RESM_REQ_EVENT_t* p_reqEvent) {
  // Argument check
  if (p_reqEvent == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__, "ResMgr ReqEvent Invalid Arg");
    return RESM_E_PAR;
  }

  if (ssnId >= EV_MAX_IDS_IN_THREAD) {
    FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__, "ResMgr ReqEvent Invalid Arg");
    return RESM_E_PAR;
  }

  if (!g_resmgr.ssnInfo[ssnId].useFlag) {
    FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__, "ResMgr ReqEvent Invalid Arg");
    return RESM_E_PAR;
  }

  // Event type check
  if (p_reqEvent->reqEvent == RESM_EV_MEM) {
    // Record event issuance request
    memcpy(&g_resmgr.ssnInfo[ssnId].reqEv, p_reqEvent,
           sizeof(RESM_REQ_EVENT_t));
  }

  return RESM_E_OK;
}

/* Get system status */
RESM_ERR_t RESM_GetStatus(uint32_t ssnId, RESM_STATUS_t* p_status) {
  int32_t ret;

  // Argument check
  if (p_status == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__, "ResMgr GetStatus Invalid Arg");
    return RESM_E_PAR;
  }

  if (ssnId >= EV_MAX_IDS_IN_THREAD) {
    FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__, "ResMgr GetStatus Invalid Arg");
    return RESM_E_PAR;
  }

  if (!g_resmgr.ssnInfo[ssnId].useFlag) {
    FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__, "ResMgr GetStatus Invalid Arg");
    return RESM_E_PAR;
  }

  if (!g_resmgr.restMemFlag) {
    // No remaining memory information
    FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__, "ResMgr GetStatus get restmem Error");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
    return RESM_E_NG;
  }
  p_status->restMemSize = g_resmgr.restMem;
  p_status->nandWriteStatus = RESM_NAND_WRITE_ENABLE;   //[]Always possible

  ret = getInetStatus(&p_status->inetStatus);
  if (ret != 0) {
    FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__,
           "ResMgr GetStatus getInetStatus Error");
    return RESM_E_NG;
  }

  return RESM_E_OK;
}

/*******************************************************************************
 * Internal API
 *******************************************************************************/
/* Session connection */
RESM_ERR_t RESM_SV_Open(const RESM_RSV_t* p_prim, uint32_t* p_ssnId) {
  int32_t ret;

  ret = get_new_id(p_ssnId);
  if (ret != 0) {
    return RESM_E_NG;
  }
  // FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "ResMgr Open Session:ID=[%d]", *p_ssnId);

  return RESM_E_OK;
}

/* Session disconnection */
RESM_ERR_t RESM_SV_Close(uint32_t ssnId) {
  // Argument check
  if (ssnId >= EV_MAX_IDS_IN_THREAD) {
    FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__, "ResMgr Close Invalid Arg");
    return RESM_E_PAR;
  }

  if (!g_resmgr.ssnInfo[ssnId].useFlag) {
    FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__, "ResMgr Close Invalid Arg");
    return RESM_E_PAR;
  }

  // Turn off used flag
  g_resmgr.ssnInfo[ssnId].useFlag = FALSE;

  // Clear event request
  memset(&g_resmgr.ssnInfo[ssnId].reqEv, 0, sizeof(RESM_REQ_EVENT_t));

  return RESM_E_OK;
}

/* Session ID check */
RESM_ERR_t RESM_SV_ChkSsnId(uint32_t ssnId) {
  if (ssnId >= EV_MAX_IDS_IN_THREAD) {
    FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__, "ResMgr ssnId[%d] over MAX", ssnId);
    return RESM_E_NG;
  }

  if (!g_resmgr.ssnInfo[ssnId].useFlag) {
    FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__, "ResMgr ssnId[%d] is unidentified",
           ssnId);
    return RESM_E_NG;
  }

  return RESM_E_OK;
}

/**********************************************
 *  Initialization function
 **********************************************/
/* Context Initialization */
static void ctxCreate(SVC_COMMON_t* p_ctx, int32_t argc, char* argv[]) {
  EFrameworkunifiedStatus resourcemanagerRet;
  FrameworkunifiedDefaultCallbackHandler cbFuncs;
  FRAMEWORKUNIFIED_MAKE_DEFAULT_CALLBACK(cbFuncs);
  FRAMEWORKUNIFIED_SET_ZONES();

  memset(p_ctx, 0, sizeof(SVC_COMMON_t));
  strcpy(p_ctx->procName, SS_RESOURCE_MANAGER);  // NOLINT

  // Create a Dispatcher
  resourcemanagerRet = FrameworkunifiedCreateDispatcherWithoutLoop(p_ctx->procName, p_ctx->hApp, argc,
                                          argv, &cbFuncs, TRUE);
  if (resourcemanagerRet != eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "[RESM_ERR]ResMgr Create Dispatcher Failed. ret[%d]", resourcemanagerRet);
    exit(EXIT_FAILURE);
    // LCOV_EXCL_STOP 4: NSFW error case.
  }

  resourcemanagerRet = FrameworkunifiedGetDispatcherFD(p_ctx->hApp, &p_ctx->nsFd);
  if (resourcemanagerRet != eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "[RESM_ERR]ResMgr Get Dispatcher FD Failed. ret[%d]", resourcemanagerRet);
    exit(EXIT_FAILURE);
    // LCOV_EXCL_STOP 4: NSFW error case.
  }

  // Initialize session
  memset(p_ctx->ssnInfo, 0, sizeof(p_ctx->ssnInfo));


//  // Initialize CPU Information
//  g_resmgr.cpuloadRate = CPULOAD_INVALID;


  return;
}

/**********************************************
 *  Get FRAMEWORKUNIFIEDLOG BasePF Flag
 **********************************************/
static EFrameworkunifiedStatus resourcemanagerlog_output_debug_info;
#define RESOURCEMANAGERLOG_BASEPF_FLAG_ID (11)

EFrameworkunifiedStatus resourcemanagerlog_flag_check(UI_8 *mode) {
  if (resourcemanagerlog_output_debug_info != eFrameworkunifiedStatusOK)
    return eFrameworkunifiedStatusFail;

  return NsLogGetFrameworkunifiedLogFlag(RESOURCEMANAGERLOG_BASEPF_FLAG_ID, mode);
}

/**********************************************
 *  Set DIAGCODE
 **********************************************/
void set_diag_ss_resourcemanager_err(uint16_t resourcemanager_code) {
#if 0  // TODO(Workaround to avoid build errors for customization by suppliers. Planned to fix on MK.)
  SVC_COMMON_t* p_ctx = &g_resmgr;
  switch (resourcemanager_code) {
    case DIAGCODE_VIDEOINHALEX_OVERTEMP_3:
      SS_PUTDIAG(p_ctx->hApp, DIAGCODE_VIDEOINHALEX_OVERTEMP_3);
      break;
    case DIAGCODE_VIDEOINHALEX_OVERTEMP_2:
      SS_PUTDIAG(p_ctx->hApp, DIAGCODE_VIDEOINHALEX_OVERTEMP_2);
      break;
    case DIAGCODE_VIDEOINHALEX_OVERTEMP_1:
      SS_PUTDIAG(p_ctx->hApp, DIAGCODE_VIDEOINHALEX_OVERTEMP_1);
      break;
    default:
      break;
  }
#endif
}

/* drop_caches Start update Task */
static void start_drop_caches(void) {
  pid_t pid = fork();
  if (pid == 0) {
    if (setuid(0) == -1) {  // LCOV_EXCL_BR_LINE 5: setuid's error case
      // LCOV_EXCL_START 5: setuid's error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to setuid(0)-1, errno=%d", errno);
      // LCOV_EXCL_STOP
    } else {
      struct sched_param param;
      // Exec drop_caches
      param.sched_priority = 0;
      if (sched_setscheduler(getpid(), SCHED_OTHER, &param) < 0) {  // LCOV_EXCL_BR_LINE 5: sched_setscheduler's error case
        // LCOV_EXCL_START 5: sched_setscheduler's error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to sched_setscheduler errno=%d",
               errno);
        // LCOV_EXCL_STOP
      } else {
        execl(DROP_CACHES_PG_PATH, basename(DROP_CACHES_PG_PATH), NULL);
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to execl %s errno=%d",
               DROP_CACHES_PG_PATH,
               errno);
      }
    }
    exit(1);
  }
  if (pid == (pid_t) -1) {  // LCOV_EXCL_BR_LINE 5: fork's error case
    // LCOV_EXCL_START 5: fork's error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to fork=-1, errno=%d", errno);
    // LCOV_EXCL_STOP
  }
}

static unsigned long tick_msec_get(void) {
  struct timespec tp;

  if (clock_gettime(CLOCK_MONOTONIC, &tp) != 0) {
    return 0; /* NG */
  }
  return ((tp.tv_sec * 1000) + (tp.tv_nsec / 1000000));
}

#define UNITAUTH_MONITOR "/tmp/UNITAUTH_MONITOR"
#define CHECK_UNITAUTH_MONITOR_TIME (2 * 1000)  // 2sec

static void check_unitauth(void) {
  int fd;
  int ret;
  char buf[64];
  pid_t pid_uauth;
  unsigned long ctime_uauth;  // NOLINT
  unsigned long ctime;  // NOLINT
  ssize_t size;

  fd = open(UNITAUTH_MONITOR, O_RDONLY);
  if (fd == -1) {
    return;
  }

  pid_uauth = 0;
  ctime_uauth = 0;
  size = read(fd, buf, sizeof(buf));
  close(fd);
  if (size != -1) {  // LCOV_EXCL_BR_LINE 5: read's error case
    ret = sscanf(buf, "%u %lu", &pid_uauth, &ctime_uauth);
    if ((ret == 0) || (ret == EOF)) {  // LCOV_EXCL_BR_LINE 5: C sscanf error
      // LCOV_EXCL_START 5: C sscanf error
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      return;
      // LCOV_EXCL_STOP
    }
  }
  if (ctime_uauth) {   // 0:ignore
    ctime = tick_msec_get();
    if (ctime > ctime_uauth) {
      if (((ctime - ctime_uauth) > CHECK_UNITAUTH_MONITOR_TIME)
          && (getpid() != pid_uauth)) {
//        print_backtrace_pid (pid_uauth); // backtrace in kernel.log
      }
    }
  }
}

/**********************************************
 *  Main Function
 **********************************************/
int32_t main(int32_t argc, char* argv[]) {
  int32_t mainRet = -1;
  int32_t rpcFd;
  int32_t timerFd;
  struct itimerspec tm;
  int32_t timerFd2;
  struct itimerspec tm2;
  uint64_t exp;
  int32_t maxFd = 0;
  fd_set fds;
  struct sched_param prcwParam;
  pthread_t ptPRCW;
  int32_t sec = 0;
  int32_t ret;

  int32_t cpu_ret;
  bool fork_dsp_task = false;
  CL_ProcessAttr_t attr;
  const char *prName;
  const char *args[iProcess_MAXIMUM_NUMBER_OF_PROCESS_ARGUMENTS];
  UI_8 mode;
  EFrameworkunifiedStatus eRet;
  struct stat statbuf;

  RPC_ID rpcId = RESMGR_RPC_ID;
  SVC_COMMON_t* p_ctx = &g_resmgr;
  EFrameworkunifiedStatus resourcemanagerRet = eFrameworkunifiedStatusOK;

  {
    const char* nfsenv = getenv("AGL_NFS");
    isNfs = (nfsenv && strcmp(nfsenv, "y") == 0) ? 1 : 0;
  }

  /* Clear context */
  ctxCreate(p_ctx, argc, argv);

  /* Start debug information display task */
  {
    char *tmp;
    tmp = getenv("AGL_DEVDIAG_FLAG");
    if ((tmp == NULL) || strcmp(tmp, "ON")) {  // != "ON"
      resourcemanagerlog_output_debug_info = eFrameworkunifiedStatusFail;
    } else {
      resourcemanagerlog_output_debug_info = eFrameworkunifiedStatusOK;
    }
  }
  eRet = resourcemanagerlog_flag_check(&mode);
  if (eRet == eFrameworkunifiedStatusOK && mode == FRAMEWORKUNIFIEDLOG_FLAG_MODE_DEBUG) {
    fork_dsp_task = true;
  }
  if (fork_dsp_task) {
    // LCOV_EXCL_START 8: there is no bs_analysis_dispinfo_debug
    if (0 == stat(DEBUG_INFO_DSP_PG_PATH, &statbuf)) {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      if (0 != CL_ProcessCreateAttrInit(&attr)) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "CL_ProcessCreateAttrInit Error");  // LCOV_EXCL_LINE 4: NSFW error case.
      }
      prName = basename(DEBUG_INFO_DSP_PG_PATH);
      if (0 != CL_ProcessCreateAttrSetName(&attr, prName)) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Set Name Error");  // LCOV_EXCL_LINE 4: NSFW error case.
      }
      args[0] = prName;
      args[1] = NULL;
      if (CL_ProcessCreate(DEBUG_INFO_DSP_PG_PATH, (char* const *) args, NULL,
                           &attr) < 0) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "CL_ProcessCreate Error");  // LCOV_EXCL_LINE 4: NSFW error case.
      }
    }
    // LCOV_EXCL_STOP 8: there is no bs_analysis_dispinfo_debug
  }

  /* Create socket */
  g_sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (g_sock < 0) {  // LCOV_EXCL_BR_LINE 5: socket error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "[RESM_ERR]ResMgr Create Socket Failed");  // LCOV_EXCL_LINE 5: socket error case
  }

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "[%s:%d] ResMgr Wakeup", p_ctx->procName,
         getpid());

  RPC_START_SERVER(rpcId);    // Start RPC Server
  RPC_get_fd(rpcId, &rpcFd);      // Event reception FD of RPC

  /* Create proc_watch thread */
  ret = pthread_create(&ptPRCW, NULL, &PRCW_main, reinterpret_cast<void*>(NULL));
  if (ret != 0) {  // LCOV_EXCL_BR_LINE 5: pthread_create error case
    // LCOV_EXCL_START 5: pthread_create error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "[RESM_ERR]ResMgr Create Thread Failed. code[%d]", ret);
    // LCOV_EXCL_STOP 5: pthread_create error case
  } else {
    prcwParam.sched_priority = RESM_SUB_PRIORITY;
    ret = pthread_setschedparam(ptPRCW, SCHED_OTHER, &prcwParam);
    if (ret != 0) {  // LCOV_EXCL_BR_LINE 5: pthread_setschedparam error case
      // LCOV_EXCL_START 5: pthread_setschedparam error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "[RESM_ERR]ResMgr Set Thread Schedparam Failed. code[%d]", ret);
      // LCOV_EXCL_STOP 5: pthread_setschedparam error case
    }
  }

  /* Create timer */
  if ((timerFd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC)) == -1) {  // LCOV_EXCL_BR_LINE 5: timerfd_create error case  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 5: timerfd_create error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "[RESM_ERR]ResMgr Create timerFd Failed. errno[%d]", errno);
    exit(EXIT_FAILURE);
    // LCOV_EXCL_STOP 5: timerfd_create error case
  }
  // for drop_caches
  {
    if ((timerFd2 = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC)) == -1) {  // LCOV_EXCL_BR_LINE 5: timerfd_create error case  // NOLINT[whitespace/line_length]
      // LCOV_EXCL_START 5: timerfd_create error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "[RESM_ERR]ResMgr Create timerFd2 Failed. errno[%d]", errno);
      exit(EXIT_FAILURE);
      // LCOV_EXCL_STOP 5: timerfd_create error case
    }
  }

  // Initialize remaining memory acquisition
  mainFree_kib = 0;
  inactFile_kib = 0;
  memTotal_kib = 0;
  cmaFree_kib = 0;
  cmaTotal_kib = 0;
  get_meminfo();
  g_resmgr.restMem = mainFree_kib + inactFile_kib;
  minRestMem = g_resmgr.restMem;
  g_resmgr.restMemFlag = TRUE;
  g_resmgr.restCma = cmaFree_kib;
  minRestCma = g_resmgr.restCma;
  g_resmgr.restCmaFlag = TRUE;

  tm.it_value.tv_sec = MONITORING_START_DELAT_TIME;
  tm.it_value.tv_nsec = 0;
  tm.it_interval.tv_sec = 1;
  tm.it_interval.tv_nsec = 0;
  if (timerfd_settime(timerFd, 0, &tm, NULL) == -1) {  // LCOV_EXCL_BR_LINE 5: timerfd_settime error case
    // LCOV_EXCL_START 5: timerfd_settime error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "[RESM_ERR]ResMgr Set timerFd Failed. errno[%d]", errno);
    exit(EXIT_FAILURE);
    // LCOV_EXCL_STOP 5: timerfd_settime error case
  }

  if (timerFd2 != -1) {  // LCOV_EXCL_BR_LINE 200: timerFd2 can't be -1
    tm2.it_value.tv_sec = DROP_CACHES_START_DELAT_TIME;
    tm2.it_value.tv_nsec = 0;
    tm2.it_interval.tv_sec = 0;
    tm2.it_interval.tv_nsec = 0;
    if (timerfd_settime(timerFd2, 0, &tm2, NULL) == -1) {  // LCOV_EXCL_BR_LINE 5: timerfd_settime error case
      // LCOV_EXCL_START 5: timerfd_settime error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "[RESM_ERR]ResMgr Set timerFd2 Failed. errno[%d]", errno);
      exit(EXIT_FAILURE);
      // LCOV_EXCL_STOP 5: timerfd_settime error case
    }
  }

  /* API to Publish Service Availability Notification. */
  resourcemanagerRet = FrameworkunifiedPublishServiceAvailability(p_ctx->hApp, TRUE);
  if (eFrameworkunifiedStatusOK != resourcemanagerRet) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "Failed to Publish Service Availability Notification:0x%x", resourcemanagerRet);
    exit(EXIT_FAILURE);
    // LCOV_EXCL_STOP 4: NSFW error case.
  }


  /* CPU Load init */
  g_fifo_status = STATUS_CHECK_CPU;
  g_tss_status = STATUS_CHECK_CPU;
  g_cpuloadRate1000 = 0;
  init_cpuload();
  cpu_ret = get_cpuload(g_cpuload);
  if ( cpu_ret != 0 ) {  // LCOV_EXCL_BR_LINE 200: cpu_ret must be 0
    // LCOV_EXCL_START 200: cpu_ret must be 0
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__, "Get cpuload Failed");
    g_cpuloadRate1000 = 0;
    // LCOV_EXCL_STOP
  }


  while (1) {
    FD_ZERO(&fds);
    FD_SET(rpcFd, &fds);
    maxFd = MY_MAX(maxFd, rpcFd);

    FD_SET(p_ctx->nsFd, &fds);
    maxFd = MY_MAX(maxFd, p_ctx->nsFd);

    FD_SET(timerFd, &fds);
    maxFd = MY_MAX(maxFd, timerFd);

    if (timerFd2 != -1) {
      FD_SET(timerFd2, &fds);
      maxFd = MY_MAX(maxFd, timerFd2);
    }
    ret = select(maxFd + 1, &fds, NULL, NULL, NULL);
    if (ret < 0) {  // LCOV_EXCL_BR_LINE 5: select error case
      // LCOV_EXCL_START 5: select error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      if (errno != EINTR) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "[RESM_ERR]ResMgr Handle Error");
        exit(EXIT_FAILURE);
      }
      continue;
      // LCOV_EXCL_STOP 5: select error case
    }
    /* API CALL from RPC */
    if ((timerFd2 != -1) && FD_ISSET(timerFd2, &fds)) {
      start_drop_caches();
      timerFd2 = -1;
    }

    /* API CALL from RPC */
    if (FD_ISSET(rpcFd, &fds)) {
      RPC_process_API_request(rpcId);
    }
    /* Events from the NSFW */
    if (FD_ISSET(p_ctx->nsFd, &fds)) {
      FrameworkunifiedDispatchProcessWithoutLoop(p_ctx->hApp);
    }

    /* Timer expiration */
    if (FD_ISSET(timerFd, &fds)) {
      read(timerFd, &exp, sizeof(uint64_t));
      /* CPU load monitoring */
      if ((sec % WTC_CPU_INTERVAL) == 0) {
        watchCPUStatus();
      }
      /* UNITAUTH monitor */
      check_unitauth();

      if (sec >= RESET_SEC) {
        sec = 0;
      }
      sec++;
    }
  }

  // Close socket
  close(g_sock);

  mainRet = 0;
  RPC_end(rpcId);

  return mainRet;
}

/*********************************************************************************
 * Session Related
 *********************************************************************************/
/* Numbering session ID */
static int32_t get_new_id(uint32_t* ssnId) {
  int32_t i;
  SSN_INFO_t* p_ssnInfo;

  for (i = 0; i < EV_MAX_IDS_IN_THREAD; i++) {
    p_ssnInfo = &g_resmgr.ssnInfo[i];

    if (p_ssnInfo->useFlag) {
      // in-use
      continue;
    }
    *ssnId = i;
    p_ssnInfo->useFlag = TRUE;

    return 0;
  }

  return -1;
}

/*********************************************************************************
 * Memory monitoring
 *********************************************************************************/
/* Memory monitoring */
void watchMem(void) {
  uint32_t restMem_b;
  SSN_INFO_t* p_ssnInfo;
  uint32_t ssnId;
  int32_t i;
  int32_t ret;

  // Get remaining memory
  mainFree_kib = 0;
  inactFile_kib = 0;
  memTotal_kib = 0;
  cmaFree_kib = 0;
  cmaTotal_kib = 0;
  g_resmgr.restMemFlag = FALSE;
  g_resmgr.restCmaFlag = FALSE;
  ret = get_meminfo();
  // KiB -> Byte
  // [Note] Unit of the value gotten by the meminfo is KiB
  restMem_b = (mainFree_kib + inactFile_kib) * KIBIBYTE_TO_BYTE;

  if (ret != 0) {
    // Failed to get remaining memory info
    FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__, "ResMgr get_meminfo Failed");
    return;
  }
  g_resmgr.restMem = mainFree_kib + inactFile_kib;
  g_resmgr.restMemFlag = TRUE;
  g_resmgr.restCma = cmaFree_kib;
  g_resmgr.restCmaFlag = TRUE;

  if (g_resmgr.restMem < minRestMem) {
    // Update minimum available memory
    minRestMem = g_resmgr.restMem;
  }
  if (g_resmgr.restCma < minRestCma) {
    // Update minimum available CMA
    minRestCma = g_resmgr.restCma;
  }

  for (i = 0; i < RESM_INET_IF_MAX; i++) {
    p_ssnInfo = &g_resmgr.ssnInfo[i];
    if (!p_ssnInfo->useFlag) {
      // Unused sessions
      continue;
    }
    ssnId = i;

    // Check event type
    if (p_ssnInfo->reqEv.reqEvent != RESM_EV_MEM) {
      // Other than memory monitoring
      continue;
    }
    // Check free memory
    if (p_ssnInfo->reqEv.prm.restMemThresh > restMem_b) {
      // Issue event
      ret = EV_set_flag(Resm_Flag_ID_Base + ssnId, RESM_EV_MEM);
      if (ret != EV_OK) {  // LCOV_EXCL_BR_LINE 200: EV_set_flag will not return ng
        // LCOV_EXCL_START 200: EV_set_flag will not return ng
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__,
               "ResMgr EV_set_flag Failed. ssnId[%d]", ssnId);
        // LCOV_EXCL_STOP 200: EV_set_flag will not return ng
      }
    }
  }

  return;
}

void FlushMemInfo(void) {
  FRAMEWORKUNIFIEDLOG(
    ZONE_INFO,
    __FUNCTION__,
    "MEMORY Info(KB):<PEAK>REST(%d) [Memtotal(%d)] / CMA Info(KB):<PEAK>REST(%d) [CMAtotal(%d)]",
    minRestMem, memTotal_kib, minRestCma, cmaTotal_kib);
}

/* Compare memory information */
static int32_t comp_meminfo_tbl(const void* data1, const void* data2) {
  return strcmp(((const meminfo_tbl*) data1)->name,
                ((const meminfo_tbl*) data2)->name);
}

/* Get memory information */
static int32_t get_meminfo(void) {
  int32_t meminfo_fd = -1;
  char srch_name[16];
  char buf[2048];
  meminfo_tbl target = { srch_name, NULL };
  meminfo_tbl* found;
  char* head;
  char* tail;
  int32_t read_byte;
  /* Strings must be in ascending order when adding entries to this table (for bsearch) */
  static const meminfo_tbl mem_table[] = {
    { "CmaFree", &cmaFree_kib },
    { "CmaTotal", &cmaTotal_kib },
    { "Inactive(file)", &inactFile_kib },
    { "MemFree", &mainFree_kib },
    { "MemTotal", &memTotal_kib },
  };
  const int32_t mem_table_count = sizeof(mem_table) / sizeof(meminfo_tbl);

  if (meminfo_fd == -1) {
    meminfo_fd = open(MEMINFO_FILE, O_RDONLY);
    if (meminfo_fd == -1) {
      fflush(NULL);
      return -1;
    }
  }

  lseek(meminfo_fd, 0L, SEEK_SET);
  read_byte = read(meminfo_fd, buf, sizeof buf - 1);
  if (read_byte < 0) {  // LCOV_EXCL_BR_LINE 200: if file exist, it will not be -1
    // LCOV_EXCL_START 200: if file exist, it will not be -1
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    fflush(NULL);
    close(meminfo_fd);
    return -1;
    // LCOV_EXCL_STOP 200: if file exis, it will not be -1
  }
  buf[read_byte] = '\0';

  head = buf;
  while (1) {
    tail = strchr(head, ':');
    if (!tail) {
      break;
    }

    *tail = '\0';
    if (strlen(head) >= sizeof(srch_name)) {
      head = tail + 1;
    } else {
      strcpy(srch_name, head);  // NOLINT
      found = reinterpret_cast<meminfo_tbl *>(bsearch(&target, mem_table, mem_table_count,
                                      sizeof(meminfo_tbl), comp_meminfo_tbl));
      head = tail + 1;
      if (found) {
        *(found->value) = strtoul(head, &tail, 10);
      }
    }
    tail = strchr(head, '\n');
    if (!tail)
      break;
    head = tail + 1;
  }
  close(meminfo_fd);

  return 0;
}

/*********************************************************************************
 * Network monitoring
 *********************************************************************************/
/* Get system information */
static int32_t getInetStatus(RESM_INET_STATUS_t* p_iStatus) {
  FILE* fh;
  char buf[READLINE_MAX_SIZE];
  char* tmp;
  char name[IFNAMSIZ];
  char hwaddr[HWADDR_LEN];
  uint32_t rx_b;
  uint32_t rx_pckt;
  uint32_t rx_err;
  uint32_t rx_drp;
  uint32_t rx_fifo;
  uint32_t rx_frm;
  uint32_t rx_mlt;
  uint32_t tx_b;
  uint32_t tx_pckt;
  uint32_t tx_err;
  uint32_t tx_drp;
  uint32_t tx_fifo;
  uint32_t tx_cll;
  uint32_t tx_crr;
  int32_t ret;

  // Open file
  fh = fopen(PROCNET_DEV_FILE, "r");
  if (fh == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__,
           "ResMgr getInetStatus() File Open Error. errno[%d]", errno);
    return -1;
  }

  // Initialize interface count
  p_iStatus->ifNum = 0;

  while (fgets(buf, sizeof(buf), fh)) {
    // Get Alias name
    tmp = get_aliasName(name, buf);
    if (tmp == NULL) {
      // No alias name
      continue;
    }
    // Get amount of receive/transmit data
    ret = sscanf(tmp, "%u%u%u%u%u%u%u%*u%u%u%u%u%u%u%u", &rx_b, &rx_pckt,
                 &rx_err, &rx_drp, &rx_fifo, &rx_frm, &rx_mlt, &tx_b, &tx_pckt,
                 &tx_err, &tx_drp, &tx_fifo, &tx_cll, &tx_crr);
    if (ret != 14) {  // LCOV_EXCL_BR_LINE 200: ret is always 14
      // LCOV_EXCL_START 200: ret is always 14
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__,
             "ResMgr getInetStatus() GET Rx and Tx size Failed");
      continue;
      // LCOV_EXCL_STOP 200: ret is always 14
    }
    // Get hardware address
    ret = get_hwaddr(name, hwaddr);
    if (ret != 0) {  // LCOV_EXCL_BR_LINE 5: get_hwaddr will not return !0
      // LCOV_EXCL_START 5: get_hwaddr will not return !0
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__,
             "ResMgr getInetStatus() GET hwaddr Failed");
      continue;
      // LCOV_EXCL_STOP 5: get_hwaddr will not return !0
    }

    // Set values in the system information structure
    strcpy(p_iStatus->ifInfo[p_iStatus->ifNum].name, name);  // NOLINT
    p_iStatus->ifInfo[p_iStatus->ifNum].rxSize = rx_b / BYTE_TO_KIBIBYTE;
    p_iStatus->ifInfo[p_iStatus->ifNum].txSize = tx_b / BYTE_TO_KIBIBYTE;
    memcpy(p_iStatus->ifInfo[p_iStatus->ifNum].hwaddr, hwaddr, HWADDR_LEN);

    p_iStatus->ifNum++;

    if (p_iStatus->ifNum >= RESM_INET_IF_MAX) {
      break;
    }
  }

  // Termination processing
  fclose(fh);

  if (p_iStatus->ifNum == 0) {
    return -1;
  }
  return 0;
}

/* Get Alias name */
static char *get_aliasName(char* name, char* line) {
  char* dot;
  char* dotname;

  while (isspace(*line)) {
    line++;
  }
  while (*line) {
    if (isspace(*line)) {
      *name++ = '\0';
      return NULL;
    }

    if (*line == ':') {
      dot = line, dotname = name;
      *name++ = *line++;

      while (isdigit(*line)) {
        *name++ = *line++;
      }
      if (*line != ':') {
        line = dot;
        name = dotname;
      }
      if (*line == '\0') {
        return NULL;
      }
      line++;
      break;
    }
    *name++ = *line++;
  }
  *name++ = '\0';

  return line;
}

/* Get hardware address */
static int32_t get_hwaddr(char* name, char* hwaddr) {
  struct ifreq ifr;
  int32_t ret;

  if (g_sock < 0) {  // LCOV_EXCL_BR_LINE 6: g_sock is not -1
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return -1;    // LCOV_EXCL_LINE 6: g_sock is not -1
  }
  // Initialization
  memset(&ifr, 0, sizeof(ifr));

  // Set alias name
  strncpy(ifr.ifr_name, name, (sizeof(ifr.ifr_name) - 1));

  // Get hardware address
  ret = ioctl(g_sock, SIOCGIFHWADDR, &ifr);
  if (ret < 0) {  // LCOV_EXCL_BR_LINE 5: ioctl error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return -1;  // LCOV_EXCL_LINE 5: ioctl error case
  }
  memcpy(hwaddr, ifr.ifr_hwaddr.sa_data, HWADDR_LEN);

  return 0;
}

/*********************************************************************************
 * CPU monitoring
 *********************************************************************************/
// // CPU monitoring
// static void watchCPU() {
// Main processing of CPU monitoring
static void watchCPUStatus() {
  unsigned long fifo_task_occ = 0;
  unsigned long tss_task_occ = 0;
  int32_t fifo_status = g_fifo_status;
  int32_t tss_status = g_tss_status;
  int32_t cpu_load_status = CPU_TASK_INIT;

//  int32_t ret;
//  static int32_t cpuLogTimer = 0;
  if ((g_fifo_status == STATUS_CHECK_CPU) || (g_tss_status == STATUS_CHECK_CPU)) {
    watchCPU();

//  if (cpuLogTimer > 0) {  // LCOV_EXCL_BR_LINE 6:Because the condition cannot be set
//    // Make to progress the timer
//    cpuLogTimer -= WTC_CPU_INTERVAL;  // LCOV_EXCL_LINE 6:Because the condition cannot be set
//  }
    if (g_cpuloadRate1000 >= CPU_LOAD_THRESHOLD) {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "CPU TOTAL:(%d.%d%%)",g_cpuloadRate1000/10, g_cpuloadRate1000%10);
      int i;
      int32_t cpuloadRate1000;
      for (i = 1; i < g_cpu_num; i++) {
        char buf[16];
        cpuloadRate1000 = calc_cpuloadRate_each(i);
        sprintf (buf, "%s(%d.%d%%) ", g_cpuload[i].cpuname, cpuloadRate1000/10, cpuloadRate1000%10);
        FRAMEWORKUNIFIEDLOG (ZONE_INFO, __FUNCTION__, "%s", buf);
      }
      if (g_fifo_status == STATUS_CHECK_CPU) {  // LCOV_EXCL_BR_LINE 6: g_fifo_status must be STATUS_CHECK_CPU
        fifo_status = STATUS_WATCH_PROCESS;
        g_fifo_timer = 0;
      }
      if (g_tss_status == STATUS_CHECK_CPU) {  // LCOV_EXCL_BR_LINE 6: g_tss_status must be STATUS_CHECK_CPU
        tss_status = STATUS_WATCH_PROCESS;
        g_tss_timer = 0;
      }
      if ((g_fifo_status != STATUS_WATCH_PROCESS ) && ( g_tss_status != STATUS_WATCH_PROCESS)) {  // LCOV_EXCL_BR_LINE 6: g_fifo_status must be STATUS_CHECK_CPU and g_tss_status must be STATUS_CHECK_CPU   // NOLINT[whitespace/line_length]
        logging_cpuload_custom(CPU_TASK_INIT);
      }
    }
  }

#if 0
  // Get CPU usage
  if (g_resmgr.cpuloadRate == CPULOAD_INVALID) {
    // First time
    init_cpuload();
    ret = get_cpuload(g_cpuload);
    if (ret == 0) {  // LCOV_EXCL_BR_LINE 5: get_cpuload will not return -1
      g_resmgr.cpuloadRate = CPULOAD_READY;
    } else {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__, "watchCPU() Get cpuload Failed");  // LCOV_EXCL_LINE 5: get_cpuload will not return -1  // NOLINT[whitespace/line_length]
    }
#endif
  if ((g_fifo_status == STATUS_WATCH_PROCESS) || (g_tss_status == STATUS_WATCH_PROCESS)) {
    if (g_fifo_status == STATUS_WATCH_PROCESS) {
      g_fifo_timer = g_fifo_timer + WTC_CPU_INTERVAL;
      if ((g_fifo_timer == WTC_CPU_INTERVAL) || (g_fifo_timer >= FIFO_TIMER_LIMIT)) {  // LCOV_EXCL_BR_LINE 200: g_fifo_timer must be bigger than WTC_CPU_INTERVAL/FIFO_TIMER_LIMIT  // NOLINT[whitespace/line_length]
        cpu_load_status = CPU_TASK_SHOW_AF;
      } else {
        // LCOV_EXCL_START 200: g_fifo_timer must be bigger than WTC_CPU_INTERVAL/FIFO_TIMER_LIMIT
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        logging_cpuload_custom(FIFO_TASK_SHOW);
        // LCOV_EXCL_STOP
      }
    }
#if 0
    return;
  } else {
    memcpy(g_cpuload_pre, g_cpuload, sizeof(*g_cpuload_pre)*g_cpu_num);
    ret = get_cpuload(g_cpuload);
    if (ret != 0) {  // LCOV_EXCL_BR_LINE 5: get_cpuload will not return -1
      // LCOV_EXCL_START 5: get_cpuload will not return -1
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__, "watchCPU() Get cpuload Failed");
      return;
      // LCOV_EXCL_STOP 5: get_cpuload will not return -1
#endif
    if (g_tss_status == STATUS_WATCH_PROCESS) {
      g_tss_timer = g_tss_timer + WTC_CPU_INTERVAL;
      if ((g_tss_timer == WTC_CPU_INTERVAL) || (g_tss_timer >= TSS_TIMER_LIMIT)) {
        cpu_load_status = CPU_TASK_SHOW_AF;
      } else {
        logging_cpuload_custom(TSS_TASK_SHOW);
      }
    }
#if 0
  }
  // Calculate CPU usage (Notes! Return as a thousandth rate(10 times of %)
  g_cpuloadRate1000 = calc_cpuloadRate();
  g_resmgr.cpuloadRate = g_cpuloadRate1000 / 10;

  ret = chk_logging(g_resmgr.cpuloadRate, cpuLogTimer);
  if (ret == CPULOG_LOGGING) {
    // Logging
#endif
    if (cpu_load_status == CPU_TASK_SHOW_AF) {
#if 0
      int i;
      int32_t cpuloadRate1000;
      char *cpunames = (char *) malloc(  // NOLINT
          sizeof("[CpuHighLoad]") + (sizeof("cpuXX(xxx%) ") * g_cpu_num));
      cpuloadRate1000 = calc_cpuloadRate();
      sprintf(cpunames, "[CpuHighLoad]%s(%d%%) ", g_cpuload[0].cpuname, cpuloadRate1000/10);  // NOLINT
      for (i = 1; i < g_cpu_num; i++) {
        char buf[16];
        cpuloadRate1000 = calc_cpuloadRate_each(i);
        sprintf(buf, "%s(%d%%) ", g_cpuload[i].cpuname, cpuloadRate1000 / 10);  // NOLINT
        strcat(cpunames, buf);  // NOLINT
      }
      free(cpunames);
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "%s", cpunames);
    }
#endif
      logging_cpuload_custom(cpu_load_status);
    }
    if (g_fifo_status == STATUS_WATCH_PROCESS) {
      fifo_task_occ = logging_cpuload_custom(CPU_FIFO_TASK_GET_OCCUPANCY);
      if ((fifo_task_occ >= TASK_STAT_THRESHOLD) && (g_fifo_timer >= FIFO_TIMER_LIMIT)) {
        fifo_status = STATUS_IDOL;
        g_fifo_timer = 0;
        exec_perf(logging_cpuload_custom(CPU_FIFO_TASK_GET_ID));
      } else if (fifo_task_occ < TASK_STAT_THRESHOLD) {
        fifo_status = STATUS_CHECK_CPU;
        g_fifo_timer = 0;
        logging_cpuload_custom(CPU_FIFO_TASK_GET_ID);
      }
    }
    if (g_tss_status == STATUS_WATCH_PROCESS) {
      tss_task_occ = logging_cpuload_custom(CPU_TSS_TASK_GET_OCCUPANCY);
      if ((tss_task_occ >= TASK_STAT_THRESHOLD) && (g_tss_timer >= TSS_TIMER_LIMIT)) {
        tss_status = STATUS_IDOL;
        g_tss_timer = 0;
        exec_perf(logging_cpuload_custom(CPU_TSS_TASK_GET_ID));
      } else if(tss_task_occ < TASK_STAT_THRESHOLD) {
        tss_status = STATUS_CHECK_CPU;
        g_tss_timer = 0;
        logging_cpuload_custom(CPU_TSS_TASK_GET_ID);
      }
    }
#if 0
    logging_cpuload_custom();
    logging_cpuload();
    // Set timer
    cpuLogTimer = CPU_HIGH_LOAD_LOG_FREQ;
  } else if (ret == CPULOG_TIMER_CLEAR) {
    // Clear Timer
    cpuLogTimer = 0;
  }

  return;
#endif
    logging_cpuload_custom(CPU_TASK_SHOW_BF);
  }
  if ((g_fifo_status == STATUS_IDOL) || (g_tss_status == STATUS_IDOL)) {
    if (g_fifo_status == STATUS_IDOL) {
      g_fifo_timer = g_fifo_timer + WTC_CPU_INTERVAL;
      if (g_fifo_timer >= CPU_HIGH_LOAD_LOG_FREQ) {
        fifo_status = STATUS_CHECK_CPU;
        g_fifo_timer = 0;
      }
    }
    if (g_tss_status == STATUS_IDOL) {
      g_tss_timer = g_tss_timer + WTC_CPU_INTERVAL;
      if (g_tss_timer >= CPU_HIGH_LOAD_LOG_FREQ) {
        tss_status = STATUS_CHECK_CPU;
        g_tss_timer = 0;
      }
    }
  }
  g_fifo_status = fifo_status;
  g_tss_status = tss_status;

  return;
}



// CPU monitoring
static void
watchCPU() {
  int32_t ret;

  memcpy(g_cpuload_pre, g_cpuload, sizeof(*g_cpuload_pre)*g_cpu_num);
  ret = get_cpuload(g_cpuload);
  if ( ret != 0 ) {
    FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__, "watchCPU() Get cpuload Failed");
    g_cpuloadRate1000 = 0;
  } else {
    // Calculate CPU usage (Notes! Return as a thousandth rate(10 times of percentage)
    g_cpuloadRate1000 = calc_cpuloadRate();
  }

  return;
}


static void init_cpuload(void) {
  int fd;
  char buf[READ_MAX_SIZE];
  int32_t ret;
  CPU_INFO_t p_cpu;
  ssize_t rsize;
  char *p, *ep;
  struct timespec req = { 0, 1000000 };  // 1msec

  g_cpu_num = 0;
  g_cpuload_pre = NULL;

  fd = open(PROC_STAT_FILE, O_RDONLY);
  if (fd == -1) {  // LCOV_EXCL_BR_LINE 5: open error case
    // LCOV_EXCL_START 5: open error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__, "ResMgr File Open Error. errno[%d]",
           errno);
    return;
    // LCOV_EXCL_STOP 5: open error case
  }
  rsize = read(fd, buf, sizeof(buf));
  if (rsize == -1) {  // LCOV_EXCL_BR_LINE 5: read error case
    // LCOV_EXCL_START 5: read error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__, "ResMgr File Read Error. errno[%d]",
           errno);
    close(fd);
    return;
    // LCOV_EXCL_STOP 5: read error case
  }
  nanosleep(&req, NULL);
  p = buf;
  ep = buf + rsize;
  while (p < ep) {
    if (strncmp(p, "cpu", 3) == 0) {
      ret = sscanf(p, "%8s %d %d %d %d %d %d %d %d %d %d", &p_cpu.cpuname[0],
                   &p_cpu.user, &p_cpu.nice, &p_cpu.system, &p_cpu.idle,
                   &p_cpu.iowait, &p_cpu.irq, &p_cpu.softirq, &p_cpu.steal,
                   &p_cpu.guest, &p_cpu.guest_nice);
      if (ret < 11) {  // LCOV_EXCL_BR_LINE 200: ret will always 11
        // LCOV_EXCL_START 200: ret will always 11
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__,
               "ResMgr get_cpuload() File Read Error");
        close(fd);
        return;
        // LCOV_EXCL_STOP 200: ret will always 11
      }
      g_cpu_num++;
      while (++p < ep) {
        if (*p == LF) {
          p++;
          break;
        }
      }
    } else {
      break;
    }
  }
  close(fd);

  g_cpuload_pre = reinterpret_cast<CPU_INFO_t *>(malloc(sizeof(CPU_INFO_t) * g_cpu_num));
  g_cpuload = reinterpret_cast<CPU_INFO_t *>(malloc(sizeof(CPU_INFO_t) * g_cpu_num));
  if ((g_cpuload_pre == NULL) || (g_cpuload == NULL)) {  // LCOV_EXCL_BR_LINE 5: malloc error case
    // LCOV_EXCL_START 5: malloc error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__,
           "ResMgr malloc Error. g_cpu_num[%d] errno[%d]", g_cpu_num, errno);
    return;
    // LCOV_EXCL_STOP 5: malloc error case
  }

  FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__, "ResMgr cpu_num:%d", g_cpu_num);

  return;
}

// Get CPU usage
static int32_t get_cpuload(CPU_INFO_t* p_cpu) {
  int fd;
  char buf[READ_MAX_SIZE];
  int32_t ret;
  int i = 0;
  ssize_t rsize;
  char *p, *ep;
  struct timespec req = { 0, 1000000 };  // 1msec

  // Open file
  fd = open(PROC_STAT_FILE, O_RDONLY);
  if (fd == -1) {  // LCOV_EXCL_BR_LINE 5: open's error case
    // LCOV_EXCL_START 5: open's error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__,
           "ResMgr get_cpuload() File Open Error. errno[%d]", errno);
    return -1;
    // LCOV_EXCL_STOP
  }
  rsize = read(fd, buf, sizeof(buf));
  if (rsize == -1) {  // LCOV_EXCL_BR_LINE 5: read error case
    // LCOV_EXCL_START 5: read error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__, "ResMgr File Read Error. errno[%d]",
           errno);
    close(fd);
    return -1;
    // LCOV_EXCL_STOP 5: read error case
  }
  nanosleep(&req, NULL);
  p = buf;
  ep = buf + rsize;
  while (p < ep) {
    if (strncmp(p, "cpu", 3) == 0) {
      ret = sscanf(p, "%8s %d %d %d %d %d %d %d %d %d %d", &p_cpu[i].cpuname[0],
                   &p_cpu[i].user, &p_cpu[i].nice, &p_cpu[i].system,
                   &p_cpu[i].idle, &p_cpu[i].iowait, &p_cpu[i].irq,
                   &p_cpu[i].softirq, &p_cpu[i].steal, &p_cpu[i].guest,
                   &p_cpu[i].guest_nice);
      if (ret < 11) {  // LCOV_EXCL_BR_LINE 200: ret will always 11
        // LCOV_EXCL_START 200: ret will always 11
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__,
               "ResMgr get_cpuload() File Read Error");
        close(fd);
        return -1;
        // LCOV_EXCL_STOP 200: ret will always 11
      }
      i++;
      while (++p < ep) {
        if (*p == LF) {
          p++;
          break;
        }
      }
    } else {
      break;
    }
  }
  close(fd);

  return 0;
}

// Calcurate CPU usage
static int32_t calc_cpuloadRate(void) {
  int val;
  int val_others;
  int total;
  int rate;
  CPU_INFO_t* info;
  CPU_INFO_t* info_pre;

  info = &g_cpuload[0];
  info_pre = &g_cpuload_pre[0];

  val = (info->user - info_pre->user)
      + (info->system - info_pre->system)
      + (info->irq - info_pre->irq)
      + (info->softirq - info_pre->softirq);

  val_others = (info->idle - info_pre->idle)
      + (info->iowait - info_pre->iowait);

  total = val + val_others;
  rate = (total > 0) ? (val*1000 / total) : 0;

  return rate;
}

// Calcurate CPU usage for each CPU
static int32_t calc_cpuloadRate_each(int num) {
  int val, valn;
  int rate;
  CPU_INFO_t* info;
  CPU_INFO_t* info_pre;

  // cpu
  info = &g_cpuload[0];
  info_pre = &g_cpuload_pre[0];
  val = (info->user - info_pre->user)
      + (info->system - info_pre->system)
      + (info->irq - info_pre->irq)
      + (info->softirq - info_pre->softirq);

  // cpu-num
  info = &g_cpuload[num];
  info_pre = &g_cpuload_pre[num];
  valn = (info->user - info_pre->user)
    + (info->system - info_pre->system)
    + (info->softirq - info_pre->softirq);

  rate = valn * 1000 / val;
  return rate;
}


//// Logging check at CPU overload
//static int32_t chk_logging(int32_t cpuload, int32_t timer) {
//  if (cpuload >= CPU_LOAD_THRESHOLD) {
//    if (timer > 0) {
//      return CPULOG_NO_LOGGING;
//    }
//    return CPULOG_LOGGING;
//  }
//
//  return CPULOG_TIMER_CLEAR;
//}


// Check if cmd to perf
  // LCOV_EXCL_START 8: dead code
bool valid_perf_cmd(char *buf) {  // LCOV_EXCL_BR_LINE 8: dead code
  const char *cmd2exclude[] = { "top", "init", "bash", NULL };
  char cmdstr[64];

  for (int i = 0;; i++) {
    if (cmd2exclude[i] == NULL) {
      break;
    }
    sprintf(cmdstr, " %s ", cmd2exclude[i]);  // NOLINT
    if (strstr(buf, cmdstr) == NULL) {
      continue;
    } else {
      return false;
    }
  }
  return true;
}
  // LCOV_EXCL_STOP 8: dead code

//// exec perf to pids
//static pid_t pids[PERF_MAX_PROCS];


static bool lower_sched_priority(int niceval) {
  struct sched_param sparam = { };
  sparam.sched_priority = 0;
  if (sched_setscheduler(0, SCHED_OTHER, &sparam) == -1) {  // LCOV_EXCL_BR_LINE 5: sched_setscheduler's error case
    // LCOV_EXCL_START 5: sched_setscheduler's error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed change scheduler to TSS, errno=%d",
           errno);
    return false;
    // LCOV_EXCL_STOP
  }
  if (nice(niceval) == -1) {  // LCOV_EXCL_BR_LINE 5: nice's error case
    // LCOV_EXCL_START 5: nice's error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to add nice val %d, errno=%d",
           niceval, errno);
    return false;
    // LCOV_EXCL_STOP
  }
  return true;
}

/*********************************************************************************
 * exec_perf sub function RECORD perf data processing
 *********************************************************************************/
static void exec_perf_Record_Perf_Data(
    pid_t* c_pids, char* perf_file, char* pidstr, int* status,

//    char pnames[PERF_MAX_PROCS][PERF_PNAME_MAX]) {
    pid_t pids[PERF_MAX_PROCS]) {

  int i;
  int fd;
  pid_t term_pid;
  int waitret;
  bool do_sigkill = false;

  FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__, "+");

  /* RECORD perf data */
  for (i = 0; i < PERF_MAX_PROCS; i++) {
    if (pids[i] <= 0) {
      break;
    }
    sprintf(perf_file, PERF_FILE"%06d", pids[i]);  // NOLINT
    sprintf(pidstr, "%d", pids[i]);  // NOLINT
    if ((c_pids[i] = fork()) == 0) {
      if (lower_sched_priority(CPULOAD_NICEVAL) == false) {  // LCOV_EXCL_BR_LINE 200: lower_sched_priority() must be true
        // LCOV_EXCL_START 200: lower_sched_priority() must be true
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to lower scheduler-1");
        exit(1);
        // LCOV_EXCL_STOP
      }
      if (setuid(0) == -1) {  // LCOV_EXCL_BR_LINE 5: setuid's error case
        // LCOV_EXCL_START 5: setuid's error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to setuid(0)-1, errno=%d",
               errno);
        exit(1);
        // LCOV_EXCL_STOP
      }
      // Redirect STDERR
      fd = open("/dev/null", O_WRONLY);
      if (fd > 0) {  // LCOV_EXCL_BR_LINE 5: open's error case
        dup2(fd, 2);
      }
      // Exec perf
      execl(PERF_PATH, basename(PERF_PATH), "record", "-p", pidstr, "-o",
            perf_file, NULL);
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to execl %s record, errno=%d",
             PERF_PATH,
             errno);
      exit(1);
    }
    if (c_pids[i] == (pid_t) -1) {  // LCOV_EXCL_BR_LINE 5: fork's error case
      // LCOV_EXCL_START 5: fork's error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to fork-1, errno=%d", errno);
      continue;
      // LCOV_EXCL_STOP
    }
    // Kill perf after PERF_RECORD_SPAN sec
    // (Killed by child process to avoid making resm process super-user with setuid.)
    if ((term_pid = fork()) == 0) {
      const struct timespec delay = { PERF_RECORD_SPAN, 0 };
      if (setuid(0) == -1) {  // LCOV_EXCL_BR_LINE 5: setuid's error case
        // LCOV_EXCL_START 5: setuid's error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to setuid(0)-2, errno=%d",
               errno);
        exit(1);
        // LCOV_EXCL_STOP
      }
      nanosleep(&delay, NULL);  // Let perf record run for a while before kill it.
      if (kill(c_pids[i], SIGINT) == -1) {  // LCOV_EXCL_BR_LINE 5: kill's error case
        // LCOV_EXCL_START 5: kill's error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Failed to kill(SIGINT), pid=%d, errno=%d", (int) c_pids[i],
               errno);
        // LCOV_EXCL_STOP
      }
      nanosleep(&delay, NULL);  // Allow perf to do ending procedure.
      exit(0);
    } else if (term_pid == (pid_t) -1) {  // LCOV_EXCL_BR_LINE 5: fork's error case
      // LCOV_EXCL_START 5: fork's error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to fork-2, errno=%d", errno);
      continue;
      // LCOV_EXCL_STOP
    } else {
      if (waitpid(term_pid, status, 0) == -1) {  // LCOV_EXCL_BR_LINE 5: waitpid's error case
        // LCOV_EXCL_START 5: waitpid's error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Failed to waitpid of killer %d, errno=%d", term_pid, errno);
        continue;
        // LCOV_EXCL_STOP
      }
    }
    FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__, "");
    // NOT block even if perf is not terminated
    if ((waitret = waitpid(c_pids[i], status, WNOHANG)) == -1) {  // LCOV_EXCL_BR_LINE 5: waitpid's error case
      // LCOV_EXCL_START 5: waitpid's error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to waitpid of RECORD %d, errno=%d",
             c_pids[i], errno);
      // LCOV_EXCL_STOP
    } else if (waitret == 0) {
      // NOT exited
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to terminate perf record, pid=%d",
             c_pids[i]);
      pids[i] = -2;  // Skip following sequences
      do_sigkill = true;
    } else if (WEXITSTATUS(*status) != 0 && WEXITSTATUS(*status) != 2) {

//      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
//             "perf record %d exited abnormally, target=%s(%d), status=%d",
//             c_pids[i], pnames[i], pids[i], WEXITSTATUS(*status));
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "perf record %d exited abnormally, target=(%d), status=%d",
             c_pids[i], pids[i], WEXITSTATUS(*status));

      pids[i] = -2;  // Skip following sequences
      do_sigkill = false;
    }
    FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__, "");
    if (pids[i] == -2) {
      if ((term_pid = fork()) == 0) {
        const struct timespec delay = { PERF_RECORD_SPAN, 0 };
        if (setuid(0) == -1) {  // LCOV_EXCL_BR_LINE 5: setuid's error case
          // LCOV_EXCL_START 5: setuid's error case
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to setuid(0)-2B, errno=%d",
                 errno);
          exit(1);
          // LCOV_EXCL_STOP
        }
        // Kill "perf record" by SIGKILL signal
        if (do_sigkill) {
          int sigret = kill(c_pids[i], SIGKILL);
          if (sigret == 0) {  // LCOV_EXCL_BR_LINE 5: kill case
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                   "SIGKILL has been sent to perf record process, pid=%d",
                   c_pids[i]);
          } else {
            // LCOV_EXCL_START 5: kill case
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                   "sending SIGKILL to perf record failed, pid=%d, errno=%d",
                   c_pids[i], errno);
            // LCOV_EXCL_STOP 5: kill case
          }
        }
        nanosleep(&delay, NULL);
        // remove perf data file possibly made
        unlink(perf_file);
        FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__, "");
        exit(0);
      } else if (term_pid == (pid_t) -1) {  // LCOV_EXCL_BR_LINE 5: fork's error case
        // LCOV_EXCL_START 5: fork's error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to fork-2B, errno=%d", errno);
        // LCOV_EXCL_STOP
      } else {
        if (waitpid(term_pid, status, 0) == -1) {    // LCOV_EXCL_BR_LINE 5: waitpid's error case
          // LCOV_EXCL_START 5: waitpid's error case
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                 "Failed to waitpid of killer-2 %d, errno=%d", term_pid, errno);
          // LCOV_EXCL_STOP
        }
        if (do_sigkill) {
          if ((waitret = waitpid(c_pids[i], status, WNOHANG)) == -1) { // LCOV_EXCL_BR_LINE 5: waitpid's error case
            // LCOV_EXCL_START 5: waitpid's error case
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                   "Failed to waitpid of RECORD(2) %d, errno=%d", c_pids[i],
                   errno);
            // LCOV_EXCL_STOP
          } else if (waitret == 0) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                   "Failed to terminate perf record by SIGKILL, pid=%d",
                   c_pids[i]);
          }
        }
      }
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__, "");
}

/*********************************************************************************
 * exec_perf sub function make perf file available to default user processing
 *********************************************************************************/

//static int32_t exec_perf_Make_Perf_File(pid_t* c_pids, char* perf_file) {
static int32_t exec_perf_Make_Perf_File(pid_t* c_pids, char* perf_file, pid_t pids[PERF_MAX_PROCS]) {

  int i;

  /* make perf file available to default user */
  if ((c_pids[0] = fork()) == 0) {
    if (setuid(0) == -1) {  // LCOV_EXCL_BR_LINE 5: setuid's error case
      // LCOV_EXCL_START 5: setuid's error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to setuid(0)-3, errno=%d", errno);
      exit(1);
      // LCOV_EXCL_STOP
    }
    for (i = 0; i < PERF_MAX_PROCS; i++) {
      if (pids[i] == -2) {
        // killed by SIGKILL
        continue;
      }
      if (pids[i] <= 0) {
        break;
      }
      sprintf(perf_file, PERF_FILE"%06d", pids[i]);  // NOLINT
      if (chmod(perf_file, 0666) != 0) {  // LCOV_EXCL_BR_LINE 5: chmod's error case
        // LCOV_EXCL_START 5: chmod's error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to chmod %s, errno=%d\n",
               perf_file, errno);
        // LCOV_EXCL_STOP
      }
    }
    exit(0);
  }
  if (c_pids[0] == (pid_t) -1) {  // LCOV_EXCL_BR_LINE 5: fork's error case
    // LCOV_EXCL_START 5: fork's error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to fork-3, errno=%d", errno);
    return -1;
    // LCOV_EXCL_STOP
  }
  if (waitpid(c_pids[0], NULL, 0) == -1) {  // LCOV_EXCL_BR_LINE 5: waitpid's error case
    // LCOV_EXCL_START 5: waitpid's error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to waitpid of CHMOD %d, errno=%d\n",
           c_pids[0], errno);
  // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__, "");
  return 0;
}

/*********************************************************************************
 * exec_perf sub function REPORT perf data into dump file processing
 *********************************************************************************/
static void exec_perf_Report_Perf_Data(pid_t* c_pids, char* perf_file,

//                                       char* perf_dump, int* status) {
                                       char* perf_dump, int* status, pid_t pids[PERF_MAX_PROCS]) {

  int i;
  int fd;
  int waitret;

  FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__, "");
  /* REPORT perf data into dump file */
  for (i = 0; i < PERF_MAX_PROCS; i++) {
    const struct timespec delay = { PERF_REPORT_DELAY, 0 };
    if (pids[i] == -2) {
      // killed by SIGKILL
      continue;
    }
    if (pids[i] <= 0) {
      break;
    }
    sprintf(perf_file, PERF_FILE"%06d", pids[i]);  // NOLINT
    sprintf(perf_dump, PERF_DUMP"%06d", pids[i]);  // NOLINT
    if ((c_pids[i] = fork()) == 0) {
      if (lower_sched_priority(CPULOAD_NICEVAL) == false) {  // LCOV_EXCL_BR_LINE 200: lower_sched_priority() will not return false
        // LCOV_EXCL_START 200: lower_sched_priority() will not return false
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to lower scheduler-2");
        exit(1);
        // LCOV_EXCL_STOP
      }
      if ((fd = open(perf_dump, (O_CREAT | O_WRONLY), 0666)) < 0) {  // LCOV_EXCL_BR_LINE 5: open error case
        // LCOV_EXCL_START 5: open error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to open %s, errno=%d\n",
               perf_dump, errno);
        // LCOV_EXCL_STOP 5: open error case
      } else {
        // Redirect STDOUT
        dup2(fd, 1);
        close(fd);
        // Redirect STDERR
        fd = open("/dev/null", O_WRONLY);
        if (fd > 0) {  // redirect stderr  // LCOV_EXCL_BR_LINE 5: open's error case
          dup2(fd, 2);
          close(fd);
        }
        // Exec perf
        FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__, "execl perf report");
        execl(PERF_PATH, basename(PERF_PATH), "report", "-i", perf_file, NULL);
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to execl %s report, errno=%d",
               PERF_PATH,
               errno);
      }
      exit(1);
    }
    if (c_pids[i] == (pid_t) -1) {  // LCOV_EXCL_BR_LINE 5: fork's error case
      // LCOV_EXCL_START 5: fork's error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to fork-4, errno=%d", errno);
      continue;
      // LCOV_EXCL_STOP
    }
    int ii = 0;
    for (; ii < PERF_REPORT_RETRY; ii++) {
      nanosleep(&delay, NULL);  // Make sure waitpid() to killer returns after perf process exited.
      if ((waitret = waitpid(c_pids[i], status, WNOHANG)) == -1) {  // LCOV_EXCL_BR_LINE 5: waitpid's error case
        // LCOV_EXCL_START 5: waitpid's error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Failed to waitpid for REPORT %d, errno=%d", c_pids[i], errno);
        pids[i] = -2;  // Skip FRAMEWORKUNIFIEDLOG perf data
        break;
        // LCOV_EXCL_STOP
      } else if (waitret == c_pids[i]) {
        break;
      }
    }
    if (ii >= PERF_REPORT_RETRY) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "perf report Failed to exit, pid=%d",
             c_pids[i]);
      pids[i] = -2;  // Skip FRAMEWORKUNIFIEDLOG perf data
    }
    if (pids[i] == -2) {
      // Terminate perf report with SIGKILL
      const struct timespec delay = { PERF_RECORD_SPAN, 0 };
      int sigret = kill(c_pids[i], SIGKILL);
      if (sigret != 0) {  // LCOV_EXCL_BR_LINE 5: kill's error case
        // LCOV_EXCL_START 5: kill's error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Failed in sending SIGKILL to perf report, pid=%d, errno=%d",
               c_pids[i], errno);
        // LCOV_EXCL_STOP
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "SIGKILL has been sent to perf report process, pid=%d",
               c_pids[i]);
      }
      nanosleep(&delay, NULL);  // Make sure waitpid() to killer returns after perf process exited.
      if ((waitret = waitpid(c_pids[i], status, WNOHANG)) == -1) {  // LCOV_EXCL_BR_LINE 5: waitpid's error case
        // LCOV_EXCL_START 5: waitpid's error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Failed to waitpid of REPORT(2) %d, errno=%d", c_pids[i], errno);
        // LCOV_EXCL_STOP
      } else if (waitret == 0) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Failed to terminate perf report by SIGKILL, pid=%d", c_pids[i]);
      }
      // Cleanup
      unlink(perf_dump);
      unlink(perf_file);
    }
  }
}

/*********************************************************************************
 * exec_perf FRAMEWORKUNIFIEDLOG perf data processing
 *********************************************************************************/
static void exec_perf_Resourcemanagerlog_Perf_Data(
    char* perf_file, char* perf_dump,

//    char pnames[PERF_MAX_PROCS][PERF_PNAME_MAX]) {
    pid_t pids[PERF_MAX_PROCS]) {

  int i;
  int perf_lines;
  char buf[READLINE_MAX_SIZE];
  FILE *rfd;

  FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__, "");
  /* FRAMEWORKUNIFIEDLOG perf data */
  for (i = 0; i < PERF_MAX_PROCS; i++) {
    if (pids[i] == -2) {
      // killed by SIGKILL
      continue;
    }
    if (pids[i] <= 0) {
      break;
    }
    sprintf(perf_file, PERF_FILE"%06d", pids[i]);  // NOLINT
    sprintf(perf_dump, PERF_DUMP"%06d", pids[i]);  // NOLINT
    if ((rfd = fopen(perf_dump, "r")) == NULL) {  // LCOV_EXCL_BR_LINE 5: fopen error case
      // LCOV_EXCL_START 5: fopen error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to open-2 %s, errno=%d\n",
             perf_dump, errno);
      continue;
      // LCOV_EXCL_STOP 5: fopen error case
    }
    perf_lines = 0;
    while (fgets(buf, sizeof(buf), rfd) > 0) {
      if (perf_lines >= PERF_MAX_LINES) {
        break;
      }
      /* skip header */
      if (buf[0] == '#') {
        continue;
      }
      if (buf[0] == '\n' || buf[0] == '\r') {
        continue;
      }
      trim_end(buf);
      if (perf_lines == 0) {

//        FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "[CpuHLPerf:%s(%d)]report by result of 'perf record -p %d'",
//               pnames[i], pids[i], pids[i]);
        FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "[CpuHLPerf:(%d)]report by result of 'perf record -p %d'",
               pids[i], pids[i]);

      }
      FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "[CpuHLPerf]%s\n", buf);
      perf_lines++;
    }
    fclose(rfd);
    // Check if no line is acquired
    if (perf_lines == 0) {

//      FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "[CpuHLPerf:%s(%d)] NO_DATA_acquired",
//             pnames[i], pids[i]);
      FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "[CpuHLPerf:(%d)] NO_DATA_acquired",
             pids[i]);

    }
    // Cleanup
    unlink(perf_dump);
    unlink(perf_file);
  }
  FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__, "-");
}

/*********************************************************************************
 * exec_perf Main processing
 *********************************************************************************/

//static void exec_perf(char pnames[PERF_MAX_PROCS][PERF_PNAME_MAX]) {
static void exec_perf(int32_t t_pid) {

  pid_t c_pids[PERF_MAX_PROCS];   // max process to exec perf(forked child pids)

  pid_t pids[PERF_MAX_PROCS];
  char perf_file[128];
  char perf_dump[128];
  char pidstr[64];
  struct stat statbuf;
  int status;

  /* Check existance of perf exec file */
  if (stat(PERF_PATH, &statbuf) == -1 || !(statbuf.st_mode & S_IXUSR)) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "%s is not available", PERF_PATH);
    return;
  }

  FRAMEWORKUNIFIEDLOG (ZONE_INFO, __FUNCTION__, "[CpuHighLoad]Please check User backtrace of following processes in kernel.log");
//  print_backtrace_pid(t_pid);
  for (int i = 0; i < PERF_MAX_PROCS; i++) {
    pids[i] = -1;
  }

  pids[0] = t_pid;


  /* RECORD perf data */

//  exec_perf_Record_Perf_Data(c_pids, perf_file, pidstr, &status, pnames);
  exec_perf_Record_Perf_Data(c_pids, perf_file, pidstr, &status, pids);


  /* make perf file available to default user */

//  if (exec_perf_Make_Perf_File(c_pids, perf_file) != 0)
  if (exec_perf_Make_Perf_File(c_pids, perf_file, pids) != 0)

    return;

  /* REPORT perf data into dump file */

//  exec_perf_Report_Perf_Data(c_pids, perf_file, perf_dump, &status);
  exec_perf_Report_Perf_Data(c_pids, perf_file, perf_dump, &status, pids);


  /* FRAMEWORKUNIFIEDLOG perf data */

//  exec_perf_Resourcemanagerlog_Perf_Data(perf_file, perf_dump, pnames);
  exec_perf_Resourcemanagerlog_Perf_Data(perf_file, perf_dump, pids);

}

// Logging at CPU overload
#define WAIT_RETRY 3           // 3sec


#if 0
static void logging_cpuload(void) {
  int32_t pipe_fd[2];  // 0:stdin,1:stdout
  pid_t c_pid;
  char buf[READLINE_MAX_SIZE];
  char buf2[READLINE_MAX_SIZE];
  char tmp[READLINE_MAX_SIZE];
  int32_t logLine = 0;
  char* ptr;
  int32_t ret;
  int32_t status;
  int32_t perfNum = 0;
  char pnames[PERF_MAX_PROCS][PERF_PNAME_MAX] = { };
  int save_0 = -1;
  int kill_flag = 1;
  int waitret;

  FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__, "+");

  // Create pipe
  ret = pipe(pipe_fd);

  if (ret != 0) {  // LCOV_EXCL_BR_LINE 5: pipe error case
    // LCOV_EXCL_START 5: pipe error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__,
           "ResMgr logging_cpuload() pipe Error");
    return;
    // LCOV_EXCL_STOP 5: pipe error case
  }

  // Create child process
  c_pid = fork();
  if (c_pid < 0) {  // LCOV_EXCL_BR_LINE 5: fork error case
    // LCOV_EXCL_START 5: fork error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__,
           "ResMgr logging_cpuload() fork Error");
    close(pipe_fd[0]);
    close(pipe_fd[1]);

    return;
    // LCOV_EXCL_STOP 5: fork error case
  }

  if (c_pid == 0) {
    /*******************************************************
     * Child-process
     *   The use of dup() and Close() between fork()-> exec() has been
     *   confirmed no probrem.
     *******************************************************/
    if (lower_sched_priority(CPULOAD_NICEVAL) == false) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to lower scheduler");
      exit(1);
    }
    close(pipe_fd[0]);     // Unneeded pipes (close stdin)

    close(1);              // Close stdout
    dup2(pipe_fd[1], 1);   // Duplicate stdout to pipe_fd[1]
    close(pipe_fd[1]);

    FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__, "execl top");
    execl("/usr/bin/top", "top", "-n", "1", "-b", NULL);
    // error
    FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__, "ResMgr logging_cpuload() execl Error");
    exit(1);
  } else {
    close(pipe_fd[1]);      // Unneeded pipes(Close stdout)

    save_0 = dup(0);
    close(0);               // Close stdin
    dup2(pipe_fd[0], 0);    // Duplicate stdin to pipe_fd[0]
    close(pipe_fd[0]);

    for (int i = 0; i < PERF_MAX_PROCS; i++) {
      pids[i] = -1;
    }

    {
      fd_set fds;
      int32_t maxFd;
      int ret;
      struct timeval tmout = { TOP_TIMEOUT, 0 };

      FD_ZERO(&fds);
      FD_SET(STDIN_FILENO, &fds);
      maxFd = STDIN_FILENO;
      ret = select(maxFd + 1, &fds, NULL, NULL, &tmout);
      if (ret < 0) {  // LCOV_EXCL_BR_LINE 5: select error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "[RESM]Handle Error errno:%d", errno);  // LCOV_EXCL_LINE 5: select error case
      } else if (FD_ISSET(STDIN_FILENO, &fds)) {
        kill_flag = 0;
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "[RESM]'top': No response during %d seconds", TOP_TIMEOUT);
      }
      if (kill_flag) {
        // Kill top after TOP_TIMEOUT sec
        // (Killed by child process to avoid making resm process super-user with setuid.)
        if (kill(c_pid, SIGKILL) == -1) {  // LCOV_EXCL_BR_LINE 5: kill error case
          // LCOV_EXCL_START 5: kill error case
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                 "Failed to kill(SIGKILL), pid=%d, errno=%d", (int) c_pid,
                 errno);
          // LCOV_EXCL_STOP 5: kill error case
        }
      } else {
        while (fgets(buf, sizeof(buf), stdin) > 0) {
          // Save ProcessName and Process ID to exec perf
          if (logLine >= 2 && perfNum < PERF_MAX_PROCS) {
            buf2[0] = 0;
            strncat(buf2, buf, sizeof(buf2) - 1);
            buf2[sizeof(buf2) - 1] = 0;
            if (valid_perf_cmd(buf2)) {
              pids[perfNum] = atoi(buf2);
              trim_end(buf2);
              strncat(pnames[perfNum], rindex(buf2, ' ') + 1,
                      sizeof(pnames[0]) - 1);
              if (pids[perfNum] >= 0
                  && strnlen(pnames[perfNum], sizeof(pnames[perfNum]) - 1)) {
                perfNum++;
              } else {
                pids[perfNum] = -1;
              }
            }
          }
          if (logLine == 0) {
            if ((buf[0] != 'C') && (buf[0] != '%')) {
              continue;
            }
            ptr = strstr(buf, "sy");
            if (ptr == NULL) {
              continue;
            }
            while (isalpha(*ptr)) {
              ptr++;
            }
            *ptr = '\0';
            escape_percent(buf, tmp);
            FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "[CpuHighLoad]%s", tmp);
            logLine++;
          } else if (logLine == 1) {
            ptr = strstr(buf, "PID");
            if (ptr == NULL) {
              continue;
            }
            trim_end(buf);
            escape_percent(buf, tmp);
            FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "[CpuHighLoad]%s", tmp);
            logLine++;
          } else if (logLine < (CPU_HIGH_LOAD_P_LOG_NUM + 2)) {
            trim_end(buf);
            FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "[CpuHighLoad]%s", buf);
            logLine++;
          }
        }
      }
    }
    FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__, "wait pid(%d) kill_flag(%d)", c_pid, kill_flag);
    if (kill_flag) {
      const struct timespec delay = {1, 0};
      int i;
      for (i = 0; i < WAIT_RETRY; i++) {
        nanosleep(&delay, NULL);
        if ((waitret = waitpid(c_pid, &status, WNOHANG)) == -1) {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to waitpid for top %d, errno=%d", c_pid, errno);
          break;
        } else if (waitret == c_pid) {
          FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__, "waitpid OK");
          break;
        }
      }
      if (i >= WAIT_RETRY) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "top Failed to exit, pid=%d", c_pid);
      }
    } else {
      if ((waitret = waitpid(static_cast<int>(c_pid), &status, 0)) < 0) {
        FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__, "waitpid(%d) Error errno(%d)", c_pid, errno);
      }
    }
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "waitpid(%d) returned (%d) errno(%d) status=%d",
           c_pid, waitret, errno, WEXITSTATUS(*status));
    if (save_0 >= 0) {
      dup2(save_0, 0);    // Reset the stdin to 0
      close(save_0);
    }
    if (!kill_flag) {
      exec_perf(pnames);
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__, "-");

  return;
}
#endif


// Tail adjustment
static void trim_end(char* buf) {
  int32_t len;

  len = strlen(buf);
  while (len > 0) {
    if (isspace(buf[len - 1])) {
      buf[len - 1] = '\0';
      len--;
      continue;
    }
    break;
  }

  return;
}


#if 0
// Escape character "%"
static void escape_percent(char* in, char* out) {
  char* head;
  char* tail;

  head = in;

  out[0] = '\0';

  while (1) {
    tail = strchr(head, '%');
    if (tail == NULL) {
      strcat(out, head);  // NOLINT
      break;
    }
    *tail = '\0';

    strcat(out, head);  // NOLINT
    strcat(out, "%%");  // NOLINT

    tail++;
    head = tail;
  }

  return;
}
#endif


/*********************************************************************************
 * Output debug information display
 *********************************************************************************/
void outputResouceInfo(void) {
  static bool madedir = false;
  struct stat sbuf;
  FILE *wfp;

  // Create directory
  if (!madedir) {
    if (stat(DEBUG_INFO_DIRPATH, &sbuf) != 0) {  // LCOV_EXCL_BR_LINE 5: stat's error case
      if (mkdir(DEBUG_INFO_DIRPATH, 0777) != 0) {  // LCOV_EXCL_BR_LINE 5: mkdir's error case
        // LCOV_EXCL_START 5: mkdir's error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to mkdir %s, errno=%d",
               DEBUG_INFO_DIRPATH,
               errno);
        return;
        // LCOV_EXCL_STOP
      }
    }
    madedir = true;
  }
  // Open files to work
  if ((wfp = fopen(DEBUG_INFO_TMPPATH, "w")) == NULL) {  // LCOV_EXCL_BR_LINE 5: fopen error case
    // LCOV_EXCL_START 5: fopen error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to open %s, errno=%d",
           DEBUG_INFO_TMPPATH,
           errno);
    return;
    // LCOV_EXCL_STOP 5: fopen error case
  }
  // Output memory information work
  if (write_meminfo_work(wfp) != 0) {  // LCOV_EXCL_BR_LINE 6: write_meminfo_work will not be error
    // LCOV_EXCL_START 6: write_meminfo_work will not be error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "Failed to edit and output in write_meminfo_work()");
    fclose(wfp);
    return;
    // LCOV_EXCL_STOP 6: write_meminfo_work will not be error
  }
  // Get CMA MEMORY information and output working info
  if (write_cmainfo_work(wfp) != 0) {  // LCOV_EXCL_BR_LINE 6: write_cmainfo_work will not be error
    // LCOV_EXCL_START 6: write_cmainfo_work will not be error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "Failed to edit and output in write_cmainfo_work()");
    fclose(wfp);
    return;
    // LCOV_EXCL_STOP 6: write_cmainfo_work will not be error
  }
  // Get top information and output work info
  if (write_cpuinfo_work(wfp) != 0) {  // LCOV_EXCL_BR_LINE 6: write_cpuinfo_work will not be error
    // LCOV_EXCL_START 6: write_cpuinfo_work will not be error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "Failed to edit and output in write_cpuinfo_work()");
    fclose(wfp);
    return;
    // LCOV_EXCL_STOP 6: write_cpuinfo_work will not be error
  }
  fclose(wfp);
  // Create output file
  if (rename(DEBUG_INFO_TMPPATH, DEBUG_INFO_FPATH) != 0) {  // LCOV_EXCL_BR_LINE 5:rename error case
    // LCOV_EXCL_START 5:rename error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to make output file %s, errno=%d",
           DEBUG_INFO_FPATH,
           errno);
    // LCOV_EXCL_STOP 5:rename error case
  }
  // Write information and output FRAMEWORKUNIFIEDLOG
  if (write_resourcemanagerloginfo_work() != 0) {  // LCOV_EXCL_BR_LINE 6: write_resourcemanagerloginfo_work will not be error
    // LCOV_EXCL_START 6: write_resourcemanagerloginfo_work will not be error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "Failed to output in write_resourcemanagerloginfo_work()");
    // LCOV_EXCL_STOP 6: write_resourcemanagerloginfo_work will not be error
  }
}

// Output memory information work
static int write_meminfo_work(FILE *wfp) {
  float total;
  float avail;
  float used;
  float min_remain;
  uint32_t used_rate;
  uint32_t used_letters;
  // Output meminfo: Getting info from the proc/meminfo is performed at 5-second intervals in watchMem, so it is diverted.
  avail = static_cast<float>((mainFree_kib + inactFile_kib));
  total = static_cast<float>(memTotal_kib);
  used = total - avail;
  min_remain = static_cast<float>(minRestMem);
  // "*MEMORY   @@@@@@ Warning!! @@@@@"
  fprintf(wfp, "*MEMORY");
  if (avail * 10 < total) {  // (Less than 1/10)
    fprintf(wfp, "   @@@@@@ Warning!! @@@@@\n");
  } else {
    fprintf(wfp, "   \n");
  }
  // "used/avail/total/used max   xxx.xMB / xxx.xMB / xxx.xMB(xx.x%) / xxx.xMB
  used /= 1024;
  avail /= 1024;
  total /= 1024;
  min_remain /= 1024;
  if (total == 0) {
    used_rate = 0;
  } else {
    used_rate = (uint32_t) (used * 1000 / total);
    if (used_rate >= 1000) {
      used_rate = 999;
    }
  }
  fprintf(
    wfp,
    " used/avail/total/min remain   %5.1fMB / %5.1fMB / %5.1fMB(%2d.%d%%) / %5.1fMB\n",
    used, avail, total, used_rate / 10, used_rate % 10, min_remain);
  if (total == 0) {
    used_letters = 0;
  } else {
    used_letters = (uint32_t) (DEBUG_INFO_MEM_LETTERS * used / total);
    if (used_letters > DEBUG_INFO_MEM_LETTERS) {
      used_letters = DEBUG_INFO_MEM_LETTERS;
    }
  }
  // "------------------*******"
  int i;
  for (i = 0; i < static_cast<int>(used_letters); i++) {
    fprintf(wfp, "-");
  }
  for (; i < DEBUG_INFO_MEM_LETTERS; i++) {
    fprintf(wfp, "*");
  }
  fprintf(wfp, "\n\n");

  return 0;
}
// Get top information and Output work
static int write_cpuinfo_work(FILE *wfp) {
  int32_t pipe_fd[2];  // 0:stdin,1:stdout
  pid_t c_pid;
  char buf[READLINE_MAX_SIZE];
  int32_t logLine = 0;
  char* ptr;
  int32_t ret;
  int32_t status;
  int save_0 = -1;
  int32_t cpu_rate;
  char fields[12][128];
  int kill_flag = 1;
  int waitret;

  // Output CPU load
  cpu_rate = g_cpuloadRate1000 < 0 ? 0 : g_cpuloadRate1000;
  fprintf(wfp, "*CPU      %2d.%d%%\n", cpu_rate / 10, cpu_rate % 10);

  // Create pipe
  ret = pipe(pipe_fd);

  if (ret != 0) {  // LCOV_EXCL_BR_LINE 5: pipe error case
    // LCOV_EXCL_START 5: pipe error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "pipe Error");
    return -1;
    // LCOV_EXCL_STOP 5: pipe error case
  }

  // Create child process
  c_pid = fork();
  if (c_pid < 0) {  // LCOV_EXCL_BR_LINE 5: fork error case
    // LCOV_EXCL_START 5: fork error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "fork Error");
    close(pipe_fd[0]);
    close(pipe_fd[1]);

    return -1;
    // LCOV_EXCL_STOP 5: fork error case
  }

  if (c_pid == 0) {
    /*******************************************************
     * Child process
     *******************************************************/
    if (lower_sched_priority(CPULOAD_NICEVAL) == false) {  // LCOV_EXCL_BR_LINE 200: lower_sched_priority can't be false
      // LCOV_EXCL_START 200: lower_sched_priority can't be false
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to lower scheduler");
      exit(1);
      // LCOV_EXCL_STOP
    }
    close(pipe_fd[0]);     // Unneeded pipes (close stdin)

    close(1);              // Close stdout
    dup2(pipe_fd[1], 1);   // Duplicate stdout to pipe_fd[1]
    close(pipe_fd[1]);

    execl("/usr/bin/top", "top", "-n", "1", "-b", NULL);
    // error
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "execl top Error");
    exit(1);
  } else {
    close(pipe_fd[1]);      // Unneeded pipes(Close stdout)

    save_0 = dup(0);
    close(0);               // Close stdin
    dup2(pipe_fd[0], 0);    // Duplicate stdin to pipe_fd[0]
    close(pipe_fd[0]);

    {
      fd_set fds;
      int32_t maxFd;
      int ret;
      struct timeval tmout = { TOP_TIMEOUT, 0 };

      FD_ZERO(&fds);
      FD_SET(STDIN_FILENO, &fds);
      maxFd = STDIN_FILENO;
      ret = select(maxFd + 1, &fds, NULL, NULL, &tmout);
      if (ret < 0) {  // LCOV_EXCL_BR_LINE 5: select error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "[RESM]Handle Error errno:%d", errno);  // LCOV_EXCL_LINE 5: select error case
      } else if (FD_ISSET(STDIN_FILENO, &fds)) {  // LCOV_EXCL_BR_LINE 5: FD_ISSET's error case
        kill_flag = 0;
      } else {
        // LCOV_EXCL_START 5: FD_ISSET's error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "[RESM]'top': No response during %d seconds", TOP_TIMEOUT);
        // LCOV_EXCL_STOP
      }
      if (kill_flag) {  // LCOV_EXCL_BR_LINE 200: kill_flag must be 0
        // LCOV_EXCL_START 200: kill_flag must be 0
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        // Kill top after TOP_TIMEOUT sec
        // (Killed by child process to avoid making resm process super-user with setuid.)
        if (kill(c_pid, SIGKILL) == -1) {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                 "Failed to kill(SIGKILL), pid=%d, errno=%d", (int) c_pid,
                 errno);
        }
        // LCOV_EXCL_STOP
      } else {
        while (fgets(buf, sizeof(buf), stdin) > 0) {
          if (logLine == 0) {
            if (buf[0] != 'C') {
              continue;
            }
            if (strstr(buf, "Cpu(s)") == NULL || strstr(buf, "sy") == NULL) {
              continue;
            }
            logLine++;
          } else if (logLine == 1) {
            ptr = strstr(buf, "PID");
            if (ptr == NULL) {
              continue;
            }
            logLine++;
          } else if (logLine < (DEBUG_INFO_CPU_TOP_LINES + 2)) {
            ret = sscanf(buf, "%128s %128s %128s %128s %128s %128s %128s %128s %128s %128s %128s %128s", fields[0],
                         fields[1], fields[2], fields[3], fields[4], fields[5],
                         fields[6], fields[7], fields[8], fields[9], fields[10],
                         fields[11]);
            fprintf(wfp, "%4s%%  %s\n", fields[8], fields[11]);
            logLine++;
          }
        }
        fprintf(wfp, "\n\n");
      }
    }
    ret = 0;
    if (kill_flag) {  // LCOV_EXCL_BR_LINE 200: kill_flag must be 0
      // LCOV_EXCL_START 200: kill_flag must be 0
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      const struct timespec delay = {1, 0};
      int i;
      for (i = 0; i < WAIT_RETRY; i++) {
        nanosleep(&delay, NULL);
        if ((waitret = waitpid(c_pid, &status, WNOHANG)) == -1) {
          ret = -1;
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to waitpid for top %d, errno=%d", c_pid, errno);
          break;
        } else if (waitret == c_pid) {
          FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__, "waitpid OK");
          break;
        }
      }
      if (i >= WAIT_RETRY) {
        ret = -1;
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "top Failed to exit, pid=%d", c_pid);
      }
      // LCOV_EXCL_STOP
    } else {
      if ((waitret = waitpid(static_cast<int>(c_pid), &status, 0)) < 0) {  // LCOV_EXCL_BR_LINE 5: waitpid's error case
        // LCOV_EXCL_START 5: waitpid's error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        ret = -1;
        FRAMEWORKUNIFIEDLOG(ZONE_RESM_DEBUG, __FUNCTION__, "waitpid(%d) Error errno(%d)", c_pid, errno);
        // LCOV_EXCL_STOP
      }
    }
    if (ret < 0) {  // LCOV_EXCL_BR_LINE 200: ret must be 0
      // LCOV_EXCL_START 200: ret must be 0
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "wait Error");
      if (save_0 >= 0) {
        close(save_0);
      }
      return -1;
      // LCOV_EXCL_STOP
    }
    if (save_0 >= 0) {
      dup2(save_0, 0);    // Reset the stdin to 0
      close(save_0);
    }
  }

  return 0;
}
// Get CMA information and Output work
static int write_cmainfo_work(FILE *wfp) {
  float total;
  float avail;
  float used;
  float min_remain;
  uint32_t used_rate;
  uint32_t used_letters;

  avail = static_cast<float>(cmaFree_kib);
  total = static_cast<float>(cmaTotal_kib);
  used = total - avail;
  min_remain = static_cast<float>(minRestCma);
  // "*CMA MEMORY   @@@@@@ Warning!! @@@@@"
  fprintf(wfp, "*CMA MEMORY");
  if (used * 5 > total * 4) {  // (4/5 Or more)
    fprintf(wfp, "   @@@@@@ Warning!! @@@@@\n");
  } else {
    fprintf(wfp, "   \n");
  }
  // "used/avail/total   xxx.xMB / xxx.xMB / xxx.xMB(xx.x%)
  used /= 1024;
  avail /= 1024;
  total /= 1024;
  min_remain /= 1024;
  if (total != 0) {
    used_rate = (uint32_t) (used * 1000 / total);
  } else {
    used_rate = 0;
  }
  if (used_rate >= 1000) {
    used_rate = 999;
  }
  fprintf(
    wfp,
    " used/avail/total/min remain   %5.1fMB / %5.1fMB / %5.1fMB(%2d.%d%%) / %5.1fMB\n",
    used, avail, total, used_rate / 10, used_rate % 10, min_remain);
  if (total == 0) {
    used_letters = 0;
  } else {
    used_letters = (uint32_t) (DEBUG_INFO_CMA_LETTERS * used / total);
    if (used_letters > DEBUG_INFO_CMA_LETTERS) {
      used_letters = DEBUG_INFO_CMA_LETTERS;
    }
  }
  // "------------------*******"
  int i;
  for (i = 0; i < static_cast<int>(used_letters); i++) {
    fprintf(wfp, "-");
  }
  for (; i < DEBUG_INFO_CMA_LETTERS; i++) {
    fprintf(wfp, "*");
  }
  fprintf(wfp, "\n\n");

  return 0;
}
// Write information and Output FRAMEWORKUNIFIEDLOG
static int write_resourcemanagerloginfo_work(void) {
  FILE *wfp;
  char l_read[READLINE_MAX_SIZE];
  int ret;

  wfp = fopen(DEBUG_INFO_FPATH, "r");
  if (wfp == NULL) {  // LCOV_EXCL_BR_LINE 5: fopen error case
    // LCOV_EXCL_START 5: fopen case error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to open %s, errno=%d",
           DEBUG_INFO_FPATH,
           errno);
    return -1;
    // LCOV_EXCL_STOP 5: fopen case error
  }
  while (1) {
    if (fgets(l_read, READLINE_MAX_SIZE, wfp) == NULL) {
      ret = feof(wfp);
      if (ret == 0) {  // LCOV_EXCL_BR_LINE 5: feof case error
        // LCOV_EXCL_START 5: feof case error
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_PERFORMANCE, __FUNCTION__, "Failed to fgets %s",
               DEBUG_INFO_FPATH);
        // LCOV_EXCL_STOP 5: feof case error
      }
      break;
    } else {
      char *line;
      line = strchr(l_read, '\n');
      if (line != NULL) {
        *line = '\0';
      }
      if (l_read[0] != '\0') {
        FRAMEWORKUNIFIEDLOG(ZONE_PERFORMANCE, __FUNCTION__, "%s", l_read);
      }
    }
  }
  fclose(wfp);
  return 0;
}
