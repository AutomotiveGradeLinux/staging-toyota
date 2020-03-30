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
/// This file has the CFrameworkunifiedExternalTransition class definitions. CFrameworkunifiedExternalTransition is derived
/// from CFrameworkunifiedTransition class.This class implements the FrameworkunifiedReaction interface to support transition
/// from one state to another state.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <native_service/frameworkunified_sm_externaltransition.h>
#include <native_service/frameworkunified_sm_state.h>
#include <native_service/frameworkunified_sm_framework_types.h>
#include <native_service/frameworkunified_sm_guard.h>
#include <native_service/frameworkunified_sm_action.h>
#include <vector>
///////////////////////////////////////////////////////////////////////////////////////////
/// CFrameworkunifiedState
/// Parameterized constructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedExternalTransition::CFrameworkunifiedExternalTransition(CFrameworkunifiedState *f_pTargetState): CFrameworkunifiedTransition(f_pTargetState) {
  m_pGuard = NULL;

  // Create the condition list
  m_pActionList = new std::vector<CFrameworkunifiedAction * >();  // LCOV_EXCL_BR_LINE 11:except branch
}

///////////////////////////////////////////////////////////////////////////////////////////
/// ~CFrameworkunifiedState
/// Class destructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedExternalTransition::~CFrameworkunifiedExternalTransition() {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "+");
  if (m_pActionList) {
    TActionListIterator l_itActionList =  m_pActionList->begin();

    while (m_pActionList->end() != l_itActionList) {
      if (NULL != *l_itActionList) {
        delete *l_itActionList;
        *l_itActionList = NULL;
      }
      l_itActionList++;
    }
    // clear condition list
    m_pActionList->clear();
    delete m_pActionList;
    m_pActionList = NULL;
  }
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedReaction
/// The reaction for an event is implemented in this function. For external transition, Exit
/// of the source state is called and entry of the target state is called recursively.
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedState *CFrameworkunifiedExternalTransition::FrameworkunifiedReaction(CFrameworkunifiedState *f_pSourceState, CEventDataPtr f_pData) {
  CFrameworkunifiedState *l_pActiveState = NULL;
  BOOL l_bAllowTransition = FALSE;

  try {
    CHKNULL(f_pSourceState);
    CHKNULL(m_pTargetState);

    // Check if External transition has Guard condition
    if (m_pGuard) {
      // Evaluate guard condition
      l_bAllowTransition = m_pGuard->FrameworkunifiedEvaluate();
    } else {
      // If Guard is not available then allow transition
      l_bAllowTransition = TRUE;
    }

    if (l_bAllowTransition) {
      l_pActiveState = ExecuteTransition(f_pSourceState, f_pData);  // LCOV_EXCL_BR_LINE 11:except branch

    } else {
      // No state changed
      FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, "Guard condition for External Transition "
        "from state %s to state %s failed"
        , f_pSourceState->m_strStateName.c_str(), m_pTargetState->m_strStateName.c_str());

      l_pActiveState = f_pSourceState;
    }
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    return NULL;
  }

  return l_pActiveState;
}

EFrameworkunifiedStatus CFrameworkunifiedExternalTransition::FrameworkunifiedSetGuard(CFrameworkunifiedGuard *f_pGuard) {
  EFrameworkunifiedStatus l_eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;
  try {
    CHKNULL(f_pGuard);
    m_pGuard = f_pGuard;
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    l_eFrameworkunifiedStatus = eFrameworkunifiedStatusNullPointer;
  }

  return l_eFrameworkunifiedStatus;
}

CFrameworkunifiedState *CFrameworkunifiedExternalTransition::ExecuteTransition(CFrameworkunifiedState *f_pSourceState, CEventDataPtr f_pData) {
  CFrameworkunifiedState *l_pActiveState = NULL;
  try {
    CHKNULL(m_pTargetState);
    CHKNULL(f_pSourceState);
    CFrameworkunifiedState *l_pTargetState = m_pTargetState;
    CFrameworkunifiedState *l_pCurrentState = f_pSourceState;




    // Find the common parent of the source and target state
    // and then call recursively  OnExit on source state and
    // OnEntry on parent state
    BOOL l_bFoundCommonParent = FALSE;
    while (l_pCurrentState->m_pParentState) {
      while (l_pTargetState->m_pParentState) {
        // Check if current source state and target state has common parent
        if (l_pCurrentState->m_pParentState == l_pTargetState->m_pParentState) {
          l_bFoundCommonParent = TRUE;
          break;
        } else {
          // Set the next target parent state
          l_pTargetState = l_pTargetState->m_pParentState;
        }
      }

      if (l_bFoundCommonParent) {
        break;
      } else {
        // Set the next source parent state
        l_pCurrentState = l_pCurrentState->m_pParentState;
        l_pTargetState = m_pTargetState;
      }
    }

    if (l_bFoundCommonParent) {
      // recursively execute the exit on the current state
      l_pCurrentState->FrameworkunifiedOnHSMStop(f_pData);

      // execute actions associated with the external transition
      CHKNULL(m_pActionList);
      TActionListIterator l_itActionList =  m_pActionList->begin();

      // iterate action list and execute actions
      while (m_pActionList->end() != l_itActionList) {
        CFrameworkunifiedAction *l_pAction = *l_itActionList;
        CHKNULL(l_pAction);

        l_pAction->FrameworkunifiedAction(f_pSourceState, l_pTargetState, f_pData);

        l_itActionList++;
      }

      // Set the target state as a active state
      CFrameworkunifiedState *l_pState = m_pTargetState;
      while (l_pState->m_pParentState != l_pTargetState->m_pParentState) {
        CHKNULL(l_pState->m_pParentState);

        l_pState->m_pParentState->m_pActiveState = l_pState;
        l_pState = l_pState->m_pParentState;
      }


      // recursively execute the entry on the target state
      l_pActiveState = l_pTargetState->FrameworkunifiedOnHSMStart(f_pData);

      FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, "External Transition from state %s to state %s"
             , f_pSourceState->m_strStateName.c_str(), m_pTargetState->m_strStateName.c_str());
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, "External Transition from state %s to state %s failed"
             , f_pSourceState->m_strStateName.c_str(), m_pTargetState->m_strStateName.c_str());
      return NULL;
    }
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
  }

  return l_pActiveState;
}

EFrameworkunifiedStatus CFrameworkunifiedExternalTransition::FrameworkunifiedAddAction(CFrameworkunifiedAction *f_pAction) {
  EFrameworkunifiedStatus l_eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;
  try {
    CHKNULL(m_pActionList);

    CHKNULL(f_pAction);

    m_pActionList->push_back(f_pAction);
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    l_eFrameworkunifiedStatus = eFrameworkunifiedStatusNullPointer;
  }

  return l_eFrameworkunifiedStatus;
}
