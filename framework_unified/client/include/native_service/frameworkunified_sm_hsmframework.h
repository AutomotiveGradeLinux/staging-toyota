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
/// This file has the CFrameworkunifiedHSMParentFramework class definitions. CFrameworkunifiedHSMParentFramework is derived from
/// CFrameworkunifiedHSM class. It defines the states required for NS statemachine framework, it creates the NS
/// statemachine and implements the reaction associated with the events
///
///////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file frameworkunified_sm_hsmframework.h
 * @brief \~english This file has the CFrameworkunifiedHSMParentFramework class definitions. CFrameworkunifiedHSMParentFramework is derived from
 *        CFrameworkunifiedHSM class. It defines the states required for NS statemachine framework, it creates the NS
 *        statemachine and implements the reaction associated with the events
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
#ifndef __FRAMEWORKUNIFIED_NATIVESERVICES_NATIVESERVICES_INC_FRAMEWORK_STATEMACHINE_FRAMEWORKUNIFIED_SM_HSMFRAMEWORK_H__  // NOLINT  (build/header_guard)
#define __FRAMEWORKUNIFIED_NATIVESERVICES_NATIVESERVICES_INC_FRAMEWORK_STATEMACHINE_FRAMEWORKUNIFIED_SM_HSMFRAMEWORK_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <native_service/frameworkunified_sm_hsm.h>
#include <native_service/frameworkunified_sm_framework_types.h>
#include <native_service/ns_timer_if.h>
#include <native_service/ns_message_center_if.h>
#include <native_service/frameworkunified_framework_types.h>
#include <map>
#include <string>

class CFrameworkunifiedHSMFramework: public CFrameworkunifiedHSM {
 public:
  /**
   * \~english state machine state
   */
  typedef enum _FRAMEWORKUNIFIED_HSM_STATES {
    esFrameworkunifiedRoot = 0,  //!< \~english Root state
    esFrameworkunifiedInitialization,  //!< \~english Initialization state
    esFrameworkunifiedLoadData,  //!< \~english LoadData state
    esFrameworkunifiedPre,  //!< \~english Pre state
    esFrameworkunifiedBackground,  //!< \~english Background state
    esFrameworkunifiedRun,  //!< \~english Run state
    esFrameworkunifiedLoadSessions,  //!< \~english LoadSessions state
    esFrameworkunifiedReady,  //!< \~english Ready state
    esFrameworkunifiedStop,  //!< \~english Stop state
    esFrameworkunifiedApp,  //!< \~english App state
    esFrameworkunifiedUserChange,  //!< \~english UserChange state
    esFrameworkunifiedThreadRoot,  //!< \~english ThreadRoot state
    esFrameworkunifiedThreadIdle,  //!< \~english ThreadIdle state
    esFrameworkunifiedThreadStart,  //!< \~english ThreadStart state
    esFrameworkunifiedThreadReady,  //!< \~english ThreadReady state
    esFrameworkunifiedThreadStop,  //!< \~english ThreadStop state
    esFrameworkunifiedStopping,  //!< \~english Stopping state
  } FRAMEWORKUNIFIED_HSM_STATES;

  /// Define evFrameworkunifiedStart event
  DEFINE_EVENT(evFrameworkunifiedStart, 1)

  /// Define evFrameworkunifiedStop event
  DEFINE_EVENT(evFrameworkunifiedStop, 5)

  /// Define evFrameworkunifiedDestroy event
  DEFINE_EVENT(evFrameworkunifiedDestroy, 3)

  /// Define evFrameworkunifiedReady event
  DEFINE_EVENT(evFrameworkunifiedReady, 12)

  /// Define evFrameworkunifiedError event
  DEFINE_EVENT(evFrameworkunifiedError, 16)

  /// Define evFrameworkunifiedPreStart event
  DEFINE_EVENT(evFrameworkunifiedPreStart, 19)

  /// Define evFrameworkunifiedPreStop event
  DEFINE_EVENT(evFrameworkunifiedPreStop, 20)

  /// Define evFrameworkunifiedBackgroundStart event
  DEFINE_EVENT(evFrameworkunifiedBackgroundStart, 21)

  /// Define evFrameworkunifiedBackgroundStop event
  DEFINE_EVENT(evFrameworkunifiedBackgroundStop, 22)

  // Define internal transition for event evFrameworkunifiedStart in sFrameworkunifiedStop state
  DEFINE_INTERNALTRANSITION(OnEventStartInStopState)

  // Define internal transition for event evFrameworkunifiedUserChange in sFrameworkunifiedInitialization state
  DEFINE_INTERNALTRANSITION(OnEventUserChangeInInitializationState)

  // Define internal transition for event evFrameworkunifiedUserChange in sFrameworkunifiedRun state
  DEFINE_INTERNALTRANSITION(OnEventUserChangeInRunState)

  DEFINE_INTERNALTRANSITION(OnEventStopInStoppingState)

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHSMFramework
  /// \~english @par Brief
  ///        CFrameworkunifiedHSMFramework default constructor
  /// \~english @param None
  /// \~english @retval None
  /// \~english @par Prerequisite
  ///        None
  /// \~english @par Change of internal state
  ///        CFrameworkunifiedHSM()
  /// \~english @par Conditions of processing failure
  ///        None
  /// \~english @par Detail
  ///        CFrameworkunifiedHSMFramework default constructor, for create CFrameworkunifiedHSMFramework object
  /// \~english @par Classification
  ///        Public
  /// \~english @par Type
  ///        sync only
  /// \~english @see
  ///
  /////////////////////////////////////////////////////////////////////////////////////
  CFrameworkunifiedHSMFramework();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHSMFramework
  /// \~english @par Brief
  ///        CFrameworkunifiedHSMFramework constructor with param
  /// \~english @param [in] f_pHApp
  ///        PVOID f_pHApp  -  for parent class constructor
  /// \~english @retval None
  /// \~english @par Prerequisite
  ///        - None
  /// \~english @par Change of internal state
  ///        CFrameworkunifiedHSM(f_pHApp)
  /// \~english @par Conditions of processing failure
  ///        - None
  /// \~english @par Detail
  ///        CFrameworkunifiedHSMFramework constructor with param, for create CFrameworkunifiedHSMFramework object
  /// \~english @par Classification
  ///        - Public
  /// \~english @par Type
  ///        - sync only
  /// \~english @see
  ///
  /////////////////////////////////////////////////////////////////////////////////////
  CFrameworkunifiedHSMFramework(PVOID f_pHApp);  // NOLINT  (readability/nolint)

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHSMFramework
  /// \~english @par Summary
  ///       pure virtual fuction
  /// \~english @param [in] f_pEventData
  ///        PVOID f_pEventData  -  event data
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
  /// FrameworkunifiedCreate
  /// Statemachine states and events created and connected in this interface.
  ///
  /// \param [in] f_pEventData
  ///     PVOID - Event data
  ///
  /// \return EFrameworkunifiedStatus
  ///         EFrameworkunifiedStatus - Returns status of operation
  ///
  ///////////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus FrameworkunifiedCreate(PVOID f_pEventData = NULL) = 0;

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHSMFramework
  /// \~english @par Summary
  ///       pure virtual fuction
  /// \~english @param [in] f_pAppState
  ///        CFrameworkunifiedState *f_pAppState  -  state object to be added in the ready state.
  /// \~english @param [in] f_bIsDefaultState
  ///        BOOL f_bIsDefaultState  -  if the state is default state
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
  /// FrameworkunifiedFrameworkConnect
  /// This adds the given state as a sub state of sReady state of the Framework
  ///
  /// \param [in] f_pAppState
  ///     CFrameworkunifiedState* - state object to be added in the ready state.
  ///
  /// \param [in] f_bIsDefaultState
  ///     BOOL - defines if the state is default state
  ///
  /// \return EFrameworkunifiedStatus
  ///         EFrameworkunifiedStatus - Returns status of operation
  ///
  ///////////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus FrameworkunifiedFrameworkConnect(CFrameworkunifiedState *f_pAppState, BOOL f_bIsDefaultState = FALSE) = 0;

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHSMFramework
  /// \~english @par Summary
  ///       pure virtual fuction
  /// \~english @param [in] f_eFrameworkunifiedState
  ///        FRAMEWORKUNIFIED_HSM_STATES f_eFrameworkunifiedState  -  enum maping to framework state
  /// \~english @param [in] f_uiEventId
  ///        UI_32 f_uiEventId  -  Event id
  /// \~english @param [in] f_pReaction
  ///        CFrameworkunifiedReaction *f_pReaction  -  Reaction object for attaching to event
  /// \~english @param [in] f_strEventName
  ///        std::string f_strEventName  -  Event name
  /// \~english @param [in] f_bIsDeferredEvent
  ///        BOOL f_bIsDeferredEvent  -  defines if the event is deferred event
  /// \~english @retval EFrameworkunifiedStatus   depend on implement class
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///       - None
  /// \~english @par Causes of failures
  ///       - None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       sync only
  /// \~english @par Detail
  ///       pure virtual function , implement by child class
  /// \~english @see
  ////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedFrameworkConnect
  /// This connects the reaction to event and add event to states
  ///
  /// \param [in] f_eFrameworkunifiedState
  ///     FRAMEWORKUNIFIED_HSM_STATES - enum maping to framework state
  ///
  /// \param [in] f_uiEventId
  ///     UI_32 - Event id
  ///
  /// \param [in] f_pReaction
  ///     CFrameworkunifiedReaction* - Reaction object for attaching to event
  ///
  /// \param [in] f_strEventName
  ///     std::string - Event name
  ///
  /// \param [in] f_bIsDeferredEvent
  ///     BOOL - defines if the event is deferred event
  ///
  /// \return EFrameworkunifiedStatus
  ///         EFrameworkunifiedStatus - Returns status of operation
  ///
  ///////////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus FrameworkunifiedFrameworkConnect(FRAMEWORKUNIFIED_HSM_STATES f_eFrameworkunifiedState, UI_32 f_uiEventId,
                                         CFrameworkunifiedReaction *f_pReaction, std::string f_strEventName = "",
                                         BOOL f_bIsDeferredEvent = FALSE) = 0;

 protected:
  /// Stores the state objects against FRAMEWORKUNIFIED_HSM_STATES enumerations
  std::map<UI_32, CFrameworkunifiedState *> *m_pFrameworkunifiedStateList;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/// It defines the states required for NS statemachine framework, it creates the NS
/// statemachine and implements the reaction associated with the events
///////////////////////////////////////////////////////////////////////////////////////////////////
class CFrameworkunifiedHSMParentFramework: public CFrameworkunifiedHSMFramework {
 public:
  /// Define evFrameworkunifiedLoadPersistenceAck event
  DEFINE_EVENT(evFrameworkunifiedLoadFileAck, 2)

  /// Define evFrameworkunifiedLoadComplete event
  DEFINE_EVENT(evFrameworkunifiedLoadComplete, 4)

  /// Define evFrameworkunifiedUserChange event
  DEFINE_EVENT(evFrameworkunifiedUserChange, 6)

  /// Define evFrameworkunifiedReInit event
  DEFINE_EVENT(evFrameworkunifiedReInit, 7)

  /// Define evFrameworkunifiedLoadUserData event
  DEFINE_EVENT(evFrameworkunifiedLoadUserData, 8)

  /// Define evFrameworkunifiedCloseSessionAck event
  DEFINE_EVENT(evFrameworkunifiedCloseSessionAck, 9)

  /// Define evFrameworkunifiedCloseSessionAck event
  DEFINE_EVENT(evFrameworkunifiedOpenSessionAck, 10)

  /// Define evFrameworkunifiedStopComplete event
  DEFINE_EVENT(evFrameworkunifiedStopComplete, 11)

  /// Define evFrameworkunifiedOpenSessionReq event
  DEFINE_EVENT(evFrameworkunifiedOpenSessionReq, 14)

  /// Define evFrameworkunifiedCloseSessionReq event
  DEFINE_EVENT(evFrameworkunifiedCloseSessionReq, 15)

  /// Define evFrameworkunifiedInit event
  DEFINE_EVENT(evFrameworkunifiedInit, 16)

  /// Define evFrameworkunifiedUserChangeComplete event
  DEFINE_EVENT(evFrameworkunifiedUserChangeComplete, 17)

  /// Define evFrameworkunifiedLoadSessions event
  DEFINE_EVENT(evFrameworkunifiedLoadSessions, 18)

  /// Define sFrameworkunifiedRoot state
  DEFINE_COMPOSITESTATE_OVERRIDE(sFrameworkunifiedRoot)

  /// Define sFrameworkunifiedRoot state
  DEFINE_COMPOSITESTATE_OVERRIDE(sFrameworkunifiedApp)

  /// Define sFrameworkunifiedInitialization state
  DEFINE_COMPOSITESTATE_OVERRIDE(sFrameworkunifiedInitialization)

  /// Define sFrameworkunifiedPre state
  DEFINE_COMPOSITESTATE_OVERRIDE(sFrameworkunifiedPre)

  /// Define sFrameworkunifiedBackground state
  DEFINE_COMPOSITESTATE_OVERRIDE(sFrameworkunifiedBackground)

  /// Define sFrameworkunifiedRun state
  DEFINE_COMPOSITESTATE_OVERRIDE(sFrameworkunifiedRun)

  /// Define sFrameworkunifiedStop state
  DEFINE_COMPOSITESTATE_OVERRIDE(sFrameworkunifiedStop)

  /// Define sFrameworkunifiedReady state
  DEFINE_COMPOSITESTATE_OVERRIDE(sFrameworkunifiedReady)

  /// Define sFrameworkunifiedLoadData state
  DEFINE_LEAFSTATE_OVERRIDE(sFrameworkunifiedLoadData)

  /// Define sFrameworkunifiedLoadSessions state
  DEFINE_LEAFSTATE_OVERRIDE(sFrameworkunifiedLoadSessions)

  /// Define sFrameworkunifiedStopping state
  DEFINE_LEAFSTATE_OVERRIDE(sFrameworkunifiedStopping)

  /// Define sFrameworkunifiedUserChange state
  DEFINE_LEAFSTATE_OVERRIDE(sFrameworkunifiedUserChange)

  // DEFINE_ACTION(aFrameworkunifiedOnStart)
  DEFINE_ACTION(aFrameworkunifiedOnStop)

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHSMParentFramework
  /// \~english @par Brief
  ///        CFrameworkunifiedHSMFramework default constructor
  /// \~english @param None
  /// \~english @retval None
  /// \~english @par Prerequisite
  ///        - None
  /// \~english @par Change of internal state
  ///        m_pFrameworkunifiedStateList = new std::map<UI_32, CFrameworkunifiedState *>();
  /// \~english @par Conditions of processing failure
  ///        apply space for m_pFrameworkunifiedStateList failed
  /// \~english @par Detail
  ///        CFrameworkunifiedHSMFramework default constructor, for create CFrameworkunifiedHSMFramework object
  /// \~english @par Classification
  ///        Public
  /// \~english @par Type
  ///        sync only
  /// \~english @see
  ///
  /////////////////////////////////////////////////////////////////////////////////////
  CFrameworkunifiedHSMParentFramework();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHSMParentFramework
  /// \~english @par Brief
  ///        CFrameworkunifiedHSMFramework constructor with param
  /// \~english @param [in] f_pHApp
  ///        PVOID f_pHApp  -  Name of the state
  /// \~english @retval None
  /// \~english @par Prerequisite
  ///        None
  /// \~english @par Change of internal state
  ///        m_pFrameworkunifiedStateList = new std::map<UI_32, CFrameworkunifiedState *>();
  /// \~english @par Conditions of processing failure
  ///        apply space for m_pFrameworkunifiedStateList failed
  /// \~english @par Detail
  ///        CFrameworkunifiedHSMFramework constructor with param, for create CFrameworkunifiedHSMFramework object
  /// \~english @par Classification
  ///        Public
  /// \~english @par Type
  ///        sync only
  /// \~english @see
  ///
  /////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// CFrameworkunifiedHSMParentFramework
  /// default constructor
  /// \param [in] f_pName
  ///     string - Name of the state
  ///
  /// \return none
  ///////////////////////////////////////////////////////////////////////////////////////////
  CFrameworkunifiedHSMParentFramework(PVOID f_pHApp);  // NOLINT  (readability/nolint)

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHSMParentFramework
  /// \~english @par Brief
  ///        CFrameworkunifiedHSMFramework destructor
  /// \~english @param None
  /// \~english @retval None
  /// \~english @par Prerequisite
  ///        None
  /// \~english @par Change of internal state
  ///        None
  /// \~english @par Conditions of processing failure
  ///        None
  /// \~english @par Detail
  ///        CFrameworkunifiedHSMFramework destructor, for destory CFrameworkunifiedHSMFramework object
  /// \~english @par Classification
  ///        Public
  /// \~english @par Type
  ///        sync only
  /// \~english @see
  ///
  /////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// ~CFrameworkunifiedHSMParentFramework
  /// Class destructor
  ///
  /// \return none
  ///////////////////////////////////////////////////////////////////////////////////////////
  virtual ~CFrameworkunifiedHSMParentFramework();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHSMParentFramework
  /// \~english @par Brief
  ///        create state and event
  /// \~english @param [in] f_pEventData
  ///        PVOID f_pEventData  -  event data
  /// \~english @retval eFrameworkunifiedStatusOK  create state and event success
  /// \~english @retval eFrameworkunifiedStatusNullPointer  NULL point exception occur when create state and event
  /// \~english @par Prerequisite
  ///        None
  /// \~english @par Change of internal state
  ///        None
  /// \~english @par Conditions of processing failure
  ///        NULL pointer exception occur
  /// \~english @par Detail
  ///        Statemachine states and events created and connected in this interface.
  /// \~english @par Classification
  ///        Public
  /// \~english @par Type
  ///        sync only
  /// \~english @see
  ///
  /////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedCreate
  /// Statemachine states and events created and connected in this interface.
  ///
  /// \param [in] f_pHApp
  ///     PVOID - application handle
  ///
  /// \return EFrameworkunifiedStatus
  ///         EFrameworkunifiedStatus - Returns status of operation
  ///
  ///////////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus FrameworkunifiedCreate(PVOID f_pEventData = NULL);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHSMParentFramework
  /// \~english @par Brief
  ///        add sub state
  /// \~english @param [in] f_pAppState
  ///        CFrameworkunifiedState *f_pAppState  -  state object to be added in the ready state.
  /// \~english @param [in] f_bIsDefaultState
  ///        BOOL f_bIsDefaultState  -  defines if the state is default state.
  /// \~english @retval eFrameworkunifiedStatusOK  add sub state success
  /// \~english @retval eFrameworkunifiedStatusNullPointer  NULL point exception occur when add sub state
  /// \~english @par Prerequisite
  ///        None
  /// \~english @par Change of internal state
  ///        None
  /// \~english @par Conditions of processing failure
  ///        NULL point exception occur when add sub state
  /// \~english @par Detail
  ///        This adds the given state as a sub state of sReady state of the Framework
  /// \~english @par Classification
  ///        Public
  /// \~english @par Type
  ///        sync only
  /// \~english @see
  ///
  /////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedFrameworkConnect
  /// This adds the given state as a sub state of sReady state of the Framework
  ///
  /// \param [in] f_pAppState
  ///     CFrameworkunifiedState* - state object to be added in the ready state.
  ///
  /// \param [in] f_bIsDefaultState
  ///     BOOL - defines if the state is default state
  ///
  /// \return EFrameworkunifiedStatus
  ///         EFrameworkunifiedStatus - Returns status of operation
  ///
  ///////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus FrameworkunifiedFrameworkConnect(CFrameworkunifiedState *f_pAppState, BOOL f_bIsDefaultState = FALSE);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHSMParentFramework
  /// \~english @par Brief
  ///        connects the reaction to event and add event to states
  /// \~english @param [in] f_eFrameworkunifiedState
  ///        FRAMEWORKUNIFIED_HSM_STATES f_eFrameworkunifiedState  -  framework state
  /// \~english @param [in] f_uiEventId
  ///        UI_32 f_uiEventId  -  Event id
  /// \~english @param [in] f_pReaction
  ///        CFrameworkunifiedReaction *f_pReaction  -  Reaction object
  /// \~english @param [in] f_strEventName
  ///        std::string f_strEventName  -  Event name
  /// \~english @param [in] f_bIsDeferredEvent
  ///        BOOL f_bIsDeferredEvent  -  defines if the event is deferred event
  /// \~english @retval eFrameworkunifiedStatusOK  add sub state success
  /// \~english @retval eFrameworkunifiedStatusNullPointer  NULL point exception occur when add sub state
  /// \~english @par Prerequisite
  ///        None
  /// \~english @par Change of internal state
  ///        None
  /// \~english @par Conditions of processing failure
  ///        NULL point exception occur when add sub state
  /// \~english @par Detail
  ///        This connects the reaction to event and add event to states
  /// \~english @par Classification
  ///        Public
  /// \~english @par Type
  ///        sync only
  /// \~english @see
  ///
  /////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedFrameworkConnect
  /// This connects the reaction to event and add event to states
  ///
  /// \param [in] f_eFrameworkunifiedState
  ///     FRAMEWORKUNIFIED_HSM_STATES - enum maping to framework state
  ///
  /// \param [in] f_uiEventId
  ///     UI_32 - Event id
  ///
  /// \param [in] f_pReaction
  ///     CFrameworkunifiedReaction* - Reaction object for attaching to event
  ///
  /// \param [in] f_strEventName
  ///     std::string - Event name
  ///
  /// \param [in] f_bIsDeferredEvent
  ///     BOOL - defines if the event is deferred event
  ///
  /// \return EFrameworkunifiedStatus
  ///         EFrameworkunifiedStatus - Returns status of operation
  ///
  ///////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus FrameworkunifiedFrameworkConnect(FRAMEWORKUNIFIED_HSM_STATES f_eFrameworkunifiedState, UI_32 f_uiEventId,
                                 CFrameworkunifiedReaction *f_pReaction, std::string f_strEventName = "",
                                 BOOL f_bIsDeferredEvent = FALSE);
};

class CFrameworkunifiedHSMChildFramework: public CFrameworkunifiedHSMFramework {
 public:
  /// Define sFrameworkunifiedThreadIdle state
  DEFINE_COMPOSITESTATE_OVERRIDE(sFrameworkunifiedThreadRoot)

  /// Define sFrameworkunifiedThreadIdle state
  DEFINE_COMPOSITESTATE_OVERRIDE(sFrameworkunifiedThreadIdle)

  /// Define sFrameworkunifiedThreadStart state
  DEFINE_COMPOSITESTATE_OVERRIDE(sFrameworkunifiedThreadStart)

  /// Define sFrameworkunifiedThreadReady state
  DEFINE_COMPOSITESTATE_OVERRIDE(sFrameworkunifiedThreadReady)

  /// Define sFrameworkunifiedThreadStop state
  DEFINE_LEAFSTATE_OVERRIDE(sFrameworkunifiedThreadStop)

  // Define internal transition for event evFrameworkunifiedDestroy
  DEFINE_INTERNALTRANSITION(OnDestroyThread)

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHSMChildFramework
  /// \~english @par Brief
  ///        CFrameworkunifiedHSMChildFramework constructor
  /// \~english @param None
  /// \~english @retval None
  /// \~english @par Prerequisite
  ///        None
  /// \~english @par Change of internal state
  ///        None
  /// \~english @par Conditions of processing failure
  ///        None
  /// \~english @par Detail
  ///        CFrameworkunifiedHSMChildFramework constructor, for create CFrameworkunifiedHSMChildFramework object
  /// \~english @par Classification
  ///        Public
  /// \~english @par Type
  ///        sync only
  /// \~english @see
  ///
  /////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// CFrameworkunifiedHSMChildFramework
  /// default constructor
  ///
  /// \return none
  ///////////////////////////////////////////////////////////////////////////////////////////
  CFrameworkunifiedHSMChildFramework();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHSMChildFramework
  /// \~english @par Brief
  ///        CFrameworkunifiedHSMChildFramework constructor with param
  /// \~english @param [in] f_pHApp
  ///        PVOID f_pHApp  - application handle
  /// \~english @retval None
  /// \~english @par Prerequisite
  ///        None
  /// \~english @par Change of internal state
  ///        m_pFrameworkunifiedStateList = new std::map<UI_32, CFrameworkunifiedState *>();
  /// \~english @par Conditions of processing failure
  ///        m_pFrameworkunifiedStateList apply space fail
  /// \~english @par Detail
  ///        CFrameworkunifiedHSMChildFramework constructor with param, for create CFrameworkunifiedHSMChildFramework object
  /// \~english @par Classification
  ///        Public
  /// \~english @par Type
  ///        sync only
  /// \~english @see
  ///
  /////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// CFrameworkunifiedHSMChildFramework
  /// default constructor
  /// \param [in] f_pHApp
  ///     PVOID - application handle
  ///
  /// \return none
  ///////////////////////////////////////////////////////////////////////////////////////////
  CFrameworkunifiedHSMChildFramework(PVOID f_pHApp);  // NOLINT  (readability/nolint)

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHSMChildFramework
  /// \~english @par Brief
  ///        CFrameworkunifiedHSMChildFramework destructor
  /// \~english @param None
  /// \~english @retval None
  /// \~english @par Prerequisite
  ///        None
  /// \~english @par Change of internal state
  ///        None
  /// \~english @par Conditions of processing failure
  ///        None
  /// \~english @par Detail
  ///        CFrameworkunifiedHSMChildFramework destructor, for destory CFrameworkunifiedHSMChildFramework object
  /// \~english @par Classification
  ///        Public
  /// \~english @par Type
  ///        sync only
  /// \~english @see
  ///
  /////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// ~CFrameworkunifiedHSMChildFramework
  /// Class destructor
  ///
  /// \return none
  ///////////////////////////////////////////////////////////////////////////////////////////
  virtual ~CFrameworkunifiedHSMChildFramework();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHSMChildFramework
  /// \~english @par Brief
  ///        create state and event
  /// \~english @param [in] f_pEventData
  ///        PVOID f_pEventData  -  event data
  /// \~english @retval eFrameworkunifiedStatusOK  create state and event success
  /// \~english @retval eFrameworkunifiedStatusNullPointer  NULL point exception occur when create state and event
  /// \~english @par Prerequisite
  ///        None
  /// \~english @par Change of internal state
  ///        None
  /// \~english @par Conditions of processing failure
  ///        NULL pointer exception occur
  /// \~english @par Detail
  ///        Statemachine states and events created and connected in this interface.
  /// \~english @par Classification
  ///        Public
  /// \~english @par Type
  ///        sync only
  /// \~english @see
  ///
  /////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedCreate
  /// Statemachine states and events created and connected in this interface.
  ///
  /// \param [in] f_pEventData
  ///     PVOID - Event data
  ///
  /// \return EFrameworkunifiedStatus
  ///         EFrameworkunifiedStatus - Returns status of operation
  ///
  ///////////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus FrameworkunifiedCreate(PVOID f_pEventData = NULL);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHSMChildFramework
  /// \~english @par Brief
  ///        add sub state
  /// \~english @param [in] f_pAppState
  ///        CFrameworkunifiedState *f_pAppState  -  state object to be added in the ready state.
  /// \~english @param [in] f_bIsDefaultState
  ///        BOOL f_bIsDefaultState  -  defines if the state is default state.
  /// \~english @retval eFrameworkunifiedStatusOK  add sub state success
  /// \~english @retval eFrameworkunifiedStatusNullPointer  NULL point exception occur when add sub state
  /// \~english @par Prerequisite
  ///        None
  /// \~english @par Change of internal state
  ///        None
  /// \~english @par Conditions of processing failure
  ///        NULL point exception occur when add sub state
  /// \~english @par Detail
  ///        This adds the given state as a sub state of sReady state of the Framework
  /// \~english @par Classification
  ///        Public
  /// \~english @par Type
  ///        sync only
  /// \~english @see
  ///
  /////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedFrameworkConnect
  /// This adds the given state as a sub state of sReady state of the Framework
  ///
  /// \param [in] f_pAppState
  ///     CFrameworkunifiedState* - state object to be added in the ready state.
  ///
  /// \param [in] f_bIsDefaultState
  ///     BOOL - defines if the state is default state
  ///
  /// \return EFrameworkunifiedStatus
  ///         EFrameworkunifiedStatus - Returns status of operation
  ///
  ///////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus FrameworkunifiedFrameworkConnect(CFrameworkunifiedState *f_pAppState, BOOL f_bIsDefaultState = FALSE);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedHSMChildFramework
  /// \~english @par Brief
  ///        connects the reaction to event and add event to states
  /// \~english @param [in] f_eFrameworkunifiedState
  ///        FRAMEWORKUNIFIED_HSM_STATES f_eFrameworkunifiedState  -  framework state
  /// \~english @param [in] f_uiEventId
  ///        UI_32 f_uiEventId  -  Event id
  /// \~english @param [in] f_pReaction
  ///        CFrameworkunifiedReaction *f_pReaction  -  Reaction object
  /// \~english @param [in] f_strEventName
  ///        std::string f_strEventName  -  Event name
  /// \~english @param [in] f_bIsDeferredEvent
  ///        BOOL f_bIsDeferredEvent  -  defines if the event is deferred event
  /// \~english @retval eFrameworkunifiedStatusOK  add sub state success
  /// \~english @retval eFrameworkunifiedStatusNullPointer  NULL point exception occur when add sub state
  /// \~english @par Prerequisite
  ///        None
  /// \~english @par Change of internal state
  ///        None
  /// \~english @par Conditions of processing failure
  ///        NULL point exception occur when add sub state
  /// \~english @par Detail
  ///        This connects the reaction to event and add event to states
  /// \~english @par Classification
  ///        Public
  /// \~english @par Type
  ///        sync only
  /// \~english @see
  ///
  /////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedFrameworkConnect
  /// This connects the reaction to event and add event to states
  ///
  /// \param [in] f_eFrameworkunifiedState
  ///     FRAMEWORKUNIFIED_HSM_STATES - enum maping to framework state
  ///
  /// \param [in] f_uiEventId
  ///     UI_32 - Event id
  ///
  /// \param [in] f_pReaction
  ///     CFrameworkunifiedReaction* - Reaction object for attaching to event
  ///
  /// \param [in] f_strEventName
  ///     std::string - Event name
  ///
  /// \param [in] f_bIsDeferredEvent
  ///     BOOL - defines if the event is deferred event
  ///
  /// \return EFrameworkunifiedStatus
  ///         EFrameworkunifiedStatus - Returns status of operation
  ///
  ///////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus FrameworkunifiedFrameworkConnect(FRAMEWORKUNIFIED_HSM_STATES f_eFrameworkunifiedState,
                                 UI_32 f_uiEventId,
                                 CFrameworkunifiedReaction *f_pReaction, std::string f_strEventName = "",
                                 BOOL f_bIsDeferredEvent = FALSE);

  /// function pointer of the Thread start routine
  CbFuncPtr m_fpStartThread;

  /// function pointer of the Thread stop routine
  CbFuncPtr m_fpStopThread;
};

#endif /* __FRAMEWORKUNIFIED_NATIVESERVICES_NATIVESERVICES_INC_FRAMEWORK_STATEMACHINE_FRAMEWORKUNIFIED_SM_HSMFRAMEWORK_H__ */  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
