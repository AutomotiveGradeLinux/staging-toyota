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

#include "tskm_main.h"
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/inotify.h>
#include <getopt.h>
#include <system_service/ss_services.h>
#include <native_service/ns_version_if.h>
#include <native_service/ns_np_service_protocol.h>
#include <system_service/ss_system_if.h>
#include <system_service/ss_version.h>
#include <system_service/ss_templates.h>
#include <native_service/frameworkunified_dispatcher.h>
#include <other_service/rpc.h>
#include <system_service/resm.h>

#include "tskm_data_init_local.h"
#include "tskm_port_pf.h"
#include "tskm_port_subsys.h"
#include "tskm_debug.h"
#include "tskm_util.h"
#include "tskm_srvr.h"
#include "tskm_comm.h"
#include "tskm_watch.h"
#include "system_service/tskm_local_type.h"
#include "system_service/tskm_xml_data.h"
#include "tskm_state.h"

#define TSKM_DATA_KEY "TSKM_KEY"

TSKM_STATIC TSKM_MAIN_CTX_t g_mainCtx;  // Main context

TSKM_STATIC BOOL s_isVupMode = FALSE;

CFrameworkunifiedVersion g_FrameworkunifiedVersion(MAJORNO, MINORNO, REVISION);

// For NsLog
FRAMEWORKUNIFIEDLOGPARAM g_FrameworkunifiedLogParams = {
FRAMEWORKUNIFIEDLOGOPTIONS, {
ZONE_TEXT_10, ZONE_TEXT_11, ZONE_TEXT_12,
ZONE_TEXT_13, ZONE_TEXT_14, ZONE_TEXT_15,
ZONE_TEXT_16, ZONE_TEXT_17, ZONE_TEXT_18,
ZONE_TEXT_19, ZONE_TEXT_20, ZONE_TEXT_21,
ZONE_TEXT_22, ZONE_TEXT_23, ZONE_TEXT_24,
ZONE_TEXT_25, ZONE_TEXT_26, ZONE_TEXT_27,
ZONE_TEXT_28, ZONE_TEXT_29, ZONE_TEXT_30,
ZONE_TEXT_31 }, FRAMEWORKUNIFIEDLOGZONES };

EFrameworkunifiedStatus OnFinishLoadFile(HANDLE hApp) {  // LCOV_EXCL_START 6: Because the condition cannot be set
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  int ret;
  TSKM_MAIN_CTX_t* p_main = &g_mainCtx;
  TSKM_EVENT_INFO_t ev;

  ret = tskm_pf_nvFileRead(hApp, &p_main->nvInfo);
  if (ret != 0) {
    TSKM_ASSERT(0);
  }

  // issure INI_INITCOMP_NVM_ACCESS
  bzero(&ev, sizeof(ev));
  ev.event = TSKM_EV_PRI_REP_WAKEUP_COMP;
  ev.errCode = TSKM_E_OK;
  ev.prm.repWakeupComp.compId = INI_INITCOMP_NVM_ACCESS;
  tskm_handleEvent(p_main, &ev);

  return l_eStatus;
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus onDataInitRequest(HANDLE hApp) {
  TSKM_ERR_t l_tskmRet = TSKM_E_OK;
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  TSKM_MAIN_CTX_t* p_main = tskm_getMainCtx();

  TSKM_PRINTF(TSKM_LOG_API, "%s()", __FUNCTION__)

  if (p_main->bootInfo.startupReason == epswfINVALID) {
    // Before issuing FrameworkunifiedOnStart = Requests before TaskManager became Availabile are not accepted
    TSKM_ASSERT(0);
    l_tskmRet = TSKM_E_STATE;
  } else {
    T_SS_SM_START_DataStructType bootInfo(epswfIGN_ACC, TRUE,
                                          e_SS_SM_DATA_RESET_MODE_USER,
                                          epsssUNLOCK, epsstWARMSTART,
                                          e_SS_SM_DRAM_BACKUP_OK,
                                          e_SS_SM_RESET_STATUS_NONE, 0);

    T_SS_SM_START_ExtDataStructType extBootInfo;

    memcpy(&extBootInfo, &p_main->extBootInfo, sizeof(extBootInfo));

    // After updating the program,
    // FALSE is notified here because it is notified when data initialization I/F is called at startup
    extBootInfo.isProgUpdated = FALSE;

    tskm_dataInitAll(&bootInfo, &extBootInfo);
  }

  PCSTR l_senderName = FrameworkunifiedGetMsgSrc(hApp);
  if (NULL != l_senderName) {
    HANDLE hSession = FrameworkunifiedMcOpenSender(hApp, l_senderName);
    if (hSession == NULL) {
      TSKM_ASSERT(0);
    } else {
      TSKM_ASSERT(
          eFrameworkunifiedStatusOK
              == FrameworkunifiedSendMsg(hSession, TSKM_DATAINIT_RESP, sizeof(l_tskmRet),
                            &l_tskmRet));
      TSKM_ASSERT(eFrameworkunifiedStatusOK == FrameworkunifiedMcClose(hSession));
    }
  }

  return l_eStatus;
}

EFrameworkunifiedStatus onTransStepRequest(HANDLE hApp) {  // LCOV_EXCL_START 6:Because the condition cannot be set
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  TSKM_MAIN_CTX_t* p_main = &g_mainCtx;
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  TSKM_EVENT_INFO_t ev;
  bzero(&ev, sizeof(ev));

  ev.event = TSKM_EV_LCL_REQ_TRANS_STEP;
  ev.errCode = TSKM_E_OK;
  tskm_handleEvent(p_main, &ev);

  return l_eStatus;
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus OnResmAvailability(HANDLE hApp) {  // LCOV_EXCL_START 6: Because the condition cannot be set
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  TSKM_MAIN_CTX_t* p_main = &g_mainCtx;

  if (0 != strcmp(NTFY_ResourceMgr_Availability, FrameworkunifiedGetLastNotification(hApp))) {
    TSKM_ASSERT(0);
  } else if (FrameworkunifiedIsServiceAvailable(hApp)) {
    // Availability = TRUE

    p_main->resmFd = tskm_sub_resmInit();
    if (-1 == p_main->resmFd) {
      TSKM_ASSERT(0);
    }
  }
  return eFrameworkunifiedStatusOK;
}
// LCOV_EXCL_STOP

//************ NS Framework Callback : must implemet ******************
EFrameworkunifiedStatus FrameworkunifiedOnInitialization(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  TSKM_PRINTF(TSKM_LOG_STATE, "+");

  l_eStatus = FrameworkunifiedRegisterServiceAvailabilityNotification(hApp, NTFY_SS_TaskManager_Availability);
  if (eFrameworkunifiedStatusOK != l_eStatus) {
    TSKM_ASSERT(0);
  }

  l_eStatus = FrameworkunifiedPublishServiceAvailability(hApp, FALSE);
  if (eFrameworkunifiedStatusOK != l_eStatus) {
    TSKM_ASSERT(0);
  }

  l_eStatus = FrameworkunifiedAttachCallbackToDispatcher(hApp, FRAMEWORKUNIFIED_NS_NPSERVICE,
                                            NPS_GET_PERS_FILE_ACK,
                                            OnFinishLoadFile);
  if (eFrameworkunifiedStatusOK != l_eStatus) {
    TSKM_ASSERT(0);
  }

  l_eStatus = FrameworkunifiedAttachCallbackToDispatcher(hApp, FRAMEWORKUNIFIED_ANY_SOURCE,
                                            TSKM_DATAINIT_REQ,
                                            onDataInitRequest);
  if (eFrameworkunifiedStatusOK != l_eStatus) {
    TSKM_ASSERT(0);
  }

  l_eStatus = FrameworkunifiedAttachCallbackToDispatcher(hApp, FrameworkunifiedGetAppName(hApp),
                                            TSKM_TRANS_STEP_REQ,
                                            onTransStepRequest);
  if (eFrameworkunifiedStatusOK != l_eStatus) {
    TSKM_ASSERT(0);
  }

  l_eStatus = FrameworkunifiedSubscribeNotificationWithCallback(hApp, NTFY_ResourceMgr_Availability, OnResmAvailability);
  if (eFrameworkunifiedStatusOK != l_eStatus) {
    TSKM_ASSERT(0);
  }

  TSKM_PRINTF(TSKM_LOG_STATE, "-");

  return l_eStatus;
}

EFrameworkunifiedStatus FrameworkunifiedOnStart(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  TSKM_PRINTF(TSKM_LOG_STATE, "+");
  l_eStatus = OnStart(hApp);
  TSKM_PRINTF(TSKM_LOG_STATE, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus FrameworkunifiedOnPreStart(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  TSKM_PRINTF(TSKM_LOG_STATE, "+");
  l_eStatus = OnStart(hApp);
  TSKM_PRINTF(TSKM_LOG_STATE, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus FrameworkunifiedOnBackgroundStart(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  TSKM_PRINTF(TSKM_LOG_STATE, "+");
  l_eStatus = OnStart(hApp);
  TSKM_PRINTF(TSKM_LOG_STATE, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus FrameworkunifiedOnStop(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  TSKM_MAIN_CTX_t* p_main = &g_mainCtx;
  // Returns Fail to send asynchronous STOP reply to SM
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;

  TSKM_EVENT_INFO_t ev;
  bzero(&ev, sizeof(ev));

  ev.event = TSKM_EV_LCL_REQ_STOP;
  ev.errCode = TSKM_E_OK;
  tskm_handleEvent(p_main, &ev);

  p_main->isOnStopDone = TSKM_TRUE;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus FrameworkunifiedOnPreStop(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus FrameworkunifiedOnBackgroundStop(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus FrameworkunifiedOnDestroy(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus FrameworkunifiedOnDebugDump(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  TSKM_MAIN_CTX_t* p_main = &g_mainCtx;
  TSKM_ERR_t l_tskmRet;

  // Call DebugDump of all running SVCs
  l_tskmRet = tskm_svcsCallDebugDump(&p_main->svcs);
  if (TSKM_E_OK != l_tskmRet) {
    TSKM_ASSERT(0);
  }

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

/*********************************************************
 *        System start process
 *********************************************************/
EFrameworkunifiedStatus OnStart(const HANDLE hApp) {
  TSKM_PRINTF(TSKM_LOG_STATE, "+");
  TSKM_MAIN_CTX_t* const p_main = &g_mainCtx;
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (p_main->isOnStartDone == TSKM_FALSE) {
    T_SS_SM_START_DataStructType startupInfo;
    TSKM_EVENT_INFO_t ev;
    l_eStatus = ReadMsg<T_SS_SM_START_DataStructType>(hApp, startupInfo);
    if (l_eStatus != eFrameworkunifiedStatusOK) {
      TSKM_ASSERT(0);
    } else {
      memcpy(&p_main->bootInfo, &startupInfo, sizeof(T_SS_SM_START_DataStructType));
    }

    if (0 != tskm_sub_getExtBootInfo(&p_main->extBootInfo)) {
      TSKM_ASSERT(0);
    }

    tskm_dataInitAll(&p_main->bootInfo, &p_main->extBootInfo);

    tskm_svcsSetBootInfo(&p_main->svcs, &p_main->bootInfo, &p_main->extBootInfo);
    p_main->isOnStartDone = TSKM_TRUE;

    l_eStatus = FrameworkunifiedPublishServiceAvailability(hApp, TRUE);
    if (l_eStatus != eFrameworkunifiedStatusOK) {
      TSKM_ASSERT(0);
    } else {
      TSKM_PRINTF(TSKM_LOG_STATE, "Availability TRUE");
    }

    // Issure INI_INITCOMP_ON_START
    bzero(&ev, sizeof(ev));
    ev.event = TSKM_EV_PRI_REP_WAKEUP_COMP;
    ev.errCode = TSKM_E_OK;
    ev.prm.repWakeupComp.compId = INI_INITCOMP_ON_START;
    tskm_handleEvent(p_main, &ev);
  }

  TSKM_PRINTF(TSKM_LOG_STATE, "-");
  return l_eStatus;
}

/*********************************************************
 *        Argument analysis process
 *********************************************************/
TSKM_STATIC EFrameworkunifiedStatus argParser(SI_32 argument, PCHAR argumentValue) {  // LCOV_EXCL_START 6: for process initialization processing
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  if ('v' == argument) {
    s_isVupMode = TRUE;
  }

  return eFrameworkunifiedStatusOK;
}
// LCOV_EXCL_STOP
/*********************************************************
 *        Creating process for TSKM internal context
 *********************************************************/
TSKM_STATIC void ctxCreate(TSKM_MAIN_CTX_t* p_main, int argc, char* argv[]) {
  int ret;
  EFrameworkunifiedStatus taskmanagerRet;
  TSKM_ERR_t tskmRet;
  FrameworkunifiedDefaultCallbackHandler cbFuncs;
  FRAMEWORKUNIFIED_MAKE_DEFAULT_CALLBACK(cbFuncs);
  CustomCommandLineOptions cmdLineOpt = { "v", NULL, argParser };

  p_main->state = TSKM_ST_ACCOFF;
  p_main->isOnStartDone = TSKM_FALSE;

  // Create dispatcher
  taskmanagerRet = FrameworkunifiedCreateDispatcherWithoutLoop(SS_TASK_MANAGER, p_main->hApp, argc,
                                          argv, &cbFuncs, TRUE, &cmdLineOpt);
  if (eFrameworkunifiedStatusOK != taskmanagerRet) {  // LCOV_EXCL_BR_LINE 6: For process initialization processing
    // LCOV_EXCL_START 6: For process initialization processing
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT_PRINT(0, "%d", taskmanagerRet);
    goto ERROR;
    // LCOV_EXCL_STOP
  }

  TSKM_PRINTF(TSKM_LOG_STATE, "s_isVupMode :%s", s_isVupMode ? "TRUE" : "FALSE");

  taskmanagerRet = FrameworkunifiedGetDispatcherFD(p_main->hApp, &p_main->nsFd);
  if (taskmanagerRet != eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 6: For process initialization processing
    // LCOV_EXCL_START 6: For process initialization processing
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT_PRINT(0, "%d", taskmanagerRet);
    exit(EXIT_FAILURE);
    // LCOV_EXCL_STOP
  }

  p_main->sigFd = tskm_pf_sysInit();  // Platform initialization
  if (p_main->sigFd == -1) {  // LCOV_EXCL_BR_LINE 6: For process initialization processing
    // LCOV_EXCL_LINE 6: For process initialization processing
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT(0);
    goto ERROR;
    // LCOV_EXCL_STOP
  }

  p_main->iFd = inotify_init1(IN_CLOEXEC);
  if (p_main->sigFd == -1) {  // LCOV_EXCL_BR_LINE 6: For process initialization processing
    // LCOV_EXCL_LINE 6: For process initialization processing
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT_ERRNO(0);
    goto ERROR;
    // LCOV_EXCL_STOP
  }

  // Obtain RESM FDs after waiting for Availability
  p_main->resmFd = -1;

  tskmRet = tskm_srvSockCreate(TSKM_SOCKET_NAME, &p_main->sock);
  TSKM_ERR_CHK(tskmRet, TSKM_E_OK, ERROR);  // LCOV_EXCL_BR_LINE 6: For process initialization processing

  ret = tskm_pf_nvFileInit(p_main->hApp);
  if (ret != 0) {  // LCOV_EXCL_BR_LINE 6: For process initialization processing
    // LCOV_EXCL_START 6: For process initialization processing
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT(0);
    goto ERROR;
    // LCOV_EXCL_STOP
  }

  ret = tskm_initServiceList(&p_main->svcs, p_main->iFd);   // Service to be started
  if (0 != ret) {  // LCOV_EXCL_BR_LINE 6: For process initialization processing
    // LCOV_EXCL_START 6: For For process initialization process
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT(0);
    goto ERROR;
    // LCOV_EXCL_STOP
  }

  tskm_initWakeupCtx(&p_main->wakeup, s_isVupMode);  // Gradual startup info
  tskm_initDownCtx(&p_main->down, s_isVupMode);      // Gradual termination info
  return;

  // LCOV_EXCL_START 6: For process initialization processing
  ERROR:
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  tskm_pf_exit(EXIT_FAILURE);
  return;
  // LCOV_EXCL_STOP
}

/*********************************************************
 *        Get main context
 *********************************************************/
TSKM_MAIN_CTX_t*
tskm_getMainCtx(void) {
  return &g_mainCtx;
}

/*********************************************************
 *        Process initialization process (MAIN function sub)
 *********************************************************/
int Init_Process(int* rpcFd, int* sockFd, int* maxFd, fd_set* fds, TSKM_MAIN_CTX_t* p_main) {
  int ret;
  uint32_t ii;

  TSKM_SRV_CONNENT_LIST_t* p_connList = &p_main->sock.connList;

  FD_ZERO(fds);

  FD_SET(*rpcFd, fds);
  *maxFd = TSKM_MAX(*rpcFd, *maxFd);  // LCOV_EXCL_BR_LINE 6:  For process initialization processing

  FD_SET(*sockFd, fds);
  *maxFd = TSKM_MAX(*sockFd, *maxFd);  // LCOV_EXCL_BR_LINE 6:  For process initialization processing

  FD_SET(p_main->nsFd, fds);
  *maxFd = TSKM_MAX(p_main->nsFd, *maxFd);  // LCOV_EXCL_BR_LINE 6:  For process initialization processing

  FD_SET(p_main->sigFd, fds);
  *maxFd = TSKM_MAX(p_main->sigFd, *maxFd);  // LCOV_EXCL_BR_LINE 6:  For process initialization processing

  if (-1 != p_main->resmFd) {  // LCOV_EXCL_BR_LINE 6:  For process initialization processing
    // LCOV_EXCL_START 6:  For process initialization processing
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FD_SET(p_main->resmFd, fds);
    *maxFd = TSKM_MAX(p_main->resmFd, *maxFd);
    // LCOV_EXCL_STOP
  }

  for (ii = 0; ii < p_connList->num; ii++) {  // LCOV_EXCL_BR_LINE 6:  For process initialization processing
    // LCOV_EXCL_START 6:  For process initialization processing
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_SRV_CONNENT_t* p_conn = &p_connList->conn[ii];
    FD_SET(p_conn->connFd, fds);
    *maxFd = TSKM_MAX(p_conn->connFd, *maxFd);
    // LCOV_EXCL_STOP
  }

  TSKM_PRINTF(TSKM_LOG_FUNC, "SELECT IN");
  ret = select(*maxFd + 1, fds, NULL, NULL, NULL);
  if (ret < 1) {
    if (errno != EINTR) {
      TSKM_ASSERT(0);
    }
    return ret;
  }
  TSKM_PRINTF(TSKM_LOG_FUNC, "SELECT OUT");
  return 0;
}

/*********************************************************
 *        Receiving Messages from CONNECT Clients (MAIN Functions Sub)
 *********************************************************/
void Recv_Msg_FromConnectClient(fd_set* fds, TSKM_MAIN_CTX_t* p_main) {
  uint32_t ii;
  TSKM_SRV_CONNENT_LIST_t* p_connList = &p_main->sock.connList;

  // Messages from CONNECT clients
  for (ii = 0; ii < p_connList->num; ii++) {
    TSKM_SRV_CONNENT_t* p_conn = &p_connList->conn[ii];
    if (FD_ISSET(p_conn->connFd, fds)) {
      TSKM_PRINTF(TSKM_LOG_FUNC, "connFd IN");
      TSKM_EVENT_INFO_t ev;
      int ret;
      ret = tskm_sockRcv(p_conn->connFd, &ev);

      if (ret < 0) {
        TSKM_ASSERT(0);
      } else {
        TSKM_SVC_CTX_t* p_svc;

        if (ret == 0) {
          // Closed Connection
          TSKM_PRINTF(TSKM_LOG_STATE, "DISSCON pid:%d", p_conn->pid);
          ev.event = TSKM_EV_PRI_REP_DISCONNECT;
          ev.errCode = TSKM_E_OK;
          ev.fromPid = p_conn->pid;
          tskm_srvSockDisconnect(&p_main->sock, p_conn);
          p_conn = NULL;  // If the connection is lost, it cannot be referenced.
        }

        p_svc = tskm_svcsGetSvcByPid(&p_main->svcs, ev.fromPid);
        if (p_svc) {  // Events from service
          TSKM_ERR_t tskmRet;
          tskmRet = tskm_svcEventHandle(p_svc, &ev);  // Service handler
          if (tskmRet != TSKM_E_OK) {
            TSKM_ASSERT(0);
          }
        }
        if (ev.event != TSKM_EV_NOP) {
          tskm_handleEvent(p_main, &ev);             // Main handler
        }
      }
      TSKM_PRINTF(TSKM_LOG_FUNC, "connFd OUT");
    }
  }
}

/*********************************************************
 *        CONNECT Requests (MAIN Functions Sub)
 *********************************************************/
void Recv_Req_Connect(fd_set* fds, TSKM_MAIN_CTX_t* p_main, int sockFd) {
  // CONNECT requests
  if (FD_ISSET(sockFd, fds)) {
    TSKM_PRINTF(TSKM_LOG_FUNC, "sockFd IN");

    TSKM_SRV_CONNENT_t* conn;
    conn = tskm_srvSockConnect(&p_main->sock);
    if (conn == NULL) {
      TSKM_ASSERT(0);
    } else {
      TSKM_SVC_CTX_t* p_svc;
      TSKM_PRINTF(TSKM_LOG_STATE, "CONNECT pid:%d", conn->pid);

      p_svc = tskm_svcsGetSvcByPid(&p_main->svcs, conn->pid);
      if (p_svc) {
        TSKM_ERR_t tskmRet;
        TSKM_EVENT_INFO_t ev;

        ev.event = TSKM_EV_PRI_REP_CONNECT;
        ev.errCode = TSKM_E_OK;
        ev.fromPid = conn->pid;
        ev.prm.repConnect.connFd = conn->connFd;

        tskmRet = tskm_svcEventHandle(p_svc, &ev);  // Service handler
        if (tskmRet != TSKM_E_OK) {
          TSKM_ASSERT(0);
        } else if (ev.event != TSKM_EV_NOP) {
          tskm_handleEvent(p_main, &ev);            // Main hander
        }
      }
    }
    TSKM_PRINTF(TSKM_LOG_FUNC, "sockFd OUT");
  }
}

/*********************************************************
 *        MAIN Function
 *********************************************************/
int main(int argc, char* argv[]) {
  RPC_ID rpcId = TSKM_RPC_ID;
  int rpcFd = 0;
  int maxFd = 0;
  int ret;
  fd_set fds;
  TSKM_MAIN_CTX_t* p_main = &g_mainCtx;

  // Logging setting
  FRAMEWORKUNIFIED_SET_ZONES();

  ctxCreate(p_main, argc, argv);  // Initialize context

  tskm_sub_init();    // Initialize sub systems

  RPC_START_SECURE_SERVER(rpcId);
  RPC_get_fd(rpcId, &rpcFd);

  uid_t uid[2];
  gid_t gid[2];

  uid[0] = 0;
  gid[0] = 0;
  uid[1] = getuid();
  gid[1] = getgid();

  if (RPC_OK != RPC_regist_credential(2, uid, 2, gid)) {  // LCOV_EXCL_BR_LINE 6: For process initialization processing
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT(0);  // LCOV_EXCL_LINE 6: For process initialization processing
  }

  // Start periodic timer thread
  ret = tskm_watch_startTimer();
  if (ret != 0) {  // LCOV_EXCL_BR_LINE 6: For process initialization processing
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT(0);  // LCOV_EXCL_LINE 6: For process initialization processing
  }

  tskm_stateTransit(p_main, TSKM_ST_ACCOFF, TSKM_ST_ACCON);

  p_main->isExec = TSKM_TRUE;
  while (p_main->isExec) {  // LCOV_EXCL_BR_LINE 6: For process initialization processing
    int sockFd = p_main->sock.sockFd;

    // Initialization processing
    if (Init_Process(&rpcFd, &sockFd, &maxFd, &fds, p_main) != 0) {  // LCOV_EXCL_BR_LINE 6: For process initialization processing
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      continue;  // LCOV_EXCL_LINE 6: For process initialization processing
    }

    // RPC API
    if (FD_ISSET(rpcFd, &fds)) {
      RPC_process_API_request(rpcId);
    }

    // Process termination signal
    if (FD_ISSET(p_main->sigFd, &fds)) {
      int count = 0;

      do {
        TSKM_EVENT_INFO_t ev;
        ev.event = TSKM_EV_SVC_REP_TERM;

        count++;

        ret = tskm_pf_cleanupProc(p_main->sigFd, &ev.fromPid, &ev.errCode);
        if (ret == -1) {
          TSKM_ASSERT(0);
          break;
        }

        TSKM_SVC_CTX_t* p_svc;

        p_svc = tskm_svcsGetSvcByPid(&p_main->svcs, ev.fromPid);
        if (p_svc) {
          TSKM_ERR_t tskmRet;

          tskmRet = tskm_svcEventHandle(p_svc, &ev);  // Service handler
          if (tskmRet != TSKM_E_OK) {
            TSKM_ASSERT(0);
          }
          if (ev.event != TSKM_EV_NOP) {
            tskm_handleEvent(p_main, &ev);           // Main handler
          }
        }
      } while (ret == 1 && count < 50);
    }

    if (FD_ISSET(p_main->nsFd, &fds)) {
      FrameworkunifiedDispatchProcessWithoutLoop(p_main->hApp);
    }

    // EV from RESM
    if (-1 != p_main->resmFd) {
      if (FD_ISSET(p_main->resmFd, &fds)) {
        TSKM_EVENT_INFO_t ev;
        int ret;
        ret = tskm_sub_resmRcv(&ev);

        if (-1 == ret) {
          TSKM_ASSERT(0);
        } else {
          tskm_handleEvent(p_main, &ev);
        }
      }
    }

    // Messages from connected clients
    Recv_Msg_FromConnectClient(&fds, p_main);

    // CONNECT request
    Recv_Req_Connect(&fds, p_main, sockFd);
  }

  tskm_srvSockDestory(&p_main->sock);

  RPC_end(rpcId);

  FrameworkunifiedDestroyDispatcherWithoutLoop(p_main->hApp);
  TSKM_PRINTF(TSKM_LOG_STATE, "tskm end");

  sleep(TSKM_CFG_WAIT_SHUTDOWN);
  return 0;
}  // LCOV_EXCL_BR_LINE 10: Final line

