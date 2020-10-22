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

#include "tskm_port_subsys.h"
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <native_service/cl_monitor.h>
#include <system_service/ss_sm_client_if.h>
#include <system_service/resm.h>
//#include <stub/ss_diag.h>
#include <string>

#include "agl_thread.h"

#include "tskm_main.h"
#include "tskm_debug.h"
#include "tskm_util.h"
#include "tskm_port_pf.h"


#ifndef oops_name2pid
  #define oops_name2pid(processname) syscall(OOPSTRACE_SYSCALL, 17, processname)
#endif


typedef struct {
  CL_MonitorSearch_t clMonSearch;
  uint32_t resmId;        // Session ID of RESM
} SUB_SYS_CTX_t;

static SUB_SYS_CTX_t g_sub;

/******************************************************************
 *        Subsystem initialization
 ******************************************************************/
int tskm_sub_init(void) {
  int clRet;
  SUB_SYS_CTX_t* p_ctx = &g_sub;

  clRet = CL_MonitorSearchInit(&p_ctx->clMonSearch);
  if (clRet != 0) {  // LCOV_EXCL_BR_LINE 6: For processing intializing process
    // LCOV_EXCL_START 6:For processing initializing process
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT_ERRNO(0);
    goto ERROR;
    // LCOV_EXCL_STOP 6: For processing initializing process
  }

  return 0;
  // LCOV_EXCL_START 6:For processing initializing process
  ERROR:
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  return 1;
  // LCOV_EXCL_STOP
}

/******************************************************************
 *        Subsystem termination
 ******************************************************************/
int tskm_sub_term(void) {
  int ret;
  SUB_SYS_CTX_t* p_ctx = &g_sub;

  ret = CL_MonitorSearchDestroy(&p_ctx->clMonSearch);
  if (ret != 0) {
    TSKM_ASSERT_ERRNO(0);
  }

  return 0;
}
/*********************************************************
 *        REBOOT
 *********************************************************/
void tskm_sub_reboot(const TSKM_ERROR_REBOOT_t* p_info) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  TSKM_ASSERT(0);

  l_eStatus = SendCpuResetRequestToSystemManager(
      e_SS_SM_CPU_RESET_REASON_GENERIC_ERR, p_info->log.messageStr);
  if (eFrameworkunifiedStatusOK != l_eStatus) {
    TSKM_ASSERT(0);
  }
}

/*********************************************************
 *        REBOOT NORMAL
 *********************************************************/
void tskm_sub_reboot_normal(void) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  l_eStatus = SendCpuResetRequestToSystemManager(
      e_SS_SM_CPU_RESET_REASON_NORMAL);
  if (l_eStatus != eFrameworkunifiedStatusOK) {
    TSKM_ASSERT(0);
  }
}

/*********************************************************
 *        LOGGING
 *********************************************************/
void tskm_sub_logging(const TSKM_LOGGING_INFO_t* p_info) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  eSMUserLogType l_eType;

  switch (p_info->type) {
    case TSKM_LOGGING_TYPE_GRP_RELAUNCH:
      l_eType = e_SS_SM_CAPTURE_GROUP_RELAUNCH;
      break;
    case TSKM_LOGGING_TYPE_MODULE_LOGS:
    default:
      l_eType = e_SS_SM_CAPTURE_MODULE_LOGS;
      break;
  }

  l_eStatus = SendUserInvokedLoggingRequestToSystemManager(l_eType,
                                                           p_info->messageStr);
  if (eFrameworkunifiedStatusOK != l_eStatus) {
    TSKM_ASSERT(0);
  }
}

/*********************************************************
 *        DEBUG DUMP
 *********************************************************/
void tskm_sub_debugDumpRes(BOOL bIsNeedSvcName, const char *p_dumpMsg) {
  if (bIsNeedSvcName) {  // LCOV_EXCL_BR_LINE 8: As the bIsNeedSvcName is set to FALSE only
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    SSDEBUGDUMP("%s", p_dumpMsg);  // LCOV_EXCL_LINE 8: As the bIsNeedSvcName is set to FALSE only
  } else {
    SSDEBUGDUMP_RAW("%s", p_dumpMsg);
  }
}

/*********************************************************
 *        EXT BOOT INFO
 *********************************************************/
int tskm_sub_getExtBootInfo(T_SS_SM_START_ExtDataStructType *p_info) {
  EFrameworkunifiedStatus l_eStatus;
  T_SS_SM_START_ExtDataStructType extStartupInfo;

  l_eStatus = GetInterfaceunifiedOnStartExtInfo(extStartupInfo);
  if (l_eStatus != eFrameworkunifiedStatusOK) {
    TSKM_ASSERT(0);
    goto ERROR;
  }

  memcpy(p_info, &extStartupInfo, sizeof(extStartupInfo));

  return 0;
  ERROR: return -1;
}

/*********************************************************
 *        Service Error Monitoring Check
 *********************************************************/
TSKM_HUNG_INFO_t*
tskm_sub_searchHungSvcs() {
  int ret;
  TSKM_HUNG_INFO_t *p_hungList = NULL;
  SUB_SYS_CTX_t* p_ctx = &g_sub;

  ret = CL_MonitorSearchTimeout(&p_ctx->clMonSearch);
  if (ret == -1) {
    TSKM_ASSERT_ERRNO(0);
    goto ERROR;
  }

  if (0 < p_ctx->clMonSearch.entry_num) {
    int ii;

    TSKM_PRINTF(TSKM_LOG_DEBUG, "TimeoutSvcNum = %d",
                p_ctx->clMonSearch.entry_num);

    if (NULL == p_ctx->clMonSearch.entry_list) {
      TSKM_ASSERT(0);
      goto ERROR;
    }

    p_hungList = (TSKM_HUNG_INFO_t *) malloc(  // NOLINT (readability/casting)
        sizeof(TSKM_HUNG_INFO_t) * (p_ctx->clMonSearch.entry_num + 1));
    if (p_hungList == NULL) {  // LCOV_EXCL_BR_LINE 5: malloc's error case.
      // LCOV_EXCL_START 5: Checked in Death testing, but it is not reflected in the coverage and excluded
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      TSKM_ASSERT_ERRNO(0);
      goto ERROR;
      // LCOV_EXCL_STOP
    }

    for (ii = 0; ii < p_ctx->clMonSearch.entry_num; ii++) {
      p_hungList[ii].pid = p_ctx->clMonSearch.entry_list[ii].pid;
      p_hungList[ii].type = p_ctx->clMonSearch.entry_list[ii].type;
    }
    // set "-1" to pid at the end of list.
    p_hungList[p_ctx->clMonSearch.entry_num].pid = -1;
  }

  ERROR: return p_hungList;
}

/*********************************************************
 *        RESM Initialization
 *********************************************************/
int tskm_sub_resmInit(void) {  // LCOV_EXCL_START 6: Because the condition cannot be set
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  int redmFd;
  SUB_SYS_CTX_t* p_ctx = &g_sub;
  RESM_ERR_t resmRet = RESM_E_OK;
  RESM_REQ_EVENT_t ev;

  int pid;

  resmRet = RESM_Open(NULL, &(p_ctx->resmId));
  if (RESM_E_OK != resmRet) {
    TSKM_ASSERT(0);

#warning RESM backtrace disabled, because 'print_backtrace_pid' not support.
#if 0
    // Backtrace outoutput of the RESM
    pid = oops_name2pid(MN_SS_RESOURCEMGR);
    if(pid > 0){
      TSKM_ASSERT(-1 != print_backtrace_pid(pid));
    }else{
      TSKM_PRINTF(TSKM_LOG_ERROR, "ERR : oops_name2pid %d , err:%d, msg:%s",
                  pid, errno, strerror(errno));
    }
#endif

    goto ERROR;
  }

  resmRet = RESM_GetEventFd(p_ctx->resmId, &redmFd);
  if (RESM_E_OK != resmRet) {
    TSKM_ASSERT(0);
    goto ERROR;
  }

  ev.reqEvent = RESM_EV_MEM;
  ev.prm.restMemThresh = 512;  // Threshold(BYTE)

  resmRet = RESM_ReqEvent(p_ctx->resmId, &ev);
  if (RESM_E_OK != resmRet) {
    TSKM_ASSERT(0);
    goto ERROR;
  }

  return redmFd;
  ERROR: return -1;
}
// LCOV_EXCL_STOP
/*********************************************************
 *        Get EV of RESM
 *********************************************************/
int tskm_sub_resmRcv(TSKM_EVENT_INFO_t* p_ev) {
  SUB_SYS_CTX_t* p_ctx = &g_sub;
  RESM_ERR_t resmRet = RESM_E_OK;
  RESM_EV_t evFlag;

  resmRet = RESM_GetEvent(p_ctx->resmId, &evFlag);
  if (RESM_E_OK != resmRet) {
    TSKM_ASSERT(0);
    goto ERROR;
  }

  if (RESM_EV_MEM != evFlag) {
    TSKM_ASSERT(0);
    goto ERROR;
  }

  TSKM_PRINTF(TSKM_LOG_ERROR, "Detect LowMem");

  p_ev->event = TSKM_EV_LCL_REP_LOWMEM;

  return 0;
  ERROR: return -1;
}

int tskm_sub_setWakeupOrder(const char* p_order) {
  // Under consideration
  return 0;
}  // LCOV_EXCL_BR_LINE 10: Gcov constraints (last row)
