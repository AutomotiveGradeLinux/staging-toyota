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
/// \brief    This file provides support for System Manager configuration.
///
///////////////////////////////////////////////////////////////////////////////

#include <boost/algorithm/string.hpp>


#include <other_service/PosixBasedOS001ClockCycleApi.h>
#include <other_service/ultoa.h>
#include <native_service/frameworkunified_framework_if.h>
#include <system_service/ss_string_maps.h>
#include <system_service/ss_templates.h>
#include <native_service/ns_config_parser_if.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <cctype>
#include <string>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <functional>
#include <list>
#include <vector>
#include <utility>

#include "ss_sm_config.h"
#include "ss_sm_systemmanagerlog.h"
#include "ss_sm_launch_configuration.h"
#include "system_launcher_cfg_format.h"
#include "ss_sm_default_paths.h"

using namespace std;  // NOLINT

// Constants for Heart Beat configuration data
const UI_32 HBApplicationHeartBeatIntervalInitial = 40u;  // Units of seconds. 0 = HB Disabled.
const UI_32 HBApplicationHeartBeatIntervalRepeat = 5u;  // Units of seconds. 0 = HB Disabled.
const UI_32 HBMaxHeartBeatRetryCount = 3u;

// Constants for Crash Detector configuration data
const BOOL CDEnabled = TRUE;

// Constants for system low memory configuration data

const UI_32 SLMTimerValue = 5000;        // msec
const UI_32 SLMThresholdValue = (1024 * 512);  // bytes
const UI_32 SLMRetryCount = 6;
const UI_32 SLMSystemmanagerLogIntervalMs = 30000;

// Constants for Module connect time and Stat Resp Time

const UI_32 ModuleConnectionNumTimesToCheck = 5;
const UI_32 ModuleConnectionTimeOutSec = 5;  // sec
const UI_32 ModuleStartRespTimeOutSec = 120;  // sec

// Constants for Critical Apps Max wait timeout

const UI_32 CriticalAppsMaxShutdownTimeFastSleepSec = 2;  // sec
const UI_32 CriticalAppsMaxShutdownTimeNormalResetSec = 15;  // sec

// Constants for User Mode data structure type
const BOOL isBoolean = TRUE;

const CHAR SMConfigDataHeader[] = "SystemManager";
const CHAR HBConfigDataHeader[] = "HeartBeat";
const CHAR CDConfigDataHeader[] = "CrashDetector";
const CHAR SLMConfigDataHeader[] = "SysLowMemory";
const CHAR UserModeConfigDataHeader[] = "UserMode";

struct CfgLaunchParams {
  CfgLaunchParams() :
        group_id(0),
        group_launch_wait(0),
        group_wait_for_trigger(0),
        priority(0),
        critical(0),
        retry_cnt(0),
        is_start_required(0),
        shutdown_critical(0),
        shutdown_wait_time(0),
        fast_shutdown_wait_time(0) {
    std::memset(&group_name[0], 0, sizeof(group_name));
    std::memset(&name[0], 0, sizeof(name));
    std::memset(&binary_name[0], 0, sizeof(binary_name));
    std::memset(&arguments[0], 0, sizeof(arguments));
    std::memset(&logging_msk_str[0], 0, sizeof(logging_msk_str));
    std::memset(&restart[0], 0, sizeof(restart));
    std::memset(&unix_user_name[0], 0, sizeof(unix_user_name));
    std::memset(&env_value_condition[0], 0, sizeof(env_value_condition));
  }

  CHAR group_name[100];
  UI_32 group_id;
  UI_32 group_launch_wait;
  BOOL group_wait_for_trigger;
  CHAR name[100];
  CHAR binary_name[MAX_PATH_LENGTH];
  UI_32 priority;
  BOOL critical;
  UI_32 retry_cnt;
  CHAR arguments[512];
  CHAR logging_msk_str[50];
  CHAR restart[32];
  BOOL is_start_required;
  BOOL shutdown_critical;
  UI_32 shutdown_wait_time;
  UI_32 fast_shutdown_wait_time;
  TUserNameBuffer unix_user_name;  // TODO(username): Stuff.: Does Posix define a useful
                                   // constant representing max length of user id?
  CHAR env_value_condition[256];
};

namespace ss_sm_cfg {
class cfg_args {
 public:
  enum {  // typedef enum _tokens
    group_name,
    group_id,
    group_launch_wait,
    group_wait_for_trigger,
    name,
    binary_name,
    priority,
    critical,
    retry_cnt,
    arguments,
    logging_msk,
    restart,
    is_start_required,
    shutdown_critical,
    shutdown_wait_time,
    fast_shutdown_wait_time,
    unix_user_name,
    env_value_condition,
    END
  };
};
};  // namespace ss_sm_cfg


/*******************************************************************************
 * CTimeSpan class                                                             *
 *                                                                             *
 * Set Initial time, reset Final and Delta to zero, return time                *
 *                                                                             */
UI_64 CTimeSpan::Begin() {
  initial = GetTimeMilliseconds();
  final = 0;
  delta = 0;
  return initial;
}  // End of UI_64 CTimeSpan::Begin()

/*
 * Set Final time, calculate Delta time, return time
 */
UI_64 CTimeSpan::End() {
  final = GetTimeMilliseconds();
  delta = (final > initial) ? (final - initial) : 0;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  return final;
}  // End of UI_64 CTimeSpan::End()

UI_64 CTimeSpan::GetInitial(void) {
  return initial;
}

UI_64 CTimeSpan::GetFinal() {
  return final;
}

UI_64 CTimeSpan::GetDelta() {
  return delta;
}

UI_64 CTimeSpan::GetTimeMilliseconds() {
  UI_64 l_totalmsec = 0;
  struct timespec timeSpec;

  if (0 != clock_gettime(CLOCK_MONOTONIC, &timeSpec)) {
    SS_ASERT_ERRNO(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  } else {
    l_totalmsec = (timeSpec.tv_sec * 1000ULL)
            + (timeSpec.tv_nsec / 1000000);
  }

  return l_totalmsec;
}

/*                                                                             *
 * End of CTimeSpan class                                                      *
 ******************************************************************************/

ModuleLaunchParams::ModuleLaunchParams() noexcept :
  name("")
  , path("")
  , arguments("")
  , restart("")
  , configuration_priority(0)
  , previous_priority(0)
  , current_priority(0)
  , critical(FALSE)
  , retry_cnt(0)
  , logging_msk_str("")
  , is_start_required(FALSE)
  , shutdown_critical(FALSE)
  , shutdown_wait_time(0)
  , fast_shutdown_wait_time(0)
  , pid(0)
  , hsession(NULL)
  , relaunch_count(0)
  , relaunch_status(NotRelaunched)
  , group_id(0xFFFF)
  , m_serviceAvailability(FALSE)
  , m_startReason()
  , m_stopReason()
  , m_ModuleDebugDumpState(MODULE_DEBUG_DUMP_STATE_NOT_REQUESTED) {
  SetModuleState(MODULE_STATE_INVALID, FALSE);
  //
  //    Module State strings
  m_ModuleStateStrMap[MODULE_STATE_INVALID] = "INVALID";
  m_ModuleStateStrMap[MODULE_STATE_SKIPPED] = "SKIPPED";
  m_ModuleStateStrMap[MODULE_STATE_LAUNCHING] = "LAUNCHING";
  m_ModuleStateStrMap[MODULE_STATE_LAUNCHED] = "LAUNCHED";
  m_ModuleStateStrMap[MODULE_STATE_LAUNCH_FAILED] = "LAUNCH_FAIL";
  m_ModuleStateStrMap[MODULE_STATE_CONNECTED] = "CONNECTED";
  m_ModuleStateStrMap[MODULE_STATE_START_SENT] = "START_SENT";
  m_ModuleStateStrMap[MODULE_STATE_START_FAILED] = "START_FAIL";
  m_ModuleStateStrMap[MODULE_STATE_STARTED] = "STARTED";
  m_ModuleStateStrMap[MODULE_STATE_STOP_FAILED] = "STOP_FAIL";
  m_ModuleStateStrMap[MODULE_STATE_STOP_SENT] = "STOP_SENT";
  m_ModuleStateStrMap[MODULE_STATE_STOPPED] = "STOPPED";
  m_ModuleStateStrMap[MODULE_STATE_START_PRE_SENT] = "PRE_START_SENT";
  m_ModuleStateStrMap[MODULE_STATE_START_PRE_FAILED] = "PRE_START_FAIL";
  m_ModuleStateStrMap[MODULE_STATE_STARTED_PRE] = "PRE_STARTED";
  m_ModuleStateStrMap[MODULE_STATE_STOP_PRE_SENT] = "PRE_STOP_SENT";
  m_ModuleStateStrMap[MODULE_STATE_STOP_PRE_FAILED] = "PRE_STOP_FAIL";
  m_ModuleStateStrMap[MODULE_STATE_STOPPED_PRE] = "PRE_STOPPED";
  m_ModuleStateStrMap[MODULE_STATE_START_BACKGROUND_SENT] = "BACKGROUND_START_SENT";
  m_ModuleStateStrMap[MODULE_STATE_START_BACKGROUND_FAILED] = "BACKGROUND_START_FAIL";
  m_ModuleStateStrMap[MODULE_STATE_STARTED_BACKGROUND] = "BACKGROUND_STARTED";
  m_ModuleStateStrMap[MODULE_STATE_STOP_BACKGROUND_SENT] = "BACKGROUND_STOP_SENT";
  m_ModuleStateStrMap[MODULE_STATE_STOP_BACKGROUND_FAILED] = "BACKGROUND_STOP_FAIL";
  m_ModuleStateStrMap[MODULE_STATE_STOPPED_BACKGROUND] = "BACKGROUND_STOPPED";

  m_startReason.SetReason(NotStarted);
  m_stopReason.SetReason(NotStopped);

  MAP_ENTRY(m_ModuleDebugDumpStateStrMap, MODULE_DEBUG_DUMP_STATE_NOT_REQUESTED);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  MAP_ENTRY(m_ModuleDebugDumpStateStrMap, MODULE_DEBUG_DUMP_STATE_REQUEST_SENT);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  MAP_ENTRY(m_ModuleDebugDumpStateStrMap, MODULE_DEBUG_DUMP_STATE_RESPONSE_RECEIVED);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  std::memset(&unix_user_name[0], 0, sizeof(unix_user_name));
}  // End of ModuleLaunchParams::ModuleLaunchParams() :

ModuleLaunchParams::~ModuleLaunchParams() {
}  // End of ModuleLaunchParams::~ModuleLaunchParams() :

SMModuleState ModuleLaunchParams::GetModuleState(void) noexcept {
    return m_moduleState;
}  // End of SMModuleState ModuleLaunchParams::GetModuleState(void)

BOOL ModuleLaunchParams::IsModuleState(SMModuleState f_moduleState) noexcept {
    return m_moduleState == f_moduleState;
}  // End of BOOL ModuleLaunchParams::IsModuleState(SMModuleState f_moduleState )

// IsModuleConnected
// Determine if a module is in a state capable of receiving messages from System Manager.
BOOL ModuleLaunchParams::IsModuleConnected() const noexcept {
  BOOL l_isModuleConnected = FALSE;

  switch (m_moduleState) {
    case MODULE_STATE_CONNECTED:
    case MODULE_STATE_START_SENT:
    case MODULE_STATE_START_FAILED:
    case MODULE_STATE_STARTED:
    case MODULE_STATE_STOP_SENT:
    case MODULE_STATE_STOP_FAILED:
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
      l_isModuleConnected = TRUE;
      break;

    default:
      l_isModuleConnected = FALSE;
      break;
  }

  return (l_isModuleConnected);
}  // End of BOOL ModuleLaunchParams::IsModuleConnected(void)

VOID ModuleLaunchParams::SetModuleState(SMModuleState f_moduleState, BOOL f_bLog) noexcept {
  m_moduleState = f_moduleState;

  switch (m_moduleState) {  // Because the module status cannot be changed from the external API
    case MODULE_STATE_INVALID:
    case MODULE_STATE_SKIPPED:
    case MODULE_STATE_LAUNCH_FAILED:
    case MODULE_STATE_LAUNCHING:
    case MODULE_STATE_LAUNCHED:
    case MODULE_STATE_CONNECTED:
      break;
    case MODULE_STATE_START_SENT:
      m_startReason.Begin();
      break;
    case MODULE_STATE_START_FAILED:
      m_startReason.End();
      m_startReason.SetReason(StartedByModuleStartFail);
      break;
    case MODULE_STATE_STARTED:
      m_startReason.End();
      m_startReason.SetReason(StartedByModuleStartComplete);
      break;

    case MODULE_STATE_STOP_SENT:
      m_stopReason.Begin();
      break;

    case MODULE_STATE_STOP_FAILED:
      m_stopReason.End();
      m_stopReason.SetReason(StoppedByModuleStopFail);
      break;
    case MODULE_STATE_STOPPED:
      m_stopReason.End();
      m_stopReason.SetReason(StoppedByModuleStopComplete);
      break;

    case MODULE_STATE_START_PRE_SENT:
      m_startReason.Begin();
      break;
    case MODULE_STATE_START_PRE_FAILED:
      m_startReason.End();
      m_startReason.SetReason(StartedByModulePreStartFail);
      break;
    case MODULE_STATE_STARTED_PRE:
      m_startReason.End();
      m_startReason.SetReason(StartedByModulePreStartComplete);
      break;
    case MODULE_STATE_STOP_PRE_SENT:
      m_stopReason.Begin();
      break;
    case MODULE_STATE_STOP_PRE_FAILED:
      m_stopReason.End();
      m_stopReason.SetReason(StoppedByModulePreStopFail);
      break;
    case MODULE_STATE_STOPPED_PRE:
      m_stopReason.End();
      m_stopReason.SetReason(StoppedByModulePreStopComplete);
      break;

    case MODULE_STATE_START_BACKGROUND_SENT:
      m_startReason.Begin();
      break;
    case MODULE_STATE_START_BACKGROUND_FAILED:
      m_startReason.End();
      m_startReason.SetReason(StartedByModuleBackgroundStartFail);
      break;
    case MODULE_STATE_STARTED_BACKGROUND:
      m_startReason.End();
      m_startReason.SetReason(StartedByModuleBackgroundStartComplete);
      break;
    case MODULE_STATE_STOP_BACKGROUND_SENT:
      m_stopReason.Begin();
      break;
    case MODULE_STATE_STOP_BACKGROUND_FAILED:
      m_stopReason.End();
      m_stopReason.SetReason(StoppedByModuleBackgroundStopFail);
      break;
    case MODULE_STATE_STOPPED_BACKGROUND:
      m_stopReason.End();
      m_stopReason.SetReason(StoppedByModuleBackgroundStopComplete);
      break;

    // default:  Don't code a 'default' here - let the compiler
    // issue a warning ( set via -Wall or -Wswitch ) when the set of
    // enumerations changes - then the maintainer will
    // automagically know to update this switch statement.
  }

  if (f_bLog) {
    char buf[255];
    char *p = buf;
    strcpy(p, m_ModuleStateStrMap[m_moduleState].c_str());  // NOLINT
    p += m_ModuleStateStrMap[m_moduleState].size();
    strcpy(p, ":");  // NOLINT
    p += sizeof(":") - 1;
    strcpy(p, name.c_str());  // NOLINT
    FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, buf);
  }
}  // End of VOID ModuleLaunchParams::SetModuleState(SMModuleState f_moduleState )

std::string ModuleLaunchParams::ModuleStateStr(void) noexcept {
  return m_ModuleStateStrMap[m_moduleState];
}  // End of std::string ModuleLaunchParams::ModuleStateStr(SMModuleState f_moduleState )

SMModuleDebugDumpState ModuleLaunchParams::GetModuleDebugDumpState(void) noexcept {  // LCOV_EXCL_START 6: Because the condition cannot be set // NOLINT(whitespace/line_length)
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  return m_ModuleDebugDumpState;
}  // End of SMModuleDebugDumpState ModuleLaunchParams::GetModuleDebugDumpState(void)
// LCOV_EXCL_STOP

std::string ModuleLaunchParams::GetModuleDebugDumpStateStr(void) noexcept {
  return m_ModuleDebugDumpStateStrMap[m_ModuleDebugDumpState];
}  // End of std::string ModuleLaunchParams::GetModuleDebugDumpStateStr()

VOID ModuleLaunchParams::SetModuleDebugDumpState(
        SMModuleDebugDumpState f_moduleDebugDumpState, BOOL f_bLog) noexcept {
  m_ModuleDebugDumpState = f_moduleDebugDumpState;

  if (f_bLog) {  // LCOV_EXCL_BR_LINE 200:f_bLog must be TRUE
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " %s set for %s",
            GetModuleDebugDumpStateStr().c_str(), name.c_str());
  }
}  // End of VOID ModuleLaunchParams::SetModuleDebugDumpState(SMModuleDebugDumpState f_moduleDebugDumpState )

EFrameworkunifiedStatus ModuleLaunchParams::SendMsgAndUpdateState(
    const UI_32 f_iCmd, const T_SS_SM_START_DataStructType * const f_startData)
        noexcept {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  char l_cBuf[500] = " Error: char l_cBuf[] not built";
  T_SS_SM_START_DataStructType l_startData;
  SMModuleState l_SendSuccess = MODULE_STATE_START_SENT;
  SMModuleState l_SendFailed = MODULE_STATE_START_FAILED;

  memcpy(&l_startData, f_startData, sizeof(T_SS_SM_START_DataStructType));

  switch (relaunch_status) {  // LCOV_EXCL_BR_LINE 6: Because all conditions cannot be satisfied from the external API
  case NotRelaunched:
    // Setting startup information at the first startup
    break;
  case RelaunchSafe:  // LCOV_EXCL_START 6: Because all conditions cannot be satisfied from the external API
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_startData.resetStatus = e_SS_SM_RESET_STATUS_NONE;
    break;
  case RelaunchErr:
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_startData.resetStatus = e_SS_SM_RESET_STATUS_NG;
    break;
  default:
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    SS_ASERT(0);
    break;
    // LCOV_EXCL_STOP
  }

  switch (f_iCmd) {
  case SS_SM_PRE_START:
    l_SendSuccess = MODULE_STATE_START_PRE_SENT;
    l_SendFailed = MODULE_STATE_START_PRE_FAILED;
    break;
  case SS_SM_PRE_STOP:
    l_SendSuccess = MODULE_STATE_STOP_PRE_SENT;
    l_SendFailed = MODULE_STATE_STOP_PRE_FAILED;
    break;
  case SS_SM_BACKGROUND_START:
    l_SendSuccess = MODULE_STATE_START_BACKGROUND_SENT;
    l_SendFailed = MODULE_STATE_START_BACKGROUND_FAILED;
    break;
  case SS_SM_BACKGROUND_STOP:
    l_SendSuccess = MODULE_STATE_STOP_BACKGROUND_SENT;
    l_SendFailed = MODULE_STATE_STOP_BACKGROUND_FAILED;
    break;
  case SS_SM_START:
  default:
    l_SendSuccess = MODULE_STATE_START_SENT;
    l_SendFailed = MODULE_STATE_START_FAILED;
    break;
  }

  const EFrameworkunifiedStatus l_eStatus = FrameworkunifiedSendMsg(hsession, f_iCmd,
                                          sizeof(T_SS_SM_START_DataStructType),
                                          static_cast<PCVOID>(&l_startData));

  // LCOV_EXCL_START 6: As no TRUE is returned
  if (IS_ZONE_SET(ZONE_ERR) || IS_ZONE_SET(ZONE_INFO)) {

    snprintf(l_cBuf, sizeof(l_cBuf), "FrameworkunifiedSendMsg(%s, system_manager protocol message, %d/%s)",
        name.c_str(), l_startData.startupReason,
        GetStr(l_startData.startupReason).c_str());
  }
  // LCOV_EXCL_STOP 6:As no TRUE is returned
  if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 4:NSFW's error
    // LCOV_EXCL_START 4:NSFW's error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR(l_cBuf);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    SetModuleState(l_SendFailed);
    // LCOV_EXCL_STOP
  } else {
    LOG_SUCCESS(l_cBuf);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    SetModuleState(l_SendSuccess);
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
// End of EFrameworkunifiedStatus ModuleLaunchParams::SendMsgAndUpdateState(T_SS_SM_START_DataStructType const * f_startData)

EFrameworkunifiedStatus ModuleLaunchParams::SendMsgAndUpdateState(
        T_SS_SM_STOP_DataStructType const * f_stopData) noexcept {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  char l_cBuf[500] = " Error: char l_cBuf[] not built";
  EFrameworkunifiedStatus l_eStatus = FrameworkunifiedSendMsg(hsession, SS_SM_STOP,
        sizeof(T_SS_SM_STOP_DataStructType),
        static_cast<PCVOID>(f_stopData));

  if (IS_ZONE_SET(ZONE_ERR) || IS_ZONE_SET(ZONE_INFO)) {  // LCOV_EXCL_BR_LINE 200:alwasy true
    snprintf(l_cBuf, sizeof(l_cBuf), "FrameworkunifiedSendMsg(%s, SS_SM_STOP, %d/%s, "
        "Last User Mode: %s)", name.c_str(),
        f_stopData->shutdownTrigger,
        GetStr(f_stopData->shutdownTrigger).c_str(),
        GetStr(f_stopData->lastUserMode).c_str());
  }
  if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 4:NSFW's error
    // LCOV_EXCL_START 4:NSFW's error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR(l_cBuf);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    SetModuleState(MODULE_STATE_STOP_FAILED);
    // LCOV_EXCL_STOP
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " %s successful", l_cBuf);
    SetModuleState(MODULE_STATE_STOP_SENT);
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
// End of EFrameworkunifiedStatus ModuleLaunchParams::SendMsgAndUpdateState(T_SS_SM_STOP_DataStructType const * f_stopData)

EFrameworkunifiedStatus ModuleLaunchParams::GetPriority(UI_32 & f_Priority) noexcept {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;

  struct sched_param cur_sch_params;
  errno = 0;
  f_Priority = 0xDEAD;

  if (0 == pid) {  // LCOV_EXCL_BR_LINE 200: pid can not be 0
    // LCOV_EXCL_START 200: pid can not be 0
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_eStatus = eFrameworkunifiedStatusInvldParam;
    LOG_ERROR("0 == pid");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    // LCOV_EXCL_STOP
  } else if (-1 >= sched_getparam(pid, &cur_sch_params)) {  // LCOV_EXCL_BR_LINE 5: system function failed
    // LCOV_EXCL_START 5: system function failed
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_eStatus = eFrameworkunifiedStatusFault;
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        " Error: 'sched_getparam( pid %d, &cur_sch_params )' returned -1, "
                "strerr() is '%s'", pid, strerror(errno));
    // LCOV_EXCL_STOP
  } else {
    l_eStatus = eFrameworkunifiedStatusOK;
    f_Priority = cur_sch_params.sched_priority;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus ModuleLaunchParams::GetPriority (UI_32 & f_Priority )

EFrameworkunifiedStatus ModuleLaunchParams::SetPriority(UI_32 f_Priority) noexcept {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;

  errno = 0;

  if (99 < f_Priority) {  // LCOV_EXCL_BR_LINE 200: priority can not greater than 99
    // LCOV_EXCL_START 200: priority can not greater than 99
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_eStatus = eFrameworkunifiedStatusInvldParam;
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: Invalid 'f_Priority' %d/0x%X",
            f_Priority, f_Priority);
    // LCOV_EXCL_STOP
  } else if (0 == pid) {  // LCOV_EXCL_BR_LINE 200: pid can not be 0
    // LCOV_EXCL_START 200: pid can not be 0
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_eStatus = eFrameworkunifiedStatusInvldParam;
    LOG_ERROR("0 == pid");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    // LCOV_EXCL_STOP
  } else {
    int rc;
    struct sched_param l_schedParam = { };
    l_schedParam.sched_priority = f_Priority;

    if (f_Priority == 0) {  // LCOV_EXCL_BR_LINE 200: f_Priority can not be 0
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      rc = sched_setscheduler(pid, SCHED_OTHER, &l_schedParam);  // LCOV_EXCL_LINE 200: f_Priority can not be 0
    } else {
      rc = sched_setscheduler(pid, SCHED_FIFO, &l_schedParam);
    }
    SS_ASERT_ERRNO(0 == rc);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    if (rc == 0) {
      previous_priority = current_priority;
      current_priority = f_Priority;
      l_eStatus = eFrameworkunifiedStatusOK;
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
          " setprio( %s, pid %d, 'previous_priority' %d -> %d ) successful",
          name.c_str(), pid, previous_priority, f_Priority);
    } else if (rc == -1) {  // LCOV_EXCL_BR_LINE 5: system function failed
      // LCOV_EXCL_START 5: system function failed
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_eStatus = eFrameworkunifiedStatusFault;
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
          " setprio( %s, pid %d, 'current_priority' %d -> %d ) returned '-1', strerr() is '%s'",
          name.c_str(), pid, current_priority, f_Priority,
          strerror(errno));
      // LCOV_EXCL_STOP
    } else {  // LCOV_EXCL_BR_LINE 5: can not be other value
      // LCOV_EXCL_START 5: can not be other value
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_eStatus = eFrameworkunifiedStatusFault;
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
          " setprio( %s, pid %d, 'current_priority' %d -> %d ) returned '%d'; "
                  "'rc' is neither original priority nor '-1': 'errno' is '%d', strerr() is '%s'",
          name.c_str(), pid, current_priority, f_Priority, rc,
          errno, strerror(errno));
      // LCOV_EXCL_STOP
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus ModuleLaunchParams::SetPriority( UI_32 f_Priority )

SysMgrConfiguration::SysMgrConfiguration() noexcept:
l_pReaderCfg(NULL) {
}

SysMgrConfiguration::~SysMgrConfiguration() {
}

static BOOL is_satisfy_env_launch_cond(CHAR *env_cond) {  // LCOV_EXCL_START 6: Because the condition cannot be set
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  BOOL l_isSatisfy = FALSE;
  BOOL l_isReverse = FALSE;
  CHAR *l_pString = NULL;

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "env_cond [%s]", env_cond);

  if (NULL != (l_pString = strstr(env_cond, "!="))) {
    l_isReverse = TRUE;
  } else if (NULL == (l_pString = strstr(env_cond, "=="))) {
    SS_ASERT(0);
  }

  if (NULL != l_pString) {
    CHAR *l_pEnv = env_cond;
    CHAR *l_pValue = l_pString + 2;
    *l_pString = '\0';

    CHAR *l_pEnvVariable = NULL;

    if (NULL != (l_pEnvVariable = std::getenv(l_pEnv))) {
      l_isSatisfy = (0 == strcmp(l_pValue, l_pEnvVariable)) ? TRUE : FALSE;
      if (l_isReverse) {
        l_isSatisfy = !l_isSatisfy;
      }
    }
  }

  return l_isSatisfy;
}

void set_grp_member_info(const LaunchInfo & launch_info,
        ModuleLaunchList & module_lst) noexcept {  // NOLINT
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  // Note: the
  ModuleLaunchParams grp_member_info;
  grp_member_info.name = launch_info.name;
  grp_member_info.path = launch_info.binary_name;
  grp_member_info.arguments = launch_info.arguments;
  grp_member_info.restart = launch_info.restart;
  grp_member_info.configuration_priority = launch_info.priority;
  grp_member_info.retry_cnt = launch_info.retry_cnt;
  grp_member_info.logging_msk_str = launch_info.logging_msk_str;
  grp_member_info.critical = launch_info.critical;
  grp_member_info.shutdown_critical = launch_info.shutdown_critical;
  grp_member_info.shutdown_wait_time = launch_info.shutdown_wait_time;
  grp_member_info.group_id = launch_info.group_id;
  grp_member_info.is_start_required = launch_info.is_start_required;
  grp_member_info.fast_shutdown_wait_time = launch_info.fast_shutdown_wait_time;

  module_lst.push_back(grp_member_info);
}
// LCOV_EXCL_STOP

ModuleLaunchListIter set_cfg_grp_member_info(
        const CfgLaunchParams & launch_info, ModuleLaunchList & module_lst) noexcept {  // NOLINT
  ModuleLaunchParams grp_member_info;
  ModuleLaunchListIter moduleLaunchListIter;

  grp_member_info.name = launch_info.name;
  grp_member_info.path = launch_info.binary_name;
  grp_member_info.arguments = (  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
      std::strcmp(launch_info.arguments, "NULL") == 0 ? "" : launch_info.arguments);
  grp_member_info.restart = (  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
      std::strcmp(launch_info.restart, "NULL") == 0 ? "" : launch_info.restart);
  grp_member_info.configuration_priority = launch_info.priority;
  grp_member_info.retry_cnt = launch_info.retry_cnt;
  grp_member_info.logging_msk_str = (  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
      std::strcmp(launch_info.logging_msk_str, "NULL") == 0 ? "" : launch_info.logging_msk_str);

  grp_member_info.critical = launch_info.critical;
  grp_member_info.shutdown_critical = launch_info.shutdown_critical;
  grp_member_info.shutdown_wait_time = launch_info.shutdown_wait_time;
  grp_member_info.group_id = launch_info.group_id;
  grp_member_info.is_start_required = launch_info.is_start_required;
  grp_member_info.fast_shutdown_wait_time = launch_info.fast_shutdown_wait_time;

  strncpy(&grp_member_info.unix_user_name[0], &launch_info.unix_user_name[0],
      sizeof(TUserNameBuffer));

  moduleLaunchListIter = module_lst.insert(module_lst.end(), grp_member_info);
  return (moduleLaunchListIter);
}

void load_parameters_order_cfg(GroupLaunchMap &groups_map,  // NOLINT
        LaunchOrderedVector &f_OrderList, SS_String &f_launchOrderName,  // NOLINT
        SS_String &f_launchCfgFn) noexcept {  // NOLINT
  SS_String l_OrderCfgFn = f_launchCfgFn;
  string::size_type pos = l_OrderCfgFn.find(".cfg");

  if (pos == string::npos) {  // LCOV_EXCL_BR_LINE 5:C error
    // LCOV_EXCL_START 5:C error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "invalid file name:%s",
            f_launchCfgFn.c_str());
    return;
    // LCOV_EXCL_STOP
  }

  l_OrderCfgFn.replace(pos, strlen(".order.cfg"), ".order.cfg");
  CNSConfigReader *l_pReaderOrderCfg = new CNSConfigReader(l_OrderCfgFn);

  if (l_pReaderOrderCfg != NULL) {  // LCOV_EXCL_BR_LINE 5:new error
    CHAR key[128];
    std::sprintf(key, "%s.order", f_launchOrderName.c_str());  // NOLINT
    SS_String value = l_pReaderOrderCfg->GetString(key);
    std::vector<std::string> strList;
    if (value.empty()) {  // LCOV_EXCL_BR_LINE 200:cannot be empty
      goto ERROR;
    }
// LCOV_EXCL_START 6: Because an initialization condition is set at SystemManager startup and the branching condition cannot be satisfied
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    boost::split(strList, value, boost::is_any_of("|"));
    for (UI_32 i = 0; i < strList.size(); i++) {
        char* endptr = NULL;
        f_OrderList.push_back(
                static_cast<UI_32>(strtoul(strList[i].c_str(), &endptr, 10)));
        if (*endptr != '\0') {
            // Discard boot order if invalid string is detected
            f_OrderList.clear();
            goto ERROR;
        }
    }
    std::sprintf(key, "%s.owlist", f_launchOrderName.c_str());  // NOLINT
    SS_String ow_list_value = l_pReaderOrderCfg->GetString(key);
    std::vector<std::string> owList;

    if (!ow_list_value.empty()) {
      boost::split(owList, ow_list_value, boost::is_any_of("|"));
      for (UI_32 i = 0; i < owList.size(); i++) {
        std::sprintf(key, "%s.%s", f_launchOrderName.c_str(),  // NOLINT
                owList[i].c_str());
        SS_String grop_ow_time = l_pReaderOrderCfg->GetString(key);
        if (owList[i].compare(0, 4, "oww_") == 0) {
          char* endptr = NULL;
          SS_String group_name = owList[i].substr(strlen("oww_"));
          UI_32 group_launch_wait = static_cast<UI_32>(strtoul(
                  grop_ow_time.c_str(), &endptr, 10));
          if (*endptr != '\0') {
            continue;
          }
          GroupLaunchMapIter grp_iter = groups_map.begin();
          while (grp_iter != groups_map.end()) {
            if (group_name == grp_iter->second.name) {
              grp_iter->second.grp_launch_wait = group_launch_wait;
              FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__,
                  "grp_launch_wait %s=%d", group_name.c_str(),
                  group_launch_wait);
              break;
            }
            grp_iter++;
          }
        }
      }
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "ConfigReader(%s) is NULL",
            l_OrderCfgFn.c_str());
  }
// LCOV_EXCL_STOP
  ERROR: if (l_pReaderOrderCfg != NULL) {  // LCOV_EXCL_BR_LINE 200:cannot be null
    delete l_pReaderOrderCfg;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  }
  return;
}

// Given a string, splits it up on the seperator character
// SubStringIterator( "a|b|| c |", '|' )
//   -> On each subsequent invocation of next() would return "a", "b", "", " c ", ""
class SubStringIterator {
  typedef std::string::const_iterator SCIter;

 public:
  SubStringIterator(const std::string& s, char c) :
      m_position(s.begin()), m_end(s.end()), m_seperator(c), m_done(false) {
  }

  std::string next() noexcept {
    SCIter begin = m_position;
    while (m_position != m_end &&
            *m_position != m_seperator) {
      ++m_position;
    }

    const std::string ret(begin, m_position);
    if (m_position == m_end) {
      m_done = true;
    } else {
      ++m_position;
    }

    return ret;
  }
  bool done() const {
    return m_done;
  }

 private:
  SCIter m_position;
  const SCIter m_end;
  const char m_seperator;
  bool m_done;
};

template<class Iter>

Iter skipPrecedingWS(Iter begin, Iter end) noexcept {  // LCOV_EXCL_START 6: Because the condition cannot be set
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  return std::find_if(begin, end,
      std::not1(std::ptr_fun((int (*)(int))std::isspace)));
}

// remove only preceding and trailing whitespace, but leave intermediate whitespace characters alone
std::string strip(const std::string& str) noexcept {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  std::string::const_iterator skipTo = skipPrecedingWS(str.begin(), str.end());
  if (skipTo == str.end()) {  // if string consists of ONLY whitespace
    return std::string("");
  } else {
    return std::string(skipTo, skipPrecedingWS(str.rbegin(), str.rend()).base());
  }
}
// LCOV_EXCL_STOP

BOOL SysMgrConfiguration::LoadParametersCfg(GroupLaunchMap &groups_map,
        ProcessNameMap &f_processNameMap, LaunchOrderedVector &f_OrderList,
        SS_String &f_launchOrderName, SS_String &f_launchCfgFn) noexcept {
  BOOL rtn_code = TRUE;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  l_pReaderCfg = new (std::nothrow) CNSConfigReader(f_launchCfgFn);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  if (NULL != l_pReaderCfg) {  // LCOV_EXCL_BR_LINE 5:new error
    UI_32 launch_idx = 1;
    CHAR key[128];
    std::sprintf(key, "ModulesLaunchConfig.Launch%d", launch_idx);  // NOLINT
    SS_String value = l_pReaderCfg->GetString(key);

    if (value.empty()) {  // LCOV_EXCL_BR_LINE 200:cannot be empty
// LCOV_EXCL_START 6: For setting the initialization conditions at SystemManager startup
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
          " Error: l_pReaderCfg->GetString(%s) returned empty-string; "
                  "is launch configuration file mis-configured ?",
          key);
      rtn_code = FALSE;
// LCOV_EXCL_STOP
    } else {
      const char* nfsenv = getenv("AGL_NFS");
      bool isNfs = (nfsenv && strcmp(nfsenv, "y") == 0) ? true : false;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

      std::list<std::string> capFiles;
      if (isNfs) {
// LCOV_EXCL_START 6: Because an initialization condition is set at SystemManager startup and the branching condition cannot be satisfied
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        const std::string capPath("/usr/debug/share/target/cap.lst");
        std::ifstream fin(capPath.c_str());
        std::string line;
        while (fin && std::getline(fin, line)) {
          std::list<std::string> strList;
          boost::split(strList, line, boost::is_any_of("|"));
          if (!strList.empty()) {
            if (strList.front()[0] == '/') {  // Only character strings beginning with '/' are considered PATH
              capFiles.push_back(strList.front().c_str());
            }
          }
        }
      }
// LCOV_EXCL_STOP
      while (!value.empty()) {
        CHAR critical[10] = { };
        CHAR is_start_required[10] = { };
        CHAR shutdown_critical[10] = { };
        CHAR group_wait_for_trigger[10] = { };
        CfgLaunchParams cfg_data;

        int at = static_cast<int>(ss_sm_cfg::cfg_args::group_name);

        SubStringIterator iter(value, '|');
        while (!iter.done() && ss_sm_cfg::cfg_args::END != at) {
          const std::string data_ = iter.next();
          if (!data_.empty()) {
            const char* data = data_.c_str();
            switch (at) {
            case ss_sm_cfg::cfg_args::group_name:
              snprintf(cfg_data.group_name,
                      sizeof(cfg_data.group_name), "%s", data);
              break;
            case ss_sm_cfg::cfg_args::group_id:
              cfg_data.group_id = static_cast<UI_32>(strtoul(data, NULL, 10));
              break;
            case ss_sm_cfg::cfg_args::group_launch_wait:
              cfg_data.group_launch_wait =
                  static_cast<UI_32>(strtoul(data, NULL, 10));
              break;
            case ss_sm_cfg::cfg_args::group_wait_for_trigger:
              snprintf(group_wait_for_trigger,
                  sizeof(group_wait_for_trigger), "%s", data);
              break;
            case ss_sm_cfg::cfg_args::name:
              snprintf(cfg_data.name, sizeof(cfg_data.name), "%s", data);
              break;
            case ss_sm_cfg::cfg_args::binary_name:
              snprintf(cfg_data.binary_name,
                  sizeof(cfg_data.binary_name), "%s", data);
              break;
            case ss_sm_cfg::cfg_args::priority:
              cfg_data.priority = static_cast<UI_32>(strtoul(data, NULL, 10));
              break;
            case ss_sm_cfg::cfg_args::critical:
              snprintf(critical, sizeof(critical), "%s", data);
              break;
            case ss_sm_cfg::cfg_args::retry_cnt:
              cfg_data.retry_cnt = static_cast<UI_32>(strtoul(data, NULL, 10));
              break;
            case ss_sm_cfg::cfg_args::arguments:
              strncpy(cfg_data.arguments, data,
                      sizeof(cfg_data.arguments) - 1);
              break;
            case ss_sm_cfg::cfg_args::logging_msk:
              snprintf(cfg_data.logging_msk_str,
                      sizeof(cfg_data.logging_msk_str), "%s", data);
              break;
            case ss_sm_cfg::cfg_args::restart:
              snprintf(cfg_data.restart, sizeof(cfg_data.restart), "%s", data);
              break;
            case ss_sm_cfg::cfg_args::is_start_required:
              snprintf(is_start_required,
                  sizeof(is_start_required), "%s", data);
              break;
            case ss_sm_cfg::cfg_args::shutdown_critical:
              snprintf(shutdown_critical,
                  sizeof(shutdown_critical), "%s", data);
              break;
            case ss_sm_cfg::cfg_args::shutdown_wait_time:
              cfg_data.shutdown_wait_time =
                  static_cast<UI_32>(strtoul(data, NULL, 10));
              break;
            case ss_sm_cfg::cfg_args::fast_shutdown_wait_time:
              cfg_data.fast_shutdown_wait_time =
                  static_cast<UI_32>(strtoul(data, NULL, 10));
              break;
            case ss_sm_cfg::cfg_args::unix_user_name:
              strncpy(&cfg_data.unix_user_name[0],
                  strip(data_).c_str(),
                  sizeof(cfg_data.unix_user_name) - 1);
              break;
            case ss_sm_cfg::cfg_args::env_value_condition:  // LCOV_EXCL_START 6: Because an initialization condition is set at SystemManager startup and the branching condition cannot be satisfied // NOLINT(whitespace/line_length)
              AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
              snprintf(cfg_data.env_value_condition,
                  sizeof(cfg_data.env_value_condition), "%s", data);
              break;
            default:
              AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
              FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                  " Error: unknown ss_sm_cfg::cfg_args::xxx/'at' "
                          "enum value '%d'", at);
              break;
            // LCOV_EXCL_STOP
            }
          }
          ++at;
        }
        ////////////////////////////////////////////////////////////////////////////////
// LCOV_EXCL_BR_START 6: Because an initialization condition is set at SystemManager startup and the branching condition cannot be satisfied
        if (0 != std::strlen(cfg_data.env_value_condition)
              && FALSE
                    == is_satisfy_env_launch_cond(
// LCOV_EXCL_BR_STOP
          cfg_data.env_value_condition)) {  // LCOV_EXCL_LINE 6: For setting the initialization conditions at SystemManager startup
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
              "exclude %s from wakeup service", cfg_data.name);
        } else {
          if (isNfs) {  // LCOV_EXCL_BR_LINE 6: Because an initialization condition is set at SystemManager startup and the branching condition cannot be satisfied
            // Since the NFS environment is used by copying the executable file containing the CAP to /tmp, it is read out.
// LCOV_EXCL_START 6: Because an initialization condition is set at SystemManager startup and the branching condition cannot be satisfied
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            std::string binPath(cfg_data.binary_name);
            for (std::list<std::string>::iterator ite =
                  capFiles.begin(); ite != capFiles.end(); ite++) {
              if (binPath == *ite) {
                std::list<std::string> nodes;
                boost::split(nodes, binPath, boost::is_any_of("/"));
                std::string newPath("/tmp/");
                newPath += nodes.back();
                FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "EXCHG %s", newPath.c_str());
                snprintf(cfg_data.binary_name,
                    sizeof(cfg_data.binary_name), "%s", newPath.c_str());
                break;
              }
            }
          }
// LCOV_EXCL_STOP
          // FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "is_start_required: %s", is_start_required);
          cfg_data.is_start_required = (std::strcmp("True",
                  is_start_required) == 0);

          if (std::strcmp("True", group_wait_for_trigger) == 0) {
              cfg_data.group_wait_for_trigger = TRUE;
          } else {
              cfg_data.group_wait_for_trigger = FALSE;
          }

          // FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "critical: %s", critical);
          cfg_data.critical = (std::strcmp("True", critical) == 0);

          // FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "shutdown_critical: %s", shutdown_critical);
          cfg_data.shutdown_critical = (std::strcmp("True",
                  shutdown_critical) == 0);

          GroupLaunchInfo grp_info;
          grp_info.start_complete = FALSE;
          grp_info.stop_complete = FALSE;
          grp_info.launch_complete = FALSE;
          grp_info.name = cfg_data.group_name;
          grp_info.id = cfg_data.group_id;
          grp_info.grp_launch_wait = cfg_data.group_launch_wait;
          grp_info.grp_wait_for_trigger =
                  cfg_data.group_wait_for_trigger;
          ModuleLaunchListIter l_moduleLaunchListIter;

          GroupLaunchMapIter grp_iter = groups_map.find(grp_info.id);
          if (grp_iter != groups_map.end()) {
            l_moduleLaunchListIter = set_cfg_grp_member_info(
                    cfg_data, grp_iter->second.modules);
          } else {
            l_moduleLaunchListIter = set_cfg_grp_member_info(
                    cfg_data, grp_info.modules);

            // add this grp info object to the list of grps
            groups_map.insert(make_pair(grp_info.id, grp_info));
          }

          std::string l_binaryNameStr = cfg_data.binary_name;
          size_t l_binaryNamePos = l_binaryNameStr.find_last_of("/");
          if (std::string::npos != l_binaryNamePos) {
            l_binaryNameStr = l_binaryNameStr.substr(l_binaryNamePos + 1);
            f_processNameMap.insert(
                make_pair(l_binaryNameStr, l_moduleLaunchListIter));
          }
        }

        std::sprintf(key, "ModulesLaunchConfig.Launch%d", ++launch_idx);  // NOLINT
        value = l_pReaderCfg->GetString(key);
      }  // end while
    }
  } else {  // LCOV_EXCL_START 6: Because new doesn't pass the failure case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    // defaults use from the header file ... this is much faster then reading from an xml file.
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__,
        " Warning: \"new CNSConfigReader(%s)\" returned NULL; using "
                "constants.", f_launchCfgFn.c_str());
    UI_32 num_of_items = static_cast<UI_32>(_countof(g_arrLaunchTableCfg));
    for (UI_32 arr_idx = 0; arr_idx < num_of_items; arr_idx++) {
      GroupLaunchInfo grp_info;
      grp_info.launch_complete = FALSE;
      grp_info.name = g_arrLaunchTableCfg[arr_idx].group_name;
      grp_info.id = g_arrLaunchTableCfg[arr_idx].group_id;
      grp_info.grp_launch_wait =
              g_arrLaunchTableCfg[arr_idx].group_launch_wait;

      GroupLaunchMapIter grp_iter = groups_map.find(grp_info.id);

      if (grp_iter != groups_map.end()) {
        set_grp_member_info(g_arrLaunchTableCfg[arr_idx],
                grp_iter->second.modules);
      } else {
        set_grp_member_info(g_arrLaunchTableCfg[arr_idx],
                grp_info.modules);
        // add this grp info object to the list of grps
        groups_map.insert(make_pair(grp_info.id, grp_info));
      }
    }
  }
  // LCOV_EXCL_STOP
  if (rtn_code == TRUE) {
    load_parameters_order_cfg(groups_map, f_OrderList, f_launchOrderName, f_launchCfgFn);
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return rtn_code;
}

VOID SysMgrConfiguration::PrintGroupInfo(GroupLaunchInfo & refGrpInfo) noexcept {
  std::stringstream l_logMsg;

  l_logMsg << endl << "Group: " << refGrpInfo.name.data() << endl
      << "    id: " << refGrpInfo.id << endl << "    wait-time: "
      << refGrpInfo.grp_launch_wait << endl << "    launch complete: "
      << (refGrpInfo.launch_complete == TRUE ? "YES" : "NO");  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  std::string l_logStr = l_logMsg.str();
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "%s", l_logStr.c_str());
}

VOID SysMgrConfiguration::PrintModuleInfo(ModuleLaunchParams & refMbrInfo) noexcept {
  std::stringstream l_logMsg;

  // LCOV_EXCL_BR_START 11:unexpected branch  // NOLINT(whitespace/line_length)
  l_logMsg << endl << "Module: " << refMbrInfo.name.data() << endl
      << "    path: " << refMbrInfo.path.data() << endl << "    args: "
      << refMbrInfo.arguments.data() << endl << "    restart: "
      << refMbrInfo.restart.data() << endl
      << "    configuration_priority: "
      << refMbrInfo.configuration_priority << endl
      << "    current_priority: " << refMbrInfo.current_priority << endl
      << "    previous_priority: " << refMbrInfo.previous_priority << endl
      << "    critical: " << (refMbrInfo.critical == TRUE ? "YES" : "NO")
      << endl
      << "    retry count: " << refMbrInfo.retry_cnt << endl
      << "    logging mask: " << refMbrInfo.logging_msk_str << endl
      << "    is_start_required: "
      << (refMbrInfo.is_start_required == TRUE ? "YES" : "NO")
      << endl
      << "    shutdown_critical: "
      << (refMbrInfo.shutdown_critical == TRUE ? "YES" : "NO") << endl
      << "    shutdown_wait_time: " << refMbrInfo.shutdown_wait_time
      << endl << "    fast_shutdown_wait_time: "
      << refMbrInfo.fast_shutdown_wait_time << endl << "    state: "
      << refMbrInfo.ModuleStateStr() << endl << "    group ID: "
      << refMbrInfo.group_id << endl << "    debug dump state: "
      << refMbrInfo.GetModuleDebugDumpStateStr();
  // LCOV_EXCL_BR_STOP
  std::string l_logStr = l_logMsg.str();
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "%s", l_logStr.c_str());
}

VOID SysMgrConfiguration::PrintAllInfo(GroupLaunchMap & refGrpMap) noexcept {
  // get first client from the map...
  GroupLaunchMapIter grp_iter = refGrpMap.begin();

  for (; grp_iter != refGrpMap.end(); grp_iter++) {
    PrintGroupInfo(grp_iter->second);
    ModuleLaunchListIter mbr_iter = grp_iter->second.modules.begin();
    for (; mbr_iter != grp_iter->second.modules.end(); mbr_iter++) {
      PrintModuleInfo(*mbr_iter);
    }
  }
}

/*
 * SM Configuration data constructor
 */
SMConfigParams::SMConfigParams() noexcept {
}

/*
 * SM Configuration data destructor
 */
SMConfigParams::~SMConfigParams() {
}

////////////////////////////////////////////////////////////////////////////////////////////
///  PrintConfigInfo
///  \brief Print the configuration data stored in config structure
///
/// \param [in] configata
///         ConfigurationData & - Ref to structure that get populated.
///
/// \return VOID
////////////////////////////////////////////////////////////////////////////////////////////
VOID SMConfigParams::PrintConfigInfo(ConfigurationData const & f_ConfigParams) noexcept {
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "AppHeartBeatInterval_Initial Info: %d",
      f_ConfigParams.HBConfig.ApplicationHeartBeatIntervalInitial);
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "AppHeartBeatInterval_Repeat Info: %d",
      f_ConfigParams.HBConfig.ApplicationHeartBeatIntervalRepeat);
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "HBMaxHeartBeatRetryCount Info: %d",
      f_ConfigParams.HBConfig.MaxHeartBeatRetryCount);
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "CDEnabled: %d",
      f_ConfigParams.CDConfig.CrashDetectorEnabled);
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "SLMTimerValue: %d",
      f_ConfigParams.SLMConfig.SLMTimerValue);
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "SLMMaxRetryCount: %d",
      f_ConfigParams.SLMConfig.SLMMaxRetryCount);
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "SLMThresholdValue: %d",
      f_ConfigParams.SLMConfig.SLMThresholdValue);
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "SLMSystemmanagerLogIntervalMs: %d",
      f_ConfigParams.SLMConfig.SLMSystemmanagerLogIntervalMs);
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "UserModeStructIsBoolean: %s",
      GetStr(f_ConfigParams.UMConfig.IsUserModeNotificationABOOL).c_str());
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "ModuleConnectionNumTimesToCheck: %d",
      f_ConfigParams.MCConfig.ModuleConnectionNumTimesToCheck);
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "ModuleConnectionTimeOutSec: %d",
      f_ConfigParams.MCConfig.ModuleConnectionTimeOutSec);
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "ModuleStartRespTimeOutSec: %d",
      f_ConfigParams.MCConfig.ModuleStartRespTimeOutSec);
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "CriticalAppsMaxShutdownTimeFastSleep: %d",
      f_ConfigParams.CAMSTConfig.CriticalAppsMaxShutdownTimeFastSleep);
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
      "CriticalAppsMaxShutdownTimeNormalReset: %d",
      f_ConfigParams.CAMSTConfig.CriticalAppsMaxShutdownTimeNormalReset);
  return;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// GetSMConfigInformation
/// \brief Read config data from cfg file and populate the configuration data structure
///
/// \param [in] f_ConfigParams & - Ref to ConfigurationData that get populated.
/// \param [in] f_FileName       - Configuration file name.
///
/// \return BOOL
///         BOOL - TRUE or FALSE
////////////////////////////////////////////////////////////////////////////////////////////
BOOL SMConfigParams::GetSMConfigInformation(ConfigurationData &f_ConfigParams,
        SS_String f_FileName) noexcept {
  EFrameworkunifiedStatus l_eStatus;
  BOOL l_rtnCode = FALSE;  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)

  CNSConfigReader *l_pReaderCfg = new (std::nothrow) CNSConfigReader();  // LCOV_EXCL_BR_LINE 10: As new does not fail
// LCOV_EXCL_BR_START 6: Because the applicable variable cannot be changed from the external API (only evaluated by the initial value)
  if (NULL == l_pReaderCfg) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        " Error. new CNSConfigReader() returned NULL pointer.");
  } else {
    if (eFrameworkunifiedStatusOK != (l_eStatus = l_pReaderCfg->Parse(f_FileName))) {
      // LCOV_EXCL_START 6: Because the applicable variable cannot be changed from the external API (only evaluated by the initial value)
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
              " Error. l_pReaderCfg->Open(%s) returned NULL pointer.",
              f_FileName.c_str());
      // LCOV_EXCL_STOP
    } else if (eFrameworkunifiedStatusOK
          != (l_eStatus = l_pReaderCfg->GetInt(
              "Heartbeat.MaxHeartBeatRetryCount",
              f_ConfigParams.HBConfig.MaxHeartBeatRetryCount))) {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      LOG_STATUS_IF_ERRORED(l_eStatus,  // LCOV_EXCL_START 6: Because the applicable variable cannot be changed from the external API (only evaluated by the initial value)
          "l_pReaderCfg->GetInt('Heartbeat.MaxHeartBeatRetryCount')");
    } else if (eFrameworkunifiedStatusOK
          != (l_eStatus = l_pReaderCfg->GetInt(
              "Heartbeat.AppHeartBeatIntervalInitial",
              f_ConfigParams.HBConfig.ApplicationHeartBeatIntervalInitial))) {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      LOG_STATUS_IF_ERRORED(l_eStatus,  // LCOV_EXCL_LINE 6: Because the applicable variable cannot be changed from the external API (only evaluated by the initial value)
          "l_pReaderCfg->GetInt('Heartbeat.AppHeartBeatIntervalInitial')");
    } else if (eFrameworkunifiedStatusOK
          != (l_eStatus = l_pReaderCfg->GetInt(
              "Heartbeat.AppHeartBeatIntervalRepeat",
              f_ConfigParams.HBConfig.ApplicationHeartBeatIntervalRepeat))) {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      LOG_STATUS_IF_ERRORED(l_eStatus,  // LCOV_EXCL_LINE 6: Because the applicable variable cannot be changed from the external API (only evaluated by the initial value)
          "l_pReaderCfg->GetInt('Heartbeat.AppHeartBeatIntervalRepeat')");
    } else if (eFrameworkunifiedStatusOK
          != (l_eStatus = l_pReaderCfg->GetBool("CrashDetector.Enabled",
              f_ConfigParams.CDConfig.CrashDetectorEnabled))) {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      LOG_STATUS_IF_ERRORED(l_eStatus,  // LCOV_EXCL_LINE 6: Because the applicable variable cannot be changed from the external API (only evaluated by the initial value)
          "l_pReaderCfg->GetBool('CrashDetector.Enabled')");
    } else if (eFrameworkunifiedStatusOK
          != (l_eStatus = l_pReaderCfg->GetInt("SysLowMemory.TimerValue",
              f_ConfigParams.SLMConfig.SLMTimerValue))) {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      LOG_STATUS_IF_ERRORED(l_eStatus,  // LCOV_EXCL_LINE 6: Because the applicable variable cannot be changed from the external API (only evaluated by the initial value)
          "l_pReaderCfg->GetInt('SysLowMemory.TimerValue')");
    } else if (eFrameworkunifiedStatusOK
          != (l_eStatus = l_pReaderCfg->GetInt(
              "SysLowMemory.MaxRetryCount",
              f_ConfigParams.SLMConfig.SLMMaxRetryCount))) {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      LOG_STATUS_IF_ERRORED(l_eStatus,  // LCOV_EXCL_LINE 6: Because the applicable variable cannot be changed from the external API (only evaluated by the initial value)
          "l_pReaderCfg->GetInt('SysLowMemory.MaxRetryCount')");
    } else if (eFrameworkunifiedStatusOK
          != (l_eStatus = l_pReaderCfg->GetInt(
              "SysLowMemory.ThresholdValue",
              f_ConfigParams.SLMConfig.SLMThresholdValue))) {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      LOG_STATUS_IF_ERRORED(l_eStatus,  // LCOV_EXCL_LINE 6: Because the applicable variable cannot be changed from the external API (only evaluated by the initial value)
          "l_pReaderCfg->GetInt('SysLowMemory.ThresholdValue')");
    } else if (eFrameworkunifiedStatusOK
          != (l_eStatus = l_pReaderCfg->GetInt(
              "SysLowMemory.InterfaceunifiedLogIntervalMs",
              f_ConfigParams.SLMConfig.SLMSystemmanagerLogIntervalMs))) {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      LOG_STATUS_IF_ERRORED(l_eStatus,  // LCOV_EXCL_LINE 6: Because the applicable variable cannot be changed from the external API (only evaluated by the initial value)
          "l_pReaderCfg->GetInt('SysLowMemory.InterfaceunifiedLogIntervalMs')");
    } else if (eFrameworkunifiedStatusOK
          != (l_eStatus = l_pReaderCfg->GetBool(
              "UserModeNotification.IsBoolean",
              f_ConfigParams.UMConfig.IsUserModeNotificationABOOL))) {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      LOG_STATUS_IF_ERRORED(l_eStatus,  // LCOV_EXCL_LINE 6: Because the applicable variable cannot be changed from the external API (only evaluated by the initial value)
          "l_pReaderCfg->GetBool('UserModeNotification.IsBoolean')");
      f_ConfigParams.UMConfig.IsUserModeNotificationABOOL = TRUE;
    } else if (eFrameworkunifiedStatusOK
          != (l_eStatus = l_pReaderCfg->GetInt(
              "Module.ModuleConnectionNumTimesToCheck",
              f_ConfigParams.MCConfig.ModuleConnectionNumTimesToCheck))) {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      LOG_STATUS_IF_ERRORED(l_eStatus,  // LCOV_EXCL_LINE 6: Because the applicable variable cannot be changed from the external API (only evaluated by the initial value)
          "l_pReaderCfg->GetInt('Module.ModuleConnectionNumTimesToCheck')");
    } else if (eFrameworkunifiedStatusOK
          != (l_eStatus = l_pReaderCfg->GetInt(
              "Module.ModuleConnectionTimeOutSec",
              f_ConfigParams.MCConfig.ModuleConnectionTimeOutSec))) {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      LOG_STATUS_IF_ERRORED(l_eStatus,  // LCOV_EXCL_LINE 6: Because the applicable variable cannot be changed from the external API (only evaluated by the initial value)
          "l_pReaderCfg->GetInt('Module.ModuleConnectionTimeOutSec')");
    } else if (eFrameworkunifiedStatusOK
          != (l_eStatus = l_pReaderCfg->GetInt(
              "Module.ModuleStartRespTimeOutSec",
              f_ConfigParams.MCConfig.ModuleStartRespTimeOutSec))) {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      LOG_STATUS_IF_ERRORED(l_eStatus,  // LCOV_EXCL_LINE 6: Because the applicable variable cannot be changed from the external API (only evaluated by the initial value)
          "l_pReaderCfg->GetInt('Module.ModuleStartRespTimeOutSec')");
    } else if (eFrameworkunifiedStatusOK
          != (l_eStatus = l_pReaderCfg->GetInt(
              "CriticalApps.CriticalAppsMaxShutdownTimeFastSleep",
              f_ConfigParams.CAMSTConfig.CriticalAppsMaxShutdownTimeFastSleep))) {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      LOG_STATUS_IF_ERRORED(l_eStatus,  // LCOV_EXCL_LINE 6: Because the applicable variable cannot be changed from the external API (only evaluated by the initial value)
          "l_pReaderCfg->GetInt('CriticalApps.CriticalAppsMaxShutdownTimeFastSleep')");
    } else if (eFrameworkunifiedStatusOK
          != (l_eStatus = l_pReaderCfg->GetInt(
              "CriticalApps.CriticalAppsMaxShutdownTimeNormalReset",
              f_ConfigParams.CAMSTConfig.CriticalAppsMaxShutdownTimeNormalReset))) {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      LOG_STATUS_IF_ERRORED(l_eStatus,  // LCOV_EXCL_LINE 6: Because the applicable variable cannot be changed from the external API (only evaluated by the initial value)
          "l_pReaderCfg->GetInt('CriticalApps.CriticalAppsMaxShutdownTimeNormalReset')");
    } else {
      l_rtnCode = TRUE;
    }

    delete l_pReaderCfg;  // LCOV_EXCL_BR_LINE 10: As the delete does not fail
  }
// LCOV_EXCL_BR_STOP

  return (l_rtnCode);
}

////////////////////////////////////////////////////////////////////////////////////////////
/// LoadSMConfigParameters
/// \brief Read System Manager configuration parameters from cfg file or
///        use defaults values in case cfg file is not available
///
/// \param [in] f_ConfigParams & - Ref to ConfigurationData that get populated.
/// \param [in] f_FileName       - Configuration file name.
///
/// \return BOOL
///         BOOL - TRUE or FALSE
////////////////////////////////////////////////////////////////////////////////////////////
BOOL SMConfigParams::LoadSMConfigParameters(ConfigurationData &f_ConfigParams,
        SS_String f_FileName) noexcept {
  BOOL rtn_code = TRUE;

  rtn_code = GetSMConfigInformation(f_ConfigParams, f_FileName);

  // cfg config file not found, use constants
  if (FALSE == rtn_code) {
    // LCOV_EXCL_START 6: Because the applicable variable cannot be changed from the external API (only evaluated by the initial value)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__,
        " Warning: cfg config file not open, using constants");

    // Heart Beat constants
    f_ConfigParams.HBConfig.ApplicationHeartBeatIntervalInitial =
        HBApplicationHeartBeatIntervalInitial;
    f_ConfigParams.HBConfig.ApplicationHeartBeatIntervalRepeat =
        HBApplicationHeartBeatIntervalRepeat;
    f_ConfigParams.HBConfig.MaxHeartBeatRetryCount = HBMaxHeartBeatRetryCount;

    // Crash Detector constants
    f_ConfigParams.CDConfig.CrashDetectorEnabled = CDEnabled;

    // System Low Memory
    f_ConfigParams.SLMConfig.SLMTimerValue = SLMTimerValue;
    f_ConfigParams.SLMConfig.SLMMaxRetryCount = SLMRetryCount;
    f_ConfigParams.SLMConfig.SLMThresholdValue = SLMThresholdValue;
    f_ConfigParams.SLMConfig.SLMSystemmanagerLogIntervalMs = SLMSystemmanagerLogIntervalMs;

    // Module connect and start resp
    f_ConfigParams.MCConfig.ModuleConnectionNumTimesToCheck =
        ModuleConnectionNumTimesToCheck;
    f_ConfigParams.MCConfig.ModuleConnectionTimeOutSec =
        ModuleConnectionTimeOutSec;
    f_ConfigParams.MCConfig.ModuleStartRespTimeOutSec = ModuleStartRespTimeOutSec;

    // Critical Apps shutdown time
    f_ConfigParams.CAMSTConfig.CriticalAppsMaxShutdownTimeFastSleep =
        CriticalAppsMaxShutdownTimeFastSleepSec;
    f_ConfigParams.CAMSTConfig.CriticalAppsMaxShutdownTimeNormalReset =
        CriticalAppsMaxShutdownTimeNormalResetSec;

    // USer Mode Data Structure Type
    f_ConfigParams.UMConfig.IsUserModeNotificationABOOL = TRUE;
    // LCOV_EXCL_STOP
  }
  PrintConfigInfo(f_ConfigParams);

  return rtn_code;
}  // LCOV_EXCL_BR_LINE 10: Final line

