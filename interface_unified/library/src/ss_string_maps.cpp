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
/// \ingroup  tag_SystemManagerIf
/// \brief    This file provides support for the System Manager client interface.
///
///////////////////////////////////////////////////////////////////////////////

#include "system_service/ss_string_maps.h"
#include <native_service/frameworkunified_types.h>
#include <native_service/ns_logger_if.h>
#include <map>
#include "system_service/ss_power_service.h"
#include "system_service/ss_templates.h"
#include "system_service/ss_system_manager_if.h"
#include "system_service/ss_system_manager_protocol.h"
#include "ss_system_if_interfaceunifiedlog.h"

/*
 * 't'emplate for string-map initialization functions
void Init_SS_xyz_StrMap(std::map<SS_xyz, SS_String> &m_strMap)
{
   MAP_ENTRY( m_strMap,  );
   MAP_ENTRY( m_strMap,  );
} // End of void Init_SS_xyz_StrMap(std::map<SS_xyz, SS_String> &m_strMap)
*/


//******************************************************************************
void Init_eSMBootMicroResetReason_StrMap(std::map<eSMBootMicroResetReason,
                                         SS_String> & m_strMap) {  // NOLINT (runtime/references)
  MAP_ENTRY(m_strMap, SS_SM_BOOT_MICRO_RESET_REASON_SELF_RESET);
  MAP_ENTRY(m_strMap, SS_SM_BOOT_MICRO_RESET_REASON_USER_FORCE_RESET);
  MAP_ENTRY(m_strMap, SS_SM_BOOT_MICRO_RESET_REASON_DSP_RESET);
  MAP_ENTRY(m_strMap, SS_SM_BOOT_MICRO_RESET_REASON_HB_TIMEOUT);
}
class EnumStringMap< eSMBootMicroResetReason,
                     Init_eSMBootMicroResetReason_StrMap>
                                                  m_oeSMBootMicroResetReasonMap;
// LCOV_EXCL_START 6:Because the condition cannot be set
SS_String GetStr(eSMBootMicroResetReason f_enum) {
  return m_oeSMBootMicroResetReasonMap.GetStr(f_enum);
}
// LCOV_EXCL_STOP

//******************************************************************************
void Init_EInterfaceunifiedSystemError_StrMap(std::map<EFrameworkunifiedSystemError, SS_String> & m_strMap) {  // NOLINT (runtime/references)
  MAP_ENTRY(m_strMap, eFrameworkunifiedSystemErrorNone);
  MAP_ENTRY(m_strMap, eFrameworkunifiedDSPHardwareReset);
}
class EnumStringMap< EFrameworkunifiedSystemError, Init_EInterfaceunifiedSystemError_StrMap> m_oEInterfaceunifiedSystemErrorMap;
// LCOV_EXCL_START 6:Because the condition cannot be set
SS_String GetStr(EFrameworkunifiedSystemError f_enum) {
  return m_oEInterfaceunifiedSystemErrorMap.GetStr(f_enum);
}
// LCOV_EXCL_STOP

//******************************************************************************
void Init_EErrorEventType_StrMap(std::map<EErrorEventType, SS_String> & m_strMap);  // NOLINT (runtime/references)
class EnumStringMap< EErrorEventType, Init_EErrorEventType_StrMap> m_oEErrorEventTypeMap;
SS_String GetStr(EErrorEventType f_enum) {
  return m_oEErrorEventTypeMap.GetStr(f_enum);
}

//******************************************************************************
void Init_BOOL_StrMap(std::map<BOOL, SS_String> & m_strMap) {  // NOLINT (runtime/references)
  MAP_ENTRY(m_strMap, TRUE);
  MAP_ENTRY(m_strMap, FALSE);
}  // End of void Init_BOOL_StrMap(std::map<BOOL, SS_String> & m_strMap)
class EnumStringMap< BOOL, Init_BOOL_StrMap> m_oBOOLMap;
SS_String GetStr(BOOL f_enum) {
  return m_oBOOLMap.GetStr(f_enum);
}

//******************************************************************************
void Init_SS_LimpHomeCutoff_StrMap(std::map<EPWR_LHC_TYPE, SS_String> & m_strMap) {  // NOLINT (runtime/references)
  MAP_ENTRY(m_strMap, epslhcINVALID);
  MAP_ENTRY(m_strMap, epslhcDISABLED);
  MAP_ENTRY(m_strMap, epslhcENABLED);
}  // End of void Init_SS_LimpHomeCutoff_StrMap(std::map<EPWR_LHC_TYPE, SS_String> & m_strMap)
class EnumStringMap< EPWR_LHC_TYPE, Init_SS_LimpHomeCutoff_StrMap> m_oSS_LimpHomeCutoffMap;
SS_String GetStr(EPWR_LHC_TYPE f_enum) {
  return m_oSS_LimpHomeCutoffMap.GetStr(f_enum);
}

//******************************************************************************
void Init_SS_ProductionMode_StrMap(std::map<EPWR_PROD_MODE_TYPE, SS_String> & m_strMap) {  // NOLINT (runtime/references)
  MAP_ENTRY(m_strMap, epspmINVALID);
  MAP_ENTRY(m_strMap, epspmDISABLED);
  MAP_ENTRY(m_strMap, epspmENABLED);
}  // End of void Init_SS_ProductionMode_StrMap(std::map<EPWR_PROD_MODE_TYPE, SS_String> & m_strMap)
class EnumStringMap< EPWR_PROD_MODE_TYPE, Init_SS_ProductionMode_StrMap>
                                                    m_oSS_ProductionModeMap;
SS_String GetStr(EPWR_PROD_MODE_TYPE f_enum) {
  return m_oSS_ProductionModeMap.GetStr(f_enum);
}

//******************************************************************************
void Init_SS_StartupStage_StrMap(std::map<EPWR_STARTUP_STAGE_TYPE, SS_String> & m_strMap) {  // NOLINT (runtime/references)
  MAP_ENTRY(m_strMap, epssusfINVALID);
  MAP_ENTRY(m_strMap, epssusSYSTEM_SERVICES_STARTED);
  MAP_ENTRY(m_strMap, epssusALL_SERVICES_LAUNCHED);
}  // End of void Init_SS_StartupStage_StrMap(std::map<EPWR_STARTUP_STAGE_TYPE, SS_String> & m_strMap)
class EnumStringMap< EPWR_STARTUP_STAGE_TYPE, Init_SS_StartupStage_StrMap>
                                                    m_oSS_StartupStageMap;
// LCOV_EXCL_START 6:Because the condition cannot be set
SS_String GetStr(EPWR_STARTUP_STAGE_TYPE f_enum) {
  return m_oSS_StartupStageMap.GetStr(f_enum);
}
// LCOV_EXCL_STOP

//******************************************************************************
void Init_SS_SystemMode_StrMap(std::map<EPWR_SYSTEM_MODE_INFO, SS_String> & m_strMap) {  // NOLINT (runtime/references)
  MAP_ENTRY(m_strMap, epssinfINVALID);
  MAP_ENTRY(m_strMap, epssinfNORMAL);
  MAP_ENTRY(m_strMap, epssinfPROGRAMMING);
}  // End of void Init_SS_SystemMode_StrMap(std::map<EPWR_SYSTEM_MODE_INFO, SS_String> & m_strMap)
class EnumStringMap< EPWR_SYSTEM_MODE_INFO, Init_SS_SystemMode_StrMap>
                                                    m_oSS_SystemModeMap;
// LCOV_EXCL_START 6:Because the condition cannot be set
SS_String GetStr(EPWR_SYSTEM_MODE_INFO f_enum) {
  return m_oSS_SystemModeMap.GetStr(f_enum);
}
// LCOV_EXCL_STOP

//******************************************************************************
void Init_SS_TransportMode_StrMap(std::map<EPWR_TRANSPORT_MODE_TYPE, SS_String> & m_strMap) {  // NOLINT (runtime/references)
  MAP_ENTRY(m_strMap, epstmINVALID);
  MAP_ENTRY(m_strMap, epstmDISABLED);
  MAP_ENTRY(m_strMap, epstmENABLED);
}  // End of void Init_SS_TransportMode_StrMap(std::map<EPWR_TRANSPORT_MODE_TYPE, SS_String> & m_strMap)
class EnumStringMap< EPWR_TRANSPORT_MODE_TYPE, Init_SS_TransportMode_StrMap>
                                                    m_oSS_TransportModeMap;
SS_String GetStr(EPWR_TRANSPORT_MODE_TYPE f_enum) {
  return m_oSS_TransportModeMap.GetStr(f_enum);
}

//******************************************************************************
void Init_EInterfaceunifiedStatus_StrMap(std::map<EFrameworkunifiedStatus, SS_String> & m_strMap);  // NOLINT (runtime/references)
class EnumStringMap< EFrameworkunifiedStatus, Init_EInterfaceunifiedStatus_StrMap> m_oEInterfaceunifiedStatusMap;
SS_String GetStr(EFrameworkunifiedStatus f_enum) {
  return m_oEInterfaceunifiedStatusMap.GetStr(f_enum);
}

//******************************************************************************
void Init_SS_SystemManagerProtocol_StrMap(std::map<SS_SystemManagerProtocol,
                                          SS_String> & m_strMap);  // NOLINT (runtime/references)
class EnumStringMap< SS_SystemManagerProtocol,
                     Init_SS_SystemManagerProtocol_StrMap> m_oSSSMProtocolMap;
SS_String GetStr(SS_SystemManagerProtocol f_enum) {
  return m_oSSSMProtocolMap.GetStr(f_enum);
}

//******************************************************************************
void Init_SS_ColdStart_StrMap(std::map<EPWR_SC_COLD_START_REQ_TYPE,
                                  SS_String> &m_strMap) {  // NOLINT (runtime/references)
  MAP_ENTRY(m_strMap, epsscrtINVALID);
  MAP_ENTRY(m_strMap, epsscrtNOT_REQUIRED);
  MAP_ENTRY(m_strMap, epsscrtREQUIRED);
}
class EnumStringMap< EPWR_SC_COLD_START_REQ_TYPE,
                     Init_SS_ColdStart_StrMap> m_oSSColdStart_Map;
// LCOV_EXCL_START 6:Because the condition cannot be set
SS_String GetStr(EPWR_SC_COLD_START_REQ_TYPE f_enum) {
  return m_oSSColdStart_Map.GetStr(f_enum);
}
// LCOV_EXCL_STOP

//******************************************************************************
void Init_SS__CWORD56__BootMode_StrMap(std::map<EPWR_SC__CWORD56__BOOT_MODE_TYPE, SS_String> &m_strMap) {  // NOLINT (runtime/references)
  MAP_ENTRY(m_strMap, eps_CWORD56_bmINVALID);
  MAP_ENTRY(m_strMap, eps_CWORD56_bmAPPLICATION_MODE);
  MAP_ENTRY(m_strMap, eps_CWORD56_bmPROGRAMMING_MODE);
}  // End of void Init_SS__CWORD56__BootMode_StrMap(std::map<EPWR_SC__CWORD56__BOOT_MODE_TYPE, SS_String> &m_strMap)
class EnumStringMap< EPWR_SC__CWORD56__BOOT_MODE_TYPE,
                     Init_SS__CWORD56__BootMode_StrMap> m_oSS_CWORD56_BootMode_Map;
SS_String GetStr(EPWR_SC__CWORD56__BOOT_MODE_TYPE f_enum) {
  return m_oSS_CWORD56_BootMode_Map.GetStr(f_enum);
}

//******************************************************************************
void Init_SS__CWORD102__BootMode_StrMap(std::map<ESMBootModeInfo, SS_String> &m_strMap) {  // NOLINT (runtime/references)
  MAP_ENTRY(m_strMap, e_SS_SM_BOOT_MODE_INVALID);
  MAP_ENTRY(m_strMap, e_SS_SM_BOOT_MODE_APPLICATION);
  MAP_ENTRY(m_strMap, e_SS_SM_BOOT_MODE_PROGRAMMING);
}  // End of void Init_SS__CWORD102__BootMode_StrMap(std::map<ESMBootModeInfo, SS_String> &m_strMap)

class EnumStringMap< ESMBootModeInfo,
                     Init_SS__CWORD102__BootMode_StrMap> m_oSS_CWORD102_BootMode_Map;
SS_String GetStr(ESMBootModeInfo f_enum) {
  return m_oSS_CWORD102_BootMode_Map.GetStr(f_enum);
}

//******************************************************************************
void Init_SS_ShutdownTrigger_StrMap(std::map<EPWR_SHUTDOWN_TRIGGER_TYPE,
                                           SS_String> & m_strMap);  // NOLINT (runtime/references)
class EnumStringMap< EPWR_SHUTDOWN_TRIGGER_TYPE,
                     Init_SS_ShutdownTrigger_StrMap> m_oSSShutdownTrigger_Map;
SS_String GetStr(EPWR_SHUTDOWN_TRIGGER_TYPE f_enum) {
  return m_oSSShutdownTrigger_Map.GetStr(f_enum);
}

//******************************************************************************
void Init_SS_UserMode_StrMap(std::map<EPWR_USER_MODE_TYPE, SS_String> & m_strMap) {  // NOLINT (runtime/references)
  // Map of System Services User Mode enum values to string description
  MAP_ENTRY(m_strMap, epsumINVALID);
  MAP_ENTRY(m_strMap, epsumOFF);
  MAP_ENTRY(m_strMap, epsumON);
}  // End of void Init_SS_UserMode_StrMap(std::map<EPWR_USER_MODE_TYPE, SS_String> & m_strMap)

class EnumStringMap< EPWR_USER_MODE_TYPE,
                     Init_SS_UserMode_StrMap> m_oSSUserMode_Map;
SS_String GetStr(EPWR_USER_MODE_TYPE f_enum) {
  return m_oSSUserMode_Map.GetStr(f_enum);
}

//******************************************************************************
void Init_SS_CpuResetReason_StrMap(std::map<ESMCpuResetReason, SS_String> &m_strMap) {  // NOLINT (runtime/references)
  MAP_ENTRY(m_strMap, e_SS_SM_CPU_RESET_REASON_NORMAL);
  MAP_ENTRY(m_strMap, e_SS_SM_CPU_RESET_REASON_DATA_RESET);
  MAP_ENTRY(m_strMap, e_SS_SM_CPU_RESET_REASON_USER_FORCE_RESET);
  MAP_ENTRY(m_strMap, e_SS_SM_CPU_RESET_REASON_GENERIC_ERR);
  MAP_ENTRY(m_strMap, e_SS_SM_CPU_RESET_REASON_DSP_ERR);
  MAP_ENTRY(m_strMap, e_SS_SM_CPU_RESET_REASON_CRITICAL_ERR);
}  // End of void Init_SS_CpuResetReason_StrMap(std::map<ESMCpuResetReason, SS_String> &m_strMap)
class EnumStringMap< ESMCpuResetReason,
                     Init_SS_CpuResetReason_StrMap> m_oSS_CpuResetReason_Map;
// LCOV_EXCL_START 6:Because the condition cannot be set
SS_String GetStr(ESMCpuResetReason f_enum) {
  return m_oSS_CpuResetReason_Map.GetStr(f_enum);
}
// LCOV_EXCL_STOP

//******************************************************************************
void Init_SS_UserModeChangeReason_StrMap(std::map<ePwrServiceUserModeChangeReasonType, SS_String> &m_strMap) {  // NOLINT (runtime/references)
  // LCOV_EXCL_BR_START 11:Excluded due to gcov restrictions(other)
  MAP_ENTRY(m_strMap, epsumcrNOT_AVAILABLE);
  MAP_ENTRY(m_strMap, epsumcrON_KEY);
  MAP_ENTRY(m_strMap, epsumcrPARKING_B);
  MAP_ENTRY(m_strMap, epsumcrPRE_BA);
  MAP_ENTRY(m_strMap, epsumcrNORMAL);
  MAP_ENTRY(m_strMap, epsumcrBACKGROUND_BA);
  // LCOV_EXCL_BR_STOP 11:Excluded due to gcov restrictions(other)
}
class EnumStringMap< EPWR_USER_MODE_CHANGE_REASON_TYPE,
                     Init_SS_UserModeChangeReason_StrMap> m_oSS_UMCR_Map;
SS_String GetStr(EPWR_USER_MODE_CHANGE_REASON_TYPE f_enum) {
  return m_oSS_UMCR_Map.GetStr(f_enum);
}

//******************************************************************************
void Init_SS_StartupReason_StrMap(std::map<EPWR_WAKEUP_FACTOR_TYPE,
                                        SS_String> & m_strMap);  // NOLINT (runtime/references)
class EnumStringMap< EPWR_WAKEUP_FACTOR_TYPE ,
                     Init_SS_StartupReason_StrMap> m_oSSStartupReason_Map;
SS_String GetStr(EPWR_WAKEUP_FACTOR_TYPE f_enum) {
  return m_oSSStartupReason_Map.GetStr(f_enum);
}

//******************************************************************************
void Init_EErrorEventType_StrMap(std::map<EErrorEventType, SS_String> & m_strMap) {  // NOLINT (runtime/references)
  MAP_ENTRY(m_strMap, eErrorEventTypeProcessCrash);
  MAP_ENTRY(m_strMap, eErrorEventTypeHeartBeatFailure);
  MAP_ENTRY(m_strMap, eErrorEventTypeSystemLowMemory);
  MAP_ENTRY(m_strMap, eErrorEventTypeReserved1);
  MAP_ENTRY(m_strMap, eErrorEventTypeUserInvokedUserForceReset);
  MAP_ENTRY(m_strMap, eErrorEventTypeReserved2);
  MAP_ENTRY(m_strMap, eErrorEventTypeReserved3);
  MAP_ENTRY(m_strMap, eErrorEventTypeUserInvokedCollectAllLogs);
  MAP_ENTRY(m_strMap, eErrorEventTypeBootMicroReset);
  MAP_ENTRY(m_strMap, eErrorEventTypeProcessExit);
  MAP_ENTRY(m_strMap, eErrorEventTypeUserInvokedCollectScreenShot);
  MAP_ENTRY(m_strMap, eErrorEventTypeUserInvokedCollectInterfaceunifiedLogs);
  MAP_ENTRY(m_strMap, eErrorEventTypeEelExport);
  MAP_ENTRY(m_strMap, eErrorEventTypeInterfaceunifiedEmmcLogs);
  MAP_ENTRY(m_strMap, eErrorEventTypeDiagEvent);
  MAP_ENTRY(m_strMap, eErrorEventTypeCanEvent);
  MAP_ENTRY(m_strMap, eErrorEventTypeDtcEvent);
  MAP_ENTRY(m_strMap, eErrorEventTypeModConnFailed);
  MAP_ENTRY(m_strMap, eErrorEventTypeStartRespFailed);
  MAP_ENTRY(m_strMap, eErrorEventTypeUserInvokedCollectDevLogs);
  MAP_ENTRY(m_strMap, eErrorEventTypeModuleInvokedResetRequest);
  MAP_ENTRY(m_strMap, eErrorEventTypeModuleInvokedCollectDebugLogs);
  MAP_ENTRY(m_strMap, eErrorEventTypeUserInvokedClearLogs);
  MAP_ENTRY(m_strMap, eErrorEventTypeGroupRelaunch);
  MAP_ENTRY(m_strMap, eErrorEventTypeMaxValue);
}  // End of void Init_EErrorEventType_StrMap(std::map<EErrorEventType, SS_String> & m_strMap)

void Init_SS_SystemManagerProtocol_StrMap(std::map<SS_SystemManagerProtocol, SS_String> &m_strMap) {  // NOLINT (runtime/references)
  // LCOV_EXCL_BR_START 11:Excluded due to gcov restrictions(other)
  MAP_ENTRY(m_strMap, SS_SM_NOTIFY_SYSTEM_LAUNCH_COMPLETE);
  MAP_ENTRY(m_strMap, SS_SM_PROTOCOL_INTERFACE_END);
  MAP_ENTRY(m_strMap, SS_SM_START);
  MAP_ENTRY(m_strMap, SS_SM_START_COMPL_RSPN);
  MAP_ENTRY(m_strMap, SS_SM_STOP);
  MAP_ENTRY(m_strMap, SS_SM_STOP_COMPL_RSPN);
  MAP_ENTRY(m_strMap, SS_SM_PRE_START);
  MAP_ENTRY(m_strMap, SS_SM_PRE_START_COMPL_RSPN);
  MAP_ENTRY(m_strMap, SS_SM_PRE_STOP);
  MAP_ENTRY(m_strMap, SS_SM_PRE_STOP_COMPL_RSPN);
  MAP_ENTRY(m_strMap, SS_SM_BACKGROUND_START);
  MAP_ENTRY(m_strMap, SS_SM_BACKGROUND_START_COMPL_RSPN);
  MAP_ENTRY(m_strMap, SS_SM_BACKGROUND_STOP);
  MAP_ENTRY(m_strMap, SS_SM_BACKGROUND_STOP_COMPL_RSPN);
  MAP_ENTRY(m_strMap, SS_SM_GET_START_EXT_INFO);
  MAP_ENTRY(m_strMap, SS_SM_GET_STOP_EXT_INFO);
  MAP_ENTRY(m_strMap, SS_SM_WAKEUP_MODULES_CMPL_RSPN);
  MAP_ENTRY(m_strMap, SS_SM_SHUTDOWN_MODULES_CMPL_RSPN);
  MAP_ENTRY(m_strMap, SS_SM_WAKEUP_MODULES);
  MAP_ENTRY(m_strMap, SS_SM_SHUTDOWN_MODULES);
  MAP_ENTRY(m_strMap, SS_SM_CRNT_STATE_QUERY);
  MAP_ENTRY(m_strMap, SS_SM_CRNT_STATE_QUERY_RSPN);
  MAP_ENTRY(m_strMap, SS_SM_SYSTEM_MODE_INFO_REQ);
  MAP_ENTRY(m_strMap, SS_SM_SYSTEM_MODE_INFO_RSPN);
  MAP_ENTRY(m_strMap, SS_SM_INITCOMP_REP);
  MAP_ENTRY(m_strMap, SS_SM_BOOT_MODE_SET_REQ);
  MAP_ENTRY(m_strMap, SS_SM_BOOT_MODE_SET_RESP);
  MAP_ENTRY(m_strMap, SS_SM_DATA_RESET_MODE_SET_REQ);
  MAP_ENTRY(m_strMap, SS_SM_PROG_UPDATE_STATE_SET_REQ);
  MAP_ENTRY(m_strMap, SS_SM_CPU_RESET_REQ);
  MAP_ENTRY(m_strMap, SS_SM_REMOTE_DATA_RESET_REQ);
  MAP_ENTRY(m_strMap, SS_SM_FWD_STARTUP_CONFIRMATION_MSG_REQ);
  MAP_ENTRY(m_strMap, SS_SM_FWD_START_CONFIRMATION_MSG_RESP);
  MAP_ENTRY(m_strMap, SS_SM_POWER_REQUEST_MSG);
  MAP_ENTRY(m_strMap, SS_SM_POWER_REQUEST_MSG_RESP);
  MAP_ENTRY(m_strMap, SS_SM_USER_MODE_SET_RESP);
  MAP_ENTRY(m_strMap, SS_SM_DEBUG_VAR_CODE_DATA_SET_REQ);
  MAP_ENTRY(m_strMap, SS_SM_SET_BOOTLOADER_INFO);

  MAP_ENTRY(m_strMap, SS_SM_GET_BOOTLOADER_INFO);

  MAP_ENTRY(m_strMap, SS_SM_EVENT_ERROR);
  MAP_ENTRY(m_strMap, SS_SM_EVENT_ERROR_TO_SSL);
  MAP_ENTRY(m_strMap, SS_SM_ERROR_EVENT_LOGGING_START_REQ);
  MAP_ENTRY(m_strMap, SS_SM_ERROR_EVENT_LOGGING_START_RSPN);
  MAP_ENTRY(m_strMap, SS_SM_ERROR_EVENT_ARTIFACT_REQ);
  MAP_ENTRY(m_strMap, SS_SM_ERROR_EVENT_ARTIFACT_RSPN);
  MAP_ENTRY(m_strMap, SS_SM_ERROR_EVENT_LOGGING_COMPLETE);
  MAP_ENTRY(m_strMap, SS_SM_ERROR_EVENT_LOGGING_COMPLETE_RSPN);
  MAP_ENTRY(m_strMap, SS_SM_ERROR_EVENT_TIMER_ID_LOGGER_START_REQ);
  MAP_ENTRY(m_strMap, SS_SM_ERROR_EVENT_TIMER_ID_DEBUG_DUMP_RSPN);
  MAP_ENTRY(m_strMap, SS_SM_ERROR_EVENT_TIMER_ID_BOOT_MICRO_LOG_RSPN);
  MAP_ENTRY(m_strMap, SS_SM_ERROR_EVENT_TIMER_ID_CORE_FILE_POLL);
  MAP_ENTRY(m_strMap, SS_SM_USER_INVOKED_LOG_REQ);
  MAP_ENTRY(m_strMap, SS_SM_ERROR_EVENT_EEL_EXPORT_REQ);
  MAP_ENTRY(m_strMap, SS_SM_ERROR_EVENT_INTERFACEUNIFIED_EMMC_LOGS_REQ);
  MAP_ENTRY(m_strMap, SS_SM_ERROR_EVENT_INTERFACEUNIFIED_EMMC_LOGS_RSPN);
  MAP_ENTRY(m_strMap, SS_SM_ERROR_EVENT_CLR_LOGS_REQ);
  MAP_ENTRY(m_strMap, SS_SM_ERROR_EVENT_PROCESS_EXIT);
  MAP_ENTRY(m_strMap, SS_SM_ERROR_EVENT_DIAG_LOG_REQ);
  MAP_ENTRY(m_strMap, SS_SM_ERROR_EVENT_CAN_LOG_REQ);
  MAP_ENTRY(m_strMap, SS_SM_GROUP_LAUNCH_TRIGGER);
  MAP_ENTRY(m_strMap, SS_SM_DEBUG_DUMP);
  MAP_ENTRY(m_strMap, SS_SM_DEBUG_DUMP_RSPN);
  MAP_ENTRY(m_strMap, SS_SM_CPU_HIGH_LOAD_DETECTED);
  MAP_ENTRY(m_strMap, SS_SM_PROPAGATE_SYSTEM_ERROR);
  MAP_ENTRY(m_strMap, SS_SM__CWORD56__HEARTBEAT_REQ);
  MAP_ENTRY(m_strMap, SS_SM__CWORD56__HEARTBEAT_RSPN);
  MAP_ENTRY(m_strMap, SS_SM_BOOT_MICRO_RESET_NTF);
  MAP_ENTRY(m_strMap, SS_SM_BOOT_MICRO_LOG_REQ);
  MAP_ENTRY(m_strMap, SS_SM_BOOT_MICRO_LOG_RSP);
  MAP_ENTRY(m_strMap, SS_SM_WAKEUP_ORDER_SET_REQ);
  MAP_ENTRY(m_strMap, SS_SM_NEXT_WAKEUP_TYPE_SET_REQ);
  // LCOV_EXCL_BR_STOP 11:Excluded due to gcov restrictions(other)
}  // End of void Init_SS_SystemManagerProtocol_StrMap(std::map<SS_SystemManagerProtocol, SS_String> &m_strMap)



void Init_EInterfaceunifiedStatus_StrMap(std::map<EFrameworkunifiedStatus, SS_String> & m_strMap) {  // NOLINT (runtime/references)
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusEmptyMediaList);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusSessionLimitMaxedOut);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusDbRecNotFound);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusDbResultError);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusDbExecuteFail);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusSemCloseFail);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusSemUnlockFail);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusSemLockFail);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusFail);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusErrOther);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusOK);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusInvldBuf);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusInvldHandle);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusInvldHndlType);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusInvldQName);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusMsgQFull);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusInvldNotification);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusInvldParam);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusInvldBufSize);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusInvldID);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusCannotRelease);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusBadConnection);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusExit);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusNotImplemented);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusThreadBusy);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusThreadSelfJoin);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusThreadInvalidVal);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusThreadNotExist);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusFault);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusServNotFound);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusServerInUse);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusDbIndexing);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusNullPointer);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusMsgNotProcessed);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusFileLoadSuccess);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusFileLoadError);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusAccessError);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusDuplicate);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusMsgQEmpty);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusThreadAlreadyRunning);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusErrNoEBADF);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusErrNoEAGAIN);
  MAP_ENTRY(m_strMap, eFrameworkunifiedStatusErrNoEINTR);
}  // End of void Init_EInterfaceunifiedStatus_StrMap(std::map<EFrameworkunifiedStatus, SS_String> & m_strMap){

void Init_SS_ShutdownTrigger_StrMap(std::map<EPWR_SHUTDOWN_TRIGGER_TYPE, SS_String> & m_strMap) {  // NOLINT (runtime/references)
  MAP_ENTRY(m_strMap, epssdmsdtINVALID);
  MAP_ENTRY(m_strMap, epssdmsdtTESTACC_OFF);
  MAP_ENTRY(m_strMap, epssdmsdtON_KEY);
  MAP_ENTRY(m_strMap, epssdmsdtIGN_LOCK);
  MAP_ENTRY(m_strMap, epssdmsdtPWR_SAVE);
  MAP_ENTRY(m_strMap, epssdmsdtTMP_STARTUP);
  MAP_ENTRY(m_strMap, epssdmsdtDIAG_DEACTIVATION);
  MAP_ENTRY(m_strMap, epssdmsdtABNORMAL_VOLTAGE);
  MAP_ENTRY(m_strMap, epssdmsdtABNORMAL_TEMP);
  MAP_ENTRY(m_strMap, epssdmsdtBATTERYCUTOFF);
  MAP_ENTRY(m_strMap, epssdmsdtLIMPHOME);
  MAP_ENTRY(m_strMap, epssdmsdtHU_CAN_ERROR);
  MAP_ENTRY(m_strMap, epssdmsdtBODY_CAN_ERROR);
  MAP_ENTRY(m_strMap, epssdmsdtTRANSPORT_MODE);
  MAP_ENTRY(m_strMap, epssdmsdtPRODUCTION_MODE);
  MAP_ENTRY(m_strMap, epssdmsdtIGN_OFF);
  MAP_ENTRY(m_strMap, epssdmsdtGENERIC_ERROR_RESET);
  MAP_ENTRY(m_strMap, epssdmsdtFATAL_ERROR_RESET);
  MAP_ENTRY(m_strMap, epssdmsdtUSER_DATA_RESET);
  MAP_ENTRY(m_strMap, epssdmsdtFACTORY_DATA_RESET);
  MAP_ENTRY(m_strMap, epssdmsdtFAST_SLEEP_MODE);
  MAP_ENTRY(m_strMap, epssdmsdtNORMAL_RESET);
}  // End of void Init_SS_ShutdownTrigger_StrMap(std::map<EPWR_SHUTDOWN_TRIGGER_TYPE, SS_String> & m_strMap)

void Init_SS_StartupReason_StrMap(std::map<EPWR_WAKEUP_FACTOR_TYPE, SS_String> & m_strMap) {  // NOLINT (runtime/references)
  MAP_ENTRY(m_strMap, epswfINVALID);
  MAP_ENTRY(m_strMap, epswfTESTACC);
  MAP_ENTRY(m_strMap, epswfON_KEY);
  MAP_ENTRY(m_strMap, epswfIGN_ACC);
  MAP_ENTRY(m_strMap, epswfDR_OPEN_CLOSE);
  MAP_ENTRY(m_strMap, epswfDX_ACTIVATION);
  MAP_ENTRY(m_strMap, epswfPASS_ACTIVATION);
  MAP_ENTRY(m_strMap, epswfSPVACTIVATION);
  MAP_ENTRY(m_strMap, epswfUSER_DATA_RESET);
}  // LCOV_EXCL_BR_LINE 10:Because the last line
// End of void Init_SS_WakeupFactor_StrMap(std::map<EPWR_WAKEUP_FACTOR_TYPE, SS_String> & m_strMap)

// EOF ss_string_maps.cpp


