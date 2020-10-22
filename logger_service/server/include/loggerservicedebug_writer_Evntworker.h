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
/// \ingroup  tag_NS_LogTraceSender
/// \brief    frameworkunifieddebug writer thread class, handles writing a log file
///           all really done by my_writer class.
///
///////////////////////////////////////////////////////////////////////////////
#ifndef LOGGER_SERVICE_SERVER_INCLUDE_LOGGERSERVICEDEBUG_WRITER_EVNTWORKER_H_
#define LOGGER_SERVICE_SERVER_INCLUDE_LOGGERSERVICEDEBUG_WRITER_EVNTWORKER_H_

#include <stdlib.h>
#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_multithreading.h>
#include <system_service/ss_devicedetection_service_ifc.h>
#include <native_service/ns_logger_if.h>
#include <native_service/ns_eventlogger.h>
#include <map>
#include <iostream>
#include <deque>
#include "loggerservicedebug_thread_if.h"


// File path names for Evnet and counter logs
extern const CHAR DEBUG_EVNTLOG_PATH_FN[];
extern const CHAR g_strEvntLogQueWorkerName[];
extern const CHAR DEBUG_LOG_PATH_FN[];
extern const CHAR Counter_LOG_PATH_FN[];
extern const CHAR Counter_LOG_PATH_STARTFN[];
extern const CHAR Counter_LOG_PATH_SHUTFN[];
extern const CHAR Mileage_LOG_PATH_FN[];

typedef struct _TevntWriterInfo {
  static const UI_32 FN_LEN = 256;
  UI_32 max_filelen;
  CHAR base_cnt_filename[FN_LEN];
  CHAR mileage_filename[FN_LEN];
} TEvntWriterInfo;

typedef enum _eDatatype_ {
  COMMON_DATA,
  EVENT_SPECIFIC_DATA
} EDatatype_SS;

typedef enum _eSystemPhase_ {
  STARTUP_SS = 0x21,
  NORMAL_SS = 0x22,
  SHUTDOWN_SS = 0x23
} ESystemPhase_SS;

#pragma pack(1)
typedef struct _stLogEventFull_ {
  UI_32 ts;
  UI_8 grp_ID;
  UI_8 event_id;
  UI_8 data[4];
  ESystemPhase_SS phase;
  UI_16 cnt_ID;
  EDatatype_SS typeofdata;
} st_LogEvent_full;
#pragma pack(0)

// Deque to hold the Event information
typedef std::deque<st_LogEvent_ss> DEQUE_Event_Type;

// Map to hold the Counter information on phase
typedef std::map<UI_16, UI_32> Counter;

/*Map to hold the all the counter information */
typedef std::map<EStatCounterGroupID, Counter> CounterInformation;

class CEvntWriterWorker {
 public:
  CEvntWriterWorker();
  CEvntWriterWorker(TEvntWriterInfo & wi);  // NOLINT (runtime/references)
  virtual ~CEvntWriterWorker();
  // initialize the object
  EFrameworkunifiedStatus Initialize(HANDLE hThread);

  /// Copy event logs to USB request from parent thread
  EFrameworkunifiedStatus OnCmdWriteEventFilesToUsb(HANDLE hThread);

  /// Event logging request from NS logger
  EFrameworkunifiedStatus OnCmdEventLog(UI_8* pbuf);

  /// Clear event logs request from parent thread
  EFrameworkunifiedStatus OnCmdClearEventLogs(HANDLE hThread);

  /// Read statistical counter request from parent thread
  EFrameworkunifiedStatus OnCmdReadStatisticalCounter(HANDLE hThread);

  /// Read statistical counter based on counter group ID
  EFrameworkunifiedStatus ReadStatisticalCounter(EStatCounterGroupID eStatCounterGroupID,
                                    SStatisticalCounter& buffer);  // NOLINT (runtime/references)

  /// Reset statistical counter request from parent thread
  EFrameworkunifiedStatus OnCmdResetStatisticalCounter(HANDLE hThread);

  /// Reset statistical counter based on counter group ID
  EFrameworkunifiedStatus ResetStatisticalCounter(EStatCounterGroupID eStatCounterGroupID);

  EFrameworkunifiedStatus OnCmdStop(HANDLE hThread);

  EFrameworkunifiedStatus OnCmdStart(HANDLE hThread);

  /// Counter logging request from NS logger
  EFrameworkunifiedStatus OnCmdIncrcount(UI_8* pbuf);

  /// Event and Counter logging request from NS logger
  EFrameworkunifiedStatus OnCmdIncrwriteevent(UI_8* pbuf);

  /// Reading counter value from file based on phase
  EFrameworkunifiedStatus readCountFromFile(PCSTR filename);

  /// Writing counter value from file based on phase
  EFrameworkunifiedStatus writeCountToFile(PCSTR filename);

  /// Writing event value from file based on phase
  EFrameworkunifiedStatus write_event_to_file(const CHAR* filename);

  /// Copy event files to USB based onUSB device number
  UI_8 copy_event_files_to_usb(TThrdEvtLogStore *pStUSBIndo);

  /// Functioncall to populate the counter value to map
  VOID OnCmdIncrcount_phase(st_LogCount *cnt);

  /// Function call to populate the event value to deque
  VOID OnCmdWriteEventLogs(st_LogEvent_ss *ev);

  /// Set the VIN number
  EFrameworkunifiedStatus OnCmdSetVIN(HANDLE hThread);

  /// Get the VIN number
  VOID OnCmdGetVIN(STVIN_NUMBER& f_stVIN_Number);  // NOLINT (runtime/references)

  /// Functional call to write events from deque to local buffer
  VOID write_events_to_buffer(STEventLogPersistBuffer* f_stEvtLogBuf);

  /// Functional call to read events from buffer to deque
  VOID read_events_from_buffer(STEventLogPersistBuffer* f_stEvtLogBuf);

  /// Functional call to immediate persist event log
  EFrameworkunifiedStatus immediate_persist_event_log(HANDLE hThread);

  /// Functional call to set mileage information in Evt thread space
  EFrameworkunifiedStatus EvtThd_SetMileage(HANDLE hThread);

  /// Functional call to write mileage data to file for persisting
  EFrameworkunifiedStatus write_mileage_to_file(const CHAR* filename);

  /// Functional call to read mileage data from persistent file during startup
  EFrameworkunifiedStatus read_mileage_from_file(const CHAR* filename);

  EFrameworkunifiedStatus OnCmdReadNumberOfEventsLogged(HANDLE hThread);

  EFrameworkunifiedStatus EventLogRegisterCbHandlers(HANDLE hThread);

  EFrameworkunifiedStatus OnCmdUploadEventLog(HANDLE hThread);

  EFrameworkunifiedStatus OnCmdCopyEvntLogToTmp(HANDLE hThread);
  /// Static Members for Event and counter logging
  static CounterInformation counter;
  static DEQUE_Event_Type deque_event_info;

 private:
  STVIN_NUMBER m_stVINnumber;
  UI_32 m_u32MileageData;
  UI_32 m_u32NumberEventsLogged;
  TUploadEventLogResp m_stUploadEventLogResp;
  EFrameworkunifiedStatus counterIncrementByValue(EStatCounterGroupID group, UI_16 id,
                                     UI_32 value);
  EFrameworkunifiedStatus writeGroupToFile(FILE *fp, EStatCounterGroupID group);
};

/// Event logging Child thread start and shut down functions
EFrameworkunifiedStatus EvntWriterWorkerOnStart(HANDLE hThread);
EFrameworkunifiedStatus EvntWriterWorkerOnStop(HANDLE hThread);

/// USBdevice detectioncall backs
EFrameworkunifiedStatus DD_USBSrvDetectionCallBack(HANDLE hApp);
EFrameworkunifiedStatus DD_USBServiceAvailabilityCallBack(HANDLE hThread);
EFrameworkunifiedStatus DD_USBOpenSessionAckCallBack(HANDLE hThread);
EFrameworkunifiedStatus DD_USBCloseSessionAckCallBack(HANDLE hApp);

inline void u16copy(PUI_8 pDest, UI_16 Source) {
  *(pDest++) = (UI_8) (Source & 0x00FF);
  *(pDest++) = (UI_8) ((Source & 0xFF00) >> 8);
}
inline void u32copy(PUI_8 pDest, UI_32 Source) {
  *(pDest++) = (UI_8) (Source & 0x000000FF);
  *(pDest++) = (UI_8) ((Source & 0x0000FF00) >> 8);
  *(pDest++) = (UI_8) ((Source & 0x00FF0000) >> 16);
  *(pDest++) = (UI_8) ((Source & 0xFF000000) >> 24);
}

#endif  // LOGGER_SERVICE_SERVER_INCLUDE_LOGGERSERVICEDEBUG_WRITER_EVNTWORKER_H_
