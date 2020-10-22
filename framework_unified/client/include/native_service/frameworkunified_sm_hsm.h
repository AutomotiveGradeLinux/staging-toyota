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
/// This file has the CFrameworkunifiedHSM class definitions. CFrameworkunifiedHSM is base class for HSM Framework.
/// This class implements interfaces for connecting child states to parent states, connecting events
/// to state.
///////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file frameworkunified_sm_hsm.h
 * @brief \~english This file has the CFrameworkunifiedHSM class definitions. CFrameworkunifiedHSM is base class for HSM Framework.
 *        This class implements interfaces for connecting child states to parent states, connecting events to state.
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
#ifndef __FRAMEWORKUNIFIED_NATIVESERVICES_NATIVESERVICES_INC_FRAMEWORK_STATEMACHINE_FRAMEWORKUNIFIED_SM_HSM_H__  // NOLINT  (build/header_guard)
#define __FRAMEWORKUNIFIED_NATIVESERVICES_NATIVESERVICES_INC_FRAMEWORK_STATEMACHINE_FRAMEWORKUNIFIED_SM_HSM_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <native_service/frameworkunified_types.h>
#include <native_service/ns_logger_if.h>
#include <native_service/frameworkunified_sm_eventdata.h>
#include <vector>
#include <exception>
#include <map>
#include <string>

class CFrameworkunifiedEvent;

class CFrameworkunifiedState;

class CFrameworkunifiedCompositeState;

class CFrameworkunifiedReaction;

class CFrameworkunifiedOrthogonalState;

class CEventData;

typedef boost::shared_ptr<CEventData> CEventDataPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// This class implements interfaces for connecting child states to parent states, connecting events
/// to state.
///////////////////////////////////////////////////////////////////////////////////////////////////
class CFrameworkunifiedHSM {
 public :

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHSM
  /// \~english @par Brief
  ///        Destructor for CFrameworkunifiedHSM
  /// \~english @retval none
  /// \~english @par Preconditons
  ///          - Self instance of CFrameworkunifiedHSM created.
  /// \~english @par Change of internal status
  ///                none
  /// \~english @par Conditions of processing failure
  ///                none
  /// \~english @par Detail
  ///          Release class CFrameworkunifiedHistoryState instance.\n
  ///          Release event in postevent list.\n
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///            none
  /// \~english @see none
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// ~CFrameworkunifiedHSM
  /// Class destructor
  ///
  /// \return none
  virtual ~CFrameworkunifiedHSM();

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHSM
  /// \~english @par Brief
  ///          Constructor for class CFrameworkunifiedHSM.
  /// \~english @param
  /// \~english @retval none
  /// \~english @par Prerequisite
  ///            none
  /// \~english @par Change of internal state
  ///            none
  /// \~english @par Conditions of processing failure
  ///            none
  /// \~english @par Detail
  ///       CFrameworkunifiedHSM class is the super class of HSM framework.
  ///       this class produce interface to connected super class state with sub class state and to connect event with
  ///       state
  ///       create the instance of CFrameworkunifiedHSM class\n
  ///       create event list as following and hold the member parameter,others will be initialized as NULL
  ///       - event list which is supported in state
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///           none
  /// \~english @see ~CFrameworkunifiedHSM
  ///////////////////////////////////////////////////////////////////////////////////////////
  CFrameworkunifiedHSM();

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHSM
  /// \~english @par Brief
  ///          Constructor for class CFrameworkunifiedHSM.
  /// \~english @param f_pHApp
  ///          PVOID - handle of application
  /// \~english @retval none
  /// \~english @par Prerequisite
  ///            none
  /// \~english @par Change of internal state
  ///            none
  /// \~english @par Conditions of processing failure
  ///            none
  /// \~english @par Detail
  ///       CFrameworkunifiedHSM class is the super class of HSM framework.
  ///       this class produce interface to connected super class state with sub class state and to connect event with
  ///       state
  ///       create the instance of CFrameworkunifiedHSM class\n
  ///       create event list as following and hold the member parameter,others will be initialized as NULL
  ///       - event list which is supported in state
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///           none
  /// \~english @see ~CFrameworkunifiedHSM
  ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// CFrameworkunifiedHSM(PVOID f_pHApp)
  /// Class constructor
  ///
  /// \return none
  CFrameworkunifiedHSM(PVOID f_pHApp);  // NOLINT  (readability/nolint)

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHSM
  /// \~english @par Brief
  ///          Returns the active state of the statemachine
  /// \~english @param none
  /// \~english @retval CFrameworkunifiedState* - Returns ActiveState
  /// \~english @retval NULL       - failed
  /// \~english @par Prerequisite
  ///            - Self instance of CFrameworkunifiedHSM created and active state exist.
  /// \~english @par Change of internal state
  ///            none
  /// \~english @par Conditions of processing failure
  ///            - active state do not exist. [NULL]
  /// \~english @par Detail
  ///       route state search will be continued until current state become active
  ///       route state search will be continued until Leaf State or orthogonal State is found\n
  ///       if there is no active state.reture NULL\n
  ///       if there is active state,this state will be hold in memeber parameter as return value
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///           none
  /// \~english @see CFrameworkunifiedState::FrameworkunifiedGetActiveState
  ///////////////////////////////////////////////////////////////////////////////////////////
  CFrameworkunifiedState *FrameworkunifiedGetActiveState();

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHSM
  /// \~english @par Brief
  ///          This creates the default event data and sends the event to the active HSM state.
  /// \~english @param f_uiEventId
  ///           UI_32 - Event Id
  /// \~english @retval eFrameworkunifiedStatusOK            add succ
  /// \~english @retval eFrameworkunifiedStatusNullPointer   set null pointer
  /// \~english @par Prerequisite
  ///            - Self instance of CFrameworkunifiedHSM created.
  /// \~english @par Change of internal state
  ///            none
  /// \~english @par Conditions of processing failure
  ///       - Event data instance create faield. [eFrameworkunifiedStatusNullPointer]
  ///       - The pointer of post event list in State machine is NULL. [eFrameworkunifiedStatusNullPointer]
  ///       - The pointer of active state in State machine is NULL. [eFrameworkunifiedStatusNullPointer]
  ///       - The name of active state in state machine is NULL. [eFrameworkunifiedStatusNullPointer]
  /// \~english @par Detail
  ///       create new event data object and send this event to active HSM state,\n
  ///       then add to event queue of the statemachine
  ///       add to event queque in order to process event using statemachine
  ///       event will be deleted from event queue when process complete
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///           none
  /// \~english @see FrameworkunifiedPostEvent(CEventDataPtr), CEventData, FrameworkunifiedQueueEvent, ProcessEvent
  ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedPostEvent
  /// This creates the default event data and sends the event to the active HSM state.
  ///
  /// \param [in] f_uiEventId
  ///     UI_32 - Event Id
  ///
  /// \return EFrameworkunifiedStatus
  ///         EFrameworkunifiedStatus - Returns status of operation
  ///
  EFrameworkunifiedStatus FrameworkunifiedPostEvent(UI_32 f_uiEventId);

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHSM
  ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedPostEvent
  /// This sends the event to the active HSM state
  ///
  /// \~english @par Brief
  ///          Add event data into event queue of state machine.
  /// \~english @param f_pEventData
  ///           CEventDataPtr - Event data
  /// \~english @retval eFrameworkunifiedStatusOK            add succ
  /// \~english @retval eFrameworkunifiedStatusNullPointer   set null pointer
  /// \~english @par Prerequisite
  ///            - Self instance of CFrameworkunifiedHSM created.
  /// \~english @par Change of internal state
  ///            none
  /// \~english @par Conditions of processing failure
  ///       - Input parameter f_pEventData is NULL. [eFrameworkunifiedStatusNullPointer]
  ///       - The pointer of post event list in State machine is NULL. [eFrameworkunifiedStatusNullPointer]
  ///       - The pointer of active state in State machine is NULL. [eFrameworkunifiedStatusNullPointer]
  ///       - The name of active state in state machine is NULL. [eFrameworkunifiedStatusNullPointer]
  /// \~english @par Detail
  ///
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///           none
  /// \~english @see FrameworkunifiedPostEvent(UI_32), CEventData, FrameworkunifiedQueueEvent, ProcessEvent
  ///////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus FrameworkunifiedPostEvent(CEventDataPtr f_pEventData);

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHSM
  /// \~english @par Brief
  ///         This is pure virtual function to be implemented by the derived classes. Statemachine
  ///         states and events created and connected in this interface.
  /// \~english @param f_pEventData
  ///           PVOID - Event data
  /// \~english @retval EFrameworkunifiedStatus      Returns status of operation
  /// \~english @par Prerequisite
  ///            - Self instance of CFrameworkunifiedHSM created.
  /// \~english @par Change of internal state
  ///            none
  /// \~english @par Conditions of processing failure
  //             none
  /// \~english @par Detail
  ///           none
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///           none
  /// \~english @see none
  ///////////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus FrameworkunifiedCreate(PVOID f_pEventData = NULL) = 0;

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHSM
  ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedStart
  /// This starts the state machine
  ///
  /// \~english @par Brief
  ///         Start the state machine.
  /// \~english @param f_pEventData
  ///           CEventDataPtr - Event data
  /// \~english @retval EFrameworkunifiedStatus      eFrameworkunifiedStatusOK   succ
  /// \~english @retval eFrameworkunifiedStatusNullPointer  set null pointer
  /// \~english @par Prerequisite
  ///            - Self instance of CFrameworkunifiedHSM created.
  /// \~english @par Change of internal state
  ///            none
  /// \~english @par Conditions of processing failure
  //            - Pointer of active state in state machine is NULL. [eFrameworkunifiedStatusNullPointer]
  /// \~english @par Detail
  ///
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///           none
  /// \~english @see FrameworkunifiedPostEvent(UI_32), CEventData, FrameworkunifiedQueueEvent, ProcessEvent
  ///////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus FrameworkunifiedStart(CEventDataPtr f_pEventData = boost::make_shared<CEventData>(0));

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHSM
  /// \~english @par Brief
  ///         Stop the state machine.
  /// \~english @param f_pEventData
  ///           CEventDataPtr - Event data
  /// \~english @retval EFrameworkunifiedStatus
  /// \~english @retval eFrameworkunifiedStatusNullPointer  null pointer
  /// \~english @par Prerequisite
  ///            - Self instance of CFrameworkunifiedHSM created.
  /// \~english @par Change of internal state
  ///            none
  /// \~english @par Conditions of processing failure
  //            - Pointer of active state in state machine is NULL. [eFrameworkunifiedStatusNullPointer]
  /// \~english @par Detail
  ///           Stop the state machine.\n
  ///           Release route state of state machine.\n
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///           none
  /// \~english @see FrameworkunifiedPostEvent(UI_32), CEventData, FrameworkunifiedQueueEvent, ProcessEvent
  ///////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus FrameworkunifiedClose(CEventDataPtr f_pEventData = boost::make_shared<CEventData>(0));

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHSM
  ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedPrintAllStates
  /// This prints all states and events associated with every state on console.
  ///
  /// \~english @par Brief
  ///         Output log of all states and events name associated with every state.
  /// \~english @param
  /// \~english @retval EFrameworkunifiedStatus      succ
  /// \~english @retval eFrameworkunifiedStatusNullPointer  null pointer
  /// \~english @par Prerequisite
  ///            - Self instance of CFrameworkunifiedHSM created.
  /// \~english @par Change of internal state
  ///            none
  /// \~english @par Conditions of processing failure
  ///           - Root state in state machine is NULL. [eFrameworkunifiedStatusNullPointer]
  ///           - Self instance of CFrameworkunifiedHSM does not create.  [eFrameworkunifiedStatusNullPointer]
  /// \~english @par Detail
  ///       print out log for connected state name and event name\n
  ///       following log will be print put
  ///       - super statemachine state name and self-statemachine state name
  ///       - all the event name on event list
  ///       - all the event on delayed event list
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///           none
  /// \~english @see 
  ///////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus FrameworkunifiedPrintAllStates();

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHSM
  /// \~english @par Brief
  ///             This connects the reaction to event and add event to child states then add child state
  ///             to parent state.
  /// \~english @param f_pParentState
  ///      CFrameworkunifiedState* - Parent state pointer
  /// \~english @param f_pChildState
  ///      CFrameworkunifiedState* - Child state pointer
  /// \~english @param f_uiEventId
  ///      UI_32 - eventi id
  /// \~english @param f_pReaction
  ///      CFrameworkunifiedReaction* - Reaction object for attaching to event
  /// \~english @param f_eIsDefaultState
  ///      BOOL - Does has default state.(default: FALSE)
  /// \~english @par
  ///      - TRUE : has default state
  ///      - FALSE: not has default state
  /// \~english @param f_eIsDeferredEventType
  ///      BOOL -  Does has the deferred event(default: FALSE)
  /// \~english @par
  ///      - TRUE : has the deferred event
  ///      - FALSE: not has the deferred event
  /// \~english @param f_strEventName
  ///      std::string - name of event(default:"")
  /// \~english @retval eFrameworkunifiedStatusOK           succ
  /// \~english @retval eFrameworkunifiedStatusNullPointer  null pointer
  /// \~english @par Prerequisite
  ///            - Self instance of CFrameworkunifiedHSM created.
  /// \~english @par Change of internal state
  ///            none
  /// \~english @par Conditions of processing failure
  ///       - Input parameter f_pParentState pointer is NULL.[eFrameworkunifiedStatusNullPointer]
  ///       - Input parameter f_pChildState pointer is NULL. [eFrameworkunifiedStatusNullPointer]
  ///       - Input parameter f_pReaction pointer is NULL. [eFrameworkunifiedStatusNullPointer]
  ///       - Input parameter f_pParentState does not instance. [eFrameworkunifiedStatusNullPointer]
  ///       - Input parameter f_pChildState does not instance. [eFrameworkunifiedStatusNullPointer]
  /// \~english @par Detail
  ///       connect process with event and add the event to sub state.
  ///       and do following considering the event delay and event declare(f_eIsDeferredEventType)
  ///        - if TRUE : add the event as delayed event(add to event name list and delayed event list)
  ///        - if FALSE:add normal event and event(add to event name list and event list)
  /// \~english @par
  ///       After adding an event to a child state, associate the child state with the parent state and
  ///       set the HSM state machine for the child state only if the parent state is a compound state.
  ///       The following associations are made depending on whether or not a default state is defined (f_eIsDefaultState).
  ///       - TRUE: Associates a child state as the default state with the parent state.
  ///       - FALSE: Associates a child state with a parent state as a normal state.
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///           none
  /// \~english @see FrameworkunifiedConnect(CFrameworkunifiedState*,UI_32,CFrameworkunifiedReaction*,std::string,BOOL),
  ///           FrameworkunifiedConnect(CFrameworkunifiedState*,CFrameworkunifiedState*,BOOL)
  ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedConnect
  /// This connects the reaction to event and add event to child states then add child state
  /// to parent state.
  ///
  /// \param [in] f_pParentState
  ///     CFrameworkunifiedState* - Parent state
  ///
  /// \param [in] f_pChildState
  ///     CFrameworkunifiedState* - Child state
  ///
  /// \param [in] f_uiEventId
  ///     UI_32 - Event id
  ///
  /// \param [in] f_pReaction
  ///     CFrameworkunifiedReaction* - Reaction object for attaching to event
  ///
  /// \param [in] f_eIsDefaultState
  ///     BOOL - Defines if the state is default state
  ///
  /// \param [in] f_eIsDeferredEventType
  ///     BOOL - defines if the event is deferred event
  ///
  /// \param [in] f_strEventName
  ///     BOOL - Event name
  ///
  /// \return EFrameworkunifiedStatus
  ///         EFrameworkunifiedStatus - Returns status of operation
  ///
  EFrameworkunifiedStatus  FrameworkunifiedConnect(CFrameworkunifiedState *f_pParentState, CFrameworkunifiedState *f_pChildState,
                         UI_32 f_uiEventId, CFrameworkunifiedReaction *f_pReaction,
                         BOOL f_eIsDefaultState = FALSE,
                         BOOL f_eIsDeferredEventType = FALSE,
                         std::string f_strEventName = "");

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHSM
  /// \~english @par Brief
  ///           This add child state to parent state.
  /// \~english @param f_pParentState
  ///      CFrameworkunifiedState* - Parent state pointer
  /// \~english @param f_pChildState
  ///      CFrameworkunifiedState* - Child state pointer
  /// \~english @param f_eIsDefaultState
  ///      BOOL - Does has default state.(default: FALSE)
  /// \~english @par
  ///      - TRUE : has default state
  ///      - FALSE: not has default state
  /// \~english @retval eFrameworkunifiedStatusOK           succ
  /// \~english @retval eFrameworkunifiedStatusNullPointer  null pointer
  /// \~english @par Prerequisite
  ///            - Self instance of CFrameworkunifiedHSM created.
  /// \~english @par Change of internal state
  ///            none
  /// \~english @par Conditions of processing failure
  ///       - Input parameter f_pParentState pointer is NULL.[eFrameworkunifiedStatusNullPointer]
  ///       - Input parameter f_pChildState pointer is NULL. [eFrameworkunifiedStatusNullPointer]
  ///       - Input parameter f_pParentState does not instance. [eFrameworkunifiedStatusNullPointer]
  ///       - Input parameter f_pChildState does not instance. [eFrameworkunifiedStatusNullPointer]
  /// \~english @par Detail
  ///       connect super state with sub state and set HSM state machine to sub state only when super state is compond
  ///       state
  ///       according to the default state define(f_eIsDefaultState),doing the connection below
  ///       - if TRUE: connect sub state as default state with super state
  ///       - if FALSE: connect sub state as normal state with super state
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///           none
  /// \~english @see FrameworkunifiedConnect(CFrameworkunifiedState*,UI_32,CFrameworkunifiedReaction*,std::string,BOOL),
  ///
  ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedConnect
  /// This add child state to parent state.
  ///
  /// \param [in] f_pParentState
  ///     CFrameworkunifiedState* - Parent state
  ///
  /// \param [in] f_pChildState
  ///     CFrameworkunifiedState* - Child state
  ///
  /// \param [in] f_eIsDefaultState
  ///     BOOL - Defines if the state is default state
  ///
  /// \return EFrameworkunifiedStatus
  ///         EFrameworkunifiedStatus - Returns status of operation
  ///
  EFrameworkunifiedStatus  FrameworkunifiedConnect(CFrameworkunifiedState *f_pParentState, CFrameworkunifiedState *f_pChildState,
                         BOOL f_eIsDefaultState = FALSE);

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHSM
  ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedConnect
  /// This connects the reaction to event and add event to states
  ///
  /// \~english @par Brief
  ///             This connects the reaction to event and add event to states.
  /// \~english @param f_pState
  ///      CFrameworkunifiedState* - pointer of state class
  /// \~english @param f_uiEventId
  ///      UI_32 - eventi id
  /// \~english @param f_pReaction
  ///      CFrameworkunifiedReaction* - Reaction object for attaching to event
  /// \~english @param f_eIsDefaultState
  ///      BOOL - Does has default state.(default: FALSE)
  /// \~english @par
  ///      - TRUE : has default state
  ///      - FALSE: not has default state
  /// \~english @param f_eIsDeferredEventType
  ///      BOOL -  Does has the deferred event(default: FALSE)
  /// \~english @par
  ///      - TRUE : has the deferred event
  ///      - FALSE: not has the deferred event
  /// \~english @param f_strEventName
  ///      std::string - name of event(default:"")
  /// \~english @retval eFrameworkunifiedStatusOK           succ
  /// \~english @retval eFrameworkunifiedStatusNullPointer  null pointer
  /// \~english @par Prerequisite
  ///            - Self instance of CFrameworkunifiedHSM created.
  /// \~english @par Change of internal state
  ///            none
  /// \~english @par Conditions of processing failure
  ///       - Input parameter f_pReaction pointer is NULL.[eFrameworkunifiedStatusNullPointer]
  ///       - Input parameter f_pState pointer is NULL. [eFrameworkunifiedStatusNullPointer]
  ///       - Input parameter f_pState pointer does not instance. [eFrameworkunifiedStatusNullPointer]
  /// \~english @par Detail
  ///       connect process with event and add event to sub state
  ///       following will be added according to whether delayed event or event define exsist
  ///        - if TRUE : add event as delayed event(add to event name list and delayed event list)
  ///        - if FALSE: add event as normal event(add to event name list and event list)
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///           none
  /// \~english @see FrameworkunifiedConnect(CFrameworkunifiedState*,CFrameworkunifiedState*,UI_32,CFrameworkunifiedReaction*,BOOL,BOOL,std::string)
  ///////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus  FrameworkunifiedConnect(CFrameworkunifiedState *f_pState, UI_32 f_uiEventId, CFrameworkunifiedReaction *f_pReaction,
                         std::string f_strEventName = "", BOOL f_eIsDeferredEventType = FALSE);

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHSM
  /// \~english @par Brief
  ///             This sets the givens state as root state in the state machine
  /// \~english @param f_pRootState
  ///      CFrameworkunifiedState* - Root state
  /// \~english @retval eFrameworkunifiedStatusOK           succ
  /// \~english @retval eFrameworkunifiedStatusNullPointer  null pointer
  /// \~english @par Prerequisite
  ///            - Self instance of CFrameworkunifiedHSM created.
  /// \~english @par Change of internal state
  ///            none
  /// \~english @par Conditions of processing failure
  ///       - Input parameter f_pRootState pointer is NULL. [eFrameworkunifiedStatusNullPointer]
  ///       - Input parameter f_pRootState does not instance. [eFrameworkunifiedStatusNullPointer]
  /// \~english @par Detail
  ///       set state(f_pRootState, which is designated as route state) to HSM state machine
  ///       set route state pointer(f_pRootState,which is set as parameter value)as pointer of
  ///       active state for member
  ///       parameter and routestate
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///           none
  /// \~english @see FrameworkunifiedConnect(CFrameworkunifiedState*,CFrameworkunifiedState*,UI_32,CFrameworkunifiedReaction*,BOOL,BOOL,std::string)
  ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedConnect
  /// This sets the givens state as root state in the state machine
  ///
  /// \param [in] f_pRootState
  ///     CFrameworkunifiedState* - Root state
  ///
  /// \return EFrameworkunifiedStatus
  ///         EFrameworkunifiedStatus - Returns status of operation
  ///
  EFrameworkunifiedStatus  FrameworkunifiedConnect(CFrameworkunifiedState *f_pRootState);

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHSM
  /// \~english @par Brief
  ///            Set Composite state as HSM state OrthogonalState.
  /// \~english @param f_pOrthogonalState
  ///     CFrameworkunifiedOrthogonalState* - Orthogonal state pointer
  /// \~english @param f_pOrthogonalRegion
  ///      CFrameworkunifiedCompositeState* - Composite state pointer
  /// \~english @retval eFrameworkunifiedStatusOK           succ
  /// \~english @retval eFrameworkunifiedStatusNullPointer  null pointer(process failed)
  /// \~english @par Prerequisite
  ///            - Self instance of CFrameworkunifiedHSM created.
  /// \~english @par Change of internal state
  ///            none
  /// \~english @par Conditions of processing failure
  ///       - Input parameter f_pOrthogonalState pointer is NULL. [eFrameworkunifiedStatusNullPointer]
  ///       - Input parameter f_pOrthogonalRegion pointer is NULL. [eFrameworkunifiedStatusNullPointer]
  ///       - Input parameter f_pOrthogonalState does not instance. [eFrameworkunifiedStatusNullPointer]
  /// \~english @par Detail
  ///          Set Composite state as HSM state OrthogonalState.
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///           none
  /// \~english @see 
  ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedConnect
  /// This sets the givens state as root state in the state machine
  ///
  /// \param [in] f_pOrthogonalState
  ///     CFrameworkunifiedOrthogonalState* - Orthogonal state
  ///
  /// \param [in] f_pOrthogonalRegion
  ///     CFrameworkunifiedCompositeState* - Orthogonal region
  ///
  /// \return EFrameworkunifiedStatus
  ///         EFrameworkunifiedStatus - Returns status of operation
  ///
  EFrameworkunifiedStatus  FrameworkunifiedConnectOrthogonal(CFrameworkunifiedOrthogonalState *f_pOrthogonalState,
                                   CFrameworkunifiedCompositeState *f_pOrthogonalRegion);

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHSM
  /// \~english @par Brief
  ///          This interface returns the application handle
  /// \~english @param none
  /// \~english @retval HANDLE  application handle
  /// \~english @par Prerequisite
  ///            - Self instance of CFrameworkunifiedHSM created.
  /// \~english @par Change of internal state
  ///            none
  /// \~english @par Conditions of processing failure
  ///            none
  /// \~english @par Detail
  ///           none
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///           none
  /// \~english @see none
  ///////////////////////////////////////////////////////////////////////////////////////////
  HANDLE FrameworkunifiedGetAppHandle();

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHSM
  /// \~english @par Brief
  ///          Output statemachine information into XML file.
  /// \~english @param none
  /// \~english @retval EFrameworkunifiedStatus   eFrameworkunifiedStatusOK succ
  /// \~english @par Prerequisite
  ///            - Self instance of CFrameworkunifiedHSM created.
  /// \~english @par Change of internal state
  ///            none
  /// \~english @par Conditions of processing failure
  ///            none
  /// \~english @par Detail
  ///            none
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///           none
  /// \~english @see none
  ///////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus FrameworkunifiedPrintXML();

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHSM
  /// \~english @par Brief
  ///          Push the event in the post event list.
  /// \~english @param f_pEventData
  ///            CEventDataPtr - pointer of event data
  /// \~english @retval EFrameworkunifiedStatus   eFrameworkunifiedStatusOK succ
  ///                                eFrameworkunifiedStatusNullPointer   NULL pointer
  /// \~english @par Prerequisite
  ///            - Self instance of CFrameworkunifiedHSM created.
  /// \~english @par Change of internal state
  ///            none
  /// \~english @par Conditions of processing failure
  ///            - If input parameter f_pEventData is NULL. [eFrameworkunifiedStatusNullPointer]
  ///            - If member variable m_pActiveState is NULL. [eFrameworkunifiedStatusNullPointer]
  ///            - If member variable m_pPostEventList is NULL. [eFrameworkunifiedStatusNullPointer]
  /// \~english @par Detail
  ///           none
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///           none
  /// \~english @see none
  ///////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus FrameworkunifiedQueueEvent(CEventDataPtr f_pEventData);

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHSM
  /// \~english @par Brief
  ///          Remove the all events of eventId f_uiEventId from event queue of statemachine.
  /// \~english @param f_pEventData
  ///            const UI_32 - event id
  /// \~english @retval EFrameworkunifiedStatus   eFrameworkunifiedStatusOK succ
  ///                                eFrameworkunifiedStatusInvldID       Not found event id
  ///                                eFrameworkunifiedStatusNullPointer   NULL pointer
  /// \~english @par Prerequisite
  ///            - Self instance of CFrameworkunifiedHSM created.
  /// \~english @par Change of internal state
  ///            none
  /// \~english @par Conditions of processing failure
  ///            - If not find event id. [eFrameworkunifiedStatusInvldID]
  ///            - If member variable m_pPostEventList is NULL. [eFrameworkunifiedStatusNullPointer]
  /// \~english @par Detail
  ///           none
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///           none
  /// \~english @see none
  ///////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus RemoveEventFromPostedEventQueue(const UI_32 f_uiEventId);

  UI_32 m_uiCurrentEvent;

  /// Application handle
  HANDLE m_pHApp;

 private:
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// ProcessEventQueue
  /// Gets the event from StateMachine events list and post it.
  ///
  /// \return EFrameworkunifiedStatus
  ///     EFrameworkunifiedStatus - Returns status of operation
  ///////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus ProcessEventQueue();

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// ProcessEvent
  /// Processes the event posted to the statemachine
  ///
  /// \param [in] f_pEventData
  ///         CEventDataPtr - Event data
  ///
  /// \return EFrameworkunifiedStatus
  ///     EFrameworkunifiedStatus - Returns status of operation
  ///////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus ProcessEvent(CEventDataPtr f_pEventData);

  /// pointer to the active state object
  CFrameworkunifiedState *m_pActiveState;

  /// pointer to the root state object
  CFrameworkunifiedState *m_pRootState;

  std::vector<CEventDataPtr> *m_pPostEventList;

  BOOL m_bIsTransitioning;
};

#endif  // __FRAMEWORKUNIFIED_NATIVESERVICES_NATIVESERVICES_INC_FRAMEWORK_STATEMACHINE_FRAMEWORKUNIFIED_SM_HSM_H__  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
