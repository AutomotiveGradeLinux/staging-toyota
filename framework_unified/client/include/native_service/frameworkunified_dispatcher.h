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
/// \ingroup  tag_NSFramework
/// \brief    Application dispatch interface functions
///////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file frameworkunified_dispatcher.h
 * @brief \~english This file contains the base class of application.
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
 *  @ingroup native_service
 *  @{
 */
#ifndef __NATIVESERVICES_FRAMEWORK_FRAMEWORKUNIFIED_DISPATCHER_H__  // NOLINT  (build/header_guard)
#define __NATIVESERVICES_FRAMEWORK_FRAMEWORKUNIFIED_DISPATCHER_H__

#include <native_service/frameworkunified_framework_types.h>
// #include <native_service/frameworkunified_sm_hsmframework.h>

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedDispatcher
/// \~english @par Brief
///        This API creates, initializes the dispatcher and runs mainloop for event handling.
/// \~english @param [in] cAppName
///        PCSTR - Pointer to application thread name
/// \~english @param [in] CbHandler
///        const FrameworkunifiedDefaultCallbackHandler* - Pointer to default callback functions.
/// \~english @par
///        FrameworkunifiedDefaultCallbackHandler Structure
/// \~english @code
///                 typedef struct _FrameworkunifiedDefaultCallbackHandler
///                 {
///                       CbFuncPtr onInitilization;               /* Function is called when a Dispatcher is created.
///                                   */
///                       CbFuncPtr onDestroy;                     /* Function is called when the Dispatcher is
///                       released.            */
///                       CbFuncPtr onStart;                       /* Function is called when the Dispatcher is started.
///                                   */
///                       CbFuncPtr onStop;                        /* Function is called when the Dispatcher is stopped.
///                                   */
///                       CbFuncPtr onPreStart;                    /* Function is called when the Dispatcher is pre started. */
///                       CbFuncPtr onPreStop;                     /* Function is called when the Dispatcher is pre stoped. */
///                       CbFuncPtr onBackgroundStart;             /* Function is called when the Dispatcher is Background started. */
///                       CbFuncPtr onBackgroundStop;              /* Function is called when the Dispatcher is Background stopped. */
///                       CbFuncPtr onDebugDump;                   /* Function is called when the Dispatcher detects
///                       abnormal state. */
///                       CbFuncPtr createStateMachine;            /* Set dummy function that does nothing.
///                                   */
///                       CbFuncPtr ssFrameworkInterface;          /* Function to connect to SystemManager
///                                   */
///                 } FrameworkunifiedDefaultCallbackHandler;
///            @endcode
/// \~english @par
///        About setting of default callback functions(FrameworkunifiedDefaultCallbackHandler)
///        - Use FRAMEWORKUNIFIED_MAKE_DEFAULT_CALLBACK when initialize of FrameworkunifiedDefaultCallbackHandler structure as argument CbHandler.
///        - Application that run this API need to define functions below.(allow to dummy function that does nothing.)
///            - EFrameworkunifiedStatus FrameworkunifiedOnInitialization(HANDLE hApp)
///            - EFrameworkunifiedStatus FrameworkunifiedOnStart(HANDLE hApp)
///            - EFrameworkunifiedStatus FrameworkunifiedOnStop(HANDLE hApp)
///            - EFrameworkunifiedStatus FrameworkunifiedOnPreStart(HANDLE hApp)
///            - EFrameworkunifiedStatus FrameworkunifiedOnPreStop(HANDLE hApp)
///            - EFrameworkunifiedStatus FrameworkunifiedOnBackgroundStart(HANDLE hApp)
///            - EFrameworkunifiedStatus FrameworkunifiedOnBackgroundStop(HANDLE hApp)
///            - EFrameworkunifiedStatus FrameworkunifiedOnDebugDump(HANDLE hApp)
///            - EFrameworkunifiedStatus FrameworkunifiedOnDestroy(HANDLE hApp)
///            - EFrameworkunifiedStatus FrameworkunifiedCreateStateMachine(HANDLE hApp)
///        - If application is resident service, link library libSS_SystemIfUnified
///          (This library provides the function FrameworkunifiedSSFrameworkInterface to connect to SystemManager.).
///        - If application is nonresident service, define function that does nothing below.
///            - EFrameworkunifiedStatus FrameworkunifiedSSFrameworkInterface(HANDLE hApp)
/// \~english @retval eFrameworkunifiedStatusNullPointer NULL pointer specified
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
/// \~english @retval eFrameworkunifiedStatusDuplicate Duplication error of entry
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusErrOther Other error has occurred(Cannot access shared memory, etc.)
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
/// \~english @par Prerequisite
///        - Prerequisites are nothing.
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - Application thread name specified in the argument (cAppName) is NULL. [eFrameworkunifiedStatusNullPointer]
///        - Pointer to default callback functions specified in the argument (CbHandler) is NULL.
///        [eFrameworkunifiedStatusNullPointer]
///        - One of members in default callback functions specified in the argument (CbHandler) is NULL.
///        [eFrameworkunifiedStatusInvldParam]
///        - Application thread name specified in the argument (cAppName) is not appropriate(The name is more than
///        15byte). [eFrameworkunifiedStatusNullPointer]
///        - Failed to open message queue (mq_open). [eFrameworkunifiedStatusNullPointer]
///        - Failed to get memory (malloc) of message queue info area. [eFrameworkunifiedStatusNullPointer]
///        - Failed to initialize socket for monitoring abnormal state (socket, bind, listen). [eFrameworkunifiedStatusFail]
///        - Session message queue is full to the NPP Service. [eFrameworkunifiedStatusMsgQFull]
///        - It is invalid transmission file descriptor of the session message to the NPP Service.
///        [eFrameworkunifiedStatusErrNoEBADF]
///        - Interruption by the system call (signal) has occurred during the transmission of the session message for
///        the NPP Service. [eFrameworkunifiedStatusErrNoEINTR]
///        - Incorrect size of the transmit buffer of the session message to the NPP Service. [eFrameworkunifiedStatusInvldBufSize]
///        - Any errors occur during the transmission of a session message to the NPP Service. [eFrameworkunifiedStatusFail]
///        - Failed to create of epoll instance (epoll_create1). [eFrameworkunifiedStatusInvldHandle]
///        - Failed to register of a descriptor of the message queue to the epoll instance (epoll_ctl).
///        [eFrameworkunifiedStatusInvldHandle]
///        - Failed to create file descriptor for receive event (eventfd). [eFrameworkunifiedStatusFail]
///        - Failed to register of a descriptor for receive event to the epoll instance (epoll_ctl). [eFrameworkunifiedStatusFail]
///        - Failed to register of a socket for detect abnormal state to the epoll instance (epoll_ctl).
///        [eFrameworkunifiedStatusFail]
///        - The result of FrameworkunifiedOnInitialization is not eFrameworkunifiedStatusOK. [eFrameworkunifiedStatusNullPointer]
/// \~english @par Detail
///        This API creates and initializes the dispatcher.\n
///        It starts to mainloop that receives request or notification, and runs registered callback to dispatcher.
///        When success starting mainloop, never return.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        FrameworkunifiedDispatcherWithArguments
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedDispatcher(PCSTR cAppName, const FrameworkunifiedDefaultCallbackHandler *CbHandler);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedDispatcherWithArguments
/// \~english @par Brief
///        This API creates, initializes the dispatcher and runs loop with command-line options.
/// \~english @param [in] cAppName
///        PCSTR   - Pointer to application thread name
/// \~english @param [in] argc
///        int   - Number of command-line options
/// \~english @param [in] argv
///        char *[]   - Array of pointer to command-line options
/// \~english @param [in] CbHandler
///        const FrameworkunifiedDefaultCallbackHandler*   - Pointer to default callback functions.
/// \~english @param [in] cmdLineOptions
///        CustomCommandLineOptions*   - Parser setting of command-line options(Optional. When don't use, set NULL.)
/// \~english @par
///        FrameworkunifiedDefaultCallbackHandler Structure
/// \~english @code
///                 typedef struct _FrameworkunifiedDefaultCallbackHandler
///                 {
///                       CbFuncPtr onInitilization;               /* Function is called when a Dispatcher is created.
///                                   */
///                       CbFuncPtr onDestroy;                     /* Function is called when the Dispatcher is
///                       released.            */
///                       CbFuncPtr onStart;                       /* Function is called when the Dispatcher is started.
///                                   */
///                       CbFuncPtr onStop;                        /* Function is called when the Dispatcher is stopped.
///                                   */
///                       CbFuncPtr onPreStart;                    /* Function is called when the Dispatcher is pre started. */
///                       CbFuncPtr onPreStop;                     /* Function is called when the Dispatcher is pre stoped. */
///                       CbFuncPtr onBackgroundStart;             /* Function is called when the Dispatcher is Background started. */
///                       CbFuncPtr onBackgroundStop;              /* Function is called when the Dispatcher is Background stopped. */
///                       CbFuncPtr onDebugDump;                   /* Function is called when the Dispatcher detects
///                       abnormal state. */
///                       CbFuncPtr createStateMachine;            /* Set dummy function that does nothing.
///                                   */
///                       CbFuncPtr ssFrameworkInterface;          /* Function to connect to SystemManager
///                                   */
///                 } FrameworkunifiedDefaultCallbackHandler;
///            @endcode
/// \~english @par
///        CustomCommandLineOptions Structure
/// \~english @code
///                 typedef struct _CustomCommandLineOptions
///                 {
///                       PCSTR            cShortOptions;          /* Short options list. */
///                       PCHAR            cLongOptions;           /* Reserved. Set to NULL. */
///                       CbArgumentParser callback;               /* Pointer to callback function to parse command-line
///                       options. */
///                 } CustomCommandLineOptions;
///            @endcode
/// \~english @par
///        About setting of default callback functions(FrameworkunifiedDefaultCallbackHandler)
///        - Use FRAMEWORKUNIFIED_MAKE_DEFAULT_CALLBACK when initialize of FrameworkunifiedDefaultCallbackHandler structure as argument CbHandler.
///        - Application that run this API need to define functions below.(allow to dummy function that does nothing.)
///            - EFrameworkunifiedStatus FrameworkunifiedOnInitialization(HANDLE hApp)
///            - EFrameworkunifiedStatus FrameworkunifiedOnStart(HANDLE hApp)
///            - EFrameworkunifiedStatus FrameworkunifiedOnStop(HANDLE hApp)
///            - EFrameworkunifiedStatus FrameworkunifiedOnPreStart(HANDLE hApp)
///            - EFrameworkunifiedStatus FrameworkunifiedOnPreStop(HANDLE hApp)
///            - EFrameworkunifiedStatus FrameworkunifiedOnBackgroundStart(HANDLE hApp)
///            - EFrameworkunifiedStatus FrameworkunifiedOnBackgroundStop(HANDLE hApp)
///            - EFrameworkunifiedStatus FrameworkunifiedOnDebugDump(HANDLE hApp)
///            - EFrameworkunifiedStatus FrameworkunifiedOnDestroy(HANDLE hApp)
///            - EFrameworkunifiedStatus FrameworkunifiedCreateStateMachine(HANDLE hApp)
///        - If application is resident service, link library libSS_SystemIfUnified
///          (This library provides the function FrameworkunifiedSSFrameworkInterface to connect to SystemManager.).
///        - If application is nonresident service, define function that does nothing below.
///            - EFrameworkunifiedStatus FrameworkunifiedSSFrameworkInterface(HANDLE hApp)
/// \~english @retval eFrameworkunifiedStatusNullPointer NULL pointer specified
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
/// \~english @retval eFrameworkunifiedStatusDuplicate Duplication error of entry
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusErrOther Other error has occurred(Cannot access shared memory, etc.)
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
/// \~english @par Prerequisite
///       - Prerequisites are nothing.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - Application thread name specified in the argument (cAppName) is NULL. [eFrameworkunifiedStatusNullPointer]
///       - Pointer to default callback functions specified in the argument (CbHandler) is NULL. [eFrameworkunifiedStatusNullPointer]
///       - One of members in default callback functions specified in the argument (CbHandler) is NULL.
///       [eFrameworkunifiedStatusInvldParam]
///       - Application thread name specified in the argument (cAppName) is not appropriate(The name is more than
///       15byte). [eFrameworkunifiedStatusNullPointer]
///       - Failed to open message queue (mq_open). [eFrameworkunifiedStatusNullPointer]
///       - Failed to get memory (malloc) of message queue info area. [eFrameworkunifiedStatusNullPointer]
///       - Failed to initialize socket for monitoring abnormal state (socket, bind, listen). [eFrameworkunifiedStatusFail]
///       - Session message queue is full to the NPP Service. [eFrameworkunifiedStatusMsgQFull]
///       - It is invalid transmission file descriptor of the session message to the NPP Service. [eFrameworkunifiedStatusErrNoEBADF]
///       - Interruption by the system call (signal) has occurred during the transmission of the session message for the
///       NPP Service. [eFrameworkunifiedStatusErrNoEINTR]
///       - Incorrect size of the transmit buffer of the session message to the NPP Service. [eFrameworkunifiedStatusInvldBufSize]
///       - Any errors occur during the transmission of a session message to the NPP Service. [eFrameworkunifiedStatusFail]
///       - Failed to create of epoll instance (epoll_create1). [eFrameworkunifiedStatusInvldHandle]
///       - Failed to register of a descriptor of the message queue to the epoll instance (epoll_ctl).
///       [eFrameworkunifiedStatusInvldHandle]
///       - Failed to create file descriptor for receive event (eventfd). [eFrameworkunifiedStatusFail]
///       - Failed to register of a descriptor for receive event to the epoll instance (epoll_ctl). [eFrameworkunifiedStatusFail]
///       - Failed to register of a socket for detect abnormal state to the epoll instance (epoll_ctl). [eFrameworkunifiedStatusFail]
///       - The result of FrameworkunifiedOnInitialization is not eFrameworkunifiedStatusOK. [eFrameworkunifiedStatusNullPointer]
/// \~english @par Detail
///        This API creates and initializes the dispatcher with command-line options.\n
///        It starts to mainloop that receives request or notification, and runs registered callback to dispatcher.
///        When success starting mainloop, never return.
///        Use this API to take over a command-line options to the dispatcher.
/// \~english @par Classification
///          Public
/// \~english @par Type
///        No match
/// \~english @see
///        FrameworkunifiedDispatcher, FrameworkunifiedCreateDispatcher, FrameworkunifiedDispatchBlock,
///        FrameworkunifiedDispatchProcess, FrameworkunifiedCloseDispatcher,
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedDispatcherWithArguments(PCSTR cAppName, int argc, char *argv[],
                                      const FrameworkunifiedDefaultCallbackHandler *CbHandler,
                                      CustomCommandLineOptions *cmdLineOptions = NULL);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedSimpleDispatcher
/// \~english @par none
///        Generatings/intializes a Dispatcher and executs main loops for handling events.
/// \~english @param [in] cAppName
///        PCSTR   - Pointer to the application thread name
/// \~english @param [in] CbHandler
///        const FrameworkunifiedDefaultCallbackHandler*   - Pointer to a default-defined callback function
/// \~english @param [in] CbShutdown
///        CbFuncPtr   - Pointer to the callack function o be called when the main loop is exited
/// \~english @param [in] isChildThread
///        BOOL   - Whether the child thread is a dispatcher (Default:TRUE)
/// \~english @par
///        FrameworkunifiedDefaultCallbackHandler Structure
/// \~english @code
///                 typedef struct _FrameworkunifiedDefaultCallbackHandler
///                 {
///                   CbFuncPtr onInitilization;       /* Callback function to run when creating dispatcher instance */
///                   CbFuncPtr onDestroy;             /* Callback function to run when releasing dispatcher instance */
///                   CbFuncPtr onStart;               /* Callback function executed when dispatcher starts processing */
///                   CbFuncPtr onStop;                /* Callback function to be executed when dispatcher termination is requested */
///                   CbFuncPtr onPreStart;            /* Callback function executed when dispatcher requestes activation */
///                   CbFuncPtr onPreStop;             /* Callback function to be executed when an-request is made to the dispatcher on board status */
///                   CbFuncPtr onBackgroundStart;     /* Callback function executed when dispatcher parking is requested */
///                   CbFuncPtr onBackgroundStop;      /* Callback function executed when requesting termination of a dispatcher's parking status */
///                   CbFuncPtr onDebugDump;           /* Callback function to be executed when an error is detected */
///                   CbFuncPtr createStateMachine;    /* Specify an empty function */
///                   CbFuncPtr ssFrameworkInterface;  /* Callback function for connecting to the systemManager */
///                 } FrameworkunifiedDefaultCallbackHandler;
///            @endcode
/// \~english @par
///        About setting the default definition callback function (FrameworkunifiedDefaultCallbackHandler)
///          The FrameworkunifiedDefaultCallbackHandler structure passed as argument CbHandler must be initialized using the FRAMEWORKUNIFIED_MAKE_DEFAULT_CALLBACK macro.
///        - The application that executes this API must define a function with the following name. (Can also be an functio)
///          - EFrameworkunifiedStatus FrameworkunifiedOnInitialization(HANDLE hApp)
///          - EFrameworkunifiedStatus FrameworkunifiedOnStart(HANDLE hApp)
///          - EFrameworkunifiedStatus FrameworkunifiedOnStop(HANDLE hApp)
///          - EFrameworkunifiedStatus FrameworkunifiedOnPreStart(HANDLE hApp)
///          - EFrameworkunifiedStatus FrameworkunifiedOnPreStop(HANDLE hApp)
///          - EFrameworkunifiedStatus FrameworkunifiedOnBackgroundStart(HANDLE hApp)
///          - EFrameworkunifiedStatus FrameworkunifiedOnBackgroundStop(HANDLE hApp)
///          - EFrameworkunifiedStatus FrameworkunifiedOnDebugDump(HANDLE hApp)
///          - EFrameworkunifiedStatus FrameworkunifiedOnDestroy(HANDLE hApp)
///          - EFrameworkunifiedStatus FrameworkunifiedCreateStateMachine(HANDLE hApp)
///          If the applicatio is a resident service, link the library libSS_SystemIfUnified
///          (which provides the function FrameworkunifiedSSFrameworkInterface that performs connection processin with SystemManager).
///        - If the applicatio is a nonresident service, define the following names with empy function.
///          - EFrameworkunifiedStatus FrameworkunifiedSSFrameworkInterface(HANDLE hApp)
/// \~english @retval eFrameworkunifiedStatusNullPointer Specifiying NULL pointers
/// \~english @retval eFrameworkunifiedStatusFail User-specific continuation.
/// \~english @retval eFrameworkunifiedStatusDuplicate Duplicate entry error
/// \~english @retval eFrameworkunifiedStatusInvldHandle Category Type
/// \~english @retval eFrameworkunifiedStatusErrOther Shared-memory accesses for sending message for NPP Service fail.
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF If the timers fail,...
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR Generating an interrupt by a system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Service does not exist
/// \~english @par Failed to acquire
///        - 
/// \~english @par Log output string
///        - The internal state is not changed.
/// \~english @par You do not use.
///        - When the pointer (CbHandler) to the default-definition callback function specified by the arguments is NULL [eFrameworkunifiedStatusNullPointer]
///        - If any of the member of the default-defined callback-function sruct specified in he arguments is NULL [eFrameworkunifiedStatusInvldParam]
///        - When the pointer (CbShutdown) to the callback fuction to be called when the main loop specified by the arguments exits is NULL [eFrameworkunifiedStatusFail]
///        - When the pointer (cAppName) to the threade name of the application specified by the arguments is NULL [eFrameworkunifiedStatusNullPointer]
///        - When the application name (cAppName) specified in the parameter exceeds 15 byte [eFrameworkunifiedStatusNullPointer]
///        - Failed to open a message queue (mq_open) [eFrameworkunifiedStatusNullPointer]
///        - When acquisition (malloc) of the message queue management information area fails, [eFrameworkunifiedStatusNullPointer]
///        - If the creation (socket, bind, listen) of sockets for error monitoring fails,... [eFrameworkunifiedStatusFail]
///        - Application handles and callback function cannot be added to Notification lists [eFrameworkunifiedStatusDuplicate]
///        - When the Notification name specified in the arguments has already been registred i the Dispatcher [eFrameworkunifiedStatusDuplicate]
///        - Full Message Queue for Sessions for NPP Service [eFrameworkunifiedStatusMsgQFull]
///        - Incorrect file descriptors for sending messages for NPP Service sessions [eFrameworkunifiedStatusErrNoEBADF]
///        - When an interrupt by a system call (signal) occurs while sending session-specific messages for NPP Service. [eFrameworkunifiedStatusErrNoEINTR]
///        - Incorrectly sized send buffers for sessions destined for the NPP Service.... [eFrameworkunifiedStatusInvldBufSize]
///        - If there are any errors sending messages for sessions intended for NPP Service [eFrameworkunifiedStatusFail]
///        - Failed to access shared memory for sending messages for sessions for NPP Service [eFrameworkunifiedStatusErrOther]
///        - Failed to create epoll instances (epoll_create1) [eFrameworkunifiedStatusInvldHandle]
///        - Failed to register Dispatcher message queue descriptor for epoll instances (epoll_ctl) [eFrameworkunifiedStatusFail]
///        - Failed to create file descriptor for receiving events to Dispatcher (eventfd) [eFrameworkunifiedStatusFail]
///        - Failed to register file descriptor for receiving Dispatcher events to epoll instances (epoll_ctl) [eFrameworkunifiedStatusFail]
///        - Failed to register socket for monitoring Dispatcher error in epoll instances (epoll_ctl) [eFrameworkunifiedStatusFail]
///        - If FrameworkunifiedOnInitialization of result is not eFrameworkunifiedStatusOK [eFrameworkunifiedStatusNullPointer]
/// \~english @par Type
///        Creats or initializes a Dispatcher, receives requests and notifications to the Dispatcher, and starts a main loop that executes the registred callback functions.\n
///        When the main loop is started normally, this API does not retur until the hread terminates.\n
///        At thread termination, the callback function specified by the third argument is called, and the processing result is returned.\n
///        \n
///        This API can be used for both main and child threads.\n
///        However, since this API is an API for chid threads created without going through the API of NS framework such as \ref FrameworkunifiedCreateChildThread,
///        it is recommended to use \ref FrameworkunifiedDispatcherWithArguments or other API to create Dispatcher in the main thread.\n
/// \~english @par Immediate
///          Brief
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Not applicable
/// \~english @see
///        FrameworkunifiedDispatcherWithArguments
///
/// \~english @par Brief
///        Generatings/intializes a Dispatcher and executs main loops for handling events
/// \~english @param [in] cAppName
///        PCSTR - Application/ thread name
/// \~english @param [in] CbHandler
///        const FrameworkunifiedDefaultCallbackHandler*   - Function that primes the dispatch pump. i.e. start function
/// \~english @param [in] CbShutdown
///        CbFuncPtr * - Shutdown function. Functions gets called if the dispatcher loop exits.
/// \~English @param[in] isChildThread
///        BOOL - Default value is TRUE. TRUE - child thread dispatcher else main thread dispatcher
/// \~english @retval Never does. Unless any callback function returns eFrameworkunifiedStatusExit
/// \~english @retval eFrameworkunifiedStatusNullPointer NULL pointer
/// \~english @retval eFrameworkunifiedStatusFail  Invalid Parameter
/// \~english @retval eFrameworkunifiedStatusErrOther Unknown Error
/// \~english @par Prerequisite
///       - Prerequisites are nothing.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - cAppName is NULL[eFrameworkunifiedStatusNullPointer]
///        - Length of cAppName is longer than MAX_NAME_SIZE_APP[eFrameworkunifiedStatusNullPointer]
///        - NativeService has some issues [eFrameworkunifiedStatusNullPointer]
///        - When the pointer (CbHandler) to the default-definition callback function specified by the arguments is NULL [eFrameworkunifiedStatusNullPointer]
///        - If any of the member of the default-defined callback-function sruct specified in he arguments is NULL [eFrameworkunifiedStatusInvldParam]
///        - When the pointer (CbShutdown) to the callback fuction to be called when the main loop specified by the arguments exits is NULL [eFrameworkunifiedStatusFail]
///        - Failed to open a message queue (mq_open) [eFrameworkunifiedStatusNullPointer]
///        - When acquisition (malloc) of the message queue management information area fails, [eFrameworkunifiedStatusNullPointer]
///        - If the creation (socket, bind, listen) of sockets for error monitoring fails,... [eFrameworkunifiedStatusFail]
///        - Application handles and callback function cannot be added to Notification lists [eFrameworkunifiedStatusDuplicate]
///        - When the Notification name specified in the arguments has already been registred i the Dispatcher [eFrameworkunifiedStatusDuplicate]
///        - Full Message Queue for Sessions for NPP Service [eFrameworkunifiedStatusMsgQFull]
///        - Incorrect file descriptors for sending messages for NPP Service sessions [eFrameworkunifiedStatusErrNoEBADF]
///        - When an interrupt by a system call (signal) occurs while sending NPP Service session-specific messages [eFrameworkunifiedStatusErrNoEINTR]
///        - Incorrectly sized send buffers for sessions destined for the NPP Service.... [eFrameworkunifiedStatusInvldBufSize]
///        - If there are any errors sending messages for sessions intended for NPP Service [eFrameworkunifiedStatusFail]
///        - When an interrupt by a system call (signal) occurs while sending Service session-specific messages. [eFrameworkunifiedStatusErrOther]
///        - Failed to create epoll instances (epoll_create1) [eFrameworkunifiedStatusInvldHandle]
///        - Failed to register Dispatcher message queue descriptor for epoll instances (epoll_ctl) [eFrameworkunifiedStatusFail]
///        - Failed to create file descriptor for receiving events to Dispatcher (eventfd) [eFrameworkunifiedStatusFail]
///        - Failed to register file descriptor for receiving Dispatcher events to epoll instances (epoll_ctl) [eFrameworkunifiedStatusFail]
///        - Failed to register socket for monitoring Dispatcher error in epoll instances (epoll_ctl) [eFrameworkunifiedStatusFail]
///        - If FrameworkunifiedOnInitialization of result is not eFrameworkunifiedStatusOK [eFrameworkunifiedStatusNullPointer]
/// \~english @par Detail
///        Creates, initializes and runs the dispatcher. This dispatcher function can be used with child thread or main
///        thread. <BR>
///        But, recommended to use for child thread. For main thread use \ref FrameworkunifiedDispatcherWithArguments<BR>
///        Intention to provide this API is for child threads created from dynamically loaded plugin, where thread wants
///        to <BR>
///        subscribe to PASA notification, attach callbacks. This can be done in the start function specified with this
///        API.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        FrameworkunifiedDispatcherWithArguments
/// \~english @todo not enough inforamtion need readon why this API is for child thread.
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSimpleDispatcher(PCSTR cAppName, const FrameworkunifiedDefaultCallbackHandler *CbHandler, CbFuncPtr CbShutdown,
                               BOOL isChildThread = TRUE);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedCreateDispatcherWithoutLoop
/// \~english @par Brief
///        This API creates and initializes the dispatcher with command-line options.
/// \~english @param [in] cAppName
///        PCSTR   - Pointer to application thread name
/// \~english @param [out] hApp
///        HANDLE   - Handle for Application
/// \~english @param [in] argc
///        int   - Number of command-line options
/// \~english @param [in] argv
///        char *[]   - Array of pointer to command-line options
/// \~english @param [in] CbHandler
///        const FrameworkunifiedDefaultCallbackHandler*   - Pointer to default callback functions.
/// \~english @param [in] bIsConnectSM
///        BOOL   - whether or not connect with System Manager(TRUE/FALSE)
/// \~english @param [in] cmdLineOptions
///        CustomCommandLineOptions*   - Parser setting of command-line options(Optional. When don't use, set NULL.)
/// \~english @par
///        FrameworkunifiedDefaultCallbackHandler Structure
/// \~english @code
///                 typedef struct _FrameworkunifiedDefaultCallbackHandler
///                 {
///                       CbFuncPtr onInitilization;               /* Function is called when a Dispatcher is created.
///                                   */
///                       CbFuncPtr onDestroy;                     /* Function is called when the Dispatcher is
///                       released.            */
///                       CbFuncPtr onStart;                       /* Function is called when the Dispatcher is started.
///                                   */
///                       CbFuncPtr onStop;                        /* Function is called when the Dispatcher is stopped.
///                                   */
///                       CbFuncPtr onPreStart;                    /* Function is called when the Dispatcher is pre started. */
///                       CbFuncPtr onPreStop;                     /* Function is called when the Dispatcher is pre stoped. */
///                       CbFuncPtr onBackgroundStart;             /* Function is called when the Dispatcher is Background started. */
///                       CbFuncPtr onBackgroundStop;              /* Function is called when the Dispatcher is Background stopped. */
///                       CbFuncPtr onDebugDump;                   /* Function is called when the Dispatcher detects
///                       abnormal state. */
///                       CbFuncPtr createStateMachine;            /* Set dummy function that does nothing.
///                                   */
///                       CbFuncPtr ssFrameworkInterface;          /* Function to connect to SystemManager
///                                   */
///                 } FrameworkunifiedDefaultCallbackHandler;
///            @endcode
/// \~english @par
///        CustomCommandLineOptions Structure
/// \~english @code
///                 typedef struct _CustomCommandLineOptions
///                 {
///                       PCSTR            cShortOptions;          /* Short options list. */
///                       PCHAR            cLongOptions;           /* Reserved. Set to NULL. */
///                       CbArgumentParser callback;               /* Pointer to callback function to parse command-line
///                       options. */
///                 } CustomCommandLineOptions;
///            @endcode
/// \~english @par
///        About setting of default callback functions(FrameworkunifiedDefaultCallbackHandler)
///        - Use FRAMEWORKUNIFIED_MAKE_DEFAULT_CALLBACK when initialize of FrameworkunifiedDefaultCallbackHandler structure as argument CbHandler.
///        - Application that run this API need to define functions below.(allow to dummy function that does nothing.)
///            - EFrameworkunifiedStatus FrameworkunifiedOnInitialization(HANDLE hApp)
///            - EFrameworkunifiedStatus FrameworkunifiedOnStart(HANDLE hApp)
///            - EFrameworkunifiedStatus FrameworkunifiedOnStop(HANDLE hApp)
///            - EFrameworkunifiedStatus FrameworkunifiedOnPreStart(HANDLE hApp)
///            - EFrameworkunifiedStatus FrameworkunifiedOnPreStop(HANDLE hApp)
///            - EFrameworkunifiedStatus FrameworkunifiedOnBackgroundStart(HANDLE hApp)
///            - EFrameworkunifiedStatus FrameworkunifiedOnBackgroundStop(HANDLE hApp)
///            - EFrameworkunifiedStatus FrameworkunifiedOnDebugDump(HANDLE hApp)
///            - EFrameworkunifiedStatus FrameworkunifiedOnDestroy(HANDLE hApp)
///            - EFrameworkunifiedStatus FrameworkunifiedCreateStateMachine(HANDLE hApp)
///        - If application is resident service, link library libSS_SystemIfUnified
///          (This library provides the function FrameworkunifiedSSFrameworkInterface to connect to SystemManager.).
///        - If application is nonresident service, define function that does nothing below.
///            - EFrameworkunifiedStatus FrameworkunifiedSSFrameworkInterface(HANDLE hApp)
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusNullPointer NULL pointer specified
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
/// \~english @retval eFrameworkunifiedStatusDuplicate Duplication error of entry
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusErrOther Other error has occurred(Cannot access shared memory, etc.)
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
/// \~english @par Prerequisite
///       - Prerequisites are nothing.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - Application thread name specified in the argument (cAppName) is NULL. [eFrameworkunifiedStatusNullPointer]
///       - Pointer to default callback functions specified in the argument (CbHandler) is NULL. [eFrameworkunifiedStatusNullPointer]
///       - One of members in default callback functions specified in the argument (CbHandler) is NULL.
///       [eFrameworkunifiedStatusInvldParam]
///       - Application thread name specified in the argument (cAppName) is not appropriate(The name is more than
///       15byte). [eFrameworkunifiedStatusNullPointer]
///       - Failed to open message queue (mq_open). [eFrameworkunifiedStatusNullPointer]
///       - Failed to get memory (malloc) of message queue info area. [eFrameworkunifiedStatusNullPointer]
///       - Failed to initialize socket for monitoring abnormal state (socket, bind, listen). [eFrameworkunifiedStatusFail]
///       - Session message queue is full to the NPP Service. [eFrameworkunifiedStatusMsgQFull]
///       - It is invalid transmission file descriptor of the session message to the NPP Service. [eFrameworkunifiedStatusErrNoEBADF]
///       - Interruption by the system call (signal) has occurred during the transmission of the session message for the
///       NPP Service. [eFrameworkunifiedStatusErrNoEINTR]
///       - Incorrect size of the transmit buffer of the session message to the NPP Service. [eFrameworkunifiedStatusInvldBufSize]
///       - Any errors occur during the transmission of a session message to the NPP Service. [eFrameworkunifiedStatusFail]
///       - Failed to create of epoll instance (epoll_create1). [eFrameworkunifiedStatusInvldHandle]
///       - Failed to register of a descriptor of the message queue to the epoll instance (epoll_ctl).
///       [eFrameworkunifiedStatusInvldHandle]
///       - Failed to create file descriptor for receive event (eventfd). [eFrameworkunifiedStatusFail]
///       - Failed to register of a descriptor for receive event to the epoll instance (epoll_ctl). [eFrameworkunifiedStatusFail]
///       - Failed to register of a socket for detect abnormal state to the epoll instance (epoll_ctl). [eFrameworkunifiedStatusFail]
/// \~english @par Detail
///        This API creates and initializes the dispatcher with command-line options without running main loop.\n
///        Use this, if application control some contexts(include NS framework).\n
///        when the dispatcher generated by this API, Application need to handle event.\n
///        Use FrameworkunifiedGetDispatcherFD and FrameworkunifiedDispatchProcessWithoutLoop for event handling.
/// \~english @par Classification
///          Public
/// \~english @par Type
///        Open Close
/// \~english @see
///        FrameworkunifiedCreateDispatcherWithoutLoop, FrameworkunifiedDestroyDispatcherWithoutLoop, FrameworkunifiedGetDispatcherFD
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedCreateDispatcherWithoutLoop(PCSTR cAppName, HANDLE &hApp, int argc, char *argv[],  // NOLINT  (readability/nolint)
                                          const FrameworkunifiedDefaultCallbackHandler *CbHandler,
                                          BOOL bIsConnectSM, CustomCommandLineOptions *cmdLineOptions = NULL);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedDispatchProcessWithoutLoop
/// \~english @par Brief
///        Receive request or notification, and run registered callback to dispatcher.
/// \~english @param [in] hApp
///        HANDLE   - Handle for Application
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
/// \~english @par Prerequisite
///       - Generation/Initialization of Dispatcher for the Application by FrameworkunifiedCreateDispatcherWithoutLoop has been done.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - HANDLE specified in the argument (hApp) is NULL. [eFrameworkunifiedStatusInvldHandle]
///       - HANDLE specified in the argument (hApp) is not appropriate(which is invalid). [eFrameworkunifiedStatusInvldHandle]
///       - If the waiting message (epoll_wait) , was terminated with an error other than interrupt (EINTR)
///       [eFrameworkunifiedStatusFail]
///       - Service name that has been set in the received message exceeds the 20byte. [eFrameworkunifiedStatusFail]
///       - Message queue in the HANDLE specified in the argument (hApp) is not for receive. [eFrameworkunifiedStatusFail]
///       - Message queue for receive is empty. [eFrameworkunifiedStatusFail]
///       - File descriptor of message queue for receive is invalid. [eFrameworkunifiedStatusFail]
///       - Interruption by the system call (signal) has occurred during the message receiving. [eFrameworkunifiedStatusFail]
///       - Size of the receive buffer of the message is illegal. [eFrameworkunifiedStatusFail]
///       - Any errors occur during the message receiving. [eFrameworkunifiedStatusFail]
///       - Service name in the received message is not appropriate(The name is NULL, Length is 0byte) [eFrameworkunifiedStatusFail]
///       - The received message data length is more than data length in the received message header. [eFrameworkunifiedStatusFail]
///       - The received message is not appropriate. [eFrameworkunifiedStatusFail]
///       - Shared memory ID for receiving message is invalid. [eFrameworkunifiedStatusFail]
///       - Failed to access to shared memory for receive message. [eFrameworkunifiedStatusFail]
///       - Data size of shared memory for receiving message is more than destination buffer size. [eFrameworkunifiedStatusFail]
///       - Message queue of the processing result destination is invalid. [eFrameworkunifiedStatusFail]
///       - The transmission data that is the received request processing result is NULL. [eFrameworkunifiedStatusFail]
///       - Service name of the processing result destination is not appropriate(The name is NULL, more than 20byte).
///       [eFrameworkunifiedStatusFail]
///       - Failed to access to shared memory for send message. [eFrameworkunifiedStatusFail]
///       - Message queue type is not for sending. [eFrameworkunifiedStatusFail]
///       - Message queue for sending is full. [eFrameworkunifiedStatusFail]
///       - File descriptor of message queue for sending is invalid. [eFrameworkunifiedStatusFail]
///       - Interruption by the system call (signal) has occurred during the message sending. [eFrameworkunifiedStatusFail]
///       - Size of the send buffer of the message is illegal. [eFrameworkunifiedStatusFail]
///       - Any errors occur during the message sending. [eFrameworkunifiedStatusFail]
///       - Failed to access to shared memory for sending message. [eFrameworkunifiedStatusFail]
///       - Failed to get memory buffer for message receiving. [eFrameworkunifiedStatusFail]
///       - Failed to read data from file descriptor that is set from application. [eFrameworkunifiedStatusFail]
///       - Failed to run callback that is set from application. [eFrameworkunifiedStatusFail]
///       - The callback that is set from application returned error. [eFrameworkunifiedStatusFail]
///       - I could not found callback function associated with file descriptor that is set from application.
///       [eFrameworkunifiedStatusFail]
/// \~english @par Detail
///        If the Dispatcher was created by FrameworkunifiedCreateDispatcherWithoutLoop, \n
///        you use this API to receive request or notification, and run registered callback to dispatcher.
/// \~english @par Classification
///          Public
/// \~english @par Type
///        No match
/// \~english @see
///        FrameworkunifiedCreateDispatcherWithoutLoop, FrameworkunifiedDestroyDispatcherWithoutLoop, FrameworkunifiedGetDispatcherFD
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedDispatchProcessWithoutLoop(HANDLE hApp);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedDestroyDispatcherWithoutLoop
/// \~english @par Brief
///        This API destroy the dispatcher created by FrameworkunifiedCreateDispatcherWithoutLoop.
/// \~english @param [in] hApp
///        HANDLE   - Handle for Application
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusNullPointer NULL pointer specified
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @par Prerequisite
///        - Generation/Initialization of Dispatcher for the Application by FrameworkunifiedCreateDispatcherWithoutLoop has been
///        done.
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - HANDLE specified in the argument (hApp) is NULL. [eFrameworkunifiedStatusInvldHandle]
///        - HANDLE specified in the argument (hApp) is not appropriate(which is invalid). [eFrameworkunifiedStatusInvldHandle]
///        - Failed to release memory referenced by HANDLE specified in the argument (hApp). [eFrameworkunifiedStatusNullPointer]
/// \~english @par Detail
///        This API destroy the dispatcher created by FrameworkunifiedCreateDispatcherWithoutLoop.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Sync
/// \~english @see
///        FrameworkunifiedCreateDispatcherWithoutLoop
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedDestroyDispatcherWithoutLoop(HANDLE hApp);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedGetDispatcherFD
/// \~english @par Brief
///        Get the Dispatcher file descriptor corresponding to the given application handle.
/// \~english @param [in] hApp
///        HANDLE   - Handle for Application
/// \~english @param [out] efd
///        int*   - Dispatcher file descriptor
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusNullPointer NULL pointer specified
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @par Prerequisite
///        - Generation/Initialization of Dispatcher for the Application by FrameworkunifiedCreateDispatcherWithoutLoop has been
///        done.
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - HANDLE specified in the argument (hApp) is NULL. [eFrameworkunifiedStatusInvldHandle]
///        - HANDLE specified in the argument (hApp) is not appropriate(which is invalid). [eFrameworkunifiedStatusInvldHandle]
///        - The argument efd is NULL. [eFrameworkunifiedStatusNullPointer]
/// \~english @par Detail
///        This API gets the Dispatcher file descriptor corresponding to the given application handle.\n
///        The application can use  file descriptor to detect event of NS Framework.\n
///        When events detected, run FrameworkunifiedDispatchProcessWithoutLoop to handling events.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see FrameworkunifiedDispatchProcessWithoutLoop
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedGetDispatcherFD(HANDLE hApp, SI_32 *efd);  // Replace int with SI_32

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedGetDefaultCbHandler
/// \~english @par Brief
///        Get default callback handler table
/// \~english @param [out] CbHandler
///        FrameworkunifiedGetDefaultCbHandler * - Default callback handler table pointer
/// \~english @par
///        FrameworkunifiedDefaultCallbackHandler Structure
/// \~english @code
///                 typedef struct _FrameworkunifiedDefaultCallbackHandler
///                 {
///                       CbFuncPtr onInitilization;               /* Function is called when a Dispatcher is created.
///                                   */
///                       CbFuncPtr onDestroy;                     /* Function is called when the Dispatcher is
///                       released.            */
///                       CbFuncPtr onStart;                       /* Function is called when the Dispatcher is started.
///                                   */
///                       CbFuncPtr onStop;                        /* Function is called when the Dispatcher is stopped.
///                                   */
///                       CbFuncPtr onPreStart;                    /* Function is called when the Dispatcher is pre started. */
///                       CbFuncPtr onPreStop;                     /* Function is called when the Dispatcher is pre stoped. */
///                       CbFuncPtr onBackgroundStart;             /* Function is called when the Dispatcher is Background started. */
///                       CbFuncPtr onBackgroundStop;              /* Function is called when the Dispatcher is Background stopped. */
///                       CbFuncPtr onDebugDump;                   /* Function is called when the Dispatcher detects
///                       abnormal state. */
///                       CbFuncPtr createStateMachine;            /* Set dummy function that does nothing.
///                                   */
///                       CbFuncPtr ssFrameworkInterface;          /* Function to connect to SystemManager
///                                   */
///                 } FrameworkunifiedDefaultCallbackHandler;
///            @endcode
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusNullPointer NULL pointer specified
/// \~english @par Prerequisite
///        - Generation/Initialization of Dispatcher for the Application by FrameworkunifiedCreateDispatcherWithoutLoop has been
///        done.
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - Pointer to default callback functions specified in the argument (CbHandler) is NULL.
///        [eFrameworkunifiedStatusNullPointer]
/// \~english @par Detail
///        Gets the default defined callback fuction set by FrameworkunifiedDispatcherWithArguments, FrameworkunifiedSimpleDispatcher, and FrameworkunifiedCreateDispatcherWithoutLoop.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see FrameworkunifiedDispatchProcessWithoutLoop
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedGetDefaultCbHandler(FrameworkunifiedDefaultCallbackHandler *CbHandler);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedAttachLostSessionCallbackToDispatcher
/// \~english Register
///        abnormal session disconnect callback
/// \~english @param[in] hApp
///        HANDLE - Application framework handle
/// \~english @param[in] fpLostSession
///        - Generation/Initialization of Dispatcher for the Application by FrameworkunifiedCreateDispatcherWithoutLoop has been
///        done.
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid Parameter
/// \~english @par Preconditions
///        - Generation/Initialization of Dispatcher for the Application by FrameworkunifiedCreateDispatcherWithoutLoop has been
///        done.
/// \~english @par Change of internal status
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - hApp is NULL [eFrameworkunifiedStatusInvldParam]
///        - hApp is invalid [eFrameworkunifiedStatusInvldParam]
///        - fpLostSession is NULL [eFrameworkunifiedStatusInvldParam]
/// \~english @par Detail
///        Registers a callback fuction to be called when a session disconnection occurs in the specified application handle.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see FrameworkunifiedCreateDispatcherWithoutLoop, FrameworkunifiedDispatcherWithArguments, FrameworkunifiedSimpleDispatcher
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedAttachLostSessionCallbackToDispatcher(HANDLE hApp, CbFuncPtr fpLostSession);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedGetLostSessionData
/// \~english @par Brief
///        Get last lost session data
/// \~english @param [in] hApp
///        HANDLE - Application framework handle
/// \~english @param [out] pServiceName
///        PSTR - Disconnect service name
/// \~english @param [out] puiSessionId
///        PUI_32 - Disconnect session ID
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid Parameter
/// \~english @retval eFrameworkunifiedStatusAccessError Error when accessing resource
/// \~english @par Preconditions
///        - Generation/Initialization of Dispatcher for the Application
///          by FrameworkunifiedCreateDispatcherWithoutLoop has been done.
/// \~english @par Change of internal status
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - hApp is invalid [eFrameworkunifiedStatusInvldParam]
///        - pServiceName is NULL [eFrameworkunifiedStatusInvldParam]
///        - puiSessionId is NULL [eFrameworkunifiedStatusInvldParam]
///        - Lost session queue is empty [eFrameworkunifiedStatusAccessError]
/// \~english @par Detail
///        Gets the service name and session ID disconnected from the hadle for the specified application.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        FrameworkunifiedCreateDispatcherWithoutLoop, FrameworkunifiedDispatcherWithArguments, FrameworkunifiedSimpleDispatcher
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedGetLostSessionData(HANDLE hApp, PSTR pServiceName, PUI_32 puiSessionId);

#endif /* __NATIVESERVICES_FRAMEWORK_FRAMEWORKUNIFIED_DISPATCHER_H__ */  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
