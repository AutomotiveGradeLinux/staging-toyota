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
/// This file has the CFrameworkunifiedOrthogonalState class definitions. CFrameworkunifiedOrthogonalState is derived from
/// CFrameworkunifiedCompositeState class.This class implements the additional functionality supported by HSM
/// Orthogonal state. It provides the standard interfaces for adding orthogonal state machines.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <native_service/frameworkunified_sm_orthogonalstate.h>
#include <native_service/frameworkunified_sm_compositestate.h>
#include <native_service/frameworkunified_sm_framework_types.h>
#include <native_service/frameworkunified_sm_hsm.h>
#include <sstream>
#include <string>

///////////////////////////////////////////////////////////////////////////////////////////
/// CFrameworkunifiedCompositeState
/// Parameterized constructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedOrthogonalState::CFrameworkunifiedOrthogonalState(std::string f_pName): CFrameworkunifiedState(f_pName) {
  try {
    m_pOrthogonalReigonList = new OrthogonalRegionList();
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
  }
}

///////////////////////////////////////////////////////////////////////////////////////////
/// ~CFrameworkunifiedOrthogonalState
/// Class destructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedOrthogonalState::~CFrameworkunifiedOrthogonalState() {
  for (UI_32 l_uiCount = 0; l_uiCount < m_pOrthogonalReigonList->size(); l_uiCount++) {
    if (m_pOrthogonalReigonList->at(l_uiCount)) {
      delete m_pOrthogonalReigonList->at(l_uiCount);
    }
  }
  m_pOrthogonalReigonList->clear();
  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "CFrameworkunifiedOrthogonalState destructor");
}
///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnEntry
/// state initialization can be performed in this function.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedOrthogonalState::FrameworkunifiedOnEntry(CEventDataPtr f_pEventData) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Entering state %s ", m_strStateName.c_str());
  return eFrameworkunifiedStatusOK;
}
///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnExit
/// state cleanup can be performed in this function.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedOrthogonalState::FrameworkunifiedOnExit(CEventDataPtr f_pEventData) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Leaving state %s ", m_strStateName.c_str());
  return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus CFrameworkunifiedOrthogonalState::FrameworkunifiedAddOrthogonalRegion(CFrameworkunifiedCompositeState *f_pOrthogonalRegion) {
  try {
    CHKNULL(f_pOrthogonalRegion);
    CHKNULL(m_pOrthogonalReigonList);
    f_pOrthogonalRegion->m_pParentState = this;
    m_pOrthogonalReigonList->push_back(f_pOrthogonalRegion);
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    return eFrameworkunifiedStatusNullPointer;
  }

  return eFrameworkunifiedStatusOK;
}

CFrameworkunifiedState *CFrameworkunifiedOrthogonalState::FrameworkunifiedOnHSMStart(CEventDataPtr f_pEventData) {
  CFrameworkunifiedState *l_pCurrentState = NULL;
  CFrameworkunifiedState *l_pActiveState = NULL;

  try {
    FrameworkunifiedOnEntry(f_pEventData);

    CHKNULL(m_pOrthogonalReigonList);
    for (UI_32 l_uiCount = 0; l_uiCount < m_pOrthogonalReigonList->size(); l_uiCount++) {
      if (m_pOrthogonalReigonList->at(l_uiCount)) {
        l_pActiveState = (m_pOrthogonalReigonList->at(l_uiCount));
        CHKNULL(l_pActiveState);

        l_pCurrentState = l_pActiveState->FrameworkunifiedOnHSMStart(f_pEventData);
        CHKNULL(l_pCurrentState);

        if (!IsOrthogonalChildState(l_pCurrentState)) {
          break;
        } else {
          l_pCurrentState = this;
        }
      }
    }

    // set current state as the active state of its parent state to maintain the Hierarchy
    if (m_pParentState) {
      m_pParentState->m_pActiveState = l_pCurrentState;
    }
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    return NULL;
  }

  return l_pCurrentState;
}

CFrameworkunifiedState *CFrameworkunifiedOrthogonalState::FrameworkunifiedOnHSMStop(CEventDataPtr f_pEventData) {
  CFrameworkunifiedState *l_pCurrentState = NULL;
  CFrameworkunifiedState *l_pActiveState = NULL;

  try {
    CHKNULL(m_pOrthogonalReigonList);
    for (UI_32 l_uiCount = 0; l_uiCount < m_pOrthogonalReigonList->size(); l_uiCount++) {
      if (m_pOrthogonalReigonList->at(l_uiCount)) {
        l_pActiveState = (m_pOrthogonalReigonList->at(l_uiCount));
        CHKNULL(l_pActiveState);

        l_pCurrentState = l_pActiveState->FrameworkunifiedOnHSMStop(f_pEventData);
        CHKNULL(l_pCurrentState);

        if (!IsOrthogonalChildState(l_pCurrentState)) {
          break;
        } else {
          l_pCurrentState = this;
        }
      }
    }
    FrameworkunifiedOnExit(f_pEventData);
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    return NULL;
  }

  return l_pCurrentState;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnEvent
/// This function processes the event. If the reaction for event is available in the current
/// state within eventlist and deferred eventlist then it is consumed in the current state
/// otherwise forwarded to the parent state. Event forwarding is done recursively till either
/// event is consumed or the root state has encountered. This also process the events posted
/// in the reactions recursively till all posted events are cleared.
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedState *CFrameworkunifiedOrthogonalState::FrameworkunifiedOnEvent(CEventDataPtr f_pEventData) {
  CFrameworkunifiedState *l_pCurrentState = NULL;
  CFrameworkunifiedState *l_pStateIterator = NULL;
  CFrameworkunifiedState *l_pOrthogonalRegion = NULL;
  BOOL l_bReactionAvailable = FALSE;
  BOOL l_bIsEventProcessed = FALSE;

  try {
    for (UI_32 l_uiCount = 0; l_uiCount < m_pOrthogonalReigonList->size(); l_uiCount++) {
      l_pOrthogonalRegion =  m_pOrthogonalReigonList->at(l_uiCount);

      if (l_pOrthogonalRegion) {
        // get the current active state
        l_pCurrentState = l_pOrthogonalRegion->FrameworkunifiedGetActiveState();
        CHKNULL(l_pCurrentState);

        l_pStateIterator = l_pCurrentState;

        // checks whether the reaction for the event is available in this orthogonal region
        while (this != l_pStateIterator) {
          if (l_pStateIterator->FrameworkunifiedIsReactionAvailable(f_pEventData->m_uiEventId)) {
            l_bReactionAvailable = TRUE;
            l_bIsEventProcessed = TRUE;
            break;
          }

          // iterate to parent state in orthogonal region
          l_pStateIterator = l_pStateIterator->m_pParentState;
        }

        // if reaction is found, post the event
        if (l_bReactionAvailable) {
          l_bReactionAvailable = FALSE;

          l_pCurrentState = l_pCurrentState->FrameworkunifiedOnEvent(f_pEventData);
          CHKNULL(l_pCurrentState);

          // check whether current active state is within the orthogonal state
          if (IsOrthogonalChildState(l_pCurrentState)) {
            l_pCurrentState = this;
          }

          break;
        } else {
          FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, "Reaction not available in orthogonal region %s",
                 l_pOrthogonalRegion->m_strStateName.c_str());
          l_pCurrentState = this;
        }
      }
    }

    // if event is not processed in any of orthogonal region, post the event to orthogonal state
    if (!l_bIsEventProcessed) {
      l_pCurrentState = CFrameworkunifiedState::FrameworkunifiedOnEvent(f_pEventData);

      // check whether current active state is within the orthogonal state
      if (IsOrthogonalChildState(l_pCurrentState)) {
        l_pCurrentState = this;
      }
    }
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    return NULL;
  }

  return l_pCurrentState;
}

BOOL CFrameworkunifiedOrthogonalState::FrameworkunifiedHasOrthogoanlRegions() {
  try {
    CHKNULL(m_pOrthogonalReigonList);

    if (m_pOrthogonalReigonList->size()) {
      return TRUE;
    } else {
      return FALSE;
    }
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    return FALSE;
  }
}

EFrameworkunifiedStatus CFrameworkunifiedOrthogonalState::FrameworkunifiedPrintStates() {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  try {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, "%s:%s",
           (m_pParentState->m_strStateName).c_str(), m_strStateName.c_str());

    for (UI_32 l_uiCount = 0; l_uiCount < m_pOrthogonalReigonList->size(); l_uiCount++) {
      if (m_pOrthogonalReigonList->at(l_uiCount)) {
        m_pOrthogonalReigonList->at(l_uiCount)->FrameworkunifiedPrintStates();
      }
    }
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    return eFrameworkunifiedStatusNullPointer;
  }
  return l_eStatus;
}

BOOL CFrameworkunifiedOrthogonalState::IsOrthogonalChildState(CFrameworkunifiedState *f_pChildState) {
  CFrameworkunifiedState *l_pParentState = f_pChildState;
  BOOL l_bIsOrthgonalChild = FALSE;
  while (l_pParentState) {
    if (this == l_pParentState) {
      l_bIsOrthgonalChild = TRUE;
      break;
    }

    l_pParentState = l_pParentState->m_pParentState;
  }

  return l_bIsOrthgonalChild;
}

CFrameworkunifiedState *CFrameworkunifiedOrthogonalState::FrameworkunifiedGetActiveState() {
  return this;
}

EFrameworkunifiedStatus CFrameworkunifiedOrthogonalState::FrameworkunifiedSetHSM(CFrameworkunifiedHSM *f_pStatemachine) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  try {
    CHKNULL(f_pStatemachine);
    for (UI_32 l_uiCount = 0; l_uiCount < m_pOrthogonalReigonList->size();
         l_uiCount++) {
      if (m_pOrthogonalReigonList->at(l_uiCount)) {
        m_pOrthogonalReigonList->at(l_uiCount)->FrameworkunifiedSetHSM(f_pStatemachine);
      }
    }
    m_pStateMachine = f_pStatemachine;
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }
  return l_eStatus;
}

EFrameworkunifiedStatus CFrameworkunifiedOrthogonalState::FrameworkunifiedPrintXML(std::ostringstream &f_strXMLString) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  try {
    f_strXMLString << "<" << m_strStateName.c_str() << ">";
    f_strXMLString << "<OrthogonalRegions>";

    for (UI_32 l_uiCount = 0; l_uiCount < m_pOrthogonalReigonList->size(); l_uiCount++) {
      if (m_pOrthogonalReigonList->at(l_uiCount)) {
        m_pOrthogonalReigonList->at(l_uiCount)->FrameworkunifiedPrintXML(f_strXMLString);
      }
    }

    f_strXMLString << "</OrthogonalRegions>";
    f_strXMLString << "</" << m_strStateName.c_str() << ">";
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    return eFrameworkunifiedStatusNullPointer;
  }
  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// UpdateHistory
/// This function stores the last active state
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedOrthogonalState::UpdateHistory() {
  return eFrameworkunifiedStatusOK;
}
