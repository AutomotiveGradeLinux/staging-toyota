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
/// \ingroup  tag_NS_InterfaceunifiedLogCapture
/// \brief    This file contains the standard set functions called by the NS
//            dispatcher on application initialization, cleanup and wakeup.
///
///////////////////////////////////////////////////////////////////////////////
// System Headers
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <native_service/frameworkunified_application.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_service_protocol.h>
#include <native_service/frameworkunified_types.h>
#include <native_service/cl_process.h>

#include <native_service/nslogutil_cmd_if.h>
#include <native_service/ns_np_service.h>
#include <native_service/ns_np_service_protocol.h>

#include <loggerservicedebug_loggerservicelog.h>
#include <loggerservicedebug_thread_if.h>
#include <loggerservicedebug_writer_Evntworker.h>

#include <native_service/frameworkunified_timer.h>
#include <ss_logger_device_detection.h>

#include <ss_logger_cfg.h>
#include <system_service/ss_logger_service_notifications.h>
#include <system_service/ss_logger_service.h>
#include <ss_logger_service_callbacks.h>

#include <system_service/ss_sm_client_if.h>

#include <system_service/ss_system_manager_if.h>
#include <system_service/ss_system_manager_protocol.h>
#include <system_service/ss_system_manager_notifications.h>

#include <system_service/ss_devicedetection_service_notifications.h>
#include <system_service/ss_devicedetection_service_ifc.h>
#include <system_service/ss_services.h>
#include <ss_logger_error_event.h>
#include <ss_logger_reader_writer_control.h>
#include <ss_logger_common.h>
#include <stub/pfdrec_thread_ifc.h>
#include <system_service/ss_templates.h>
#include <system_service/ss_logger_store_logs.h>
#include <queue>
#include <string>

// const definitions
#define TMPFS_PATH "/tmp"

#define SYS_ILLEGAL_LOG_DELAY_TIME  ( 20 * 1000 )
const CHAR LOGGERSERVICELOGGER_SETPARAMS[] = TMPFS_PATH "/loggerservicelogger_setparams.log";
const CHAR Counter_LOG_PATH_FN[] = TMPFS_PATH "/loggerservice_counter.log";
const CHAR Mileage_LOG_PATH_FN[] = TMPFS_PATH "/loggerservice_mileage.log";

const CHAR g_strLogQueWriterWorkerName[] = "pdg.LogQueWriter";
const CHAR g_strTransmitQueWriterWorkerName[] = "pdg.TransmitQueWriter";
const CHAR g_strUdpQueWriterWorkerName[] = "pdg.UdpQueWriter";
const CHAR g_strEvntLogQueWorkerName[] = "pdg.EvntLogQue";

// handles to writer threads
static HANDLE g_thrdLogWriter = NULL;  // Var. that holds the child handle to the Log Writer thread
static HANDLE g_thrdTxWriter = NULL;  // Var. that holds the child handle to the Tx Writer thread

/// Event_logger
HANDLE g_thrdEvntLogWriter = NULL;  // Var. that holds the child handle to the EVNTLOGGER Writer thread
static TEvntWriterInfo Evntlog_wi = { };
TUploadEventLogResp g_stUploadEventLogResp;

// global variable to store the service status
ELOGGERSERVICESTATUS g_eLoggerServiceStatus = eLSInit;

// class to usb and reader threads
LoggerserviceDebugChildThread g_loggerservicedebug_threads;

/// Device Detection Class Instance
DeviceDetectionServiceIf g_devDetect_t;

CLoggerCfg g_loggerCfg;
CErrorEvent g_errorEventHandler;
CLoggerServiceCallbacks g_serviceCallbacks;
CLoggerDeviceDetection g_deviceDetection;
CReaderWriterControl g_ReaderWriterControl;
CPFDRECThread g_PFDRECThread;

/// Decide the function NormalStartupProcess execution
static bool g_normal_startup_status = false;

/// Callbacks for messages that will be received and processed by this module
static EFrameworkunifiedStatus OnUsbEject(HANDLE hApp);

static EFrameworkunifiedStatus OnUsbStoreLogs(HANDLE hApp);

static EFrameworkunifiedStatus SMSessionAckCb(HANDLE hApp);

/// Positive Response Call backs from child thread for  statistical counter read
static EFrameworkunifiedStatus cbStatisticalCounterSuccessResp(HANDLE hApp);

/// Error Response Call backs from child thread for  statistical counter read
static EFrameworkunifiedStatus cbStatisticalCounterErrorResp(HANDLE hApp);

/// Response Call backs from child thread for  clear event logs
static EFrameworkunifiedStatus cbClearEventLogsResponse(HANDLE hApp);

/// Response Call backs from child thread for  copying event logs to USB
static EFrameworkunifiedStatus cbCopyEventLogsUSBResponse(HANDLE hApp);

/// Response Call backs from child thread for Reading number of events logged
static EFrameworkunifiedStatus cbReadNumbOfEventsResponse(HANDLE hApp);

/// Response Call backs from child thread for Upload eventlog
static EFrameworkunifiedStatus cbUploadEventLogResponse(HANDLE hApp);

/// Notificatin from SystemManager when All Services wakeup completed
static EFrameworkunifiedStatus cbServiceWakeupStatus(HANDLE hApp);


/// generate SYS_ILG_LOG
static EFrameworkunifiedStatus SysIllegalLogTimer_OnInterval(HANDLE hApp);


/// Call back tables for response from event logger child thread
FrameworkunifiedProtocolCallbackHandler evtLogChildThread_handler[] = { {  // LCOV_EXCL_BR_LINE 11:Unexpected branch
    eThrdCmdStatisticalCounterSuccessResp, cbStatisticalCounterSuccessResp }, {
    eThrdCmdStatisticalCounterErrorResp, cbStatisticalCounterErrorResp }, {
    eThrdCmdCopyEventLogUSBResponse, cbCopyEventLogsUSBResponse }, {
    eThrdCmdClearEventLogResponse, cbClearEventLogsResponse }, {
    eThrdCmdNumberOfEventsLoggedResponse, cbReadNumbOfEventsResponse }, {
    eThrdCmdUploadEventLogResponse, cbUploadEventLogResponse } };

//////////////////////////////////////////
//  Function : FrameworkunifiedOnInitialization
//////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOnInitialization(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  // 1. Register Srv Availability Notification
  if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedRegisterServiceAvailabilityNotification(hApp, NTFY_SS_LoggerService_Availability))) {  // LCOV_EXCL_BR_LINE 4:NSFW  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 4:NSFW
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "Failed to set service availability notification:0x%x ", l_eStatus);
    // LCOV_EXCL_STOP
  }

  // 2. Publish Service not available
  if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedPublishServiceAvailability(hApp, FALSE))) {  // LCOV_EXCL_BR_LINE 4:NSFW  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 4:NSFW
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "Failed to set service availability notification:%d ", l_eStatus);
    // LCOV_EXCL_STOP
  }

  l_eStatus = g_loggerCfg.Initialize(hApp);
  LOG_STATUS_IF_ERRORED(l_eStatus, "g_loggerCfg.Initialize()");  // LCOV_EXCL_BR_LINE 15: macro

  std::string l_cfgFilePathAndName =
      "/usr/agl/conf/BS/ss/logger_service/rodata/ss_logger.cfg";
  if (eFrameworkunifiedStatusOK != (l_eStatus = g_loggerCfg.Load(l_cfgFilePathAndName))) {  // LCOV_EXCL_BR_LINE 200:To ensure success
    // LCOV_EXCL_START 200:To ensure success
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(
        ZONE_ERR, __FUNCTION__,
        " Error. Failed to load SS_Logger configuration from %s with error: %d",
        l_cfgFilePathAndName.c_str(), l_eStatus);
    // LCOV_EXCL_STOP
  } else if (eFrameworkunifiedStatusOK != (l_eStatus = g_loggerCfg.Validate())) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           " Error. Configuration validation failed with error: %d.",
           l_eStatus);
  }

  g_loggerCfg.Print();  // LCOV_EXCL_BR_LINE 11:Unexpected branch

  l_eStatus = g_deviceDetection.Initialize(hApp, &g_loggerCfg);  // LCOV_EXCL_BR_LINE 11:Unexpected branch
  LOG_STATUS_IF_ERRORED(l_eStatus, "g_deviceDetection.Initialize");  // LCOV_EXCL_BR_LINE 15: macro

  l_eStatus = g_ReaderWriterControl.Initialize(&g_loggerCfg);  // LCOV_EXCL_BR_LINE 11:Unexpected branch
  LOG_STATUS_IF_ERRORED(l_eStatus, "g_ReaderWriterControl.Initialize()");  // LCOV_EXCL_BR_LINE 15: macro

  if (NULL == (g_thrdEvntLogWriter = FrameworkunifiedCreateChildThreadWithPriority( hApp, g_strEvntLogQueWorkerName, EvntWriterWorkerOnStart, EvntWriterWorkerOnStop, frameworkunified::framework::CFrameworkunifiedThreadPriorities::GetPriority(std::string(g_strEvntLogQueWorkerName))))) {  // LCOV_EXCL_BR_LINE 4:NSFW  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 4:NSFW
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(
        ZONE_ERR,
        __FUNCTION__,
        "FrameworkunifiedCreateChildThreadWithPriority Failed Status:0x%x for thread create of %s",
        l_eStatus, g_strEvntLogQueWorkerName);
    return (l_eStatus);
    // LCOV_EXCL_STOP
  } else {
    FRAMEWORKUNIFIEDLOG(  // LCOV_EXCL_BR_LINE 15: macro
        ZONE_INFO,
        __FUNCTION__,
        "FrameworkunifiedCreateChildThreadWithPriority Success Status: for thread create of %s",
        g_strEvntLogQueWorkerName);  // LCOV_EXCL_BR_LINE 15: macro
  }

  l_eStatus = g_errorEventHandler.Initialize(hApp, &g_loggerCfg,  // LCOV_EXCL_BR_LINE 11:Unexpected branch
                                             &g_ReaderWriterControl,
                                             g_thrdEvntLogWriter,
                                             g_strEvntLogQueWorkerName);  // LCOV_EXCL_BR_LINE 11:Unexpected branch
  if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 200:To ensure success
    // LCOV_EXCL_START 200:To ensure success
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           " Error. g_errorEventHandler.Initialize() returned: %d.", l_eStatus);
    // LCOV_EXCL_STOP
  }
  l_eStatus = g_serviceCallbacks.Initialize(hApp, &g_loggerCfg,
                                            &g_errorEventHandler);  // LCOV_EXCL_BR_LINE 11:Unexpected branch
  LOG_STATUS_IF_ERRORED(l_eStatus, "g_serviceCallbacks.Initialize()");  // LCOV_EXCL_BR_LINE 15: macro

  // Callback to Event log child Thread
  l_eStatus = FrameworkunifiedAttachCallbacksToDispatcher(
      hApp, g_strEvntLogQueWorkerName, evtLogChildThread_handler,
      _countof(evtLogChildThread_handler));  // LCOV_EXCL_BR_LINE 11:Unexpected branch
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,  // LCOV_EXCL_BR_LINE 15: macro
         "Status of callback for child thread attach:%X", l_eStatus);  // LCOV_EXCL_BR_LINE 15: macro

  // setup call backs for my children
  FrameworkunifiedAttachCallbackToDispatcher(hApp, AppName, eThrdCmdUsbEject, OnUsbEject);  // LCOV_EXCL_BR_LINE 11:Unexpected branch
  FrameworkunifiedAttachCallbackToDispatcher(hApp, AppName, eThrdCmdUsbStoreLogs,
                                OnUsbStoreLogs);

  // Publications
  FrameworkunifiedNotificationsList publish_notifs[] = {
  // Notifications name,length, state
      { NTFY_LOGGER_SETCONTROLMASK, sizeof(CHANGELOGPARAMS), eFrameworkunifiedStateVar } };

  // Indicate to Notification Service what I will be Publishing!
  if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedNPRegisterNotifications(hApp, publish_notifs, _countof(publish_notifs)))) {  // LCOV_EXCL_BR_LINE 4:NSFW  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 4:NSFW
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "FrameworkunifiedNPRegisterNotifications Failed Status:0x%x ", l_eStatus);
    return (l_eStatus);
    // LCOV_EXCL_STOP
  }

  /// Start the Writer threads
  strncpy(Evntlog_wi.mileage_filename, Mileage_LOG_PATH_FN,
          Evntlog_wi.FN_LEN - 1);
  Evntlog_wi.mileage_filename[Evntlog_wi.FN_LEN - 1] = '\0';
  strncpy(Evntlog_wi.base_cnt_filename, Counter_LOG_PATH_FN,
          Evntlog_wi.FN_LEN - 1);
  Evntlog_wi.base_cnt_filename[Evntlog_wi.FN_LEN - 1] = '\0';
  Evntlog_wi.max_filelen = g_loggerCfg.m_logMaxFileSize;

  if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedStartChildThread(hApp, g_thrdEvntLogWriter, sizeof(Evntlog_wi), &Evntlog_wi))) {  // LCOV_EXCL_BR_LINE 4:NSFW  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 4:NSFW
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __PRETTY_FUNCTION__,
           "Fail to Start Writer Worker thread. Status:0x%x", l_eStatus);
    // LCOV_EXCL_STOP
  }

  l_eStatus = RegisterSMSessionAckCallback(SMSessionAckCb);  // LCOV_EXCL_BR_LINE 11:Unexpected branch
  LOG_STATUS_IF_ERRORED(l_eStatus, "RegisterSMSessionAckCallback()");  // LCOV_EXCL_BR_LINE 15: macro

  l_eStatus = g_PFDRECThread.Initialize(hApp);  // LCOV_EXCL_BR_LINE 11:Unexpected branch
  LOG_STATUS_IF_ERRORED(l_eStatus, "g_PFDRECThread.Initialize()");  // LCOV_EXCL_BR_LINE 15: macro

  // Publish Service available this can also be published from FrameworkunifiedOnStart callback
  if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedPublishServiceAvailability(hApp, TRUE))) {  // LCOV_EXCL_BR_LINE 4:NSFW
    // LCOV_EXCL_START 4:NSFW
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "Failed to set service availability notification:0x%x ", l_eStatus);
    // LCOV_EXCL_STOP
  }

l_eStatus = FrameworkunifiedSubscribeNotificationWithCallback(hApp, NTFY_SSServiceWakeupStatus, cbServiceWakeupStatus);  // LCOV_EXCL_BR_LINE 11:Unexpected branch
    LOG_STATUS_IF_ERRORED(l_eStatus, "FrameworkunifiedSubscribeNotificationWithCallback()");  // LCOV_EXCL_BR_LINE 15: macro

  // Set Service status to init
  g_eLoggerServiceStatus = eLSInit;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

static void *accOffOnCollect(void* param) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  SS_LoggerStoreLogs(SS_STORELOGS_ACCOFFON_PRESS);
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return NULL;
}

// Normal startup process
static EFrameworkunifiedStatus NormalStartupProcess(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (false == g_normal_startup_status) {
    T_SS_SM_START_DataStructType prm;

    l_eStatus = FrameworkunifiedGetMsgDataOfSize(hApp, &prm, sizeof(T_SS_SM_START_DataStructType), eSMRRetain);
    if (eFrameworkunifiedStatusOK != l_eStatus) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __PRETTY_FUNCTION__, "Failed PasGetMsgDataOfSize() [%d]", l_eStatus);
    }

    if (prm.resetStatus == e_SS_SM_RESET_STATUS_NG) {
      HANDLE timer =
          FrameworkunifiedAttachTimerCallback(hApp, SYS_ILLEGAL_LOG_DELAY_TIME, 0, SysIllegalLogTimer_OnInterval);
      if (timer == NULL) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __PRETTY_FUNCTION__, " Failed to register timer.");
      }
    }

    if (0 == access("/tmp/accoffon", F_OK)) {
      pthread_t threadAccOffOn;
      pthread_create( &threadAccOffOn, NULL, accOffOnCollect, NULL);
    }

    if (g_eLoggerServiceStatus == eLSStop) {
      // Publish Service available this can also be published from FrameworkunifiedOnStart callback
      if (eFrameworkunifiedStatusOK
          != (l_eStatus = FrameworkunifiedPublishServiceAvailability(hApp, TRUE))) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Failed to set service availability notification:0x%x ",
               l_eStatus);
      }

      // Start the Writer threads
      if (eFrameworkunifiedStatusOK
          != (l_eStatus = FrameworkunifiedStartChildThread(hApp, g_thrdEvntLogWriter,
                                              sizeof(Evntlog_wi), &Evntlog_wi))) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __PRETTY_FUNCTION__,
               "Fail to Start Writer Worker thread. Status:0x%x", l_eStatus);
      }
    }
    // Set Service status to init
    g_eLoggerServiceStatus = eLSStart;

    // Set normal_startup_status flag (true)
    g_normal_startup_status = true;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

/**
 * EFrameworkunifiedStatus FrameworkunifiedOnPreStart(HANDLE hApp)
 * @brief Used to .
 *
 * @param hApp Handle to the SS_Power Framework Obj.
 *
 * @return method status of completion or failure.
 */
EFrameworkunifiedStatus FrameworkunifiedOnPreStart(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  EFrameworkunifiedStatus l_eStatus = NormalStartupProcess(hApp);

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

/**
 * EFrameworkunifiedStatus FrameworkunifiedOnBackgroundStart(HANDLE hApp)
 * @brief Used to .
 *
 * @param hApp Handle to the SS_Power Framework Obj.
 *
 * @return method status of completion or failure.
 */
EFrameworkunifiedStatus FrameworkunifiedOnBackgroundStart(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  EFrameworkunifiedStatus l_eStatus = NormalStartupProcess(hApp);

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

/**
 * EFrameworkunifiedStatus FrameworkunifiedOnStart(HANDLE hApp)
 * @brief Used to .
 *
 * @param hApp Handle to the SS_Power Framework Obj.
 *
 * @return method status of completion or failure.
 */
EFrameworkunifiedStatus FrameworkunifiedOnStart(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  EFrameworkunifiedStatus l_eStatus = NormalStartupProcess(hApp);

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

static EFrameworkunifiedStatus SysIllegalLogTimer_OnInterval(HANDLE hApp) {  // LCOV_EXCL_START 200: There is no SUBCP_STS_COMNG case
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  l_eStatus = SS_LoggerStoreLogs(SS_STORELOGS_SYS_ILLEGAL);
  if (l_eStatus != eFrameworkunifiedStatusOK) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __PRETTY_FUNCTION__, " Failed SS_LoggerStoreLogs()");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
// LCOV_EXCL_STOP

/**
 * EFrameworkunifiedStatus FrameworkunifiedOnPreStop(HANDLE hApp)
 * @brief Used to .
 *
 * @param hApp Handle to the SS_Power Framework Obj.
 *
 * @return method status of completion.
 */
EFrameworkunifiedStatus FrameworkunifiedOnPreStop(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eFrameworkunifiedStatusOK;
}

/**
 * EFrameworkunifiedStatus FrameworkunifiedOnBackgroundStop(HANDLE hApp)
 * @brief Used to .
 *
 * @param hApp Handle to the SS_Power Framework Obj.
 *
 * @return method status of completion.
 */
EFrameworkunifiedStatus FrameworkunifiedOnBackgroundStop(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eFrameworkunifiedStatusOK;
}

/**
 * EFrameworkunifiedStatus FrameworkunifiedOnStop(HANDLE hApp)
 * @brief Used to .
 *
 * @param hApp Handle to the SS_Power Framework Obj.
 *
 * @return method status of completion or failure.
 */
EFrameworkunifiedStatus FrameworkunifiedOnStop(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  /*Get stop factor form hApp*/
  T_SS_SM_STOP_DataStructType errorType;

  // LCOV_EXCL_BR_START 4: NSFW error
  if (eFrameworkunifiedStatusOK
      != FrameworkunifiedGetMsgDataOfSize(hApp, (PVOID) &errorType, sizeof(errorType),
                             eSMRRelease)) {
  // LCOV_EXCL_BR_STOP
    // LCOV_EXCL_START 4: NSFW error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __PRETTY_FUNCTION__,
           " FrameworkunifiedGetMsgDataOfSize failed with error");
    // LCOV_EXCL_STOP
  } else {
    EFrameworkunifiedStatus loggerserviceRet = SS_LoggerStoreLogs(SS_STORELOGS_INTERFACEUNIFIEDLOG);
    LOG_STATUS_IF_ERRORED(loggerserviceRet, "Fail to save Loggerservice Log");
    EFrameworkunifiedStatus naviLog_status = g_errorEventHandler.SaveNaviLog(
        errorType.shutdownTrigger);
    LOG_STATUS_IF_ERRORED(naviLog_status, "Fail to save Navi Log");
  }
  StopLoggingFunction(hApp);

  if (g_eLoggerServiceStatus == eLSStart) {  // LCOV_EXCL_BR_LINE 200: g_eLoggerServiceStatus must be eLSStart on ACC-OFF
    LoggerService_OnStop(hApp);
  }

  // Set normal_startup_status flag (false)
  g_normal_startup_status = false;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

/**
 * EFrameworkunifiedStatus FrameworkunifiedCreateStateMachine(HANDLE hApp)
 * @brief Used to .
 *
 * @param hApp Handle to the SS_Power Framework Obj.
 *
 * @return method status of completion or failure.
 */
EFrameworkunifiedStatus FrameworkunifiedCreateStateMachine(HANDLE hApp) {  // LCOV_EXCL_START 8:dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eFrameworkunifiedStatusOK;
}
// LCOV_EXCL_STOP

/**
 * EFrameworkunifiedStatus LoggerService_OnStop(HANDLE hApp)
 * @brief Logger Stop is called on Shutdown complete cmd
 *
 * @param hApp Handle to the SS_Power Framework Obj.
 *
 * @return method status of completion or failure.
 */
EFrameworkunifiedStatus LoggerService_OnStop(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // Set Service status to init
  g_eLoggerServiceStatus = eLSStop;

  /// Start the Writer threads
  if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedStopChildThread(hApp, g_thrdEvntLogWriter, sizeof(Evntlog_wi), &Evntlog_wi))) {  // LCOV_EXCL_BR_LINE 4:NSFW  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 4:NSFW
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __PRETTY_FUNCTION__,
           "Fail to Start Writer Worker thread. Status:0x%x", l_eStatus);
    // LCOV_EXCL_STOP
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__,
           "Successful in Sending Stop to ChildThread");
  }

  // Publish Service available this can also be published from FrameworkunifiedOnStart callback
  if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedPublishServiceAvailability(hApp, FALSE))) {  // LCOV_EXCL_BR_LINE 4:NSFW  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 4:NSFW
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "Failed to set service availability notification:0x%x ", l_eStatus);
    // LCOV_EXCL_STOP
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

EFrameworkunifiedStatus SMSessionAckCb(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  HANDLE l_hSession;

  INTERFACEUNIFIEDLOG_RECEIVED_FROM(hApp);  // LCOV_EXCL_BR_LINE 15: macro

  l_hSession = FrameworkunifiedGetOpenSessionHandle(hApp);

  if (NULL == l_hSession) {  // LCOV_EXCL_BR_LINE 4:NSFW
    // LCOV_EXCL_START 4:NSFW
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(
        ZONE_ERR, __FUNCTION__,
        " Error. FrameworkunifiedGetSessionHandle() returned a NULL session handle. "
        "Error events originating from System Manager will not be detected.");
    // LCOV_EXCL_STOP
  } else {
    l_eStatus = FrameworkunifiedSetSessionHandle(hApp, FrameworkunifiedGetMsgSrc(hApp), l_hSession);
    LOG_STATUS_IF_ERRORED(l_eStatus, "FrameworkunifiedSetSessionHandle()");  // LCOV_EXCL_BR_LINE 15: macro

    l_eStatus = g_errorEventHandler.RegisterSessionErrorEvent(l_hSession);
    LOG_STATUS_IF_ERRORED(l_eStatus,  // LCOV_EXCL_BR_LINE 15: macro
                          "g_errorEventHandler.RegisterSessionErrorEvent()");
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

EFrameworkunifiedStatus FrameworkunifiedOnWakeup(HANDLE hApp) {  // LCOV_EXCL_START 8:dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}
// LCOV_EXCL_STOP

//////////////////////////////////////////
//  Function : FrameworkunifiedOnShutdown
//////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOnShutdown(HANDLE hApp) {  // LCOV_EXCL_START 8:dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}
// LCOV_EXCL_STOP

//////////////////////////////////////////
//  Function : FrameworkunifiedOnEShutdown
//////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOnEShutdown(HANDLE hApp) {  // LCOV_EXCL_START 8:dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}
// LCOV_EXCL_STOP

//////////////////////////////////////////
//  Function : FrameworkunifiedOnDebugDump
//////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOnDebugDump(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

////////////////////////////////////////////////////////////////////////////////////////////
/// HACK! mb20100701
/// \todo Add behavior to this function
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOnDestroy(HANDLE hApp) {  // LCOV_EXCL_START 14:For process termination processing
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eFrameworkunifiedStatusNotImplemented;
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus OnUsbEject(HANDLE hApp) {  // LCOV_EXCL_START 7:debug code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");

  if (g_loggerservicedebug_threads.Running(
      LoggerserviceDebugChildThread::kLoggerserviceDebugCaptureLogScript)) {
    g_loggerservicedebug_threads.Stop(LoggerserviceDebugChildThread::kLoggerserviceDebugCaptureLogScript);
    FRAMEWORKUNIFIEDLOG(
        ZONE_INFO,
        __FUNCTION__,
        "Stopped Thread: %s",
        g_loggerservicedebug_threads.Name(
            LoggerserviceDebugChildThread::kLoggerserviceDebugCaptureLogScript));
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return (l_eStatus);
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus OnUsbStoreLogs(HANDLE hApp) {  // LCOV_EXCL_START 7:debug code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");
  TThrdCaptureLogsEvt evt = { };
  if (eFrameworkunifiedStatusOK
      != (l_eStatus = FrameworkunifiedGetMsgDataOfSize(hApp, &evt, sizeof(evt)))) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedGetMsgDataOfSize Failed Status:0x%x ",
           l_eStatus);
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
    return l_eStatus;
  }

  l_eStatus = FrameworkunifiedSendChild(hApp, g_thrdLogWriter, eThrdCmdWriterStop, 0, NULL);
  l_eStatus = FrameworkunifiedSendChild(hApp, g_thrdTxWriter, eThrdCmdWriterStop, 0, NULL);

  l_eStatus = FrameworkunifiedSendChild(hApp, g_thrdLogWriter, eThrdCmdWriteFilesToUsb,
                           sizeof(evt), &evt);
  l_eStatus = FrameworkunifiedSendChild(hApp, g_thrdTxWriter, eThrdCmdWriteFilesToUsb,
                           sizeof(evt), &evt);

  l_eStatus = FrameworkunifiedSendChild(hApp, g_thrdLogWriter, eThrdCmdWriterResume, 0,
                           NULL);
  l_eStatus = FrameworkunifiedSendChild(hApp, g_thrdTxWriter, eThrdCmdWriterResume, 0, NULL);

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return (l_eStatus);
}
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////////////////
/// Function :cbStatisticalCounterPosResponse
///////////////////////////////////////////////////////////////////////
// LCOV_EXCL_START 8:dead code
EFrameworkunifiedStatus cbStatisticalCounterSuccessResp(HANDLE hApp) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  TStatisticalCntCmdSuccessResp l_stCmdSuccessResp;
  SS_loggerserviceprotocol l_eResponseCmd;
  HANDLE l_hSession = NULL;
  if (eFrameworkunifiedStatusOK
      != (l_eStatus = ReadMsg<TStatisticalCntCmdSuccessResp>(hApp,
                                                             l_stCmdSuccessResp))) {
    LOG_ERROR("ReadMsg()");
  } else {
    if (NULL
        != (l_hSession = FrameworkunifiedGetSessionHandle(
            hApp, l_stCmdSuccessResp.stSessiondata.strSrcName.c_str(),
            l_stCmdSuccessResp.stSessiondata.session_id))) {
      l_eResponseCmd = SS_LOGGER_READ_STATL_COUNTER_SUCCESS_RESP;
      if (eFrameworkunifiedStatusOK
          != (l_eStatus = FrameworkunifiedSendMsg(l_hSession, l_eResponseCmd,
                                     sizeof(SStatisticalCounter),
                                     &l_stCmdSuccessResp.stBuffer))) {
        LOG_ERROR("FrameworkunifiedSendMsg()");
      }
    } else {
      l_eStatus = eFrameworkunifiedStatusInvldHandle;
      LOG_ERROR("FrameworkunifiedGetSessionHandle()");
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////////////////
/// Function :cbStatisticalCounterErrResponse
///////////////////////////////////////////////////////////////////////
// LCOV_EXCL_START 8:dead code
EFrameworkunifiedStatus cbStatisticalCounterErrorResp(HANDLE hApp) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  TSessionData l_stSessiondata;
  SS_loggerserviceprotocol l_eResponseCmd;
  HANDLE l_hSession = NULL;
  if (eFrameworkunifiedStatusOK
      != (l_eStatus = ReadMsg<TSessionData>(hApp, l_stSessiondata))) {
    LOG_ERROR("ReadMsg()");
  } else {
    if (NULL
        != (l_hSession = FrameworkunifiedGetSessionHandle(hApp,
                                             l_stSessiondata.strSrcName.c_str(),
                                             l_stSessiondata.session_id))) {
      l_eResponseCmd = SS_LOGGER_READ_STATL_COUNTER_ERROR_RESP;
      if (eFrameworkunifiedStatusOK
          != (l_eStatus = FrameworkunifiedSendMsg(l_hSession, l_eResponseCmd, 0x00, NULL))) {
        LOG_ERROR("FrameworkunifiedSendMsg()");
      }
    } else {
      l_eStatus = eFrameworkunifiedStatusInvldHandle;
      LOG_ERROR("FrameworkunifiedGetSessionHandle()");
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////////////////
/// Function :cbClearEventLogsResponse
///////////////////////////////////////////////////////////////////////
// LCOV_EXCL_START 8:dead code
EFrameworkunifiedStatus cbClearEventLogsResponse(HANDLE hApp) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  TClearEvntLogCmdResponse l_stCmdResponse;
  HANDLE l_hSession = NULL;
  SS_loggerserviceprotocol l_eResponseCmd;

  if (eFrameworkunifiedStatusOK
      != (l_eStatus = ReadMsg<TClearEvntLogCmdResponse>(hApp, l_stCmdResponse))) {
    LOG_ERROR("ReadMsg()");
  } else {
    if (NULL
        != (l_hSession = FrameworkunifiedGetSessionHandle(
            hApp, l_stCmdResponse.stSessiondata.strSrcName.c_str(),
            l_stCmdResponse.stSessiondata.session_id))) {
      if (l_stCmdResponse.u8Response == (UI_8) CLEAR_EVENT_LOG_SUCCESS) {
        l_eResponseCmd = SS_LOGGERCLEAREVENT_SUCCESS_RESP;
      } else {
        l_eResponseCmd = SS_LOGGERCLEAREVENT_ERROR_RESP;
      }

      if (eFrameworkunifiedStatusOK
          != (l_eStatus = FrameworkunifiedSendMsg(l_hSession, l_eResponseCmd, 0x00, NULL))) {
        LOG_ERROR("FrameworkunifiedSendMsg()");
      }
    } else {
      l_eStatus = eFrameworkunifiedStatusInvldHandle;
      LOG_ERROR("FrameworkunifiedGetSessionHandle()");
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////////////////
/// Function :cbCopyEventLogsUSBResponse
///////////////////////////////////////////////////////////////////////
// LCOV_EXCL_START 8:dead code
EFrameworkunifiedStatus cbCopyEventLogsUSBResponse(HANDLE hApp) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  TWriteFilesToUsbCmdResponse l_stCmdResponse;
  HANDLE l_hSession = NULL;
  SS_loggerserviceprotocol l_eResponseCmd;
  EEvtLoggerErrorCode l_eResponseCode;

  if (eFrameworkunifiedStatusOK
      != (l_eStatus = ReadMsg<TWriteFilesToUsbCmdResponse>(hApp,
                                                           l_stCmdResponse))) {
    LOG_ERROR("ReadMsg()");
  } else {
    if (NULL
        != (l_hSession = FrameworkunifiedGetSessionHandle(
            hApp, l_stCmdResponse.stSessiondata.strSrcName.c_str(),
            l_stCmdResponse.stSessiondata.session_id))) {
      if (l_stCmdResponse.u8Response == (UI_8) COPY_EVT_USB_SUCCESS) {
        l_eResponseCmd = SS_LOGGERCOPYEVENTUSB_SUCCESS_RESP;
        l_eResponseCode = NO_ERROR_INFO;
      } else {
        l_eResponseCmd = SS_LOGGERCOPYEVENTUSB_ERROR_RESP;
        l_eResponseCode = (EEvtLoggerErrorCode) l_stCmdResponse.u8Response;
      }

      if (eFrameworkunifiedStatusOK
          != (l_eStatus = FrameworkunifiedSendMsg(l_hSession, l_eResponseCmd,
                                     sizeof(EEvtLoggerErrorCode),
                                     &l_eResponseCode))) {
        LOG_ERROR("FrameworkunifiedSendMsg()");
      }
    } else {
      l_eStatus = eFrameworkunifiedStatusInvldHandle;
      LOG_ERROR("FrameworkunifiedGetSessionHandle()");
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START 8:dead code
EFrameworkunifiedStatus cbReadNumbOfEventsResponse(HANDLE hApp) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  UI_16 l_NumOfEvents = 0;
  TEvntsLogged l_stNumberOfEvtsLogged;
  HANDLE l_hsession = NULL;

  if (eFrameworkunifiedStatusOK
      != (l_eStatus = ReadMsg<TEvntsLogged>(hApp, l_stNumberOfEvtsLogged))) {
    LOG_ERROR("ReadMsg()");
  } else {
    l_NumOfEvents = l_stNumberOfEvtsLogged.u16numberofeventslogged;
    if (NULL
        != (l_hsession = FrameworkunifiedGetSessionHandle(
            hApp, l_stNumberOfEvtsLogged.stSessiondata.strSrcName.c_str(),
            l_stNumberOfEvtsLogged.stSessiondata.session_id))) {
      if (eFrameworkunifiedStatusOK
          != (l_eStatus = FrameworkunifiedSendMsg(l_hsession,
                                     SS_LOGGER_ENG_READ_NUMOFEVENTS_RESP,
                                     sizeof(UI_16), &l_NumOfEvents))) {
        LOG_ERROR("FrameworkunifiedSendMsg()");
      }
    } else {
      l_eStatus = eFrameworkunifiedStatusNullPointer;
      LOG_ERROR("FrameworkunifiedGetSessionHandle()");
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START 8:dead code
EFrameworkunifiedStatus cbUploadEventLogResponse(HANDLE hApp) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  HANDLE l_hsession = NULL;

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "UploadEventLog Response Msg Len = %d",
         FrameworkunifiedGetMsgLength(hApp));
  if (eFrameworkunifiedStatusOK
      != (l_eStatus = ReadMsg<TUploadEventLogResp>(hApp, g_stUploadEventLogResp))) {
    LOG_ERROR("ReadMsg()");
  } else {
    if (NULL
        != (l_hsession = FrameworkunifiedGetSessionHandle(
            hApp, g_stUploadEventLogResp.stSessiondata.strSrcName.c_str(),
            g_stUploadEventLogResp.stSessiondata.session_id))) {
      if (eFrameworkunifiedStatusOK
          != (l_eStatus = FrameworkunifiedSendMsg(l_hsession, SS_LOGGER_UPLOAD_EVENTLOG_RESP,
                                     sizeof(STEventLogPersistBuffer),
                                     &g_stUploadEventLogResp.stEventLogBuffer))) {
        LOG_ERROR("FrameworkunifiedSendMsg()");
      }
    } else {
      l_eStatus = eFrameworkunifiedStatusNullPointer;
      LOG_ERROR("FrameworkunifiedGetSessionHandle()");
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus cbServiceWakeupStatus(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  l_eStatus = g_errorEventHandler.CreateKernelLog(hApp,
                                                  SS_LOGGER_KBOOTLOG_CREATE);
  LOG_STATUS_IF_ERRORED(l_eStatus, "CreateKernelLog(SS_LOGGER_KBOOTLOG_CREATE)");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

void StopLoggingFunction(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  // PFDRECThread Stop
  EFrameworkunifiedStatus l_eStatus = g_PFDRECThread.Finalize(hApp);
  LOG_STATUS_IF_ERRORED(l_eStatus, "g_PFDRECThread.Finalize()");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}
