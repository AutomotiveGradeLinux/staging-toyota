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
/// This file has the CFrameworkunifiedTransition class definitions.This class provides the interface
/// for reacting local transition and external transition
///
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <native_service/frameworkunified_sm_transition.h>
#include <native_service/frameworkunified_sm_state.h>

///////////////////////////////////////////////////////////////////////////////////////////
/// CFrameworkunifiedTransition
/// Parameterized constructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedTransition::CFrameworkunifiedTransition(CFrameworkunifiedState *f_pTargetState): m_pTargetState(f_pTargetState) {
  if (m_pTargetState) {
    // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
    FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, "Transition created with Target %s ",
           (m_pTargetState->m_strStateName).c_str());
    // LCOV_EXCL_BR_STOP
  }
}
///////////////////////////////////////////////////////////////////////////////////////////
/// ~CFrameworkunifiedTransition
/// Class destructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedTransition::~CFrameworkunifiedTransition() {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "+");
}

