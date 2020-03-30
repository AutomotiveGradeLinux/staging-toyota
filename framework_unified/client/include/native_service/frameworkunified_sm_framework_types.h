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
/// This File has public Macro definition that simplifies the statemachine implementation
///
///////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file frameworkunified_sm_framework_types.h
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
#ifndef __FRAMEWORKUNIFIED_NATIVESERVICES_NATIVESERVICES_INC_FRAMEWORK_STATEMACHINE_FRAMEWORKUNIFIED_FRAMEWORKUNIFIED_SM_FRAMEWORK_TYPES_H__  // NOLINT  (build/header_guard)
#define __FRAMEWORKUNIFIED_NATIVESERVICES_NATIVESERVICES_INC_FRAMEWORK_STATEMACHINE_FRAMEWORKUNIFIED_FRAMEWORKUNIFIED_SM_FRAMEWORK_TYPES_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <native_service/frameworkunified_sm_leafstate.h>
#include <native_service/frameworkunified_sm_internaltransition.h>
#include <native_service/frameworkunified_sm_compositestate.h>
#include <native_service/frameworkunified_sm_eventfactory.h>
#include <native_service/frameworkunified_sm_externaltransition.h>
#include <native_service/frameworkunified_sm_localtransition.h>
#include <native_service/frameworkunified_sm_orthogonalstate.h>
#include <native_service/frameworkunified_sm_shallowhistorystate.h>
#include <native_service/frameworkunified_sm_deephistorystate.h>
#include <native_service/frameworkunified_sm_guard.h>
#include <native_service/frameworkunified_sm_conditionconnector.h>
#include <native_service/frameworkunified_sm_action.h>
#include <native_service/frameworkunified_sm_eventdata.h>
#include <exception>
#include <string>

class CFrameworkunifiedState;


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Defines the null pointer exception
///////////////////////////////////////////////////////////////////////////////////////////////////
class CNullPointerException: public std::exception {
  virtual const char *what() const throw() {
    return "Null pointer exception";
  }
};


/// Macro to check pointer for null value and throw null pointer exception
#define CHKNULL(x) if (NULL == x) throw CNullPointerException();

#define SHALLOWHISTORYSTATE   "ShallowHistory"
#define DEEPHISTORYSTATE    "DeepHistory"

/// Defines the state machine class
#define DEFINE_STATEMACHINE(class_name) \
    class C## class_name : public CFrameworkunifiedHSM{ \
     public: \
      C## class_name(); \
      virtual ~C## class_name(); \
      EFrameworkunifiedStatus FrameworkunifiedCreate(PVOID f_pEventData = NULL); \
    };

/// Defines the leafstate class that does not override entry and exit of base state
#define DEFINE_LEAFSTATE(class_name) \
    class C## class_name : public CFrameworkunifiedLeafState { \
     public: \
      C## class_name(std::string f_pName):CFrameworkunifiedLeafState(f_pName) {} };

/// Defines the Composite state that does not override entry and exit of base state
#define DEFINE_COMPOSITESTATE(class_name) \
    class C## class_name : public CFrameworkunifiedCompositeState { \
     public: \
      C## class_name(std::string f_pName):CFrameworkunifiedCompositeState(f_pName) {} };

/// Defines the Orthogonal state that does not override entry and exit of base state
#define DEFINE_ORTHOGONALSTATE(class_name) \
    class C## class_name : public CFrameworkunifiedOrthogonalState { \
     public: \
      C## class_name(std::string f_pName):CFrameworkunifiedOrthogonalState(f_pName) {} };


///////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup StateMachine_Macro
/// \~english @par Brief
///        Macros that inherit CFrameworkunifiedLeafState classes.
/// \~english @param [in] class_name Class name
/// \~english @retval None
/// \~english @par Prerequisite
///       - None
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - None
/// \~english @par Detail
///       Macros that inherit from CFrameworkunifiedLeafState classes and override the following methods:
///       The following overriding methods should be implemented by the user.
///       - FrameworkunifiedOnEntry
///       - FrameworkunifiedOnExit
/// \~english @par Classification
///          Public
/// \~english @see  None
///////////////////////////////////////////////////////////////////////////////////////////
/// Defines the leafstate that overrides entry and exit of base state
#define DEFINE_LEAFSTATE_OVERRIDE(class_name) \
    class C## class_name : public CFrameworkunifiedLeafState{ \
     public: \
      C## class_name(std::string f_pName); \
      virtual ~C## class_name(); \
      EFrameworkunifiedStatus FrameworkunifiedOnEntry(CEventDataPtr f_pEventData); \
      EFrameworkunifiedStatus FrameworkunifiedOnExit(CEventDataPtr f_pEventData); \
    };

///////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup StateMachine_Macro
/// \~english @par Brief
///        Macros that inherit CFrameworkunifiedCompositeState classes.
/// \~english @param [in] class_name Class name
/// \~english @retval None
/// \~english @par Prerequisite
///       - None
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - None
/// \~english @par Detail
///       Macros that inherit from CFrameworkunifiedCompositeState classes and override the following methods:
///       The following overriding methods should be implemented by the user.
///       - FrameworkunifiedOnEntry
///       - FrameworkunifiedOnExit
/// \~english @par Classification
///          Public
/// \~english @see  None
///////////////////////////////////////////////////////////////////////////////////////////
/// Defines the Composite state class that overrides entry and exit of base state
#define DEFINE_COMPOSITESTATE_OVERRIDE(class_name) \
    class C## class_name : public CFrameworkunifiedCompositeState{ \
     public: \
      C## class_name(std::string f_pName); \
      virtual ~C## class_name(); \
      EFrameworkunifiedStatus FrameworkunifiedOnEntry(CEventDataPtr f_pEventData); \
      EFrameworkunifiedStatus FrameworkunifiedOnExit(CEventDataPtr f_pEventData); \
    };

///////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup StateMachine_Macro
/// \~english @par Brief
///        Macros that inherit CFrameworkunifiedOrthogonalState classes.
/// \~english @param [in] class_name Class name
/// \~english @retval None
/// \~english @par Prerequisite
///       - None
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - None
/// \~english @par Detail
///       Macros that inherit from CFrameworkunifiedOrthogonalState classes and override the following methods:
///       The following overriding methods should be implemented by the user.
///       - FrameworkunifiedOnEntry
///       - FrameworkunifiedOnExit
/// \~english @par Classification
///          Public
/// \~english @see  None
///////////////////////////////////////////////////////////////////////////////////////////
/// Defines the Orthogonal state that overrides entry and exit of base state
#define DEFINE_ORTHOGONALSTATE_OVERRIDE(class_name) \
    class C## class_name : public CFrameworkunifiedOrthogonalState{ \
     public: \
      C## class_name(std::string f_pName); \
      virtual ~C## class_name(); \
      EFrameworkunifiedStatus FrameworkunifiedOnEntry(CEventDataPtr f_pEventData); \
      EFrameworkunifiedStatus FrameworkunifiedOnExit(CEventDataPtr f_pEventData); \
    };

///////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup StateMachine_Macro
/// \~english @par Brief
///        Macros that inherit CFrameworkunifiedInternalTransition classes.
/// \~english @param [in] class_name Class name
/// \~english @retval None
/// \~english @par Prerequisite
///       - None
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - None
/// \~english @par Detail
///       Macros that inherit from CFrameworkunifiedInternalTransition classes and override the following methods:
///       The following overriding methods should be implemented by the user.
///       - FrameworkunifiedReaction
/// \~english @par Classification
///          Public
/// \~english @see  None
///////////////////////////////////////////////////////////////////////////////////////////
/// Defines the InternalTransition class
#define DEFINE_INTERNALTRANSITION(class_name) \
    class C## class_name : public CFrameworkunifiedInternalTransition{ \
     public: \
        virtual CFrameworkunifiedState* FrameworkunifiedReaction(CFrameworkunifiedState* f_pSourceState, CEventDataPtr f_pData); \
    };

///////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup StateMachine_Macro
/// \~english @par Brief
///        Macros that define application events.
/// \~english @param [in] event_name Event name
/// \~english @param [in] value Event value
/// \~english @retval UI_32  Event value for the application
/// \~english @par Prerequisite
///       - None
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - None
/// \~english @par Detail
///       Values of events defined in CFrameworkunifiedEventFactory classes (evFrameworkunifiedEventLimit:10) are specifiedMacros that define events for an application by adding event values (value).
/// \~english @par Classification
///          Public
/// \~english @see  None
///////////////////////////////////////////////////////////////////////////////////////////
/// Defines the application event
#define DEFINE_EVENT(event_name, value) \
    static const UI_32 _## event_name = CFrameworkunifiedEventFactory::evFrameworkunifiedEventLimit + value;

#define DEFINE_ACTION(class_name) \
    class C## class_name: public CFrameworkunifiedAction{ \
     public: \
        C## class_name(std::string f_strName):CFrameworkunifiedAction(f_strName) {} \
        VOID FrameworkunifiedAction(CFrameworkunifiedState *f_pSourceState, CFrameworkunifiedState *f_pTargetState, CEventDataPtr f_pEventData); \
    };

///////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup StateMachine_Macro
/// \~english @par Brief
///        This macro creates an instance for an internal transition.
/// \~english @param [in] class_name Class name
/// \~english @retval None
/// \~english @par Prerequisite
///       - None
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - None
/// \~english @par Detail
///       This macro creates an instance for an internal transition of a class with the specified name.\n
///       Otherwise, the CNullPointerException objects are excepted.
/// \~english @par Classification
///          Public
/// \~english @see  None
///////////////////////////////////////////////////////////////////////////////////////////
/// creates the instance of the internal transition
#define CREATE_INTERNALTRANSITION(class_name) \
    C## class_name *l_pTrn## class_name = new C## class_name(); \
    CHKNULL(l_pTrn## class_name);

///////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup StateMachine_Macro
/// \~english @par Brief
///        This macro creates an instance for an external transition.
/// \~english @param [in] target_state State name for external transition
/// \~english @retval None
/// \~english @par Prerequisite
///       - None
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - None
/// \~english @par Detail
///       Macros that instantiate CFrameworkunifiedExternalTransition classes for external transitions.\n
///       Otherwise, the CNullPointerException objects are excepted.
/// \~english @par Classification
///          Public
/// \~english @see  None
///////////////////////////////////////////////////////////////////////////////////////////
/// creates the instance of the external transition
#define CREATE_EXTERNALTRANSITION(target_state) \
    CFrameworkunifiedExternalTransition *l_pTrn## target_state  = new CFrameworkunifiedExternalTransition(l_p## target_state); \
    CHKNULL(l_pTrn## target_state);

/// creates the instance of the external transition
#define CREATE_CONDITIONCONNECTOR(connector_name) \
    CFrameworkunifiedConditionConnector *l_pTrn## connector_name = new CFrameworkunifiedConditionConnector( #connector_name); \
    CHKNULL(l_pTrn## connector_name);


/// creates the instance of the local transition
#define CREATE_LOCALTRANSITION(target_state) \
    CFrameworkunifiedLocalTransition *l_pLocalTrn## target_state  = new CFrameworkunifiedLocalTransition(l_p## target_state); \
    CHKNULL(l_pLocalTrn## target_state);

///////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup StateMachine_Macro
/// \~english @par Brief
///        A macro that instantiates an orthographic region.
/// \~english @param [in] target_state State name for external transition
/// \~english @retval None
/// \~english @par Prerequisite
///       - None
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - None
/// \~english @par Detail
///       Macros that create instances of compound states (CFrameworkunifiedCompositeState classes) as instances of orthographic regions.\n
/// \~english @par Classification
///          Public
/// \~english @see  None
///////////////////////////////////////////////////////////////////////////////////////////
/// create orthogonal region
#define CREATE_ORTHOGONALREGION(name) \
    CFrameworkunifiedCompositeState *l_p## name = new CFrameworkunifiedCompositeState(#name);

/// create action for external transition
#define CREATE_ACTION(class_name) \
    C## class_name *l_pTrn## class_name = new C## class_name( #class_name); \
    CHKNULL(l_pTrn## class_name);


///////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup StateMachine_Macro
/// \~english @par Brief
///        A macro that defines events for the framework.
/// \~english @param [in] eventid Event ID
/// \~english @retval None
/// \~english @par Prerequisite
///       - None
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - None
/// \~english @par Detail
///       These macros define the IDs of the current events in the CFrameworkunifiedHSMParentFramework classes.
/// \~english @par Classification
///          Public
/// \~english @see  None
///////////////////////////////////////////////////////////////////////////////////////////
/// framework event
#define FRAMEWORKUNIFIED_EVENT(eventid) \
    CFrameworkunifiedHSMParentFramework::_## eventid

///////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup StateMachine_Macro
/// \~english @par Brief
///        Is a macro that defines events for an application
/// \~english @param [in] eventid Event ID
/// \~english @retval None
/// \~english @par Prerequisite
///       - None
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - None
/// \~english @par Detail
///       A macro that defines an event ID for an application.
/// \~english @par Classification
///          Public
/// \~english @see  None
///////////////////////////////////////////////////////////////////////////////////////////
/// Application event
#define EVENT(eventid) \
    _## eventid

/// FrameworkunifiedReaction function signature for internal transition class
#define IMPLEMENT_INTERNALTRANSITION(name) \
    CFrameworkunifiedState* C## name::FrameworkunifiedReaction(CFrameworkunifiedState* f_pSourceState, CEventDataPtr f_pData)

#define IMPLEMENT_ACTION(action_name) \
    void C## action_name::FrameworkunifiedAction(CFrameworkunifiedState *f_pSourceState, CFrameworkunifiedState *f_pTargetState, CEventDataPtr f_pEventData)

#define DEFINE_GUARD(guard_name)\
  class C## guard_name : public CFrameworkunifiedGuard{ \
   public: \
      C## guard_name(std::string f_pName):CFrameworkunifiedGuard(f_pName) {} \
      virtual BOOL FrameworkunifiedEvaluate();\
  };

#define CREATE_GUARD(guard_name)\
    CFrameworkunifiedGuard *l_pGrd## guard_name = new C## guard_name(#guard_name);

#define CONNECT_GUARD(transition, guard)\
  l_pTrn## transition->FrameworkunifiedSetGuard(l_pGrd## guard);

#define CONNECT_CONDITION(connector_name , guard , target_state) \
  CHKNULL(l_pTrn## connector_name)\
  l_pTrn## connector_name->FrameworkunifiedAddCondition(l_pGrd## guard , l_p## target_state); \

#define IMPLEMENT_GUARD(guard_name) \
  BOOL C## guard_name::FrameworkunifiedEvaluate()


///////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup StateMachine_Macro
/// \~english @par Brief
///        These macros create instances for Shallow history transitions.
/// \~english @param [in] parent State name for external transition
/// \~english @retval None
/// \~english @par Prerequisite
///       - None
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - None
/// \~english @par Detail
///       These macros create instances of CFrameworkunifiedExternalTransition classes for external transitions of Shallow history transitions.\n
///       Otherwise, the CNullPointerException objects are excepted.
/// \~english @par Classification
///          Public
/// \~english @see  None
///////////////////////////////////////////////////////////////////////////////////////////
/// creates the instance of the shallow history transition
#define CREATE_SHALLOWHISTORYTRANSITION(parent) \
    CFrameworkunifiedExternalTransition *l_pTrn##parent##SHALLOWHISTORYSTATE = \
    new CFrameworkunifiedExternalTransition(l_p##parent##SHALLOWHISTORYSTATE); \
    CHKNULL(l_pTrn##parent##SHALLOWHISTORYSTATE);

/// creates the instance of the deep history transition
#define CREATE_DEEPHISTORYTRANSITION(parent) \
    CFrameworkunifiedExternalTransition *l_pTrn##parent##DEEPHISTORYSTATE = \
    new CFrameworkunifiedExternalTransition(l_p##parent##DEEPHISTORYSTATE); \
    CHKNULL(l_pTrn##parent##DEEPHISTORYSTATE);

#define CONNECT_CONDITION_SHALLOWHISTORYTRANSITION(connector_name , guard , target_state) \
    CHKNULL(l_pTrn## connector_name)\
    l_pTrn## connector_name->FrameworkunifiedAddCondition(l_pGrd## guard , l_p## target_state##SHALLOWHISTORYSTATE); \


#define CONNECT_CONDITION_DEEPHISTORYTRANSITION(connector_name , guard , target_state) \
    CHKNULL(l_pTrn## connector_name)\
    l_pTrn## connector_name->FrameworkunifiedAddCondition(l_pGrd## guard , l_p## target_state##DEEPHISTORYSTATE); \


#define CONNECT_ACTION(transition, action_name) \
    CHKNULL(l_pTrn## action_name); \
    CHKNULL(l_pTrn## transition); \
    l_pTrn## transition->FrameworkunifiedAddAction(l_pTrn## action_name); \

#ifdef DEBUG
#define FRAMEWORKUNIFIED_PRINT_HSM(x) (FrameworkunifiedGetStateMachine(x))->FrameworkunifiedPrintXML();
#define PRINT_HSM() FrameworkunifiedPrintXML();
#else
#define FRAMEWORKUNIFIED_PRINT_HSM(x)
#define PRINT_HSM()
#endif

/// defines options for integrating user change state
typedef enum _EUserChangeOptions {
  eUserchangeIgnore = 0,
  eUserchangeReInit,
  eUserchangeRetPrevState
} EUserChangeOptions;
#endif /* __FRAMEWORKUNIFIED_NATIVESERVICES_NATIVESERVICES_INC_FRAMEWORK_STATEMACHINE_FRAMEWORKUNIFIED_FRAMEWORKUNIFIED_SM_FRAMEWORK_TYPES_H__ */  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
