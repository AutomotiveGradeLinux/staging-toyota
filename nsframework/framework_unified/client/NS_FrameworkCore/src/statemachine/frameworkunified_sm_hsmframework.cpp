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

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup <<Group Tag>> <<Group Name>>
/// \ingroup  tag_NSFramework
/// .
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NSFramework
/// \brief
///
/// This file has the CFrameworkunifiedHSMParentFramework class definitions. CFrameworkunifiedHSMParentFramework is derived from
/// CFrameworkunifiedHSM class. It defines the states required for NS statemachine framework, it creates the NS
/// statemachine and implements the reaction associated with the events
///
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <native_service/frameworkunified_sm_hsmframework.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_framework_types.h>
#include <native_service/ns_np_service_protocol.h>
#include <native_service/ns_np_service_notification.h>
#include <native_service/frameworkunified_sm_framework_dispatch.h>
#include <native_service/ns_plogger_if.h>

#include <map>
#include <string>

#include "frameworkunified_framework_core.h"
#include "frameworkunified_framework_internal.h"
#include "frameworkunified_framework_utility.h"
#include "frameworkunified_sm_framework_core.h"

extern EFrameworkunifiedStatus FrameworkunifiedHSMOnLoadData(HANDLE hApp) __attribute__((weak));
extern EFrameworkunifiedStatus FrameworkunifiedHSMOnStopInsFrameworkunifiedRun(HANDLE hApp) __attribute__((weak));

CFrameworkunifiedHSMFramework::CFrameworkunifiedHSMFramework(): CFrameworkunifiedHSM() {
}
CFrameworkunifiedHSMFramework::CFrameworkunifiedHSMFramework(PVOID f_pHApp): CFrameworkunifiedHSM(f_pHApp) {
}
CFrameworkunifiedHSMParentFramework::CFrameworkunifiedHSMParentFramework(): CFrameworkunifiedHSMFramework() {
  try {
    m_pFrameworkunifiedStateList = new std::map<UI_32, CFrameworkunifiedState *>();
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
  }
}
///////////////////////////////////////////////////////////////////////////////////////////
/// CFrameworkunifiedHSMParentFramework
/// default constructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHSMParentFramework::CFrameworkunifiedHSMParentFramework(PVOID f_pHApp): CFrameworkunifiedHSMFramework(f_pHApp) {
  try {
    m_pFrameworkunifiedStateList = new std::map<UI_32, CFrameworkunifiedState *>();
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
  }
}
///////////////////////////////////////////////////////////////////////////////////////////
/// ~CFrameworkunifiedHSMParentFramework
/// Class destructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHSMParentFramework::~CFrameworkunifiedHSMParentFramework() {
  // TODO(framework_unifeid) Auto-generated destructor stub
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedCreate
/// Statemachine states and events created and connected in this interface.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSMParentFramework::FrameworkunifiedCreate(PVOID f_pEventData) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  HSMConfigOptions *l_tHSMConfig = NULL;
  try {
    if (f_pEventData) {
      l_tHSMConfig  = reinterpret_cast<HSMConfigOptions *>(f_pEventData);
    }

    // Create states
    CREATE_STATE(sFrameworkunifiedRoot)
    CREATE_STATE(sFrameworkunifiedApp)
    CREATE_STATE(sFrameworkunifiedInitialization)
    CREATE_STATE(sFrameworkunifiedLoadData)
    CREATE_STATE(sFrameworkunifiedRun)
    CREATE_STATE(sFrameworkunifiedLoadSessions)
    CREATE_STATE(sFrameworkunifiedReady)
    CREATE_STATE(sFrameworkunifiedStop)
    CREATE_STATE(sFrameworkunifiedStopping)
    CREATE_STATE(sFrameworkunifiedPre)  // LCOV_EXCL_BR_LINE 15:Excluded due to inlined functions
    CREATE_STATE(sFrameworkunifiedBackground)  // LCOV_EXCL_BR_LINE 15:Excluded due to inlined functions

    // connect states as per the statemachine
    CONNECT_DEFAULTSTATE(sFrameworkunifiedRoot, sFrameworkunifiedApp)
    CONNECT_STATE(sFrameworkunifiedRoot, sFrameworkunifiedStop)

    CONNECT_DEFAULTSTATE(sFrameworkunifiedApp, sFrameworkunifiedInitialization)
    CONNECT_STATE(sFrameworkunifiedApp, sFrameworkunifiedRun)

    CONNECT_DEFAULTSTATE(sFrameworkunifiedInitialization, sFrameworkunifiedLoadData)
    CONNECT_STATE(sFrameworkunifiedInitialization, sFrameworkunifiedPre)  // LCOV_EXCL_BR_LINE 15:Excluded due to inlined functions
    CONNECT_STATE(sFrameworkunifiedInitialization, sFrameworkunifiedBackground)  // LCOV_EXCL_BR_LINE 15:Excluded due to inlined functions

    CONNECT_DEFAULTSTATE(sFrameworkunifiedRun, sFrameworkunifiedLoadSessions)
    CONNECT_STATE(sFrameworkunifiedRun, sFrameworkunifiedReady)

    CONNECT_DEFAULTSTATE(sFrameworkunifiedStop, sFrameworkunifiedStopping)

    // Create External Transition
    CREATE_EXTERNALTRANSITION(sFrameworkunifiedApp)
    CREATE_EXTERNALTRANSITION(sFrameworkunifiedStop)

    CREATE_EXTERNALTRANSITION(sFrameworkunifiedLoadSessions)
    CREATE_EXTERNALTRANSITION(sFrameworkunifiedReady)

    CREATE_EXTERNALTRANSITION(sFrameworkunifiedPre)  // LCOV_EXCL_BR_LINE 15:Excluded due to inlined functions
    CREATE_EXTERNALTRANSITION(sFrameworkunifiedBackground)  // LCOV_EXCL_BR_LINE 15:Excluded due to inlined functions

    // CREATE_ACTION(aFrameworkunifiedOnStart)

    // this is a patch, this code will be fixed once we change the macro for CONNECT_ACTION
    CFrameworkunifiedExternalTransition *l_pTrnsFrameworkunifiedRunevFrameworkunifiedStart = new CFrameworkunifiedExternalTransition(l_psFrameworkunifiedRun);

    // CONNECT_ACTION(sFrameworkunifiedRunevFrameworkunifiedStart,aFrameworkunifiedOnStart)

    // Connecting events to sFrameworkunifiedInitialization state
    CONNECT_EVENT(sFrameworkunifiedInitialization, evFrameworkunifiedStart, sFrameworkunifiedRunevFrameworkunifiedStart)
    CONNECT_EVENT(sFrameworkunifiedInitialization, evFrameworkunifiedPreStart, sFrameworkunifiedPre)  // LCOV_EXCL_BR_LINE 15:Excluded due to inlined functions  // NOLINT(whitespace/line_length)
    CONNECT_EVENT(sFrameworkunifiedInitialization, evFrameworkunifiedBackgroundStart, sFrameworkunifiedBackground)  // LCOV_EXCL_BR_LINE 15:Excluded due to inlined functions  // NOLINT(whitespace/line_length)

    // Create Local transitions
    CREATE_LOCALTRANSITION(sFrameworkunifiedInitialization);
    CREATE_LOCALTRANSITION(sFrameworkunifiedStop);

    // Connect events to state

    // Connecting events to sFrameworkunifiedLoadData state
    CONNECT_DEFERREDEVENT(sFrameworkunifiedLoadData, evFrameworkunifiedStart)
    CONNECT_DEFERREDEVENT(sFrameworkunifiedLoadData, evFrameworkunifiedUserChange)
    CONNECT_LOCAL_EVENT(sFrameworkunifiedLoadData, evFrameworkunifiedLoadComplete, sFrameworkunifiedInitialization)

    // Connecting events to App state
    CONNECT_EVENT(sFrameworkunifiedApp, evFrameworkunifiedStop, sFrameworkunifiedStop)
    CONNECT_LOCAL_EVENT(sFrameworkunifiedStopping, evFrameworkunifiedStopComplete, sFrameworkunifiedStop)
    CONNECT_DEFERREDEVENT(sFrameworkunifiedStopping, evFrameworkunifiedStart)

    // Connecting events to Connecting state
    CONNECT_EVENT(sFrameworkunifiedLoadSessions, evFrameworkunifiedReady, sFrameworkunifiedReady)
    // CONNECT_EVENT(sFrameworkunifiedReady,evFrameworkunifiedLoadSessions,sFrameworkunifiedRun)
    CONNECT_EVENT(sFrameworkunifiedReady, evFrameworkunifiedLoadSessions, sFrameworkunifiedLoadSessions)

    // Connecting events to Stop state
    CONNECT_EVENT(sFrameworkunifiedStop, evFrameworkunifiedInit, sFrameworkunifiedApp)

    // Create internal transitions
    CREATE_INTERNALTRANSITION(OnEventStartInStopState)
    CONNECT_EVENT(sFrameworkunifiedStop, evFrameworkunifiedStart, OnEventStartInStopState)

    CREATE_INTERNALTRANSITION(OnEventStopInStoppingState)
    CONNECT_EVENT(sFrameworkunifiedStopping, evFrameworkunifiedStop, OnEventStopInStoppingState)

    // Connecting events to sFrameworkunifiedRun state
    CONNECT_EVENT(sFrameworkunifiedRun, evFrameworkunifiedPreStop, sFrameworkunifiedPre)  // LCOV_EXCL_BR_LINE 15:Excluded due to inlined functions
    CONNECT_EVENT(sFrameworkunifiedRun, evFrameworkunifiedBackgroundStop, sFrameworkunifiedBackground)  // LCOV_EXCL_BR_LINE 15:Excluded due to inlined functions  // NOLINT(whitespace/line_length)

    // Connecting events to sFrameworkunifiedBackground state
    CONNECT_EVENT(sFrameworkunifiedBackground, evFrameworkunifiedPreStop, sFrameworkunifiedPre)  // LCOV_EXCL_BR_LINE 15:Excluded due to inlined functions

    if (NULL != l_tHSMConfig) {
      if (eUserchangeIgnore != l_tHSMConfig->eUserChange) {
        if (eUserchangeReInit == l_tHSMConfig->eUserChange) {
          // Create Internal Transition
          CREATE_INTERNALTRANSITION(OnEventUserChangeInInitializationState)
          CONNECT_EVENT(sFrameworkunifiedInitialization, evFrameworkunifiedUserChange, OnEventUserChangeInInitializationState)

          // Create Internal Transition
          CREATE_INTERNALTRANSITION(OnEventUserChangeInRunState)
          CONNECT_EVENT(sFrameworkunifiedRun, evFrameworkunifiedUserChange, OnEventUserChangeInRunState)

          CREATE_ACTION(aFrameworkunifiedOnStop)

          // this is a patch, this code will be fixed once we change the macro for CONNECT_ACTION
          CFrameworkunifiedExternalTransition *l_pTrnsFrameworkunifiedAppevFrameworkunifiedInit = new CFrameworkunifiedExternalTransition(l_psFrameworkunifiedApp);

          CONNECT_ACTION(sFrameworkunifiedAppevFrameworkunifiedInit, aFrameworkunifiedOnStop)

          CONNECT_EVENT(sFrameworkunifiedApp, evFrameworkunifiedInit, sFrameworkunifiedAppevFrameworkunifiedInit)

        } else if (eUserchangeRetPrevState == l_tHSMConfig->eUserChange) {
          CREATE_STATE(sFrameworkunifiedUserChange)

          CONNECT_STATE(sFrameworkunifiedRoot, sFrameworkunifiedUserChange)

          CREATE_EXTERNALTRANSITION(sFrameworkunifiedUserChange)

          CONNECT_EVENT(sFrameworkunifiedApp, evFrameworkunifiedUserChange, sFrameworkunifiedUserChange)
          CONNECT_EVENT(sFrameworkunifiedUserChange, evFrameworkunifiedStop, sFrameworkunifiedStop)

          CONNECT_DEFERREDEVENT(sFrameworkunifiedUserChange, evFrameworkunifiedStart)
          CONNECT_DEFERREDEVENT(sFrameworkunifiedUserChange, evFrameworkunifiedUserChange)

          // add shallow history state in state sFrameworkunifiedApp
          ADD_SHALLOWHISTORYSTATE(sFrameworkunifiedApp)

          // create transition to shallow history state in state sFrameworkunifiedApp
          CREATE_SHALLOWHISTORYTRANSITION(sFrameworkunifiedApp)

          // Connecting events to UserChange State
          CONNECT_SHALLOWHISTORYEVENT(sFrameworkunifiedUserChange, evFrameworkunifiedUserChangeComplete, sFrameworkunifiedApp)
        } else {
          FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, "Incorrect  Userchange config option %d", l_tHSMConfig->eUserChange);
        }
      }
    }

    // Connecting FrameworkRoot to statemachine
    CONNECTROOT(sFrameworkunifiedRoot)

    // Creating the application specific statemachine
    CHKNULL(m_pHApp);
    FrameworkunifiedCreateStateMachineInternal(m_pHApp);

    // Print statemachine xml
    PRINT_HSM();
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());

    eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return eStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedFrameworkConnect
/// This adds the given state as a sub state of sReady state of the Framework
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSMParentFramework::FrameworkunifiedFrameworkConnect(CFrameworkunifiedState *f_pAppState, BOOL f_bIsDefaultState) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  try {
    CHKNULL(m_pFrameworkunifiedStateList);
    CHKNULL(f_pAppState);

    if (m_pFrameworkunifiedStateList->find(esFrameworkunifiedReady) == m_pFrameworkunifiedStateList->end()) {
      throw CNullPointerException();
    }
    CFrameworkunifiedState *l_pReady = reinterpret_cast<CFrameworkunifiedCompositeState *>(m_pFrameworkunifiedStateList->find(esFrameworkunifiedReady)->second);
    CHKNULL(l_pReady);

    eStatus = FrameworkunifiedConnect(l_pReady, f_pAppState, f_bIsDefaultState);
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());

    eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return eStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedFrameworkConnect
/// This connects the reaction to event and add event to states
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSMParentFramework::FrameworkunifiedFrameworkConnect(FRAMEWORKUNIFIED_HSM_STATES f_eFrameworkunifiedState, UI_32 f_uiEventId,
                                                       CFrameworkunifiedReaction *f_pReaction, std::string f_strEventName,
                                                       BOOL f_bIsDeferredEvent) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  try {
    CHKNULL(m_pFrameworkunifiedStateList);

    if (m_pFrameworkunifiedStateList->find(f_eFrameworkunifiedState) == m_pFrameworkunifiedStateList->end()) {
      throw CNullPointerException();
    }
    CFrameworkunifiedState *l_pFrameworkunifiedState = reinterpret_cast<CFrameworkunifiedCompositeState *>(m_pFrameworkunifiedStateList->find(f_eFrameworkunifiedState)->second);

    CHKNULL(l_pFrameworkunifiedState);

    eStatus = FrameworkunifiedConnect(l_pFrameworkunifiedState, f_uiEventId, f_pReaction, f_strEventName, f_bIsDeferredEvent);
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());

    eStatus = eFrameworkunifiedStatusNullPointer;
  }
  return eStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// CsFrameworkunifiedRoot
/// Parameterized constructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedRoot::CsFrameworkunifiedRoot(std::string f_pName): CFrameworkunifiedCompositeState(f_pName) {
}

///////////////////////////////////////////////////////////////////////////////////////////
/// ~CsFrameworkunifiedRoot
/// class destructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedRoot::~CsFrameworkunifiedRoot() {
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnEntry
/// Initializes the application and registers the service availability notification
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedRoot::FrameworkunifiedOnEntry(CEventDataPtr f_pEventData) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Entering state %s ", m_strStateName.c_str());

  HANDLE l_pHApp = FrameworkunifiedGetAppHandle();
  CHKNULL(l_pHApp);

  FRAMEWORKUNIFIEDLOG_PERFORMANCE_DEBUG("FrameworkunifiedOnInitialization +");
  if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedOnInitializationInternal(l_pHApp))) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, "Warning:Failed to Initialize application:0x%x ", eStatus);
  }
  FRAMEWORKUNIFIEDLOG_PERFORMANCE_DEBUG("FrameworkunifiedOnInitialization -");

  // Publish service unavailability
  if (FrameworkunifiedIsAutoPublishServiceAvailableEnabled()) {
    if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedPublishServiceAvailability(l_pHApp, FALSE))) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, "Warning:Failed to set service availability notification:0x%x ", eStatus);
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Service unavailable published");
    }
  }

  return eStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnExit
/// state cleanup can be performed in this function.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedRoot::FrameworkunifiedOnExit(CEventDataPtr f_pEventData) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Leaving state %s ", m_strStateName.c_str());
  return eFrameworkunifiedStatusOK;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// CsFrameworkunifiedApp
/// Parameterized constructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedApp::CsFrameworkunifiedApp(std::string f_pName): CFrameworkunifiedCompositeState(f_pName) {
}

///////////////////////////////////////////////////////////////////////////////////////////
/// ~CsFrameworkunifiedRoot
/// class destructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedApp::~CsFrameworkunifiedApp() {
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnEntry
/// Initializes the application and registers the service availability notification
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedApp::FrameworkunifiedOnEntry(CEventDataPtr f_pEventData) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Entering state %s ", m_strStateName.c_str());
  return eStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnExit
/// state cleanup can be performed in this function.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedApp::FrameworkunifiedOnExit(CEventDataPtr f_pEventData) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  try {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Entering state %s ", m_strStateName.c_str());
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return eStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// CsFrameworkunifiedUserChange
/// Parameterized constructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedUserChange::CsFrameworkunifiedUserChange(std::string f_pName): CFrameworkunifiedLeafState(f_pName) {
}

///////////////////////////////////////////////////////////////////////////////////////////
/// ~CsFrameworkunifiedRoot
/// class destructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedUserChange::~CsFrameworkunifiedUserChange() {
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnEntry
/// Initializes the application and registers the service availability notification
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedUserChange::FrameworkunifiedOnEntry(CEventDataPtr f_pEventData) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Entering state %s ", m_strStateName.c_str());

  if (!FrameworkunifiedIsReactionAvailable(FRAMEWORKUNIFIED_EVENT(evFrameworkunifiedLoadUserData))) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_DEV_INFO, __FUNCTION__, " Reaction not available for evFrameworkunifiedUserChangeComplete");
    FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, " No reaction for loading user specific data ");

    FrameworkunifiedPostEvent(FRAMEWORKUNIFIED_EVENT(evFrameworkunifiedUserChangeComplete));
  } else {
    FrameworkunifiedPostEvent(FRAMEWORKUNIFIED_EVENT(evFrameworkunifiedLoadUserData));
  }

  return eStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnExit
/// state cleanup can be performed in this function.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedUserChange::FrameworkunifiedOnExit(CEventDataPtr f_pEventData) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Leaving state %s ", m_strStateName.c_str());
  return eFrameworkunifiedStatusOK;
}


///////////////////////////////////////////////////////////////////////////////////////////
/// CsFrameworkunifiedInitialization
/// Parameterized constructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedInitialization::CsFrameworkunifiedInitialization(std::string f_pName): CFrameworkunifiedCompositeState(f_pName) {
}

///////////////////////////////////////////////////////////////////////////////////////////
/// ~CsFrameworkunifiedInitialization
/// class destructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedInitialization::~CsFrameworkunifiedInitialization() {
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnEntry
/// state initialization can be performed in this function.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedInitialization::FrameworkunifiedOnEntry(CEventDataPtr f_pEventData) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  try {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Entering state %s ", m_strStateName.c_str());
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());

    eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return eStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnExit
/// state cleanup can be performed in this function.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedInitialization::FrameworkunifiedOnExit(CEventDataPtr f_pEventData) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Leaving state %s ", m_strStateName.c_str());
  return eFrameworkunifiedStatusOK;
}

CFrameworkunifiedState *CFrameworkunifiedHSMParentFramework::COnEventUserChangeInInitializationState::FrameworkunifiedReaction(CFrameworkunifiedState *f_pSourceState,
                                                                                        CEventDataPtr f_pEventData) {
  try {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, "In Reaction OnEventUserChangeInInitializationState");
    CHKNULL(f_pSourceState);

    // post the event to move to sFrameworkunifiedApp state
    f_pSourceState->FrameworkunifiedPostEvent(FRAMEWORKUNIFIED_EVENT(evFrameworkunifiedInit));
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    return NULL;
  }

  return f_pSourceState;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// CsFrameworkunifiedLoadData
/// Parameterized constructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedLoadData::CsFrameworkunifiedLoadData(std::string f_pName): CFrameworkunifiedLeafState(f_pName) {
}

///////////////////////////////////////////////////////////////////////////////////////////
/// CsFrameworkunifiedLoadData
/// Parameterized constructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedLoadData::~CsFrameworkunifiedLoadData() {
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnEntry
/// state initialization can be performed in this function.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedLoadData::FrameworkunifiedOnEntry(CEventDataPtr f_pEventData) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Entering state %s ", m_strStateName.c_str());
  try {
    HANDLE l_pHApp = FrameworkunifiedGetAppHandle();
    CHKNULL(l_pHApp);

    CHKNULL(FrameworkunifiedGetStateMachine(l_pHApp));

    UI_32 l_uiCurrentEventId = FrameworkunifiedGetStateMachine(l_pHApp)->m_uiCurrentEvent;
    if (!(FRAMEWORKUNIFIED_EVENT(evFrameworkunifiedUserChangeComplete) == l_uiCurrentEventId)) {
      if (FrameworkunifiedHSMOnLoadData) {
        if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedHSMOnLoadData(l_pHApp))) {
          FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, "Warning:Failed to load application data:0x%X ", eStatus);
        }
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, "Warning: FrameworkunifiedHSMOnLoadData not defined by the application");
      }

      if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedSubscribeNotificationWithHSMEvent(l_pHApp,
                                                                          NTFY_NPPService_UserChange,
                                                                          FRAMEWORKUNIFIED_EVENT(evFrameworkunifiedUserChange)))) {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, "FrameworkunifiedSubscribeNotificationWithHSMEvent "
          "NTFY_NPPService_UserChange Failed Status:0x%x ", eStatus);
        return eStatus;
      }
      // Attach callback : Event to start the StateMachine
      if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedAttachHSMEventToDispatcher(l_pHApp,
                                                                   FRAMEWORKUNIFIED_ANY_SOURCE,
                                                                   NPS_GET_PERS_FILE_ACK,
                                                                   FRAMEWORKUNIFIED_EVENT(evFrameworkunifiedLoadFileAck)))) {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__,
               "FrameworkunifiedAttachCallbackToDispatcher NPS_GET_PERS_FILE_ACK (FRAMEWORKUNIFIED_ANY_SOURCE) Failed Status:0x%x ", eStatus);
        return eStatus;
      }
      if (!FrameworkunifiedIsReactionAvailable(FRAMEWORKUNIFIED_EVENT(evFrameworkunifiedLoadFileAck))) {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, " Reaction not available for evFrameworkunifiedLoadFileAck");

        FrameworkunifiedPostEvent(FRAMEWORKUNIFIED_EVENT(evFrameworkunifiedLoadComplete));
      }
    } else {
      FrameworkunifiedPostEvent(FRAMEWORKUNIFIED_EVENT(evFrameworkunifiedLoadComplete));
    }
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());

    eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return eStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnExit
/// state cleanup can be performed in this function.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedLoadData::FrameworkunifiedOnExit(CEventDataPtr f_pEventData) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Leaving state %s ", m_strStateName.c_str());
  try {
    HANDLE l_pHApp = FrameworkunifiedGetAppHandle();
    CHKNULL(l_pHApp);

    CFrameworkunifiedHSM *l_pHSM = FrameworkunifiedGetStateMachine(l_pHApp);
    CHKNULL(l_pHSM);

    UI_32 l_uiCurrentEventId = l_pHSM->m_uiCurrentEvent;

    if (FRAMEWORKUNIFIED_EVENT(evFrameworkunifiedStop) == l_uiCurrentEventId) {
      l_pHSM->RemoveEventFromPostedEventQueue(FRAMEWORKUNIFIED_EVENT(evFrameworkunifiedStart));
    }
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());

    eStatus = eFrameworkunifiedStatusNullPointer;
  }
  return eStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// CsFrameworkunifiedPre
/// Parameterized constructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedPre::CsFrameworkunifiedPre(std::string f_pName): CFrameworkunifiedCompositeState(f_pName) {  // LCOV_EXCL_BR_LINE 11:Excluded due to gcov constraints (others)  // NOLINT(whitespace/line_length)
}

///////////////////////////////////////////////////////////////////////////////////////////
/// ~CsFrameworkunifiedPre
/// class destructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedPre::~CsFrameworkunifiedPre() {
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnEntry
/// state initialization can be performed in this function.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedPre::FrameworkunifiedOnEntry(CEventDataPtr f_pEventData) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Entering state %s ", m_strStateName.c_str());

  HANDLE l_pHApp = FrameworkunifiedGetAppHandle();
  CHKNULL(l_pHApp);  // LCOV_EXCL_BR_LINE 15:Excluded due to inlined functions
  CHKNULL(f_pEventData);  // LCOV_EXCL_BR_LINE 15:Excluded due to inlined functions

  // Only evFrameworkunifiedPreStart or evFrameworkunifiedPreStop events occur when functions are executed.
  if (f_pEventData->m_uiEventId == FRAMEWORKUNIFIED_EVENT(evFrameworkunifiedPreStart)) {
    FRAMEWORKUNIFIEDLOG_PERFORMANCE_DEBUG("FrameworkunifiedOnPreStartInternal +");  // LCOV_EXCL_BR_LINE 15:Excluded due to inlined functions
    eStatus = FrameworkunifiedOnPreStartInternal(l_pHApp);
    if (eFrameworkunifiedStatusOK != eStatus) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, "Warning:Failed to pre start application:0x%x ", eStatus);
    }
    FRAMEWORKUNIFIEDLOG_PERFORMANCE_DEBUG("FrameworkunifiedOnPreStartInternal -");  // LCOV_EXCL_BR_LINE 15:Excluded due to inlined functions
  } else {
    FRAMEWORKUNIFIEDLOG_PERFORMANCE_DEBUG("FrameworkunifiedOnPreStopInternal +");  // LCOV_EXCL_BR_LINE 15:Excluded due to inlined functions
    eStatus = FrameworkunifiedOnPreStopInternal(l_pHApp);
    if (eFrameworkunifiedStatusOK != eStatus) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, "Warning:Failed to pre stop application:0x%x ", eStatus);
    }
    FRAMEWORKUNIFIEDLOG_PERFORMANCE_DEBUG("FrameworkunifiedOnPreStopInternal -");  // LCOV_EXCL_BR_LINE 15:Excluded due to inlined functions
  }

  return eStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnExit
/// state cleanup can be performed in this function.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedPre::FrameworkunifiedOnExit(CEventDataPtr f_pEventData) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Leaving state %s ", m_strStateName.c_str());
  return eFrameworkunifiedStatusOK;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// CsFrameworkunifiedBackground
/// Parameterized constructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedBackground::CsFrameworkunifiedBackground(std::string f_pName): CFrameworkunifiedCompositeState(f_pName) {  // LCOV_EXCL_BR_LINE 11:Excluded due to gcov constraints (others)  // NOLINT(whitespace/line_length)
}

///////////////////////////////////////////////////////////////////////////////////////////
/// ~CsFrameworkunifiedBackground
/// class destructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedBackground::~CsFrameworkunifiedBackground() {
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnEntry
/// state initialization can be performed in this function.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedBackground::FrameworkunifiedOnEntry(CEventDataPtr f_pEventData) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Entering state %s ", m_strStateName.c_str());

  HANDLE l_pHApp = FrameworkunifiedGetAppHandle();
  CHKNULL(l_pHApp);  // LCOV_EXCL_BR_LINE 15:Excluded due to inlined functions
  CHKNULL(f_pEventData);  // LCOV_EXCL_BR_LINE 15:Excluded due to inlined functions

  // Only evFrameworkunifiedBackgroundStart or evFrameworkunifiedBackgroundStop events occur when functions are executed.
  if (f_pEventData->m_uiEventId == FRAMEWORKUNIFIED_EVENT(evFrameworkunifiedBackgroundStart)) {
    FRAMEWORKUNIFIEDLOG_PERFORMANCE_DEBUG("FrameworkunifiedOnBackgroundStartInternal +");  // LCOV_EXCL_BR_LINE 15:Excluded due to inlined functions  // NOLINT(whitespace/line_length)
    eStatus = FrameworkunifiedOnBackgroundStartInternal(l_pHApp);
    if (eFrameworkunifiedStatusOK != eStatus) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, "Warning:Failed to background start application:0x%x ", eStatus);
    }
    FRAMEWORKUNIFIEDLOG_PERFORMANCE_DEBUG("FrameworkunifiedOnBackgroundStartInternal -");  // LCOV_EXCL_BR_LINE 15:Excluded due to inlined functions  // NOLINT(whitespace/line_length)
  } else {
    FRAMEWORKUNIFIEDLOG_PERFORMANCE_DEBUG("FrameworkunifiedOnBackgroundStopInternal +");  // LCOV_EXCL_BR_LINE 15:Excluded due to inlined functions  // NOLINT(whitespace/line_length)
    eStatus = FrameworkunifiedOnBackgroundStopInternal(l_pHApp);
    if (eFrameworkunifiedStatusOK != eStatus) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, "Warning:Failed to background stop application:0x%x ", eStatus);
    }
    FRAMEWORKUNIFIEDLOG_PERFORMANCE_DEBUG("FrameworkunifiedOnBackgroundStopInternal -");  // LCOV_EXCL_BR_LINE 15:Excluded due to inlined functions  // NOLINT(whitespace/line_length)
  }

  return eStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnExit
/// state cleanup can be performed in this function.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedBackground::FrameworkunifiedOnExit(CEventDataPtr f_pEventData) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Leaving state %s ", m_strStateName.c_str());
  return eFrameworkunifiedStatusOK;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// CsFrameworkunifiedRun
/// Parameterized constructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedRun::CsFrameworkunifiedRun(std::string f_pName): CFrameworkunifiedCompositeState(f_pName) {
}

///////////////////////////////////////////////////////////////////////////////////////////
/// ~CsFrameworkunifiedRun
/// class destructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedRun::~CsFrameworkunifiedRun() {
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnEntry
/// Subscribes for the service availability notification and calls FrameworkunifiedOnStart
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedRun::FrameworkunifiedOnEntry(CEventDataPtr f_pEventData) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Entering state %s ", m_strStateName.c_str());

  try {
    HANDLE hApp = FrameworkunifiedGetAppHandle();

    if (frameworkunifiedCheckValidAppHandle(hApp)) {
      FrameworkunifiedOnStartInternal(hApp);

      CFrameworkunifiedFrameworkApp *pApp = reinterpret_cast<CFrameworkunifiedFrameworkApp *>(hApp);

      for (UI_32 l_uiCount = 0; l_uiCount < pApp->servicenotificationlist.size(); l_uiCount++) {
        ServiceNotificationInfo objNotification = pApp->servicenotificationlist.at(l_uiCount);

        if (eFrameworkunifiedStatusOK != (l_eStatus =  FrameworkunifiedSubscribeNotificationWithHSMEvent
                                          (hApp, objNotification.sNotificationName, objNotification.uiEventId))) {
          FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "failed to subscribe notification %s"
                 , objNotification.sNotificationName);
        } else {
          FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, "successfully subscribe notification %s"
                 , objNotification.sNotificationName);
        }
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __PRETTY_FUNCTION__, "Application handle is NULL");
    }
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());

    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnExit
/// state cleanup can be performed in this function.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedRun::FrameworkunifiedOnExit(CEventDataPtr f_pEventData) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Leaving state %s ", m_strStateName.c_str());

  try {
    HANDLE hApp = FrameworkunifiedGetAppHandle();
    CFrameworkunifiedFrameworkApp *pApp = reinterpret_cast<CFrameworkunifiedFrameworkApp *>(hApp);

    if (frameworkunifiedCheckValidAppHandle(hApp)) {
      // Let application also handle exit of sFrameworkunifiedRun state
      if (FrameworkunifiedHSMOnStopInsFrameworkunifiedRun) {
        if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedHSMOnStopInsFrameworkunifiedRun(hApp))) {
          FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, "Warning: FrameworkunifiedHSMOnStopInsFrameworkunifiedRun returned: 0x%X ", l_eStatus);
        }
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, "Warning: FrameworkunifiedHSMOnStopInsFrameworkunifiedRun not defined by the application");
      }

      for (UI_32 l_uiCount = 0; l_uiCount < pApp->servicenotificationlist.size(); l_uiCount++) {
        ServiceNotificationInfo objNotification = pApp->servicenotificationlist.at(l_uiCount);

        if (eFrameworkunifiedStatusOK != (l_eStatus =  FrameworkunifiedUnsubscribeNotificationWithHSMEvent(hApp,
                                                                                 objNotification.sNotificationName))) {
          FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "failed to unsubscribe notification %s",
                 objNotification.sNotificationName);
        } else {
          FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, "successfully unsubscribed from notification %s",
                 objNotification.sNotificationName);
        }
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __PRETTY_FUNCTION__, "Application handle is NULL");
    }
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());

    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }
  return l_eStatus;
}

CFrameworkunifiedState *CFrameworkunifiedHSMParentFramework::COnEventUserChangeInRunState::FrameworkunifiedReaction(CFrameworkunifiedState *f_pSourceState,
                                                                             CEventDataPtr f_pEventData) {
  try {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, "In Reaction OnEventUserChangeInRunState");
    CHKNULL(f_pSourceState);

    // post the event to move to sFrameworkunifiedApp state
    f_pSourceState->FrameworkunifiedPostEvent(FRAMEWORKUNIFIED_EVENT(evFrameworkunifiedInit));

    CHKNULL(f_pSourceState);

    // post the event to move to sFrameworkunifiedRun state
    f_pSourceState->FrameworkunifiedPostEvent(FRAMEWORKUNIFIED_EVENT(evFrameworkunifiedStart));
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    return NULL;
  }
  return f_pSourceState;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// CsFrameworkunifiedLoadSessions
/// Parameterized constructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedLoadSessions::CsFrameworkunifiedLoadSessions(std::string f_pName): CFrameworkunifiedLeafState(f_pName) {
}

///////////////////////////////////////////////////////////////////////////////////////////
/// ~CsFrameworkunifiedLoadSessions
/// class destructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedLoadSessions::~CsFrameworkunifiedLoadSessions() {
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnEntry
/// state initialization can be performed in this function.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedLoadSessions::FrameworkunifiedOnEntry(CEventDataPtr f_pEventData) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  try {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, " Entering state %s ", m_strStateName.c_str());

    HANDLE hApp = FrameworkunifiedGetAppHandle();
    CHKNULL(hApp);

    CFrameworkunifiedFrameworkApp *pApp = reinterpret_cast<CFrameworkunifiedFrameworkApp *>(hApp);

    if (!pApp->servicenotificationlist.size()) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, " No Mandatory services set ");

      FrameworkunifiedPostEvent(FRAMEWORKUNIFIED_EVENT(evFrameworkunifiedReady));
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_DEV_INFO, __FUNCTION__, " Wait for service availability of mandatory services ");
    }
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());

    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnExit
/// state cleanup can be performed in this function.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedLoadSessions::FrameworkunifiedOnExit(CEventDataPtr f_pEventData) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Leaving state %s ", m_strStateName.c_str());
  return eFrameworkunifiedStatusOK;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// CsFrameworkunifiedReady
/// Parameterized constructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedReady::CsFrameworkunifiedReady(std::string f_pName): CFrameworkunifiedCompositeState(f_pName) {
}

///////////////////////////////////////////////////////////////////////////////////////////
/// ~CsFrameworkunifiedReady
/// class destructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedReady::~CsFrameworkunifiedReady() {
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnEntry
/// state initialization can be performed in this function.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedReady::FrameworkunifiedOnEntry(CEventDataPtr f_pEventData) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Entering state %s ", m_strStateName.c_str());
  try {
    HANDLE l_pHApp = FrameworkunifiedGetAppHandle();
    CHKNULL(l_pHApp);
    if (FrameworkunifiedIsAutoPublishServiceAvailableEnabled()) {  // LCOV_EXCL_BR_LINE 11:Excluded due to gcov constraints (others)
      // Publish Service available this can also be published from FrameworkunifiedOnStart callback
      if (eFrameworkunifiedStatusOK != (FrameworkunifiedPublishServiceAvailability(l_pHApp, TRUE))) {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, "warning:Failed to Publish service availability notification:0x%x ",
          l_eStatus);
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Service available published");
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Auto Service available published feature disabled");
    }
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());

    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnExit
/// state cleanup can be performed in this function.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedReady::FrameworkunifiedOnExit(CEventDataPtr f_pEventData) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Leaving state %s ", m_strStateName.c_str());
  try {
    ServiceSessionTable::iterator session_iterator;
    SessionHandleTable::iterator session_handle_iterator;

    HANDLE l_pHApp = FrameworkunifiedGetAppHandle();
    CHKNULL(l_pHApp);

    if (FrameworkunifiedIsAutoPublishServiceAvailableEnabled()) {  // LCOV_EXCL_BR_LINE 11:Excluded due to gcov constraints (others)
      // Publish Service available this can also be published from FrameworkunifiedOnStart callback
      if (eFrameworkunifiedStatusOK != (FrameworkunifiedPublishServiceAvailability(l_pHApp, FALSE))) {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, "Warning:Failed to set service availability notification:0x%x ", l_eStatus);
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Service unavailable published");
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Auto Service available published feature disabled");
    }
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());

    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// CsFrameworkunifiedStop
/// Parameterized constructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedStop::CsFrameworkunifiedStop(std::string f_pName): CFrameworkunifiedCompositeState(f_pName) {
}

///////////////////////////////////////////////////////////////////////////////////////////
/// ~CsFrameworkunifiedStop
/// class destructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedStop::~CsFrameworkunifiedStop() {
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnEntry
/// state initialization can be performed in this function.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedStop::FrameworkunifiedOnEntry(CEventDataPtr f_pEventData) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Entering state %s ", m_strStateName.c_str());
  try {
    HANDLE l_pHApp = FrameworkunifiedGetAppHandle();
    CHKNULL(l_pHApp);

    // every application need to implement this function
    // all the resource deallocation like file release,etc should be done in this callback
    l_eStatus = FrameworkunifiedOnStopInternal(l_pHApp);
    if (eFrameworkunifiedStatusOK != l_eStatus) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, "WARNING: FrameworkunifiedOnStop returned %d", l_eStatus);
      // We just need to print the error.
      // Returning this status to state machine is not required.
      // Currently, If not OK is sent to state machine while FrameworkunifiedOnEntry, then state machine becomes dead.
      l_eStatus = eFrameworkunifiedStatusOK;  // Therefore assign ok.
    }
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnExit
/// state cleanup can be performed in this function.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedStop::FrameworkunifiedOnExit(CEventDataPtr f_pEventData) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Leaving state %s ", m_strStateName.c_str());
  return eFrameworkunifiedStatusOK;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// CsFrameworkunifiedStopping
/// Parameterized constructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedStopping::CsFrameworkunifiedStopping(std::string f_pName): CFrameworkunifiedLeafState(f_pName) {
}

///////////////////////////////////////////////////////////////////////////////////////////
/// ~CsFrameworkunifiedStopping
/// class destructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedStopping::~CsFrameworkunifiedStopping() {
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnEntry
/// state initialization can be performed in this function.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedStopping::FrameworkunifiedOnEntry(CEventDataPtr f_pEventData) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Entering state %s ", m_strStateName.c_str());
  try {
    if (FrameworkunifiedIsWaitInStoppingStateEnabled()) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "Event evFrameworkunifiedStopComplete not posted. User has to post it.");
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Reaction not available for evFrameworkunifiedStopAck");
      FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "Posting event evFrameworkunifiedStopComplete");
      FrameworkunifiedPostEvent(FRAMEWORKUNIFIED_EVENT(evFrameworkunifiedStopComplete));
    }
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnExit
/// state cleanup can be performed in this function.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSMParentFramework::CsFrameworkunifiedStopping::FrameworkunifiedOnExit(CEventDataPtr f_pEventData) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Leaving state %s ", m_strStateName.c_str());
  return eFrameworkunifiedStatusOK;
}

CFrameworkunifiedState *CFrameworkunifiedHSMParentFramework::COnEventStopInStoppingState::FrameworkunifiedReaction(CFrameworkunifiedState *f_pSourceState,
                                                                            CEventDataPtr f_pEventData) {
  try {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, "In Reaction OnEventStopInStoppingState");
    CHKNULL(f_pSourceState);

    (VOID)f_pSourceState->FrameworkunifiedRemoveEventFromDeferredEventList(FRAMEWORKUNIFIED_EVENT(evFrameworkunifiedStart));
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    return NULL;
  }
  return f_pSourceState;
}

CFrameworkunifiedState *CFrameworkunifiedHSMParentFramework::COnEventStartInStopState::FrameworkunifiedReaction(CFrameworkunifiedState *f_pSourceState,
                                                                         CEventDataPtr f_pEventData) {
  try {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, "In Reaction OnEventStartInStopState");
    CHKNULL(f_pSourceState);

    // post the event to move to sFrameworkunifiedApp state
    f_pSourceState->FrameworkunifiedPostEvent(FRAMEWORKUNIFIED_EVENT(evFrameworkunifiedInit));

    CHKNULL(f_pSourceState);

    // post the event to transit to sFrameworkunifiedRun state
    f_pSourceState->FrameworkunifiedPostEvent(FRAMEWORKUNIFIED_EVENT(evFrameworkunifiedStart));
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    return NULL;
  }
  return f_pSourceState;
}

VOID CFrameworkunifiedHSMParentFramework::CaFrameworkunifiedOnStop::FrameworkunifiedAction(CFrameworkunifiedState *f_pSourceState, CFrameworkunifiedState *f_pTargetState,
                                                    CEventDataPtr f_pData) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, "In Action %s", m_strName.c_str());

  HANDLE l_pHApp = f_pSourceState->FrameworkunifiedGetAppHandle();
  if (NULL != l_pHApp) {
    FrameworkunifiedOnStopInternal(l_pHApp);
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Application handle is NULL in Action %s", m_strName.c_str());
  }
}

CFrameworkunifiedHSMChildFramework::CFrameworkunifiedHSMChildFramework(): CFrameworkunifiedHSMFramework() {
}


CFrameworkunifiedHSMChildFramework::CFrameworkunifiedHSMChildFramework(PVOID f_pHApp): CFrameworkunifiedHSMFramework(f_pHApp) {
  try {
    m_pFrameworkunifiedStateList = new std::map<UI_32, CFrameworkunifiedState *>();
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
  }
}

CFrameworkunifiedHSMChildFramework::~CFrameworkunifiedHSMChildFramework() {
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedCreate
/// Statemachine states and events created and connected in this interface.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSMChildFramework::FrameworkunifiedCreate(PVOID f_pEventData) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  try {
    // Create states
    CREATE_STATE(sFrameworkunifiedThreadRoot)
    CREATE_STATE(sFrameworkunifiedThreadIdle)
    CREATE_STATE(sFrameworkunifiedThreadStart)
    CREATE_STATE(sFrameworkunifiedThreadReady)
    CREATE_STATE(sFrameworkunifiedThreadStop)

    // connect states as per the statemachine
    CONNECT_DEFAULTSTATE(sFrameworkunifiedThreadRoot, sFrameworkunifiedThreadIdle)
    CONNECT_STATE(sFrameworkunifiedThreadRoot, sFrameworkunifiedThreadStart)
    CONNECT_STATE(sFrameworkunifiedThreadRoot, sFrameworkunifiedThreadStop)
    CONNECT_STATE(sFrameworkunifiedThreadRoot, sFrameworkunifiedThreadReady)

    // Create External Transition
    CREATE_EXTERNALTRANSITION(sFrameworkunifiedThreadIdle)
    CREATE_EXTERNALTRANSITION(sFrameworkunifiedThreadStart)
    CREATE_EXTERNALTRANSITION(sFrameworkunifiedThreadReady)
    CREATE_EXTERNALTRANSITION(sFrameworkunifiedThreadStop)

    // Connect events to state
    CONNECT_EVENT(sFrameworkunifiedThreadIdle, evFrameworkunifiedStart, sFrameworkunifiedThreadStart)
    CONNECT_EVENT(sFrameworkunifiedThreadStart, evFrameworkunifiedReady, sFrameworkunifiedThreadReady)
    CONNECT_EVENT(sFrameworkunifiedThreadStart, evFrameworkunifiedError, sFrameworkunifiedThreadIdle)
    CONNECT_EVENT(sFrameworkunifiedThreadReady, evFrameworkunifiedStop, sFrameworkunifiedThreadStop)
    CONNECT_EVENT(sFrameworkunifiedThreadStop, evFrameworkunifiedStart, sFrameworkunifiedThreadStart)

    // Create Internal Transition
    CREATE_INTERNALTRANSITION(OnDestroyThread)
    CONNECT_EVENT(sFrameworkunifiedThreadRoot, evFrameworkunifiedDestroy, OnDestroyThread)

    // Connecting AppRoot to statemachine
    CONNECTROOT(sFrameworkunifiedThreadRoot)
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());

    eStatus = eFrameworkunifiedStatusNullPointer;
  }
  return eStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// COnDestroyThread::FrameworkunifiedReaction
/// This reaction is executed when request for terminating the thread is received.
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedState *CFrameworkunifiedHSMChildFramework::COnDestroyThread::FrameworkunifiedReaction(CFrameworkunifiedState *f_pSourceState,
                                                                CEventDataPtr f_pEventData) {
  // return the NULL to exit the dispatcher loop of thread
  return NULL;
}

EFrameworkunifiedStatus CFrameworkunifiedHSMChildFramework::FrameworkunifiedFrameworkConnect(CFrameworkunifiedState *f_pAppState, BOOL f_bIsDefaultState) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  try {
    CHKNULL(m_pFrameworkunifiedStateList);
    CHKNULL(f_pAppState);

    if (m_pFrameworkunifiedStateList->find(esFrameworkunifiedThreadReady) == m_pFrameworkunifiedStateList->end()) {
      throw CNullPointerException();
    }
    CFrameworkunifiedState *l_pReady = reinterpret_cast<CFrameworkunifiedCompositeState *>(m_pFrameworkunifiedStateList->find(esFrameworkunifiedThreadReady)->second);
    CHKNULL(l_pReady);

    eStatus = FrameworkunifiedConnect(l_pReady, f_pAppState, f_bIsDefaultState);
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());

    eStatus = eFrameworkunifiedStatusNullPointer;
  }
  return eStatus;
}

EFrameworkunifiedStatus CFrameworkunifiedHSMChildFramework::FrameworkunifiedFrameworkConnect(FRAMEWORKUNIFIED_HSM_STATES f_eFrameworkunifiedState, UI_32 f_uiEventId,
                                                      CFrameworkunifiedReaction *f_pReaction,
                                                      std::string f_strEventName, BOOL f_bIsDeferredEvent) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  try {
    CHKNULL(m_pFrameworkunifiedStateList);

    if (m_pFrameworkunifiedStateList->find(f_eFrameworkunifiedState) == m_pFrameworkunifiedStateList->end()) {
      throw CNullPointerException();
    }
    CFrameworkunifiedState *l_pFrameworkunifiedState = reinterpret_cast<CFrameworkunifiedCompositeState *>(m_pFrameworkunifiedStateList->find(f_eFrameworkunifiedState)->second);

    CHKNULL(l_pFrameworkunifiedState);

    eStatus = FrameworkunifiedConnect(l_pFrameworkunifiedState, f_uiEventId, f_pReaction, f_strEventName, f_bIsDeferredEvent);
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());

    eStatus = eFrameworkunifiedStatusNullPointer;
  }
  return eStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// CFrameworkunifiedThreadRoot
/// Parameterized constructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHSMChildFramework::CsFrameworkunifiedThreadRoot::CsFrameworkunifiedThreadRoot(std::string f_pName)
  : CFrameworkunifiedCompositeState(f_pName) {
}

///////////////////////////////////////////////////////////////////////////////////////////
/// ~CsFrameworkunifiedThreadRoot
/// class destructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHSMChildFramework::CsFrameworkunifiedThreadRoot::~CsFrameworkunifiedThreadRoot() {
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnEntry
/// publishes the service availability notification
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSMChildFramework::CsFrameworkunifiedThreadRoot::FrameworkunifiedOnEntry(CEventDataPtr f_pEventData) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Entering state %s ", m_strStateName.c_str());
  return eStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnExit
/// state cleanup can be performed in this function.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSMChildFramework::CsFrameworkunifiedThreadRoot::FrameworkunifiedOnExit(CEventDataPtr f_pEventData) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Leaving state %s ", m_strStateName.c_str());
  return eFrameworkunifiedStatusOK;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// CFrameworkunifiedThreadIdle
/// Parameterized constructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHSMChildFramework::CsFrameworkunifiedThreadIdle::CsFrameworkunifiedThreadIdle(std::string f_pName)
  : CFrameworkunifiedCompositeState(f_pName) {
}

///////////////////////////////////////////////////////////////////////////////////////////
/// ~CsFrameworkunifiedThreadIdle
/// class destructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHSMChildFramework::CsFrameworkunifiedThreadIdle::~CsFrameworkunifiedThreadIdle() {
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnEntry
/// publishes the service availability notification
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSMChildFramework::CsFrameworkunifiedThreadIdle::FrameworkunifiedOnEntry(CEventDataPtr f_pEventData) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Entering state %s ", m_strStateName.c_str());
  return eStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnExit
/// state cleanup can be performed in this function.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSMChildFramework::CsFrameworkunifiedThreadIdle::FrameworkunifiedOnExit(CEventDataPtr f_pEventData) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Leaving state %s ", m_strStateName.c_str());
  return eFrameworkunifiedStatusOK;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// CFrameworkunifiedThreadStart
/// Parameterized constructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHSMChildFramework::CsFrameworkunifiedThreadStart::CsFrameworkunifiedThreadStart(std::string f_pName)
  : CFrameworkunifiedCompositeState(f_pName) {
}

///////////////////////////////////////////////////////////////////////////////////////////
/// ~CsFrameworkunifiedThreadStart
/// class destructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHSMChildFramework::CsFrameworkunifiedThreadStart::~CsFrameworkunifiedThreadStart() {
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnEntry
/// publishes the service availability notification
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSMChildFramework::CsFrameworkunifiedThreadStart::FrameworkunifiedOnEntry(CEventDataPtr f_pEventData) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  try {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Entering state %s ", m_strStateName.c_str());

    CFrameworkunifiedFrameworkApp *pApp = reinterpret_cast< CFrameworkunifiedFrameworkApp * >
                             (FrameworkunifiedGetAppHandle());
    CHKNULL(reinterpret_cast<CFrameworkunifiedHSMChildFramework *>(pApp->m_pFrameworkunifiedStateMachine));
    CHKNULL((reinterpret_cast<CFrameworkunifiedHSMChildFramework *>(pApp->m_pFrameworkunifiedStateMachine))->m_fpStartThread);
    if (eFrameworkunifiedStatusOK ==
        (reinterpret_cast<CFrameworkunifiedHSMChildFramework *>(pApp->m_pFrameworkunifiedStateMachine))->m_fpStartThread(pApp)) {
      FrameworkunifiedPostEvent(FRAMEWORKUNIFIED_EVENT(evFrameworkunifiedReady));
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, " Thread start Failed: %s", m_strStateName.c_str());
    }
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());

    eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return eStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnExit
/// state cleanup can be performed in this function.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSMChildFramework::CsFrameworkunifiedThreadStart::FrameworkunifiedOnExit(CEventDataPtr f_pEventData) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Leaving state %s ", m_strStateName.c_str());
  return eFrameworkunifiedStatusOK;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// CFrameworkunifiedThreadReady
/// Parameterized constructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHSMChildFramework::CsFrameworkunifiedThreadReady::CsFrameworkunifiedThreadReady(std::string f_pName)
  : CFrameworkunifiedCompositeState(f_pName) {
}

///////////////////////////////////////////////////////////////////////////////////////////
/// ~CsFrameworkunifiedThreadReady
/// class destructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHSMChildFramework::CsFrameworkunifiedThreadReady::~CsFrameworkunifiedThreadReady() {
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnEntry
/// publishes the service availability notification
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSMChildFramework::CsFrameworkunifiedThreadReady::FrameworkunifiedOnEntry(CEventDataPtr f_pEventData) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Entering state %s ", m_strStateName.c_str());
  return eStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnExit
/// state cleanup can be performed in this function.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSMChildFramework::CsFrameworkunifiedThreadReady::FrameworkunifiedOnExit(CEventDataPtr f_pEventData) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, " Leaving state %s ", m_strStateName.c_str());
  return eFrameworkunifiedStatusOK;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// CFrameworkunifiedThreadStop
/// Parameterized constructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHSMChildFramework::CsFrameworkunifiedThreadStop::CsFrameworkunifiedThreadStop(std::string f_pName)
  : CFrameworkunifiedLeafState(f_pName) {
}

///////////////////////////////////////////////////////////////////////////////////////////
/// ~CsFrameworkunifiedThreadStop
/// class destructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHSMChildFramework::CsFrameworkunifiedThreadStop::~CsFrameworkunifiedThreadStop() {
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnEntry
/// publishes the service availability notification
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSMChildFramework::CsFrameworkunifiedThreadStop::FrameworkunifiedOnEntry(CEventDataPtr f_pEventData) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  try {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Entering state %s ", m_strStateName.c_str());

    HANDLE hApp = FrameworkunifiedGetAppHandle();
    CHKNULL(hApp);
    CFrameworkunifiedFrameworkApp *pApp = reinterpret_cast< CFrameworkunifiedFrameworkApp * >
                             (hApp);
    CHKNULL(reinterpret_cast<CFrameworkunifiedHSMChildFramework *>(pApp->m_pFrameworkunifiedStateMachine));
    CHKNULL((reinterpret_cast<CFrameworkunifiedHSMChildFramework *>(pApp->m_pFrameworkunifiedStateMachine))->m_fpStopThread);

    eStatus = (reinterpret_cast<CFrameworkunifiedHSMChildFramework *>(pApp->m_pFrameworkunifiedStateMachine))->m_fpStopThread(pApp);

    if (eFrameworkunifiedStatusOK != eStatus) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, "WARNING: Thread stop function returned %d", eStatus);
      // We just need to print the error.
      // Returning this status to state machine is not required.
      // Currently, If not OK is sent to state machine while FrameworkunifiedOnEntry, then state machine becomes dead.
      eStatus = eFrameworkunifiedStatusOK;  // Therefore assign ok.
    }
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());

    eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return eStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnExit
/// state cleanup can be performed in this function.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSMChildFramework::CsFrameworkunifiedThreadStop::FrameworkunifiedOnExit(CEventDataPtr f_pEventData) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Leaving state %s ", m_strStateName.c_str());
  return eFrameworkunifiedStatusOK;
}
