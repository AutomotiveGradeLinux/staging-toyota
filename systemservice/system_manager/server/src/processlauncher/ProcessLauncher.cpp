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
/// \brief    This file provides support for process launching and termination.
///
///////////////////////////////////////////////////////////////////////////////
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_multithreading.h>
#include <system_service/ss_templates.h>
#include <system_service/ss_string_maps.h>
#include <native_service/ns_plogger_if.h>
#include <string>

#include "ProcessLauncher.h"
#include "ss_sm_process_launcher_protocol.h"
#include "ss_sm_process_launcher.h"
#include "ss_sm_systemmanagerlog.h"
#include "ss_sm_signals.h"

CProcessLauncher::CProcessLauncher() {  // LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
}
// LCOV_EXCL_STOP

CProcessLauncher::CProcessLauncher(void *) {
}

CProcessLauncher::~CProcessLauncher() {  // LCOV_EXCL_START 14: resident process end
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  m_mapOfProcesses.clear();
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus CProcessLauncher::PLTerminateModule(
                             HANDLE hThread,
                             T_ProcessLauncherTerminationResp *f_pTerminateRespData) {
  EFrameworkunifiedStatus l_eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  T_ProcessLauncherTerminationReq l_ModuleTerminateReq;

  // LCOV_EXCL_BR_START 4:NSFW error case
  if (eFrameworkunifiedStatusOK
      != (l_eStatus = ReadMsg < T_ProcessLauncherTerminationReq > (hThread, l_ModuleTerminateReq, eSMRRetain))) {
  // LCOV_EXCL_BR_STOP
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("ReadMsg()");  // LCOV_EXCL_LINE 4:NSFW error case
  } else {
    // get pointer to the process object launched earlier
    Process * l_pProcessPtr = m_mapOfProcesses[l_ModuleTerminateReq.path];  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

    // Confirm that the process pointer for the module exists in the map
    if (NULL != l_pProcessPtr) {  // LCOV_EXCL_BR_LINE 200: l_pProcessPtr must not be null
      // check if process still exists
      if (l_pProcessPtr->DoesProcessExist()) {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
               " Terminating %s, PID: %d.", l_ModuleTerminateReq.path, l_pProcessPtr->GetProcessId());
        // kill unresponsive process
        l_pProcessPtr->KillProcess(SS_SM_ABORT_SIGNAL);

        if (NULL != f_pTerminateRespData) {  // LCOV_EXCL_BR_LINE 200: f_pTerminateRespData must not be null
          std::strcpy(f_pTerminateRespData->name,  // NOLINT
                      l_ModuleTerminateReq.name);
          std::strcpy(f_pTerminateRespData->path,  // NOLINT
                      l_ModuleTerminateReq.path);
          std::strcpy(f_pTerminateRespData->args,  // NOLINT
                      l_ModuleTerminateReq.args);
          f_pTerminateRespData->moduleIterator =
                      l_ModuleTerminateReq.moduleIterator;
          f_pTerminateRespData->groupIterator =
                      l_ModuleTerminateReq.groupIterator;
        }
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Info: %s has already terminated.", l_ModuleTerminateReq.path);

        l_eStatus = eFrameworkunifiedStatusServNotFound;
      }
      // remove process entry
      m_mapOfProcesses.erase(l_ModuleTerminateReq.path);
      // delete process object
      delete l_pProcessPtr;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    } else {  // LCOV_EXCL_BR_LINE 200: l_pProcessPtr will not be null
      // LCOV_EXCL_START 200: l_pProcessPtr will not be null
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_eStatus = eFrameworkunifiedStatusDbRecNotFound;
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: %s not in m_mapOfProcesses", l_ModuleTerminateReq.path);
      // LCOV_EXCL_STOP
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus CProcessLauncher::PLOnCmdTerminateModule(HANDLE hThread) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;

  T_ProcessLauncherTerminationResp l_TerminateRespData;
  if (eFrameworkunifiedStatusOK == (l_eStatus = PLTerminateModule(hThread, &l_TerminateRespData))) {
    // reply to System Manager about successful module termination
    l_eStatus = FrameworkunifiedSendParent(hThread,
                              ePLThrdCmd_TERMINATE_MODULE_RESP,
                              sizeof(l_TerminateRespData),
                              &l_TerminateRespData);
    LOG_STATUS(l_eStatus, "FrameworkunifiedSendParent(ePLThrdCmd_TERMINATE_MODULE_RESP");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  } else if (eFrameworkunifiedStatusServNotFound == l_eStatus) {  // Already terminated
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Info. Module already terminated.");
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        " Error: PLTerminateModule(%s) errored: %d/'%s'",
        l_TerminateRespData.name, l_eStatus, GetStr(l_eStatus).c_str());
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus CProcessLauncher::PLOnCmdModuleStatus(HANDLE hThread) {
  return eFrameworkunifiedStatusOK;
}

VOID buildArgList(StringList& arg_list, PSTR args) {  // NOLINT
  arg_list.clear();
  char * pch = NULL;
  pch = std::strtok(args, " ");
  while (pch != NULL) {
    arg_list.push_back(pch);
    pch = std::strtok(NULL, " ");
  }
}

EFrameworkunifiedStatus CProcessLauncher::PLLaunchModule(HANDLE hThread, T_ProcessLaunchResp &f_LaunchRespData) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus                   l_eStatus;
  Process                     *l_pProcessPtr = NULL;
  T_ProcessLauncherLaunchReq   l_ModuleLaunchReq;

  // LCOV_EXCL_BR_START 4:NSFW error case
  if (eFrameworkunifiedStatusOK
      != (l_eStatus = ReadMsg < T_ProcessLauncherLaunchReq > (hThread, l_ModuleLaunchReq))) {
  // LCOV_EXCL_BR_STOP
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("ReadMsg()");  // LCOV_EXCL_LINE 4:NSFW error case
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
            "Sending to Launcher name: %s, path: %s, args: %s, lmsk: %s, "
                    "prio: %d ", l_ModuleLaunchReq.name,
            l_ModuleLaunchReq.path, l_ModuleLaunchReq.args,
            l_ModuleLaunchReq.logging_mask, l_ModuleLaunchReq.priority);

    StringList args;
    buildArgList(args, l_ModuleLaunchReq.args);  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)

    int log_mask_str_len = static_cast<int>(std::strlen(l_ModuleLaunchReq.logging_mask));
    if (log_mask_str_len > 0) {
      // Create mask argument to be passed to application.
      // The argument must be of the form "-m 0x00000000,0x00000000,...", assuming n 32-bit mask values.
      // The storage size of the argument must include one byte for the NULL termination character.
      args.push_back("-m");
      args.push_back(l_ModuleLaunchReq.logging_mask);
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " %s args: logging_mask: %s",
              l_ModuleLaunchReq.path, l_ModuleLaunchReq.logging_mask);
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_PROC_LAUNCH_INFO, __FUNCTION__,
              " '%s' has no logging_mask specified. Using compile time defaults.",
              l_ModuleLaunchReq.path);
    }

    std::strcpy(f_LaunchRespData.name, l_ModuleLaunchReq.name);  // NOLINT Module queue name
    std::strcpy(f_LaunchRespData.path, l_ModuleLaunchReq.path);  // NOLINT Module path and file name
    std::strcpy(f_LaunchRespData.args, l_ModuleLaunchReq.args);  // NOLINT
    f_LaunchRespData.moduleIterator = l_ModuleLaunchReq.moduleIterator;
    f_LaunchRespData.groupIterator = l_ModuleLaunchReq.groupIterator;

    // Process exists?  If not, re-launch the process and create a new map entry.
    l_pProcessPtr = m_mapOfProcesses[l_ModuleLaunchReq.path];  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    if (NULL == l_pProcessPtr) {  // LCOV_EXCL_BR_LINE 200: l_pProcessPtr must not be null
      FRAMEWORKUNIFIEDLOG(ZONE_PROC_LAUNCH_INFO, __FUNCTION__,
              " '%s' is not in the module map. Creating new process.",
              l_ModuleLaunchReq.name);

      SS_String sPathAndFileName = l_ModuleLaunchReq.path;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
      l_pProcessPtr = new (std::nothrow) Process(l_ModuleLaunchReq.cpu_assign);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

      if (NULL == l_pProcessPtr) {  // LCOV_EXCL_BR_LINE 11::new operation failed
        // LCOV_EXCL_START 11::new operation failed
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: Process() returned NULL");
        FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
        return eFrameworkunifiedStatusThreadNotExist;
        // LCOV_EXCL_STOP 11::new operation failed
      }

      const char* uname =
              ('\0' == f_LaunchRespData.moduleIterator->unix_user_name[0]) ?
                      NULL : &f_LaunchRespData.moduleIterator->unix_user_name[0];  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

      l_pProcessPtr->CreateProcess(sPathAndFileName, "", l_ModuleLaunchReq.priority, args, uname);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

      FRAMEWORKUNIFIEDLOG(ZONE_PROC_LAUNCH_INFO, __FUNCTION__,
              " Process Name: %s Process File: %s Pid: %d Is Alive: %s\n",
              l_pProcessPtr->GetProcessName().data(),
              l_pProcessPtr->GetExecutableFileName(),
              l_pProcessPtr->GetProcessId(),
              (l_pProcessPtr->DoesProcessExist() == FALSE) ? "No" : "Yes");

      // Add the process in the map
      m_mapOfProcesses[l_ModuleLaunchReq.path] = l_pProcessPtr;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

      // launch response
      f_LaunchRespData.pid = l_pProcessPtr->GetProcessId();
      f_LaunchRespData.priority = l_pProcessPtr->GetPriority();  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
      l_eStatus = eFrameworkunifiedStatusOK;
    } else {
      // LCOV_EXCL_START 200: l_pProcessPtr must not be null
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_PROC_LAUNCH_INFO, __FUNCTION__,
              " %s already in Process Map", l_ModuleLaunchReq.name);
      // the module is already in the list. Send the response to system manager with PID 0x7FFFFFFF
      f_LaunchRespData.pid = 0x7FFFFFFF;
      f_LaunchRespData.priority = l_ModuleLaunchReq.priority;
      l_eStatus = eFrameworkunifiedStatusOK;
      // LCOV_EXCL_STOP
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus CProcessLauncher::PLOnCmdLaunchModule(HANDLE hThread) {
  EFrameworkunifiedStatus l_eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  l_eStatus = PLCmdLaunchModule(hThread, ePLThrdCmd_LAUNCH_MODULE_RESP, "ePLThrdCmd_LAUNCH_MODULE_RESP");  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  LOG_STATUS_IF_ERRORED(l_eStatus, " PLCmdLaunchModule(ePLThrdCmd_LAUNCH_MODULE_RESP)");  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus CProcessLauncher::PLOnCmdRelaunchModule(HANDLE hThread) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // Terminate unresponsive module
  CALL_AND_LOG_STATUS(PLTerminateModule(hThread));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  l_eStatus = PLCmdLaunchModule(hThread, ePLThrdCmd_RELAUNCH_MODULE_RESP, "ePLThrdCmd_RELAUNCH_MODULE_RESP");  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  LOG_STATUS_IF_ERRORED(l_eStatus, "PLCmdLaunchModule(ePLThrdCmd_RELAUNCH_MODULE_RESP)");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus CProcessLauncher::PLCmdLaunchModule(HANDLE hThread, UI_32 f_protocol_ID, std::string f_protocol_str) {
  EFrameworkunifiedStatus l_eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  T_ProcessLaunchResp l_LaunchRespData;
  l_LaunchRespData.pid = 0;
  if (eFrameworkunifiedStatusOK != (l_eStatus = PLLaunchModule(hThread, l_LaunchRespData))) {  // LCOV_EXCL_BR_LINE 4:NSFW error case
    // LCOV_EXCL_START 4:NSFW error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
            " Error: PLLaunchModule(%s, %s) errored: %d/'%s'",
            l_LaunchRespData.name, f_protocol_str.c_str(), l_eStatus,
            GetStr(l_eStatus).c_str());
    // LCOV_EXCL_STOP 4:NSFW error case
  } else {
    l_eStatus = FrameworkunifiedSendParent(hThread, f_protocol_ID,
            sizeof(T_ProcessLaunchResp), &l_LaunchRespData);

    if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 4:NSFW error case
      // LCOV_EXCL_START 4:NSFW error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
              " Error: FrameworkunifiedSendParent( %s, %s ) errored: %d/'%s'",
              l_LaunchRespData.name, f_protocol_str.c_str(), l_eStatus,
              GetStr(l_eStatus).c_str());
      // LCOV_EXCL_STOP 4:NSFW
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus CProcessLauncher::PLOnCmdStop(HANDLE hThread) {  // LCOV_EXCL_START 14: resident process end
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  return eFrameworkunifiedStatusOK;
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus CProcessLauncher::PLOnCmdStart(HANDLE hThread) {
  return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus CProcessLauncher::PLOnCmdHeartbeatStatusReq(HANDLE hThread) {
  FrameworkunifiedSendParent(hThread, ePLThrdCmd_THREAD_STATUS_RESP, 0, NULL);
  return eFrameworkunifiedStatusOK;
}

