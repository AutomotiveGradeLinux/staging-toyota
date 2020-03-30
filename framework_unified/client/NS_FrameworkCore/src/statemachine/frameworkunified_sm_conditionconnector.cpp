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

//////////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup <<Group Tag>> <<Group Name>>
/// \ingroup  tag_NSFramework
/// .
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NSFramework
/// \brief
///
///   This file has class definition of the CFrameworkunifiedConditionConnector. This class is responsible for
///   implementing interfaces required to use condition connector in statemachine.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <native_service/frameworkunified_sm_conditionconnector.h>
#include <native_service/frameworkunified_sm_state.h>
#include <native_service/frameworkunified_sm_framework_types.h>
#include <native_service/frameworkunified_sm_guard.h>

#include <vector>
#include <string>

///////////////////////////////////////////////////////////////////////////////////////////
/// CFrameworkunifiedConditionConnector
/// Parameterized constructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedConditionConnector::CFrameworkunifiedConditionConnector(std::string f_strName): CFrameworkunifiedExternalTransition(NULL),
  m_strName(f_strName) {
  // Create the condition list
  m_pConditionList = new std::vector<CCondition * >();
}

///////////////////////////////////////////////////////////////////////////////////////////
/// ~CFrameworkunifiedConditionConnector
/// destructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedConditionConnector::~CFrameworkunifiedConditionConnector() {
  if (m_pConditionList) {
    TConditionIterator l_itConditionList =  m_pConditionList->begin();

    while (m_pConditionList->end() != l_itConditionList) {
      delete *l_itConditionList;
      l_itConditionList++;
    }
    // clear condition list
    m_pConditionList->clear();
    delete m_pConditionList;
  }
}

///////////////////////////////////////////////////////////////////////////////////////////
/// CCondition
/// Parameterized constructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedConditionConnector::CCondition::CCondition(CFrameworkunifiedGuard *f_pGuard, CFrameworkunifiedState *f_pTargetState)
  : m_pGuard(f_pGuard), m_pTargetState(f_pTargetState) {
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedReaction
/// This API evaluates the guards added in the condition list. If the guard is evaluated as
/// true then statemachine transitions to target state associated with guard.
//////////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedState *CFrameworkunifiedConditionConnector::FrameworkunifiedReaction(CFrameworkunifiedState *f_pSourceState, CEventDataPtr f_pData) {
  CFrameworkunifiedState *l_pActiveState = NULL;
  BOOL l_bAllowTransition = FALSE;
  try {
    CHKNULL(f_pSourceState);
    CHKNULL(m_pConditionList);
    TConditionIterator l_itConditionList
      =  m_pConditionList->begin();

    // iterate condition list and set the target state
    while (m_pConditionList->end() != l_itConditionList) {
      CCondition *l_pCondition = *l_itConditionList;
      CHKNULL(l_pCondition);

      CFrameworkunifiedGuard *l_pGuard = l_pCondition->m_pGuard;
      CFrameworkunifiedState *l_pTargetState = l_pCondition->m_pTargetState;

      CHKNULL(l_pGuard);
      CHKNULL(l_pTargetState);

      if (l_pGuard->FrameworkunifiedEvaluate()) {
        m_pTargetState = l_pTargetState;
        l_bAllowTransition = TRUE;
        break;
      }
      l_itConditionList++;
    }

    if (l_bAllowTransition) {
      // Transition to target state
      l_pActiveState = ExecuteTransition(f_pSourceState, f_pData);

    } else {
      // No state changed
      FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__,
             "Guard condition for External Transition from state %s to condition connector %s failed",
             f_pSourceState->m_strStateName.c_str(), m_strName.c_str());

      l_pActiveState = f_pSourceState;
    }
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    return NULL;
  }
  return l_pActiveState;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedAddCondition
/// Adds condition to condition list
//////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedConditionConnector::FrameworkunifiedAddCondition(CFrameworkunifiedGuard *f_pGuard, CFrameworkunifiedState *f_pTargetState) {
  EFrameworkunifiedStatus l_eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;
  try {
    CHKNULL(m_pConditionList);
    CHKNULL(f_pGuard);
    CHKNULL(f_pTargetState);

    CCondition *l_pCondition = new CCondition(f_pGuard, f_pTargetState);

    m_pConditionList->push_back(l_pCondition);
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    l_eFrameworkunifiedStatus = eFrameworkunifiedStatusNullPointer;
  }


  return l_eFrameworkunifiedStatus;
}
