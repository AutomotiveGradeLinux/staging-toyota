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

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_StateMachine
/// \brief
///
/// This file has the CFrameworkunifiedFinalState class definitions. CFrameworkunifiedFinalState is derived from
/// CFrameworkunifiedState class.This class implements the additional functionality supported by HSM Final
/// state.
///
///////////////////////////////////////////////////////////////////////////////
//@{

#ifndef FRAMEWORK_UNIFIED_CLIENT_NS_FRAMEWORKCORE_INCLUDE_STATEMACHINE_FRAMEWORKUNIFIED_SM_FINALSTATE_H_
#define FRAMEWORK_UNIFIED_CLIENT_NS_FRAMEWORKCORE_INCLUDE_STATEMACHINE_FRAMEWORKUNIFIED_SM_FINALSTATE_H_


#include <native_service/frameworkunified_sm_state.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
/// This class implements the additional functionality supported by HSM Final state.
///////////////////////////////////////////////////////////////////////////////////////////////////
class CFrameworkunifiedFinalState : public CFrameworkunifiedState {
 public :

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// CFrameworkunifiedFinalState
  /// Parameterized constructor
  /// \param [in] f_pName
  ///     string - Name of the state
  ///
  /// \return none
  ///////////////////////////////////////////////////////////////////////////////////////////
  CFrameworkunifiedFinalState(std::string f_pName);  // NOLINT  (readability/nolint)

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// ~CFrameworkunifiedFinalState
  /// Class destructor
  ///
  /// \return none
  ///////////////////////////////////////////////////////////////////////////////////////////
  virtual ~CFrameworkunifiedFinalState();

  virtual CFrameworkunifiedState *FrameworkunifiedGetActiveState();

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// UpdateHistory
  /// This function stores the last active state
  ///
  /// \return EFrameworkunifiedStatus
  ///     EFrameworkunifiedStatus - Returns status of operation
  ///////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus UpdateHistory();

 protected :

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedOnEntry
  /// state initialization can be performed in this function.
  ///
  /// \param [in] f_pEventData
  ///     CEventDataPtr - Event data
  ///
  /// \return EFrameworkunifiedStatus
  ///     EFrameworkunifiedStatus - Returns status of operation
  ///////////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus FrameworkunifiedOnEntry(CEventDataPtr f_pEventData);

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedOnExit
  /// state cleanup can be performed in this function.
  ///
  /// \param [in] f_pEventData
  ///     CEventDataPtr - Event data
  ///
  /// \return EFrameworkunifiedStatus
  ///     EFrameworkunifiedStatus - Returns status of operation
  ///////////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus FrameworkunifiedOnExit(CEventDataPtr f_pEventData);
};

#endif  // FRAMEWORK_UNIFIED_CLIENT_NS_FRAMEWORKCORE_INCLUDE_STATEMACHINE_FRAMEWORKUNIFIED_SM_FINALSTATE_H_
// @}
