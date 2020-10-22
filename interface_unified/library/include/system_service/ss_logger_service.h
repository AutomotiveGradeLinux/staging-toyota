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
 * @file ss_logger_service.h
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
#ifndef SS_LOGGER_SERVICE_H_  // NOLINT (build/header_guard)
#define SS_LOGGER_SERVICE_H_


#include <native_service/frameworkunified_types.h>

#define MAX_STATISTICAL_BUFFER 240

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  logger_service
/// \brief    Shared Memory names
///////////////////////////////////////////////////////////////////////////////
#define SHMEM_DRLOCATIONLOG      "/DRLocationLog"

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  logger_service
/// \brief    Shared Memory names
///////////////////////////////////////////////////////////////////////////////
#define SHMEM_DRINITIALLOG      "/DRInitialLog"

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  logger_service
/// \brief    VIN numbers
///////////////////////////////////////////////////////////////////////////////
typedef struct _TVINnumber {
  static const UI_8 VIN_LEN = 18;
  CHAR        VINstr[VIN_LEN];
} STVIN_NUMBER;

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  logger_service
/// \brief    CAN Diagnostic status data
///////////////////////////////////////////////////////////////////////////////
typedef struct _TInterfaceunifiedCANMileageInfo {
  UI_8  DidA_ExtTest_Pres;
  UI_8  EngRun_Stat;
  UI_8  Odo_MSB_H;
  UI_8  Odo_MSB_L;
  UI_8  Odo_LSB_H;
  UI_8  Odo_LSB_L;
  UI_8  PN14_SupBat_Volt;
} STLOGGER_CANDIAGSTAT;

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  logger_service
/// \brief    CAN current date and time
///////////////////////////////////////////////////////////////////////////////
/**
* @struct __CWORD62_DateAndTime
* @brief \~english CAN current date and time
*/
typedef struct __CWORD62_DateAndTime {
  UI_8 DateTime_Stat; /*!< \~english Date time status*/
  UI_8 DateTimeDay; /*!< \~english Date*/
  UI_8 DateTimeHour; /*!< \~english Hour*/
  UI_8 DateTimeMinute; /*!< \~english Minute*/
  UI_8 DateTimeMonth; /*!< \~english Month*/
  UI_8 DateTimeSecond; /*!< \~english Second*/
  UI_8 DateTimeYear; /*!< \~english Year*/
  UI_8 TimeFormat; /*!< \~english Format*/
}STCanCurrentDateTime;

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  logger_service
/// \brief    Logger Device Type
///////////////////////////////////////////////////////////////////////////////
typedef enum _ELoggerDeviceTypes {
  eDevUSB1,
  eDevUSB2,
  eDevSD,
  eTotalDevicesTypes,
  eInvalid_DevType
}EDEV_TYPE;

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  logger_service
/// \brief    Logger State
///////////////////////////////////////////////////////////////////////////////
typedef enum _ELoggerState {
  eDeactivate,
  eActivate,
  eInvalid_LoggerState
}ELOGGER_STAT;

typedef enum _ELoggerErrorCodes {
  eSelectedDeviceNotFound = 0x0001,
  eWriteToDeviceFailed  = 0x0002,
  eScreenCaptureFailed  = 0x0003,
  eScreenCaptureSaveTimeExpired = 0x0004,
  eScreenCaptureStoreTimeExpired = 0x0005,
  eNoLogToStore           = 0x0006,
}ELOGGERERR_CODES;

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  logger_service
/// \brief    Screen Capture Event data
///////////////////////////////////////////////////////////////////////////////
typedef struct _TInterfaceunifiedScreenCaptureEvt {
  static const UI_16              STR_BUFF_LEN = 64;
  BOOL                            fSucessful;
  UI_32                           uiSceenShotId;
  CHAR                            strNameAndLocation[STR_BUFF_LEN];
  UI_32                           uiFaultReasonCode;
}STScreenCaptureEvt;

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  logger_service
/// \brief    USB source number to store event logs.
///////////////////////////////////////////////////////////////////////////////
typedef enum _EEventLoggerUSBDeviceNumber {
  USB0 = 0,
  USB1 = 1,
  SD = 2
}EEvtLogUSBDevNumber;

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  logger_service
/// \brief    USB source number to store emergency error logs.
///////////////////////////////////////////////////////////////////////////////
typedef enum _EDevNumber {
  eEEL_USB1 = 1,
  eEEL_USB2 = 2,
  eEEL_SD = 3
}EDevNumber;

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  logger_service
/// \brief    Counter group ID.
///////////////////////////////////////////////////////////////////////////////
typedef enum _ECounterGroupID {
  STARTUP_SHUTDOWN_COUNTER_GROUP = 0x01,
  APP_USAGE_COUNTER_GROUP,
  F_BLK_STABILITY_COUNTER_GROUP,
  MAX_COUNTER_GROUP
}EStatCounterGroupID;

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  logger_service
/// \brief    Statistical counter.
///////////////////////////////////////////////////////////////////////////////
typedef struct _SStatisticalCounter {
  UI_16 u16StartupCounterLength;        ///< No of Counter from startup phase
  UI_16 u16NormalCounterLength;        ///< No of Counter from Normal phase
  UI_16 u16ShutDownCounterLength;      ///< No of Counter from Shut down phase
  UI_8 StatisticalCountBuffer[MAX_STATISTICAL_BUFFER];  ///< Counter values for startup,normal and shutsown phases
}SStatisticalCounter;

typedef enum _EEventLoggerSuccessCode {
  COPY_EVT_USB_SUCCESS = 0x20,
  CLEAR_EVENT_LOG_SUCCESS = 0x21,
  STATISTICAL_COUNTER_READ_SUCCESS = 0x22
}EEvtLoggerSuccessCode;

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  logger_service
/// \brief    Event Logger error code
///////////////////////////////////////////////////////////////////////////////
typedef enum _EEventLoggerErrorCode {
  USB_DEVICE_NOT_AVAILABLE = 0x10,
  USB_DEVICE_WRITE_ERROR = 0x11,
  CLEAR_EVENT_LOG_FAILED = 0x12,
  STATISTICAL_COUNTER_READ_FAILED = 0x13,
  NO_ERROR_INFO = 0x14
}EEvtLoggerErrorCode;

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  logger_service
/// \brief    Emergency Log error code
///////////////////////////////////////////////////////////////////////////////
typedef enum _EEmergencyLogErrorCode {
  eDEVICE_NOT_AVAILABLE = 0x10,
  eDEVICE_WRITE_ERROR = 0x11,
  eNO_ERROR_INFO = 0x12
}EELL_ErrorCode;

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  logger_service
/// \brief    Event logger common information
///////////////////////////////////////////////////////////////////////////////
typedef struct _SEventLoggerCommonInfo {
  UI_8 BodyCAN_Stat:4;
  UI_8 HeadUnitCAN_Stat:4;
  UI_8 HMIInteraction:4;
  UI_8 IGN_Status:4;
  UI_8 FOT_Temp:2;
  UI_8 SystemVoltage:6;
}STEventLoggerCommonInfo;

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  logger_service
/// \brief    Event logger resets counter information
///////////////////////////////////////////////////////////////////////////////
typedef struct _SEventLoggerResetInfo {
  UI_8 _CWORD56__ResetInfo;
  UI_8 _CWORD102__ResetInfo;
}STEventLoggerResetInfo;

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  logger_service
/// \brief    Event logger _CWORD56_ events information
///////////////////////////////////////////////////////////////////////////////
typedef struct _SEventLoggerEventInfo {
  UI_8 NumberOfEvents;
  UI_8 EventGroup;
  UI_8 EventIdentifier;
  UI_8 EventData[4];
}STEventLoggerEventInfo;

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  logger_service
/// \brief    Event logger CAN events information
///////////////////////////////////////////////////////////////////////////////
typedef struct _SEventCANLoggerEventInfo {
    BOOL                 success;
    BOOL                 valid;
    UI_32                triggerNumber;
    STCanCurrentDateTime dateAndTime;
}STEventCANLoggerEventInfo;

#endif /* SS_LOGGER_SERVICE_H_ */  // NOLINT (build/header_guard)
/** @}*/
/** @}*/
/** @}*/

