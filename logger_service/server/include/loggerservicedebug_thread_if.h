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

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NS_LogTraceSender
/// \brief    This file supports the logging thread interface.
///
///////////////////////////////////////////////////////////////////////////////
#ifndef LOGGER_SERVICE_SERVER_INCLUDE_LOGGERSERVICEDEBUG_THREAD_IF_H_
#define LOGGER_SERVICE_SERVER_INCLUDE_LOGGERSERVICEDEBUG_THREAD_IF_H_

#include <pthread.h>
#ifdef AGL_STUB
#else
#include <sys/neutrino.h>
#include <sys/dispatch.h>
#endif
#include <errno.h>
#include <sched.h>
#include <stdio.h>
#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_thread_priority.h>
#include <system_service/ss_logger_service.h>
#include <system_service/ss_logger_service_local.h>
#include <string>
#include "loggerservicedebug_loggerservicelog.h"

typedef std::string ThreadName;    //< service name

typedef enum _EThreadCommandIds {
  eThrdCmdUsbEject = 5463,        ///< USB eject --> frameworkunifieddebug
  eThrdCmdUsbCaptureLogs,       ///< USB Capture log --> frameworkunifieddebug
  eThrdCmdUsbStoreLogs,               ///< USB Store log --> frameworkunifieddebug
  eThrdCmdWriteFilesToUsb,      ///< frameworkunifieddebug --> Writer
  eThrdCmdWriteDataToFile,      ///< Reader --> Writer
  eThrdCmdWriterStop,        ///< frameworkunifieddebug --> Writer (Stop writing to the file)
  eThrdCmdWriterResume,  ///< frameworkunifieddebug --> Writer (Continue writing to the file)
  eThrdCmdCaptureLogScriptExit,    ///< LogCaptureScript - > frameworkunifieddebug
  eThrdCmdWriteEvntFilesToUsb,         ///< Wreite Event files to USB
  eThrdCmdcopytofile,
  eThrdCmdClearEvntLogs,        ///< Clear event logs
  eThrdCmdStatisticalCounter,      ///< Read statistical counter
  eThrdCmdStatisticalCounterSuccessResp,  ///< Statistical counter positiv Response from child thread
  eThrdCmdStatisticalCounterErrorResp,  ///< Statistical counter Negative Response from child thread
  eThrdCmdResetStatisticalCntrs,
  eThrdCmdCopyEventLogUSBResponse,   ///< Copy event logs to usb response from child thread
  eThrdCmdClearEventLogResponse,  ///< Clear event logs response fromchild thread
  eThrdCmdPersistLogs,         ///< Statistical counter Response fromchild thread
  eThrdCmdSetVINnumber,
  eThrdCmdImmPersistEvtLog,
  eThrdCmdMileageData,
  eThrdCmdGetNumberOfEventsLogged,
  eThrdCmdNumberOfEventsLoggedResponse,
  eThrdCmdUploadEventLog,
  eThrdCmdUploadEventLogResponse,
  eThrdCmdCopyEvntLogToTmp,
  eThrdCmdCopyEvntLogToTmpResponse
} EThreadCommandIds;

typedef struct _TThrdWriteDataToFileEvt {
  static const UI_32 LN_LEN = 1024;
  CHAR line[LN_LEN];
  UI_32 length;
} TThrdWriteLogDataToFileEvt, TThrdWriteTransmitDataToFileEvt;

typedef struct _TThrdWriteFilesToUsbEvt {
  static const UI_32 LOC_PATH_LEN = 256;
  CHAR location[LOC_PATH_LEN];  ///< full path where files should be copied too
  UI_32 loc_len;
} TThrdWriteLogFilesToUsbEvt, TThrdWriteTransmitFilesToUsbEvt;

typedef struct _TThrdCaptureLogsEvt {
  static const UI_32 FN_LEN = 256;
  CHAR location[FN_LEN];
  CHAR parse_fn[FN_LEN];  ///< full path an name of the file that should be parsed for event actions.
} TThrdCaptureLogsEvt;

// Structure to hold the USB device informaton
typedef struct _TThrdEvtLogStore {
  BOOL IsDeviceAvailable;
  static const UI_32 FN_LEN = 256;
  CHAR location[FN_LEN];
  CHAR parse_fn[FN_LEN];  ///< full path an name of the file that should be parsed for event actions.
} TThrdEvtLogStore;

typedef struct _TInitFlag {
  UI_8 mileage_flagbit :1;
  UI_8 statcounter_flagbit :1;
  UI_8 eventlog_flagbit :1;
  UI_8 reghandlers_flagbit :1;
} TInitFlag;

typedef struct _TSessionData {
  std::string strSrcName;
  UI_32 session_id;
} TSessionData;

typedef struct _TEvntNumber {
  TSessionData stSessiondata;
  UI_16 u16numberofeventslogged;
} TEvntsLogged;

typedef struct _TWriteFilesToUsbCmdData {
  TSessionData stSessiondata;
  EEvtLogUSBDevNumber eDevId;
} TWriteFilesToUsbCmdData;

typedef struct _TWriteFilesToUsbCmdResponse {
  TSessionData stSessiondata;
  UI_8 u8Response;
} TWriteFilesToUsbCmdResponse;

typedef struct _TClearEvntLogCmdResponse {
  TSessionData stSessiondata;
  UI_8 u8Response;
} TClearEvntLogCmdResponse;

typedef struct _TStatisticalCountersCmd {
  TSessionData stSessiondata;
  EStatCounterGroupID eGroupId;
} TStatisticalCountersCmd;

typedef struct _TStatisticalCntCmdSuccessResp {
  TSessionData stSessiondata;
  SStatisticalCounter stBuffer;
} TStatisticalCntCmdSuccessResp;

typedef struct _TUploadEventLogResp {
  TSessionData stSessiondata;
  STEventLogPersistBuffer stEventLogBuffer;
} TUploadEventLogResp;

typedef struct _TEventLogCopyStatus {
  static const UI_8 FilePathSize = 64;
  CHAR EventLogFilePath[FilePathSize];
  UI_8 status;
} TEventLogCopyStatus;

typedef enum _ECaptureLogScriptResp {
  ekCLSStatusCompleteSuccessful,
  ekCLSStatusInitError,
  ekCLSStatusExecuteError,
  ekCLSStatusCopyError,
  ekCLSStatusChmodError,
} ECaptureLogScriptResp;

class ThreadWrapper {
 public:

  ThreadWrapper(const char* t_name, void* (*routine)(void*), void* arg) {   // LCOV_EXCL_START 8:ThreadWrapper class is never used
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    pthread_attr_t attr;
    struct sched_param params;

    if (EOK != pthread_attr_init(&attr)) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Error: pthread_attr_init failed errno: %d", errno);
    }
    if (EOK != pthread_attr_getschedparam(&attr, &params)) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Error: pthread_attr_getschedparam failed errno: %d", errno);
    }
    SI_32 prio = frameworkunified::framework::CFrameworkunifiedThreadPriorities::GetPriority(
        std::string(t_name));
    if (-1 != prio) {
      params.sched_priority = prio;
      if (EOK != pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED)) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Error: pthread_attr_setinheritsched failed errno: %d", errno);
      }
      if (EOK != pthread_attr_setschedparam(&attr, &params)) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Error: pthread_attr_setschedparam failed errno: %d", errno);
      }
    }

    if (EOK != pthread_create(&m_thread, &attr, routine, arg)) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: pthread_create failed errno: %d",
             errno);
    }
  }
  // LCOV_EXCL_STOP


  ~ThreadWrapper() {  // LCOV_EXCL_START 8:ThreadWrapper class is never used
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
// todo: Need a clean way to cleanup thread in case class closes
  }
  // LCOV_EXCL_STOP


  operator pthread_t() const {  // LCOV_EXCL_START 8:ThreadWrapper class is never used
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return m_thread;
  }
  // LCOV_EXCL_STOP

 private:
  pthread_t m_thread;
};


class CChild {
 public:
  CChild(const char* parentsName, const char * sendDataToQue,
         const char * clientsName, void* (*routine)(void*),
         const char * sendToUdpQue = "")
      : m_clientName(clientsName),
        m_parentName(parentsName),
        m_sendDataToQueuName(sendDataToQue),
        m_sendDataToUdpQueName(sendToUdpQue),
        m_barInit(InitBarrier()),
        m_thread(clientsName, routine, reinterpret_cast<void*>(this)) {  // LCOV_EXCL_START 7:debug code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  }
  // LCOV_EXCL_STOP

  void Join() {  // LCOV_EXCL_START 7:debug code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    if (EOK != pthread_join(m_thread, NULL)) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: pthread_join failed errno: %d",
             errno);
    }
  }
  // LCOV_EXCL_STOP

  const char * ParentName() {  // LCOV_EXCL_START 7:debug code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return m_parentName.c_str();
  }
  // LCOV_EXCL_STOP

  const char * ClientName() {  // LCOV_EXCL_START 7:debug code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return m_clientName.c_str();
  }
  // LCOV_EXCL_STOP

  const char * SendDataToQue() {  // LCOV_EXCL_START 7:debug code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return m_sendDataToQueuName.c_str();
  }
  // LCOV_EXCL_STOP

  const char * SendDataToUdpQueue() {  // LCOV_EXCL_START 7:debug code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return m_sendDataToUdpQueName.c_str();
  }
  // LCOV_EXCL_STOP

  bool IsValidWaitBarrier() {  // LCOV_EXCL_START 7:debug code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    if (m_barInit) {
      int wbret = pthread_barrier_wait(&m_barrier);
      return ( PTHREAD_BARRIER_SERIAL_THREAD == wbret) || (0 == wbret);
    }

    return false;
  }
  // LCOV_EXCL_STOP

 private:
  bool InitBarrier() {  // LCOV_EXCL_START 7:debug code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    /// Note: the count value should match the number of threads that are readers and usb notifiers
    return (EOK == pthread_barrier_init(&m_barrier, NULL, 2) ? true : false);
  }
  // LCOV_EXCL_STOP

 private:
  ThreadName m_clientName;
  ThreadName m_parentName;
  ThreadName m_sendDataToQueuName;
  ThreadName m_sendDataToUdpQueName;
  bool m_barInit;
  ThreadWrapper m_thread;
  pthread_barrier_t m_barrier;
};

class LoggerserviceDebugChildThread {
 public:
  typedef enum e {
    kLoggerserviceDebugLogQueReader,
    kLoggerserviceDebugTransmitQueReader,
    kLoggerservicePerformanceLogQueReader,
    kLoggerserviceDebugCaptureLogScript,
    kLoggerserviceDebugEventLogque,
    kLoggerserviceDebugMax
  } ELoggerserviceDebugThreads;

  LoggerserviceDebugChildThread();
  virtual ~LoggerserviceDebugChildThread();
  BOOL Start(ELoggerserviceDebugThreads id, const char* parentsName,
             const char * sendDataToQue, void* (*routine)(void*),
             const char * sendToUdpQue = "");
  void Stop(ELoggerserviceDebugThreads id);
  void StopAll();
  const char * Name(ELoggerserviceDebugThreads id);
  void LiftWaitBarrier(ELoggerserviceDebugThreads id);
  BOOL Running(ELoggerserviceDebugThreads id) const;
 private:
  ThreadName names[kLoggerserviceDebugMax];
  CChild * threads[kLoggerserviceDebugMax];
};

#endif  // LOGGER_SERVICE_SERVER_INCLUDE_LOGGERSERVICEDEBUG_THREAD_IF_H_
