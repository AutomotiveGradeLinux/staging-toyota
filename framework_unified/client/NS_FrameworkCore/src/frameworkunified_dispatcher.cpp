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

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NSFramework
/// \brief
///
///
///
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <pthread.h>

#include <native_service/ns_logger_if.h>
#include <native_service/ns_version_if.h>
#include <native_service/ns_message_center_if.h>
#include <native_service/frameworkunified_framework_sync.h>
#include <native_service/frameworkunified_sm_hsmframework.h>
#include <native_service/frameworkunified_sm_framework_dispatch.h>
#include <native_service/frameworkunified_thread_priority.h>
#include <native_service/ns_plogger_if.h>
#include <native_service/ns_resource_controler.h>
#include <native_service/frameworkunified_service_protocol.h>
#include <native_service/frameworkunified_dispatcher.h>
#include <native_service/frameworkunified_framework_if.h>
#include <other_service/strlcpy.h>

#include <string>
#include <utility>
#include <map>

#include "frameworkunified_framework_core.h"
#include "frameworkunified_framework_internal.h"
#include "ns_message_center_internal.h"
#include "ns_mc_internal.h"
#include "frameworkunified_framework_error_internal.hpp"
#include "frameworkunified_framework_utility.h"
#include "frameworkunified_sm_framework_core.h"

#ifdef DISPATCHER_PROFILER
#include "frameworkunified_msgprofiler.h"
#include <boost/bind.hpp>
#endif

/*
 * static variable
 */
static FrameworkunifiedDefaultCallbackHandler g_defaultCallbackHandler;

//////////////////////////
// prototype declaration
//////////////////////////
static EFrameworkunifiedStatus GetDispatcherMqFD(HANDLE hApp, int *fd);
static EFrameworkunifiedStatus CreateMultiWaitingFD(HANDLE hApp);
static EFrameworkunifiedStatus ProcessMq(HANDLE hApp);
static EFrameworkunifiedStatus ProcessFd(HANDLE hApp , int fd);

//////////////////////////////////////////
// Function : ostream &operator<<
//////////////////////////////////////////
template <typename K, typename V, class C, class A>
std::ostream &operator<< (std::ostream &os, const std::map<K, V, C, A> &m) {
  os << "{ ";
  typename std::map<K, V, C, A>::const_iterator p;
  for (p = m.begin(); p != m.end(); ++p) {
    os << p->first << ":" << p->second << ", ";
  }
  return os << "}";
}

EFrameworkunifiedStatus CreateDispatcher(PCSTR cAppName, HANDLE &hApp, BOOL bIsChildThread) {  // NOLINT (readability/nolint)
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  CFrameworkunifiedFrameworkApp *pApp = new CFrameworkunifiedFrameworkApp;  // LCOV_EXCL_BR_LINE 11:except branch
  if (pApp != NULL) {  // LCOV_EXCL_BR_LINE 5:fail safe for new
    pApp->checkCode = APP_CHECK_CODE;
    pApp->self = pthread_self();
    // app name size should not exceed the max length
    memset(pApp->cAppName, 0, MAX_NAME_SIZE_APP);
    memset(pApp->sServiceAvailabilityNotification, 0, MAX_SYS_INFO_SIZE);
    pApp->bIsAvailable = FALSE;
    /*
     * todo
     * Setting NULL for arguments causes an Exception
     */
    if (strlen(cAppName) < MAX_NAME_SIZE_APP) {
      memcpy(pApp->cAppName, cAppName, strlen(cAppName));
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, " Error : Aborting ... Exceeds Max App name size MAX_NAME_SIZE_APP : %d ",
             MAX_NAME_SIZE_APP);
      /*
       * Error is set here, but the error is overwritten by the following process
       */
      eStatus = eFrameworkunifiedStatusFail;
    }

    FRAMEWORKUNIFIEDLOG0(ZONE_NS_DIS, __FUNCTION__, " Application Created ");

    // opening handle to receive message for the application
    pApp->hAppRcvMsgQ = NULL;
    if (NULL == (pApp->hAppRcvMsgQ = McOpenReceiver(pApp->cAppName))) {
      FRAMEWORKUNIFIEDLOG0(ZONE_NS_ERR, __FUNCTION__, " Error : Failed to McOpenReceiver ");
      eStatus = eFrameworkunifiedStatusNullPointer;
    } else {
      if (strcmp(cAppName, "NS_NPPService") != 0) {
        McFlushReceiver(pApp->hAppRcvMsgQ);
      }
    }

    // opening socket for abnormal monitoring
    pApp->siMonitorSock = -1;
    if ((eFrameworkunifiedStatusOK == eStatus) && (-1 == (pApp->siMonitorSock = mcOpenMonitor(pApp->cAppName)))) {
      FRAMEWORKUNIFIEDLOG0(ZONE_NS_ERR, __FUNCTION__, " Error : Failed to mcOpenMonitor ");
      eStatus = eFrameworkunifiedStatusFail;
      pApp->siMonitorSock = -1;
    }

    // opening handle to send messages to itself
    pApp->hAppSndMsgQ = NULL;
    if ((eFrameworkunifiedStatusOK == eStatus) && (NULL == (pApp->hAppSndMsgQ = McOpenSender(pApp->cAppName)))) {
      // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
      FRAMEWORKUNIFIEDLOG0(ZONE_NS_ERR, __FUNCTION__, " Error : Failed to McOpenSender ");
      // LCOV_EXCL_BR_STOP
      eStatus = eFrameworkunifiedStatusNullPointer;
    }

    // opening handles for Notification Persistence Service and Service Directory
    pApp->hNPSndMsgQ = NULL;
    if ((eFrameworkunifiedStatusOK == eStatus) &&
        (NULL == (pApp->hNPSndMsgQ = FrameworkunifiedMcOpenSender(reinterpret_cast<HANDLE>(pApp), FRAMEWORKUNIFIED_NS_NPSERVICE)))) {
      // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, " Error : Failed to FrameworkunifiedMcOpenSender < NS_NPPService >");
      // LCOV_EXCL_BR_STOP
      eStatus = eFrameworkunifiedStatusNullPointer;
    }

    // stores the message queue handle of the parent thread.
    // It will set after the dispatcher of child thread is created. And will be NULL for main thread.
    pApp->hParentSndMsgQ = NULL;

    if ((eFrameworkunifiedStatusOK == eStatus) && (!bIsChildThread)) {
      // Define any system callback's with the dispatcher.
      if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedAttachSystemCallbacksDispatcher(reinterpret_cast<HANDLE>(pApp)))) {
        // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, " Error :: (%d) Failed to FrameworkunifiedAttachSystemCallbacksDispatcher ", eStatus);
        // LCOV_EXCL_BR_STOP
      }

      if (eFrameworkunifiedStatusOK == eStatus) {
        // Define the logger callback's for setting up gets and sets with the dispatcher.
        if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedAttachLoggerCallbacksDispatcher(reinterpret_cast<HANDLE>(pApp)))) {
          // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
          FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, " Error :: (%d) Failed to FrameworkunifiedAttachLoggerCallbacksDispatcher ", eStatus);
          // LCOV_EXCL_BR_STOP
        }
      }
    }

    // initialize
    pApp->m_pFrameworkunifiedStateMachine = NULL;
    pApp->uiProtocolCmd = 0;

    // Clear the Defer'ed message pop flag.
    pApp->fPopDeferedMsg = FALSE;

    // Clear the Defer'ed sync response flag.
    pApp->fDeferredSyncResponse = FALSE;

    // buffer initialize
    memset(pApp->uiMsgRcvBuffer, 0, MAX_MSGQ_BUFFER);
    memset(pApp->cMsgSrcName, '\0', MAX_NAME_SIZE_APP);
    memset(pApp->cSystemInfo, '\0', MAX_SYS_INFO_SIZE);

    pApp->defer_evfd = -1;
    pApp->efd = -1;
    if (eFrameworkunifiedStatusOK == eStatus) {
      eStatus = CreateMultiWaitingFD((HANDLE)pApp);
      if (eFrameworkunifiedStatusOK != eStatus) {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : CreateMultiWaitingFD Failed, status=%d", eStatus);
      }
    }

#ifdef DISPATCHER_PROFILER
    // Assign default value
    pApp->m_pFrameworkunifiedMsgProfiler = NULL;
#endif

    if (eFrameworkunifiedStatusOK != eStatus) {
      if (NULL != pApp->hAppRcvMsgQ) {
        McClose(pApp->hAppRcvMsgQ);
        pApp->hAppRcvMsgQ = NULL;
      }

      if (0 < pApp->siMonitorSock) {
        mcCloseMonitor(pApp->cAppName);
        pApp->siMonitorSock = -1;
      }

      if (NULL != pApp->hAppSndMsgQ) {
        McClose(pApp->hAppSndMsgQ);
        pApp->hAppSndMsgQ = NULL;
      }

      if (NULL != pApp->hNPSndMsgQ) {
        FrameworkunifiedMcClose(pApp->hNPSndMsgQ);
        pApp->hNPSndMsgQ = NULL;
      }

      // LCOV_EXCL_BR_START 6:According to line 158, pApp->hParentSndMsgQ is NULL.
      if (NULL != pApp->hParentSndMsgQ) {
      // LCOV_EXCL_BR_STOP
        // LCOV_EXCL_START 6:According to line 158, pApp->hParentSndMsgQ is NULL.
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        McClose(pApp->hParentSndMsgQ);
        pApp->hParentSndMsgQ = NULL;
        // LCOV_EXCL_STOP
      }

      if (0 < pApp->defer_evfd) {
        close(pApp->defer_evfd);
        pApp->defer_evfd = -1;
      }

      if (0 < pApp->efd) {
        close(pApp->efd);
        pApp->efd = -1;
      }

      delete pApp;  // LCOV_EXCL_BR_LINE 11:except branch
      pApp = NULL;
    }
  }

  hApp = pApp;

  return eStatus;
}


//////////////////////////////////////////
// Function : FrameworkunifiedCreateDispatcher
//////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedCreateDispatcher(PCSTR cAppName, HANDLE &hApp, BOOL bIsChildThread) {  // NOLINT (readability/nolint)
  return CreateDispatcher(cAppName, hApp, bIsChildThread);
}


//////////////////////////////////////////
// Function : FrameworkunifiedDispatchBlock
//////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedDispatchBlock(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus     = eFrameworkunifiedStatusOK;
//  FRAMEWORKUNIFIEDLOG0(ZONE_NS_DIS, __FUNCTION__, "+");

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

    if (eFrameworkunifiedStatusOK != (eStatus = McReceiveWithSession(pApp->hAppRcvMsgQ, pApp->cMsgSrcName,
                                                        &pApp->uiProtocolCmd, &pApp->uiSessionId, MAX_MSGQ_BUFFER,
                                                        (PVOID)(pApp->uiMsgRcvBuffer)))) {
      // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error :: (%d) Failed on McReceive", eStatus);
      // LCOV_EXCL_BR_STOP
    } else {
//      FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, "Message received on dispatcher:: Source: %s, Cmd: 0x%X, SessionId: %d ",
//             pApp->cMsgSrcName, pApp->uiProtocolCmd, pApp->uiSessionId);
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

//  FRAMEWORKUNIFIEDLOG0(ZONE_NS_DIS, __FUNCTION__, "-");
  return eStatus;
}


EFrameworkunifiedStatus FrameworkunifiedProcessServiceProtocolCallback(HANDLE hApp, PCSTR pRequester) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    Services::iterator s_iterator;

    s_iterator = pApp->services.find(pRequester);
    if (s_iterator != pApp->services.end()) {
      SessionTable::iterator session_iterator;
      session_iterator = (s_iterator->second).find(pApp->uiSessionId);
      if (session_iterator != (s_iterator->second).end()) {
        //    FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, " %s found :: (%s)  ", pApp->cMsgSrcName, s_iterator->second);

        // find the protocol req/ack handler
        ServiceProtocolTable::iterator spt_iterator;
        spt_iterator = (session_iterator->second).find(pApp->uiProtocolCmd);
        if (spt_iterator != (session_iterator->second).end()) {
          // calling the protocol req/ack handler
          eStatus = (*spt_iterator).second(hApp);
        } else if ((session_iterator->second).end() != (spt_iterator =
          (session_iterator->second).find(FRAMEWORKUNIFIED_ANY_COMMAND))) {
          // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
          FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__,
                 "Processing callback function for (FRAMEWORKUNIFIED_ANY_COMMAND). "
                 "Dispatcher protocol command: (0x%x)", pApp->uiProtocolCmd);
          // LCOV_EXCL_BR_STOP
          eStatus = (*spt_iterator).second(hApp);
        } else {
          eStatus = eFrameworkunifiedStatusMsgNotProcessed;
        }
      } else {
        eStatus = eFrameworkunifiedStatusMsgNotProcessed;
      }
    } else {
      eStatus = eFrameworkunifiedStatusMsgNotProcessed;
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  return eStatus;
}

EFrameworkunifiedStatus FrameworkunifiedProcessServiceProtocol(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

    // Check if the callback is associated with the Protocol for message source
    eStatus = FrameworkunifiedProcessServiceProtocolCallback(hApp, const_cast<PCSTR>(pApp->cMsgSrcName));

    if (eFrameworkunifiedStatusMsgNotProcessed == eStatus) {
      // Check if the callback is associated with the Protocol for FRAMEWORKUNIFIED_ANY_SOURCE
      eStatus = FrameworkunifiedProcessServiceProtocolCallback(hApp, FRAMEWORKUNIFIED_ANY_SOURCE);
      if (eFrameworkunifiedStatusMsgNotProcessed == eStatus) {
        if (pApp->m_pFrameworkunifiedStateMachine) {
          // Check if the HSM event is associated with the Protocol for message source
          eStatus = FrameworkunifiedProcessServiceProtocolHSMEvent(hApp, const_cast<PCSTR>(pApp->cMsgSrcName));

          if (eFrameworkunifiedStatusMsgNotProcessed == eStatus) {
            // Check if the HSM event is associated with the Protocol for FRAMEWORKUNIFIED_ANY_SOURCE
            eStatus = FrameworkunifiedProcessServiceProtocolHSMEvent(hApp, FRAMEWORKUNIFIED_ANY_SOURCE);
          }
        }
      }
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  return eStatus;
}


EFrameworkunifiedStatus FrameworkunifiedSetDeferredSyncResponse(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusInvldBuf;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    pApp->fDeferredSyncResponse = TRUE;
    eStatus = eFrameworkunifiedStatusOK;
  }

  return eStatus;
}


static void frameworkunifiedProcessSyncResponse(HANDLE hApp, EFrameworkunifiedStatus retValue) {
  EFrameworkunifiedStatus eStatus;
  CHAR systemInfo[MAX_SYS_INFO_SIZE];
  UI_32 seq_id;
  CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

  if (frameworkunifiedGetIsTypeOfSync(hApp) == TRUE) {
    if (pApp->fDeferredSyncResponse == TRUE) {
      if (pApp->responsedata.size() != 0) {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, "Response data is ignored for "
          ":: source: %s, Protocol Command: 0x%x, Session Id: %u.",
               pApp->cMsgSrcName != 0 ? pApp->cMsgSrcName : NULL,
               pApp->uiProtocolCmd, pApp->uiSessionId);
        pApp->responsedata.clear();
      }
    } else {
      FrameworkunifiedGetSystemInfo(hApp, systemInfo);
      seq_id = (UI_32)strtoul(systemInfo, NULL, 16);

      eStatus = frameworkunifiedSendSyncResponse(hApp, PROTOCOL_FRAMEWORKUNIFIED_ANY_COMMAND, seq_id, retValue, pApp->responsedata.size(),
                                    pApp->responsedata.data());
      if (eStatus != eFrameworkunifiedStatusOK) {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "retValu:%d errno:%d", eStatus, errno);
      }
      pApp->responsedata.clear();
    }
  } else {
    if (pApp->responsedata.size() != 0) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, "Response data is ignored for :: "
        "source: %s, Protocol Command: 0x%x, Session Id: %u.",
             pApp->cMsgSrcName != 0 ? pApp->cMsgSrcName : NULL,
             pApp->uiProtocolCmd, pApp->uiSessionId);
      pApp->responsedata.clear();
    }
  }
  pApp->fDeferredSyncResponse = FALSE;
}


inline static time_t frameworkunifiedTimeval2usec(const struct timeval &time) {
  return static_cast<time_t>(time.tv_sec * 1000000 + time.tv_usec);
}


static void frameworkunifiedPrintRusage(HANDLE hApp, const struct rusage &start, const struct rusage &end) {
  UI_32 occupy_time;
  UI_32 average;
  CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

  occupy_time = static_cast<UI_32>(frameworkunifiedTimeval2usec(end.ru_utime) - frameworkunifiedTimeval2usec(start.ru_utime));
  occupy_time += static_cast<UI_32>(frameworkunifiedTimeval2usec(end.ru_stime) - frameworkunifiedTimeval2usec(start.ru_stime));

  pApp->total_occupy_time += occupy_time;
  pApp->total_run_count++;
  average = static_cast<UI_32>(pApp->total_occupy_time / pApp->total_run_count);

  if (occupy_time > pApp->max_occupy_time) {
    pApp->max_occupy_time = occupy_time;
  }

//  FRAMEWORKUNIFIEDLOG(ZONE_NS_DEBUG_DUMP, __FUNCTION__, "cur:%d.%06d avg:%d.%06d max:%d.%06d",
//         occupy_time / 1000000, occupy_time % 1000000,
//         average / 1000000, average % 1000000,
//         pApp->max_occupy_time / 1000000, pApp->max_occupy_time % 1000000);
}


static inline void frameworkunifiedGetRusage(struct rusage *r) {
  if (r == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "r is NULL");
    return;
  }
  memset(r, 0, sizeof(struct rusage));
  if (getrusage(RUSAGE_THREAD, r) == -1) {
    // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "getrusage errno:%d", errno);
    // LCOV_EXCL_BR_STOP
  }
}

static int frameworkunifiedPushLostSessionInfo(HANDLE hApp, PCSTR serviceName) {
  int cntFound = 0;
  CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

  ServiceSessionTable::iterator s_iterator = pApp->sessiontable.find(serviceName);
  if (s_iterator != pApp->sessiontable.end()) {
    // LCOV_EXCL_BR_START 11:except branch
    SessionHandleTable::iterator sh_iterator = pApp->sessiontable[serviceName].begin();
    // LCOV_EXCL_BR_STOP
    while (sh_iterator != pApp->sessiontable[serviceName].end()) {
      LostSessionInfo l_info;
      l_info.sServiceName = serviceName;
      l_info.iSessionId = sh_iterator->first;
      pApp->lostSessionQueue.push(l_info);
      cntFound++;
      sh_iterator++;
    }
  }
  return cntFound;
}

///
/// Abnormal monitor(server): Client socket close handle
///
EFrameworkunifiedStatus frameworkunifiedProcessClientAbnormalState(HANDLE hApp, int sockfd) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
  AcceptMonitorTable::iterator it;

  if ((it = pApp->acceptMonitorTable.find(sockfd)) == pApp->acceptMonitorTable.end()) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : %d, Invalid fd", sockfd);
  } else {
    char clientName[16];
    if (mcGetClientNameFromConnectKey((it->second).c_str(), clientName, sizeof(clientName)) < 0) {
      eStatus = eFrameworkunifiedStatusFail;
      // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : Invlid connectKey:%s", (it->second).c_str());
      // LCOV_EXCL_BR_STOP
    } else {
      if (pApp->lostSessionHandler != NULL) {
        int cntFound = frameworkunifiedPushLostSessionInfo(hApp, clientName);
        if (cntFound) {
          // calling lost session handler
          try {
            eStatus = pApp->lostSessionHandler(hApp);
          } catch (const std::exception &e) {
            FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Recovering from: %s", e.what());
            eStatus = eFrameworkunifiedStatusErrOther;
          }
        }
      }
    }
    pApp->acceptMonitorTable.erase(sockfd);
  }

  return eStatus;  // LCOV_EXCL_BR_LINE 11:except branch
}

///
/// Abnormal monitor(server): Add
///
EFrameworkunifiedStatus frameworkunifiedAddAcceptMonitor(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
  int acceptSock;

  if ((acceptSock = mcGetAcceptMonitor(pApp->cAppName, pApp->cMsgSrcName)) < 0) {
    // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : mcGetAcceptMonitor srv:%s clnt:%s",
           pApp->cAppName != 0 ? pApp->cAppName : NULL, pApp->cMsgSrcName != 0 ? pApp->cMsgSrcName : NULL);
    // LCOV_EXCL_BR_STOP
    eStatus = eFrameworkunifiedStatusFail;
  } else {
    char connectKey[32];
    mcSetConnectKey(connectKey, sizeof(connectKey), pApp->cAppName, pApp->cMsgSrcName);
    pApp->acceptMonitorTable.insert(std::make_pair(acceptSock, connectKey));
  }

  return eStatus;
}

///
/// Abnormal monitor(server): Del
///
EFrameworkunifiedStatus frameworkunifiedDelAcceptMonitor(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
  long acceptSock;  // NOLINT  (readability/nolint)

  char connectKey[32];
  mcSetConnectKey(connectKey, sizeof(connectKey), pApp->cAppName, pApp->cMsgSrcName);
  if (frameworkunifiedGetResource(FRAMEWORKUNIFIED_RES_ABNMLMON, connectKey, &acceptSock) < 0) {
    eStatus = eFrameworkunifiedStatusFail;
  } else {
    if (pApp->acceptMonitorTable.find(static_cast<Socket>(acceptSock)) != pApp->acceptMonitorTable.end()) {
      pApp->acceptMonitorTable.erase(static_cast<Socket>(acceptSock));
    }
  }

  return eStatus;
}

///
/// Abnormal monitor(client): Server socket close handle
///
EFrameworkunifiedStatus frameworkunifiedProcessServerAbnormalState(HANDLE hApp, int sockfd) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
  ConnectMonitorTable::iterator it;

  if ((it = pApp->connectMonitorTable.find(sockfd)) == pApp->connectMonitorTable.end()) {
    // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : %d, Invalid fd", sockfd);
    // LCOV_EXCL_BR_STOP
  } else {
    char serverName[16];
    if (mcGetServerNameFromConnectKey((it->second).c_str(), serverName, sizeof(serverName)) < 0) {
      eStatus = eFrameworkunifiedStatusFail;
      // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : Invlid connectKey:%s", (it->second).c_str());
      // LCOV_EXCL_BR_STOP
    } else {
      if (pApp->lostSessionHandler != NULL) {
        int cntFound = frameworkunifiedPushLostSessionInfo(hApp, serverName);  // LCOV_EXCL_BR_LINE 11:except branch
        if (cntFound) {
          // calling lost session handler
          try {
            eStatus = pApp->lostSessionHandler(hApp);
          } catch (const std::exception &e) {
            // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
            FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Recovering from: %s", e.what());
            // LCOV_EXCL_BR_STOP
            eStatus = eFrameworkunifiedStatusErrOther;
          }
        }
      }
    }
    pApp->connectMonitorTable.erase(sockfd);
  }

  return eStatus;  // LCOV_EXCL_BR_LINE 11:except branch
}

///
/// Abnormal monitor(client): Add
///
EFrameworkunifiedStatus frameworkunifiedAddConnectMonitor(HANDLE hApp, PCSTR serverName, PCSTR clientName) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

  int connectSock = mcConnectMonitor(serverName, clientName);
  if (connectSock < 0) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error::%s:%s mcConnectMonitor",
           serverName != 0 ? serverName : NULL, pApp->cAppName != 0 ? pApp->cAppName : NULL);
    eStatus = eFrameworkunifiedStatusFail;
  } else {
    if (pApp->connectMonitorTable.find(connectSock) == pApp->connectMonitorTable.end()) {
      char connectKey[32];
      mcSetConnectKey(connectKey, sizeof(connectKey), serverName, clientName);

      struct epoll_event event;
      event.events = EPOLLRDHUP;
      event.data.fd = connectSock;

      if (epoll_ctl(pApp->efd, EPOLL_CTL_ADD, connectSock, &event) < 0) {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : epoll_ctl:%s", strerror(errno));
        eStatus = eFrameworkunifiedStatusFail;
      }

      if (eStatus == eFrameworkunifiedStatusOK) {
        CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
        pApp->connectMonitorTable.insert(std::make_pair(connectSock, connectKey));
      }
    }
  }

  return eStatus;
}

///
/// Abnormal monitor(client): Del
///
EFrameworkunifiedStatus frameworkunifiedDelConnectMonitor(HANDLE hApp, PCSTR serverName, PCSTR clientName) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

  char connectKey[32];
  mcSetConnectKey(connectKey, sizeof(connectKey), serverName, clientName);

  long connectSock;  // NOLINT  (readability/nolint)
  if (frameworkunifiedGetResource(FRAMEWORKUNIFIED_RES_ABNMLMON, connectKey, &connectSock) < 0) {
    eStatus = eFrameworkunifiedStatusFail;
  } else {
    if (pApp->connectMonitorTable.find(static_cast<Socket>(connectSock)) != pApp->connectMonitorTable.end()) {
      pApp->connectMonitorTable.erase(static_cast<Socket>(connectSock));
    }

    if (epoll_ctl(pApp->efd, EPOLL_CTL_DEL, static_cast<int>(connectSock), NULL) < 0) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : epoll_ctl:%s", strerror(errno));
      eStatus = eFrameworkunifiedStatusFail;
    }
  }

  return eStatus;
}

///
/// Abnormal monitor(common): Attach callback
///
EFrameworkunifiedStatus FrameworkunifiedAttachLostSessionCallbackToDispatcher(HANDLE hApp, CbFuncPtr fpLostSession) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) == FALSE || fpLostSession == NULL) {
    eStatus = eFrameworkunifiedStatusInvldParam;
  } else {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    pApp->lostSessionHandler = fpLostSession;
  }

  return eStatus;
}

///
/// Abnormal monitor(common): Get lost session data
///
EFrameworkunifiedStatus FrameworkunifiedGetLostSessionData(HANDLE hApp, PSTR pServiceName, PUI_32 puiSessionId) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) == FALSE || pServiceName == NULL || puiSessionId == NULL) {
    eStatus = eFrameworkunifiedStatusInvldParam;
  } else {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    if (pApp->lostSessionQueue.empty()) {
      eStatus = eFrameworkunifiedStatusAccessError;
    } else {
      LostSessionInfo info = pApp->lostSessionQueue.front();
      pApp->lostSessionQueue.pop();
      strcpy(pServiceName, info.sServiceName.c_str());  // NOLINT  (readability/nolint)
      *puiSessionId = info.iSessionId;
    }
  }

  return eStatus;
}


//////////////////////////////////////////
// Function : FrameworkunifiedDispatchProcess
//////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedDispatchProcess(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus     = eFrameworkunifiedStatusOK;
  // FRAMEWORKUNIFIEDLOG0(ZONE_NS_DIS, __FUNCTION__, "+");

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    struct rusage start, end;
    BOOL enable_rusage = NsLogIsZoneSet(ZONE_NS_DEBUG_DUMP);

    if (enable_rusage == TRUE) {
      frameworkunifiedGetRusage(&start);
    }

    if (0 != strcmp(FRAMEWORKUNIFIED_NS_NPSERVICE, pApp->cMsgSrcName)) {  // < if not from NPP service
      switch (pApp->uiProtocolCmd) {
        case PROTOCOL_REGISTER_EVENTS: {
            FRAMEWORKUNIFIEDLOG(ZONE_NS_IMP_INFO, __FUNCTION__, "%s subscribing to session event on sessionid %d",
                   pApp->cMsgSrcName != 0 ? pApp->cMsgSrcName : NULL, pApp->uiSessionId);

            eStatus = ProcessProtocolEvents(hApp);
          }
          break;

        case PROTOCOL_UNREGISTER_EVENTS: {
            FRAMEWORKUNIFIEDLOG(ZONE_NS_IMP_INFO, __FUNCTION__, "%s unsubscribing private event on sessionid %d",
                   pApp->cMsgSrcName != 0 ? pApp->cMsgSrcName : NULL, pApp->uiSessionId);

            eStatus = UnregisterPrivateEvents(hApp);
          }
          break;

        case PROTOCOL_CLOSE_SESSION_REQ:
        case PROTOCOL_CLOSE_SESSION_REQ_SYNC: {
            FRAMEWORKUNIFIEDLOG(ZONE_NS_IMP_INFO, __FUNCTION__, "Close session request received from %s",
                   pApp->cMsgSrcName != 0 ? pApp->cMsgSrcName : NULL);

            // Handle the session events on the close session request
            (VOID)HandleSessionEventOnCloseSession(hApp);

            frameworkunifiedDelAcceptMonitor(hApp);

            eStatus = FrameworkunifiedProcessServiceProtocol(hApp);
          }
          break;

        case PROTOCOL_OPEN_SESSION_REQ:
        case PROTOCOL_OPEN_SESSION_REQ_SYNC: {
            FRAMEWORKUNIFIEDLOG(ZONE_NS_IMP_INFO, __FUNCTION__, "Open session request received from %s",
                   pApp->cMsgSrcName != 0 ? pApp->cMsgSrcName : NULL);

            eStatus = FrameworkunifiedProcessServiceProtocol(hApp);
            if (eStatus == eFrameworkunifiedStatusOK) {
              frameworkunifiedAddAcceptMonitor(hApp);
            }
          }
          break;

        default: {
            eStatus = FrameworkunifiedProcessServiceProtocol(hApp);
          }
          break;
      }
    } else {
      NotificationTable::iterator n_iterator;
      FrameworkunifiedGetSystemInfo(pApp, pApp->cSystemInfo);
      n_iterator = pApp->notifications.find(static_cast<PSTR>(pApp->cSystemInfo));
      if (n_iterator != pApp->notifications.end()) {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, " Notification found in NS_NP :: (%s)."
               " Processing callback function.", n_iterator->first.c_str());
        eStatus = (*n_iterator).second(hApp);
      } else {
        NotificationEventTable::iterator n_iterator;
        FrameworkunifiedGetSystemInfo(pApp, pApp->cSystemInfo);
        n_iterator = pApp->notificationevents.find(static_cast<PSTR>(pApp->cSystemInfo));
        if (n_iterator != pApp->notificationevents.end()) {
          CFrameworkunifiedHSMFramework *l_pStateMachine = FrameworkunifiedGetStateMachine(hApp);
          if (l_pStateMachine) {
            FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, " Notification found in NS_NP :: (%s)."
                   " Processing HSM event.", n_iterator->first.c_str());
            eStatus = l_pStateMachine->FrameworkunifiedPostEvent(n_iterator->second);
          }
        } else {
          /// Paul A. Dennis (FOUND BUG)
          /// For Persistent Data NS_NP sends the data back as a normal command not via the
          /// notification logic... The Ack's for NPReadPersistedData,NPRegisterFilePath and
          /// NPLoadFileFromPersistentStorage were never getting back
          /// to the requester because its's not looking in the correct location.
          /// So, Check the Services to see if the user has defined the Ack commands via the
          /// services map
          eStatus = FrameworkunifiedProcessServiceProtocol(hApp);
        }
      }
    }

    frameworkunifiedProcessSyncResponse(hApp, eStatus);

    if ((eStatus != eFrameworkunifiedStatusOK) && (eStatus != eFrameworkunifiedStatusExit) && (eStatus != eFrameworkunifiedStatusMsgNotProcessed)) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, " Error :: %d, (%s) Event Info :: (%s), (%d), (%s)",
             eStatus,
             pApp->cAppName != 0 ? static_cast<PSTR>(pApp->cAppName) : NULL,
             pApp->cMsgSrcName != 0 ? static_cast<PSTR>(pApp->cMsgSrcName) : NULL,
             pApp->uiProtocolCmd,
             pApp->cSystemInfo != 0 ? static_cast<PSTR>(pApp->cSystemInfo) : NULL);
    }

    if (eStatus == eFrameworkunifiedStatusMsgNotProcessed) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, "WARNING:: No handler found for :: "
             "Source: %s, Protocol Command: 0x%x, Session Id: %u.",
             pApp->cMsgSrcName != 0 ? pApp->cMsgSrcName : NULL,
             pApp->uiProtocolCmd, pApp->uiSessionId);
      FrameworkunifiedClearMsgData(hApp);
      eStatus = eFrameworkunifiedStatusOK;
    } else {
//      FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, "Message processed. Status (0x%X) ::"
//             " Source: %s Command: 0x%x  SessionId: %d.",
//             eStatus, pApp->cMsgSrcName != 0 ? pApp->cMsgSrcName : NULL,
//             pApp->uiProtocolCmd, pApp->uiSessionId);
    }

    if (enable_rusage == TRUE) {
      frameworkunifiedGetRusage(&end);
      frameworkunifiedPrintRusage(hApp, start, end);
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  // FRAMEWORKUNIFIEDLOG0(ZONE_NS_DIS, __FUNCTION__, "-");
  return eStatus;
}




//////////////////////////////////////////
// Function : FrameworkunifiedCloseDispatcher
//////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedCloseDispatcher(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus     = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    pApp->checkCode = 0;

    if (NULL != pApp->hAppRcvMsgQ) {
      McClose(pApp->hAppRcvMsgQ);
      pApp->hAppRcvMsgQ = NULL;
    }

    if (pApp->siMonitorSock) {
      mcCloseMonitor(pApp->cAppName);
      pApp->siMonitorSock = 0;
    }

    if (NULL != pApp->hAppSndMsgQ) {
      McClose(pApp->hAppSndMsgQ);
      pApp->hAppSndMsgQ = NULL;
    }

    if (NULL != pApp->hNPSndMsgQ) {
      FrameworkunifiedMcClose(pApp->hNPSndMsgQ);
      pApp->hNPSndMsgQ = NULL;
    }

    if (NULL != pApp->hParentSndMsgQ) {
      McClose(pApp->hParentSndMsgQ);
      pApp->hParentSndMsgQ = NULL;
    }

    if (0 < pApp->defer_evfd) {
      close(pApp->defer_evfd);
      pApp->defer_evfd = 0;
    }

    if (0 < pApp->efd) {
      close(pApp->efd);
      pApp->efd = 0;
    }

    delete(pApp);
    pApp = NULL;  // mb20110110 Added per  comment 216
    // NOTE: However, client still needs to set his own pointer to NULL
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  return eStatus;
}

//////////////////////////////////////////
// Function : FrameworkunifiedPopedDeferedMessage
//////////////////////////////////////////
BOOL FrameworkunifiedPopedDeferedMessage(HANDLE hApp) {
  BOOL rtnFlag = FALSE;

  // LCOV_EXCL_BR_START 6:FrameworkunifiedPopedDeferedMessage is internal function, hApp is checked in extenal API
  if (frameworkunifiedCheckValidAppHandle(hApp)) {
  // LCOV_EXCL_BR_STOP
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

    // check if the pop flag is send and there is something to pop!
    if (pApp->fPopDeferedMsg && !pApp->deferedMsgQueue.empty()) {
      // get the message at the top\front of the queue
      DeferedMsgInfo defered(pApp->deferedMsgQueue.front());

      // remove this item from the queue using pop
      pApp->deferedMsgQueue.pop();

      // copy the data into the pApp
      pApp->uiProtocolCmd = defered.uiProtocolCmd;
      strlcpy(pApp->cMsgSrcName, defered.cMsgSrcName, sizeof(pApp->cMsgSrcName));
      std::memcpy(pApp->uiMsgRcvBuffer, defered.uiMsgRcvBuffer, MAX_MSGQ_BUFFER);

      // clear the pop flag!
      pApp->fPopDeferedMsg = FALSE;

      // set the return flag TRUE, we have pop'ed
      rtnFlag = TRUE;
    }
  }

  return rtnFlag;
}



//////////////////////////////////////////
// Function : FrameworkunifiedDispatcher
//////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedDispatcher(PCSTR cAppName, const FrameworkunifiedDefaultCallbackHandler *CbHandler) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, "In");
  return FrameworkunifiedDispatcherWithArguments(cAppName, 0, NULL, CbHandler);
}

///////////////////////////////////////////////////////////////////////////////////////////
// FrameworkunifiedDispatcherWithArguments
// Creates, initializes and runs the dispatcher
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedDispatcherWithArguments(PCSTR cAppName, int argc, char *argv[],
                                      const FrameworkunifiedDefaultCallbackHandler *CbHandler,
                                      CustomCommandLineOptions *cmdLineOptions) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (cAppName == NULL) {
    return eFrameworkunifiedStatusNullPointer;
  }

  NsLogSetProcessName(cAppName);
  PLOG_TEXT("FrameworkunifiedDispatcher Start");
  // set main thread name as provided in dispatcher
  pthread_setname_np(pthread_self(), cAppName);

  FRAMEWORKUNIFIEDLOG0(ZONE_NS_DIS, __FUNCTION__, "In");

  if ((eStatus = RegistDefaultCbHandler(CbHandler)) != eFrameworkunifiedStatusOK) {
    return eStatus;
  }

  try {
    HANDLE hFrameworkApp = NULL;

    if (eFrameworkunifiedStatusOK == (eStatus = FrameworkunifiedCreateDispatcher(cAppName, hFrameworkApp, FALSE))) {
      // LCOV_EXCL_BR_START 6:if FrameworkunifiedCreateDispatcher return eFrameworkunifiedStatusOK, hFrameworkApp is not NULL
      if (NULL != hFrameworkApp) {
      // LCOV_EXCL_BR_STOP
        THApp hApp(hFrameworkApp);

        /// Parse the Arguments via the FrameworkunifiedArgumentParser
        /// passing an handle to the app and argument list
        eStatus = FrameworkunifiedArgumentParser(hApp, argc, argv, cmdLineOptions);

        if (eFrameworkunifiedStatusOK == eStatus) {
          eStatus = FrameworkunifiedDispatcherMain(hApp);
        }
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "hFrameworkApp is NULL");
        eStatus = eFrameworkunifiedStatusNullPointer;
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "FrameworkunifiedCreateDispatcher error, status=%d", eStatus);
    }
  } catch (const std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error: Failed to FrameworkunifiedCreateDispatcher: %s", e.what());
    eStatus = eFrameworkunifiedStatusNullPointer;
  }


  return eStatus;
}

// required for parsing subargument of -r
char *g_cPlogSubOpts[] = {
#define PLOG_OUTPUT    0
  const_cast<PSTR>("output"),
  NULL
};

EFrameworkunifiedStatus FrameworkunifiedArgumentParser(HANDLE hApp, int argc, char *argv[], CustomCommandLineOptions *cmdLineOptions) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  TFrameworkunifiedZoneMask mask = {};
  PCHAR l_cMask = NULL;
  UI_32 l_iMaskCount = 0;
  CHAR l_logoutput[MAX_QUEUE_NAME_SIZE];
  CHAR l_tFilePath[MAX_PATH_LENGTH];

  BOOL l_bHasConfigFilepath = FALSE;

  SI_32 l_iOpt = 0;
  SI_32 l_iPriority = 10;
  PSTR l_cOptions;
  PSTR l_cValue;

  FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, "In");

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
#ifdef DISPATCHER_PROFILER
    CFrameworkunifiedFrameworkApp *pApp = reinterpret_cast< CFrameworkunifiedFrameworkApp * >(hApp);
#endif

    if (argc > 0 && argv != NULL) {
      // l:m:t:c: options are reserved for NS Framework and should not be used by other FRAMEWORKUNIFIED Applications.
      CHAR l_cNSOptions[MAX_STRING_SIZE_TAG] = {};

      strncpy(l_cNSOptions, "l:m:c:p:qr:s", (MAX_STRING_SIZE_TAG - 1));
      optind = 1;

      if (NULL != cmdLineOptions) {
        // append custom options
        strncat(l_cNSOptions, cmdLineOptions->cShortOptions, (MAX_STRING_SIZE_TAG - strlen(l_cNSOptions) - 1));
      }
      while ((eFrameworkunifiedStatusOK == eStatus) && ((l_iOpt = getopt(argc, argv, l_cNSOptions)) != -1)) {
        switch (l_iOpt) {
          case 'm':  // mask value..
            FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, "-m (mask value): %s", optarg);
            /*
             * @todo
             * If the second of the command-line arguments is-m and the third is not, an exception occurs.
             */
            char *saveptr;
            l_cMask = strtok_r(optarg, ",", &saveptr);

            while (NULL != l_cMask) {
              mask[l_iMaskCount++] = static_cast<TFrameworkunifiedZone>(strtoul(l_cMask, NULL, 16));
              l_cMask = strtok_r(NULL, ",",  &saveptr);
            }
            NsLogSetControlMask(mask);
            break;
          case 'l':  // log output
            strncpy(l_logoutput, optarg, (MAX_QUEUE_NAME_SIZE - 1));
            FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, "-l (log output option): %s", l_logoutput);
            NsLogSetLogMethod(NsLogDetermineLogMethod(l_logoutput));
            break;
          case 'c':  // config file option
            strncpy(l_tFilePath, optarg, (MAX_PATH_LENGTH - 1));
            FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, "-c (set config file): %s", l_tFilePath);
            eStatus = SetConfigHandle(hApp, l_tFilePath);
            l_bHasConfigFilepath = TRUE;
            break;
          case 'p':  // priority
            FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, "-p (set priority): %s", optarg);
            if (eFrameworkunifiedStatusOK != frameworkunified::framework::CFrameworkunifiedThreadPriorities::ParseThreadArguments(optarg)) {
              FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, "WARNING!! Failed to parse sub arguments of -p");
            }
            l_iPriority = frameworkunified::framework::CFrameworkunifiedThreadPriorities::GetPriority(FrameworkunifiedGetAppName(hApp));
            // set only if valid prio, otherwise use the default one
            if (FRAMEWORKUNIFIED_PRIORITY_NOT_FOUND != l_iPriority) {
              pthread_setschedprio(pthread_self(),
              frameworkunified::framework::CFrameworkunifiedThreadPriorities::GetPriority(FrameworkunifiedGetAppName(hApp)));
            }
            break;
          case 'q':
#ifdef DISPATCHER_PROFILER
            FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, "-q (set msg dispatcher profiler)");
            FrameworkunifiedMsgProfiler::m_bMsgProfilerEnabled = TRUE;
            pApp->m_pFrameworkunifiedMsgProfiler = new(nothrow) FrameworkunifiedMsgProfiler(FrameworkunifiedGetAppName(hApp));
#endif
            break;
          case 'r':
            NSLogEnablePLog(TRUE);
            l_cOptions = optarg;
            while (*l_cOptions != '\0') {
              switch (getsubopt(&l_cOptions, g_cPlogSubOpts, &l_cValue)) {
                case PLOG_OUTPUT: {
                    if (l_cValue != NULL) {
                      NSLogSetPlogOutputOptions(static_cast<UI_8>(strtoul(l_cValue, NULL, 16)));
                    }
                    break;
                  }
                default: {
                    break;
                  }
              }
            }
            break;
          case 's':
            NSLogEnableSysEventLog(TRUE);
            break;
          default:
            if (NULL != cmdLineOptions) {
              // option other than l:m:t:c: is a custom option, therefore let application handle that option.
              if (eFrameworkunifiedStatusOK != cmdLineOptions->callback(l_iOpt, optarg)) {
                // Do not terminate an application even if invalid arguments received
                // eStatus = eFrameworkunifiedStatusFail;
                FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, "WARNING!! Invalid cmd line argument received.");
              }
            } else {
              // Do not terminate an application even if invalid arguments received
              // eStatus = eFrameworkunifiedStatusFail;
              FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, "WARNING!! Invalid cmd line argument received.");
            }
            break;
        }
      }
    }

    // if no config option is selected then use the default config path
    if ((FALSE == l_bHasConfigFilepath) && (eFrameworkunifiedStatusOK == eStatus)) {
      eStatus = SetDefaultConfigFile(hApp);
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  return eStatus;
}

//////////////////////////////////////////
// Function : FrameworkunifiedSimpleDispatcher
//////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSimpleDispatcher(PCSTR cAppName, const FrameworkunifiedDefaultCallbackHandler *CbHandler, CbFuncPtr CbShutdown,
                               BOOL isChildThread) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  // if main thread then set log process name
  if (!isChildThread) {
    NsLogSetProcessName(cAppName);
  }

  if ((eStatus = RegistDefaultCbHandler(CbHandler)) != eFrameworkunifiedStatusOK) {
    return eStatus;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, "+");

  if (NULL != CbShutdown) {
    try {
      HANDLE hFrameworkApp = NULL;

      if (eFrameworkunifiedStatusOK == (eStatus = FrameworkunifiedCreateDispatcher(cAppName, hFrameworkApp, isChildThread))) {
        // LCOV_EXCL_BR_START 6:if FrameworkunifiedCreateDispatcher return eFrameworkunifiedStatusOK, hFrameworkApp is not NULL
        if (NULL != hFrameworkApp) {
        // LCOV_EXCL_BR_STOP
          THApp hApp(hFrameworkApp);

          // Call application defined start function
          eStatus = FrameworkunifiedOnInitializationInternal(hApp);

          if (eFrameworkunifiedStatusOK != eStatus) {
            FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "FrameworkunifiedOnInitialization failed ");
            throw happ_error();
          } else {
            while (eFrameworkunifiedStatusExit != eStatus) {
              try {
                // dispatch block: waits on receive message
                if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedDispatchBlock(hApp))) {
                  FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error: FrameworkunifiedDispatchBlock : 0x%x ",  eStatus);
                  continue;
                }
                // dispatch process: calls respective call back functions
                if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedDispatchProcess(hApp))) {
                  FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error: FrameworkunifiedDispatchProcess : 0x%x ",  eStatus);
                }
              } catch (const frameworkunified::framework::error::CSystemError &err) {
                FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR :: %s", err.what());
                eStatus = eFrameworkunifiedStatusErrOther;
                FrameworkunifiedSendSystemErrMessage(hApp, err.m_eSystemError);
              } catch (const std::exception &err) {
                FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Recovering from: %s", err.what());
                eStatus = eFrameworkunifiedStatusErrOther;
              }
            }
            FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exiting dispatcher ");
          }
          // Call application defined stop function
          eStatus = CbShutdown(hApp);
        } else {
          FRAMEWORKUNIFIEDLOG0(ZONE_NS_ERR, __FUNCTION__, "hFrameworkApp is NULL");
          eStatus = eFrameworkunifiedStatusNullPointer;
        }
      } else {
        FRAMEWORKUNIFIEDLOG0(ZONE_NS_ERR, __FUNCTION__, "FrameworkunifiedCreateDispatcher error, status=%d", eStatus);
      }
    } catch (const THApp::Exception &) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error: Failed to FrameworkunifiedCreateDispatcher ");
      eStatus = eFrameworkunifiedStatusNullPointer;
    }
  } else {
    eStatus = eFrameworkunifiedStatusFail;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus RunDispatcher(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  int efd;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
#ifdef DISPATCHER_PROFILER
    // Get the application handle
    CFrameworkunifiedFrameworkApp *pApp = reinterpret_cast< CFrameworkunifiedFrameworkApp * >(hApp);
    if (NULL != pApp->m_pFrameworkunifiedMsgProfiler) {
      FrameworkunifiedAttachMsgProfilerCallbacksDispatcher(hApp);
    }
#endif
    FrameworkunifiedGetDispatcherFD(hApp, &efd);

    while (1) {
      frameworkunifiedFdHandler(hApp, efd);
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  return eStatus;
}


EFrameworkunifiedStatus FrameworkunifiedDispatcherMain(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    try {
      FRAMEWORKUNIFIEDLOG0(ZONE_NS_DIS, __FUNCTION__, "Success: FrameworkunifiedCreateDispatcher ");

      // Initialize the global structures
      // register service with service dir
      // register notifications
      FRAMEWORKUNIFIEDLOG_PERFORMANCE_DEBUG("FrameworkunifiedOnInitialization +");
      eStatus = FrameworkunifiedOnInitializationInternal(hApp);
      FRAMEWORKUNIFIEDLOG_PERFORMANCE_DEBUG("FrameworkunifiedOnInitialization -");

      if (eFrameworkunifiedStatusOK != eStatus) {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "FrameworkunifiedOnInitialization failed ");
        throw happ_error();
      } else {
        eStatus = RunDispatcher(hApp);
        CFrameworkunifiedSyncData::FrameworkunifiedReleaseSyncDataInstance();
      }
    } catch (const frameworkunified::framework::error::CSystemError &err) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR :: %s", err.what());
      eStatus = eFrameworkunifiedStatusErrOther;
      FrameworkunifiedSendSystemErrMessage(hApp, err.m_eSystemError);
    } catch (const THApp::Exception &) {
      FRAMEWORKUNIFIEDLOG0(ZONE_NS_ERR, __FUNCTION__, "Error: Failed to FrameworkunifiedCreateDispatcher ");
      eStatus = eFrameworkunifiedStatusNullPointer;
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  return eStatus;
}

EFrameworkunifiedStatus FrameworkunifiedCreateDispatcherWithoutLoop(PCSTR cAppName, HANDLE &hApp, int argc, char *argv[],  // NOLINT  (readability/nolint)
                                          const FrameworkunifiedDefaultCallbackHandler *CbHandler,
                                          BOOL bIsConnectSM, CustomCommandLineOptions *cmdLineOptions) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  THApp *l_hApp = NULL;

  if (cAppName == NULL) {
    return eFrameworkunifiedStatusNullPointer;
  }

  NsLogSetProcessName(cAppName);

  PLOG_TEXT("FrameworkunifiedDispatcher Start");
  // set main thread name as provided in dispatcher
  pthread_setname_np(pthread_self(), cAppName);

  if ((eStatus = RegistDefaultCbHandler(CbHandler)) != eFrameworkunifiedStatusOK) {
    return eStatus;
  }

  FRAMEWORKUNIFIEDLOG0(ZONE_NS_DIS, __FUNCTION__, "In");

  try {
    HANDLE hFrameworkApp = NULL;

    if (eFrameworkunifiedStatusOK == (eStatus = FrameworkunifiedCreateDispatcher(cAppName, hFrameworkApp,
      bIsConnectSM == TRUE ? FALSE : TRUE))) {
      // LCOV_EXCL_BR_START 6:if FrameworkunifiedCreateDispatcher return eFrameworkunifiedStatusOK, hFrameworkApp is not NULL
      if (NULL != hFrameworkApp) {
      // LCOV_EXCL_BR_STOP
        l_hApp = new THApp(hFrameworkApp);
        hApp = *l_hApp;
        CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
        pApp->pRsrcMgr = reinterpret_cast<void *>(l_hApp);

        eStatus = FrameworkunifiedArgumentParser(hApp, argc, argv, cmdLineOptions);

        if (eFrameworkunifiedStatusOK == eStatus) {
          FRAMEWORKUNIFIEDLOG_PERFORMANCE_DEBUG("FrameworkunifiedOnInitialization +");
          eStatus = FrameworkunifiedOnInitializationInternal(hApp);
          FRAMEWORKUNIFIEDLOG_PERFORMANCE_DEBUG("FrameworkunifiedOnInitialization -");

          if (eFrameworkunifiedStatusOK != eStatus) {
            FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "FrameworkunifiedOnInitialization failed ");
          }
        }
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "FrameworkunifiedCreateDispatcher error, status=%d", eStatus);
    }
  } catch (const THApp::Exception &) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error: Failed to FrameworkunifiedCreateDispatcher ");
    eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return eStatus;
}

EFrameworkunifiedStatus FrameworkunifiedDispatchProcessWithoutLoop(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  int efd;  // FD for multi waiting

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    FrameworkunifiedGetDispatcherFD(hApp, &efd);
    eStatus = frameworkunifiedFdHandler(hApp, efd);
    if (eFrameworkunifiedStatusOK != eStatus) {
      eStatus = eFrameworkunifiedStatusFail;
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  return eStatus;
}

EFrameworkunifiedStatus FrameworkunifiedDestroyDispatcherWithoutLoop(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    try {
      CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
      THApp *RsrcMgr = reinterpret_cast<THApp *>(pApp->pRsrcMgr);
      delete RsrcMgr;
    } catch (const std::exception &e) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error: %s", e.what());
      eStatus = eFrameworkunifiedStatusNullPointer;
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  return eStatus;
}

static EFrameworkunifiedStatus GetDispatcherMqFD(HANDLE hApp, int *fd) {
  if (fd == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "fd is NULL");
    return eFrameworkunifiedStatusNullPointer;
  }
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    *fd = McGetQueueFD(pApp->hAppRcvMsgQ);
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  return eStatus;
}

static EFrameworkunifiedStatus CreateMultiWaitingFD(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  int efd;        // FD for multi waiting
  int q_fd;       // FD for Dispatcher's message queue
  int defer_evfd;     // FD for receiving Defer events to Dispatcher
  struct epoll_event ev;  // Info struct to associate with multiwaiting FD

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

    // Create efd
    efd = epoll_create1(EPOLL_CLOEXEC);
    if (-1 != efd) {
      pApp->efd = efd;

      // get fd to Dispatcher's message queue
      eStatus = GetDispatcherMqFD(hApp, &q_fd);
      if (eFrameworkunifiedStatusOK == eStatus) {
        // efd to monitor the q_fd.
        ev.events = EPOLLIN;
        ev.data.fd = q_fd;
        if (-1 == epoll_ctl(efd, EPOLL_CTL_ADD, q_fd, &ev)) {
          eStatus = eFrameworkunifiedStatusFail;
          FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : epoll_ctl(q_fd, ADD) Failed, "
            "status=%d, errno=%d", eStatus, errno);
        }
      } else {
        eStatus = eFrameworkunifiedStatusFail;
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : GetDispatcherMqFD Failed, status=%d", eStatus);
      }

      // create FD for receiving Defer events to Dispatcher
      defer_evfd = eventfd(0, EFD_CLOEXEC);
      if (-1 != defer_evfd) {
        pApp->defer_evfd = defer_evfd;

        // efd to monitor the defer_evfd.
        ev.events = EPOLLIN;
        ev.data.fd = defer_evfd;
        if (-1 == epoll_ctl(efd, EPOLL_CTL_ADD, defer_evfd, &ev)) {
          eStatus = eFrameworkunifiedStatusFail;
          FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : epoll_ctl(defer_evfd, ADD) Failed,"
            " status=%d, errno=%d", eStatus, errno);
        }
      } else {
        eStatus = eFrameworkunifiedStatusFail;
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : eventfd Failed, status=%d, errno=%d", eStatus, errno);
      }

      // add abnormal monitor socket polling
      if (-1 != pApp->siMonitorSock) {
        ev.events = EPOLLIN;
        ev.data.fd = pApp->siMonitorSock;
        if (-1 == epoll_ctl(efd, EPOLL_CTL_ADD, pApp->siMonitorSock, &ev)) {
          eStatus = eFrameworkunifiedStatusFail;
          FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : epoll_ctl:%s", strerror(errno));
        }
      }
    } else {
      eStatus = eFrameworkunifiedStatusFail;
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : epoll_create1 Failed, status=%d, errno=%d", eStatus, errno);
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : %d, Invalid hApp ", eStatus);
  }

  return eStatus;
}

EFrameworkunifiedStatus FrameworkunifiedGetDispatcherFD(HANDLE hApp, int *efd) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    if (NULL != efd) {
      CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
      *efd = pApp->efd;
    } else {
      eStatus = eFrameworkunifiedStatusNullPointer;
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : %d, Invalid hApp ", eStatus);
  }

  return eStatus;
}

EFrameworkunifiedStatus FrameworkunifiedOnInitializationInternal(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusFail;

  if (g_defaultCallbackHandler.onInitilization != NULL) {
    try {
      eStatus = g_defaultCallbackHandler.onInitilization(hApp);
    } catch (const std::exception &e) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error: %s", e.what());
    }
  }
  return eStatus;
}

EFrameworkunifiedStatus FrameworkunifiedOnDestroyInternal(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusFail;

  if (g_defaultCallbackHandler.onDestroy != NULL) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    if (pApp->hParentSndMsgQ == NULL) {
      // main thread
      try {
        eStatus = g_defaultCallbackHandler.onDestroy(hApp);
      } catch (const std::exception &e) {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error: %s", e.what());
      }
    } else {
      eStatus = eFrameworkunifiedStatusOK;
    }
  }
  return eStatus;
}

EFrameworkunifiedStatus FrameworkunifiedOnStartInternal(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusFail;

  if (g_defaultCallbackHandler.onStart != NULL) {
    try {
      eStatus = g_defaultCallbackHandler.onStart(hApp);
    } catch (const std::exception &e) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error: %s", e.what());
    }
  }
  return eStatus;
}

EFrameworkunifiedStatus FrameworkunifiedOnStopInternal(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusFail;

  if (g_defaultCallbackHandler.onStop != NULL) {
    try {
      eStatus = g_defaultCallbackHandler.onStop(hApp);
    } catch (const std::exception &e) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error: %s", e.what());
    }
  }
  return eStatus;
}

EFrameworkunifiedStatus FrameworkunifiedOnPreStartInternal(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusFail;

  if (g_defaultCallbackHandler.onPreStart != NULL) {
    try {
      eStatus = g_defaultCallbackHandler.onPreStart(hApp);
    } catch (const std::exception &e) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error: %s", e.what());
    }
  }
  return eStatus;  // LCOV_EXCL_BR_LINE 11:Excluded due to gcov constraints (others)
}

EFrameworkunifiedStatus FrameworkunifiedOnPreStopInternal(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusFail;

  if (g_defaultCallbackHandler.onPreStop != NULL) {
    try {
      eStatus = g_defaultCallbackHandler.onPreStop(hApp);
    } catch (const std::exception &e) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error: %s", e.what());
    }
  }
  return eStatus;  // LCOV_EXCL_BR_LINE 11:Excluded due to gcov constraints (others)
}

EFrameworkunifiedStatus FrameworkunifiedOnBackgroundStartInternal(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusFail;

  if (g_defaultCallbackHandler.onBackgroundStart != NULL) {
    try {
      eStatus = g_defaultCallbackHandler.onBackgroundStart(hApp);
    } catch (const std::exception &e) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error: %s", e.what());
    }
  }
  return eStatus;  // LCOV_EXCL_BR_LINE 11:Excluded due to gcov constraints (others)
}

EFrameworkunifiedStatus FrameworkunifiedOnBackgroundStopInternal(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusFail;

  if (g_defaultCallbackHandler.onBackgroundStop != NULL) {
    try {
      eStatus = g_defaultCallbackHandler.onBackgroundStop(hApp);
    } catch (const std::exception &e) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error: %s", e.what());
    }
  }
  return eStatus;  // LCOV_EXCL_BR_LINE 11:Excluded due to gcov constraints (others)
}

EFrameworkunifiedStatus FrameworkunifiedOnDebugDumpInternal(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusFail;

  if (g_defaultCallbackHandler.onDebugDump != NULL) {
    try {
      eStatus = g_defaultCallbackHandler.onDebugDump(hApp);
    } catch (const std::exception &e) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error: %s", e.what());
    }
  }
  return eStatus;
}

EFrameworkunifiedStatus FrameworkunifiedCreateStateMachineInternal(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusFail;

  if (g_defaultCallbackHandler.createStateMachine != NULL) {
    try {
      eStatus = g_defaultCallbackHandler.createStateMachine(hApp);
    } catch (const std::exception &e) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error: %s", e.what());
    }
  }
  return eStatus;
}

EFrameworkunifiedStatus FrameworkunifiedSSFrameworkInterfaceInternal(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusFail;

  if (g_defaultCallbackHandler.ssFrameworkInterface != NULL) {
    try {
      eStatus = g_defaultCallbackHandler.ssFrameworkInterface(hApp);
    } catch (const std::exception &e) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error: %s", e.what());
    }
  }
  return eStatus;
}

EFrameworkunifiedStatus RegistDefaultCbHandler(const FrameworkunifiedDefaultCallbackHandler *CbHandler) {
  if (CbHandler == NULL) {
    return eFrameworkunifiedStatusNullPointer;
  } else if (CbHandler->onInitilization == NULL || CbHandler->onDestroy == NULL || CbHandler->onStart == NULL ||
             CbHandler->onStop == NULL
             || CbHandler->onPreStart == NULL || CbHandler->onPreStop == NULL
             || CbHandler->onBackgroundStart == NULL || CbHandler->onBackgroundStop == NULL
             || CbHandler->onDebugDump == NULL || CbHandler->createStateMachine == NULL
             || CbHandler->ssFrameworkInterface == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, " [ ABORT ] DefaultCallbackHandler is NULL");
    return eFrameworkunifiedStatusInvldParam;
  }

  g_defaultCallbackHandler = *CbHandler;
  return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus FrameworkunifiedGetDefaultCbHandler(FrameworkunifiedDefaultCallbackHandler *CbHandler) {
  if (CbHandler == NULL) {
    return eFrameworkunifiedStatusNullPointer;
  }

  *CbHandler = g_defaultCallbackHandler;
  return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus frameworkunifiedFdHandler(HANDLE hApp , int efd) {
  struct epoll_event events[MAX_FD_MULTIWAITING];
  int nfds;  // The number of events received
  int q_fd;  // The FD of events received
  int n;    // Loop counter
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  const char *key;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    eStatus = GetDispatcherMqFD(hApp, &q_fd);
    // LCOV_EXCL_BR_START 6:Because of hApp is valid, GetDispatcherMqFD return eFrameworkunifiedStatusOK.
    if (eFrameworkunifiedStatusOK == eStatus) {
    // LCOV_EXCL_BR_STOP
      nfds = epoll_wait(efd, events, MAX_FD_MULTIWAITING, -1);
      if (-1 != nfds) {
        for (n = 0; n < nfds; ++n) {
          if (pApp->siMonitorSock == events[n].data.fd) {
            // abnormal monitor connect request
            int socket;

            if ((socket = mcAcceptMonitor(pApp->cAppName, pApp->siMonitorSock)) >= 0) {
              struct epoll_event event;

              event.events = EPOLLRDHUP;
              event.data.fd = socket;

              if (epoll_ctl(pApp->efd, EPOLL_CTL_ADD, socket, &event) < 0) {
                FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : epoll_ctl:%s", strerror(errno));
              }
            }
          }
        }

        for (n = 0; n < nfds; ++n) {
          // The branch processing by FD on which the message was received
          if (q_fd == events[n].data.fd) {
            // process the message to the Dispatcher's message queue
            eStatus = ProcessMq(hApp);
          } else if (pApp->defer_evfd == events[n].data.fd) {
            // process the Defer event to the Dispatcher
            uint64_t data;
            ssize_t len;

            len = read(pApp->defer_evfd, &data, sizeof(uint64_t));
            if (len < 0) {
              // read error
              eStatus = eFrameworkunifiedStatusFail;
              FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : read Failed, status=%d, errno=%d, defer_evfd=%d",
                     eStatus, errno, pApp->defer_evfd);
            } else {
              while (TRUE == (FrameworkunifiedPopedDeferedMessage(hApp))) {
                try {
                  // dispatch process: calls respective call back functions
                  if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedDispatchProcess(hApp))) {
                    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : FrameworkunifiedDispatchProcess Status : 0x%x",  eStatus);
                  }
                } catch (const frameworkunified::framework::error::CSystemError &err) {
                  FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : %s", err.what());
                  eStatus = eFrameworkunifiedStatusErrOther;
                  FrameworkunifiedSendSystemErrMessage(hApp, err.m_eSystemError);
                } catch (const std::exception &err) {
                  FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Recovering from : %s", err.what());
                  eStatus = eFrameworkunifiedStatusErrOther;
                }
              }
            }
          } else if (pApp->siMonitorSock == events[n].data.fd) {
            // do nothing
          } else if (frameworkunifiedSearchResourseKey(FRAMEWORKUNIFIED_RES_ABNMLMON, events[n].data.fd, &key) == 0) {
            if (pApp->acceptMonitorTable.find(events[n].data.fd) != pApp->acceptMonitorTable.end()) {
              // server: accept socket close
              eStatus = frameworkunifiedProcessClientAbnormalState(hApp, events[n].data.fd);
            } else if (pApp->connectMonitorTable.find(events[n].data.fd) != pApp->connectMonitorTable.end()) {
              // client: connect socket close
              eStatus = frameworkunifiedProcessServerAbnormalState(hApp, events[n].data.fd);
            }

            if (epoll_ctl(pApp->efd, EPOLL_CTL_DEL, events[n].data.fd, NULL) < 0) {
              FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : epoll_ctl:%s", strerror(errno));
            }
            close(events[n].data.fd);
            frameworkunifiedUnregistResouce(FRAMEWORKUNIFIED_RES_ABNMLMON, key);
          } else {
            // process the message to the FD set by the user
            eStatus = ProcessFd(hApp, events[n].data.fd);
          }
        }
      } else {
        if (errno == EINTR) {
          // signal interrupt
        } else {
          eStatus = eFrameworkunifiedStatusFail;
          FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : epoll_wait Failed, status=%d, errno=%d", eStatus, errno);
        }
      }
    } else {
      // LCOV_EXCL_START 6:Because of hApp is valid, GetDispatcherMqFD return eFrameworkunifiedStatusOK.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      eStatus = eFrameworkunifiedStatusFail;
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : GetDispatcherMqFD Failed, status=%d", eStatus);
      // LCOV_EXCL_STOP
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : %d, Invalid hApp ", eStatus);
  }

  return eStatus;
}

static EFrameworkunifiedStatus ProcessMq(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
#ifdef DISPATCHER_PROFILER
    CFrameworkunifiedFrameworkApp *pApp = reinterpret_cast< CFrameworkunifiedFrameworkApp * >(hApp);
#endif

    // handle defer'ed message
    if (FALSE == FrameworkunifiedPopedDeferedMessage(hApp)) {
      // dispatch block: waits on receive message
      if ((eStatus = FrameworkunifiedDispatchBlock(hApp)) != eFrameworkunifiedStatusOK) {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : FrameworkunifiedDispatchBlock Status : 0x%x",  eStatus);
      }
      #ifdef DISPATCHER_PROFILER
      else if (NULL != pApp->m_pFrameworkunifiedMsgProfiler) {  // NOLINT (readability/nolint)
        pApp->m_pFrameworkunifiedMsgProfiler->MsgReceived();
      }
      #endif
    }
    try {
      // dispatch process: calls respective call back functions
      eStatus = FrameworkunifiedDispatchProcess(hApp);
    } catch (const frameworkunified::framework::error::CSystemError &err) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : %s", err.what());
      eStatus = eFrameworkunifiedStatusErrOther;
      FrameworkunifiedSendSystemErrMessage(hApp, err.m_eSystemError);
    } catch (const std::exception &err) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Recovering from : %s", err.what());
      eStatus = eFrameworkunifiedStatusErrOther;
    }
#ifdef DISPATCHER_PROFILER
    if (NULL != pApp->m_pFrameworkunifiedMsgProfiler) {
      pApp->m_pFrameworkunifiedMsgProfiler->MsgProcessed();
    }
#endif
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : %d, Invalid hApp ", eStatus);
  }

  return eStatus;
}

static EFrameworkunifiedStatus ProcessFd(HANDLE hApp , int fd) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    FdTable::iterator f_iterator;
    struct rusage start, end;
    BOOL enable_rusage = NsLogIsZoneSet(ZONE_NS_DEBUG_DUMP);

    if (enable_rusage == TRUE) {
      frameworkunifiedGetRusage(&start);
    }

    // finding callback func associated with FD
    f_iterator = pApp->fds.find(fd);
    if (f_iterator != pApp->fds.end()) {
      try {
        // calling func associated with FD
        eStatus = (*f_iterator).second(hApp);
      } catch (const frameworkunified::framework::error::CSystemError &err) {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR :: %s", err.what());
        eStatus = eFrameworkunifiedStatusErrOther;
        FrameworkunifiedSendSystemErrMessage(hApp, err.m_eSystemError);
      } catch (const std::exception &err) {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Recovering from: %s", err.what());
        eStatus = eFrameworkunifiedStatusErrOther;
      }
    } else {
      eStatus = eFrameworkunifiedStatusFail;
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : %d, Callback associated with FD is not found. ", eStatus);
    }

    if (enable_rusage == TRUE) {
      frameworkunifiedGetRusage(&end);
      frameworkunifiedPrintRusage(hApp, start, end);
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : %d, Invalid hApp ", eStatus);
  }

  return eStatus;
}

/////////////////////////////////////////////////////////
/// EFrameworkunifiedStatus SetConfigHandle(HANDLE hApp, PCHAR sFilePath )
/// This Function opens the file from give path and sets the file handle in hApp
////////////////////////////////////////////////////////
EFrameworkunifiedStatus SetConfigHandle(HANDLE hApp, PCHAR sFilePath) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __PRETTY_FUNCTION__, "This function is not implemented");

  return eStatus;
}

/////////////////////////////////////////////////////////
/// EFrameworkunifiedStatus SetDefaultFilePath(HANDLE hApp)
/// This Function fetches the file path from Environment variable from PASA_APPCONFIG_FILES and
/// pass it to the SetConfigHandle function
////////////////////////////////////////////////////////
EFrameworkunifiedStatus SetDefaultConfigFile(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    // Get the file path from environment variable
    PCHAR l_strEnvValue = getenv(PASA_APPCONFIG_FILES);

    if (l_strEnvValue) {
      // FRAMEWORKUNIFIEDLOG( ZONE_NS_DIS, __FUNCTION__, "Configuration file path %s ",l_strEnvValue);

      std::string l_strFilePath(l_strEnvValue);

      if (!l_strFilePath.empty()) {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, "Configuration file path %s ", l_strFilePath.c_str());
        // Open the configuration file
        SetConfigHandle(hApp, (PCHAR)l_strFilePath.c_str());
      } else {
        eStatus = eFrameworkunifiedStatusFail;
      }
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

EFrameworkunifiedStatus FrameworkunifiedProcessServiceProtocolHSMEvent(HANDLE hApp, PCSTR pRequester) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    EventServices::iterator s_iterator;

    // finding the service
    s_iterator = pApp->eventservices.find(pRequester);
    if (s_iterator != pApp->eventservices.end()) {
      EventSessionTable::iterator session_iterator;
      session_iterator = (s_iterator->second).find(pApp->uiSessionId);
      if (session_iterator != (s_iterator->second).end()) {
        ServiceEventProtocolTable::iterator spt_iterator;

        //      FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, " %s found :: (%s)  ", pApp->cMsgSrcName, s_iterator->second);

        // find the protocol req/ack handler
        spt_iterator = (session_iterator->second).find(pApp->uiProtocolCmd);
        if (spt_iterator != (session_iterator->second).end()) {
          // FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, " Protocol Command :: (0x%x)  ", pApp->uiProtocolCmd);
          // Post corresponding event
          CFrameworkunifiedHSMFramework *l_pStateMachine = FrameworkunifiedGetStateMachine(hApp);
          if (l_pStateMachine) {
            eStatus = l_pStateMachine->FrameworkunifiedPostEvent(spt_iterator->second);
          }
        } else if ((session_iterator->second).end() != (spt_iterator =
          (session_iterator->second).find(FRAMEWORKUNIFIED_ANY_COMMAND))) {
          FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, "Processing HSM event for (FRAMEWORKUNIFIED_ANY_COMMAND). "
            "Dispatcher protocol command: (0x%x)",
                 pApp->uiProtocolCmd);
          // Post corresponding event
          CFrameworkunifiedHSMFramework *l_pStateMachine = FrameworkunifiedGetStateMachine(hApp);
          if (l_pStateMachine) {
            eStatus = l_pStateMachine->FrameworkunifiedPostEvent(spt_iterator->second);
          }
        } else {
          eStatus = eFrameworkunifiedStatusMsgNotProcessed;
        }

        // if FrameworkunifiedPostEvent returns NULL
        if (eFrameworkunifiedStatusNullPointer == eStatus) {
          eStatus = eFrameworkunifiedStatusExit;
        }
      } else {
        eStatus = eFrameworkunifiedStatusMsgNotProcessed;
      }

    } else {
      eStatus = eFrameworkunifiedStatusMsgNotProcessed;
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  return eStatus;
}

//////////////////////////////////////////
// Function : ProcessProtocolEvents
//////////////////////////////////////////
EFrameworkunifiedStatus ProcessProtocolEvents(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

    // common event table iterator
    EventTable::iterator e_iterator;
    ServiceSessionIdListTable::iterator ssidl_iterator;

    // public state event table iterator
    PublicStateEventTable::iterator se_iterator;

    // private state event table iterator
    PrivateStateEventTable::iterator pse_iterator;
    ServiceSessionEventData::iterator ssed_iterator;
    SessionEventData::iterator sed_iterator;

    HANDLE l_pClientHandle = NULL;

    // get the name of the service from which message is received
    PCSTR l_cServiceName = pApp->cMsgSrcName;
    const UI_32 l_uiSessionId = pApp->uiSessionId;

    // number of events to register
    UI_32 l_uiListSize = static_cast<UI_32>(FrameworkunifiedGetMsgLength(hApp) / sizeof(UI_32));

    // check if the received service name and data is valid
    if (NULL != l_cServiceName && 0 < l_uiListSize) {
      PUI_32 l_pData = new UI_32[l_uiListSize];
      UI_32 l_uiEventId = 0;

      // get the list of events
      eStatus = FrameworkunifiedGetMsgDataOfSize(hApp, l_pData, static_cast<UI_32>(l_uiListSize * sizeof(UI_32)));

      if (NULL != l_pData && eFrameworkunifiedStatusOK == eStatus) {
        for (UI_32 uiCount = 0; uiCount < l_uiListSize; uiCount++) {
          // received event id
          l_uiEventId = l_pData[uiCount];

          // find the event in map of EventTable
          e_iterator = pApp->eventtable.find(l_uiEventId);

          // if event is registered by any of the clients
          if (pApp->eventtable.end() != e_iterator) {
            // table of services with list of their session id's registering this event
            ServiceSessionIdListTable &l_mServiceSessionIdListTable = e_iterator->second;

            // find the service name in map of ServiceSessionIdListTable
            ssidl_iterator = l_mServiceSessionIdListTable.find(l_cServiceName);

            // if service have already registered this event with any of its session
            if (l_mServiceSessionIdListTable.end() != ssidl_iterator) {
              // list of sessions id's registering with this event
              SessionIdList &l_vSessionIdList = ssidl_iterator->second;

              // event is registered with this session
              if (IsEventAlreadyRegistered(l_vSessionIdList, l_uiSessionId)) {
                // get the message queue handle from map servicesessionhandletable
                l_pClientHandle = GetMsgQueueHandle(hApp, l_cServiceName, l_uiSessionId);
              } else {  // event is not registered with this session
                // create new message queue handle and insert it in map servicesessionhandletable
                l_pClientHandle = GetNewHandleAndUpdateEventInfoTable(hApp,
                  l_cServiceName, l_uiSessionId, l_uiEventId);

                // insert the current session id
                l_vSessionIdList.push_back(l_uiSessionId);
              }
            } else {  // if service have not registered this event with any of its session
              // create a list for storing session id's registering this event for this service
              SessionIdList l_vSessionIdList;

              // insert the current session id
              l_vSessionIdList.push_back(l_uiSessionId);

              // create new message queue handle and insert it in map servicesessionhandletable
              l_pClientHandle = GetNewHandleAndUpdateEventInfoTable(hApp, l_cServiceName, l_uiSessionId, l_uiEventId);

              // make a entry for service name and session id's list in map of ServiceSessionIdListTable
              l_mServiceSessionIdListTable.insert(make_pair(l_cServiceName, l_vSessionIdList));
            }
          } else {  // if event is not registered by any of the clients
            // create a list for storing session id's registering this event for this service
            SessionIdList l_vSessionIdList;

            // insert the current session id
            l_vSessionIdList.push_back(l_uiSessionId);

            // create new message queue handle and insert it in map servicesessionhandletable
            l_pClientHandle = GetNewHandleAndUpdateEventInfoTable(hApp, l_cServiceName, l_uiSessionId, l_uiEventId);

            // create a table for storing services and its session id list, registering this event
            ServiceSessionIdListTable l_mServiceSessionIdListTable;

            // make a entry for service name and session id's list in map of ServiceSessionIdListTable
            l_mServiceSessionIdListTable.insert(make_pair(l_cServiceName, l_vSessionIdList));

            // insert the table with event id in EventTable
            pApp->eventtable.insert(make_pair(l_uiEventId, l_mServiceSessionIdListTable));
          }

          // check whether event is defined as public state event
          se_iterator = pApp->publicstateeventtable.find(l_uiEventId);

          // if event is a state event, publish the event
          if (pApp->publicstateeventtable.end() != se_iterator) {
            FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "%d is a public state event", l_uiEventId);

            if (NULL != (se_iterator->second)) {
              // send the event data for state event
              eStatus = FrameworkunifiedSendMsg(l_pClientHandle,
                                   l_uiEventId,
                                   (se_iterator->second)->uiLength,
                                   (se_iterator->second)->pData);
              FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "Sending event:%d to service:%s, session:%d, status:%d",
                     l_uiEventId,
                     l_cServiceName,
                     l_uiSessionId,
                     eStatus);
            } else {
              FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "Event %d yet not published ", l_uiEventId);
            }
          } else {  // check whether the event is defined as private state event
            pse_iterator = pApp->privatestateeventtable.find(l_uiEventId);

            // if event found, publish the event
            if (pApp->privatestateeventtable.end() != pse_iterator) {
              FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "%d is a private state event", l_uiEventId);

              ssed_iterator = (pse_iterator->second).find(l_cServiceName);

              // check whether the event had been published for this service before subscription
              if ((pse_iterator->second).end() != ssed_iterator) {
                sed_iterator = (ssed_iterator->second).find(l_uiSessionId);

                // check whether the event had been published for this service for the same session
                if ((ssed_iterator->second).end() != sed_iterator) {
                  if (NULL != (sed_iterator->second)) {
                    FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, "%d had been published for the "
                      "service:%s on sessionId:%d ", l_uiEventId,
                           l_cServiceName, l_uiSessionId);

                    // send the event data for state event
                    eStatus = FrameworkunifiedSendMsg(l_pClientHandle,
                                         l_uiEventId,
                                         (sed_iterator->second)->uiLength,
                                         (sed_iterator->second)->pData);
                    FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "Sending event:%d to service:%s, session:%d, status:%d",
                           l_uiEventId,
                           l_cServiceName,
                           l_uiSessionId,
                           eStatus);
                  } else {
                    FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "Event %d yet not published ", l_uiEventId);
                  }
                }
              }
            }
          }
        }  // end of for loop of events list
      } else {
        eStatus = eFrameworkunifiedStatusInvldBuf;
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error :: %d, PROTOCOL_REGISTER_EVENTS Invalid data ", eStatus);
      }

      if (NULL != l_pData) {
        delete[] l_pData;
        l_pData = NULL;
      }
    } else {
      eStatus = eFrameworkunifiedStatusInvldParam;
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error :: %d, PROTOCOL_REGISTER_EVENTS Service Name invalid ", eStatus);
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error :: %d, Invalid hApp ", eStatus);
  }

  return eStatus;
}

//////////////////////////////////////////
// Function : IsEventAlreadyRegistered
//////////////////////////////////////////
BOOL IsEventAlreadyRegistered(const SessionIdList &vSessionIdList, const UI_32 uiSessionId) {
  for (UI_32 uiCount = 0; uiCount < vSessionIdList.size(); uiCount++) {
    if (uiSessionId == vSessionIdList[uiCount]) {
      return TRUE;
    }
  }

  return FALSE;
}

//////////////////////////////////////////
// Function : GetMsgQueueHandle
//////////////////////////////////////////
HANDLE GetMsgQueueHandle(HANDLE hApp, PCSTR serviceName, const UI_32 uiSessionId) {
  HANDLE l_pClientHandle = NULL;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

    ServiceSessionHandleTable::iterator ssh_iterator;;
    SessionToEventInfo::iterator sh_iterator;

    // find the service name
    ssh_iterator = pApp->servicesessionhandletable.find(serviceName);

    if (pApp->servicesessionhandletable.end() != ssh_iterator) {
      // search for session id
      sh_iterator = ssh_iterator->second.find(uiSessionId);

      if (ssh_iterator->second.end() != sh_iterator) {
        SessionEventInfo *l_ptSessionEventInfo = sh_iterator->second;
        if (NULL != l_ptSessionEventInfo) {
          l_pClientHandle = l_ptSessionEventInfo->m_hSession;
          FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "Handle found for service: %s, session: %d, handle:%p.",
                 serviceName, uiSessionId, l_pClientHandle);
        }
      }
    }
  }

  if (NULL == l_pClientHandle) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, "Handle NULL for service: %s, session: %d.",
           serviceName != 0 ? serviceName : NULL, uiSessionId);
  }

  return l_pClientHandle;
}

//////////////////////////////////////////
// Function : GetNewHandleAndUpdateEventInfoTable
//////////////////////////////////////////
HANDLE GetNewHandleAndUpdateEventInfoTable(HANDLE hApp, PCSTR serviceName, const UI_32 uiSessionId,
                                           const UI_32 uiEventId) {
  HANDLE l_pClientHandle = NULL;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

    ServiceSessionHandleTable::iterator ssh_iterator;;
    SessionToEventInfo::iterator sh_iterator;

    ssh_iterator = pApp->servicesessionhandletable.find(serviceName);

    if (pApp->servicesessionhandletable.end() == ssh_iterator) {
      SessionToEventInfo l_mSessionToEventInfoTable;
      pApp->servicesessionhandletable.insert(make_pair(serviceName, l_mSessionToEventInfoTable));
    }

    sh_iterator = pApp->servicesessionhandletable[serviceName].find(uiSessionId);

    if (pApp->servicesessionhandletable[serviceName].end() == sh_iterator) {
      l_pClientHandle = FrameworkunifiedMcOpenSender(hApp, serviceName);
      if (NULL != l_pClientHandle) {
        (reinterpret_cast<MsgQInfo *>(l_pClientHandle))->sessionId = uiSessionId;
      }
      SessionEventInfo *l_ptSessionEventInfo = new(std::nothrow) SessionEventInfo;
      if (NULL != l_ptSessionEventInfo) {  // LCOV_EXCL_BR_LINE 5: new's error case.
        l_ptSessionEventInfo->m_hSession = l_pClientHandle;
        l_ptSessionEventInfo->m_vEvents.push_back(uiEventId);
        pApp->servicesessionhandletable[serviceName].insert(std::make_pair(uiSessionId, l_ptSessionEventInfo));
      } else {
        // LCOV_EXCL_START 5: malloc's error case.
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error(NULL) createing new session event info for "
               "service:%s, session:%d, event:%d.",
               serviceName, uiSessionId, uiEventId);
        // LCOV_EXCL_STOP
      }
    } else {
      SessionEventInfo *l_ptSessionEventInfo = sh_iterator->second;
      if (NULL != l_ptSessionEventInfo) {
        if (NULL == l_ptSessionEventInfo->m_hSession) {
          l_ptSessionEventInfo->m_hSession = FrameworkunifiedMcOpenSender(hApp, serviceName);
          if (NULL != l_ptSessionEventInfo->m_hSession) {
            (reinterpret_cast<MsgQInfo *>(l_ptSessionEventInfo->m_hSession))->sessionId = uiSessionId;
          }
        }
        l_pClientHandle = l_ptSessionEventInfo->m_hSession;
        l_ptSessionEventInfo->m_vEvents.push_back(uiEventId);
      }
    }
  }
  return l_pClientHandle;
}

//////////////////////////////////////////
// Function : UnregisterPrivateEvents
//////////////////////////////////////////
EFrameworkunifiedStatus UnregisterPrivateEvents(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

    // get the name of the service from which message is received
    PCSTR l_cServiceName = pApp->cMsgSrcName;
    const UI_32 l_uiSessionId = pApp->uiSessionId;

    // number of events to unregister
    UI_32 l_uiListSize = static_cast<UI_32>(FrameworkunifiedGetMsgLength(hApp) / sizeof(UI_32));

    // check if the received service name and data is valid
    if (NULL != l_cServiceName && 0 < l_uiListSize) {
      PUI_32 l_pData = new(std::nothrow) UI_32[l_uiListSize];
      UI_32 l_uiEventId = 0;

      if (NULL != l_pData) {  // LCOV_EXCL_BR_LINE 5: new's error case.
        // get the list of events
        eStatus = FrameworkunifiedGetMsgDataOfSize(hApp, l_pData, static_cast<UI_32>(l_uiListSize * sizeof(UI_32)));

        if (eFrameworkunifiedStatusOK == eStatus) {
          for (UI_32 uiCount = 0; uiCount < l_uiListSize; uiCount++) {
            // received event id
            l_uiEventId = l_pData[uiCount];

            // remove the entry from map of Events and Session id's of services
            eStatus = RemoveEventEntryFromEventTable(hApp, l_uiEventId, l_cServiceName, l_uiSessionId);
            (VOID)RemoveEntryFromSessionEventInfoTable(hApp, l_cServiceName, l_uiSessionId, l_uiEventId);
          }
        } else {
          eStatus = eFrameworkunifiedStatusInvldBuf;
          FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error :: %d, PROTOCOL_REGISTER_EVENTS Invalid data ", eStatus);
        }
        delete[] l_pData;
        l_pData = NULL;
      } else {
        // LCOV_EXCL_START 5: new's error case.
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        eStatus = eFrameworkunifiedStatusNullPointer;
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error :: PROTOCOL_UNREGISTER_EVENTS: "
          "Unable to allocate memory for l_pData");
        // LCOV_EXCL_STOP
      }
    } else {
      eStatus = eFrameworkunifiedStatusInvldParam;
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error :: %d, PROTOCOL_UNREGISTER_EVENTS Service Name invalid ", eStatus);
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error :: %d, Invalid hApp ", eStatus);
  }

  return eStatus;
}

//////////////////////////////////////////
// Function : HandleSessionEventOnCloseSession
//////////////////////////////////////////
EFrameworkunifiedStatus HandleSessionEventOnCloseSession(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    // close session request structure
    CloseSessionReq tClose = {};
    if (FrameworkunifiedGetMsgLength(hApp) == sizeof(tClose)) {
      if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedGetMsgDataOfSize(hApp, &tClose, sizeof(tClose)))) {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error :: %d, FrameworkunifiedGetMsgDataOfSize Failed ", eStatus);
      } else {
        CleanAllEventsOfSession(hApp, pApp->cMsgSrcName, tClose.sessionId);
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Invalid data length received:: Expected: %ld, Received: %d",
             static_cast<long int>(sizeof(tClose)), FrameworkunifiedGetMsgLength(hApp));  // NOLINT  (readability/nolint)
      eStatus = eFrameworkunifiedStatusFail;
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Invalid application handle status.");
  }

  return eStatus;
}

//////////////////////////////////////////
// Function : FrameworkunifiedOnDummy
//////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOnDummy(HANDLE hApp) {
  return eFrameworkunifiedStatusOK;
}

#ifdef DISPATCHER_PROFILER
EFrameworkunifiedStatus FrameworkunifiedAttachMsgProfilerCallbacksDispatcher(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    if (NULL != pApp->m_pFrameworkunifiedMsgProfiler) {
      eStatus = FrameworkunifiedAttachCallbackToDispatcher(hApp,
                                              "NSProfileUtil",
                                              PROTOCOL_DIS_MSGPROFILER,
                                              boost::bind(&FrameworkunifiedMsgProfiler::PrintProfileInfo,
                                              pApp->m_pFrameworkunifiedMsgProfiler, hApp));
      if (eFrameworkunifiedStatusOK == eStatus) {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, "", "FrameworkunifiedAttachCallbackToDispatcher success for PROTOCOL_DIS_MSGPROFILER.");
      }
    }
  }
  return eStatus;
}

EFrameworkunifiedStatus FrameworkunifiedAttachChildMsgProfilerCallbacksDispatcher(HANDLE hChildApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, "", "+");
  if (frameworkunifiedCheckValidAppHandle(hChildApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hChildApp);
    if (NULL != pApp->m_pFrameworkunifiedMsgProfiler) {
      eStatus = FrameworkunifiedAttachCallbackToDispatcher(hChildApp,
                                              pApp->cParentAppName,
                                              PROTOCOL_DIS_MSGPROFILER,
                                              boost::bind(&FrameworkunifiedMsgProfiler::PrintProfileInfo,
                                              pApp->m_pFrameworkunifiedMsgProfiler, hChildApp));
      if (eFrameworkunifiedStatusOK == eStatus) {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, "", "FrameworkunifiedAttachCallbackToDispatcher success for PROTOCOL_DIS_MSGPROFILER.");
      }
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, "", "-");
  return eStatus;
}
#endif
// EOF
