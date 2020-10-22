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
/// This file has the CFrameworkunifiedState class definitions. CFrameworkunifiedState is base class for all types of
/// state classes.This class implements the basic functionality required for HSM state.
/// It provides the standard interfaces for entering, exiting and reacting in a state.
///
///////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file frameworkunified_sm_state.h
 * @brief \~english This file has the CFrameworkunifiedState class definitions. CFrameworkunifiedState is base class for all types of
 *        state classes.This class implements the basic functionality required for HSM state.
 *        It provides the standard interfaces for entering, exiting and reacting in a state.
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
#ifndef __FRAMEWORKUNIFIED_NATIVESERVICES_NATIVESERVICES_INC_FRAMEWORK_STATEMACHINE_FRAMEWORKUNIFIED_SM_STATE_H__  // NOLINT  (build/header_guard)
#define __FRAMEWORKUNIFIED_NATIVESERVICES_NATIVESERVICES_INC_FRAMEWORK_STATEMACHINE_FRAMEWORKUNIFIED_SM_STATE_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <native_service/frameworkunified_sm_reaction.h>
#include <string>
#include <map>
#include <exception>
#include <vector>

class CFrameworkunifiedReaction;
class CFrameworkunifiedHSM;

/// defines the map of event reactions
typedef std::map<UI_32, CFrameworkunifiedReaction *> EventReactionList;

typedef std::vector<UI_32>  DeferredEventList;

/// defines the map of event names
typedef std::map<UI_32, std::string>   EventNameList;

/// defines the iterator for event reaction map
typedef std::map<UI_32, CFrameworkunifiedReaction *>::iterator EventReactionIterator;;

typedef std::vector<CEventDataPtr>  EventInfoList;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// This class implements the basic functionality required for HSM state. It provides the standard
/// interfaces for entering, exiting and reacting in a state.
///////////////////////////////////////////////////////////////////////////////////////////////////
class CFrameworkunifiedState {
 public :
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedState
  /// \~english @par Brief
  ///        CFrameworkunifiedState constructor
  /// \~english @param [in] f_pName
  ///        std::string   - Name of the state
  /// \~english @retval None
  /// \~english @par Prerequisite
  ///       - None
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Detail
  ///       This clase is a base class of state machine. \n
  ///       It creates object of CFrameworkunifiedState class, and initializes the member variable with state name specified by
  ///       argument(f_pName). \n
  ///       It creates the following event lists, and saves them as member variables. About the other member variables,
  ///       it initialzes them with NULL. \n
  ///       If error occurs, output the error log. \n
  ///       - list of events associated with the state(used for managing state of eace event)
  ///       - list of deferred events associated with the state(used for managing state of deferred event)
  ///       - list of deferred events posted in the state(used for posting list of deferred events)
  ///       - list of event names(used for managing event names)
  /// \~english @par Classification
  ///          public
  /// \~english @par Type
  ///       sync only
  /// \~english @see ~CFrameworkunifiedState
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// CFrameworkunifiedState
  /// Parameterized constructor
  /// \param [in] f_pName
  ///     string - Name of the state
  ///
  /// \return none
  CFrameworkunifiedState(std::string f_pName);  // NOLINT  (readability/nolint)

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedState
  /// \~english @par Brief
  ///        CFrameworkunifiedState destructor
  /// \~english @param None
  /// \~english @retval None
  /// \~english @par Prerequisite
  ///       - Calls constructor to successfully create object.
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Detail
  ///       This class destroys object of CFrameworkunifiedState class.\n
  ///       It destroys the following event list. If error occurs, output the error log.
  ///       - list of events associated with the state
  ///       - list of deferred events associated with the state
  ///       - list of deferred events posted in the state
  ///       - list of event names
  /// \~english @par Classification
  ///          public
  /// \~english @par Type
  ///       sync only
  /// \~english @see CFrameworkunifiedState
  ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// ~CFrameworkunifiedState
  /// Class destructor
  ///
  /// \return none
  virtual ~CFrameworkunifiedState();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedState
  /// \~english @par Brief
  ///        add event information
  /// \~english @param [in] f_uiEventId
  ///        UI_32 f_uiEventId  -  event ID
  /// \~english @param [in] f_pReaction
  ///        CFrameworkunifiedReaction *f_pReaction  -  event reaction
  /// \~english @param [in] f_strEventName
  ///        std::string f_strEventName  -  event name
  /// \~english @retval eFrameworkunifiedStatusOK   add event success
  /// \~english @retval eFrameworkunifiedStatusNullPointer  exception occur when add event
  /// \~english @par Prerequisite
  ///        None
  /// \~english @par Change of internal state
  ///        None
  /// \~english @par Conditions of processing failure
  ///        given param is NULL
  /// \~english @par Detail
  ///        add event information to event list, failed if given param is NULL
  /// \~english @par Classification
  ///        Public
  /// \~english @par Type
  ///        sync only
  /// \~english @see
  ///
  /////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedAddEvent
  /// Associates the event id with the reaction in the state. When the event is posted to the
  /// state the associated reaction is executed. This also adds the event name to the map
  /// which is used for debugging.
  ///
  /// \param [in] f_uiEventId
  ///     UI_32 - Event id of the event to be added in the state
  ///
  /// \param [in] f_pReaction
  ///     CFrameworkunifiedReaction* - Reaction to be associated with the with event id in the state
  ///
  /// \param [in] f_strEventName
  ///     string - Name of the event (used for debugging purpose only)
  ///
  /// \return EFrameworkunifiedStatus
  ///         EFrameworkunifiedStatus - Returns status of operation
  ///
  ///////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus FrameworkunifiedAddEvent(UI_32 f_uiEventId, CFrameworkunifiedReaction *f_pReaction, std::string f_strEventName = "");

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedState
  /// \~english @par Brief
  ///        This function processes the event.
  /// \~english @param [in] f_pEventData
  ///      CEventDataPtr - Pointer to event data class
  /// \~english @retval CFrameworkunifiedState*  Pointer to current CFrameworkunifiedState object after processing event
  /// \~english @par Prerequisite
  ///        - Calls constructor to successfully create object.
  ///        - State machine of current state in the applicaton is setted in FrameworkunifiedSetHSM.
  /// \~english @par Change of internal state
  ///        - None
  /// \~english @par Conditions of processing failure
  ///        - Failed to call constructor to create object. [this(current CFrameworkunifiedState object]
  ///        - Pointer to event data class is NULL. [NULL]
  /// \~english @par Detail
  ///        This function processes the event. If the reaction for event is available in the current
  ///        state within eventlist and deferred eventlist then it is consumed in the current state
  ///        otherwise forwarded to the parent state. Event forwarding is done recursively till either
  ///        event is consumed or the root state has encountered. This also process the events posted
  ///        in the reactions recursively till all posted events are cleared.
  /// \~english @par Classification
  ///        Public
  /// \~english @par Type
  ///        sync only
  /// \~english @see CEventData, FrameworkunifiedIsReactionAvailable
  ///
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

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedState
  /// \~english @par Brief
  ///        add deferred event information
  /// \~english @param [in] f_uiEventId
  ///        UI_32 f_uiEventId  -  event ID
  /// \~english @param [in] f_strEventName
  ///        std::string f_strEventName  -  event name
  /// \~english @retval eFrameworkunifiedStatusOK   add deferred event success
  /// \~english @retval eFrameworkunifiedStatusNullPointer  exception occur when add deferred event
  /// \~english @par Prerequisite
  ///        None
  /// \~english @par Change of internal state
  ///        None
  /// \~english @par Conditions of processing failure
  ///        given param is NULL
  /// \~english @par Detail
  ///        add deferred event information to event list , failed if given param is NULL
  /// \~english @par Classification
  ///        Public
  /// \~english @par Type
  ///        sync only
  /// \~english @see
  ///
  /////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedAddDeferredEvent
  /// Associates the deferred event id with the reaction in the state. When the event is posted
  /// to the state the event is deferred and stored in the state. In case of implicit recall
  /// of the deferred events, events are processed before exiting the state. The deferred
  /// events can also be recalled explicitly in the state  This also adds the event name
  /// to the map which is used for debugging.
  ///
  /// \param [in] f_uiEventId
  ///     UI_32 - Event id of the event to be added in the state
  ///
  /// \param [in] f_strEventName
  ///     string - Name of the event (used for debugging purpose only)
  ///
  /// \return EFrameworkunifiedStatus
  ///         EFrameworkunifiedStatus - Returns status of operation
  ///
  ///////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus FrameworkunifiedAddDeferredEvent(UI_32 f_uiEventId, std::string f_strEventName = "");

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedState
  /// \~english @par Brief
  ///        This function creates new eventdata object and add it to event queue of the state machine.
  /// \~english @param [in] f_uiEventId
  ///        UI_32 f_uiEventId  -  event ID
  /// \~english @retval eFrameworkunifiedStatusOK   post event success
  /// \~english @retval eFrameworkunifiedStatusNullPointer  exception occur when post event
  /// \~english @par Prerequisite
  ///        - Calls constructor to successfully create object.
  ///        - State machine of current state in the applicaton is setted in FrameworkunifiedSetHSM.
  /// \~english @par Change of internal state
  ///        - None
  /// \~english @par Conditions of processing failure
  ///       - State machine of application is NULL. [eFrameworkunifiedStatusNullPointer]
  ///       - Failed to create object of eventdata. [eFrameworkunifiedStatusNullPointer]
  ///       - Pointer to event list in which to add eventdata in state machine is NULL. [eFrameworkunifiedStatusNullPointer]
  ///       - Pointer to active state in state machine is NULL. [eFrameworkunifiedStatusNullPointer]
  ///       - Name of active state in state machine is NULL. [eFrameworkunifiedStatusNullPointer]
  /// \~english @par Detail
  ///       This function creates new eventdata object and add it to event queue of the state machine.\n
  ///       The events are posted in the reaction which are executed in the state.
  ///       The event queue is processed once the execution of the reaction is completed.
  /// \~english @par Classification
  ///        Public
  /// \~english @par Type
  ///        sync only
  /// \~english @see FrameworkunifiedPostEvent(CEventDataPtr), CEventData, CFrameworkunifiedHSM::FrameworkunifiedQueueEvent, CFrameworkunifiedHSM::ProcessEvent
  ///
  ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedPostEvent
  /// This function creates new eventdata object and add the to event queue of the state.
  /// The events are posted in the reaction which are executed in the state.
  /// The event queue is processed once the execution of the reaction is completed.
  ///
  /// \param [in] f_uiEventId
  ///     UI_32 - Event Id
  ///
  /// \return EFrameworkunifiedStatus
  ///         EFrameworkunifiedStatus - Returns status of operation
  ///
  EFrameworkunifiedStatus FrameworkunifiedPostEvent(UI_32 f_uiEventId);

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedState
  /// \~english @par Brief
  ///        This function adds event data to event queue of the state machine.
  /// \~english @param [in] f_pEventData
  ///      CEventDataPtr - Pointer to event data class
  /// \~english @retval eFrameworkunifiedStatusOK   post event success
  /// \~english @retval eFrameworkunifiedStatusNullPointer  exception occur when post event
  /// \~english @par Prerequisite
  ///        - Calls constructor to successfully create object.
  ///        - State machine of current state in the applicaton is setted in FrameworkunifiedSetHSM.
  /// \~english @par Change of internal state
  ///        - None
  /// \~english @par Conditions of processing failure
  ///      - State machine of application is NULL. [eFrameworkunifiedStatusNullPointer]
  ///       - Pointer to event data class specified by argument(f_pEventData) is NULL. [eFrameworkunifiedStatusNullPointer]
  ///       - Pointer to event list in which to add eventdata in state machine is NULL. [eFrameworkunifiedStatusNullPointer]
  ///       - Pointer to active state in state machine is NULL. [eFrameworkunifiedStatusNullPointer]
  ///       - Name of active state in state machine is NULL. [eFrameworkunifiedStatusNullPointer]
  /// \~english @par Detail
  ///        This function adds event data to event queue of the state machine. The events are posted in the reaction
  ///        which are executed in the state. The event queue is processed once the execution of the
  ///        reaction is completed.
  /// \~english @par Classification
  ///        Public
  /// \~english @par Type
  ///        sync only
  /// \~english @see CEventData, FrameworkunifiedIsReactionAvailable, CFrameworkunifiedHSM::FrameworkunifiedQueueEvent, CFrameworkunifiedHSM::ProcessEvent
  ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedPostEvent
  /// This function adds the event queue of the state. The events are posted in the reaction
  /// which are executed in the state. The event queue is processed once the execution of the
  /// reaction is completed.
  ///
  /// \param [in] f_pEventData
  ///     CEventDataPtr - Event data
  ///
  /// \return EFrameworkunifiedStatus
  ///         EFrameworkunifiedStatus - Returns status of operation
  ///
  EFrameworkunifiedStatus FrameworkunifiedPostEvent(CEventDataPtr f_pEventData);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedState
  /// \~english @par Brief
  ///        delete event from deferred event list
  /// \~english @param [in] f_uiEventId
  ///        UI_32 f_uiEventId  -  event ID
  /// \~english @retval eFrameworkunifiedStatusOK   delete deferred event success
  /// \~english @retval eFrameworkunifiedStatusInvldID   invalid event ID
  /// \~english @retval eFrameworkunifiedStatusNullPointer  exception occur when delete deferred event
  /// \~english @par Prerequisite
  ///        None
  /// \~english @par Change of internal state
  ///        None
  /// \~english @par Conditions of processing failure
  ///        None
  /// \~english @par Detail
  ///        delete event from deferred event list by event ID
  ///        failed if event id is not exist. exception occur if given param is NULL
  /// \~english @par Classification
  ///        Public
  /// \~english @par Type
  ///        sync only
  /// \~english @see
  ///
  /////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedRemoveEventFromDeferredEventList
  /// This function removes the event from the posted deferred queue list of the state.
  ///
  /// \param [in] f_uiEventId
  ///     UI_32 - Event id
  ///
  /// \return EFrameworkunifiedStatus
  ///         EFrameworkunifiedStatus - eFrameworkunifiedStatusOK if event removed
  ///            eFrameworkunifiedStatusInvldID if event not found in list
  ///
  ///////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus FrameworkunifiedRemoveEventFromDeferredEventList(UI_32 f_uiEventId);

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedState
  /// \~english @par Brief
  ///        This function indicates if the state has sub states.
  /// \~english @param None
  /// \~english @retval BOOL  if the state has sub states
  /// \~english @par Prerequisite
  ///        - Calls constructor to successfully create object.
  /// \~english @par Change of internal state
  ///        - None
  /// \~english @par Conditions of processing failure
  ///        - None
  /// \~english @par Detail
  ///        This function indicates if the state has sub states. It returns TRUE only in the CompositeState
  ///        where this function is overridden
  /// \~english @par Classification
  ///        Public
  /// \~english @par Type
  ///        sync only
  /// \~english @see CFrameworkunifiedCompositeState::FrameworkunifiedHasSubStates
  ///
  ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedHasSubStates
  /// This indicates if the state has sub states. It returns TRUE only in the CompositeState
  /// where this function is overridden
  ///
  /// \return TRUE/FASLE
  ///     BOOL - returns TRUE if it has sub states otherwise returns false.
  virtual BOOL FrameworkunifiedHasSubStates();

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedState
  /// \~english @par Brief
  ///        This function indicates if the state has sub states.
  /// \~english @param None
  /// \~english @retval BOOL  if the state has sub states
  /// \~english @par Prerequisite
  ///        - Calls constructor to successfully create object.
  /// \~english @par Change of internal state
  ///        - None
  /// \~english @par Conditions of processing failure
  ///        - None
  /// \~english @par Detail
  ///        This function indicates if the state has sub states. It returns TRUE only in the OrthogonalState
  ///        where this function is overridden
  /// \~english @par Classification
  ///        Public
  /// \~english @par Type
  ///        sync only
  /// \~english @see CFrameworkunifiedOrthogonalState
  ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedHasOrthogoanlRegions
  /// This indicates if the state has sub states. It returns TRUE only in the OrthogonalState
  /// where this function is overridden
  ///
  /// \return TRUE/FASLE
  ///     BOOL - returns TRUE if it has sub states otherwise returns false.
  ///////////////////////////////////////////////////////////////////////////////////////////
  virtual BOOL FrameworkunifiedHasOrthogoanlRegions();

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedState
  /// \~english @par Brief
  ///        This logs the state name and event name associated with the state
  /// \~english @param None
  /// \~english @retval eFrameworkunifiedStatusOK  success
  /// \~english @retval eFrameworkunifiedStatusNullPointer  null pointer exception
  /// \~english @par Prerequisite
  ///        - Calls constructor to successfully create object.
  /// \~english @par Change of internal state
  ///        - None
  /// \~english @par Conditions of processing failure
  ///        - Failed to call constructor to create object. [eFrameworkunifiedStatusNullPointer]
  /// \~english @par Detail
  ///       This logs the state name and event name associated with the state. \n
  ///       It outputs the following logs:
  ///       - state name of parent state machine and self state machine
  ///       - all event names in event list
  ///       - all event names in deferred event list
  ///       CFrameworkunifiedCompositeState and CFrameworkunifiedOrthogonalState class overrides this function
  ///       and use it.
  /// \~english @par Classification
  ///        Public
  /// \~english @par Type
  ///        sync only
  /// \~english @see
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
  /// \ingroup CFrameworkunifiedState
  /// \~english @par Brief
  ///        Initialize state.
  /// \~english @param [in] f_pEventData
  ///      CEventDataPtr - Pointer to event data class
  /// \~english @retval CFrameworkunifiedState*  Pointer to current CFrameworkunifiedState object after processing event
  /// \~english @retval NULL        error occurs in internal process
  /// \~english @par Prerequisite
  ///        - Calls constructor to successfully create object.
  ///        - State machine of current state in the applicaton is setted in FrameworkunifiedSetHSM.
  /// \~english @par Change of internal state
  ///        - None
  /// \~english @par Conditions of processing failure
  ///        - Failed to call function(FrameworkunifiedOnEntry) recursively. [NULL]
  /// \~english @par Detail
  ///        This function is called recursively till the leaf state is reached. This internally
  ///        calls the Entry function of the current state.
  /// \~english @par Classification
  ///        Public
  /// \~english @par Type
  ///        sync only
  /// \~english @see CEventData, FrameworkunifiedOnEntry, FrameworkunifiedOnHSMStop
  ///////////////////////////////////////////////////////////////////////////////////////////
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
  /// \ingroup CFrameworkunifiedState
  /// \~english @par Brief
  ///        Clearup state.
  /// \~english @param [in] f_pEventData
  ///      CEventDataPtr - Pointer to event data class
  /// \~english @retval CFrameworkunifiedState*  Pointer to current CFrameworkunifiedState object after processing event
  /// \~english @retval NULL        error occurs in internal process
  /// \~english @par Prerequisite
  ///        - Calls constructor to successfully create object.
  ///        - State machine of current state in the applicaton is setted in FrameworkunifiedSetHSM.
  /// \~english @par Change of internal state
  ///        - None
  /// \~english @par Conditions of processing failure
  ///       - Failed to call constructor to create object. [NULL]
  ///       - Pointer to list of deferred events posted in the state is NULL. [NULL]
  ///       - Pointer to event data getted from list of deferred events posted in the state is NULL. [NULL]
  ///       - Event name of event data getted from list of deferred events posted in the state is NULL. [NULL]
  /// \~english @par Detail
  ///       This function is called recursively till the required parent state is reached. This
  ///       internally calls the Exit function of the current state.
  /// \~english @par Classification
  ///        Public
  /// \~english @par Type
  ///        sync only
  /// \~english @see CEventData, FrameworkunifiedOnExit, FrameworkunifiedOnHSMStart
  ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedOnHSMStop
  /// This function is called recursively till the required parent state is reached. This
  /// internally calls the Exit function of the current state.
  ///
  /// \param [in] f_pEventData
  ///     CECEventDataPtr Event data
  /// \return CurrentState
  ///     CFrameworkunifiedState* - Returns current state after operation
  ///////////////////////////////////////////////////////////////////////////////////////////
  virtual CFrameworkunifiedState *FrameworkunifiedOnHSMStop(CEventDataPtr f_pEventData);

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedState
  /// \~english @par Brief
  ///        Check the availability of event.
  /// \~english @param [in] f_uiEventId
  ///       UI_32  -  event ID
  /// \~english @retval TRUE  reaction is available for this event
  /// \~english @retval FALSE reaction is not available for this event
  /// \~english @par Prerequisite
  ///       - Calls constructor to successfully create object.
  /// \~english @par Change of internal state
  ///        - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Detail
  ///       If the event specified by argument(f_uiEventId) is in event list,
  ///       the reaction is available for this event, return TRUE.
  ///       If the event specified by argument(f_uiEventId) is in deferred event list
  ///       but not in event list, the reaction is available for this event, return TRUE.
  ///       Otherwise, the event specified by argument(f_uiEventId) isn't available, return FALSE.
  /// \~english @par Classification
  ///        Public
  /// \~english @par Type
  ///        sync only
  /// \~english @see FrameworkunifiedPostEvent
  ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedIsReactionAvailable
  ///
  ///
  /// \param [in] f_uiEventId
  ///     UI_32 - Event identifier
  ///
  /// \return TRUE/FASLE
  ///     BOOL - returns TRUE if it reaction is available for this event
  virtual BOOL FrameworkunifiedIsReactionAvailable(UI_32 f_uiEventId);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedState
  /// \~english @par Summary
  ///       pure virtual fuction
  /// \~english @param None
  /// \~english @retval CFrameworkunifiedState* depend on implement class
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///       - None
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       sync only
  /// \~english @par Detail
  ///       pure virtual function , implement by child class
  /// \~english @see
  ////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedGetActiveState
  /// This interface returns the Active state of the current composite state. In case of
  /// non-composite state current state is active state
  ///
  /// \return Active state
  ///     CFrameworkunifiedState* - Active state
  ///////////////////////////////////////////////////////////////////////////////////////////
  virtual CFrameworkunifiedState *FrameworkunifiedGetActiveState() = 0;

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedState
  /// \~english @par Brief
  ///        This Interface associates the gives statemachine with the current state
  /// \~english @param [in] f_pStatemachine
  ///      CFrameworkunifiedHSM*  - Statemachine associated with the state.
  /// \~english @retval eFrameworkunifiedStatusOK  success
  /// \~english @retval eFrameworkunifiedStatusNullPointer  NULL pointer exception
  /// \~english @par Prerequisite
  ///       - Calls constructor to successfully create object.
  /// \~english @par Change of internal state
  ///        - None
  /// \~english @par Conditions of processing failure
  ///       - Pointer to state machine(CFrameworkunifiedHSM class) specified by argument(f_pStatemachine) is NULL.
  ///       [eFrameworkunifiedStatusNullPointer]
  /// \~english @par Detail
  ///       This Interface sets state machine of member variable to state machine specified by argument,
  ///       so it can associate the giving statemachine with current state.
  /// \~english @par Classification
  ///        Public
  /// \~english @par Type
  ///        sync only
  /// \~english @see CFrameworkunifiedHSM
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
  virtual EFrameworkunifiedStatus FrameworkunifiedSetHSM(CFrameworkunifiedHSM *f_pStatemachine);

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedState
  /// \~english @par Brief
  ///        Get application handle.
  /// \~english @param None
  /// \~english @retval HANDLE  application handle
  /// \~english @par Prerequisite
  ///        - Calls constructor to successfully create object.
  ///        - State machine of current state in the applicaton is setted in FrameworkunifiedSetHSM.
  /// \~english @par Change of internal state
  ///        - None
  /// \~english @par Conditions of processing failure
  ///       - Failed to get application handle. [NULL]
  /// \~english @par Detail
  ///       This interface gets and returns the application handle with following steps:
  ///       - If member variable used for state machine of application is not setted, return NULL.
  ///       - Get application handle from state machine of application.
  /// \~english @par Classification
  ///        Public
  /// \~english @par Type
  ///        sync only
  /// \~english @see CFrameworkunifiedHSM::FrameworkunifiedGetAppHandle
  ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedGetAppHandle
  /// This interface returns the application handle
  ///
  /// \return HANDLE
  ///         HANDLE - Application Handle
  ///
  virtual HANDLE FrameworkunifiedGetAppHandle();

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedState
  /// \~english @par Brief
  ///        Update the State information in the given stream in the form of XML tags
  /// \~english @param [out] f_strXMLString
  ///      std::ostringstream &  - reference to the XML stream
  /// \~english @retval eFrameworkunifiedStatusOK  success
  /// \~english @retval eFrameworkunifiedStatusNullPointer  NULL pointer exception
  /// \~english @par Prerequisite
  ///        - Calls constructor to successfully create object.
  ///        - State machine of current state in the applicaton is setted in FrameworkunifiedSetHSM.
  /// \~english @par Change of internal state
  ///        - None
  /// \~english @par Conditions of processing failure
  ///       - Failed to call constructor to create object. [eFrameworkunifiedStatusNullPointer]
  /// \~english @par Detail
  ///       Update the following State information in the given stream in the form of XML tags
  ///       - state name
  ///       - all event information in event list(eventID, event name)
  ///       - all event information in deferred event list(eventID, event name)
  /// \~english @par Classification
  ///        Public
  /// \~english @par Type
  ///        sync only
  /// \~english @see CEventData, FrameworkunifiedIsReactionAvailable, CFrameworkunifiedHSM::FrameworkunifiedQueueEvent, CFrameworkunifiedHSM::ProcessEvent
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
  virtual EFrameworkunifiedStatus FrameworkunifiedPrintXML(std::ostringstream &f_strXMLString);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedState
  /// \~english @par Summary
  ///       pure virtual fuction
  /// \~english @param None
  /// \~english @retval None
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///       - None
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       sync only
  /// \~english @par Detail
  ///       pure virtual function , implement by child class
  /// \~english @see
  ////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// UpdateHistory
  /// This function stores the last active state
  ///
  /// \return EFrameworkunifiedStatus
  ///     EFrameworkunifiedStatus - Returns status of operation
  ///////////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus UpdateHistory() = 0;

 protected :
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedOnEntry
  /// This is pure virtual function implemented by the derived classes of the application.
  /// state initialization can be performed in this function.
  ///
  /// \param [in] f_pEventData
  ///     CEventDataPtr - Event data
  ///
  /// \return EFrameworkunifiedStatus
  ///     EFrameworkunifiedStatus - Returns status of operation
  ///////////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus FrameworkunifiedOnEntry(CEventDataPtr f_pEventData) = 0;

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedOnExit
  /// This is pure virtual function implemented by the derived classes of the application.
  /// state cleanup can be performed in this function.
  ///
  /// \param [in] f_pEventData
  ///     CEventDataPtr - Event data
  ///
  /// \return EFrameworkunifiedStatus
  ///     EFrameworkunifiedStatus - Returns status of operation
  ///////////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus FrameworkunifiedOnExit(CEventDataPtr f_pEventData) = 0;

 private:
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// IsEventDeferred
  /// This checks if the given event is marked as deferred in the state.
  ///
  /// \param [in] f_uiEventId
  ///     UI_32 - Event identifier
  ///
  /// \return BOOL
  ///     BOOL - event deferred or not
  ///////////////////////////////////////////////////////////////////////////////////////////
  BOOL IsEventDeferred(UI_32 f_uiEventId);

 public:
  /// state name
  std::string m_strStateName;

  /// Pointer to the parent state
  CFrameworkunifiedState *m_pParentState;

  /// pointer to the default state
  CFrameworkunifiedState *m_pDefaultState;

  /// pointer to the active state
  CFrameworkunifiedState *m_pActiveState;

  /// pointer to the map that stores the event name against event id
  EventNameList *m_pEventName;

 protected :
  /// pointer to the map that stores the reaction against event id
  EventReactionList *m_pEventList;

  /// pointer to the map that stores deferred event ids
  DeferredEventList *m_pDeferredEventList;

  /// pointer to the vector that stores the eventinfo of the posted deferred event
  EventInfoList *m_pDeferredPostEventList;

  /// pointer to the application statemachine
  CFrameworkunifiedHSM *m_pStateMachine;
};

#endif  // __FRAMEWORKUNIFIED_NATIVESERVICES_NATIVESERVICES_INC_FRAMEWORK_STATEMACHINE_FRAMEWORKUNIFIED_SM_STATE_H__  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
