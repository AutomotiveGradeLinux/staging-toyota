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
// tag_NSFramework
// Application framework's interface header file.
//
// The interfaces exposed here are
//    - Application framework Dispatcher
//      - attaching/detaching service protocol callbacks
//      - attaching/detaching notification callbacks
//    - Message Center
//      - send messaging to services/clients
//      - read the incoming message information
//    - Service Directory
//      - open and close a service
//      - register/unregister a service
//    - Notification & Persistence Service
//      - register/unregister and publish a notification
//      - subscribe/unsubscribe to a notification
//
/////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file frameworkunified_framework_if.h
 * @brief \~english Application framework's interface header file.
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
 *  @ingroup Native_service
 *  @{
 */
/** @addtogroup framework
 *  @ingroup native_service
 *  @{
 */
#ifndef __FRAMEWORKUNIFIED_NATIVESERVICES_FRAMEWORK_FRAMEWORKUNIFIED_FRAMEWORK_H__  // NOLINT  (build/header_guard)
#define __FRAMEWORKUNIFIED_NATIVESERVICES_FRAMEWORK_FRAMEWORKUNIFIED_FRAMEWORK_H__

#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_framework_types.h>
#include <native_service/frameworkunified_service_protocol.h>
#include <native_service/ns_shared_mem_if.h>
#include <native_service/ns_np_service.h>
///////////////////////////////////
// To avoid integration build break: removing dependency on SM framework.
// #include <native_service/frameworkunified_sm_hsmframework.h>

// QUICK FIX Files added here, which was included by frameworkunified_sm_hsmframework.h
// if we simply comment frameworkunified_sm_hsmframework.h inclusion, then other modules build may
// break, if they are not including below files explicitly and was relying of frameworkunified_framework_if.h.
#include <native_service/ns_timer_if.h>
#include <native_service/ns_message_center_if.h>
#include <native_service/ns_logger_if.h>
#include <map>
#include <string>
#include <vector>
///////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////
//    Dispatcher functions
////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedAttachCallbacksToDispatcher
/// \~english @par Brief
///       API to register multiple Callback-informations to the Dispatcher.
/// \~english @param [in] hApp
///        HANDLE   - HANDLE for Application
/// \~english @param [in] pServiceName
///        PCSTR    - Pointer to the service name
/// \~english @param [in] pMsgHandler
///        FrameworkunifiedProtocolCallbackHandler*    - Pointer to the Message Handler structure
/// \~english @param [in] uiHandlerCount
///        UI_32     - Message Handler count(1 to 4294967295)
/// \~english @param [in] hSession
///        HANDLE    - Session handle (Default:NULL)
/// \~english @par
///        FrameworkunifiedProtocolCallbackHandler Structure
/// \~english @code
///            typedef struct _FrameworkunifiedProtocolCallbackHandler
///            {
///                  /* Command ID/request ID/approval ID  of protocol on the service */
///                  UI_32 iCmd;
///                  /* Pointer to Callback function that would be called on receiving iCmd from pServiceName. */
///                  CbFuncPtr callBack;
///            }FrameworkunifiedProtocolCallbackHandler;
///            @endcode
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @par Prerequisite
///       - Generation/Initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been
///       done.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - HANDLE specified in the argument (hApp) is NULL. [eFrameworkunifiedStatusInvldParam]
///       - HANDLE specified in the argument (hApp) is not appropriate(which is invalid). [eFrameworkunifiedStatusInvldParam]
///       - Pointer(pServiceName) is NULL to the service name specified in the argument. [eFrameworkunifiedStatusInvldParam]
///       - Message Handler number specified in the argument is 0.  [eFrameworkunifiedStatusInvldParam]
///       - NULL Callback function pointer of the Message Handler structure specified by the argument.
///       [eFrameworkunifiedStatusInvldParam]
/// \~english @par Detail
///       Register multiple Callback-informations(iCmd,CbFuncPtr) of the service that has been specified by the argument
///       \n
///       to the Dispatcher.\n
///       Thus, if the iCmd is requested from the service, so the appropriate Callback function is called. \n
///       When multiple registration, if an error occurs in the second and subsequent, becomes effective only \n
///       information that has been registered so far. \n
///       For number of upper limit of the registration possible message handler, depending on \n
///       the securable size as the area of the message handler structure. Therefore, note to ensure \n
///       the size of the area of the message handler structure at the call side of API, and define \n
///       the number of message handler.
/// \~english @par
///        Please note the following points when using this API for application.\n
///       - If the handle (hSession) for the session specified by the arguments is not appropriate (disabled),
///         the system assumes that the session ID = MAX_SESSION_ID_VAL(0xFFFF) is specified.\n
///       - It is not possible to register more than one callback with the same command ID (iCmd) for the same service/session ID,
///         and the callback function will win.
///       - Operatio is not guaranteed if the number of elements of the array indicated by the address of the pointer (pMsgHandler)
///         to the message handler structure does not match the number of arrays specified by the number of
///         party-specified message hadlers (uiHandlerCount).
///       - The callback information is registered to the heap.
/// \~english @par Classification
///          Public
/// \~english @see  FrameworkunifiedAttachCallbackToDispatcher, FrameworkunifiedDetachCallbackFromDispatcher, FrameworkunifiedDetachCallbacksFromDispatcher,
///                 FrameworkunifiedAttachParentCallbacksToDispatcher, FrameworkunifiedDetachParentCallbacksFromDispatcher,
///                 FrameworkunifiedDetachServiceFromDispatcher
////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedAttachCallbacksToDispatcher(HANDLE hApp, PCSTR pServiceName,
                                          const FrameworkunifiedProtocolCallbackHandler *pMsgHandler,
                                          UI_32 uiHandlerCount, HANDLE hSession = NULL);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedAttachCallbackToDispatcher
/// \~english @par Brief
///       API to register a Callback-information to the Dispatcher.
/// \~english @param [in] hApp
///        HANDLE   - Handle for Application
/// \~english @param [in] pServiceName
///        PCSTR    - Pointer to the service name
/// \~english @param [in] iCmd
///        UI_32    - Command ID/request ID/approval ID  of protocol on the service
/// \~english @param [in] fpOnCmd
///        CbFuncPtr  - Pointer to Callback function that would be called on receiving iCmd from pServiceName.
/// \~english @param [in] hSession
///        HANDLE    - Session handle (Default:NULL)
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @par Prerequisite
///       - Generation/Initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been
///       done.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - HANDLE specified in the argument (hApp) is NULL. [eFrameworkunifiedStatusInvldParam]
///       - HANDLE specified in the argument (hApp) is not appropriate(which is invalid). [eFrameworkunifiedStatusInvldParam]
///       - Pointer(pServiceName) is NULL to the service name specified in the argument. [eFrameworkunifiedStatusInvldParam]
///       - Pointer (fpOnCmd) is NULL to the Callback function specified in the argument. [eFrameworkunifiedStatusInvldParam]
/// \~english @par Detail
///       Register a Callback-information(iCmd,CbFuncPtr) of the service that has been specified by the argument \n
///       to the Dispatcher.\n
///       Thus, if the iCmd is requested from the service, so the appropriate Callback function is called.
/// \~english @par
///        Please note the following points when using this API for application.\n
///       - If the handle (hSession) for the session specified by the arguments is not appropriate (disabled),
///         the system assumes that the session ID = MAX_SESSION_ID_VAL(0xFFFF) is specified.
///       - The callback information is registered to the heap.
/// \~english @par Classification
///          Public
/// \~english @see  FrameworkunifiedAttachCallbacksToDispatcher, FrameworkunifiedDetachCallbackFromDispatcher, FrameworkunifiedDetachCallbacksFromDispatcher,
///                 FrameworkunifiedAttachParentCallbacksToDispatcher, FrameworkunifiedDetachParentCallbacksFromDispatcher,
///                 FrameworkunifiedDetachServiceFromDispatcher
////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedAttachCallbackToDispatcher(HANDLE hApp, PCSTR pServiceName, UI_32 iCmd, CbFuncPtr fpOnCmd,
                                         HANDLE hSession = NULL);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedAttachCallbacksToDispatcherWithFd
/// \~english @par Brief
///       API to register multiple Callback and file descriptor set to the Dispatcher.
/// \~english @param [in] hApp
///        HANDLE   - HANDLE for Application
/// \~english @param [in] handlers
///        FrameworkunifiedFdProtocolCallbackHandler*  - Pointer to the Message Handler structure to be registered
/// \~english @param [in] handlerCount
///        UI_32  - Message Handler Count(1 to 4294967295)
/// \~english @par
///        FrameworkunifiedFdProtocolCallbackHandler Structure
/// \~english @code
///            typedef struct _FrameworkunifiedFdProtocolCallbackHandler
///            {
///                  int fd;               /* File descriptor for message waiting control */
///                  CbFuncPtr callBack;   /*  Pointer to Callback function               */
///            }FrameworkunifiedFdProtocolCallbackHandler;
///            @endcode
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
/// \~english @par Prerequisite
///       - Generation/Initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been
///       done.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - HANDLE specified in the argument (hApp) is NULL. [eFrameworkunifiedStatusInvldParam]
///       - HANDLE specified in the argument (hApp) is not appropriate(which is invalid). [eFrameworkunifiedStatusInvldParam]
///       - Pointer (handlers) to the Message Handler structure of the registration object specified in the argument is
///       NULL. [eFrameworkunifiedStatusInvldParam]
///       - Message Handler number of registered specified by the argument is 0. [eFrameworkunifiedStatusInvldParam]
///       - File descriptor in the Message Handler structure(FrameworkunifiedFdProtocolCallbackHandler) specified in the argument is
///       incorrect(If fd is less than or equal to 0). [eFrameworkunifiedStatusInvldParam]
///       - Pointer(fpOnCmd) to the Callback function in the Message Handler structure(FrameworkunifiedFdProtocolCallbackHandler)
///       specified in the argument is NULL. [eFrameworkunifiedStatusInvldParam]
///       - File descriptor specified in the argument is incorrect(If fd is less than or equal to 0).
///       [eFrameworkunifiedStatusInvldParam]
///       - Pointer (fpOnCmd) is NULL to the Callback function specified in the argument. [eFrameworkunifiedStatusInvldParam]
///       - Monitoring file descriptor for epoll managed by the HANDLE(hApp) specified in the argument is not
///       appropriate. [eFrameworkunifiedStatusFail]
///       - File descriptor in the Message Handler structure(FrameworkunifiedFdProtocolCallbackHandler) specified in the argument is
///       already registered to the monitoring control(epoll_ctl). [eFrameworkunifiedStatusFail]
///       - It failed to add the file descriptor of the message waiting control to the monitoring of epoll_ctl().
///       [eFrameworkunifiedStatusFail]
/// \~english @par Detail
///       Register multiple callbacks that link to file descriptor that has been specified by the argument to
///       the Dispatcher.\n
///       For epoll_ctl() of Dispatcher, add multiple file descriptor for the specified message waiting control \n
///       by the argument.\n
///       Thus, for the file descriptor registered in the message waiting control, when receiving a message, \n
///       so the appropriate Callback function is called. \n
///       When multiple registration, if an error occurs in the second and subsequent, becomes effective only \n
///       information that has been registered so far. \n
///       For number of upper limit of the registration possible message handler, depending on \n
///       the securable size as the area of the message handler structure. Therefore, note to ensure \n
///       the size of the area of the message handler structure at the call side of API, and define \n
///       the number of message handler.
/// \~english @par
///        Please note the following points when using this API for application.\n
///       - The operation cannot be guaranteed if the number of elements of the array entity indicated by the address of the pointer (handlers)
///         to the message handler structure to be registered does not match the number of arrays specified in the number of message handlers o be registered (handlerCount).\n
///       - The callback information is registered to the heap.
/// \~english @par Classification
///          Public
/// \~english @see  FrameworkunifiedAttachCallbackToDispatcherWithFd, FrameworkunifiedDetachCallbackFromDispatcherWithFd,
///                  FrameworkunifiedDetachCallbacksFromDispatcherWithFd
////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedAttachCallbacksToDispatcherWithFd(HANDLE hApp, const FrameworkunifiedFdProtocolCallbackHandler *handlers,
                                                UI_32 handlerCount);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedAttachCallbackToDispatcherWithFd
/// \~english @par Brief
///       API to register a callback that link to a single file descriptor to the Dispatcher.
/// \~english @param [in] hApp
///        HANDLE   - HANDLE for Application
/// \~english @param [in] fd
///        int        - File descriptor for message waiting control
/// \~english @param [in] fpOnCmd
///        CbFuncPtr  - Pointer to Callback function
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
/// \~english @par Prerequisite
///       - Generation/Initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been
///       done.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - HANDLE specified in the argument (hApp) is NULL. [eFrameworkunifiedStatusInvldParam]
///       - HANDLE specified in the argument (hApp) is not appropriate(which is invalid). [eFrameworkunifiedStatusInvldParam]
///       - File descriptor specified in the argument is incorrect(If fd is less than or equal to 0).
///       [eFrameworkunifiedStatusInvldParam]
///       - Pointer (fpOnCmd) is NULL to the Callback function specified in the argument. [eFrameworkunifiedStatusInvldParam]
///       - Monitoring file descriptor for epoll managed by the HANDLE(hApp) specified in the argument is not
///       appropriate. [eFrameworkunifiedStatusFail]
///       - File descriptor(fd) specified in the argument is already registered to the monitoring control(epoll_ctl).
///       [eFrameworkunifiedStatusFail]
///       - It failed to add the file descriptor(fd) of the message waiting control to the monitoring of epoll_ctl().
///       [eFrameworkunifiedStatusFail]
/// \~english @par Detail
///       Register a callback that link to a file descriptor that has been specified by the argument to
///       the Dispatcher.\n
///       For epoll_ctl() of Dispatcher, add a file descriptor(fd) for the specified message waiting control \n
///       by the argument.\n
///       Thus, for the file descriptor registered in the message waiting control, when receiving a message, \n
///       so the appropriate Callback function is called.
/// \~english @par
///        Please note the following points when using this API for application.\n
///       - The callback information is registered to the heap.
/// \~english @par Classification
///          Public
/// \~english @see  FrameworkunifiedAttachCallbacksToDispatcherWithFd, FrameworkunifiedDetachCallbackFromDispatcherWithFd,
///                  FrameworkunifiedDetachCallbacksFromDispatcherWithFd
////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedAttachCallbackToDispatcherWithFd(HANDLE hApp, int fd, CbFuncPtr fpOnCmd);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedAttachParentCallbacksToDispatcher
/// \~english @par Brief
///       API to register multiple callback information for the parent thread to the Dispatcher.
/// \~english @param [in] hChildApp
///        HANDLE   - HANDLE for Application
/// \~english @param [in] pMsgHandler
///        FrameworkunifiedProtocolCallbackHandler*    - Pointer to the Message Handler structure
/// \~english @param [in] uiHandlerCount
///        UI_32     - Message Handler count(1 to 4294967295)
/// \~english @par
///        FrameworkunifiedProtocolCallbackHandler Structure
/// \~english @code
///            typedef struct _FrameworkunifiedProtocolCallbackHandler
///            {
///                  /* Command ID/request ID/approval ID  of protocol on the service */
///                  UI_32 iCmd;
///                  /* Pointer to Callback function that would be called on receiving iCmd from pServiceName. */
///                  CbFuncPtr callBack;
///            }FrameworkunifiedProtocolCallbackHandler;
///            @endcode
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @par Prerequisite
///       - Generation/Initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been
///       done.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - HANDLE(hApp) specified in the argument is NULL. [eFrameworkunifiedStatusInvldParam]
///       - HANDLE(hApp) specified in the argument is not appropriate(which is invalid). [eFrameworkunifiedStatusInvldParam]
///       - Parent service name in the HANDLE(hApp) specified in the argument is NULL . [eFrameworkunifiedStatusInvldParam]
///       - Message Handler number specified in the argument is 0.  [eFrameworkunifiedStatusInvldParam]
///       - NULL Callback function pointer of the Message Handler structure specified by the argument.
///       [eFrameworkunifiedStatusInvldParam]
/// \~english @par Detail
///       Register multiple Callback-informations(iCmd,CbFuncPtr) to the Dispatcher to process \n
///       the message from the parent thread.  \n
///       Thus, if the iCmd is requested from the parent service(application), so the appropriate \n
///       Callback function is called. \n
///       When multiple registration, if an error occurs in the second and subsequent, becomes \n
///       effective only information that has been registered so far.\n
///       For number of upper limit of the registration possible message handler, depending on \n
///       the securable size as the area of the message handler structure. Therefore, note to ensure \n
///       the size of the area of the message handler structure at the call side of API, and define \n
///       the number of message handler.
/// \~english @par
///        Please note the following points when using this API for application.\n
///       - Operation is not guaranteed if the number of elements of the array indicated by the address of the pointer (pMsgHandler)
///         to the message handler structure does not match the number of arrays specified by the number of
///         party-specified message hadlers (uiHandlerCount).
///       - The callback information is registered to the heap.
/// \~english @par Classification
///          Public
/// \~english @see  FrameworkunifiedAttachCallbackToDispatcher, FrameworkunifiedAttachCallbackToDispatcher,
///                  FrameworkunifiedDetachCallbackFromDispatcher, FrameworkunifiedDetachCallbacksFromDispatcher,
///                  FrameworkunifiedAttachParentCallbacksToDispatcher, FrameworkunifiedDetachParentCallbacksFromDispatcher,
///                  FrameworkunifiedDetachServiceFromDispatcher
////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedAttachParentCallbacksToDispatcher(HANDLE hChildApp, const FrameworkunifiedProtocolCallbackHandler *pMsgHandler,
                                                UI_32 uiHandlerCount);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedDetachCallbacksFromDispatcher
/// \~english @par Brief
///       API to cancel multiple Callback-informations from the Dispatcher.
/// \~english @param [in] hApp
///        HANDLE   - HANDLE for Application
/// \~english @param [in] pServiceName
///        PCSTR    - Pointer to the service name
/// \~english @param [in] puiCmdArray
///        PUI_32    - Pointer to Command ID/request ID/approval ID  of protocol on the service
/// \~english @param [in] uiCommandCount
///        UI_32    - Cancel handler count
/// \~english @param [in] hSession
///        HANDLE    - Session handle (Default:NULL)
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
/// \~english @par Prerequisite
///       - Generation/Initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been
///       done.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - HANDLE specified in the argument (hApp) is NULL. [eFrameworkunifiedStatusInvldParam]
///       - HANDLE specified in the argument (hApp) is not appropriate(which is invalid). [eFrameworkunifiedStatusInvldParam]
///       - Pointer(pServiceName) is NULL to the service name specified in the argument. [eFrameworkunifiedStatusInvldParam]
///       - Pointer(puiCmdArray) to cancel target ID specified in the argument is NULL. [eFrameworkunifiedStatusInvldParam]
///       - The number(uiCommandCount) of handlers to cancel specified by the argument is 0. [eFrameworkunifiedStatusInvldParam]
///       - Specified service (ServiceName) in the argument is not present. [eFrameworkunifiedStatusFail]
/// \~english @par Detail
///       Cancel a Callback-information(iCmd,CbFuncPtr) of the service that has been specified by the argument \n
///       from the Dispatcher.\n
///       Thus, if the iCmd is requested from the service, so the appropriate Callback function is not called. \n
///       If you have already specified the iCmd of revocation has been subject, to return the eFrameworkunifiedStatusOK.
/// \~english @par
///        Please note the following points when using this API for application.\n
///       - If the iCmad to be canceled is specified, the eFrameworkunifiedStatusOK is returned.
///       - Event if an error occurs during deletion of two or more iCmd, the subsequent iCmd is deleted.
///       - If a iCmd larger than the iCmd array is specified eFrameworkunifiedStatusOK is returned and all iCmd is deleted.\n
///       - If the hade (hSession) for the session specified by the arguments is not appropriate (disabled),
///         the system assumes that the session ID = MAX_SESSION_ID_VAL(0xFFFF) is specified.\n
/// \~english @par Classification
///          Public
/// \~english @see  FrameworkunifiedAttachCallbackToDispatcher, FrameworkunifiedAttachCallbacksToDispatcher, FrameworkunifiedDetachCallbackFromDispatcher,
///                 FrameworkunifiedAttachParentCallbacksToDispatcher, FrameworkunifiedDetachParentCallbacksFromDispatcher,
///                 FrameworkunifiedDetachServiceFromDispatcher
////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedDetachCallbacksFromDispatcher(HANDLE hApp, PCSTR pServiceName, const PUI_32 puiCmdArray,
                                            UI_32 uiCommandCount, HANDLE hSession = NULL);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedDetachCallbackFromDispatcher
/// \~english @par Brief
///       API to cancel a Callback-information from the Dispatcher.
/// \~english @param [in] hApp
///        HANDLE   - HANDLE for Application
/// \~english @param [in] pServiceName
///        PCSTR    - Pointer to the service name
/// \~english @param [in] iCmd
///        UI_32    - Command ID/request ID/approval ID  of protocol on the service
/// \~english @param [in] hSession
///        HANDLE    - Session handle (Default:NULL)
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
/// \~english @par Prerequisite
///       - Generation/Initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been
///       done.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - HANDLE specified in the argument (hApp) is NULL. [eFrameworkunifiedStatusInvldParam]
///       - HANDLE specified in the argument (hApp) is not appropriate(which is invalid). [eFrameworkunifiedStatusInvldParam]
///       - Pointer(pServiceName) is NULL to the service name specified in the argument. [eFrameworkunifiedStatusInvldParam]
///       - Specified service (ServiceName) in the argument is not present. [eFrameworkunifiedStatusFail]
/// \~english @par Detail
///       Cancel a Callback-information(iCmd,CbFuncPtr) of the service that has been specified by the argument \n
///       from the Dispatcher.\n
///       Thus, if the iCmd is requested from the service, so the appropriate Callback function is not called. \n
///       If you have already specified the cancellation has been the subject or unregistered iCmd, to return \n
///       the eFrameworkunifiedStatusOK.
/// \~english @par
///        Please note the following points when using this API for application.\n
///       - If the handle (hSession) for the session specified by the arguments is not appropriate (disabled),
///         the system assumes that the session ID = MAX_SESSION_ID_VAL(0xFFFF) is specified.\n
/// \~english @par Classification
///          Public
/// \~english @see  FrameworkunifiedAttachCallbackToDispatcher, FrameworkunifiedAttachCallbacksToDispatcher, FrameworkunifiedDetachCallbacksFromDispatcher,
///                 FrameworkunifiedAttachParentCallbacksToDispatcher, FrameworkunifiedDetachParentCallbacksFromDispatcher,
///                 FrameworkunifiedDetachServiceFromDispatcher
////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedDetachCallbackFromDispatcher(HANDLE hApp, PCSTR pServiceName, UI_32 iCmd, HANDLE hSession = NULL);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedDetachCallbacksFromDispatcherWithFd
/// \~english @par Brief
///       API to cancel multiple Callback and file descriptor set from the Dispatcher.
/// \~english @param [in] hApp
///        HANDLE   - HANDLE for Application
/// \~english @param [in] fdArray
///        int*  - Pointer to cancel the target file descriptor array
/// \~english @param [in] handlerCount
///        UI_32  - Handler count of cancellation
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
/// \~english @par Prerequisite
///       - Generation/Initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been
///       done.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - HANDLE specified in the argument (hApp) is NULL. [eFrameworkunifiedStatusInvldParam]
///       - HANDLE specified in the argument (hApp) is not appropriate(which is invalid). [eFrameworkunifiedStatusInvldParam]
///       - Pointer(fdArray) is NULL to cancel the target file descriptor array specified in the argument.
///       [eFrameworkunifiedStatusInvldParam]
///       - Handler count(handlerCount) of canceled specified by the argument is 0. [eFrameworkunifiedStatusInvldParam]
///       - File descriptor of the file descriptor in the array(fdArray) specified by the argument is incorrect(If fd is
///       less than or equal to 0). [eFrameworkunifiedStatusInvldParam]
///       - File descriptor in the file descriptor array(fdArray) specified in the argument is already deleted from the
///       management table, and can not search. [eFrameworkunifiedStatusFail]
///       - There is no file descriptor management table of the specified HANDLE(hApp) in argument. [eFrameworkunifiedStatusFail]
///       - Monitoring file descriptor for epoll managed by the HANDLE(hApp) specified in the argument is not
///       appropriate. [eFrameworkunifiedStatusFail]
///       - It failed to delete the file descriptor(fd) of the message waiting control from the monitoring of
///       epoll_ctl(). [eFrameworkunifiedStatusFail]
/// \~english @par Detail
///       Cancel Callback and file descriptor set that has been specified by the argument from the Dispatcher.\n
///       For epoll_ctl() of Dispatcher, delete multiple file descriptor for the specified message waiting control \n
///       by the argument.\n
///       Thus, for the file descriptor became cancellation target, when receiving the message, the appropriate \n
///       Callback function is not called.
/// \~english @par
///        Please note the following points when using this API for application.\n
///       - Operation cannot be guaranteed if the number of elements of the array specified by the address of the pointer
///         to the file descriptor array to be canceled (fdArray) does not match the number of arrays specified by the number of
///         file descriptors to be canceled (uiCommandCount).
/// \~english @par Classification
///          Public
/// \~english @see  FrameworkunifiedAttachCallbacksToDispatcherWithFd, FrameworkunifiedAttachCallbackFromDispatcherWithFd,
///                 FrameworkunifiedDetachCallbackFromDispatcherWithFd
////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedDetachCallbacksFromDispatcherWithFd(HANDLE hApp, const int *fdArray, UI_32 uiCommandCount);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedDetachCallbackFromDispatcherWithFd
/// \~english @par Brief
///       API to cancel a Callback and file descriptor from the Dispatcher.
/// \~english @param [in] hApp
///        HANDLE   - HANDLE for Application
/// \~english @param [in] fd
///        int        - Cancel the target file descriptor
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
/// \~english @par Prerequisite
///       - Generation/Initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been
///       done.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - HANDLE(hApp) specified in the argument is NULL. [eFrameworkunifiedStatusInvldParam]
///       - HANDLE(hApp) specified in the argument is not appropriate(which is invalid). [eFrameworkunifiedStatusInvldParam]
///       - File descriptor(fd) specified in the argument is incorrect(If fd is less than or equal to 0).
///       [eFrameworkunifiedStatusInvldParam]
///       - File descriptor specified in the argument is already deleted from the management table, and can not search.
///       [eFrameworkunifiedStatusFail]
///       - There is no file descriptor management table of the specified HANDLE(hApp) in argument. [eFrameworkunifiedStatusFail]
///       - Monitoring file descriptor for epoll managed by the HANDLE(hApp) specified in the argument is not
///       appropriate. [eFrameworkunifiedStatusFail]
///       - It failed to delete the file descriptor(fd) of the message waiting control from the monitoring of
///       epoll_ctl(). [eFrameworkunifiedStatusFail]
/// \~english @par Detail
///       Cancel a Callback and file descriptor that has been specified by the argument from the Dispatcher.\n
///       For epoll_ctl() of Dispatcher, delete a file descriptor(fd) for the specified message waiting control \n
///       by the argument.\n
///       Thus, for the file descriptor became cancellation target, when receiving the message, the appropriate \n
///       Callback function is not called.
/// \~english @par Classification
///          Public
/// \~english @see  FrameworkunifiedAttachCallbacksToDispatcherWithFd, FrameworkunifiedAttachCallbackFromDispatcherWithFd,
///                 FrameworkunifiedDetachCallbacksFromDispatcherWithFd
////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedDetachCallbackFromDispatcherWithFd(HANDLE hApp, int fd);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedDetachParentCallbacksFromDispatcher
/// \~english @par Brief
///       API to cancel the multiple of Callback information for the parent thread from Dispatcher.
/// \~english @param [in] hApp
///        HANDLE   - HANDLE for Application
/// \~english @param [in] puiCmdArray
///        PUI_32    -  Pointer to an array of command/request on the service protocol
/// \~english @param [in] uiCommandCount
///        UI_32     - Handler count of cancellation
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
/// \~english @par Prerequisite
///       - Generation/Initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been
///       done.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - HANDLE(hApp) specified in the argument is NULL. [eFrameworkunifiedStatusInvldParam]
///       - HANDLE(hApp) specified in the argument is not appropriate(which is invalid). [eFrameworkunifiedStatusInvldParam]
///       - Pointer(puiCmdArray) to an array of command/request that you specified in the argument is NULL.
///       [eFrameworkunifiedStatusInvldParam]
///       - Handler count(uiCommandCount) of canceled specified by the argument is 0.  [eFrameworkunifiedStatusInvldParam]
///       [eFrameworkunifiedStatusInvldParam]
///       - Parent service name in the HANDLE(hApp) specified in the argument is NULL . [eFrameworkunifiedStatusInvldParam]
///       - There is no parent service of the target in the HANDLE(hApp) specified in the argument. [eFrameworkunifiedStatusFail]
/// \~english @par Detail
///       Cancel multiple Callback-informations(iCmd,CbFuncPtr) from the Dispatcher to process \n
///       the message from the parent thread.  \n
///       Thus, if iCmd has been requested from the parent service (application), the corresponding \n
///       Callback function is no longer referred to. \n
///       If you have already specified the iCmd of revocation has been subject, to return the eFrameworkunifiedStatusOK.
/// \~english @par
///        Please note the following points when using this API for application.\n
///       - Operation cannot be guaranteed if the number of elements of the array specified by the address of the pointer (puiCmdArray)
///         to the array of commands/requests on the service protocol does not match the number of arrays specified by the number of hadles to cancel (uiCommandCount).
/// \~english @par Classification
///          Public
/// \~english @see  FrameworkunifiedAttachCallbackToDispatcher, FrameworkunifiedAttachCallbackToDispatcher,
///                  FrameworkunifiedDetachCallbackFromDispatcher, FrameworkunifiedDetachCallbacksFromDispatcher,
///                  FrameworkunifiedAttachParentCallbacksToDispatcher, FrameworkunifiedDetachParentCallbacksFromDispatcher,
///                  FrameworkunifiedDetachServiceFromDispatcher
////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedDetachParentCallbacksFromDispatcher(HANDLE hApp, const PUI_32 puiCmdArray, UI_32 uiCommandCount);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedDetachServiceFromDispatcher
/// \~english @par Brief
///       Disconnect the service from Dispatcher, and API to cancel all the associated callbacks.
/// \~english @param [in] hApp
///        HANDLE   - HANDLE for Application
/// \~english @param [in] pServiceName
///        PCSTR    -  Pointer to the service name
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
/// \~english @par Prerequisite
///       - Generation/Initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been
///       done.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - HANDLE(hApp) specified in the argument is NULL. [eFrameworkunifiedStatusInvldParam]
///       - HANDLE(hApp) specified in the argument is not appropriate(which is invalid). [eFrameworkunifiedStatusInvldParam]
///       - Pointer (pServiceName) to the service name specified in the argument is NULL. [eFrameworkunifiedStatusInvldParam]
///       - Information corresponding to the service name(pServiceName) specified in the argument is not registered in
///       Dispatcher. [eFrameworkunifiedStatusFail]
///       - Callback information(Event information) of the service name(pServiceName) specified in the argument is not
///       registered. [eFrameworkunifiedStatusFail]
/// \~english @par Detail
///       Cancel the entry of service information from Dispatcher, and cancel all callback \n
///       information(event information) related to the service.
/// \~english @par
///        Please note the following points when using this API for application.\n
///       - Service registration available/API processing when an event is registered:delete service, delete event, return eFrameworkunifiedStatusOK
///       - Service registration available/API processing when an event is registered:delete service, return eFrameworkunifiedStatusOK
///       - Service registration available/API processing when an event is registered:delete event, return eFrameworkunifiedStatusOK
///       - Service registration available/API processing when an event is registered:nothing, return eFrameworkunifiedStatusFail
/// \~english @par Classification
///          Public
/// \~english @see  FrameworkunifiedAttachCallbacksToDispatcher,  FrameworkunifiedAttachCallbackToDispatcher,
///                 FrameworkunifiedDetachCallbackFromDispatcher, FrameworkunifiedAttachParentCallbacksToDispatcher
////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedDetachServiceFromDispatcher(HANDLE hApp, PCSTR pServiceName);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedSubscribeNotificationsWithCallback
/// \~english @par Brief
///       API to send a message to the notification service in order to register multiple Notification of list format.
/// \~english @param [in] hApp
///        HANDLE   - HANDLE for Application
/// \~english @param [in] pNtfyHandler
///        FrameworkunifiedNotificationCallbackHandler   - Pointer to a structure of the Notification Handler
/// \~english @param [in] uiHandlerCount
///        UI_32    - Notification Handler number
/// \~english @par
///        FrameworkunifiedNotificationCallbackHandler Structure
/// \~english @code
///                 #define MAX_STRING_SIZE_NOTIFICATION  64
///                 typedef struct _FrameworkunifiedNotificationCallbackHandler
///                 {
///                       CHAR cNotification[MAX_STRING_SIZE_NOTIFICATION]; /* Notification Name          */
///                       CbFuncPtr callBack;                               /* Callback function pointer  */
///                 }FrameworkunifiedNotificationCallbackHandler;
///            @endcode
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid type of handle
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
/// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
/// \~english @retval eFrameworkunifiedStatusErrOther Other error has occurred(Cannot access shared memory, etc.)
/// \~english @par Prerequisite
///       - NS_NPPService of the process has been started.
///       - Generation/Initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been
///       done.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - Pointer (pNtfyHandler) is NULL to the structure of the Notification Handler specified in the argument.
///       [eFrameworkunifiedStatusInvldParam]
///       - Notification Handler number specified in the argument (uiHandlerCount) 0. [eFrameworkunifiedStatusInvldParam]
///       - HANDLE specified in the argument (hApp) is NULL. [eFrameworkunifiedStatusInvldParam]
///       - HANDLE specified in the argument (hApp) is not appropriate(which is invalid). [eFrameworkunifiedStatusInvldParam]
///       - Notification Handler number specified in the argument (uiHandlerCount) has exceeded the maximum number of
///       Notification. [eFrameworkunifiedStatusFail]
///       - The acquisition of the buffer for storing a plurality of information of Notification Handler specified by
///       the argument has failed. [eFrameworkunifiedStatusInvldParam]
///       - Can not be acquired memory in the acquisition of the buffer of the message for transmission (malloc).
///       [eFrameworkunifiedStatusFail]
///       - Message queue HANDLE in HANDLE specified in the argument (hApp) is NULL. [eFrameworkunifiedStatusInvldParam]
///       - HANDLE specified in the argument message queue HANDLE in (hApp) is not appropriate (which is invalid).
///       [eFrameworkunifiedStatusInvldParam]
///       - Message queue name of HANDLE specified in the argument (hApp) is not appropriate(The name is NULL, more than
///       20byte) [eFrameworkunifiedStatusInvldHandle]
///       - Message queue type of HANDLE specified in the argument (hApp) is not a transmission.
///       [eFrameworkunifiedStatusInvldHndlType]
///       - Session message queue is full to the NPP Service. [eFrameworkunifiedStatusMsgQFull]
///       - It is invalid transmission file descriptor of the session message to the NPP Service. [eFrameworkunifiedStatusErrNoEBADF]
///       - Interruption by the system call (signal) has occurred during the transmission of the session message for the
///       NPP Service. [eFrameworkunifiedStatusErrNoEINTR]
///       - Incorrect size of the transmit buffer of the session message to the NPP Service. [eFrameworkunifiedStatusInvldBufSize]
///       - Any errors occur during the transmission of a session message to the NPP Service. [eFrameworkunifiedStatusFail]
///       - It failed to shared memory access for transmission of the session message for the NPP Service.
///       [eFrameworkunifiedStatusErrOther]
/// \~english @par Detail
///       The Callback information of multiple Notification upon receipt that is specified in the form of a list, \n
///       set to the notification service (NS_NPPService). \n
///       In addition, collectively set the Callback information at the time of multiple Notification received that \n
///       have been specified in the list format to Dispatcher.\n
///       When the notification service receives the public message of the Notification, it sends a NPS_NOTIFY_EV_REQ \n
///       message to the client that has registered the Notification. The client having received the NPS_NOTIFY_EV_REQ
///       \n
///       message, based on the Callback information registered in the Dispatcher, call a Callback corresponding to the
///       \n
///       Notification.
/// \~english @par
///        Please note the following points when using this API for application.\n
///       - Operation is not guaranteed if the number of elemets of the array entity indicated by the address of the pointer (pNtfyHandler)
///         to the Notification Handler structure does not match the number of arrays specified by the Notification Handler number (uiHandlerCount).
/// \~english @par Classification
///          Public
/// \~english @see  FrameworkunifiedNPRegisterNotification, FrameworkunifiedNPUnRegisterNotification, FrameworkunifiedNPUnRegisterNotifications,
///                 FrameworkunifiedNPPublishNotification, FrameworkunifiedSubscribeNotificationWithCallback,
///                 FrameworkunifiedUnsubscribeNotificationWithCallback,
///                 FrameworkunifiedUnsubscribeNotificationsWithCallback
////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSubscribeNotificationsWithCallback(HANDLE hApp,  const FrameworkunifiedNotificationCallbackHandler *pNtfyHandler,
                                                 UI_32 uiHandlerCount);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedSubscribeNotificationWithCallback
/// \~english @par Brief
///       API to set the Callback information at the time of Notification received Dispatcher.
/// \~english @param [in] hApp
///         HANDLE - Application handle
/// \~english @param [in] pNotification
///         PCSTR - Notification String.
/// \param [in] fpOnCmd
///         CbFuncPtr - Callback function that will be called on receiving pNotification.
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusDuplicate Duplication error of entry
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
/// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
/// \~english @retval eFrameworkunifiedStatusErrOther Other error has occurred(Cannot access shared memory, etc.)
/// \~english @par Prerequisite
///       - NS_NPPService of the process has been started.
///       - Generation/Initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been
///       done.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - Pointer (pNotification) is NULL to the Notification name specified. [eFrameworkunifiedStatusInvldParam]
///       - HANDLE specified in the argument (hApp) is NULL. [eFrameworkunifiedStatusInvldParam]
///       - CbFuncPtr specified in the argument (fpOnCmd) is NULL. [eFrameworkunifiedStatusInvldParam]
///       - HANDLE specified in the argument (hApp) is not appropriate (which is invalid). [eFrameworkunifiedStatusInvldParam]
///       - Can not add HANDLE specified in the argument (hApp) and CbFuncPtr specified in the argument (fpOnCmd) to
///       notification list [eFrameworkunifiedStatusDuplicate]
///       - Message queue HANDLE in HANDLE specified in the argument (hApp) is NULL. [eFrameworkunifiedStatusInvldParam]
///       - HANDLE specified in the argument message queue HANDLE in (hApp) is not appropriate (which is invalid).
///       [eFrameworkunifiedStatusInvldParam]
///       - Message queue name of HANDLE specified in the argument (hApp) is not appropriate(The name is NULL, more than
///       20byte) [eFrameworkunifiedStatusInvldHandle]
///       - Notification name specified by the argument has already been registered in the Dispatcher.
///       [eFrameworkunifiedStatusDuplicate]
///       - Session message queue is full to the NPP Service. [eFrameworkunifiedStatusMsgQFull]
///       - It is invalid transmission file descriptor of the session message to the NPP Service. [eFrameworkunifiedStatusErrNoEBADF]
///       - Interruption by the system call (signal) has occurred during the transmission of the session message for the
///       NPP Service. [eFrameworkunifiedStatusErrNoEINTR]
///       - Incorrect size of the transmit buffer of the session message to the NPP Service. [eFrameworkunifiedStatusInvldBufSize]
///       - Any errors occur during the transmission of a session message to the NPP Service. [eFrameworkunifiedStatusFail]
///       - It failed to shared memory access for transmission of the session message for the NPP Service.
///       [eFrameworkunifiedStatusErrOther]
/// \~english @par Detail
///      The Callback information at the time of Notification received that has been specified by the argument, \n
///      set in the notification service (NS_NPPService).\n
///      Also, it sets the Callback information at the time of Notification received that has been specified by \n
///      the argument to the Dispatcher.\n
///       When the notification service receives the public message of the Notification, it sends a NPS_NOTIFY_EV_REQ \n
///       message to the client that has registered the Notification. The client having received the NPS_NOTIFY_EV_REQ
///       \n
///       message, based on the Callback information registered in the Dispatcher, call a Callback corresponding to the
///       \n
///       Notification.
/// \~english @par Classification
///          Public
/// \~english @see  FrameworkunifiedNPRegisterNotification, FrameworkunifiedNPUnRegisterNotification, FrameworkunifiedNPUnRegisterNotifications,
///                 FrameworkunifiedNPPublishNotification, FrameworkunifiedUnsubscribeNotificationWithCallback,
///                 FrameworkunifiedSubscribeNotificationsWithCallback, FrameworkunifiedUnsubscribeNotificationsWithCallback
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSubscribeNotificationWithCallback(HANDLE hApp, PCSTR pNotification, CbFuncPtr fpOnCmd);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedUnsubscribeNotificationsWithCallback
/// \~english @par Brief
///       API to bulk delete the Callback information at the time of multiple Notification received from the Dispatcher.
/// \~english @param [in] hApp
///        HANDLE   - HANDLE for Application
/// \~english @param [in] pNtfyHandler
///        FrameworkunifiedNotificationCallbackHandler   - Pointer to a structure of the Notification Handler
/// \~english @param [in] uiHandlerCount
///        UI_32    - Notification Handler number
/// \~english @par
///        FrameworkunifiedNotificationCallbackHandler Structure
/// \~english @code
///                 #define MAX_STRING_SIZE_NOTIFICATION  64
///                 typedef struct _FrameworkunifiedNotificationCallbackHandler
///                 {
///                       CHAR cNotification[MAX_STRING_SIZE_NOTIFICATION]; /* Notification Name          */
///                       CbFuncPtr callBack;                               /* Callback function pointer  */
///                 }FrameworkunifiedNotificationCallbackHandler;
///            @endcode
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid type of handle
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
/// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
/// \~english @retval eFrameworkunifiedStatusErrOther Other error has occurred(Cannot access shared memory, etc.)
/// \~english @par Prerequisite
///       - NS_NPPService of the process has been started.
///       - Generation/Initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been
///       done.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - Pointer (pNtfyHandler) is NULL to the structure of the Notification Handler specified in the argument.
///       [eFrameworkunifiedStatusInvldParam]
///       - Notification Handler number specified in the argument (uiHandlerCount) 0. [eFrameworkunifiedStatusInvldParam]
///       - HANDLE specified in the argument (hApp) is NULL. [eFrameworkunifiedStatusInvldParam]
///       - HANDLE specified in the argument (hApp) is not appropriate (which is invalid). [eFrameworkunifiedStatusInvldParam]
///       - Notification Handler number specified in the argument (uiHandlerCount) has exceeded the maximum number of
///       Notification. [eFrameworkunifiedStatusFail]
///       - In Notification name in the structure of the Notification Handler specified in the argument, there is not
///       applicable in those registered as Callback information. [eFrameworkunifiedStatusFail]
///       - The acquisition of the buffer for storing a plurality of information of Notification Handler specified by
///       the argument has failed. [eFrameworkunifiedStatusInvldParam]
///       - Can not be acquired memory in the acquisition of the buffer of the message for transmission (malloc).
///       [eFrameworkunifiedStatusFail]
///       - Message queue HANDLE in HANDLE specified in the argument (hApp) is NULL. [eFrameworkunifiedStatusInvldParam]
///       - HANDLE specified in the argument message queue HANDLE in (hApp) is not appropriate (which is invalid).
///       [eFrameworkunifiedStatusInvldParam]
///       - Message queue name of HANDLE specified in the argument (hApp) is not appropriate(The name is NULL, more than
///       20byte) [eFrameworkunifiedStatusInvldHandle]
///       - Message queue type of HANDLE specified in the argument (hApp) is not a transmission.
///       [eFrameworkunifiedStatusInvldHndlType]
///       - Session message queue is full to the NPP Service. [eFrameworkunifiedStatusMsgQFull]
///       - It is invalid transmission file descriptor of the session message to the NPP Service. [eFrameworkunifiedStatusErrNoEBADF]
///       - Interruption by the system call (signal) has occurred during the transmission of the session message for the
///       NPP Service. [eFrameworkunifiedStatusErrNoEINTR]
///       - Incorrect size of the transmit buffer of the session message to the NPP Service. [eFrameworkunifiedStatusInvldBufSize]
///       - Any errors occur during the transmission of a session message to the NPP Service. [eFrameworkunifiedStatusFail]
///       - It failed to shared memory access for transmission of the session message for the NPP Service.
///       [eFrameworkunifiedStatusErrOther]
/// \~english @par Detail
///       The Callback information of multiple Notification upon receipt that is specified in the form of a list, \n
///       collectively removed from the notification service (NS_NPPService). \n
///       In addition, the bulk delete the Callback information at the time of multiple Notification received that \n
///       are specified in the form of a list from the Dispatcher.\n
///       By bulk delete the Callback information, if the notification services receives the public message
///       Notification, \n
///       because the Notification information applicable also been deleted, Callback does not occur.
/// \~english @par Classification
///          Public
/// \~english @see  FrameworkunifiedNPRegisterNotification, FrameworkunifiedNPUnRegisterNotification, FrameworkunifiedNPRegisterNotifications,
/// FrameworkunifiedNPUnRegisterNotifications,
///                 FrameworkunifiedNPPublishNotification, FrameworkunifiedSubscribeNotificationWithCallback,
///                 FrameworkunifiedSubscribeNotificationsWithCallback,
///                 FrameworkunifiedUnsubscribeNotificationWithCallback
////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedUnsubscribeNotificationsWithCallback(HANDLE hApp, const FrameworkunifiedNotificationCallbackHandler *pNtfyHandler,
                                                   UI_32 uiHandlerCount);


////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedUnsubscribeNotificationWithCallback
/// \~english @par Brief
///       API to detach a notification callback from the dispatcher.
/// \~english @param [in] hApp
///         HANDLE - Application handle
/// \~english @param [in] pNotification
///         PCSTR - Notification String.
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
/// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
/// \~english @retval eFrameworkunifiedStatusErrOther Other error has occurred(Cannot access shared memory, etc.)
/// \~english @par Prerequisite
///       - NS_NPPService of the process has been started.
///       - Generation/Initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been
///       done.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - Pointer (pNotification) is NULL to the Notification name specified. [eFrameworkunifiedStatusInvldParam]
///       - Length of pointer (pNotification) is 0 to the Notification name specified. [eFrameworkunifiedStatusInvldParam]
///       - HANDLE specified in the argument (hApp) is NULL. [eFrameworkunifiedStatusInvldParam]
///       - HANDLE specified in the argument (hApp) is not appropriate (which is invalid). [eFrameworkunifiedStatusInvldParam]
///       - Missing HANDLE specified in the argument (hApp) in notification list [eFrameworkunifiedStatusFail]
///       - Can not be acquired memory in the acquisition of the buffer of the message for transmission (malloc).
///       [eFrameworkunifiedStatusFail]
///       - Message queue HANDLE in HANDLE specified in the argument (hApp) is NULL. [eFrameworkunifiedStatusInvldParam]
///       - HANDLE specified in the argument message queue HANDLE in (hApp) is not appropriate (which is invalid).
///       [eFrameworkunifiedStatusInvldParam]
///       - Message queue name of HANDLE specified in the argument (hApp) is not appropriate(The name is NULL, more than
///       20byte) [eFrameworkunifiedStatusInvldHandle]
///       - Session message queue is full to the NPP Service. [eFrameworkunifiedStatusMsgQFull]
///       - It is invalid transmission file descriptor of the session message to the NPP Service. [eFrameworkunifiedStatusErrNoEBADF]
///       - Interruption by the system call (signal) has occurred during the transmission of the session message for the
///       NPP Service. [eFrameworkunifiedStatusErrNoEINTR]
///       - Incorrect size of the transmit buffer of the session message to the NPP Service. [eFrameworkunifiedStatusInvldBufSize]
///       - Any errors occur during the transmission of a session message to the NPP Service. [eFrameworkunifiedStatusFail]
///       - It failed to shared memory access for transmission of the session message for the NPP Service.
///       [eFrameworkunifiedStatusErrOther]
/// \~english @par Detail
///      The Callback information at the time of Notification received that has been specified by the argument, \n
///      to remove from the notification service (NS_NPPService).\n
///      Also, delete the Callback information of Notification upon receipt, which is specified by the argument \n
///      from the Dispatcher.
///       By bulk delete the Callback information, if the notification services receives the public message
///       Notification, \n
///       because the Notification information applicable also been deleted, Callback does not occur.
/// \~english @par Classification
///          Public
/// \~english @see  FrameworkunifiedNPRegisterNotification, FrameworkunifiedNPUnRegisterNotification, FrameworkunifiedNPRegisterNotifications,
/// FrameworkunifiedNPUnRegisterNotifications,
///                 FrameworkunifiedNPPublishNotification, FrameworkunifiedSubscribeNotificationWithCallback,
///                 FrameworkunifiedSubscribeNotificationsWithCallback,
///                 FrameworkunifiedUnsubscribeNotificationsWithCallback
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedUnsubscribeNotificationWithCallback(HANDLE hApp, PCSTR pNotification);



////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedGetAppName
/// \~english @par Brief
///        Get the application thread name that is registered with the dispatcher.
/// \~english @param [in] hApp
///        HANDLE   - Handle for Application
/// \~english @retval PCSTR Pointer to application thread name
/// \~english @retval NULL  NULL pointer (Failed to get application name)
/// \~english @par Prerequisite
///       - Generation/Initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been
///       done.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - HANDLE specified in the argument (hApp) is NULL. [NULL]
///       - HANDLE specified in the argument (hApp) is not appropriate (which is invalid). [NULL]
/// \~english @par Detail
///        Get the application thread name that was registered at initialization of dispatcher.
/// \~english @par Classification
///          Public
/// \~english @see FrameworkunifiedDispatcherWithArguments, FrameworkunifiedDispatcher, FrameworkunifiedCreateDispatcherWithoutLoop
////////////////////////////////////////////////////////////////////////////////////////////
PCSTR FrameworkunifiedGetAppName(HANDLE hApp);



////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedSetThreadSpecificData
/// \~english @par Brief
///        Set pointer to the application-specific data to the application HANDLE.
/// \~english @param [in] hApp
///        HANDLE   - HANDLE for Application
/// \~english @param [in] data
///        PVOID   - Pointer to the application-specific data
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @par Prerequisite
///       - Generation/Initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been
///       done.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - HANDLE specified in the argument (hApp) is NULL. [eFrameworkunifiedStatusInvldHandle]
///       - HANDLE specified in the argument (hApp) is not appropriate (which is invalid). [eFrameworkunifiedStatusInvldHandle]
/// \~english @par Detail
///        This API sets application-specific data to internal pointer inside application framwork HANDLE.\n
///        You can share data in callback function or thread.
/// \~english @par Classification
///          Public
/// \~english @see FrameworkunifiedGetThreadSpecificData
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSetThreadSpecificData(HANDLE hApp, PVOID data);



////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedGetThreadSpecificData
/// \~english @par Brief
///        Get pointer to the application-specific data from the application HANDLE.
/// \~english @param [in] hApp
///        HANDLE   - HANDLE for Application
/// \~english @retval PVOID pointer to application-specific data.
/// \~english @retval NULL NULL pointer(failed to get data)
/// \~english @par Prerequisite
///       - Generation/Initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been
///       done.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - HANDLE specified in the argument (hApp) is NULL. [NULL]
///       - HANDLE specified in the argument (hApp) is not appropriate (which is invalid). [NULL]
/// \~english @par Detail
///        This API gets application-specific data that set with FrameworkunifiedSetThreadSpecificData from internal pointer inside
///        application framwork HANDLE.\n
///        You can share data in callback function or thread.
/// \~english @par Classification
///          Public
/// \~english @see FrameworkunifiedSetThreadSpecificData
////////////////////////////////////////////////////////////////////////////////////////////
PVOID FrameworkunifiedGetThreadSpecificData(HANDLE hApp);



////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedOpenService
/// \~english @par Brief
///        API to acquire a service HANDLE.
/// \~english @param [in] hApp
///        HANDLE   - HANDLE for Application
/// \~english @param [in] pServiceName
///        PCSTR   - Pointer to service name
/// \~english @retval HANDLE Handle to the service
/// \~english @retval NULL NULL pointer (Failed to get service handle)
/// \~english @par Prerequisite
///       - Generation/Initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been
///       done.
///       - Only when will open the session by FrameworkunifiedOpenSession, you can use the handle obtained by this API.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - HANDLE specified in the argument (hApp) is NULL. [NULL]
///       - HANDLE specified in the argument (hApp) is not appropriate (which is invalid). [NULL]
///       - Service name specified in the argument (pServiceName) is NULL. [NULL]
///       - Service name specified in the argument (pServiceName) is not appropriate (NULL, over 20byte). [NULL]
///       - Normalized service name for message queue is not appropriate (over 15byte). [NULL]
///       - Can not be acquired memory for message queue infomation(malloc). [NULL]
///       - Can not be acquired memory for message queue name(malloc). [NULL]
///       - Failed to open message queue(mq_open). [NULL]
/// \~english @par Detail
///        Create a communication resource for communicating with the specified service , return the HANDLE.
/// \~english @par Classification
///          Public
/// \~english @see FrameworkunifiedCloseService, FrameworkunifiedOpenSession
////////////////////////////////////////////////////////////////////////////////////////////
HANDLE FrameworkunifiedOpenService(HANDLE hApp, PCSTR pServiceName);



////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedCloseService
/// \~english @par Brief
///        API to close service HANDLE.
/// \~english @param [in] hApp
///        HANDLE   - Handle for Application
/// \~english @param [in] hService
///        HANDLE   - Handle to the service(FrameworkunifiedOpenService HANDLE acquired in)
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid type of handle
/// \~english @par Prerequisite
///       - Generation/Initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been
///       done.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - HANDLE specified in the argument (hApp) is NULL. [eFrameworkunifiedStatusInvldHandle]
///       - HANDLE specified in the argument (hApp) is not appropriate (which is invalid). [eFrameworkunifiedStatusInvldHandle]
///       - HANDLE specified in the argument (hService) is NULL. [eFrameworkunifiedStatusInvldHandle]
///       - HANDLE specified in the argument (hService) is not appropriate (which is invalid). [eFrameworkunifiedStatusInvldHandle]
///       - Message queue HANDLE in HANDLE specified in the argument (hService) is NULL. [eFrameworkunifiedStatusInvldHandle]
///       - HANDLE specified in the argument message queue HANDLE in (hService) is not appropriate (which is invalid).
///       [eFrameworkunifiedStatusInvldHandle]
///       - Message queue type of HANDLE specified in the argument (hService) is mismatch.(eQTypeSender or
///       eQTypeReveiver) [eFrameworkunifiedStatusInvldHndlType]
///       - Failed to close message queue specified in the argument (hService). [eFrameworkunifiedStatusInvldHandle]
/// \~english @par Detail
///       This API closes service HANDLE specified by argument.\n
///       And close message queue of the service.
/// \~english @par Classification
///          Public
/// \~english @see FrameworkunifiedOpenService
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedCloseService(HANDLE hApp, HANDLE hService);

//////////////////////////////////////////////////
// message center functions
//////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedMcOpenSender
/// \~english @par Brief
///        Generate a handle to the message queue for sending.
/// \~english @param [in] hApp
///        HANDLE   - Handle for Application
/// \~english @param [in] pName
///        PCSTR   - Message queue name of destination service
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid type of handle
/// \~english @par Prerequisite
///       - Generation/Initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been
///       done.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - HANDLE specified in the argument (hApp) is NULL. [NULL]
///       - HANDLE specified in the argument (hApp) is not appropriate (which is invalid). [NULL]
///       - Message queue name specified in the argument (pName) is NULL. [NULL]
///       - Message queue name specified in the argument (pName) is not appropriate (NULL, over 20byte). [NULL]
///       - Normalized message queue name is not appropriate (over 15byte). [NULL]
///       - Can not be acquired memory for message queue infomation(malloc). [NULL]
///       - Can not be acquired memory for message queue name(malloc). [NULL]
///       - Failed to open message queue(mq_open). [NULL]
/// \~english @par Detail
///       This API generates and returns a handle to the message queue for sending of service.
/// \~english @par Classification
///          Public
/// \~english @see FrameworkunifiedMcClose
////////////////////////////////////////////////////////////////////////////////////////////
HANDLE FrameworkunifiedMcOpenSender(HANDLE hApp, PCSTR pName);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedMcClose
/// \~english @par Brief
///        API for closing a message queue handle
/// \~english @param [in] hService
///        HANDLE   - Handle for services (handle obtained by FrameworkunifiedMcOpenSender)
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @par Prerequisite
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hService specified in the arguments is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - If the hService specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusInvldHandle]
///       - When the message queue handle in the handle (hService) specified by the arguments is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - When the message queue handle specified in the argument (hService) is invalid (invalid) [eFrameworkunifiedStatusInvldHandle]
///       - If the message queue types of the handles (hService) specified in the arguments do not match, [eFrameworkunifiedStatusInvldHandle]
///       - Closing the message queue for the handle (hService) specified in the arguments fails, [eFrameworkunifiedStatusInvldHandle]
/// \~english @par Detail
///        Closes the message queue for the specified service.
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Sync
/// \~english @see FrameworkunifiedMcOpenSender
///
/// To close message queue handle to a service
///
/// \param [in] hService
///         HANDLE - Handle to a service
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see FrameworkunifiedMcOpenSender, FrameworkunifiedSendMsg, FrameworkunifiedGetMsgLength, FrameworkunifiedGetMsgSrc, FrameworkunifiedGetMsgDataOfSize,
///      FrameworkunifiedClearMsgData, FrameworkunifiedGetDataUSID, FrameworkunifiedForwardMessage, FrameworkunifiedGetSystemInfo
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedMcClose(HANDLE hService);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedSendMsg
/// \~english @par Brief
///        Asynchronous API that sends messages to the service
/// \~english @param [in] hService
///        HANDLE   - Handle for services (handle acquired by FrameworkunifiedMcOpenSender, etc.)
/// \~english @param [in] uiCmd
///        UI_32   - Command ID, Request ID, and Authorization ID on the service's protocol
/// \~english @param [in] uiLength
///        UI_32   - Data size to be sent (0 to 4294967255)
/// \~english @param [in] pData
///        PCVOID   - Pointer to the data to send
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusNullPointer Specifying NULL pointers
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid handle type
/// \~english @retval eFrameworkunifiedStatusInvldBuf Bad buffer
/// \~english @retval eFrameworkunifiedStatusInvldQName Invalid message queue name
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid file descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR Generating an interrupt by a system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer size
/// \~english @retval eFrameworkunifiedStatusFail Some error occurred
/// \~english @retval eFrameworkunifiedStatusErrOther Other errors (Failed to open/allocate shared memory, etc.)
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @par Prerequisite
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
///       - Handles for services must be acquired (FrameworkunifiedMcOpenSender, etc.).
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hService specified in the arguments is NULL, [eFrameworkunifiedStatusNullPointer]
///       - If the hService specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusNullPointer]
///       - When the message queue name of the handle (hService) specified in the arguments is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - When the name of the message queue of the handle (hService) specified in the argument exceeds 20 bytes [eFrameworkunifiedStatusInvldHandle]
///       - When the message queue handle in the handle (hService) specified by the arguments is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - When the message queue handle specified in the argument (hService) is invalid (invalid) [eFrameworkunifiedStatusInvldHandle]
///       - When the send data (pData) specified in the parameter is NULL and the data length (uiLength) is not 0, [eFrameworkunifiedStatusInvldBuf]
///       - When the name of the message queue handle in the handle (hService) specified in the arguments is NULL, [eFrameworkunifiedStatusInvldQName]
///       - If the name of the message queue handle (hService) specified in the argument exceeds 20 bytes, [eFrameworkunifiedStatusInvldQName]
///       - When the message queue type of the handle (hService) specified in the parameter is not the send type, [eFrameworkunifiedStatusInvldHndlType]
///       - Message queue for sessions for services is full [eFrameworkunifiedStatusMsgQFull]
///       - Incorrect file descriptor for sending messages for sessions intended for the service [eFrameworkunifiedStatusErrNoEBADF]
///       - When an interrupt by a system call (signal) occurs while sending session-specific messages for services [eFrameworkunifiedStatusErrNoEINTR]
///       - Incorrect size of send buffers for sessions intended for services [eFrameworkunifiedStatusInvldBufSize]
///       - If there are any errors sending messages for sessions intended for the services [eFrameworkunifiedStatusFail]
///       - Failed to access shared memory for sending messages for sessions for services [eFrameworkunifiedStatusErrOther]
///       - When the size (uiLength) of the data to be sent specified in the parameter is out of range, [eFrameworkunifiedStatusInvldParam]
/// \~english @par Detail
///       Messages are sent to the service using the message queue associated with the service handle.\n
///       A destination is a service specified as arguments such as FrameworkunifiedMcOpenSender when a service handle was created.\n
///       This API operates asynchronously.
/// \~english @par
///       - The consistency between the size of the data to be sent and the size of the area indicated by the pointer to the data to be sent is ensure by the caller.
///       - The maximum data size (uiLength) described above is a theoretical value. The maximum data size that can be actually used depends on the environment requirements.\n
///         The maximum value is the upper limit of the type (UINT_MAX) minus the following size.\n
///         Message header section\n
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Method
/// \~english @see FrameworkunifiedMcOpenSender, FrameworkunifiedOpenService
///
/// Send message to the service.
///
/// \param [in] hService
///         HANDLE - Handle to the service.
/// \param [in] uiCmd
///         UI_32 - Protocol message id.
/// \param [in] uiLength
///         UI_32 - Length of the data to be send.
/// \param [in] pData
///         PVOID - Pointer to the data.
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see FrameworkunifiedMcOpenSender, FrameworkunifiedMcClose, FrameworkunifiedGetMsgLength, FrameworkunifiedGetMsgSrc, FrameworkunifiedGetMsgDataOfSize,
///      FrameworkunifiedClearMsgData, FrameworkunifiedGetDataUSID, FrameworkunifiedForwardMessage, FrameworkunifiedGetSystemInfo
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSendMsg(HANDLE hService, UI_32 uiCmd, UI_32 uiLength, PCVOID pData);


////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedSendPriorityMsg
/// Send message to a service. It places your message at the top of the services message queue.
/// \b Applications should not use this unless absolutely required - use cases must be reviewed by
/// \b Framework Team.
///
/// \param [in] hService
///         HANDLE - Handle to the service.
/// \param [in] uiCmd
///         UI_32 - Protocol message id.
/// \param [in] uiLength
///         UI_32 - Length of the data to be send.
/// \param [in] pData
///         PVOID - Pointer to the data.
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see FrameworkunifiedMcOpenSender, FrameworkunifiedMcClose, FrameworkunifiedGetMsgLength, FrameworkunifiedGetMsgSrc, FrameworkunifiedGetMsgDataOfSize,
///      FrameworkunifiedClearMsgData, FrameworkunifiedGetDataUSID, FrameworkunifiedForwardMessage, FrameworkunifiedGetSystemInfo
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSendPriorityMsg(HANDLE hService, UI_32 uiCmd, UI_32 uiLength, PCVOID pData);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedSendSelf
/// \~english @par Brief
///        Asynchronous APIs that send messages to the application's own Dispatcher
/// \~english @param [in] hApp
///        HANDLE   - Handle for the application
/// \~english @param [in] iCmd
///        UI_32   - Command ID, Request ID, and Authorization ID on the service's protocol
/// \~english @param [in] length
///        UI_32   - Data size to be sent (0 to 4294967255)
/// \~english @param [in] data
///        PCVOID   - Pointer to the data to send
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusNullPointer Specifying NULL pointers
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid handle type
/// \~english @retval eFrameworkunifiedStatusInvldQName Invalid message queue name
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid file descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR Generating an interrupt by a system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer size
/// \~english @retval eFrameworkunifiedStatusFail Some error occurred
/// \~english @retval eFrameworkunifiedStatusErrOther Other errors (Failed to open/allocate shared memory, etc.)
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @par Prerequisite
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hService specified in the arguments is NULL, [eFrameworkunifiedStatusNullPointer]
///       - If the hService specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusNullPointer]
///       - When the message queue handle for the own Dispatcher in the handle (hService) specified by the arguments is NULL [eFrameworkunifiedStatusInvldHandle]
///       - Invalid message queue handle for its own Dispatcher in the handle (hService) specified by the arguments [eFrameworkunifiedStatusInvldHandle]
///       - When the send data (data) specified in the parameter is NULL, [eFrameworkunifiedStatusInvldBuf]
///       - When the service name of the handle (hService) specified in the arguments is not appropriate (when the service name exceeds NULL, 20 bytes) [eFrameworkunifiedStatusInvldQName]
///       - When the name of the message queue handle for the own Dispatcher in the handle (hService) specified in the arguments exceeds 20 bytes [eFrameworkunifiedStatusInvldQName]
///       - Failed to access shared memory for sending messages for its own Dispatcher [eFrameworkunifiedStatusErrOther]
///       - When the message queue type for the own Dispatcher of the handle (hService) specified in the parameter is not the send type [eFrameworkunifiedStatusInvldHndlType]
///       - If the message queue for your Dispatcher is full [eFrameworkunifiedStatusMsgQFull]
///       - Incorrect file descriptor for sending messages for own Dispatcher [eFrameworkunifiedStatusErrNoEBADF]
///       - When an interrupt by a system call (signal) occurs while sending messages for the Dispatcher itself [eFrameworkunifiedStatusErrNoEINTR]
///       - Incorrect size of send buffers for sessions intended for services [eFrameworkunifiedStatusInvldBufSize]
///       - If there are any errors sending messages for your Dispatcher [eFrameworkunifiedStatusFail]
///       - When the size (length) of the data to be sent specified in the parameter is out of range, [eFrameworkunifiedStatusInvldParam]
/// \~english @par Detail
///       Send messages to the application's own Dispatcher.\n
///       This API operates asynchronously.
/// \~english @par
///        Please note the following points when using this API for application.\n
///       - The maximum data size (length) described above is a theoretical value. The maximum data size that can be actually used depends on the environment requirements.\n
///         The maximum value is the upper limit of the type (UINT_MAX) minus the following size.\n
///         Message header section\n
///       - Operation cannot be guaranteed when the size of the allocated area indicated by the address of the pointer (data) to the data buffer does not match the size specified by the size of the data buffer (length).
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Method
/// \~english @see FrameworkunifiedSendMsg
///
/// Send message to the hApp.
///
/// \param [in] hApp
///         HANDLE - Handle to the dispatcher to receive message
/// \param [in] iCmd
///         UI_32 - Protocol message id.
/// \param [in] length
///         UI_32 - Length of the data to be send.
/// \param [in] data
///         PVOID - Pointer to the data.
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see FrameworkunifiedMcOpenSender, FrameworkunifiedMcClose, FrameworkunifiedGetMsgLength, FrameworkunifiedGetMsgSrc, FrameworkunifiedGetMsgDataOfSize,
///      FrameworkunifiedClearMsgData, FrameworkunifiedGetDataUSID, FrameworkunifiedForwardMessage, FrameworkunifiedGetSystemInfo
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSendSelf(HANDLE hApp, UI_32 iCmd, UI_32 length, PCVOID data);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedInvokeSync
/// \~english @par Brief
///        API for synchronous communication of the message.
/// \~english @param [in] hService
///        HANDLE   - Handle for the message queue
/// \~english @param [in] iCmd
///        UI_32   -  Command ID
/// \~english @param [in] msgLenght
///        UI_32   -  Data length of the send message
/// \~english @param [in] msgData
///        PCVOID   - Pointer to send the message
/// \~english @param [in] responseLength
///        UI_32   -  Size of the received message storage buffer
/// \~english @param [out] responseData
///        PVOID   -  Pointer to the received message storage buffer
/// \~english @param [out] receivedLength
///        UI_32 * -  Data length of the received message
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusNullPointer NULL pointer specified
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid type of handle
/// \~english @retval eFrameworkunifiedStatusInvldQName Illegal Message Queue name
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
/// \~english @retval eFrameworkunifiedStatusInvldBuf Invalid buffer
/// \~english @retval eFrameworkunifiedStatusBadConnection It can not be a socket connection
/// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
/// \~english @retval eFrameworkunifiedStatusErrOther Other error has occurred(Cannot access shared memory, etc.)
/// \~english @par Prerequisite
///       - Generation/Initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been
///       done.
///       - Handles for message queue must be acquired (FrameworkunifiedMcOpenSender, etc.).
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - HANDLE(hService) specified in the argument is NULL. [eFrameworkunifiedStatusNullPointer]
///       - HANDLE(hService) specified in the argument is not appropriate (which is invalid). [eFrameworkunifiedStatusNullPointer]
///       - It failed to open the message queue for the response received [eFrameworkunifiedStatusFail]
///       - Send message queue HANDLE in HANDLE(hService) specified in the argument is NULL. [eFrameworkunifiedStatusInvldHandle]
///       - Send message queue HANDLE in HANDLE(hService) specified in the argument is not appropriate (which is
///       invalid). [eFrameworkunifiedStatusInvldHandle]
///       - Message queue HANDLE for the response received is NULL. [eFrameworkunifiedStatusInvldHandle]
///       - Message queue HANDLE for the response received is not appropriate (which is invalid).
///       [eFrameworkunifiedStatusInvldHandle]
///       - The size of the received message storage buffer specified by the argument is not 0, and the pointer is NULL
///       to the received message storage buffer. [eFrameworkunifiedStatusInvldHandle]
///       - It fails to get the message storage buffer for the response received. [eFrameworkunifiedStatusFail]
///       - Random number generation to fail for the sequence ID of the message for transmission. [eFrameworkunifiedStatusFail]
///       - Message queue name of HANDLE(hService) specified in the argument is NULL. [eFrameworkunifiedStatusInvldQName]
///       - Message queue name of HANDLE(hService) specified in the argument exceeds 20byte. [eFrameworkunifiedStatusInvldQName]
///       - Message queue HANDLE name in the HANDLE(hService) specified in the argument is NULL. [eFrameworkunifiedStatusInvldHandle]
///       - Message queue HANDLE name in the HANDLE(hService) specified in the argument exceeds 20byte.
///       [eFrameworkunifiedStatusInvldHandle]
///       - Message queue type of HANDLE(hService) specified in the argument is not a transmission type.
///       [eFrameworkunifiedStatusInvldHndlType]
///       - To the session message queue is full. [eFrameworkunifiedStatusMsgQFull]
///       - It is invalid transmit and receive file descriptor of the session message. [eFrameworkunifiedStatusErrNoEBADF]
///       - Interruption by the system call (signal) occurs during the transmission and reception of the session
///       message. [eFrameworkunifiedStatusErrNoEINTR]
///       - Size of the transmit buffer of the message for the session to the service is illegal.
///       [eFrameworkunifiedStatusInvldBufSize]
///       - Any errors occur during the sending and receiving of a session message. [eFrameworkunifiedStatusFail]
///       - In the shared memory access for the session message to be sent to service failed. [eFrameworkunifiedStatusErrOther]
///       - It failed to Socket connection of the response message for the reception. [eFrameworkunifiedStatusFail]
///       - The wait control of the response message received failed. [eFrameworkunifiedStatusFail]
///       - Interruption by the system call (signal) to wait in the response message received has occurred.
///       [eFrameworkunifiedStatusFail]
///       - In response message at the time of failure to receive, it failed to re-connection of the Socket.
///       [eFrameworkunifiedStatusBadConnection]
/// \~english @par Detail
///       Under the following procedure, perform synchronous communication of the message.
///        - During the first call, it generates a message queue name for the response message received by the
///        McCreateInvokerName(),
///          and to open the message queue for the response message received by the McOpenSyncReceiver ().
///        - By the McInvokeSync(), perform synchronous communication of the message.
/// \~english @par
///       If the received response message is greater than the storage buffer, to store up to store buffer length.\n
///       The user is able to detect an abnormality by comparing store buffer length and a received message length.
/// \~english @par Classification
///          Public
/// \~english @see FrameworkunifiedSendMsg
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedInvokeSync(HANDLE hService, UI_32 iCmd, UI_32 msgLenght, PCVOID msgData, UI_32 responseLength,
                         PVOID responseData, UI_32 *receivedLength);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedSetSyncResponseData
/// \~english @par Brief
///        API to set the response data of the synchronization communication.
/// \~english @param [in] hApp
///        HANDLE   - Handle for Application
/// \~english @param [in] data
///        PVOID   - Pointer to the response data
/// \~english @param [in] size
///        UI_32   - The response data size
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusNullPointer NULL pointer specified
/// \~english @par Prerequisite
///       - Generation/Initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been
///       done.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - HANDLE(hApp) specified in the argument is NULL. [eFrameworkunifiedStatusNullPointer]
///       - HANDLE(hApp) specified in the argument is not appropriate (which is invalid). [eFrameworkunifiedStatusNullPointer]
///       - Pointer to the response data(data) specified in the argument is NULL. [eFrameworkunifiedStatusNullPointer]
///       - If the size (size) of the response data specified in the arguments cannot be allocated, [eFrameworkunifiedStatusFail]
///       - When the response data size (size) specified in the parameter is out of range, [eFrameworkunifiedStatusInvldParam]
/// \~english @par Detail
///       Store the response data specified in the storage area of the response data \n
///       in the handle specified by the argument.
/// \~english @par
///        Response data set by this PROCESS is automatically sent by the Dispatcher after the callback function completes execution, so the API does not need to send the response data.
/// \~english @par
///       - The response data is registered to the heap.
///       - If the pointer (data) to the response data is normal (not NULL) and 0 is specified for the size (size) of the response data, the response data is set to empty.
///       - The maximum size (size) of the response data described above is a theoretical value. The maximum size that can be used is environment-dependent.\n
///         The maximum value is the upper limit of the type (UINT_MAX) minus the following size.\n
///         Message header section and response message header section\n
/// \~english @par Classification
///          Public
/// \~english @see FrameworkunifiedInvokeSync
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSetSyncResponseData(HANDLE hApp, PVOID data, UI_32 size);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedGetMsgLength
/// \~english @par Brief
///        API to retrieve the data length of the received message.
/// \~english @param [in] hApp
///        HANDLE   - Handle for Application
/// \~english @retval UI_32 The data length of the received message
/// \~english @retval 0     Error or No data
/// \~english @par Prerequisite
///       - Generation/Initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been
///       done.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - HANDLE(hApp) specified in the argument is NULL. [0]
///       - HANDLE(hApp) specified in the argument is not appropriate (which is invalid). [0]
/// \~english @par Detail
///       To return the data length of the received messages in the handle(hApp) \n
///       that you specified in the argument  as a return value.  \n
///       It is possible to get by to use when the message was received. \n
///       If the return value is 0, there is a need for caution because of either \n
///       error or no data.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Sync
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////////////
UI_32 FrameworkunifiedGetMsgLength(HANDLE hApp);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedGetMsgProtocol
/// \~english @par Brief
///        API to get the Command ID/Request ID/Approval ID on the protocol.
/// \~english @param [in] hApp
///        HANDLE   - Handle for Application
/// \~english @retval UI_32 Command ID/Request ID/Approval ID
/// \~english @retval 0     Error
/// \~english @par Prerequisite
///       - Generation/Initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been
///       done.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - HANDLE(hApp) specified in the argument is NULL. [0]
///       - HANDLE(hApp) specified in the argument is not appropriate (which is invalid). [0]
/// \~english @par Detail
///       To return the Command ID/Request ID/Approval ID that are within  \n
///       the handle(hApp) specified in the argument as a return value.
/// \~english @par
///        The application shall execute this API using an application handle passed as an argument to the callback function.
///        (Messages are received within the framework, stored in the application handles (hApp),
///        and then executed by the application callback functions.)
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Sync
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////////////
UI_32 FrameworkunifiedGetMsgProtocol(HANDLE hApp);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedGetMsgDataOfSize
/// \~english @par Brief
///        API to get the data of the received message
/// \~english @param [in] hApp
///        HANDLE   - Handle for the application
/// \~english @param [out] pData
///        PVOID   - Pointer to the buffer where the received message data will be copied
/// \~english @param [in] uiSize
///        UI_32   - Size of the buffer to which received message data is copied
/// \~english @param [in] eRetrieveMethod
///        ESMRetrieveTypes  - Shared memory-acquisition-type (optional; eSMRRelease if not specified)
/// \~english @par
///       enum ESMRetrieveTypes variables
///       - eSMRRelease = 0 : The shared memory data is deleted after acquisition.
///       - eSMRRetain = 1 : Shared memory data is maintained.
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusNullPointer Specifying NULL pointers
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer size
/// \~english @retval eFrameworkunifiedStatusInvldBuf Bad buffer
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid handle type
/// \~english @retval eFrameworkunifiedStatusErrOther Other errors
/// \~english @par Prerequisite
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [eFrameworkunifiedStatusNullPointer]
///       - If the hApp specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusNullPointer]
///       - When the data size is larger than the buffer size (uiSize) specified by the parameter, [eFrameworkunifiedStatusInvldBufSize]
///       - If the buffer (pData) specified in the parameter is NULL, [eFrameworkunifiedStatusInvldBuf]
///       - Incoming messages are incorrect [eFrameworkunifiedStatusInvldHndlType]
///       - If the shared memory ID specified in the received message is invalid [FrameworkunifiedStatusErrOther]
///       - If accessing shared memory (shm_open/fstat/mmap) fails [eFrameworkunifiedStatusErrOther]
///       - If the size of the data in shared memory is larger than the buffer size (uiSize) specified by the arguments [FrameworkunifiedStatusErrOther]
/// \~english @par Detail
///        Receive message data is acquired from the receive buffer in the application handle (hApp) or the shared memory (message is stored in the shared memory when the size is 4096 bytes or more), and is stored in the area passed in the parameter pData.\n
///        When data exists in the shared memory, the data is held or deleted in the shared memory by specifying the eRetrieveMethod of arguments of this API
/// \~english @par
///        Please note the following points when using this API for application.\n
///        - Execute this API using an application handle passed as an argument of a callback function.
///          (Messages are received within the framework, stored in the application handles (hApp), and then executed by the application callback functions.)
///        - Before executing this API, the size of the received message must be checked by the FrameworkunifiedGetMsgLength to determine whether data is to be retained in the shared memory.
///        - If it is kept in shared memory, the application must execute the FrameworkunifiedClearMsgData and delete it.
///        - If an invalid value other than the ESMRetrieveTypes defined value is specified for the shared memory allocation type (eRetrieveMethod), the shared memory allocation type behaves as if eSMRRetain were specified.
/// \~english @par Classification
///          Public
/// \~english @par Type
///        Sync
/// \~english @see FrameworkunifiedSendMsg, FrameworkunifiedGetMsgLength, FrameworkunifiedClearMsgData
///
/// \param [in] hApp
///         HANDLE - Application framework handle
/// \param [out] pData
///         PVOID - Buffer to which the received message data will be copied.
/// \param [out] uiSize
///         UI_32 - Size of the buffer to which the received message data will be copied.
/// \param [in] eRetrieveMethod
///         ESMRetrieveTypes -
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \~english @par Type
///        Sync
/// \see FrameworkunifiedMcOpenSender, FrameworkunifiedMcClose, FrameworkunifiedSendMsg, FrameworkunifiedGetMsgLength, FrameworkunifiedGetMsgSrc,
///      FrameworkunifiedClearMsgData, FrameworkunifiedGetDataUSID, FrameworkunifiedForwardMessage, FrameworkunifiedGetSystemInfo
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedGetMsgDataOfSize(HANDLE hApp, PVOID pData, UI_32 uiSize, ESMRetrieveTypes eRetrieveMethod = eSMRRelease);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedGetDataPointer
/// \~english @par Brief
///        API to get start address of data from received message
/// \~english @param [in] hApp
///        HANDLE   - Handle for the application
/// \~english @param [out] datap
///        void**   - Pointer to the data of the received message
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer size
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @par Prerequisite
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - If the hApp specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusInvldHandle]
///       - When the receive buffer address of the handle (hApp) specified in the parameter is NULL [eFrameworkunifiedStatusInvldBufSize]
///       - When the pointer (datap) to the data of the received message specified by the parameter is NULL, [eFrameworkunifiedStatusInvldParam]
/// \~english @par Detail
///        This function gets the received message data from the receive buffer in the application handle (hApp)
///        and returns the start address of the received message in the argument datap.\n
/// \~english @par
///        Please note the following points when using this API for application.\n
///        - This API can be executed for messages that do not use shared memory (messages with a message size less than 4096 bytes).
///          Check the size of received messages in the FrameworkunifiedGetMsgLength prior to executing this API.
///        - Since the pointers acquired by this API are part of the Dispatcher, the pointers acquired by this API must not be released by the application.
///        - Execute this API using an application handle passed as an argument of a callback function.
///          (Messages are received within the framework, stored in the application handles (hApp), and then executed by the application callback functions.)
/// \~english @par Classification
///          Public
/// \~english @par Type
///        Sync
/// \~english @see FrameworkunifiedSendMsg, FrameworkunifiedGetMsgLength, FrameworkunifiedGetMsgDataOfSize
///
/// \param [in] hApp
///         HANDLE - Application framework handle
/// \param [out] datap
///         void ** - Pointer to which the receive message data.
///
/// \return status
///         EFrameworkunifiedStatus - success or error
/// \~english @par Type
///        Sync
///
/// \see FrameworkunifiedMcOpenSender, FrameworkunifiedMcClose, FrameworkunifiedSendMsg, FrameworkunifiedGetMsgLength, FrameworkunifiedGetMsgDataOfSize,
///      FrameworkunifiedClearMsgData, FrameworkunifiedGetDataUSID, FrameworkunifiedForwardMessage, FrameworkunifiedGetSystemInfo
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedGetDataPointer(HANDLE hApp, void **datap);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedGetMsgSrc
/// \~english @par Brief
///        API to get the application name of the sender of the received message
/// \~english @param [in] hApp
///        HANDLE   - Handle for the application
/// \~english @retval PCSTR Name of the application from which the message was sent
/// \~english @retval NULL NULL pointer (Acquisition failure)
/// \~english @par Prerequisite
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [NULL]
///       - If the hApp specified in the arguments is not appropriate (not valid) [NULL]
/// \~english @par Detail
///        This API returns the application name of the sender of the received message.\n
///        The application name acquired by this API is used for establishing sessions, creating message queues for responses,
///        and registering/releasing callbacks to the Dispatcher.
/// \~english @par
///        Please note the following points when using this API for application.\n
///        - Execute this API using an application handle passed as an argument of a callback function.
///          (Messages are received within the framework, stored in the application handles (hApp), and then executed by the application callback functions.)
///        - Since the pointers acquired by this API are part of the Dispatcher, the pointers acquired by this API must not be released by the application.
/// \~english @par Classification
///          Public
/// \~english @par Type
///        Sync
/// \~english @see FrameworkunifiedOpenService, FrameworkunifiedCreateSession, FrameworkunifiedMcOpenSender, \n
///            FrameworkunifiedAttachCallbackToDispatcher, FrameworkunifiedAttachCallbacksToDispatcher, \n
///            FrameworkunifiedDetachCallbackFromDispatcher, FrameworkunifiedDetachCallbacksFromDispatcher, FrameworkunifiedDetachServiceFromDispatcher
///
/// Source of the message.
///
/// \param [in] hApp
///         HANDLE - Application framework handle
///
/// \return pSourceName
///         PCSTR - Source name of the message.
/// \~english @par Type
///        Sync
///
/// \see FrameworkunifiedMcOpenSender, FrameworkunifiedMcClose, FrameworkunifiedSendMsg, FrameworkunifiedGetMsgLength, FrameworkunifiedGetMsgDataOfSize,
///      FrameworkunifiedClearMsgData, FrameworkunifiedGetDataUSID, FrameworkunifiedForwardMessage, FrameworkunifiedGetSystemInfo
////////////////////////////////////////////////////////////////////////////////////////////
PCSTR FrameworkunifiedGetMsgSrc(HANDLE hApp);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedGetLastNotification
/// \~english @par Brief
///        Get the most recent Notification names
/// \~english @param [in] hApp
///        HANDLE   - Handle for the application
/// \~english @retval PCSTR Pointers to Notification
/// \~english @retval NULL NULL pointer (Acquisition failure)
/// \~english @par Prerequisite
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [NULL]
///       - If the hApp specified in the arguments is not appropriate (not valid) [NULL]
/// \~english @par Detail
///        This API acquires and returns the most recent Notification name from the received data.
/// \~english @par
///        Please note the following points when using this API for application.\n
///        - Execute this API using an application handle passed as an argument of a callback function.
///          (Messages are received within the framework, stored in the application handles (hApp), and then executed by the application callback functions.)
///        - Since the pointers acquired by this API are part of the Dispatcher, the pointers acquired by this API must not be released by the application.
/// \~english @par Classification
///          Public
/// \~english @par Type
///        Sync
/// \~english @see FrameworkunifiedNPPublishNotification
///
/// Returns the last notification string that was received.
///
/// \param [in] hApp
///         HANDLE - Application framework handle
///
/// \return pSourceName
///         PCSTR - the last notification name that has been received.
/// \~english @par Type
///        Sync
///
/// \see FrameworkunifiedMcOpenSender, FrameworkunifiedMcClose, FrameworkunifiedSendMsg, FrameworkunifiedGetMsgLength, FrameworkunifiedGetMsgDataOfSize,
///      FrameworkunifiedClearMsgData, FrameworkunifiedGetDataUSID, FrameworkunifiedForwardMessage, FrameworkunifiedGetSystemInfo
////////////////////////////////////////////////////////////////////////////////////////////
PCSTR FrameworkunifiedGetLastNotification(HANDLE hApp);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedClearMsgData
/// \~english @par Brief
///        API to delete shared memory used for sending and receiving messages
/// \~english @param [in] hApp
///        HANDLE   - Handle for the application
/// \~english @retval eFrameworkunifiedStatusOK Successful or Use Shared Memory Messages
/// \~english @retval eFrameworkunifiedStatusNullPointer Specifying NULL pointers
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusInvldID Invalid ID
/// \~english @retval eFrameworkunifiedStatusFail Some error occurred
/// \~english @retval eFrameworkunifiedStatusInvldBuf Bad buffer
/// \~english @par Prerequisite
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [eFrameworkunifiedStatusNullPointer]
///       - If the hApp specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusNullPointer]
///       - Framework generating failure [eFrameworkunifiedStatusInvldParam]
///       - If the shared memory ID specified in the message is invalid [eFrameworkunifiedStatusInvldParam]
///       - If an attempt is made to remove a non-existent shared memory, [eFrameworkunifiedStatusInvldID]
///       - If accessing shared memory (shm_unlink) fails, [eFrameworkunifiedStatusFail]
///       - When the receive buffer address of the handle (hApp) specified in the parameter is NULL [eFrameworkunifiedStatusInvldBuf]
/// \~english @par Detail
///        This API deletes the shared memory used for sending and receiving messages.\n
///        When an application gets received messages by specifying eSMRRetain as FrameworkunifiedGetMsgDataOfSize, it deletes the shared memories by using this API...\n
/// \~english @par
///        Please note the following points when using this API for application.\n
///        - The shared memory information to be deleted by this API shall be executed by the application on the message receiving side because it is obtained from the received message.\n
///        - Execute this API using an application handle passed as an argument of a callback function.
///          (Messages are received within the framework, stored in the application handles (hApp), and then executed by the application callback functions.)
/// \~english @par Classification
///          Public
/// \~english @par Type
///        Sync
/// \~english @see FrameworkunifiedSendMsg, FrameworkunifiedGetMsgDataOfSize
///
/// Explicit call to clear the data from shared memory.
/// To be used incase of forwarding messages ???
///
/// \param [in] hApp
///         HANDLE - Application framework handle
///
/// \return status
///         EFrameworkunifiedStatus - success or error
/// \~english @par Type
///        Sync
///
/// \see FrameworkunifiedMcOpenSender, FrameworkunifiedMcClose, FrameworkunifiedSendMsg, FrameworkunifiedGetMsgLength, FrameworkunifiedGetMsgSrc, FrameworkunifiedGetMsgDataOfSize,
///      FrameworkunifiedGetDataUSID, FrameworkunifiedForwardMessage, FrameworkunifiedGetSystemInfo
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedClearMsgData(HANDLE hApp);

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedForwardMessage
/// Forward a message to a service or a client.
///
/// \param [in] hApp
///         HANDLE - Handle to the service
/// \param [in] hChildQ
///         HANDLE - Handle to the child thread
/// \param [in] iCmd
///         UI_32 - .
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see FrameworkunifiedMcOpenSender, FrameworkunifiedMcClose, FrameworkunifiedSendMsg, FrameworkunifiedGetMsgLength, FrameworkunifiedGetMsgSrc, FrameworkunifiedGetMsgDataOfSize,
///      FrameworkunifiedClearMsgData, FrameworkunifiedGetDataUSID, FrameworkunifiedGetSystemInfo
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedForwardMessage(HANDLE hApp, HANDLE hChildQ, UI_32 iCmd);







////////////////////////////////////////////////////////////////////////////////////////////
//        Notification & Persistence Service
////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedNPRegisterNotification
/// \~english @par Brief
///       To the notification service to register the Notification, API to send a message.
/// \~english @param [in] hApp
///        HANDLE   - HANDLE for Application
/// \~english @param [in] pNotification
///        PCSTR    - A pointer to the Notification name
/// \~english @param [in] max_length
///        UI_32    - The maximum length of the Notification message (64byte below)
/// \~english @param [in] persType
/// \~english @par
///        EFrameworkunifiedNotificationType     - Type of Notification
///       enum EFrameworkunifiedNotificationType Variable
///       - eFrameworkunifiedNotificationVar  : Non Persistent. Not stored locally by NPS.
///       - eFrameworkunifiedStateVar : Non Persistent. stored locally by NPS.
///       - eFrameworkunifiedPersistedStateVar : Not to be used. Persistent. Also stored locally by NPS.
///       - eFrameworkunifiedPersistedStateUserVar : Not to be used. User specific Persistent. Also stored locally by NPS.
///       - eFrameworkunifiedImmediatePersistedStateVar : Not to be used.
///       - eFrameworkunifiedUnknown : Not to be used. This is not a type of notification.
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid type of handle
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
/// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
/// \~english @retval eFrameworkunifiedStatusErrOther Other error has occurred(Cannot access shared memory, etc.)
/// \~english @par Prerequisite
///       - NS_NPPService of the process has been started.
///       - Generation/Initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been
///       done.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - Pointer (pNotification) is NULL to the Notification name specified. [eFrameworkunifiedStatusInvldParam]
///       - HANDLE specified in the argument (hApp) is NULL. [eFrameworkunifiedStatusInvldParam]
///       - HANDLE specified in the argument (hApp) is not appropriate (which is invalid). [eFrameworkunifiedStatusInvldParam]
///       - It is not appropriate length of the specified Notification name in argument(0, or more than a 64byte).
///       [eFrameworkunifiedStatusInvldParam]
///       - Can not be acquired memory in the acquisition of the buffer of the message for transmission (malloc).
///       [eFrameworkunifiedStatusFail]
///       - Message queue HANDLE in HANDLE specified in the argument (hApp) is NULL. [eFrameworkunifiedStatusInvldParam]
///       - HANDLE specified in the argument message queue HANDLE in (hApp) is not appropriate (which is invalid).
///       [eFrameworkunifiedStatusInvldParam]
///       - Message queue name of HANDLE specified in the argument (hApp) is not appropriate(The name is NULL, more than
///       20byte) [eFrameworkunifiedStatusInvldHandle]
///       - Message queue type of HANDLE specified in the argument (hApp) is not a transmission.
///       [eFrameworkunifiedStatusInvldHndlType]
///       - Session message queue is full to the NPP Service. [eFrameworkunifiedStatusMsgQFull]
///       - It is invalid transmission file descriptor of the session message to the NPP Service. [eFrameworkunifiedStatusErrNoEBADF]
///       - Interruption by the system call (signal) has occurred during the transmission of the session message for the
///       NPP Service. [eFrameworkunifiedStatusErrNoEINTR]
///       - Incorrect size of the transmit buffer of the session message to the NPP Service. [eFrameworkunifiedStatusInvldBufSize]
///       - Any errors occur during the transmission of a session message to the NPP Service. [eFrameworkunifiedStatusFail]
///       - It failed to shared memory access for transmission of the session message for the NPP Service.
///       [eFrameworkunifiedStatusErrOther]
/// \~english @par Detail
///       To the notification service (NS_NPPService) to register the Notification, \n
///       send a message for the Notification registration.
/// \~english @par Classification
///          Public
/// \~english @par Type
///         Pub-Sub
/// \~english @see  FrameworkunifiedNPRegisterNotifications, FrameworkunifiedNPUnRegisterNotification, FrameworkunifiedNPUnRegisterNotifications,
///                 FrameworkunifiedNPPublishNotification, FrameworkunifiedSubscribeNotificationWithCallback,
///                 FrameworkunifiedUnsubscribeNotificationWithCallback,
///                 FrameworkunifiedSubscribeNotificationsWithCallback, FrameworkunifiedUnsubscribeNotificationsWithCallback
////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPRegisterNotification(HANDLE hApp, PCSTR pNotification,  const UI_32 max_length,
                                     const EFrameworkunifiedNotificationType persType);

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPSetPersistNotfnDefaultValue
/// API to send message to Notification Service to set the default value for the notification.
///
/// \param [in] hApp
///         HANDLE - Handle to the Framework
/// \param [in] pNotification
///         PCSTR - Name of Notification
/// \param [in] data
///         PVOID - Set the default value for the notification.
/// \param [in] uiLength
///         const UI_32 - length of the data
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPSetPersistNotfnDefaultValue(HANDLE hApp, PCSTR pNotification, PVOID data, const UI_32 uiLength);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedNPSetPersistentNotfnType
/// \~english @par Brief
///       API to send message to Notification Service to set persistent notification category.
/// \~english @param [in] hApp
///       HANDLE - Handle to the Framework
/// \~english @param [in] pNotification
///       PCSTR - Name of Notification
/// \~english @param [in] ePersistCategory
///         EFrameworkunifiedPersistCategory - Persistent Category
/// \~english @return status
///       EFrameworkunifiedStatus - success or error
/// \~english @code
///                 typedef enum _EFrameworkunifiedNotificationType
///                 {
///                       eFrameworkunifiedNotificationVar = 0,        /* Not persistent (not retained by NPS) */
///                       eFrameworkunifiedStateVar,                   /* Not persistent (held by NPS) */
///                       eFrameworkunifiedPersistedStateVar,          /* persistent. Only kept in NPS */
///                       eFrameworkunifiedPersistedStateUserVar,      /* Invalid handle type. Only kept in NPS */
///                       eFrameworkunifiedImmediatePersistedStateVar, /* Invalid handle. See \ref
///                                                       FrameworkunifiedNPRegisterImmediatePersistNotification */
///                       eFrameworkunifiedUnknown                     /* No Notification typing. Not recommended */
///                 } EFrameworkunifiedNotificationType;
///            @endcode
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusNullPointer Specifying NULL pointers
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid handle type
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid file descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR Generating an interrupt by a system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer size
/// \~english @retval eFrameworkunifiedStatusFail Some error occurred
/// \~english @par Prerequisite
///       - The process of NS_NPPService is started.
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [eFrameworkunifiedStatusInvldParam]
///       - If the hApp specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusInvldParam]
///       - When the message queue information handle specified in the argument (hApp) is NULL, [eFrameworkunifiedStatusNullPointer]
///       - When the message queue handle in the handle (hApp) specified by the arguments is NULL, [eFrameworkunifiedStatusInvldParam]
///       - When the application-name storage area address in the handle (hApp) specified by the arguments is NULL, [eFrameworkunifiedStatusInvldParam]
///       - If the application name in the handle (hApp) specified by the arguments is empty, [eFrameworkunifiedStatusInvldParam]
///       - When the message queue type of the handle (hApp) specified in the parameter is not the send type, [eFrameworkunifiedStatusInvldHndlType]
///       - If the message queue handle in the hApp specified by the arguments is inappropriate (not valid) [eFrameworkunifiedStatusInvldHandle]
///       - The message queue name of the handle (hApp) specified in the arguments is not appropriate (the name exceeds NULL, 20 bytes) [eFrameworkunifiedStatusInvldHandle]
///       - Full Message Queue for Sessions for NPP Service [eFrameworkunifiedStatusMsgQFull]
///       - Incorrect file descriptors for sending messages for NPP Service sessions [eFrameworkunifiedStatusErrNoEBADF]
///       - When an interrupt by a system call (signal) occurs while sending NPP Service session-specific messages [eFrameworkunifiedStatusErrNoEINTR]
///       - Incorrectly sized send buffers for sessions destined for NPP Service [eFrameworkunifiedStatusInvldBufSize]
///       - If there are any errors sending messages for sessions intended for NPP Service [eFrameworkunifiedStatusFail]
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Pub-Sub
/// \~english @see  none
///
////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPSetPersistentNotfnType(HANDLE hApp, PCSTR pNotification, EFrameworkunifiedPersistCategory ePersistCategory);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedNPRegisterImmediatePersNotification
/// \~english @par Brief
///       API to send message to Notification Service to register a immediate notification
/// \~english @param [in] hApp
///         HANDLE - Handle to the Framework
/// \~english @param [in] pNotification
///         PCSTR - Name of Notification
/// \~english @param [in] max_length
///         const UI_32 - Max size of the notification message (should be <= 1Kb)
/// \~english @param [in] delay
///         const UI_32 - Min time interval between every persist (in sec)
/// \~english @return status
///         EFrameworkunifiedStatus - success or error
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid handle type
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid file descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR Generating an interrupt by a system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer size
/// \~english @retval eFrameworkunifiedStatusFail Some error occurred
/// \~english @par Prerequisite
///       - The process of NS_NPPService is started.
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the pNotification to the Notification specified in the parameter is NULL, [eFrameworkunifiedStatusInvldParam]
///       - If the hApp specified in the parameter is NULL, [eFrameworkunifiedStatusInvldParam]
///       - If the hApp specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusInvldParam]
///       - When the length of the Notification specified by the arguments is 0, [eFrameworkunifiedStatusInvldParam]
///       - If the length of the Notification specified in the arguments is 64 (MAX_STRING_SIZE_NOTIFICATION) or greater [eFrameworkunifiedStatusInvldParam]
///       - Failed to get buffers to store Notification [eFrameworkunifiedStatusInvldParam]
///       - When memory could not be allocated due to the allocation of message-sending buffers (malloc),... [eFrameworkunifiedStatusFail]
///       - When the message queue type of the handle (hApp) specified in the parameter is not the send type, [eFrameworkunifiedStatusInvldHndlType]
///       - When the message queue handle in the handle (hApp) specified by the arguments is NULL, [eFrameworkunifiedStatusInvldParam]
///       - If the message queue handle in the hApp specified by the arguments is inappropriate (not valid) [eFrameworkunifiedStatusInvldParam]
///       - The message queue name of the handle (hApp) specified in the arguments is not appropriate (the name exceeds NULL, 20 bytes) [eFrameworkunifiedStatusInvldHandle]
///       - Full Message Queue for Sessions for NPP Service [eFrameworkunifiedStatusMsgQFull]
///       - Incorrect file descriptors for sending messages for NPP Service sessions [eFrameworkunifiedStatusErrNoEBADF]
///       - When an interrupt by a system call (signal) occurs while sending NPP Service session-specific messages [eFrameworkunifiedStatusErrNoEINTR]
///       - Incorrectly sized send buffers for sessions destined for NPP Service [eFrameworkunifiedStatusInvldBufSize]
///       - If there are any errors sending messages for sessions intended for NPP Service [eFrameworkunifiedStatusFail]
/// \~english @par Detail
///       This API registers a Notification to the notification service (NS_NPPService).\n
///       By sending a Notification registration message to the notification service, the specified Notification is registered in the received notification service.
/// \~english @par Classification
///         Public
/// \~english @par Type
///         Pub-Sub
/// \~english @see  FrameworkunifiedNPRegisterNotifications, FrameworkunifiedNPUnRegisterNotification, FrameworkunifiedNPUnRegisterNotifications,
///                 FrameworkunifiedNPPublishNotification, FrameworkunifiedSubscribeNotificationWithCallback,
///                 FrameworkunifiedUnsubscribeNotificationWithCallback,FrameworkunifiedSubscribeNotificationsWithCallback,
///                 FrameworkunifiedUnsubscribeNotificationsWithCallback
////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPRegisterImmediatePersistNotification(HANDLE hApp, PCSTR pNotification,  const UI_32 max_length,
                                                     const UI_32 delay);


////////////////////////////////////////////////////////////////////////////////////////////
///  FrameworkunifiedNPPersistentSync
///  Processing which synchronizes by NPPService (syncfs)
///
/// \param [in] hApp
///         HANDLE - Handle to the Framework
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPPersistentSync(HANDLE hApp);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedNPRegisterNotifications
/// \~english @par Brief
///       API to send a message to the notification service in order to register multiple Notification of list format.
/// \~english @param [in] hApp
///        HANDLE   - HANDLE for Application
/// \~english @param [in] pList
///        FrameworkunifiedNotificationsList   - Pointer to the Notification list to be registered
/// \~english @param [in] uiListLength
///        UI_32    - List length
/// \~english @par
///        FrameworkunifiedNotificationsList Structure
/// \~english @code
///                 #define MAX_STRING_SIZE_NOTIFICATION  64
///                 typedef struct _FrameworkunifiedNotificationsList
///                 {
///                       CHAR cNotification[MAX_STRING_SIZE_NOTIFICATION]; /* Notification Name                      */
///                       UI_32 uiLengthData;                               /* Notification Name length(64byte below) */
///                       EFrameworkunifiedNotificationType  persType;                   /* Notification Type                      */
///                 }
///            @endcode
/// \~english @par
///       enum EFrameworkunifiedNotificationType Variable
///       - eFrameworkunifiedNotificationVar  : Non Persistent. Not stored locally by NPS.
///       - eFrameworkunifiedStateVar : Non Persistent. stored locally by NPS.
///       - eFrameworkunifiedPersistedStateVar : Not to be used. Persistent. Also stored locally by NPS.
///       - eFrameworkunifiedPersistedStateUserVar : Not to be used. User specific Persistent. Also stored locally by NPS.
///       - eFrameworkunifiedImmediatePersistedStateVar : Not to be used.
///       - eFrameworkunifiedUnknown : Not to be used. This is not a type of notification.
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid type of handle
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
/// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
/// \~english @retval eFrameworkunifiedStatusErrOther Other error has occurred(Cannot access shared memory, etc.)
/// \~english @par Prerequisite
///       - NS_NPPService of the process has been started.
///       - Generation/Initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been
///       done.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - A pointer to the Notification list to register specified in the argument (pList) is NULL.
///       [eFrameworkunifiedStatusInvldParam]
///       - HANDLE specified in the argument (hApp) is NULL. [eFrameworkunifiedStatusInvldParam]
///       - HANDLE specified in the argument (hApp) is not appropriate (which is invalid). [eFrameworkunifiedStatusInvldParam]
///       - And fail to acquire the buffer for storing a plurality Notification. [eFrameworkunifiedStatusInvldParam]
///       - List specified in the argument length (uiListLength) has exceeded the maximum number of Notification.
///       [eFrameworkunifiedStatusFail]
///       - Can not be acquired memory in the acquisition of the buffer of the message for transmission (malloc).
///       [eFrameworkunifiedStatusFail]
///       - Message queue HANDLE in HANDLE specified in the argument (hApp) is NULL. [eFrameworkunifiedStatusInvldParam]
///       - HANDLE specified in the argument message queue HANDLE in (hApp) is not appropriate (which is invalid).
///       [eFrameworkunifiedStatusInvldParam]
///       - Message queue name of HANDLE specified in the argument (hApp) is not appropriate(The name is NULL, more than
///       20byte) [eFrameworkunifiedStatusInvldHandle]
///       - Message queue type of HANDLE specified in the argument (hApp) is not a transmission.
///       [eFrameworkunifiedStatusInvldHndlType]
///       - Session message queue is full to the NPP Service. [eFrameworkunifiedStatusMsgQFull]
///       - It is invalid transmission file descriptor of the session message to the NPP Service. [eFrameworkunifiedStatusErrNoEBADF]
///       - Interruption by the system call (signal) has occurred during the transmission of the session message for the
///       NPP Service. [eFrameworkunifiedStatusErrNoEINTR]
///       - Incorrect size of the transmit buffer of the session message to the NPP Service. [eFrameworkunifiedStatusInvldBufSize]
///       - Any errors occur during the transmission of a session message to the NPP Service. [eFrameworkunifiedStatusFail]
///       - It failed to shared memory access for transmission of the session message for the NPP Service.
///       [eFrameworkunifiedStatusErrOther]
/// \~english @par Detail
///       This API is to register multiple Notification of list format to the notification service (NS_NPPService).\n
///       By transmitting multiple Notification registration message to the notification service, Notification \n
///       specified in the received notification service is registered. \n
///       It sends a message for multiple Notification registered with the notification service (NS_NPPService) \n
///       in order to register multiple Notification of list format.
/// \~english @par Classification
///          Public
/// \~english @see  FrameworkunifiedNPRegisterNotification, FrameworkunifiedNPUnRegisterNotification, FrameworkunifiedNPUnRegisterNotifications,
///                 FrameworkunifiedNPPublishNotification, FrameworkunifiedSubscribeNotificationWithCallback,
///                 FrameworkunifiedUnsubscribeNotificationWithCallback,
///                 FrameworkunifiedSubscribeNotificationsWithCallback, FrameworkunifiedUnsubscribeNotificationsWithCallback
////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPRegisterNotifications(HANDLE hApp, const  FrameworkunifiedNotificationsList *pList, UI_32 uiListLength);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedNPUnRegisterNotification
/// \~english @par Brief
///       API to send message to Notification Service to remove a notification.
/// \~english @param [in] hApp
///        HANDLE   - HANDLE for Application
/// \~english @param [in] pNotification
///        PCSTR    - A pointer to the Notification name
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid type of handle
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
/// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
/// \~english @retval eFrameworkunifiedStatusErrOther Other error has occurred(Cannot access shared memory, etc.)
/// \~english @par Prerequisite
///       - NS_NPPService of the process has been started.
///       - Generation/Initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been
///       done.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - Pointer (pNotification) is NULL to the Notification name specified. [eFrameworkunifiedStatusInvldParam]
///       - HANDLE specified in the argument (hApp) is NULL. [eFrameworkunifiedStatusInvldParam]
///       - HANDLE specified in the argument (hApp) is not appropriate (which is invalid). [eFrameworkunifiedStatusInvldParam]
///       - It fails to get the buffer that stores the Notification. [eFrameworkunifiedStatusInvldParam]
///       - It is not appropriate length of the specified Notification name in argument(0, or more than a 64byte).
///       [eFrameworkunifiedStatusInvldParam]
///       - Can not be acquired memory in the acquisition of buffer for internal processing (malloc). [eFrameworkunifiedStatusFail]
///       - Can not be acquired memory in the acquisition of the buffer of the message for transmission (malloc).
///       [eFrameworkunifiedStatusFail]
///       - Message queue type of HANDLE specified in the argument (hApp) is not a transmission.
///       [eFrameworkunifiedStatusInvldHndlType]
///       - Message queue HANDLE in HANDLE specified in the argument (hApp) is NULL. [eFrameworkunifiedStatusInvldParam]
///       - HANDLE specified in the argument message queue HANDLE in (hApp) is not appropriate (which is invalid).
///       [eFrameworkunifiedStatusInvldParam]
///       - Message queue name of HANDLE specified in the argument (hApp) is not appropriate(The name is NULL, more than
///       20byte) [eFrameworkunifiedStatusInvldHandle]
///       - Session message queue is full to the NPP Service. [eFrameworkunifiedStatusMsgQFull]
///       - It is invalid transmission file descriptor of the session message to the NPP Service. [eFrameworkunifiedStatusErrNoEBADF]
///       - Interruption by the system call (signal) has occurred during the transmission of the session message for the
///       NPP Service. [eFrameworkunifiedStatusErrNoEINTR]
///       - Incorrect size of the transmit buffer of the session message to the NPP Service. [eFrameworkunifiedStatusInvldBufSize]
///       - Any errors occur during the transmission of a session message to the NPP Service. [eFrameworkunifiedStatusFail]
///       - It failed to shared memory access for transmission of the session message for the NPP Service.
///       [eFrameworkunifiedStatusErrOther]
/// \~english @par Detail
///       This API is, to remove the Notification from the notification service (NS_NPPService). \n
///       By deleting, Notification specified from the notification service will no longer be delivered.
/// \~english @par Classification
///          Public
/// \~english @see  FrameworkunifiedNPRegisterNotification, FrameworkunifiedNPRegisterNotifications, FrameworkunifiedNPUnRegisterNotifications,
///                 FrameworkunifiedNPPublishNotification, FrameworkunifiedSubscribeNotificationWithCallback,
///                 FrameworkunifiedUnsubscribeNotificationWithCallback,
///                 FrameworkunifiedSubscribeNotificationsWithCallback, FrameworkunifiedUnsubscribeNotificationsWithCallback
////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPUnRegisterNotification(HANDLE hApp, PCSTR pNotification);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedNPUnRegisterNotifications
/// \~english @par Brief
///       API to send message to Notification Service to delete a notification.
/// \~english @param [in] hApp
///        HANDLE   - HANDLE for Application
/// \~english @param [in] pList
///        FrameworkunifiedNotificationsList   - Pointer to the Notification list that you want to delete
/// \~english @param [in] uiListLength
///        UI_32    - List length
/// \~english @par
///        FrameworkunifiedNotificationsList Structure
/// \~english @code
///                 #define MAX_STRING_SIZE_NOTIFICATION  64
///                 typedef struct _FrameworkunifiedNotificationsList
///                 {
///                       CHAR cNotification[MAX_STRING_SIZE_NOTIFICATION]; /* Notification Name                      */
///                       UI_32 uiLengthData;                               /* Notification Name length(64byte below) */
///                       EFrameworkunifiedNotificationType  persType;                   /* Notification Type                      */
///                 }
///            @endcode
/// \~english @par
///       enum EFrameworkunifiedNotificationType Variable
///       - eFrameworkunifiedNotificationVar  : Non Persistent. Not stored locally by NPS.
///       - eFrameworkunifiedStateVar : Non Persistent. stored locally by NPS.
///       - eFrameworkunifiedPersistedStateVar : Not to be used. Persistent. Also stored locally by NPS.
///       - eFrameworkunifiedPersistedStateUserVar : Not to be used. User specific Persistent. Also stored locally by NPS.
///       - eFrameworkunifiedImmediatePersistedStateVar : Not to be used.
///       - eFrameworkunifiedUnknown : Not to be used. This is not a type of notification.
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusNullPointer NULL pointer specified
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid type of handle
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
/// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
/// \~english @retval eFrameworkunifiedStatusErrOther Other error has occurred(Cannot access shared memory, etc.)
/// \~english @par Prerequisite
///       - NS_NPPService of the process has been started.
///       - Generation/Initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been
///       done.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - HANDLE specified in the argument (hApp) is NULL. [eFrameworkunifiedStatusInvldParam]
///       - HANDLE specified in the argument (hApp) is not appropriate (which is invalid). [eFrameworkunifiedStatusInvldParam]
///       - Message queue HANDLE in HANDLE specified in the argument (hApp) is NULL. [eFrameworkunifiedStatusNullPointer]
///       - It fails to get the buffer that stores multiple Notification. [eFrameworkunifiedStatusInvldParam]
///       - List specified in the argument length (uiListLength) has exceeded the maximum number of Notification.
///       [eFrameworkunifiedStatusFail]
///       - Can not be acquired memory in the acquisition of buffer for internal processing (malloc). [eFrameworkunifiedStatusFail]
///       - Can not be acquired memory in the acquisition of the buffer of the message for transmission (malloc).
///       [eFrameworkunifiedStatusFail]
///       - Message queue type of HANDLE specified in the argument (hApp) is not a transmission.
///       [eFrameworkunifiedStatusInvldHndlType]
///       - HANDLE specified in the argument message queue HANDLE in (hApp) is not appropriate (which is invalid).
///       [eFrameworkunifiedStatusInvldParam]
///       - Message queue name of HANDLE specified in the argument (hApp) is not appropriate(The name is NULL, more than
///       20byte) [eFrameworkunifiedStatusInvldHandle]
///       - Session message queue is full to the NPP Service. [eFrameworkunifiedStatusMsgQFull]
///       - It is invalid transmission file descriptor of the session message to the NPP Service. [eFrameworkunifiedStatusErrNoEBADF]
///       - Interruption by the system call (signal) has occurred during the transmission of the session message for the
///       NPP Service. [eFrameworkunifiedStatusErrNoEINTR]
///       - Incorrect size of the transmit buffer of the session message to the NPP Service. [eFrameworkunifiedStatusInvldBufSize]
///       - Any errors occur during the transmission of a session message to the NPP Service. [eFrameworkunifiedStatusFail]
///       - It failed to shared memory access for transmission of the session message for the NPP Service.
///       [eFrameworkunifiedStatusErrOther]
/// \~english @par Detail
///       This API is, delete multiple Notification of list format from the notification service (NS_NPPService). \n
///       By deleting, Notification specified from the notification service will no longer be delivered.
/// \~english @par Classification
///          Public
/// \~english @see  FrameworkunifiedNPRegisterNotification, FrameworkunifiedNPRegisterNotifications, FrameworkunifiedNPUnRegisterNotification,
///                 FrameworkunifiedNPPublishNotification, FrameworkunifiedSubscribeNotificationWithCallback,
///                 FrameworkunifiedUnsubscribeNotificationWithCallback,
///                 FrameworkunifiedSubscribeNotificationsWithCallback, FrameworkunifiedUnsubscribeNotificationsWithCallback
////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPUnRegisterNotifications(HANDLE hApp, const  FrameworkunifiedNotificationsList *pList,
                                        UI_32 uiListLength);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedNPPublishNotification
/// \~english @par Brief
///       API to send message to Notification Service to notify subscribers
/// \~english @param [in] hApp
///        HANDLE   - HANDLE for Application
/// \~english @param [in] pNotification
///        PCSTR    - A pointer to the Notification name
/// \~english @param [in] pData
///        VOID *   - Data buffer pointer
/// \~english @param [in] iLength
///        UI_32    - Size of data buffer
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid type of handle
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer size
/// \~english @retval eFrameworkunifiedStatusFail Some kind of error has occurred
/// \~english @retval eFrameworkunifiedStatusErrOther Other error has occurred(Cannot access shared memory, etc.)
/// \~english @par Prerequisite
///       - NS_NPPService of the process has been started.
///       - Generation/Initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been
///       done.
/// \~english @par Conditions of processing failure
///       - Pointer (pNotification) is NULL to the Notification name specified. [eFrameworkunifiedStatusInvldParam]
///       - HANDLE specified in the argument (hApp) is NULL. [eFrameworkunifiedStatusInvldParam]
///       - HANDLE specified in the argument (hApp) is not appropriate (which is invalid) [eFrameworkunifiedStatusInvldParam]
///       - Can not be acquired memory in the acquisition of the buffer of the message for transmission (malloc).
///       [eFrameworkunifiedStatusFail]
///       - Message queue HANDLE in HANDLE specified in the argument (hApp) is NULL. [eFrameworkunifiedStatusInvldParam]
///       - HANDLE specified in the argument message queue HANDLE in (hApp) is not appropriate (which is invalid).
///       [eFrameworkunifiedStatusInvldParam]
///       - Message queue name of HANDLE specified in the argument (hApp) is not appropriate(The name is NULL, more than
///       20byte) [eFrameworkunifiedStatusInvldHandle]
///       - Message queue type of HANDLE specified in the argument (hApp) is not a transmission.
///       [eFrameworkunifiedStatusInvldHndlType]
///       - Session message queue is full to the NPP Service. [eFrameworkunifiedStatusMsgQFull]
///       - It is invalid transmission file descriptor of the session message to the NPP Service. [eFrameworkunifiedStatusErrNoEBADF]
///       - Interruption by the system call (signal) has occurred during the transmission of the session message for the
///       NPP Service. [eFrameworkunifiedStatusErrNoEINTR]
///       - Incorrect size of the transmit buffer of the session message to the NPP Service. [eFrameworkunifiedStatusInvldBufSize]
///       - Any errors occur during the transmission of a session message to the NPP Service. [eFrameworkunifiedStatusFail]
///       - It failed to shared memory access for transmission of the session message for the NPP Service.
///       [eFrameworkunifiedStatusErrOther]
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Detail
///       Send a message to the notification service(NS_NPPService), and notifies the Subscriber.\n
///       - Publisher is required registration of Notification in advance FrameworkunifiedNPRegisterNotification.
///       - Publisher is possible cancellation of the Notification in FrameworkunifiedNPUnRegisterNotification.
///       - Subscriber need a reception procedure of Notification in advance FrameworkunifiedSubscribeNotification.
///       - Subscriber is capable of receiving cancellation of the Notification in FrameworkunifiedUnsubscribeNotification.
/// \~english @par Classification
///          Public
/// \~english @see  FrameworkunifiedNPRegisterNotification, FrameworkunifiedNPUnRegisterNotification
///                 FrameworkunifiedNPRegisterNotifications, FrameworkunifiedNPUnRegisterNotifications
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPPublishNotification(HANDLE hApp, PCSTR pNotification,
                                    PCVOID pData, UI_32 iLength);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedNPReadPersistedData
/// \~english @par Brief
///       API to requested the persistent data corresponding to the notification if available
/// \~english @param [in] hApp
///         HANDLE - Handle to the Framework
/// \~english @param [in] pNotification
///         PCSTR - Name of Notification
/// \~english @return status
///         EFrameworkunifiedStatus - success or error
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid handle type
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid file descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR Generating an interrupt by a system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer size
/// \~english @retval eFrameworkunifiedStatusFail Some error occurred
/// \~english @par Prerequisite
///       - The process of NS_NPPService is started.
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the pNotification to the Notification specified in the parameter is NULL, [eFrameworkunifiedStatusInvldParam]
///       - If the hApp specified in the parameter is NULL, [eFrameworkunifiedStatusInvldParam]
///       - If the hApp specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusInvldParam]
///       - When the length of the Notification specified by the arguments is 0, [eFrameworkunifiedStatusInvldParam]
///       - Failed to get buffers to store Notification [eFrameworkunifiedStatusInvldParam]
///       - When memory could not be allocated due to the allocation of message-sending buffers (malloc),... [eFrameworkunifiedStatusFail]
///       - When the message queue type of the handle (hApp) specified in the parameter is not the send type, [eFrameworkunifiedStatusInvldHndlType]
///       - When the message queue handle in the handle (hApp) specified by the arguments is NULL, [eFrameworkunifiedStatusInvldParam]
///       - If the message queue handle in the hApp specified by the arguments is inappropriate (not valid) [eFrameworkunifiedStatusInvldParam]
///       - The message queue name of the handle (hApp) specified in the arguments is not appropriate (the name exceeds NULL, 20 bytes) [eFrameworkunifiedStatusInvldHandle]
///       - Full Message Queue for Sessions for NPP Service [eFrameworkunifiedStatusMsgQFull]
///       - Incorrect file descriptors for sending messages for NPP Service sessions [eFrameworkunifiedStatusErrNoEBADF]
///       - When an interrupt by a system call (signal) occurs while sending NPP Service session-specific messages [eFrameworkunifiedStatusErrNoEINTR]
///       - Incorrectly sized send buffers for sessions destined for the NPP Service [eFrameworkunifiedStatusInvldBufSize]
///       - If there are any errors sending messages for sessions intended for NPP Service [eFrameworkunifiedStatusFail]
/// \~english @par Classification
///         Public
/// \~english @par Type
///         Pub-Sub
/// \~english @see  none
////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPReadPersistedData(HANDLE hApp, PCSTR pNotification);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  FrameworkunifiedNPRegisterPersistentFile
/// \~english @par Brief
///        APIs that send messages to notification services and register Persistent file tags
/// \~english @param [in] hApp
///        HANDLE   - Handle for the application
/// \~english @param [in] pTag
///        PCSTR    - Tag name
/// \~english @param [in] bIsUserFile
///        BOOL     - User specified (TRUE: user-specific/FALSE: all users) (Default:FALSE)
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusNullPointer Specifying NULL pointers
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid handle type
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid file descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR Generating an interrupt by a system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer size
/// \~english @retval eFrameworkunifiedStatusFail Some error occurred
/// \~english @par Prerequisite
///       - The process of NS_NPPService is started.
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [eFrameworkunifiedStatusInvldParam]
///       - If the hApp specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusInvldParam]
///       - When the message queue information handle specified in the argument (hApp) is NULL, [eFrameworkunifiedStatusNullPointer]
///       - When the message queue handle in the handle (hApp) specified by the arguments is NULL, [eFrameworkunifiedStatusInvldParam]
///       - When the application-name storage area address in the handle (hApp) specified by the arguments is NULL, [eFrameworkunifiedStatusInvldParam]
///       - If the application name in the handle (hApp) specified by the arguments is empty, [eFrameworkunifiedStatusInvldParam]
///       - When the tag name (pTag) specified in the arguments is NULL, [eFrameworkunifiedStatusInvldParam]
///       - When the tag name (pTag) specified in the arguments is an empty character, [eFrameworkunifiedStatusInvldParam]
///       - When the message queue type of the handle (hApp) specified in the parameter is not the send type, [eFrameworkunifiedStatusInvldHndlType]
///       - If the message queue handle in the hApp specified by the arguments is inappropriate (not valid) [eFrameworkunifiedStatusInvldHandle]
///       - The message queue name of the handle (hApp) specified in the arguments is not appropriate (the name exceeds NULL, 20 bytes) [eFrameworkunifiedStatusInvldHandle]
///       - Full Message Queue for Sessions for NPP Service [eFrameworkunifiedStatusMsgQFull]
///       - Incorrect file descriptors for sending messages for NPP Service sessions [eFrameworkunifiedStatusErrNoEBADF]
///       - When an interrupt by a system call (signal) occurs while sending Service session-specific messages [eFrameworkunifiedStatusErrNoEINTR]
///       - Incorrectly sized send buffers for sessions destined for NPP Service [eFrameworkunifiedStatusInvldBufSize]
///       - If there are any errors sending messages for sessions intended for NPP Service [eFrameworkunifiedStatusFail]
/// \~english @par Detail
///       Sends messages to the notification service (NS_NPPService) and registers Persistent file tags.\n
///       Tag name is 64 bytes or less. If the tag name exceeds 64 bytes, the tag name is truncated to 64 bytes or less. Therefore, specify a tag name of 64 bytes or less.
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Fire and Forget
/// \~english @see  FrameworkunifiedNPLoadPersistentFile, FrameworkunifiedNPReleasePersistentFile
///
/// API to send message to Notification Service to add a file path as to be persisted
///
/// \param [in] pTag
///         PCSTR - A unique identifier for the file path specified,
///                 this is used in retrieve/Load the file later
///
/// \param [in] bIsUserFile
///      BOOL - set TRUE if want to register a file for user.
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see FrameworkunifiedNPLoadPersistentFile, FrameworkunifiedNPReleasePersistentFile
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPRegisterPersistentFile(HANDLE hApp, PCSTR pTag, BOOL bIsUserFile = FALSE);

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPSetFilePersistentType
/// API to send message to Notification Service to set persistent file category.
///
/// \param [in] hApp
///         HANDLE - Handle to the Framework
/// \param [in] pTag
///         PCSTR - File tag
/// \param [in] ePersistCategory
///         EFrameworkunifiedPersistCategory - Persistent category
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPSetFilePersistentType(HANDLE hApp, PCSTR pTag, EFrameworkunifiedPersistCategory ePersistCategory);


////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  FrameworkunifiedNPLoadPersistentFile
/// \~english @par Brief
///        APIs that send messages to notification services and request Load of Persistent file tags
/// \~english @param [in] hApp
///        HANDLE   - Handle for the application
/// \~english @param [in] pDstFilePath
///        PCSTR    - Address of arbitrary data
/// \~english @param [in] pTag
///        PCSTR    - Tag name
/// \~english @param [in] hUser
///        BOOL     - Parameter error(Default:NULL)
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusNullPointer Specifying NULL pointers
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid handle type
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid file descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR Generating an interrupt by a system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer size
/// \~english @retval eFrameworkunifiedStatusFail Some error occurred
/// \~english @par Prerequisite
///       - The process of NS_NPPService is started.
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [eFrameworkunifiedStatusInvldParam]
///       - If the hApp specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusInvldParam]
///       - When the message queue information handle specified in the argument (hApp) is NULL, [eFrameworkunifiedStatusNullPointer]
///       - When the message queue handle in the handle (hApp) specified by the arguments is NULL, [eFrameworkunifiedStatusInvldParam]
///       - When the application-name storage area address in the handle (hApp) specified by the arguments is NULL, [eFrameworkunifiedStatusInvldParam]
///       - If the application name in the handle (hApp) specified by the arguments is empty, [eFrameworkunifiedStatusInvldParam]
///       - When the Load destination file path (pDstFilePath) specified in the parameter is NULL, [eFrameworkunifiedStatusInvldParam]
///       - When the Load destination filepath (pDstFilePath) specified in the parameter is empty, [eFrameworkunifiedStatusInvldParam]
///       - When the tag name (pTag) specified in the arguments is NULL, [eFrameworkunifiedStatusInvldParam]
///       - When the tag name (pTag) specified in the arguments is an empty character, [eFrameworkunifiedStatusInvldParam]
///       - When the message queue type of the handle (hApp) specified in the parameter is not the send type, [eFrameworkunifiedStatusInvldHndlType]
///       - If the message queue handle in the hApp specified by the arguments is inappropriate (not valid) [eFrameworkunifiedStatusInvldHandle]
///       - The message queue name of the handle (hApp) specified in the arguments is not appropriate (the name exceeds NULL, 20 bytes) [eFrameworkunifiedStatusInvldHandle]
///       - Full Message Queue for Sessions for NPP Service [eFrameworkunifiedStatusMsgQFull]
///       - Incorrect file descriptors for sending messages for NPP Service sessions [eFrameworkunifiedStatusErrNoEBADF]
///       - When an interrupt by a system call (signal) occurs while sending NPP Service session-specific messages [eFrameworkunifiedStatusErrNoEINTR]
///       - Incorrectly sized send buffers for sessions destined for NPP Service [eFrameworkunifiedStatusInvldBufSize]
///       - If there are any errors sending messages for sessions intended for NPP Service [eFrameworkunifiedStatusFail]
/// \~english @par Detail
///       Sends messages to the notification service (NS_NPPService) and requests the Load of Persistent file tags.\n
///       The copy destination file path is 255 bytes or less, and any file that exceeds 255 bytes is truncated to 255 bytes. Therefore, specify a file path of 255 bytes or less.\n
///       Tag name is 64 bytes or less. If the tag name exceeds 64 bytes, the tag name is truncated to 64 bytes or less. Therefore, specify a tag name of 64 bytes or less.\n
///
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Method(Async)
/// \~english @see  FrameworkunifiedNPRegisterPersistentFile, FrameworkunifiedNPReleasePersistentFile
///
/// API to send message to Notification Service to copy file from persistent memory to
/// specified path. The caller recieves an acknowledgement once NPS completes file copy
///
/// \param [in] pDstFilePath
///         PCSTR - Complete file path to which the file should be copied from persistent area
/// \param [in] pTag
///         PCSTR - Tag associated with the file that needs to be copied to pDstFilePath
/// \param [in] hUser
///         HANDLE - Handle for the user
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see FrameworkunifiedNPRegisterPersistentFile, FrameworkunifiedNPReleasePersistentFile
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPLoadPersistentFile(HANDLE hApp, PCSTR pDstFilePath, PCSTR pTag, HANDLE hUser = NULL);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedNPReleasePersistentFile
/// \~english @par Brief
///       API to send message to Notification Service to notify that the file can be persisted.
/// \~english @param [in] hApp
///       HANDLE - Handle to the Framework
/// \~english @param [in] bIsPersist
///       BOOL - If the specified file needs to be persisted or not on release
/// \~english @param [in] pTag
///       PCSTR - Tag associated with the file released
/// \~english @param [in] pFullFilePath
///       PCSTR - Full path name of the file to be persisted
/// \~english @param [in] hUser
///       HANDLE - Handle for the user
/// \~english @return status
///       EFrameworkunifiedStatus - success or error
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusNullPointer Specifying NULL pointers
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid handle type
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid file descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR Generating an interrupt by a system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer size
/// \~english @retval eFrameworkunifiedStatusFail Some error occurred
/// \~english @par Prerequisite
///       - The process of NS_NPPService is started.
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [eFrameworkunifiedStatusInvldParam]
///       - If the hApp specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusInvldParam]
///       - When the message queue information handle specified in the argument (hApp) is NULL, [eFrameworkunifiedStatusNullPointer]
///       - When the message queue handle in the handle (hApp) specified by the arguments is NULL, [eFrameworkunifiedStatusInvldParam]
///       - When the application-name storage area address in the handle (hApp) specified by the arguments is NULL, [eFrameworkunifiedStatusInvldParam]
///       - If the application name in the handle (hApp) specified by the arguments is empty, [eFrameworkunifiedStatusInvldParam]
///       - When the Release destination file path (pFullFilePath) specified in the parameter is NULL, [eFrameworkunifiedStatusInvldParam]
///       - When the Release destination filepath (pFullFilePath) specified in the parameter is empty, [eFrameworkunifiedStatusInvldParam]
///       - When the tag name (pTag) specified in the arguments is NULL, [eFrameworkunifiedStatusInvldParam]
///       - When the tag name (pTag) specified in the arguments is an empty character, [eFrameworkunifiedStatusInvldParam]
///       - When the message queue type of the handle (hApp) specified in the parameter is not the send type, [eFrameworkunifiedStatusInvldHndlType]
///       - If the message queue handle in the hApp specified by the arguments is inappropriate (not valid) [eFrameworkunifiedStatusInvldHandle]
///       - The message queue name of the handle (hApp) specified in the arguments is not appropriate (the name exceeds NULL, 20 bytes) [eFrameworkunifiedStatusInvldHandle]
///       - Full Message Queue for Sessions for NPP Service [eFrameworkunifiedStatusMsgQFull]
///       - Incorrect file descriptors for sending messages for NPP Service sessions [eFrameworkunifiedStatusErrNoEBADF]
///       - When an interrupt by a system call (signal) occurs while sending NPP Service session-specific messages [eFrameworkunifiedStatusErrNoEINTR]
///       - Incorrectly sized send buffers for sessions destined for NPP Service [eFrameworkunifiedStatusInvldBufSize]
///       - If there are any errors sending messages for sessions intended for NPP Service [eFrameworkunifiedStatusFail]
/// \~english @par Detail
///       Sends messages to the notification service (NS_NPPService) and requests the Load of Persistent file tags.\n
///       The copy destination file path is 255 bytes or less, and any file that exceeds 255 bytes is truncated to 255 bytes. Therefore, specify a file path of 255 bytes or less.\n
///       Tag name is 64 bytes or less. If the tag name exceeds 64 bytes, the tag name is truncated to 64 bytes or less. Therefore, specify a tag name of 64 bytes or less.\n
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Pub-Sub
/// \~english @see  FrameworkunifiedNPRegisterPersistentFile, FrameworkunifiedNPLoadPersistentFile
///
////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPReleasePersistentFile(HANDLE hApp, BOOL bIsPersist, PCSTR pTag, PCSTR pFullFilePath,
                                      HANDLE hUser = NULL);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedNPReleasePersistentFile
/// \~english @par Brief
///       API to send message to Notification Service to notify that the file can be persisted.
/// \~english @param [in] hApp
///       HANDLE - Handle to the Framework
/// \~english @param [in] eReleaseType
///         EFrameworkunifiedReleaseType - eFrameworkunifiedNotOnRelease     :not on release
///                           eFrameworkunifiedPersistOnShutdown:persist on shutdown
///                           eFrameworkunifiedPersistInstantly :persist instantly
/// \~english @param [in] pTag
///       PCSTR - Tag associated with the file released
/// \~english @param [in] pFullFilePath
///       PCSTR - Full path name of the file to be persisted
/// \~english @param [in] hUser
///       HANDLE - Handle for the user
/// \~english @return status
///       EFrameworkunifiedStatus - success or error
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusNullPointer Specifying NULL pointers
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid handle type
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid file descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR Generating an interrupt by a system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer size
/// \~english @retval eFrameworkunifiedStatusFail Some error occurred
/// \~english @par Prerequisite
///       - The process of NS_NPPService is started.
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [eFrameworkunifiedStatusInvldParam]
///       - If the hApp specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusInvldParam]
///       - When the message queue information handle specified in the argument (hApp) is NULL, [eFrameworkunifiedStatusNullPointer]
///       - When the message queue handle in the handle (hApp) specified by the arguments is NULL, [eFrameworkunifiedStatusInvldParam]
///       - When the application-name storage area address in the handle (hApp) specified by the arguments is NULL, [eFrameworkunifiedStatusInvldParam]
///       - If the application name in the handle (hApp) specified by the arguments is empty, [eFrameworkunifiedStatusInvldParam]
///       - When the Load destination file path (pDstFilePath) specified in the parameter is NULL, [eFrameworkunifiedStatusInvldParam]
///       - When the Load destination filepath (pDstFilePath) specified in the parameter is empty, [eFrameworkunifiedStatusInvldParam]
///       - When the tag name (pTag) specified in the arguments is NULL, [eFrameworkunifiedStatusInvldParam]
///       - When the tag name (pTag) specified in the arguments is an empty character, [eFrameworkunifiedStatusInvldParam]
///       - When the message queue type of the handle (hApp) specified in the parameter is not the send type, [eFrameworkunifiedStatusInvldHndlType]
///       - If the message queue handle in the hApp specified by the arguments is inappropriate (not valid) [eFrameworkunifiedStatusInvldHandle]
///       - The message queue name of the handle (hApp) specified in the arguments is not appropriate (the name exceeds NULL, 20 bytes) [eFrameworkunifiedStatusInvldHandle]
///       - Full Message Queue for Sessions for NPP Service [eFrameworkunifiedStatusMsgQFull]
///       - Incorrect file descriptors for sending messages for NPP Service sessions [eFrameworkunifiedStatusErrNoEBADF]
///       - When an interrupt by a system call (signal) occurs while sending NPP Service session-specific messages [eFrameworkunifiedStatusErrNoEINTR]
///       - Incorrectly sized send buffers for sessions destined for NPP Service [eFrameworkunifiedStatusInvldBufSize]
///       - If there are any errors sending messages for sessions intended for NPP Service [eFrameworkunifiedStatusFail]
/// \~english @par Detail
///       Sends messages to the notification service (NS_NPPService) and requests the Load of Persistent file tags.\n
///       The copy destination file path is 255 bytes or less, and any file that exceeds 255 bytes is truncated to 255 bytes. Therefore, specify a file path of 255 bytes or less.\n
///       Tag name is 64 bytes or less. If the tag name exceeds 64 bytes, the tag name is truncated to 64 bytes or less. Therefore, specify a tag name of 64 bytes or less.\n
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Pub-Sub
/// \~english @see  FrameworkunifiedNPRegisterPersistentFile, FrameworkunifiedNPLoadPersistentFile
///
////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPReleasePersistentFile(HANDLE hApp, EFrameworkunifiedReleaseType eReleaseType, PCSTR pTag, PCSTR pFullFilePath,
                                      HANDLE hUser = NULL);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  FrameworkunifiedNPRegisterPersistentFolder
/// API to send message to Notification Service to add a folder path to be persisted
///
/// \param [in] hApp
///         HANDLE - Handle to the Framework
/// \param [in] pTag
///         PCSTR - A unique identifier for the folder path specified,
///                 this is used in retrieve/Load the folder later
///
/// \param [in] bIsUserFolder
///      BOOL - set TRUE if want to register a folder for user.
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see FrameworkunifiedNPLoadPersistentFolder, FrameworkunifiedNPReleasePersistentFolder
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPRegisterPersistentFolder(HANDLE hApp, PCSTR pTag, BOOL bIsUserFolder = FALSE);

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPSetFolderPersistentType
/// API to send message to Notification Service to set persistent folder category.
///
/// \param [in] hApp
///         HANDLE - Handle to the Framework
/// \param [in] pTag
///         PCSTR - A unique identifier for the folder path specified,
///                 this is used in retrieve/Load the folder later
/// \param [in] ePersistCategory
///         EFrameworkunifiedPersistCategory - Persistent category
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPSetFolderPersistentType(HANDLE hApp, PCSTR pTag, EFrameworkunifiedPersistCategory ePersistCategory);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  FrameworkunifiedNPLoadPersistentFolder
/// API to send message to Notification Service to copy folder from persistent memory to
/// specified path. The caller recieves an acknowledgement once NPS completes folder copy
///
/// \param [in] hApp
///         HANDLE - Handle to the Framework
/// \param [in] pDstFolderPath
///         PCSTR - Complete folder path to which the folder should be copied from persistent area
/// \param [in] pTag
///         PCSTR - Tag associated with the folder that needs to be copied to pDstFolderPath
/// \param [in] hUser
///         HANDLE - Handle for the user
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see FrameworkunifiedNPRegisterPersistentFolder, FrameworkunifiedNPReleasePersistentFolder
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPLoadPersistentFolder(HANDLE hApp, PCSTR pDstFolderPath, PCSTR pTag, HANDLE hUser = NULL);

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPReleasePersistentFolder
/// API to send message to Notification Service to notify that the folder can be persisted.
///
/// \param [in] hApp
///         HANDLE - Handle to the Framework
/// \param [in] bIsPersist
///         BOOL - If the specified folder needs to be persisted or not on release
/// \param [in] pTag
///         PCSTR - Tag associated with the folder released
/// \param [in] pFullFolderPath
///         PCSTR - Full path name of the folder to be persisted
/// \param [in] hUser
///         HANDLE - Handle for the user
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see FrameworkunifiedNPRegisterPersistentFolder, FrameworkunifiedNPLoadPersistentFolder
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPReleasePersistentFolder(HANDLE hApp, BOOL bIsPersist, PCSTR pTag, PCSTR pFullFolderPath,
                                        HANDLE hUser = NULL);

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPReleasePersistentFolder
/// API to send message to Notification Service to notify that the folder can be persisted.
///
/// \param [in] hApp
///         HANDLE - Handle to the Framework
/// \param [in] eFrameworkunifiedReleaseType
///         EFrameworkunifiedReleaseType - eFrameworkunifiedNotOnRelease     :not on release
///                           eFrameworkunifiedPersistOnShutdown:persist on shutdown
///                           eFrameworkunifiedPersistInstantly :persist instantly
/// \param [in] pTag
///         PCSTR - Tag associated with the folder released
/// \param [in] pFullFolderPath
///         PCSTR - Full path name of the folder to be persisted
/// \param [in] hUser
///         HANDLE - Handle for the user
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see FrameworkunifiedNPRegisterPersistentFolder, FrameworkunifiedNPLoadPersistentFolder
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPReleasePersistentFolder(HANDLE hApp, EFrameworkunifiedReleaseType eFrameworkunifiedReleaseType, PCSTR pTag,
                                        PCSTR pFullFolderPath, HANDLE hUser = NULL);


////////////////////////////////////////////////////////////////////////////////////////////
//        Session APIs
////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedOpenSession
/// \~english @par Brief
///       API sends a message to the service requesting a session.
/// \~english @param [in] hService
///        HANDLE   - Handle to the service(FrameworkunifiedOpenService HANDLE acquired in)
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusNullPointer NULL pointer specified
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid type of handle
/// \~english @retval eFrameworkunifiedStatusInvldQName Illegal Message Queue name
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
/// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
/// \~english @retval eFrameworkunifiedStatusErrOther Other error has occurred(Cannot access shared memory, etc.)
/// \~english @par Prerequisite
///       - Generation/Initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been
///       done.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - HANDLE specified in the argument (Service) is NULL. [eFrameworkunifiedStatusNullPointer]
///       - HANDLE specified in the argument (hService) is not appropriate (which is invalid). [eFrameworkunifiedStatusNullPointer]
///       - Message queue name of HANDLE specified in the argument (hService) is NULL. [eFrameworkunifiedStatusInvldHandle]
///       - Message queue name of HANDLE specified in the argument (hService) exceeds 20byte. [eFrameworkunifiedStatusInvldHandle]
///       - Message queue HANDLE in HANDLE specified in the argument (hService) is NULL. [eFrameworkunifiedStatusInvldHandle]
///       - HANDLE specified in the argument message queue HANDLE in (hService) is not appropriate (which is invalid).
///       [eFrameworkunifiedStatusInvldHandle]
///       - Message queue HANDLE name in the HANDLE specified in the argument (hService) is NULL. [eFrameworkunifiedStatusInvldQName]
///       - Message queue HANDLE name in the HANDLE specified in the argument (hService) exceeds 20byte.
///       [eFrameworkunifiedStatusInvldQName]
///       - Message queue type of HANDLE specified in the argument (hService) is not a transmission type.
///       [eFrameworkunifiedStatusInvldHndlType]
///       - To the service session message queue is full. [eFrameworkunifiedStatusMsgQFull]
///       - It is invalid transmission file descriptor of the session message to the service. [eFrameworkunifiedStatusErrNoEBADF]
///       - Interruption by the system call (signal) during the transmission of the session message to the service
///       occurs. [eFrameworkunifiedStatusErrNoEINTR]
///       - Size of the transmit buffer of the message for the session to the service is illegal.
///       [eFrameworkunifiedStatusInvldBufSize]
///       - Any errors occur during the transmission of a session message to the service. [eFrameworkunifiedStatusFail]
///       - In the shared memory access for the session message to be sent to service failed. [eFrameworkunifiedStatusErrOther]
/// \~english @par Detail
///       To send a message of the session request to the service.
/// \~english @par Classification
///          Public
/// \~english @see FrameworkunifiedGetOpenSessionHandle, FrameworkunifiedOpenSessionWithData, FrameworkunifiedCloseSession, FrameworkunifiedGenerateNewSessionId,
///                FrameworkunifiedGetSessionId
////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOpenSession(HANDLE hService);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedOpenSessionWithData
/// \~english @par Brief
///       API to send to service the message of the session request together with the data.
/// \~english @param [in] hService
///        HANDLE   - Handle to the service(FrameworkunifiedOpenService HANDLE acquired in)
/// \~english @param [in] pData
///        PVOID    - Pointer to the session data for transmission
/// \~english @param [in] length
///        UI_32    - Size of the session data for transmission
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusInvldBuf Invalid buffer
/// \~english @retval eFrameworkunifiedStatusNullPointer NULL pointer specified
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid type of handle
/// \~english @retval eFrameworkunifiedStatusInvldQName Illegal Message Queue name
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
/// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
/// \~english @retval eFrameworkunifiedStatusErrOther Other error has occurred(Cannot access shared memory, etc.)
/// \~english @par Prerequisite
///       - Generation/Initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been
///       done.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - A pointer to the session data for transmission to the specified non-zero size at NULL in argument.
///       [eFrameworkunifiedStatusInvldBuf]
///       - The pointer is NULL to the session data for transmission specified in the argument. [eFrameworkunifiedStatusErrOther]
///       - Size of the session data for transmission specified in the argument is 0. [eFrameworkunifiedStatusErrOther]
///       - HANDLE specified in the argument (Service) is NULL. [eFrameworkunifiedStatusNullPointer]
///       - HANDLE specified in the argument (hService) is not appropriate (which is invalid). [eFrameworkunifiedStatusNullPointer]
///       - Message queue name of HANDLE specified in the argument (hService) is NULL. [eFrameworkunifiedStatusInvldHandle]
///       - Message queue name of HANDLE specified in the argument (hService) exceeds 20byte. [eFrameworkunifiedStatusInvldHandle]
///       - Message queue HANDLE in HANDLE specified in the argument (hService) is NULL. [eFrameworkunifiedStatusInvldHandle]
///       - HANDLE specified in the argument message queue HANDLE in (hService) is not appropriate (which is invalid).
///       [eFrameworkunifiedStatusInvldHandle]
///       - Message queue HANDLE name in the HANDLE specified in the argument (hService) is NULL. [eFrameworkunifiedStatusInvldQName]
///       - Message queue HANDLE name in the HANDLE specified in the argument (hService) exceeds 20byte.
///       [eFrameworkunifiedStatusInvldQName]
///       - Message queue type of HANDLE specified in the argument (hService) is not a transmission type.
///       [eFrameworkunifiedStatusInvldHndlType]
///       - To the service session message queue is full. [eFrameworkunifiedStatusMsgQFull]
///       - It is invalid transmission file descriptor of the session message to the service. [eFrameworkunifiedStatusErrNoEBADF]
///       - Interruption by the system call (signal) during the transmission of the session message to the service
///       occurs. [eFrameworkunifiedStatusErrNoEINTR]
///       - Size of the transmit buffer of the message for the session to the service is illegal.
///       [eFrameworkunifiedStatusInvldBufSize]
///       - Any errors occur during the transmission of a session message to the service. [eFrameworkunifiedStatusFail]
///       - In the shared memory access for the session message to be sent to service failed. [eFrameworkunifiedStatusErrOther]
/// \~english @par Detail
///       To send to the service a message of session request together with the data.
/// \~english @par Classification
///          Public
/// \~english @see FrameworkunifiedOpenSession, FrameworkunifiedGetOpenSessionHandle, FrameworkunifiedCloseSession, FrameworkunifiedGenerateNewSessionId,
///                FrameworkunifiedGetSessionId
////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOpenSessionWithData(HANDLE hService, PVOID pData, UI_32 length);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedOpenSessionSync
/// \~english @par Brief
///       Synchronous API of sending a message of the session request to the service.
/// \~english @param [in] hService
///        HANDLE   - Handle to the service(FrameworkunifiedOpenService/FrameworkunifiedGetOpenSessionHandle/FrameworkunifiedGetOpenSessionSyncHandle HANDLE
///        acquired in)
/// \~english @param [out] ack
///        OpenSessionAck   - Pointer to the response data of the session initiation
/// \~english @par
///        OpenSessionAck Structure
/// \~english @code
///                 #define MAX_QUEUE_NAME_SIZE 20
///                 typedef struct _OpenSessionAck
///                 {
///                       EFrameworkunifiedStatus eStatus;                      /* Status                */
///                       UI_32 sessionId;                         /* Session ID            */
///                       CHAR  cSessionName[MAX_QUEUE_NAME_SIZE]; /* Session Name(20byte)  */
///                       UI_32 sessionType;                       /* Session Type          */
///                 }OpenSessionAck;
///            @endcode
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusNullPointer NULL pointer specified
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid type of handle
/// \~english @retval eFrameworkunifiedStatusInvldQName Illegal Message Queue name
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
/// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
/// \~english @retval eFrameworkunifiedStatusBadConnection It can not be a socket connection
/// \~english @retval eFrameworkunifiedStatusErrOther Other error has occurred(Cannot access shared memory, etc.)
/// \~english @par Prerequisite
///       - Generation/Initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been
///       done.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - HANDLE specified in the argument (Service) is NULL. [eFrameworkunifiedStatusNullPointer]
///       - HANDLE specified in the argument (hService) is not appropriate (which is invalid). [eFrameworkunifiedStatusNullPointer]
///       - It failed to open the message queue for the response received [eFrameworkunifiedStatusFail]
///       - Message queue HANDLE in HANDLE specified in the argument (hService) is NULL. [eFrameworkunifiedStatusInvldHandle]
///       - HANDLE specified in the argument message queue HANDLE in (hService) is not appropriate (which is invalid).
///       [eFrameworkunifiedStatusInvldHandle]
///       - It fails to get the message storage buffer for the response received. [eFrameworkunifiedStatusFail]
///       - Random number generation to fail for the sequence ID of the message for transmission. [eFrameworkunifiedStatusFail]
///       - Message queue name of HANDLE specified in the argument (hService) is NULL. [eFrameworkunifiedStatusInvldHandle]
///       - Message queue name of HANDLE specified in the argument (hService) exceeds 20byte. [eFrameworkunifiedStatusInvldHandle]
///       - Message queue HANDLE name in the HANDLE specified in the argument (hService) is NULL. [eFrameworkunifiedStatusInvldQName]
///       - Message queue HANDLE name in the HANDLE specified in the argument (hService) exceeds 20byte.
///       [eFrameworkunifiedStatusInvldQName]
///       - Message queue type of HANDLE specified in the argument (hService) is not a transmission type.
///       [eFrameworkunifiedStatusInvldHndlType]
///       - To the service session message queue is full. [eFrameworkunifiedStatusMsgQFull]
///       - It is invalid transmit and receive file descriptor of the session message to the service.
///       [eFrameworkunifiedStatusErrNoEBADF]
///       - Interruption by the system call (signal) occurs during the transmission and reception of the session message
///       to the service. [eFrameworkunifiedStatusErrNoEINTR]
///       - Size of the transmit buffer of the message for the session to the service is illegal.
///       [eFrameworkunifiedStatusInvldBufSize]
///       - Any errors occur during the sending and receiving of a session message to the service. [eFrameworkunifiedStatusFail]
///       - In the shared memory access for the session message to be sent to service failed. [eFrameworkunifiedStatusErrOther]
///       - It failed to Socket connection of the response message for the reception from the service. [eFrameworkunifiedStatusFail]
///       - The wait control of the response message received from the service failed. [eFrameworkunifiedStatusFail]
///       - Interruption by the system call (signal) to wait in the response message received from the service has
///       occurred. [eFrameworkunifiedStatusFail]
///       - In response message at the time of failure to receive from the service, it failed to re-connection of the
///       Socket. [eFrameworkunifiedStatusBadConnection]
/// \~english @par Detail
///       Send in the synchronous type the message of the session request to the service.
/// \~english @par Classification
///          Public
/// \~english @see FrameworkunifiedOpenSessionWithDataSync, FrameworkunifiedGetOpenSessionSyncHandle, FrameworkunifiedCloseSessionSync,
/// FrameworkunifiedGenerateNewSessionId,
///                 FrameworkunifiedGetSessionId
////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOpenSessionSync(HANDLE hService, OpenSessionAck *ack);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedOpenSessionWithDataSync
/// \~english @par Brief
///       Synchronous API that transmits to the service a message session request with the data.
/// \~english @param [in] hService
///        HANDLE   - Handle to the service(FrameworkunifiedOpenService/FrameworkunifiedGetOpenSessionHandle/FrameworkunifiedGetOpenSessionSyncHandle HANDLE
///        acquired in)
/// \~english @param [in] pData
///        PVOID    - Pointer to session data for transmission
/// \~english @param [in] length
///        UI_32    - Size of the session data for transmission
/// \~english @param [out] ack
///        OpenSessionAck   - Pointer to the response data of the session initiation
/// \~english @par
///        OpenSessionAck Structure
/// \~english @code
///                 #define MAX_QUEUE_NAME_SIZE 20
///                 typedef struct _OpenSessionAck
///                 {
///                       EFrameworkunifiedStatus eStatus;                      /* Status                */
///                       UI_32 sessionId;                         /* Session ID            */
///                       CHAR  cSessionName[MAX_QUEUE_NAME_SIZE]; /* Session Name(20byte)  */
///                       UI_32 sessionType;                       /* Session Type          */
///                 }OpenSessionAck;
///            @endcode
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusInvldBuf Invalid Buffer
/// \~english @retval eFrameworkunifiedStatusNullPointer NULL pointer specified
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid type of handle
/// \~english @retval eFrameworkunifiedStatusInvldQName Illegal Message Queue name
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
/// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
/// \~english @retval eFrameworkunifiedStatusBadConnection It can not be a socket connection
/// \~english @retval eFrameworkunifiedStatusErrOther Other error has occurred(Cannot access shared memory, etc.)
/// \~english @par Prerequisite
///       - Generation/Initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been
///       done.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - A pointer to the session data for transmission to the specified non-zero size at NULL in argument.
///       [eFrameworkunifiedStatusInvldBuf]
///       - The pointer is NULL to the session data for transmission specified in the argument. [eFrameworkunifiedStatusErrOther]
///       - Size of the session data for transmission specified in the argument is 0. [eFrameworkunifiedStatusErrOther]
///       - HANDLE specified in the argument (Service) is NULL. [eFrameworkunifiedStatusNullPointer]
///       - HANDLE specified in the argument (hApp) is not appropriate (which is invalid). [eFrameworkunifiedStatusNullPointer]
///       - It failed to open the message queue for the response received [eFrameworkunifiedStatusFail]
///       - Message queue HANDLE in HANDLE specified in the argument (hService) is NULL. [eFrameworkunifiedStatusInvldHandle]
///       - HANDLE specified in the argument message queue HANDLE in (hApp) is not appropriate (which is invalid).
///       [eFrameworkunifiedStatusInvldHandle]
///       - It fails to get the message storage buffer for the response received. [eFrameworkunifiedStatusFail]
///       - Random number generation to fail for the sequence ID of the message for transmission. [eFrameworkunifiedStatusFail]
///       - Message queue name of HANDLE specified in the argument (hService) is NULL. [eFrameworkunifiedStatusInvldHandle]
///       - Message queue name of HANDLE specified in the argument (hService) exceeds 20byte. [eFrameworkunifiedStatusInvldHandle]
///       - Message queue HANDLE name in the HANDLE specified in the argument (hService) is NULL. [eFrameworkunifiedStatusInvldQName]
///       - Message queue HANDLE name in the HANDLE specified in the argument (hService) exceeds 20byte.
///       [eFrameworkunifiedStatusInvldQName]
///       - Message queue type of HANDLE specified in the argument (hService) is not a transmission type.
///       [eFrameworkunifiedStatusInvldHndlType]
///       - To the service session message queue is full. [eFrameworkunifiedStatusMsgQFull]
///       - It is invalid transmit and receive file descriptor of the session message to the service.
///       [eFrameworkunifiedStatusErrNoEBADF]
///       - Interruption by the system call (signal) occurs during the transmission and reception of the session message
///       to the service. [eFrameworkunifiedStatusErrNoEINTR]
///       - Size of the transmit buffer of the message for the session to the service is illegal.
///       [eFrameworkunifiedStatusInvldBufSize]
///       - Any errors occur during the sending and receiving of a session message to the service. [eFrameworkunifiedStatusFail]
///       - In the shared memory access for transmission and reception of the service for the message for the session
///       failed. [eFrameworkunifiedStatusErrOther]
///       - It failed to Socket connection of the response message for the reception from the service. [eFrameworkunifiedStatusFail]
///       - The wait control of the response message received from the service failed. [eFrameworkunifiedStatusFail]
///       - Interruption by the system call (signal) to wait in the response message received from the service has
///       occurred. [eFrameworkunifiedStatusFail]
///       - In response message at the time of failure to receive from the service, it failed to re-connection of the
///       Socket. [eFrameworkunifiedStatusBadConnection]
/// \~english @par Detail
///       Send in the synchronous type to service the message of the session request along with the data.
/// \~english @par Classification
///          Public
/// \~english @see FrameworkunifiedOpenSessionSync, FrameworkunifiedGetOpenSessionSyncHandle, FrameworkunifiedCloseSessionSync, FrameworkunifiedGenerateNewSessionId,
///                 FrameworkunifiedGetSessionId
////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOpenSessionWithDataSync(HANDLE hService, PVOID pData, UI_32 length, OpenSessionAck *ack);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedGetOpenSessionHandle
/// \~english @par Brief
///       Asynchronous API to get the session handler.
/// \~english @param [in] hApp
///        HANDLE   - HANDLE for Application (FrameworkunifiedCreateDispatcherWithoutLoop() HANDLE acquired in)
/// \~english @retval HANDLE HANDLE for the session
/// \~english @retval NULL   Failure to HANDLE acquisition for the session
/// \~english @par Prerequisite
///       - Generation/Initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been
///       done.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       Under the following conditions, HANDLE is returned as NULL.
///       - HANDLE specified in the argument (hApp) is NULL.
///       - HANDLE specified in the argument (hApp) is not appropriate (which is invalid).
///       - Data size received by the HANDLE specified in the argument (hApp) is not the data size of
///       ProtocolOpenSessionAck message.
///       - Can not get the data and the data length received by the HANDLE specified in the argument (hApp).
///       - Size of the session name of the data received by the specified HANDLE (hApp) in the argument is more than
///       20byte.
///       - Not be able to acquire the memory of the buffer for the internal processing.
///       - Session name is NULL of data received by the HANDLE specified in the argument (hApp).
///       - It failed to generate the message queue name.
///       - It failed to open the message queue.
/// \~english @par Detail
///       This API will be used in an asynchronous type specification. \n
///       Acquires received data ProtocolOpenSessionAck from HANDLE specified in the argument (hApp), \n
///       obtains the session HANDLE from the session name.
/// \~english @par Classification
///          Public
/// \~english @see FrameworkunifiedOpenSession, FrameworkunifiedOpenSessionWithData, FrameworkunifiedCloseSession, FrameworkunifiedGenerateNewSessionId,
///                FrameworkunifiedGetSessionId
////////////////////////////////////////////////////////////////////////////////////
HANDLE FrameworkunifiedGetOpenSessionHandle(HANDLE hApp);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedGetOpenSessionSyncHandle
/// \~english @par Brief
///       API to get the session ID without using the received data of ProtocolOpenSessionAck.
/// \~english @param [in] hApp
///        HANDLE   - HANDLE for Application (FrameworkunifiedCreateDispatcherWithoutLoop() HANDLE acquired in)
/// \~english @param [in] tAck
///        OpenSessionAck   - Pointer to the OpenSessionAck structure
/// \~english @par
///        OpenSessionAck Structure
/// \~english @code
///                 #define MAX_QUEUE_NAME_SIZE 20
///                 typedef struct _OpenSessionAck
///                 {
///                       EFrameworkunifiedStatus eStatus;                      /* Status               */
///                       UI_32 sessionId;                         /* Session ID           */
///                       CHAR  cSessionName[MAX_QUEUE_NAME_SIZE]; /* Session Name(20byte) */
///                       UI_32 sessionType;                       /* Session Type         */
///                 }OpenSessionAck;
///            @endcode
/// \~english @retval HANDLE HANDLE for the session
/// \~english @retval NULL   Failure to HANDLE acquisition for the session
/// \~english @par Prerequisite
///       - Generation/Initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been
///       done.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       Under the following conditions, HANDLE is returned as NULL.
///       - HANDLE specified in the argument (hApp) is NULL.
///       - HANDLE specified in the argument (hApp) is not appropriate (which is invalid).
///       - Data size received by the HANDLE specified in the argument (hApp) is not the data size of
///       ProtocolOpenSessionAck message.
///       - Size of the session name of the data received by the specified HANDLE (hApp) in the argument is more than
///       20byte.
///       - Not be able to acquire the memory of the buffer for the internal processing.
///       - Session name is NULL of data received by the HANDLE specified in the argument (hApp).
///       - It failed to generate the message queue name.
///       - It failed to open the message queue.
/// \~english @par Detail
///       This API is to be used at the time of the synchronous type specification. \n
///       Use the session name of OpenSessionAck structure specified by the argument, to get the session HANDLE.
/// \~english @par Classification
///          Public
/// \~english @see FrameworkunifiedOpenSessionSync, FrameworkunifiedOpenSessionWithDataSync, FrameworkunifiedCloseSessionSync, FrameworkunifiedGenerateNewSessionId,
///                FrameworkunifiedGetSessionId
////////////////////////////////////////////////////////////////////////////////////
HANDLE FrameworkunifiedGetOpenSessionSyncHandle(HANDLE hApp, OpenSessionAck *tAck);

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedGenerateSessionHandle
/// API will be called by server on receiving the Open session request to Create session
///
///
/// \param [in] hApp
///         HANDLE - Handle to the framework application
/// \param [in] pServiceName
///         PCSTR - Service Name
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
///
/// \see FrameworkunifiedOpenSession, FrameworkunifiedCloseSession, FrameworkunifiedGenerateNewSessionId
///    FrameworkunifiedGetSessionId
///
///
////////////////////////////////////////////////////////////////////////////////////////////
HANDLE FrameworkunifiedGenerateSessionHandle(HANDLE hApp, PCSTR pServiceName);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedCloseSession
/// \~english @par Brief
///       API to close the session.
/// \~english @param [in] hService
///        HANDLE   - Handle to the service(FrameworkunifiedOpenService HANDLE acquired in)
/// \~english @param [in] hSession
///        HANDLE   - Handle to the session(FrameworkunifiedOpenSession HANDLE acquired in)
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusNullPointer NULL pointer specified
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid type of handle
/// \~english @retval eFrameworkunifiedStatusInvldQName Illegal Message Queue name
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
/// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
/// \~english @retval eFrameworkunifiedStatusErrOther Other error has occurred(Cannot access shared memory, etc.)
/// \~english @par Prerequisite
///       - Generation/Initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been
///       done.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - HANDLE specified in the argument (hService) is NULL. [eFrameworkunifiedStatusNullPointer]
///       - HANDLE specified in the argument (hService) is not appropriate (which is invalid). [eFrameworkunifiedStatusNullPointer]
///       - HANDLE specified in the argument (hSession) is NULL. [eFrameworkunifiedStatusNullPointer]
///       - HANDLE specified in the argument (hSession) is not appropriate (which is invalid). [eFrameworkunifiedStatusNullPointer]
///       - Message queue name of HANDLE specified in the argument (hService) is NULL. [eFrameworkunifiedStatusInvldHandle]
///       - Message queue name of HANDLE specified in the argument (hService) exceeds 20byte. [eFrameworkunifiedStatusInvldHandle]
///       - Message queue HANDLE in HANDLE specified in the argument (hService) is NULL. [eFrameworkunifiedStatusInvldHandle]
///       - HANDLE specified in the argument message queue HANDLE in (hService) is not appropriate (which is invalid).
///       [eFrameworkunifiedStatusInvldHandle]
///       - Message queue HANDLE name in the HANDLE specified in the argument (hService) is NULL. [eFrameworkunifiedStatusInvldQName]
///       - Message queue HANDLE name in the HANDLE specified in the argument (hService) exceeds 20byte.
///       [eFrameworkunifiedStatusInvldQName]
///       - Message queue type of HANDLE specified in the argument (hService) is not a transmission type.
///       [eFrameworkunifiedStatusInvldHndlType]
///       - To the service session message queue is full. [eFrameworkunifiedStatusMsgQFull]
///       - It is invalid transmission file descriptor of the session message to the service. [eFrameworkunifiedStatusErrNoEBADF]
///       - Interruption by the system call (signal) during the transmission of the session message to the service
///       occurs. [eFrameworkunifiedStatusErrNoEINTR]
///       - Size of the transmit buffer of the message for the session to the service is illegal.
///       [eFrameworkunifiedStatusInvldBufSize]
///       - Any errors occur during the transmission of a session message to the service. [eFrameworkunifiedStatusFail]
///       - In the shared memory access for the session message to be sent to service failed. [eFrameworkunifiedStatusErrOther]
///       - Message queue type of HANDLE specified in the argument (hSession) is mismatch.(eQTypeSender or
///       eQTypeReveiver) [eFrameworkunifiedStatusInvldHndlType]
///       - Failed to close message queue HANDLE specified in the argument (hSession). [eFrameworkunifiedStatusInvldHandle]
/// \~english @par Detail
///       Send a message of the session close request to the service.\n
///       And close message queue of the session.
///       This is asynchronous API.
/// \~english @par Classification
///          Public
/// \~english @see FrameworkunifiedOpenSession, FrameworkunifiedOpenSessionWithData, FrameworkunifiedGetOpenSessionHandle
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedCloseSession(HANDLE hService, HANDLE hSession);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedCloseSessionSync
/// \~english @par Brief
///       API to close the session (synchronous).
/// \~english @param [in] hService
///        HANDLE   - Handle to the service(FrameworkunifiedOpenService HANDLE acquired in)
/// \~english @param [in] hSession
///        HANDLE   - Handle to the session(FrameworkunifiedOpenSessionSync HANDLE acquired in)
/// \~english @param [out] ack
///        CloseSessionAck   - Pointer to the response data of the session close
/// \~english @par
///        CloseSessionAck Structure
/// \~english @code
///                 typedef struct _CloseSessionAck
///                 {
///                       UI_32 sessionId;                         /* Session ID            */
///                       EFrameworkunifiedStatus eStatus;                      /* Status                */
///                 }CloseSessionAck;
///            @endcode
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusNullPointer NULL pointer specified
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid type of handle
/// \~english @retval eFrameworkunifiedStatusInvldQName Illegal Message Queue name
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
/// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
/// \~english @retval eFrameworkunifiedStatusBadConnection It can not be a socket connection
/// \~english @retval eFrameworkunifiedStatusErrOther Other error has occurred(Cannot access shared memory, etc.)
/// \~english @retval eFrameworkunifiedStatusOK [eStatus in CloseSessionAck]Success in serivice side.
/// \~english @retval eFrameworkunifiedStatusFail [eStatus in CloseSessionAck]Some sort of error occurred in service side.
/// \~english @par Prerequisite
///       - Generation/Initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been
///       done.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure (API)
///       - CloseSessionAck specified in the argument (ack) is NULL. [eFrameworkunifiedStatusInvldParam]
///       - HANDLE specified in the argument (Service) is NULL. [eFrameworkunifiedStatusNullPointer]
///       - HANDLE specified in the argument (hService) is not appropriate (which is invalid). [eFrameworkunifiedStatusNullPointer]
///       - HANDLE specified in the argument (hSession) is NULL. [eFrameworkunifiedStatusNullPointer]
///       - HANDLE specified in the argument (hSession) is not appropriate (which is invalid). [eFrameworkunifiedStatusNullPointer]
///       - It failed to open the message queue for the response received [eFrameworkunifiedStatusFail]
///       - Message queue HANDLE in HANDLE specified in the argument (hService) is NULL. [eFrameworkunifiedStatusInvldHandle]
///       - HANDLE specified in the argument message queue HANDLE in (hService) is not appropriate (which is invalid).
///       [eFrameworkunifiedStatusInvldHandle]
///       - It fails to get the message storage buffer for the response received. [eFrameworkunifiedStatusFail]
///       - Random number generation to fail for the sequence ID of the message for transmission. [eFrameworkunifiedStatusFail]
///       - Message queue name of HANDLE specified in the argument (hService) is NULL. [eFrameworkunifiedStatusInvldHandle]
///       - Message queue name of HANDLE specified in the argument (hService) exceeds 20byte. [eFrameworkunifiedStatusInvldHandle]
///       - Message queue HANDLE name in the HANDLE specified in the argument (hService) is NULL. [eFrameworkunifiedStatusInvldQName]
///       - Message queue HANDLE name in the HANDLE specified in the argument (hService) exceeds 20byte.
///       [eFrameworkunifiedStatusInvldQName]
///       - Message queue type of HANDLE specified in the argument (hService) is not a transmission type.
///       [eFrameworkunifiedStatusInvldHndlType]
///       - To the service session message queue is full. [eFrameworkunifiedStatusMsgQFull]
///       - It is invalid transmit and receive file descriptor of the session message to the service.
///       [eFrameworkunifiedStatusErrNoEBADF]
///       - Interruption by the system call (signal) occurs during the transmission and reception of the session message
///       to the service. [eFrameworkunifiedStatusErrNoEINTR]
///       - Size of the transmit buffer of the message for the session to the service is illegal.
///       [eFrameworkunifiedStatusInvldBufSize]
///       - Any errors occur during the sending and receiving of a session message to the service. [eFrameworkunifiedStatusFail]
///       - In the shared memory access for the session message to be sent to service failed. [eFrameworkunifiedStatusErrOther]
///       - It failed to Socket connection of the response message for the reception from the service. [eFrameworkunifiedStatusFail]
///       - The wait control of the response message received from the service failed. [eFrameworkunifiedStatusFail]
///       - Interruption by the system call (signal) to wait in the response message received from the service has
///       occurred. [eFrameworkunifiedStatusFail]
///       - In response message at the time of failure to receive from the service, it failed to re-connection of the
///       Socket. [eFrameworkunifiedStatusBadConnection]
///       - Failed to close message queue HANDLE specified in the argument (hSession). [eFrameworkunifiedStatusInvldHandle]
///       - Message queue type of HANDLE specified in the argument (hSession) is mismatch.(eQTypeSender or
///       eQTypeReveiver) [eFrameworkunifiedStatusInvldHndlType]
/// \~english @par Conditions of processing failure (eStatus in CloseSessionAck)
///       - Any errors occur during in service side. [eFrameworkunifiedStatusFail]
/// \~english @par Detail
///       Send a message synchronously of the session close request to the service.\n
///       And close message queue of the session.\n
///       The eStatus value in CloseSessionAck is dependent on service. So we indecates major return value.
/// \~english @par Classification
///          Public
/// \~english @see FrameworkunifiedOpenSessionSync, FrameworkunifiedOpenSessionWithDataSync
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedCloseSessionSync(HANDLE hService, HANDLE hSession, CloseSessionAck *ack);

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedGenerateNewSessionId
/// Used on the Server side to get the next session id
///
///
/// \return sessionID
///         UI_32 - session ID value
///
///
/// \see FrameworkunifiedOpenSession, FrameworkunifiedGetOpenSessionHandle, FrameworkunifiedCloseSession,
///    FrameworkunifiedGetSessionId
///
///
////////////////////////////////////////////////////////////////////////////////////////////
UI_32 FrameworkunifiedGenerateNewSessionId();

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedGetSessionId
/// \~english @par Brief
///        Returns the session id corresponding to the given session handle.
/// \~english @param [in] hSession
///        HANDLE   - Handle to the session(FrameworkunifiedMcOpenSender HANDLE acquired in)
/// \~english @retval UI_32 session ID value
/// \~english @retval MAX_SESSION_ID_VAL Failed to get session ID value
/// \~english @par Prerequisite
///       - Generation/Initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been
///       done.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - HANDLE specified in the argument (hSession) is NULL. [MAX_SESSION_ID_VAL]
///       - HANDLE specified in the argument (hSession) is not appropriate (which is invalid). [MAX_SESSION_ID_VAL]
/// \~english @par Detail
///        Returns the session id corresponding to the given session handle.
/// \~english @par Classification
///          Public
/// \~english @see FrameworkunifiedOpenSession, FrameworkunifiedOpenSessionSync, FrameworkunifiedOpenSessionWithData, FrameworkunifiedOpenSessionWithDataSync, \n
///                 FrameworkunifiedGetOpenSessionHandle, FrameworkunifiedGetOpenSessionSyncHandle, FrameworkunifiedGetCurrentSessionHandle, \n
///                 FrameworkunifiedMcOpenSender, FrameworkunifiedCreateSession
////////////////////////////////////////////////////////////////////////////////////////////
UI_32 FrameworkunifiedGetSessionId(HANDLE hSession);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedGetMsgSessionId
/// \~english @par Brief
///        Get session ID from received message.
/// \~english @param [in] hApp
///        HANDLE   - Handle for Application
/// \~english @retval UI_32 session ID value
/// \~english @retval MAX_SESSION_ID_VAL Failed to get session ID value
/// \~english @par Prerequisite
///       - Generation/Initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been
///       done.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - HANDLE specified in the argument (hApp) is NULL. [MAX_SESSION_ID_VAL]
///       - HANDLE specified in the argument (hApp) is not appropriate (which is invalid). [MAX_SESSION_ID_VAL]
/// \~english @par Detail
///        Get session ID from received message.\n
///        Mostly, this API is used to get session ID from message received by callback function that is registered to
///        Dispatcher.
/// \~english @par Classification
///          Public
/// \~english @see FrameworkunifiedGetSessionHandle
////////////////////////////////////////////////////////////////////////////////////////////
UI_32 FrameworkunifiedGetMsgSessionId(HANDLE hApp);







////////////////////////////////////////////////////////////////////////////////////////////
//        Defer Message APIs
////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedGetDeferQueueCnt
/// Get the number of messages in the defer queue
///
///
/// \param [in] hApp
///         HANDLE - Handle to the App / Thread
///
/// \return count
///         UI_32 - number of messages on Defered Queue
///
///
/// \see FrameworkunifiedDeferMessage, FrameworkunifiedRetrieveDeferMessage
///
///
////////////////////////////////////////////////////////////////////////////////////////////
UI_32 FrameworkunifiedGetDeferQueueCnt(HANDLE hApp);


////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedIsDeferQueueEmpty
/// \~english @par Brief
///        Empty determination of DeferQueue
/// \~english @param [in] hApp
///        HANDLE   - Handle for Application
/// \~english @retval BOOL DeferQueue status (TRUE: empty or FALSE: not empty)
/// \~english @retval TRUE Judgment failure
/// \~english @par Prerequisite
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [TRUE]
///       - If the hApp specified in the arguments is not appropriate (not valid) [TRUE]
/// \~english @par Detail
///        Returns whether the DeferQueue in the application-handle given by the arguments is empty
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Not applicable
/// \~english @see FrameworkunifiedDeferMessage, FrameworkunifiedRetrieveDeferMessage
/// FrameworkunifiedIsDeferQueueEmpty
/// Get the number of messages in the defer queue
///
///
/// \param [in] hApp
///         HANDLE - Handle to the App / Thread
///
/// \return count
///         BOOL - Indicates if the defer queue is empty, TRUE (empty), FALSE (not empty)
///
///
/// \see FrameworkunifiedDeferMessage, FrameworkunifiedRetrieveDeferMessage, FrameworkunifiedGetDeferQueueCnt
///
///
////////////////////////////////////////////////////////////////////////////////////////////
BOOL FrameworkunifiedIsDeferQueueEmpty(HANDLE hApp);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedDeferMessage
/// \~english @par Brief
///        Push messages to the DeferQueue
/// \~english @param [in] hApp
///        HANDLE   - Handle for Application
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @par Prerequisite
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [eFrameworkunifiedStatusInvldParam]
///       - If the hApp specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusInvldParam]
/// \~english @par Detail
///        Push received messages in the application handles specified by the arguments to the DeferQueue.
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Set Get
/// \~english @see FrameworkunifiedRetrieveDeferMessage
/// FrameworkunifiedDeferMessage
/// Defers the current message; Pushes the message on to a defer msg queue
///
///
/// \param [in] hApp
///         HANDLE - Handle to the App / Thread
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
///
/// \see FrameworkunifiedClearDeferMessages, FrameworkunifiedRetrieveDeferMessage
///
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedDeferMessage(HANDLE hApp);


////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedClearDeferMessages
/// Clear all the defer messages
///
///
/// \param [in] hApp
///         HANDLE - Handle to the App / Thread
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
///
/// \see FrameworkunifiedDeferMessage, FrameworkunifiedRetrieveDeferMessage
///
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedClearDeferMessages(HANDLE hApp);


////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedRetrieveDeferMessage
/// \~english @par Brief
///        Write Messages Registered in DeferQueue to FD
/// \~english @param [in] hApp
///        HANDLE   - Handle for Application
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @par Prerequisite
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [eFrameworkunifiedStatusInvldParam]
///       - If the hApp specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusInvldParam]
/// \~english @par Detail
///        Write the messages registered in the DeferQueue in the application handler specified by arguments to the FD.\n
///        This allows the main loop to receive the message.
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Set Get
/// \~english @see FrameworkunifiedDeferMessage
/// FrameworkunifiedRetrieveDeferMessage
/// Enables Checking for deferred message and retrieves messages when
/// callback returns to the dispatcher.
///
/// \param [in] hApp
///         HANDLE - Handle to the App / Thread
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
///
/// \see FrameworkunifiedDeferMessage, FrameworkunifiedClearDeferMessages
///
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedRetrieveDeferMessage(HANDLE hApp);

// TODO(framework_unified) : clean up the old FrameworkunifiedRegisterPersistentStorage api's
// 20110429_brp
// defined for backward compatibility; will be removed once the persistence feature is finalized
EFrameworkunifiedStatus FrameworkunifiedRegisterPersistentStorage(HANDLE hApp, PCSTR pFullFilePath);
EFrameworkunifiedStatus FrameworkunifiedReleaseFileToPersistentStorage(HANDLE hApp, PCSTR pFullFilePath, BOOL persist);



////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedRegisterEvent
/// \~english @par Brief
///        Registering Applications for Delivery of Service-Specific Events
/// \~english @param [in] hSession
///        HANDLE   - Handles for sessions (obtained by FrameworkunifiedOpenSession)
/// \~english @param [in] uiEventId
///        UI_32   - Event ID (Identification ID on the service's protocol)
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusNullPointer Specifying NULL pointers
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid handle type
/// \~english @retval eFrameworkunifiedStatusInvldQName Invalid message queue name
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid file descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR Generating an interrupt by a system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer size
/// \~english @retval eFrameworkunifiedStatusFail Some error occurred
/// \~english @par Prerequisite
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
///       - Services and sessions are established (FrameworkunifiedOpenSession)
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hSession specified in the parameter is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - If the hSession specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusNullPointer]
///       - When the message queue handle in the handle (hSession) specified by the arguments is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - If the application name in the handler (hSession) specified by the arguments is not set, [eFrameworkunifiedStatusInvldHandle]
///       - When the message queue handle specified in the argument (hSession) is invalid (invalid) [eFrameworkunifiedStatusInvldHandle]
///       - When the name of the application in the handle (hSession) specified by the arguments exceeds 20 bytes [eFrameworkunifiedStatusInvldQName]
///       - When the name of the message queue in the handle (hSession) specified in the arguments is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - When the name of the message queue in the handle (hSession) specified in the argument exceeds 20 bytes [eFrameworkunifiedStatusInvldHandle]
///       - When the message queue type of the handle (hSession) specified in the parameter is not the send type, [eFrameworkunifiedStatusInvldHndlType]
///       - Message queue for sessions for services is full [eFrameworkunifiedStatusMsgQFull]
///       - Incorrect file descriptor for sending messages for sessions intended for the service [eFrameworkunifiedStatusErrNoEBADF]
///       - When an interrupt by a system call (signal) occurs while sending session-specific messages for services [eFrameworkunifiedStatusErrNoEINTR]
///       - Incorrect size of send buffers for sessions intended for services [eFrameworkunifiedStatusInvldBufSize]
///       - If there are any errors sending messages for sessions intended for the services [eFrameworkunifiedStatusFail]
/// \~english @par Detail
///        Issues a request for registering the application that executes this API to the delivery target of the device-specific events of the service associated with the handle (hSession) specified in the arguments.\n
///        The application specifies the type of events to receive in the uiEventId of arguments.\n
///        (The event ID of this API refers to the identification ID on the protocol of the service, and the application identifies the content of the message. The specifiable service-specific events are events set by the service, such as the FrameworkunifiedDefinePublicStateEvents.)
/// \~english @par
///        Please note the following points when using this API for application.
///       - This API is deprecated and should now use FrameworkunifiedSubscribeToSessionEventWithCallback.
///       - When this API is used, the application shall use a FrameworkunifiedAttachCallbackToDispatcher or the like to register callback functions in its own Dispatcher for handling events.
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Pub-Sub
/// \~english @see FrameworkunifiedRegisterEvents, FrameworkunifiedUnRegisterEvent, FrameworkunifiedUnRegisterEvents,
///                 FrameworkunifiedAttachCallbacksToDispatcher, FrameworkunifiedAttachCallbackToDispatcher,
///                 FrameworkunifiedAttachCallbacksToDispatcherWithFd, FrameworkunifiedAttachCallbackToDispatcherWithFd,
///                 FrameworkunifiedSubscribeToSessionEventWithCallback, FrameworkunifiedSubscribeToSessionEventsWithCallbacks,
///                 FrameworkunifiedDefineStateEvents, FrameworkunifiedDefinePublicStateEvents, FrameworkunifiedDefinePrivateStateEvents
///
/// API to register for event of a service.
/// This API is depricated, use FrameworkunifiedSubscribeToSessionEventWithCallback.
///
/// \param [in] hSession
///         HANDLE - Handle to the application session
/// \param [in] uiEventId
///     UI_32 - Event registering for.
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see FrameworkunifiedRegisterEvents, FrameworkunifiedUnRegisterEvent, FrameworkunifiedUnRegisterEvents, FrameworkunifiedBroadcastEvent, FrameworkunifiedDefineStateEvents,
/// FrameworkunifiedPublishEvent
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedRegisterEvent(HANDLE hSession, UI_32 uiEventId);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedRegisterEvents
/// \~english @par Brief
///        Registering Applications for Delivery of Service-Specific Events
/// \~english @param [in] hSession
///        HANDLE   - Handles for sessions (obtained by FrameworkunifiedOpenSession)
/// \~english @param [in] puiEventsArray
///        PVOID   - The event ID to deliver (the identification ID on the service's protocol)
/// \~english @param [in] uiListSize
///        UI_32   - Number of elements in the event ID array (0 to 4294967295)
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusNullPointer Specifying NULL pointers
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldBuf Bad buffer
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid handle type
/// \~english @retval eFrameworkunifiedStatusInvldQName Invalid message queue name
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid file descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR Generating an interrupt by a system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer size
/// \~english @retval eFrameworkunifiedStatusFail Some error occurred
/// \~english @retval eFrameworkunifiedStatusErrOther Other errors (Failed to open/allocate shared memory, etc.)
/// \~english @par Prerequisite
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
///       - Services and sessions are established (FrameworkunifiedOpenSession)
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hSession specified in the parameter is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - If the hSession specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusNullPointer]
///       - When the message queue handle in the handle (hSession) specified by the arguments is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - If the application name in the handler (hSession) specified by the arguments is not set, [eFrameworkunifiedStatusInvldHandle]
///       - When the message queue handle specified in the argument (hSession) is invalid (invalid) [eFrameworkunifiedStatusInvldHandle]
///       - When the array (puiEventsArray) of event IDs specified by arguments is NULL, [eFrameworkunifiedStatusInvldBuf]
///       - When the name of the application in the handle (hSession) specified by the arguments exceeds 20 bytes [eFrameworkunifiedStatusInvldQName]
///       - When the name of the message queue in the handle (hSession) specified in the arguments is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - When the name of the message queue in the handle (hSession) specified in the argument exceeds 20 bytes [eFrameworkunifiedStatusInvldHandle]
///       - When the message queue type of the handle (hSession) specified in the parameter is not the send type, [eFrameworkunifiedStatusInvldHndlType]
///       - Message queue for sessions for services is full [eFrameworkunifiedStatusMsgQFull]
///       - Incorrect file descriptor for sending messages for sessions intended for the service [eFrameworkunifiedStatusErrNoEBADF]
///       - When an interrupt by a system call (signal) occurs while sending session-specific messages for services [eFrameworkunifiedStatusErrNoEINTR]
///       - Incorrect size of send buffers for sessions intended for services [eFrameworkunifiedStatusInvldBufSize]
///       - If there are any errors sending messages for sessions intended for the services [eFrameworkunifiedStatusFail]
///       - Failed to access shared memory for sending messages for sessions for services [eFrameworkunifiedStatusErrOther]
/// \~english @par Detail
///        Issues a request for registering the application that executes this API to the delivery target of the device-specific events of the service associated with the handle (hSession) specified in the arguments.\n
///        The application specifies more than one type of events to receive in the puiEventsArray of arguments.\n
///        (The event ID of this API refers to the identification ID on the protocol of the service, and the application identifies the content of the message. The specifiable service-specific events are events set by the service, such as the FrameworkunifiedDefinePublicStateEvents.)
/// \~english @par
///        Please note the following points when using this API for application.
///       - This API is deprecated and should now use FrameworkunifiedSubscribeToSessionEventsWithCallbacks.
///       - When this API is used, the application shall use a FrameworkunifiedAttachCallbackToDispatcher or the like to register callback functions in its own Dispatcher for handling events.
///       - The maximum number of event IDs that can be registered depends on the size that can be allocated as the area for the event ID array.\n
///         Therefore, the API caller should consider the size of the event ID array and define the number of event IDs (uiListSize).
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Pub-Sub
/// \~english @see FrameworkunifiedRegisterEvent, FrameworkunifiedUnRegisterEvent, FrameworkunifiedUnRegisterEvents,
///                 FrameworkunifiedAttachCallbacksToDispatcher, FrameworkunifiedAttachCallbackToDispatcher,
///                 FrameworkunifiedAttachCallbacksToDispatcherWithFd, FrameworkunifiedAttachCallbackToDispatcherWithFd,
///                 FrameworkunifiedSubscribeToSessionEventWithCallback, FrameworkunifiedSubscribeToSessionEventsWithCallbacks,
///                 FrameworkunifiedDefineStateEvents, FrameworkunifiedDefinePublicStateEvents, FrameworkunifiedDefinePrivateStateEvents
///
/// API to register for events of a service.
/// This API is depricated, use FrameworkunifiedSubscribeToSessionEventsWithCallbacks.
///
/// \param [in] hSession
///         HANDLE - Handle to the application session
/// \param [in] puiEventsArray
///     PVOID - Array of events registering for.
/// \param [in] uiListSize
///     UI_32 - count of elements in array.
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see FrameworkunifiedRegisterEvent, FrameworkunifiedUnRegisterEvent, FrameworkunifiedUnRegisterEvents, FrameworkunifiedBroadcastEvent, FrameworkunifiedDefineStateEvents,
/// FrameworkunifiedPublishEvent
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedRegisterEvents(HANDLE hSession, PVOID puiEventsArray, UI_32 uiListSize);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedUnRegisterEvent
/// \~english @par Brief
///        Unregistering Service-Specific Events from Delivery
/// \~english @param [in] hSession
///        HANDLE   - Handles for sessions (obtained by FrameworkunifiedOpenSession)
/// \~english @param [in] uiEventId
///        UI_32   - Event ID (Identification ID on the service's protocol)
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusNullPointer Specifying NULL pointers
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid handle type
/// \~english @retval eFrameworkunifiedStatusInvldQName Invalid message queue name
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid file descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR Generating an interrupt by a system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer size
/// \~english @retval eFrameworkunifiedStatusFail Some error occurred
/// \~english @par Prerequisite
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
///       - Services and sessions are established (FrameworkunifiedOpenSession)
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hSession specified in the parameter is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - If the hSession specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusNullPointer]
///       - When the message queue handle in the handle (hSession) specified by the arguments is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - If the application name in the handler (hSession) specified by the arguments is not set, [eFrameworkunifiedStatusInvldHandle]
///       - When the message queue handle specified in the argument (hSession) is invalid (invalid) [eFrameworkunifiedStatusInvldHandle]
///       - When the name of the application in the handle (hSession) specified by the arguments exceeds 20 bytes [eFrameworkunifiedStatusInvldQName]
///       - When the name of the message queue in the handle (hSession) specified in the arguments is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - When the name of the message queue in the handle (hSession) specified in the argument exceeds 20 bytes [eFrameworkunifiedStatusInvldHandle]
///       - When the message queue type of the handle (hSession) specified in the parameter is not the send type, [eFrameworkunifiedStatusInvldHndlType]
///       - Message queue for sessions for services is full [eFrameworkunifiedStatusMsgQFull]
///       - Incorrect file descriptor for sending messages for sessions intended for the service [eFrameworkunifiedStatusErrNoEBADF]
///       - When an interrupt by a system call (signal) occurs while sending session-specific messages for services [eFrameworkunifiedStatusErrNoEINTR]
///       - Incorrect size of send buffers for sessions intended for services [eFrameworkunifiedStatusInvldBufSize]
///       - If there are any errors sending messages for sessions intended for the services [eFrameworkunifiedStatusFail]
/// \~english @par Detail
///        Issues a request to unregister the application that executed this API from the delivery target of the service-specific event associated with the handle (hSession) specified in the arguments.\n
///        The application specifies events that are no longer needed for delivery in the uiEventId of arguments.\n
///        (The event ID of this API is the identification ID on the protocol of the service, which the application identifies the content of the message.)
/// \~english @par
///        Please note the following points when using this API for application.
///       - This API is deprecated and should now use FrameworkunifiedUnSubscribeSessionEventWithCallback.
///       - When this API is used, the application shall unregister callbacks for events registered in its own Dispatcher by means of FrameworkunifiedDetachCallbackFromDispatcher, etc.
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Pub-Sub
/// \~english @see FrameworkunifiedRegisterEvent, FrameworkunifiedRegisterEvents, FrameworkunifiedUnRegisterEvents,
///                 FrameworkunifiedDetachCallbackFromDispatcher, FrameworkunifiedDetachCallbacksFromDispatcher,
///                 FrameworkunifiedDetachCallbackFromDispatcherWithFd, FrameworkunifiedDetachCallbacksFromDispatcherWithFd,
///                 FrameworkunifiedUnSubscribeSessionEventWithCallback, FrameworkunifiedUnSubscribeSessionEventsWithCallbacks,
///                 FrameworkunifiedDefineStateEvents, FrameworkunifiedDefinePublicStateEvents, FrameworkunifiedDefinePrivateStateEvents
///
/// API to unregister from event of a service.
/// This API is depricated, use FrameworkunifiedUnSubscribeSessionEventsWithCallbacks.
///
/// \param [in] hSession
///         HANDLE - Handle to the application session
/// \param [in] uiEventId
///     UI_32 - Event registering for.
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see FrameworkunifiedUnRegisterEvents, FrameworkunifiedRegisterEvent, FrameworkunifiedBroadcastEvent, FrameworkunifiedDefineStateEvents, FrameworkunifiedPublishEvent
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedUnRegisterEvent(HANDLE hSession, UI_32 uiEventId);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedUnRegisterEvents
/// \~english @par Brief
///        Unregistering Service-Specific Events from Delivery
/// \~english @param [in] hSession
///        HANDLE   - Handles for sessions (obtained by FrameworkunifiedOpenSession)
/// \~english @param [in] puiEventsArray
///        PVOID   - The event ID to deliver (the identification ID on the service's protocol)
/// \~english @param [in] uiListSize
///        UI_32   - Number of elements in the event ID array (0 to 4294967295)
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusNullPointer Specifying NULL pointers
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldBuf Bad buffer
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid handle type
/// \~english @retval eFrameworkunifiedStatusInvldQName Invalid message queue name
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid file descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR Generating an interrupt by a system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer size
/// \~english @retval eFrameworkunifiedStatusFail Some error occurred
/// \~english @retval eFrameworkunifiedStatusErrOther Other errors (Failed to open/allocate shared memory, etc.)
/// \~english @par Prerequisite
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
///       - Services and sessions are established (FrameworkunifiedOpenSession)
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hSession specified in the parameter is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - If the hSession specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusNullPointer]
///       - When the message queue handle in the handle (hSession) specified by the arguments is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - If the application name in the handler (hSession) specified by the arguments is not set, [eFrameworkunifiedStatusInvldHandle]
///       - When the message queue handle specified in the argument (hSession) is invalid (invalid) [eFrameworkunifiedStatusInvldHandle]
///       - When the array (puiEventsArray) of event IDs specified by arguments is NULL, [eFrameworkunifiedStatusInvldBuf]
///       - When the name of the application in the handle (hSession) specified by the arguments exceeds 20 bytes [eFrameworkunifiedStatusInvldQName]
///       - When the name of the message queue in the handle (hSession) specified in the arguments is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - When the name of the message queue in the handle (hSession) specified in the argument exceeds 20 bytes [eFrameworkunifiedStatusInvldHandle]
///       - When the message queue type of the handle (hSession) specified in the parameter is not the send type, [eFrameworkunifiedStatusInvldHndlType]
///       - Message queue for sessions for services is full [eFrameworkunifiedStatusMsgQFull]
///       - Incorrect file descriptor for sending messages for sessions intended for the service [eFrameworkunifiedStatusErrNoEBADF]
///       - When an interrupt by a system call (signal) occurs while sending session-specific messages for services [eFrameworkunifiedStatusErrNoEINTR]
///       - Incorrect size of send buffers for sessions intended for services [eFrameworkunifiedStatusInvldBufSize]
///       - If there are any errors sending messages for sessions intended for the services [eFrameworkunifiedStatusFail]
///       - Failed to access shared memory for sending messages for sessions for services [eFrameworkunifiedStatusErrOther]
/// \~english @par Detail
///        Issues a request to unregister the application that executed this API from the delivery target of the service-specific event associated with the handle (hSession) specified in the arguments.\n
///        The application specifies the unneeded events in the puiEventsArray of arguments.\n
///        (The event ID of this API is the identification ID on the protocol of the service, which the application identifies the content of the message.)
/// \~english @par
///        Please note the following points when using this API for application.
///       - This API is deprecated and should now use FrameworkunifiedUnSubscribeSessionEventsWithCallbacks.
///       - After executing this API, the callback of the event registered in the Dispatcher of the application itself must be unregistered by FrameworkunifiedDetachCallbackFromDispatcher, etc.
///       - The maximum number of event IDs that can be registered depends on the size that can be allocated as the area for the event ID array.\n
///         Therefore, the API caller should consider the size of the event ID array and define the number of event IDs (uiListSize).
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Pub-Sub
/// \~english @see FrameworkunifiedRegisterEvent, FrameworkunifiedRegisterEvents, FrameworkunifiedUnRegisterEvent,
///                 FrameworkunifiedDetachCallbackFromDispatcher, FrameworkunifiedDetachCallbacksFromDispatcher,
///                 FrameworkunifiedDetachCallbackFromDispatcherWithFd, FrameworkunifiedDetachCallbacksFromDispatcherWithFd,
///                 FrameworkunifiedUnSubscribeSessionEventWithCallback, FrameworkunifiedUnSubscribeSessionEventsWithCallbacks,
///                 FrameworkunifiedDefineStateEvents, FrameworkunifiedDefinePublicStateEvents, FrameworkunifiedDefinePrivateStateEvents
///
/// API to unregister from events of a service.
/// This API is depricated, use FrameworkunifiedUnSubscribeSessionEventsWithCallbacks.
///
/// \param [in] hSession
///         HANDLE - Handle to the application session
/// \param [in] puiEventsArray
///     PVOID - Array of events registering for.
/// \param [in] uiListSize
///     UI_32 - count of elements in array.
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see FrameworkunifiedUnRegisterEvent, FrameworkunifiedRegisterEvent, FrameworkunifiedBroadcastEvent, FrameworkunifiedDefineStateEvents, FrameworkunifiedPublishEvent
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedUnRegisterEvents(HANDLE hSession, PVOID puiEventsArray, UI_32 uiListSize);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedDefineStateEvents
/// \~english @par Brief
///        Subscribing to Application-Specific Events in the Dispatcher
/// \~english @param [in] hApp
///        HANDLE   - Handle for the application
/// \~english @param [in] puiEvents
///        PVOID   - Pointer to an array (UI_32 arrays) of event IDs (identification IDs on the service protocol)
/// \~english @param [in] uiListSize
///        UI_32   - Number of elements in the event ID array (0 to 4294967295)
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @par Prerequisite
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - If the hApp specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusInvldHandle]
///       - When the array (puiEvents) of event IDs specified by arguments is NULL, [eFrameworkunifiedStatusInvldHandle]
/// \~english @par Detail
///        Register unique events in your Dispatcher that applications can deliver to other applications.\n
///        (The event ID of this API is the identification ID on the protocol of the service, which the application identifies the content of the message.)\n
///        For application-specific events registered by this API, the event is delivered by Dispatcher when the application executes the FrameworkunifiedPublishPublicEvent.\n
///        Events are distributed to applications registered by FrameworkunifiedSubscribeToSessionEventsWithCallbacks, etc.
/// \~english @par
///        Please note the following points when using this API for application.
///       - This API is deprecated, use FrameworkunifiedDefinePublicStateEvents or FrameworkunifiedDefinePrivateStateEvents now.
///       - The maximum number of event IDs that can be registered depends on the size that can be allocated as the area for the event ID array.\n
///         Therefore, the API caller should consider the size of the event ID array and define the number of event IDs (uiListSize).
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Pub-Sub
/// \~english @see FrameworkunifiedDefinePublicStateEvents, FrameworkunifiedDefinePrivateStateEvents,
///                 FrameworkunifiedPublishEvent, FrameworkunifiedBroadcastEvent, FrameworkunifiedPublishPublicEvent, FrameworkunifiedPublishPrivateEvent
///                 FrameworkunifiedRegisterEvent, FrameworkunifiedRegisterEvents, FrameworkunifiedUnRegisterEvent, FrameworkunifiedUnRegisterEvents,
///                 FrameworkunifiedSubscribeToSessionEventWithCallback, FrameworkunifiedSubscribeToSessionEventsWithCallbacks,
///                 FrameworkunifiedUnSubscribeSessionEventWithCallback, FrameworkunifiedUnSubscribeSessionEventsWithCallbacks,
///
/// API to define the state events of the service.
/// This API is depricated, use FrameworkunifiedDefinePublicStateEvents or FrameworkunifiedDefinePrivateStateEvents.
///
/// \param [in] hApp
///         HANDLE - Handle to the application framework
/// \param [in] puiEvents
///     PVOID - Array of state events or single state event.
/// \param [in] uiListSize
///     UI_32 - size of events list.
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see FrameworkunifiedRegisterEvent, FrameworkunifiedBroadcastEvent, FrameworkunifiedPublishEvent, FrameworkunifiedUnRegisterEvent, FrameworkunifiedUnRegisterEvents
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedDefineStateEvents(HANDLE hApp, PVOID puiEvents, UI_32 uiListSize);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedPublishEvent
/// \~english @par Brief
///        Deliver application-specific events
/// \~english @param [in] hApp
///        HANDLE   - Handle for the application
/// \~english @param [in] uiEventId
///        UI_32   - The event ID to deliver (the identification ID on the service's protocol)
/// \~english @param [in] pClientName
///        PCSTR   - Destination application name (distributed to all registered applications when NULL is specified),
/// \~english @param [in] pData
///        PCVOID   - Pointer to trasmission data
/// \~english @param [in] iLength
///        UI_32   - Transmission data length
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusErrOther Other errors (Failed to open/allocate shared memory, etc.)
/// \~english @par Prerequisite
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
///       - The events to be delivered must be specified in a FrameworkunifiedDefinePublicStateEvents, etc.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - If the hApp specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusInvldHandle]
///       - When the application name of the delivery destination in the handle (hApp) specified by the arguments is NULL, [eFrameworkunifiedStatusInvldParam]
///       - Sessioninformation is not found with the application being delivered to within the handles (hApp) specified in the arguments. [eFrameworkunifiedStatusErrOther]
///       - If the destination handles are inappropriate (disabled) [eFrameworkunifiedStatusErrOther]
///       - If the message queue handle in the destination handle is NULL [eFrameworkunifiedStatusErrOther]
///       - If the application name in the destination handles is not set, [eFrameworkunifiedStatusErrOther]
///       - If the message queue handle in the destination handle is inappropriate (disabled) [eFrameworkunifiedStatusErrOther]
///       - When the send data length (iLength) specified in the arguments is not 0 and the pointer to the send data pointer (pData) is NULL, [eFrameworkunifiedStatusErrOther]
///       - When the name of the application in the destination handle exceeds 20 bytes [eFrameworkunifiedStatusErrOther]
///       - When the name of the message queue in the destination handle is NULL, [eFrameworkunifiedStatusErrOther]
///       - When the name of the message queue in the destination handle exceeds 20 bytes [eFrameworkunifiedStatusErrOther]
///       - If the message queue type of the destination handler is not the send type [eFrameworkunifiedStatusErrOther]
///       - When the message queue is full [eFrameworkunifiedStatusErrOther]
///       - File descriptor for sending messages is invalid [eFrameworkunifiedStatusErrOther]
///       - When an interrupt by a systemcall (signal) occurs while sending messages [eFrameworkunifiedStatusErrOther]
///       - If the size of the message transmission buffer is invalid [eFrameworkunifiedStatusErrOther]
///       - If there are any errors sending messages [eFrameworkunifiedStatusErrOther]
///       - Failed to access shared memory for sending messages [eFrameworkunifiedStatusErrOther]
/// \~english @par Detail
///        Delivers application-specific events to subscribed applications.\n
///        (The event ID of this API is the identification ID on the protocol of the service, which the application identifies the content of the message.)\n
///        The delivery destination of events is the application specified by the argument pClientName or (when the argument pClientName is NULL)
///        all the applications registered by FrameworkunifiedSubscribeToSessionEventsWithCallbacks etc.\n
///        The data (pData) to be distributed is the same for all distribution destinations.
/// \~english @par
///        Please note the following points when using this API for application.\n
///       - Operation cannot be guaranteed if the size of the area indicated by the pointer (pData) address to the send data does not match the size specified by the send data length (iLength) address.
/// \~english @par
///        This API is deprecated, use FrameworkunifiedPublishPublicEvent or FrameworkunifiedPublishPrivateEvent now.
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Pub-Sub
/// \~english @see FrameworkunifiedDefineStateEvents, FrameworkunifiedDefinePublicStateEvents,
///                 FrameworkunifiedBroadcastEvent, FrameworkunifiedPublishPublicEvent,
///                 FrameworkunifiedRegisterEvent, FrameworkunifiedRegisterEvents, FrameworkunifiedUnRegisterEvent, FrameworkunifiedUnRegisterEvents,
///                 FrameworkunifiedAttachCallbacksToDispatcher, FrameworkunifiedAttachCallbackToDispatcher,
///                 FrameworkunifiedAttachCallbacksToDispatcherWithFd, FrameworkunifiedAttachCallbackToDispatcherWithFd,
///                 FrameworkunifiedDetachCallbackFromDispatcher, FrameworkunifiedDetachCallbacksFromDispatcher,
///                 FrameworkunifiedDetachCallbackFromDispatcherWithFd, FrameworkunifiedDetachCallbacksFromDispatcherWithFd,
///                 FrameworkunifiedSubscribeToSessionEventWithCallback, FrameworkunifiedSubscribeToSessionEventsWithCallbacks,
///                 FrameworkunifiedUnSubscribeSessionEventWithCallback, FrameworkunifiedUnSubscribeSessionEventsWithCallbacks
///
/// This API is used to publish the event to one of the client subscribed to this event.
/// This API can be used to publish the event to all the clients subscribed to this event by
/// passing pClientName as NULL.
/// This API is depricated, use FrameworkunifiedPublishPrivateEvent or FrameworkunifiedPublishPublicEvent.
///
/// \param [in] hApp
///         HANDLE - Handle to the application framework
/// \param [in] uiEventId
///         UI_32 - Event id of Message
/// \param [in] pClientName
///         PCSTR - Name of the client
/// \param [in] pData
///         PCVOID - Data buffer
/// \param [in] iLength
///         UI_32 - Size of data buffer
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see FrameworkunifiedRegisterEvent, FrameworkunifiedBroadcastEvent, FrameworkunifiedDefineStateEvents, FrameworkunifiedUnRegisterEvent, FrameworkunifiedUnRegisterEvents
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedPublishEvent(HANDLE hApp, UI_32 uiEventId, PCSTR pClientName, PCVOID pData, UI_32 iLength);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedBroadcastEvent
/// \~english @par Brief
///        Broadcast application-specific events
/// \~english @param [in] hApp
///        HANDLE   - Handle for the application
/// \~english @param [in] uiEventId
///        UI_32   - The event ID to deliver (the identification ID on the service's protocol)
/// \~english @param [in] pData
///        PCVOID   - Pointer to trasmission data
/// \~english @param [in] iLength
///        UI_32   - Transmission data length
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusNullPointer Specifying NULL pointers
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusErrOther Other errors
/// \~english @par Prerequisite
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
///       - The events to be delivered must be specified in a FrameworkunifiedDefinePublicStateEvents, etc.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [eFrameworkunifiedStatusNullPointer]
///       - If the hApp specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusNullPointer]
///       - When the application name of the delivery destination in the handle (hApp) specified by the arguments is NULL, [eFrameworkunifiedStatusInvldParam]
///       - Sessioninformation is not found with the application being delivered to within the handles (hApp) specified in the arguments. [eFrameworkunifiedStatusErrOther]
///       - If the destination handles are inappropriate (disabled) [eFrameworkunifiedStatusErrOther]
///       - If the message queue handle in the destination handle is NULL [eFrameworkunifiedStatusErrOther]
///       - If the application name in the destination handles is not set, [eFrameworkunifiedStatusErrOther]
///       - If the message queue handle in the destination handle is inappropriate (disabled) [eFrameworkunifiedStatusErrOther]
///       - When the send data length (iLength) specified in the arguments is not 0 and the pointer to the send data pointer (pData) is NULL, [eFrameworkunifiedStatusErrOther]
///       - When the name of the application in the destination handle exceeds 20 bytes [eFrameworkunifiedStatusErrOther]
///       - When the name of the message queue in the destination handle is NULL, [eFrameworkunifiedStatusErrOther]
///       - When the name of the message queue in the destination handle exceeds 20 bytes [eFrameworkunifiedStatusErrOther]
///       - If the message queue type of the destination handler is not the send type [eFrameworkunifiedStatusErrOther]
///       - When the message queue is full [eFrameworkunifiedStatusErrOther]
///       - File descriptor for sending messages is invalid [eFrameworkunifiedStatusErrOther]
///       - When an interrupt by a systemcall (signal) occurs while sending messages [eFrameworkunifiedStatusErrOther]
///       - If the size of the message transmission buffer is invalid [eFrameworkunifiedStatusErrOther]
///       - If there are any errors sending messages [eFrameworkunifiedStatusErrOther]
/// \~english @par Detail
///        Broadcast application-specific events to all registered applications.\n
///        (The event ID of this API is the identification ID on the protocol of the service, which the application identifies the content of the message.)\n
///        Events are delivered to all applications registered by FrameworkunifiedSubscribeToSessionEventsWithCallbacks, etc.
///        The data (pData) to be distributed is the same for all distribution destinations.
/// \~english @par
///        This API is deprecated and should now use FrameworkunifiedPublishPublicEvent.
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Pub-Sub
/// \~english @see FrameworkunifiedDefineStateEvents, FrameworkunifiedDefinePublicStateEvents,
///                 FrameworkunifiedPublishEvent, FrameworkunifiedPublishPublicEvent,
///                 FrameworkunifiedRegisterEvents, FrameworkunifiedUnRegisterEvent, FrameworkunifiedUnRegisterEvents,
///                 FrameworkunifiedAttachCallbacksToDispatcher, FrameworkunifiedAttachCallbackToDispatcher,
///                 FrameworkunifiedAttachCallbacksToDispatcherWithFd, FrameworkunifiedAttachCallbackToDispatcherWithFd,
///                 FrameworkunifiedDetachCallbackFromDispatcher, FrameworkunifiedDetachCallbacksFromDispatcher,
///                 FrameworkunifiedDetachCallbackFromDispatcherWithFd, FrameworkunifiedDetachCallbacksFromDispatcherWithFd,
///                 FrameworkunifiedSubscribeToSessionEventWithCallback, FrameworkunifiedSubscribeToSessionEventsWithCallbacks,
///                 FrameworkunifiedUnSubscribeSessionEventWithCallback, FrameworkunifiedUnSubscribeSessionEventsWithCallbacks
///
/// This API is used to publish the event to all the clients subscribed to this event.
/// This API is depricated, use FrameworkunifiedPublishPublicEvent.
///
/// \param [in] hApp
///         HANDLE - Handle to the application framework
/// \param [in] uiEventId
///         UI_32 - Event id of Message
/// \param [in] pData
///         PCVOID - Data buffer
/// \param [in] iLength
///         UI_32 - Size of data buffer
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see FrameworkunifiedRegisterEvent, FrameworkunifiedPublishEvent, FrameworkunifiedDefineStateEvents, FrameworkunifiedUnRegisterEvent, FrameworkunifiedUnRegisterEvents
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedBroadcastEvent(HANDLE hApp, UI_32 uiEventId, PCVOID pData, UI_32 iLength);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedDefinePublicStateEvents
/// \~english @par Brief
///        Subscribing to Application-Specific Events in the Dispatcher
/// \~english @param [in] hApp
///        HANDLE   - Handle for the application
/// \~english @param [in] puiEvents
///        PUI_32   - The event ID to deliver (the identification ID on the service's protocol)
/// \~english @param [in] uiListSize
///        UI_32   - Number of elements in the event ID array (0 to 4294967295)
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @par Prerequisite
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - If the hApp specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusInvldHandle]
///       - When the array (puiEvents) of event IDs specified by arguments is NULL, [eFrameworkunifiedStatusInvldHandle]
/// \~english @par Detail
///        Register unique events in your Dispatcher that applications can deliver to other applications.\n
///        (The event ID of this API is the identification ID on the protocol of the service, which the application identifies the content of the message.)\n
///        For application-specific events registered by this API, the event is delivered by Dispatcher when the application executes the FrameworkunifiedPublishPublicEvent.\n
///        Events are distributed to all applications registered by FrameworkunifiedSubscribeToSessionEventsWithCallbacks, etc.
/// \~english @par
///        The difference from FrameworkunifiedDefinePrivateStateEvents is that when the Dispatcher distributes events registered by this API, the same data is distributed to all registered applications.
/// \~english @par
///        Please note the following points when using this API for application.
///       - The maximum number of event IDs that can be registered depends on the size that can be allocated as the area for the event ID array.\n
///         Therefore, the API caller should consider the size of the event ID array and define the number of event IDs (uiListSize).
///       - FrameworkunifiedDefinePrivateStateEvents should be used to deliver individual data by registered application sessions.
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Pub-Sub
/// \~english @see FrameworkunifiedPublishPublicEvent, FrameworkunifiedDefinePrivateStateEvents, FrameworkunifiedPublishPrivateEvent,
///                 FrameworkunifiedSubscribeToSessionEventWithCallback, FrameworkunifiedSubscribeToSessionEventsWithCallbacks,
///                 FrameworkunifiedUnSubscribeSessionEventWithCallback, FrameworkunifiedUnSubscribeSessionEventsWithCallbacks
///
/// This API is used to define the public state events of the service.
/// Public Event means the data associated with the event will be same for all the clients
/// subscribed to it.
///
/// \param [in] hApp
///         HANDLE - Handle to the application framework
/// \param [in] puiEvents
///     PUI_32 - Array of state events or single state event.
/// \param [in] uiListSize
///     UI_32 - size of events list.
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see FrameworkunifiedDefinePrivateStateEvents
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedDefinePublicStateEvents(HANDLE hApp, PUI_32 puiEvents, UI_32 uiListSize);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedDefinePrivateStateEvents
/// \~english @par Brief
///        Subscribing to Application-Specific Events in the Dispatcher
/// \~english @param [in] hApp
///        HANDLE   - Handle for the application
/// \~english @param [in] puiEvents
///        PUI_32   - The event ID to deliver (the identification ID on the service's protocol)
/// \~english @param [in] uiListSize
///        UI_32   - Number of elements in the event ID array (0 to 4294967295)
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @par Prerequisite
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - If the hApp specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusInvldHandle]
///       - When the array (puiEvents) of event IDs specified by arguments is NULL, [eFrameworkunifiedStatusInvldHandle]
/// \~english @par Detail
///        Register unique events in your Dispatcher that applications can deliver to other applications.\n
///        (The event ID of this API is the identification ID on the protocol of the service, which the application identifies the content of the message.)\n
///        For application-specific events registered by this API, the event is delivered by Dispatcher when the application executes the FrameworkunifiedPublishPrivateEvent.\n
///        Events are distributed to applications registered by FrameworkunifiedSubscribeToSessionEventsWithCallbacks, etc.
/// \~english @par
///        The difference from FrameworkunifiedDefinePublicStateEvents is that when the Dispatcher distributes events registered by this API, it distributes individual data by sessions of the registered application.
/// \~english @par
///        Please note the following points when using this API for application.
///       - The maximum number of event IDs that can be registered depends on the size that can be allocated as the area for the event ID array.\n
///         Therefore, the API caller should consider the size of the event ID array and define the number of event IDs (uiListSize).
///       - Use FrameworkunifiedDefinePublicStateEvents to deliver broadcasts to all registered applications.
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Pub-Sub
/// \~english @see FrameworkunifiedPublishPrivateEvent, FrameworkunifiedDefinePublicStateEvents, FrameworkunifiedPublishPublicEvent,
///                 FrameworkunifiedSubscribeToSessionEventWithCallback, FrameworkunifiedSubscribeToSessionEventsWithCallbacks,
///                 FrameworkunifiedUnSubscribeSessionEventWithCallback, FrameworkunifiedUnSubscribeSessionEventsWithCallbacks
///
/// This API is used to define the private state events of the service.
/// Private event means the data associated with the event will be specific for
/// every session of all the clients on which it has subscribed.
///
/// \param [in] hApp
///         HANDLE - Handle to the application framework
/// \param [in] puiEvents
///     PUI_32 - Array of state events or single state event.
/// \param [in] uiListSize
///     UI_32 - size of events list.
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see FrameworkunifiedDefinePublicStateEvents
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedDefinePrivateStateEvents(HANDLE hApp, PUI_32 puiEvents, UI_32 uiListSize);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedSubscribeToSessionEventWithCallback
/// \~english @par Brief
///        Register delivery of service-specific events and set callback for receiving events
/// \~english @param [in] hApp
///        HANDLE   - Handle for the application
/// \~english @param [in] uiEventId
///        UI_32   - Event ID (Identification ID on the service protocol) (PROTOCOL_FRAMEWORKUNIFIED_BASE_CMD to PROTOCOL_FRAMEWORKUNIFIED_MAX_CMD)
/// \~english @param [in] fpOnCmd
///        CbFuncPtr   - Pointer to callback function when event is received
/// \~english @param [in] hSession
///        HANDLE   - Handles for sessions (obtained by FrameworkunifiedOpenSession)
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid handle type
/// \~english @retval eFrameworkunifiedStatusInvldQName Invalid message queue name
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid file descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR Generating an interrupt by a system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer size
/// \~english @retval eFrameworkunifiedStatusFail Some error occurred
/// \~english @par Prerequisite
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
///       - Services and sessions are established (FrameworkunifiedOpenSession)
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - If the hApp specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusInvldHandle]
///       - If the hSession specified in the parameter is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - If the hSession specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusInvldHandle]
///       - If the callback function pointer (fpOnCmd) specified in the parameter is NULL, [eFrameworkunifiedStatusInvldParam]
///       - When the message queue handle in the handle (hSession) specified by the arguments is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - If the application name in the handler (hSession) specified by the arguments is not set, [eFrameworkunifiedStatusInvldHandle]
///       - When the message queue handle specified in the argument (hSession) is invalid (invalid) [eFrameworkunifiedStatusInvldHandle]
///       - When the name of the application in the handle (hSession) specified by the arguments exceeds 20 bytes [eFrameworkunifiedStatusInvldQName]
///       - When the name of the message queue in the handle (hSession) specified in the arguments is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - When the name of the message queue in the handle (hSession) specified in the argument exceeds 20 bytes [eFrameworkunifiedStatusInvldHandle]
///       - When the message queue type of the handle (hSession) specified in the parameter is not the send type, [eFrameworkunifiedStatusInvldHndlType]
///       - When the message queue is full [eFrameworkunifiedStatusMsgQFull]
///       - File descriptor for sending messages is invalid [eFrameworkunifiedStatusErrNoEBADF]
///       - When an interrupt by a systemcall (signal) occurs while sending messages [eFrameworkunifiedStatusErrNoEINTR]
///       - If the size of the message transmission buffer is invalid [eFrameworkunifiedStatusInvldBufSize]
///       - If there are any errors sending messages [eFrameworkunifiedStatusFail]
/// \~english @par Detail
///        Sets the callback function for the event ID (iCmd) specified in the application Dispatcher.\n
///        Then, it issues a request for registering the application that executed this API to the delivery target of the device-specific events of the service associated with the handle (hSession) specified in the argument.\n
///        The application specifies the type of event to be received from the service by the event ID (uiEventId) of the arguments.\n
///        (The event ID of this API refers to the identification ID on the protocol of the service, and the application identifies the content of the message. The specifiable service-specific events are events set by the service, such as the FrameworkunifiedDefinePublicStateEvents.)
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Pub-Sub
/// \~english @see FrameworkunifiedSubscribeToSessionEventsWithCallbacks, FrameworkunifiedDefinePublicStateEvents, FrameworkunifiedDefinePrivateStateEvents,
///                 FrameworkunifiedUnSubscribeSessionEventWithCallback, FrameworkunifiedUnSubscribeSessionEventsWithCallbacks
///
/// This API is used for subscribing to single event of a service.
/// This API also attaches the session event with callback.
///
/// \param [in] hApp
///         HANDLE - Application handle
/// \param [in] uiEventId
///     UI_32 - Event id
/// \param [in] fpOnCmd
///     CbFuncPtr - pointer to callback function
/// \param [in] hSession
///     HANDLE - Session handle
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see FrameworkunifiedSubscribeToSessionEventsWithCallbacks
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSubscribeToSessionEventWithCallback(HANDLE hApp, UI_32 uiEventId, CbFuncPtr fpOnCmd, HANDLE hSession);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedSubscribeToSessionEventsWithCallbacks
/// \~english @par Brief
///        Registering multiple service-specific events for delivery and configuring callback settings when an event is received
/// \~english @param [in] hApp
///        HANDLE   - Handle for the application
/// \~english @param [in] pEventHandler
///        const FrameworkunifiedProtocolCallbackHandler* - Pointer to array of event handler structure
/// \~english @param [in] uiHandlerCount
///        UI_32   - Number of event handlers to register (0 to 214748362)
/// \~english @param [in] hSession
///        HANDLE   - Handles for sessions (obtained by FrameworkunifiedOpenSession)
/// \~english @par
///        FrameworkunifiedProtocolCallbackHandler Structure
/// \~english @code
///            typedef struct _FrameworkunifiedProtocolCallbackHandler
///            {
///                  UI_32 iCmd;            /* Event ID (Identification ID on the service's protocol) */
///                  CbFuncPtr callBack;    /* Reference to an XML stream       */
///            }FrameworkunifiedProtocolCallbackHandler;
///            @endcode
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid handle type
/// \~english @retval eFrameworkunifiedStatusInvldQName Invalid message queue name
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid file descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR Generating an interrupt by a system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer size
/// \~english @retval eFrameworkunifiedStatusFail Some error occurred
/// \~english @par Prerequisite
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
///       - Services and sessions are established (FrameworkunifiedOpenSession)
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - If the hApp specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusInvldHandle]
///       - If the hSession specified in the parameter is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - If the hSession specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusInvldHandle]
///       - When the event handler structure array (pEventHandler) specified by arguments is NULL, [eFrameworkunifiedStatusInvldParam]
///       - When the message queue handle in the handle (hSession) specified by the arguments is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - If the application name in the handler (hSession) specified by the arguments is not set, [eFrameworkunifiedStatusInvldHandle]
///       - When the message queue handle specified in the argument (hSession) is invalid (invalid) [eFrameworkunifiedStatusInvldHandle]
///       - When the name of the application in the handle (hSession) specified by the arguments exceeds 20 bytes [eFrameworkunifiedStatusInvldQName]
///       - When the name of the message queue in the handle (hSession) specified in the arguments is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - When the name of the message queue in the handle (hSession) specified in the argument exceeds 20 bytes [eFrameworkunifiedStatusInvldHandle]
///       - When the message queue type of the handle (hSession) specified in the parameter is not the send type, [eFrameworkunifiedStatusInvldHndlType]
///       - When the message queue is full [eFrameworkunifiedStatusMsgQFull]
///       - File descriptor for sending messages is invalid [eFrameworkunifiedStatusErrNoEBADF]
///       - When an interrupt by a systemcall (signal) occurs while sending messages [eFrameworkunifiedStatusErrNoEINTR]
///       - If the size of the message transmission buffer is invalid [eFrameworkunifiedStatusInvldBufSize]
///       - If there are any errors sending messages [eFrameworkunifiedStatusFail]
///       - Failed to access shared memory for sending messages [eFrameworkunifiedStatusErrOther]
/// \~english @par Detail
///        Sets the callback functions for the IDs specified by the pEventHandler arguments in the Dispatcher of the application.\n
///        Then, it issues a request for registering the application that executed this API to the delivery target of the device-specific events of the service associated with the handle (hSession) specified in the argument.\n
///        The application specifies the type of event to be received from the service by the event ID.\n
///        (The event ID of this API refers to the identification ID on the protocol of the service, and the application identifies the content of the message. The specifiable service-specific events are events set by the service, such as the FrameworkunifiedDefinePublicStateEvents.)
/// \~english @par
///        Please note the following points when using this API for application.\n
///       - If the callback function pointer in the event handler structure (pEventHandler) specified in the arguments is NULL, the callback function up to NULL is registered.\n
///         Later, FrameworkunifiedSendMsg() is executed inside this API, and the result becomes the return code of this API.\n
///         Do not set NULL in the callback function pointer because operation cannot be guaranteed.
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Pub-Sub
/// \~english @see FrameworkunifiedSubscribeToSessionEventWithCallback, FrameworkunifiedDefinePublicStateEvents, FrameworkunifiedDefinePrivateStateEvents,
///                 FrameworkunifiedUnSubscribeSessionEventWithCallback, FrameworkunifiedUnSubscribeSessionEventsWithCallbacks
///
/// This API is used for subscribing to multiple events of a service.
/// This API also attaches the session event with callbacks.
///
/// \param [in] hApp
///         HANDLE - Application handle
/// \param [in] pEventHandler
///     FrameworkunifiedProtocolCallbackHandler - Array of event handler structures
/// \param [in] uiHandlerCount
///     UI_32 - number of event handlers
/// \param [in] hSession
///     HANDLE - Session handle
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see FrameworkunifiedSubscribeToSessionEventWithCallback
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSubscribeToSessionEventsWithCallbacks(HANDLE hApp, const FrameworkunifiedProtocolCallbackHandler *pEventHandler,
                                                    UI_32 uiHandlerCount, HANDLE hSession);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedUnSubscribeSessionEventWithCallback
/// \~english @par Brief
///        Undeliver service-specific events and delete callbacks when events are received
/// \~english @param [in] hApp
///        HANDLE   - Handle for the application
/// \~english @param [in] uiEventId
///        UI_32   - Event ID (Identification ID on the service protocol) (PROTOCOL_FRAMEWORKUNIFIED_BASE_CMD to PROTOCOL_FRAMEWORKUNIFIED_MAX_CMD)
/// \~english @param [in] hSession
///        HANDLE   - Handles for sessions (obtained by FrameworkunifiedOpenSession)
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid handle type
/// \~english @retval eFrameworkunifiedStatusInvldQName Invalid message queue name
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid file descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR Generating an interrupt by a system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer size
/// \~english @retval eFrameworkunifiedStatusFail Some error occurred
/// \~english @par Prerequisite
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
///       - Services and sessions are established (FrameworkunifiedOpenSession)
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - If the hApp specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusInvldHandle]
///       - If the hSession specified in the parameter is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - If the hSession specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusInvldHandle]
///       - When the message queue handle in the handle (hSession) specified by the arguments is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - If the application name in the handler (hSession) specified by the arguments is not set, [eFrameworkunifiedStatusInvldHandle]
///       - When the message queue handle specified in the argument (hSession) is invalid (invalid) [eFrameworkunifiedStatusInvldHandle]
///       - When the name of the application in the handle (hSession) specified by the arguments exceeds 20 bytes [eFrameworkunifiedStatusInvldQName]
///       - When the name of the message queue in the handle (hSession) specified in the arguments is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - When the name of the message queue in the handle (hSession) specified in the argument exceeds 20 bytes [eFrameworkunifiedStatusInvldHandle]
///       - When the message queue type of the handle (hSession) specified in the parameter is not the send type, [eFrameworkunifiedStatusInvldHndlType]
///       - When the message queue is full [eFrameworkunifiedStatusMsgQFull]
///       - File descriptor for sending messages is invalid [eFrameworkunifiedStatusErrNoEBADF]
///       - When an interrupt by a systemcall (signal) occurs while sending messages [eFrameworkunifiedStatusErrNoEINTR]
///       - If the size of the message transmission buffer is invalid [eFrameworkunifiedStatusInvldBufSize]
///       - If there are any errors sending messages [eFrameworkunifiedStatusFail]
///       - If the service specified by the parameter hSession is not registered in the Dispatcher, [eFrameworkunifiedStatusFail]
/// \~english @par Detail
///        Issues a request to unregister the application that executed this API from the delivery target of device-specific events
///        of services associated with the handle (hSession) specified in the arguments.\n
///        Then, the callback functions for the specified event ID (iCmd) are deleted from the application Dispatcher.\n
///        The application specifies the type of event to be undelivered from the service by the event ID (uiEventId) of the arguments.\n
///        (The event ID of this API refers to the identification ID on the protocol of the service, and the application identifies the content of the message.
///        The specifiable service-specific events are events set by the service, such as the FrameworkunifiedDefinePublicStateEvents.)
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Pub-Sub
/// \~english @see FrameworkunifiedUnSubscribeSessionEventsWithCallbacks, FrameworkunifiedDefinePublicStateEvents, FrameworkunifiedDefinePrivateStateEvents,
///                 FrameworkunifiedSubscribeToSessionEventWithCallback, FrameworkunifiedSubscribeToSessionEventsWithCallbacks
///
/// API to unsubscribe from event of a service. Also detaches callback.
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
/// \see FrameworkunifiedUnSubscribeSessionEventsWithCallbacks
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedUnSubscribeSessionEventWithCallback(HANDLE hApp, UI_32 uiEventId, HANDLE hSession);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedUnSubscribeSessionEventsWithCallbacks
/// \~english @par Brief
///        Undeliver multiple service-specific events and delete callbacks when an event is received
/// \~english @param [in] hApp
///        HANDLE   - Handle for the application
/// \~english @param [in] pEventsArray
///        PUI_32   - Pointer to an array of Event IDs (Identification IDs in the service's protocol)
/// \~english @param [in] uiListSize
///        UI_32   - Number of elements in the event ID (identification ID in the service protocol) array (1 to 1073741813)
/// \~english @param [in] hSession
///        HANDLE   - Handles for sessions (obtained by FrameworkunifiedOpenSession)
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid handle type
/// \~english @retval eFrameworkunifiedStatusInvldBuf Bad buffer
/// \~english @retval eFrameworkunifiedStatusInvldQName Invalid message queue name
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid file descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR Generating an interrupt by a system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer size
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusFail Some error occurred
/// \~english @par Prerequisite
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
///       - Services and sessions are established (FrameworkunifiedOpenSession)
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - If the hApp specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusInvldHandle]
///       - If the hSession specified in the parameter is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - If the hSession specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusInvldHandle]
///       - When the message queue handle in the handle (hSession) specified by the arguments is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - If the application name in the handler (hSession) specified by the arguments is not set, [eFrameworkunifiedStatusInvldHandle]
///       - When the message queue handle specified in the argument (hSession) is invalid (invalid) [eFrameworkunifiedStatusInvldHandle]
///       - When the array (pEventsArray) of event IDs specified by arguments is NULL, [eFrameworkunifiedStatusInvldBuf]
///       - When the name of the application in the handle (hSession) specified by the arguments exceeds 20 bytes [eFrameworkunifiedStatusInvldQName]
///       - When the name of the message queue in the handle (hSession) specified in the arguments is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - When the name of the message queue in the handle (hSession) specified in the argument exceeds 20 bytes [eFrameworkunifiedStatusInvldHandle]
///       - When the message queue type of the handle (hSession) specified in the parameter is not the send type, [eFrameworkunifiedStatusInvldHndlType]
///       - When the message queue is full [eFrameworkunifiedStatusMsgQFull]
///       - File descriptor for sending messages is invalid [eFrameworkunifiedStatusErrNoEBADF]
///       - When an interrupt by a systemcall (signal) occurs while sending messages [eFrameworkunifiedStatusErrNoEINTR]
///       - If the size of the message transmission buffer is invalid [eFrameworkunifiedStatusInvldBufSize]
///       - If there are any errors sending messages [eFrameworkunifiedStatusFail]
///       - When the number of elements (uiListSize) of the event ID array specified by the arguments is 0, [eFrameworkunifiedStatusInvldParam]
///       - If the service specified by the parameter hSession is not registered in the Dispatcher, [eFrameworkunifiedStatusFail]
/// \~english @par Detail
///        Issues a request to unregister the application that executed this API from the delivery target of the plurality of unique events of the service associated with the handle (hSession) specified by the argument.\n
///        Then, the callback functions for the IDs specified in the pEventsArray arguments are deleted from the application Dispatcher.\n
///        The application specifies the type of event to be undelivered from the service by the event ID (pEventsArray) of the arguments.\n
///        (The event ID of this API refers to the identification ID on the protocol of the service, and the application identifies the content of the message. The specifiable service-specific events are events set by the service, such as the FrameworkunifiedDefinePublicStateEvents.)
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Pub-Sub
/// \~english @see FrameworkunifiedUnSubscribeSessionEventWithCallback, FrameworkunifiedDefinePublicStateEvents, FrameworkunifiedDefinePrivateStateEvents,
///                 FrameworkunifiedSubscribeToSessionEventWithCallback, FrameworkunifiedSubscribeToSessionEventsWithCallbacks
///
/// API to unsubscribe from multiple events of a service. Also detaches callbacks.
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
/// \see FrameworkunifiedUnSubscribeSessionEventWithCallback
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedUnSubscribeSessionEventsWithCallbacks(HANDLE hApp, PUI_32 pEventsArray, UI_32 uiListSize,
                                                    HANDLE hSession);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedPublishPublicEvent
/// \~english @par Brief
///        Broadcast application-specific events
/// \~english @param [in] hApp
///        HANDLE   - Handle for the application
/// \~english @param [in] uiEventId
///        UI_32   - The event ID to deliver (the identification ID on the service's protocol)
/// \~english @param [in] pData
///        PCVOID   - Pointer to trasmission data
/// \~english @param [in] uiLength
///        UI_32   - Transmission data length
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusErrOther Other errors (Failed to open/allocate shared memory, etc.)
/// \~english @par Prerequisite
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
///       - The events to be delivered must be specified in the FrameworkunifiedDefinePublicStateEvents.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - If the hApp specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusInvldHandle]
///       - When the application name of the delivery destination in the handle (hApp) specified by the arguments is NULL, [eFrameworkunifiedStatusInvldParam]
///       - Sessioninformation is not found with the application being delivered to within the handles (hApp) specified in the arguments. [eFrameworkunifiedStatusErrOther]
///       - If the destination handles are inappropriate (disabled) [eFrameworkunifiedStatusErrOther]
///       - If the message queue handle in the destination handle is NULL [eFrameworkunifiedStatusErrOther]
///       - If the application name in the destination handles is not set, [eFrameworkunifiedStatusErrOther]
///       - If the message queue handle in the destination handle is inappropriate (disabled) [eFrameworkunifiedStatusErrOther]
///       - When the send data length (uiLength) specified in the arguments is not 0 and the pointer to the send data pointer (pData) is NULL [eFrameworkunifiedStatusErrOther]
///       - When the name of the application in the destination handle exceeds 20 bytes [eFrameworkunifiedStatusErrOther]
///       - When the name of the message queue in the destination handle is NULL, [eFrameworkunifiedStatusErrOther]
///       - When the name of the message queue in the destination handle exceeds 20 bytes [eFrameworkunifiedStatusErrOther]
///       - If the message queue type of the destination handler is not the send type [eFrameworkunifiedStatusErrOther]
///       - When the message queue is full [eFrameworkunifiedStatusErrOther]
///       - File descriptor for sending messages is invalid [eFrameworkunifiedStatusErrOther]
///       - When an interrupt by a systemcall (signal) occurs while sending messages [eFrameworkunifiedStatusErrOther]
///       - If the size of the message transmission buffer is invalid [eFrameworkunifiedStatusErrOther]
///       - If there are any errors sending messages [eFrameworkunifiedStatusErrOther]
///       - Failed to access shared memory for sending messages [eFrameworkunifiedStatusErrOther]
/// \~english @par Detail
///        Broadcast application-specific events to subscribed applications.\n
///        (The event ID of this API is the identification ID on the protocol of the service, which the application identifies the content of the message.)\n
///        Events are delivered to all applications registered by FrameworkunifiedSubscribeToSessionEventsWithCallbacks, etc.\n
///        The data (pData) to be distributed is the same for all distribution destinations.
/// \~english @par
///        FrameworkunifiedPublishPrivateEvent should be used to deliver individual data by registered application sessions.
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Pub-Sub
/// \~english @see FrameworkunifiedDefinePublicStateEvents, FrameworkunifiedDefinePrivateStateEvents, FrameworkunifiedPublishPrivateEvent,
///                 FrameworkunifiedSubscribeToSessionEventWithCallback, FrameworkunifiedSubscribeToSessionEventsWithCallbacks,
///                 FrameworkunifiedUnSubscribeSessionEventWithCallback, FrameworkunifiedUnSubscribeSessionEventsWithCallbacks
///
/// This API is used to publish the event to all the client over all the session it is subscribed to.
///
/// \param [in] hApp
///         HANDLE - Handle to the application framework
/// \param [in] uiEventId
///         UI_32 - Event id of Message
/// \param [in] pData
///         PCVOID - Data buffer
/// \param [in] uiLength
///         UI_32 - Size of data buffer
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see FrameworkunifiedPublishPrivateEvent
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedPublishPublicEvent(HANDLE hApp, UI_32 uiEventId, PCVOID pData, UI_32 uiLength);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedPublishPrivateEvent
/// \~english @par Brief
///        Delivers application-specific events to a specified session
/// \~english @param [in] hApp
///        HANDLE   - Handle for the application
/// \~english @param [in] uiEventId
///        UI_32   - The event ID to deliver (the identification ID on the service's protocol)
/// \~english @param [in] pData
///        PCVOID   - Pointer to trasmission data
/// \~english @param [in] uiLength
///        UI_32   - Transmission data length
/// \~english @param [in] hSession
///        HANDLE   - Handles for sessions
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusServNotFound Perform the event processing
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldBuf Bad buffer
/// \~english @retval eFrameworkunifiedStatusInvldQName Invalid message queue name
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid handle type
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid file descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR Generating an interrupt by a system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer size
/// \~english @retval eFrameworkunifiedStatusFail Some error occurred
/// \~english @retval eFrameworkunifiedStatusErrOther Other errors (Failed to open/allocate shared memory, etc.)
/// \~english @par Prerequisite
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
///       - The events to be delivered must be specified in the FrameworkunifiedDefinePrivateStateEvents.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the application handle (hApp) specified in the parameter is NULL, [eFrameworkunifiedStatusInvldParam]
///       - If the application handle (hApp) specified in the arguments is not appropriate (disabled) [eFrameworkunifiedStatusInvldParam]
///       - If the session handle (hSession) specified in the parameter is NULL, [eFrameworkunifiedStatusInvldParam]
///       - If the sessionhandle (hSession) specified in the arguments is not appropriate (is not valid) [eFrameworkunifiedStatusInvldParam]
///       - The event ID (uiEventId) specified in the parameter is not registered as an event to be delivered to the Dispatcher. [eFrameworkunifiedStatusServNotFound]
///       - When the session handle (hSession) specified in the arguments is not registered as the delivery destination in the Dispatcher [eFrameworkunifiedStatusServNotFound]
///       - When the message queue handle in the session handle (hSession) specified by the arguments is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - When the application name in the session handle (hSession) specified by the arguments is not set [eFrameworkunifiedStatusInvldHandle]
///       - The message queue handle in the session handle (hSession) specified by the arguments is invalid. [eFrameworkunifiedStatusInvldHandle]
///       - When the send data length (uiLength) specified in the arguments is not 0 and the pointer to the send data pointer (pData) is NULL [eFrameworkunifiedStatusInvldBuf]
///       - When the name of the application in the session handle (hSession) specified by the arguments exceeds 20 bytes [eFrameworkunifiedStatusInvldQName]
///       - When the name of the message queue in the session handle (hSession) specified in the arguments is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - When the name of the message queue in the session handle (hSession) specified in the arguments exceeds 20 bytes [eFrameworkunifiedStatusInvldHandle]
///       - When the message queue type of the session handle (hSession) specified in the parameter is not the send type [eFrameworkunifiedStatusInvldHndlType]
///       - When the message queue is full [eFrameworkunifiedStatusMsgQFull]
///       - File descriptor for sending messages is invalid [eFrameworkunifiedStatusErrNoEBADF]
///       - When an interrupt by a systemcall (signal) occurs while sending messages [eFrameworkunifiedStatusErrNoEINTR]
///       - If the size of the message transmission buffer is invalid [eFrameworkunifiedStatusInvldBufSize]
///       - If there are any errors sending messages [eFrameworkunifiedStatusFail]
///       - Failed to access shared memory for sending messages [eFrameworkunifiedStatusErrOther]
/// \~english @par Detail
///        Delivers application-specific events to specified sessions in registered applications.\n
///        (The event ID of this API is the identification ID on the protocol of the service, which the application identifies the content of the message.)\n
///        Events are delivered to the sessions specified in the hSession of arguments among the applications registered in the FrameworkunifiedSubscribeToSessionEventsWithCallbacks, etc.
/// \~english @par
///        Please note the following points when using this API for application.
///        - When broadcasting to all registered applications, use FrameworkunifiedPublishPublicEvent.
///        - This API shall be executed from within a callback function, and a session-use handle acquired using FrameworkunifiedGetCurrentSessionHandle shall be passed to the parameter hSession.
///        - Operation cannot be guaranteed if the size of the area indicated by the pointer (pData) address to the send data does not match
///          the size specified by the send data length (iLength) address.
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Pub-Sub
/// \~english @see FrameworkunifiedDefinePrivateStateEvents, FrameworkunifiedDefinePublicStateEvents, FrameworkunifiedPublishPublicEvent,
///                 FrameworkunifiedSubscribeToSessionEventWithCallback, FrameworkunifiedSubscribeToSessionEventsWithCallbacks,
///                 FrameworkunifiedUnSubscribeSessionEventWithCallback, FrameworkunifiedUnSubscribeSessionEventsWithCallbacks
///
/// This API is used to publish the event over session to the client subscribed to it using session handle.
///
/// \param [in] hApp
///         HANDLE - Handle to the application framework
/// \param [in] uiEventId
///         UI_32 - Event id of Message
/// \param [in] pData
///         PCVOID - Data buffer
/// \param [in] uiLength
///         UI_32 - Size of data buffer
/// \param [in] hSession
///         HANDLE - Session Handle
///
/// \return EFrameworkunifiedStatus
///         eFrameworkunifiedStatusOK - If publish event success
///     eFrameworkunifiedStatusInvldParam - Invalid parameter passed
///     eFrameworkunifiedStatusServNotFound - If event uiEventId not subscribed on hSession by a client
///     Any other values returned by FrameworkunifiedSendMsg
///
/// \see FrameworkunifiedPublishPublicEvent
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedPublishPrivateEvent(HANDLE hApp, UI_32 uiEventId, PCVOID pData, UI_32 uiLength, HANDLE hSession);

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedGetServiceAvailabilityTable
/// returns the list that stores the available services
///
/// \param [in] hApp
///         HANDLE - Handle to the application session

/// \return ServiceAvailabilityList
///         ServiceAvailabilityTable - ServiceAvailabilityList
///
/// \see FrameworkunifiedRegisterEvent
///
////////////////////////////////////////////////////////////////////////////////////////////
std::map<std::string, EFrameworkunifiedServiceAvailableStatus> FrameworkunifiedGetServiceAvailabilityTable(HANDLE hApp);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedIsStateMachineApp
/// \~english @par Brief
///        Returns the presence or absence of a State Machine Application
/// \~english @param [in] hApp
///        HANDLE   - Handle for the application
/// \~english @retval BOOL State Machine Application existence (TRUE: existent/FALSE: nonexistent)
/// \~english @retval FALSE Judgment failure
/// \~english @par Prerequisite
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [FALSE]
///       - If the hApp specified in the arguments is not appropriate (not valid) [FALSE]
/// \~english @par Detail
///       Determines whether there is a State Machine Application from the application handle specified by the arguments, and returns the handle.
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Not applicable
/// \~english @see FrameworkunifiedSetStateMachine, FrameworkunifiedHSMDispatcherWithArguments
///
/// FrameworkunifiedIsStateMachineApp
///   Returns TRUE if it's a state machine application else FALSE.
///
/// \param [in] hApp
///         HANDLE - Handle to the application session
///
/// \return TRUE - if a state machine application.
///     FALSE - if not a state machine application
///
/// \see
///
////////////////////////////////////////////////////////////////////////////////////////////
BOOL FrameworkunifiedIsStateMachineApp(HANDLE hApp);

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedGetXMLConfigHandle
///   Returns the handle to config file handle
///
/// \param [in] hApp
///         HANDLE - Handle to the application session
///
/// \return handle
///         HANDLE - Config file handle
///
/// \see
///
////////////////////////////////////////////////////////////////////////////////////////////
HANDLE FrameworkunifiedGetXMLConfigHandle(HANDLE hApp);

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedGetNumberOfSession
/// API to get the number of sessions created in the system.
///
/// \param [in] hApp
///         HANDLE - Application handle
/// \param [in] strServiceName
///         PCSTR - Service name.
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see
///
////////////////////////////////////////////////////////////////////////////////////////////
UI_32 FrameworkunifiedGetNumberOfSession(HANDLE hApp, PCSTR strServiceName);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedSetSessionHandle
/// \~english @par Brief
///        Registering Session Handles with an Application Dispatcher
/// \~english @param [in] hApp
///        HANDLE   - Handle for the application
/// \~english @param [in] strServiceName
///        PCSTR   - Service name established for the session
/// \~english @param [in] hSession
///        HANDLE   - Handles for sessions (Acquired by FrameworkunifiedGetOpenSessionHandle, etc.)
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusFail Some error occurred
/// \~english @par Prerequisite
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [eFrameworkunifiedStatusFail]
///       - If the hApp specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusFail]
///       - If the hSession specified in the parameter is NULL, [eFrameworkunifiedStatusFail]
///       - If the hSession specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusFail]
///       - If the service-name (strServiceName) specified in the parameter is NULL, [eFrameworkunifiedStatusFail]
///       - If the length of the service-name (strServiceName) specified in the arguments is 0, [eFrameworkunifiedStatusFail]
/// \~english @par Detail
///        After the application establishes a session with the service using a FrameworkunifiedOpenSession or the like,
///        the session handle is registered in the application Dispatcher in association with the service name and session ID.
///        Session handles shall be acquired by using a FrameworkunifiedGetOpenSessionHandle or the like when a response is received from the service.
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Sync
/// \~english @see FrameworkunifiedGetSessionHandle, FrameworkunifiedRemoveSessionHandle,
///                 FrameworkunifiedOpenSession, FrameworkunifiedOpenSessionWithData, FrameworkunifiedOpenSessionSync, FrameworkunifiedOpenSessionWithDataSync,
///                 FrameworkunifiedGetOpenSessionHandle, FrameworkunifiedGetOpenSessionSyncHandle,
///
///   Sets the session handle in the Application Framework. It stores the mapping between
///   source, session id and session handle in the application framework.
///
///
/// \param [in] hApp
///         HANDLE - Application framework handle
/// \param [in] strServiceName
///         PCSTR -Name of the associated service name
/// \param [in] hSession
///         HANDLE - Session handle
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see FrameworkunifiedCreateSession, FrameworkunifiedGetSessionHandle, FrameworkunifiedRemoveSessionHandle.
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSetSessionHandle(HANDLE hApp, PCSTR strServiceName, HANDLE hSession);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedGetSessionHandle
/// \~english @par Brief
///        Get the session handle for the given service name and session ID
/// \~english @param [in] hApp
///        HANDLE   - Handle for the application
/// \~english @param [in] strServiceName
///        PCSTR   - service name
/// \~english @param [in] uiSessionId
///        UI_32   - session ID
/// \~english @retval HANDLE Handles for sessions
/// \~english @retval NULL NULL pointer(Acquisition failure)
/// \~english @par Prerequisite
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [NULL]
///       - If the hApp specified in the arguments is not appropriate (not valid) [NULL]
///       - If the service-name (strServiceName) specified in the parameter is NULL, [NULL]
///       - If the session with the service name (strServiceName) specified in the parameter is not registered in the Dispatcher, [NULL]
///       - If the session with the session ID name (uiSessionId) specified in the parameter is not registered in the Dispatcher, [NULL]
/// \~english @par Detail
///        Gets and returns the session handle associated with the specified service and session id from the application Dispatcher.
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Sync
/// \~english @see FrameworkunifiedSetSessionHandle, FrameworkunifiedRemoveSessionHandle
///
///   Returns the requested session handle associated with service name and session id.
///
///
/// \param [in] hApp
///         HANDLE - Application framework handle
/// \param [in] strServiceName
///         PCSTR -Name of the associated service name
/// \param [in] uiSessionId
///         HANDLE - Session Id
///
/// \return handle
///         HANDLE - Session handle
///
/// \see FrameworkunifiedSetSessionHandle
////////////////////////////////////////////////////////////////////////////////////////////
HANDLE FrameworkunifiedGetSessionHandle(HANDLE hApp, PCSTR strServiceName, UI_32 uiSessionId);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedGetCurrentSessionHandle
/// \~english @par Brief
///        Get the session handle for the most recent incoming message
/// \~english @param [in] hApp
///        HANDLE   - Handle for the application
/// \~english @retval HANDLE Handles for sessions
/// \~english @retval NULL NULL pointer (Acquisition failure)
/// \~english @par Prerequisite
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [NULL]
///       - If the hApp specified in the arguments is not appropriate (not valid) [NULL]
///       - When the source service name obtained from received messages is NULL [NULL]
///       - If the Dispatcher does not have a session associated with the source service-name retrieved from the received message [NULL]
///       - When the session ID acquired from the received message is not registered in the Dispatcher, [NULL]
/// \~english @par Detail
///        Gets and returns the session-handle associated with the most recently received message from the application's Dispatcher.
/// \~english @par
///        The application shall execute this API using an application handle passed as an argument to the callback function.\n
///        (The callback functions are executed after the information required to execute the APIs is stored in the application handles (hApp) by receiving messages in the frameworks.)
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Sync
/// \~english @see FrameworkunifiedGetSessionHandle, FrameworkunifiedSetSessionHandle
///
///   Returns the session handle for the current session. Useful, if used in the callback
///   function of protocol. Current session means on which the protocol message received.
///
///
/// \param [in] hApp
///         HANDLE - Application framework handle
/// \return handle
///         HANDLE - Session handle
///
/// \see FrameworkunifiedSetSessionHandle, FrameworkunifiedGetSessionHandle.
////////////////////////////////////////////////////////////////////////////////////////////
HANDLE FrameworkunifiedGetCurrentSessionHandle(HANDLE hApp);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedRemoveSessionHandle
/// \~english @par Brief
///        Deletes the session handle for the given service name and session ID
/// \~english @param [in] hApp
///        HANDLE   - Handle for the application
/// \~english @param [in] strServiceName
///        PCSTR   - Service name
/// \~english @param [in] uiSessionId
///        UI_32   - session ID
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusFail Some error occurred
/// \~english @par Prerequisite
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [eFrameworkunifiedStatusFail]
///       - If the hApp specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusFail]
///       - If the service-name (strServiceName) specified in the parameter is NULL, [eFrameworkunifiedStatusFail]
///       - If the length of the service-name (strServiceName) specified in the arguments is 0, [eFrameworkunifiedStatusFail]
/// \~english @par Detail
///        This service call deletes the session handle associated with the specified service name and session ID among the session handles registered in the application's Dispatcher by FrameworkunifiedSetSessionHandle.
/// \~english @par
///        Please note the following points when using this API for application.\n
///       - When a session ID that has been deleted (or unregistered) is specified, this API returns an eFrameworkunifiedStatusOK.
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Sync
/// \~english @see FrameworkunifiedSetSessionHandle
///
///   Removes the session handle from the Application Framework associated with service name
///   and session id. It actually removes the mapping between source, session id and session
///   handle in the application framework which was set using FrameworkunifiedSetSessionHandle.
///
///
/// \param [in] hApp
///         HANDLE - Application framework handle
/// \param [in] strServiceName
///         PCSTR -Name of the associated service name
/// \param [in] uiSessionId
///         HANDLE - Session Id
///
/// \return handle
///         HANDLE - Session handle
///
/// \see FrameworkunifiedSetSessionHandle
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedRemoveSessionHandle(HANDLE hApp, PCSTR strServiceName, UI_32 uiSessionId);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedSetMandatoryServiceInfo
/// \~english @par Brief
///        Registers the specified Notification and event ID in the Notification list of the Dispatcher.
/// \~english @param [in] hApp
///        HANDLE   - Handle for the application
/// \~english @param [in] pNotification
///        PCSTR   - Notification name
/// \~english @param [in] uiEventId
///        UI_32   - Event ID (Identification ID on the service's protocol)
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusFail Some error occurred
/// \~english @par Prerequisite
///       - The process of NS_NPPService is started.
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [eFrameworkunifiedStatusInvldParam]
///       - If the hApp specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusInvldParam]
///       - When the Notification name (pNotification) specified in the parameter is NULL, [eFrameworkunifiedStatusInvldParam]
///       - If the length of the Notification name (pNotification) specified in the arguments is 64 bytes or greater, [eFrameworkunifiedStatusFail]
/// \~english @par Detail
///        Registers the specified Notification and event ID in the Notification list of the Dispatcher.\n
///        (The event ID of this API is the identification ID on the protocol of the service, which the application identifies the content of the message.)\n
///        By registering a Notification with this API, a Notification registration/unregistration request
///        to the notification service (NS_NPPService) is automatically issued by the Dispatcher when the Dispatcher starts/ends.
/// \~english @par
///        Please note the following points when using this API for application.\n
///       - If the Notification name is specified as an empty character, eFrameworkunifiedStatusOK is returned. Cannot guarantee operation.
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Broadcast
/// \~english @see 
///
///   Sets the given service as Mandatory service in application handle
///
///
/// \param [in] hApp
///         HANDLE - Application framework handle
/// \param [in] pNotification
///         PCSTR -Name of the Notification
/// \param [in] uiEventId
///         UI_32 - event id
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSetMandatoryServiceInfo(HANDLE hApp, PCSTR pNotification, UI_32 uiEventId);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedGetSessionName
/// \~english @par Brief
///        Gets the name of the message queue from the session handle
/// \~english @param [in] hSession
///        HANDLE   - Handles for sessions
/// \~english @retval PCSTR Pointer to the message queue name
/// \~english @retval NULL NULL pointer (Acquisition failure)
/// \~english @par Prerequisite
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hSession specified in the parameter is NULL, [NULL]
///       - If the hSession specified in the arguments is not appropriate (not valid) [NULL]
/// \~english @par Detail
///        Gets the name of the message queue from the session handle (hSession) passed in the parameter.
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Sync
/// \~english @see 
///
///   Returns the session name corresponding to the given session handle.
///
///
/// \param [in] hSession
///         HANDLE - Session handle
///
/// \return session_name
///         PCSTR - Session name
///
/// \see
////////////////////////////////////////////////////////////////////////////////////////////
PCSTR FrameworkunifiedGetSessionName(HANDLE hSession);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedIsServiceAvailable
/// \~english @par Brief
///        Determine the availability of services
/// \~english @param [in] hApp
///        HANDLE   - Handle for Application
/// \~english @retval BOOL Use (TRUE: Available, FALSE: Not Available)
/// \~english @retval FALSE Acquisition failure
/// \~english @par Prerequisite
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [FALSE]
///       - If the hApp specified in the arguments is not appropriate (not valid) [FALSE]
///       - If the received message is not a service-availability notice... [FALSE]
///       - Incoming messages are invalid [FALSE]
/// \~english @par Detail
///        Based on the received Notification, the service is judged to be usable or unusable, and true/false is returned.
/// \~english @par
///        The application shall execute this API using the handle of the application passed as the argument of the service availability notification callback function.\n
///        (The callback functions are executed after the information required to execute the APIs is stored in the application handles (hApp) by receiving messages in the frameworks.)
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Broadcast
/// \~english @see FrameworkunifiedGetServiceNameOnServiceAvailabilityNotification,
///                 FrameworkunifiedSubscribeNotificationWithCallback, FrameworkunifiedSubscribeNotificationsWithCallback
///
///   This API should only be called in the callback/reaction associated with the Service
///   availability notification. It returns TRUE if service is available and FALSE if service
///   is not available.
///
/// \param [in] hApp
///         HANDLE - Application framework handle
///
/// \return service_availability
///         BOOL - returns TRUE if service is available otherwise returns FALSE
///
/// \see
////////////////////////////////////////////////////////////////////////////////////////////
BOOL FrameworkunifiedIsServiceAvailable(HANDLE hApp);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedPublishServiceAvailability
/// \~english @par Brief
///        Publish application availability status
/// \~english @param [in] hApp
///        HANDLE   - Handle for the application
/// \~english @param [in] bIsAvailable
///        BOOL   - Availability status of the application (TRUE: Available, FALSE: Not Available)
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldQName Invalid message queue name
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid handle type
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid file descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR Generating an interrupt by a system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer size
/// \~english @retval eFrameworkunifiedStatusFail Some error occurred
/// \~english @par Prerequisite
///       - The process of NS_NPPService is started.
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
///       - Notification must be registered by FrameworkunifiedRegisterServiceAvailabilityNotification.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [eFrameworkunifiedStatusInvldParam]
///       - If the hApp specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusInvldParam]
///       - If the service-availability Notification is not set for the handle (hApp) specified in the parameter, [eFrameworkunifiedStatusFail]
///       - When the message queue handle for NPP service in the handle (hApp) specified in the parameter is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - If the application name in the handler (hApp) specified by the arguments is not set, [eFrameworkunifiedStatusInvldHandle]
///       - When the message queue handle for NPP services in the handle (hApp) specified in the arguments is invalid (invalid) [eFrameworkunifiedStatusInvldHandle]
///       - When the name of the application in the handle (hApp) specified by the arguments exceeds 20 bytes [eFrameworkunifiedStatusInvldQName]
///       - When the name of the message queue for the NPP service in the handle (hApp) specified in the parameter is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - When the name of the message queue for NPP services in the handle (hApp) specified in the arguments exceeds 20 bytes [eFrameworkunifiedStatusInvldHandle]
///       - If the message queue type is not the send type [eFrameworkunifiedStatusInvldHndlType]
///       - When the message queue is full [eFrameworkunifiedStatusMsgQFull]
///       - File descriptor for sending messages is invalid [eFrameworkunifiedStatusErrNoEBADF]
///       - When an interrupt by a systemcall (signal) occurs while sending messages [eFrameworkunifiedStatusErrNoEINTR]
///       - If the size of the message transmission buffer is invalid [eFrameworkunifiedStatusInvldBufSize]
///       - If there are any errors sending messages [eFrameworkunifiedStatusErrFail]
/// \~english @par Detail
///        Notifies (issues a Notification) the availability status of an application to another application.\n
///        Applications must use this API to notify other applications when the availability of services provided by the application changes.\n
///        (However, the state machine managed application generated by FrameworkunifiedHSMDispatcherWithArguments, etc. is excluded because the state machine automatically issues a Notification.)
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Broadcast
/// \~english @see FrameworkunifiedRegisterServiceAvailabilityNotification, FrameworkunifiedGetSelfAvailability
///
///   This API publishes service availability status of the service. This API needs to be
///   used when service is up and ready to process clients requests.
///     FrameworkunifiedRegisterServiceAvailabilityNotification must be called before calling this API.
///   In Frameworkunified HSM Framework based application service availability is published by the Framework
///   So this API can be used only in Non-FRAMEWORKUNIFIED HSM based application.
///
/// \param [in] hApp
///         HANDLE - Application framework handle
/// \param [in] bIsAvailable
///     BOOL - TRUE if Service Available and FALSE if service is unavailable
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see
///   FrameworkunifiedRegisterServiceAvailabilityNotification
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedPublishServiceAvailability(HANDLE hApp, BOOL bIsAvailable);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedGetSelfAvailability
/// \~english @par Brief
///        Gets the availability status of the application itself
/// \~english @param [in] hApp
///        HANDLE   - Handle for the application
/// \~english @retval BOOL Use (TRUE: Available, FALSE: Not Available)
/// \~english @retval FALSE Acquisition failure
/// \~english @par Prerequisite
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [FALSE]
///       - If the hApp specified in the arguments is not appropriate (not valid) [FALSE]
/// \~english @par Detail
///        Gets the availability status of the applications managed in the Dispatcher.\n
///        Availability states are the values specified by the application when the FrameworkunifiedPublishServiceAvailability is executed.
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Not applicable
/// \~english @see FrameworkunifiedPublishServiceAvailability
///
///   This API get service availability status of the service. This API needs to be
///   used when service is up and ready to process clients requests.
///     FrameworkunifiedRegisterServiceAvailabilityNotification must be called before calling this API.
///   In Frameworkunified HSM Framework based application service availability is published by the Framework
///   So this API can be used only in Non-FRAMEWORKUNIFIED HSM based application.
///
/// \param [in] hApp
///         HANDLE - Application framework handle
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see
///   FrameworkunifiedRegisterServiceAvailabilityNotification, FrameworkunifiedPublishServiceAvailability
////////////////////////////////////////////////////////////////////////////////////////////
BOOL FrameworkunifiedGetSelfAvailability(HANDLE hApp);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedRegisterServiceAvailabilityNotification
/// \~english @par Brief
///        Registering Application Availability Notification with NPP-Service
/// \~english @param [in] hApp
///        HANDLE   - Handle for the application
/// \~english @param [in] pNotification
///        PCSTR   - Pointers to Notification names
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldQName Invalid message queue name
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid handle type
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid file descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR Generating an interrupt by a system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer size
/// \~english @retval eFrameworkunifiedStatusFail Some error occurred
/// \~english @par Prerequisite
///       - The process of NS_NPPService is started.
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [eFrameworkunifiedStatusInvldParam]
///       - If the hApp specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusInvldParam]
///       - When the Notification name (pNotification) specified in the arguments is NULL, [eFrameworkunifiedStatusInvldParam]
///       - If the length of the Notification name (pNotification) specified in the arguments is 64 bytes or greater, [eFrameworkunifiedStatusFail]
///       - When the length of the Notification name (pNotification) specified in the arguments is 0, [eFrameworkunifiedStatusInvldParam]
///       - Get buffer for sending messages (malloc) failed [eFrameworkunifiedStatusFail]
///       - When the message queue handle for NPP service in the handle (hApp) specified in the parameter is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - When the message queue handle for NPP services in the handle (hApp) specified in the arguments is invalid (invalid) [eFrameworkunifiedStatusInvldHandle]
///       - When the name of the application in the handle (hApp) specified by the arguments exceeds 20 bytes [eFrameworkunifiedStatusInvldQName]
///       - When the name of the message queue for the NPP service in the handle (hApp) specified in the parameter is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - When the name of the message queue for NPP services in the handle (hApp) specified in the arguments exceeds 20 bytes [eFrameworkunifiedStatusInvldHandle]
///       - If the message queue type is not the send type [eFrameworkunifiedStatusInvldHndlType]
///       - When the message queue is full [eFrameworkunifiedStatusMsgQFull]
///       - File descriptor for sending messages is invalid [eFrameworkunifiedStatusErrNoEBADF]
///       - When an interrupt by a systemcall (signal) occurs while sending messages [eFrameworkunifiedStatusErrNoEINTR]
///       - If the size of the message transmission buffer is invalid [eFrameworkunifiedStatusInvldBufSize]
///       - If there are any errors sending messages [eFrameworkunifiedStatusErrFail]
/// \~english @par Detail
///        Requests that NPPservice register a Notification to notify the application availability status.\n
///        A Notification registered by this API can publish the availability status to other applications by executing FrameworkunifiedPublishServiceAvailability from the application.
/// \~english @par
///        Please note the following points when using this API for application.\n
///       - Operation is not guaranteed when the same application executes this API more than once with different Notification names.
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Broadcast
/// \~english @see FrameworkunifiedUnRegisterServiceAvailabilityNotification, FrameworkunifiedPublishServiceAvailability
///
///   This API registers the given service availability notification. Need to be used by
///   service.
///
/// \param [in] hApp
///         HANDLE - Application framework handle
/// \param [in] pNotification
///         PCSTR -Name of the Notification
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see
///   FrameworkunifiedPublishServiceAvailability
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedRegisterServiceAvailabilityNotification(HANDLE hApp, PCSTR pNotification);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedUnRegisterServiceAvailabilityNotification
/// \~english @par Brief
///        Revoke application availability Notification from NPPservice
/// \~english @param [in] hApp
///        HANDLE   - Handle for the application
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldQName Invalid message queue name
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid handle type
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid file descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR Generating an interrupt by a system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer size
/// \~english @retval eFrameworkunifiedStatusFail Some error occurred
/// \~english @par Prerequisite
///       - The process of NS_NPPService is started.
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
///       - Notification must be registered by FrameworkunifiedRegisterServiceAvailabilityNotification.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [eFrameworkunifiedStatusInvldParam]
///       - If the hApp specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusInvldParam]
///       - If the service-availability Notification is not set for the handle (hApp) specified in the parameter, [eFrameworkunifiedStatusInvldParam]
///       - Get buffer for sending messages (malloc) failed [eFrameworkunifiedStatusFail]
///       - When the message queue handle for NPP service in the handle (hApp) specified in the parameter is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - When the message queue handle for NPP services in the handle (hApp) specified in the arguments is invalid (invalid) [eFrameworkunifiedStatusInvldHandle]
///       - When the name of the application in the handle (hApp) specified by the arguments exceeds 20 bytes [eFrameworkunifiedStatusInvldQName]
///       - When the name of the message queue for the NPP service in the handle (hApp) specified in the parameter is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - When the name of the message queue for NPP services in the handle (hApp) specified in the arguments exceeds 20 bytes [eFrameworkunifiedStatusInvldHandle]
///       - If the message queue type is not the send type [eFrameworkunifiedStatusInvldHndlType]
///       - When the message queue is full [eFrameworkunifiedStatusMsgQFull]
///       - File descriptor for sending messages is invalid [eFrameworkunifiedStatusErrNoEBADF]
///       - When an interrupt by a systemcall (signal) occurs while sending messages [eFrameworkunifiedStatusErrNoEINTR]
///       - If the size of the message transmission buffer is invalid [eFrameworkunifiedStatusInvldBufSize]
///       - If there are any errors sending messages [eFrameworkunifiedStatusFail]
/// \~english @par Detail
///        Requests that NPPservice abandon the Notification to notify the availability status of the application that executed this API.
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Broadcast
/// \~english @see FrameworkunifiedRegisterServiceAvailabilityNotification
///
///   This API unregisters the service availability notification
///
/// \param [in] hApp
///         HANDLE - Application framework handle
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see
///   FrameworkunifiedPublishServiceAvailability, FrameworkunifiedRegisterServiceAvailabilityNotification
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedUnRegisterServiceAvailabilityNotification(HANDLE hApp);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedCreateSession
/// \~english @par Brief
///        Create handles for sessions and register them with the application's Dispatcher
/// \~english @param [in] hApp
///        HANDLE   - Handle for the application
/// \~english @param [in] pSessionName
///        PCSTR   - Session name (application name or thread name)
/// \~english @retval HANDLE Handles for sessions
/// \~english @retval NULL NULL pointer (Acquisition failure)
/// \~english @par Prerequisite
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [NULL]
///       - If the hApp specified in the arguments is not appropriate (not valid) [NULL]
///       - The session name (pSessionName) specified in the arguments is invalid.
///         (For NULL, if the length is 0 or 20 bytes or more) [NULL]
///       - When acquisition (malloc) of the message queue management information area fails, [NULL]
///       - When the normalized message queue name from the session name (pSessionName) is inappropriate (exceeds 16 bytes) [NULL]
///       - When acquisition (malloc) of the storage area for the message queue name fails, [NULL]
///       - The open of the message queue (mq_open) failed [NULL]
/// \~english @par Detail
///        Creates handles (send message queues) for sessions with the specified application and registers them in the application's Dispatcher.\n
///        The pSessionName is set to the application name or thread name of the client that establishes the session.\n
///        This API is used to establish a session with a client application when an application on the service providing side receives a session start request from the client application.
///        The application shall execute this API from the corresponding callback function.
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Sync
/// \~english @see FrameworkunifiedDestroySession
///
///   This API generates the session handle and sets it in the Application Framework. This API
///     should be used by the server in the callback function attached to the open session request.
///
/// \param [in] hApp
///         HANDLE - Application framework handle
/// \param [in] pSessionName
///         PCSTR - SessionName can be application name or thread name
///
/// \return session_handle
///         HANDLE - Session Handle
///
////////////////////////////////////////////////////////////////////////////////////////////
HANDLE FrameworkunifiedCreateSession(HANDLE hApp, PCSTR pSessionName);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedDestroySession
/// \~english @par Brief
///        Destroys the session handle and removes the session handle from the application Dispatcher
/// \~english @param [in] hApp
///        HANDLE   - Handle for the application
/// \~english @param [in] hSession
///        HANDLE   - Handle for sessions (handle obtained with FrameworkunifiedCreateSession)
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusFail Some error occurred
/// \~english @par Prerequisite
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [eFrameworkunifiedStatusFail]
///       - If the hApp specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusFail]
///       - If the hSession specified in the parameter is NULL, [eFrameworkunifiedStatusFail]
///       - If the hSession specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusFail]
///       - When the length of the message queue name of the handle (hSession) specified in the arguments is 0, [eFrameworkunifiedStatusFail]
/// \~english @par Detail
///        Destroys the specified session handle and removes the specified session handle from the application Dispatcher.\n
///        This API is used when the service-providing application receives a session termination request from the client application and discards the session with the client application.
///        The application shall execute this API from the corresponding callback function.
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Sync
/// \~english @see FrameworkunifiedCreateSession
///
///   This API destroys the session handle and removes it from the Application Framework.
///     This API should be called by the server in the callback attached to the close session
///   request
///
/// \param [in] hApp
///         HANDLE - Application framework handle
/// \param [in] hSession
///         HANDLE - Session handle
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedDestroySession(HANDLE hApp, HANDLE hSession);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedSendResponse
/// \~english @par Brief
///        Sending a Response to the Source of an Incoming Message
/// \~english @param [in] hApp
///        HANDLE   - Handle for the application
/// \~english @param [in] iCmd
///        UI_32   -  Identification ID on the service's protocol
/// \~english @param [in] length
///        UI_32   -  Data size to be sent (0 to 4294967255)
/// \~english @param [in] data
///        PCVOID   - Pointer to the data to send
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusNullPointer Specifying NULL pointers
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid handle type
/// \~english @retval eFrameworkunifiedStatusInvldBuf Bad buffer
/// \~english @retval eFrameworkunifiedStatusInvldQName Invalid message queue name
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid file descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR Generating an interrupt by a system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer size
/// \~english @retval eFrameworkunifiedStatusFail Some error occurred
/// \~english @retval eFrameworkunifiedStatusErrOther Other errors (Failed to open/allocate shared memory, etc.)
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @par Prerequisite
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
///       - Sessions are registered in the Dispatcher by FrameworkunifiedCreateSession, etc.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [eFrameworkunifiedStatusFail]
///       - If the hApp specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusFail]
///       - When the source service name obtained from received messages is NULL [eFrameworkunifiedStatusFail]
///       - If the Dispatcher does not have a session associated with the source service-name retrieved from the received message [eFrameworkunifiedStatusFail]
///       - When the session ID acquired from the received message is not registered in the Dispatcher, [eFrameworkunifiedStatusFail]
///       - If the sessionhandle for incoming messages is NULL [eFrameworkunifiedStatusNullPointer]
///       - If the handles for sessions corresponding to incoming messages are inappropriate (disabled) [eFrameworkunifiedStatusNullPointer]
///       - When the message queue handle corresponding to the received message in the session handle is NULL [eFrameworkunifiedStatusInvldHandle]
///       - When the application name length in the session handle corresponding to received messages is 0, [eFrameworkunifiedStatusInvldHandle]
///       - Incorrect (invalid) message queue handle in session handle for incoming message [eFrameworkunifiedStatusInvldHandle]
///       - When the data (data) of the send message specified by the parameter is NULL and the data length (length) is not 0, [eFrameworkunifiedStatusInvldBuf]
///       - When the length of the application name in the session handle corresponding to the received message exceeds 20 bytes [eFrameworkunifiedStatusInvldQName]
///       - When the message queue name of the session handle corresponding to the received message is NULL [eFrameworkunifiedStatusInvldHandle]
///       - When the message queue name of the session handle corresponding to the received message exceeds 20 bytes [eFrameworkunifiedStatusInvldHandle]
///       - When the message queue type of the session handle corresponding to the received message is not the transmission type [eFrameworkunifiedStatusInvldHndlType]
///       - When the message queue is full [eFrameworkunifiedStatusMsgQFull]
///       - File descriptor for sending messages is invalid [eFrameworkunifiedStatusErrNoEBADF]
///       - When an interrupt by a systemcall (signal) occurs while sending messages [eFrameworkunifiedStatusErrNoEINTR]
///       - If the size of the message transmission buffer is invalid [eFrameworkunifiedStatusInvldBufSize]
///       - If there are any errors sending messages [eFrameworkunifiedStatusFail]
///       - Failed to access shared memory for sending messages [eFrameworkunifiedStatusErrOther]
///       - When the size (length) of the data to be sent specified in the parameter is out of range, [eFrameworkunifiedStatusInvldParam]
/// \~english @par Detail
///        Gets the session ID and session name from received messages, and the corresponding session handles from Dispatcher.\n
///        Send a response to the sender of the received message using the acquired session handle.
/// \~english @par
///        Please note the following points when using this API for application.\n
///       - The maximum data size (length) described above is a theoretical value. The maximum data size that can be actually used depends on the environment requirements.\n
///         The maximum value is the upper limit of the type (UINT_MAX) minus the following size.\n
///         Message header section\n
///       - Operation cannot be guaranteed when the size of the allocated area indicated by the address of the pointer (data) to the data buffer does not match the size specified by the size of the data buffer (length).
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Method
/// \~english @see FrameworkunifiedCreateSession, FrameworkunifiedSetSessionHandle
///
///   This API retrieves the session handle from the application framework and sends the response
///   to the client on the same session. The session id and message source name available in the
///   hApp at that instance is used to get the session handle. This API will work if server has
///   created session using FrameworkunifiedCreateSession or has used FrameworkunifiedSetSessionHandle after creating the
///   session.
///
/// \param [in] hApp
///         HANDLE - Application framework handle
/// \param [in] iCmd
///         UI_32 - Protocol command
/// \param [in] length
///         UI_32 - size of message data
/// \param [in] data
///         PCVOID - message data
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see FrameworkunifiedCreateSession, FrameworkunifiedSetSessionHandle, FrameworkunifiedSendRequest.
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSendResponse(HANDLE hApp, UI_32 iCmd, UI_32 length, PCVOID data);

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedSendRequest
///   This API retrieves the session handle from the application framework and sends the request
///   to the server with specific service name and session id. This API will work, if application
///     has previously stored associated session handle using FrameworkunifiedSetSessionHandle.
///
/// \param [in] hApp
///         HANDLE - Application handle
/// \param [in] pServerName
///         PCSTR - Name of the server
/// \param [in] uiSessionId
///         UI_32 - session id
/// \param [in] iCmd
///         UI_32 - Protocol command
/// \param [in] length
///         UI_32 - size of message data
/// \param [in] data
///         PCVOID - message data
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see  FrameworkunifiedSetSessionHandle, FrameworkunifiedSendResponse
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSendRequest(HANDLE hApp, PCSTR pServerName, UI_32 uiSessionId, UI_32 iCmd,
                          UI_32 length, PCVOID data);

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPSetPersonality
/// API to send message to Notification Service to set Personality
///
/// \param [in] hApp
///         HANDLE - Application Handle
/// \param [in] pUserName
///         PCSTR - Name of the new Personality
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see FrameworkunifiedNPRegisterPersistentFile, FrameworkunifiedNPLoadPersistentFile
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPSetPersonality(HANDLE hApp, PCSTR pUserName);

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPChangePersonality
/// API to send message to Notification Service to set new Personality
///
/// \param [in] hApp
///         HANDLE - Application Handle
/// \param [in] pUserName
///         PCSTR - Name of the new Personality
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see FrameworkunifiedNPRegisterPersistentFile, FrameworkunifiedNPLoadPersistentFile
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPChangePersonality(HANDLE hApp, PCSTR pUserName);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  FrameworkunifiedSendStopToNSNPP
/// \~english @par Brief
///        API that sends a message to the notification service and requests a shutdown
/// \~english @param [in] hApp
///        HANDLE           - Handle for the application
/// \~english @param [in] eShutdownType
///        EFrameworkunifiedShutdownType - Shutdown type
/// \~english @param [in] uiStopMsgData
///        UI_32            - Flag subject to resetting (Default:0)
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid handle type
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid file descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR Generating an interrupt by a system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer size
/// \~english @retval eFrameworkunifiedStatusFail Some error occurred
/// \~english @par Prerequisite
///       - The process of NS_NPPService is started.
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [eFrameworkunifiedStatusInvldParam]
///       - If the hApp specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusInvldParam]
///       - When the application-name storage area address in the handle (hApp) specified by the arguments is NULL, [eFrameworkunifiedStatusInvldParam]
///       - If the application name in the handle (hApp) specified by the arguments is empty, [eFrameworkunifiedStatusInvldParam]
///       - When the message queue handle in the handle (hApp) specified by the arguments is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - When the message queue type of the handle (hApp) specified in the parameter is not the send type, [eFrameworkunifiedStatusInvldHndlType]
///       - If the message queue handle in the hApp specified by the arguments is inappropriate (not valid) [eFrameworkunifiedStatusInvldHandle]
///       - The message queue name of the handle (hApp) specified in the arguments is not appropriate (the name exceeds NULL, 20 bytes) [eFrameworkunifiedStatusInvldHandle]
///       - Full Message Queue for Sessions for NPP Service [eFrameworkunifiedStatusMsgQFull]
///       - Incorrect file descriptors for sending messages for NPP Service sessions [eFrameworkunifiedStatusErrNoEBADF]
///       - When an interrupt by a system call (signal) occurs while sending NPP Service session-specific messages [eFrameworkunifiedStatusErrNoEINTR]
///       - Incorrectly sized send buffers for sessions destined for NPP Service [eFrameworkunifiedStatusInvldBufSize]
///       - If there are any errors sending messages for sessions intended for NPP Service [eFrameworkunifiedStatusFail]
/// \~english @par Detail
///       Sends a message to the notification service (NS_NPPService) and requests shutdown.\n
///       The reset target flag (uiStopMsgData) is valid only when the shutdown type (eShutdownType) is eFrameworkunifiedDataResetShutdown.\n
///       The reset target flag (uiStopMsgData) indicates the target of data reset at shutdown.\n
///       The target can be OR-set by using EFrameworkunifiedPersistCategory defined values.\n
///       The specified target is reset from the non-volatile memory.
///         Eg: uiStopMsgData = (eFrameworkunifiedUserData | eFrameworkunifiedFactoryCustomerData) = 0x0101\n
///           eFrameworkunifiedUserData bit            : 1 : user data resets\n
///           eFrameworkunifiedFactoryData bit         : 0 : factory data not reset-target\n
///           eFrameworkunifiedFactoryCustomerData bit : 1 : factory customer data resets\n
///           eFrameworkunifiedDealerData bit          : 0 : persist dealer data not reset-target\n
/// \~english @par Classification
///          Public
/// \~english @par Type
///         Sync
/// \~english @see  
///
/// FrameworkunifiedSendStopToNSNPP
/// To be used by System Manager Service only. This API would push NPP Service to persist
/// the data/files. Ideally this should be sent during system shutdown by System Manager Service
///
/// \param [in] hApp
///         HANDLE - Application Handle
///
/// \param [in] eShutdownType
///         EFrameworkunifiedShutdownType - shutdown type
///
/// \param [in] uiStopMsgData
///         UI_32 - Flag representing which data to persist and which data to delete from
///         persistent memory during shutdown.
///         This needs to be specified only when shutdown type is eFrameworkunifiedDataResetShutdown.
///         If not specified with DataReset type, then NPP will do normal shutdown.
///         For other shutdown type this parameter has no effect.
///         uiStopMsgData = 0 means Normal shutdown persisting all data
///         else value from enum EFrameworkunifiedPersistCategory, multiple value from enum can be ORed.
///         Bit value 1 represent resetting of data while value 0 bit represent data persisting
///         Eg: uiStopMsgData = (eFrameworkunifiedUserData|eFrameworkunifiedFactoryCustomerData) = 0x0101 indicates
///           eFrameworkunifiedUserData bit value 1 means delete user data
///           eFrameworkunifiedFactoryData  bit value 0 means persist factory data
///           eFrameworkunifiedFactoryCustomerData bit value 1 means delete factory customer data
///           eFrameworkunifiedDealerData bit value 0 means persist dealer data
///
/// \return status
///         EFrameworkunifiedStatus - success if request sent to NPPService or error
/// \~english @par Type
///         Pub-Sub
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSendStopToNSNPP(HANDLE hApp, EFrameworkunifiedShutdownType eShutdownType, UI_32 uiStopMsgData = 0x0);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  FrameworkunifiedNPGetReadyStatusOfNPP
/// \~english @par Brief
///        APIs that send messages to notification services and obtain Ready status
/// \~english @param [in] hApp
///        HANDLE           - Handle for the application
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid handle type
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid file descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR Generating an interrupt by a system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer size
/// \~english @retval eFrameworkunifiedStatusFail Some error occurred
/// \~english @par Prerequisite
///       - The process of NS_NPPService is started.
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [eFrameworkunifiedStatusInvldParam]
///       - If the hApp specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusInvldParam]
///       - When the application-name storage area address in the handle (hApp) specified by the arguments is NULL, [eFrameworkunifiedStatusInvldParam]
///       - If the application name in the handle (hApp) specified by the arguments is empty, [eFrameworkunifiedStatusInvldParam]
///       - When the message queue handle in the handle (hApp) specified by the arguments is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - When the message queue type of the handle (hApp) specified in the parameter is not the send type, [eFrameworkunifiedStatusInvldHndlType]
///       - If the message queue handle in the hApp specified by the arguments is inappropriate (not valid) [eFrameworkunifiedStatusInvldHandle]
///       - The message queue name of the handle (hApp) specified in the arguments is not appropriate (the name exceeds NULL, 20 bytes) [eFrameworkunifiedStatusInvldHandle]
///       - Full Message Queue for Sessions for NPP Service [eFrameworkunifiedStatusMsgQFull]
///       - Incorrect file descriptors for sending messages for NPP Service sessions [eFrameworkunifiedStatusErrNoEBADF]
///       - When an interrupt by a system call (signal) occurs while sending NPP Service session-specific messages [eFrameworkunifiedStatusErrNoEINTR]
///       - Incorrectly sized send buffers for sessions destined for NPP Service [eFrameworkunifiedStatusInvldBufSize]
///       - If there are any errors sending messages for sessions intended for NPP Service [eFrameworkunifiedStatusFail]
/// \~english @par Detail
///       Sends messages to the notification service (NS_NPPService) and acquires the Ready status.\n
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Method
/// \~english @see  
///
/// FrameworkunifiedNPGetReadyStatusOfNPP
/// To be used by System Manager Service only. SystemManager will use this API to get the
/// ready status of NPPService.
///
/// \param [in] hApp
///         HANDLE - Application Handle
///
/// \return status
///         EFrameworkunifiedStatus - success if request sent to NPPService or error
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPGetReadyStatusOfNPP(HANDLE hApp);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedGetServiceNameOnServiceAvailabilityNotification
/// \~english @par Brief
///        Get the available service names
/// \~english @param [in] hApp
///        HANDLE   - Handle for the application
/// \~english @param [out] pServiceName
///        PSTR   - Invalid buffer size
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusFail Some error occurred
/// \~english @par Prerequisite
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [eFrameworkunifiedStatusInvldParam]
///       - If the hApp specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusInvldParam]
///       - When the service-name storage area (pServiceName) specified in the parameter is NULL, [eFrameworkunifiedStatusInvldParam]
///       - If the received message is not a service-availability notice... [eFrameworkunifiedStatusFail]
///       - Incoming messages are invalid [eFrameworkunifiedStatusFail]
/// \~english @par Detail
///        When an application receives a service availability notification, it gets the name of the available service.
/// \~english @par
///        Please note the following points when using this API for application.
///        - The application shall execute this API using an application handle passed as an argument to the callback function.\n
///         (The callback functions are executed after the information required to execute the APIs is stored in the application handles (hApp) by receiving messages in the frameworks.)
///        - The size of the buffer passed in the argument pServiceName must be at least MAX_NAME_SIZE_APP(20).
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Not applicable
/// \~english @see FrameworkunifiedIsServiceAvailable, FrameworkunifiedSubscribeNotificationWithCallback,
///                 FrameworkunifiedSubscribeNotificationsWithCallback
///
/// To be used when client receives service availability notification to get the available
/// service name.
///
/// \param [in] hApp
///         HANDLE - Application Handle
/// \param [out] pServiceName
///         PSTR - Name of the available service.
///          Size of the buffer should be equal to or greater than MAX_NAME_SIZE_APP
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedGetServiceNameOnServiceAvailabilityNotification(HANDLE hApp, PSTR pServiceName);

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedGetCurrentUser
/// Get the current user for the application
///
/// \param [in] hApp
///         HANDLE - Application handle
///
/// \return userhandle
///      HANDLE - Returns current user handle
///
///
////////////////////////////////////////////////////////////////////////////////////////////
HANDLE FrameworkunifiedGetCurrentUser(HANDLE hApp);

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedSetUser
/// Set the current user for the application in application framework
///
/// \param [in] hApp
///         HANDLE - Application handle
/// \param [in] hUser
///         HANDLE - User handle
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSetUser(HANDLE hApp, HANDLE hUser);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedSetAppData
/// \~english @par Brief
///        Hold the specified arbitrary data
/// \~english @param [in] hApp
///        HANDLE   - Handle for the application
/// \~english @param [in] pKey
///        PCSTR    - Key strings
/// \~english @param [in] pData
///        PVOID    - Service name that can be used
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusFail Retention failure
/// \~english @par Prerequisite
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [eFrameworkunifiedStatusInvldHandle]
///       - If the hApp specified in the arguments is not appropriate (not valid) [eFrameworkunifiedStatusInvldHandle]
///       - If the addresses of the key strings are NULL [eFrameworkunifiedStatusInvldParam]
///       - Failed to retain arbitrary data. [eFrameworkunifiedStatusFail]
/// \~english @par Detail
///       This function stores an arbitrary data address in the heap using the specified key string as a key.
///       Since the object to be held is an address, the caller must specify an accessible global address, etc.
///       The caller should pay attention to the addresses of arbitrary data, since they retain the addresses of arbitrary data even if they are NULL.
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Set Get
/// \~english @see FrameworkunifiedGetAppData, FrameworkunifiedRemoveAppData
///
/// FrameworkunifiedSetAppData
/// This API stores the data pointer against a key in application framework. This can be used
/// as a global data and can be accessed in the FRAMEWORKUNIFIED functions. Please note that application
/// developer should avoid using global data.
///
/// \param [in] hApp
///         HANDLE - Application handle
/// \param [in] pKey
///         PSTR - Data key
/// \param [in] pData
///         PVOID - data
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see FrameworkunifiedGetAppData, FrameworkunifiedRemoveAppData
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSetAppData(HANDLE hApp, PCSTR pKey, PVOID pData);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedGetAppData
/// \~english @par Brief
///        Returns the address of any data associated with the key string
/// \~english @param [in] hApp
///        HANDLE   - Handle for the application
/// \~english @param [in] pKey
///        PCSTR    - Key strings
/// \~english @retval PVOID Address of arbitrary data
/// \~english @retval NULL NULL pointers (unsuccessful retrieval, anomaly parameters)
/// \~english @par Prerequisite
///       - Creation/initialization (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) of the Dispatcher for the application must be performed.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the hApp specified in the parameter is NULL, [NULL]
///       - If the hApp specified in the arguments is not appropriate (not valid) [NULL]
///       - If the addresses of the Key strings are NULL [NULL]
/// \~english @par Detail
///       Returns the corresponding data address using the specified key string as the key.
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Set Get
/// \~english @see FrameworkunifiedSetAppData, FrameworkunifiedRemoveAppData
///
/// FrameworkunifiedGetAppData
/// This API retrieves the data stored against a key in application framework.
///
/// \param [in] hApp
///         HANDLE - Application handle
/// \param [in] pKey
///         PSTR - Data key
/// \return [in] pData
///         PVOID - data
///
/// \see FrameworkunifiedSetAppData, FrameworkunifiedRemoveAppData
////////////////////////////////////////////////////////////////////////////////////////////
PVOID FrameworkunifiedGetAppData(HANDLE hApp, PCSTR pKey);

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedRemoveAppData
/// This API removes the data stored against a key in application framework.
///
/// \param [in] hApp
///         HANDLE - Application handle
/// \param [in] pKey
///         PSTR - Data key
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see FrameworkunifiedSetAppData, FrameworkunifiedGetAppData
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedRemoveAppData(HANDLE hApp, PCSTR pKey);

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedGetSystemInfo
/// Gets some internal notification information
///
/// \param [in] hApp
///         HANDLE - Handle to a Application Framework
/// \param [in, out] pSystemInfo
///         PVOID - Buffer pointer to which system info is copied.
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
/// \see FrameworkunifiedMcOpenSender, FrameworkunifiedSendMsg, FrameworkunifiedGetMsgLength, FrameworkunifiedGetMsgSrc, FrameworkunifiedGetMsgDataOfSize,
///      FrameworkunifiedClearMsgData, FrameworkunifiedGetDataUSID, FrameworkunifiedForwardMessage, FrameworkunifiedGetSystemInfo
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedGetSystemInfo(HANDLE hApp, PVOID pSystemInfo);

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedSetDeferredSyncResponse
/// Set sync response deferred flag
///
/// \param [in] hApp
///         HANDLE - Handle to the dispatcher to receive message
///
/// \return status
///         EFrameworkunifiedStatus - success or error
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSetDeferredSyncResponse(HANDLE hApp);

#endif /* __FRAMEWORKUNIFIED_NATIVESERVICES_FRAMEWORK_FRAMEWORKUNIFIED_FRAMEWORK_H__ */  // NOLINT  (build/header_guard)
/**@}*/  // end of framework
/**@}*/  // end of framework_unified
/**@}*/  // end of native_service
/**@}*/  // end of BaseSystem
//@}
