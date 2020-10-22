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
/// \ingroup  tag_SystemManager
/// \brief    This file provides support for IAT error event logging.
///
///////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/mman.h>
#include <sys/procfs.h>
#include <libgen.h>
#include <spawn.h>
#include <errno.h>
#include <sys/timeb.h>
#include <system_service/ss_system_manager_notifications.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_multithreading.h>
#include <system_service/ss_services.h>
#include <system_service/ss_client_names.h>
#include <system_service/ss_power_service_if.h>
#include <processlauncher/ProcessLauncher_if.h>
#include <processlauncher/ss_sm_process_launcher_protocol.h>
#include <processlauncher/ss_sm_process_launcher.h>
#include <heartbeat/ss_hb_if.h>
#include <system_service/ss_heartbeat_service_protocol.h>
#include <system_service/ss_power_service_notifications.h>
#include <system_service/ss_power_service_protocol.h>
#include <stub/ss_diag.h>
#include <system_service/ss_test_clients.h>
#include <native_service/ns_np_service_protocol.h>
#include <native_service/frameworkunified_application.h>

#include <system_service/ss_sm_thread_names.h>
#include <system_service/ss_templates.h>
#include <native_service/ns_plogger_if.h>
#include <native_service/frameworkunified_thread_priority.h>
#include <native_service/ns_np_service_nor_persistence.h>
#include <boost/bind.hpp>
#include <string>
#include "ss_system_manager.h"
#include "ss_sm_systemmanagerlog.h"
#include "ss_sm_default_paths.h"
#include "ss_sm_version.h"

template<typename C, eFrameworkunifiedStatus (C::*M)(HANDLE)>
  EFrameworkunifiedStatus SysMgrCallback(HANDLE hApp) {  // LCOV_EXCL_START 6: Can not set condition
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;
  C * pObj = static_cast<C *>(&CSystemManager::GetInstance());
  if (pObj) {
    l_eStatus = (pObj->*M)(hApp);
  }
  return l_eStatus;
}
// LCOV_EXCL_STOP

//******************************************************************************
void Init_EErrorEventReset_StrMap(std::map<EErrorEventResetType, SS_String> & m_strMap) {  // NOLINT
  MAP_ENTRY(m_strMap, eErrorEventResetTypeNone);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  MAP_ENTRY(m_strMap, eErrorEventResetTypeHard);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
}
class EnumStringMap<EErrorEventResetType, Init_EErrorEventReset_StrMap> m_oEErrorEventResetTypeMap;
SS_String GetStr(EErrorEventResetType f_enum) {  // LCOV_EXCL_START 6: Can not set condition
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  return m_oEErrorEventResetTypeMap.GetStr(f_enum);
}
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////////////////////////
/// \ingroup ErrorEventInit
///          This function initializes the error event handling subsystem.
///          This function MUST be called by System Manager initialization.
///
/// \param HANDLE  f_hApp  AGL FW application handle.
///
/// \return Status
///         EFrameworkunifiedStatus - success or error
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::ErrorEventInit(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  Timer *l_pTimer;

  m_isPrevErrEventCompleted = TRUE;
  m_errorEventResult = eFrameworkunifiedStatusOK;
  m_errorEventQueueLocked = FALSE;
  m_errorEventCurrentIter = m_errorEventQueue.end();

  l_pTimer = &m_errorEventTimers[eSM_ERROR_EVENT_TIMER_LOGGER_START_REQ];
  l_pTimer->Initialize(f_hApp, SS_SM_ERROR_EVENT_TIMER_ID_LOGGER_START_REQ,
      boost::bind(&CSystemManager::OnErrorEventLoggerStartRequestTimeout, this, _1));  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  l_pTimer = &m_errorEventTimers[eSM_ERROR_EVENT_TIMER_DEBUG_DUMP_RSPN];
  l_pTimer->Initialize(f_hApp, SS_SM_ERROR_EVENT_TIMER_ID_DEBUG_DUMP_RSPN,
      boost::bind(&CSystemManager::OnDebugDumpCompleteTimeout, this, _1));  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  l_pTimer = &m_errorEventTimers[eSM_ERROR_EVENT_TIMER_BOOT_MICRO_LOG_RSPN];
  l_pTimer->Initialize(f_hApp, SS_SM_ERROR_EVENT_TIMER_ID_BOOT_MICRO_LOG_RSPN,
      boost::bind(&CSystemManager::OnErrorEventBootMicroLogResponseTimeout, this, _1));  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  l_pTimer = &m_errorEventTimers[eSM_ERROR_EVENT_TIMER_CORE_FILE_POLL];
  l_pTimer->Initialize(f_hApp, SS_SM_ERROR_EVENT_TIMER_ID_CORE_FILE_POLL,
      boost::bind(&CSystemManager::OnErrorEventCoreFilePollTimeout, this, _1));  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup ErrorEventEnqueue
/// Conditional queue error events for processing by the error event state
/// machine.
///
/// \param f_hApp               Handle to AGL application.
///
/// \param f_eventType          Error event type.
///
/// \param f_moduleQueueName    Error event trigger module queue name.
///
/// \param f_resetType          Error event reset type (none, hard).
///
/// \param f_loggingInfo        Logging information if applicable (optional).
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::ErrorEventEnqueue(HANDLE f_hApp,
        EErrorEventType f_eventType, std::string &f_moduleQueueName,
        EErrorEventResetType f_resetType, const SMLoggingInfo &f_loggingInfo) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  ERROR_EVENT_INFO l_errorEventInfo;
  ERROR_EVENT_QUEUE_ITER l_errorEventIter;
  ERROR_EVENT_QUEUE_RET l_retVal;
  BOOL isErrorReset = FALSE;

  if (eErrorEventResetTypeHard == f_resetType) {
    // Forced reset if shutdown process is not completed after Reboot requesting
    m_GroupLaunchTimer->StartTimer(
        m_aTimerIDs[eSM_TIMER_SHUTDOWN_COMPLETE_MONITOR],
        SM_SHUTDOWN_COMPLETE_MONITOR_TIMEOUT, 0, 0, 0);

      switch (f_loggingInfo.resetReason) {
      case e_SS_SM_CPU_RESET_REASON_GENERIC_ERR:
      case e_SS_SM_CPU_RESET_REASON_DSP_ERR:
        isErrorReset = TRUE;
        break;
      default:
        break;
      }
    // LCOV_EXCL_START 6:This variable cannot be modified by external API
    if ((m_ResetCount > SS_SM_ERR_NAVI_RESET_LIMIT) && isErrorReset) {
    // LCOV_EXCL_BR_STOP
      // LCOV_EXCL_START 6:This variable cannot be modified by external API (evaluated only initial value)
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      fprintf(stderr,
          "SS_SysManager/%s:will NAVIDET as continuous error(%d) \n", __FUNCTION__, m_ResetCount);
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "will NAVIDET as continuous error(%d)", m_ResetCount);
      m_pfStopCompleteHandler = (SysMgrCallback<CSystemManager, &CSystemManager::perform_force_reset>);
      // LCOV_EXCL_STOP
    }
  }

  if (TRUE == m_errorEventQueueLocked) {
    l_eStatus = eFrameworkunifiedStatusAccessError;
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__,
        " Warning. The Error Event Queue has been locked. No further items may be enqueued. System reset pending.");  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
  } else {
    GetTimeStamp(l_errorEventInfo.m_eventEnqueueTimeStamp);

    l_errorEventInfo.m_moduleQueueName = f_moduleQueueName;
    l_errorEventInfo.m_eventType = f_eventType;
    l_errorEventInfo.m_errorEventResetType = f_resetType;
    l_errorEventInfo.m_loggingInfo = f_loggingInfo;
    l_errorEventInfo.m_prio = eErrorEventPrioDefault;  // Make Prios of all ErrorEvent first-win as default
                              // It needs to add eErrorEventPrioxxx properly if design modification about priority required

    if (eErrorEventResetTypeNone != f_resetType) {
      FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, " System will reset after logging");
    }

    l_retVal = m_errorEventQueue.insert(l_errorEventInfo);
    if (FALSE == l_retVal.second) {
      std::string l_errorEventName = GetStr(l_errorEventInfo.m_eventType);
      TEXT(__FUNCTION__,  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
          " ERROR EVENT: %s is already queued or is in progress. Event dropped.",  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
          l_errorEventName.c_str());  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
    }

    l_eStatus = ErrorEventProcessNext(f_hApp);
    LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus, "ErrorEventProcessNext()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup PerformResetAfterLogging
/// perform CPU reset after logging complete
///
/// \param f_hApp             Handle to AGL application.
///        f_eCpuResetReason  CPU reset reason
///        f_messageStr       Error message string
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///
///
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::PerformResetAfterLogging(HANDLE f_hApp,
        ESMCpuResetReason f_eCpuResetReason, std::string f_messageStr) {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
    EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

    l_eStatus = PerformCpuReset(f_hApp, f_eCpuResetReason, f_messageStr);
    LOG_STATUS_REC_HIST_IF_ERRORED(l_eStatus, "PerformCpuReset()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
    // LCOV_EXCL_BR_START 200: Can not satisfy the condition for calling from caller
    if (eFrameworkunifiedStatusOK != l_eStatus) {
    // LCOV_EXCL_BR_STOP
      // LCOV_EXCL_START 200: Can not satisfy the condition for calling from caller
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      SS_ASERT_LOG(0, "PerformCpuReset() : %d", l_eStatus);
      // Erasing ErrorEvent only if resetting process is failed
      m_errorEventQueue.erase(m_errorEventCurrentIter);  // Erase last processed event.
      // LCOV_EXCL_STOP
    } else {
      m_errorEventQueueLocked = TRUE;  // Permanent lock until post reset.
      m_errorEventQueue.clear();  // Drop queued events.
      SendDeferMsg(f_hApp);
    }

    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
    return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup ErrorEventProcessNext
/// Process the next error event from error event queue.
///
/// \param f_hApp  Handle to AGL application.
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///
/// \Notes (1) Removal from the queue occurs AFTER the entry has been processed
///            AND just before the NEXT entry is to be obtained.  Therefore,
///            the first item enqueued MUST not be removed since it has not
///            been processed.  This is ensured by setting the iterator to END
///            during module initialization. After processing the first event,
///            the iterator will no longer be END until all events have been
///            processed.
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::ErrorEventProcessNext(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  ERROR_EVENT_QUEUE_ITER l_errorEventIter;
  SMErrorEventNtfData l_errorEventNtfData;

  if (TRUE == m_isPrevErrEventCompleted) {  // Remove last completed item from front of queue.
    // See Note #1.
    if ((0 != m_errorEventQueue.size()) &&
        (m_errorEventQueue.end() != m_errorEventCurrentIter)) {
      // Completed logging by ErrorEvent
      switch (m_errorEventCurrentIter->m_errorEventResetType) {
      case eErrorEventResetTypeNone:
        switch (m_errorEventCurrentIter->m_eventType) {
        case eErrorEventTypeInterfaceunifiedEmmcLogs:
        case eErrorEventTypeUserInvokedClearLogs:
          if (eFrameworkunifiedStatusOK == m_errorEventResult) {
            if (PowerHalSetResetInfo(AGL_ERRLOG_COUNTER, 0)) {
              // Logging only.
              FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                      "Could not reset AGL_ERRLOG_COUNTER");
            }
          }
          break;
        default:
          break;
        }

        if (eErrorEventTypeInterfaceunifiedEmmcLogs == m_errorEventCurrentIter->m_eventType) {
          HANDLE hSession = FrameworkunifiedMcOpenSender(f_hApp,
              m_errorEventCurrentIter->m_moduleQueueName.c_str());

          if (hSession == NULL) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
            // LCOV_EXCL_START 4: NSFW error case
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            SS_ASERT(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
            // LCOV_EXCL_STOP
          } else {
            if (eFrameworkunifiedStatusOK  // LCOV_EXCL_BR_LINE 4: NSFW error case
                != FrameworkunifiedSendMsg(hSession,
                    SS_SM_ERROR_EVENT_INTERFACEUNIFIED_EMMC_LOGS_RSPN,
                    sizeof(m_errorEventResult),
                    &m_errorEventResult)) {
              // LCOV_EXCL_START 4: NSFW error case
              AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
              SS_ASERT(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
              // LCOV_EXCL_STOP
            }
            if (eFrameworkunifiedStatusOK != FrameworkunifiedMcClose(hSession)) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
              // LCOV_EXCL_START 4: NSFW error case
              AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
              SS_ASERT(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
              // LCOV_EXCL_STOP
            }
          }
        }
        m_errorEventQueue.erase(m_errorEventCurrentIter);  // Erase last processed event.
        break;
      case eErrorEventResetTypeHard:
        TEXT(__FUNCTION__, "ERROR EVENT: Hard reset required for previously processed error event."  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
            "Initiating reset and flushing logging queue.");  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)

        if (eFrameworkunifiedStatusOK != m_errorEventResult) {
          // Logging at next launching by setting L to NaviDet if logging failed before rebooting
          fprintf(stderr,
              "SS_SysManager/%s:will NAVIDET as LOG save failed\n", __FUNCTION__);
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
              "will NAVIDET as LOG save failed");
          m_pfStopCompleteHandler = (SysMgrCallback<CSystemManager,
              &CSystemManager::perform_force_reset>);
        }

        if ((m_errorEventCurrentIter->m_loggingInfo.resetReason == e_SS_SM_CPU_RESET_REASON_CRITICAL_ERR) ||
            (m_errorEventCurrentIter->m_loggingInfo.resetReason == e_SS_SM_CPU_RESET_REASON_GENERIC_ERR) ||
            (m_errorEventCurrentIter->m_loggingInfo.resetReason == e_SS_SM_CPU_RESET_REASON_DSP_ERR) ||
            (m_errorEventCurrentIter->m_loggingInfo.resetReason == e_SS_SM_CPU_RESET_REASON_USER_FORCE_RESET)) {
          BOOL l_user_reset = FALSE;
          if (m_errorEventCurrentIter->m_loggingInfo.resetReason == e_SS_SM_CPU_RESET_REASON_USER_FORCE_RESET) {
            l_user_reset = TRUE;
          }
          m_ResetFactor = GetResetFactor(m_errorEventCurrentIter->m_moduleQueueName.c_str(), l_user_reset);
        }

        l_eStatus = PerformResetAfterLogging(f_hApp,
            m_errorEventCurrentIter->m_loggingInfo.resetReason,
            m_errorEventCurrentIter->m_loggingInfo.messageStr);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
        LOG_STATUS_REC_HIST_IF_ERRORED(l_eStatus,  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
            "PerformResetAfterLogging()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)

        break;
      // LCOV_EXCL_START 6:Can not satisfy condition to branch
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      default:
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
            " Error. Unknown reset type encountered: 0x%X/%d. No action taken.",
            m_errorEventCurrentIter->m_errorEventResetType,
            m_errorEventCurrentIter->m_errorEventResetType);
        break;
      // LCOV_EXCL_STOP
      }
    }

    // Check if a new work item exists.
    if (0 != m_errorEventQueue.size()) {
      // ErrorEvent startup process
      m_errorEventCurrentIter = m_errorEventQueue.begin();

      l_errorEventNtfData.EventType = m_errorEventCurrentIter->m_eventType;

      if (eErrorEventResetTypeHard == m_errorEventCurrentIter->m_errorEventResetType) {
        l_errorEventNtfData.isNeedReboot = TRUE;
      } else {
        l_errorEventNtfData.isNeedReboot = FALSE;
      }

      // Check BufferSize
      size_t modl_len = sizeof(l_errorEventNtfData.ModuleName) - 1;

      // SS Logger uses the l_errorEventNtfData.ModuleName as the LOGGERSERVICE_EMMC_LOGS and
      // EEL_EXPORT destination path. eErrorEventTypeDiagEvent sends the destination path.
      // For all other event types, the binary name is sent to logger.
      // LCOV_EXCL_BR_START 6: Because the applicable variable cannot be changed from the external API (only evaluated by the initial value)
      if ((eErrorEventTypeInterfaceunifiedEmmcLogs
          == m_errorEventCurrentIter->m_eventType) ||
          (eErrorEventTypeEelExport
              == m_errorEventCurrentIter->m_eventType) ||
          (eErrorEventTypeDiagEvent
              == m_errorEventCurrentIter->m_eventType)) {
      // LCOV_EXCL_BR_STOP
        strncpy(l_errorEventNtfData.ModuleName,
            m_errorEventCurrentIter->m_loggingInfo.path.c_str(),
            sizeof(l_errorEventNtfData.ModuleName) - 1);

        size_t path_len = strlen(m_errorEventCurrentIter->m_loggingInfo.path.c_str());
        if (path_len > modl_len) {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
              "Input buffer size over. input : %ld, enable : %ld",
              path_len, modl_len);
        }
        l_errorEventNtfData.ModuleName[sizeof(l_errorEventNtfData.ModuleName) - 1] = 0;
      } else {
        strncpy(l_errorEventNtfData.ModuleName,
            m_errorEventCurrentIter->m_loggingInfo.suffixStr.c_str(),
            sizeof(l_errorEventNtfData.ModuleName) - 1);
      }

      // When the Reboot process is started during the execution of the LOG save sequence, an undefined value was set
      // in m_errorEventCurrentIter. Therefore, the Reboot start timing due to an error in SS_Loger was changed from
      // the time of ErrorEvent queuing to the time of dequeuing.

      // LCOV_EXCL_BR_START 6: Because the following conditions cannot be satisfied when called from an external API
      if ((SERVICE_LOGGER == m_errorEventCurrentIter->m_moduleQueueName)
          && ((eErrorEventTypeProcessCrash
              == m_errorEventCurrentIter->m_eventType)
              || (eErrorEventTypeProcessExit
                  == m_errorEventCurrentIter->m_eventType)
              || (eErrorEventTypeHeartBeatFailure
                  == m_errorEventCurrentIter->m_eventType)
              || (eErrorEventTypeModConnFailed
                  == m_errorEventCurrentIter->m_eventType)
              || (eErrorEventTypeStartRespFailed
                  == m_errorEventCurrentIter->m_eventType))) {
      // LCOV_EXCL_BR_STOP
        // LCOV_EXCL_START 6: Because the following conditions cannot be satisfied when called from an external API
        // Note: If logger crashes, a core file will exist.
        std::string l_hmiDispStr;
        l_hmiDispStr = SERVICE_LOGGER;
        if (eErrorEventResetTypeNone != m_errorEventCurrentIter->m_errorEventResetType) {
          l_hmiDispStr += " - Shutting down, check for core file.";

          // Since LOG save cannot be performed when an SSL error is detected, the LOG is saved at the next startup
          // by rebooting with NaviDet = L after completion of the process.
          fprintf(stderr, "SS_SysManager/%s:will NAVIDET as SSL dead\n", __FUNCTION__);
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "will NAVIDET as SSL dead");
          m_pfStopCompleteHandler = (SysMgrCallback<CSystemManager,
              &CSystemManager::perform_force_reset>);

          m_ResetFactor = GetResetFactor(SERVICE_LOGGER, FALSE);

          l_eStatus = PerformResetAfterLogging(f_hApp,
              e_SS_SM_CPU_RESET_REASON_GENERIC_ERR);
          LOG_STATUS_REC_HIST_IF_ERRORED(l_eStatus, "PerformResetAfterLogging()");
        } else {
          l_hmiDispStr += " - Check for core file.";
        }

        SMErrorEventNtfData l_errorEventNtfData;
        l_errorEventNtfData.EventType = m_errorEventCurrentIter->m_eventType;
        l_errorEventNtfData.isNeedReboot = FALSE;
        snprintf(l_errorEventNtfData.ModuleName,
            sizeof(l_errorEventNtfData.ModuleName), "%s",
            l_hmiDispStr.c_str());

        l_eStatus = FrameworkunifiedPublishEvent(f_hApp,  // Show indication on HMI.
            SS_SM_EVENT_ERROR,
            NULL, &l_errorEventNtfData, sizeof(l_errorEventNtfData));

        LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus,
            "FrameworkunifiedPublishEvent(SS_SM_EVENT_ERROR)");
      } else if (m_errorEventCurrentIter->m_eventType == eErrorEventTypeModuleInvokedResetRequest
          && (m_errorEventCurrentIter->m_loggingInfo.resetReason == e_SS_SM_CPU_RESET_REASON_NORMAL
              || m_errorEventCurrentIter->m_loggingInfo.resetReason == e_SS_SM_CPU_RESET_REASON_DATA_RESET)) {
        l_eStatus = PerformResetAfterLogging(f_hApp,
            m_errorEventCurrentIter->m_loggingInfo.resetReason,
            m_errorEventCurrentIter->m_loggingInfo.messageStr);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
        LOG_STATUS_REC_HIST_IF_ERRORED(l_eStatus, "PerformResetAfterLogging()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
      } else {
        BOOL bIsNeedLogging = TRUE;

        {
          UI_32 l_ErrLogCount = 0;
          if (PowerHalGetResetInfo(AGL_ERRLOG_COUNTER, &l_ErrLogCount)) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
              "Could not get AGL_ERRLOG_COUNTER form power_hal, assume to 0.");
            l_ErrLogCount = 0;
          }

          {
            if ((l_ErrLogCount >= SS_SM_ERR_LOGGING_LIMIT)
                && (eErrorEventResetTypeHard == m_errorEventCurrentIter->m_errorEventResetType)) {
              fprintf(stderr, "SS_SysManager/%s:Skip Logging by continuous error \n", __FUNCTION__);
              FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Skip Logging by continuous error ");

              m_ResetFactor = PSM_FACTOR_AGL_WITH_HISTORY;

              l_eStatus =
                  PerformResetAfterLogging(f_hApp,
                      m_errorEventCurrentIter->m_loggingInfo.resetReason,
                      m_errorEventCurrentIter->m_loggingInfo.messageStr);
              LOG_STATUS_REC_HIST_IF_ERRORED(l_eStatus, "PerformResetAfterLogging()");
              bIsNeedLogging = FALSE;
            } else if (eErrorEventResetTypeHard
                == m_errorEventCurrentIter->m_errorEventResetType) {
              l_ErrLogCount++;
              if (PowerHalSetResetInfo(AGL_ERRLOG_COUNTER, l_ErrLogCount)) {
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                  "Could not set AGL_ERRLOG_COUNTER to power_hal.");
              }
            }
          }
        }

        if (bIsNeedLogging) {
          m_isPrevErrEventCompleted = FALSE;
          m_errorEventResult = eFrameworkunifiedStatusOK;
          BOOL result =
              m_errorEventTimers[eSM_ERROR_EVENT_TIMER_LOGGER_START_REQ].Start(
                  SS_ERROR_EVENT_START_REQ_TO_SEC, 0, 0, 0);
          if (FALSE == result) {  // LCOV_EXCL_BR_LINE 6: Because the applicable variable cannot be changed from the external API (only evaluated by the initial value)
            // LCOV_EXCL_START 6: Because the applicable variable cannot be changed from the external API (only evaluated by the initial value)
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                " Error. Failed to start timer eSM_ERROR_EVENT_TIMER_LOGGER_START_REQ.");
            // LCOV_EXCL_STOP
          }

          l_eStatus = FrameworkunifiedPublishEvent(f_hApp,
              SS_SM_EVENT_ERROR_TO_SSL,
              NULL, &l_errorEventNtfData,
              sizeof(l_errorEventNtfData));

          if (l_eStatus != eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
            // LCOV_EXCL_START 4: NSFW error case.
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
                " FrameworkunifiedPublishEvent(SS_SM_EVENT_ERROR_TO_SSL, "
                    "module name (optional): '%s', "
                    "event type: %d) returned '%d'/'%s'",
                l_errorEventNtfData.ModuleName,
                l_errorEventNtfData.EventType, l_eStatus,
                GetStr(l_eStatus).c_str());
            // LCOV_EXCL_STOP 4: NSFW error case.
          }
        }
      }
    }

    // If Reboot has been started up to this point, m_errorEventQueue is cleared and checking is performed at the end because m_errorEventQueue is cleared.
    if (0 == m_errorEventQueue.size()) {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
          " Info. Error Event queue empty. Error logging complete.");
      m_errorEventCurrentIter = m_errorEventQueue.end();  // Invalidate iterator in case of spurious events.
      SendDeferMsg(f_hApp);
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnLoggingStartRequestFromSL
///          This function gets called when System Logger initiates artifact
///          collection for a trigger source that originates within System
///          Logger or as a result of System Manager raising a logging
///          event notification to System Logger.
///
/// \param [in] hApp
///         HANDLE - Handle of the Client Application
///
/// \return Status
///         EFrameworkunifiedStatus - success or error
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnLoggingStartRequest(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  ModuleLaunchListIter l_ModuleListIter;
  HANDLE l_hLoggerServiceSession;
  EErrorEventType l_eventType;
  SMErrorEventNtfData l_errorEventNtfData;

  m_errorEventTimers[eSM_ERROR_EVENT_TIMER_LOGGER_START_REQ].Stop();

  INTERFACEUNIFIEDLOG_RECEIVED_FROM(f_hApp);

  if (eFrameworkunifiedStatusOK
      != (l_eStatus = ReadMsg < EErrorEventType > (f_hApp, l_eventType))) {
    LOG_ERROR("ReadMsg()");
  } else {
    if (0 == m_errorEventQueue.size()) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
          " Error. Logging start request received for event type: %d/'%s', "
              "but the logging queue is empty.!", l_eventType,
          GetStr(l_eventType).c_str());
      l_eStatus = eFrameworkunifiedStatusFail;
    } else if (m_errorEventCurrentIter == m_errorEventQueue.end()) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
          " Error. Logging start request received for event type: %d/'%s', "
              "but the current event iterator is invalid.",
          l_eventType, GetStr(l_eventType).c_str());
      l_eStatus = eFrameworkunifiedStatusFail;
    } else if (m_errorEventCurrentIter->m_eventType != l_eventType) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
          " Error. Logging start request received for event type: %d/'%s', "
              "but current work item is for event type: %d/'%s'.",
          l_eventType, GetStr(l_eventType).c_str(),
          m_errorEventCurrentIter->m_eventType,
          GetStr(m_errorEventCurrentIter->m_eventType).c_str());
      l_eStatus = eFrameworkunifiedStatusFail;
    } else {
      l_errorEventNtfData.EventType = l_eventType;
      l_errorEventNtfData.isNeedReboot = FALSE;

      // For boot micro reset, customize the string as per the reset reason.
      if (l_eventType == eErrorEventTypeBootMicroReset) {
        switch (m_BootMicroResetReason) {
        case SS_SM_BOOT_MICRO_RESET_REASON_SELF_RESET:
          (void) snprintf(l_errorEventNtfData.ModuleName,
              sizeof(l_errorEventNtfData.ModuleName),
              "Boot Micro Reset. Reason: %d, Self Reset",
              m_BootMicroResetReason);
          break;

        case SS_SM_BOOT_MICRO_RESET_REASON_USER_FORCE_RESET:  // User Force Reset timeout.
          (void) snprintf(l_errorEventNtfData.ModuleName,
              sizeof(l_errorEventNtfData.ModuleName),
              "Boot Micro Reset. Reason: %d, UFR Timeout",
              m_BootMicroResetReason);
          break;

        case SS_SM_BOOT_MICRO_RESET_REASON_DSP_RESET:
          (void) snprintf(l_errorEventNtfData.ModuleName,
              sizeof(l_errorEventNtfData.ModuleName),
              "Boot Micro Reset. Reason: %d, DSP Reset",
              m_BootMicroResetReason);
          break;

        case SS_SM_BOOT_MICRO_RESET_REASON_HB_TIMEOUT:
          (void) snprintf(l_errorEventNtfData.ModuleName,
              sizeof(l_errorEventNtfData.ModuleName),
              "Boot Micro Reset. Reason: %d, HB Timeout",
              m_BootMicroResetReason);
          break;

        default:
          (void) snprintf(l_errorEventNtfData.ModuleName,
              sizeof(l_errorEventNtfData.ModuleName),
              "Boot Micro Reset. Reason: %d, Unknown",
              m_BootMicroResetReason);
          break;
        }
      } else {
        // All other error event types format =
        //     Binary Name <circumstantial> PID: TID: Additional info: </circumstantial>
        std::string l_hmiDispStr = ErrorEventCreateHmiDisplayString();

        (void) snprintf(l_errorEventNtfData.ModuleName,
            sizeof(l_errorEventNtfData.ModuleName), "%s",
            l_hmiDispStr.c_str());
      }

      l_eStatus = FrameworkunifiedPublishEvent(f_hApp, SS_SM_EVENT_ERROR,  // To HMI
          NULL, &l_errorEventNtfData, sizeof(l_errorEventNtfData));

      LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus,
          "FrameworkunifiedPublishEvent(SS_SM_EVENT_ERROR)");
    }

    if (eFrameworkunifiedStatusOK
        != (l_eStatus = GetModuleIterator(SERVICE_LOGGER, l_ModuleListIter))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
          " Error: GetModuleIterator(%s) errored: %d/'%s'",
          SERVICE_LOGGER, l_eStatus, GetStr(l_eStatus).c_str());
    } else {
      l_hLoggerServiceSession = l_ModuleListIter->hsession;

      l_eStatus = FrameworkunifiedSendMsg(l_hLoggerServiceSession,
          SS_SM_ERROR_EVENT_LOGGING_START_RSPN, 0, NULL);

      LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus,
          "FrameworkunifiedSendMsg(SS_SM_ERROR_EVENT_LOGGING_START_RSPN)");
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnErrorEventLoggerStartRequestTimeout
///
/// \brief This function is called if System Logger fails to respond in a
///        timely fashion to the published logger start request event
///        SS_SM_EVENT_ERROR_TO_SSL. See OnLoggingStartRequest() which
///        is the expected path should the timer period NOT expire.
///
/// \param [in] hApp
///         HANDLE - Handle of the Client Application
///
/// \return Status
///         EFrameworkunifiedStatus - success or error
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnErrorEventLoggerStartRequestTimeout(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  SMErrorEventNtfData l_errorEventNtfData;

  FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
    " Error. Logger failed to respond to message SS_SM_EVENT_ERROR_TO_SSL in a timely fashion.  Logging aborted.");

  // Publish to HMI even if the error event will not be processed.
  // In this way, developers can be made aware of the error condition
  // and determine why logging did not proceed as expected.
  if (m_errorEventCurrentIter != m_errorEventQueue.end()) {
    l_errorEventNtfData.EventType = m_errorEventCurrentIter->m_eventType;
    l_errorEventNtfData.isNeedReboot = FALSE;

    std::string l_hmiDispStr = ErrorEventCreateHmiDisplayString();
    snprintf(l_errorEventNtfData.ModuleName,
        sizeof(l_errorEventNtfData.ModuleName), "%s.", l_hmiDispStr.c_str());

    l_eStatus = FrameworkunifiedPublishEvent(f_hApp, SS_SM_EVENT_ERROR,  // To HMI
        NULL, &l_errorEventNtfData, sizeof(l_errorEventNtfData));  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

    LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus, "FrameworkunifiedPublishEvent(SS_SM_EVENT_ERROR)");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
  }

  m_isPrevErrEventCompleted = TRUE;
  m_errorEventResult = eFrameworkunifiedStatusFail;
  l_eStatus = ErrorEventProcessNext(f_hApp);
  LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus, "ErrorEventProcessNext()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

EFrameworkunifiedStatus CSystemManager::OnErrorEventLoggingComplete(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  HANDLE l_hLoggerServiceSession;
  ModuleLaunchListIter l_ModuleListIter;

  // If logger does not send a logging start and instead immediately
  // sends a logging complete response, then the start timer must be
  // stopped to prevent a start response timeout from occuring.
  m_errorEventTimers[eSM_ERROR_EVENT_TIMER_LOGGER_START_REQ].Stop();

  m_errorEventResult = eFrameworkunifiedStatusFail;
  if (sizeof(m_errorEventResult) != FrameworkunifiedGetMsgLength(f_hApp)) {
    LOG_ERROR("DataSize mismatch");
  } else if (eFrameworkunifiedStatusOK
      != (l_eStatus = FrameworkunifiedGetMsgDataOfSize(f_hApp, &m_errorEventResult,
          sizeof(m_errorEventResult), eSMRRelease))) {
    LOG_ERROR("FrameworkunifiedGetMsgDataOfSize()");
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "m_errorEventResult = %d", m_errorEventResult);
  }
  m_isPrevErrEventCompleted = TRUE;

  if (eFrameworkunifiedStatusOK
      != (l_eStatus = GetModuleIterator(SERVICE_LOGGER, l_ModuleListIter))) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        " Error: GetModuleIterator(%s) errored: %d/'%s'",
        SERVICE_LOGGER, l_eStatus, GetStr(l_eStatus).c_str());
  } else {
    l_hLoggerServiceSession = l_ModuleListIter->hsession;

    l_eStatus = FrameworkunifiedSendMsg(l_hLoggerServiceSession,
        SS_SM_ERROR_EVENT_LOGGING_COMPLETE_RSPN, 0, NULL);

    LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus,
        "FrameworkunifiedSendMsg(SS_SM_ERROR_EVENT_LOGGING_START_RSPN)");
  }

  FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, " Info. Done processing error event %s.",
      GetStr(m_errorEventCurrentIter->m_eventType).c_str());

  l_eStatus = ErrorEventProcessNext(f_hApp);
  LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus, "ErrorEventProcessNext()");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup ActOnModuleFailure
///          This function is called to take appropriate action of the failure
///          reported by heart beat or crash detector. Attributes set in
///          Launch configuration file are considered while taking action
///          on the failed module.
///
/// \param  f_hApp                  AGL FW application handle.
///         f_moduleName            Failed module name.
///         f_refSoftResetRequired  Return parameter indicating if a
///                                 soft reset will be required.
/// \return Status
///         EFrameworkunifiedStatus - success or error
///////////////////////////////////////////////////////////////////////////////
const CHAR RestartModule[] = "me";

EFrameworkunifiedStatus CSystemManager::ActOnModuleFailure(HANDLE f_hApp,
        SS_String f_moduleName, BOOL &f_refResetRequired) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  ModuleLaunchListIter l_ModuleListIter;
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  ESMPLThreadCommandIds l_PLCmd = ePLThrdCmd_NONE;
  SS_String l_PLCmdStr = "";

  f_refResetRequired = FALSE;

  l_eStatus = GetModuleIterator(f_moduleName.c_str(), l_ModuleListIter);
  if (l_eStatus != eFrameworkunifiedStatusOK) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        " Error: Module %s not found in Group Launch Map", f_moduleName.c_str());

    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
    return (eFrameworkunifiedStatusDbRecNotFound);
  }

  // LCOV_EXCL_BR_START 6: Because the condition cannot be set
  if (eFrameworkunifiedStatusOK
      != (l_eStatus = RemoveModuleEntryFromHB(f_hApp,
          l_ModuleListIter->name.c_str()))) {
  // LCOV_EXCL_BR_STOP
    // LCOV_EXCL_START 6: Because the condition cannot be set
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        " Error: RemoveModuleEntryFromHB(%s) errored: %d/'%s'",
        l_ModuleListIter->name.c_str(), l_eStatus,
        GetStr(l_eStatus).c_str());
    // LCOV_EXCL_STOP
  }

  if (TRUE == l_ModuleListIter->critical
      && (l_ModuleListIter->relaunch_count >= l_ModuleListIter->retry_cnt)) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        " Error: Critical module '%s' re-launch counter exceeded limit (%d).",
        l_ModuleListIter->name.c_str(), l_ModuleListIter->retry_cnt);

    l_ModuleListIter->SetModuleState(MODULE_STATE_SKIPPED);

    // Module has crossed re-launched limit and is not going to be re-launched.
    // It must however still be terminated.
    l_PLCmd = ePLThrdCmd_TERMINATE_MODULE_REQST;
    l_PLCmdStr = "ePLThrdCmd_TERMINATE_MODULE_REQST";

    // Module is critical. Restart system.
    f_refResetRequired = TRUE;
  // Restart required?
  } else if (0 == std::strcmp(l_ModuleListIter->restart.c_str(), RestartModule)) {  // LCOV_EXCL_BR_LINE 6: Because the condition cannot be set // NOLINT(whitespace/line_length)
    // LCOV_EXCL_START 6: Because the condition cannot be set
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "Module: %s, relaunch_count: %d",
        l_ModuleListIter->name.c_str(),
        l_ModuleListIter->relaunch_count);

    if (l_ModuleListIter->relaunch_count < l_ModuleListIter->retry_cnt) {
      SS_String l_ModulePath = (m_BinaryFilesPath + l_ModuleListIter->path);

      FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__,
          " Requesting ReLaunch of '%s' '%s'",
          l_ModuleListIter->name.c_str(), l_ModulePath.c_str());

      l_PLCmd = ePLThrdCmd_RELAUNCH_MODULE_REQST;
      l_PLCmdStr = "ePLThrdCmd_RELAUNCH_MODULE_REQST";
      l_ModuleListIter->relaunch_count++;
      l_ModuleListIter->relaunch_status = RelaunchErr;
    } else if ((FALSE == l_ModuleListIter->critical)
        && (l_ModuleListIter->relaunch_count
            >= l_ModuleListIter->retry_cnt)) {
      FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__,
          " Warning: Skipping re-launch of non-critical %s module, "
              "re-launch counter exceeded limit(%d). Terminating module.",
          l_ModuleListIter->name.c_str(),
          l_ModuleListIter->retry_cnt);

      l_ModuleListIter->SetModuleState(MODULE_STATE_SKIPPED);

      // non critical module has crossed re-launched limit and is not going to be re-launched.
      // We must terminate the module in the event that it has become non responsive.
      l_PLCmd = ePLThrdCmd_TERMINATE_MODULE_REQST;
      l_PLCmdStr = "ePLThrdCmd_TERMINATE_MODULE_REQST";
    }
    // LCOV_EXCL_STOP
  } else {  // LCOV_EXCL_BR_LINE 6: Because the condition cannot be set
    // LCOV_EXCL_START 6: Because the condition cannot be set
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__,
        " Warning: %s is not required to be re-launched, "
            "skipping re-launch and terminating module.",
        l_ModuleListIter->name.c_str());

    l_ModuleListIter->SetModuleState(MODULE_STATE_SKIPPED);

    // Module restart not required.
    // We must terminate the module in the event that it has become non responsive.
    l_PLCmd = ePLThrdCmd_TERMINATE_MODULE_REQST;
    l_PLCmdStr = "ePLThrdCmd_TERMINATE_MODULE_REQST";
    // LCOV_EXCL_STOP
  }


  if (ePLThrdCmd_NONE != l_PLCmd) {
    l_eStatus = SendRequestToLauncher(f_hApp, l_ModuleListIter, l_PLCmd, l_PLCmdStr);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

    LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus, "SendRequestToLauncher()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup ErrorEventCreateHmiDisplayString
///
/// \brief  Create error event HMI colored square display string.
///
/// \return Error Display String.
///////////////////////////////////////////////////////////////////////////////
std::string CSystemManager::ErrorEventCreateHmiDisplayString(void) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  std::stringstream l_displayNamess;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  std::stringstream l_pidss;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  std::stringstream l_hmiDispStr;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  if (0 != strncmp("",
          m_errorEventCurrentIter->m_loggingInfo.binaryFileName,
          sizeof(m_errorEventCurrentIter->m_loggingInfo.binaryFileName))) {
    l_displayNamess << m_errorEventCurrentIter->m_loggingInfo.binaryFileName;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  } else {
    l_displayNamess << m_errorEventCurrentIter->m_moduleQueueName;
  }

  if (m_errorEventCurrentIter->m_loggingInfo.pid != 0) {
    l_pidss << " PID: " << m_errorEventCurrentIter->m_loggingInfo.pid;
  }


  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_hmiDispStr.str());
}  // LCOV_EXCL_BR_LINE 10:The final line

