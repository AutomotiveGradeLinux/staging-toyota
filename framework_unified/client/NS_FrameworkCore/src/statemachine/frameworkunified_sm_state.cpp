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

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup <<Group Tag>> <<Group Name>>
/// \ingroup  tag_NSFramework
/// .
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NSFramework
/// \brief
///
/// This file has the CFrameworkunifiedState class implementation. CFrameworkunifiedState is base class for all types of
/// state classes.This class implements the basic functionality required for HSM state.
/// It provides the standard interfaces for entering, exiting and reacting in a state.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <native_service/frameworkunified_sm_state.h>
#include <native_service/frameworkunified_sm_reaction.h>
#include <native_service/frameworkunified_sm_framework_types.h>
#include <native_service/frameworkunified_sm_hsm.h>
#include <sstream>
#include <string>
#include <utility>
#include "frameworkunified_sm_framework_core.h"
#include "frameworkunified_framework_internal.h"

///////////////////////////////////////////////////////////////////////////////////////////
/// CFrameworkunifiedState
/// Parameterized constructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedState::CFrameworkunifiedState(std::string f_pName): m_strStateName(f_pName) {
  try {
    // EventList  stores the list of events associated with the state
    m_pEventList = new EventReactionList();  // LCOV_EXCL_BR_LINE 11:except branch

    // Deferred eventlist stores the list of deferred events associated
    // with the state
    m_pDeferredEventList = new DeferredEventList();  // LCOV_EXCL_BR_LINE 11:except branch

    // Deferred PostEventList stores the list of posted deferred events posted
    // in the state
    m_pDeferredPostEventList =   new EventInfoList();  // LCOV_EXCL_BR_LINE 11:except branch

    // EventName map stores the Name of event against event id, The event name is
    // only used for debugging so this can be disbaled in case of release build
    m_pEventName = new EventNameList();  // LCOV_EXCL_BR_LINE 11:except branch

    m_pDefaultState = NULL;
    m_pActiveState = NULL;
    m_pParentState = NULL;
    m_pStateMachine = NULL;

    FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, "%s state created ", f_pName.c_str());  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error: Failed in %s state ", f_pName.c_str());
  }
}

///////////////////////////////////////////////////////////////////////////////////////////
/// ~CFrameworkunifiedState
/// Class destructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedState::~CFrameworkunifiedState() {
  EventReactionIterator l_objEventIterator;
  EventReactionIterator l_objDeferredEventIterator;

  try {
    // Delete the event list
    CHKNULL(m_pEventList);
    for (l_objEventIterator = m_pEventList->begin();
         l_objEventIterator != m_pEventList->end();
         l_objEventIterator++) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_DEV_INFO, __FUNCTION__, " deleting the event %d in state %s",
             (*l_objEventIterator).first , m_strStateName.c_str());

      if (NULL != (*l_objEventIterator).second) {
        (*l_objEventIterator).second->m_ucRefCount--;

        if (0 == ((*l_objEventIterator).second->m_ucRefCount)) {
          delete(*l_objEventIterator).second;
          (*l_objEventIterator).second = NULL;
        }
      }
    }

    // Delete the eventlist
    m_pEventList->clear();
    delete m_pEventList;
    m_pEventList = NULL;

    // Delete the deferred eventlist
    CHKNULL(m_pDeferredEventList);
    m_pDeferredEventList->clear();
    delete m_pDeferredEventList;
    m_pDeferredEventList = NULL;

    // delete deferred Post event list
    CHKNULL(m_pDeferredPostEventList);
    m_pDeferredPostEventList->clear();
    delete m_pDeferredPostEventList;
    m_pDeferredPostEventList = NULL;

    // delete event name list
    CHKNULL(m_pEventName);
    m_pEventName->clear();
    delete m_pEventName;
    m_pEventName = NULL;
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
  }
}
///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedAddEvent
/// Associates the event id with the reaction in the state. When the event is posted to the
/// state the associated reaction is executed. This also adds the event name to the map
/// which is used for debugging.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedState::FrameworkunifiedAddEvent(UI_32 f_uiEventId, CFrameworkunifiedReaction *f_pReaction, std::string f_strEventName) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  try {
    // LCOV_EXCL_BR_START 15:marco defined in frameworkunified_sm_framework_types.h
    CHKNULL(m_pEventName);
    CHKNULL(m_pEventList);
    CHKNULL(f_pReaction);
    // LCOV_EXCL_BR_STOP
    // associate the eventname with event id (debugging only)
    m_pEventName->insert(std::pair<UI_32, std::string>(f_uiEventId, f_strEventName));

    f_pReaction->m_ucRefCount++;

    // associate the reaction with event id
    m_pEventList->insert(std::pair<UI_32, CFrameworkunifiedReaction *>(f_uiEventId, f_pReaction));

    // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
    FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, "Reaction associated with the event %d %s in the state %s "
           , f_uiEventId, (m_pEventName->find(f_uiEventId)->second).c_str(), m_strStateName.c_str());
    // LCOV_EXCL_BR_STOP
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error: Failed to add event %d %s in state %s",
           f_uiEventId , (m_pEventName->find(f_uiEventId)->second).c_str(), m_strStateName.c_str());
    eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return eStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnEvent
/// This function processes the event. If the reaction for event is available in the current
/// state within eventlist and deferred eventlist then it is consumed in the current state
/// otherwise forwarded to the parent state. Event forwarding is done recursively till either
/// event is consumed or the root state has encountered. This also process the events posted
/// in the reactions recursively till all posted events are cleared.
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedState *CFrameworkunifiedState::FrameworkunifiedOnEvent(CEventDataPtr f_pEventData) {
  CFrameworkunifiedState *l_pCurrentState = this;
  CFrameworkunifiedReaction *l_pEventReaction = NULL;

  try {
    // LCOV_EXCL_BR_START 15:marco defined in frameworkunified_sm_framework_types.h
    CHKNULL(m_pEventList);
    CHKNULL(m_pDeferredEventList);
    // LCOV_EXCL_BR_STOP
    // Find the reaction object for given event id

    /**
     * @todo
     * Unauthorized accesses will occur if NULL is specified for the event data.
     */
    if (m_pEventList->end() != m_pEventList->find(f_pEventData->m_uiEventId)) {
      l_pEventReaction = reinterpret_cast<CFrameworkunifiedReaction *>((m_pEventList->find(f_pEventData->m_uiEventId))->second);
    }

    if (l_pEventReaction) {
      // execute the reaction associated with the event
      l_pCurrentState = l_pEventReaction->FrameworkunifiedReaction(this, f_pEventData);  // LCOV_EXCL_BR_LINE 11:except branch

      // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
      FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_DEV_INFO, __FUNCTION__, "Reaction completed for event %d %s in state %s "
             , f_pEventData->m_uiEventId, (m_pEventName->find(f_pEventData->m_uiEventId)->second).c_str(),
             m_strStateName.c_str());
      // LCOV_EXCL_BR_STOP
    } else if (IsEventDeferred(f_pEventData->m_uiEventId)) {
      // If given event is deferred event then handle defer event
      CHKNULL(m_pDeferredPostEventList);

      m_pDeferredPostEventList->push_back(f_pEventData);

      FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, "deferred event %d %s posted to state %s ",
             f_pEventData->m_uiEventId,
             (m_pEventName->find(f_pEventData->m_uiEventId)->second).c_str(),
             m_strStateName.c_str());

    } else {
      // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
      FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_DEV_INFO, __FUNCTION__, "Reaction not available or event %d not found in state %s"
             , f_pEventData->m_uiEventId, m_strStateName.c_str());
      // LCOV_EXCL_BR_STOP
      // check if the current state has parent state
      if (m_pParentState) {
        // No reaction available fot given event in the current state
        // then forward event to parent state
        // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
        FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, "Forwarding an event %d to %s"
               , f_pEventData->m_uiEventId, m_pParentState->m_strStateName.c_str());
        // LCOV_EXCL_BR_STOP
        l_pCurrentState = m_pParentState->FrameworkunifiedOnEvent(f_pEventData);  // LCOV_EXCL_BR_LINE 11:except branch

      } else {
        // No parent is available then reached root state,
        // No reaction available in the statemachine then discard the event
        // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
        FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, "Discarding an event %d ", f_pEventData->m_uiEventId);
        // LCOV_EXCL_BR_STOP
      }

      CHKNULL(l_pCurrentState);

      // This is a recursive function that recurse in parent states till the event is
      // consumed or discarded So Setting the current state as the active state of the
      // returned state, as the return state is parent state
      if (l_pCurrentState->m_pActiveState) {
        l_pCurrentState = l_pCurrentState->m_pActiveState;

      } else {
        // do nothing in leaf state, as current state is active state
      }
    }
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    return NULL;
  }
  return l_pCurrentState;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedAddDeferredEvent
/// When the event is posted to the state the event is deferred and stored in the state.
/// In case of implicit recall of the deferred events, events are processed before exiting the state.
/// The deferred events can also be recalled explicitly in the state.
/// This also adds the event name to the map which is used for debugging.
//////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedState::FrameworkunifiedAddDeferredEvent(UI_32 f_uiEventId, std::string f_strEventName) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  try {
    // LCOV_EXCL_BR_START 15:marco defined in frameworkunified_sm_framework_types.h
    CHKNULL(m_pEventName);

    CHKNULL(m_pDeferredEventList);
    // LCOV_EXCL_BR_STOP
    // associate the deferred eventname with event id (debugging only)
    m_pEventName->insert(std::pair<UI_32, std::string>(f_uiEventId, f_strEventName));

    // associate the reaction with deferred event id
    m_pDeferredEventList->push_back(f_uiEventId);  // LCOV_EXCL_BR_LINE 11:except branch
    // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
    FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, "Added deferred event %d %s in the state %s "
           , f_uiEventId, (m_pEventName->find(f_uiEventId)->second).c_str(), m_strStateName.c_str());
    // LCOV_EXCL_BR_STOP
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error: Failed to add event %d %s in state %s",
           f_uiEventId, (m_pEventName->find(f_uiEventId)->second).c_str(), m_strStateName.c_str());

    eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return eStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedPostEvent
/// This function creates new eventdata object and add the to event queue of the state.
/// The events are posted in the reaction which are executed in the state.
/// The event queue is processed once the execution of the reaction is completed.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedState::FrameworkunifiedPostEvent(UI_32 f_uiEventId) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  try {
    CHKNULL(m_pStateMachine);  // LCOV_EXCL_BR_LINE 15:marco defined in frameworkunified_sm_framework_types.h

    CEventDataPtr l_pEventData(new CEventData(f_uiEventId));  // LCOV_EXCL_BR_LINE 11:except branch

    l_eStatus = m_pStateMachine->FrameworkunifiedPostEvent(l_pEventData);  // LCOV_EXCL_BR_LINE 11:except branch  // LCOV_EXCL_BR_LINE 11:except branch
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedPostEvent
/// This function adds the event queue of the state. The events are posted in the reaction
/// which are executed in the state. The event queue is processed once the execution of the
/// reaction is completed.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedState::FrameworkunifiedPostEvent(CEventDataPtr f_pEventData) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  try {
    CHKNULL(m_pStateMachine);
    CHKNULL(f_pEventData);

    l_eStatus = m_pStateMachine->FrameworkunifiedPostEvent(f_pEventData);
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnHSMStart
/// This function is called recursively till the leaf state is reached. This internally
/// calls the Entry function of the current state.
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedState *CFrameworkunifiedState::FrameworkunifiedOnHSMStart(CEventDataPtr f_pEventData) {
  CFrameworkunifiedState *l_pActiveState = this;
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  try {
    // Call Entry method of the current state. Entry method of state is called in the order of
    // Hierarchy from Outer state to Inner state
    if (eFrameworkunifiedStatusOK == (eStatus = FrameworkunifiedOnEntry(f_pEventData))) {
      // If current state has sub states then enter into active state for state entry
      // active state is same as the default state. In this case the FrameworkunifiedOnStart is called
      // recursively and recursion breaks when the current state is leafstate that does not have
      // any active/default state.
      if (m_pActiveState) {
        l_pActiveState = m_pActiveState->FrameworkunifiedOnHSMStart(f_pEventData);  // LCOV_EXCL_BR_LINE 11:except branch
      } else {
        l_pActiveState = this;
      }

      // set current state as the active state of its parent state to maintain the Hierarchy
      if (m_pParentState) {
        m_pParentState->m_pActiveState = this;
      }
    } else {
      // If FrameworkunifiedOnEntry failed then statemachine should report the error
      // We can throw an exception but for now as a quick fix we are setting
      // l_pActiveState as NULL which will stop the statemachine
      // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error:%d in FrameworkunifiedOnEntry of state %s", eStatus,
             l_pActiveState->m_strStateName.c_str());
      // LCOV_EXCL_BR_STOP
      // l_pActiveState = NULL;
      /* Commenting it, because it was making state machine inactive. This should not be the expected behavior.
       * Just log and take no action, if user return non-ok value.
       * User defined error values should be handled separately */
    }
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    return NULL;
  }

  return l_pActiveState;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnHSMStop
/// This function is called recursively till the required parent state is reached. This
/// internally calls the Exit function of the current state.
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedState *CFrameworkunifiedState::FrameworkunifiedOnHSMStop(CEventDataPtr f_pEventData) {
  CFrameworkunifiedState *l_pActiveState = this;
  try {
    // if active state is composite state, update the shallow and deep history state(if exists)
    UpdateHistory();  // LCOV_EXCL_BR_LINE 11:except branch

    // if current state has active state then recursively call the FrameworkunifiedOnHSMStop till current
    // state has no active state i.e. current state is leaf state
    if (m_pActiveState) {
      m_pActiveState->FrameworkunifiedOnHSMStop(f_pEventData);
    }

    m_pActiveState =  m_pDefaultState;

    // Post deferred events to statemachine event queue
    CHKNULL(m_pDeferredPostEventList);  // LCOV_EXCL_BR_LINE 15:marco defined in frameworkunified_sm_framework_types.h

    // recall deferred events till the vector is empty
    while (!m_pDeferredPostEventList->empty()) {
      // get the first event list object
      CEventDataPtr l_pEventData = m_pDeferredPostEventList->front();

      CHKNULL(l_pEventData);
      CHKNULL(m_pEventName);

      FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, "Recalling event %d %s in state %s"
             , l_pEventData->m_uiEventId,
             (m_pEventName->find(l_pEventData->m_uiEventId)->second).c_str(), m_strStateName.c_str());

      m_pDeferredPostEventList->erase(m_pDeferredPostEventList->begin());

      // recall the event stored in the eventinfo object
      FrameworkunifiedPostEvent(l_pEventData);
    }

    FrameworkunifiedOnExit(f_pEventData);
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    return NULL;
  }

  return l_pActiveState;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// IsEventDeferred
/// This checks if the given event is marked as deferred in the state.
///////////////////////////////////////////////////////////////////////////////////////////
BOOL CFrameworkunifiedState::IsEventDeferred(UI_32 f_uiEventId) {
  BOOL bStatus = FALSE;
  try {
    CHKNULL(m_pDeferredEventList);  // LCOV_EXCL_BR_LINE 15:marco defined in frameworkunified_sm_framework_types.h
    for (UI_32 l_uiCount = 0; l_uiCount < m_pDeferredEventList->size(); l_uiCount++) {
      if (f_uiEventId == m_pDeferredEventList->at(l_uiCount)) {
        bStatus = TRUE;
        break;
      }
    }
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
  }

  return bStatus;  // LCOV_EXCL_BR_LINE 15:marco defined in frameworkunified_sm_framework_types.h
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedRemoveEventFromDeferredEventList
/// This function removes the event from the posted deferred queue list of the state.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedState::FrameworkunifiedRemoveEventFromDeferredEventList(UI_32 f_uiEventId) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusInvldID;
  FRAMEWORKUNIFIEDLOG_CUT(ZONE_NS_FUNC, __FUNCTION__, "+");

  try {
    CHKNULL(m_pDeferredPostEventList);
    int32_t l_siCnt = static_cast<int32_t>(m_pDeferredPostEventList->size() - 1);

    for (; l_siCnt >= 0; l_siCnt--) {
      if (NULL != m_pDeferredPostEventList->at(l_siCnt).get()) {
        if (f_uiEventId == m_pDeferredPostEventList->at(l_siCnt).get()->m_uiEventId) {
          m_pDeferredPostEventList->erase(m_pDeferredPostEventList->begin() + l_siCnt);
          l_eStatus = eFrameworkunifiedStatusOK;
        }
      }
    }
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }
  FRAMEWORKUNIFIEDLOG_CUT(ZONE_NS_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedRecallEvent
/// This indicates if the state has sub states. It returns TRUE only in the CompositeState
/// where this function is overridden
///////////////////////////////////////////////////////////////////////////////////////////
BOOL CFrameworkunifiedState::FrameworkunifiedHasSubStates() {
  return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedPrintStates
/// This logs the state name and events associated with the state
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedState::FrameworkunifiedPrintStates() {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  EventReactionIterator l_objEventIterator;
  try {
    CHKNULL(m_pEventList);
    CHKNULL(m_pDeferredEventList);
    CHKNULL(m_pEventName);

    FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, "%s:%s",
           (m_pParentState->m_strStateName).c_str(), m_strStateName.c_str());

    // print events
    for (l_objEventIterator = m_pEventList->begin();
         l_objEventIterator != m_pEventList->end(); l_objEventIterator++) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, "event %d %s", ((*l_objEventIterator).first),
             (m_pEventName->find((*l_objEventIterator).first)->second).c_str());
    }

    // print deferred events
    for (UI_32 l_uiCount = 0; l_uiCount < m_pDeferredEventList->size(); l_uiCount++) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, "deferred event %d %s", m_pDeferredEventList->at(l_uiCount),
             (m_pEventName->find(m_pDeferredEventList->at(l_uiCount))->second).c_str());
    }
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error: Failed to print in state %s", m_strStateName.c_str());
    eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return eStatus;
}

BOOL CFrameworkunifiedState::FrameworkunifiedHasOrthogoanlRegions() {
  return FALSE;
}

BOOL CFrameworkunifiedState::FrameworkunifiedIsReactionAvailable(UI_32 f_uiEventId) {
  BOOL IsReactionAvailable = FALSE;
  CFrameworkunifiedReaction *l_pEventReaction = NULL;

  try {
    // LCOV_EXCL_BR_START 15:marco defined in frameworkunified_sm_framework_types.h
    CHKNULL(m_pEventList);
    CHKNULL(m_pDeferredEventList);
    // LCOV_EXCL_BR_STOP
    if (m_pEventList->end() != m_pEventList->find(f_uiEventId)) {
      // Find the reaction object for given event id
      l_pEventReaction = reinterpret_cast<CFrameworkunifiedReaction *>((m_pEventList->find(f_uiEventId))->second);
    }

    if (l_pEventReaction) {
      IsReactionAvailable = TRUE;
    } else {
      if (IsEventDeferred(f_uiEventId)) {
        IsReactionAvailable = TRUE;
      }
    }
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
  }

  return IsReactionAvailable;  // LCOV_EXCL_BR_LINE 11:except branch
}

EFrameworkunifiedStatus CFrameworkunifiedState::FrameworkunifiedSetHSM(CFrameworkunifiedHSM *f_pStatemachine) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  try {
    CHKNULL(f_pStatemachine);  // LCOV_EXCL_BR_LINE 15:marco defined in frameworkunified_sm_framework_types.h
    m_pStateMachine = f_pStatemachine;
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }
  return l_eStatus;  // LCOV_EXCL_BR_LINE 11:except branch
}

HANDLE CFrameworkunifiedState:: FrameworkunifiedGetAppHandle() {
  HANDLE l_pHApp = NULL;
  try {
    CHKNULL(m_pStateMachine);  // LCOV_EXCL_BR_LINE 15:marco defined in frameworkunified_sm_framework_types.h
    l_pHApp = m_pStateMachine->FrameworkunifiedGetAppHandle();  // LCOV_EXCL_BR_LINE 11:except branch
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
  }
  return l_pHApp;  // LCOV_EXCL_BR_LINE 11:except branch
}

EFrameworkunifiedStatus CFrameworkunifiedState::FrameworkunifiedPrintXML(std::ostringstream &f_strXMLString) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  EventReactionIterator l_objEventIterator;
  try {
    CHKNULL(m_pEventList);
    CHKNULL(m_pDeferredEventList);
    CHKNULL(m_pEventName);

    f_strXMLString << "<" << m_strStateName.c_str() << ">";

    f_strXMLString << "<EventList>";
    // print events
    for (l_objEventIterator = m_pEventList->begin();
         l_objEventIterator != m_pEventList->end(); l_objEventIterator++) {
      std::string l_strEventName =
        (m_pEventName->find((*l_objEventIterator).first)->second);

      UI_32 l_uiEventId = (*l_objEventIterator).first;

      f_strXMLString << "<Event " << "Id = " << "\"" << l_uiEventId << "\">";

      f_strXMLString << "<Name>" << l_strEventName.c_str() << "</Name>";

      f_strXMLString << "</Event>";
    }
    f_strXMLString << "</EventList>";

    // print deferred events
    f_strXMLString << "<DeferredEventList>";
    for (UI_32 l_uiCount = 0; l_uiCount < m_pDeferredEventList->size(); l_uiCount++) {
      UI_32 l_uiEventId = m_pDeferredEventList->at(l_uiCount);

      std::string l_strEventName = (m_pEventName->find(l_uiEventId)->second);

      f_strXMLString << "<Event " << "Id = " << "\"" << l_uiEventId << "\">";

      f_strXMLString << "<Name>" << l_strEventName.c_str() << "</Name>";

      f_strXMLString << "</Event>";
    }

    f_strXMLString << "</DeferredEventList>";
    f_strXMLString << "</" << m_strStateName.c_str() << ">";
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error: Failed to print in state %s", m_strStateName.c_str());
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }
  return l_eStatus;
}
