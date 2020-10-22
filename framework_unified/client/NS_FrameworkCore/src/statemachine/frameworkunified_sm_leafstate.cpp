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
/// This file has the CFrameworkunifiedLeafState class definitions. CFrameworkunifiedLeafState is derived from C
/// FrameworkunifiedState class.This class implements the additional functionality supported by HSM Leafstate.
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <native_service/frameworkunified_sm_leafstate.h>
#include <native_service/frameworkunified_sm_framework_types.h>
#include <string>

///////////////////////////////////////////////////////////////////////////////////////////
/// CFrameworkunifiedState
/// Parameterized constructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedLeafState::CFrameworkunifiedLeafState(std::string f_pName): CFrameworkunifiedState(f_pName) {  // LCOV_EXCL_BR_LINE 11:Unexpected branch
}

///////////////////////////////////////////////////////////////////////////////////////////
/// ~CFrameworkunifiedState
/// Class destructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedLeafState::~CFrameworkunifiedLeafState() {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, "CFrameworkunifiedLeafState destructor");
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnEntry
/// state initialization can be performed in this function.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedLeafState::FrameworkunifiedOnEntry(CEventDataPtr f_pEventData) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Entering state %s ", m_strStateName.c_str());
  return eFrameworkunifiedStatusOK;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnExit
/// state cleanup can be performed in this function.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedLeafState::FrameworkunifiedOnExit(CEventDataPtr f_pEventData) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Leaving state %s ", m_strStateName.c_str());
  return eFrameworkunifiedStatusOK;
}

CFrameworkunifiedState *CFrameworkunifiedLeafState::FrameworkunifiedGetActiveState() {
  return this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// UpdateHistory
/// This function stores the last active state
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedLeafState::UpdateHistory() {
  return eFrameworkunifiedStatusOK;
}
