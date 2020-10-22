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
/// This file has the CFrameworkunifiedLeafState class definitions. CFrameworkunifiedLeafState is derived from C
/// FrameworkunifiedState class.This class implements the additional functionality supported by HSM Leafstate.
///////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file frameworkunified_sm_leafstate.h
 * @brief \~english This file has the CFrameworkunifiedLeafState class definitions. CFrameworkunifiedLeafState is derived from C
 *        FrameworkunifiedState class.This class implements the additional functionality supported by HSM Leafstate.
 *
 */
/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup native_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup framework_unified
 *  @ingroup native_service
 *  @{
 */
/** @addtogroup framework
 *  @ingroup framework_unified
 *  @{
 */
/** @addtogroup statemachine
 *  @ingroup framework
 *  @{
 */
#ifndef _FRAMEWORKUNIFIEDLEAFSTATE_H  // NOLINT  (build/header_guard)
#define _FRAMEWORKUNIFIEDLEAFSTATE_H

///////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <native_service/frameworkunified_sm_state.h>
#include <string>
///////////////////////////////////////////////////////////////////////////////////////////////////
/// This class implements the additional functionality supported by HSM Leafstate.
///////////////////////////////////////////////////////////////////////////////////////////////////
class CFrameworkunifiedLeafState : public CFrameworkunifiedState {
 public :

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedLeafState
  /// \~english @par Brief
  ///        It is a constructor for CFrameworkunifiedLeafState classes.
  /// \~english @param [in] f_pName
  ///        std::string   - state name
  /// \~english @retval None
  /// \~english @par Prerequisite
  ///       - None
  /// \~english @par Change of internal state
  ///       - The internal state is not changed.
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Detail
  ///       Inherits CFrameworkunifiedState, which is a superclass of state machines.\n
  ///       This class is implemented by HSM Leafstate as an additional class of functional support.\n
  ///       It inherits from the CFrameworkunifiedState classes and instantiates the CFrameworkunifiedLeafState.
  /// \~english @par Immediate
  ///          Brief
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Not applicable
  /// \~english @see ~CFrameworkunifiedLeafState
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// CFrameworkunifiedState
  /// Parameterized constructor
  /// \param [in] f_pName
  ///     string - Name of the state
  ///
  /// \return none
  CFrameworkunifiedLeafState(std::string f_pName);  // NOLINT  (readability/nolint)

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedLeafState
  /// \~english @par Brief
  ///        Destructors for CFrameworkunifiedLeafState classes.
  /// \~english @retval None
  /// \~english @par Prerequisite
  ///       - Its own instance (CFrameworkunifiedLeafState) has been created.
  /// \~english @par Change of internal state
  ///       - The internal state is not changed.
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Detail
  ///       Frees instances of CFrameworkunifiedLeafState classes.
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Not applicable
  /// \~english @see CFrameworkunifiedLeafState::CFrameworkunifiedLeafState
  ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// ~CFrameworkunifiedState
  /// Class destructor
  ///
  /// \return none
  virtual ~CFrameworkunifiedLeafState();

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedLeafState
  /// \~english @par Brief
  ///        Get the active state.
  /// \~english @retval CFrameworkunifiedState* Pointers to active-state (CFrameworkunifiedState classes) objects
  /// \~english @par Prerequisite
  ///       - Its own instance (CFrameworkunifiedLeafState) has been created.
  /// \~english @par Change of internal state
  ///       - The internal state is not changed.
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Detail
  ///       This method overrides and uses CFrameworkunifiedState::FrameworkunifiedGetActiveState, which is pure virtual functions.\n
  ///       Returns the current state (this) as the active state.
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Not applicable
  /// \~english @see CFrameworkunifiedState::FrameworkunifiedGetActiveState
  ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedGetActiveState
  /// This interface returns the Active state of the current composite state. In case of
  /// non-composite state current state is active state
  ///
  /// \return Active state
  ///     CFrameworkunifiedState* - Active state
  virtual CFrameworkunifiedState *FrameworkunifiedGetActiveState();

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedLeafState
  /// \~english @par Brief
  ///        Save the last active state.
  /// \~english @retval eFrameworkunifiedStatusOK success
  /// \~english @par Prerequisite
  ///       - Its own instance (CFrameworkunifiedLeafState) has been created.
  /// \~english @par Change of internal state
  ///       - The internal state is not changed.
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Detail
  ///       This method overrides CFrameworkunifiedState::UpdateHistory, which is pure virtual functions.Used and saves the last active state.
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Not applicable
  /// \~english @see CFrameworkunifiedState::UpdateHistory
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// UpdateHistory
  /// This function stores the last active state
  ///
  /// \return EFrameworkunifiedStatus
  ///     EFrameworkunifiedStatus - Returns status of operation
  EFrameworkunifiedStatus UpdateHistory();

 protected :

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedLeafState
  /// \~english @par Brief
  ///        Initialize state
  /// \~english @param f_pEventData
  ///      CEventDataPtr - Pointer to the event data class
  /// \~english @retval eFrameworkunifiedStatusOK success
  /// \~english @par Prerequisite
  ///       - Its own instance (CFrameworkunifiedLeafState) has been created.
  /// \~english @par Change of internal state
  ///       - The internal state is not changed.
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Detail
  ///       This method overrides and uses CFrameworkunifiedState::FrameworkunifiedOnEntry, which is pure virtual functions, to initialize state.
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Not applicable
  /// \~english @see CFrameworkunifiedState::FrameworkunifiedOnEntry, FrameworkunifiedOnExit
  ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedOnEntry
  /// state initialization can be performed in this function.
  ///
  /// \param [in] f_pEventData
  ///     CEventDataPtr - Event data
  ///
  /// \return EFrameworkunifiedStatus
  ///     EFrameworkunifiedStatus - Returns status of operation
  virtual EFrameworkunifiedStatus FrameworkunifiedOnEntry(CEventDataPtr f_pEventData);

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedLeafState
  /// \~english @par Brief
  ///        Clean up state.
  /// \~english @param f_pEventData
  ///      CEventDataPtr - Pointer to the event data class
  /// \~english @retval eFrameworkunifiedStatusOK success
  /// \~english @par Prerequisite
  ///       - Its own instance (CFrameworkunifiedLeafState) has been created.
  /// \~english @par Change of internal state
  ///       - The internal state is not changed.
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Detail
  ///       This method overrides and uses CFrameworkunifiedState::FrameworkunifiedOnExit, which is pure virtual functions, to clean up state.
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Not applicable
  /// \~english @see CFrameworkunifiedState::FrameworkunifiedOnExit, FrameworkunifiedOnEntry
  ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedOnExit
  /// state cleanup can be performed in this function.
  ///
  /// \param [in] f_pEventData
  ///     CEventDataPtr - Event data
  ///
  /// \return EFrameworkunifiedStatus
  ///     EFrameworkunifiedStatus - Returns status of operation
  virtual EFrameworkunifiedStatus FrameworkunifiedOnExit(CEventDataPtr f_pEventData);
};

#endif  // _FRAMEWORKUNIFIEDLEAFSTATE_H  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
