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
 * @file ss_logger_service_local.h
 * @brief \~english This file supports the the Logger Service.
 */
/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup system_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup logger_service
 *  @ingroup system_service
 *  @{
 */
#ifndef SS_LOGGER_SERVICE_LOCAL_H_  // NOLINT (build/header_guard)
#define SS_LOGGER_SERVICE_LOCAL_H_

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  logger_service
/// \brief    Screen Capture Event data
///////////////////////////////////////////////////////////////////////////////
typedef struct _TInterfaceunifiedSetLoggerParams {
  ELOGGER_STAT Logger_State;
  EDEV_TYPE Device_Type;
}STLoggerSetParams;

typedef struct _TInterfaceunifiedSetLoggerAllParams {
  ELOGGER_STAT   Logger_State;
  EDEV_TYPE     Device_Type;
  ELOGGER_STAT   Logger_UDPState;
  UI_32         Log_FolderCnt;
}STLoggerSetAllParams;

typedef struct _TErrorLogger_FolderInfo {
  static const UI_16  STR_BUFF_LEN = 64;
  CHAR                FoldernameAndLogname[STR_BUFF_LEN];
  CHAR                StorageTarget[STR_BUFF_LEN];
}STLoggerFolderInfo;

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  logger_service
/// \brief    Event logger resets counter information
///////////////////////////////////////////////////////////////////////////////
typedef union _uEvtLoggerCommonInfo {
  STEventLoggerCommonInfo u_stEvtLoggerCommonInfo;
  UI_8 CommonData[4];
}UEvtLoggerCommonInfo;

#pragma pack(1)
///////////////////////////////////////////////////////////////////////////////
/// \ingroup  logger_service
/// \brief    Logger Event information
///////////////////////////////////////////////////////////////////////////////
typedef struct _stLogEventss_ {
  UI_32 ts;
  UI_8 grp_ID;
  UI_8 event_id;
  UI_8 data[4];
}st_LogEvent_ss;
#pragma pack(0)

/// Macros definition for the Event Logs
#define MAX_EVT_RECORDS    20000
#define MAX_EVENTLOG_SIZE 10*MAX_EVT_RECORDS      ///< Max size for event logging

typedef struct _TInterfaceunifiedEventLogPersistBuffer {
  static const UI_16    EVT_BUFMAXSIZE = MAX_EVT_RECORDS;
  st_LogEvent_ss      EvtLog_Buffer[EVT_BUFMAXSIZE];
  UI_16          Current_Log_Size;
}STEventLogPersistBuffer;

#endif /* SS_LOGGER_SERVICE_LOCAL_H_ */  // NOLINT (build/header_guard)
/** @}*/
/** @}*/
/** @}*/


