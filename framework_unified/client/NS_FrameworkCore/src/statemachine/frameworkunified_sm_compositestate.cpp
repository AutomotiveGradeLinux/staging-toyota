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
/// This file has the CFrameworkunifiedCompositeState class definitions. CFrameworkunifiedCompositeState is derived from C
/// FrameworkunifiedState class.This class implements the additional functionality supported by HSM Composite
/// state. It provides the standard interfaces for adding state.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <native_service/frameworkunified_sm_compositestate.h>
#include <native_service/frameworkunified_sm_framework_types.h>
#include <native_service/frameworkunified_sm_hsm.h>
#include <native_service/frameworkunified_sm_historystate.h>

#include <sstream>
#include <string>
#include <map>
#include <utility>

///////////////////////////////////////////////////////////////////////////////////////////
/// CFrameworkunifiedCompositeState
/// Parameterized constructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedCompositeState::CFrameworkunifiedCompositeState(std::string f_pName): CFrameworkunifiedState(f_pName) {
  try {
    // ChildState map stores the pointers to sub state, key is state name
    m_pChildStates = new ChildStateList();

    m_pDefaultState = NULL;

    m_pActiveState = NULL;
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error: Failed in %s state ", f_pName.c_str());
  }
}

///////////////////////////////////////////////////////////////////////////////////////////
/// ~CFrameworkunifiedState
/// Class destructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedCompositeState::~CFrameworkunifiedCompositeState() {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_DEV_INFO, __FUNCTION__, "CFrameworkunifiedCompositeState destructor");
  StateIterator l_objStateIterator;
  try {
    CHKNULL(m_pChildStates);

    // Deleting the States
    for (l_objStateIterator = m_pChildStates->begin();
         l_objStateIterator != m_pChildStates->end(); l_objStateIterator++) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_DEV_INFO, __FUNCTION__, " deleting the state %s in state %s ",
             (reinterpret_cast<CFrameworkunifiedState *>((*l_objStateIterator).second))->m_strStateName.c_str(),
             m_strStateName.c_str());

      CHKNULL((*l_objStateIterator).second);

      delete(*l_objStateIterator).second;
      (*l_objStateIterator).second = NULL;
    }
    m_pChildStates->clear();
    delete m_pChildStates;
    m_pChildStates = NULL;
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error: Failed to delete state %s",
           ((reinterpret_cast<CFrameworkunifiedState *>((*l_objStateIterator).second))->m_strStateName).c_str());
  }
}
///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedAddState
/// sets the given state as a substate of the current state. If the f_eStateType is
/// eFrameworkunifiedDefaultState then substate is default state for current state.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedCompositeState::FrameworkunifiedAddState(CFrameworkunifiedState *f_pState,
                                           FRAMEWORKUNIFIED_STATE_TYPE f_eStateType) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  std::map<std::string, CFrameworkunifiedState *>::iterator l_itChildStates;
  CFrameworkunifiedHistoryState *l_pHistoryState = NULL;
  try {
    CHKNULL(f_pState);

    CHKNULL(m_pChildStates);

    // Set current state as the parent state of given state object
    f_pState->m_pParentState = this;

    // if given state is default state then set it as the default and active state of parent
    // state
    if (eFrameworkunifiedDefaultState == f_eStateType) {
      m_pDefaultState = f_pState;
      m_pActiveState = f_pState;

      // set default shallow history state
      l_itChildStates = m_pChildStates->find(SHALLOWHISTORYSTATE);
      if (m_pChildStates->end() !=  l_itChildStates) {
        l_pHistoryState = static_cast<CFrameworkunifiedHistoryState *>((*l_itChildStates).second);
        CHKNULL(l_pHistoryState);
        eStatus = l_pHistoryState->SetDefaultHistory();
      }

      // set default deep history state
      l_itChildStates = m_pChildStates->find(DEEPHISTORYSTATE);
      if (m_pChildStates->end() !=  l_itChildStates) {
        l_pHistoryState = static_cast<CFrameworkunifiedHistoryState *>((*l_itChildStates).second);
        CHKNULL(l_pHistoryState);
        eStatus = l_pHistoryState->SetDefaultHistory();
      }
    }

    // Insert the state in the map with key as state name
    m_pChildStates->insert(std::pair<std::string, CFrameworkunifiedState *>(f_pState->m_strStateName, f_pState));

    FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " %s state added in the state %s "
           , (f_pState->m_strStateName).c_str(), (this->m_strStateName).c_str());
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error: Failed to add state %s",
           f_pState->m_strStateName.c_str());

    eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return eStatus;
}
///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedGetDefaultState
/// Returns the default state of the current composite state.
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedState *CFrameworkunifiedCompositeState::FrameworkunifiedGetDefaultState() {
  return m_pDefaultState;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnEntry
/// state initialization can be performed in this function.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedCompositeState::FrameworkunifiedOnEntry(CEventDataPtr f_pEventData) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Entering state %s ", m_strStateName.c_str());
  return eFrameworkunifiedStatusOK;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnExit
/// state cleanup can be performed in this function.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedCompositeState::FrameworkunifiedOnExit(CEventDataPtr f_pEventData) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Leaving state %s ", m_strStateName.c_str());
  return eFrameworkunifiedStatusOK;
}


///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedPrintStates
/// This logs the state name and events associated with the state
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedCompositeState::FrameworkunifiedPrintStates() {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  StateIterator l_objStateIterator;
  EventReactionIterator l_objEventIterator;
  try {
    CHKNULL(m_pEventList);

    if (m_pParentState) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, "%s:%s",
             (m_pParentState->m_strStateName).c_str(), m_strStateName.c_str());
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, "NULL:%s", m_strStateName.c_str());
    }

    // print event
    for (l_objEventIterator = m_pEventList->begin();
         l_objEventIterator != m_pEventList->end(); l_objEventIterator++) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, "event %d %s", ((*l_objEventIterator).first),
             (m_pEventName->find((*l_objEventIterator).first)->second).c_str());
    }
    CHKNULL(m_pDeferredEventList);
    // print deferred events
    for (UI_32 l_uiCount = 0; l_uiCount < m_pDeferredEventList->size(); l_uiCount++) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, "deferred event %d %s", m_pDeferredEventList->at(l_uiCount),
             (m_pEventName->find(m_pDeferredEventList->at(l_uiCount))->second).c_str());
    }
    CHKNULL(m_pChildStates);
    // print states
    for (l_objStateIterator = m_pChildStates->begin();
         l_objStateIterator != m_pChildStates->end(); l_objStateIterator++) {
      (reinterpret_cast<CFrameworkunifiedState *>((*l_objStateIterator).second))->FrameworkunifiedPrintStates();
    }
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error: Failed to print in state %s",
           m_strStateName.c_str());
    eStatus = eFrameworkunifiedStatusNullPointer;
  }
  return eStatus;;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedHasSubStates
/// This indicates if the state has sub states. It returns TRUE only in the CompositeState
/// where this function is overridden
///////////////////////////////////////////////////////////////////////////////////////////
BOOL CFrameworkunifiedCompositeState::FrameworkunifiedHasSubStates() {
  return TRUE;
}

CFrameworkunifiedState *CFrameworkunifiedCompositeState::FrameworkunifiedGetActiveState() {
  CFrameworkunifiedState *l_pActiveState = NULL;
  if (NULL != m_pActiveState) {
    l_pActiveState = m_pActiveState->FrameworkunifiedGetActiveState();
  } else {
    l_pActiveState = this;
  }
  return l_pActiveState;
}

EFrameworkunifiedStatus CFrameworkunifiedCompositeState::FrameworkunifiedSetHSM(CFrameworkunifiedHSM *f_pStatemachine) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  StateIterator l_objStateIterator;
  try {
    CHKNULL(f_pStatemachine);
    // iterate child states
    for (l_objStateIterator = m_pChildStates->begin();
         l_objStateIterator != m_pChildStates->end(); l_objStateIterator++) {
      (reinterpret_cast<CFrameworkunifiedState *>((*l_objStateIterator).second))->FrameworkunifiedSetHSM(f_pStatemachine);
    }
    m_pStateMachine = f_pStatemachine;
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }
  return l_eStatus;
}

EFrameworkunifiedStatus CFrameworkunifiedCompositeState::FrameworkunifiedPrintXML(std::ostringstream &f_strXMLString) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  EventReactionIterator l_objEventIterator;
  StateIterator l_objStateIterator;
  try {
    CHKNULL(m_pEventList);
    CHKNULL(m_pEventName);
    CHKNULL(m_pDeferredEventList);

    f_strXMLString << "<" << m_strStateName.c_str() << ">";

    CHKNULL(m_pChildStates);
    f_strXMLString << "<ChildStates>";
    // print states
    for (l_objStateIterator = m_pChildStates->begin();
         l_objStateIterator != m_pChildStates->end(); l_objStateIterator++) {
      (reinterpret_cast<CFrameworkunifiedState *>((*l_objStateIterator).second))->FrameworkunifiedPrintXML(f_strXMLString);
    }

    f_strXMLString << "</ChildStates>";

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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// UpdateHistory
/// This function stores the last active state
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedCompositeState::UpdateHistory() {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  // update shallow history state
  l_eStatus = CheckHistory(SHALLOWHISTORYSTATE);

  if (eFrameworkunifiedStatusOK != l_eStatus) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "%s Shallow History Update Failed", m_strStateName.c_str());
    return l_eStatus;
  }

  // update deep history state
  l_eStatus = CheckHistory(DEEPHISTORYSTATE);

  if (eFrameworkunifiedStatusOK != l_eStatus) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "%s Deep History Update Failed", m_strStateName.c_str());
    return l_eStatus;
  }

  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// CheckHistory
/// This function searches for history state in this composite state and updates it.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedCompositeState::CheckHistory(std::string l_cHistoryType) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  CHKNULL(m_pChildStates);

  CFrameworkunifiedState *l_pHistoryState = NULL;

  std::map<std::string, CFrameworkunifiedState *>::iterator l_itChildStates;

  // update history state
  l_itChildStates = m_pChildStates->find(l_cHistoryType);

  if (m_pChildStates->end() !=  l_itChildStates) {
    l_pHistoryState = (*l_itChildStates).second;
    CHKNULL(l_pHistoryState);
    l_eStatus = l_pHistoryState->UpdateHistory();
  }

  return l_eStatus;
}
