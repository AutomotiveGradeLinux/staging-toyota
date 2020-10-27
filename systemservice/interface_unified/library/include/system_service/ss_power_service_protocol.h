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

/**
 * @file ss_power_service_protocol.h
 * @brief \~english This file supports the Power Service interface.
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
#ifndef POWER_SERVICE_CLIENT_INCLUDE_SYSTEM_SERVICE_SS_POWER_SERVICE_PROTOCOL_H_  // NOLINT (build/header_guard)
#define POWER_SERVICE_CLIENT_INCLUDE_SYSTEM_SERVICE_SS_POWER_SERVICE_PROTOCOL_H_


#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_service_protocol.h>
#include "system_service/ss_power_service.h"

/// \ingroup PSM_Local
/**
 * power service protocol --> define all protocol messages in and out of power that are pertinent to the functionality offered by power.
 */
typedef enum _SS_PowerServiceProtocol {
  // Diagnostic Protocol
  SS_POWER_PRINT_CONNECTIONS        = 0x32,   //!< client to service
  SS_POWER_PRINT_STACK              = 0x33,   //!< client to service

  SS_POWER_WAKEUP_COMPLETE          = 0x41,   //!< service to client  // used by power state machine
  SS_POWER_SHUTDOWN_COMPLETE        = 0x42,   //!< service to client  // used by power state machine
  SS_POWER_VOLTAGE_STATE            = 0x43,   //!< client to service
  SS_POWER_LVI2_SHUTDOWN_COMPLETE   = 0x44,   //!< service to client
  SS_POWER_CRANK_STATE              = 0x45,   //!< client to service


  // System Manager commands
  SS_POWER_WAKEUP_MODULES_CMPL_RSPN = 0x51,   //!< service to shadow
  SS_POWER_SYSTEM_LAUNCH_COMPLETE   = 0x56,   //!< client to service


  // SSM Service commands
  SS_POWER_STATE_CHANGE_REQ         = 0x40,   //!< client to service : send wakeup to System Manager
  SS_POWER_STATE_CHANGE_RESP        = 0x46,   //!< \~english Response to request of power state transition to power_supply_manager_shadow.

  SS_POWER_COMM_WAKEUP              = 0x58,   //!< client to service : Power On expected

  SS_POWER_SHUTDOWN_REQ             = 0x59,   //!< client to service : Comm Sleep from Shadow
  SS_POWER_SHUTDOWN_RESP            = 0x57,   //!< service to shadow : Shutdown complete from System Manager

  SS_POWER_CRNT_STATE_QUERY         = 0x60,   //!< client to service
  SS_POWER_CRNT_STATE_QUERY_RSPN    = 0x61,   //!< service to client

  SS_POWER_SYSTEM_MODE_INFO_REQ     = 0x63,   //!< Client (Shadow) to Service (Power Service)
  SS_POWER_SYSTEM_MODE_INFO_RESP    = 0x64,   //!< Service to client *****

  SS_POWER_INITCOMP_REP             = 0x70,   //!< Client (Shadow) to Service (Power Service)

  //*************************************************************************
  // CPMShowPowerPopup Notification Protocol:                               *
  //                                                                        *
  // Request by the Power Shadow to the Power Service to publish a          *
  // 'Voltage State' notification.                                          *
  //                                                                        *
  // The Request message carries the requested notification.                *
  // The Response message carries the EFrameworkunifiedStatus of the request.            *
  SS_POWER_PUBLISH_POWER_POPUP_REQ,           //!< Shadow to Pwr Svc
  SS_POWER_PUBLISH_POWER_POPUP_RESP,          //!< Pwr Svc to Shadow
  //                                                                        *
  // End of CPMShowPowerPopup Notification Protocol                         *
  //*************************************************************************

  //*************************************************************************
  // Shutdown Condition Notification Protocol:                              *
  //                                                                        *
  // Request by the Power Shadow to the Power Service to publish the        *
  // 'Shutdown Condition' notification.                                     *
  //                                                                        *
  // The Request message carries the requested PowerPopup.                  *
  // The Response message carries the EFrameworkunifiedStatus of the request.            *
  SS_POWER_PUBLISH_SHUTDOWN_CONDITION_REQ,    //!< Shadow to Pwr Svc
  SS_POWER_PUBLISH_SHUTDOWN_CONDITION_RESP,   //!< Pwr Svc to Shadow
  //                                                                        *
  // End of Shutdown Condition Notification Protocol                        *
  //*************************************************************************

  //*************************************************************************
  // Startup Confirmation Protocol:                                         *
  //                                                                        *
  // Request by the Power Shadow to the Power Service to forward to the     *
  // System Manager the Startup Confirmation message.                       *
  //                                                                        *
  // The Request message carries the Startup Confirmation message.          *
  // The Response message carries the EFrameworkunifiedStatus of the request.            *
  SS_POWER_FWD_START_CONFIRMATION_MSG_REQ,    //!< Shadow to Pwr Svc
  SS_POWER_FWD_START_CONFIRMATION_MSG_RESP,   //!< Pwr Svc to Shadow
  //                                                                        *
  // End of Startup Confirmation Protocol                                   *
  //*************************************************************************

  //*************************************************************************
  // Power Request Protocol:                                                *
  //                                                                        *
  // Power Request message from the Power Shadow to the Power Service with  *
  // _CWORD56_'s "Power_Request" IPC_Command data.                               *
  //                                                                        *
  // The Request message carries the _CWORD56_'s data.                           *
  // The Response message will use the SS_POWER_STATE_CHANGE_RESP protocol  *
  // ID.                                                                    *
  SS_POWER_POWER_REQUEST_MSG,                 //!< Shadow to Pwr Svc
  //                                                                        *
  // End of Power Request Protocol                                          *
  //*************************************************************************

  //*************************************************************************
  // Shutdown Request Protocol:                                             *
  //                                                                        *
  // Shutdown Request message from the Power Shadow to the Power Service    *
  // with _CWORD56_'s "Shutdown_Request" IPC_Command data.                       *
  //                                                                        *
  // The Request message carries the _CWORD56_'s data.                           *
  // The Response message will use the SS_POWER_STATE_CHANGE_RESP protocol  *
  // ID.                                                                    *
  SS_POWER_SHUTDOWN_REQUEST_MSG,              //!< Shadow to Pwr Svc
  //                                                                        *
  // End of Shutdown Request Protocol                                       *
  //*************************************************************************

  //*************************************************************************
  // User Mode Protocol:                                                    *
  //                                                                        *
  // Request by the Power Service to the System Manager, originally from    *
  // the Power Shadow, to turn on or off the video & audio output of HMI    *
  // and Audio services.                                                    *
  // User Mode 'On'  => turn video & audio output on                        *
  // User Mode 'Off' => turn video & audio output off                       *
  //                                                                        *
  // The Request message carries the requested User Mode state.             *
  // The Response message carries the actual User Mode state.               *
  SS_POWER_USER_MODE_SET_RESP,                //!< Power Service to Shadow
  //                                                                        *
  // Note: See the corresponding Power Service / System Manager             *
  // 'SS_SystemManagerProtocol' enums in /SystemServices/inc/               *
  // ss_system_manager_protocol.h                                           *
  //                                                                        *
  // End of User Mode Protocol                                              *
  //*************************************************************************

  //*************************************************************************
  // Heartbeat Protocol:                                                    *
  //                                                                        *
  // Request by the Power Service to the System Manager, originally from    *
  // the Power Shadow, to support the heartbeat request response system.    *
  //                                                                        *
  // The Request message carries the expected application processor         *
  // heartbeat response interval in units of seconds.                       *
  // The Response message has a data length of zero and serves as a         *
  // periodic heartbeat message to the boot micro.                          *
  SS_POWER_HEARTBEAT_REQ,                     //!< Shadow to Power Service
  SS_POWER_HEARTBEAT_RESP,                    //!< Power Service to Shadow
  //                                                                        *
  // End of Heartbeat Protocol                                              *
  //*************************************************************************

  //*************************************************************************
  // Hard Reset Request Protocol :                                          *
  //                                                                        *
  // Request by the Power Service to the Shadow to support the sending of   *
  // a reset request with type argument to the boot micro.                  *
  //                                                                        *
  // The Request message carries epsCpuResetReason as data payload.         *
  SS_POWER_HARD_RESET_REQ,                    //!< Power Service to Shadow
  //                                                                        *
  // End of Hard Reset Request Protocol                                     *
  //*************************************************************************
}SS_PowerServiceProtocol;

#endif /* POWER_SERVICE_CLIENT_INCLUDE_SYSTEM_SERVICE_SS_POWER_SERVICE_PROTOCOL_H_ */  // NOLINT (build/header_guard)
/** @}*/  // end of interface_unified
/** @}*/  // end of system_service
/** @}*/  // end of BaseSystem
// EOF
