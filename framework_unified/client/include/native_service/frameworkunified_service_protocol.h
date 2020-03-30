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


//////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup tag_NativeServices
/// \brief
///
//////////////////////////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file frameworkunified_service_protocol.h
 * @brief \~english Include enums, structs and frameworkunified_types.h
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
#ifndef FRAMEWORKUNIFIED_SERVICE_PROTOCOL_H_  // NOLINT  (build/header_guard)
#define FRAMEWORKUNIFIED_SERVICE_PROTOCOL_H_

#include <native_service/frameworkunified_types.h>

#define NS_INVALID_SESSION 0xFFFF

typedef enum _EBaseProtocol {
  PROTOCOL_OPEN_SESSION_REQ      = 0x00,  ///< Open session request (client > service)
  PROTOCOL_OPEN_SESSION_ACK      = 0x01,  ///< Open session ack (service > client)
  PROTOCOL_OPEN_SESSION_REQ_SYNC = 0x02,  ///< Open session request sync (client > service)
  PROTOCOL_CLOSE_SESSION_REQ      = 0x03,  ///< Close session request (client > service)
  PROTOCOL_CLOSE_SESSION_ACK      = 0x04,  ///< Close session ack (service > client)
  PROTOCOL_CLOSE_SESSION_REQ_SYNC = 0x05,  ///< Close session request sync (client > service)

  PROTOCOL_REGISTER_EVENTS     = 0x06,  ///< Register for events (client > service)
  PROTOCOL_REGISTER_EVENTS_ACK = 0x07,  ///< Register for events ack (service > client)
  PROTOCOL_UNREGISTER_EVENTS   = 0x08,  ///< Un-Register for events (client > service)

  PROTOCOL_THREAD_INITIALIZATION = 0x09,  ///< Initialize thread after creation
  PROTOCOL_THREAD_WAKEUP = 0x0A,  ///< Wakeup Thread after sleep
  PROTOCOL_THREAD_SHUTDOWN = 0x0B,  ///< Stop the thread
  PROTOCOL_THREAD_DESTROY = 0x0C,  ///< Destroy the thread
  PROTOCOL_RSV_0D = 0x0D,  ///< Reserved
  PROTOCOL_RSV_0E = 0x0E,  ///< Reserved
  PROTOCOL_DIS_MSGPROFILER = 0x0F,  ///< Reserved

  PROTOCOL_FRAMEWORKUNIFIED_BASE_CMD = 0x10,        ///< Protocols between range PROTOCOL_FRAMEWORKUNIFIED_BASE_CMD to PROTOCOL_FRAMEWORKUNIFIED_MAX_CMD
  PROTOCOL_FRAMEWORKUNIFIED_MAX_CMD = 0xFFFFFFF0,  ///< are available for FRAMEWORKUNIFIED applications to use.

  PROTOCOL_RSV_xF1 = 0xFFFFFFF1,  ///< Reserved
  PROTOCOL_RSV_xF2 = 0xFFFFFFF2,  ///< Reserved
  PROTOCOL_RSV_xF3 = 0xFFFFFFF3,  ///< Reserved
  PROTOCOL_RSV_xF4 = 0xFFFFFFF4,  ///< Reserved
  PROTOCOL_RSV_xF5 = 0xFFFFFFF5,  ///< Reserved
  PROTOCOL_RSV_xF6 = 0xFFFFFFF6,  ///< Reserved
  PROTOCOL_RSV_xF7 = 0xFFFFFFF7,  ///< Reserved
  PROTOCOL_RSV_xF8 = 0xFFFFFFF8,  ///< Reserved
  PROTOCOL_RSV_xF9 = 0xFFFFFFF9,  ///< Reserved
  PROTOCOL_RSV_xFA = 0xFFFFFFFA,  ///< Reserved
  PROTOCOL_RSV_xFB = 0xFFFFFFFB,  ///< Reserved
  PROTOCOL_RSV_xFC = 0xFFFFFFFC,  ///< Reserved

  PROTOCOL_TIMER_DUMMY_CMD = 0xFFFFFFFD,  ///< NS internally use this command in timer

  PROTOCOL_FRAMEWORKUNIFIED_NOTIFICATION = 0xFFFFFFFE,
  PROTOCOL_FRAMEWORKUNIFIED_ANY_COMMAND  = 0xFFFFFFFF
} EBaseProtocol;

typedef struct _OpenSessionAck {
  EFrameworkunifiedStatus eStatus;
  UI_32 sessionId;
  CHAR  cSessionName[MAX_QUEUE_NAME_SIZE];
  UI_32 sessionType;
} OpenSessionAck;


typedef struct _CloseSessionReq {
  UI_32 sessionId;
  CHAR  cSessionName[MAX_QUEUE_NAME_SIZE];
} CloseSessionReq;

typedef struct _CloseSessionAck {
  UI_32 sessionId;
  EFrameworkunifiedStatus eStatus;
} CloseSessionAck;

#endif /* FRAMEWORKUNIFIED_SERVICE_PROTOCOL_H_ */  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
