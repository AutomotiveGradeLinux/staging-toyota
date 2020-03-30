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

#include "pri_main.h"
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/eventfd.h>
#include <stdlib.h>
#include <native_service/frameworkunified_dispatcher.h>
#include <system_service/ss_system_if.h>

#include "tskm_debug.h"
#include "tskm_comm.h"
#include "tskm_port_pf.h"
#include "tskm_port_subsys.h"
#include "tskm_util.h"



#define PRI_PROC_NAME_MAX 32

// Context
typedef struct {
  T_PRIM_PRM prm;

  TSKM_SVCID_t svcId;      // Set valid value by REQ_WAKEUP
  char procName[PRI_PROC_NAME_MAX];
  // State
  TSKM_BOOL_t isExec;
  T_SS_SM_START_DataStructType bootInfo;
  T_SS_SM_START_ExtDataStructType extBootInfo; TSKM_BOOL_t isDynamic;
  uint32_t wakeupStepDone;  // Executed local step
  TSKM_BOOL_t shmDone;
  uint32_t downStepDone;    // Executed local step
  TSKM_BOOL_t isExitStart;

#define PRI_MONITOR_DEFAULT_TIMEOUT 50
  uint32_t timeout;    // Service monitoring timeout period (valid only for the INI_Main type service)

  // Resources
  int connFd;       // TSKM communication socket
  int nsFd;         // NSFW socket
  int pipeFd[2];    // for exitDone
  HANDLE hApp;      // appHandle
} PRI_CTX_t;

static PRI_CTX_t g_pri;

/*********************************************
 *    Create shared memory
 *********************************************/
TSKM_STATIC void shmMake(PRI_CTX_t* p_ctx) {
  const PRIM_SHAREDATA_TBL* shmEntry;
  for (shmEntry = p_ctx->prm.shmTbl; shmEntry->shmName[0] != '\0'; shmEntry++) {
    int ret;
    ret = tskm_pf_shmCreate(shmEntry->shmName, shmEntry->size, NULL);
    if (ret != 0) {  // LCOV_EXCL_BR_LINE 5: Checked in Death testing, but it is not reflected in the coverage and excluded
      // LCOV_EXCL_START 5: Checked in Death testing, but it is not reflected in the coverage and excluded
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      TSKM_ASSERT(0);
      goto ERROR;
      // LCOV_EXCL_STOP 5: Checked in Death testing, but it is not reflected in the coverage and excluded
    }
  }
  p_ctx->shmDone = TSKM_TRUE;
  return;

  // LCOV_EXCL_START 5: Checked in Death testing, but it is not reflected in the coverage and excluded
  ERROR:
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  tskm_pf_abort();
  // LCOV_EXCL_STOP
}

/*********************************************
 *    Call backup check CB
 *********************************************/
TSKM_STATIC uint32_t wakeupExFuncCallback(PRI_CTX_t* p_ctx,
                                          TSKM_EV_PRI_REQ_WAKEUP_PRM_t* p_prm) {
  const PRIM_EXFUNC_TBL* funcEntry;
  uint32_t maxStep = 0;

  for (funcEntry = p_ctx->prm.wakeupExFuncTbl; funcEntry->localStep != 0;
      funcEntry++) {
    if (funcEntry->localStep == p_prm->localStep) {
      funcEntry->func(funcEntry->prm);
    }
    maxStep = TSKM_MAX(maxStep, funcEntry->localStep);
  }
  return maxStep;
}

/*********************************************
 *    Gradual startup request
 *********************************************/
TSKM_STATIC void wakeupRequest(PRI_CTX_t* p_ctx,
                               TSKM_EV_PRI_REQ_WAKEUP_PRM_t* p_prm) {
  TSKM_EV_PRI_REQ_WAKEUP_PRM_t prm = *p_prm;
  uint32_t max = 0;

  // Execute step by step
  for (prm.localStep = p_ctx->wakeupStepDone + 1;
      (prm.localStep <= p_prm->localStep && prm.localStep < PRIM_STEPFORK_MAX);
      prm.localStep++) {
    max = wakeupExFuncCallback(p_ctx, &prm);
  }

  if (max <= p_prm->localStep) {
    // Completed gradual startup
    p_ctx->wakeupStepDone = PRIM_STEPFORK_MAX;
  } else {
    p_ctx->wakeupStepDone = p_prm->localStep;
  }
}

/*********************************************
 *    All startup requests
 *********************************************/
TSKM_STATIC void allWakeup(PRI_CTX_t* p_ctx,
                           TSKM_EV_PRI_REQ_WAKEUP_PRM_t* p_prm) {
  if (!p_ctx->shmDone) {
    shmMake(p_ctx);
  }

  if (p_ctx->wakeupStepDone < PRIM_STEPFORK_MAX) {
    wakeupRequest(p_ctx, p_prm);
  }
}

/*********************************************
 *   Startup request handles
 *********************************************/
TSKM_STATIC void wakeupRequestHandle(PRI_CTX_t* p_ctx,
                                     TSKM_EV_PRI_REQ_WAKEUP_PRM_t* p_prm) {
  TSKM_EVENT_INFO_t ev;
  int ret;

  bzero(&ev, sizeof(ev));

  p_ctx->svcId = p_prm->svcId;
  memcpy(&p_ctx->bootInfo, &p_prm->bootInfo, sizeof(p_ctx->bootInfo));
  memcpy(&p_ctx->extBootInfo, &p_prm->extBootInfo, sizeof(p_ctx->extBootInfo));
  p_ctx->isDynamic = p_prm->isDynamic;

  if (p_prm->localStep == TSKM_LSTEP_ALL) {
    allWakeup(p_ctx, p_prm);
  } else if (p_prm->localStep == TSKM_LSTEP_LAST) {
    wakeupRequest(p_ctx, p_prm);
  } else if (p_prm->localStep == TSKM_LSTEP_SHM) {
    shmMake(p_ctx);
  } else {
    wakeupRequest(p_ctx, p_prm);
  }

  ev.prm.resWakeup.isShmDone = p_ctx->shmDone;
  ev.prm.resWakeup.isStepDone =
      (p_ctx->wakeupStepDone >= PRIM_STEPFORK_MAX) ?
      TSKM_TRUE :
                                                     TSKM_FALSE;

  // LCOV_EXCL_BR_START 6: Because it depends on the test order
  if (ev.prm.resWakeup.isShmDone && ev.prm.resWakeup.isStepDone) {
    // LCOV_EXCL_BR_STOP
    ev.prm.resWakeup.isLast = TSKM_TRUE;
  }

  ev.event = TSKM_EV_PRI_RES_WAKEUP;
  ev.errCode = TSKM_E_OK;
  ret = tskm_sockSend(p_ctx->connFd, &ev);
  if (ret <= 0) {  // LCOV_EXCL_BR_LINE 5: Checked in Death testing, but it is not reflected in the coverage and excluded
    // LCOV_EXCL_START 5: Checked in Death testing, but it is not reflected in the coverage and excluded
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT(0);
    tskm_pf_abort();
    // LCOV_EXCL_STOP 5: Checked in Death testing, but it is not reflected in the coverage and excluded
  }
}

/*********************************************
 *    Gradual termination callback
 *********************************************/
TSKM_STATIC uint32_t downExFuncCallback(PRI_CTX_t* p_ctx,
                                        TSKM_EV_PRI_REQ_DOWN_PRM_t* p_prm) {
  const PRIM_EXFUNC_TBL* funcEntry;
  uint32_t maxStep = 0;

  for (funcEntry = p_ctx->prm.downExFuncTbl; funcEntry->localStep != 0;
      funcEntry++) {
    if (funcEntry->localStep == p_prm->localStep) {
      funcEntry->func(funcEntry->prm);
    }
    maxStep = TSKM_MAX(maxStep, funcEntry->localStep);
  }
  return maxStep;
}

/*********************************************
 *    Gradual termination requests
 *********************************************/
TSKM_STATIC void downRequest(PRI_CTX_t* p_ctx,
                             TSKM_EV_PRI_REQ_DOWN_PRM_t* p_prm) {
  TSKM_EV_PRI_REQ_DOWN_PRM_t prm = *p_prm;
  uint32_t max = 0;

  // Execute step by step
  for (prm.localStep = p_ctx->downStepDone + 1;
      (prm.localStep <= p_prm->localStep && prm.localStep < PRIM_ACCOFF_MAX);
      prm.localStep++) {
    max = downExFuncCallback(p_ctx, &prm);
  }

  if (max <= p_prm->localStep) {
    p_ctx->downStepDone = PRIM_ACCOFF_MAX;  // Completed all steps
  } else {
    p_ctx->downStepDone = p_prm->localStep;
  }
}

TSKM_STATIC void downRequestHandle(PRI_CTX_t* p_ctx,
                                   TSKM_EV_PRI_REQ_DOWN_PRM_t* p_prm) {
  TSKM_EVENT_INFO_t ev;
  int ret;

  bzero(&ev, sizeof(ev));

  if (p_prm->localStep == TSKM_LSTEP_ALL || p_prm->localStep == TSKM_LSTEP_LAST) {
    downRequest(p_ctx, p_prm);
  } else if (p_prm->localStep == TSKM_LSTEP_SHM) {
    TSKM_ASSERT(0);
  } else if (p_prm->localStep == TSKM_LSTEP_BUPCHK) {
    TSKM_ASSERT(0);
  } else {
    downRequest(p_ctx, p_prm);
  }

  if (p_ctx->downStepDone >= PRIM_ACCOFF_MAX) {
    /* It is not notified when the last function is executed, and it is left to the exitDone.
     TSKM_PRINTF(TSKM_LOG_DEBUG,"ACCOFF DONE");
     ev.prm.resDown.isLast = TSKM_TRUE;
     p_ctx->isExec = TSKM_FALSE;
     ret =  tskm_sockSend(p_ctx->connFd,&ev);
     if(ret <= 0){
     TSKM_ASSERT(0);
     tskm_pf_abort();
     }
     */
  } else {
    ev.event = TSKM_EV_PRI_RES_DOWN;
    ev.errCode = TSKM_E_OK;
    ret = tskm_sockSend(p_ctx->connFd, &ev);
    if (ret <= 0) {  // LCOV_EXCL_BR_LINE 5: Checked in Death testing, but it is not reflected in the coverage and excluded
      // LCOV_EXCL_START 5: Checked in Death testing, but it is not reflected in the coverage and excluded
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      TSKM_ASSERT(0);
      tskm_pf_abort();
      // LCOV_EXCL_STOP 5: Checked in Death testing, but it is not reflected in the coverage and excluded
    }
  }
}

/*********************************************
 *  Termination completion notification to the TSKM 
 *********************************************/
TSKM_STATIC void sendLastDoneRes(PRI_CTX_t* p_ctx) {
  int ret;
  TSKM_EVENT_INFO_t ev;

  bzero(&ev, sizeof(ev));

  ev.event = TSKM_EV_PRI_RES_DOWN;
  ev.errCode = TSKM_E_OK;
  ev.prm.resDown.isLast = TSKM_TRUE;
  ret = tskm_sockSend(p_ctx->connFd, &ev);
  if (ret <= 0) {  // LCOV_EXCL_BR_LINE 5: Checked in Death testing, but it is not reflected in the coverage and excluded
    // LCOV_EXCL_START 5: Checked in Death testing, but it is not reflected in the coverage and excluded
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT(0);
    tskm_pf_abort();
    // LCOV_EXCL_STOP 5: Checked in Death testing, but it is not reflected in the coverage and excluded
  }
}

/*********************************************
 *  Call Touch CB
 *********************************************/
TSKM_STATIC void touchService(PRI_CTX_t* p_ctx) {
  char touchName[32];

  if (p_ctx->isExitStart) {
    // If termination has already begun, the system just processes as timeout for retrying not respond Touch
    return;
  }

  p_ctx->prm.onTouch(p_ctx->hApp);

  tskm_pf_mkTouchFileName(getpid(), touchName);

  if ((access(touchName, F_OK) == 0)) {
    // Synchronize by deleting files.
    TSKM_PRINTF(TSKM_LOG_STATE, "del:%s", touchName);
    unlink(touchName);
  } else {
    TSKM_ASSERT_PRINT(0, "%s", touchName);
  }
}

/*********************************************
 *  Call Debugdump CB
 *********************************************/
TSKM_STATIC void callDebugDump(PRI_CTX_t* p_ctx) {
  if (!p_ctx->prm.onDebugDump) {  // LCOV_EXCL_BR_LINE 6: As NULL checked by INI_Init
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT(0);  // LCOV_EXCL_LINE 6: As NULL checked by INI_Init
  } else {
    p_ctx->prm.onDebugDump(p_ctx->hApp);
  }
}

/*********************************************
 *  Call LowMemory detection
 *********************************************/
TSKM_STATIC void callLowMem(PRI_CTX_t* p_ctx) {
  if (!p_ctx->prm.onLowMem) {
    TSKM_ASSERT(0);
  } else if (!p_ctx->isExitStart) {
    // Notify LowMemory only before starting terminating process
    p_ctx->prm.onLowMem(p_ctx->hApp);
  }
}

/*********************************************
 *   Event handles
 *********************************************/
TSKM_STATIC void eventHandle(PRI_CTX_t* p_ctx, TSKM_EVENT_INFO_t* p_ev) {
  //Processing according to the request
  switch (p_ev->event) {
    case TSKM_EV_PRI_REQ_WAKEUP:
      wakeupRequestHandle(p_ctx, &p_ev->prm.reqWakeup);
      break;
    case TSKM_EV_PRI_REQ_DOWN:
      downRequestHandle(p_ctx, &p_ev->prm.reqDown);
      break;
    case TSKM_EV_PRI_REQ_TOUCH:
      touchService(p_ctx);
      break;
    case TSKM_EV_PRI_REQ_DEBUGDUMP:
      callDebugDump(p_ctx);
      break;
    case TSKM_EV_PRI_REP_LOWMEM:
      callLowMem(p_ctx);
      break;
    default:
      TSKM_ASSERT(0);
      break;
  }
}

/*********************************************
 *   Initialize Context
 *********************************************/
TSKM_STATIC void initCtx(T_PRIM_PRM* p_prm, PRI_CTX_t* p_ctx, int argc,
                         char* argv[]) {
  FrameworkunifiedDefaultCallbackHandler cbFuncs;

  p_ctx->prm = *p_prm;

  cbFuncs.onInitilization = p_ctx->prm.onInit;
  cbFuncs.onDestroy = p_ctx->prm.onDestory;
  cbFuncs.onDebugDump = p_ctx->prm.onDebugDump;
  cbFuncs.onStart = FrameworkunifiedOnStart;
  cbFuncs.onStop = FrameworkunifiedOnStop;
  cbFuncs.createStateMachine = FrameworkunifiedCreateStateMachine;
  cbFuncs.ssFrameworkInterface = FrameworkunifiedSSFrameworkInterface;

  EFrameworkunifiedStatus taskmanagerRet;
  taskmanagerRet = FrameworkunifiedCreateDispatcherWithoutLoop(p_ctx->prm.name, p_ctx->hApp, argc,
                                          argv, &cbFuncs, FALSE);
  if (eFrameworkunifiedStatusOK != taskmanagerRet) {  // LCOV_EXCL_BR_LINE 5: Checked in Death testing, but it is not reflected in the coverage and excluded
    // LCOV_EXCL_START 5: Checked in Death testing, but it is not reflected in the coverage and excluded
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT_PRINT(0, "%d", taskmanagerRet);
    goto ERROR;
    // LCOV_EXCL_STOP 5: Checked in Death testing, but it is not reflected in the coverage and excluded
  }

  taskmanagerRet = FrameworkunifiedGetDispatcherFD(p_ctx->hApp, &p_ctx->nsFd);
  if (taskmanagerRet != eFrameworkunifiedStatusOK) {
    TSKM_ASSERT_PRINT(0, "%d", taskmanagerRet);
    exit(EXIT_FAILURE);
  }
  p_ctx->connFd = tskm_cliSockConnect(TSKM_SOCKET_NAME);
  if (p_ctx->connFd < 0) {  // LCOV_EXCL_BR_LINE 5: Checked in Death testing, but it is not reflected in the coverage and excluded
    // LCOV_EXCL_START 5: Checked in Death testing, but it is not reflected in the coverage and excluded
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT(0);
    goto ERROR;
    // LCOV_EXCL_STOP 5: Checked in Death testing, but it is not reflected in the coverage and excluded
  }
  if (pipe(p_ctx->pipeFd) != 0) {  // LCOV_EXCL_BR_LINE 5: Checked in Death testing, but it is not reflected in the coverage and excluded
    // LCOV_EXCL_START 5: Checked in Death testing, but it is not reflected in the coverage and excluded
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT_ERRNO(0);
    goto ERROR;
    // LCOV_EXCL_STOP 5: Checked in Death testing, but it is not reflected in the coverage and excluded
  }

  return;

  // LCOV_EXCL_START 5: Checked in Death testing, but it is not reflected in the coverage and excluded
  ERROR:
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  tskm_pf_abort();
  // LCOV_EXCL_STOP
}

/*********************************************
 *  Destroy context
 *********************************************/
TSKM_STATIC void termCtx(PRI_CTX_t* p_ctx) {
  if (p_ctx->shmDone) {  // LCOV_EXCL_BR_LINE 6: Since it has been set to True by INI_Handler and cannot be changed
    const PRIM_SHAREDATA_TBL* shmEntry = p_ctx->prm.shmTbl;
    for (shmEntry = p_ctx->prm.shmTbl; shmEntry->shmName[0] != '\0';
        shmEntry++) {
      TSKM_ASSERT(0 == tskm_pf_shmDelete(shmEntry->shmName));  // LCOV_EXCL_BR_LINE 8: For processing in which only return value 0 is set
    }
  }

  if (p_ctx->connFd > 0) {  // LCOV_EXCL_BR_LINE 6: For processing in which only return value 0 is set
    tskm_sockDestory(p_ctx->connFd);
  }

  EFrameworkunifiedStatus taskmanagerRet;
  taskmanagerRet = FrameworkunifiedDestroyDispatcherWithoutLoop(p_ctx->hApp);
  TSKM_ASSERT(taskmanagerRet == eFrameworkunifiedStatusOK);

  if (p_ctx->isDynamic) {
    TSKM_PRINTF(TSKM_LOG_STATE, "EXIT %s", p_ctx->procName);
  } else {
    // Hung up running services to prevent adversely affecting to system termination processing during process termination processing
    sleep(TSKM_CFG_WAIT_SHUTDOWN);
  }
}

/*******************************************************************
 *    Initialize PRI context
 *******************************************************************/
void pri_init(T_PRIM_PRM* p_prm, int argc, char* argv[], int *fdNum,
              int fdlist[INI_FD_MAX]) {
  int ret = 0;
  PRI_CTX_t* p_ctx = &g_pri;

  strncpy(p_ctx->procName, basename(argv[0]), sizeof(p_ctx->procName) - 1);

  ret = tskm_pf_procInit();
  if (ret != 0) {  // LCOV_EXCL_BR_LINE 6: Return value of 0 only
    // LCOV_EXCL_START 6: Return value of 0 only
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT(0);
    goto ERROR;
    // LCOV_EXCL_STOP 6: Return value of 0 only
  }

  ret = tskm_comm_procInit();
  if (ret != 0) {  // LCOV_EXCL_BR_LINE 5: Checked in Death testing, but it is not reflected in the coverage and excluded
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT(0);
    goto ERROR;
    // LCOV_EXCL_STOP 5: Checked in Death testing, but it is not reflected in the coverage and excluded
  }

  initCtx(p_prm, p_ctx, argc, argv);

  *fdNum = 3;
  fdlist[0] = p_ctx->connFd;
  fdlist[1] = p_ctx->pipeFd[0];
  fdlist[2] = p_ctx->nsFd;

  p_ctx->svcId = TSKM_SVCID_NONE;
  p_ctx->isExec = TSKM_TRUE;

  p_ctx->bootInfo.startupReason = epswfINVALID;
  p_ctx->bootInfo.isUserModeOn = FALSE;
  p_ctx->bootInfo.dataResetMode = e_SS_SM_DATA_RESET_MODE_NONE;
  p_ctx->bootInfo.securityStatus = epsssINVALID;
  p_ctx->bootInfo.wakeupType = epsstINVALID;
  p_ctx->bootInfo.dramBackupStatus = e_SS_SM_DRAM_BACKUP_UNSET;
  p_ctx->bootInfo.resetStatus = e_SS_SM_RESET_STATUS_UNSET;

  memset(&p_ctx->extBootInfo, 0, sizeof(p_ctx->extBootInfo));

  p_ctx->timeout = PRI_MONITOR_DEFAULT_TIMEOUT;

  return;
  // LCOV_EXCL_START 5: Checked in Death testing, but it is not reflected in the coverage and excluded
  ERROR:
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  tskm_pf_abort();  // ABORT
// LCOV_EXCL_STOP
}

/*******************************************************************
 *    Primary library handler
 *******************************************************************/
BOOL pri_handler(fd_set* p_fds) {
  PRI_CTX_t* p_ctx = &g_pri;

  if (FD_ISSET(p_ctx->connFd, p_fds)) {
    int ret;
    TSKM_EVENT_INFO_t ev;
    ret = tskm_sockRcv(p_ctx->connFd, &ev);
    // LCOV_EXCL_BR_START 5: True condition is checked in Death tests, but not reflected in coverage and excluded
    if (ret > 0) {
    // LCOV_EXCL_BR_STOP
      eventHandle(p_ctx, &ev);
    } else {  // LCOV_EXCL_BR_LINE 5: Checked in Death testing, but it is not reflected in the coverage and excluded
      // LCOV_EXCL_START 5: Checked in Death testing, but it is not reflected in the coverage and excluded
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      TSKM_ASSERT(0);
      goto ERROR;
      // LCOV_EXCL_STOP
    }
  }

  if (FD_ISSET(p_ctx->pipeFd[0], p_fds)) {
    // only use exitDone
    uint32_t tmp;
    TSKM_ASSERT(sizeof(tmp) == read(p_ctx->pipeFd[0], &tmp, sizeof(tmp)));
    TSKM_ASSERT(p_ctx->downStepDone == PRIM_ACCOFF_MAX);  // Check if all exit functions are complete
    if (p_ctx->isDynamic) {
      // A nonresident service completes its termination processing by terminating the process (because the SIGNAL will overtake the sockets)
    } else {
      // The resident service completes termination processing with a termination notice (Do not terminate processes to reduce the impact on system termination)
      sendLastDoneRes(p_ctx);
    }
    p_ctx->isExec = TSKM_FALSE;
  }

  if (FD_ISSET(p_ctx->nsFd, p_fds)) {
    FrameworkunifiedDispatchProcessWithoutLoop(p_ctx->hApp);
  }
  return p_ctx->isExec;

  // LCOV_EXCL_START 5: Checked in Death testing, but it is not reflected in the coverage and excluded
  ERROR:
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  tskm_pf_abort();  // ABORT
  return 0;
// LCOV_EXCL_STOP
}

/*******************************************************************
 *    Process termination
 *******************************************************************/
void pri_term(void) {
  PRI_CTX_t* p_ctx = &g_pri;
  termCtx(p_ctx);
}

/*******************************************************************
 *    Service Monitoring Status Setting
 *******************************************************************/
int pri_setMonitorState(BOOL bIsRun, uint32_t timeout) {
  PRI_CTX_t* p_ctx = &g_pri;
  int ret = INI_SUCCESS;

  if (TSKM_SVCID_NONE == p_ctx->svcId) {
    // Ignore requests until svcId is acquired
  } else if ((TRUE == bIsRun) && (0 == timeout)) {
    // When RUN is specified with timeout = 0, monitoring is disabled
  } else {
    ret = tskm_comm_setSvcWatchState(p_ctx->svcId, bIsRun, timeout);
    if (INI_SUCCESS != ret) {
      TSKM_ASSERT(0);
    }
  }

  return ret;
}

/*******************************************************************
 *    MAIN function
 *******************************************************************/
int pri_main(T_PRIM_PRM* p_prm, int argc, char* argv[]) {
  int mainRet = -1;
  int fdlist[INI_FD_MAX];
  int fdNum;
  int ii;
  BOOL isExec = TRUE;

  pri_init(p_prm, argc, argv, &fdNum, fdlist);

  while (isExec) {
    PRI_CTX_t* p_ctx = &g_pri;
    int maxFd = 0;
    fd_set fds;
    int ret;

    FD_ZERO(&fds);

    for (ii = 0; ii < fdNum; ii++) {
      FD_SET(fdlist[ii], &fds);
      maxFd = TSKM_MAX(fdlist[ii], maxFd);
    }

    TSKM_ASSERT(INI_SUCCESS == pri_setMonitorState(FALSE, 0));
    ret = select(maxFd + 1, &fds, NULL, NULL, NULL);
    TSKM_ASSERT(INI_SUCCESS == pri_setMonitorState(TRUE, p_ctx->timeout));
    if (ret < 1) {
      if (errno != EINTR) {
        TSKM_ASSERT(0);
      }
      continue;
    }

    isExec = pri_handler(&fds);
  }

  mainRet = 0;

  pri_term();
  return mainRet;
}

/*******************************************************************
 *    Termination request
 *******************************************************************/
void pri_exitStart(void *rsv) {
  int ret;
  PRI_CTX_t* p_ctx = &g_pri;
  TSKM_EVENT_INFO_t ev;

  bzero(&ev, sizeof(ev));
  p_ctx->isExitStart = TRUE;
  ev.event = TSKM_EV_PRI_REQ_EXIT;
  ev.errCode = TSKM_E_OK;

  ret = tskm_sockSend(p_ctx->connFd, &ev);
  if (ret <= 0) {  // LCOV_EXCL_BR_LINE 6: The caller's external API does not execute the second or subsequent processing and cannot be checked
    TSKM_ASSERT(0);
    goto ERROR;
  }
  return;
  // LCOV_EXCL_START 6: The caller's external API does not execute the second or subsequent processing and cannot be checked
  ERROR:
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  tskm_pf_abort();
  // LCOV_EXCL_STOP
}

void pri_exitDone(int status) {
  PRI_CTX_t* p_ctx = &g_pri;
  uint32_t l_status = (uint32_t) status;

  // LCOV_EXCL_BR_START 6: The caller's external API does not execute the second or subsequent processing and cannot be checked
  if (p_ctx->pipeFd[1] > 0) {
  // LCOV_EXCL_BR_STOP
    // LCOV_EXCL_START 6: The caller's external API does not execute the second or subsequent processing and cannot be checked
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT_ERRNO(
        write(p_ctx->pipeFd[1], &l_status, sizeof(l_status))
            == sizeof(l_status));
    // LCOV_EXCL_STOP
  } else {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT(0);  // LCOV_EXCL_LINE 6: The caller's external API does not execute the second or subsequent processing and cannot be checked
  }
}

/*******************************************************************
 *    Event completion notification at startup
 *******************************************************************/
int32_t pri_stepForkComp(uint64_t id) {
  int ret;
  PRI_CTX_t* p_ctx = &g_pri;
  TSKM_EVENT_INFO_t ev;

  bzero(&ev, sizeof(ev));
  ev.event = TSKM_EV_PRI_REP_WAKEUP_COMP;
  ev.errCode = TSKM_E_OK;
  ev.prm.repWakeupComp.compId = id;
  ret = tskm_sockSend(p_ctx->connFd, &ev);
  if (ret <= 0) {
    TSKM_ASSERT(0);
    goto ERROR;
  }
  return INI_SUCCESS;
  ERROR: return INI_FALSE;
}

/*******************************************************************
 *    Event completion notification at termination
 *******************************************************************/
int32_t pri_accOffComp(uint64_t id) {
  int ret;
  PRI_CTX_t* p_ctx = &g_pri;
  TSKM_EVENT_INFO_t ev;

  bzero(&ev, sizeof(ev));

  ev.event = TSKM_EV_PRI_REP_DOWN_COMP;
  ev.errCode = TSKM_E_OK;
  ev.prm.repDownComp.compId = id;
  ret = tskm_sockSend(p_ctx->connFd, &ev);
  if (ret <= 0) {
    TSKM_ASSERT(0);
    goto ERROR;
  }
  return INI_SUCCESS;
  ERROR: return INI_FALSE;
}

/*******************************************************************
 *    Get private info
 *******************************************************************/
void*
pri_getPrivate() {
  PRI_CTX_t* p_ctx = &g_pri;
  return p_ctx->prm.priv;
}

/*******************************************************************
 *    Get app-handle
 *******************************************************************/
HANDLE pri_getHandle() {
  PRI_CTX_t* p_ctx = &g_pri;
  return p_ctx->hApp;
}

/*******************************************************************
 *    Service monitoring status setting timeout setting
 *******************************************************************/
int32_t pri_setMonitorTimeout(uint32_t timeout) {
  PRI_CTX_t* p_ctx = &g_pri;
  p_ctx->timeout = timeout;
  return INI_SUCCESS;
}

/*******************************************************************
 *    Get boot info
 *******************************************************************/
int32_t pri_getBootInfo(T_SS_SM_START_DataStructType *info) {
  PRI_CTX_t* p_ctx = &g_pri;

  if (p_ctx->bootInfo.startupReason == epswfINVALID) {
    TSKM_ASSERT(0);
    return INI_FALSE;
  }

  *info = p_ctx->bootInfo;

  return INI_SUCCESS;
}

/*******************************************************************
 *    Get extended boot info
 *******************************************************************/
int32_t pri_getExtBootInfo(T_SS_SM_START_ExtDataStructType *info) {
  PRI_CTX_t* p_ctx = &g_pri;

  if (p_ctx->bootInfo.startupReason == epswfINVALID) {
    TSKM_ASSERT(0);
    return INI_FALSE;
  }

  *info = p_ctx->extBootInfo;

  return INI_SUCCESS;
}

/*******************************************************************
 *    DebugDump response
 *******************************************************************/
void pri_sendDebugDumpRes(const char *buf) {  // LCOV_EXCL_START 7: Debugging code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  int ret;
  PRI_CTX_t* p_ctx = &g_pri;
  TSKM_EVENT_INFO_t ev;
  TSKM_EV_PRI_EX_RES_DEBUGDUMP_PRM_t *p_prm;

  ev.event = TSKM_EV_PRI_RES_DEBUGDUMP;
  ev.errCode = TSKM_E_OK;
  ev.hasExtend = TSKM_TRUE;

  ev.extendPrm = malloc(sizeof(TSKM_EV_PRI_EX_RES_DEBUGDUMP_PRM_t));
  if (!ev.extendPrm) {
    TSKM_ASSERT(0);
    goto ERROR;
  }

  ev.extendSize = sizeof(TSKM_EV_PRI_EX_RES_DEBUGDUMP_PRM_t);

  p_prm = (TSKM_EV_PRI_EX_RES_DEBUGDUMP_PRM_t *) ev.extendPrm;  // NOLINT (readability/casting)
  snprintf(p_prm->dumpMsg, TSKM_EV_DEBUGDUMP_SIZE, "%s", buf);

  ret = tskm_sockSend(p_ctx->connFd, &ev);
  if (ret <= 0) {
    TSKM_ASSERT(0);
  }

  ERROR: return;
}
// LCOV_EXCL_STOP
/*************************************************
 * Empty functions implemented for building software
 **************************************************/
EFrameworkunifiedStatus FrameworkunifiedOnStart(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
EFrameworkunifiedStatus FrameworkunifiedOnStop(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus FrameworkunifiedCreateStateMachine(HANDLE hApp) {  // LCOV_EXCL_START 6: Because the condition cannot be set
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
// LCOV_EXCL_STOP
