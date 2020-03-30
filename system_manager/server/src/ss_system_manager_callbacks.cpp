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
/// \ingroup  tag_SystemManager
/// \brief    This file provides support for System Manager business logic.
///
///////////////////////////////////////////////////////////////////////////////
//#include <stub/el_gpio.h>
#include <system_service/ss_heartbeat_service_protocol.h>
#include <system_service/ss_system_manager_if.h>
#include <system_service/ss_system_manager_if_local.h>
#include <native_service/frameworkunified_multithreading.h>
#include <sys/wait.h>
#include <system_service/ss_system_manager_notifications.h>
#include <system_service/ss_system_manager_notifications_local.h>
#include <native_service/frameworkunified_framework_if.h>
#include <system_service/ss_services.h>
#include <system_service/ss_power_service_if.h>
#include <processlauncher/ProcessLauncher.h>
#include <processlauncher/ss_sm_process_launcher_protocol.h>
#include <processlauncher/ss_sm_process_launcher.h>
#include <heartbeat/ss_hb_thread.h>

#include <system_service/ss_power_service_notifications.h>
#include <system_service/ss_power_service_protocol.h>

#include <native_service/ns_np_service.h>
#include <native_service/ns_np_service_protocol.h>
#include <native_service/ns_backup.h>

//#include <stub/ss_diag.h>

#include <system_service/ss_error_message.h>
#include <system_service/ss_test_clients.h>
#include <system_service/ss_sm_rom_access.h>
#include <system_service/ss_sm_ram_access.h>
#include <system_service/ss_templates.h>
#include <native_service/ns_plogger_if.h>
//#include <stub/el_mem.h>

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <string>
//#include <power_hal.h>

#include "ss_system_manager_callbacks.h"
#include "ss_system_manager.h"
#include "ss_sm_systemmanagerlog.h"
#include "ss_sm_config.h"

using namespace std;  // NOLINT

extern SS_String GetStr(SS_SMModuleState f_enum);

template<typename C, eFrameworkunifiedStatus (C::*M)(HANDLE)> EFrameworkunifiedStatus SysMgrCallback(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;
  C * pObj = static_cast<C *>(&CSystemManager::GetInstance());
  if (pObj) {  // LCOV_EXCL_BR_LINE 5 : new error
    l_eStatus = (pObj->*M)(hApp);
  }
  return l_eStatus;
}

template<typename C, eFrameworkunifiedStatus (C::*M)(HANDLE, UI_32)>
EFrameworkunifiedStatus SysMgrCbType2(HANDLE hApp, UI_32 f_UI_32) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;
  C * pObj = static_cast<C *>(&CSystemManager::GetInstance());
  if (pObj) {  // LCOV_EXCL_BR_LINE 5 : new error
    l_eStatus = (pObj->*M)(hApp, f_UI_32);
  }
  return l_eStatus;
}

template<typename C, BOOL (C::*M)(UI_32)>
BOOL SysMgrCbType3(UI_32 f_UI_32) {
  BOOL l_bValue = FALSE;
  C * pObj = static_cast<C *>(&CSystemManager::GetInstance());
  if (pObj) {  // LCOV_EXCL_BR_LINE 5 : new error
    l_bValue = (pObj->*M)(f_UI_32);
  }
  return l_bValue;
}

enum {
  POWERON = TRUE,
  POWEROFF = FALSE
};

VOID CSystemManager::LogGroupModulesState(UI_32 f_groupId, std::string pStr) noexcept {
  if (IS_ZONE_SET(ZONE_INFO)) {  // LCOV_EXCL_BR_LINE 6: Because the applicable variable cannot be changed from the external API (only evaluated by the initial value)
    // LCOV_EXCL_START 6: Because the applicable variable cannot be changed from the external API (only evaluated by the initial value)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

    GroupLaunchMapIter l_GroupIterator = m_MapProclaunchGrps.find(f_groupId);
    if (l_GroupIterator == m_MapProclaunchGrps.end()) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             " Error: Group %d not found; 'm_MapProclaunchGrps' is empty",
             f_groupId);
    } else {
      std::string sIsOKFlag;
      TEXT(__FUNCTION__, " Group %d/%s, %d modules: %s, %s", f_groupId,
          l_GroupIterator->second.name.c_str(),
          l_GroupIterator->second.modules.size(), pStr.c_str(),
          GetStr(m_SMCurrentState).c_str());

      ModuleLaunchListIter l_ModuleListIterator = l_GroupIterator->second.modules.begin();
      for (int i = 1;
          l_ModuleListIterator != l_GroupIterator->second.modules.end();
          l_ModuleListIterator++, i++) {
        sIsOKFlag = GetModuleCondition(l_ModuleListIterator) ? " " :  // 'blank' means OK
            "X";  // 'X' means state mismatch

        TEXT(__FUNCTION__,
          "   %2d: %-24s Start Req'd: %-5s %s %-24s %3dms, %3dms, %d",
          i, l_ModuleListIterator->name.c_str(),
        l_ModuleListIterator->is_start_required ?
                "TRUE" : "FALSE", sIsOKFlag.c_str(),
        l_ModuleListIterator->ModuleStateStr().c_str(),
        static_cast<SI_16>(l_ModuleListIterator->m_startReason.GetDelta()),
        static_cast<SI_16>(l_ModuleListIterator->m_stopReason.GetDelta()),
        l_ModuleListIterator->pid);
      }
    }
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
    // LCOV_EXCL_STOP
  }
}  // End of VOID CSystemManager::LogGroupModulesState( UI_32 f_groupId, std::string pStr)

VOID CSystemManager::LogAllGroupModulesState(std::string pStr) noexcept {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  std::string l_sIsOKFlag;
  GroupLaunchMapIter l_GroupIterator = m_MapProclaunchGrps.begin();

  std::stringstream l_comStream;

  l_comStream << pStr << "," << GetStr(m_SMCurrentState);

  std::string l_comStr = l_comStream.str();
  SystemmanagerLogString(ZONE_STATE, __FUNCTION__, l_comStr);

  while (l_GroupIterator != m_MapProclaunchGrps.end()) {
    std::stringstream l_tblStream;
    l_tblStream << "G" << l_GroupIterator->second.id << "/" << l_GroupIterator->second.name;

    ModuleLaunchListIter l_ModuleListIterator = l_GroupIterator->second.modules.begin();

    for (int i = 1;
        l_ModuleListIterator != l_GroupIterator->second.modules.end();
        l_ModuleListIterator++, i++) {
      // Since NPPService exceptionally transitions to STARTED when StartReq'd = N, it is not checked
      l_sIsOKFlag =
          GetModuleCondition(l_ModuleListIterator)
              | (FRAMEWORKUNIFIED_NS_NPSERVICE == l_ModuleListIterator->name) ?
              " " :  // 'blank' means OK
              "X";   // 'X' means state mismatch

      std::string l_isStartReqd = l_ModuleListIterator->is_start_required ? "T" : "F";

      l_tblStream << endl << " " << std::setw(2) << std::right << i << ":"
          << l_ModuleListIterator->pid << " "
          << l_ModuleListIterator->name << " " << "StartReq:"
          << l_isStartReqd << " " << l_sIsOKFlag
          << l_ModuleListIterator->ModuleStateStr();

      if (l_ModuleListIterator->is_start_required && (FRAMEWORKUNIFIED_NS_NPSERVICE != l_ModuleListIterator->name)) {  // LCOV_EXCL_BR_LINE 6: Because the condition cannot be set // NOLINT(whitespace/line_length)
        l_tblStream << " "
            << static_cast<SI_32>(l_ModuleListIterator->m_startReason.GetDelta())
            << "ms "
            << static_cast<SI_32>(l_ModuleListIterator->m_stopReason.GetDelta())
            << "ms";
      }
    }

    l_GroupIterator++;

    // Outputs each GROUP so that buffers do not overflow.
    std::string l_logMsgStr = l_tblStream.str();
    SystemmanagerLogString(ZONE_STATE, __FUNCTION__, l_logMsgStr);
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}  // End of VOID CSystemManager::LogAllGroupModulesState(std::string pStr)

BOOL CSystemManager::GetModuleCondition(ModuleLaunchListIter & f_ModuleIter) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  BOOL l_bIsModuleStateGood = TRUE;

  switch (m_SMCurrentState) {  // LCOV_EXCL_BR_LINE 6: As no values are passed into the default case
  case SS_SM_READY_TO_LAUNCH_APP:
    l_bIsModuleStateGood = f_ModuleIter->IsModuleState(MODULE_STATE_INVALID);
    break;

  case SS_SM_APPS_LAUNCH_IN_PROGRESS:
    l_bIsModuleStateGood = f_ModuleIter->IsModuleState(MODULE_STATE_SKIPPED)
        || f_ModuleIter->IsModuleState(MODULE_STATE_LAUNCH_FAILED)
        || f_ModuleIter->IsModuleState(MODULE_STATE_LAUNCHING)
        || f_ModuleIter->IsModuleState(MODULE_STATE_LAUNCHED);
    break;

  case SS_SM_APPS_LAUNCHED_READY_TO_START:
    l_bIsModuleStateGood = f_ModuleIter->IsModuleState(MODULE_STATE_SKIPPED)
        || f_ModuleIter->IsModuleState(MODULE_STATE_LAUNCH_FAILED)
        || f_ModuleIter->IsModuleState(MODULE_STATE_CONNECTED)
        || f_ModuleIter->IsModuleState(MODULE_STATE_LAUNCHED);
    break;

  case SS_SM_APPS_START_IN_PROGRESS:
    l_bIsModuleStateGood = f_ModuleIter->IsModuleState(MODULE_STATE_SKIPPED)
        || f_ModuleIter->IsModuleState(MODULE_STATE_LAUNCH_FAILED)
        || (f_ModuleIter->is_start_required
            && (f_ModuleIter->IsModuleState(MODULE_STATE_START_SENT)
                || f_ModuleIter->IsModuleState(
                    MODULE_STATE_STARTED)))
        || (!f_ModuleIter->is_start_required
            && (f_ModuleIter->IsModuleState(MODULE_STATE_LAUNCHED)
                || f_ModuleIter->IsModuleState(
                    MODULE_STATE_CONNECTED)));
    break;

  case SS_SM_APPS_START_COMPLETE:
      l_bIsModuleStateGood =
          ((f_ModuleIter->IsModuleState(MODULE_STATE_SKIPPED) == TRUE)
              || (f_ModuleIter->IsModuleState(MODULE_STATE_LAUNCH_FAILED) == TRUE)
              || ((f_ModuleIter->is_start_required == TRUE)
                  && (f_ModuleIter->IsModuleState(MODULE_STATE_STARTED) == TRUE))
              || ((f_ModuleIter->is_start_required == FALSE)
                  && ((f_ModuleIter->IsModuleState(MODULE_STATE_LAUNCHED) == TRUE)
                      || (f_ModuleIter->IsModuleState(MODULE_STATE_CONNECTED) == TRUE)))) ? (BOOL) TRUE : (BOOL) FALSE;
    break;

  case SS_SM_APPS_PRE_START_IN_PROGRESS:
      l_bIsModuleStateGood =
          ((f_ModuleIter->IsModuleState(MODULE_STATE_SKIPPED) == TRUE)
              || (f_ModuleIter->IsModuleState(MODULE_STATE_LAUNCH_FAILED) == TRUE)
              || ((f_ModuleIter->is_start_required == TRUE)
                  && ((f_ModuleIter->IsModuleState(MODULE_STATE_START_PRE_SENT) == TRUE)
                      || (f_ModuleIter->IsModuleState(MODULE_STATE_STARTED_PRE) == TRUE)))
              || ((f_ModuleIter->is_start_required == FALSE)
                  && ((f_ModuleIter->IsModuleState(MODULE_STATE_LAUNCHED) == TRUE)
                      || (f_ModuleIter->IsModuleState(MODULE_STATE_CONNECTED) == TRUE)))) ? (BOOL) TRUE : (BOOL) FALSE;
    break;

    case SS_SM_APPS_PRE_STOP_IN_PROGRESS:
      l_bIsModuleStateGood =
          ((f_ModuleIter->IsModuleState(MODULE_STATE_SKIPPED) == TRUE)
              || (f_ModuleIter->IsModuleState(MODULE_STATE_LAUNCH_FAILED) == TRUE)
              || ((f_ModuleIter->is_start_required == TRUE)
                  && ((f_ModuleIter->IsModuleState(MODULE_STATE_STOP_PRE_SENT) == TRUE)
                      || (f_ModuleIter->IsModuleState(MODULE_STATE_STOPPED_PRE) == TRUE)))
              || ((f_ModuleIter->is_start_required == FALSE)
                  && ((f_ModuleIter->IsModuleState(MODULE_STATE_LAUNCHED) == TRUE)
                      || (f_ModuleIter->IsModuleState(MODULE_STATE_CONNECTED) == TRUE)))) ? (BOOL) TRUE : (BOOL) FALSE;
      break;

    case SS_SM_APPS_PRE_RUN_COMPLETE:
      l_bIsModuleStateGood =
          ((f_ModuleIter->IsModuleState(MODULE_STATE_SKIPPED) == TRUE)
              || (f_ModuleIter->IsModuleState(MODULE_STATE_LAUNCH_FAILED) == TRUE)
              || ((f_ModuleIter->is_start_required == TRUE)
                  && ((f_ModuleIter->IsModuleState(MODULE_STATE_STARTED_PRE) == TRUE)
                      || (f_ModuleIter->IsModuleState(MODULE_STATE_STOPPED_PRE) == TRUE)))
              || ((f_ModuleIter->is_start_required == FALSE)
                  && ((f_ModuleIter->IsModuleState(MODULE_STATE_LAUNCHED) == TRUE)
                      || (f_ModuleIter->IsModuleState(MODULE_STATE_CONNECTED) == TRUE)))) ? (BOOL) TRUE : (BOOL) FALSE;
      break;

    case SS_SM_APPS_BACKGROUND_START_IN_PROGRESS:
      l_bIsModuleStateGood =
          ((f_ModuleIter->IsModuleState(MODULE_STATE_SKIPPED) == TRUE)
              || (f_ModuleIter->IsModuleState(MODULE_STATE_LAUNCH_FAILED) == TRUE)
              || ((f_ModuleIter->is_start_required == TRUE)
                  && ((f_ModuleIter->IsModuleState(MODULE_STATE_START_BACKGROUND_SENT) == TRUE)
                      || (f_ModuleIter->IsModuleState(MODULE_STATE_STARTED_BACKGROUND) == TRUE)))
              || ((f_ModuleIter->is_start_required == FALSE)
                  && ((f_ModuleIter->IsModuleState(MODULE_STATE_LAUNCHED) == TRUE)
                      || (f_ModuleIter->IsModuleState(MODULE_STATE_CONNECTED) == TRUE)))) ? (BOOL) TRUE : (BOOL) FALSE;
      break;

    case SS_SM_APPS_BACKGROUND_STOP_IN_PROGRESS:
      l_bIsModuleStateGood =
          ((f_ModuleIter->IsModuleState(MODULE_STATE_SKIPPED) == TRUE)
              || (f_ModuleIter->IsModuleState(MODULE_STATE_LAUNCH_FAILED) == TRUE)
              || ((f_ModuleIter->is_start_required == TRUE)
                  && ((f_ModuleIter->IsModuleState(MODULE_STATE_STOP_BACKGROUND_SENT) == TRUE)
                      || (f_ModuleIter->IsModuleState(MODULE_STATE_STOPPED_BACKGROUND) == TRUE)))
              || ((f_ModuleIter->is_start_required == FALSE)
                  && ((f_ModuleIter->IsModuleState(MODULE_STATE_LAUNCHED) == TRUE)
                      || (f_ModuleIter->IsModuleState(MODULE_STATE_CONNECTED) == TRUE)))) ? (BOOL) TRUE : (BOOL) FALSE;
      break;
    case SS_SM_APPS_BACKGROUND_RUN_COMPLETE:
      l_bIsModuleStateGood =
          ((f_ModuleIter->IsModuleState(MODULE_STATE_SKIPPED) == TRUE)
              || (f_ModuleIter->IsModuleState(MODULE_STATE_LAUNCH_FAILED) == TRUE)
              || ((f_ModuleIter->is_start_required == TRUE)
                  && ((f_ModuleIter->IsModuleState(MODULE_STATE_STARTED_BACKGROUND) == TRUE)
                      || (f_ModuleIter->IsModuleState(MODULE_STATE_STOPPED_BACKGROUND) == TRUE)))
              || ((f_ModuleIter->is_start_required == FALSE)
                  && ((f_ModuleIter->IsModuleState(MODULE_STATE_LAUNCHED) == TRUE)
                      || (f_ModuleIter->IsModuleState(MODULE_STATE_CONNECTED) == TRUE)))) ? (BOOL) TRUE : (BOOL) FALSE;
      break;

  case SS_SM_APPS_STOPPING_AT__CWORD56__REQ:
  case SS_SM_APPS_STOPPING_AT_INTERNAL_REQ:
    l_bIsModuleStateGood = f_ModuleIter->IsModuleState(MODULE_STATE_SKIPPED)
        || f_ModuleIter->IsModuleState(MODULE_STATE_LAUNCH_FAILED)
        || (f_ModuleIter->is_start_required
            && (f_ModuleIter->IsModuleState(MODULE_STATE_STOP_SENT)
                || f_ModuleIter->IsModuleState(
                    MODULE_STATE_STOPPED)))
        || (!f_ModuleIter->is_start_required
            && (f_ModuleIter->IsModuleState(MODULE_STATE_LAUNCHED)
                || f_ModuleIter->IsModuleState(
                    MODULE_STATE_CONNECTED)));
    break;

  case SS_SM_WAITING_FOR_CRITICAL_APPS_AT__CWORD56__REQ:
  case SS_SM_WAITING_FOR_CRITICAL_APPS_AT_INTERNAL_REQ:
    l_bIsModuleStateGood = f_ModuleIter->IsModuleState(MODULE_STATE_SKIPPED)
        || f_ModuleIter->IsModuleState(MODULE_STATE_LAUNCH_FAILED)
        || (f_ModuleIter->is_start_required
            && f_ModuleIter->IsModuleState(MODULE_STATE_STOPPED))
        || (!f_ModuleIter->is_start_required
            && (f_ModuleIter->IsModuleState(MODULE_STATE_LAUNCHED)
                || f_ModuleIter->IsModuleState(
                    MODULE_STATE_CONNECTED)));
    break;

    // default:  Don't code a 'default' here - let the compiler
    // issue a warning ( set via -Wall or -Wswitch ) when the set of
    // enumerations changes - then the maintainer will
    // automagically know to update this switch statement.
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_bIsModuleStateGood;
}  // End of BOOL CSystemManager::GetModuleCondition( ModuleLaunchListIter & f_ModuleIter )

/*****************************************************************************
 @ingroup: CSystemManager
 @brief: Post Termination handler
 @note: .
 @param void
 @return void
*****************************************************************************/
EFrameworkunifiedStatus CSystemManager::SendRequestToHeartBeat(HANDLE hApp,
        EHBProtocolMessages tRequest, VOID *tReqMessageData,
        UI_32 u32SizeofMessage) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  BOOL bValid = FALSE;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  switch (tRequest) {
  case SS_HEARTBEAT_START:
    bValid = TRUE;
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "SS_HEARTBEAT_START");
    break;

  case SS_HEARTBEAT_STOP:
    bValid = TRUE;
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "SS_HEARTBEAT_STOP");
    tReqMessageData = NULL;
    break;

  case SS_HEARTBEAT_DELETE_MODULE_ENTRY:
    bValid = TRUE;
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "SS_HEARTBEAT_DELETE_MODULE_ENTRY");
    break;

  case SS_HEARTBEAT_APPEND_MODULE_ENTRY:
    bValid = TRUE;
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "SS_HEARTBEAT_APPEND_MODULE_ENTRY");
    break;

  default:
    bValid = FALSE;
    l_eStatus = eFrameworkunifiedStatusInvldParam;
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error : Ignored Invalid message id 0x%X", tRequest);
    break;
  }

  if (TRUE == bValid) {  // LCOV_EXCL_BR_LINE 200:tRequest cannot be default, so bValid cannot be false
    if (INVALID_HANDLE != m_hHeartbeatThread.m_ThreadHdl) {  // LCOV_EXCL_BR_LINE 200:m_ThreadHdl cannot be null
      if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedSendChild(hApp, m_hHeartbeatThread.m_ThreadHdl, tRequest, u32SizeofMessage, tReqMessageData))) {  // LCOV_EXCL_BR_LINE 4:NSFW error case  //NOLINT (whitespace/line_length)
        // LCOV_EXCL_START 4: nsfw error
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        LOG_ERROR("FrameworkunifiedSendChild()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
        // LCOV_EXCL_STOP
      }
    } else {
      // LCOV_EXCL_START 200: m_ThreadHdl cannot be null
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_eStatus = eFrameworkunifiedStatusInvldHandle;
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error : Invalid Heartbeat Thread Handle, cannot send message");
      // LCOV_EXCL_STOP
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnModuleConnectWaitTimeout
///  SM maintains timer (40 sec) to check if all modules opened session with him whoever requires Start.
///  This timer is common to all modules. After timeout it checks the status of every module in group map.
///  If a module requires start and does not connect, SM will initiate error event logging followed by Soft Reset
/// \param [in]
///
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnModuleConnectWaitTimeout(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  static SI_32 l_NumTimesChecked = 0;
  {
    char l_cFormat[] = " Received from (Timer expiration %d)";
    char l_cBuf[sizeof(l_cFormat) + 1];
    snprintf(l_cBuf, sizeof(l_cBuf), l_cFormat, l_NumTimesChecked + 1);
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, l_cBuf);
  }

  std::string l_ProcName;

  SetCmdHist("SM_TIMER_MODULE_CONNECT_WAIT_TIMER", m_TimerCmdHist, // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
      m_TimerHistIter, FrameworkunifiedGetMsgSrc(hApp)); // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  BOOL l_bModuleConnMissed = false;

  GroupLaunchMap::reverse_iterator l_grp_riter = m_MapProclaunchGrps.rbegin();

  ModuleLaunchList::reverse_iterator l_mod_riter;

  if (l_grp_riter == m_MapProclaunchGrps.rend()) {  // LCOV_EXCL_BR_LINE 200: group cannot be empty
    // LCOV_EXCL_START 200: group cannot be empty
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: Group Map is Empty");  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
    l_eStatus = eFrameworkunifiedStatusDbRecNotFound;
    // LCOV_EXCL_STOP
  } else {
    // scan the group map in reverse order
    for (; l_grp_riter != m_MapProclaunchGrps.rend(); l_grp_riter++) {
      l_mod_riter = l_grp_riter->second.modules.rbegin();
      for (; l_mod_riter != l_grp_riter->second.modules.rend();
          l_mod_riter++) {
        SMModuleState l_ModuleState = l_mod_riter->GetModuleState();
        // if the module is still in MODULE_STATE_LAUNCHED state at
        // this point though it requires start something is wrong with
        // the module. SM initiates error event logging followed by
        // soft rest
        if (l_mod_riter->is_start_required
            && ((l_ModuleState == MODULE_STATE_INVALID)
                || (l_ModuleState == MODULE_STATE_SKIPPED)
                || (l_ModuleState == MODULE_STATE_LAUNCHING)
                || (l_ModuleState == MODULE_STATE_LAUNCHED)
                || (l_ModuleState == MODULE_STATE_LAUNCH_FAILED))) {
          FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__,
              " Module %s/%s is not connected",
              l_mod_riter->name.c_str(),
              l_mod_riter->ModuleStateStr().c_str());
          l_bModuleConnMissed = true;
          l_ProcName.assign(l_mod_riter->name);

          if (l_ModuleState == MODULE_STATE_LAUNCHED) {
            CALL_AND_LOG_STATUS_IF_ERRORED(  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
                l_mod_riter->SetPriority(  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
                    m_SystemManagerPriority - 1));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
          }
        }
      }
    }
  }

  PreLaunchModuleListIter l_itr;
  for (l_itr = m_PreLaunchModuleList.begin();
      m_PreLaunchModuleList.end() != l_itr; l_itr++) {
    if ((-1 == l_itr->pid) && (l_itr->critical)) {  // LCOV_EXCL_BR_LINE 200: all prelaunch mode is not critical  // NOLINT(whitespace/line_length)
      // LCOV_EXCL_START 200: all prelaunch mode is not critical
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, " Module %s is not launched",  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
          l_itr->name.c_str());  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
      l_bModuleConnMissed = true;
      l_ProcName.assign(l_itr->name);
    }
    // LCOV_EXCL_STOP
  }

  l_NumTimesChecked++;
  if (true == l_bModuleConnMissed) {
    if (l_NumTimesChecked < m_SMConfig.MCConfig.ModuleConnectionNumTimesToCheck) {
      char l_cFormat[] =
          " l_NumTimesChecked %d < ModuleConnectionNumTimesToCheck %d, "
              "will re-check in %d seconds. ";
      char l_cBuf[sizeof(l_cFormat) + 1 + 1 + 1];
      snprintf(l_cBuf, sizeof(l_cBuf), l_cFormat, l_NumTimesChecked,
          m_SMConfig.MCConfig.ModuleConnectionNumTimesToCheck,
          m_SMConfig.MCConfig.ModuleConnectionTimeOutSec);
      FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, l_cBuf);

    } else {
      l_eStatus = eFrameworkunifiedStatusBadConnection;
      char l_cFormat[] =
          " Module connection timeout for %s and "
              "l_NumTimesChecked %d == ModuleConnectionNumTimesToCheck %d. "
              " Stopping ModuleConnectTimeOut timer and resetting _CWORD102_.";
      char l_cBuf[sizeof(l_cFormat) + MAX_NAME_SIZE_APP + 1 + 1];
      snprintf(l_cBuf, sizeof(l_cBuf), l_cFormat, l_ProcName.c_str(),
          l_NumTimesChecked,
          m_SMConfig.MCConfig.ModuleConnectionNumTimesToCheck);
      LOG_ERROR(l_cBuf);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

      m_GroupLaunchTimer->StopTimer(m_aTimerIDs[eSM_TIMER_MODULE_CONNECT_WAIT_TIMER]); // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

      SMLoggingInfo l_loggingInfo; // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
      l_loggingInfo.resetReason = e_SS_SM_CPU_RESET_REASON_GENERIC_ERR; // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

      // initiate error event logging
      l_eStatus = ErrorEventEnqueue(hApp, eErrorEventTypeModConnFailed,
          l_ProcName, eErrorEventResetTypeHard, l_loggingInfo);
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: ModConnFailed %s",
          l_ProcName.c_str());
      fprintf(stderr, "SS_SysManager/%s/Error: ModConnFailed %s\n", // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
          __FUNCTION__, l_ProcName.c_str()); // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
      LOG_STATUS_REC_HIST_IF_ERRORED(l_eStatus,  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
          "ErrorEventEnqueue(eErrorEventTypeModConnFailed)");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
    }
  } else {
    char l_cFormat[] =
        " l_NumTimesChecked = %d, ModuleConnectionNumTimesToCheck = %d. "
            "All modules connected. Stopping ModuleConnectTimeOut timer.";
    char l_cBuf[sizeof(l_cFormat) + 1 + 1];
    snprintf(l_cBuf, sizeof(l_cBuf), l_cFormat, l_NumTimesChecked,
        m_SMConfig.MCConfig.ModuleConnectionNumTimesToCheck);
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, l_cBuf);

    m_GroupLaunchTimer->StopTimer(m_aTimerIDs[eSM_TIMER_MODULE_CONNECT_WAIT_TIMER]); // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::OnModuleConnectWaitTimeout( HANDLE hApp )

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnClientStartMonitorTimerExpiry
///
///
/// \param [in]
///
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnClientStartMonitorTimerExpiry(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Received from group %d"
    //, m_ActiveGroupId);
      , m_SystemStarter.get_id());

  char cBuf[100];
  SetCmdHist("SM_TIMER_CLIENT_START_MONITOR", m_TimerCmdHist, m_TimerHistIter, FrameworkunifiedGetMsgSrc(hApp)); // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  // sprintf( cBuf, "ProcessGroupAsStarted(%d)", m_ActiveGroupId);
  sprintf(cBuf, "ProcessGroupAsStarted(%d)", m_SystemStarter.get_id());  // NOLINT
  l_eStatus = ProcessGroupAsStarted(hApp, m_SystemStarter.get_id());
  LOG_STATUS_REC_HIST_IF_ERRORED(l_eStatus, cBuf);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");


  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup send_power_request_complete_response
/// send power request completion response to power
///
/// \param [in]
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::send_power_request_complete_response(HANDLE hApp, std::string pStr) noexcept {
  EFrameworkunifiedStatus l_eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // send Wake-up complete response to Power
  l_eStatus = FrameworkunifiedSendMsg(m_hPowerServiceSession,
      SS_SM_WAKEUP_MODULES_CMPL_RSPN, sizeof(wakeInfo), (PVOID) & m_Wake);
  LOG_STATUS_REC_HIST_IF_ERRORED(l_eStatus, "FrameworkunifiedSendMsg(SS_SM_WAKEUP_MODULES_CMPL_RSPN)");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)

  LogAllGroupModulesState(pStr);

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::send_power_request_complete_response( HANDLE hApp )

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnNPPStatusCheckMonitorTimerExpiry
///
/// \brief Called when NPP fails to provide a ready status in the specified.
///        amount of time.  This function will periodically request a new status
///        ACK from NPP service until one is received. See OnNPPReadyStatusCallback().
///
///        Note: NPP usually pushes a ready event to SM and the reversion to
///              polling the NPP ready ACK is for additional robustness. Failure
///              to receive an NPP ready ACK will prevent system startup.
///
/// \param [in] hApp - Handle to framework application.
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnNPPStatusCheckMonitorTimerExpiry(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  SetCmdHist("SM_TIMER_NPP_STATUS_CHECK_MONITOR", m_TimerCmdHist, // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
      m_TimerHistIter, FrameworkunifiedGetMsgSrc(hApp)); // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " m_bIsNPP_ServicesStarted is '%s'",
      GetStr(m_bIsNPP_ServicesStarted).c_str());

  if (FALSE == m_bIsNPP_ServicesStarted) {
    l_eStatus = FrameworkunifiedNPGetReadyStatusOfNPP(hApp);
    LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus, "FrameworkunifiedNPGetReadyStatusOfNPP()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnNPPReadyStatusCallback
///
/// \brief Called when NPP returns a status ACK indicating that the service is
///        operational and ready to respond to subsequent requests.
///
/// \param [in] hApp - Handle to framework application.
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnNPPReadyStatusCallback(HANDLE hApp) {  // LCOV_EXCL_START 6: Because the condition cannot be set
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  INTERFACEUNIFIEDLOG_RECEIVED_FROM(hApp);  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
  FRAMEWORKUNIFIEDLOG(ZONE_PERFORMANCE, __FUNCTION__, "from %s", FrameworkunifiedGetMsgSrc(hApp));

  SetCmdHist("NPS_GET_READYSTATUS_ACK", m_SMCmdHist, m_SMHistIter, FrameworkunifiedGetMsgSrc(hApp));

  l_eStatus = OnNPPReadyEventCallback(hApp);
  LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus, "OnNPPReadyEventCallback()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus CSystemManager::OnNPPReadyEventCallback(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  PCSTR l_moduleName = FrameworkunifiedGetMsgSrc(hApp);
  FRAMEWORKUNIFIEDLOG(ZONE_PERFORMANCE, __FUNCTION__, "from %s", l_moduleName);

  SetCmdHist("NPS_NPP_READY_EVENT", m_SMCmdHist, m_SMHistIter, l_moduleName); // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  if (TRUE == m_bIsNPP_ServicesStarted) {
    l_eStatus = eFrameworkunifiedStatusMsgNotProcessed;
    LOG_ERROR("TRUE == m_bIsNPP_ServicesStarted");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  } else {
    ModuleLaunchListIter l_ModuleListIter;
    m_bIsNPP_ServicesStarted = TRUE;
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " m_bIsNPP_ServicesStarted set 'TRUE'");
    m_GroupLaunchTimer->StopTimer(m_aTimerIDs[eSM_TIMER_NPP_STATUS_CHECK_MONITOR]);

    UI_32 l_cbCmdIdArrayNPPService[] = { NPS_NPP_READY_EVENT, NPS_GET_READYSTATUS_ACK };

    if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedDetachCallbacksFromDispatcher(hApp, FRAMEWORKUNIFIED_NS_NPSERVICE, l_cbCmdIdArrayNPPService, static_cast<UI_32>(_countof(l_cbCmdIdArrayNPPService))))) {  // LCOV_EXCL_BR_LINE 4:NSFW error case  //NOLINT (whitespace/line_length)
      // LCOV_EXCL_START 4: nsfw error code
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      LOG_ERROR("FrameworkunifiedDetachCallbacksFromDispatcher()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
      // LCOV_EXCL_STOP
    } else if (eFrameworkunifiedStatusOK != (l_eStatus = register_all_notification_callbacks(hApp))) {    // LCOV_EXCL_BR_LINE 4:NSFW error case  //NOLINT (whitespace/line_length)
      // LCOV_EXCL_START 4: nsfw error code
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      LOG_ERROR("register_all_notification_callbacks()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
      // LCOV_EXCL_STOP
    } else if (eFrameworkunifiedStatusOK != (l_eStatus = GetModuleIterator(l_moduleName, l_ModuleListIter))) {  // LCOV_EXCL_BR_LINE 200:NPP always in launch map  //NOLINT (whitespace/line_length)
      // LCOV_EXCL_START 200 : NPP service always in launch map
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
          " Error: Module %s not found in Group Launch Map",  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
          l_moduleName);  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
      // LCOV_EXCL_STOP
    } else {
      l_ModuleListIter->SetModuleState(MODULE_STATE_STARTED);

      if (m_oSystemLauncher.get_id() == 1) {
        m_oSystemLauncher.advance_id();
      }

      l_eStatus = OnLaunchGroup(hApp);
      LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus, "OnLaunchGroup(hApp)");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
      if (eFrameworkunifiedStatusOK == l_eStatus) {   // LCOV_EXCL_BR_LINE 200: OnLaunchGroup() return ok
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " successful");
      }
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::OnNPPReadyEventCallback( HANDLE hApp )

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnBackupMgrAvailCallback
/// BackupManager Availability Notification Callback
///
/// \param [in]
///
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnBackupMgrAvailCallback(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (sizeof(ServiceAvailability) != FrameworkunifiedGetMsgLength(hApp)) {  // LCOV_EXCL_BR_LINE 4: nsfw error
    // LCOV_EXCL_START 4: nsfw error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    SS_ASERT(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    // LCOV_EXCL_STOP
    l_eStatus = eFrameworkunifiedStatusFail;
  } else if (strcmp(FrameworkunifiedGetLastNotification(hApp), NTFY_BackupMgr_Availability) != 0) {  // LCOV_EXCL_BR_LINE 200:cannot be false //NOLINT (whitespace/line_length)
    // LCOV_EXCL_START 200: cannot be false
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    SS_ASERT(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    l_eStatus = eFrameworkunifiedStatusFail;
    // LCOV_EXCL_STOP
  } else {
    m_bIsBackupAvail = FrameworkunifiedIsServiceAvailable(hApp);
    FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "BackupAvail:%s", m_bIsBackupAvail ? "T" : "F");

    l_eStatus = OnLaunchGroup(hApp);
    LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus, "OnLaunchGroup(hApp)");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::OnBackupMgrAvailCallback(HANDLE hApp)

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnPowerRequestCallback
/// PowerRequest callback handler
///
/// \param [in]
///
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnPowerRequestCallback(HANDLE hApp) noexcept {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  Pwr_ServiceSetInterface tServiceIf;
  INTERFACEUNIFIEDLOG_RECEIVED_FROM(hApp);  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)

  SetCmdHist("SS_SM_POWER_REQUEST_MSG", m__CWORD56_CmdHist, m__CWORD56_HistIter, FrameworkunifiedGetMsgSrc(hApp));

  //  OnSystemManagerDebugDump(hApp);
  // ReadMsg():                                                        *
  //     Check hApp ptr, msg size, msg reception, read msg if all ok.  *
  //     Report any errors found.                                      *
  //                                                                   *
  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg < Pwr_ServiceSetInterface > (hApp, tServiceIf))) {  // LCOV_EXCL_BR_LINE 4:NSFW error case  //NOLINT (whitespace/line_length)
    // LCOV_EXCL_START 4: nsfw error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("ReadMsg()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    // LCOV_EXCL_STOP
  } else {
    m_lastUserMode = tServiceIf.data.powerRequestMsg.userMode;
    //
    // 2012 December 04 Temporary hack ( I hope ) to set m_Wake to nominal
    // values. At present, the 'Power Request Message Response' is sent
    // back to Power Services via the 'send_power_request_complete_response()'
    // function, which uses m_Wake.
    m_Wake.powerupType =
        m_SSBool_to_PowerTypeEnumMap[m_SSUserMode_to_SSBoolEnumMap[m_lastUserMode]];

    m_Wake.up.factor = tServiceIf.data.powerRequestMsg.startupReason;

    if (epswfINVALID != m_Wake.up.factor) {
      m_StartUpReason = m_Wake.up.factor;
    }

    m_userModeChangeReason = tServiceIf.data.powerRequestMsg.userModeChangeReason;
    m_Wake.up.userModeChangeReason = m_userModeChangeReason;

    FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "UserMode:%s, SM State:%s",
        GetStr(m_lastUserMode).c_str(),
        GetStr(m_SMCurrentState).c_str());
    FRAMEWORKUNIFIEDLOG(ZONE_PERFORMANCE, __FUNCTION__, "UserMode:%s, SM State:%s",
        GetStr(m_lastUserMode).c_str(),
        GetStr(m_SMCurrentState).c_str());
    CALL_AND_LOG_STATUS(PublishPowerOnOffNotification(hApp));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

    switch (m_SMCurrentState) {  // LCOV_EXCL_BR_LINE 6: As not all cases can be passed through
      case SS_SM_READY_TO_LAUNCH_APP:
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
            " Error: Received a PowerRequest command while still in "
            "the '%s' state !!!",
            GetStr(m_SMCurrentState).c_str());
        l_eStatus = eFrameworkunifiedStatusErrOther;
        break;

      case SS_SM_APPS_LAUNCH_IN_PROGRESS:
        // When Start is received while processes are launching, we
        // need to send the Start request later, once the 'Open
        // Session Request' is received from the application.
        //*****
        // Deliberate commenting-out of case-break to allow this case to
        // use following case's logic.
        // break;
        //*****

      case SS_SM_APPS_LAUNCHED_READY_TO_START:
      case SS_SM_APPS_START_COMPLETE:
      case SS_SM_APPS_PRE_RUN_COMPLETE:
      case SS_SM_APPS_BACKGROUND_RUN_COMPLETE:
      case SS_SM_APPS_START_IN_PROGRESS:
      case SS_SM_APPS_PRE_START_IN_PROGRESS:
      case SS_SM_APPS_PRE_STOP_IN_PROGRESS:
      case SS_SM_APPS_BACKGROUND_START_IN_PROGRESS:
      case SS_SM_APPS_BACKGROUND_STOP_IN_PROGRESS:
        FRAMEWORKUNIFIEDLOG(ZONE_PERFORMANCE, __FUNCTION__, "Startup Reason is '%s'", GetStr(m_StartUpReason).c_str());
        if ((m_userModeChangeReason == epsumcrPARKING_B)
            || ((m_userModeChangeReason == epsumcrPRE_BA) && (m_SMCurrentState == SS_SM_APPS_PRE_RUN_COMPLETE))
            || ((m_userModeChangeReason == epsumcrNORMAL) && (m_SMCurrentState == SS_SM_APPS_START_COMPLETE))
            || ((m_userModeChangeReason == epsumcrBACKGROUND_BA)
                && (m_SMCurrentState == SS_SM_APPS_BACKGROUND_RUN_COMPLETE))) {
          // send the Power Request complete response to PowerService
          // LCOV_EXCL_BR_START 15: Excluded due to inlined functions
          CALL_AND_LOG_STATUS_IF_ERRORED(send_power_request_complete_response(hApp, "Power Request"));
          // LCOV_EXCL_BR_STOP 15: Excluded due to inlined functions
        } else {
          //
          // Publish the UserMode 'Off' notification now for those
          // applications that need immediate notification ( HMI )
          if (m_lastUserMode == epsumOFF) {
            CALL_AND_LOG_STATUS(PublishUserModeNotification(hApp));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
          }

          CALL_AND_LOG_STATUS(BeginStartup(hApp));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
        }
        break;

      case SS_SM_APPS_STOPPING_AT__CWORD56__REQ:
      case SS_SM_WAITING_FOR_CRITICAL_APPS_AT__CWORD56__REQ:
        // Abort the Stop.
        // Reset System Manager to handle (re)starting as if this is a
        // nominal start.
        m_GroupLaunchTimer->StopTimer(m_aTimerIDs[eSM_TIMER_CLIENT_STOP_MONITOR]);

        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Aborted %s, resetting to %s,"
            " restarting Application groups at group %d",
            GetStr(m_SMCurrentState).c_str(),
            GetStr(SS_SM_APPS_START_IN_PROGRESS).c_str()
            , m_SystemStarter.get_id());

        CALL_AND_LOG_STATUS(BeginStartup(hApp));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

        break;

      case SS_SM_APPS_STOPPING_AT_INTERNAL_REQ:
      case SS_SM_WAITING_FOR_CRITICAL_APPS_AT_INTERNAL_REQ:  // LCOV_EXCL_START 8: As no value is set
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__,
              " Warning: System Manager state is %d/'%s', ignoring "
              "PowerRequest command", m_SMCurrentState,
              GetStr(m_SMCurrentState).c_str());
          break;
      }  // LCOV_EXCL_STOP
    }
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
    return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::OnPowerRequestCallback( HANDLE hApp )

///////////////////////////////////////////////////////////////////////////////
/// \ingroup
/// Begin the Startup process
///
/// \param [in]
///
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::BeginStartup(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if ((m_userModeChangeReason != epsumcrPARKING_B) &&
      (m_userModeChangeReason != epsumcrPRE_BA) &&
      (m_userModeChangeReason != epsumcrNORMAL) &&
      (m_userModeChangeReason != epsumcrBACKGROUND_BA)) {
    // Not a power state transition request.
    return eFrameworkunifiedStatusOK;
  }

  switch (m_userModeChangeReason) {  // LCOV_EXCL_BR_LINE 6: Excluded because it has been guarded to prevent out-of-range values from entering immediately before.
  case epsumcrPRE_BA:
    // LCOV_EXCL_BR_START 6: This function is an internal function, and it is guarded so that the value outside the range is not entered by the caller, so it is excluded.
    if ((m_SMCurrentState == SS_SM_APPS_START_COMPLETE) || (m_SMCurrentState == SS_SM_APPS_BACKGROUND_RUN_COMPLETE)) {
      // LCOV_EXCL_BR_STOP 6: This function is an internal function, and it is guarded so that the value outside the range is not entered by the caller, so it is excluded.
      m_SystemStarter.to_end();
      SMSetState(hApp, SS_SM_APPS_PRE_STOP_IN_PROGRESS);
    } else {
      m_SystemStarter.to_begin();
      SMSetState(hApp, SS_SM_APPS_PRE_START_IN_PROGRESS);
    }
    break;
  case epsumcrNORMAL:
    m_SystemStarter.to_begin();
    SMSetState(hApp, SS_SM_APPS_START_IN_PROGRESS);
    break;
  case epsumcrBACKGROUND_BA:
    if (m_SMCurrentState == SS_SM_APPS_START_COMPLETE) {
      m_SystemStarter.to_end();
      SMSetState(hApp, SS_SM_APPS_BACKGROUND_STOP_IN_PROGRESS);
    } else {
      m_SystemStarter.to_begin();
      SMSetState(hApp, SS_SM_APPS_BACKGROUND_START_IN_PROGRESS);
    }
    break;
  default:
    break;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
      " Starting 'Start Resp Monitor' Timer with '%d' sec",
      m_SMConfig.MCConfig.ModuleStartRespTimeOutSec);

  m_GroupLaunchTimer->StartTimer(
      m_aTimerIDs[eSM_TIMER_START_RESP_MONITOR_WAIT_TIMER],
      m_SMConfig.MCConfig.ModuleStartRespTimeOutSec, 0, 0, 0);

  l_eStatus = start_all_modules_of_group(hApp, m_SystemStarter.get_id());
  if (eFrameworkunifiedStatusOK != l_eStatus) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        " Error: start_all_modules_of_group(%d) errored: %d/'%s'",
        m_SystemStarter.get_id(), l_eStatus, GetStr(l_eStatus).c_str());
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::BeginStartup( HANDLE hApp )

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnWakeupCallback
/// WakeUp callback handler
///
/// \param [in]
///
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnWakeupCallback(HANDLE hApp) {  // LCOV_EXCL_START 6: Because the condition cannot be set
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  INTERFACEUNIFIEDLOG_RECEIVED_FROM(hApp);
  FRAMEWORKUNIFIEDLOG(ZONE_PERFORMANCE, __FUNCTION__, "Received from %s", FrameworkunifiedGetMsgSrc(hApp));

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::OnWakeupCallback( HANDLE hApp )
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnModuleStartCompleteResponse
/// Start Response\Ack Handlers
///
/// \param [in]
///
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnModuleStartCompleteResponse(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  INTERFACEUNIFIEDLOG_RECEIVED_FROM(hApp);  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
  FRAMEWORKUNIFIEDLOG(ZONE_PERFORMANCE, __FUNCTION__, "%s", FrameworkunifiedGetMsgSrc(hApp));

  SetCmdHist("system_manager protocol completion response", m_SMCmdHist, m_SMHistIter, FrameworkunifiedGetMsgSrc(hApp));  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  ModuleLaunchListIter l_ModuleListIter;
  std::string l_moduleName = FrameworkunifiedGetMsgSrc(hApp);
  l_eStatus = GetModuleIterator(l_moduleName.c_str(), l_ModuleListIter);
  if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 200:always be eFrameworkunifiedStatusOK
    // LCOV_EXCL_START 200: always be eFrameworkunifiedStatusOK
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        " Error: Module %s not found in Group Launch Map",
        l_moduleName.c_str());
    // LCOV_EXCL_STOP
  } else {
    switch (m_SMCurrentState) {  // LCOV_EXCL_BR_LINE 200: cannot be SS_SM_READY_TO_LAUNCH_APP
    case SS_SM_READY_TO_LAUNCH_APP:
      // LCOV_EXCL_START 200: cannot be SS_SM_READY_TO_LAUNCH_APP
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
          " Error: Received while in the '%s' state !!!",
          GetStr(m_SMCurrentState).c_str());
      l_eStatus = eFrameworkunifiedStatusErrOther;
      break;
      // LCOV_EXCL_STOP

    case SS_SM_APPS_LAUNCH_IN_PROGRESS:
    case SS_SM_APPS_LAUNCHED_READY_TO_START:
    case SS_SM_APPS_START_IN_PROGRESS:
    case SS_SM_APPS_PRE_START_IN_PROGRESS:
    case SS_SM_APPS_PRE_STOP_IN_PROGRESS:
    case SS_SM_APPS_BACKGROUND_START_IN_PROGRESS:
    case SS_SM_APPS_BACKGROUND_STOP_IN_PROGRESS:
      {
        const SMModuleState l_state = InProgressStateToState();  // LCOV_EXCL_BR_LINE 11: Gcov constraints (because exception-handling routes are automatically generated) // NOLINT(whitespace/line_length)
        if (l_state != MODULE_STATE_INVALID) {
          const PCSTR l_loglist[] = {"start", "pre start", "pre stop", "background start", "background stop", ""};
          PCSTR l_logtxt;
          switch (l_state) {  // LCOV_EXCL_BR_LINE 6: As no other value is returned
          case MODULE_STATE_STARTED:
            l_logtxt = l_loglist[0];
            break;
          case MODULE_STATE_STARTED_PRE:
            l_logtxt = l_loglist[1];
            break;
          case MODULE_STATE_STOPPED_PRE:
            l_logtxt = l_loglist[2];
            break;
          case MODULE_STATE_STARTED_BACKGROUND:
            l_logtxt = l_loglist[3];
            break;
          case MODULE_STATE_STOPPED_BACKGROUND:
            l_logtxt = l_loglist[4];
            break;
          // LCOV_EXCL_START 6: As no other value is returned
          default:
            AGL_ASSERT_NOT_TESTED();
            l_logtxt = l_loglist[5];
            break;
          // LCOV_EXCL_STOP 6: As no other value is returned
          }
          l_eStatus = ModuleCompleteResponse(hApp, l_ModuleListIter, l_state,  // LCOV_EXCL_BR_LINE 11: Gcov constraints (because exception-handling routes are automatically generated) // NOLINT(whitespace/line_length)
                                             SysMgrCbType3<CSystemManager, &CSystemManager::IsGroupStarted>,
                                             SysMgrCbType2<CSystemManager, &CSystemManager::ProcessGroupAsStarted>,
                                             l_logtxt);
        }
      }
      break;

    case SS_SM_APPS_STOPPING_AT__CWORD56__REQ:
    case SS_SM_APPS_STOPPING_AT_INTERNAL_REQ:
    case SS_SM_WAITING_FOR_CRITICAL_APPS_AT__CWORD56__REQ:
    case SS_SM_WAITING_FOR_CRITICAL_APPS_AT_INTERNAL_REQ:
      // SystemManager changed state while this module was starting;
      // tell this module to stop.
      if (!l_ModuleListIter->IsModuleState(MODULE_STATE_STOP_SENT)
          && !l_ModuleListIter->IsModuleState(MODULE_STATE_STOPPED)) {
        T_SS_SM_STOP_DataStructType l_SM_STOP_Struct;
        l_SM_STOP_Struct.shutdownTrigger = m_shutdownTrigger;
        l_SM_STOP_Struct.lastUserMode = m_lastUserMode;

        CALL_AND_LOG_STATUS(                               // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
            l_ModuleListIter->SendMsgAndUpdateState(
                &l_SM_STOP_Struct));
      }
      break;
    default:
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: Received while in the '%s' state !!!", GetStr(m_SMCurrentState).c_str());
      l_eStatus = eFrameworkunifiedStatusErrOther;
      break;
    }        // End switch ( m_SMCurrentState )
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::OnModuleStartCompleteResponse( HANDLE hApp )

///////////////////////////////////////////////////////////////////////////////
/// \ingroup ModuleCompleteResponse
/// Start Response\Ack Handlers
///
/// \param [in]
///
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::ModuleCompleteResponse(HANDLE hApp,
        ModuleLaunchListIter f_ModuleListIter, SMModuleState f_moduleState,
        FncCbType3 f_isGroupDoneFnc, FncCbType2 f_groupDoneFnc,
        PCSTR f_sCompleteTypeText) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  std::string l_moduleName = FrameworkunifiedGetMsgSrc(hApp);
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
      " Received from %s, m_SystemStarter.get_id() is %d",
      l_moduleName.c_str()
      , m_SystemStarter.get_id());

  f_ModuleListIter->SetModuleState(f_moduleState);
  if (f_ModuleListIter->group_id != m_SystemStarter.get_id()) {
    // In PastModel002, it is assumed that there is no service that returns a FrameworkunifiedOnStop reply after the NPP service.
    // However, since _CWORD71_ does not assume this, completion of waiting for termination of the CRITICAL service must be
    // judged even when services other than the Group1 service terminate last.
    // Initially, SS_SM_WAITING_FOR_CRITICAL_APPS_AT_XXXX should be handled separately by ProcessModuleStopCompleteResponse(),
    // but this is limited to ModuleCompleteResponse in order to avoid code deviation from PastModel002 and limit the scope of effect.
    // LCOV_EXCL_BR_START 200 : m_SMCurrentState can't be SS_SM_WAITING_FOR_CRITICAL_APPS_AT__CWORD56__REQ and SS_SM_WAITING_FOR_CRITICAL_APPS_AT_INTERNAL_REQ  // NOLINT(whitespace/line_length)
    if ((MODULE_STATE_STOPPED == f_moduleState)
        && ((m_SMCurrentState
            == SS_SM_WAITING_FOR_CRITICAL_APPS_AT__CWORD56__REQ)
            || (m_SMCurrentState
                == SS_SM_WAITING_FOR_CRITICAL_APPS_AT_INTERNAL_REQ))) {
    // LCOV_EXCL_BR_STOP 200 : m_SMCurrentState can't be SS_SM_WAITING_FOR_CRITICAL_APPS_AT__CWORD56__REQ and SS_SM_WAITING_FOR_CRITICAL_APPS_AT_INTERNAL_REQ  // NOLINT(whitespace/line_length)
      // LCOV_EXCL_START 200 : m_SMCurrentState can't be SS_SM_WAITING_FOR_CRITICAL_APPS_AT__CWORD56__REQ and SS_SM_WAITING_FOR_CRITICAL_APPS_AT_INTERNAL_REQ  // NOLINT(whitespace/line_length)
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      check_all_groups_have_stopped(hApp);
      // LCOV_EXCL_STOP
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
          " %s ( group %d ) received out-of-synch; m_SystemStarter.get_id() is %d",
          l_moduleName.c_str(), f_ModuleListIter->group_id
          , m_SystemStarter.get_id());
    }
  } else {
    if ((*f_isGroupDoneFnc)(f_ModuleListIter->group_id)) {  // LCOV_EXCL_BR_LINE 11: Gcov constraints (because exception-handling routes are automatically generated) // NOLINT(whitespace/line_length)
      l_eStatus = (*f_groupDoneFnc)(hApp, f_ModuleListIter->group_id); // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    } else if (IS_ZONE_SET(ZONE_INFO)) {  // LCOV_EXCL_BR_LINE 6: Because the applicable variable cannot be changed from the external API (only evaluated by the initial value)
      // LCOV_EXCL_START 6: Because the applicable variable cannot be changed from the external API (only evaluated by the initial value)
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      TEXT(__FUNCTION__,  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
          " Group %d %s incomplete, m_SystemStarter.get_id() is %d",  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
          f_ModuleListIter->group_id, f_sCompleteTypeText  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
          , m_SystemStarter.get_id());  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
      char cBuf[100];
      sprintf(cBuf, "%s incomplete", f_sCompleteTypeText);  // NOLINT
      LogGroupModulesState(f_ModuleListIter->group_id, cBuf);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
      // LCOV_EXCL_STOP
    }
  }

  const SMModuleState l_transition_state = InProgressStateToState();  // LCOV_EXCL_BR_LINE 11: Gcov constraints (because exception-handling routes are automatically generated) // NOLINT(whitespace/line_length)
  if (l_transition_state == f_moduleState) {  // LCOV_EXCL_BR_LINE 6:  As no other value is returned
    if (have_all_services_start_completed(l_transition_state) == TRUE) {
      if ((m_SMCurrentState == SS_SM_APPS_PRE_START_IN_PROGRESS)
          || (m_SMCurrentState == SS_SM_APPS_PRE_STOP_IN_PROGRESS)) {
        SMSetState(hApp, SS_SM_APPS_PRE_RUN_COMPLETE);  // LCOV_EXCL_BR_LINE 11: Gcov constraints (because exception-handling routes are automatically generated) // NOLINT(whitespace/line_length)
      } else if ((m_SMCurrentState == SS_SM_APPS_BACKGROUND_START_IN_PROGRESS)
          || (m_SMCurrentState == SS_SM_APPS_BACKGROUND_STOP_IN_PROGRESS)) {
        SMSetState(hApp, SS_SM_APPS_BACKGROUND_RUN_COMPLETE);  // LCOV_EXCL_BR_LINE 11: Gcov constraints (because exception-handling routes are automatically generated) // NOLINT(whitespace/line_length)
      } else {
        SMSetState(hApp, SS_SM_APPS_START_COMPLETE);  // LCOV_EXCL_BR_LINE 11: Gcov constraints (because exception-handling routes are automatically generated) // NOLINT(whitespace/line_length)
      }
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::ModuleCompleteResponse(

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnStartRespMonitorTimeout
///  SM maintains timer to check if all modules sent start response to him whoever requires Start.
///  This timer is common to all modules. After timeout it checks the status of every module in group map.
///  If a module sent start and does not respond, SM will initiate error event logging followed by Soft Reset
/// \param [in]
///
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnStartRespMonitorTimeout(HANDLE hApp) {  // LCOV_EXCL_START 6: Because the condition cannot be set
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  INTERFACEUNIFIEDLOG_RECEIVED_FROM(hApp);  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
  FRAMEWORKUNIFIEDLOG(ZONE_PERFORMANCE, __FUNCTION__, "from %s", FrameworkunifiedGetMsgSrc(hApp));

  std::string l_ProcName;
  BOOL l_isStartRespNotReceived = false;

  SetCmdHist("SM_TIMER_START_RESP_MONITOR_WAIT_TIMER", m_TimerCmdHist,
      m_TimerHistIter, FrameworkunifiedGetMsgSrc(hApp));
  GroupLaunchMap::reverse_iterator l_grp_riter = m_MapProclaunchGrps.rbegin();
  ModuleLaunchList::reverse_iterator l_mod_riter;

  if (l_grp_riter == m_MapProclaunchGrps.rend()) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: Group Map is Empty");
  } else {
    // scan the group map in reverse order
    for (; l_grp_riter != m_MapProclaunchGrps.rend(); l_grp_riter++) {
      l_mod_riter = l_grp_riter->second.modules.rbegin();
      for (; l_mod_riter != l_grp_riter->second.modules.rend(); l_mod_riter++) {
        // if a module sent start and not received response from module SM wait for "ModuleStartRespTimeout"
        // and initiated error event
        // logging followed by soft reset

        if (l_mod_riter->is_start_required) {
          if (l_mod_riter->IsModuleState(MODULE_STATE_START_SENT)) {
            FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__,
                " Module [%s] did not respond(%s) to SM in allocated time [%d] sec",
                l_mod_riter->name.c_str(),
                l_mod_riter->ModuleStateStr().c_str(),
                m_SMConfig.MCConfig.ModuleStartRespTimeOutSec);
            l_isStartRespNotReceived = true;
            l_ProcName.assign(l_mod_riter->name);
          }
        }
      }
    }
  }

  if (true == l_isStartRespNotReceived) {
    char l_cFormat[] = " Error. Module '%s' start response timeout.";
    char l_cBuf[sizeof(l_cFormat) + MAX_NAME_SIZE_APP];
    snprintf(l_cBuf, sizeof(l_cBuf), l_cFormat, l_ProcName.c_str());
    TEXT(__FUNCTION__, l_cBuf);  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
    FRAMEWORKUNIFIEDLOG(ZONE_PERFORMANCE, __FUNCTION__, l_cBuf);

    SMLoggingInfo l_loggingInfo;
    l_loggingInfo.resetReason = e_SS_SM_CPU_RESET_REASON_GENERIC_ERR;

    // initate error event logging
    l_eStatus = ErrorEventEnqueue(hApp, eErrorEventTypeStartRespFailed,
        l_ProcName, eErrorEventResetTypeHard, l_loggingInfo);
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: StartRespFailed %s", l_ProcName.c_str());
    fprintf(stderr, "SS_SysManager/%s/Error: StartRespFailed %s\n",
        __FUNCTION__, l_ProcName.c_str());
    LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus,  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
        "ErrorEventEnqueue(eErrorEventTypeStartRespFailed)");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_PERFORMANCE, __FUNCTION__,
        " All modules returned Start Response.");

    l_eStatus = FrameworkunifiedSendChild(hApp, m_hHeartbeatThread.m_ThreadHdl,
        SS_HEARTBEAT_AVAIL_CHECK_REQ, 0, NULL);
    SS_ASERT(l_eStatus == eFrameworkunifiedStatusOK);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::OnStartRespMonitorTimeout(HANDLE hApp)
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnShutdownCompleteMonitorTimeout
///  SM maintains timer to check if all modules sent start response to him whoever requires Start.
///  This timer is common to all modules. After timeout it checks the status of every module in group map.
///  If a module sent start and does not respond, SM will initiate error event logging followed by Soft Reset
/// \param [in]
///
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnShutdownCompleteMonitorTimeout(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  INTERFACEUNIFIEDLOG_RECEIVED_FROM(hApp);  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)

  FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: Reset Timeout");

  CALL_AND_LOG_STATUS(PerformUmountRamd());  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  l_eStatus = SecureChipOff();
  if (eFrameworkunifiedStatusOK != l_eStatus) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: SecureChipOff Failed");
  }

  l_eStatus = perform_force_reset(hApp);

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::OnShutdownCompleteMonitorTimeout(HANDLE hApp)

///////////////////////////////////////////////////////////////////////////////
/// \ingroup start_all_modules_of_group
/// Send START to all modules of the given group
///
/// \param [in]
///
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::start_all_modules_of_group(HANDLE hApp,
        UI_32 f_ui32GroupNumber) noexcept {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  GroupLaunchMapIter l_GroupIterator = m_MapProclaunchGrps.find(
      f_ui32GroupNumber);
  if (l_GroupIterator == m_MapProclaunchGrps.end()) {  // LCOV_EXCL_BR_LINE 6: Because the condition cannot be set
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR,
        __FUNCTION__
        , " Error: Group %d not found; 'm_MapProclaunchGrps' is empty",
        f_ui32GroupNumber);
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_eStatus = eFrameworkunifiedStatusInvldParam;  // LCOV_EXCL_LINE 6: Because the condition cannot be set
  } else {
    T_SS_SM_START_DataStructType f_startupData(m_StartUpReason,
        m_PowerType_to_SSBoolEnumMap[m_Wake.powerupType],
        m_DataResetMode, m_startUpConfirmationMsg.securityStatus,
        m_startUpConfirmationMsg.wakeupType, m_DramBackupStatus,
        m_ResetStatus, m_ResetCount);

    UI_32 l_numModulesMessaged = 0;
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
        " Attempting to send Start Requests to Group %d/'%s'",
        f_ui32GroupNumber, l_GroupIterator->second.name.c_str());

    for (ModuleLaunchListIter l_ModuleListIterator =
        l_GroupIterator->second.modules.begin();
        l_ModuleListIterator != l_GroupIterator->second.modules.end();
        l_ModuleListIterator++) {
      //
      // Every branch below must ZONE_INFO log how it dispenses w/
      // the module.
      //  Note: ModuleLaunchParams::SetModuleState() performs that
      //    ZONE_INFO logging internally.
      if (!l_ModuleListIterator->is_start_required) {
        if (FRAMEWORKUNIFIED_NS_NPSERVICE == l_ModuleListIterator->name) {
          const SMModuleState l_transition_state = InProgressStateToState();
          l_ModuleListIterator->SetModuleState(l_transition_state);
        } else {
          // Not an error
          FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
              " %s 'is_start_required' is False;"
                  " leaving state as '%s'",
              l_ModuleListIterator->name.c_str(),
              l_ModuleListIterator->ModuleStateStr().c_str());
        }
        l_eStatus = eFrameworkunifiedStatusOK;
      } else if (NULL == l_ModuleListIterator->hsession) {  // LCOV_EXCL_BR_LINE 6: hsession can not be null.
        l_eStatus = eFrameworkunifiedStatusOK;  // Log a message. But its not a failure
        FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, " Warning: NULL == hsession;"
            " %s not connected yet",
            l_ModuleListIterator->name.c_str());
      } else {
        switch (l_ModuleListIterator->GetModuleState()) {  // LCOV_EXCL_BR_LINE 6: As not all cases can be passed through
        case MODULE_STATE_INVALID:
        case MODULE_STATE_SKIPPED:
        case MODULE_STATE_LAUNCH_FAILED:
          // LCOV_EXCL_START 6: Because the state of the corresponding variable cannot be set
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          l_eStatus = eFrameworkunifiedStatusOK;  // Log a message. But its not a failure
          FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
              " %s is %s, NOT sending system_manager protocol message",
              l_ModuleListIterator->name.c_str(),
              l_ModuleListIterator->ModuleStateStr().c_str());
          break;
          // LCOV_EXCL_STOP

        case MODULE_STATE_START_SENT:
        case MODULE_STATE_START_PRE_SENT:
        case MODULE_STATE_STOP_PRE_SENT:
        case MODULE_STATE_START_BACKGROUND_SENT:
        case MODULE_STATE_STOP_BACKGROUND_SENT:
          l_eStatus = eFrameworkunifiedStatusOK;
          FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
              " %s is %s, NOT sending system_manager protocol message",
              l_ModuleListIterator->name.c_str(),
              l_ModuleListIterator->ModuleStateStr().c_str());
          l_numModulesMessaged++;
          break;

        case MODULE_STATE_LAUNCHING:
        case MODULE_STATE_LAUNCHED:
          // LCOV_EXCL_START 6: Because the state of the corresponding variable cannot be set
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          l_eStatus = eFrameworkunifiedStatusOK;  // Log a message. But its not a failure
          FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__,
              " %s is %s, NOT sending system_manager protocol message",
              l_ModuleListIterator->name.c_str(),
              l_ModuleListIterator->ModuleStateStr().c_str());
          break;
          // LCOV_EXCL_STOP

        case MODULE_STATE_CONNECTED:
        case MODULE_STATE_STARTED:
        case MODULE_STATE_START_FAILED:
        case MODULE_STATE_STOP_FAILED:
        case MODULE_STATE_STOP_SENT:
        case MODULE_STATE_STOPPED:
        case MODULE_STATE_STARTED_PRE:
        case MODULE_STATE_START_PRE_FAILED:
        case MODULE_STATE_STOPPED_PRE:
        case MODULE_STATE_STOP_PRE_FAILED:
        case MODULE_STATE_STARTED_BACKGROUND:
        case MODULE_STATE_START_BACKGROUND_FAILED:
        case MODULE_STATE_STOPPED_BACKGROUND:
        case MODULE_STATE_STOP_BACKGROUND_FAILED:
          {
            const UI_32 l_iCmd = InProgressStateToSendMsg();
            if (l_iCmd != SS_SYSTEM_MANAGER_PROTOCOL_ENDING_INDEX) {  // LCOV_EXCL_BR_LINE 6: Excluded due to value never returned at present
              // LCOV_EXCL_BR_START 15: Excluded due to inlined functions
              CALL_AND_LOG_STATUS_IF_ERRORED(l_ModuleListIterator->SendMsgAndUpdateState(l_iCmd, &f_startupData));
              // LCOV_EXCL_BR_STOP 15: Excluded due to inlined functions
              l_numModulesMessaged++;
            }
          }
          break;
          // default:  Don't code a 'default' here - let the compiler
          // detect when the set of module_state enumerations changes -
          // then the System Service's System Manager maintainer will
          // automagically know to update this switch statement.
        }
      }
    }

    if (0 == l_numModulesMessaged) {
      LogGroupModulesState(f_ui32GroupNumber, " Warning: NO modules were sent system_manager protocol message");

      char l_cBuf[100];
      snprintf(l_cBuf, sizeof(l_cBuf), "ProcessGroupAsStarted(%d)", l_GroupIterator->second.id);
      l_eStatus = ProcessGroupAsStarted(hApp, l_GroupIterator);
      LOG_STATUS_REC_HIST_IF_ERRORED(l_eStatus, l_cBuf);  // LCOV_EXCL_BR_LINE 6: Because the condition cannot be set
    } else {
      m_GroupLaunchTimer->StartTimer(
          m_aTimerIDs[eSM_TIMER_CLIENT_START_MONITOR],
          SS_CLIENT_START_MONITOR_TIMER_CONFIG, 0, 0, 0);
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::start_all_modules_of_group(HANDLE hApp, UI_32 f_ui32GroupNumber)

///////////////////////////////////////////////////////////////////////////////
/// \ingroup start_all_modules_of_group
/// mark the specified group as started. If there is another group to start,
/// do so, else send Start Complete Response to the Start Requester.
///
/// \param [in]
///
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::ProcessGroupAsStarted(HANDLE hApp, UI_32 f_groupId) {
  EFrameworkunifiedStatus l_eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  GroupLaunchMapIter l_GroupIterator = m_MapProclaunchGrps.find(f_groupId);
  if (l_GroupIterator == m_MapProclaunchGrps.end()) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        " Error: Group %d not found; 'm_MapProclaunchGrps' is empty", f_groupId);
    l_eStatus = eFrameworkunifiedStatusInvldParam;
  } else {
    l_eStatus = ProcessGroupAsStarted(hApp, l_GroupIterator);
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::ProcessGroupAsStarted( HANDLE hApp, UI_32 f_groupId )

///////////////////////////////////////////////////////////////////////////////
/// \ingroup start_all_modules_of_group
/// mark the specified group as started. If there is another group to start,
/// do so, else send Start Complete Response to the Start Requester.
///
/// \param [in]
///
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::ProcessGroupAsStarted(HANDLE hApp,
        GroupLaunchMapIter f_GroupIterator) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  UI_32 l_groupID = m_SystemStarter.get_id();


  m_GroupLaunchTimer->StopTimer(m_aTimerIDs[eSM_TIMER_CLIENT_START_MONITOR]);

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
      " Group %d/%s start completed, m_SystemStarter.get_id() is %d",
      f_GroupIterator->second.id, f_GroupIterator->second.name.c_str(),
      l_groupID);

  if (f_GroupIterator->second.id != l_groupID) {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO,
        __FUNCTION__
        , " The just-started group ( %d/%s ) is not the current Active "
            " Group ( %d ); not starting next group's modules",
        f_GroupIterator->second.id,
        f_GroupIterator->second.name.c_str(), l_groupID);
  } else {
    if ((m_SMCurrentState == SS_SM_APPS_START_IN_PROGRESS)
        || (m_SMCurrentState == SS_SM_APPS_PRE_START_IN_PROGRESS)
        || (m_SMCurrentState == SS_SM_APPS_BACKGROUND_START_IN_PROGRESS)) {
      // check if WakeUp Level has been achieved
      if (m_SystemStarter.is_end() == FALSE) {
        // Start modules from next group
        l_groupID = m_SystemStarter.advance_id();
        char l_cBuf[100] = {0};
        snprintf(l_cBuf, sizeof(l_cBuf), "start_all_modules_of_group(%u)", l_groupID);
        l_eStatus = start_all_modules_of_group(hApp, l_groupID);
        LOG_STATUS_REC_HIST_IF_ERRORED(l_eStatus, l_cBuf);   // LCOV_EXCL_BR_LINE 6: Because the condition cannot be set
      }
    } else if ((m_SMCurrentState == SS_SM_APPS_PRE_STOP_IN_PROGRESS)
        || (m_SMCurrentState == SS_SM_APPS_BACKGROUND_STOP_IN_PROGRESS)) {
      if (m_SystemStarter.is_begin() == FALSE) {
        // Stop modules from next group
        l_groupID = m_SystemStarter.decrement_id();
        char l_cBuf[100] = {0};
        snprintf(l_cBuf, sizeof(l_cBuf), "stop_all_modules_of_group(%u)", l_groupID);
        l_eStatus = start_all_modules_of_group(hApp, l_groupID);
        LOG_STATUS_REC_HIST_IF_ERRORED(l_eStatus, l_cBuf);   // LCOV_EXCL_BR_LINE 6: Because the condition cannot be set
      }
    } else {
      // MISRA C++-2008 Rule 6-4-2
      // NOP
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::ProcessGroupAsStarted( GroupLaunchMapIter f_GroupIterator )

///////////////////////////////////////////////////////////////////////////////
/// \ingroup calculate_max_shutdown_time
/// Calculate maximum shutdown time of apps critical to shutdown
/// from the launch group map
///
/// \param [in]
///
/// \return UI_32
/// Max Value of critical shutdown apps shutdown time
///////////////////////////////////////////////////////////////////////////////
UI_32 CSystemManager::calculate_max_shutdown_time() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  UI_32 l_MaxShutdownTimeValue = 0;
  std::string moduleName;

  GroupLaunchMapIter l_GroupIterator = m_MapProclaunchGrps.begin();
  for (; l_GroupIterator != m_MapProclaunchGrps.end(); l_GroupIterator++) {
    ModuleLaunchListIter l_ModuleListIterator =
        l_GroupIterator->second.modules.begin();
    for (; l_ModuleListIterator != l_GroupIterator->second.modules.end();
        l_ModuleListIterator++) {
      if (l_ModuleListIterator->shutdown_critical) {
        if (l_MaxShutdownTimeValue < l_ModuleListIterator->shutdown_wait_time) {
          moduleName = l_ModuleListIterator->name;
          l_MaxShutdownTimeValue = l_ModuleListIterator->shutdown_wait_time;
        }
      }
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Max time: Module %s, time %d s",
      moduleName.c_str(), l_MaxShutdownTimeValue);
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_MaxShutdownTimeValue;
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnShutdownModulesRequest
/// Send Shutdown to all modules in reverse order of group
///
/// \param [in]
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnShutdownModulesRequest(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__,
      " Received from %s, m_SystemStarter.get_id() is %d",
      FrameworkunifiedGetMsgSrc(hApp)
      , m_SystemStarter.get_id());

  FRAMEWORKUNIFIEDLOG(ZONE_PERFORMANCE, __FUNCTION__, "m_SystemStarter.get_id() is %d", m_SystemStarter.get_id());

  SetCmdHist("SS_SM_SHUTDOWN_MODULES", m__CWORD56_CmdHist, m__CWORD56_HistIter, FrameworkunifiedGetMsgSrc(hApp)); // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  Pwr_ServiceSetInterface l_tServiceSetIf;
  // ReadMsg():                              *
  //   Check hApp ptr, msg size, msg reception, read msg if all ok.    *
  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg < Pwr_ServiceSetInterface > (hApp, l_tServiceSetIf))) {   // LCOV_EXCL_BR_LINE 4: nsfw error
    // LCOV_EXCL_START 4: nsfw error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("ReadMsg()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    // LCOV_EXCL_STOP
  } else {
    BOOL isImmShutdown = FALSE;

    // If the Shutdown is not completed after the Shutdown is requested, it is forcibly reset.
    m_GroupLaunchTimer->StartTimer(
        m_aTimerIDs[eSM_TIMER_SHUTDOWN_COMPLETE_MONITOR],
        SM_SHUTDOWN_COMPLETE_MONITOR_TIMEOUT, 0, 0, 0);

    if (m_isImmResetReq) {
      FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, " ImmReset");
      isImmShutdown = TRUE;
    }

    switch (l_tServiceSetIf.data.shutdownRequestMsg.lastUserMode) {
    case epsumINVALID:
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
          " Error: shutdownRequestMsg.lastUserMode == epsumINVALID");
      l_eStatus = eFrameworkunifiedStatusInvldParam;
      break;

    case epsumOFF:
    case epsumON: {
      switch (m_SMCurrentState) {
      case SS_SM_READY_TO_LAUNCH_APP:
      case SS_SM_APPS_LAUNCH_IN_PROGRESS:
      case SS_SM_APPS_LAUNCHED_READY_TO_START:
        isImmShutdown = TRUE;
      case SS_SM_APPS_START_COMPLETE:
      case SS_SM_APPS_START_IN_PROGRESS:
      case SS_SM_APPS_PRE_START_IN_PROGRESS:
      case SS_SM_APPS_PRE_STOP_IN_PROGRESS:
      case SS_SM_APPS_PRE_RUN_COMPLETE:
      case SS_SM_APPS_BACKGROUND_START_IN_PROGRESS:
      case SS_SM_APPS_BACKGROUND_STOP_IN_PROGRESS:
      case SS_SM_APPS_BACKGROUND_RUN_COMPLETE:

        if (l_tServiceSetIf.data.shutdownRequestMsg.shutdownTrigger == epssdmsdtIGN_OFF) {
          ROM_AccessIf rom_access;

          rom_access.Initialize(); // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
          if (eFrameworkunifiedStatusOK != (l_eStatus = SetErrLogCount(rom_access, 0))) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "SetErrLogCount(%d)", l_eStatus);
          }

          m_ResetCount = 0;
          if (eFrameworkunifiedStatusOK != (l_eStatus = SetResetCount(rom_access, m_ResetCount))) {
              FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "SetResetCount(%d)", l_eStatus);
          }
        }

        if (!m_isPrevErrEventCompleted
            || ((!((m_SMCurrentState == SS_SM_APPS_START_COMPLETE)
                || (m_SMCurrentState == SS_SM_APPS_PRE_RUN_COMPLETE)
                || (m_SMCurrentState == SS_SM_APPS_BACKGROUND_RUN_COMPLETE)))
                && (l_tServiceSetIf.data.shutdownRequestMsg.shutdownTrigger
                    == epssdmsdtIGN_OFF
                    || l_tServiceSetIf.data.shutdownRequestMsg.shutdownTrigger
                        == epssdmsdtNORMAL_RESET))) {
          FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "shutdown pending");
          if (!m_dqDeferMsg.empty()) {
            SS_ASERT(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
          } else {
            SM_POWER_EVENT_INFO deferMsg(SS_SM_SHUTDOWN_MODULES, l_tServiceSetIf);
            m_dqDeferMsg.push(deferMsg);
          }
        } else {
          // The shutdownTrigger has already been validated by
          // Power::OnShutdownRequestMsg()
          m_shutdownTrigger = l_tServiceSetIf.data.shutdownRequestMsg.shutdownTrigger;
          m_lastUserMode = l_tServiceSetIf.data.shutdownRequestMsg.lastUserMode;

          // Notify services ShutDownTrigger the FrameworkunifiedOnStop, if dataResetMode is configured
          // When using FastSleep, be aware that it may be overwritten here
          switch (m_DataResetMode) {
          case e_SS_SM_DATA_RESET_MODE_FACTORY:
            m_shutdownTrigger = epssdmsdtFACTORY_DATA_RESET;
            break;
          case e_SS_SM_DATA_RESET_MODE_USER:
            m_shutdownTrigger = epssdmsdtUSER_DATA_RESET;
            break;
          default:
            break;
          }

          if (isImmShutdown) {
            m_GroupLaunchTimer->StopTimer(m_aTimerIDs[eSM_TIMER_GROUP_LAUNCH_WAIT_TIMER]);

            // Do not use m_shutdownTrigger because it is updated on DataReset
            switch (l_tServiceSetIf.data.shutdownRequestMsg.shutdownTrigger) {
            case epssdmsdtGENERIC_ERROR_RESET:
            case epssdmsdtFATAL_ERROR_RESET:
              CALL_AND_LOG_STATUS((*m_pfStopCompleteHandler)(hApp));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
              break;
            default:
              FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__,
                  " Warning: Received a Shutdown command while in "
                  "the '%s' state - returning Shutdown "
                  "Response msg now",
                  GetStr(m_SMCurrentState).c_str());

              CALL_AND_LOG_STATUS(SendShutdownResponseMessage(hApp));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
              break;
            }

          } else {
            InitiateAllGroupsShutdown(hApp);
          }
        }
        break;

      case SS_SM_APPS_STOPPING_AT__CWORD56__REQ:
      case SS_SM_APPS_STOPPING_AT_INTERNAL_REQ:
      case SS_SM_WAITING_FOR_CRITICAL_APPS_AT__CWORD56__REQ:
      case SS_SM_WAITING_FOR_CRITICAL_APPS_AT_INTERNAL_REQ:
        FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__,
            " Warning: Received a Shutdown command while already in "
            "the '%s' state !!!",
            GetStr(m_SMCurrentState).c_str());
        break;
      }  // End switch ( m_SMCurrentState )
      break;
    }  // End case epsumOFF | epsumON
    }  // End switch (m_lastUserMode )
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::OnShutdownModulesRequest( HANDLE hApp )

///////////////////////////////////////////////////////////////////////////////
/// \ingroup SendSystemModeInfoResponse
/// send SystemModeInfo to
/// _CWORD56_ ( via Power Service-> PSMShadow ) as an IPC 'Start Notification'
/// message, informing _CWORD56_ of the startup state of the _CWORD102_.
///
/// \param [in] HANDLE hApp
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::SendSystemModeInfoResponse(HANDLE hApp,
        EPWR_STARTUP_STAGE_TYPE f_startupStage) noexcept {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  SystemModeInfo l_SystemModeInfo;

  memcpy(&l_SystemModeInfo, &m_SystemModeInfo, sizeof(l_SystemModeInfo));

  FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "stage:%d", f_startupStage);
  l_SystemModeInfo.startupStage = f_startupStage;

  l_eStatus = FrameworkunifiedSendMsg(m_hPowerServiceSession, SS_SM_SYSTEM_MODE_INFO_RSPN,
      sizeof(l_SystemModeInfo), (PVOID) & l_SystemModeInfo);

  if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        " Error: SendSystemModeInfoResponse(%s) errored: 0x%X",
        GetStr(l_SystemModeInfo.startupStage).c_str(), l_eStatus);
  } else {
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
        " SendSystemModeInfoResponse(%s) successful",
        GetStr(l_SystemModeInfo.startupStage).c_str());
    // LCOV_EXCL_STOP
  }

  LOG_STATUS(l_eStatus, "FrameworkunifiedSendMsg(SS_SM_SYSTEM_MODE_INFO_RSPN)");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::SendSystemModeInfoResponse( HANDLE hApp )

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnSystemModeInfoRequest
/// Handle System Mode Info request. Send System Mode Info read from NVM
///
/// \param [in] HANDLE hApp
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnSystemModeInfoRequest(HANDLE hApp) noexcept {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  INTERFACEUNIFIEDLOG_RECEIVED_FROM(hApp);  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
  FRAMEWORKUNIFIEDLOG(ZONE_PERFORMANCE, __FUNCTION__, "from %s", FrameworkunifiedGetMsgSrc(hApp));
  m_isRcvModeInfo = TRUE;

  SetCmdHist("SS_SM_SYSTEM_MODE_INFO_REQ", m__CWORD56_CmdHist, m__CWORD56_HistIter, FrameworkunifiedGetMsgSrc(hApp));

  if (m_SystemModeInfo.startupStage == epssusfINVALID) {
    // Transitioning to SYSTEM_SERVICES_STARTED if startupStage == INVALID
    m_SystemModeInfo.startupStage = epssusSYSTEM_SERVICES_STARTED;
  }

  l_eStatus = SendSystemModeInfoResponse(hApp, epssusSYSTEM_SERVICES_STARTED);
  LOG_STATUS(l_eStatus, "SendSystemModeInfoResponse()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  if (TRUE == m_isRstPending) {
    ESMCpuResetReason l_resetReason = m_rstPendingInfo.resetReason;
    std::string l_messageStr = m_rstPendingInfo.messageStr;

    m_isRstPending = FALSE;
    m_rstPendingInfo.resetReason = e_SS_SM_CPU_RESET_REASON_INVALID;
    memset(m_rstPendingInfo.messageStr, 0, sizeof(m_rstPendingInfo.messageStr));

    l_eStatus = PerformCpuReset(hApp, l_resetReason, l_messageStr);
    LOG_STATUS(l_eStatus, "PerformCpuReset()");  // LCOV_EXCL_BR_LINE 6: Because it is executed only once and cannot see all branches
  }

  if (m_SystemModeInfo.startupStage == epssusALL_SERVICES_LAUNCHED) {
    // If startupStage == ALL_SERVICES_LAUNCHED
    // Notify here because Launch of all SVCs is completed first.
    l_eStatus = SendSystemModeInfoResponse(hApp, epssusALL_SERVICES_LAUNCHED);
    LOG_STATUS(l_eStatus, "SendSystemModeInfoResponse()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::OnSystemModeInfoRequest( HANDLE hApp )

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnInitCompReportCallback
/// Handle InitComp report.
///
/// \param [in] HANDLE hApp
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnInitCompReportCallback(HANDLE hApp) noexcept {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  INTERFACEUNIFIEDLOG_RECEIVED_FROM(hApp);  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
  FRAMEWORKUNIFIEDLOG(ZONE_PERFORMANCE, __FUNCTION__, "from %s", FrameworkunifiedGetMsgSrc(hApp));

  SetCmdHist("SS_SM_INITCOMP_REP", m__CWORD56_CmdHist, m__CWORD56_HistIter, FrameworkunifiedGetMsgSrc(hApp));

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::OnInitCompReportCallback( HANDLE hApp )

///////////////////////////////////////////////////////////////////////////////
/// \ingroup have_all_services_start_completed
///  check if state of all services is MODULE_STATE_STARTED
///
/// \param [in]
///
///
/// \return BOOL
/// Success ==> TRUE
/// Failure ==> FALSE
///////////////////////////////////////////////////////////////////////////////
BOOL CSystemManager::have_all_services_start_completed(const SMModuleState f_moduleState) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  BOOL bIsStartCompleted = TRUE;
  SMModuleState l_failedState = MODULE_STATE_INVALID;

  switch (f_moduleState) {
    case MODULE_STATE_STARTED:
      l_failedState = MODULE_STATE_START_FAILED;
      break;
    case MODULE_STATE_STARTED_PRE:
      l_failedState = MODULE_STATE_START_PRE_FAILED;
      break;
    case MODULE_STATE_STOPPED_PRE:
      l_failedState = MODULE_STATE_STOP_PRE_FAILED;
      break;
    case MODULE_STATE_STARTED_BACKGROUND:
      l_failedState = MODULE_STATE_START_BACKGROUND_FAILED;
      break;
    case MODULE_STATE_STOPPED_BACKGROUND:
      l_failedState = MODULE_STATE_STOP_BACKGROUND_FAILED;
      break;
    default:
      bIsStartCompleted = FALSE;
      break;
  }
  if (l_failedState != MODULE_STATE_INVALID) {
    GroupLaunchMapIter l_GroupIterator = m_MapProclaunchGrps.begin();
    for (; l_GroupIterator != m_MapProclaunchGrps.end(); l_GroupIterator++) {
      ModuleLaunchListIter l_ModuleListIterator = l_GroupIterator->second.modules.begin();
      for (; l_ModuleListIterator != l_GroupIterator->second.modules.end(); l_ModuleListIterator++) {
        if ((l_ModuleListIterator->is_start_required == TRUE)
            && ((l_ModuleListIterator->IsModuleState(f_moduleState) == FALSE)
                && (l_ModuleListIterator->IsModuleState(MODULE_STATE_SKIPPED) == FALSE)
                && (l_ModuleListIterator->IsModuleState(l_failedState) == FALSE))) {
          // Allow _SKIPPED/START_FAILED for StartRespMonitor conditions
          bIsStartCompleted = FALSE;
          break;
        }
      }
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return bIsStartCompleted;
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup is_critical_service_stopped
///
///
/// \param [in]
///
///
/// \return BOOL
/// Success ==> TRUE
/// Failure ==> FALSE
///////////////////////////////////////////////////////////////////////////////
BOOL CSystemManager::is_service_shutdown_ready(ModuleLaunchListIter &modIte) {
  if (((modIte->shutdown_critical && modIte->is_start_required)
      || (modIte->name == FRAMEWORKUNIFIED_NS_NPSERVICE))
      && modIte->IsModuleState(MODULE_STATE_STOPPED) == FALSE) {
    // Not shutdown_ready if NPP or shutdown_critical services are not STOPPED.
    return FALSE;
  }
  return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup have_critical_services_stopped
///  check if state of all critical services is MODULE_STATE_STOPPED
///
/// \param [in]
///
///
/// \return BOOL
/// Success ==> TRUE
/// Failure ==> FALSE
///////////////////////////////////////////////////////////////////////////////
BOOL CSystemManager::have_critical_services_stopped() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  uint numStoppingModules = 0;

  GroupLaunchMapIter l_GroupIterator = m_MapProclaunchGrps.begin();
  for (; l_GroupIterator != m_MapProclaunchGrps.end(); l_GroupIterator++) {
    ModuleLaunchListIter l_ModuleListIterator = l_GroupIterator->second.modules.begin();
    for (; l_ModuleListIterator != l_GroupIterator->second.modules.end(); l_ModuleListIterator++) {
      if (l_ModuleListIterator->IsModuleState(MODULE_STATE_STOP_SENT)
          && l_ModuleListIterator->shutdown_critical) {
        numStoppingModules++;
      }
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (numStoppingModules == 0);
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup perform_force_reset
///
///
/// \param [in]
///
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::perform_force_reset(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "+");

  // Since initialization must be performed by the PSMs during FactReset, do not NaviDet them as much as possible.
  if (e_SS_SM_DATA_RESET_MODE_FACTORY == m_DataResetMode) {
    fprintf(stderr,
        "SS_SysManager/%s/NO NAVIDET!! but SendShutdownComp for FactReset\n", __FUNCTION__);
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        "NO NAVIDET!! but SendShutdownComp for FactReset");
    CALL_AND_LOG_STATUS(send_shutdown_complete_response(hApp));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    sleep(60);
  } else if (access("/nv/bs/ss_debug", F_OK) == 0) {
    fprintf(stderr, "SS_SysManager/%s/NO NAVIDET!! but SendShutdownComp\n", __FUNCTION__);
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "NO NAVIDET!! but SendShutdownComp");
    CALL_AND_LOG_STATUS(send_shutdown_complete_response(hApp));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    sleep(60);
  }

  FRAMEWORKUNIFIEDLOG(ZONE_SYSLOG, __FUNCTION__, "Perform Final Umount(0)");
  CALL_AND_LOG_STATUS(PerformFinalUmount(TRUE));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  fprintf(stderr, "SS_SysManager/%s/Perform Final Umount(0)\n", __FUNCTION__);
  fflush(stderr);
  usleep(100 * 1000);

//  SS_ASERT(0 == EL_Gpio_finalize());  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

//  if (0 == PsmNaviResetPower(hApp)) {  // LCOV_EXCL_BR_LINE 200: power_hal if, can not to be error.
//    SS_ASERT_ERRNO(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
//  } else {
//    usleep(300 * 1000);
//    fprintf(stderr, "SS_SysManager/%s/Error: Not Reset !!!\n", __FUNCTION__);
//    l_eStatus = eFrameworkunifiedStatusFail;
//  }

  send_shutdown_complete_response(hApp);  // Try to NAVI_RESET even if ignored

  FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// \ingroup send_shutdown_complete_response
///
///
/// \param [in]
///
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::send_shutdown_complete_response(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  CALL_AND_LOG_STATUS(ResetModulesStateToConnected(hApp));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  SMSetState(hApp, SS_SM_APPS_LAUNCHED_READY_TO_START);
  // m_ActiveGroupId = SS_SM_INITIAL_GROUP;

  // all the modules have stopped, send Shutdown Response to Power Service
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
      " All critical services have stopped, Active Group ID is now %d",
      m_SystemStarter.get_id());
#if 1
    CALL_AND_LOG_STATUS(SendShutdownResponseMessage(hApp));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
#else
  //
  // Normal, production flow is to call SendShutdownResponseMessage(), which
  // sends the Shutdown Response message to the _CWORD56_, causing the _CWORD56_ to
  // remove power from the _CWORD102_ before queued-up FRAMEWORKUNIFIEDLOG messages can be  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
  // processed by SS_LoggerService.
  //
  // Optional development flow is to NOT call SendShutdownResponseMessage(),
  // thus giving the _CWORD102_ time to process its FRAMEWORKUNIFIEDLOG messages. Since this is an  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
  // abnormal, 'bad' condition of NOT sending a Shutdown Response message to
  // the _CWORD56_, be very obvious about this state by displaying both
  // compile-time and run-time warning messages.
  //
#warning NOT calling SendShutdownResponseMessage(hApp) !
  FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__,  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
      " Warning: SendShutdownResponseMessage( hApp ) "
      "#def'f out; ShutdownResponse NOT sent to _CWORD56_");
#endif

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::SendShutdownResponseMessage(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  l_eStatus = FrameworkunifiedSendMsg(m_hPowerServiceSession,  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
      SS_SM_SHUTDOWN_MODULES_CMPL_RSPN, 0, (PVOID) NULL);  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
  LOG_STATUS_REC_HIST_IF_ERRORED(l_eStatus,  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
      "FrameworkunifiedSendMsg(PowerService,SS_SM_SHUTDOWN_MODULES_CMPL_RSPN)");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::SendShutdownResponseMessage( HANDLE hApp )

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnClientStopMonitorTimerExpiry
///
///
/// \param [in]
///
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnClientStopMonitorTimerExpiry(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Received from group %d", m_SystemStarter.get_id());

  SetCmdHist("SM_TIMER_CLIENT_STOP_MONITOR", m_TimerCmdHist, m_TimerHistIter, FrameworkunifiedGetMsgSrc(hApp));  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  if ((m_SMCurrentState == SS_SM_WAITING_FOR_CRITICAL_APPS_AT__CWORD56__REQ)
      || (m_SMCurrentState == SS_SM_WAITING_FOR_CRITICAL_APPS_AT_INTERNAL_REQ)
      || m_SystemStarter.get_id() <= SS_SM_THIRD_GROUP) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: StopCompTimeout:gid:%d", m_SystemStarter.get_id());
    fprintf(stderr, "SS_SysManager/%s/Error: StopCompTimeout:gid:%d\n", __FUNCTION__, m_SystemStarter.get_id());

    for (GroupLaunchMapIter l_GroupIterator = m_MapProclaunchGrps.begin();
        l_GroupIterator != m_MapProclaunchGrps.end();
        l_GroupIterator++) {
      for (ModuleLaunchListIter l_ModuleListIterator = l_GroupIterator->second.modules.begin();
          l_ModuleListIterator != l_GroupIterator->second.modules.end();
          l_ModuleListIterator++) {
        if (l_ModuleListIterator->group_id >= m_SystemStarter.get_id()
            && is_service_shutdown_ready(l_ModuleListIterator) == FALSE) {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: %s", l_ModuleListIterator->name.c_str());
          fprintf(stderr, "SS_SysManager/%s/Error: %s\n", __FUNCTION__, l_ModuleListIterator->name.c_str());  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
        }
      }
    }

    if (!m_SystemStarter.is_begin()) {
      // Continue termination processing to perform BackupManager termination processing.
      m_SystemStarter.decrement_id();
      check_all_groups_have_stopped(hApp);
    } else {
      // DEAD CODE
      fprintf(stderr, "SS_SysManager/%s:SVC stop timeout\n", __FUNCTION__);
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " SVC stop timeout");
      {
        RAM_AccessIf ram_access;
        RAM_SM_INFO_t info;

        SS_ASERT(eFrameworkunifiedStatusOK == ram_access.getRamInfo(&info));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
        info.isErrorReset = TRUE;
        SS_ASERT(eFrameworkunifiedStatusOK == ram_access.setRamInfo(&info));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
      }
      l_eStatus = CallStopCompleteHandler(hApp);
      LOG_STATUS_REC_HIST_IF_ERRORED(l_eStatus, "CallStopCompleteHandler( hApp )");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
    }
  } else {
    // The current m_ActiveGroupId group just timed out. If possible,
    // decrement it, and then see if there are more groups to send
    // stop requests to.
    if (!m_SystemStarter.is_begin()) {
      m_SystemStarter.decrement_id();
    }
    check_all_groups_have_stopped(hApp);
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::OnClientStopMonitorTimerExpiry( HANDLE hApp )

///////////////////////////////////////////////////////////////////////////////
/// \ingroup check_all_groups_have_stopped
/// check that all the groups has stopped
///
/// \param [in]
///
///
/// \return VOID
///////////////////////////////////////////////////////////////////////////////
VOID CSystemManager::check_all_groups_have_stopped(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  UI_32 l_ActiveGroupId = m_SystemStarter.get_id();
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
      " m_SystemStarter.get_id() is %d, 'm_NPPStopSent' is '%s'"
      , m_SystemStarter.get_id(), GetStr(m_NPPStopSent).c_str());

  // send stop to next group in reverse order
//  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " m_NPPStopSent is %s", GetStr(m_NPPStopSent).c_str());  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
  if (!m_NPPStopSent) {
    l_eStatus = stop_all_modules_of_group(hApp);
    if (eFrameworkunifiedStatusOK != l_eStatus) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
          " Error: stop_all_modules_of_group(%d) errored: %d/'%s'",
          l_ActiveGroupId, l_eStatus, GetStr(l_eStatus).c_str());
    }
  } else {  // (m_ActiveGroupId < SS_SM_INITIAL_GROUP)
    BOOL l_bIsDetectTimeout;

    if ((l_bIsDetectTimeout = !have_critical_services_stopped())) {
      // If the Critical service is not terminated even when the final GROUP is completed, DUMP the applicable service and issue NAVI_DET.
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " FinalCriticalCheck");
      fprintf(stderr, "SS_SysManager/%s:FinalCriticalCheck\n", __FUNCTION__);  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)

      for (GroupLaunchMapIter l_GroupIterator = m_MapProclaunchGrps.begin();
          l_GroupIterator != m_MapProclaunchGrps.end();
          l_GroupIterator++) {
        for (ModuleLaunchListIter l_ModuleListIterator = l_GroupIterator->second.modules.begin();
            l_ModuleListIterator != l_GroupIterator->second.modules.end();
            l_ModuleListIterator++) {
          if (is_service_shutdown_ready(l_ModuleListIterator) == FALSE) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: %s",
                l_ModuleListIterator->name.c_str());
            fprintf(stderr, "SS_SysManager/%s/Error: %s\n",  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
                __FUNCTION__,
                l_ModuleListIterator->name.c_str());  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
          }
        }
      }
      fprintf(stderr, "SS_SysManager/%s:critical service no stop\n", __FUNCTION__);
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "critical service no stop");
      {
        RAM_AccessIf ram_access;
        RAM_SM_INFO_t info;

        SS_ASERT(eFrameworkunifiedStatusOK == ram_access.getRamInfo(&info));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
        info.isErrorReset = TRUE;
        SS_ASERT(eFrameworkunifiedStatusOK == ram_access.setRamInfo(&info));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
      }
    }

    CALL_AND_LOG_STATUS(CallStopCompleteHandler(hApp, l_bIsDetectTimeout));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

    // In PastModel002, there was a transition to WAITING_FOR_CRITICAL_APPS in the following processing, but in _CWORD71_,
    // the transition to WAITING_FOR_CRITICAL_APPS was eliminated by revising the termination processing.
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return;
}  // End of VOID CSystemManager::check_all_groups_have_stopped( HANDLE hApp )

///////////////////////////////////////////////////////////////////////////////
/// \ingroup MarkModuleStateStopped
/// Sets Module State to STOP COMPLETE
///
/// \param [in]
///
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnNppStopComplete
/// Npp Stop Complete complete Response\Ack Handlers
///
/// \param [in]
///
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnNppStopComplete(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  INTERFACEUNIFIEDLOG_RECEIVED_FROM(hApp);  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
  FRAMEWORKUNIFIEDLOG(ZONE_PERFORMANCE, __FUNCTION__, "from %s", FrameworkunifiedGetMsgSrc(hApp));

  SetCmdHist("NPS_NPP_STOP_ACK", m_SMCmdHist, m_SMHistIter, FrameworkunifiedGetMsgSrc(hApp));  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  char l_cBuf[100];
  snprintf(l_cBuf, sizeof(l_cBuf), "ProcessModuleStopCompleteResponse(%s)", FRAMEWORKUNIFIED_NS_NPSERVICE);
  l_eStatus = ProcessModuleStopCompleteResponse(hApp, FRAMEWORKUNIFIED_NS_NPSERVICE);
  LOG_STATUS_REC_HIST_IF_ERRORED(l_eStatus, l_cBuf);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus CSystemManager::GetModuleIterator(PCSTR f_moduleName,
        ModuleLaunchListIter &f_moduleIter) noexcept {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;

  ModuleLaunchListIter l_moduleIter;
//  static int l_numTimesEntered = 0 ;
//  l_numTimesEntered++ ;
//  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
//       , " NTE %d: Looking for %s"
//       , l_numTimesEntered
//       , f_moduleName);

  BOOL l_bIsFound = FALSE;
  for (GroupLaunchMapIter l_GroupIter = m_MapProclaunchGrps.begin();
      !l_bIsFound && l_GroupIter != m_MapProclaunchGrps.end();
      l_GroupIter++) {
//    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
//       , " NTE %d: Checking group %d/%s"
//       , l_numTimesEntered
//       , l_GroupIter->second.id
//       , l_GroupIter->second.name.c_str());

    for (l_moduleIter = l_GroupIter->second.modules.begin();
        !l_bIsFound && (l_moduleIter != l_GroupIter->second.modules.end());
        l_moduleIter++) {
//    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
//       , " NTE %d: Checking PCSTR '%s' against std::string '%s': %d, %s"
//       , l_numTimesEntered
//       , f_moduleName
//       , l_moduleIter->name.c_str()
//       , strcmp(l_moduleIter->name.c_str(), f_moduleName)
//       , (0 == strcmp(l_moduleIter->name.c_str(), f_moduleName) ? "True" : "False"));

      l_bIsFound =
          (0 == strcmp(l_moduleIter->name.c_str(), f_moduleName));
      if (l_bIsFound) {
        f_moduleIter = l_moduleIter;
//        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
////            " NTE %d:"
//               " '%s' is in group %d/%s"
////           , l_numTimesEntered
//           , f_moduleIter->name.c_str()
//           , l_GroupIter->second.id
//           , l_GroupIter->second.name.c_str());
        l_eStatus = eFrameworkunifiedStatusOK;
      }
    }
  }

  if (!l_bIsFound) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
//      " NTE %d:"
        " Error: '%s' is not in group map 'm_MapProclaunchGrps'"
//       , l_numTimesEntered
        , f_moduleName);
    if (0 != strcmp(f_moduleName, SERVICE_SYSMANAGER)) {
      SysMgrConfiguration m_launchConfig;
      m_launchConfig.PrintAllInfo(m_MapProclaunchGrps);
    }
    l_eStatus = eFrameworkunifiedStatusDbRecNotFound;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::GetModuleIterator( PCSTR f_moduleName

BOOL CSystemManager::IsGroupStarted(UI_32 f_groupID) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  BOOL l_bIsStarted = TRUE;

  GroupLaunchMapIter l_GroupIter = m_MapProclaunchGrps.find(f_groupID);
  if (l_GroupIter == m_MapProclaunchGrps.end()) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: Group Map Empty; was looking for group %d", f_groupID);
  } else {
    ModuleLaunchListIter l_ModuleListIter = l_GroupIter->second.modules.begin();
    const SMModuleState l_dest_state = InProgressStateToState();  // LCOV_EXCL_BR_LINE 11: Gcov constraints (because exception-handling routes are automatically generated) // NOLINT(whitespace/line_length)
    if (l_dest_state != MODULE_STATE_INVALID) {
      for (; l_ModuleListIter != l_GroupIter->second.modules.end(); l_ModuleListIter++) {
        if (((l_ModuleListIter->IsModuleState(l_dest_state) == FALSE)
            && (l_ModuleListIter->IsModuleState(MODULE_STATE_START_FAILED) == FALSE)
            && (l_ModuleListIter->IsModuleState(MODULE_STATE_START_PRE_FAILED) == FALSE)
            && (l_ModuleListIter->IsModuleState(MODULE_STATE_STOP_PRE_FAILED) == FALSE)
            && (l_ModuleListIter->IsModuleState(MODULE_STATE_START_BACKGROUND_FAILED) == FALSE)
            && (l_ModuleListIter->IsModuleState(MODULE_STATE_STOP_BACKGROUND_FAILED) == FALSE)
            && (l_ModuleListIter->IsModuleState(MODULE_STATE_STOP_FAILED) == FALSE))
            || ((l_ModuleListIter->is_start_required == TRUE)
                // LCOV_EXCL_START 6: Because the value cannot be stored to make the condition true
                && ((l_ModuleListIter->IsModuleState(MODULE_STATE_LAUNCHING) == TRUE)
                    || (l_ModuleListIter->IsModuleState(MODULE_STATE_CONNECTED) == TRUE)
                    || (l_ModuleListIter->IsModuleState(MODULE_STATE_LAUNCHED) == TRUE)))) {
                // LCOV_EXCL_STOP 6: Because the value cannot be stored to make the condition true
          l_bIsStarted = FALSE;
          FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " No: '%s' in group %d/%s is '%s'", l_ModuleListIter->name.c_str(),
                 l_GroupIter->second.id, l_GroupIter->second.name.c_str(),
                 l_ModuleListIter->ModuleStateStr().c_str());
          break;
        }
      }
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_bIsStarted;
}  // End of BOOL CSystemManager::IsGroupStarted(UI_32 f_groupId)

BOOL CSystemManager::IsGroupStopped(UI_32 f_groupID) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  BOOL l_bIsStopped = TRUE;
  ModuleLaunchListIter l_ModuleListIter;

  GroupLaunchMapIter l_GroupIter = m_MapProclaunchGrps.find(f_groupID);
  if (l_GroupIter == m_MapProclaunchGrps.end()) {
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__,
        " Warning: Group Map Empty; was looking for group %d",
        f_groupID);
  } else {
    for (l_ModuleListIter = l_GroupIter->second.modules.begin();
        l_bIsStopped
            && l_ModuleListIter != l_GroupIter->second.modules.end();
        l_ModuleListIter++) {
      if (l_ModuleListIter->IsModuleState(MODULE_STATE_STOP_SENT)) {
        l_bIsStopped = FALSE;
      }
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_bIsStopped;
}  // End of BOOL CSystemManager::IsGroupStopped(UI_32 f_groupId)

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnStopComplete
/// Shutdown complete Response\Ack Handlers
///
/// \param [in]
///
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnModuleStopCompleteResponse(HANDLE hApp) noexcept {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusErrOther;
  INTERFACEUNIFIEDLOG_RECEIVED_FROM(hApp);  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)

  PCSTR l_moduleName = FrameworkunifiedGetMsgSrc(hApp);

  FRAMEWORKUNIFIEDLOG(ZONE_PERFORMANCE, __FUNCTION__, "from %s", l_moduleName);
  SetCmdHist("SS_SM_STOP_COMPL_RSPN", m_SMCmdHist, m_SMHistIter, l_moduleName);

  char l_cBuf[100];
  snprintf(l_cBuf, sizeof(l_cBuf), "ProcessModuleStopCompleteResponse(%s)", l_moduleName);
  l_eStatus = ProcessModuleStopCompleteResponse(hApp, l_moduleName);
  LOG_STATUS_REC_HIST_IF_ERRORED(l_eStatus, l_cBuf);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::OnModuleStopCompleteResponse( HANDLE hApp )

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnGetStartExtInfo
///
/// \param [in]
///
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnGetStartExtInfo(HANDLE hApp) noexcept {
  EFrameworkunifiedStatus l_eStatus;
  T_SS_SM_START_ExtDataStructType l_extInfo = { 0 };

  SS_STATIC_ASERT(sizeof(T_SS_SM_START_ExtDataStructType) == SS_SM_START_EXT_INFO_SIZE);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+ ");

  SetCmdHist("SS_SM_GET_START_EXT_INFO", m_SMCmdHist, m_SMHistIter, FrameworkunifiedGetMsgSrc(hApp));

  l_extInfo.isProgUpdated =
      (m_ProgUpdateState & SS_SM_PROG_UPDATE_STATE_UPDATED) ? TRUE : FALSE;
  l_extInfo.isMapUpdated =
      (m_ProgUpdateState & SS_SM_PROG_UPDATE_STATE_MAP_UPDATED) ? TRUE : FALSE;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  l_extInfo.isMapDiffUpdated =
      (m_ProgUpdateState & SS_SM_PROG_UPDATE_STATE_MAPDIFF_UPDATED) ? TRUE : FALSE;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  ModuleLaunchListIter l_ModuleListIter;

  l_eStatus = GetModuleIterator(FrameworkunifiedGetMsgSrc(hApp), l_ModuleListIter);
  if (eFrameworkunifiedStatusOK != l_eStatus) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: %s not found", FrameworkunifiedGetMsgSrc(hApp));
  } else {
    switch (l_ModuleListIter->relaunch_status) {
    case NotRelaunched:
      l_extInfo.relaunchStatus = e_SS_SM_RELAUNCH_STATUS_NONE;
      break;
    case RelaunchSafe:
      // LCOV_EXCL_START 6: Because the applicable variable cannot be changed from the external API (only evaluated by the initial value)
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_extInfo.relaunchStatus = e_SS_SM_RELAUNCH_STATUS_SAFE;
      break;
    case RelaunchErr:
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_extInfo.relaunchStatus = e_SS_SM_RELAUNCH_STATUS_ERR;
      break;
    default:
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      SS_ASERT(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
      l_extInfo.relaunchStatus = e_SS_SM_RELAUNCH_STATUS_NONE;
      break;
      // LCOV_EXCL_STOP
    }
  }

  // Instead of sending Return Message, this function must call FrameworkunifiedSetSyncResponseData()
  // because this request is sent by FrameworkunifiedInvokeSync()
  l_eStatus = FrameworkunifiedSetSyncResponseData(hApp, &l_extInfo, sizeof(l_extInfo));
  LOG_STATUS_IF_ERRORED(l_eStatus, "FrameworkunifiedSetSyncResponseData()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::OnGetStartExtInfo( HANDLE hApp )

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnGetStopExtInfo
///
/// \param [in]
///
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnGetStopExtInfo(HANDLE hApp) noexcept {
  EFrameworkunifiedStatus l_eStatus;
  T_SS_SM_STOP_ExtDataStructType l_extInfo = { 0 };

  SS_STATIC_ASERT(sizeof(T_SS_SM_STOP_ExtDataStructType) == SS_SM_STOP_EXT_INFO_SIZE);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+ ");

  SetCmdHist("SS_SM_GET_STOP_EXT_INFO", m_SMCmdHist, m_SMHistIter, FrameworkunifiedGetMsgSrc(hApp));

  l_extInfo.isProgUpdated =
      (m_ProgUpdateState & SS_SM_PROG_UPDATE_STATE_UPDATED) ? TRUE : FALSE;

  // Instead of sending Return Message, this function must call FrameworkunifiedSetSyncResponseData()
  // because this request is sent by FrameworkunifiedInvokeSync()
  l_eStatus = FrameworkunifiedSetSyncResponseData(hApp, &l_extInfo, sizeof(l_extInfo));
  LOG_STATUS_IF_ERRORED(l_eStatus, "FrameworkunifiedSetSyncResponseData()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::OnGetStopExtInfo( HANDLE hApp )

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnModuleStopCompleteNotification
///
/// \param [in]
///
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnModuleStopCompleteNotification(HANDLE hApp) {  // LCOV_EXCL_START 6: Because the condition cannot be set
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus l_eStatus;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+ ");
  INTERFACEUNIFIEDLOG_RECEIVED_FROM(hApp);  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
  FRAMEWORKUNIFIEDLOG(ZONE_PERFORMANCE, __FUNCTION__, "from %s", FrameworkunifiedGetMsgSrc(hApp));

  ShutdownComplete l_shutdownNotif;   /// \file   frameworkunified_framework_types.h
  // ReadMsg():                                                      *
  //   Check hApp ptr, msg size, msg reception, read msg if all ok.  *
  //   Report any errors found.                                      *
  //                                                                 *
  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg < ShutdownComplete > (hApp, l_shutdownNotif))) {
    LOG_ERROR("ReadMsg()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  } else {
    PCSTR l_moduleName = l_shutdownNotif.cServiceName;
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
        " '%s' 'Shutdown Complete Status' is 0x%X", l_moduleName,
        l_shutdownNotif.eShutdownCompleteStatus);

    char l_cBuf[100];
    snprintf(l_cBuf, sizeof(l_cBuf),
        "ProcessModuleStopCompleteResponse(%s)", l_moduleName);
    l_eStatus = ProcessModuleStopCompleteResponse(hApp, l_moduleName);
    LOG_STATUS_REC_HIST_IF_ERRORED(l_eStatus, l_cBuf);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::onmodulestopcompletenotification( HANDLE hApp )
// LCOV_EXCL_STOP

EFrameworkunifiedStatus CSystemManager::ProcessModuleStopCompleteResponse(HANDLE hApp, PCSTR f_moduleName) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;

  ModuleLaunchListIter l_ModuleListIter;
  l_eStatus = GetModuleIterator(f_moduleName, l_ModuleListIter);
  if (eFrameworkunifiedStatusOK != l_eStatus) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: Module %s not found in Group Launch Map", f_moduleName);
  } else {
    switch (m_SMCurrentState) {
    case SS_SM_READY_TO_LAUNCH_APP:
    case SS_SM_APPS_LAUNCH_IN_PROGRESS:
    case SS_SM_APPS_LAUNCHED_READY_TO_START:
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
          " Error: Received from client while in "
          "the '%s' state !!!",
          GetStr(m_SMCurrentState).c_str());
      l_eStatus = eFrameworkunifiedStatusErrOther;
      break;

    case SS_SM_APPS_STOPPING_AT__CWORD56__REQ:
    case SS_SM_APPS_STOPPING_AT_INTERNAL_REQ:
    case SS_SM_WAITING_FOR_CRITICAL_APPS_AT__CWORD56__REQ:
    case SS_SM_WAITING_FOR_CRITICAL_APPS_AT_INTERNAL_REQ:
      if (m_SystemStarter.get_id() == SS_SM_THIRD_GROUP) {
        // Execute shutdown_critical services termination synchronization prior to CoreModule(backupManager termination processing
        l_ModuleListIter->SetModuleState(MODULE_STATE_STOPPED);
        if (have_critical_services_stopped()) {  // Transition after all critical services have ended
          ProcessGroupOnModuleStopResponse(hApp, m_SystemStarter.get_id());
        }
      } else {
        l_eStatus = ModuleCompleteResponse(hApp, l_ModuleListIter,
                MODULE_STATE_STOPPED,
                (SysMgrCbType3<CSystemManager,
                    &CSystemManager::IsGroupStopped>),
                (SysMgrCbType2<CSystemManager,
                    &CSystemManager::ProcessGroupOnModuleStopResponse>),
                "stop");
      }
      break;

    case SS_SM_APPS_START_COMPLETE:
    case SS_SM_APPS_START_IN_PROGRESS:
    case SS_SM_APPS_PRE_RUN_COMPLETE:
    case SS_SM_APPS_PRE_START_IN_PROGRESS:
    case SS_SM_APPS_PRE_STOP_IN_PROGRESS:
    case SS_SM_APPS_BACKGROUND_RUN_COMPLETE:
    case SS_SM_APPS_BACKGROUND_START_IN_PROGRESS:
    case SS_SM_APPS_BACKGROUND_STOP_IN_PROGRESS:
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
          " Error: Received from client while in "
          "the '%s' state !!!",
          GetStr(m_SMCurrentState).c_str());
      l_eStatus = eFrameworkunifiedStatusErrOther;
      break;
    }  // End switch ( m_SMCurrentState )
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::ProcessModuleStopCompleteResponse(

EFrameworkunifiedStatus CSystemManager::ProcessGroupOnModuleStopResponse(HANDLE hApp, UI_32 f_groupID) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  UI_32 l_ActiveGroupId = m_SystemStarter.get_id();

  m_GroupLaunchTimer->StopTimer(m_aTimerIDs[eSM_TIMER_CLIENT_STOP_MONITOR]);

  if (f_groupID != l_ActiveGroupId) {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
        " Group %d stop complete, but m_SystemStarter.get_id() is %d",
        f_groupID
        // , m_ActiveGroupId);  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
        , l_ActiveGroupId);
  } else {
    if (!m_SystemStarter.is_begin()) {
      m_SystemStarter.decrement_id();
    }
    check_all_groups_have_stopped(hApp);
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::ProcessGroupOnModuleStopResponse( HANDLE hApp

///////////////////////////////////////////////////////////////////////////////
/// \ingroup stop_all_modules_of_group
/// Send SS_SM_STOP to all modules of the given group
///
/// \param [in]
///
///
/// \return VOID
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::stop_all_modules_of_group(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  ModuleLaunchList::reverse_iterator l_revIter;

  UI_32 l_uiMaxGroupWaitTime_sec = 0;
  UI_32 l_uiMaxGroupWaitTime_msec = 0;
  UI_32 l_uiModuleWaitTime = 0;

  // GroupLaunchMapIter l_GroupIterator = m_MapProclaunchGrps.find(m_ActiveGroupId);
  GroupLaunchMapIter l_GroupIterator = m_MapProclaunchGrps.find(m_SystemStarter.get_id());
  if (l_GroupIterator == m_MapProclaunchGrps.end()) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        " Error: Group %d not found; 'm_MapProclaunchGrps' is empty"
        //, m_ActiveGroupId);  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
        , m_SystemStarter.get_id());
    l_eStatus = eFrameworkunifiedStatusDbRecNotFound;
  } else {
    int l_numModulesMessaged = 0;
    FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "StopReq grp:%d", m_SystemStarter.get_id());

    T_SS_SM_STOP_DataStructType l_SM_STOP_Struct;
    l_SM_STOP_Struct.shutdownTrigger = m_shutdownTrigger;
    l_SM_STOP_Struct.lastUserMode = m_lastUserMode;

    for (l_revIter = l_GroupIterator->second.modules.rbegin();
        l_revIter != l_GroupIterator->second.modules.rend();
        l_revIter++) {
      // LCOV_EXCL_BR_START 11: Excluded due to gcov constraints (others)
      if ((l_revIter->IsModuleState(MODULE_STATE_START_SENT) == TRUE)
          || (l_revIter->IsModuleState(MODULE_STATE_STARTED) == TRUE)
          || (l_revIter->IsModuleState(MODULE_STATE_START_PRE_SENT) == TRUE)
          || (l_revIter->IsModuleState(MODULE_STATE_STARTED_PRE) == TRUE)
          || (l_revIter->IsModuleState(MODULE_STATE_STOP_PRE_SENT) == TRUE)
          || (l_revIter->IsModuleState(MODULE_STATE_STOPPED_PRE) == TRUE)
          || (l_revIter->IsModuleState(MODULE_STATE_START_BACKGROUND_SENT) == TRUE)
          || (l_revIter->IsModuleState(MODULE_STATE_STARTED_BACKGROUND) == TRUE)
          || (l_revIter->IsModuleState(MODULE_STATE_STOP_BACKGROUND_SENT) == TRUE)
          || (l_revIter->IsModuleState(MODULE_STATE_STOPPED_BACKGROUND) == TRUE)) {
        // LCOV_EXCL_BR_STOP 11: Excluded due to gcov constraints (others)
        if (FRAMEWORKUNIFIED_NS_NPSERVICE == l_revIter->name) {
          if (epssdmsdtFACTORY_DATA_RESET == m_shutdownTrigger) {
            CALL_AND_LOG_STATUS(FrameworkunifiedSendStopToNSNPP(hApp, eFrameworkunifiedDataResetShutdown, eFrameworkunifiedUserData | eFrameworkunifiedFactoryData));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
          } else if (epssdmsdtUSER_DATA_RESET == m_shutdownTrigger) {
            CALL_AND_LOG_STATUS(FrameworkunifiedSendStopToNSNPP(hApp, eFrameworkunifiedDataResetShutdown, eFrameworkunifiedUserData));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
          } else if (epssdmsdtFAST_SLEEP_MODE == m_shutdownTrigger) {
            CALL_AND_LOG_STATUS(FrameworkunifiedSendStopToNSNPP(hApp, eFrameworkunifiedQuickShutdown));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
          } else {
            CALL_AND_LOG_STATUS(FrameworkunifiedSendStopToNSNPP(hApp, eFrameworkunifiedNormalShutdown));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
          }

          if (eFrameworkunifiedStatusOK != l_eStatus) {
            l_revIter->SetModuleState(MODULE_STATE_STOP_FAILED);
          } else {
            l_revIter->SetModuleState(MODULE_STATE_STOP_SENT);
            l_numModulesMessaged++;
          }

          m_NPPStopSent = TRUE;
          FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " m_NPPStopSent is %s", GetStr(m_NPPStopSent).c_str());  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
        // End else if ( FRAMEWORKUNIFIED_NS_NPSERVICE == l_ModuleListIter->name )
        } else {
          l_eStatus = l_revIter->SendMsgAndUpdateState(&l_SM_STOP_Struct);
          LOG_STATUS_REC_HIST_IF_ERRORED(l_eStatus, "l_revIter->SendMsgAndUpdateState(&l_SM_STOP_Struct)");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
          if (eFrameworkunifiedStatusOK == l_eStatus) {
            if (epssdmsdtFAST_SLEEP_MODE == m_shutdownTrigger) {
              l_uiModuleWaitTime = l_revIter->fast_shutdown_wait_time;

            } else {
              l_uiModuleWaitTime = l_revIter->shutdown_wait_time;
            }
            if (l_uiModuleWaitTime > l_uiMaxGroupWaitTime_sec) {
              l_uiMaxGroupWaitTime_sec = l_uiModuleWaitTime;
            }
            l_numModulesMessaged++;
          }
        }  // End if MODULE_STATE_STARTED || MODULE_STATE_START_SENT
      } else if (l_revIter->IsModuleState(MODULE_STATE_INVALID)
          && FRAMEWORKUNIFIED_NS_NPSERVICE == l_revIter->name) {
        m_NPPStopSent = TRUE;
        FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "StopNotSent:%s is INVALID", FRAMEWORKUNIFIED_NS_NPSERVICE);
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "StopNotSent:%s is %s",
            l_revIter->name.c_str(),
            l_revIter->ModuleStateStr().c_str());
      }
    }

    if (0 == l_numModulesMessaged) {
      LogGroupModulesState(m_SystemStarter.get_id()  // m_ActiveGroupId  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
          , " Warning: NO modules were sent SS_SM_STOP");

      FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "StopComp grp:%d", l_GroupIterator->second.id);

      if (m_SystemStarter.get_id() > SS_SM_INITIAL_GROUP) {
        m_SystemStarter.decrement_id();
        FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, " m_ActiveGroupId is now %d", m_SystemStarter.get_id());

        check_all_groups_have_stopped(hApp);
      }
    } else {
      if (epssdmsdtFAST_SLEEP_MODE == m_shutdownTrigger) {
        // fast shutdown wait time always comes in milli seconds
        UI_32 l_uiTotalWaitTime = l_uiMaxGroupWaitTime_sec;
        l_uiMaxGroupWaitTime_sec = l_uiTotalWaitTime / 1000;
        l_uiMaxGroupWaitTime_msec = l_uiTotalWaitTime % 1000;
      } else {
        // Other types of shutdown except fast shutdown always comes in seconds.
        l_uiMaxGroupWaitTime_msec = 0;
      }

      FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "StopTimer grp:%d time:%d.%d",
          m_SystemStarter.get_id(), l_uiMaxGroupWaitTime_sec,
          l_uiMaxGroupWaitTime_msec);
      m_GroupLaunchTimer->StartTimerMulti(
          m_aTimerIDs[eSM_TIMER_CLIENT_STOP_MONITOR],
          l_uiMaxGroupWaitTime_sec, l_uiMaxGroupWaitTime_msec, 0, 0,
          m_SystemStarter.get_id());
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of VOID CSystemManager::stop_all_modules_of_group(HANDLE hApp)

EFrameworkunifiedStatus CSystemManager::ValidateUserModeMessage(HANDLE hApp, EPWR_USER_MODE_TYPE &l_eUserModeState) {  // LCOV_EXCL_START 6: Because the condition cannot be set // NOLINT(whitespace/line_length)
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  Pwr_ServiceSetInterface tServiceSetIf;
  EPWR_USER_MODE_TYPE l_my_eUserModeState;

  // ReadMsg():                                                          *
  //   Check hApp ptr, msg size, msg reception, read msg if all ok.      *
  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg < Pwr_ServiceSetInterface > (hApp, tServiceSetIf))) {
    LOG_ERROR("ReadMsg()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  } else {
    l_my_eUserModeState = tServiceSetIf.data.user_mode.mode;
    switch (l_my_eUserModeState) {
    case epsumINVALID:
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: l_eUserModeState == epsumINVALID");
      l_eStatus = eFrameworkunifiedStatusInvldParam;
      break;

    case epsumOFF:
    case epsumON:
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Validated '%s'",
          l_my_eUserModeState == epsumON ? "epsumON" : "epsumOFF");
      l_eUserModeState = l_my_eUserModeState;
      l_eStatus = eFrameworkunifiedStatusOK;
      break;

      // default:  Don't code a 'default' here - let the compiler
      // issue a warning ( set via -Wall or -Wswitch ) when the set of
      // enumerations changes - then the maintainer will
      // automagically know to update this switch statement.
    }  // End switch
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::ValidateUserModeMessage( HANDLE hApp, EPWR_USER_MODE_TYPE &l_eUserModeState )
// LCOV_EXCL_STOP

//*****************************************************************************
// Boot Mode Protocol functions                                               *
//                                                                            *
EFrameworkunifiedStatus CSystemManager::OnSetBootModeRequest(HANDLE hApp) noexcept {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  INTERFACEUNIFIEDLOG_RECEIVED_FROM(hApp);  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
  FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "from %s", FrameworkunifiedGetMsgSrc(hApp));

  SetCmdHist("SS_SM_BOOT_MODE_SET_REQ", m_SMCmdHist, m_SMHistIter, FrameworkunifiedGetMsgSrc(hApp));

  ESMBootModeInfo l_eBootModeState = e_SS_SM_BOOT_MODE_INVALID;
  l_eStatus = ValidateBootModeMessage(hApp, l_eBootModeState);

  if (eFrameworkunifiedStatusOK != l_eStatus) {
    LOG_ERROR_REC_HIST(l_eStatus, "ValidateBootModeMessage(&l_eBootModeState)");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
        " Per %s's request, changing 'CSystemManager::m_BootMode' from '0x%x' to '0x%x'",
        FrameworkunifiedGetMsgSrc(hApp), m_SystemModeInfo.systemMode,
        l_eBootModeState);

    // Got valid boot mode flag
    m_BootMode = l_eBootModeState;

    if (m_UsingVMPlayer) {  // LCOV_EXCL_BR_LINE 6: For setting the initialization conditions at SystemManager startup
      // LCOV_EXCL_START 6: For setting the initialization conditions at SystemManager startup
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, " Warning: 'm_UsingVMPlayer' is True");
      l_eStatus = eFrameworkunifiedStatusOK;
      // LCOV_EXCL_STOP
    } else {
      // Initialize ROM access object
      ROM_AccessIf rom_access;
      EBOOT_MODE boot_mode = PROGRAMMING_MODE;
      rom_access.Initialize();

      if (m_BootMode == e_SS_SM_BOOT_MODE_APPLICATION) {
        boot_mode = APPLICATION_MODE;
      }

      FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "boot_mode:%s", (boot_mode == APPLICATION_MODE) ? "APL" : "PROG");

      // Try setting boot mode. If it's failed revert back to Invalid mode
      if (eFrameworkunifiedStatusOK == (l_eStatus = rom_access.SetBootMode(boot_mode))) {
        l_eStatus = PublishBootModeNotification(hApp);
      } else {
        l_eBootModeState = e_SS_SM_BOOT_MODE_INVALID;
      }
    }
  }
  //
  // Return current status to the client, good or bad
  //  ( ReturnBootModeAndStatus() will report errors as needed )
  //
  l_eStatus = ReturnBootModeAndStatus(hApp, SS_SM_BOOT_MODE_SET_RESP,
      "SS_SM_BOOT_MODE_SET_RESP", m_BootMode, l_eStatus);
  LOG_STATUS_IF_ERRORED(l_eStatus, "ReturnBootModeAndStatus(SS_SM_BOOT_MODE_SET_RESP");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::OnSetBootModeRequest( HANDLE hApp )

//*****************************************************************************
// Wakeup Order Protocol functions                                            *
//                                                                            *
EFrameworkunifiedStatus CSystemManager::OnSetWakeupOrderRequest(HANDLE hApp) noexcept {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  TSystemManagerWakeupOrderInfo tWakeupOrderInfo;

  INTERFACEUNIFIEDLOG_RECEIVED_FROM(hApp);  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
  FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "from %s", FrameworkunifiedGetMsgSrc(hApp));

  SetCmdHist("SS_SM_WAKEUP_ORDER_SET_REQ", m_SMCmdHist, m_SMHistIter, FrameworkunifiedGetMsgSrc(hApp));

  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg < TSystemManagerWakeupOrderInfo
      > (hApp, tWakeupOrderInfo))) {
    LOG_ERROR("ReadMsg()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  } else {
    RAM_AccessIf ram_access;
    RAM_SM_INFO_t info;

    if (eFrameworkunifiedStatusOK != (l_eStatus = ram_access.getRamInfo(&info))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             " Error: getRamInfo(%d)", l_eStatus);
    } else {
      snprintf(info.order_name, sizeof(info.order_name), "%s",
               tWakeupOrderInfo.order_name);
      if (eFrameworkunifiedStatusOK != (l_eStatus = ram_access.setRamInfo(&info))) {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                 " Error: setRamInfo(%d)", l_eStatus);
      }
    }
  }
  l_eStatus = FrameworkunifiedSetSyncResponseData(hApp, &l_eStatus, sizeof(l_eStatus));
  LOG_STATUS_IF_ERRORED(l_eStatus, "FrameworkunifiedSetSyncResponseData()");  // LCOV_EXCL_BR_LINE 6:Error Pattern can't pass here

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::OnSetWakeupOrderRequest( HANDLE hApp )

//*****************************************************************************
// Next Wakeup Type Set Protocol functions                                    *
//                                                                            *
EFrameworkunifiedStatus CSystemManager::OnSetNextWakeupTypeRequest(HANDLE hApp) noexcept {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  ESMNextWakeupType l_wakeupType;

  INTERFACEUNIFIEDLOG_RECEIVED_FROM(hApp);  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
  FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "from %s", FrameworkunifiedGetMsgSrc(hApp));
  SetCmdHist("SS_SM_NEXT_WAKEUP_TYPE_SET_REQ", m_SMCmdHist, m_SMHistIter, FrameworkunifiedGetMsgSrc(hApp));

  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg < ESMNextWakeupType > (hApp, l_wakeupType))) {
    LOG_ERROR("ReadMsg()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  } else {
    switch (l_wakeupType) {
    case e_SS_SM_NEXT_WAKEUP_TYPE_NONE:
    case e_SS_SM_NEXT_WAKEUP_TYPE_COLD:
    case e_SS_SM_NEXT_WAKEUP_TYPE_HOT:
      {
        ROM_AccessIf rom_access;
        rom_access.Initialize();
        if (eFrameworkunifiedStatusOK != (l_eStatus = SetNextWakeupType(rom_access, l_wakeupType))) {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: SetNextWakeupType(%d)", l_eStatus);
        } else {
          FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "SetNextWakeupType(%d)", l_wakeupType);
        }
      }
      break;
    default:
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "unknown type(%d)", l_wakeupType);
      l_eStatus = eFrameworkunifiedStatusInvldParam;
    }
  }
  l_eStatus = FrameworkunifiedSetSyncResponseData(hApp, &l_eStatus, sizeof(l_eStatus));
  LOG_STATUS_IF_ERRORED(l_eStatus, "FrameworkunifiedSetSyncResponseData()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::OnSetWakeupOrderRequest( HANDLE hApp )

EFrameworkunifiedStatus CSystemManager::PublishBootModeNotification(HANDLE hApp) noexcept {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  TSS_BootModeListStruct l_bootModes;
  l_bootModes.hostBootMode = m_BootMode;
  l_bootModes._CWORD56_BootMode = m_startUpConfirmationMsg._CWORD56_BootMode;

  char l_cBuf[500] = { 0 };
  snprintf(l_cBuf, sizeof(l_cBuf), "FrameworkunifiedNPPublishNotification(%s, %s, %s)",
      NTFY_SSHUBootModes, GetStr(m_BootMode).c_str(),
      GetStr(m_startUpConfirmationMsg._CWORD56_BootMode).c_str());

  l_eStatus = FrameworkunifiedNPPublishNotification(hApp, NTFY_SSHUBootModes, &l_bootModes, sizeof(l_bootModes));

  REC_HIST_IF_SUCCESSFUL(NTFY_SSHUBootModes, m_PubCmdHist, m_PubHistIter, "", l_eStatus);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)

  LOG_STATUS_REC_HIST_IF_ERRORED(l_eStatus, l_cBuf);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::PublishBootModeNotification(ESMBootModeInfo f_BootMode)

EFrameworkunifiedStatus CSystemManager::ReturnBootModeAndStatus(HANDLE hApp,
        SS_SystemManagerProtocol protocolID, PCSTR p_sIDName,
        ESMBootModeInfo f_BootMode, EFrameworkunifiedStatus eFrameworkunifiedStatus) noexcept {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
    EFrameworkunifiedStatus l_eStatus;
    TSystemManagerBootModeRequestResponse l_response;
    PCSTR p_sBootModeName = (
            f_BootMode == e_SS_SM_BOOT_MODE_APPLICATION ?
            "BOOT_MODE_APPLICATION" : "BOOT_MODE_PROGRAMMING");  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

    (void) p_sBootModeName;
    l_response.hostBootMode = f_BootMode;
    l_response.eFrameworkunifiedStatus = eFrameworkunifiedStatus;

    ModuleLaunchListIter l_ModuleListIter;
    PCSTR l_moduleName = FrameworkunifiedGetMsgSrc(hApp);
    if (eFrameworkunifiedStatusOK != (l_eStatus = GetModuleIterator(l_moduleName, l_ModuleListIter))) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: Module %s not found in Group Launch Map", l_moduleName);
    } else if (l_ModuleListIter->hsession != NULL) {
        if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedSendMsg(l_ModuleListIter->hsession, protocolID, sizeof(TSystemManagerBootModeRequestResponse), (PVOID) & l_response))) {  // LCOV_EXCL_BR_LINE 4:NSFW error case  //NOLINT (whitespace/line_length)
            // LCOV_EXCL_START 4: nsfw error
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                    " Error: FrameworkunifiedSendMsg(%s, %s) errored: %d/'%s'", p_sIDName,
                    p_sBootModeName, l_eStatus, GetStr(l_eStatus).c_str());
            // LCOV_EXCL_STOP
        }
    } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                " NULL == hsession; %s not connected yet",
                l_ModuleListIter->name.c_str());
    }

    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
    return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::ReturnBootModeAndStatus()

EFrameworkunifiedStatus CSystemManager::ValidateBootModeMessage(HANDLE hApp,
        ESMBootModeInfo &l_eBootModeState) noexcept {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  TSystemManagerBootModeRequestResponse tBootModeInfoStruct;
  ESMBootModeInfo l_my_eBootModeState;

  // ReadMsg():                                                        *
  //     Check hApp ptr, msg size, msg reception, read msg if all ok.  *
  //     Report any errors found.                                      *
  //                                                                   *
  if (eFrameworkunifiedStatusOK
      != (l_eStatus = ReadMsg < TSystemManagerBootModeRequestResponse
              > (hApp, tBootModeInfoStruct))) {
    LOG_ERROR("ReadMsg()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
// BUGBUG: Temporarily disable authorization for reproservice join
#if 0
  } else if ( eFrameworkunifiedStatusOK !=
            (l_eStatus = ValidateRequester(
                            hApp
                            , ListOfServicesAuthorizedToSetBootMode,
                            "change Boot Mode"))) {
      // ValidateRequester() will report via FRAMEWORKUNIFIEDLOG any  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
      // errors it detects, as well as reporting the name of an
      // unauthorized requester
    }
#endif
  } else {
    l_my_eBootModeState = tBootModeInfoStruct.hostBootMode;
    switch (l_my_eBootModeState) {  // LCOV_EXCL_BR_LINE 6: Out-of-range values for the enum are not feasible
      case e_SS_SM_BOOT_MODE_INVALID:
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
            " Error: l_eBootModeState == e_SS_SM_BOOT_MODE_INVALID");
        l_eStatus = eFrameworkunifiedStatusInvldParam;
        break;

      case e_SS_SM_BOOT_MODE_APPLICATION:
      case e_SS_SM_BOOT_MODE_PROGRAMMING:
        l_eBootModeState = l_my_eBootModeState;
        break;

      // default:  Don't code a 'default' here - let the compiler
      // issue a warning ( set via -Wall or -Wswitch ) when the set of
      // enumerations changes - then the maintainer will
      // automagically know to update this switch statement.
    }  // End switch
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::ValidateBootModeMessage()

//                                                                            *
// End of Boot Mode Protocol functions                                        *
//*****************************************************************************
EFrameworkunifiedStatus CSystemManager::OnRebootNotification(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "+");  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  CALL_AND_LOG_STATUS(PerformForceUmountRamd());  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  CALL_AND_LOG_STATUS(PerformFinalUmount(FALSE));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  // Wait for reset from SYS after completion of sync
  fprintf(stderr, "SS_SysManager/%s/wait reset for RebootNotification\n", __FUNCTION__);
  sleep(10);
  fprintf(stderr, "SS_SysManager/%s/Not reset 10sec after RebootNotification\n", __FUNCTION__);

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::OnRebootNotification()

//*****************************************************************************
// Data Reset Mode Protocol functions                                         *
//                                                                            *
EFrameworkunifiedStatus CSystemManager::OnSetDataResetModeRequest(HANDLE hApp) noexcept {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  ESMDataResetModeInfo l_dataResetMode;
  INTERFACEUNIFIEDLOG_RECEIVED_FROM(hApp);  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
  FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "from %s", FrameworkunifiedGetMsgSrc(hApp));

  SetCmdHist("SS_SM_DATA_RESET_MODE_SET_REQ", m_SMCmdHist, m_SMHistIter, FrameworkunifiedGetMsgSrc(hApp));

  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg < ESMDataResetModeInfo > (hApp, l_dataResetMode))) {
    LOG_ERROR("ReadMsg()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  } else {
    // Set Data Reset Mode to ROM access object
    ROM_AccessIf rom_access;
    rom_access.Initialize();

    if (eFrameworkunifiedStatusOK != (l_eStatus = SetDataResetMode(rom_access, l_dataResetMode))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: SetDataResetMode(%d)", l_eStatus);
  } else {
      m_DataResetMode = l_dataResetMode;
    }
  }
  //
  // Instead of sending Return Message, this function must call FrameworkunifiedSetSyncResponseData()
  // because this request is sent by FrameworkunifiedInvokeSync()
  //
  l_eStatus = FrameworkunifiedSetSyncResponseData(hApp, &l_eStatus, sizeof(l_eStatus));
  LOG_STATUS_IF_ERRORED(l_eStatus, "FrameworkunifiedSetSyncResponseData()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::OnSetDataResetModeRequest( HANDLE hApp )

EFrameworkunifiedStatus CSystemManager::OnSetProgUpdateStateRequest(HANDLE hApp) noexcept {
  EFrameworkunifiedStatus l_eStatus;
  SMProgUpdateState l_progUpdateState;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "from %s", FrameworkunifiedGetMsgSrc(hApp));

  SetCmdHist("SS_SM_PROG_UPDATE_STATE_SET_REQ", m_SMCmdHist, m_SMHistIter, FrameworkunifiedGetMsgSrc(hApp));

  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg < SMProgUpdateState > (hApp, l_progUpdateState))) {  // LCOV_EXCL_BR_LINE 4: nsfw error
    // LCOV_EXCL_START 4: nsfw error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("ReadMsg()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    // LCOV_EXCL_STOP
  } else {
    // Set Program Update Status to ROM access object
    ROM_AccessIf rom_access;
    rom_access.Initialize();

    m_ProgUpdateState |= l_progUpdateState;
    if (SS_SM_PROG_UPDATE_STATE_UPDATED & l_progUpdateState) {
      SetDramStatusInfo(SYSUP_DRAM_DISCARD);
    }
    if (eFrameworkunifiedStatusOK != (l_eStatus = SetProgUpdateState(rom_access, m_ProgUpdateState))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: SetProgUpdateState(%d)", l_eStatus);
    }
  }
  // Instead of sending Return Message, this function must call FrameworkunifiedSetSyncResponseData()
  // because this request is sent by FrameworkunifiedInvokeSync()
  l_eStatus = FrameworkunifiedSetSyncResponseData(hApp, &l_eStatus, sizeof(l_eStatus));
  LOG_STATUS_IF_ERRORED(l_eStatus, "FrameworkunifiedSetSyncResponseData()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::OnSetProgUpdateStateRequest( HANDLE hApp )

EFrameworkunifiedStatus CSystemManager::CallStopCompleteHandler(HANDLE hApp, BOOL bIsDetectTimeout /* = FALSE */) {
  FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  m_isImmediateReset = FALSE;

  {
    RAM_AccessIf ram_access;
    RAM_SM_INFO_t info;

    if (eFrameworkunifiedStatusOK != (l_eStatus = ram_access.getRamInfo(&info))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: ram_access.getRamInfo(%d)", l_eStatus);
    } else {
      info.isImmediateReset = m_isImmediateReset;
      if (eFrameworkunifiedStatusOK != (l_eStatus = ram_access.setRamInfo(&info))) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: ram_access.setRamInfo(%d)", l_eStatus);
      }
    }
  }

  if (bIsDetectTimeout) {
    fprintf(stderr, "SS_SysManager/%s/State StopComplete(timeout)\n", __FUNCTION__);
  } else {
    fprintf(stderr, "SS_SysManager/%s/State StopComplete\n", __FUNCTION__);
  }

  CALL_AND_LOG_STATUS(PerformUmountRamd());  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  CALL_AND_LOG_STATUS(PerformUmountNand());  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  l_eStatus = SecureChipOff();
  if (eFrameworkunifiedStatusOK != l_eStatus) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: SecureChipOff Failed");
  }

  l_eStatus = (*m_pfStopCompleteHandler)(hApp);

  FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::CallStopCompleteHandler( HANDLE hApp )


//*****************************************************************************
// Soft Reset Protocol functions                                              *
//                                                                            *
static char *get_line(const char *filepath) {
  char line[LINE_MAX];
  FILE *fp;

  if ((fp = fopen(filepath, "rb")) == NULL)
    return NULL;
  if (fgets(line, LINE_MAX, fp) == NULL) {  // LCOV_EXCL_BR_LINE 5:fgets error
    // LCOV_EXCL_START 5: fgets error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    fclose(fp);
    return NULL;
    // LCOV_EXCL_STOP
  }
  fclose(fp);

  return strdup(line);
}

#define PATH_ROOTFS_SIDEA "/dev/mmcblk0p4"
#define PATH_ROOTFS_SIDEB "/dev/mmcblk0p5"

static const char *get_path_rootfs(void) {
  char *side;
  const char *devpath;

  if ((side = get_line("/proc/device-tree/bootinfo/nand_rootfs_part")) == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "get nand_rootfs_partfailed.");
    return PATH_ROOTFS_SIDEA;
  }

  if (!strcmp(side, "SideA")) {
    devpath = PATH_ROOTFS_SIDEA;
  } else if (!strcmp(side, "SideB")) {
    devpath = PATH_ROOTFS_SIDEB;
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Invalid side, fall back to SideA");
    devpath = PATH_ROOTFS_SIDEA;
  }
  free(side);

  return devpath;
}

#define READ_SIZE       (4096)
#define OUTPUT_MAX      (1024)

static int output_count = 0;
static int check_pramfs_body(int emmcfd, off_t offset, size_t read_count) {
  int rtn = -1;
  size_t i;
  char *prDat, *emDat;

  if (output_count >= OUTPUT_MAX) {
    return rtn;
  }

  /* Pramfs MMAP */
//  prDat = reinterpret_cast<char *>(EL_mem_exram_mmap(EL_MEM_TYPE_REGION_PRAMFS, offset, read_count, PROT_READ,
//                                    MAP_SHARED, EL_MEM_CACHE_VALID));
  if (prDat == MAP_FAILED) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Map (Pramfs) failed offset=0x%lx", offset);
    goto FAIL_MAP;
  }

  /* EMMC MMAP */
  emDat = reinterpret_cast<char *>(mmap(NULL, read_count, PROT_READ, MAP_SHARED, emmcfd, offset));
  if (emDat == MAP_FAILED) {  // LCOV_EXCL_BR_LINE 5: mmap error
    // LCOV_EXCL_START 5: nsfw error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Map (EMMC) failed offset=0x%lx", offset);  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
    goto FAIL_MAP2;
    // LCOV_EXCL_STOP
  }

  rtn = memcmp(prDat, emDat, read_count);
  if (rtn) {
    for (i = 0; i < read_count; i++) {
      if (*(prDat+i) != *(emDat+i)) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "%08lx : %02x : %02x", i + offset, *(prDat+i), *(emDat+i));
        output_count++;
      }
    }
  }
  munmap(emDat, read_count);
FAIL_MAP2:
//  EL_mem_exram_munmap(prDat, read_count);
FAIL_MAP:

  return rtn;
}

static void checker_pramfs() {
  int i, r_cnt, emmcfd;
  size_t pramfs_size;
  off_t offset = 0;
  const char *path = get_path_rootfs();

  FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "Check Start %s", path);

  emmcfd = open(path, O_RDONLY|O_CLOEXEC);
  if (emmcfd < 0) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "open fail errno=%d", errno);
    goto FAIL_OPEN;
  }

//  pramfs_size = EL_mem_getLength(EL_MEM_ID_PRAMFS_ALL);

  r_cnt = static_cast<int>(pramfs_size/READ_SIZE);
  for (i=0 ; i < r_cnt ; i++) {
    check_pramfs_body(emmcfd, offset, READ_SIZE);
    if (output_count >= OUTPUT_MAX) {
      goto LOG_MAX;
    }
    offset += READ_SIZE;
  }
  if (pramfs_size%READ_SIZE) {
    check_pramfs_body(emmcfd, offset, pramfs_size%READ_SIZE);
  }

LOG_MAX:
  if (output_count >= OUTPUT_MAX) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "NG part num over %d", OUTPUT_MAX);
  }
  close(emmcfd);
FAIL_OPEN:
  FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "Check End");
  return;
}

EFrameworkunifiedStatus CSystemManager::PerformCpuReset(HANDLE hApp,
        ESMCpuResetReason f_eSmCpuResetReason, std::string f_messageStr) noexcept {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  if (FALSE == m_isRcvModeInfo) {
    // If the condition for implementing the CpuReset request is not satisfied, keep the request.
    FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "Cpu Reset Pending");
    m_isRstPending = TRUE;
    m_rstPendingInfo.resetReason = f_eSmCpuResetReason;
    snprintf(m_rstPendingInfo.messageStr,
        sizeof(m_rstPendingInfo.messageStr), "%s", f_messageStr.c_str());
  } else {
    switch (f_eSmCpuResetReason) {
    case e_SS_SM_CPU_RESET_REASON_CRITICAL_ERR:
      /* Outputs the pramfs validation result to the frameworkunifiedlog when falsification is checked */
      checker_pramfs();
      m_pfStopCompleteHandler = (SysMgrCallback<CSystemManager, &CSystemManager::perform_force_reset>);
      // The break is omitted because the same process as for abnormal rebooting is performed.
    case e_SS_SM_CPU_RESET_REASON_GENERIC_ERR:
    case e_SS_SM_CPU_RESET_REASON_DSP_ERR:
      m_isImmResetReq = TRUE;
      FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, " will reset immediately");
      // The break is omitted because the same process as for a normal reboot is performed.

    case e_SS_SM_CPU_RESET_REASON_NORMAL:
    case e_SS_SM_CPU_RESET_REASON_USER_FORCE_RESET: {
        TSystemManagerCpuResetInfo l_resetInfo;

        l_resetInfo.resetReason = f_eSmCpuResetReason;
        snprintf(l_resetInfo.messageStr, SS_SM_RESET_MSG_STR_SIZE, "%s", f_messageStr.c_str());
        l_eStatus = FrameworkunifiedSendMsg(m_hPowerServiceSession, SS_SM_CPU_RESET_REQ,
            sizeof(l_resetInfo), (PVOID) & l_resetInfo);
        LOG_STATUS(l_eStatus,  // LCOV_EXCL_BR_LINE 6: Because the condition cannot be set
            "FrameworkunifiedSendMsg(m_hPowerServiceSession, SS_SM_CPU_RESET_REQ)");
      }
      break;
    case e_SS_SM_CPU_RESET_REASON_DATA_RESET: {
        ESMDataResetType l_eSmDataResetType;

        switch (m_DataResetMode) {
        case e_SS_SM_DATA_RESET_MODE_FACTORY:
          l_eSmDataResetType = e_SS_SM_DATA_RESET_TYPE_FACTORY;
          break;
        case e_SS_SM_DATA_RESET_MODE_USER:
          l_eSmDataResetType = e_SS_SM_DATA_RESET_TYPE_USER;
          break;
        default:
          SS_ASERT_LOG(0, "unexpected data reset mode : %d", m_DataResetMode);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
          l_eStatus = eFrameworkunifiedStatusInvldParam;
        break;
        }
        if (l_eStatus == eFrameworkunifiedStatusOK) {
          l_eStatus = FrameworkunifiedSendMsg(m_hPowerServiceSession,
              SS_SM_REMOTE_DATA_RESET_REQ, sizeof(l_eSmDataResetType),
              (PVOID) & l_eSmDataResetType);
          LOG_STATUS(l_eStatus, "FrameworkunifiedSendMsg(m_hPowerServiceSession, SS_SM_REMOTE_DATA_RESET_REQ)");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
        }
      }
      break;
    case e_SS_SM_CPU_RESET_REASON_INVALID:
    default:
      SS_ASERT_LOG(0, " Error: Unknown CPU reset request type: 0x%X/%d",  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
          f_eSmCpuResetReason, f_eSmCpuResetReason);
      l_eStatus = eFrameworkunifiedStatusInvldParam;
      break;
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::PerformCpuReset( ESMCpuResetReason f_eSmCpuResetReason )

//*****************************************************************************
// CPU Reset Protocol functions                                               *
//                                                                            *
EFrameworkunifiedStatus CSystemManager::OnCpuResetRequest(HANDLE hApp) noexcept {  // SS_SM_CPU_RESET_REQ
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;
  std::string l_moduleName = FrameworkunifiedGetMsgSrc(hApp);
  INTERFACEUNIFIEDLOG_RECEIVED_FROM(hApp);  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
  FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "from %s", l_moduleName.c_str());

  // If you implement FastSleep,
  // ShutdownTrigger of FrameworkunifiedOnStop needs to be changed to FAST_SLEEP_MODE,
  // and also MaxShutdownTime needs to be changed to CriticalAppsMaxShutdownTimeFastSleep

  try {
    TSystemManagerCpuResetInfo l_resetInfo;
    SetCmdHist("SS_SM_CPU_RESET_REQ", m_SMCmdHist, m_SMHistIter,  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
        FrameworkunifiedGetMsgSrc(hApp));  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg < TSystemManagerCpuResetInfo > (hApp, l_resetInfo))) {
      LOG_ERROR("ReadMsg()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    } else {
      char l_cBuf[100];
      EErrorEventType l_errorEventType;
      SMLoggingInfo l_loggingInfo;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

      l_loggingInfo.messageStr = l_resetInfo.messageStr;
      l_loggingInfo.suffixStr = l_resetInfo.suffixStr;

      FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__,
          " CPU Reset Reason: '%d'/'%s' from %s",
          l_resetInfo.resetReason,
          GetStr(l_resetInfo.resetReason).c_str(),
          FrameworkunifiedGetMsgSrc(hApp));

      if (e_SS_SM_CPU_RESET_REASON_IMMRESET_NORMAL == l_resetInfo.resetReason) {
        m_isImmResetReq = TRUE;
        l_resetInfo.resetReason = e_SS_SM_CPU_RESET_REASON_NORMAL;
      }
      switch (l_resetInfo.resetReason) {
      case e_SS_SM_CPU_RESET_REASON_DATA_RESET:
        switch (m_DataResetMode) {
        case e_SS_SM_DATA_RESET_MODE_FACTORY:
        case e_SS_SM_DATA_RESET_MODE_USER:
          break;
        case e_SS_SM_DATA_RESET_MODE_NONE:
        default:
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "unexpected data reset mode : %d", m_DataResetMode);
          throw eFrameworkunifiedStatusInvldParam;
        }
      case e_SS_SM_CPU_RESET_REASON_CRITICAL_ERR:
      case e_SS_SM_CPU_RESET_REASON_GENERIC_ERR:
      case e_SS_SM_CPU_RESET_REASON_DSP_ERR:
      case e_SS_SM_CPU_RESET_REASON_NORMAL:
        l_errorEventType = eErrorEventTypeModuleInvokedResetRequest;
        l_loggingInfo.resetReason = l_resetInfo.resetReason;
        break;
      case e_SS_SM_CPU_RESET_REASON_USER_FORCE_RESET:
        // Separate ErrorEventType only for USER_FORCE_RESET, because the logs to be saved differ.
        l_errorEventType = eErrorEventTypeUserInvokedUserForceReset;
        l_loggingInfo.resetReason = e_SS_SM_CPU_RESET_REASON_USER_FORCE_RESET;
        break;
      case e_SS_SM_CPU_RESET_REASON_INVALID:
      default:
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
            " Error: Unknown CPU reset request type: 0x%X/%d",
            l_resetInfo.resetReason, l_resetInfo.resetReason);
        throw eFrameworkunifiedStatusInvldParam;
      }

      l_eStatus = ErrorEventEnqueue(hApp, l_errorEventType, l_moduleName,
          eErrorEventResetTypeHard, l_loggingInfo);

      snprintf(l_cBuf, sizeof(l_cBuf), "ErrorEventEnqueue(%s)",
               GetStr(l_errorEventType).c_str());  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
      LOG_STATUS_REC_HIST_IF_ERRORED(l_eStatus, l_cBuf);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
    }
  } catch (EFrameworkunifiedStatus e) {
    l_eStatus = e;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

//                                                                            *
// End of CPU Reset Protocol functions                                        *
//*****************************************************************************
EFrameworkunifiedStatus CSystemManager::ValidateRequester(HANDLE hApp,
        std::vector<std::string> &L, PCSTR p_sActionName) {  // LCOV_EXCL_START 6: Because the condition cannot be set
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  //
  // FrameworkunifiedGetMsgSrc( hApp ) is known to return a non-null string from having
  // been successfully called by this function's caller.
  //
  PCSTR p_sRequesterName = FrameworkunifiedGetMsgSrc(hApp);
  if (L.end() == find(L.begin(), L.end(), p_sRequesterName)) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        " Error: Unauthorized service '%s' requested to %s;"
        " request refused.", p_sRequesterName, p_sActionName);
    l_eStatus = eFrameworkunifiedStatusInvldID;
  } else {
    l_eStatus = eFrameworkunifiedStatusOK;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::ValidateRequester( HANDLE hApp )
// LCOV_EXCL_STOP

VOID CSystemManager::InitiateAllGroupsShutdown(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  char l_cBuf[100];

  // Prioritize safe termination sequence and do not save logs during termination
  m_errorEventQueueLocked = TRUE;

  snprintf(l_cBuf, sizeof(l_cBuf), "SaveLastUserModeToNVM(%s)", GetStr(m_lastUserMode).c_str());
  l_eStatus = SaveLastUserModeToNVM(m_lastUserMode);
  LOG_STATUS_REC_HIST_IF_ERRORED(l_eStatus, l_cBuf);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)

  m_MaxShutdownTime = calculate_max_shutdown_time();

  l_eStatus = FrameworkunifiedNPPublishNotification(hApp, NTFY_SSSystemMgrShutdownStarted, NULL, 0);

  SetCmdHist(NTFY_SSSystemMgrShutdownStarted, m_PubCmdHist, m_PubHistIter, "");  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus,  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
      "FrameworkunifiedNPPublishNotification(" NTFY_SSSystemMgrShutdownStarted ")");

  m_GroupLaunchTimer->StopTimer(m_aTimerIDs[eSM_TIMER_START_RESP_MONITOR_WAIT_TIMER]);
  m_GroupLaunchTimer->StopTimer(m_aTimerIDs[eSM_TIMER_CLIENT_START_MONITOR]);

  // Stop Heartbeat thread activity
  l_eStatus = SendRequestToHeartBeat(hApp, SS_HEARTBEAT_STOP, NULL, 0);
  LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus, "SendRequestToHeartBeat(SS_HEARTBEAT_STOP)");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)

  SMSetState(hApp, SS_SM_APPS_STOPPING_AT__CWORD56__REQ);

  FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "shutdownTrigger:%s, lastUserMode:%s",
      GetStr(m_shutdownTrigger).c_str(), GetStr(m_lastUserMode).c_str());

  fprintf(stderr,  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
      "SS_SysManager/%s/State shutdownTrigger:%s, lastUserMode:%s\n",  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
      __FUNCTION__, GetStr(m_shutdownTrigger).c_str(),  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
      GetStr(m_lastUserMode).c_str());  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  /// Send SS_SM_STOP to all modules, starting with the m_ActiveGroupId group
  m_NPPStopSent = FALSE;
  check_all_groups_have_stopped(hApp);

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return;
}  // End of EFrameworkunifiedStatus CSystemManager::InitiateAllGroupsShutdown(HANDLE hApp)

VOID CSystemManager::SendDeferMsg(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (!m_dqDeferMsg.empty()) {
    EFrameworkunifiedStatus l_eStatus;
    SM_POWER_EVENT_INFO deferMsg;

    deferMsg = m_dqDeferMsg.front();
    m_dqDeferMsg.pop();

    HANDLE hSender = McOpenSender(SERVICE_SYSMANAGER);
    if (hSender == NULL) {  // LCOV_EXCL_BR_LINE 4: nsfw error
      // LCOV_EXCL_START 4: nsfw error
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: McOpenSender(SERVICE_SYSMANAGER)");
      // LCOV_EXCL_STOP
    } else {
      CALL_AND_LOG_STATUS_IF_ERRORED(  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
          McSend(hSender, SERVICE_POWER, deferMsg.first,
              sizeof(Pwr_ServiceSetInterface),
              (PVOID) & deferMsg.second));
      CALL_AND_LOG_STATUS_IF_ERRORED(McClose(hSender));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  return;
}  // End of VOID CSystemManager::SendDeferMsg(HANDLE hApp)

///////////////////////////////////////////////////////////////////////////////
/// \ingroup PublishPowerOnOffNotification
/// Power Off notification handler
///
/// \param [in]
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::PublishPowerOnOffNotification(HANDLE hApp) noexcept {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  l_eStatus = PerformPowerOnOffUserModePublication(hApp, NTFY_SSSystemMgrPowerOnOff);

  SetCmdHist(NTFY_SSSystemMgrPowerOnOff, m_PubCmdHist, m_PubHistIter, "");
  LOG_STATUS_REC_HIST_IF_ERRORED(l_eStatus, "PerformPowerOnOffUserModePublication()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus CSystemManager::PublishUserModeNotification(HANDLE hApp) noexcept {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;

  l_eStatus = PerformPowerOnOffUserModePublication(hApp, NTFY_SSSystemMgrUserMode);

  SetCmdHist(NTFY_SSSystemMgrUserMode, m_PubCmdHist, m_PubHistIter, "");
  LOG_STATUS_REC_HIST_IF_ERRORED(l_eStatus, "PerformPowerOnOffUserModePublication()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::PublishUserModeNotification(

EFrameworkunifiedStatus CSystemManager::PerformPowerOnOffUserModePublication(HANDLE hApp,
        const char * p_NotificationStr) noexcept {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  char l_cBuf[500] = { 0 };

  if (m_SMConfig.UMConfig.IsUserModeNotificationABOOL) {
    BOOL l_bUuserMode(m_PowerType_to_SSBoolEnumMap[m_Wake.powerupType]);

    snprintf(l_cBuf, sizeof(l_cBuf), "FrameworkunifiedNPPublishNotification(%s, %s)",
        p_NotificationStr, GetStr(l_bUuserMode).c_str());
    l_eStatus = FrameworkunifiedNPPublishNotification(hApp, p_NotificationStr,
        &l_bUuserMode, sizeof(l_bUuserMode));
  } else {
    T_SS_SM_UserModeOnOffNotification_Struct l_UserModeOnOffStruct(
        m_PowerType_to_SSBoolEnumMap[m_Wake.powerupType],
        m_StartUpReason, m_userModeChangeReason);

    snprintf(l_cBuf, sizeof(l_cBuf),
        "FrameworkunifiedNPPublishNotification(%s, %s, %s, %s)", p_NotificationStr,
        GetStr(l_UserModeOnOffStruct.isUserModeOn).c_str(),
        GetStr(m_StartUpReason).c_str(),
        GetStr(m_userModeChangeReason).c_str());
    l_eStatus = FrameworkunifiedNPPublishNotification(hApp, p_NotificationStr,
        &l_UserModeOnOffStruct, sizeof(l_UserModeOnOffStruct));
  }

  SetCmdHist(p_NotificationStr, m_PubCmdHist, m_PubHistIter, "");
  LOG_STATUS(l_eStatus, l_cBuf);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::PerformPowerOnOffUserModePublication(

EFrameworkunifiedStatus CSystemManager::ResetModulesStateToConnected(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  ModuleLaunchListIter l_moduleIter;
  for (GroupLaunchMapIter l_GroupIter = m_MapProclaunchGrps.begin();
      l_GroupIter != m_MapProclaunchGrps.end(); l_GroupIter++) {
    for (l_moduleIter = l_GroupIter->second.modules.begin();
        l_moduleIter != l_GroupIter->second.modules.end();
        l_moduleIter++) {
      if (l_moduleIter->name == FRAMEWORKUNIFIED_NS_NPSERVICE) {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " %s ( Group %d/%s ) is %s",
            l_moduleIter->name.c_str(), l_GroupIter->second.id,
            l_GroupIter->second.name.c_str(),
            l_moduleIter->ModuleStateStr().c_str());
        l_moduleIter->SetModuleState(MODULE_STATE_STARTED);  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
      // End else if ( FRAMEWORKUNIFIED_NS_NPSERVICE == l_ModuleListIter->name )
      } else {
        switch (l_moduleIter->GetModuleState()) {
        case MODULE_STATE_INVALID:
        case MODULE_STATE_SKIPPED:
        case MODULE_STATE_LAUNCHING:
        case MODULE_STATE_LAUNCHED:
        case MODULE_STATE_LAUNCH_FAILED:
          FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
              " %s ( Group %d/%s ) is %s, NOT setting to "
              "MODULE_STATE_CONNECTED",
              l_moduleIter->name.c_str(), l_GroupIter->second.id,
              l_GroupIter->second.name.c_str(),
              l_moduleIter->ModuleStateStr().c_str());  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
          break;

        case MODULE_STATE_STARTED:
        case MODULE_STATE_START_SENT:
        case MODULE_STATE_CONNECTED:
        case MODULE_STATE_START_FAILED:
        case MODULE_STATE_STOP_FAILED:
        case MODULE_STATE_STOP_SENT:
        case MODULE_STATE_STOPPED:
        case MODULE_STATE_START_PRE_SENT:
        case MODULE_STATE_START_PRE_FAILED:
        case MODULE_STATE_STARTED_PRE:
        case MODULE_STATE_STOP_PRE_SENT:
        case MODULE_STATE_STOP_PRE_FAILED:
        case MODULE_STATE_STOPPED_PRE:
        case MODULE_STATE_START_BACKGROUND_SENT:
        case MODULE_STATE_START_BACKGROUND_FAILED:
        case MODULE_STATE_STARTED_BACKGROUND:
        case MODULE_STATE_STOP_BACKGROUND_SENT:
        case MODULE_STATE_STOP_BACKGROUND_FAILED:
        case MODULE_STATE_STOPPED_BACKGROUND:
          FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " %s ( Group %d/%s ) is %s",
              l_moduleIter->name.c_str(), l_GroupIter->second.id,
              l_GroupIter->second.name.c_str(),
              l_moduleIter->ModuleStateStr().c_str());
          l_moduleIter->SetModuleState(MODULE_STATE_CONNECTED);
          break;
          // default:  Don't code a 'default' here - let the compiler
          // issue a warning ( set via -Wall or -Wswitch ) when the set of
          // enumerations changes - then the maintainer will
          // automagically know to update this switch statement.
        }
      }
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::ResetModulesStateToConnected(HANDLE hApp)

EFrameworkunifiedStatus CSystemManager::OnVarCodeSetRequest(HANDLE hApp) noexcept {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  INTERFACEUNIFIEDLOG_RECEIVED_FROM(hApp);  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
  FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "from %s", FrameworkunifiedGetMsgSrc(hApp));

  SetCmdHist("SS_SM_DEBUG_VAR_CODE_DATA_SET_REQ", m_SMCmdHist, m_SMHistIter, FrameworkunifiedGetMsgSrc(hApp));

  UI_32 l_msgLen;
  if (NULL != m_pVarCodeStr) {
    delete[] m_pVarCodeStr;
  }
  if (0 == (l_msgLen = FrameworkunifiedGetMsgLength(hApp))) {
    l_eStatus = eFrameworkunifiedStatusInvldBufSize;
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, " Warning: Variant code string length is 0.");
  } else if (NULL == (m_pVarCodeStr = new (std::nothrow) CHAR[l_msgLen])) {  // LCOV_EXCL_BR_LINE 5: new's error case
    // LCOV_EXCL_START 5: new's error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_eStatus = eFrameworkunifiedStatusNullPointer;
    LOG_ERROR("new (std::nothrow) CHAR[l_msgLen]");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    // LCOV_EXCL_STOP
  } else {
    l_eStatus = FrameworkunifiedGetMsgDataOfSize(hApp, m_pVarCodeStr, l_msgLen, eSMRRelease);
    if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 4: nsfw error
      // LCOV_EXCL_START 4: nsfw error
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      delete[] m_pVarCodeStr;  // LCOV_EXCL_LINE 6: Because it cannot pass the delete[] failure case
      m_pVarCodeStr = NULL;

      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
          " Error: FrameworkunifiedGetMsgDataOfSize(hApp, m_pVarCodeStr, "
              "l_msgLen '%d', eSMRRelease) errored: %d/'%s'",
          l_msgLen, l_eStatus, GetStr(l_eStatus).c_str());
      // LCOV_EXCL_STOP
    } else {
      // Guarantee NULL termination of the string.
      m_pVarCodeStr[l_msgLen - 1] = 0;
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " m_pVarCodeStr is '%s'", m_pVarCodeStr);
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus CSystemManager::OnSetBootloaderInfoRequest(HANDLE hApp) noexcept {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "from %s", FrameworkunifiedGetMsgSrc(hApp));
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  TSystemManagerBootInfoStruct l_bootInfo;

  SetCmdHist("SS_SM_SET_BOOTLOADER_INFO", m_SMCmdHist, m_SMHistIter, FrameworkunifiedGetMsgSrc(hApp));

  l_eStatus = ReadMsg < TSystemManagerBootInfoStruct > (hApp, l_bootInfo);
  if (eFrameworkunifiedStatusOK != l_eStatus) {
    LOG_ERROR("ReadMsg()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  }

  // Instead of sending Return Message, this function must call FrameworkunifiedSetSyncResponseData()
  // because this request is sent by FrameworkunifiedInvokeSync()
  l_eStatus = FrameworkunifiedSetSyncResponseData(hApp, &l_eStatus, sizeof(l_eStatus));
  LOG_STATUS_IF_ERRORED(l_eStatus, "FrameworkunifiedSetSyncResponseData()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}


EFrameworkunifiedStatus CSystemManager::OnGetBootloaderInfoRequest( HANDLE hApp ) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "from %s", FrameworkunifiedGetMsgSrc(hApp));
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  SetCmdHist("SS_SM_GET_BOOTLOADER_INFO", m_SMCmdHist, m_SMHistIter, FrameworkunifiedGetMsgSrc(hApp));  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  // Instead of sending Return Message, this function must call FrameworkunifiedSetSyncResponseData()
  // because this request is sent by FrameworkunifiedInvokeSync()
  l_eStatus = FrameworkunifiedSetSyncResponseData(hApp, m_BootMemory, sizeof(LBM_RAM_t));
  LOG_STATUS_IF_ERRORED(l_eStatus, "FrameworkunifiedSetSyncResponseData()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}


EFrameworkunifiedStatus CSystemManager::On_CWORD56_HeartBeatRequest(HANDLE hApp) noexcept {
  FRAMEWORKUNIFIEDLOG(ZONE_PERIODIC_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  PCSTR l_senderName = FrameworkunifiedGetMsgSrc(hApp);

  SetCmdHist("SS_SM__CWORD56__HEARTBEAT_REQ", m_SMCmdHist, m_SMHistIter, l_senderName);

  FRAMEWORKUNIFIEDLOG(ZONE_PERIODIC_INFO, __FUNCTION__, " Received from %s", l_senderName);
  EPWR_HB_REQ_MSG_STRUCT l_hbReq;

  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg < EPWR_HB_REQ_MSG_STRUCT > (hApp, l_hbReq))) {
    LOG_ERROR("ReadMsg()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  } else {

    m_GroupLaunchTimer->StopTimer(m_aTimerIDs[eSM_TIMER__CWORD56__HEARTBEAT_RESPONSE]);

    if (l_hbReq.IntervalSec > 0) {
      m_GroupLaunchTimer->StartTimer(
          m_aTimerIDs[eSM_TIMER__CWORD56__HEARTBEAT_RESPONSE], 0, 0,
          l_hbReq.IntervalSec, 0);
    } else if (l_hbReq.IntervalSec == 0) {  // LCOV_EXCL_BR_LINE 8: As the condition is never false
      l_eStatus = FrameworkunifiedSendMsg(m_hPowerServiceSession, SS_SM__CWORD56__HEARTBEAT_RSPN, 0, NULL);

      LOG_STATUS(l_eStatus, "FrameworkunifiedSendMsg(m_hPowerServiceSession, SS_SM__CWORD56__HEARTBEAT_RSPN)");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_PERIODIC_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

EFrameworkunifiedStatus CSystemManager::On_CWORD56_HeartBeatResponseIntervalTimerExpiry(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_PERIODIC_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  SetCmdHist("SM_TIMER__CWORD56__HEARTBEAT_RESPONSE", m_TimerCmdHist, m_TimerHistIter, FrameworkunifiedGetMsgSrc(hApp));  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  l_eStatus = FrameworkunifiedSendMsg(m_hPowerServiceSession, SS_SM__CWORD56__HEARTBEAT_RSPN, 0, NULL);

  LOG_STATUS(l_eStatus, "FrameworkunifiedSendMsg(m_hPowerServiceSession, SS_SM__CWORD56__HEARTBEAT_RSPN)");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  FRAMEWORKUNIFIEDLOG(ZONE_PERIODIC_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

// EOF of /SS_SystemManager/src/ss_system_manager_callbacks.cpp

