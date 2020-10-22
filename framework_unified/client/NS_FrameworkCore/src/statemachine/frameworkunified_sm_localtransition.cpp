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
// Include Files
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <native_service/frameworkunified_sm_localtransition.h>
#include <native_service/frameworkunified_sm_state.h>
#include <native_service/frameworkunified_sm_framework_types.h>

///////////////////////////////////////////////////////////////////////////////////////////
/// CFrameworkunifiedExternalTransition
/// Parameterized constructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedLocalTransition::CFrameworkunifiedLocalTransition(CFrameworkunifiedState *f_pTargetState): CFrameworkunifiedTransition(f_pTargetState) {
}

///////////////////////////////////////////////////////////////////////////////////////////
/// ~CFrameworkunifiedExternalTransition
/// Class destructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedLocalTransition::~CFrameworkunifiedLocalTransition() {
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedReaction
/// The reaction for an event is implemented in this function. For local transition from inner
/// state to outer state exit of the inner state is invoked, but entry of outer state is not
/// invoked and from outer state to inner state entry of the inner state is invoked but exit of
/// outer state is not invoked
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedState *CFrameworkunifiedLocalTransition::FrameworkunifiedReaction(CFrameworkunifiedState *f_pSourceState, CEventDataPtr f_pData) {
  CFrameworkunifiedState *l_pActiveState = NULL;
  CFrameworkunifiedState *l_pCurrentState = NULL;
  try {
    CHKNULL(f_pSourceState);  // LCOV_EXCL_BR_LINE 15: marco defined in frameworkunified_sm_framework_types.h
    CHKNULL(m_pTargetState);  // LCOV_EXCL_BR_LINE 15: marco defined in frameworkunified_sm_framework_types.h

    // Local transition from inner state to outer state
    if (f_pSourceState->m_pParentState == m_pTargetState) {
      // Source is child of target
      l_pCurrentState = f_pSourceState->FrameworkunifiedOnHSMStop(f_pData);

      if (l_pCurrentState == f_pSourceState) {
        l_pActiveState = m_pTargetState;

      } else {
        l_pActiveState = l_pCurrentState;
      }

      /**
       * @todo
       * If the parent state is registered and OnEvent is performed without OrthogonalReigonList, illegal accesses occur.
       */
      l_pActiveState->m_pActiveState = NULL;

      // Local transition from outer state to inner state
    } else if (f_pSourceState == m_pTargetState->m_pParentState) {
      // Source is parent to target state
      l_pActiveState = m_pTargetState->FrameworkunifiedOnHSMStart(f_pData);
    }
    FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, "Local Transition from state %s to state %s"
           , f_pSourceState->m_strStateName.c_str(), m_pTargetState->m_strStateName.c_str());
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());

    return NULL;
  }

  return l_pActiveState;
}
