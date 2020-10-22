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
/// \ingroup
/// \brief    Application framework's core structures and data types.
///
///
///
///////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file frameworkunified_framework_types.h
 * @brief \~english Application framework's core structures and data types.
 *
 */
/** @addtogroup BaseSystem
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
#ifndef __NATIVESERVICES_FRAMEWORK_FRAMEWORKUNIFIED_FRAMEWORK_TYPES_H__  // NOLINT  (build/header_guard)
#define __NATIVESERVICES_FRAMEWORK_FRAMEWORKUNIFIED_FRAMEWORK_TYPES_H__

#include <native_service/frameworkunified_types.h>
#include <boost/function.hpp>

#define MAX_LEN      MAX_QUEUE_NAME_SIZE
#define MAX_MSGQ_BUFFER  MAX_QUEUE_MSG_SIZE
#define MAX_SESSION_ID_VAL 0xFFFF
#define MAX_FD_MULTIWAITING 8

#define FRAMEWORKUNIFIED_ANY_SOURCE   "NS_ANY_SRC"
#define FRAMEWORKUNIFIED_ANY_COMMAND  0xFFFFFFFF
#define FRAMEWORKUNIFIED_NS_NPSERVICE "NS_NPPService"
#define FRAMEWORKUNIFIED_NS_VERSIONUTIL "nsverutil"
#define FRAMEWORKUNIFIED_NS_MSGPROFILERUTIL "NSDisMsgProfile"
#define FRAMEWORKUNIFIED_NS_REMOTECONTROLSERVICE "NS_RemoteControlService"

// call back function definition
typedef boost::function< EFrameworkunifiedStatus(HANDLE) > TCbFunction;
typedef TCbFunction CallbackFunctionPtr;
typedef TCbFunction CbFuncPtr;
typedef EFrameworkunifiedStatus(*CbArgumentParser)(SI_32 argument, PCHAR argumentValue);

typedef enum _EFrameworkunifiedServiceAvailableStatus {
  eFrameworkunifiedServiceAvailable = 0,
  eFrameworkunifiedServiceNotAvailable,
} EFrameworkunifiedServiceAvailableStatus;

// Share Memory Retrieve Types
typedef enum _ESMRetrieveTypes {
  eSMRRelease = 0,  ///< data will be released on a read
  eSMRRetain = 1,  ///< data will be retained in the shared memory
} ESMRetrieveTypes;

/**
 * \~english Callbacks structure of Application Life cycle
 */
typedef struct _FrameworkunifiedDefaultCallbackHandler {
  CbFuncPtr onInitilization;  //!< \~english Callback of initialization
  CbFuncPtr onDestroy;  //!< \~english Callback of destroy
  CbFuncPtr onStart;  //!< \~english Callback of startup
  CbFuncPtr onStop;  //!< \~english Callback of stop
  CbFuncPtr onPreStart;  //!< \~english Callback of pre start
  CbFuncPtr onPreStop;  //!< \~english Callback of pre stop
  CbFuncPtr onBackgroundStart;  //!< \~english Callback of Background start
  CbFuncPtr onBackgroundStop;  //!< \~english Callback of Background stop
  CbFuncPtr onDebugDump;  //!< \~english Callback of debug dump
  CbFuncPtr createStateMachine;  //!< \~english Callback of created state machine
  CbFuncPtr ssFrameworkInterface;  //!< \~english Callback to Establish a session with the system_manager
} FrameworkunifiedDefaultCallbackHandler;

// LCOV_EXCL_BR_START 11:Occurred in the above test
typedef struct _FrameworkunifiedProtocolCallbackHandler {
  UI_32 iCmd;
  CbFuncPtr callBack;
} FrameworkunifiedProtocolCallbackHandler;
// LCOV_EXCL_BR_STOP

typedef struct _FrameworkunifiedFdProtocolCallbackHandler {
  int fd;
  CbFuncPtr callBack;
} FrameworkunifiedFdProtocolCallbackHandler;

// LCOV_EXCL_BR_START 11:Occurred in the above test
typedef struct _FrameworkunifiedNotificationCallbackHandler {
  CHAR cNotification[MAX_STRING_SIZE_NOTIFICATION];
  CbFuncPtr callBack;
} FrameworkunifiedNotificationCallbackHandler;
// LCOV_EXCL_BR_STOP

typedef struct _FrameworkunifiedNotificationsList {
  CHAR cNotification[MAX_STRING_SIZE_NOTIFICATION];
  UI_32 uiLengthData;
  EFrameworkunifiedNotificationType  persType;
} FrameworkunifiedNotificationsList;

typedef struct _FrameworkunifiedProtocolEvent {
  UI_32 iCmd;
  UI_32 iEventId;
} FrameworkunifiedProtocolEvent;

typedef struct _FrameworkunifiedNotificationEvent {
  CHAR cNotification[MAX_STRING_SIZE_NOTIFICATION];
  UI_32 iEventId;
} FrameworkunifiedNotificationEvent;

typedef struct _ServiceAvailability {
  CHAR cServiceName[MAX_NAME_SIZE_APP];
  EFrameworkunifiedServiceAvailableStatus eServiceAvailability;
} ServiceAvailability;

typedef enum _EFrameworkunifiedShutdowntatus {
  eFrameworkunifiedShutdownOk = 0,
  eFrameworkunifiedShutdownTimeout,
  eFrameworkunifiedShutdownFail,
} EFrameworkunifiedShutdownStatus;

typedef struct _ShutdownComplete {
  CHAR cServiceName[MAX_NAME_SIZE_APP];
  EFrameworkunifiedShutdownStatus eShutdownCompleteStatus;
} ShutdownComplete;

typedef struct _CustomCommandLineOptions {
  PCSTR cShortOptions;
  PCHAR cLongOptions;  // reserved for long options.
  CbArgumentParser callback;
} CustomCommandLineOptions;

/// Scheduling Policy
typedef enum _EFrameworkunifiedSchedPolicy {
  eFrameworkunifiedSchedPolicyInherit = 0,
  eFrameworkunifiedSchedPolicyTSS,
  eFrameworkunifiedSchedPolicyFIFO,
  eFrameworkunifiedSchedPolicyRR,
  eFrameworkunifiedSchedPolicyMAX
} EFrameworkunifiedSchedPolicy;

/// Child Thread Attribute
typedef struct _FrameworkunifiedChildThreadAttr {
  EFrameworkunifiedSchedPolicy schedPolicy;
  SI_32 schedPriority;
} FrameworkunifiedChildThreadAttr;

#endif /* __NATIVESERVICES_FRAMEWORK_FRAMEWORKUNIFIED_FRAMEWORK_TYPES_H__ */  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
