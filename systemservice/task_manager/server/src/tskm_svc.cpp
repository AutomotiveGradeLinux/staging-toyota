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

#include "system_service/tskm_svc.h"
#include <string.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <errno.h>

#include <native_service/ns_np_service_if.h>
#include <string>

#include "tskm_debug.h"
#include "tskm_util.h"
#include "tskm_port_pf.h"
#include "tskm_port_subsys.h"
#include "tskm_comm.h"


/*********************************************************
 *       Is the target event of the service handler?

 *********************************************************/
TSKM_STATIC TSKM_BOOL_t isSvcEvent(const TSKM_EVENT_INFO_t* p_ev) {
  switch (p_ev->event) {
    case TSKM_EV_PRI_REP_CONNECT:
    case TSKM_EV_PRI_REP_DISCONNECT:
    case TSKM_EV_PRI_RES_WAKEUP:
    case TSKM_EV_PRI_RES_DOWN:
    case TSKM_EV_PRI_RES_DEBUGDUMP:
    case TSKM_EV_PRI_REQ_EXIT:
    case TSKM_EV_SVC_REP_TERM:
      return TSKM_TRUE;
      break;
    default:
      break;
  }
  return TSKM_FALSE;
}

/*********************************************************
 *       Get service context
 *********************************************************/
TSKM_STATIC TSKM_SVC_CTX_t*
getSvcCtxBySvcId(TSKM_SVCS_CTX_t* p_svcs, TSKM_SVCID_t svcId) {
  uint32_t ii;

  for (ii = 0; ii < p_svcs->svcNum; ii++) {
    if (p_svcs->svcList[ii].attr->svcId == svcId) {
      return &p_svcs->svcList[ii];
    }
  }
  TSKM_ASSERT(0);
  return NULL;
}

/*********************************************************
 *       Get service context
 *********************************************************/
TSKM_STATIC TSKM_SVC_CTX_t*
getSvcCtxByPid(TSKM_SVCS_CTX_t* p_svcs, pid_t pid) {
  uint32_t ii;

  for (ii = 0; ii < p_svcs->svcNum; ii++) {
    if (p_svcs->svcList[ii].pid == pid) {
      return &p_svcs->svcList[ii];
    }
  }
  return NULL;
}

/*********************************************************
 *       Get service context
 *********************************************************/
TSKM_STATIC TSKM_SVC_CTX_t*
getSvcCtxByName(TSKM_SVCS_CTX_t* p_svcs, const char *p_name) {  // LCOV_EXCL_START 6: Because the condition cannot be set
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  uint32_t ii;

  for (ii = 0; ii < p_svcs->svcNum; ii++) {
    if (0
        == strncmp(p_svcs->svcList[ii].attr->name, p_name,
                   strlen(p_svcs->svcList[ii].attr->name))) {
      return &p_svcs->svcList[ii];
    }
  }
  return NULL;
}
// LCOV_EXCL_STOP
/*********************************************************
 *      Issuing a start request to the service
 *********************************************************/
TSKM_STATIC TSKM_ERR_t wakeupRequest(TSKM_SVC_CTX_t* p_svc,
                                     TSKM_GSTEP_REQ_INFO_t* p_req) {
  TSKM_EVENT_INFO_t ev;
  int ret;
  bzero(&ev, sizeof(ev));

  ev.event = TSKM_EV_PRI_REQ_WAKEUP;
  ev.errCode = TSKM_E_OK;
  ev.prm.reqWakeup.svcId = p_svc->attr->svcId;
  ev.prm.reqWakeup.localStep = p_req->localStep;
  ev.prm.reqWakeup.isDynamic =
      (p_svc->attr->lifeCycle == TSKM_SVC_LC_DYNAMIC) ? TSKM_TRUE : TSKM_FALSE;
  // Since there is no startup in the P_CWORD72_ but only communication in the ALL/LAST,
  // the startup information is optimized including the startup information in the communication of the WakeupRequest.
  memcpy(&ev.prm.reqWakeup.bootInfo, &p_svc->bootInfo,
         sizeof(ev.prm.reqWakeup.bootInfo));
  memcpy(&ev.prm.reqWakeup.extBootInfo, &p_svc->extBootInfo,
         sizeof(ev.prm.reqWakeup.extBootInfo));

  ret = tskm_sockSend(p_svc->connFd, &ev);
  if (ret <= 0) {
    TSKM_ASSERT(0);
    goto ERROR;
  }
  p_svc->waitResCnt++;
  p_svc->state = TSKM_SVC_WAKEUP;

  return TSKM_E_OK;
  ERROR: return TSKM_E_NG;
}

/*********************************************************
 *       Touch services and extend EXIT timeouts
 *********************************************************/
TSKM_STATIC TSKM_ERR_t reqTouch(TSKM_SVC_CTX_t* p_svc) {
#define TSKM_BUF_LEN     ( 4 * ( sizeof(struct inotify_event) ) )  /* read size */
  TSKM_ERR_t funcRet = TSKM_E_NG;
  char touchFileName[32];
  BOOL isNeedRetry = FALSE;

  int iFd = p_svc->iFd;
  int wd = 0;

  TSKM_EVENT_INFO_t ev;
  int ret;
  bzero(&ev, sizeof(ev));

  // Create monitoring files
  tskm_pf_mkTouchFileName(p_svc->pid, touchFileName);
  if (0 != tskm_pf_touch(touchFileName)) {
    goto ERROR;
  }

  // Synchronize by iNortify
  wd = inotify_add_watch(iFd, touchFileName, IN_DELETE_SELF);
  if (wd == -1) {
    TSKM_ASSERT_ERRNO(0);
    goto ERROR;
  }

  // Send Touch request
  ev.event = TSKM_EV_PRI_REQ_TOUCH;
  ev.errCode = TSKM_E_OK;
  ret = tskm_sockSend(p_svc->connFd, &ev);
  if (ret <= 0) {
    // Immediately after the nonresident service process terminates, since the TaskManager service status is other than DORMANT
    // (mainly DOWN), the socket may be discarded and communication may fail, so retry
    funcRet = TSKM_E_RETRY;
    TSKM_PRINTF(TSKM_LOG_WARN, "ret = %d", ret);
    goto ERROR;
  }

  // Wait for Touch completion
  while (1) {
    int maxFd = 0;
    int ret;
    fd_set fds;
    FD_ZERO(&fds);
    struct timeval timeout = { 0 };
    timeout.tv_sec = TSKM_CFG_TOUCH_TIMEOUT;

    FD_SET(iFd, &fds);
    maxFd = iFd;

    ret = select(maxFd + 1, &fds, NULL, NULL, &timeout);
    if (ret == 0) {
      TSKM_ASSERT_PRINT(0, "TIMEOUT:%s", touchFileName);  // Timeout occurs
      isNeedRetry = TRUE;
      break;
    } else if (ret < 1) {
      if (errno == EINTR) {
        continue;
      } else {
        TSKM_ASSERT(0);
        goto ERROR;
      }
    }

    if (FD_ISSET(iFd, &fds)) {
      int length;
      uint8_t buf[TSKM_BUF_LEN];

      length = static_cast<int>(read(iFd, buf, TSKM_BUF_LEN));
      if (length < 0) {
        TSKM_ASSERT_ERRNO(0);
        goto ERROR;
      }
      struct inotify_event *event = (struct inotify_event *) buf;
      if (event->mask & IN_DELETE_SELF) {
        TSKM_PRINTF(TSKM_LOG_STATE, "TouchOK");
        wd = 0;
        // When a file is deleted, the association with the monitoring target is automatically released and inotify_rm_watch is no longer needed.
        break;
      } else {
        TSKM_ASSERT_PRINT(0, "%x", event->mask);
      }
    }
  }

  if (isNeedRetry) {
    funcRet = TSKM_E_RETRY;
  } else {
    funcRet = TSKM_E_OK;
  }

  ERROR: if (wd > 0) {
    TSKM_ASSERT_ERRNO(0 == inotify_rm_watch(iFd, wd))
  }

  if (access(touchFileName, F_OK) == 0) {
    // Timeout care
    unlink(touchFileName);
  }

  return funcRet;
}

/*********************************************************
 *       Availability Monitoring Callbacks for Services
 *********************************************************/
EFrameworkunifiedStatus OnSvcAvailability(HANDLE hApp) {  // LCOV_EXCL_START 6: Because the condition cannot be set
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  PCSTR availabilityName;
  TSKM_MAIN_CTX_t* p_main = tskm_getMainCtx();
  TSKM_SVC_CTX_t* p_svc;

  availabilityName = FrameworkunifiedGetLastNotification(hApp);

  std::string str = availabilityName;

//  unsigned int position;
//  if (std::string::npos != (position = static_cast<unsigned int>(str.find("/Availability", 0)))) {
  ssize_t position;
  if (0 <= (position = str.find("/Availability", 0))) {
    str.erase(position, position + strlen("/Availability"));
  }

  p_svc = getSvcCtxByName(&p_main->svcs, str.c_str());
  if (p_svc) {
    p_svc->isAvailable = FrameworkunifiedIsServiceAvailable(hApp) ? TSKM_TRUE : TSKM_FALSE;
    TSKM_PRINTF(TSKM_LOG_SVCSTATE, "%s Availability %s", p_svc->attr->name,
                (p_svc->isAvailable == TSKM_TRUE) ? "TRUE" : "FALSE");
  }

  return eFrameworkunifiedStatusOK;
}
// LCOV_EXCL_STOP
/*********************************************************
 *       Starting Availability Monitoring of Services
 *********************************************************/
TSKM_STATIC TSKM_ERR_t startWatchAvailability(TSKM_SVC_CTX_t* p_svc) {
  TSKM_ERR_t funcRet = TSKM_E_OK;

  // LCOV_EXCL_BR_START 8: Since the condition is checked by the caller, it is not true.
  if (p_svc->attr->type == TSKM_SVC_TYPE_UNKNONW) {
  // LCOV_EXCL_BR_STOP
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    p_svc->isAvailable = TSKM_TRUE;  // LCOV_EXCL_LINE 8: Since the condition is checked by the caller, it is not true.
  } else {
    EFrameworkunifiedStatus taskmanagerStatus;
    TSKM_MAIN_CTX_t* p_main = tskm_getMainCtx();
    SS_String availabilityName = p_svc->attr->name;

    availabilityName.append("/Availability");

    taskmanagerStatus = FrameworkunifiedSubscribeNotificationWithCallback(p_main->hApp,
                                                     availabilityName.c_str(),
                                                     OnSvcAvailability);
    if (eFrameworkunifiedStatusOK != taskmanagerStatus) {
      TSKM_ASSERT(0);
      funcRet = TSKM_E_NG;
    }
  }

  return funcRet;
}

/*********************************************************
 *       Service startup sub
 *********************************************************/
void svcExec_Process(TSKM_SVC_CTX_t* p_svc) {
  TSKM_ERR_t funcRet;

  if (p_svc->attr->type == TSKM_SVC_TYPE_NATIVE) {
    p_svc->state = TSKM_SVC_WAITCONNECT;
    TSKM_PRINTF(TSKM_LOG_DEBUG, "[ST:%d] WAIT_EXEC", p_svc->pid);
    funcRet = startWatchAvailability(p_svc);
    if (TSKM_E_OK != funcRet) {
      TSKM_ASSERT(0);
      p_svc->isAvailable = TSKM_TRUE;
    }
  } else {
    p_svc->state = TSKM_SVC_RUNNING;
    TSKM_PRINTF(TSKM_LOG_SVCSTATE, "[ST:%d] RUN", p_svc->pid);
    p_svc->isAvailable = TSKM_TRUE;
  }
  return;
}

/*********************************************************
 *       Service startup main
 *********************************************************/
TSKM_STATIC TSKM_ERR_t svcExec(TSKM_SVC_CTX_t* p_svc) {
  TSKM_ERR_t funcRet = TSKM_E_NG;
  pid_t pid;

  if (p_svc == NULL) {  // LCOV_EXCL_BR_LINE 6:double check
    // LCOV_EXCL_START 6: double check
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT(0);
    goto ERROR;
    // LCOV_EXCL_STOP
  }

  if (p_svc->state == TSKM_SVC_DISABLE) {
    // Prohibited startup
    TSKM_ASSERT(0);
    return TSKM_E_STATE;
  } else if (p_svc->state == TSKM_SVC_WAITCONNECT) {
    return TSKM_E_OK;
  } else if (p_svc->state != TSKM_SVC_DORMANT) {
    // Already started
    funcRet = reqTouch(p_svc);
    if (TSKM_E_OK != funcRet) {
      TSKM_PRINTF(TSKM_LOG_WARN, "funcRet = %d", funcRet);
      goto ERROR;
    }
    return TSKM_E_OK;
  }

  pid = tskm_pf_createProc(p_svc->attr);
  if (pid <= 0) {
    TSKM_ASSERT(0);
    goto ERROR;
  }
  TSKM_PRINTF(TSKM_LOG_STATE, "EXEC %s:%d", p_svc->attr->name, pid);

  p_svc->pid = pid;
  p_svc->waitReqCnt = 0;

  // Startup service
  svcExec_Process(p_svc);

  funcRet = TSKM_E_OK;
  ERROR: return funcRet;
}

/*********************************************************
 *       Issuing a start request to the service
 *********************************************************/
TSKM_STATIC TSKM_ERR_t svcWakeupRequest(TSKM_SVC_CTX_t* p_svc,
                                        TSKM_GSTEP_REQ_INFO_t* p_req) {
  if (p_svc == NULL || p_svc->state == TSKM_SVC_DISABLE) {  // LCOV_EXCL_BR_LINE 6: Since this function has always been called after checking // NOLINT(whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return TSKM_E_STATE;  // LCOV_EXCL_LINE 6: Since this function has always been called after checking
  } else if (p_svc->state == TSKM_SVC_WAITCONNECT) {
    // Remember it once and issue a request when the CONNECT completes.
    p_svc->request[p_svc->waitReqCnt] = *p_req;
    p_svc->waitReqCnt++;
  } else {
    TSKM_ERR_t tskmRet;
    tskmRet = wakeupRequest(p_svc, p_req);
    TSKM_ERR_CHK_DFT;
  }

  return TSKM_E_OK;
  ERROR: return TSKM_E_NG;
}

/*********************************************************
 *      Service reboot processing
 *********************************************************/
TSKM_STATIC TSKM_ERR_t svcErrTermPost(TSKM_SVC_CTX_t* p_svc) {
  TSKM_MAIN_CTX_t* p_main = tskm_getMainCtx();

  TSKM_PRINTF(TSKM_LOG_SYSTEMDATA, "ERR TERM SVC(%s:%d)", p_svc->attr->name,
              p_svc->pid);

  if (p_svc->attr->lifeCycle == TSKM_SVC_LC_ALWAYS || (p_svc->errTermCnt > p_svc->attr->retryCnt && (uint32_t) -1 != p_svc->attr->retryCnt)) {  // NOLINT(whitespace/line_length)
    TSKM_ERROR_REBOOT_t rebootInfo;

    TSKM_ASSERT(0);
    memset(&rebootInfo, 0, sizeof(TSKM_ERROR_REBOOT_t));
    rebootInfo.type = TSKM_ERROR_REBOOT_NORMAL;
    snprintf(rebootInfo.log.messageStr, TSKM_LOGGING_MSG_STR_SIZE,
             "TaskManager:SVC ErrTerm");
    tskm_sub_reboot(&rebootInfo);
  } else {
    TSKM_LOGGING_INFO_t logInfo;
    logInfo.type = TSKM_LOGGING_TYPE_MODULE_LOGS;

    snprintf(logInfo.messageStr, TSKM_LOGGING_MSG_STR_SIZE,
             "TaskManager:SVC ErrTerm");
    tskm_sub_logging(&logInfo);

    if (p_svc->attr->lifeCycle == TSKM_SVC_LC_ALWAYS_RECOVERABLE) {
      TSKM_ERR_t ret;
      TSKM_GSTEP_REQ_INFO_t req = { 0 };

      ret = svcExec(p_svc);

      if (TSKM_E_OK != ret) {
        TSKM_ASSERT_PRINT(0, "ret = %d", ret);
        goto ERROR;
      } else if (p_svc->state == TSKM_SVC_WAITCONNECT) {
        // In the state waiting for execution
        req.svcId = p_svc->attr->svcId;
        req.localStep = TSKM_LSTEP_ALL;
        ret = svcWakeupRequest(p_svc, &req);
        if (TSKM_E_OK != ret) {
          TSKM_ASSERT_PRINT(0, "ret = %d", ret);
          goto ERROR;
        }
      }
    }
  }

  return TSKM_E_OK;
  ERROR: return TSKM_E_NG;
}

/*********************************************************
 *      Connection handler
 * ret:TRUE     SVC status changed
 * ret:FALSE    SVC status not changed
 *********************************************************/
TSKM_STATIC TSKM_BOOL_t connectHandle(TSKM_SVC_CTX_t* p_svc,
                                      const TSKM_EVENT_INFO_t* p_inEv) {
  uint32_t ii;

  TSKM_ASSERT(p_svc->state == TSKM_SVC_WAITCONNECT);

  p_svc->connFd = p_inEv->prm.repConnect.connFd;
  p_svc->state = TSKM_SVC_WAKEUP;
  TSKM_PRINTF(TSKM_LOG_DEBUG, "[ST:%d] WAKEUP", p_svc->pid);

  p_svc->waitResCnt = 0;

  if (p_svc->waitReqCnt) {
    for (ii = 0; ii < p_svc->waitReqCnt; ii++) {
      TSKM_ASSERT(TSKM_E_OK == wakeupRequest(p_svc, &p_svc->request[ii]));
    }
    p_svc->waitReqCnt = 0;
  }

  return TSKM_TRUE;
}

/*********************************************************
 *      Disconnection handler
 * ret:TRUE     SVC status changed
 * ret:FALSE    SVC status not changed
 *********************************************************/
TSKM_STATIC TSKM_BOOL_t disConnectHandle(TSKM_SVC_CTX_t* p_svc,
                                         const TSKM_EVENT_INFO_t* p_inEv) {
  // No status change

  return TSKM_TRUE;
}

/*********************************************************
 *      Response handler for the activation request
 * ret:TRUE     SVC status changed
 * ret:FALSE    SVC status not changed
 *********************************************************/
TSKM_STATIC TSKM_BOOL_t resWakeupHandle(TSKM_SVC_CTX_t* p_svc,
                                        const TSKM_EVENT_INFO_t* p_inEv) {
  TSKM_ASSERT(p_svc->state == TSKM_SVC_WAKEUP);
  TSKM_ASSERT(p_svc->waitResCnt > 0);
  TSKM_BOOL_t isStateChg = TSKM_FALSE;

  if (p_svc->isShmDone == TSKM_FALSE && p_inEv->prm.resWakeup.isShmDone) {
    p_svc->isShmDone = TSKM_TRUE;
    isStateChg = TSKM_TRUE;
    TSKM_PRINTF(TSKM_LOG_SVCSTATE, "%s:SHM DONE", p_svc->attr->name);
  }
  if (p_svc->isStepDone == TSKM_FALSE && p_inEv->prm.resWakeup.isStepDone) {
    p_svc->isStepDone = TSKM_TRUE;
    isStateChg = TSKM_TRUE;
    TSKM_PRINTF(TSKM_LOG_SVCSTATE, "%s:STEP DONE", p_svc->attr->name);
  }

  if (p_svc->waitResCnt > 0) {
    p_svc->waitResCnt--;
    if (p_svc->waitResCnt == 0) {  // Transition when the wait runs out
      if (p_inEv->prm.resWakeup.isLast) {
        p_svc->state = TSKM_SVC_RUNNING;  // Startup completed
        TSKM_PRINTF(TSKM_LOG_SVCSTATE, "[ST:%d] RUN", p_svc->pid);
      }
      isStateChg = TSKM_TRUE;
    }
  }
  return isStateChg;
}

/*********************************************************
 *      Response handler for the termination request
 * ret:TRUE     SVC state changed
 * ret:FALSE    SVC state not changed
 *********************************************************/
TSKM_STATIC TSKM_BOOL_t resDownHandle(TSKM_SVC_CTX_t* p_svc,
                                      const TSKM_EVENT_INFO_t* p_inEv) {
  TSKM_ASSERT(p_svc->state == TSKM_SVC_DOWN);
  TSKM_ASSERT(p_svc->waitResCnt > 0);
  TSKM_BOOL_t isStateChg = TSKM_FALSE;

  TSKM_PRINTF(TSKM_LOG_DEBUG, "pid:%d waitCnt:%d", p_svc->pid,
              p_svc->waitResCnt);
  if (p_svc->waitResCnt > 0) {
    p_svc->waitResCnt--;
    if (p_svc->waitResCnt == 0) {  // Transition when the wait runs out
      if (p_inEv->prm.resWakeup.isLast) {
        p_svc->state = TSKM_SVC_FINDOWN;   // Termination complete
        TSKM_PRINTF(TSKM_LOG_SVCSTATE, "[ST:%d] FIN_DOWN", p_svc->pid);
      }
      isStateChg = TSKM_TRUE;
    }
  }
  return isStateChg;
}

/*********************************************************
 *      Response Handlers for DebugDump Requests
 * ret:TRUE     SVC state changed
 * ret:FALSE    SVC state not changed
 *********************************************************/
TSKM_STATIC TSKM_BOOL_t resDebugDumpHandle(TSKM_SVC_CTX_t* p_svc,
                                           const TSKM_EVENT_INFO_t* p_inEv) {
  TSKM_BOOL_t isStateChg = TSKM_FALSE;
  TSKM_EV_PRI_EX_RES_DEBUGDUMP_PRM_t *p_prm;

  if (!p_inEv->hasExtend || !p_inEv->extendPrm) {
    TSKM_ASSERT(0);
    goto ERROR;
  }

  p_prm = (TSKM_EV_PRI_EX_RES_DEBUGDUMP_PRM_t *) p_inEv->extendPrm;  // NOLINT (readability/casting)

  // FALSE is specified here because the required service names have been added in the PrimaryLib.
  tskm_sub_debugDumpRes(FALSE, p_prm->dumpMsg);

  ERROR: return isStateChg;
}

/*********************************************************
 *      Service termination request
 * ret:TRUE     SVC state changed
 * ret:FALSE    SVC state not changed
 *********************************************************/
TSKM_STATIC TSKM_BOOL_t reqExit(TSKM_SVC_CTX_t* p_svc,
                                const TSKM_EVENT_INFO_t* p_inEv) {
  int ret;
  TSKM_ERR_t tskmRet;
  TSKM_ASSERT(p_svc->state == TSKM_SVC_RUNNING);
  TSKM_ASSERT(p_svc->waitResCnt == 0);
  TSKM_GSTEP_REQ_INFO_t req = { 0 };

  req.svcId = p_svc->attr->svcId;
  req.localStep = TSKM_LSTEP_ALL;
  tskmRet = tskm_svcDownRequest(p_svc, &req);
  TSKM_ERR_CHK_DFT;

  return TSKM_TRUE;

  // LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  ERROR: ret = tskm_pf_terminateProcGroup(static_cast<uint16_t>(p_svc->pid));
  if (ret != 0) {  // LCOV_EXCL_BR_LINE 8: dead code
    TSKM_PRINTF(TSKM_LOG_WARN, "ret = %d", ret);
  }

  return TSKM_TRUE;
  // LCOV_EXCL_STOP
}

/*********************************************************
 *      Service termination handler
 * ret:TRUE     SVC state changed
 * ret:FALSE    SVC state not changed
 *********************************************************/
TSKM_STATIC TSKM_BOOL_t repTermHandle(TSKM_SVC_CTX_t* p_svc,
                                      const TSKM_EVENT_INFO_t* p_inEv) {
  int ret;

  // Check error
  if (p_svc->attr->lifeCycle == TSKM_SVC_LC_DYNAMIC) {
    if (p_svc->state != TSKM_SVC_DOWN) {
      // A STATE other than DOWN does not terminate.
      TSKM_PRINTF(TSKM_LOG_ERROR, "ERR TERM %s(%d) waitCnt:%d",
                  p_svc->attr->name, p_svc->pid, p_svc->waitResCnt);
      TSKM_ASSERT(0);
    }
  } else {
    // The resident service terminated.
    TSKM_PRINTF(TSKM_LOG_ERROR, "ERR TERM %s(%d) waitCnt:%d", p_svc->attr->name,
                p_svc->pid, p_svc->waitResCnt);
    TSKM_ASSERT(0);
  }

  TSKM_PRINTF(TSKM_LOG_STATE, "[ST:%d] DORMANT", p_svc->pid);

  p_svc->state = TSKM_SVC_DORMANT;
  p_svc->waitResCnt = 0;
  p_svc->pid = 0;

  if (p_svc->attr->type == TSKM_SVC_TYPE_NATIVE) {
    EFrameworkunifiedStatus taskmanagerStatus;
    TSKM_MAIN_CTX_t* p_main = tskm_getMainCtx();
    SS_String availabilityName = p_svc->attr->name;
    availabilityName.append("/Availability");

    taskmanagerStatus = FrameworkunifiedUnsubscribeNotificationWithCallback(
        p_main->hApp, availabilityName.c_str());
    if (eFrameworkunifiedStatusOK != taskmanagerStatus) {
      TSKM_ASSERT(0);
    }

    p_svc->isAvailable = TSKM_FALSE;
    p_svc->watchCnt = 0;

    HANDLE hMq = McOpenSender(FRAMEWORKUNIFIED_NS_NPSERVICE);
    if (NULL == hMq) {   // LCOV_EXCL_BR_LINE 4: NSFW error case.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      TSKM_ASSERT(0);  // LCOV_EXCL_LINE 4: NSFW error case.
    } else {
      ServiceAvailability availInfo = { };

      snprintf(availInfo.cServiceName, MAX_NAME_SIZE_APP, "%s",
               p_svc->attr->name);
      availInfo.eServiceAvailability = eFrameworkunifiedServiceNotAvailable;

      taskmanagerStatus = NPPublishNotification(hMq, p_svc->attr->name,
                                        availabilityName.c_str(), &availInfo,
                                        sizeof(availInfo));
      if (eFrameworkunifiedStatusOK != taskmanagerStatus) {
        TSKM_ASSERT(0);
      }

      taskmanagerStatus = McClose(hMq);
      if (eFrameworkunifiedStatusOK != taskmanagerStatus) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        TSKM_ASSERT(0);  // LCOV_EXCL_LINE 4: NSFW error case.
      }
    }
  }

  if (p_inEv->errCode != TSKM_E_OK) {
    p_svc->errTermCnt++;
    // Notify 'NG' at Relaunch after abnormal termination
    p_svc->bootInfo.resetStatus = e_SS_SM_RESET_STATUS_NG;

    ret = svcErrTermPost(p_svc);
    if (ret != TSKM_E_OK) {
      TSKM_ASSERT(0);
    }
  } else {
    // NONE is notified at Relaunch after normal completion.
    p_svc->bootInfo.resetStatus = e_SS_SM_RESET_STATUS_NONE;
  }

  return TSKM_TRUE;
}

/*********************************************************
 *       Get service context
 *********************************************************/
TSKM_SVC_CTX_t*
tskm_svcsGetSvcBySvcId(TSKM_SVCS_CTX_t* p_svcs, TSKM_SVCID_t svcId) {
  return getSvcCtxBySvcId(p_svcs, svcId);
}

/*********************************************************
 *       Get service context
 *********************************************************/
TSKM_SVC_CTX_t*
tskm_svcsGetSvcByPid(TSKM_SVCS_CTX_t* p_svcs, pid_t pid) {
  return getSvcCtxByPid(p_svcs, pid);
}

/*********************************************************
 *       Check for waiting services
 *********************************************************/
TSKM_BOOL_t tskm_svcsIsWaiting(TSKM_SVCS_CTX_t* p_svcs) {
  uint32_t ii;
  for (ii = 0; ii < p_svcs->svcNum; ii++) {
    if (p_svcs->svcList[ii].waitResCnt > 0
        || p_svcs->svcList[ii].state == TSKM_SVC_WAITCONNECT) {
      return TSKM_TRUE;
    }
  }
  return TSKM_FALSE;
}

/*********************************************************
 *       Check if ShutdownWait services are terminated
 *********************************************************/
TSKM_SVC_WAIT_STATE_t tskm_svcsGetSvcTermWaitState(TSKM_SVCS_CTX_t* p_svcs) {
  uint32_t ii;
  TSKM_SVC_WAIT_STATE_t waitState = TSKM_SVC_WAIT_NONE;

  for (ii = 0; ii < p_svcs->svcNum; ii++) {
    if (p_svcs->svcList[ii].attr->shotdownWait) {
      // Check shutdownWait SVC Only
      if (p_svcs->svcList[ii].state != TSKM_SVC_DORMANT) {
        // DORMANT is terminated or not started, so no checking is required
        if (TSKM_SVC_LC_DYNAMIC != p_svcs->svcList[ii].attr->lifeCycle
            && p_svcs->svcList[ii].state != TSKM_SVC_FINDOWN) {  // Check of termination of resident SVCs
          waitState = TSKM_SVC_WAIT_BOTH;
          break;
        } else if (TSKM_SVC_LC_DYNAMIC == p_svcs->svcList[ii].attr->lifeCycle) {  // Check of termination of non-resident SVCs
          waitState = TSKM_SVC_WAIT_TRANSIENT;
        }
      }
    }
  }
  return waitState;
}

/*********************************************************
 *       Update boot info of all services
 *********************************************************/
TSKM_ERR_t tskm_svcsSetBootInfo(TSKM_SVCS_CTX_t* p_svcs,
                                T_SS_SM_START_DataStructType* p_info,
                                T_SS_SM_START_ExtDataStructType *p_exInfo) {
  uint32_t ii = 0;

  for (ii = 0; ii < p_svcs->svcNum; ii++) {
    p_svcs->svcList[ii].bootInfo = *p_info;
    p_svcs->svcList[ii].extBootInfo = *p_exInfo;
  }
  return TSKM_E_OK;
}

/*********************************************************
 *       Terminates a running non-resident services
 *********************************************************/
TSKM_ERR_t tskm_svcsAvtiveSvcTerm(TSKM_SVCS_CTX_t* p_svcs) {
  uint32_t ii;

  for (ii = 0; ii < p_svcs->svcNum; ii++) {
    TSKM_SVC_CTX_t* p_svc = &p_svcs->svcList[ii];

    if (TSKM_SVC_LC_DYNAMIC == p_svc->attr->lifeCycle &&              // Non-resident SVC
        TSKM_SVC_RUNNING == p_svc->state && p_svc->waitResCnt == 0) {  // Running
      TSKM_ERR_t tskmRet;
      TSKM_GSTEP_REQ_INFO_t req = { 0 };

      req.svcId = p_svc->attr->svcId;
      req.localStep = TSKM_LSTEP_ALL;
      tskmRet = tskm_svcDownRequest(p_svc, &req);
      TSKM_ERR_CHK_DFT;
    }
  }

  return TSKM_E_OK;

  ERROR:
  return TSKM_E_NG;
}

/*********************************************************
 *   Calls back the DebugDump of the running services
 *********************************************************/
TSKM_ERR_t tskm_svcsCallDebugDump(TSKM_SVCS_CTX_t* p_svcs) {
  for (uint32_t ii = 0; ii < p_svcs->svcNum; ii++) {
    TSKM_SVC_CTX_t* p_svc = &p_svcs->svcList[ii];

    if (TSKM_SVC_RUNNING == p_svc->state) {  // Running
      TSKM_EVENT_INFO_t ev;
      bzero(&ev, sizeof(ev));

      // Send DebugDump request
      ev.event = TSKM_EV_PRI_REQ_DEBUGDUMP;
      ev.errCode = TSKM_E_OK;
      if (0 >= tskm_sockSend(p_svc->connFd, &ev)) {
        TSKM_ASSERT(0);
        goto ERROR;
      }
    }
  }

  return TSKM_E_OK;
  ERROR: return TSKM_E_NG;
}

/*********************************************************
 *   Calls back the LowMemory of the running services
 *********************************************************/
TSKM_ERR_t tskm_svcsCallLowMem(TSKM_SVCS_CTX_t* p_svcs) {
  for (uint32_t ii = 0; ii < p_svcs->svcNum; ii++) {
    TSKM_SVC_CTX_t* p_svc = &p_svcs->svcList[ii];
    if (TSKM_SVC_RUNNING == p_svc->state) {  // Running
      TSKM_EVENT_INFO_t ev;
      bzero(&ev, sizeof(ev));

      // Send LowMemory detection notification
      ev.event = TSKM_EV_PRI_REP_LOWMEM;
      ev.errCode = TSKM_E_OK;
      if (0 >= tskm_sockSend(p_svc->connFd, &ev)) {
        TSKM_ASSERT(0);
        goto ERROR;
      }
    }
  }

  return TSKM_E_OK;
  ERROR: return TSKM_E_NG;
}

/*********************************************************
 *      Event handler
 *********************************************************/
TSKM_ERR_t tskm_svcEventHandle(TSKM_SVC_CTX_t* p_svc, TSKM_EVENT_INFO_t* p_ev) {
  TSKM_FUNC_IN();
  TSKM_BOOL_t isStateChg = TSKM_FALSE;

  if (isSvcEvent(p_ev) == TSKM_FALSE) {
    // If it is not an event for the service, it will be sent as follows.
    TSKM_FUNC_OUT();
    return TSKM_E_OK;
  }

  switch (p_ev->event) {
    case TSKM_EV_PRI_REP_CONNECT:   // Connection Registration from Service
      isStateChg = connectHandle(p_svc, p_ev);
      break;
    case TSKM_EV_PRI_REP_DISCONNECT:   // Disconnection Registration from Service
      isStateChg = disConnectHandle(p_svc, p_ev);
      break;
    case TSKM_EV_PRI_RES_WAKEUP:    // Response to a startup request from a service
      isStateChg = resWakeupHandle(p_svc, p_ev);
      break;
    case TSKM_EV_PRI_RES_DOWN:
      isStateChg = resDownHandle(p_svc, p_ev);
      break;
    case TSKM_EV_PRI_RES_DEBUGDUMP:
      isStateChg = resDebugDumpHandle(p_svc, p_ev);
      break;
    case TSKM_EV_PRI_REQ_EXIT:
      isStateChg = reqExit(p_svc, p_ev);
      break;
    case TSKM_EV_SVC_REP_TERM:      // Service termination
      isStateChg = repTermHandle(p_svc, p_ev);
      break;
    default:
      break;
  }

  // Overwrite service state change
  if (isStateChg) {
    p_ev->event = TSKM_EV_LCL_CHG_SVC_STATE;
    p_ev->prm.chgSvc.svcId = p_svc->attr->svcId;
  } else {
    p_ev->event = TSKM_EV_NOP;
  }

  TSKM_FUNC_OUT();
  return TSKM_E_OK;
}
/*********************************************************
 *       Startup service
 *********************************************************/
TSKM_ERR_t tskm_svcExec(TSKM_SVC_CTX_t* p_svc) {
  return svcExec(p_svc);
}

/*********************************************************
 *       Issue a startup request to the service
 *********************************************************/
TSKM_ERR_t tskm_svcWakeupRequest(TSKM_SVC_CTX_t* p_svc,
                                 TSKM_GSTEP_REQ_INFO_t* p_req) {
  return svcWakeupRequest(p_svc, p_req);
}

/*********************************************************
 *       Issue a termination request to the service
 *********************************************************/
TSKM_ERR_t tskm_svcDownRequest(TSKM_SVC_CTX_t* p_svc,
                               TSKM_GSTEP_REQ_INFO_t* p_req) {
  int ret;
  TSKM_EVENT_INFO_t ev;

  bzero(&ev, sizeof(ev));

  if (tskm_svcIsCommunicatable(p_svc) == TSKM_FALSE) {
    return TSKM_E_OK;
  }

  ev.event = TSKM_EV_PRI_REQ_DOWN;
  ev.errCode = TSKM_E_OK;
  ev.prm.reqDown.localStep = p_req->localStep;

  ret = tskm_sockSend(p_svc->connFd, &ev);
  if (ret <= 0) {
    TSKM_ASSERT(0);
    goto ERROR;
  }
  p_svc->waitResCnt++;
  p_svc->state = TSKM_SVC_DOWN;

  return TSKM_E_OK;
  ERROR: return TSKM_E_NG;
}

/*********************************************************
 *       Prohibit starting service
 *********************************************************/
TSKM_ERR_t tskm_svcDisableRequest(TSKM_SVC_CTX_t* p_svc) {
  if (p_svc->state == TSKM_SVC_DORMANT || p_svc->state == TSKM_SVC_DISABLE) {
    p_svc->state = TSKM_SVC_DISABLE;
    return TSKM_E_OK;
  }
  return TSKM_E_STATE;
}

/*********************************************************
 *       Allow starting service
 *********************************************************/
TSKM_ERR_t tskm_svcEnableRequest(TSKM_SVC_CTX_t* p_svc) {
  if (p_svc->state == TSKM_SVC_DISABLE) {
    p_svc->state = TSKM_SVC_DORMANT;
  }
  return TSKM_E_OK;
}

/*********************************************************
 *      Queriy whether the service is ready for communication
 *********************************************************/
TSKM_BOOL_t tskm_svcIsCommunicatable(TSKM_SVC_CTX_t* p_svc) {
  TSKM_BOOL_t ret = TSKM_FALSE;
  if (p_svc == NULL || p_svc->attr->type == TSKM_SVC_TYPE_UNKNONW) {
  } else {
    switch (p_svc->state) {
      case TSKM_SVC_WAKEUP:
      case TSKM_SVC_RUNNING:
      case TSKM_SVC_DOWN:
        ret = TSKM_TRUE;
        break;
      default:
        break;
    }
  }
  return ret;
}  // LCOV_EXCL_BR_LINE 10: Final line

