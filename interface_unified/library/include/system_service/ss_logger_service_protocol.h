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
 * @file ss_logger_service_protocol.h
 * @brief \~english This file supports the the Logger Service.
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
#ifndef _SS_LOGGER_SERVICE_PROTOCOL_H_  // NOLINT (build/header_guard)
#define _SS_LOGGER_SERVICE_PROTOCOL_H_

#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_service_protocol.h>


#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  logger_service
/// \brief    Logger service event IDs
///////////////////////////////////////////////////////////////////////////////
typedef enum _LoggerServiceProtocol {
  SS_LOGGER_MILEAGE_DATA = 0x60,
  SS_LOGGER_SET_PARAMS,
  SS_LOGGER_STORE_SCREENCAPTURE_AND_LOG,                          // CLR+1+3
  SS_LOGGER_STORE_SCREENCAPTURE,                                  // CLR+1+2
  SS_LOGGER_STORE_LOG,                                            // ONHOOK+7+9
  SS_LOGGER_SCREENCAPTURE_EVT_ACK,                                // Screen shot response (filename)
  SS_LOGGER_EVENT_COMMONINFO,
  SS_LOGGER__CWORD56__EVENT_INFO,
  SS_LOGGER__CWORD56__RESET_INFO,
  SS_LOGGER_SET_DATETIME,
  SS_LOGGER_SET_VIN,
  SS_LOGGER_UDP_LOGGING,
  SS_LOGGER_ERROR_EVENT_ARTIFACT_REQ,
  SS_LOGGER_ERROR_EVENT_ARTIFACT_RESP,
  SS_LOGGER_SHUTDOWN_COMPLETE,
  SS_LOGGER_DIAGDTC_ACTIVE,
  SS_LOGGER_SET_DIAGID,
  // Timer protocol IDs
  SS_LOGGER_ERROR_EVENT_TIMER_ID_LOGGING_START_RSPN,
  SS_LOGGER_ERROR_EVENT_TIMER_ID_ARTIFACT_RESPONSE,
  SS_LOGGER_ERROR_EVENT_TIMER_ID_SCREEN_CAPTURE_RSPN,
}SS_LoggerServiceProtocol;

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  logger_service
/// \brief    Logger service event IDs
///////////////////////////////////////////////////////////////////////////////
typedef enum _SS_LOGGERSERVICEPROTOCOL {
  SS_LOGGERCOPYEVENTUSB =  0x80,                   // HMI to Dispatcher
  SS_LOGGERCOPYEVENTUSB_SUCCESS_RESP,
  SS_LOGGERCOPYEVENTUSB_ERROR_RESP,
  SS_LOGGERCOPYEMERGENCYLOGS,
  SS_LOGGERCOPYEMERGENCYLOGS_SUCCESS_RESP,
  SS_LOGGERCOPYEMERGENCYLOGS_ERROR_RESP,
  SS_LOGGERCLEAREVENT,                          // HMI to Dispatcher
  SS_LOGGERCLEAREVENT_SUCCESS_RESP,
  SS_LOGGERCLEAREVENT_ERROR_RESP,
  SS_LOGGER_READ_STATL_COUNTER,
  SS_LOGGER_READ_STATL_COUNTER_SUCCESS_RESP,
  SS_LOGGER_READ_STATL_COUNTER_ERROR_RESP,
  SS_LOGGER_RESET_STATL_COUNTER,
  SS_LOGGER_RESET_STATL_COUNTER_SUCCESS_RESP,
  SS_LOGGER_RESET_STATL_COUNTER_ERROR_RESP,
  SS_LOGGER_ENG_READ_NUMOFEVENTS,
  SS_LOGGER_ENG_READ_NUMOFEVENTS_RESP,
  SS_LOGGER_ENG_READ_STATISTICAL_COUNTERS,
  SS_LOGGER_ENG_READ_STATISTICAL_COUNTERS_RESP,
  SS_LOGGER_UPLOAD_EVENTLOG,
  SS_LOGGER_UPLOAD_EVENTLOG_RESP
}SS_loggerserviceprotocol;

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  logger_service
/// \brief    Private events for the following categories of devices
///////////////////////////////////////////////////////////////////////////////
typedef enum _LoggerServerEvents {
  SS_LOGGER_SCREENCAPTURE_EVT = 0xA0,                             // Screen shot request from PLM.
  SS_LOGGER_ERRORINFO_EVT,
  SS_LOGGER_LOGINFO_EVT,
  SS_LOGGER_LOGSTARTED_EVT
}SS_LoggerServerEvents;

/////////////////////////////////////////////////////////////////////////////////////
/// \brief   eSSLoggerCANProtocolID
///
/// \note    These IDs depict the possible events coming from CAN to logger_service
///
/////////////////////////////////////////////////////////////////////////////////////
enum eSSLoggerCANProtocolID {
    eSSLoggerCANProtocolIDCANTrigger = 0x10000000,  /// Start logging to USB
    eSSLoggerCANProtocolIDDTCTrigger                /// Start logging of DTC to emmc
};

/////////////////////////////////////////////////////////////////////////////////////
/// \brief   eSSLoggerCANEvent
///
/// \note    Events that are published to CAN for HK and CAN logging. Data structures
///          attached to the single events can be found in below table
///          | EventId                   | Return type               |
///          | :------------------------ | :------------------------ |
///          | eSSLoggerCANEventStart    | None                      |
///          | eSSLoggerCANEventError    | STEventCANLoggerEventInfo |
///          | eSSLoggerCANEventFinished | STEventCANLoggerEventInfo |
///
/////////////////////////////////////////////////////////////////////////////////////
enum eSSLoggerCANEvent {
    eSSLoggerCANEventStart = 0x10001000,  /// Logging has started for HK and CAN events
    eSSLoggerCANEventError,               /// Logging has aborted with an error
    eSSLoggerCANEventFinished             /// Logging has finished succesfully
};

#ifdef __cplusplus
}
#endif

#endif /* _SS_DEVICEDETECTION_SERVICE_PROTOCOL_H_ */  // NOLINT (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
