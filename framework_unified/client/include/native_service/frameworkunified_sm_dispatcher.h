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
///
///
///
///////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file frameworkunified_sm_dispatcher.h
 * @brief \~english Application dispatch interface functions
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
#ifndef __NATIVESERVICES_FRAMEWORK_FRAMEWORKUNIFIED_SM_DISPATCHER_H__  // NOLINT  (build/header_guard)
#define __NATIVESERVICES_FRAMEWORK_FRAMEWORKUNIFIED_SM_DISPATCHER_H__
#include <native_service/frameworkunified_framework_types.h>
#include <native_service/frameworkunified_sm_framework_types.h>

class CFrameworkunifiedHSMFramework;

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedHSMDispatcher
/// \~english @par Brief
///        Creates, initializes and runs the HSM dispatcher.
/// \~english @param [in] cAppName
///        PCSTR - pointer of Application/ thread name
/// \~english @retval EFrameworkunifiedStatus
/// \~english @par Prerequisite
///        - Prerequisites are nothing.
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - none
/// \~english @par Detail
///        - none
/// \~english @par Classification
///        Public
/// \~english @par Type
///        none
/// \~english @see
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedHSMDispatcher(PCSTR cAppName);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedHSMDispatcherWithArguments
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
/// \~english @param [in] f_pFrameworkunifiedHSM
///        CFrameworkunifiedHSMFramework* - state machine object pointer(default NULL)
/// \~english @param [in] cmdLineOptions
///        CustomCommandLineOptions*   - Parser setting of command-line options(Optional. When don't use, set NULL.)
/// \~english @par
///        FrameworkunifiedDefaultCallbackHandler Structure
/// \~english @code
///                 typedef struct _FrameworkunifiedDefaultCallbackHandler
///                 {
///                       CbFuncPtr onInitilization;      /* Function is called when a Dispatcher is created.      */
///                       CbFuncPtr onDestroy;            /* Function is called when the Dispatcher is released.   */
///                       CbFuncPtr onStart;              /* Function is called when the Dispatcher is started.    */
///                       CbFuncPtr onStop;               /* Function is called when the Dispatcher is stopped.    */
///                       CbFuncPtr onPreStart;           /* Function is called when the Dispatcher is pre started. */
///                       CbFuncPtr onPreStop;            /* Function is called when the Dispatcher is pre stoped. */
///                       CbFuncPtr onBackgroundStart;    /* Function is called when the Dispatcher is Background started. */
///                       CbFuncPtr onBackgroundStop;     /* Function is called when the Dispatcher is Background stopped. */
///                       CbFuncPtr onDebugDump;  /* Function is called when the Dispatcher detects abnormal state. */
///                       CbFuncPtr createStateMachine;      /* Set dummy function that does nothing.               */
///                       CbFuncPtr ssFrameworkInterface;     /* Function to connect to SystemManager               */
///                 } FrameworkunifiedDefaultCallbackHandler;
///            @endcode
/// \~english @par
///        CustomCommandLineOptions Structure
/// \~english @code
///                 typedef struct _CustomCommandLineOptions
///                 {
///                     PCSTR            cShortOptions;          /* Short options list. */
///                     PCHAR            cLongOptions;           /* Reserved. Set to NULL. */
///                     CbArgumentParser callback;  /* Pointer to callback function to parse command-line options. */
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
/// \~english @par
///        Please note the following points when using this API for application.
///        - Error handling is not performed when the state machine event registration process (CFrameworkunifiedHSMFramework::FrameworkunifiedCreate) fails.\n
///          The behavior of state machine objects must be guaranteed by the application implementation.
///        - When an exception occurs during Dispatcher generating, an error log is output and eFrameworkunifiedStatusOK is returned.
/// \~english @par Classification
///          Public
/// \~english @par Type
///        No match
/// \~english @see none
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedHSMDispatcherWithArguments(PCSTR cAppName, int argc, char *argv[],
                                         const FrameworkunifiedDefaultCallbackHandler *CbHandler,
                                         CFrameworkunifiedHSMFramework *f_pFrameworkunifiedHSM = NULL,
                                         CustomCommandLineOptions *cmdLineOptions = NULL);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedSetHSMType
/// \~english @par Brief
///        Set the Application Statemachine Type
/// \~english @param [in] cAppName
///        PCSTR - pointer of Application/ thread name
/// \~english @code
///     typedef enum _EUserChangeOptions {
///       eUserchangeIgnore = 0,
///       eUserchangeReInit,
///       eUserchangeRetPrevState
///     } EUserChangeOptions;
///            @endcode
/// \~english @retval EFrameworkunifiedStatus
/// \~english @par Prerequisite
///        - Prerequisites are nothing.
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - none
/// \~english @par Detail
///        - none
/// \~english @par Classification
///        Public
/// \~english @par Type
///        none
/// \~english @see 
///
////////////////////////////////////////////////////////////////////////////////////////////
void FrameworkunifiedSetHSMType(EUserChangeOptions f_eHSMType);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedEnableAutoPublishServiceAvailable
/// \~english @par Brief
///        Enables the AutoPublishServiceAvailability Feature
/// \~english @param none
/// \~english @retval EFrameworkunifiedStatus
/// \~english @par Prerequisite
///        - Prerequisites are nothing.
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - none
/// \~english @par Detail
///           Enables the AutoPublishServiceAvailability Feature that publishes the service
///           availability on entry of sFrameworkunifiedReady state and publishes the service unavailability
///           on exit of sFrameworkunifiedReady state.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        none
/// \~english @see 
///
////////////////////////////////////////////////////////////////////////////////////////////
void FrameworkunifiedEnableAutoPublishServiceAvailable();

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedDisableAutoPublishServiceAvailable
/// \~english @par Brief
///        Disable to the AutoPublishServiceAvailability Feature.
/// \~english @retval None
/// \~english @par Prerequisite
///       - Prerequisite is nothing.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       None.
/// \~english @par Detail
///        This API disable feature that notify automatically service availability to clients
///        at the timing of changing of internal state.\n
///        If you disable this feature, the application needs to publish service availability state explicitly
///        using FrameworkunifiedPublishServiceAvailability.
/// \~english @par Classification
///          Forbidden
/// \~english @see FrameworkunifiedHSMDispatcherWithArguments FrameworkunifiedPublishServiceAvailability
////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedDisableAutoPublishServiceAvailable
/// Disables the AutoPublishServiceAvailability Feature that publishes the service
/// availability on entry of sFrameworkunifiedReady state and publishes the service unavailability
/// on exit of sFrameworkunifiedReady state. Applications are responsible for publishing the service
/// availability.
void FrameworkunifiedDisableAutoPublishServiceAvailable();

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedHSMEnableWaitInStoppingState
////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedHSMEnableWaitInStoppingState
/// If HSM application should wait in sStoppingState until user explicitly publishes evFrameworkunifiedStopComplete,
/// then call this function before FrameworkunifiedHSMDispatcher APIs.
/// Default behavior is application will not wait is sStoppingState.
///
/// \~english @par Brief
///         Change state application when call FrameworkunifiedOnStop().
/// \~english @retval None
/// \~english @par Prerequisite
///       - Prerequisite is nothing.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       None.
/// \~english @par Detail
///            If HSM application should wait in sStoppingState until user explicitly publishes evFrameworkunifiedStopComplete,
///            then call this function before FrameworkunifiedHSMDispatcher APIs.
///            Default behavior is application will not wait in sStoppingState.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        none
/// \~english @see FrameworkunifiedHSMDisableWaitInStoppingState
VOID FrameworkunifiedHSMEnableWaitInStoppingState();

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedHSMDisableWaitInStoppingState
/// \~english @par Brief
///         Chanage state to not wait in sStoppingState.
/// \~english @retval None
/// \~english @par Prerequisite
///       - Prerequisite is nothing.
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       None.
/// \~english @par Detail
///       If HSM application should not wait in sStoppingState, then call this function before FrameworkunifiedHSMDispatcher APIs.\n
///       Default behavior is application will not wait is sStoppingState. So, it's not mandatory to use it.\n
/// \~english @par Classification
///        Public
/// \~english @par Type
///        none
/// \~english @see FrameworkunifiedHSMEnableWaitInStoppingState
VOID FrameworkunifiedHSMDisableWaitInStoppingState();

#endif  // __NATIVESERVICES_FRAMEWORK_FRAMEWORKUNIFIED_SM_DISPATCHER_H__  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
