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
/// \ingroup  tag_NSFramework
/// \brief   Internal functions of the application framework.
///
///
///
///////////////////////////////////////////////////////////////////////////////

#ifndef FRAMEWORK_UNIFIED_CLIENT_NS_FRAMEWORKCORE_INCLUDE_FRAMEWORKUNIFIED_FRAMEWORK_INTERNAL_H_
#define FRAMEWORK_UNIFIED_CLIENT_NS_FRAMEWORKCORE_INCLUDE_FRAMEWORKUNIFIED_FRAMEWORK_INTERNAL_H_

#include <native_service/ns_shared_mem_if.h>
#include <native_service/frameworkunified_types.h>
#include "frameworkunified_framework_core.h"
#define PASA_APPCONFIG_FILES "PASA_APPCONFIG_FILES"

#define FRAMEWORKUNIFIEDLOG_CUT(zone, funcname, print_fmt, args...)

//////////////////////////////////////////////////
// dispatcher internal functions
//////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedAttachSystemCallbacksDispatcher
/// All system defined callbacks will be attached to the dispatcher
///
/// \param [in] hApp
///         HANDLE - Handle to the application framework.
///
/// \return eStatus
///         EFrameworkunifiedStatus - success or error
///
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedAttachSystemCallbacksDispatcher(HANDLE hApp);

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedSendSystemErrMessage
/// This API is used to send application's critical error message to system manager.
///
/// \param [in] hApp
///         HANDLE - Handle to the application framework.
/// \param [in] eSystemError
///         EFrameworkunifiedSystemError - System error type
///
/// \return EFrameworkunifiedStatus - success or error
///
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSendSystemErrMessage(HANDLE hApp, EFrameworkunifiedSystemError eSystemError);

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedCreateDispatcher
/// Creates and initializes an applications/threads dispatcher.
///
/// \param [in] cAppname
///         PCSTR - Application/ thread name
/// \param [out] hApp
///         HANDLE& - Reference of application handle
/// \param [in] bIsChildThread
///         BOOL - child thread
///
/// \return status
///         EFrameworkunifiedStatus - eFrameworkunifiedStatusOK if Success
///            possible errors from call CreateDispatcher
///
/// \see FrameworkunifiedDispatchBlock,
///      FrameworkunifiedDispatchProcess, FrameworkunifiedCloseDispatcher,
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedCreateDispatcher(PCSTR cAppName, HANDLE &hApp, BOOL bIsChildThread);  // NOLINT  (readability/nolint)

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedCreateHSMDispatcher
/// Creates and initializes an applications/threads hsm dispatcher.
///
/// \param [in] cAppname
///         PCSTR - Application/ thread name
/// \param [out] hApp
///         HANDLE& - Reference of application handle
/// \param [in] bIsThread
///         BOOL - child thread
/// \param [in] f_pFrameworkunifiedHSM
///         CFrameworkunifiedHSMFramework* - statemachine instances
///
/// \return status
///         EFrameworkunifiedStatus - eFrameworkunifiedStatusOK if Success
///            possible errors from call CreateDispatcher
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedCreateHSMDispatcher(PCSTR cAppName,
                                  HANDLE &hApp,  // NOLINT  (readability/nolint)
                                  BOOL bIsThread,
                                  CFrameworkunifiedHSMFramework *f_pFrameworkunifiedHSM = NULL);

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedDispatchBlock
/// Blocked until the dipatcher receives a notification of a service request or ack.
///
/// \param [in] hApp
///         HANDLE - Application framework handle
///
/// \return status
///         EFrameworkunifiedStatus -Success or error
///
/// \see FrameworkunifiedCreateDispatcher,
///      FrameworkunifiedDispatchProcess, FrameworkunifiedCloseDispatcher,
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedDispatchBlock(HANDLE hApp);

////////////////////////////////////////////////////////////////////////////////////////////
/// frameworkunifiedFdHandler
/// FD POLLIN blocked & process.
///
/// \param [in] hApp
///         HANDLE - Application framework handle
/// \param [in] efd
///         int - epoll FD
///
/// \return status
///         EFrameworkunifiedStatus -Success or error
///
/// \see FrameworkunifiedCreateDispatcher, FrameworkunifiedDispatchProcess, FrameworkunifiedDispatchBlock,
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus frameworkunifiedFdHandler(HANDLE hApp , int efd);

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedDispatchProcess
/// Processes the Service/system request or notification.
///
/// \param [in] hApp
///         HANDLE - Application framework handle
///
/// \return status
///         EFrameworkunifiedStatus -Success or error
///
/// \see FrameworkunifiedCreateDispatcher, FrameworkunifiedDispatchBlock
///      FrameworkunifiedCloseDispatcher,
///
/// \details
/// \code
/// Pseudo code:
///   IF message_source != Notification Service
///     FIND source_name
///       IF source_name available
///         FIND protocol_cmd
///           IF protocol_cmd available
///             execute callback
///           ELSE
///             ignore
///           END
///       ELSE
///         ignore
///       END
///   ELSE  // notification
///     FIND notification_callback
///       IF available
///         execute callback
///   END
/// \endcode
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedDispatchProcess(HANDLE hApp);

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedCloseDispatcher
/// Closing the dispatcher
///
/// \param [in] hApp
///         HANDLE - Application framework handle
///
/// \return status
///         EFrameworkunifiedStatus -Success or error
///
/// \see FrameworkunifiedCreateDispatcher, FrameworkunifiedDispatchBlock
///      FrameworkunifiedDispatchProcess.
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedCloseDispatcher(HANDLE hApp);


////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedPopedDeferedMessage
/// Checks the Defer'ed Message Queue should be pop'ed off
/// the front of the queue!
///
/// \param [in] hApp
///         HANDLE - Application framework handle
///
/// \return status
///         BOOL -  TRUE (a message has been pop'ed of the defer'ed msg queue)
///         FALSE (no msg was pop'ed of the defer'ed msg queue)
///
/// \see FrameworkunifiedCreateDispatcher, FrameworkunifiedDispatchBlock
///      FrameworkunifiedDispatchProcess.
///
////////////////////////////////////////////////////////////////////////////////////////////
BOOL FrameworkunifiedPopedDeferedMessage(HANDLE hApp);



////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedDispatcherMain
/// Main processing loop that is shared between FrameworkunifiedDispacther and FrameworkunifiedDispatcherWithCmdLnArguments.
///
/// \param [in] hApp
///         HANDLE - Application framework handle
///
/// \return status
///         BOOL -  TRUE (a message has been pop'ed of the defer'ed msg queue)
///         FALSE (no msg was pop'ed of the defer'ed msg queue)
///
/// \see FrameworkunifiedCreateDispatcher, FrameworkunifiedDispatchBlock
///      FrameworkunifiedDispatchProcess.
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedDispatcherMain(HANDLE hApp);

EFrameworkunifiedStatus FrameworkunifiedHSMDispatcherMain(HANDLE hApp);


////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedDispatcherMain
/// Main processing loop that is shared between FrameworkunifiedDispacther and FrameworkunifiedDispatcherWithCmdLnArguments.
///
/// \param [in] hApp
///         HANDLE - Application framework handle
///
/// \return status
///         BOOL -  TRUE (a message has been pop'ed of the defer'ed msg queue)
///         FALSE (no msg was pop'ed of the defer'ed msg queue)
///
/// \see FrameworkunifiedCreateDispatcher, FrameworkunifiedDispatchBlock
///      FrameworkunifiedDispatchProcess.
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedArgumentParser(HANDLE hApp, int argc, char *argv[], CustomCommandLineOptions *cmdLineOptions);


EFrameworkunifiedStatus FrameworkunifiedOnInitializationInternal(HANDLE hApp);
EFrameworkunifiedStatus FrameworkunifiedOnDestroyInternal(HANDLE hApp);
EFrameworkunifiedStatus FrameworkunifiedOnStartInternal(HANDLE hApp);
EFrameworkunifiedStatus FrameworkunifiedOnStopInternal(HANDLE hApp);
EFrameworkunifiedStatus FrameworkunifiedOnPreStartInternal(HANDLE hApp);
EFrameworkunifiedStatus FrameworkunifiedOnPreStopInternal(HANDLE hApp);
EFrameworkunifiedStatus FrameworkunifiedOnBackgroundStartInternal(HANDLE hApp);
EFrameworkunifiedStatus FrameworkunifiedOnBackgroundStopInternal(HANDLE hApp);
EFrameworkunifiedStatus FrameworkunifiedOnDebugDumpInternal(HANDLE hApp);
EFrameworkunifiedStatus FrameworkunifiedCreateStateMachineInternal(HANDLE hApp);
EFrameworkunifiedStatus FrameworkunifiedSSFrameworkInterfaceInternal(HANDLE hApp);
EFrameworkunifiedStatus RegistDefaultCbHandler(const FrameworkunifiedDefaultCallbackHandler *CbHandler);


//////////////////////////////////////////////////
// message center internal functions
//////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////
/// frameworkunifiedGetIsTypeOfSync
/// Gets if message type is sync, return TRUE. or not return FALSE
///
/// \param [in] hApp
///         HANDLE - Handle to a Application Framework
///
/// \return status
///         BOOL - TRUE or FALSE
///
/// \see FrameworkunifiedInvokeSync, frameworkunifiedSendSyncResponse
///
////////////////////////////////////////////////////////////////////////////////////////////
BOOL frameworkunifiedGetIsTypeOfSync(HANDLE hApp);

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedSendSyncResponse
/// Send Sync Response to the hApp.
///
/// \param [in] hApp
///         HANDLE - Handle to the dispatcher to receive message
/// \param [in] seq_id
///         UI_32 - Sync sequence ID
/// \param [in] retValue
///         EFrameworkunifiedStatus - Return Status
/// \param [in] length
///         UI_32 - Length of the data to be receive response.
/// \param [in] data
///         PVOID - Pointer to the response data.
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see FrameworkunifiedInvokeSync
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus frameworkunifiedSendSyncResponse(HANDLE hApp, UI_32 iCmd, UI_32 seq_id, EFrameworkunifiedStatus retValue, UI_32 length, PCVOID data);

//////////////////////////////////////////////////
// logger internal functions
//////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedAttachLoggerCallbacksDispatcher(HANDLE hApp);

#ifdef DISPATCHER_PROFILER
////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedAttachMsgProfilerCallbacksDispatcher
/// This function attaches callback function for profiler protocol for main thread.
/// Protocol is attached considering the source as "nsprofilerutil"
///
/// \param [in] hApp
///         HANDLE - Handle to a Application Framework
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedAttachMsgProfilerCallbacksDispatcher(HANDLE hApp);

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedAttachMsgProfilerCallbacksDispatcher
/// This function attaches callback function for profiler protocol for child threads.
/// Protocol is attached considering the source as parent thread
///
/// \param [in] hApp
///         HANDLE - Handle to a Application Framework
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedAttachChildMsgProfilerCallbacksDispatcher(HANDLE hApp);
#endif

EFrameworkunifiedStatus FrameworkunifiedSetLogMask(HANDLE hApp);
EFrameworkunifiedStatus FrameworkunifiedGetLogMask(HANDLE hApp);
EFrameworkunifiedStatus FrameworkunifiedSetOutputLogOpts(HANDLE hApp);
EFrameworkunifiedStatus FrameworkunifiedGetOutputLogOpts(HANDLE hApp);
EFrameworkunifiedStatus FrameworkunifiedSetLogSeverity(HANDLE hApp);
EFrameworkunifiedStatus FrameworkunifiedGetLogSeverity(HANDLE hApp);
EFrameworkunifiedStatus FrameworkunifiedDebugDumpRequest(HANDLE hApp);

EFrameworkunifiedStatus NSRcsSetLogSettings(HANDLE hApp);
EFrameworkunifiedStatus NSRcsGetLogSettings(HANDLE hApp);
EFrameworkunifiedStatus NSRcsSetLogMask(HANDLE hApp);
EFrameworkunifiedStatus NSRcsSetOutputLogOpts(HANDLE hApp);
EFrameworkunifiedStatus NSRcsSetLogSeverity(HANDLE hApp);

////////////////////////////////////////////////////////////////////////////////////////////
/// SetConfigHandle
/// This Function opens the configuration file from given path and sets the file handle in hApp
///
/// \param [in] cAppname
///         HANDLE - Application handle
/// \param [in] sFilePath
///         PCHAR - File path of the config file
///
/// \return status of the operation
///
/// \see SetDefaultFilePath,
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus SetConfigHandle(HANDLE hApp, PCHAR sFilePath);

////////////////////////////////////////////////////////////////////////////////////////////
/// SetDefaultFilePath
/// This Function fetches the default config path, opens the configuration file and sets the file
/// handle in the application
///
/// \param [in] cAppname
///         HANDLE - Application handle
/// \param [in] sFilePath
///         PCHAR - File path of the config file
///
/// \return status of the operation
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus SetDefaultConfigFile(HANDLE hApp);

EFrameworkunifiedStatus FrameworkunifiedProcessServiceProtocolHSMEvent(HANDLE hApp, PCSTR pRequester);

////////////////////////////////////////////////////////////////////////////////////////////
/// ProcessProtocolEvents
/// This Function registers the events of server with the client.
/// If the event is published previously and is a state event then send the event data to the client.
///
/// \param [in] hApp
///         HANDLE - Application handle
///
/// \return status of the operation
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus ProcessProtocolEvents(HANDLE hApp);

////////////////////////////////////////////////////////////////////////////////////////////
/// ProcessPrivateEvents
/// This Function registers the events of server with the client.
/// If the event is published previously and is a state event then send the event data to the client.
///
/// \param [in] hApp
///         HANDLE - Application handle
///
/// \return status of the operation
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus ProcessPrivateEvents(HANDLE hApp);

////////////////////////////////////////////////////////////////////////////////////////////
/// CleanAllEventsOfSession
/// This Function closes the session handle and removes its entry from the table of
/// ServiceSessionHandleTable of application framework handle.
///
/// \param [in] hApp
///         HANDLE - Application handle
/// \param [in] serviceName
///         PCSTR - Name of a service
/// \param [in] sessionId
///         UI_32 - Session id
///
/// \return status of the operation
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CleanAllEventsOfSession(HANDLE hApp, PCSTR serviceName, UI_32 sessionId);

////////////////////////////////////////////////////////////////////////////////////////////
/// RemoveEntryFromSessionEventInfoTable
/// This Function removes the entry of event id from the session table based on the service
/// name and session id.
///
/// \param [in] hApp
///         HANDLE - Application handle
/// \param [in] serviceName
///         PCSTR - Name of service
/// \param [in] sessionId
///         UI_32 - Session id
/// \param [in] eventId
///         UI_32 - Event id which needs to be removed from internal table
///
/// \return eFrameworkunifiedStatusOK- Success, or eFrameworkunifiedStatusFail
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus RemoveEntryFromSessionEventInfoTable(HANDLE hApp, PCSTR serviceName, UI_32 sessionId, UI_32 eventId);

////////////////////////////////////////////////////////////////////////////////////////////
/// HandleSessionEventOnCloseSession
/// This Function does the handling of session events on close session request
///
/// \param [in] hApp
///         HANDLE - Application handle
///
/// \return status of the operation
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus HandleSessionEventOnCloseSession(HANDLE hApp);

////////////////////////////////////////////////////////////////////////////////////////////
/// UnregisterPrivateEvents
/// This Function unregisters the private event.
///
/// \param [in] hApp
///         HANDLE - Application handle
///
/// \return status of the operation
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus UnregisterPrivateEvents(HANDLE hApp);

////////////////////////////////////////////////////////////////////////////////////////////
/// GetMsgQueueHandle
/// Gets the message queue handle from servicesessionhandletable map.
///
/// \param [in] hApp
///         HANDLE - Application handle
/// \param [in] serviceName
///         PCSTR - Service name of receiver
/// \param [in] uiSessionId
///         UI_32 - session id
///
/// \return Message queue handle
///
////////////////////////////////////////////////////////////////////////////////////////////
HANDLE GetMsgQueueHandle(HANDLE hApp, PCSTR serviceName, const UI_32 uiSessionId);

////////////////////////////////////////////////////////////////////////////////////////////
/// GetNewMsgQueueHandle
/// This function creates and returns the message queue handle.
///
/// \param [in] hApp
///         HANDLE - Application handle
/// \param [in] serviceName
///         PCSTR - Service name of receiver
/// \param [in] uiSessionId
///         UI_32 - session id
/// \param [in] uiEventId
///         const UI_32 - event id
///
/// \return Message queue handle
///
////////////////////////////////////////////////////////////////////////////////////////////
HANDLE GetNewHandleAndUpdateEventInfoTable(HANDLE hApp, PCSTR serviceName, const UI_32 uiSessionId,
                                           const UI_32 uiEventId);

////////////////////////////////////////////////////////////////////////////////////////////
/// IsEventAlreadyRegistered
/// Searches for the session id in the vector
///
/// \param [in] vSessionIdList
///         SessionIdList - list of sessions id's
/// \param [in] uiSessionId
///         UI_32 - session id
///
/// \return true or false
///
////////////////////////////////////////////////////////////////////////////////////////////
BOOL IsEventAlreadyRegistered(const SessionIdList &vSessionIdList, const UI_32 uiSessionId);

////////////////////////////////////////////////////////////////////////////////////////////
/// RemoveEventEntryFromEventTable
/// This Function removes the entry of event for received service on the active session id
/// from Event Table.
///
/// \param [in] hApp
///         HANDLE - Application handle
/// \param [in] uiEventId
///         UI_32 - event id
/// \param [in] serviceName
///         PCSTR - Service name of receiver
/// \param [in] uiSessionId
///         UI_32 - session id
///
/// \return success or failure
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus RemoveEventEntryFromEventTable(HANDLE hApp, UI_32 uiEventId, PCSTR cServiceName, const UI_32 uiSessionId);

////////////////////////////////////////////////////////////////////////////////////////////
/// SendEventMessage
/// This function sends the private event message to all the sessions of service registered
/// for event.
///
/// \param [in] hApp
///         HANDLE - Application handle
/// \param [in] vSessionIdList
///         SessionIdList - session id list
/// \param [in] uiEventId
///         UI_32 - event id
/// \param [in] cServiceName
///         PCSTR - Service name of receiver
/// \param [in] pData
///         PCVOID - data to send
/// \param [in] uiLength
///         UI_32 - length of data
///
/// \return success or failure
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus SendEventMessage(HANDLE hApp, const SessionIdList &vSessionIdList, UI_32 uiEventId, PCSTR cServiceName,
                            PCVOID pData, UI_32 uiLength);

////////////////////////////////////////////////////////////////////////////////////////////
/// UpdatePublicStateEventData
/// This API updates the event data of public state event for all the session of
/// all subscribed clients.
///
/// \param [in] hApp
///         HANDLE - Application handle
/// \param [in] uiEventId
///         UI_32 - event id
/// \param [in] pData
///         PCVOID - data to send
/// \param [in] uiLength
///         UI_32 - length of data
///
/// \return success or failure
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus UpdatePublicStateEventData(HANDLE hApp, UI_32 uiEventId, PCVOID pData, UI_32 uiLength);

////////////////////////////////////////////////////////////////////////////////////////////
/// UpdatePrivateStateEventData
/// This API updates the event data of private state event for session with
/// session id uiSessionId of client pClientName.
///
/// \param [in] hApp
///         HANDLE - Application handle
/// \param [in] uiEventId
///         UI_32 - event id
/// \param [in] pClientName
///         PCSTR - Event Subscriber name
/// \param [in] uiSessionId
///         UI_32 - session id
/// \param [in] pData
///         PCVOID - data to send
/// \param [in] uiLength
///         UI_32 - length of data
///
/// \return success or failure
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus UpdatePrivateStateEventData(HANDLE hApp, UI_32 uiEventId, PCSTR pClientName, UI_32 uiSessionId, PCVOID pData,
                                       UI_32 uiLength);

////////////////////////////////////////////////////////////////////////////////////////////
/// DeleteSessionEventData
/// This function is called while destroying the session with client.
/// This method removes the private event data associated with the session.
///
/// \param [in] hApp
///         HANDLE - Application handle
/// \param [in] pClientName
///         PCSTR - Event Subscriber name
/// \param [in] uiSessionId
///         UI_32 - session id
///
/// \return success or failure
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus DeleteSessionEventData(HANDLE hApp, PCSTR pClientName, UI_32 uiSessionId);

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedGetDataUSID
/// Gets the shared memory ID
///
/// \param [in] hApp
///         HANDLE - Application framework handle
///
/// \return status
///         UI_32 - success or error
///
/// \see FrameworkunifiedMcOpenSender, FrameworkunifiedMcClose, FrameworkunifiedSendMsg, FrameworkunifiedGetMsgLength, FrameworkunifiedGetMsgSrc, FrameworkunifiedGetMsgDataOfSize,
///      FrameworkunifiedClearMsgData, FrameworkunifiedForwardMessage, FrameworkunifiedGetSystemInfo
///
////////////////////////////////////////////////////////////////////////////////////////////
TMemID FrameworkunifiedGetDataUSID(HANDLE hApp);

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedIsAutoPublishServiceAvailableEnabled
/// This function is used to check whether auto publishing of service availability is
/// enabled or not.
///
/// \param None
///
/// \return BOOL
///         BOOL - true if auto publish is enabled for service else false
///
////////////////////////////////////////////////////////////////////////////////////////////
BOOL FrameworkunifiedIsAutoPublishServiceAvailableEnabled();

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedIsWaitInStoppingStateEnabled
/// Check if wait in stopping state is enabled.
///
/// \param [in] none
///
/// \return BOOL - TRUE if enabled or FALSE
///
////////////////////////////////////////////////////////////////////////////////////////////
BOOL FrameworkunifiedIsWaitInStoppingStateEnabled();

#endif  // FRAMEWORK_UNIFIED_CLIENT_NS_FRAMEWORKCORE_INCLUDE_FRAMEWORKUNIFIED_FRAMEWORK_INTERNAL_H_
