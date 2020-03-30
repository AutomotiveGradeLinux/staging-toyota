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
/// This file has the CFrameworkunifiedHistoryState class definitions. CFrameworkunifiedHistoryState is derived from
/// CFrameworkunifiedState class.This class implements the additional functionality supported by HSM History
/// state.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <native_service/frameworkunified_sm_hsm.h>
#include <native_service/frameworkunified_sm_historystate.h>
#include <native_service/frameworkunified_sm_framework_types.h>
#include <native_service/frameworkunified_framework_if.h>

#include <string>

///////////////////////////////////////////////////////////////////////////////////////////
/// CFrameworkunifiedHistoryState
/// Parameterized constructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHistoryState::CFrameworkunifiedHistoryState(std::string f_pName): CFrameworkunifiedState(f_pName), m_pLastActiveState(NULL), m_uiEventId(0) {  // LCOV_EXCL_BR_LINE 11:except branch
}

///////////////////////////////////////////////////////////////////////////////////////////
/// ~CFrameworkunifiedHistoryState
/// Class destructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHistoryState::~CFrameworkunifiedHistoryState() {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_DEV_INFO, __FUNCTION__, "CFrameworkunifiedHistoryState destructor");
}

CFrameworkunifiedState *CFrameworkunifiedHistoryState::FrameworkunifiedGetActiveState() {
  return this;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnEntry
/// state initialization can be performed in this function.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHistoryState::FrameworkunifiedOnEntry(CEventDataPtr f_pEventData) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Entering state %s ", m_strStateName.c_str());

  try {
    CHKNULL(m_pLastActiveState);
    CHKNULL(m_pEventList);

    CFrameworkunifiedExternalTransition *l_pTrnTargetState = new CFrameworkunifiedExternalTransition(m_pLastActiveState);

    m_pActiveState = l_pTrnTargetState->FrameworkunifiedReaction(this, f_pEventData);
    CHKNULL(m_pActiveState);
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
EFrameworkunifiedStatus CFrameworkunifiedHistoryState::FrameworkunifiedOnExit(CEventDataPtr f_pEventData) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Leaving state %s ", m_strStateName.c_str());
  return eFrameworkunifiedStatusOK;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// SetDefaultHistory
/// This function sets the default active state in history state
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHistoryState::SetDefaultHistory() {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  try {
    CHKNULL(m_pParentState);  // LCOV_EXCL_BR_LINE 15: marco defined in frameworkunified_sm_framework_types.h
    m_pLastActiveState = m_pParentState->m_pDefaultState;
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return eStatus;  // LCOV_EXCL_BR_LINE 11: except branch
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnHSMStart
/// This function internally calls the Entry function of the current state.
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedState *CFrameworkunifiedHistoryState::FrameworkunifiedOnHSMStart(CEventDataPtr f_pEventData) {
  CFrameworkunifiedState *l_pActiveState = this;
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  try {
    // Call Entry method of the current state. Entry method of state is called in the order of
    // Hierarchy from Outer state to Inner state
    if (eFrameworkunifiedStatusOK == (eStatus = FrameworkunifiedOnEntry(f_pEventData))) {
      l_pActiveState = m_pActiveState;
    } else {
      // If FrameworkunifiedOnEntry failed then statemachine should report the error
      // We can throw an exception but for now as a quick fix we are setting
      // l_pActiveState as NULL which will stop the statemachine
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error:%d in state %s", eStatus,
             l_pActiveState->m_strStateName.c_str());
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
/// This function internally calls the Exit function of the current state.
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedState *CFrameworkunifiedHistoryState::FrameworkunifiedOnHSMStop(CEventDataPtr f_pEventData) {
  return this;
}
