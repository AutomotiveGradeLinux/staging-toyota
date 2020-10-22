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
/// This file has the CFrameworkunifiedCompositeState class definitions. CFrameworkunifiedCompositeState is derived from C
/// FrameworkunifiedState class.This class implements the additional functionality supported by HSM Composite
/// state. It provides the standard interfaces for adding state.
///
///////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file frameworkunified_sm_compositestate.h
 * @brief \~english This file has the CFrameworkunifiedCompositeState class definitions. CFrameworkunifiedCompositeState is derived from C
 *        FrameworkunifiedState class.This class implements the additional functionality supported by HSM Composite
 *        state. It provides the standard interfaces for adding state.
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
#ifndef _FRAMEWORKUNIFIEDCOMPOSITESTATE_H  // NOLINT  (build/header_guard)
#define _FRAMEWORKUNIFIEDCOMPOSITESTATE_H

///////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <native_service/frameworkunified_sm_state.h>
#include <map>
#include <string>

typedef std::map<std::string, CFrameworkunifiedState *> ChildStateList;
typedef std::map<std::string, CFrameworkunifiedState *>::iterator StateIterator;

class CFrameworkunifiedHSM;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// This class implements the additional functionality supported by HSM Composite
/// state. It provides the standard interfaces for adding state.
///////////////////////////////////////////////////////////////////////////////////////////////////
class CFrameworkunifiedCompositeState : public CFrameworkunifiedState {
 public :

  typedef enum _FRAMEWORKUNIFIED_STATE_TYPE {
    eFrameworkunifiedState = 0,
    eFrameworkunifiedDefaultState
  } FRAMEWORKUNIFIED_STATE_TYPE;

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedCompositeState
  /// \~english @par Brief
  ///          Constructor for class CFrameworkunifiedCompositeState.
  /// \~english @param [in] f_strName
  ///          std::string -Name of state
  /// \~english @retval
  /// \~english @par Prerequisite
  ///          none
  /// \~english @par Change of internal state
  ///          none
  /// \~english @par Conditions of processing failure
  ///          none
  /// \~english @par Detail
  ///          This class inherit from CFrameworkunifiedState(basic class of state machine), \n
  ///          and create object of CFrameworkunifiedCompositeState class. \n
  ///          Create and initialize child state, and set the pointor to member variable. \n
  ///          Initialize other member variables with NULL. \n
  ///          Output the process error logs.
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///           none
  /// \~english @see CFrameworkunifiedState, ~CFrameworkunifiedCompositeState
  ///////////////////////////////////////////////////////////////////////////////////////////
  CFrameworkunifiedCompositeState(std::string f_pName);  // NOLINT  (readability/nolint)

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedCompositeState
  /// \~english @par Brief
  ///        Destructor for CFrameworkunifiedCompositeState
  /// \~english @retval none
  /// \~english @par Preconditons
  ///          - Self instance of CFrameworkunifiedCompositeState created.
  /// \~english @par Change of internal status
  ///                none
  /// \~english @par Conditions of processing failure
  ///                none
  /// \~english @par Detail
  ///           Release the objects of CFrameworkunifiedState class and CFrameworkunifiedCompositeState class. \n
  ///           Release all the child states registered in child states list of member variable. \n
  ///           Output error log if failed.
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///            none
  /// \~english @see CFrameworkunifiedState, CFrameworkunifiedCompositeState::CFrameworkunifiedCompositeState
  ///////////////////////////////////////////////////////////////////////////////////////////
  virtual ~CFrameworkunifiedCompositeState();

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedCompositeState
  /// \~english @par Brief
  ///           sets the given state as a substate of the current state. If the f_eStateType is
  ///           eFrameworkunifiedDefaultState then substate is default state for current state.
  /// \~english @param [in] f_pState
  ///           CFrameworkunifiedState* - Pointer to the substate object to be added in the current state
  /// \~english @param [in] f_eStateType
  ///           FRAMEWORKUNIFIED_STATE_TYPE - state type indicating if it is default state
  /// \~english @retval EFrameworkunifiedStatus
  ///            success or fail
  /// \~english @par Preconditons
  ///        -  Self instance of CFrameworkunifiedCompositeState created.
  /// \~english @par Change of internal status
  ///        -  none
  /// \~english @par Conditions of processing failure
  ///           - If parameter f_pState is NULL. [eFrameworkunifiedStatusNullPointer]
  ///            - If m_pChildStates is NULL. [eFrameworkunifiedStatusNullPointer]
  /// \~english @par Detail
  ///           sets the given state as a substate of the current state. If the f_eStateType is
  ///              eFrameworkunifiedDefaultState then substate is default state for current state.
  /// \~english @par Classification
  ///          public
  /// \~english @par Type
  ///          none
  /// \~english @see none
  ////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus FrameworkunifiedAddState(CFrameworkunifiedState *f_pState,
                         FRAMEWORKUNIFIED_STATE_TYPE f_eStateType = eFrameworkunifiedState);

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedCompositeState
  /// \~english @par Brief
  ///           Returns the default state of the current composite state.
  /// \~english @param none
  /// \~english @retval CFrameworkunifiedState*  - pointer default state
  /// \~english @par Preconditons
  ///        -  Self instance of CFrameworkunifiedCompositeState created.
  /// \~english @par Change of internal status
  ///        -  none
  /// \~english @par Conditions of processing failure
  ///           none
  /// \~english @par Detail
  ///           Returns the default state of the current composite state.
  /// \~english @par Classification
  ///          public
  /// \~english @par Type
  ///          none
  /// \~english @see none
  ////////////////////////////////////////////////////////////////////////////////////////////
  CFrameworkunifiedState *FrameworkunifiedGetDefaultState();

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedCompositeState
  /// \~english @par Brief
  ///           Logs the state name and events associated with the state.
  /// \~english @param none
  /// \~english @retval eFrameworkunifiedStatusOK succ
  /// \~english @retval eFrameworkunifiedStatusNullPointer failed
  /// \~english @par Preconditons
  ///        -  Self instance of CFrameworkunifiedCompositeState created.
  /// \~english @par Change of internal status
  ///        -  none
  /// \~english @par Conditions of processing failure
  ///        -  Don't create instance of CFrameworkunifiedCompositeState [eFrameworkunifiedStatusNullPointer]
  /// \~english @par Detail
  ///           This function overrides the fuction of CFrameworkunifiedState::FrameworkunifiedPrintStates.
  ///           This function outputs log of state name and events associated with the state as follows:
  ///           - state name of parent state machine and self state machine
  ///           - all the event names in event list
  ///           - all the event names in delay event list
  /// \~english @par Classification
  ///          public
  /// \~english @par Type
  ///          none
  /// \~english @see none
  ///////////////////////////////////////////////////////////////////////////////////////////
  virtual  EFrameworkunifiedStatus FrameworkunifiedPrintStates();

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedCompositeState
  /// \~english @par Brief
  ///    This indicates if the state has sub states. It returns TRUE only in the CompositeState
  ///          where this function is overridden
  /// \~english @param none
  /// \~english @retval BOOL   TRUE - has substate FALSE - has not substatue
  /// \~english @par Preconditons
  ///        -  Self instance of CFrameworkunifiedCompositeState created.
  /// \~english @par Change of internal status
  ///        -  none
  /// \~english @par Conditions of processing failure
  ///        -  none
  /// \~english @par Detail
  ///           This function overrides the fuction of CFrameworkunifiedState::FrameworkunifiedHasSubStates.
  ///           Return true only in CFrameworkunifiedCompositeState class.
  /// \~english @par Classification
  ///          public
  /// \~english @par Type
  ///          none
  /// \~english @see CFrameworkunifiedState::FrameworkunifiedHasSubStates
  virtual BOOL FrameworkunifiedHasSubStates();

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedCompositeState
  /// \~english @par Brief
  ///       This interface returns the Active state of the current composite state. In case of
  ///          non-composite state current state is active state
  /// \~english @param none
  /// \~english @retval CFrameworkunifiedState*  pointer of active state
  /// \~english @par Preconditons
  ///        -  Self instance of CFrameworkunifiedCompositeState created.
  /// \~english @par Change of internal status
  ///        -  none
  /// \~english @par Conditions of processing failure
  ///        -  none
  /// \~english @par Detail
  ///           This function overrides the pure virtual fuction of CFrameworkunifiedState::FrameworkunifiedGetActiveState.
  ///           Reture active state of current composite state. If it isn't a composite state,
  ///           return current state as an active state.
  /// \~english @par Classification
  ///          public
  /// \~english @par Type
  ///          none
  /// \~english @see CFrameworkunifiedState::FrameworkunifiedGetActiveState
  ///////////////////////////////////////////////////////////////////////////////////////////
  virtual CFrameworkunifiedState *FrameworkunifiedGetActiveState();

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedCompositeState
  /// \~english @par Brief
  ///       This function stores the last active state
  /// \~english @param none
  /// \~english @retval EFrameworkunifiedStatus  eFrameworkunifiedStatusOK
  /// \~english @par Preconditons
  ///        -  Self instance of CFrameworkunifiedCompositeState created.
  /// \~english @par Change of internal status
  ///        -  none
  /// \~english @par Conditions of processing failure
  ///        -  none
  /// \~english @par Detail
  ///           This function overrides the pure virtual fuction of CFrameworkunifiedState::UpdateHistory.
  ///           Store the last active state.
  /// \~english @par Classification
  ///          public
  /// \~english @par Type
  ///          none
  /// \~english @see CFrameworkunifiedState::UpdateHistory
  ///////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus UpdateHistory();

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedCompositeState
  /// \~english @par Brief
  ///       Update the State information in the given stream in the form of XML tags
  ///
  /// \~english @param f_strXMLString
  ///         std::ostringstream & - reference to the XML stream
  /// \~english @retval EFrameworkunifiedStatus  eFrameworkunifiedStatusOK
  ///                               eFrameworkunifiedStatusNullPointer
  /// \~english @par Preconditons
  ///        -  Self instance of CFrameworkunifiedCompositeState created.
  ///        -  State machine of current state in the applicaton is setted in FrameworkunifiedSetHSM.
  /// \~english @par Change of internal status
  ///        -  Self instance of CFrameworkunifiedCompositeState created failed. [eFrameworkunifiedStatusNullPointer].
  /// \~english @par Conditions of processing failure
  ///        -  none
  /// \~english @par Detail
  ///           This function overrides the fuction of CFrameworkunifiedState::FrameworkunifiedPrintXML.
  ///           Set parameter f_strXMLString with the following state information by XML type.
  ///           - state name
  ///           - all the event information in event list(event ID, event name)
  ///           - all the event information in delay event list(event ID, event name)
  /// \~english @par Classification
  ///          public
  /// \~english @par Type
  ///          none
  /// \~english @see CFrameworkunifiedState::FrameworkunifiedPrintXML
  ///////////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus FrameworkunifiedPrintXML(std::ostringstream &f_strXMLString);


 protected :

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedCompositeState
  /// \~english @par Brief
  ///        Initialize state
  /// \~english @param f_pEventData
  ///       CEventDataPtr - Pointer to the event data class
  /// \~english @retval eFrameworkunifiedStatusOK
  /// \~english @par Preconditions
  ///       - Its own instance (CFrameworkunifiedCompositeState) has been created.
  /// \~english @par Change of internal status
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
  ///
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
  /// \ingroup CFrameworkunifiedCompositeState
  /// \~english @par Brief
  ///        Clean up state.
  /// \~english @param f_pEventData
  ///       CEventDataPtr - Pointer to the event data class
  /// \~english @retval eFrameworkunifiedStatusOK
  /// \~english @par Preconditions
  ///       - Its own instance (CFrameworkunifiedCompositeState) has been created.
  /// \~english @par Change of internal status
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
  ///
  /// state cleanup can be performed in this function.
  ///
  /// \param [in] f_pEventData
  ///     CEventDataPtr - Event data
  ///
  /// \return EFrameworkunifiedStatus
  ///     EFrameworkunifiedStatus - Returns status of operation
  ///////////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus FrameworkunifiedOnExit(CEventDataPtr f_pEventData);

  /// stores the pointers to sub state, key is state name
  ChildStateList *m_pChildStates;

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedCompositeState
  /// \~english @par Brief
  ///        Associate the state machine with the current state.
  /// \~english @param [in] f_pStatemachine
  ///      CFrameworkunifiedHSM*  - Pointers to state machines (CFrameworkunifiedHSM classes)
  /// \~english @retval eFrameworkunifiedStatusOK
  /// \~english @retval eFrameworkunifiedStatusNullPointer  Specifying NULL pointers
  /// \~english @par Preconditions
  ///       - Its own instance (CFrameworkunifiedCompositeState) has been created.
  /// \~english @par Change of internal status
  ///       - The internal state is not changed.
  /// \~english @par Conditions of processing failure
  ///       - When the pointers to the state machines (CFrameworkunifiedHSM classes) specified in the arguments (f_pStatemachine) are NULL [eFrameworkunifiedStatusNullPointer]
  /// \~english @par Detail
  ///       This method is used by overriding CFrameworkunifiedState::FrameworkunifiedSetHSM.\n
  ///       Associates a state machine with the current state by setting a pointer (f_pStatemachine)
  ///       to the state machine (CFrameworkunifiedHSM) specified by the argument to the pointer to the state machine of the member variable. \n
  ///       Calling FrameworkunifiedSetHSM() for child states also associates the state machine of the child states with the state of the current state.
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Not applicable
  /// \~english @see CFrameworkunifiedHSM, CFrameworkunifiedState::FrameworkunifiedSetHSM
  ///
  /// Sets the statemachine in the status
  ///
  /// \param [in] f_pStatemachine
  ///     CFrameworkunifiedHSM* - Statemachine pointer
  ///
  /// \return EFrameworkunifiedStatus
  ///     EFrameworkunifiedStatus - Returns status of operation
  ///////////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus FrameworkunifiedSetHSM(CFrameworkunifiedHSM *f_pStatemachine);

 private:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CheckHistory
  /// This function searches for history state in this composite state and updates it.
  ///
  /// \param [in] l_cHistoryType
  ///     string - Type of history state (shallow or deep)
  ///
  /// \return EFrameworkunifiedStatus
  ///     EFrameworkunifiedStatus - Returns status of operation
  ///////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus CheckHistory(std::string l_cHistoryType);
};

#endif // _FRAMEWORKUNIFIEDCOMPOSITESTATE_H  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
