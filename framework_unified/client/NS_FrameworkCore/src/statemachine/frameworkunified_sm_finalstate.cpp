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
/// This file has the CFrameworkunifiedFinalState class definitions. CFrameworkunifiedFinalState is derived from C
/// FrameworkunifiedState class.This class implements the additional functionality supported by HSM Final
/// state.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <native_service/frameworkunified_sm_hsm.h>
#include <string>
#include "frameworkunified_sm_finalstate.h"
///////////////////////////////////////////////////////////////////////////////////////////
/// ~CFrameworkunifiedFinalState
/// Class destructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedFinalState::~CFrameworkunifiedFinalState() {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "CFrameworkunifiedFinalState destructor");
}

///////////////////////////////////////////////////////////////////////////////////////////
/// CFrameworkunifiedFinalState
/// Parameterized constructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedFinalState::CFrameworkunifiedFinalState(std::string f_pName): CFrameworkunifiedState(f_pName) {
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnEntry
/// state initialization can be performed in this function.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedFinalState::FrameworkunifiedOnEntry(CEventDataPtr f_pEventData) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Entering state %s ", m_strStateName.c_str());
  return eFrameworkunifiedStatusOK;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnExit
/// state cleanup can be performed in this function.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedFinalState::FrameworkunifiedOnExit(CEventDataPtr f_pEventData) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Leaving state %s ", m_strStateName.c_str());
  return eFrameworkunifiedStatusOK;
}

CFrameworkunifiedState *CFrameworkunifiedFinalState::FrameworkunifiedGetActiveState() {
  return this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// UpdateHistory
/// This function stores the last active state
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedFinalState::UpdateHistory() {
  return eFrameworkunifiedStatusOK;
}
