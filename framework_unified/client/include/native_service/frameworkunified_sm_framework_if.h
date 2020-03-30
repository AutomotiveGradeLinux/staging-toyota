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
///
///
///////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file frameworkunified_sm_framework_if.h
 * @brief \~english This File has public Macro definition that simplifies the statemachine implementation
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
#ifndef __FRAMEWORKUNIFIED_NATIVESERVICES_NATIVESERVICES_INC_FRAMEWORK_STATEMACHINE_FRAMEWORKUNIFIED_SM_FRAMEWORK_IF_H__  // NOLINT  (build/header_guard)
#define __FRAMEWORKUNIFIED_NATIVESERVICES_NATIVESERVICES_INC_FRAMEWORK_STATEMACHINE_FRAMEWORKUNIFIED_SM_FRAMEWORK_IF_H__

#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_sm_framework_types.h>
#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_framework_types.h>
#include <native_service/frameworkunified_sm_framework_dispatch.h>

///////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup StateMachine_Macro
/// \~english @par Brief
///        This macro creates an instance of a state object.
/// \~english @param [in] class_name Class name
/// \~english @retval None
/// \~english @par Prerequisite
///       - None
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - None
/// \~english @par Detail
///       This macro creates an instance of a state object with the specified class name.
/// \~english @par Classification
///          Public
/// \~english @see  None
///////////////////////////////////////////////////////////////////////////////////////////
/// Create a instance of state object
#define CREATE_STATE(class_name) \
    C## class_name *l_p## class_name = new C## class_name(#class_name);

///////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup StateMachine_Macro
/// \~english @par Brief
///        This macro associates a framework event with a process and adds it to a state.
/// \~english @param [in] state State
/// \~english @param [in] eventid Event ID
/// \~english @param [in] reaction Pointers to CFrameworkunifiedReaction objects
/// \~english @retval None
/// \~english @par Prerequisite
///       - None
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - None
/// \~english @par Detail
///       This macros retrieves the current state machine from the application handles used in the process
///       and associates framework events with the process and adds them to the state using FrameworkunifiedFrameworkConnect().
/// \~english @par Classification
///          Public
/// \~english @see  None
///////////////////////////////////////////////////////////////////////////////////////////
/// Connect framework event to reaction and add to state
#define FRAMEWORKUNIFIED_CONNECT_EVENT(state, eventid, reaction) \
    (FrameworkunifiedGetStateMachine(hApp))->FrameworkunifiedFrameworkConnect(CFrameworkunifiedHSMParentFramework::e## state, \
        CFrameworkunifiedHSMParentFramework::_## eventid, l_pTrn## reaction, #eventid);

#define FRAMEWORKUNIFIED_CONNECT_LOCAL_EVENT(state, eventid, reaction) \
    (FrameworkunifiedGetStateMachine(hApp))->FrameworkunifiedFrameworkConnect(CFrameworkunifiedHSMParentFramework::e## state, \
        CFrameworkunifiedHSMParentFramework::_## eventid, l_pLocalTrn## reaction, #eventid);

/// Connect framework event to reaction and add to state as deferred event
#define FRAMEWORKUNIFIED_CONNECT_DEFERREDEVENT(state, eventid) \
    (FrameworkunifiedGetStateMachine(hApp))->FrameworkunifiedFrameworkConnect(CFrameworkunifiedHSMParentFramework::e## state, \
        CFrameworkunifiedHSMParentFramework::_## eventid, NULL, #eventid, TRUE);

///////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup StateMachine_Macro
/// \~english @par Brief
///        This macro associates a child state as the default state with the ready state of the framework.
/// \~english @param [in] child Child state
/// \~english @retval None
/// \~english @par Prerequisite
///       - None
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - None
/// \~english @par Detail
///       This macro retrieves the current state machine from the application handles used in the process and uses FrameworkunifiedFrameworkConnect() to associate the specified child state with the ready state of the framework as the default state.
/// \~english @par Classification
///          Public
/// \~english @see  None
///////////////////////////////////////////////////////////////////////////////////////////
/// connect state to framework ready state as default state
#define FRAMEWORKUNIFIED_CONNECT_DEFAULTSTATE(child) \
    (FrameworkunifiedGetStateMachine(hApp))->FrameworkunifiedFrameworkConnect(l_p## child, TRUE);

///////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup StateMachine_Macro
/// \~english @par Brief
///        This macro associates the child state with the ready state of the framework.
/// \~english @param [in] child Child state
/// \~english @retval None
/// \~english @par Prerequisite
///       - None
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - None
/// \~english @par Detail
///       This macro retrieves the current state machine from the application handles used in the process and associates the specified child state with the ready state of the framework using FrameworkunifiedFrameworkConnect().
/// \~english @par Classification
///          Public
/// \~english @see  None
///////////////////////////////////////////////////////////////////////////////////////////
/// connect state to framework  ready state
#define FRAMEWORKUNIFIED_CONNECT_STATE(child) \
    (FrameworkunifiedGetStateMachine(hApp))->FrameworkunifiedFrameworkConnect(l_p## child);

///////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup StateMachine_Macro
/// \~english @par Brief
///        A macro that associates a child state as the defaut state with a parent state.
/// \~english @param [in] parent Parent state
/// \~english @param [in] child Child state
/// \~english @retval None
/// \~english @par Prerequisite
///       - None
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - None
/// \~english @par Detail
///       This macro takes the current state machine from the application handles used in the process and uses FrameworkunifiedConnect() to associate the specified child state with the specified parent state as the default state.
/// \~english @par Classification
///          Public
/// \~english @see  None
///////////////////////////////////////////////////////////////////////////////////////////
/// connect child state to parent state as default state
#define CONNECT_DEFAULTSTATE(parent, child) \
    (FrameworkunifiedGetStateMachine(hApp))->FrameworkunifiedConnect(l_p## parent, l_p## child, TRUE);

///////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup StateMachine_Macro
/// \~english @par Brief
///        A macro that associates a child state with a parent state.
/// \~english @param [in] parent Parent state
/// \~english @param [in] child Child state
/// \~english @retval None
/// \~english @par Prerequisite
///       - None
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - None
/// \~english @par Detail
///       This macro takes the current state machine from the application handles used in the process and associates the specified child state with the specified parent state using FrameworkunifiedConnect().
/// \~english @par Classification
///          Public
/// \~english @see  None
///////////////////////////////////////////////////////////////////////////////////////////
/// connect child state to parent state
#define CONNECT_STATE(parent, child) \
    (FrameworkunifiedGetStateMachine(hApp))->FrameworkunifiedConnect(l_p## parent, l_p## child);

///////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup StateMachine_Macro
/// \~english @par Brief
///        This macro associates a state as the state of the root.
/// \~english @param [in] orthogonalstate quadrature state(CFrameworkunifiedOrthogonalState*)
/// \~english @param [in] orthogonalregion orthographic region(CFrameworkunifiedCompositeState*)
/// \~english @retval None
/// \~english @par Prerequisite
///       - None
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - None
/// \~english @par Detail
///       This macro gets the current state machine from the application handles used in the process and associates the specified orthographic state as the state of the root using FrameworkunifiedConnectOrthogonal().
/// \~english @par Classification
///          Public
/// \~english @see  None
///////////////////////////////////////////////////////////////////////////////////////////
/// connect child state to parent state
#define CONNECT_ORTHOGONAL_REGION(orthogonalstate, orthogonalregion) \
    (FrameworkunifiedGetStateMachine(hApp))->FrameworkunifiedConnectOrthogonal(l_p## orthogonalstate, l_p## orthogonalregion);

/// connect the deferred  event and reactions and associate them with the state
#define CONNECT_DEFERREDEVENT(state, eventid) \
    FrameworkunifiedGetStateMachine(hApp)->FrameworkunifiedConnect(l_p## state, _## eventid, NULL, #eventid, TRUE);

///////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup StateMachine_Macro
/// \~english @par Brief
///        This macro associates a framework event with a process and adds it to a state.
/// \~english @param [in] state State
/// \~english @param [in] eventid Event ID
/// \~english @param [in] reaction Pointers to CFrameworkunifiedReaction objects
/// \~english @retval None
/// \~english @par Prerequisite
///       - None
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - None
/// \~english @par Detail
///       This macros retrieves the current state machine from the application handles used in the process and associates framework events with the process and adds them to the state using FrameworkunifiedConnect().
/// \~english @par Classification
///          Public
/// \~english @see  None
///////////////////////////////////////////////////////////////////////////////////////////
/// connect event to reaction and add to state
#define CONNECT_EVENT(state, eventid, reaction) \
    (FrameworkunifiedGetStateMachine(hApp))->FrameworkunifiedConnect(l_p## state, _## eventid, l_pTrn## reaction, #eventid);

/// connect event to reaction and add to state
#define CONNECT_LOCAL_EVENT(state, eventid, reaction) \
        (FrameworkunifiedGetStateMachine(hApp))->FrameworkunifiedConnect(l_p## state, _## eventid, l_pLocalTrn## reaction, #eventid);

///////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup StateMachine_Macro
/// \~english @par Brief
///        This macro logs state and events of the state machine.
/// \~english @retval None
/// \~english @par Prerequisite
///       - None
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - None
/// \~english @par Detail
///       This macro gets the current state machine from the application handles used in the process and logs the state and events of the state machine using FrameworkunifiedPrintAllStates().
/// \~english @par Classification
///          Public
/// \~english @see  None
///////////////////////////////////////////////////////////////////////////////////////////
/// print state machine states and events
#define PRINTSTATEMACHINE() \
    (FrameworkunifiedGetStateMachine(hApp))->FrameworkunifiedPrintAllStates();

/// Connect application defined event to application defined reaction and add to framework state
#define CONNECT_FRAMEWORKUNIFIED_EVENT(state, eventid, reaction) \
        (FrameworkunifiedGetStateMachine(hApp))->FrameworkunifiedConnect(l_p## state, CFrameworkunifiedHSMParentFramework::_## eventid, \
            l_pTrn## reaction, #eventid);

/// Connect application defined deferred event to framework state
#define CONNECT_FRAMEWORKUNIFIED_DEFERREDEVENT(state, eventid) \
        (FrameworkunifiedGetStateMachine(hApp))->FrameworkunifiedConnect(l_p## state, CFrameworkunifiedHSMParentFramework::_## eventid, \
            NULL, #eventid, TRUE);

///////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup StateMachine_Macro
/// \~english @par Brief
///        This macro associates an application event with a process and adds it to a state.
/// \~english @param [in] state State
/// \~english @param [in] eventid Event ID
/// \~english @param [in] reaction Pointers to CFrameworkunifiedReaction objects
/// \~english @retval None
/// \~english @par Prerequisite
///       - None
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - None
/// \~english @par Detail
///       This macros retrieves the current state machine from the application handles used in the process, and associates the events of the application with the process and adds them to the state using FrameworkunifiedFrameworkConnect().
/// \~english @par Classification
///          Public
/// \~english @see  None
///////////////////////////////////////////////////////////////////////////////////////////
#define FRAMEWORKUNIFIED_CONNECT_APP_EVENT(state, eventid, reaction) \
    (FrameworkunifiedGetStateMachine(hApp))->FrameworkunifiedFrameworkConnect(CFrameworkunifiedHSMParentFramework::e## state, _## eventid, \
        l_pTrn## reaction, #eventid);

#define FRAMEWORKUNIFIED_CONNECT_APP_DEFERREDEVENT(state, eventid) \
    (FrameworkunifiedGetStateMachine(hApp))->FrameworkunifiedFrameworkConnect(CFrameworkunifiedHSMParentFramework::e## state, _## eventid, \
        NULL, #eventid, TRUE);

///////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup StateMachine_Macro
/// \~english @par Brief
///        Macros that create and add Shallow history states to their parents.
/// \~english @param [in] parent Parent state
/// \~english @retval None
/// \~english @par Prerequisite
///       - None
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - None
/// \~english @par Detail
///       This macro retrieves the current state machine from the application handles used in the process after a new Shallow history state is created, and adds the Shallow history state created to the parent state using FrameworkunifiedConnect().
/// \~english @par Classification
///          Public
/// \~english @see  None
///////////////////////////////////////////////////////////////////////////////////////////
// create a new shallow history state and add it in parent state
#define ADD_SHALLOWHISTORYSTATE(parent) \
    CFrameworkunifiedShallowHistoryState *l_p##parent##SHALLOWHISTORYSTATE = new CFrameworkunifiedShallowHistoryState(SHALLOWHISTORYSTATE);\
    (FrameworkunifiedGetStateMachine(hApp))->FrameworkunifiedConnect(l_p## parent, l_p##parent##SHALLOWHISTORYSTATE);\
    CHKNULL(l_p##parent##SHALLOWHISTORYSTATE);\
    l_p##parent##SHALLOWHISTORYSTATE->SetDefaultHistory();

// create a new shallow history state and add it in parent state
#define ADD_DEEPHISTORYSTATE(parent) \
    CFrameworkunifiedDeepHistoryState *l_p##parent##DEEPHISTORYSTATE = new CFrameworkunifiedDeepHistoryState(DEEPHISTORYSTATE);\
    (FrameworkunifiedGetStateMachine(hApp))->FrameworkunifiedConnect(l_p## parent, l_p##parent##DEEPHISTORYSTATE);\
    CHKNULL(l_p##parent##DEEPHISTORYSTATE);\
    l_p##parent##DEEPHISTORYSTATE->SetDefaultHistory();

/// connect deep history event to reaction and add to state
#define CONNECT_DEEPHISTORYEVENT(state, eventid, reaction) \
    (FrameworkunifiedGetStateMachine(hApp))->FrameworkunifiedConnect(l_p## state, _## eventid, l_pTrn##reaction##DEEPHISTORYSTATE, #eventid);

///////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup StateMachine_Macro
/// \~english @par Brief
///        Macros that associate Shallow history events with processes and add them to states.
/// \~english @param [in] state State
/// \~english @param [in] eventid Event ID
/// \~english @param [in] reaction Pointers to CFrameworkunifiedReaction objects
/// \~english @retval None
/// \~english @par Prerequisite
///       - None
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - None
/// \~english @par Detail
///       This macros retrieves the current state machine from the application handles used in the process and associates Shallow history events with the process and adds them to the state using FrameworkunifiedConnect().
/// \~english @par Classification
///          Public
/// \~english @see  None
///////////////////////////////////////////////////////////////////////////////////////////
/// connect shallow history event to reaction and add to state
#define CONNECT_SHALLOWHISTORYEVENT(state, eventid, reaction) \
    (FrameworkunifiedGetStateMachine(hApp))->FrameworkunifiedConnect(l_p## state, _## eventid, l_pTrn##reaction##SHALLOWHISTORYSTATE, #eventid);


///////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup StateMachine_Macro
///////////////////////////////////////////////////////////////////////////////////////////

/// FrameworkunifiedCreateStateMachine
/// The application state machine is created in this interface. User defined events can also
/// be connected to the framework states in this interface.
///
/// \~english @par Brief
///        - It's a callback function to create state machine used by application.
/// \~english @param [in] hApp
///        HANDLE - handle to application
/// \~english @retval eFrameworkunifiedStatusOK -successed
/// \~english @retval Except eFrameworkunifiedStatusOK - failure
/// \~english @par Prerequisite
///       - None
/// \~english @par Inside state change
///       - None
/// \~english @par Conditions of processing failure
///       - None
/// \~english @par Detail
///       The application state machine is created in this interface. User defined events can also\n
///       be connected to the framework states in this interface.
///       - FrameworkunifiedDispatcherWithArguments
///       - FrameworkunifiedDispatcher
///       - FrameworkunifiedCreateDispatcherWithoutLoop
/// \~english @par Classification
///       - Public
/// \~english @see FrameworkunifiedDispatcherWithArguments, FrameworkunifiedDispatcher, FrameworkunifiedCreateDispatcherWithoutLoop
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedCreateStateMachine(HANDLE hApp);

///////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup StateMachine_Macro
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
/// All HSM applications must define this function. This function will be called on entry
/// of sFrameworkunifiedLoadData state.
//  Expected in this function:
/// All the persistent data of the applications should be loaded in this function.
///
/// \~english @par Brief
///        - This function defines loading persistent data of HSM application.
/// \~english @param [in] hApp
///        HANDLE - handle to application
/// \~english @retval eFrameworkunifiedStatusOK -successed
/// \~english @retval Except eFrameworkunifiedStatusOK - failure
/// \~english @par Prerequisite
///       - None
/// \~english @par Inside state change
///       - None
/// \~english @par Conditions of processing failure
///       - The implementation specification of the callback depends on the implementation (design) of the application.
///         For this reason, the application should consider the Public of errors in the failed condition.
/// \~english @par Detail
///       - All HSM applications must define this function. This function will be called on entry\n
///         of sFrameworkunifiedLoadData state.Expected in this function:\n
///         All the persistent data of the applications should be loaded in this function.
/// \~english @par Classification
///       - Private
/// \~english @par Detail
///       - None
/// \~english @see  None
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedHSMOnLoadData(HANDLE hApp);

///////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup StateMachine_Macro
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
/// All HSM applications must define this function. This function will be called on exit
/// of sFrameworkunifiedRun state.
//  Expected in this function:
/// Send stop request to all the child threads.
///
/// \~english @par Brief
///        - This function defines sending stop request to the child threads of HSM application.
/// \~english @param [in] hApp
///        HANDLE - handle to application
/// \~english @retval eFrameworkunifiedStatusOK -successed
/// \~english @retval Except eFrameworkunifiedStatusOK - failure
/// \~english @par Prerequisite
///       - None
/// \~english @par Inside state change
///       - None
/// \~english @par Conditions of processing failure
///       - The implementation specification of the callback depends on the implementation (design) of the application.
///         For this reason, the application should consider the Public of errors in the failed condition.
/// \~english @par Detail
///       - All HSM applications must define this function.\n
///         This function will be called on exit of sFrameworkunifiedRun state.\n
///         Expected in this function:Send stop request to all the child threads.
/// \~english @par Classification
///       - Private
/// \~english @par Type
///       - None
/// \~english @see  None
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedHSMOnStopInsFrameworkunifiedRun(HANDLE hApp);

#endif /* __FRAMEWORKUNIFIED_NATIVESERVICES_NATIVESERVICES_INC_FRAMEWORK_STATEMACHINE_FRAMEWORKUNIFIED_SM_FRAMEWORK_IF_H__ */  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
