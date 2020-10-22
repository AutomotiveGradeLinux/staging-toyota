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
/// This file has the CFrameworkunifiedShallowHistoryState class definitions. CFrameworkunifiedShallowHistoryState is derived
/// from CFrameworkunifiedHistoryState class.This class implements the additional functionality supported by
/// HSM ShallowHistory state.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <native_service/frameworkunified_sm_shallowhistorystate.h>
#include <native_service/frameworkunified_sm_framework_types.h>
#include <string>

// define static members of class
const UI_32 CFrameworkunifiedShallowHistoryState::m_suievShallowHistory = 71;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// CFrameworkunifiedShallowHistoryState
/// Parameterized constructor
////////////////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedShallowHistoryState::CFrameworkunifiedShallowHistoryState(std::string f_pName): CFrameworkunifiedHistoryState(f_pName) {
  m_uiEventId = m_suievShallowHistory;
  m_cEventName = "evShallowHistory";
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// ~CFrameworkunifiedShallowHistoryState
/// Class destructor
////////////////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedShallowHistoryState::~CFrameworkunifiedShallowHistoryState() {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_DEV_INFO, __FUNCTION__, "CFrameworkunifiedShallowHistoryState destructor");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// UpdateHistory
/// This function stores the last active state
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedShallowHistoryState::UpdateHistory() {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  try {
    CHKNULL(m_pParentState);

    m_pLastActiveState = m_pParentState->m_pActiveState;
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return l_eStatus;
}
