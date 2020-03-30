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

/**
 * @file ss_system_if.h
 * @brief \~english This file contains api build the session of heartbeat observation system manager
 */
/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup system_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup interface_unified
 *  @ingroup system_service
 *  @{
 */
#ifndef SS_SYSTEM_IF_H_  // NOLINT (build/header_guard)
#define SS_SYSTEM_IF_H_

#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_application.h>
#include <native_service/frameworkunified_framework_types.h>
#include <native_service/frameworkunified_sm_framework_if.h>

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup system_manager
/// \~english @par Summary:
///       Establish SystemManger and HeartBeat monitoring session
/// \~english @param [in] hApp
///                   HANDLE - Handle for application
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusNullPointer Handle error
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue error
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF File descriptor error
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR System call error
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Message buffer error
/// \~english @retval eFrameworkunifiedStatusErrOther Session establishment failure
/// \~english @retval eFrameworkunifiedStatusFail Message sharing memory error

/// \~english @par Preconditions:
///       - Dispather for application is created / initialized(FrameworkunifiedCreateDispatcherWithoutLoop etc.)
/// \~english @par Change of the internal state:
///       - No change of internal state by this API occurs
/// \~english @par Causes of failures:
///       - Handle(hApp) specified in the argument is not appropriate(invalid) [eFrameworkunifiedStatusNullPointer]
///       - Management information area acquisition(malloc) failed [eFrameworkunifiedStatusNullPointer]
///       - Message queue for session for service is full [eFrameworkunifiedStatusMsgQFull]
///       - Invalid file descriptor for sending session messages for service [eFrameworkunifiedStatusErrNoEBADF]
///       - System call(signal) interrupt occurs
///         while sending a session message for a service [eFrameworkunifiedStatusErrNoEINTR]
///       - The size of the message transmission buffer for service is incorrect [eFrameworkunifiedStatusInvldBufSize]
///       - Some error occurred when sending session message for service [eFrameworkunifiedStatusFail]
///       - Failed to access shared memory for sending session messages for service [eFrameworkunifiedStatusErrOther]
/// \~english @par Detail:
///       Establish HeartBeat monitoring and SystemManager session.\n
///
/// \~english @par Classification:
///          Public
/// \~english @see   FrameworkunifiedMcOpenSender, FrameworkunifiedAttachCallbacksToDispatcher, FrameworkunifiedSendMsg
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSSFrameworkInterface(HANDLE hApp);

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(IMPL_AGL_APPLICATION_CALLBACKS_PRE_BACKGROUND)
#define FRAMEWORKUNIFIED_MAKE_DEFAULT_CALLBACK(x) \
  x.onInitilization = FrameworkunifiedOnInitialization; \
  x.onDestroy = FrameworkunifiedOnDestroy; \
  x.onStart = FrameworkunifiedOnStart; \
  x.onStop  = FrameworkunifiedOnStop; \
  x.onDebugDump = FrameworkunifiedOnDebugDump; \
  x.createStateMachine = FrameworkunifiedCreateStateMachine; \
  x.ssFrameworkInterface = FrameworkunifiedSSFrameworkInterface; \
  x.onPreStart = FrameworkunifiedOnDummy; \
  x.onPreStop = FrameworkunifiedOnDummy; \
  x.onBackgroundStart = FrameworkunifiedOnDummy; \
  x.onBackgroundStop = FrameworkunifiedOnDummy; 
#else
#define FRAMEWORKUNIFIED_MAKE_DEFAULT_CALLBACK(x) \
  x.onInitilization = FrameworkunifiedOnInitialization; \
  x.onDestroy = FrameworkunifiedOnDestroy; \
  x.onStart = FrameworkunifiedOnStart; \
  x.onStop  = FrameworkunifiedOnStop; \
  x.onDebugDump = FrameworkunifiedOnDebugDump; \
  x.createStateMachine = FrameworkunifiedCreateStateMachine; \
  x.ssFrameworkInterface = FrameworkunifiedSSFrameworkInterface; \
  x.onPreStart = FrameworkunifiedOnPreStart; \
  x.onPreStop = FrameworkunifiedOnPreStop; \
  x.onBackgroundStart = FrameworkunifiedOnBackgroundStart; \
  x.onBackgroundStop = FrameworkunifiedOnBackgroundStop; 
#endif

#ifdef __cplusplus
}
#endif


#endif /* SS_SYSTEM_IF_H_ */  // NOLINT (build/header_guard)
/** @}*/    // end of interface_unified
/** @}*/    // end of system_service
/** @}*/    // end of BaseSystem
