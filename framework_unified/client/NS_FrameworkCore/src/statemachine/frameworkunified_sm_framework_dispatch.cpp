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

//////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup tag_NSFramework
/// \brief
///
///
//////////////////////////////////////////////////////////////////////////////////////////////////

#include <native_service/frameworkunified_sm_framework_dispatch.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_service_protocol.h>
#include <native_service/ns_message_center_if.h>
#include <native_service/ns_logger_if.h>
#include <map>
#include <utility>
#include "frameworkunified_framework_core.h"

////////////////////////////////////////////////////////////////////////////////////////////
/// Registers a single event with the dispatcher for a given service.
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedAttachHSMEventToDispatcher(HANDLE hApp,
                                         PCSTR pServiceName,
                                         UI_32 iCmd,
                                         UI_32 iEvent,
                                         HANDLE hSession) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && pServiceName) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    EventServices::iterator s_iterator;
    EventSessionTable::iterator session_iterator;

    UI_32 uiSessionId = 0;
    if (hSession) {
      uiSessionId = FrameworkunifiedGetSessionId(hSession);
    }

    // finding the service
    s_iterator = pApp->eventservices.find(pServiceName);
    if (s_iterator == pApp->eventservices.end()) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, "%s : (service NOT found): service name [%s]", pApp->cAppName, pServiceName);
      pApp->eventservices.insert(std::make_pair(pServiceName, EventSessionTable()));
    }

    session_iterator = pApp->eventservices[pServiceName].find(uiSessionId);
    if (session_iterator == pApp->eventservices[pServiceName].end()) {
      pApp->eventservices[pServiceName].insert
      (std::make_pair(uiSessionId, ServiceEventProtocolTable()));
    }

    (pApp->eventservices[pServiceName].find(uiSessionId)->second).insert(std::make_pair(iCmd, iEvent));
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  return eStatus;
}


////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedAttachHSMEventsToDispatcher
/// Registers a multiple event with the dispatcher for a given service.
////////////////////////////////////////////////////////////////////////////////////////////

EFrameworkunifiedStatus FrameworkunifiedAttachHSMEventsToDispatcher(HANDLE hApp, PCSTR pServiceName,
                                          const FrameworkunifiedProtocolEvent *pEventIds, UI_32 uiEventCount, HANDLE hSession) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && pServiceName && pEventIds) {
    // setup callbacks
    for (UI_32 i = 0; i < uiEventCount; ++i) {
      if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedAttachHSMEventToDispatcher(hApp,
                                                                   pServiceName,
                                                                   pEventIds[ i ].iCmd,
                                                                   pEventIds[ i ].iEventId,
                                                                   hSession))) {
        break;
      }
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  return eStatus;
}

/////////////////////////////////////////////////////
// Function : FrameworkunifiedAttachParentCallbacksToDispatcher
/////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedAttachParentHSMEventsToDispatcher(HANDLE hChildApp, const FrameworkunifiedProtocolEvent *pEventIds,
                                                UI_32 uiEventCount) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hChildApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hChildApp);

    eStatus = FrameworkunifiedAttachHSMEventsToDispatcher(hChildApp, pApp->cParentAppName, pEventIds, uiEventCount);
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  return eStatus;
}

/////////////////////////////////////////////////////
// Function : FrameworkunifiedDetachParentCallbacksFromDispatcher
/////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedDetachParentHSMEventsFromDispatcher(HANDLE hChildApp,
                                                  const PUI_32 puiEventArray, UI_32 uiEventCount) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hChildApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hChildApp);

    eStatus = FrameworkunifiedDetachHSMEventsFromDispatcher(hChildApp, pApp->cParentAppName, puiEventArray, uiEventCount);
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  return eStatus;
}


/////////////////////////////////////////////////////
// Function : FrameworkunifiedDetachHSMEventsFromDispatcher
/////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedDetachHSMEventsFromDispatcher(HANDLE hApp, PCSTR pServiceName, const PUI_32 puiEventArray,
                                            UI_32 uiEventCount, HANDLE hSession) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && pServiceName) {
    // setup callbacks
    for (UI_32 i = 0; i < uiEventCount; ++i) {
      /**
       * @todo
       *  When an event-struct pointer is turned NULL, the array is accessed without NULL checking,
       *  and a segmentation fault occurs.
       */
      if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedDetachHSMEventFromDispatcher(hApp, pServiceName,
                                                                puiEventArray[ i ], hSession))) {
        break;
      }
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  return eStatus;
}


/////////////////////////////////////////////
// Function : FrameworkunifiedDetachHSMEventFromDispatcher
/////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedDetachHSMEventFromDispatcher(HANDLE hApp, PCSTR pServiceName, UI_32 iEvent, HANDLE hSession) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  if (frameworkunifiedCheckValidAppHandle(hApp) && pServiceName) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    EventServices::iterator s_iterator;

    UI_32 uiSessionId = 0;
    if (hSession) {
      uiSessionId = FrameworkunifiedGetSessionId(hSession);
    }

    // finding the service
    s_iterator = pApp->eventservices.find(pServiceName);
    if (s_iterator != pApp->eventservices.end()) {
      EventSessionTable::iterator session_iterator;
      session_iterator = (s_iterator->second).find(uiSessionId);
      if (session_iterator != (s_iterator->second).end()) {
        ServiceEventProtocolTable::iterator spt_iterator;
        FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, "%s : (found): service [%s]", pApp->cAppName, pServiceName);

        spt_iterator = (session_iterator->second).find(iEvent);
        if (spt_iterator != (session_iterator->second).end()) {
          FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, "%s : Cmd found [%d] service [%s]",
            pApp->cAppName, iEvent, pServiceName);
          (session_iterator->second).erase(spt_iterator);
        } else {
          FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, "%s : Error : Cmd NOT found [%d] service [%s]",
            pApp->cAppName, iEvent, pServiceName);
        }
      }

    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "%s : Error : Cannot find service [%s]", pApp->cAppName, pServiceName);
      eStatus = eFrameworkunifiedStatusFail;
    }

  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  return eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedAttachNotificationEventsToDispatcher
/// API to attach a event to the dispatcher on receiving a specific notification.
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSubscribeNotificationsWithHSMEvent(HANDLE hApp,  const FrameworkunifiedNotificationEvent *pNtfyEvent,
                                                 UI_32 uiEventCount) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && pNtfyEvent) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

    /// Deal with handling batch processing of subscriptions.
    if (eFrameworkunifiedStatusOK == (eStatus = FrameworkunifiedNPHSMSubscribeToNotificationsEvents(hApp, pNtfyEvent, uiEventCount))) {
      for (UI_32 i = 0; i < uiEventCount; ++i) {
        // service found
        pApp->notificationevents.insert(std::make_pair(pNtfyEvent[ i ].cNotification, pNtfyEvent[ i ].iEventId));

        FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, "%s : attaching call-back for notification [%s]", pApp->cAppName,
               pNtfyEvent[ i ].cNotification);
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "%s : Error : [%d] Unable to Subscribe "
        "to batch set of notifications", pApp->cAppName,
             eStatus);
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedAttachNotificationEventToDispatcher
/// API to attach a event to the dispatcher on receiving a specific notification.
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSubscribeNotificationWithHSMEvent(HANDLE hApp, PCSTR pNotification, UI_32 iEventId) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && pNotification) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

    if (eFrameworkunifiedStatusOK == (eStatus = FrameworkunifiedNPHSMSubscribeToNotificationEvent(hApp, pNotification))) {
      // service found
      pApp->notificationevents.insert(std::make_pair(pNotification, iEventId));

      FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, "%s : attaching call-back for notification [%s]",
        pApp->cAppName, pNotification);
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "%s : Error : [%d] Unable to Subscribe "
        "to notification [%s]", pApp->cAppName, eFrameworkunifiedStatusOK, pNotification);
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  return eStatus;
}



////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedDetachNotificationEventsFromDispatcher
/// API to detach a notification event from the dispatcher.
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedUnsubscribeNotificationsWithHSMEvent(HANDLE hApp,  const FrameworkunifiedNotificationEvent *pNtfyEvent,
                                                   UI_32 uiEventCount) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && pNtfyEvent) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

    NotificationEventTable::iterator n_iterator;
    PCSTR pNotification = NULL;

    if (eFrameworkunifiedStatusOK == (eStatus = FrameworkunifiedNPHSMUnsubscribeFromNotificationEvents(hApp, pNtfyEvent, uiEventCount))) {
      for (UI_32 l_unCount = 0; l_unCount < uiEventCount; ++l_unCount) {
        pNotification = pNtfyEvent[ l_unCount ].cNotification;
        n_iterator = pApp->notificationevents.find(pNotification);
        if (n_iterator != pApp->notificationevents.end()) {
          pApp->notificationevents.erase(n_iterator);
          FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, "%s : removed notification [%s]", pApp->cAppName, pNotification);
        } else {
          FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "%s : Error : Cannot find notification [%s]",
            pApp->cAppName, pNotification);
          eStatus = eFrameworkunifiedStatusFail;
        }
      }

      FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, "%s : detaching call-back for notification [%s]",
        pApp->cAppName, pNotification);
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "%s : Error : [%d] Unable to UnSubscribe from notifications ",
        pApp->cAppName, eFrameworkunifiedStatusOK);
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  return eStatus;
}


////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedDetachNotificationEventsFromDispatcher
/// API to detach a notification event from the dispatcher.
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedUnsubscribeNotificationWithHSMEvent(HANDLE hApp, PCSTR pNotification) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && pNotification) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    NotificationEventTable::iterator n_iterator;

    if (eFrameworkunifiedStatusOK == (eStatus = FrameworkunifiedNPHSMUnsubscribeFromNotificationEvent(hApp, pNotification))) {
      // \todo : error handling on all map function calls
      n_iterator = pApp->notificationevents.find(pNotification);
      if (n_iterator != pApp->notificationevents.end()) {
        pApp->notificationevents.erase(n_iterator);
        FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, "%s : removed notification [%s]", pApp->cAppName, pNotification);
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "%s : Error : Cannot find notification [%s]",
          pApp->cAppName, pNotification);
        eStatus = eFrameworkunifiedStatusFail;
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "%s : Error : [%d] Unable to UnSubscribe from notification [%s]",
        pApp->cAppName, eFrameworkunifiedStatusOK, pNotification);
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  return eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedGetStateMachine
///   returns the pointer to the statemachine object
////////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHSMFramework *FrameworkunifiedGetStateMachine(HANDLE hApp) {
  CFrameworkunifiedHSMFramework *l_pHSMFramework = NULL;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    l_pHSMFramework = pApp->m_pFrameworkunifiedStateMachine;
  }

  return l_pHSMFramework;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedSetStateMachine
///   sets the statemachine object
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSetStateMachine(HANDLE hApp,
                              CFrameworkunifiedHSM *f_pFrameworkunifiedHSM) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    if (NULL != f_pFrameworkunifiedHSM) {
      (reinterpret_cast<CFrameworkunifiedFrameworkApp *>(hApp))->m_pFrameworkunifiedStateMachine =
      reinterpret_cast<CFrameworkunifiedHSMFramework *>(f_pFrameworkunifiedHSM);
    } else {
      eStatus = eFrameworkunifiedStatusNullPointer;
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  return eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedSubscribeToSessionEventWithHSMEvent
/// This API is used for subscribing to single event of a service.
/// This API also attaches the session event with HSM events.
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSubscribeToSessionEventWithHSMEvent(HANDLE hApp,
                                                  UI_32 uiEventId,
                                                  UI_32 uiHSMEventId,
                                                  HANDLE hSession) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && frameworkunifiedCheckValidMsgQ(hSession)) {
    if (eFrameworkunifiedStatusOK == (eStatus = FrameworkunifiedAttachHSMEventToDispatcher(hApp,
                                                    (reinterpret_cast<MsgQInfo *>(hSession))->cMsgQName, uiEventId,
                                                    uiHSMEventId, hSession))) {
      eStatus = FrameworkunifiedSendMsg(hSession, PROTOCOL_REGISTER_EVENTS, sizeof(UI_32), (PVOID)&uiEventId);
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "FrameworkunifiedAttachCallbackToDispatcher "
        "for PROTOCOL_REGISTER_EVENTS Failed Status:: %d", eStatus);
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  return eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedSubscribeToSessionEventsWithHSMEvents
/// This API is used for subscribing to multiple events of a service.
/// This API also attaches the session events with hsm events.
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSubscribeToSessionEventsWithHSMEvents(HANDLE hApp, const FrameworkunifiedProtocolEvent *pEventIds,
                                                    UI_32 uiEventCount,
                                                    HANDLE hSession) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && frameworkunifiedCheckValidMsgQ(hSession) && NULL != pEventIds) {
    eStatus = FrameworkunifiedAttachHSMEventsToDispatcher(hApp,
      (reinterpret_cast<MsgQInfo *>(hSession))->cMsgQName, pEventIds,
      uiEventCount, hSession);

    UI_32 l_uiCmdList[uiEventCount];  // NOLINT  (readability/nolint)
    for (UI_32 l_uiCnt = 0; l_uiCnt < uiEventCount; l_uiCnt++) {
      l_uiCmdList[l_uiCnt] = pEventIds[l_uiCnt].iCmd;
    }

    eStatus = FrameworkunifiedSendMsg(hSession, PROTOCOL_REGISTER_EVENTS, static_cast<UI_32>(uiEventCount * sizeof(UI_32)),
      l_uiCmdList);
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  return eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedUnSubscribeSessionEventWithHSMEvent
/// API to unsubscribe from event of a service. Also detaches HSM event.
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedUnSubscribeSessionEventWithHSMEvent(HANDLE hApp, UI_32 uiEventId, HANDLE hSession) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && frameworkunifiedCheckValidMsgQ(hSession)) {
    eStatus = FrameworkunifiedSendMsg(hSession, PROTOCOL_UNREGISTER_EVENTS, sizeof(uiEventId), (PVOID)&uiEventId);

    if (eFrameworkunifiedStatusOK != FrameworkunifiedDetachHSMEventFromDispatcher(hApp,
      (reinterpret_cast<MsgQInfo *>(hSession))->cMsgQName, uiEventId, hSession)) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error : FrameworkunifiedDetachCallbackFromDispatcher failed status:: %d", eStatus);
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  return eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedUnSubscribeSessionEventsWithHSMEvents
/// API to unsubscribe from multiple events of a service. Also detaches HSM events.
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedUnSubscribeSessionEventsWithHSMEvents(HANDLE hApp, PUI_32 pEventsArray, UI_32 uiListSize,
                                                    HANDLE hSession) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && frameworkunifiedCheckValidMsgQ(hSession) && NULL != pEventsArray) {
    eStatus = FrameworkunifiedSendMsg(hSession, PROTOCOL_UNREGISTER_EVENTS,
      static_cast<UI_32>(uiListSize * sizeof(uiListSize)), pEventsArray);

    if (eFrameworkunifiedStatusOK != FrameworkunifiedDetachHSMEventsFromDispatcher(hApp,
                                  (reinterpret_cast<MsgQInfo *>(hSession))->cMsgQName, pEventsArray,
                                   uiListSize)) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error : FrameworkunifiedDetachCallbackFromDispatcher failed status:: %d", eStatus);
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  return eStatus;
}
