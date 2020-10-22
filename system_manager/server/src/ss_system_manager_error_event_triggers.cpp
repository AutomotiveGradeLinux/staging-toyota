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
#include <string.h>
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
#include <native_service/cl_process.h>
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
#include <string>
#include "ss_system_manager.h"
#include "ss_sm_systemmanagerlog.h"
#include "ss_sm_signals.h"
#include "ss_sm_default_paths.h"
#include "ss_sm_version.h"

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnHeartBeatErrorDetected
///          This function gets called when a heartbeat failure is detected.
///
/// \param [in] f_hApp
///         HANDLE - Handle of the Client Application
///
/// \return Status
///         EFrameworkunifiedStatus - success or error
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnHeartBeatErrorDetected(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  BOOL l_resetRequired;
  EErrorEventResetType l_resetType;
  SMLoggingInfo l_loggingInfo;
  std::string l_moduleBinaryName;
  UI_32 l_ix;
  THbReportData l_HBReport;

  INTERFACEUNIFIEDLOG_RECEIVED_FROM(f_hApp);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg < THbReportData > (f_hApp, l_HBReport))) {
    LOG_ERROR("ReadMsg()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Report contains : %d entries.", l_HBReport.nNumOfModules);

    for (l_ix = 0; l_ix < l_HBReport.nNumOfModules; l_ix++) {
      if (HB_STATUS_TIMEOUT == l_HBReport.tModuleList[l_ix].ProcHBState) {
        std::string l_moduleQueueName = l_HBReport.tModuleList[l_ix].ProcQueueName;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

        m_HBReport.eEntireState = l_HBReport.eEntireState;
        m_HBReport.nNumOfModules = l_HBReport.nNumOfModules;
        memcpy(&m_HBReport.tModuleList[l_ix], &l_HBReport.tModuleList[l_ix], sizeof(TSmModuleInfo));

        TEXT(__FUNCTION__,  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
             " ERROR EVENT: Heartbeat error detected from: %s.",  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
             l_moduleQueueName.c_str());  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)

        l_eStatus = GetBinaryNameOfProcess(l_moduleQueueName, l_moduleBinaryName);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

        if (eFrameworkunifiedStatusOK != l_eStatus) {
          FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__,
                 " Warning. Unable to identify binary name for SM child process with queue name: %s."
                 " The process name will be missing from the .csv and the core file will not be archived.",
                 l_moduleQueueName.c_str());
        } else {
          strncpy(l_loggingInfo.binaryFileName,
                  l_moduleBinaryName.c_str(),
                  sizeof(l_loggingInfo.binaryFileName) - 1);
        }

        if (eFrameworkunifiedStatusOK != (l_eStatus = ActOnModuleFailure(f_hApp, l_moduleQueueName, l_resetRequired))) {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                 " Error. ActOnModuleFailure(%s,%d) errored: %d/'%s'",
                 l_moduleQueueName.c_str(), l_resetRequired,
                 l_eStatus, GetStr(l_eStatus).c_str());
        }

        l_resetType =
            (FALSE == l_resetRequired) ?
                eErrorEventResetTypeNone :
                eErrorEventResetTypeHard;

        if (eErrorEventResetTypeNone != l_resetType) {
          l_loggingInfo.resetReason =
              e_SS_SM_CPU_RESET_REASON_GENERIC_ERR;
        }

        l_eStatus = ErrorEventEnqueue(f_hApp,
            eErrorEventTypeHeartBeatFailure, l_moduleQueueName,
            l_resetType, l_loggingInfo);

        LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus,  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
            "ErrorEventEnqueue(eErrorEventTypeHeartBeatFailure)");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
      }
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnProcessTermDetected
///
///
///
/// \param Handle to AGL application.
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnProcessTermDetected(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  int l_clRet;
  int l_count = 0;

  // When multiple child processes terminate, CL_ProcessCleanup() returns 1. If 1 is returned, it repeats.
  do {
    CL_ProcessCleanupInfo_t l_procInfo;

    l_count++;

    l_clRet = CL_ProcessCleanup(m_ClProcessSigFd, &l_procInfo);
    if (l_clRet != 0 && l_clRet != 1) {
      FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__,
          " Warning: CL_ProcessCleanup(%d):%s", l_clRet,
          strerror(errno));
      l_eStatus = eFrameworkunifiedStatusFail;
      break;
    }

    FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "PID:%d code:%d status:%d",
        l_procInfo.pid, l_procInfo.code, l_procInfo.status);

    // Terminate a process group to reclaim descendants of an anomaly terminated process
    SS_ASERT_ERRNO(0 == CL_ProcessAbortGroup(l_procInfo.pid));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

    BOOL bIsExited = (CLD_EXITED == l_procInfo.code) ? TRUE : FALSE;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    SMProcessExitInfo l_procExitInfo;
    memset(&l_procExitInfo, 0, sizeof(l_procExitInfo));
    l_procExitInfo.pid = l_procInfo.pid;
    // Process exit value when si_code == CLD_EXITED, else delivered signal number.
    l_procExitInfo.exitValue = l_procInfo.status;
    l_procExitInfo.signalNumber = SS_SM_ABORT_SIGNAL;

    // If GroupRelaunch is required, kill the remaining services
    SS_String l_moduleQueueName("");
    if (eFrameworkunifiedStatusOK == FindNameOfTerminatedProcess(l_procExitInfo.pid, l_moduleQueueName)) {
      for (GroupRelaunchModuleListIter l_itr =
          m_GroupRelaunchModuleList.begin();
          m_GroupRelaunchModuleList.end() != l_itr; l_itr++) {
        if (l_itr->name == l_moduleQueueName) {
          if (l_itr->bIsKilled) {  // LCOV_EXCL_BR_LINE 200: relaunch module bIsKilled is always false
            // LCOV_EXCL_START 200: relaunch module bIsKilled is always false
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            // Normal during forced Relaunch
            l_procExitInfo.exitValue = SS_SM_EXIT_RELAUNCH;
            bIsExited = TRUE;
            l_itr->bIsKilled = FALSE;
            // LCOV_EXCL_STOP
          } else {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "GroupRelaunch(%d/%d)",
                m_GroupRelaunchCount, m_GroupRelaunchLimit);

            if (m_GroupRelaunchCount < m_GroupRelaunchLimit) {
              SS_ASERT(  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
                  FrameworkunifiedNPPublishNotification(f_hApp,  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
                      NTFY_SSNeedAplRestart, NULL, 0));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

              // Issue ErrorEvent
              SMLoggingInfo l_loggingInfo;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
              SS_String l_moduleBinaryName("");  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

              l_loggingInfo.pid = l_procExitInfo.pid;
              l_loggingInfo.exitValue = l_procExitInfo.exitValue;
              l_loggingInfo.signalNumber = SS_SM_ABORT_SIGNAL;
              if (eFrameworkunifiedStatusOK != GetBinaryNameOfProcess(l_moduleQueueName, l_moduleBinaryName)) {  // LCOV_EXCL_BR_LINE 200: cannot be error
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                SS_ASERT(0);  // LCOV_EXCL_LINE 200: cannot be error
              } else {
                snprintf(l_loggingInfo.binaryFileName,
                    sizeof(l_loggingInfo.binaryFileName),
                    "%s", l_moduleBinaryName.c_str());
              }

              l_eStatus = ErrorEventEnqueue(f_hApp,
                  eErrorEventTypeGroupRelaunch, l_itr->name,
                  eErrorEventResetTypeNone, l_loggingInfo);
              LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus,  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
                  "ErrorEventEnqueue()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)

              // Relaunch normally and do not leave anomaly LOGs
              l_procExitInfo.exitValue = SS_SM_EXIT_RELAUNCH;
              bIsExited = TRUE;

              for (GroupRelaunchModuleListIter l_itr =
                  m_GroupRelaunchModuleList.begin();
                  m_GroupRelaunchModuleList.end() != l_itr;
                  l_itr++) {
                if (l_itr->name != l_moduleQueueName) {  // LCOV_EXCL_BR_LINE 200: name always be equal because of outer if judge  // NOLINT(whitespace/line_length)
                  // LCOV_EXCL_START 200: name always be equal
                  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                  ModuleLaunchListIter l_ModuleListIter;
                  if (eFrameworkunifiedStatusOK
                      != GetModuleIterator(
                          l_itr->name.c_str(),
                          l_ModuleListIter)) {
                    SS_ASERT(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
                  } else {
                    killpg(l_ModuleListIter->pid,
                        SS_SM_TERMINATE_SIGNAL);
                    l_itr->bIsKilled = TRUE;
                  }
                  // LCOV_EXCL_STOP
                }
              }

              m_GroupRelaunchCount++;
            } else {
              // Reboot system
              ModuleLaunchListIter l_ModuleListIter;
              if (eFrameworkunifiedStatusOK == GetModuleIterator(l_moduleQueueName.c_str(), l_ModuleListIter)) {  // LCOV_EXCL_BR_LINE 200:cannot be error   // NOLINT(whitespace/line_length)
                l_ModuleListIter->relaunch_count = l_ModuleListIter->retry_cnt;
              } else {
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                SS_ASERT(0);  // LCOV_EXCL_LINE 200: cannot be error
              }
            }
            break;
          }
        }
      }
    }

    if (isPreLaunchedProcess(l_procExitInfo.pid)) {  // LCOV_EXCL_BR_LINE 200:prelaunch mode is not valid at UT test //NOLINT (whitespace/line_length)
      // LCOV_EXCL_START 200 :prelaunch mode is not valid at UT test
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_eStatus = OnPreLaunchedProcessTermDetected(f_hApp, l_procExitInfo, bIsExited);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
      if (eFrameworkunifiedStatusOK != l_eStatus) {
        LOG_ERROR("OnPreLaunchedProcessTermDetected()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
        l_eStatus = eFrameworkunifiedStatusFail;
        // If an error occurs here, the zombie process may be collected and continued.
      }
      // LCOV_EXCL_STOP
    } else if (bIsExited) {
      l_eStatus = OnProcessExitDetected(f_hApp, l_procExitInfo);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
      if (eFrameworkunifiedStatusOK != l_eStatus) {
        LOG_ERROR("OnProcessExitDetected()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
        l_eStatus = eFrameworkunifiedStatusFail;
        // If an error occurs here, the zombie process may be collected and continued.
      }
    } else {
      l_eStatus = OnProcessCrashDetected(f_hApp, l_procExitInfo);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
      if (eFrameworkunifiedStatusOK != l_eStatus) {
        LOG_ERROR("OnProcessCrashDetected()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
        l_eStatus = eFrameworkunifiedStatusFail;
        // If an error occurs here, the zombie process may be collected and continued.
      }
    }
  } while (l_clRet == 1 && l_count < 50);

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

BOOL CSystemManager::isPreLaunchedProcess(int f_pid) {
  BOOL l_bIsExist = FALSE;

  PreLaunchModuleListIter l_itr;
  for (l_itr = m_PreLaunchModuleList.begin();
      m_PreLaunchModuleList.end() != l_itr; l_itr++) {
    if (l_itr->pid == f_pid) {    // LCOV_EXCL_BR_LINE 200:prelaunch mode is not valid at UT test //NOLINT (whitespace/line_length)
      // LCOV_EXCL_START 200 :prelaunch mode is not valid at UT test
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_bIsExist = TRUE;
      break;
      // LCOV_EXCL_STOP
    }
  }
  return l_bIsExist;
}

EFrameworkunifiedStatus CSystemManager::OnPreLaunchedProcessTermDetected(HANDLE f_hApp, SMProcessExitInfo &f_procExitInfo, BOOL f_bIsExited) {  // LCOV_EXCL_START 200 :prelaunch mode is not valid at UT test  // NOLINT(whitespace/line_length)
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  PreLaunchModuleListIter l_itr;
  for (l_itr = m_PreLaunchModuleList.begin();
      m_PreLaunchModuleList.end() != l_itr; l_itr++) {
    if (l_itr->pid == static_cast<int>(f_procExitInfo.pid)) {
      break;
    }
  }

  try {
    if (m_PreLaunchModuleList.end() == l_itr) {
      SS_ASERT(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
      throw eFrameworkunifiedStatusFail;
    }

    BOOL l_bIsNeedRelaunched = FALSE;

    if ((SS_SM_RELAUNCH_NO_LIMIT == l_itr->relaunchLimit)
        || (l_itr->relaunchLimit > l_itr->relaunchCount)) {
      l_itr->relaunchCount++;
      FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "Relaunch %s(%d/%d)",
          l_itr->name.c_str(), l_itr->relaunchCount,
          l_itr->relaunchLimit);
      if (-1 == (l_itr->pid = l_itr->LaunchFunc())) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Relaunch %s Failed",
            l_itr->name.c_str());
      } else {
        l_bIsNeedRelaunched = TRUE;
      }
    }

    if (!l_bIsNeedRelaunched) {
      if (l_itr->critical) {
        SMLoggingInfo l_loggingInfo;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
        EErrorEventType l_ErrorType;

        l_itr->pid = -1;

        l_loggingInfo.pid = f_procExitInfo.pid;
        l_loggingInfo.exitValue = f_procExitInfo.exitValue;
        l_loggingInfo.signalNumber = f_procExitInfo.signalNumber;
        l_loggingInfo.resetReason =
            e_SS_SM_CPU_RESET_REASON_GENERIC_ERR;
        snprintf(l_loggingInfo.binaryFileName,
            sizeof(l_loggingInfo.binaryFileName), "%s",
            l_itr->binaryFileName.c_str());

        if (f_bIsExited) {
          FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "%s exited",
              l_itr->name.c_str());
          l_ErrorType = eErrorEventTypeProcessExit;
        } else {
          FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "%s crashed",
              l_itr->name.c_str());
          l_ErrorType = eErrorEventTypeProcessCrash;
        }

        l_eStatus = ErrorEventEnqueue(f_hApp, l_ErrorType, l_itr->name,
            eErrorEventResetTypeHard, l_loggingInfo);
        LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus, "ErrorEventEnqueue()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "%s removed",
            l_itr->name.c_str());
        l_itr->pid = -1;
      }
    }
  } catch (EFrameworkunifiedStatus e) {
    l_eStatus = e;
  }

  return l_eStatus;
}
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnProcessCrashDetected
/// Initiate logging and recovery for an abnormal process termination.
/// The terminated process may or may not be a child of system manager.
/// If the process is a child of SM, then a validation check must ensue
/// to ensure that the process crash is not the result of a failed heartbeat
/// and subsequent termination signal sent from System Manager. This function
/// is NOT called for processes that exit normally.
///
/// See OnProcessCrashDetected.
///
/// \param Handle to AGL application.
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnProcessCrashDetected(HANDLE f_hApp,
        SMProcessExitInfo &f_procExitInfo) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;
  BOOL l_resetRequired = TRUE;
  SS_String l_moduleQueueName("");
  SS_String l_moduleBinaryName("");  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  EErrorEventResetType l_resetType;

  INTERFACEUNIFIEDLOG_RECEIVED_FROM(f_hApp);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  TEXT(__FUNCTION__, " PROCESS CRASH: Process PID: %d", f_procExitInfo.pid);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  if (eFrameworkunifiedStatusOK
      != (l_eStatus = FindNameOfTerminatedProcess(f_procExitInfo.pid, l_moduleQueueName))) {
    // Normally, logging COULD continue, but stopping logging also solves a PosixBasedOS001 errata
    // where a process crashes and is removed from the SM process launch map and is ALSO
    // detected by the exit detector as a 'normal' exit. The FindNameOfTerminatedProcess()
    // will continue properly IF a crash and removal from the map has not occurred
    // for the same process.
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        " Error. FindNameOfTerminatedProcess() returned error: %d/'%s' for PID: %d."
            " No recovery possible. Logging has been suspended for this event.",
        l_eStatus, GetStr(l_eStatus).c_str(), f_procExitInfo.pid);
  } else {
    l_eStatus = GetBinaryNameOfProcess(l_moduleQueueName, l_moduleBinaryName);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 200: cannot be error
      // LCOV_EXCL_START 200: cannot be error
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__,
          " Warning. Unable to identify binary for SM child process PID: %d, Queue Name: %s."
              " The process binary name will be missing from the .csv file.",
          f_procExitInfo.pid, l_moduleQueueName.c_str());
      // LCOV_EXCL_STOP
    } else {
      strncpy(f_procExitInfo.binaryFileName, l_moduleBinaryName.c_str(),
          sizeof(f_procExitInfo.binaryFileName) - 1);

      f_procExitInfo.binaryFileName[sizeof(f_procExitInfo.binaryFileName) - 1] = '\0';
    }

    if (eFrameworkunifiedStatusOK
        != (l_eStatus = ActOnModuleFailure(f_hApp, l_moduleQueueName,
            l_resetRequired))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
          " Error. ActOnModuleFailure(%s,%d) errored: %d/'%s'",
          l_moduleQueueName.c_str(), l_resetRequired, l_eStatus,
          GetStr(l_eStatus).c_str());
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__,
          "Info. ActOnModuleFailure() returned reset status: %d/'%s'.",
          l_resetRequired, GetStr(l_resetRequired).c_str());
    }

    l_resetType =
        (FALSE == l_resetRequired) ?
            eErrorEventResetTypeNone : eErrorEventResetTypeHard;

    SMLoggingInfo l_loggingInfo;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

    l_loggingInfo.pid = f_procExitInfo.pid;
    l_loggingInfo.exitValue = f_procExitInfo.exitValue;
    l_loggingInfo.signalNumber = f_procExitInfo.signalNumber;
    snprintf(l_loggingInfo.binaryFileName,
        sizeof(l_loggingInfo.binaryFileName), "%s",
        f_procExitInfo.binaryFileName);
    if (eErrorEventResetTypeNone != l_resetType) {
      l_loggingInfo.resetReason = e_SS_SM_CPU_RESET_REASON_GENERIC_ERR;
    }

    l_eStatus = ErrorEventEnqueue(f_hApp, eErrorEventTypeProcessCrash,
        l_moduleQueueName, l_resetType, l_loggingInfo);

    LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus,  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
        "ErrorEventEnqueue(eErrorEventTypeProcessCrash)");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnProcessExitDetected
/// Initiate logging and recovery for System Manager child processes that
/// exit normally via the last brace of main(), exit(), or have their last
/// thread exit. This function is NOT called for processes that terminate
/// unexpectedly.
///
/// See OnProcessExitDetected.
///
/// \param Handle to AGL application.
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnProcessExitDetected(HANDLE f_hApp,
        SMProcessExitInfo &f_procExitInfo) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;
  SS_String l_moduleQueueName("");

  INTERFACEUNIFIEDLOG_RECEIVED_FROM(f_hApp);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  TEXT(__FUNCTION__, "PROCESS EXIT: PID:%d exitValue:%d", f_procExitInfo.pid,  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
      f_procExitInfo.exitValue);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  if (eFrameworkunifiedStatusOK
      != (l_eStatus = FindNameOfTerminatedProcess(f_procExitInfo.pid,
          l_moduleQueueName))) {
     // This happens when SMs to EXEC debugging commands, etc.
     // Since this is not an error, set LOGs to WARN levels and set the return codes to OK.
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "%s: pid:%d is unknown service",
        GetStr(l_eStatus).c_str(), f_procExitInfo.pid);
    l_eStatus = eFrameworkunifiedStatusOK;
  } else {
    if (0 == f_procExitInfo.exitValue) {
      ModuleLaunchListIter l_moduleListIter;

      if (eFrameworkunifiedStatusOK != (l_eStatus = RemoveModuleEntryFromHB(f_hApp, l_moduleQueueName.c_str()))) {  // LCOV_EXCL_BR_LINE 4: NSFW  // NOLINT(whitespace/line_length)
        // LCOV_EXCL_START 4: NSFW
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
            " Error: RemoveModuleEntryFromHB(%s) errored: %d/'%s'",
            l_moduleQueueName.c_str(), l_eStatus,
            GetStr(l_eStatus).c_str());
        // LCOV_EXCL_STOP
      }

      l_eStatus = GetModuleIterator(l_moduleQueueName.c_str(),
          l_moduleListIter);
      if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 200:cannot be error
        // LCOV_EXCL_START 200:cannot be error
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
            " Error: Module %s not found in Group Launch Map",
            l_moduleQueueName.c_str());
        // LCOV_EXCL_STOP
      } else {
        l_eStatus = SendRequestToLauncher(f_hApp, l_moduleListIter,  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
            ePLThrdCmd_TERMINATE_MODULE_REQST,  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
            "ePLThrdCmd_TERMINATE_MODULE_REQST");  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

        LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus,  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
            "SendRequestToLauncher()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)

        // Do not add relaunch_count for normal termination.
        l_moduleListIter->relaunch_status = RelaunchSafe;
      }
    } else if (SS_SM_EXIT_RELAUNCH == f_procExitInfo.exitValue) {
      ModuleLaunchListIter l_moduleListIter;

      if (eFrameworkunifiedStatusOK != (l_eStatus = RemoveModuleEntryFromHB(f_hApp, l_moduleQueueName.c_str()))) {  // LCOV_EXCL_BR_LINE 4: nsfw error  // NOLINT(whitespace/line_length)
        // LCOV_EXCL_START 4: nsfw error
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
            " Error: RemoveModuleEntryFromHB(%s) errored: %d/'%s'",
            l_moduleQueueName.c_str(), l_eStatus,
            GetStr(l_eStatus).c_str());
        // LCOV_EXCL_STOP
      }

      l_eStatus = GetModuleIterator(l_moduleQueueName.c_str(),
          l_moduleListIter);
      if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 200:cannot be error
        // LCOV_EXCL_START 200:cannot be error
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
            " Error: Module %s not found in Group Launch Map",
            l_moduleQueueName.c_str());
        // LCOV_EXCL_STOP
      } else {
        l_eStatus = SendRequestToLauncher(f_hApp, l_moduleListIter,  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
            ePLThrdCmd_RELAUNCH_MODULE_REQST,  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
            "ePLThrdCmd_RELAUNCH_MODULE_REQST");  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

        LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus,  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
            "SendRequestToLauncher()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)

        // Do not add relaunch_count for normal Relaunch
        l_moduleListIter->relaunch_status = RelaunchSafe;
      }
    } else {
      SS_String l_moduleBinaryName("");  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
      BOOL l_resetRequired = TRUE;
      EErrorEventResetType l_resetType;

      l_eStatus = GetBinaryNameOfProcess(l_moduleQueueName, l_moduleBinaryName);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
      if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 200: cannot be error
        // LCOV_EXCL_START 200: cannot be error
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__,
            " Warning. Unable to identify binary for SM child process PID: %d, Queue Name: %s."
                " The process binary name will be missing from the .csv file.",
            f_procExitInfo.pid, l_moduleQueueName.c_str());
        // LCOV_EXCL_STOP
      } else {
        strncpy(f_procExitInfo.binaryFileName,
            l_moduleBinaryName.c_str(),
            sizeof(f_procExitInfo.binaryFileName) - 1);

        f_procExitInfo.binaryFileName[sizeof(f_procExitInfo.binaryFileName)
            - 1] = '\0';
      }

      if (eFrameworkunifiedStatusOK
          != (l_eStatus = ActOnModuleFailure(f_hApp,
              l_moduleQueueName, l_resetRequired))) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
            " Error. ActOnModuleFailure(%s,%d) errored: %d/'%s'",
            l_moduleQueueName.c_str(), l_resetRequired, l_eStatus,
            GetStr(l_eStatus).c_str());
      }

      l_resetType =
          (FALSE == l_resetRequired) ?
              eErrorEventResetTypeNone : eErrorEventResetTypeHard;

      SMLoggingInfo l_loggingInfo;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

      l_loggingInfo.pid = f_procExitInfo.pid;
      l_loggingInfo.exitValue = f_procExitInfo.exitValue;
      l_loggingInfo.signalNumber = f_procExitInfo.signalNumber;
      snprintf(l_loggingInfo.binaryFileName,
          sizeof(l_loggingInfo.binaryFileName), "%s",
          f_procExitInfo.binaryFileName);
      if (eErrorEventResetTypeNone != l_resetType) {
        l_loggingInfo.resetReason = e_SS_SM_CPU_RESET_REASON_GENERIC_ERR;
      }

      l_eStatus = ErrorEventEnqueue(f_hApp, eErrorEventTypeProcessExit,
          l_moduleQueueName, l_resetType, l_loggingInfo);
      LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus,  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
          "ErrorEventEnqueue(eErrorEventTypeProcessExit)");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnBootMicroResetNotification
/// Called from the logging shadow when the boot micro notifies the shadow of
/// an unexpected boot micro reset during the last power cycle.
///
/// \param [in] hApp - Application handle.
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnBootMicroResetNotification(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  std::string l_moduleName;
  EFrameworkunifiedStatus l_eStatus;

  INTERFACEUNIFIEDLOG_RECEIVED_FROM(hApp);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg < eSMBootMicroResetReason > (hApp, m_BootMicroResetReason))) {
    LOG_ERROR("ReadMsg()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  } else {
    l_moduleName = FrameworkunifiedGetMsgSrc(hApp);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

    TEXT(__FUNCTION__,  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
        " ERROR EVENT: A boot micro reset has occurred. Reset reason: %d/'%s'.",  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
        m_BootMicroResetReason, GetStr(m_BootMicroResetReason).c_str());  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)

    l_eStatus = ErrorEventEnqueue(hApp, eErrorEventTypeBootMicroReset, l_moduleName);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

    LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus,  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
        "ErrorEventEnqueue(eErrorEventTypeBootMicroReset)");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup RequestBootMicroLog
/// Request boot micro log from the logging shadow.
///
/// \param [in] hApp - Application handle.
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::RequestBootMicroLog(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;

  l_eStatus = FrameworkunifiedPublishEvent(hApp,   // Event received by PS Logging Shadow
      SS_SM_BOOT_MICRO_LOG_REQ,
      NULL,
      NULL, 0);

  LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus,  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
      "FrameworkunifiedPublishEvent(SS_SM_BOOT_MICRO_LOG_REQ)");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)

  bool result =
      m_errorEventTimers[eSM_ERROR_EVENT_TIMER_BOOT_MICRO_LOG_RSPN].Start(
          SS_ERROR_EVENT_BOOT_MICRO_LOG_RESPONSE_TO_SEC, 0, 0, 0);
  if (FALSE == result) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        " Error. Failed to start timer eSM_TIMER_BOOT_MICRO_LOG_RESPONSE.");
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnLowSystemMemory
/// Called when a low memory error has been detected.
///
/// \param [in] hApp - Application handle.
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnLowSystemMemory(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  SysMem l_SysMem;
  std::string l_moduleName;
  EFrameworkunifiedStatus l_eStatus;
  SMLoggingInfo l_loggingInfo;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg < SysMem > (hApp, l_SysMem))) {
    LOG_ERROR("ReadMsg()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  } else {
    m_FreeMemAvailable = l_SysMem.FreeMemoryBytes;

    TEXT(__FUNCTION__,  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
        " ERROR EVENT: System Low Memory detected. Remaining memory: %d.",  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
        m_FreeMemAvailable);  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)

    l_moduleName = FrameworkunifiedGetMsgSrc(hApp);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    l_loggingInfo.resetReason = e_SS_SM_CPU_RESET_REASON_GENERIC_ERR;
    l_eStatus = ErrorEventEnqueue(hApp, eErrorEventTypeSystemLowMemory,
        l_moduleName, eErrorEventResetTypeHard, l_loggingInfo);

    LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus,  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
        "ErrorEventEnqueue(eErrorEventTypeSystemLowMemory)");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::OnLowSystemMemory( HANDLE hApp )

EFrameworkunifiedStatus CSystemManager::OnPropagateSystemError(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  INTERFACEUNIFIEDLOG_RECEIVED_FROM(hApp);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  SS_ASERT(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnUserInvokedLoggingRequest
/// Called when the end user invokes a error event logging request by means
/// of hard key or other direct input method.
///
/// \param [in] hApp - Application handle.
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnUserInvokedLoggingRequest(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  TSystemManagerLoggingRequestInfo l_logInfo;
  EErrorEventType l_errorEventType;
  std::string l_moduleName;
  EFrameworkunifiedStatus l_eStatus;
  SMLoggingInfo l_loggingInfo;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg < TSystemManagerLoggingRequestInfo > (f_hApp, l_logInfo))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT(whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("ReadMsg()");  // LCOV_EXCL_LINE 4: NSFW error case
  } else {
    l_moduleName = FrameworkunifiedGetMsgSrc(f_hApp);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    l_loggingInfo.messageStr = l_logInfo.messageStr;
    l_loggingInfo.suffixStr = l_logInfo.suffixStr;

    switch (l_logInfo.logType) {
    case e_SS_SM_CAPTURE_ALL_LOGS:
      l_errorEventType = eErrorEventTypeUserInvokedCollectAllLogs;
      break;

    case e_SS_SM_SCREEN_CAPTURE:
      l_errorEventType = eErrorEventTypeUserInvokedCollectScreenShot;
      break;

    case e_SS_SM_CAPTURE_INTERFACEUNIFIED_LOGS:
      l_errorEventType = eErrorEventTypeUserInvokedCollectInterfaceunifiedLogs;
      break;

    case e_SS_SM_CAPTURE_DEV_LOGS:
      l_errorEventType = eErrorEventTypeUserInvokedCollectDevLogs;
      break;

    case e_SS_SM_CAPTURE_MODULE_LOGS:
      l_errorEventType = eErrorEventTypeModuleInvokedCollectDebugLogs;
      break;

    case e_SS_SM_CAPTURE_DTC_LOGS:
      l_errorEventType = eErrorEventTypeDtcEvent;
      break;

    case e_SS_SM_CAPTURE_NAVI_LOGS:
      l_errorEventType = eErrorEventTypeUserInvokedCollectNaviLog;
      break;

    case e_SS_SM_CAPTURE_GROUP_RELAUNCH:
      l_errorEventType = eErrorEventTypeGroupRelaunch;
      break;

    default:
      l_errorEventType = eErrorEventTypeMaxValue;
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
          " Error. Received unknown user invoked log type: %d. Dropping request.",
          l_logInfo.logType);
      break;
    }

    if (eErrorEventTypeMaxValue != l_errorEventType) {  // LCOV_EXCL_BR_LINE 200: will not be the else case
      TEXT(__FUNCTION__,  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
          " ERROR EVENT: User invoked logging request %d/'%s' received. "  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
              "Adding the request to the event queue.",  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
          l_logInfo.logType, GetStr(l_logInfo.logType).c_str());  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)

      l_eStatus = ErrorEventEnqueue(f_hApp, l_errorEventType,
          l_moduleName, eErrorEventResetTypeNone, l_loggingInfo);

      LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus,  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
          "ErrorEventEnqueue(UserInvoked)");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnEelExportRequest
/// Called when a removable device is inserted and contains the EEL_Export
/// trigger.
///
/// \param [in] hApp - Application handle.
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnEelExportRequest(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  UI_32 l_strlen = FrameworkunifiedGetMsgLength(f_hApp);
  CHAR l_path[l_strlen];  // NOLINT

  if (l_strlen == 0) {  // LCOV_EXCL_BR_LINE 200: restricted by iterface_unified
    // LCOV_EXCL_START 200: restricted by iterface_unified
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200:test assert
    SS_ASERT(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    return eFrameworkunifiedStatusFail;
    // LCOV_EXCL_STOP
  }

  if (sizeof(l_path) != FrameworkunifiedGetMsgLength(f_hApp)) {  // LCOV_EXCL_BR_LINE 6: must be equal
    // LCOV_EXCL_START 6: must be equal
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("DataSize mismatch");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    // LCOV_EXCL_STOP
  } else if (eFrameworkunifiedStatusOK  // LCOV_EXCL_BR_LINE 4: NSFW error case
      != (l_eStatus = FrameworkunifiedGetMsgDataOfSize(f_hApp, &l_path[0],
          static_cast<UI_32>(sizeof(l_path)), eSMRRelease))) {
    // LCOV_EXCL_START 4: NSFW error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("FrameworkunifiedGetMsgDataOfSize()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    // LCOV_EXCL_STOP
  } else {
    TEXT(__FUNCTION__,
        " ERROR EVENT: EelExport request received. Adding the request to the event queue.");  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
    std::string l_moduleName = FrameworkunifiedGetMsgSrc(f_hApp);
    SMLoggingInfo l_loggingInfo;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

    l_path[l_strlen - 1] = '\0';
    l_loggingInfo.path = l_path;

    l_eStatus = ErrorEventEnqueue(f_hApp, eErrorEventTypeEelExport,
        l_moduleName, eErrorEventResetTypeNone, l_loggingInfo);

    LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus,  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
        "ErrorEventEnqueue(eErrorEventTypeEelExport)");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnSystemmanagerEmmcLogsRequest
/// Called when a removable device is inserted and contains the LOGGERSERVICE_EMMC_LOGS
/// trigger.
///
/// \param [in] hApp - Application handle.
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnSystemmanagerEmmcLogsRequest(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  UI_32 l_strlen = FrameworkunifiedGetMsgLength(f_hApp);
  CHAR l_path[l_strlen];  // NOLINT

  if (l_strlen == 0) {  // LCOV_EXCL_BR_LINE 4:NSFW
    // LCOV_EXCL_START 4:NSFW
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    SS_ASERT(0);
    return eFrameworkunifiedStatusFail;
    // LCOV_EXCL_STOP
  }

  if (sizeof(l_path) != FrameworkunifiedGetMsgLength(f_hApp)) {  // LCOV_EXCL_BR_LINE 8: CHAR l_path[l_strlen]
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("DataSize mismatch");  // LCOV_EXCL_LINE 8: CHAR l_path[l_strlen]
  } else if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedGetMsgDataOfSize(f_hApp, &l_path[0], static_cast<UI_32>(sizeof(l_path)), eSMRRelease))) {  // LCOV_EXCL_BR_LINE 4:NSFW error case  // NOLINT(whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("FrameworkunifiedGetMsgDataOfSize()");  // LCOV_EXCL_LINE 4:NSFW error case
  } else {
    TEXT(__FUNCTION__,  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
        " ERROR EVENT: LOGGERSERVICE_EMMC_LOGS request received. Adding the request to the event queue.");  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
    std::string l_moduleName = FrameworkunifiedGetMsgSrc(f_hApp);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    SMLoggingInfo l_loggingInfo;

    l_path[l_strlen - 1u] = '\0';
    l_loggingInfo.path = l_path;

    l_eStatus = ErrorEventEnqueue(f_hApp, eErrorEventTypeInterfaceunifiedEmmcLogs,
        l_moduleName, eErrorEventResetTypeNone, l_loggingInfo);

    LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus,  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
        "ErrorEventEnqueue(eErrorEventTypeInterfaceunifiedEmmcLogs)");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnSystemmanagerClearLogsRequest
/// Called when a clear LOGGERSERVICE_EMMC_LOGS requested
/// trigger.
///
/// \param [in] hApp - Application handle.
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnSystemmanagerClearLogsRequest(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  std::string l_moduleName = FrameworkunifiedGetMsgSrc(f_hApp);

  l_eStatus = ErrorEventEnqueue(f_hApp, eErrorEventTypeUserInvokedClearLogs,  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
      l_moduleName, eErrorEventResetTypeNone);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus,  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
      "ErrorEventEnqueue(eErrorEventTypeUserInvokedClearLogs)");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnDiagLoggingRequest
/// Called by logger to initiate log artifact collection and storage on behalf
/// of diagnostic services.
///
/// \param [in] hApp - Application handle.
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnDiagLoggingRequest(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  UI_32 l_strlen = FrameworkunifiedGetMsgLength(f_hApp);
  CHAR l_path[l_strlen];  // NOLINT

  if (l_strlen == 0) {  // LCOV_EXCL_BR_LINE 200: restricted by iterface_unified
    // LCOV_EXCL_START 200: restricted by iterface_unified
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200:test assert
    SS_ASERT(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    return eFrameworkunifiedStatusFail;
    // LCOV_EXCL_STOP
  }

  if (sizeof(l_path) != FrameworkunifiedGetMsgLength(f_hApp)) {  // LCOV_EXCL_BR_LINE 6: must be equal
    // LCOV_EXCL_START 6: must be equal
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("DataSize mismatch");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    // LCOV_EXCL_STOP
  } else if (eFrameworkunifiedStatusOK  // LCOV_EXCL_BR_LINE 4: NSFW error case
      != (l_eStatus = FrameworkunifiedGetMsgDataOfSize(f_hApp, &l_path[0],
    	static_cast<UI_32>(sizeof(l_path)), eSMRRelease))) {  // NOLINT
    // LCOV_EXCL_START 4: NSFW error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("FrameworkunifiedGetMsgDataOfSize()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    // LCOV_EXCL_STOP
  } else {
    TEXT(__FUNCTION__,  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
        " ERROR EVENT: SS_SM_ERROR_EVENT_DIAG_LOG_REQ request received. Adding the request to the event queue.");  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
    std::string l_moduleName = FrameworkunifiedGetMsgSrc(f_hApp);
    SMLoggingInfo l_loggingInfo;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

    l_path[l_strlen - 1u] = '\0';
    l_loggingInfo.path = l_path;

    l_eStatus = ErrorEventEnqueue(f_hApp, eErrorEventTypeDiagEvent,
        l_moduleName, eErrorEventResetTypeNone, l_loggingInfo);

    LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus,  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
        "ErrorEventEnqueue(eErrorEventTypeDiagEvent)");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnCanLoggingRequest
///  This function is called by logger to initiate log artifact collection
///  and storage when signaled via CAN.
///
/// \param [in] hApp - Application handle.
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnCANLoggingRequest(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  std::string l_moduleName;

  l_moduleName = FrameworkunifiedGetMsgSrc(f_hApp);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  TEXT(__FUNCTION__,  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
      " ERROR EVENT: SS_SM_ERROR_EVENT_CAN_LOG_REQ received from %s.",  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
      l_moduleName.c_str());  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)

  l_eStatus = ErrorEventEnqueue(f_hApp, eErrorEventTypeCanEvent, l_moduleName);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus,  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
      "ErrorEventEnqueue(eErrorEventTypeCanEvent)");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

