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
/// \brief    Framework service protocol session APIs implementation
///
///
///
///////////////////////////////////////////////////////////////////////////////
#include <string.h>

#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_framework_types.h>
#include <native_service/frameworkunified_service_protocol.h>
#include <native_service/ns_logger_if.h>
#include <other_service/strlcpy.h>
#include <utility>
#include "frameworkunified_framework_core.h"
#include "frameworkunified_framework_internal.h"

//////////////////////////////////////////////////////
/// FrameworkunifiedOpenService
//////////////////////////////////////////////////////
HANDLE FrameworkunifiedOpenService(HANDLE hApp, PCSTR pServiceName) {
  HANDLE hService = NULL;

  if (frameworkunifiedCheckValidAppHandle(hApp) && NULL != pServiceName) {
    if (NULL == (hService = FrameworkunifiedMcOpenSender(hApp, pServiceName))) {
      // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error::hApp:0x%p, %s failed to FrameworkunifiedMcOpenSender", hApp, pServiceName);
      // LCOV_EXCL_BR_STOP
    } else {
      CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
      frameworkunifiedAddConnectMonitor(hApp, pServiceName, pApp->cAppName);
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__,
           "Invalid param. hApp:0x%p, name:0x%p",
           hApp, pServiceName);
  }

  return static_cast<HANDLE>(hService);
}

//////////////////////////////////////////////////////
/// FrameworkunifiedCloseService
//////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedCloseService(HANDLE hApp, HANDLE hService) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && frameworkunifiedCheckValidMsgQ(hService)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    MsgQInfo *pMsgQ = static_cast<MsgQInfo *>(hService);

    if (eFrameworkunifiedStatusOK != frameworkunifiedDelConnectMonitor(hApp, pMsgQ->cMsgQName, pApp->cAppName)) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error : frameworkunifiedDelConnectMonitor(%s, %s) fail",
             pMsgQ->cMsgQName != 0 ? pMsgQ->cMsgQName : NULL, pApp->cAppName != 0 ? pApp->cAppName : NULL);
    }

    if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedMcClose(hService))) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error : %s Failed to FrameworkunifiedMcClose",
             pApp->cAppName != 0 ? pApp->cAppName : NULL);
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  return eStatus;
}

HANDLE FrameworkunifiedGenerateSessionHandle(HANDLE hApp, PCSTR pServiceName) {
  HANDLE l_pSession = NULL;
  if ((frameworkunifiedCheckValidAppHandle(hApp)) && (NULL != pServiceName)) {
    l_pSession = FrameworkunifiedMcOpenSender(hApp, pServiceName);

    if (NULL != l_pSession) {
      MsgQInfo *pMsgQ    = reinterpret_cast<MsgQInfo *>(l_pSession);
      pMsgQ->sessionId = FrameworkunifiedGenerateNewSessionId();
    }
  }

  return l_pSession;
}

//////////////////////////////////////////////////////
/// FrameworkunifiedOpenSessionWithData
//////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOpenSessionWithData(HANDLE hService, PVOID pData, UI_32 length) {
  return FrameworkunifiedSendMsg(hService, PROTOCOL_OPEN_SESSION_REQ, length, pData);
}


//////////////////////////////////////////////////////
/// FrameworkunifiedOpenSession
//////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOpenSession(HANDLE hService) {
  UI_32 l_uiDummy = 0;
  return FrameworkunifiedSendMsg(hService, PROTOCOL_OPEN_SESSION_REQ, 0, &l_uiDummy);
}

//////////////////////////////////////////////////////
/// FrameworkunifiedOpenSessionWithDataSync
//////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOpenSessionWithDataSync(HANDLE hService, PVOID pData, UI_32 length, OpenSessionAck *ack) {
  UI_32 rcvLen;
  return FrameworkunifiedInvokeSync(hService, PROTOCOL_OPEN_SESSION_REQ_SYNC, length, pData, sizeof(OpenSessionAck), ack, &rcvLen);
}


//////////////////////////////////////////////////////
/// FrameworkunifiedOpenSessionSync
//////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOpenSessionSync(HANDLE hService, OpenSessionAck *ack) {
  UI_32 l_uiDummy = 0;
  UI_32 rcvLen;
  return FrameworkunifiedInvokeSync(hService, PROTOCOL_OPEN_SESSION_REQ_SYNC, 0, &l_uiDummy, sizeof(OpenSessionAck), ack, &rcvLen);
}

//////////////////////////////////////////////////////
/// FrameworkunifiedGetSessionHandle
//////////////////////////////////////////////////////
HANDLE FrameworkunifiedGetOpenSessionHandle(HANDLE hApp) {
  HANDLE hSession = NULL;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    OpenSessionAck tAck;

    if (sizeof(OpenSessionAck) == FrameworkunifiedGetMsgLength(hApp)) {
      if (eFrameworkunifiedStatusOK != FrameworkunifiedGetMsgDataOfSize(hApp, &tAck, sizeof(tAck))) {
          FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error : FrameworkunifiedGetMsgDataOfSize");
      } else {
        if (eFrameworkunifiedStatusOK == tAck.eStatus) {
          hSession = FrameworkunifiedMcOpenSender(hApp, tAck.cSessionName);
          if (NULL != hSession) {
            (reinterpret_cast<MsgQInfo *>(hSession))->sessionId = tAck.sessionId;
          }
        }
      }
    } else {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error : FrameworkunifiedGetOpenSessionHandle");
    }
  }

  return hSession;
}

//////////////////////////////////////////////////////
/// FrameworkunifiedGetOpenSessionSyncHandle
//////////////////////////////////////////////////////
HANDLE FrameworkunifiedGetOpenSessionSyncHandle(HANDLE hApp, OpenSessionAck *tAck) {
  HANDLE hSession = NULL;

  if (frameworkunifiedCheckValidAppHandle(hApp) && (NULL != tAck)) {
    if (eFrameworkunifiedStatusOK == tAck->eStatus) {
      hSession = FrameworkunifiedMcOpenSender(hApp, tAck->cSessionName);
      if (NULL != hSession) {
        (reinterpret_cast<MsgQInfo *>(hSession))->sessionId = tAck->sessionId;
      }
    }
  }

  return hSession;
}

//////////////////////////////////////////////////////
/// frameworkunifiedCloseSessionInner
//////////////////////////////////////////////////////
static EFrameworkunifiedStatus frameworkunifiedCloseSessionInner(HANDLE hService, HANDLE hSession, CloseSessionAck *ack) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusInvldHandle;
  CloseSessionReq tClose = {};

  if (frameworkunifiedCheckValidMsgQ(hService) && frameworkunifiedCheckValidMsgQ(hSession)) {
    tClose.sessionId = (reinterpret_cast<MsgQInfo *>(hSession))->sessionId;
    strlcpy(tClose.cSessionName, (reinterpret_cast<MsgQInfo *>(hSession))->cMsgQName, sizeof(tClose.cSessionName));

    FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "Info : sessionid %d", tClose.sessionId);

    if (NULL != ack) {
      UI_32 rcvLen;
      eStatus = FrameworkunifiedInvokeSync(hService, PROTOCOL_CLOSE_SESSION_REQ_SYNC, sizeof(tClose), (PVOID)&tClose,
                              sizeof(CloseSessionAck), ack, &rcvLen);
      if (rcvLen != sizeof(CloseSessionAck)) {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error : Invalid receive length %d", rcvLen);
      }
    } else {
      eStatus = FrameworkunifiedSendMsg(hService, PROTOCOL_CLOSE_SESSION_REQ, sizeof(tClose), (PVOID)&tClose);
    }
    if (eFrameworkunifiedStatusOK == eStatus) {
      eStatus = FrameworkunifiedMcClose(hSession);
      hSession = NULL;
    }
  }
  return eStatus;
}

//////////////////////////////////////////////////////
/// FrameworkunifiedCloseSession
//////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedCloseSession(HANDLE hService, HANDLE hSession) {
  return frameworkunifiedCloseSessionInner(hService, hSession, NULL);
}

//////////////////////////////////////////////////////
/// FrameworkunifiedCloseSessionSync
//////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedCloseSessionSync(HANDLE hService, HANDLE hSession, CloseSessionAck *ack) {
  if (NULL == ack) {
    return eFrameworkunifiedStatusInvldParam;
  }
  return frameworkunifiedCloseSessionInner(hService, hSession, ack);
}

//////////////////////////////////////////////////////
/// FrameworkunifiedGetSessionId -> On Client side
//////////////////////////////////////////////////////
UI_32 FrameworkunifiedGetSessionId(HANDLE hSession) {
  if (frameworkunifiedCheckValidMsgQ(hSession)) {
    return ((reinterpret_cast<MsgQInfo *>(hSession))->sessionId);
  } else {
    return MAX_SESSION_ID_VAL;
  }
}

//////////////////////////////////////////////////////
/// FrameworkunifiedGetSessionId -> On Client side
//////////////////////////////////////////////////////
UI_32 FrameworkunifiedGetMsgSessionId(HANDLE hApp) {
  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    return pApp->uiSessionId;
  }
  return MAX_SESSION_ID_VAL;
}


//////////////////////////////////////////////////////
/// GenerateNewSessionId -> On Server side
//////////////////////////////////////////////////////
UI_32 FrameworkunifiedGenerateNewSessionId() {
  static UI_16 lastSessionId = 0;
  lastSessionId++;
  UI_32 sessionId = (lastSessionId % MAX_SESSION_ID_VAL);
  return sessionId;
}



//////////////////////////////////////////////////////
/// FrameworkunifiedRegisterEvents
//////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedRegisterEvents(HANDLE hSession, PVOID puiEventsArray, UI_32 uiListSize) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (hSession) {
    eStatus = FrameworkunifiedSendMsg(hSession, PROTOCOL_REGISTER_EVENTS,
      static_cast<UI_32>(uiListSize * sizeof(uiListSize)), puiEventsArray);
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  return eStatus;
}

//////////////////////////////////////////////////////
/// FrameworkunifiedRegisterEvent
//////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedRegisterEvent(HANDLE hSession, UI_32 uiEvent) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (hSession) {
    eStatus = FrameworkunifiedSendMsg(hSession, PROTOCOL_REGISTER_EVENTS, sizeof(uiEvent), (PVOID)&uiEvent);
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  return eStatus;
}

//////////////////////////////////////////////////////
/// FrameworkunifiedUnRegisterEvents
//////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedUnRegisterEvents(HANDLE hSession, PVOID puiEventsArray, UI_32 uiListSize) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (hSession) {
    eStatus = FrameworkunifiedSendMsg(hSession, PROTOCOL_UNREGISTER_EVENTS,
      static_cast<UI_32>(uiListSize * sizeof(uiListSize)), puiEventsArray);
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  return eStatus;
}

//////////////////////////////////////////////////////
/// FrameworkunifiedUnRegisterEvent
//////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedUnRegisterEvent(HANDLE hSession, UI_32 uiEvent) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (hSession) {
    eStatus = FrameworkunifiedSendMsg(hSession, PROTOCOL_UNREGISTER_EVENTS, sizeof(uiEvent), (PVOID)&uiEvent);
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  return eStatus;
}

//////////////////////////////////////////////////////
/// FrameworkunifiedDefineStateEvents
//////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedDefineStateEvents(HANDLE hApp, PVOID puiEvents, UI_32 uiListSize) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && puiEvents) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

    EventData *l_pEventData = NULL;

    UI_32 l_uiEventId;
    PUI_32 l_pEventList = (PUI_32)puiEvents;

    for (UI_32 uiCount = 0; uiCount < uiListSize; uiCount++) {
      l_uiEventId = l_pEventList[uiCount];

      // checks if event exists in state event list
      if (pApp->publicstateeventtable.end() == pApp->publicstateeventtable.find(l_uiEventId)) {
        // insert the event in state event list, data is passed as NULL as there is no data published
        pApp->publicstateeventtable.insert(std::make_pair(l_uiEventId, l_pEventData));
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "Event %d already registered as state event ", l_uiEventId);
      }
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  return eStatus;
}

//////////////////////////////////////////////////////
/// FrameworkunifiedBroadcastEvent
//////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedBroadcastEvent(HANDLE hApp, UI_32 uiEventId, PCVOID pData, UI_32 uiLength) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    eStatus = FrameworkunifiedPublishEvent(hApp, uiEventId, NULL, pData, uiLength);
  } else {
    eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return eStatus;
}

//////////////////////////////////////////////////////
/// FrameworkunifiedPublishEvent
//////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedPublishEvent(HANDLE hApp, UI_32 uiEventId, PCSTR pClientName, PCVOID pData, UI_32 uiLength) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  EFrameworkunifiedStatus eRetStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    EventTable::iterator e_iterator;
    ServiceSessionIdListTable::iterator ssidl_iterator;

    PCSTR l_cServiceName = "";

    // set the data, if event is a state events
    UpdatePublicStateEventData(hApp, uiEventId, pData, uiLength);

    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

    // check if event is registered by client/s
    e_iterator = pApp->eventtable.find(uiEventId);

    if (pApp->eventtable.end() != e_iterator) {
      // for loop for event table
      for (ssidl_iterator = e_iterator->second.begin();
           ssidl_iterator != e_iterator->second.end();
           ssidl_iterator++) {
        l_cServiceName = ssidl_iterator->first.c_str();

        if (NULL != pClientName) {
          // send message to only specified client
          if (!std::strcmp(pClientName, l_cServiceName)) {
            eStatus = SendEventMessage(hApp, ssidl_iterator->second, uiEventId, l_cServiceName, pData, uiLength);
            break;
          }
        } else {
          // send message to all registered client
          eRetStatus = SendEventMessage(hApp, ssidl_iterator->second, uiEventId, l_cServiceName, pData, uiLength);
          if (eFrameworkunifiedStatusOK != eRetStatus) {
            eStatus = eRetStatus;
          }
        }
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_IMP_INFO, __FUNCTION__, "No subscriber has subscribed for event %d.", uiEventId);
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  return eStatus;
}

//////////////////////////////////////////////////////
/// UpdatePublicStateEventData
//////////////////////////////////////////////////////
EFrameworkunifiedStatus UpdatePublicStateEventData(HANDLE hApp, UI_32 uiEventId, PCVOID pData, UI_32 uiLength) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    if ((NULL == pData) && (uiLength > 0)) {
      eStatus = eFrameworkunifiedStatusInvldParam;
    } else {
      PublicStateEventTable::iterator se_iterator;
      EventData *l_pEventData = NULL;

      CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

      // set the data, if event is a state events
      se_iterator = pApp->publicstateeventtable.find(uiEventId);
      if (pApp->publicstateeventtable.end() != se_iterator) {
        l_pEventData = se_iterator->second;
        if (NULL != l_pEventData) {
          if (NULL != l_pEventData->pData) {
            delete[](static_cast<PCHAR>(l_pEventData->pData));
            (se_iterator->second)->pData = NULL;
          }

          delete l_pEventData;
          se_iterator->second = NULL;
        }

        l_pEventData = new(std::nothrow) EventData();
        /*
         * @todo
         * UpdatePublicStateEventData() in the pApp->publicstateeventtable
         * The data area is being created, but there is no process to release.
         */
        if (NULL != l_pEventData) {
          l_pEventData->uiLength = uiLength;
          // LCOV_EXCL_BR_START 6:pData is checked in the top of this function, it should be not NULL here.
          if (NULL == pData) {
          // LCOV_EXCL_BR_STOP
            // LCOV_EXCL_START 6:pData is checked in the top of this function, it should be not NULL here.
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            l_pEventData->pData = NULL;
            // LCOV_EXCL_STOP
          } else {
            l_pEventData->pData = new(std::nothrow) CHAR[uiLength];
            if (NULL != l_pEventData->pData) {
              std::memset(l_pEventData->pData, 0, uiLength);
              std::memcpy(l_pEventData->pData, pData, uiLength);
            } else {
              eStatus = eFrameworkunifiedStatusNullPointer;
            }
          }

          se_iterator->second = l_pEventData;
        } else {
          eStatus = eFrameworkunifiedStatusNullPointer;
        }
      }
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

//////////////////////////////////////////////////////
/// SendEventMessage
//////////////////////////////////////////////////////
EFrameworkunifiedStatus SendEventMessage(HANDLE hApp,
                            const SessionIdList &vSessionIdList,
                            UI_32 uiEventId,
                            PCSTR cServiceName,
                            PCVOID pData,
                            UI_32 uiLength) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  EFrameworkunifiedStatus eStatusSendMsg = eFrameworkunifiedStatusOK;

  UI_32 l_uiSessionId = 0;
  HANDLE l_hClientHandle = NULL;

  if ((frameworkunifiedCheckValidAppHandle(hApp)) && (NULL != cServiceName)) {
    for (UI_32 l_uiCount = 0;
         l_uiCount < vSessionIdList.size();
         l_uiCount++) {
      l_uiSessionId = vSessionIdList[l_uiCount];
      l_hClientHandle = GetMsgQueueHandle(hApp, cServiceName, l_uiSessionId);

      // send message to all the registered sessions of the service
      if (NULL != l_hClientHandle) {
        if (eFrameworkunifiedStatusOK != (eStatusSendMsg = FrameworkunifiedSendMsg(l_hClientHandle, uiEventId, uiLength, pData))) {
          eStatus = eFrameworkunifiedStatusErrOther;
          FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, "FrameworkunifiedSendMsg failed for EventID %d to service %s, status %d",
                 uiEventId, cServiceName, eStatusSendMsg);
        }
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, "Unable to send event %d to service %s, session %d. NULL handle found.",
               uiEventId, cServiceName, l_uiSessionId);
        eStatus = eFrameworkunifiedStatusErrOther;
      }
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

//////////////////////////////////////////////////////
/// RemoveEventEntryFromEventTable
//////////////////////////////////////////////////////
EFrameworkunifiedStatus RemoveEventEntryFromEventTable(HANDLE hApp, UI_32 uiEventId,
                                          PCSTR cServiceName, const UI_32 uiSessionId) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if ((frameworkunifiedCheckValidAppHandle(hApp)) && (NULL != cServiceName)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

    EventTable::iterator e_iterator;
    ServiceSessionIdListTable::iterator ssidl_iterator;

    // event table = event id, service name, vector of session ids
    // check if event is registered by client/s
    e_iterator = pApp->eventtable.find(uiEventId);

    if (pApp->eventtable.end() != e_iterator) {
      ssidl_iterator = e_iterator->second.find(cServiceName);

      if (e_iterator->second.end() != ssidl_iterator) {
        // search for the session id in list of registered session of service with the event uiEventId
        for (UI_32 l_uiCount = 0;
             l_uiCount < ssidl_iterator->second.size();
             l_uiCount++) {
          if (uiSessionId == ssidl_iterator->second[l_uiCount]) {
            ssidl_iterator->second.erase(ssidl_iterator->second.begin() + l_uiCount);
            break;
          }
        }

        // no other session of service cServiceName is registered with the event uiEventId
        if (ssidl_iterator->second.empty()) {
          e_iterator->second.erase(ssidl_iterator);

          // no other session is registered for the event uiEventId
          if (e_iterator->second.empty()) {
            pApp->eventtable.erase(e_iterator);
          }
        }
      }
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

////////////////////////////////////////////
// Function : CleanAllEventsOfSession
////////////////////////////////////////////
EFrameworkunifiedStatus CleanAllEventsOfSession(HANDLE hApp, PCSTR serviceName, UI_32 sessionId) {
  FRAMEWORKUNIFIEDLOG_CUT(ZONE_NS_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if ((frameworkunifiedCheckValidAppHandle(hApp)) && (NULL != serviceName)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

    ServiceSessionHandleTable::iterator ssh_iterator;
    SessionToEventInfo::iterator sh_iterator;
    HANDLE l_pClientHandle = NULL;
    SessionEventInfo *l_ptSessionEventInfo = NULL;

    // check whether client has registered for private event/s
    ssh_iterator = pApp->servicesessionhandletable.find(serviceName);

    if (pApp->servicesessionhandletable.end() != ssh_iterator) {
      // get the table of session id and session handle of the received service over which private events are
      // registered
      sh_iterator =  ssh_iterator->second.find(sessionId);

      // client has registered for private event/s over this session
      if (ssh_iterator->second.end() != sh_iterator) {
        l_ptSessionEventInfo = sh_iterator->second;

        if (NULL != l_ptSessionEventInfo) {
          l_pClientHandle = l_ptSessionEventInfo->m_hSession;
          if (NULL != l_pClientHandle) {
            // close the session handle
            eStatus = FrameworkunifiedMcClose(l_pClientHandle);
            l_ptSessionEventInfo->m_hSession = NULL;
          }
          // Remove all the event entries from Event registry table
          for (UI_32 l_uiCount = 0;
               l_uiCount < l_ptSessionEventInfo->m_vEvents.size();
               l_uiCount++) {
            (VOID)RemoveEventEntryFromEventTable(hApp,
                                                 l_ptSessionEventInfo->m_vEvents[l_uiCount],
                                                 serviceName,
                                                 sessionId);
          }
          l_ptSessionEventInfo->m_vEvents.clear();  // clear the event list
          delete l_ptSessionEventInfo;
          sh_iterator->second = NULL;  // l_ptSessionEventInfo
        }

        FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "Deleting session event info entry for service: %s, session id: %d.",
               serviceName, sessionId);
        // remove the entry from SessionHandle table
        ssh_iterator->second.erase(sh_iterator);

        // remove the entry from ServiceSessionHandle table if this service have not registered for
        // private events over other session
        if (ssh_iterator->second.empty()) {
          pApp->servicesessionhandletable.erase(ssh_iterator);
        }
      }
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Invalid application handle or service name NULL");
  }
  FRAMEWORKUNIFIEDLOG_CUT(ZONE_NS_FUNC, __FUNCTION__, "-");
  return eStatus;
}

//////////////////////////////////////////////////////
/// RemoveEntryFromSessionEventInfoTable
//////////////////////////////////////////////////////
EFrameworkunifiedStatus RemoveEntryFromSessionEventInfoTable(HANDLE hApp, PCSTR serviceName, UI_32 sessionId, UI_32 eventId) {
  FRAMEWORKUNIFIEDLOG_CUT(ZONE_NS_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if ((frameworkunifiedCheckValidAppHandle(hApp)) && (NULL != serviceName)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

    ServiceSessionHandleTable::iterator ssh_iterator;
    SessionToEventInfo::iterator sh_iterator;
    HANDLE l_pClientHandle = NULL;
    SessionEventInfo *l_ptSessionEventInfo = NULL;

    // check whether client has registered for private event/s
    ssh_iterator = pApp->servicesessionhandletable.find(serviceName);

    if (pApp->servicesessionhandletable.end() != ssh_iterator) {
      // get the table of session id and session handle of the received service over which private events are
      // registered
      sh_iterator =  ssh_iterator->second.find(sessionId);

      // client has registered for private event/s over this session
      if (ssh_iterator->second.end() != sh_iterator) {
        l_ptSessionEventInfo = sh_iterator->second;

        if (NULL != l_ptSessionEventInfo) {
          // Remove event entry from session event info table
          for (UI_32 l_uiCount = 0;
               l_uiCount < l_ptSessionEventInfo->m_vEvents.size();
               l_uiCount++) {
            if (eventId == l_ptSessionEventInfo->m_vEvents[l_uiCount]) {
              FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "Removing session event info entry for "
                     "service:%s, session id:%d. event:%d",
                     serviceName, sessionId, eventId);
              l_ptSessionEventInfo->m_vEvents.erase(l_ptSessionEventInfo->m_vEvents.begin() + l_uiCount);
              break;
            }
          }

          if (l_ptSessionEventInfo->m_vEvents.empty()) {
            l_pClientHandle = l_ptSessionEventInfo->m_hSession;
            if (NULL != l_pClientHandle) {
              // close the session handle
              eStatus = FrameworkunifiedMcClose(l_pClientHandle);
              l_ptSessionEventInfo->m_hSession = NULL;
            }
            delete l_ptSessionEventInfo;
            sh_iterator->second = NULL;  // l_ptSessionEventInfo

            // remove the entry from SessionHandle table
            ssh_iterator->second.erase(sh_iterator);
          }
        }

        // remove the entry from ServiceSessionHandle table if this service have not registered for
        // private events over other session
        if (ssh_iterator->second.empty()) {
          pApp->servicesessionhandletable.erase(ssh_iterator);
        }
      }
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Invalid application handle status:: %d", eStatus);
  }

  FRAMEWORKUNIFIEDLOG_CUT(ZONE_NS_FUNC, __FUNCTION__, "-");
  return eStatus;
}


//////////////////////////////////////////////////////
/// FrameworkunifiedDefinePublicStateEvents
//////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedDefinePublicStateEvents(HANDLE hApp, PUI_32 puiEvents, UI_32 uiListSize) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && NULL != puiEvents) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

    UI_32 l_uiEventId;

    for (UI_32 uiCount = 0; uiCount < uiListSize; uiCount++) {
      l_uiEventId = puiEvents[uiCount];

      // checks if event exists in state event list
      if (pApp->publicstateeventtable.end() == pApp->publicstateeventtable.find(l_uiEventId)) {
        EventData *l_pEventData = NULL;
        // insert the event in state event list, data is passed as NULL as there is no data published
        pApp->publicstateeventtable.insert(std::make_pair(l_uiEventId, l_pEventData));
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "Event %d already registered as public state event ", l_uiEventId);
      }
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  return eStatus;
}

//////////////////////////////////////////////////////
/// FrameworkunifiedDefinePrivateStateEvents
//////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedDefinePrivateStateEvents(HANDLE hApp, PUI_32 puiEvents, UI_32 uiListSize) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && NULL != puiEvents) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

    UI_32 l_uiEventId = 0;

    for (UI_32 uiCount = 0; uiCount < uiListSize; uiCount++) {
      l_uiEventId = puiEvents[uiCount];

      // checks if event exists in state event list
      if (pApp->privatestateeventtable.end() == pApp->privatestateeventtable.find(l_uiEventId)) {
        ServiceSessionEventData servicesessioneventdata;
        pApp->privatestateeventtable.insert(std::make_pair(l_uiEventId, servicesessioneventdata));
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "Event %d already registered as private state event", l_uiEventId);
      }
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  return eStatus;
}

//////////////////////////////////////////////////////
/// FrameworkunifiedSubscribeToSessionEventWithCallback
//////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSubscribeToSessionEventWithCallback(HANDLE hApp, UI_32 uiCmd, CbFuncPtr fpOnCmd, HANDLE hSession) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && frameworkunifiedCheckValidMsgQ(hSession)) {
    if (eFrameworkunifiedStatusOK == (eStatus = FrameworkunifiedAttachCallbackToDispatcher(hApp,
                                                                 (reinterpret_cast<MsgQInfo *>(hSession))->cMsgQName,
                                                                 uiCmd,
                                                                 fpOnCmd,
                                                                 hSession))) {
      eStatus = FrameworkunifiedSendMsg(hSession, PROTOCOL_REGISTER_EVENTS, sizeof(UI_32), (PVOID)&uiCmd);
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "FrameworkunifiedAttachCallbackToDispatcher for "
        "PROTOCOL_REGISTER_EVENTS Failed Status:: %d", eStatus);
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  return eStatus;
}

//////////////////////////////////////////////////////
/// FrameworkunifiedSubscribeToSessionEventsWithCallbacks
//////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSubscribeToSessionEventsWithCallbacks(HANDLE hApp, const FrameworkunifiedProtocolCallbackHandler *pMsgHandler,
                                                    UI_32 uiHandlerCount, HANDLE hSession) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (NULL == pMsgHandler) {
    eStatus = eFrameworkunifiedStatusInvldParam;
  } else if (frameworkunifiedCheckValidAppHandle(hApp) && frameworkunifiedCheckValidMsgQ(hSession)) {
    /**
     * @todo
     *  If the CbFuncPtr in the pMsgHandler is set to NULL, the expected value is eFrameworkunifiedStatusInvldParam,
     *  but eFrameworkunifiedStatusOK is returned in the implementation.
     *  [Proposed measures]
     *  Exit by referring to the return code of the FrameworkunifiedAttachCallbacksToDispatcher executed in the FrameworkunifiedSubscribeToSessionEventsWithCallbacks.
     */
    /**
     * @todo
     *  The behavior when NULL is set for a CbFuncPtr in a pMsgHandler in which more than one callback message is registered
     *  is not specified in the specifications.
     *  Ex: Prepare a pMsgHandler in which three callback information are registered,
     *      and specify NULL as the callback function pointer of the second callback information, and execute it.
     *  - Register the first callback function.
     *  - The second callback function is not registered with an error.
     *  - It is unspecified whether the third callback function is registered.
     *  [Proposed measures]
     *  Specifies that if there is callback information including NULL in the callback function pointer in pMsgHandler,
     *  the callback information will not be registered from the element following that callback function pointer.
     */
    eStatus = FrameworkunifiedAttachCallbacksToDispatcher(hApp,
                                             (reinterpret_cast<MsgQInfo *>(hSession))->cMsgQName,
                                             pMsgHandler,
                                             uiHandlerCount,
                                             hSession);
    UI_32 l_uiCmdList[uiHandlerCount];  // NOLINT  (readability/nolint)
    for (UI_32 l_uiCnt = 0; l_uiCnt < uiHandlerCount; l_uiCnt++) {
      l_uiCmdList[l_uiCnt] = pMsgHandler[l_uiCnt].iCmd;
    }

    eStatus = FrameworkunifiedSendMsg(hSession, PROTOCOL_REGISTER_EVENTS,
      static_cast<UI_32>(uiHandlerCount * sizeof(UI_32)), l_uiCmdList);
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  return eStatus;
}

//////////////////////////////////////////////////////
/// FrameworkunifiedUnSubscribeSessionEventWithCallback
//////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedUnSubscribeSessionEventWithCallback(HANDLE hApp, UI_32 uiEvent, HANDLE hSession) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && frameworkunifiedCheckValidMsgQ(hSession)) {
    eStatus = FrameworkunifiedSendMsg(hSession, PROTOCOL_UNREGISTER_EVENTS, sizeof(uiEvent), (PVOID)&uiEvent);
    if (eFrameworkunifiedStatusOK == eStatus) {
      if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedDetachCallbackFromDispatcher(hApp,
                                                              (reinterpret_cast<MsgQInfo *>(hSession))->cMsgQName,
                                                              uiEvent,
                                                              hSession))) {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error : FrameworkunifiedDetachCallbackFromDispatcher failed status:: %d", eStatus);
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error : Failed to send PROTOCOL_UNREGISTER_EVENTS request to service:: %d",
        eStatus);
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  return eStatus;
}

//////////////////////////////////////////////////////
/// FrameworkunifiedUnSubscribeSessionEventsWithCallbacks
//////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedUnSubscribeSessionEventsWithCallbacks(HANDLE hApp, PUI_32 puiEventsArray, UI_32 uiListSize,
                                                    HANDLE hSession) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && frameworkunifiedCheckValidMsgQ(hSession)) {
    eStatus = FrameworkunifiedSendMsg(hSession, PROTOCOL_UNREGISTER_EVENTS, static_cast<UI_32>(uiListSize * sizeof(uiListSize)),
                        puiEventsArray);
    if (eFrameworkunifiedStatusOK == eStatus) {
      if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedDetachCallbacksFromDispatcher(hApp,
                                                              (reinterpret_cast<MsgQInfo *>(hSession))->cMsgQName,
                                                              puiEventsArray,
                                                              uiListSize,
                                                              hSession))) {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error : FrameworkunifiedDetachCallbackFromDispatcher failed status:: %d", eStatus);
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error : Failed to send PROTOCOL_UNREGISTER_EVENTS request to service:: %d",
                              eStatus);
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  return eStatus;
}

//////////////////////////////////////////////////////
/// FrameworkunifiedPublishPrivateEvent
//////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedPublishPrivateEvent(HANDLE hApp, UI_32 uiEventId, PCVOID pData, UI_32 uiLength, HANDLE hSession) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && frameworkunifiedCheckValidMsgQ(hSession)) {
    EventTable::iterator e_iterator;
    ServiceSessionIdListTable::iterator ssidl_iterator;

    PCSTR l_cServiceName = "";
    BOOL l_bIsPublished = FALSE;

    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

    // set the data, if event is a public state events
    UpdatePrivateStateEventData(hApp,
                                uiEventId,
                                (reinterpret_cast<MsgQInfo *>(hSession))->cMsgQName,
                                (reinterpret_cast<MsgQInfo *>(hSession))->sessionId,
                                pData,
                                uiLength);

    // check if event is registered by client/s
    e_iterator = pApp->eventtable.find(uiEventId);

    if (pApp->eventtable.end() != e_iterator) {
      // for loop for event table
      for (ssidl_iterator = e_iterator->second.begin();
           ssidl_iterator != e_iterator->second.end();
           ssidl_iterator++) {
        l_cServiceName = ssidl_iterator->first.c_str();

        // send message to only specified client
        if (!std::strcmp(l_cServiceName, (reinterpret_cast<MsgQInfo *>(hSession))->cMsgQName)) {
          for (UI_32 l_uiCount = 0;
               l_uiCount < ssidl_iterator->second.size();
               l_uiCount++) {
            if ((reinterpret_cast<MsgQInfo *>(hSession))->sessionId == ssidl_iterator->second[l_uiCount]) {
              if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedSendMsg(hSession, uiEventId, uiLength, pData))) {
                FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "FrameworkunifiedSendMsg Failed for Event %d", uiEventId);
              }

              l_bIsPublished = TRUE;
              break;
            }
          }
        }

        if (l_bIsPublished) {
          break;
        }
      }

      if (!l_bIsPublished) {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, "Failed to publish event %d to client [%s], session: [%d]. "
               "Client not subscribed to the event on specified session.",
               uiEventId, (reinterpret_cast<MsgQInfo *>(hSession))->cMsgQName != 0 ? \
               (reinterpret_cast<MsgQInfo *>(hSession))->cMsgQName : NULL,
               (reinterpret_cast<MsgQInfo *>(hSession))->sessionId);
        eStatus = eFrameworkunifiedStatusServNotFound;
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, "Client %s is not subscribed to Event %d, session: [%d]",
             (reinterpret_cast<MsgQInfo *>(hSession))->cMsgQName, uiEventId,
             (reinterpret_cast<MsgQInfo *>(hSession))->sessionId);
      eStatus = eFrameworkunifiedStatusServNotFound;
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

//////////////////////////////////////////////////////
/// FrameworkunifiedPublishPublicEvent
//////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedPublishPublicEvent(HANDLE hApp, UI_32 uiEventId, PCVOID pData, UI_32 uiLength) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  EFrameworkunifiedStatus eRetStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    EventTable::iterator e_iterator;
    ServiceSessionIdListTable::iterator ssidl_iterator;

    PCSTR l_cServiceName = "";

    // set the data, if event is a state events
    UpdatePublicStateEventData(hApp, uiEventId, pData, uiLength);

    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

    // check if event is registered by client/s
    e_iterator = pApp->eventtable.find(uiEventId);

    if (pApp->eventtable.end() != e_iterator) {
      // for loop for event table
      for (ssidl_iterator = e_iterator->second.begin();
           ssidl_iterator != e_iterator->second.end();
           ssidl_iterator++) {
        l_cServiceName = ssidl_iterator->first.c_str();

        eRetStatus = SendEventMessage(hApp, ssidl_iterator->second, uiEventId, l_cServiceName, pData, uiLength);
        if (eFrameworkunifiedStatusOK != eRetStatus) {
          eStatus = eRetStatus;
        }
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, "No client subscribed to EventID %d.", uiEventId);
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  return eStatus;
}

//////////////////////////////////////////////////////
/// UpdatePrivateStateEventData
//////////////////////////////////////////////////////
EFrameworkunifiedStatus UpdatePrivateStateEventData(HANDLE hApp, UI_32 uiEventId, PCSTR pClientName, UI_32 uiSessionId,
                                       PCVOID pData,
                                       UI_32 uiLength) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if ((NULL == pData) && (uiLength > 0)) {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  if ((eFrameworkunifiedStatusOK == eStatus) && (frameworkunifiedCheckValidAppHandle(hApp)) && (NULL != pClientName)) {
    PrivateStateEventTable::iterator pse_iterator;
    ServiceSessionEventData::iterator ssed_iterator;
    SessionEventData::iterator sed_iterator;

    EventData *l_pEventData = NULL;

    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

    // set the data, if event is a state events
    pse_iterator = pApp->privatestateeventtable.find(uiEventId);
    if (pApp->privatestateeventtable.end() != pse_iterator) {
      ssed_iterator = (pse_iterator->second).find(pClientName);

      if ((pse_iterator->second).end() != ssed_iterator) {
        sed_iterator = (ssed_iterator->second).find(uiSessionId);
        if ((ssed_iterator->second).end() != sed_iterator) {
          l_pEventData = sed_iterator->second;

          if (NULL != l_pEventData) {
            if (NULL != l_pEventData->pData) {
              delete[](static_cast<PCHAR>(l_pEventData->pData));
              (sed_iterator->second)->pData = NULL;
            }

            delete l_pEventData;
            sed_iterator->second = NULL;
          }

          l_pEventData = new(std::nothrow) EventData();

          if (NULL != l_pEventData) {
            l_pEventData->uiLength = uiLength;

            if (NULL == pData) {
              l_pEventData->pData = NULL;
            } else {
              l_pEventData->pData = new(std::nothrow) CHAR[uiLength];
              if (NULL != l_pEventData->pData) {
                std::memset(l_pEventData->pData, 0, uiLength);
                std::memcpy(l_pEventData->pData, pData, uiLength);
              } else {
                eStatus = eFrameworkunifiedStatusNullPointer;
                l_pEventData->uiLength = 0;
              }
            }

            sed_iterator->second = l_pEventData;
          } else {
            eStatus = eFrameworkunifiedStatusNullPointer;
          }
        } else {  // if client has not yet subscribed to this event over the session uiSessionId
          l_pEventData = new(std::nothrow) EventData();

          if (NULL != l_pEventData) {
            l_pEventData->uiLength = uiLength;

            if (NULL == pData) {
              l_pEventData->pData = NULL;
            } else {
              l_pEventData->pData = new(std::nothrow) CHAR[uiLength];
              if (NULL != l_pEventData->pData) {
                std::memset(l_pEventData->pData, 0, uiLength);
                std::memcpy(l_pEventData->pData, pData, uiLength);
              } else {
                eStatus = eFrameworkunifiedStatusNullPointer;
              }
            }

            (ssed_iterator->second).insert(std::make_pair(uiSessionId, l_pEventData));
          } else {
            eStatus = eFrameworkunifiedStatusNullPointer;
          }
        }
      } else {  // if client has not yet subscribed to this event
        SessionEventData sessionEventData;

        l_pEventData = new(std::nothrow) EventData();

        if (NULL != l_pEventData) {
          l_pEventData->uiLength = uiLength;

          if (NULL == pData) {
            l_pEventData->pData = NULL;
          } else {
            l_pEventData->pData = new(std::nothrow) CHAR[uiLength];
            if (NULL != l_pEventData->pData) {
              std::memset(l_pEventData->pData, 0, uiLength);
              std::memcpy(l_pEventData->pData, pData, uiLength);
            } else {
              eStatus = eFrameworkunifiedStatusNullPointer;
              l_pEventData->uiLength = 0;
            }
          }
          sessionEventData.insert(std::make_pair(uiSessionId, l_pEventData));

          (pse_iterator->second).insert(std::make_pair(pClientName, sessionEventData));
        } else {
          eStatus = eFrameworkunifiedStatusNullPointer;
        }
      }
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

//////////////////////////////////////////////////////
/// DeleteSessionEventData
//////////////////////////////////////////////////////
EFrameworkunifiedStatus DeleteSessionEventData(HANDLE hApp, PCSTR pClientName, UI_32 uiSessionId) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  // map: eventid -> servicename -> sessionid -> eventdata(data, length)

  if (frameworkunifiedCheckValidAppHandle(hApp) && NULL != pClientName) {
    PrivateStateEventTable::iterator pse_iterator;
    ServiceSessionEventData::iterator ssed_iterator;
    SessionEventData::iterator sed_iterator;

    EventData *l_pEventData = NULL;

    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

    // delete the private event data associated with the service over this session
    for (pse_iterator  = pApp->privatestateeventtable.begin();
         pse_iterator != pApp->privatestateeventtable.end();
         ++pse_iterator) {
      ssed_iterator = (pse_iterator->second).find(pClientName);

      if ((pse_iterator->second).end() != ssed_iterator) {
        sed_iterator = (ssed_iterator->second).find(uiSessionId);
        if ((ssed_iterator->second).end() != sed_iterator) {
          l_pEventData = sed_iterator->second;

          if (NULL != l_pEventData) {
            if (NULL != l_pEventData->pData) {
              delete[](static_cast<PCHAR>(l_pEventData->pData));
              (sed_iterator->second)->pData = NULL;
            }

            delete l_pEventData;
            sed_iterator->second = NULL;
          }
        }
      }
    }
  } else {
    eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return eStatus;
}

//////////////////////////////////////////////////////
/// Returns the number of session opened by the server for one client
//////////////////////////////////////////////////////
UI_32 FrameworkunifiedGetNumberOfSession(HANDLE hApp, PCSTR strServiceName) {
  UI_32 l_uiSessionCount = 0;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    Services::iterator s_iterator = pApp->services.find(strServiceName);
    if (s_iterator != pApp->services.end()) {
      l_uiSessionCount = static_cast<UI_32>((s_iterator->second).size());
    }
  }

  return l_uiSessionCount;
}

//////////////////////////////////////////////////////
/// Sets the handle in the Application Framework
//////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSetSessionHandle(HANDLE hApp, PCSTR strServiceName, HANDLE hSession) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  UI_32 l_uiSessionId = 0;
  if (frameworkunifiedCheckValidMsgQ(hSession) && NULL != strServiceName && frameworkunifiedCheckValidAppHandle(hApp) &&
      0 != std::strlen(strServiceName)) {
    MsgQInfo *pMsgQ    = reinterpret_cast<MsgQInfo *>(hSession);

    // Get session Id
    l_uiSessionId = pMsgQ->sessionId;

    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    ServiceSessionTable::iterator session_iterator;
    SessionHandleTable::iterator session_handle_iterator;

    // Find the service name
    session_iterator = pApp->sessiontable.find(strServiceName);
    if (session_iterator == pApp->sessiontable.end()) {
      pApp->sessiontable.insert(std::make_pair(strServiceName, SessionHandleTable()));
    }

    // Find the session id
    session_handle_iterator = pApp->sessiontable[strServiceName].find(l_uiSessionId);
    if (session_handle_iterator != pApp->sessiontable[strServiceName].end()) {
      pApp->sessiontable[strServiceName].erase(l_uiSessionId);
    }

    // Set the session handle
    pApp->sessiontable[strServiceName].insert(std::make_pair(l_uiSessionId, hSession));

  } else {
    eStatus = eFrameworkunifiedStatusFail;
  }
  return eStatus;
}

//////////////////////////////////////////////////////
/// reads the handle from the Application Framework
//////////////////////////////////////////////////////
HANDLE FrameworkunifiedGetSessionHandle(HANDLE hApp, PCSTR strServiceName, UI_32 uiSessionId) {
  HANDLE hSession = NULL;
  if (frameworkunifiedCheckValidAppHandle(hApp) && strServiceName) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    ServiceSessionTable::iterator session_iterator;
    SessionHandleTable::iterator session_handle_iterator;

    // Find the service name
    session_iterator = pApp->sessiontable.find(strServiceName);
    if (session_iterator != pApp->sessiontable.end()) {
      // Find the session id
      session_handle_iterator = pApp->sessiontable[strServiceName].find(uiSessionId);
      if (session_handle_iterator != pApp->sessiontable[strServiceName].end()) {
        // Fetch the session handle
        hSession = session_handle_iterator->second;
      }
    }
  }
  return hSession;
}
//////////////////////////////////////////////////////
/// Close the handle stored in the Application Framework
//////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedRemoveSessionHandle(HANDLE hApp, PCSTR strServiceName, UI_32 uiSessionId) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  if (frameworkunifiedCheckValidAppHandle(hApp) && strServiceName && 0 != std::strlen(strServiceName)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

    pApp->sessiontable[strServiceName].erase(uiSessionId);
    if (0 == pApp->sessiontable[strServiceName].size()) {
      // If no entry is available for current service then remove the servicename
      pApp->sessiontable.erase(strServiceName);
    }
  } else {
    eStatus = eFrameworkunifiedStatusFail;
  }

  return eStatus;
}

//////////////////////////////////////////////////////
/// FrameworkunifiedGetSession|Name -> On Client side
//////////////////////////////////////////////////////
PCSTR FrameworkunifiedGetSessionName(HANDLE hSession) {
  if (frameworkunifiedCheckValidMsgQ(hSession)) {
    return ((reinterpret_cast<MsgQInfo *>(hSession))->cMsgQName);
  } else {
    return NULL;
  }
}


HANDLE FrameworkunifiedGetCurrentSessionHandle(HANDLE hApp) {
  HANDLE hSession = NULL;
  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    UI_32 l_uiSessionId = FrameworkunifiedGetMsgSessionId(hApp);
    PCSTR pRequester   = FrameworkunifiedGetMsgSrc(hApp);

    hSession = FrameworkunifiedGetSessionHandle(hApp, pRequester, l_uiSessionId);
  }
  return hSession;
}

HANDLE FrameworkunifiedCreateSession(HANDLE hApp, PCSTR pSessionName) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  HANDLE hSession = NULL;
  if (frameworkunifiedCheckValidAppHandle(hApp) && pSessionName && strlen(pSessionName)) {
    hSession = FrameworkunifiedGenerateSessionHandle(hApp, pSessionName);
    if (hSession) {
      if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedSetSessionHandle(hApp, pSessionName, hSession))) {
        if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedMcClose(hSession))) {
          FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __PRETTY_FUNCTION__, "Close session failed");
        }
        hSession = NULL;

        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __PRETTY_FUNCTION__, "FrameworkunifiedCreateSession Failed Status:0x%x ", eStatus);
      }
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }
  return hSession;
}

EFrameworkunifiedStatus FrameworkunifiedDestroySession(HANDLE hApp, HANDLE hSession) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusFail;

  if (frameworkunifiedCheckValidAppHandle(hApp) && frameworkunifiedCheckValidMsgQ(hSession)) {
    UI_32 uiSessionId = (reinterpret_cast<MsgQInfo *>(hSession))->sessionId;
    CHAR pRequester[MAX_NAME_SIZE_APP] = {};
    strncpy(pRequester, (reinterpret_cast<MsgQInfo *>(hSession))->cMsgQName, sizeof(pRequester) - 1);

    if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedMcClose(hSession))) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __PRETTY_FUNCTION__, "Close session failed. Status: %d", eStatus);
    }

    if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedRemoveSessionHandle(hApp, pRequester, uiSessionId))) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __PRETTY_FUNCTION__, "Remove session failed. Status: %d", eStatus);
    }

    DeleteSessionEventData(hApp, pRequester, uiSessionId);
  }

  return eStatus;
}
