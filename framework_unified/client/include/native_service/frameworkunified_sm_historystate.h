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

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_StateMachine
/// \brief
///
/// This file has the declaration of CFrameworkunifiedHistoryState interface.
/// CFrameworkunifiedHistoryState is derived from CFrameworkunifiedState class.
///////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file frameworkunified_sm_historystate.h
 * @brief \~english This file has the declaration of CFrameworkunifiedHistoryState interface.
 *        CFrameworkunifiedHistoryState is derived from CFrameworkunifiedState class.
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
#ifndef _FRAMEWORKUNIFIEDHISTORYSTATE_H  // NOLINT  (build/header_guard)
#define _FRAMEWORKUNIFIEDHISTORYSTATE_H

#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_sm_state.h>
#include <string>

/**
 * @class CFrameworkunifiedHistoryState
 * \~english @brief this file has the CFrameworkunifiedHistoryState class definitions
 * \~english @par   Brief Introduction
 *        This class implements the additional functionality supported by HSM History state.
 *
 */
class CFrameworkunifiedHistoryState : public CFrameworkunifiedState {
 public :

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHistoryState
  /// \~english @par Brief
  ///        -Constructor of CFrameworkunifiedHistoryState.
  /// \~english @param [in] f_pName
  ///        std::string   - Name of the state
  /// \~english @retval None
  /// \~english @par Preconditons
  ///       - None
  /// \~english @par Change of internal status
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Detail
  ///       - This class inherits from CFrameworkunifiedState, which is the base class of state machine.
  ///         This class implements the additional functionality supported by HSM History state.
  ///         This function creates an object of CFrameworkunifiedHistoryState class.
  /// \~english @par Classification
  ///       Public
  /// \~english @see ~CFrameworkunifiedHistoryState
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// CFrameworkunifiedHistoryState
  /// Parameterized constructor
  /// \param [in] f_pName
  ///     string - Name of the state
  ///
  /// \return none
  CFrameworkunifiedHistoryState(std::string f_pName);  // NOLINT  (readability/nolint)

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHistoryState
  /// \~english @par Brief
  ///        -Destructor of CFrameworkunifiedHistoryState.
  /// \~english @param None
  /// \~english @retval None
  /// \~english @par Preconditons
  ///       -Calls constructor to successfully create object.
  /// \~english @par Change of internal status
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Detail
  ///       -Destruct the object of this class.
  /// \~english @par Classification
  ///          Public
  /// \~english @see CFrameworkunifiedHistoryState::CFrameworkunifiedHistoryState
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// ~CFrameworkunifiedHistoryState
  /// Class destructor
  ///
  /// \return none
  virtual ~CFrameworkunifiedHistoryState();

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHistoryState
  /// \~english @par Brief
  ///       - Gets the pointor of this class.
  /// \~english @param None
  /// \~english @retval None
  /// \~english @par Preconditons
  ///       - Calls constructor to successfully create object.
  /// \~english @par Change of internal status
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Detail
  ///       - Gets the pointor of this class.The pointor of this class's base class will get it.
  /// \~english @par Classification
  ///       - Public
  /// \~english @see None
  ///////////////////////////////////////////////////////////////////////////////////////////
  virtual CFrameworkunifiedState *FrameworkunifiedGetActiveState();

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHistoryState
  /// \~english @par Brief
  ///          - This function stores the last active state.
  /// \~english @param None
  /// \~english @retval EFrameworkunifiedStatus - Returns status of operation
  /// \~english @par Preconditons
  ///          - None
  /// \~english @par Change of internal status
  ///          - None
  /// \~english @par Conditions of processing failure
  ///          - None
  /// \~english @par Detail
  ///          - This function stores the last active state.
  /// \~english @par Classification
  ///          - Public
  /// \~english @see None
  ///////////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus UpdateHistory() = 0;

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHistoryState
  /// \~english @par Brief
  ///          - This function sets the default active state in history state.
  /// \~english @param None
  /// \~english @retval eFrameworkunifiedStatusOK - Success
  /// \~english @retval eFrameworkunifiedStatusNullPointer - Null Pointer
  /// \~english @par Preconditons
  ///          -Calls constructor to successfully create object.
  /// \~english @par Change of internal status
  ///          - None
  /// \~english @par Conditions of processing failure
  ///          -m_pParentState is NULL.[eFrameworkunifiedStatusNullPointer]
  /// \~english @par Detail
  ///          -This function sets the default state of parent state to the newest active state.
  /// \~english @par Classification
  ///          Public
  /// \~english @see None
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// SetDefaultHistory
  /// This function sets the default active state in history state
  /// \param
  ///
  /// \return EFrameworkunifiedStatus
  ///     EFrameworkunifiedStatus - Returns status of operation
  EFrameworkunifiedStatus SetDefaultHistory();

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
  EFrameworkunifiedStatus FrameworkunifiedOnEntry(CEventDataPtr f_pEventData);

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
  EFrameworkunifiedStatus FrameworkunifiedOnExit(CEventDataPtr f_pEventData);

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedOnHSMStart
  /// This function is called recursively till the leaf state is reached. This internally
  /// calls the Entry function of the current state.
  ///
  /// \param [in] f_pEventData
  ///     CEventDataPtr - Event data
  ///
  /// \return CurrentState
  ///     CFrameworkunifiedState* - Returns current state after operation
  ///////////////////////////////////////////////////////////////////////////////////////////
  virtual CFrameworkunifiedState *FrameworkunifiedOnHSMStart(CEventDataPtr f_pEventData);

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedOnHSMStop
  /// This function is called recursively till the required parent state is reached. This
  /// internally calls the Exit function of the current state.
  ///
  /// \param [in] f_pEventData
  ///     CEventDataPtr - Event data
  ///
  /// \return CurrentState
  ///     CFrameworkunifiedState* - Returns current state after operation
  ///////////////////////////////////////////////////////////////////////////////////////////
  virtual CFrameworkunifiedState *FrameworkunifiedOnHSMStop(CEventDataPtr f_pEventData);

  // last active state
  CFrameworkunifiedState *m_pLastActiveState;

  // event id of history state
  UI_32 m_uiEventId;

  // event name
  std::string m_cEventName;
};
#endif  // _FRAMEWORKUNIFIEDHISTORYSTATE_H  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
