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
/// \ingroup  tag_StateMachine
/// \brief
///
/// This File has public Macro definition that simplifies the statemachine implementation
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef FRAMEWORK_UNIFIED_CLIENT_NS_FRAMEWORKCORE_INCLUDE_STATEMACHINE_FRAMEWORKUNIFIED_SM_FRAMEWORK_CORE_H_
#define FRAMEWORK_UNIFIED_CLIENT_NS_FRAMEWORKCORE_INCLUDE_STATEMACHINE_FRAMEWORKUNIFIED_SM_FRAMEWORK_CORE_H_
/// Include files
#include <native_service/frameworkunified_framework_types.h>
#include <vector>
#include <utility>

/// connect child state to parent state and sets it as default state
#define CONNECT_DEFAULTSTATE(parent, child) \
    FrameworkunifiedConnect(l_p## parent, l_p## child, TRUE);

/// connect child state to parent state
#define CONNECT_STATE(parent, child) \
    FrameworkunifiedConnect(l_p## parent, l_p## child);

/// connect the deferred  event and reactions and associate them with the state
#define CONNECT_DEFERREDEVENT(state, eventid) \
    FrameworkunifiedConnect(l_p## state, _## eventid, NULL, #eventid, TRUE); \

/// connect the event and reactions and associate them with the state
#define CONNECT_EVENT(state, eventid, reaction) \
    FrameworkunifiedConnect(l_p## state, _## eventid, l_pTrn## reaction, #eventid);

/// connect the event and reactions and associate them with the state
#define CONNECT_LOCAL_EVENT(state, eventid, reaction) \
    FrameworkunifiedConnect(l_p## state, _## eventid, l_pLocalTrn## reaction, #eventid);

/// Sets the state as root state in the statemachine
#define CONNECTROOT(state) \
    FrameworkunifiedConnect(l_p## state);

/// Creates the state
#define CREATE_STATE(class_name) \
    CHKNULL(m_pFrameworkunifiedStateList) \
    C## class_name *l_p## class_name = new C## class_name(#class_name); \
    CHKNULL(l_p## class_name) \
    m_pFrameworkunifiedStateList->insert(std::pair<UI_32, CFrameworkunifiedState*>(e## class_name, l_p## class_name));

// create a new shallow history state and add it in parent state
#define ADD_SHALLOWHISTORYSTATE(parent) \
    CFrameworkunifiedShallowHistoryState *l_p##parent##SHALLOWHISTORYSTATE = new CFrameworkunifiedShallowHistoryState(SHALLOWHISTORYSTATE);\
    FrameworkunifiedConnect(l_p## parent, l_p##parent##SHALLOWHISTORYSTATE);\
    l_p##parent##SHALLOWHISTORYSTATE->SetDefaultHistory();

// create a new shallow history state and add it in parent state
#define ADD_DEEPHISTORYSTATE(parent) \
    CFrameworkunifiedDeepHistoryState *l_p##parent##DEEPHISTORYSTATE = new CFrameworkunifiedDeepHistoryState(DEEPHISTORYSTATE);\
    FrameworkunifiedConnect(l_p## parent, l_p##parent##DEEPHISTORYSTATE);\
    l_p##parent##DEEPHISTORYSTATE->SetDefaultHistory();

/// connect deep history event to reaction and add to state
#define CONNECT_DEEPHISTORYEVENT(state, eventid, reaction) \
    FrameworkunifiedConnect(l_p## state, _## eventid, l_pTrn##reaction##DEEPHISTORYSTATE, #eventid);

/// connect shallow history event to reaction and add to state
#define CONNECT_SHALLOWHISTORYEVENT(state, eventid, reaction) \
    FrameworkunifiedConnect(l_p## state, _## eventid, l_pTrn##reaction##SHALLOWHISTORYSTATE, #eventid);

typedef struct _HSMConfigOptions {
  EUserChangeOptions eUserChange;
  BOOL bAutoPublishServiceAvaialble;
  BOOL bWaitInStoppingState;
} HSMConfigOptions;

#endif  // FRAMEWORK_UNIFIED_CLIENT_NS_FRAMEWORKCORE_INCLUDE_STATEMACHINE_FRAMEWORKUNIFIED_SM_FRAMEWORK_CORE_H_
