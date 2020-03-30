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
 * @file frameworkunified_sm_framework_dispatch.h
 * @brief \~english This file contains basic api to operate state machine
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
#ifndef __FRAMEWORKUNIFIED_NATIVESERVICES_NATIVESERVICES_INC_FRAMEWORK_STATEMACHINE_FRAMEWORKUNIFIED_SM_FRAMEWORK_DISPATCH_H__  // NOLINT  (build/header_guard)
#define __FRAMEWORKUNIFIED_NATIVESERVICES_NATIVESERVICES_INC_FRAMEWORK_STATEMACHINE_FRAMEWORKUNIFIED_SM_FRAMEWORK_DISPATCH_H__

#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_framework_types.h>
#include <native_service/frameworkunified_sm_framework_types.h>
#include <native_service/frameworkunified_sm_hsmframework.h>

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedSetStateMachine
/// \~english @par Brief
///        Get pointer to the statemachine object.
/// \~english @param [in] hApp
///        HANDLE   - Handle to the framework application
/// \~english @retval CFrameworkunifiedHSMParentFramework* pointer to the statemachine object
/// \~english @retval NULL Failed to get pointer to the statemachine object
/// \~english @par Prerequisite
///       - Generation/Initialization of Dispatcher for the Application by FrameworkunifiedHSMDispatcherWithArguments has been done.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - HANDLE specified in the argument (hApp) is not appropriate (which is invalid) . [NULL]
///       - Pointer to statemachine object is NULL. [NULL]
/// \~english @par Detail
///        Get pointer to the statemachine object associated with application framework handle.\n
///        Do not release the statemachine object in application, because it is a part of the Dispathcer.
/// \~english @par Classification
///          public
/// \~english @par type
///          sync only
/// \~english @see FrameworkunifiedHSMDispatcherWithArguments,FrameworkunifiedGetStateMachine
////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedSetStateMachine
///   sets the statemachine object
///
/// \param [in] hApp
///         HANDLE - Handle to the application session
///
/// \param [in] f_pFrameworkunifiedHSM
///         CFrameworkunifiedHSM - Pointer to statemachine object
///
/// \return pointer to the StateMachine object
///         CFrameworkunifiedHSMParentFramework* - pointer to the StateMachine object
///
/// \see
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSetStateMachine(HANDLE hApp,
                              CFrameworkunifiedHSM *f_pFrameworkunifiedHSM);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedAttachHSMEventToDispatcher
/// \~english @par Brief
///        Register an event in dispatcher
/// \~english @param [in] hApp
///        HANDLE hApp - Application framework handle
/// \~english @param [in] pServiceName
///        PCSTR pServiceName - Service Name
/// \~english @param [in] iCmd
///        UI_32 iCmd - service protocol command/request/acknowledgment ID
/// \~english @param [in] iEvent
///        UI_32 iEvent - event to be posted to active state on receiving iCmd from pServiceName.
/// \~english @param [in] hSession
///        HANDLE hSession - Session Handle.
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldHandle invalid state handle
/// \~english @par Prerequisite
///        - Generation/Initialization of Dispatcher for the Application by FrameworkunifiedHSMDispatcherWithArguments has been done.
/// \~english @par Change of internal state
///        - None
/// \~english @par Conditions of processing failure
///       - HANDLE specified in the argument (hApp) is NULL [eFrameworkunifiedStatusInvldHandle]
///       - HANDLE specified in the argument (hApp) is not appropriate (which is invalid) [eFrameworkunifiedStatusInvldHandle]
///       - Pointer to service name specified in the argument (pServiceName) is NULL [eFrameworkunifiedStatusInvldHandle]
/// \~english @par Detail
///        Register an event information (iCmd,iEvent) for service specified in the argument in dispatcher.\n
///        After that, when receiving an iCmd from service, post the event to active state.
/// \~english @par
///        Please note the following points when using this API for application.
///        - Operation is not guaranteed if a different handle is set for an application handle.
///        - Operation is not guaranteed if an invalid handle is set for a session handle.
///        - This API registers one event for one command and one callback for one event.\n
///          When registration is performed for the same command, the previously registered event is activated.
///       - The callback information is registered to the heap.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        sync only
/// \~english @see FrameworkunifiedAttachHSMEventsToDispatcher
///
/////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedAttachHSMEventToDispatcher
/// Registers a single event with the dispatcher for a given service.
///
///
/// \param [in] hApp
///         HANDLE - Application framework handle
/// \param [in] pServiceName
///         PCSTR - Service Name
/// \param [in] iCmd
///         UI_32 - service protocol command/request/acknowledgment ID
/// \param [in] iEvent
///         UI_32 - event to be posted to active state on receiving iCmd from pServiceName.
/// \param [in] hSession
///         HANDLE - Session Handle.
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedAttachHSMEventToDispatcher(HANDLE hApp, PCSTR pServiceName, UI_32 iCmd, UI_32 iEvent,
                                         HANDLE hSession = NULL);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedAttachHSMEventsToDispatcher
/// \~english @par Brief
///        Register events in dispatcher.
/// \~english @param [in] hApp
///        HANDLE hApp - Application framework handle
/// \~english @param [in] pServiceName
///        PCSTR pServiceName - Service Name
/// \~english @param [in] pEventIds
///        const FrameworkunifiedProtocolEvent *pEventIds - structure mapping event ids with the messages
/// \~english @param [in] uiEventCount
///        UI_32 uiEventCount - no of events
/// \~english @param [in] hSession
///        HANDLE hSession - Session Handle.
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldHandle invalid state handle
/// \~english @par Prerequisite
///        - Generation/Initialization of Dispatcher for the Application by FrameworkunifiedHSMDispatcherWithArguments has been done.
/// \~english @par Change of internal state
///        - None
/// \~english @par Conditions of processing failure
///       - HANDLE specified in the argument (hApp) is NULL [eFrameworkunifiedStatusInvldHandle]
///       - HANDLE specified in the argument (hApp) is not appropriate (which is invalid) [eFrameworkunifiedStatusInvldHandle]
///       - Pointer to service name specified in the argument (pServiceName) is NULL [eFrameworkunifiedStatusInvldHandle]
///       - Pointer to FrameworkunifiedProtocolEvent structure specified in the argument is NULL [eFrameworkunifiedStatusInvldHandle]
/// \~english @par Detail
///        Register events information (iCmd,iEvent) for service specified in the argument in dispatcher.\n
///        After that, when receiving an iCmd from service, post the event to active state.\n
///        If register failed, only the events that register before it are valid.\n
///        When multiple registration, if an error occurs in the second and subsequent, becomes effective only \n
///        information that has been registered so far. \n
///        For number of upper limit of the registration possible event, depending on \n
///        the securable size as the area of the FrameworkunifiedProtocolEvent structure. Therefore, note to ensure \n
///        the size of the area of the FrameworkunifiedProtocolEvent structure at the call side of API, and define \n
///        the number of event.
/// \~english @par
///        Please note the following points when using this API for application.
///        - Operation is not guaranteed if a different handle is set to Handle for the application.
///        - Operation is not guaranteed if an invalid handle is set for a session handle.
///        - If an error occurs when multiple events are registered, it is impossible to determine which event was valid.
///          Therefore, either register multiple event information again or delete multiple event information at the same time when registration fails.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        sync only
/// \~english @see FrameworkunifiedAttachHSMEventToDispatcher
///
/////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedAttachHSMEventsToDispatcher
/// Registers a multiple event with the dispatcher for a given service.
///
///
/// \param [in] hApp
///         HANDLE - Application framework handle
/// \param [in] pServiceName
///         PCSTR - Service Name
/// \param [in] pEventIds
///         const FrameworkunifiedProtocolEvent* - structure mapping event ids with the messages
/// \param [in] uiEventCount
///         UI_32 - no of events
/// \param [in] hSession
///         HANDLE - Session Handle.
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedAttachHSMEventsToDispatcher(HANDLE hApp, PCSTR pServiceName,
                                          const FrameworkunifiedProtocolEvent *pEventIds,
                                          UI_32 uiEventCount,
                                          HANDLE hSession = NULL);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedAttachHSMEventsToDispatcher
/// \~english @par Brief
///        register a notification in service
/// \~english @param [in] hApp
///        HANDLE hApp - Handle to the Framework
/// \~english @param [in] pNotification
///        PCSTR pNotification - Name of Notification
/// \~english @param [in] max_length
///        const UI_32 max_length - Max size of the notification message
/// \~english @param [in] persType
///        const EFrameworkunifiedNotificationType persType  - Type of persistent
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldHandle invalid state handle
/// \~english @par Prerequisite
///        - None
/// \~english @par Change of internal state
///        - None
/// \~english @par Conditions of processing failure
///       - HANDLE specified in the argument (hApp) is NULL [eFrameworkunifiedStatusInvldHandle]
///       - HANDLE specified in the argument (hApp) is not appropriate (which is invalid) [eFrameworkunifiedStatusInvldHandle]
///       - Pointer(pNotification) to service name is NULL [eFrameworkunifiedStatusInvldHandle]
/// \~english @par Detail
///        send message to Notification Service to register a notification
/// \~english @par Classification
///        Public
/// \~english @par Type
///        sync only
/// \~english @see
///
/////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPHSMRegisterNotificatsionEvent
/// API to send message to Notification Service to register a notification
///
/// \param [in] hApp
///         HANDLE - Handle to the Framework
/// \param [in] pNotification
///         PCSTR - Name of Notification
/// \param [in] max_length
///         const UI_32 - Max size of the notification message
/// \param [in] persType
///         const EFrameworkunifiedNotificationType - Type of persistent
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPHSMRegisterNotificatsionEvent(HANDLE hApp, PCSTR pNotification, const UI_32 max_length,
                                              const EFrameworkunifiedNotificationType persType);


/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedAttachHSMEventsToDispatcher
/// \~english @par Brief
///        register many notifications in service
/// \~english @param [in] hApp
///        HANDLE hApp - Handle to the Framework
/// \~english @param [in] pList
///        const  FrameworkunifiedNotificationsList *pList - List of the notifications that need to be registered
/// \~english @param [in] uiListLength
///        UI_32 uiListLength - Length of the list
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldHandle invalid state handle
/// \~english @par Prerequisite
///        - None
/// \~english @par Change of internal state
///        - None
/// \~english @par Conditions of processing failure
///       - HANDLE specified in the argument (hApp) is NULL [eFrameworkunifiedStatusInvldHandle]
///       - HANDLE specified in the argument (hApp) is not appropriate (which is invalid) [eFrameworkunifiedStatusInvldHandle]
///       - Pointer(pList) to service name is NULL [eFrameworkunifiedStatusInvldHandle]
/// \~english @par Detail
///        send message to Notification Service to register many notifications
/// \~english @par Classification
///        Public
/// \~english @par Type
///        sync only
/// \~english @see
///
/////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPHSMRegisterNotificationsEvents
/// API to send message to Notification Service to register a notification
///
/// \param [in] hApp
///         HANDLE - Handle to the Framework
/// \param [in] pList
///         FrameworkunifiedNotificationsList - List of the notifications that need to be registered
/// \param [in] uiListLength
///         const UI_32 - Length of the list
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPHSMRegisterNotificationsEvents(HANDLE hApp, const  FrameworkunifiedNotificationsList *pList,
                                               UI_32 uiListLength);


/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedAttachHSMEventsToDispatcher
/// \~english @par Brief
///        remove a notification in service
/// \~english @param [in] hApp
///        HANDLE hApp - Handle to the Framework
/// \~english @param [in] pNotification
///        cPCSTR pNotification - Name of Notification
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldHandle invalid state handle
/// \~english @par Prerequisite
///        - None
/// \~english @par Change of internal state
///        - None
/// \~english @par Conditions of processing failure
///       - HANDLE specified in the argument (hApp) is NULL [eFrameworkunifiedStatusInvldHandle]
///       - HANDLE specified in the argument (hApp) is not appropriate (which is invalid) [eFrameworkunifiedStatusInvldHandle]
///       - Pointer(pNotification) to service name is NULL [eFrameworkunifiedStatusInvldHandle]
/// \~english @par Detail
///        send message to Notification Service to remove a notification
/// \~english @par Classification
///        Public
/// \~english @par Type
///        sync only
/// \~english @see
///
/////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPHSMUnRegisterNotificationEvent
/// API to send message to Notification Service to remove a notification
///
/// \param [in] hApp
///         HANDLE - Handle to the Framework
/// \param [in] pNotification
///         PCSTR - Name of Notification
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPHSMUnRegisterNotificationEvent(HANDLE hApp, PCSTR pNotification);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedAttachHSMEventsToDispatcher
/// \~english @par Brief
///        attach an event to the dispatcher on receiving a specific notification.
/// \~english @param [in] hApp
///        HANDLE hApp - Handle to the Framework
/// \~english @param [in] pNotification
///        cPCSTR pNotification - Name of Notification
/// \~english @param [in] iEventId
///        UI_32 iEventId  - event ID
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldHandle invalid state handle
/// \~english @par Prerequisite
///        - None
/// \~english @par Change of internal state
///        - None
/// \~english @par Conditions of processing failure
///       - HANDLE specified in the argument (hApp) is NULL [eFrameworkunifiedStatusInvldHandle]
///       - HANDLE specified in the argument (hApp) is not appropriate (which is invalid) [eFrameworkunifiedStatusInvldHandle]
///       - Pointer(pNotification) to service name is NULL [eFrameworkunifiedStatusInvldHandle]
/// \~english @par Detail
///        attach an event to the dispatcher on receiving a specific notification.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        sync only
/// \~english @see
///
/////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedAttachNotificationEventToDispatcher
/// API to attach a event to the dispatcher on receiving a specific notification.
///
/// \param [in] hApp
///         HANDLE - Application framework handle
/// \param [in] pNotification
///         PCSTR - Notification String.
/// \param [in] iEventId
///         UI_32 - event to be posted to active state on receiving iCmd from pServiceName.
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSubscribeNotificationWithHSMEvent(HANDLE hApp, PCSTR pNotification,
                                                UI_32 iEventId);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedAttachHSMEventsToDispatcher
/// \~english @par Brief
///        attach many events to eath dispatcher on receiving a specific notification.
/// \~english @param [in] hApp
///        HANDLE hApp - Handle to the Framework
/// \~english @param [in] pNtfyEvent
///        const FrameworkunifiedNotificationEvent *pNtfyEvent - Notification event structure.
/// \~english @param [in] uiEventCount
///        UI_32 uiEventCount  - Number of Notification event
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldHandle invalid state handle
/// \~english @par Prerequisite
///        - None
/// \~english @par Change of internal state
///        - None
/// \~english @par Conditions of processing failure
///       - HANDLE specified in the argument (hApp) is NULL [eFrameworkunifiedStatusInvldHandle]
///       - HANDLE specified in the argument (hApp) is not appropriate (which is invalid) [eFrameworkunifiedStatusInvldHandle]
///       - Pointer to FrameworkunifiedProtocolEvent structure specified in the argument is NULL [eFrameworkunifiedStatusInvldHandle]
/// \~english @par Detail
///        attach many events to eath dispatcher on receiving a specific notification.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        sync only
/// \~english @see
///
/////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedAttachNotificationEventsToDispatcher
/// API to attach a event to the dispatcher on receiving a specific notification.
///
/// \param [in] hApp
///         HANDLE - Application framework handle
/// \param [in] pNtfyEvent
///         const FrameworkunifiedNotificationEvent* - Notification event structure.
/// \param [in] uiEventCount
///         UI_32 - Number of Notification event
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSubscribeNotificationsWithHSMEvent(HANDLE hApp,  const FrameworkunifiedNotificationEvent *pNtfyEvent,
                                                 UI_32 uiEventCount);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedAttachHSMEventsToDispatcher
/// \~english @par Brief
///        detach events from the dispatcher.
/// \~english @param [in] hApp
///        HANDLE hApp - Handle to the Framework
/// \~english @param [in] pNtfyHandler
///        const FrameworkunifiedNotificationEvent *pNtfyHandler - Notification handler structure.
/// \~english @param [in] uiHandlerCount
///        UI_32 uiHandlerCount - Notification handler structure.
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldHandle invalid state handle
/// \~english @par Prerequisite
///        - None
/// \~english @par Change of internal state
///        - None
/// \~english @par Conditions of processing failure
///       - HANDLE specified in the argument (hApp) is NULL [eFrameworkunifiedStatusInvldHandle]
///       - HANDLE specified in the argument (hApp) is not appropriate (which is invalid) [eFrameworkunifiedStatusInvldHandle]
///       - Pointer to FrameworkunifiedProtocolEvent structure specified in the argument is NULL [eFrameworkunifiedStatusInvldHandle]
/// \~english @par Detail
///        detach events from the dispatcher.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        sync only
/// \~english @see
///
/////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedUnsubscribeNotificationsWithHSMEvent
/// API to detach a notification event from the dispatcher.
///
/// \param [in] hApp
///         HANDLE - Application framework handle
/// \param [in] pNtfyHandler
///         const FrameworkunifiedNotificationEvent* - Notification handler structure.
/// \param [in] uiHandlerCount
///         UI_32 - Number of notification handlers
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedUnsubscribeNotificationsWithHSMEvent(HANDLE hApp, const FrameworkunifiedNotificationEvent *pNtfyHandler,
                                                   UI_32 uiHandlerCount);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedAttachHSMEventsToDispatcher
/// \~english @par Brief
///        detach a notification event from the dispatcher.
/// \~english @param [in] hApp
///        HANDLE hApp - Handle to the Framework
/// \~english @param [in] pNotification
///        PCSTR pNotification  - Notification String.
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldHandle invalid state handle
/// \~english @par Prerequisite
///        - None
/// \~english @par Change of internal state
///        - None
/// \~english @par Conditions of processing failure
///       - HANDLE specified in the argument (hApp) is NULL [eFrameworkunifiedStatusInvldHandle]
///       - HANDLE specified in the argument (hApp) is not appropriate (which is invalid) [eFrameworkunifiedStatusInvldHandle]
///       - Pointer(pNotification) to service name is NULL [eFrameworkunifiedStatusInvldHandle]
/// \~english @par Detail
///        detach a notification event from the dispatcher.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        sync only
/// \~english @see
///
/////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedDetachNotificationEventsFromDispatcher
/// API to detach a notification event from the dispatcher.
///
/// \param [in] hApp
///         HANDLE - Application framework handle
/// \param [in] pNotification
///         PCSTR - Notification String.
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedUnsubscribeNotificationWithHSMEvent(HANDLE hApp, PCSTR pNotification);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedAttachHSMEventsToDispatcher
/// \~english @par Brief
///        Registers a multiple HSM event with the dispatcher for a given service.
/// \~english @param [in] hApp
///        HANDLE hApp - Child Thread handle
/// \~english @param [in] pEventIds
///        const FrameworkunifiedProtocolEvent *pEventIds  - structure mapping event ids with the messages
/// \~english @param [in] uiEventCount
///        UI_32 uiEventCount  - no of events
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldHandle invalid state handle
/// \~english @par Prerequisite
///        - None
/// \~english @par Change of internal state
///        - None
/// \~english @par Conditions of processing failure
///       - HANDLE specified in the argument (hApp) is NULL [eFrameworkunifiedStatusInvldHandle]
///       - HANDLE specified in the argument (hApp) is not appropriate (which is invalid) [eFrameworkunifiedStatusInvldHandle]
///       - Pointer to FrameworkunifiedProtocolEvent structure specified in the argument is NULL [eFrameworkunifiedStatusInvldHandle]
/// \~english @par Detail
///        Registers a multiple HSM event with the dispatcher for a given service.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        sync only
/// \~english @see
///
/////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedAttachParentHSMEventsToDispatcher
/// Registers a multiple HSM event with the dispatcher for a given service.
///
///
/// \param [in] hChildApp
///         HANDLE - Application framework handle
/// \param [in] pEventIds
///         const FrameworkunifiedProtocolEvent* - structure mapping event ids with the messages
/// \param [in] uiEventCount
///         UI_32 - no of events
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedAttachParentHSMEventsToDispatcher(HANDLE hChildApp, const FrameworkunifiedProtocolEvent *pEventIds,
                                                UI_32 uiEventCount);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedAttachHSMEventsToDispatcher
/// \~english @par Brief
///        detach notification HSM events from the dispatcher.
/// \~english @param [in] hApp
///        HANDLE hApp - Child Thread handle
/// \~english @param [in] puiEventArray
///        const PUI_32 puiEventArray  - Array of events
/// \~english @param [in] uiEventCount
///        UI_32 uiEventCount  - Number of events
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldHandle invalid state handle
/// \~english @par Prerequisite
///        - None
/// \~english @par Change of internal state
///        - None
/// \~english @par Conditions of processing failure
///       - HANDLE specified in the argument (hApp) is NULL [eFrameworkunifiedStatusInvldHandle]
///       - HANDLE specified in the argument (hApp) is not appropriate (which is invalid) [eFrameworkunifiedStatusInvldHandle]
/// \~english @par Detail
///        detach notification HSM events from the dispatcher.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        sync only
/// \~english @see
///
/////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedDetachParentHSMEventsFromDispatcher
/// API to detach a notification HSM event from the dispatcher.
///
///
/// \param [in] hChildApp
///         HANDLE - Child Thread handle
/// \param [in] puiEventArray
///         const PUI_32 puiEventArray - Array of events
/// \param [in] uiEventCount
///         UI_32 - Number of events
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedDetachParentHSMEventsFromDispatcher(HANDLE hChildApp, const PUI_32 puiEventArray, UI_32 uiEventCount);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedDetachHSMEventsFromDispatcher
/// \~english @par Brief
///        detach multiple events from the dispatcher
/// \~english @param [in] hApp
///        HANDLE hApp - Application framework handle
/// \~english @param [in] pServiceName
///        PCSTR pServiceName  - Service Name
/// \~english @param [in] puiCmdArray
///        const PUI_32 puiCmdArray  - Array of commands
/// \~english @param [in] uiCmdCount
///        UI_32 uiCmdCount  - Number of Commands
/// \~english @param [in] hSession
///        HANDLE hSession  - Session Handle.
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldHandle invalid state handle
/// \~english @retval eFrameworkunifiedStatusFail process failed
/// \~english @retval eFrameworkunifiedStatusInvldParam invalid parameter
/// \~english @par Prerequisite
///        - Generation/Initialization of Dispatcher for the Application by FrameworkunifiedHSMDispatcherWithArguments has been done.
/// \~english @par Change of internal state
///        - None
/// \~english @par Conditions of processing failure
///       - HANDLE specified in the argument (hApp) is NULL [eFrameworkunifiedStatusInvldHandle]
///       - HANDLE specified in the argument (hApp) is not appropriate (which is invalid) [eFrameworkunifiedStatusInvldHandle]
///       - Pointer to service name specified in the argument (pServiceName) is NULL [eFrameworkunifiedStatusInvldHandle]
///       - Pointer(pServiceName) to service name doesn't exist [eFrameworkunifiedStatusFail]
///       - Pointer to event array specified in the argument which will be deleted is NULL [eFrameworkunifiedStatusInvldParam]
/// \~english @par Detail
///        Detach multiple events(puiCmdArray) information for service specified in the argument from dispatcher.\n
///        After that, when receiving command from service, post the event to unactive state.\n
///        If the information has been deleted, return eFrameworkunifiedStatusOK.\n
///        If the specific event has not been registered, return eFrameworkunifiedStatusOK.\n
///        If number of puiCmdArray to be deleted is 0, return eFrameworkunifiedStatusOK.\n
///        When deleting multiple events, if an error occurs, stop deleting. \n
/// \~english @par
///        Please note the following points when using this API for application.
///        - Operation is not guaranteed if a different handle is set for an application handle.
///        - Operation is not guaranteed if an invalid handle is set for a session handle.
///        - Operatuib is not guaranteed if a puiCmdArray that differs from the events to be deleted is set.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        sync only
/// \~english @see  FrameworkunifiedDetachHSMEventFromDispatcher
///
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedDetachHSMEventsFromDispatcher
/// API to detach a notification HSM event from the dispatcher.
///
///
/// \param [in] hApp
///         HANDLE - Application framework handle
/// \param [in] pServiceName
///         PCSTR - Service Name
/// \param [in] puiCmdArray
///         const PUI_32 - Array of commands
/// \param [in] uiCmdCount
///         UI_32 - Number of Commands
/// \param [in] hSession
///         HANDLE - Session Handle.
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see
///
EFrameworkunifiedStatus FrameworkunifiedDetachHSMEventsFromDispatcher(HANDLE hApp, PCSTR pServiceName, const PUI_32 puiCmdArray,
                                            UI_32 uiCmdCount, HANDLE hSession = NULL);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedDetachHSMEventFromDispatcher
/// \~english @par Brief
///        detach an event from the dispatcher
/// \~english @param [in] hApp
///        HANDLE hApp - Application framework handle
/// \~english @param [in] pServiceName
///        PCSTR pServiceName  - Service Name
/// \~english @param [in] iCmd
///        UI_32 iCmd  - service protocol command/request/acknowledgment ID
/// \~english @param [in] hSession
///        HANDLE hSession  - Session Handle.
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldHandle invalid state handle
/// \~english @retval eFrameworkunifiedStatusFail process failed
/// \~english @par Prerequisite
///        - Generation/Initialization of Dispatcher for the Application by FrameworkunifiedHSMDispatcherWithArguments has been done.
/// \~english @par Change of internal state
///        - None
/// \~english @par Conditions of processing failure
///       - HANDLE specified in the argument (hApp) is NULL [eFrameworkunifiedStatusInvldHandle]
///       - HANDLE specified in the argument (hApp) is not appropriate (which is invalid) [eFrameworkunifiedStatusInvldHandle]
///       - Pointer to service name specified in the argument (pServiceName) is NULL [eFrameworkunifiedStatusInvldHandle]
///       - Pointer(pServiceName) to service name doesn't exist [eFrameworkunifiedStatusFail]
/// \~english @par Detail
///        Detach an event information for service specified in the argument from dispatcher.\n
///        After that, when receiving command from service, post the event to unactive state.\n
///        If the information has been deleted or the specific iCmd has not been registered, return eFrameworkunifiedStatusOK.\n
/// \~english @par
///        Please note the following points when using this API for application.
///        - Operation is not guaranteed if a different handle is set for an application handle.
///        - Operation is not guaranteed if an invalid handle is set for a session handle.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        sync only
/// \~english @see  FrameworkunifiedDetachHSMEventsFromDispatcher
///
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedDetachHSMEventFromDispatcher
/// API to detach a notification HSM event from the dispatcher.
///
///
/// \param [in] hApp
///         HANDLE - Application framework handle
/// \param [in] pServiceName
///         PCSTR - Service Name
/// \param [in] iCmd
///         UI_32 - service protocol command/request/acknowledgment ID
/// \param [in] hSession
///         HANDLE - Session Handle.
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see
///
EFrameworkunifiedStatus FrameworkunifiedDetachHSMEventFromDispatcher(HANDLE hApp, PCSTR pServiceName, UI_32 iCmd, HANDLE hSession = NULL);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedGetStateMachine
/// \~english @par Brief
///        Get pointer to the statemachine object.
/// \~english @param [in] hApp
///        HANDLE   - Handle to the framework application
/// \~english @retval CFrameworkunifiedHSMParentFramework* pointer to the statemachine object
/// \~english @retval NULL Failed to get pointer to the statemachine object
/// \~english @par Prerequisite
///       - Generation/Initialization of Dispatcher for the Application by FrameworkunifiedHSMDispatcherWithArguments has been done.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - HANDLE specified in the argument (hApp) is not appropriate (which is invalid) . [NULL]
///       - HANDLE specified in the argument (hApp) is not generated by FrameworkunifiedHSMDispatcherWithArguments. [NULL]
/// \~english @par Detail
///        Get pointer to the statemachine object associated with application framework handle.\n
///        Do not release the statemachine object in application, because it is a part of the Dispathcer.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        sync only
/// \~english @see FrameworkunifiedHSMDispatcherWithArguments,FrameworkunifiedSetStateMachine
////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedGetStateMachine
///   returns the pointer to the statemachine object
///
/// \param [in] hApp
///         HANDLE - Handle to the application session
///
/// \return pointer to the StateMachine object
///         CFrameworkunifiedHSMParentFramework* - pointer to the StateMachine object
///
/// \see
///
CFrameworkunifiedHSMFramework *FrameworkunifiedGetStateMachine(HANDLE hApp);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedDetachHSMEventFromDispatcher
/// \~english @par Brief
///        subscribing to single event of a service. attaches the session event with HSM events.
/// \~english @param [in] hApp
///        HANDLE hApp - Application handle
/// \~english @param [in] uiEventId
///        UI_32 uiEventId  - Session Event id
/// \~english @param [in] uiHSMEventId
///        UI_32 uiHSMEventId- Statemachine Event id
/// \~english @param [in] hSession
///        HANDLE hSession  - Session Handle.
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldHandle invalid state handle
/// \~english @retval eFrameworkunifiedStatusFail process failed
/// \~english @par Prerequisite
///        - None
/// \~english @par Change of internal state
///        - None
/// \~english @par Conditions of processing failure
///       - HANDLE specified in the argument (hApp) is NULL [eFrameworkunifiedStatusInvldHandle]
///       - HANDLE specified in the argument (hApp) is not appropriate (which is invalid) [eFrameworkunifiedStatusInvldHandle]
/// \~english @par Detail
///        This API is used for subscribing to single event of a service.
///        This API also attaches the session event with HSM events.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        sync only
/// \~english @see
///
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedSubscribeToSessionEventWithHSMEvent
/// This API is used for subscribing to single event of a service.
/// This API also attaches the session event with HSM events.
///
/// \param [in] hApp
///         HANDLE - Application handle
/// \param [in] uiEventId
///     UI_32 - Session Event id
/// \param [in] uiHSMEventId
///     UI_32 - Statemachine Event id
/// \param [in] hSession
///     HANDLE - Session handle
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see FrameworkunifiedSubscribeToSessionEventsWithHSMEvents
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSubscribeToSessionEventWithHSMEvent(HANDLE hApp, UI_32 uiEventId, UI_32 uiHSMEventId, HANDLE hSession);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedDetachHSMEventFromDispatcher
/// \~english @par Brief
///        subscribing to multiple events of a service. attaches the session events with HSM events.
/// \~english @param [in] hApp
///        HANDLE hApp - Application handle
/// \~english @param [in] pEventIds
///        const FrameworkunifiedProtocolEvent *pEventIds  - structure mapping session event id's with the hsm event id's
/// \~english @param [in] uiEventCount
///        UI_32 uiEventCount  - count of events
/// \~english @param [in] hSession
///        HANDLE hSession  - Session Handle.
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldHandle invalid state handle
/// \~english @retval eFrameworkunifiedStatusFail process failed
/// \~english @par Prerequisite
///        - None
/// \~english @par Change of internal state
///        - None
/// \~english @par Conditions of processing failure
///       - HANDLE specified in the argument (hApp) is NULL [eFrameworkunifiedStatusInvldHandle]
///       - HANDLE specified in the argument (hApp) is not appropriate (which is invalid) [eFrameworkunifiedStatusInvldHandle]
///       - Pointer to FrameworkunifiedProtocolEvent structure specified in the argument is NULL [eFrameworkunifiedStatusInvldHandle]
/// \~english @par Detail
///        This API is used for subscribing to multiple events of a service.
///        his API also attaches the session events with hsm events.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        sync only
/// \~english @see
///
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedSubscribeToSessionEventsWithHSMEvents
/// This API is used for subscribing to multiple events of a service.
/// This API also attaches the session events with hsm events.
///
/// \param [in] hApp
///         HANDLE - Application handle
/// \param [in] pEventIds
///         FrameworkunifiedProtocolEvent - structure mapping session event id's with the hsm event id's
/// \param [in] uiEventCount
///     UI_32 - count of events
/// \param [in] hSession
///     HANDLE - Session handle
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see FrameworkunifiedSubscribeToSessionEventWithHSMEvent
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSubscribeToSessionEventsWithHSMEvents(HANDLE hApp, const FrameworkunifiedProtocolEvent *pEventIds, UI_32 uiEventCount,
                                                    HANDLE hSession);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedDetachHSMEventFromDispatcher
/// \~english @par Brief
///        unsubscribe from event of a service. detaches HSM event from session event.
/// \~english @param [in] hApp
///        HANDLE hApp - Application handle
/// \~english @param [in] uiEventId
///        UI_32 uiEventId  - Event id
/// \~english @param [in] hSession
///        HANDLE hSession  - Session Handle.
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldHandle invalid state handle
/// \~english @retval eFrameworkunifiedStatusFail process failed
/// \~english @par Prerequisite
///        - None
/// \~english @par Change of internal state
///        - None
/// \~english @par Conditions of processing failure
///       - HANDLE specified in the argument (hApp) is NULL [eFrameworkunifiedStatusInvldHandle]
///       - HANDLE specified in the argument (hApp) is not appropriate (which is invalid) [eFrameworkunifiedStatusInvldHandle]
/// \~english @par Detail
///        API to unsubscribe from event of a service. Also detaches HSM event from session event.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        sync only
/// \~english @see
///
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedUnSubscribeSessionEventWithHSMEvent
/// API to unsubscribe from event of a service. Also detaches HSM event from session event.
///
/// \param [in] hApp
///         HANDLE - Application Handle
/// \param [in] uiEventId
///     UI_32 - Event id
/// \param [in] hSession
///         HANDLE - Session Handle
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see FrameworkunifiedUnSubscribeSessionEventsWithHSMEvents
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedUnSubscribeSessionEventWithHSMEvent(HANDLE hApp, UI_32 uiEventId, HANDLE hSession);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedDetachHSMEventFromDispatcher
/// \~english @par Brief
///        unsubscribe from multiple events of a service. detaches HSM events from session events.
/// \~english @param [in] hApp
///        HANDLE hApp - Application handle
/// \~english @param [in] pEventsArray
///        PUI_32 pEventsArray  - Array of events id's.
/// \~english @param [in] uiListSize
///        UI_32 uiListSize  - count of elements in array.
/// \~english @param [in] hSession
///        HANDLE hSession  - Session Handle.
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldHandle invalid state handle
/// \~english @retval eFrameworkunifiedStatusFail process failed
/// \~english @par Prerequisite
///        - None
/// \~english @par Change of internal state
///        - None
/// \~english @par Conditions of processing failure
///       - HANDLE specified in the argument (hApp) is NULL [eFrameworkunifiedStatusInvldHandle]
///       - HANDLE specified in the argument (hApp) is not appropriate (which is invalid) [eFrameworkunifiedStatusInvldHandle]
/// \~english @par Detail
///        API to unsubscribe from multiple events of a service. Also detaches HSM events from session events.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        sync only
/// \~english @see
///
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedUnSubscribeSessionEventsWithHSMEvents
/// API to unsubscribe from multiple events of a service. Also detaches HSM events from session events.
///
/// \param [in] hApp
///         HANDLE - Application Handle
/// \param [in] pEventsArray
///     PUI_32 - Array of events id's.
/// \param [in] uiListSize
///     UI_32 - count of elements in array.
/// \param [in] hSession
///         HANDLE - Session Handle
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see FrameworkunifiedUnSubscribeSessionEventWithHSMEvent
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedUnSubscribeSessionEventsWithHSMEvents(HANDLE hApp, PUI_32 pEventsArray, UI_32 uiListSize,
                                                    HANDLE hSession);

#endif /* __FRAMEWORKUNIFIED_NATIVESERVICES_NATIVESERVICES_INC_FRAMEWORK_STATEMACHINE_FRAMEWORKUNIFIED_SM_FRAMEWORK_DISPATCH_H__ */  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
