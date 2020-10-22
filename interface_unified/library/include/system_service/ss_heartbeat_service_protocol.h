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

//////////////////////////////////////////////////////////////////////////////////////////////////
/// Handle HeartBeat Module Start and Stop notifications.
//////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @file ss_heartbeat_service_protocol.h
 * @brief  API Header for HeartBeat messages used by senders and receivers.
 *
 * Declares the external Protocol messages to the HeartBeat.
 */

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup system_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup system_manager
 *  @ingroup system_service
 *  @{
 */

#ifndef SS_HEARTBEAT_SERVICE_PROTOCOL_H_  // NOLINT (build/header_guard)
#define SS_HEARTBEAT_SERVICE_PROTOCOL_H_

#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_service_protocol.h>


/// \brief HeatBeat Protocol Command
/// heartbeat service protocol --> define all protocol messages in and out of heart beat thread
/// that are pertinent to the functionality offered by heart beat thread.
typedef enum T_SMHBProtocolMessages {
  // Diagnostic Protocol
  SS_HEARTBEAT_PRINT_CONNECTIONS           = 0x32,  ///< client to hb thread
  SS_HEARTBEAT_PRINT_STACK                 = 0x33,  ///< client to hb thread
  SS_HEARTBEAT_PERIODIC_STATUS_REQ         = 0xD0,  ///< sysmgr to hb thread
  SS_HEARTBEAT_PERIODIC_RESP               = 0xD1,  ///< hb thread to sysmgr
  SS_HEARTBEAT_START                       = 0xD2,  ///< sysmgr to hb thread
  SS_HEARTBEAT_STOP                        = 0xD3,  ///< sysmgr to hb thread
  SS_HEARTBEAT_DELETE_MODULE_ENTRY         = 0xD5,  ///< sysmgr to hb thread
  SS_HEARTBEAT_MSG_CATEGORY_REPORT         = 0xD6,  ///< report message
  SS_HEARTBEAT_ERROR_DETECTED              = 0xD8,  ///< hb thread to sysmgr
  SS_HEARTBEAT_APPEND_MODULE_ENTRY         = 0xD9,  ///< sysmgr to hb thread
  SS_HEARTBEAT_AVAIL_CHECK_REQ             = 0xDA,  ///< sysmgr to hb thread
  SS_HEARTBEAT_AVAIL_CHECK_RESP            = 0xDB,  ///< sysmgr to hb thread
  SS_HEARTBEAT_RESPONSE                    = 0xE0,  ///< client to hb thread
  SS_HEARTBEAT_REQUEST                     = 0xE1   ///< hb thread to client
}EHBProtocolMessages;

#endif /* SS_HEARTBEAT_SERVICE_PROTOCOL_H_ */  // NOLINT (build/header_guard)

/** @}*/
/** @}*/
/** @}*/
