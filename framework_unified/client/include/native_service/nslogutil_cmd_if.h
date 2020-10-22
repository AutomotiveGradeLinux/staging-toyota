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
/// \file
/// Declares COMMANDS related to Log Utility
///
//////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @file
 * @brief \~english Declares COMMANDS related to Log Utility
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
/** @addtogroup utility
 *  @ingroup framework_unified
 *  @{
 */

#ifndef COMMANDS_H_  // NOLINT  (build/header_guard)
#define COMMANDS_H_

// Notifications
#define NTFY_LOGGER_SETCONTROLMASK    "LoggerService/SetControlMask"

///////////////////////////////////////////////////////////////////////////////
/// \ingroup tag_NSLogger
/// \brief    Logger Utility Commands
///////////////////////////////////////////////////////////////////////////////
typedef enum _COMMANDS {
  SET_LOG_MASK = 0xAEA,     // < Sets the NS log mask for a process. uses CHANGELOGPARAMS
  GET_LOG_MASK,         // < Gets the NS log mask for a process. uses CHANGELOGPARAMS
  SET_LOG_OUT_OPT,        // < Sets the NS log output option mask for a process. uses CHANGELOGPARAMS
  GET_LOG_OUT_OPT,      // < Gets the NS log output option mask for a process. uses CHANGELOGPARAMS
  GET_LOG_MASK_RESPONSE,    // < Get the response about NS log masks
  GET_LOG_OUT_OPT_RESPONSE,  // < Get the response about NS log output options
  SET_MSGTX_RX_ON_AND_TX_OFF,  // < Sets TransmissionLogger to receive ON, transmit OFF
  SET_MSGTX_RX_OFF_AND_TX_ON,  // < Sets TransmissionLogger to receive OFF, transmit ON
  SET_MSGTX_RX_AND_TX_ON,   // < Sets TransmissionLogger to receive ON, transmit ON
  SET_MSGTX_RX_AND_TX_OFF,  // < Sets TransmissionLogger to receive OFF, transmit OFF
  kDebugDumpRequest,      // < Sets a debug dump log to the debug dump queue
  SET_LOG_SEVERITY,     // < Sets the NS severity log level
  GET_LOG_SEVERITY,     // < Gets the NS severity log level
  GET_LOG_SEVERITY_RESPONSE,  // < Get the response about NS serverity log level

  // list of protocols provided for rcs logger plugin
  NSRCS_SET_LOG_SETTINGS_REQ,   // < Set the NS RCS log setting request
  NSRCS_SET_LOG_SETTINGS_RESP,  // < Set the NS RCS log setting response
  NSRCS_GET_LOG_SETTINGS_REQ,   // < Get the NS RCS log setting request
  NSRCS_GET_LOG_SETTINGS_RESP,  // < Get the NS RCS log setting response
  NSRCS_SET_LOG_MASK_REQ,     // < Set the NS RCS log mask request
  NSRCS_SET_LOG_MASK_RESP,    // < Get the NS RCS log mask response
  NSRCS_SET_LOG_OUT_OPT_REQ,    // < Set the NS RCS log output option request
  NSRCS_SET_LOG_OUT_OPT_RESP,   // < Set the NS RCS log output option response
  NSRCS_SET_LOG_SEVERITY_REQ,   // < Set the NS RCS severity log level request
  NSRCS_SET_LOG_SEVERITY_RESP,  // < Set the NS RCS severity log level response
} COMMANDS;

#endif /* COMMANDS_H_ */  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
