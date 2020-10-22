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
/// This file has the CFrameworkunifiedOrthogonalState class definitions. CFrameworkunifiedOrthogonalState is derived from
/// CFrameworkunifiedCompositeState class.This class implements the additional functionality supported by HSM
/// Orthogonal state. It provides the standard interfaces for adding orthogonal state machines.
///
///////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file frameworkunified_sm_orthogonalstate.h
 * @brief \~english This file has the CFrameworkunifiedOrthogonalState class definitions. CFrameworkunifiedOrthogonalState is derived from
 *        CFrameworkunifiedCompositeState class.This class implements the additional functionality supported by HSM
 *        Orthogonal state. It provides the standard interfaces for adding orthogonal state machines.
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
#ifndef _FRAMEWORKUNIFIEDORTHOGONALSTATE_H  // NOLINT  (build/header_guard)
#define _FRAMEWORKUNIFIEDORTHOGONALSTATE_H

///////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <native_service/frameworkunified_sm_state.h>
#include <vector>

typedef std::vector<CFrameworkunifiedState *> OrthogonalRegionList;

class CFrameworkunifiedCompositeState;

class CFrameworkunifiedHSM;
///////////////////////////////////////////////////////////////////////////////////////////////////
/// This class implements the additional functionality supported by HSM
/// Orthogonal state. It provides the standard interfaces for adding orthogonal state machines.
///////////////////////////////////////////////////////////////////////////////////////////////////
class CFrameworkunifiedOrthogonalState : public CFrameworkunifiedState {
 public :
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup tag_CFrameworkunifiedOrthogonalState
  /// \~english @par Brief
  ///        Constructors for CFrameworkunifiedOrthogonalState classes
  /// \~english @param [in] f_pName
  ///        std::string   - state name
  /// \~english @retval None
  /// \~english @par Prerequisite
  ///       - None
  /// \~english @par Change of internal state
  ///       - The internal state is not changed.
  /// \~english @par Conditions of processing failure
  ///       - When the quadrature region list cannot be created due to insufficient memory
  /// \~english @par Detail
  ///       Generates an orthographic region list.\n
  ///       CFrameworkunifiedOrthogonalState classes are used to manage the status of independent features individually.\n
  ///       For example, when the function of the clock is set to the following two, the management of the internal state is performed according to the function.\n
  ///       - Clock function (Clock display by 12h/24h)\n
  ///       - Alarm function (on/off setting)
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Not applicable
  /// \~english @see ~CFrameworkunifiedOrthogonalState
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// CFrameworkunifiedCompositeState
  /// Parameterized constructor
  /// \param [in] f_pName
  ///     string - Name of the state
  ///
  /// \return none
  CFrameworkunifiedOrthogonalState(std::string f_pName);  // NOLINT  (readability/nolint)

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup tag_CFrameworkunifiedOrthogonalState
  /// \~english @par Brief
  ///        Destructors of CFrameworkunifiedOrthogonalState classes
  /// \~english @retval None
  /// \~english @par Prerequisite
  ///       - Its own instance (CFrameworkunifiedOrthogonalState) has been created.
  /// \~english @par Change of internal state
  ///       - The internal state is not changed.
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Detail
  ///       Frees the memory in the orthographic area list and clears the memory contents.
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Not applicable
  /// \~english @see CFrameworkunifiedOrthogonalState
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// ~CFrameworkunifiedOrthogonalState
  /// Class destructor
  ///
  /// \return none
  virtual ~CFrameworkunifiedOrthogonalState();



  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup tag_CFrameworkunifiedOrthogonalState
  /// \~english @par Brief
  ///        Initialize state
  /// \~english @param [in] f_pEventData
  ///        CEventDataPtr   - Pointer to the event data class
  /// \~english @retval Successful pointer to the current state after processing
  /// \~english @retval NULL       Exception supine in internal processing
  /// \~english @par Prerequisite
  ///       - Its own instance (CFrameworkunifiedOrthogonalState) has been created.
  ///       - The State Machine for the current state of the application is set in the FrameworkunifiedSetHSM.
  /// \~english @par Change of internal state
  ///       - The internal state is not changed.
  /// \~english @par Conditions of processing failure
  ///       - Failed recursive function call (FrameworkunifiedOnEntry call)[NULL]
  ///       - If the pointers to the orthographic regions are NULL [NULL]
  /// \~english @par Detail
  ///       Loops as many times as the number of elements in the quadrature area list, and initializes the active state (calls FrameworkunifiedOnEntry).\n
  ///       If the current state is Leaf State, it does not initialize because it does not have any active state.\n
  ///       If, after initialization, the current state is a child state of Orthogonal State, sets itself (the this pointer) to the current state.\n
  ///       Breaks looping if the current state is not a child state of Orthogonal State.\n
  ///       To maintain the hierarchy, the active state of the parent state is set to the current state.
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Not applicable
  /// \~english @see FrameworkunifiedOnHSMStop, FrameworkunifiedOnEntry, CFrameworkunifiedState::FrameworkunifiedOnHSMStart, IsOrthogonalChildState
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
  CFrameworkunifiedState *FrameworkunifiedOnHSMStart(CEventDataPtr f_pEventData);

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup tag_CFrameworkunifiedOrthogonalState
  /// \~english @par Brief
  ///        Clean up state.
  /// \~english @param [in] f_pEventData
  ///        CEventDataPtr   - Pointer to the event data class
  /// \~english @retval Successful pointer to the current state after processing
  /// \~english @retval NULL                                 Exception supine in internal processing
  /// \~english @par Prerequisite
  ///       - Its own instance (CFrameworkunifiedOrthogonalState) has been created.
  ///       - The State Machine for the current state of the application is set in the FrameworkunifiedSetHSM.
  /// \~english @par Change of internal state
  ///       - The internal state is not changed.
  /// \~english @par Conditions of processing failure
  ///       - If the pointers to the orthographic regions are NULL [NULL]
  ///       - If the pointers to the orthographic area lists of the member variables are NULL [NULL]
  ///       - Active state pointers from orthographic area lists of member variables are NULL [NULL]
  ///       - If the current state of the state being cleaned up is NULL [NULL]
  /// \~english @par Detail
  ///       Loops as many times as the number of elements in the quadrature area list, and performs cleanup of the active state (calls FrameworkunifiedOnExit).\n
  ///       If the current state is Leaf State,Do not clean up because it does not have any active state.\n
  ///       If the current state is a child state of Orthogonal State (orthographic state) after the cleanup process finishes,Sets itself (the this pointer) to the current state.\n
  ///       Breaks looping if the current state is not a child state of Orthogonal State.
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Not applicable
  /// \~english @see FrameworkunifiedOnHSMStart, CFrameworkunifiedState::FrameworkunifiedOnHSMStop, CFrameworkunifiedState::FrameworkunifiedPostEvent, FrameworkunifiedOnExit,
  ///   IsOrthogonalChildState
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
  CFrameworkunifiedState *FrameworkunifiedOnHSMStop(CEventDataPtr f_pEventData);

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedAddOrthogonalRegion
  /// This Interface adds orthogonal region in the orthogonal state.
  ///
  /// \param [in] f_pOrthogonalRegion
  ///     CFrameworkunifiedCompositeState - Orthogonal region object to be associated with state.
  ///
  /// \return CurrentState
  ///     CFrameworkunifiedState* - Returns current state after operation
  ///////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus FrameworkunifiedAddOrthogonalRegion(CFrameworkunifiedCompositeState *f_pOrthogonalRegion);

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup tag_CFrameworkunifiedOrthogonalState
  /// \~english @par Brief
  ///        Checks whether OrthogonalRegion (orthographic area) is registered or not.
  /// \~english @retval TRUE  OrthogonalRegion (orthographic area) is registered
  /// \~english @retval FALSE OrthogonalRegion (orthographic area) is not registered
  /// \~english @par Prerequisite
  ///       - Its own instance (CFrameworkunifiedOrthogonalState) has been created.
  ///       - OrthogonalRegion (orthographic area) is registered in FrameworkunifiedAddOrthogonalRegion.
  /// \~english @par Change of internal state
  ///       - The internal state is not changed.
  /// \~english @par Conditions of processing failure
  ///       - If the pointers to the orthographic regions are NULL [FALSE]
  ///       - When the OrthogonalRegion (orthographic area) is not registered and the number of elements (size) of the orthographic area is 0, [FALSE]
  /// \~english @par Detail
  ///       Checks whether OrthogonalRegion (orthographic area) is registered by referring to the number of elements (size) of the orthographic area list.
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Not applicable
  /// \~english @see 
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedHasOrthogoanlRegions
  /// This checks if the current state has Orthogonal regions
  ///
  ///
  /// \return TRUE/FASLE
  ///     BOOL - returns TRUE if orthogonal regions are available in the state
  virtual BOOL FrameworkunifiedHasOrthogoanlRegions();

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup tag_CFrameworkunifiedOrthogonalState
  /// \~english @par Brief
  ///        Event handling
  /// \~english @param [in] f_pEventData
  ///        CEventDataPtr   - Pointer to the event data class
  /// \~english @retval Successful pointer to the current state after processing
  /// \~english @retval NULL       Operation cannot be guaranteed.
  /// \~english @par Prerequisite
  ///       - Its own instance (CFrameworkunifiedOrthogonalState) has been created.
  ///       - The State Machine for the current state of the application is set in the FrameworkunifiedSetHSM.
  ///       - OrthogonalRegion (orthographic area) is registered in FrameworkunifiedAddOrthogonalRegion.
  /// \~english @par Change of internal state
  ///       - The internal state is not changed.
  /// \~english @par Conditions of processing failure
  ///       - If the pointers to the orthographic regions are NULL [NULL]
  ///       - If the event data class is pointed to by NULL [NULL]
  /// \~english @par Detail
  ///       If the event specified in the argument (f_pEventData) is available in the current state,Set itself (this) to current state and return it.\n
  ///       Invokes the parent-state FrameworkunifiedOnEvent if not available in the current state
  ///       (Repeated until the root state is reached.)\n
  ///       However, since the FrameworkunifiedState that is the parent state of the Orthogonal State (orthographic state) is the root state,You call the FrameworkunifiedOnEvent of FrameworkunifiedState.\n
  ///       If the current state is a child state of the Orthogonal State (quadrature state) after the above process, sets itself (the this pointer) to the current state.
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Not applicable
  /// \~english @see CFrameworkunifiedCompositeState::FrameworkunifiedGetActiveState, CFrameworkunifiedState::FrameworkunifiedIsReactionAvailable, CFrameworkunifiedState::FrameworkunifiedOnEvent,
  /// IsOrthogonalChildState
  ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedOnEvent
  /// This function processes the event. If the reaction for event is available in the current
  /// state within eventlist and deferred eventlist then it is consumed in the current state
  /// otherwise forwarded to the parent state. Event forwarding is done recursively till either
  /// event is consumed or the root state has encountered. This also process the events posted
  /// in the reactions recursively till all posted events are cleared.
  ///
  /// \param [in] f_pEventData
  ///     CEventDataPtr - Event data
  ///
  ///
  /// \return state
  ///         CFrameworkunifiedState* - Returns current state after the event is processed
  ///
  virtual CFrameworkunifiedState *FrameworkunifiedOnEvent(CEventDataPtr f_pEventData);

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup tag_CFrameworkunifiedOrthogonalState
  /// \~english @par Brief
  ///        Logs the state name and event name.
  /// \~english @retval eFrameworkunifiedStatusOK success
  /// \~english @retval eFrameworkunifiedStatusNullPointer Specifying NULL pointers
  /// \~english @par Prerequisite
  ///       - Its own instance (CFrameworkunifiedOrthogonalState) has been created.
  ///       - OrthogonalRegion (orthographic area) is registered in FrameworkunifiedAddOrthogonalRegion.
  /// \~english @par Change of internal state
  ///       - The internal state is not changed.
  /// \~english @par Conditions of processing failure
  ///       - If the pointers to the orthographic regions are NULL [eFrameworkunifiedStatusNullPointer]
  /// \~english @par Detail
  ///       Size of the log output data
  ///       - State name of the parent state machine and the state name of the self-state machine
  ///       - All event names on the event list
  ///       - All event names on the deferred event list
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Not applicable
  /// \~english @see CFrameworkunifiedCompositeState::FrameworkunifiedPrintStates
  ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedPrintStates
  /// This logs the state name and events associated with the state
  ///
  /// \return EFrameworkunifiedStatus
  ///     EFrameworkunifiedStatus - Returns status of operation
  ///////////////////////////////////////////////////////////////////////////////////////////
  virtual  EFrameworkunifiedStatus FrameworkunifiedPrintStates();

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup tag_CFrameworkunifiedOrthogonalState
  /// \~english @par Brief
  ///        Get the active state.
  /// \~english @retval Successful pointer to the active state
  /// \~english @par Prerequisite
  ///       - Its own instance (CFrameworkunifiedOrthogonalState) has been created.
  /// \~english @par Change of internal state
  ///       - The internal state is not changed.
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Detail
  ///       In this class, the current state (this state) is always returned as the active state.
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Not applicable
  /// \~english @see None
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
  /// \ingroup tag_CFrameworkunifiedOrthogonalState
  /// \~english @par Brief
  ///        Associate the state machine with the current state.
  /// \~english @param [in] f_pStatemachine
  ///        CFrameworkunifiedHSM* - Pointers to state machines (CFrameworkunifiedHSM classes)
  /// \~english @retval eFrameworkunifiedStatusOK success
  /// \~english @retval eFrameworkunifiedStatusNullPointer Specifiying NULL pointers
  /// \~english @par Prerequisite
  ///       - Its own instance (CFrameworkunifiedOrthogonalState) has been created.
  /// \~english @par Change of internal state
  ///       - The internal state is not changed.
  /// \~english @par Conditions of processing failure
  ///       - When the pointers to the state machines (CFrameworkunifiedHSM classes) specified in the arguments (f_pStatemachine) are NULL
  ///       [eFrameworkunifiedStatusNullPointer]
  /// \~english @par Detail
  ///       Associates a state machine with the current state by setting a pointer (f_pStatemachine) to the state machine (CFrameworkunifiedHSM) specified by the argument to the pointer to the state machine of the member variables.
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Not applicable
  /// \~english @see CFrameworkunifiedCompositeState::FrameworkunifiedSetHSM
  ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedSetHSM
  /// This Interface associates the gives statemachine with the current state
  ///
  /// \param [in] f_pStatemachine
  ///     CFrameworkunifiedHSM* - Statemachine associated with the state.
  ///
  /// \return EFrameworkunifiedStatus
  ///     EFrameworkunifiedStatus - Returns status of operation
  ///////////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus FrameworkunifiedSetHSM(CFrameworkunifiedHSM *f_pStatemachine);

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup tag_CFrameworkunifiedOrthogonalState
  /// \~english @par Brief
  ///        Outputs state information in the form of XML tags.
  /// \~english @param [out] f_strXMLString
  ///        std::ostringstream& - Only kept in NPS
  /// \~english @retval eFrameworkunifiedStatusOK success
  /// \~english @retval eFrameworkunifiedStatusNullPointer Specifiying NULL pointers
  /// \~english @par Prerequisite
  ///       - Its own instance (CFrameworkunifiedOrthogonalState) has been created.
  ///       - The State Machine for the current state of the application is set in the FrameworkunifiedSetHSM.
  /// \~english @par Change of internal state
  ///       - The internal state is not changed.
  /// \~english @par Conditions of processing failure
  ///       - If generating of own instance (CFrameworkunifiedOrthogonalState) failed, [eFrameworkunifiedStatusNullPointer]
  /// \~english @par Detail
  ///       Outputs the following state information in XML tag format to the XML stream specified by the argument.
  ///       - Sate name
  ///       - All event information (event ID and event name) in the event list
  ///       - All event information (event ID and event name) in the deferred event list
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Not applicable
  /// \~english @see CFrameworkunifiedCompositeState::FrameworkunifiedPrintXML
  ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedPrintXML
  /// Update the State information in the given stream in the form of XML tags
  ///
  /// \param [in] f_strXMLString
  ///     std::ostringstream & - reference to the XML stream
  ///
  /// \return EFrameworkunifiedStatus
  ///     EFrameworkunifiedStatus - Returns status of operation
  ///////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus FrameworkunifiedPrintXML(std::ostringstream &f_strXMLString);

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup tag_CFrameworkunifiedOrthogonalState
  /// \~english @par Brief
  ///        Always returns eFrameworkunifiedStatusOK (Successful).
  /// \~english @retval eFrameworkunifiedStatusOK success
  /// \~english @par Prerequisite
  ///       - Its own instance (CFrameworkunifiedOrthogonalState) has been created.
  /// \~english @par Change of internal state
  ///       - The internal state is not changed.
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Detail
  ///       In this class, eFrameworkunifiedStatusOK is always returned because the class does not require the History feature.\n
  ///       The History feature holds the active state prior to preprocessing and
  ///       returns the inside state to the active state prior to processing after the processing is executed.
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Not applicable
  /// \~english @see 
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// UpdateHistory
  /// This function stores the last active state
  ///
  /// \return EFrameworkunifiedStatus
  ///     EFrameworkunifiedStatus - Returns status of operation
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

 private:
  /// Pointer to the map that stores the pointer to Orthogonal region list
  OrthogonalRegionList *m_pOrthogonalReigonList;

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// IsOrthogonalChildState
  /// Checks if the given state is a child of orthogonal region
  ///
  /// \param [in] f_pChildState
  ///     CFrameworkunifiedState* - Child state pointer
  ///
  /// \return TRUE/FASLE
  ///     BOOL - returns TRUE if the given state is child of orthogonal state
  ///////////////////////////////////////////////////////////////////////////////////////////
  BOOL IsOrthogonalChildState(CFrameworkunifiedState *f_pChildState);
};

#endif  // _FRAMEWORKUNIFIEDORTHOGONALSTATE_H  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
