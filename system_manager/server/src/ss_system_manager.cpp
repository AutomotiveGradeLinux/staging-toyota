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

//#include <stub/mounter_ex_if.h>
#include <native_service/cl_lock.h>
#include <native_service/cl_process.h>
#include <native_service/cl_monitor.h>
//#include <stub/el_mem.h>
//#include <stub/el_gpio.h>

#include <native_service/frameworkunified_application.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_multithreading.h>
#include <native_service/frameworkunified_thread_priority.h>

#include <native_service/ns_np_service_nor_persistence.h>
#include <native_service/ns_np_service_protocol.h>
#include <native_service/ns_plogger_if.h>
#include <native_service/ns_np_service_if.h>
#include <native_service/ns_backup.h>

#include <heartbeat/ss_hb_if.h>

#include <processlauncher/ProcessLauncher_if.h>
#include <processlauncher/ss_sm_process_launcher.h>
#include <processlauncher/ss_sm_process_launcher_protocol.h>

#include <system_service/ss_system_manager_conf.h>
//#include <stub/ss_diag.h>

#include <system_service/ss_client_names.h>
#include <system_service/ss_heartbeat_service_protocol.h>
#include <system_service/ss_power_service_if.h>
#include <system_service/ss_power_service_notifications.h>
#include <system_service/ss_power_service_protocol.h>
#include <system_service/ss_services.h>
#include <system_service/ss_sm_rom_access.h>
#include <system_service/ss_sm_ram_access.h>
#include <system_service/ss_sm_thread_names.h>
#include <system_service/ss_sm_thread_names_local.h>
#include <system_service/ss_sm_client_if.h>
#include <system_service/ss_string_maps.h>
#include <system_service/ss_system_manager_notifications.h>
#include <system_service/ss_system_manager_notifications_local.h>
#include <system_service/ss_templates.h>
#include <system_service/ss_sys_boot_area_map.h>
#include <system_service/ss_sm_client_if_local.h>
#include <system_service/ss_test_clients.h>
//#include <vehicle_service/Clock_API.h>
//#include <power_hal.h>
//#include <security_hal.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/procfs.h>
#include <sys/timeb.h>
#include <sys/wait.h>
#include <inttypes.h>
#include <libgen.h>
#include <spawn.h>
#include <pthread.h>
#include <linux/oom.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sched.h>
#include <other_service/itoa.h>
#include <iomanip>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include "ss_sm_signals.h"
#include "ss_sm_default_paths.h"
#include "ss_sm_systemmanagerlog.h"
#include "ss_sm_version.h"
#include "ss_system_manager.h"
using namespace std;  // NOLINT

static CSystemManager g_oSystemManger;

#define SS_SM_TEMP_FILE_FOR_STORE_LOGS "/nv/romaccess/sm_tempStoreLogs"
#define TR_USB_PATH "/usr/agl/bin/realtimeUsbLog"

// \brief: public structure that is used for logging by NSLogger.
FRAMEWORKUNIFIEDLOGPARAM g_FrameworkunifiedLogParams = {
  FRAMEWORKUNIFIEDLOGOPTIONS,
  {
    ZONE_TEXT_10, ZONE_TEXT_11, ZONE_TEXT_12,
    ZONE_TEXT_13, ZONE_TEXT_14, ZONE_TEXT_15,
    ZONE_TEXT_16, ZONE_TEXT_17, ZONE_TEXT_18,
    ZONE_TEXT_19, ZONE_TEXT_20, ZONE_TEXT_21,
    ZONE_TEXT_22, ZONE_TEXT_23, ZONE_TEXT_24,
    ZONE_TEXT_25, ZONE_TEXT_26, ZONE_TEXT_27,
    ZONE_TEXT_28, ZONE_TEXT_29, ZONE_TEXT_30,
    ZONE_TEXT_31
  },
  FRAMEWORKUNIFIEDLOGZONES
};

std::string NumberToString(int Number) {  // LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  std::ostringstream ss;
  ss << Number;
  return ss.str();
}
// LCOV_EXCL_STOP

/* Check if "Mount Point" exsists as Directory */
static bool CheckMountPoint(const char* mntpt) {
  bool exists_mntpt = false;
  struct stat stat_buf;

  if (NULL != mntpt) {  // LCOV_EXCL_BR_LINE 200: cannot be null
    if (0 == lstat(mntpt, &stat_buf)) {
      if(S_ISDIR(stat_buf.st_mode)) {
        exists_mntpt = true;
      }
    }
  }
  return exists_mntpt;
}


/*****************************************************************************/
/**
 @ingroup: CSystemManager
 @brief: No Argument Constructor.
 @note: .
 @param void
 @return void
 */
/*****************************************************************************/
template<typename C, eFrameworkunifiedStatus (C::*M)(HANDLE)> EFrameworkunifiedStatus SysMgrCallback(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;
  C * pObj = static_cast<C *>(&g_oSystemManger);
  if (pObj) {
    l_eStatus = (pObj->*M)(hApp);
  }
  return l_eStatus;
}

//**************************************************************************
//  System Manager State String Map                                        *
//                                                                         *
void Init_SS_SMModuleState_StrMap(std::map<SS_SMModuleState, SS_String> & m_strMap) {  // NOLINT
  MAP_ENTRY(m_strMap, SS_SM_READY_TO_LAUNCH_APP);
  MAP_ENTRY(m_strMap, SS_SM_APPS_LAUNCH_IN_PROGRESS);
  MAP_ENTRY(m_strMap, SS_SM_APPS_LAUNCHED_READY_TO_START);
  MAP_ENTRY(m_strMap, SS_SM_APPS_START_IN_PROGRESS);
  MAP_ENTRY(m_strMap, SS_SM_APPS_START_COMPLETE);
  MAP_ENTRY(m_strMap, SS_SM_APPS_STOPPING_AT__CWORD56__REQ);
  MAP_ENTRY(m_strMap, SS_SM_APPS_STOPPING_AT_INTERNAL_REQ);
  MAP_ENTRY(m_strMap, SS_SM_WAITING_FOR_CRITICAL_APPS_AT__CWORD56__REQ);
  MAP_ENTRY(m_strMap, SS_SM_WAITING_FOR_CRITICAL_APPS_AT_INTERNAL_REQ);
  MAP_ENTRY(m_strMap, SS_SM_APPS_PRE_START_IN_PROGRESS);
  MAP_ENTRY(m_strMap, SS_SM_APPS_PRE_STOP_IN_PROGRESS);
  MAP_ENTRY(m_strMap, SS_SM_APPS_PRE_RUN_COMPLETE);
  MAP_ENTRY(m_strMap, SS_SM_APPS_BACKGROUND_START_IN_PROGRESS);
  MAP_ENTRY(m_strMap, SS_SM_APPS_BACKGROUND_STOP_IN_PROGRESS);
  MAP_ENTRY(m_strMap, SS_SM_APPS_BACKGROUND_RUN_COMPLETE);
}  // End of void Init_SS_SMModuleState_StrMap(std::map<SS_SMModuleState,

class EnumStringMap<SS_SMModuleState, Init_SS_SMModuleState_StrMap>
                    m_oSS_SMModuleStateStrMap;
SS_String GetStr(SS_SMModuleState f_enum) {
  return m_oSS_SMModuleStateStrMap.GetStr(f_enum);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
}  // End of SS_String :GetStr(SS_SMModuleState f_enum)
//                                                                         *
//  End of System Manager State String Map                                 *
//**************************************************************************

std::string CSystemManager::m_bootOpt;  // NOLINT

/*****************************************************************************/
/**
 @ingroup: CSystemManager
 @brief: No Argument Constructor.
 @note: .
 @param void
 @return void
 */
/*****************************************************************************/
CSystemManager & CSystemManager::GetInstance() {
  return g_oSystemManger;
}

/*****************************************************************************/
/**
 @ingroup: CSystemManager
 @brief: No Argument Constructor.
 @note: .
 @param void
 @return void
 */
/*****************************************************************************/
CSystemManager::CSystemManager() :
  m_hHeartbeatThread(INVALID_HANDLE, eSM_ThreadNotExist),
  m_hProcLauncherThread(INVALID_HANDLE, eSM_ThreadNotExist),
  m_SystemLaunchProgress(SS_SM_INITIAL_GROUP),
  m_GroupLaunchTimer(NULL),
  m_GroupRelaunchCount(0),
  m_GroupRelaunchLimit(0),
  m_hPowerServiceSession(NULL),
  m_ActiveGroupId(SS_SM_INITIAL_GROUP),
  m_StartUpReason(epswfINVALID),
  m_BootMemory(NULL),
  m_BootMemoryMapSize(0),
  m_DataResetMode(e_SS_SM_DATA_RESET_MODE_NONE),
  m_ProgUpdateState(SS_SM_PROG_UPDATE_STATE_NONE),
  m_NextWakeupType(e_SS_SM_NEXT_WAKEUP_TYPE_NONE),
  m_DramBackupStatus(e_SS_SM_DRAM_BACKUP_NG),
  m_isIlgReset(FALSE),
  m_ResetStatus(e_SS_SM_RESET_STATUS_NONE),
  m_ResetCount(0),
  m_SMCurrentState(SS_SM_READY_TO_LAUNCH_APP),

  m_MaxShutdownTime(0),
  m_NbrDebugDumpRspnRecv(0),
  m_isRcvModeInfo(FALSE),
  m_BootMode(e_SS_SM_BOOT_MODE_INVALID),
  m_BinaryFilesPath(""),
  m_ConfigFilesPath(""),
  m_pfStopCompleteHandler(
      SysMgrCallback<CSystemManager, &CSystemManager::send_shutdown_complete_response>),
  m_VersionNumberStruct(0, eFrameworkunifiedStatusErrOther),
  m_BuildInfoStr(""),
  m_isRstPending(FALSE),
  m_rstPendingInfo(),
  m_UsingVMPlayer(FALSE),
  NVM_VALID_SIGNATURE(0xBA5EBA11),
  m_lastUserMode(epsumON),
  m_shutdownTrigger(epssdmsdtINVALID),
  m_isImmediateReset(FALSE),
  m_isImmResetReq(FALSE),
  m_needReNotifyStartPrm(FALSE),
  m_pVarCodeStr(NULL),
  m_SSLGroupLaunchMapIterator(NULL),
  m_ModuleLaunchListIter(NULL),
  m_bIsNPP_ServicesStarted(FALSE),
  m_bIsBackupAvail(FALSE),
  m_oSystemLauncher(),
  m_SystemStarter(),
  m_NPPStopSent(FALSE),
  m_userModeChangeReason(epsumcrNOT_AVAILABLE),
  m__CWORD56_CmdHist(SS_SM__CWORD56__CMD_HIST_SIZE),
  m_SMCmdHist(SS_SM_CMD_HIST_SIZE),
  m_TimerCmdHist(SS_SM_TIMER_CMD_HIST_SIZE),
  m_PubCmdHist(SS_SM_PUB_CMD_HIST_SIZE),
  m_ErrHist(SS_SM_ERR_HIST_SIZE),
  m_ClProcessSigFd(-1),
  m_SysMemMonitor(),
  m_FreeMemAvailable(0),
  m_coreFileSizeBytes(0),
  m_BootMicroResetReason(SS_SM_BOOT_MICRO_RESET_REASON_SELF_RESET),
  m_errorEventQueueLocked(FALSE),
  m_isPrevErrEventCompleted(TRUE),
  m_errorEventResult(eFrameworkunifiedStatusOK),
  m_requestedArtifactId(eArtifactIdInterfaceunifiedDebugLog) {  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  m_InitialBoot = true;

  m_Wake.powerupType = epswsPWRON;
  m_Wake.up.level = epswlFULLRUN;

  m_SystemModeInfo.lastUserMode = epsumINVALID;
  m_SystemModeInfo.limpHomeCutoff = epslhcINVALID;
  m_SystemModeInfo.productionMode = epspmINVALID;
  m_SystemModeInfo.transportMode = epstmINVALID;
  m_SystemModeInfo.systemMode = epssinfINVALID;
  m_SystemModeInfo.startupStage = epssusfINVALID;

  bzero(&m_SMConfig, sizeof(m_SMConfig));
  bzero(&m_startUpConfirmationMsg, sizeof(m_startUpConfirmationMsg));
  bzero(&m_HBReport, sizeof(m_HBReport));
  bzero(&m_Lbm_Ram, sizeof(LBM_RAM_t));

  m__CWORD56_HistIter = m__CWORD56_CmdHist.begin();
  m_TimerHistIter = m_TimerCmdHist.begin();
  m_SMHistIter = m_SMCmdHist.begin();
  m_PubHistIter = m_PubCmdHist.begin();
  m_ErrHistIter = m_ErrHist.begin();
    // LCOV_EXCL_BR_START 11:unexpected branch  // NOLINT(whitespace/line_length)
  ListOfServicesAuthorizedToSetBootMode.push_back(SERVICE_SOFTWAREUPDATE);
  //
  // ToDo: Remove "Updater Thread" name from the Authorized list
  ListOfServicesAuthorizedToSetBootMode.push_back("UpdaterThread");
  //
  // ToDo Jay 02 July 2012 Replace with actual name
  ListOfServicesAuthorizedToSetBootMode.push_back(SERVICE_HMI);
  ListOfServicesAuthorizedToSetBootMode.push_back(SERVICE_VS_DIAG);
  ListOfServicesAuthorizedToSetBootMode.push_back(SERVICE_PSM_SHADOW);
  ListOfServicesAuthorizedToSetBootMode.push_back(TC_SysManager);
  ListOfServicesAuthorizedToSetBootMode.push_back(SERVICE_VUP);
  ListOfServicesAuthorizedToSetBootMode.push_back(SS_UPDATESERVICE);
  ListOfServicesAuthorizedToSetBootMode.push_back("SSBOX");

  ////******************************************************************////
  ////       Initialization of enum<=>enum and enum=>string maps        ////
  ////                                                                  ////
  //
  // **** Initialization of BootMode enum maps ****
  // Map of Power Service SystemModeInfo to System Services BootMode
  // enum values
  m_PSSMI_to_SSBootMode_map[epssinfINVALID] = e_SS_SM_BOOT_MODE_INVALID;
  m_PSSMI_to_SSBootMode_map[epssinfNORMAL] = e_SS_SM_BOOT_MODE_APPLICATION;
  m_PSSMI_to_SSBootMode_map[epssinfPROGRAMMING] = e_SS_SM_BOOT_MODE_PROGRAMMING;
  //
  // Map of System Services to Power Service SystemModeInfo BootMode
  // enum values
  m_SSBootMode_to_PSSMI_map[e_SS_SM_BOOT_MODE_INVALID] = epssinfINVALID;
  m_SSBootMode_to_PSSMI_map[e_SS_SM_BOOT_MODE_APPLICATION] = epssinfNORMAL;
  m_SSBootMode_to_PSSMI_map[e_SS_SM_BOOT_MODE_PROGRAMMING] = epssinfPROGRAMMING;
  //
  // **** Initialization of powerupType enum maps ****
  // Map of System Services powerupType to BOOL enum values
  m_PowerType_to_SSBoolEnumMap[epswsPWRON] = TRUE;
  m_PowerType_to_SSBoolEnumMap[epswsPWROFF] = FALSE;
  //
  // Map of System Services powerupType to System Services User Mode enum values
  m_PowerType_to_SSUserModeEnumMap[epswsPWRON] = epsumON;
  m_PowerType_to_SSUserModeEnumMap[epswsPWROFF] = epsumOFF;
  //
  // Map of BOOL to System Services powerupType enum values
  m_SSBool_to_PowerTypeEnumMap[TRUE] = epswsPWRON;
  m_SSBool_to_PowerTypeEnumMap[FALSE] = epswsPWROFF;
  //
  // **** Initialization of User Mode ( aka Last User Mode ) maps ****
  // Map of BOOL to System Services User Mode enum values
  m_SSBool_to_SSUserModeEnumMap[FALSE] = epsumOFF;
  m_SSBool_to_SSUserModeEnumMap[TRUE] = epsumON;
  //
  // Map of BOOL to System Services User Mode enum values
  m_SSUserMode_to_SSBoolEnumMap[epsumOFF] = FALSE;
  m_SSUserMode_to_SSBoolEnumMap[epsumON] = TRUE;
  //
  // Map of ROM to System Services User Mode enum values
  m_ROM_to_SSUserModeEnumMap[USER_OFF] = epsumOFF;
  m_ROM_to_SSUserModeEnumMap[USER_ON] = epsumON;
  //
  // Map of System Services to ROM User Mode enum values
  m_SS_to_ROMUserModeEnumMap[epsumOFF] = USER_OFF;
  m_SS_to_ROMUserModeEnumMap[epsumON] = USER_ON;
  //
  // **** Initialization of Limp Home Cutoff maps ****
  // Map of Control to Limp Home Cutoff Mode enum values
  m_Control_to_LimpHomeCutoffMode_map[DISABLE_MODE] = epslhcDISABLED;
  m_Control_to_LimpHomeCutoffMode_map[ENABLE_MODE] = epslhcENABLED;
  //
  // Map of Limp Home Cutoff Mode to Control enum values
  m_LimpHomeCutoff_to_ControlMode_map[epslhcDISABLED] = DISABLE_MODE;
  m_LimpHomeCutoff_to_ControlMode_map[epslhcENABLED] = ENABLE_MODE;
  //
  // **** Initialization of Production Mode maps ****
  // Map of Control to Production Mode enum values
  m_Control_to_ProductionMode_map[DISABLE_MODE] = epspmDISABLED;
  m_Control_to_ProductionMode_map[ENABLE_MODE] = epspmENABLED;
  //
  // Map of Production to Mode Control enum values
  m_Production_to_ControlMode_map[epspmDISABLED] = DISABLE_MODE;
  m_Production_to_ControlMode_map[epspmENABLED] = ENABLE_MODE;
  //
  // **** Initialization of Transport Mode maps ****
  // Map of Control Mode to Transport Mode enum values
  m_Control_to_TransportMode_map[DISABLE_MODE] = epstmDISABLED;
  m_Control_to_TransportMode_map[ENABLE_MODE] = epstmENABLED;
  //
  // Map of Transport Mode to Control Mode enum values
  m_Transport_to_ControlMode_map[epstmDISABLED] = DISABLE_MODE;
  m_Transport_to_ControlMode_map[epstmENABLED] = ENABLE_MODE;

//  m_PsmSysupDram_to_SysupDramMap[PSM_SYSUP_DRAM_NORMAL]   = SYSUP_DRAM_NORMAL;
//  m_PsmSysupDram_to_SysupDramMap[PSM_SYSUP_DRAM_SFTERRSR] = SYSUP_DRAM_SFTERRSR;
//  m_PsmSysupDram_to_SysupDramMap[PSM_SYSUP_DRAM_DISCARD]  = SYSUP_DRAM_DISCARD;

//  m_PsmStartStatus_to_SysupCndMap[PSM_START_STATUS_FACTRESET] = SYSUP_CND_FACTRESET;
//  m_PsmStartStatus_to_SysupCndMap[PSM_START_STATUS_NORMAL]    = SYSUP_CND_NORMAL;

//  m_PsmComSts_to_SubcpuStsMap[PSM_COM_STS_NORMAL]   = SUBCPU_STS_NOMAL;
//  m_PsmComSts_to_SubcpuStsMap[PSM_COM_STS_ABNORMAL] = SUBCPU_STS_COMNG;

//  m_PsmDramSts_to_SdramStsMap[PSM_DRAM_STS_NORMAL]   = SDRAM_STS_NOMAL;
//  m_PsmDramSts_to_SdramStsMap[PSM_DRAM_STS_ABNORMAL] = SDRAM_STS_BUPNG;

//  m_PsmDramSts_to_RamjudgeStsMap[PSM_DRAM_STS_NORMAL]   = RAMJUDGE_STS_NOMAL;
//  m_PsmDramSts_to_RamjudgeStsMap[PSM_DRAM_STS_ABNORMAL] = RAMJUDGE_STS_NG;

//  m_PsmDramSts_to_SelfRefreshMap[PSM_DRAM_STS_NORMAL]   = SELF_REFRESH_OK;
//  m_PsmDramSts_to_SelfRefreshMap[PSM_DRAM_STS_ABNORMAL] = SELF_REFRESH_NG;

  // LCOV_EXCL_BR_STOP
  ////                                                                  ////
  ////    End of Initialization of enum<=>enum and enum=>string maps    ////
  ////******************************************************************////

  m_startUpConfirmationMsg.wakeupType = epsstINVALID;
  m_startUpConfirmationMsg.coldStartRequest = epsscrtINVALID;
  m_startUpConfirmationMsg.HWVersion = UINT32_MAX;
  m_startUpConfirmationMsg.matchedHardwareType = UINT32_MAX;
  m_startUpConfirmationMsg.softwareType = UINT32_MAX;
  m_startUpConfirmationMsg.imageType = UINT32_MAX;
  m_startUpConfirmationMsg.majorVersion = UINT32_MAX;
  m_startUpConfirmationMsg.minorVersion = UINT32_MAX;

  m_SystemManagerPriority = PR_SS_SYSMANAGER;
}

/*****************************************************************************/
/**
 @ingroup: CSystemManager
 @brief: Destructor
 @note: .
 @param void
 @return void
 */
/*****************************************************************************/
CSystemManager::~CSystemManager() {  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  if (NULL != m_pVarCodeStr) {
    delete[] m_pVarCodeStr;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  }
}

/*****************************************************************************/
/**
 @ingroup: CSystemManager
 @brief: Initialize_memory Memory initialization processing
 @note: .
 @param
 @return void
 */
/*****************************************************************************/
void CSystemManager::Initialize_memory(HANDLE hApp, bool* l_isInitFail, EFrameworkunifiedStatus* l_eStatus_work, BOOL isIllReset) {
  int ret;
  EFrameworkunifiedStatus l_eStatus;

  /// Start Process for Realtime USB Logging
  {
    PreLaunchModuleParams l_rtuparam;
    l_rtuparam.LaunchFunc     = CreateRtUsb;
    l_rtuparam.relaunchLimit  = 1;
    l_rtuparam.name           = "realtimeUsbLog";
    l_rtuparam.binaryFileName = TR_USB_PATH;
/*    l_rtuparam.pid = CreateRtUsb();
    if (l_rtuparam.pid == 1) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: CreateRtUsb()");
    }  */
    m_PreLaunchModuleList.push_back(l_rtuparam);
  }

  {
    PreLaunchModuleParams l_param;
    l_param.LaunchFunc = CreateSyslogd;
    l_param.relaunchLimit = 1;   // limit of abnormal relaunching
    l_param.name = "syslogd";
    l_param.binaryFileName = "/usr/bin/syslogd";
    if (l_param.pid == 1) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: CreateSyslogd()");
    }
    m_PreLaunchModuleList.push_back(l_param);
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  int fd = open("/dev/urandom", O_RDONLY);  // LCOV_EXCL_BR_LINE 5: standard lib error
  if (fd == -1) {  // LCOV_EXCL_BR_LINE 5: standard lib error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    SS_ASERT_ERRNO(0);  // LCOV_EXCL_LINE 5: standard lib error
  } else {
    char buf[4];
    ret = static_cast<int>(read(fd, buf, 4));  // LCOV_EXCL_BR_LINE 5: standard lib error
    if (-1 == ret) {  // LCOV_EXCL_BR_LINE 5: standard lib error
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      SS_ASERT_ERRNO(0);  // LCOV_EXCL_LINE 5: standard lib error
    } else if (4 != ret) {  // LCOV_EXCL_BR_LINE 5: standard lib error
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      SS_ASERT(0);  // LCOV_EXCL_LINE 5: standard lib error
    } else {
      fprintf(stderr, "WakeupID:%X%X%X%X\n", buf[0], buf[1], buf[2], buf[3]);
      FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "WakeupID:%X%X%X%X", buf[0], buf[1], buf[2], buf[3]);
    }
    close(fd);
  }

  FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "bootOpt:%s", m_bootOpt.c_str());

  *l_eStatus_work = FrameworkunifiedAttachCallbackToDispatcherWithFd(hApp, m_ClProcessSigFd,  // LCOV_EXCL_BR_LINE 4: nsfw error
        SysMgrCallback<CSystemManager, &CSystemManager::OnProcessTermDetected>);

  if (eFrameworkunifiedStatusOK != *l_eStatus_work) {  // LCOV_EXCL_BR_LINE 4: nsfw error
    // LCOV_EXCL_START 4: nsfw error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("FrameworkunifiedAttachCallbackToDispatcherWithFd()");
    *l_isInitFail = true;
    // LCOV_EXCL_STOP
  }

  pthread_mutex_init(&sm_hist_mutex, NULL);

  ReadPathFromEnvironmentVariables();
  ReadUsingVMPlayerEnvironmentVariable();

  FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "\n"
      "bootIf\n"
      " sts        :%#x %s\n"
      " dram123      :%#x/%#x/%#x %s\n"
      " syscomSts    :%#x %s\n"
      " pwSts      :%#x %s\n"
      " ramjudgeSts    :%#x %s\n"
      " dram_self_refresh:%#x %s", m_BootMemory->sts,
      (m_BootMemory->sts == SYSUP_CND_FACTRESET) ? "FACT" : "NML",
              m_BootMemory->dram1, m_BootMemory->dram2, m_BootMemory->dram3,
              (m_BootMemory->dram1 == SYSUP_DRAM_NORMAL) ? "NML" :
              (m_BootMemory->dram1 == SYSUP_DRAM_DISCARD) ? "DISCARD" : "ERROR",
      m_BootMemory->syscomSts,
              (m_BootMemory->syscomSts == SUBCPU_STS_COMNG) ? "NG" : "NML",
      m_BootMemory->pwSts,
              (m_BootMemory->pwSts == SDRAM_STS_BUPNG) ? "NG" : "NML",
      m_BootMemory->ramjudgeSts,
              (m_BootMemory->ramjudgeSts == RAMJUDGE_STS_NG) ? "NG" : "NML",
      m_BootMemory->dram_self_refresh,
              (m_BootMemory->dram_self_refresh == SELF_REFRESH_NG) ? "NG" : "OK");

  {
    DRAM_BACKUP_STATE l_bkupState =
        (SDRAM_STS_BUPNG == m_BootMemory->pwSts) ? DRAM_BACKUP_STATE_NG : DRAM_BACKUP_STATE_OK;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    ROM_AccessIf rom_access;
    if (eFrameworkunifiedStatusOK != (*l_eStatus_work = rom_access.SystemInitialize(l_bkupState))) {
      l_eStatus = *l_eStatus_work;
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: rom_access.SystemInitialize(%d)", l_eStatus);
      *l_isInitFail = true;
    }
  }

  if (eFrameworkunifiedStatusOK != (*l_eStatus_work = SetSystemModeInfoStruct())) {
    LOG_ERROR("SetSystemModeInfoStruct()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    *l_isInitFail = true;
  }

  m_BootMode = m_PSSMI_to_SSBootMode_map[m_SystemModeInfo.systemMode];
  FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "Bootmode is '%s'", GetStr(m_BootMode).c_str());
  FRAMEWORKUNIFIEDLOG(ZONE_PERFORMANCE, __FUNCTION__, "Bootmode is '%s'", GetStr(m_BootMode).c_str());

  {
    ROM_AccessIf rom_access;
    rom_access.Initialize();  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    BOOL isLastIlgReset = FALSE;

    if (eFrameworkunifiedStatusOK != (*l_eStatus_work = GetLastIlgReset(rom_access, isLastIlgReset))) {
      l_eStatus = *l_eStatus_work;
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: GetLastIlgReset(%d)", l_eStatus);
    }

    m_isIlgReset = isIllReset ? TRUE : isLastIlgReset;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

    if (eFrameworkunifiedStatusOK != (*l_eStatus_work = SetLastIlgReset(rom_access, m_isIlgReset))) {
      l_eStatus = *l_eStatus_work;
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: SetLastIlgReset(%d)", l_eStatus);
    }
  }

  {
    RAM_WAKEUP_STATE l_ramWakeupState =
        (SDRAM_STS_BUPNG == m_BootMemory->pwSts) ? RAM_WAKEUP_STATE_BACKUP_NG : RAM_WAKEUP_STATE_DONT_CARE;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    RAM_AccessIf l_cRamAccess(l_ramWakeupState);
    RAM_SM_INFO_t info;

    if (eFrameworkunifiedStatusOK != (*l_eStatus_work = l_cRamAccess.getRamInfo(&info))) {
      l_eStatus = *l_eStatus_work;
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: l_cRamAccess.getRamInfo(%d)", l_eStatus);
    } else {
      m_isImmediateReset = info.isImmediateReset;
      info.isImmediateReset = TRUE;
      if (eFrameworkunifiedStatusOK != (*l_eStatus_work = l_cRamAccess.setRamInfo(&info))) {
        l_eStatus = *l_eStatus_work;
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: l_cRamAccess.setRamInfo(%d)", l_eStatus);
      }
    }
  }

  // Select configuration file for wakeup services
  BOOL bIsVupMode = (m_SystemModeInfo.systemMode == epssinfPROGRAMMING) ? TRUE : FALSE;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  T_SS_SM_INIT_HOOK_IN_PARAM inPrm;

  std::memset(&inPrm, 0, sizeof(inPrm));
  inPrm.bIsVupMode = bIsVupMode;
  inPrm.cbRebootNoticeFunc = SysMgrCallback<CSystemManager, &CSystemManager::OnRebootNotification>;

  if (eFrameworkunifiedStatusOK != (*l_eStatus_work = ss_sm_initHook(hApp, &inPrm, &m_productCustomPrm))) {
    SS_ASERT_LOG(0, "ERROR: ss_sm_initHook()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    *l_isInitFail = true;
  }

  SetEnvVariableForVupMode(bIsVupMode);

  // Get the Version Number
  // GetVersionNumber() will report any errors
  m_VersionNumberStruct.m_eSystemmanagerStatus = GetVersionNumber(m_VersionNumberStruct.m_VersionNumber);

  *l_eStatus_work = GetBuildInfo(m_BuildInfoStr);
  l_eStatus = *l_eStatus_work;

  LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus, "GetBuildInfo()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)

  INTERFACEUNIFIEDLOG_WHEN_COMPILED;  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " %s priority is %d", SERVICE_SYSMANAGER, m_SystemManagerPriority);
  FRAMEWORKUNIFIEDLOG(ZONE_PERFORMANCE, __FUNCTION__, " %s was compiled at %s @ %s", __FILE__, __DATE__, __TIME__);

  FRAMEWORKUNIFIEDLOG(ZONE_PERFORMANCE, __FUNCTION__, " Host Processor Software Version is '0x%016llX'",
         m_VersionNumberStruct.m_VersionNumber);

  FRAMEWORKUNIFIEDLOG(ZONE_PERFORMANCE, __FUNCTION__, " Host Processor Build Version is '%s'", m_BuildInfoStr.c_str());

  LogESystemmanagerStatusEnums();
  LogProtocolIDs();
}

/*****************************************************************************/
/**
 @ingroup: CSystemManager
 @brief: Initialize_callbacks Callback enrollment process
 @note: .
 @param
 @return void
 */
/*****************************************************************************/
void CSystemManager::Initialize_callbacks(HANDLE hApp, bool *l_isInitFail, EFrameworkunifiedStatus *l_eStatus_work) {
  EFrameworkunifiedStatus l_eStatus;

  m_GroupLaunchTimer = new (std::nothrow) TimerCtrl(eSM_TIMERS_END);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    if (NULL == m_GroupLaunchTimer) {  // LCOV_EXCL_BR_LINE 5: Because new doesn't pass the failure case
    // LCOV_EXCL_START 5: Because new doesn't pass the failure case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    *l_eStatus_work = eFrameworkunifiedStatusNullPointer;
    LOG_ERROR("new(std::nothrow)TimerCtrl(eSM_TIMERS_END)");
    *l_isInitFail = true;
    // LCOV_EXCL_STOP
  }

  // LCOV_EXCL_BR_START 11:unexpected branch  // NOLINT(whitespace/line_length)
  m_GroupLaunchTimer->Initialize(hApp);
  m_aTimerIDs[eSM_TIMER_GROUP_MONITOR] =
      m_GroupLaunchTimer->CreateTimer(SysMgrCallback<CSystemManager, &CSystemManager::OnGroupLaunchTimerExpiry>);
  m_aTimerIDs[eSM_TIMER_PROCESS_LAUNCH_MONITOR] =
      m_GroupLaunchTimer->CreateTimer(SysMgrCallback<CSystemManager, &CSystemManager::OnProcessLaunchTimerExpiry>);
  m_aTimerIDs[eSM_TIMER_HEARTBEAT_MONITOR] =
      m_GroupLaunchTimer->CreateTimer(SysMgrCallback<CSystemManager, &CSystemManager::OnHeartBeatMonitorTimerExpiry>);
  m_aTimerIDs[eSM_TIMER_PROCESSLAUNCHER_MONITOR] =
      m_GroupLaunchTimer->CreateTimer(SysMgrCallback<CSystemManager,
                                      &CSystemManager::OnProcessLaunchMonitorTimerExpiry>);
  m_aTimerIDs[eSM_TIMER_CLIENT_START_MONITOR] =
      m_GroupLaunchTimer->CreateTimer(SysMgrCallback<CSystemManager,
                                      &CSystemManager::OnClientStartMonitorTimerExpiry>);
  m_aTimerIDs[eSM_TIMER_NPP_STATUS_CHECK_MONITOR] =
      m_GroupLaunchTimer->CreateTimer(SysMgrCallback<CSystemManager,
                                      &CSystemManager::OnNPPStatusCheckMonitorTimerExpiry>);
  m_aTimerIDs[eSM_TIMER__CWORD56__HEARTBEAT_RESPONSE] =
      m_GroupLaunchTimer->CreateTimer(SysMgrCallback<CSystemManager,
                                      &CSystemManager::On_CWORD56_HeartBeatResponseIntervalTimerExpiry>);
  m_aTimerIDs[eSM_TIMER_LAUNCH_GROUP_TRIGGER_PROC_RESP_TIMER] =
      m_GroupLaunchTimer->CreateTimer(SysMgrCallback<CSystemManager,
                                      &CSystemManager::OnLaunchGroupTriggerProcessResponseTimerExpiry>);
  m_aTimerIDs[eSM_TIMER_GROUP_LAUNCH_WAIT_TIMER] =
      m_GroupLaunchTimer->CreateTimer(SysMgrCallback<CSystemManager, &CSystemManager::OnGroupLaunchWaitTimeout>);
  m_aTimerIDs[eSM_TIMER_MODULE_CONNECT_WAIT_TIMER] =
      m_GroupLaunchTimer->CreateTimer(SysMgrCallback<CSystemManager, &CSystemManager::OnModuleConnectWaitTimeout>);
  m_aTimerIDs[eSM_TIMER_START_RESP_MONITOR_WAIT_TIMER] =
      m_GroupLaunchTimer->CreateTimer(SysMgrCallback<CSystemManager, &CSystemManager::OnStartRespMonitorTimeout>);
  m_aTimerIDs[eSM_TIMER_SHUTDOWN_COMPLETE_MONITOR] =
      m_GroupLaunchTimer->CreateTimer(SysMgrCallback<CSystemManager,
                                      &CSystemManager::OnShutdownCompleteMonitorTimeout>);
  m_aTimerIDs[eSM_TIMER_CLIENT_STOP_MONITOR] =
      m_GroupLaunchTimer->CreateTimer(SysMgrCallback<CSystemManager, &CSystemManager::OnClientStopMonitorTimerExpiry>);

  FrameworkunifiedProtocolCallbackHandler l_cbArrayAnySource[] = {
      // Command ID,                   Call back functions
    { SS_SM_PROTOCOL_OPEN_SESSION_REQ, SysMgrCallback<CSystemManager, &CSystemManager::OnOpenSession> },
    { SS_SM_CPU_RESET_REQ,             SysMgrCallback<CSystemManager, &CSystemManager::OnCpuResetRequest> }
  };

  FrameworkunifiedProtocolCallbackHandler l_cbArrayGroupLaunchService[] = {
      // Command ID,              Call back functions
    { SS_SM_GROUP_LAUNCH_TRIGGER, SysMgrCallback<CSystemManager, &CSystemManager::OnLaunchGroupTriggerProcessResponse> }
  };

  FrameworkunifiedProtocolCallbackHandler l_cbArrayNPPService[] = {
      // Command ID,           Call back functions
    { NPS_NPP_READY_EVENT,     SysMgrCallback<CSystemManager, &CSystemManager::OnNPPReadyEventCallback> },
    { NPS_GET_READYSTATUS_ACK, SysMgrCallback<CSystemManager, &CSystemManager::OnNPPReadyStatusCallback> },
    { NPS_NPP_STOP_ACK,        SysMgrCallback<CSystemManager, &CSystemManager::OnNppStopComplete> }
  };

  FrameworkunifiedProtocolCallbackHandler l_cbArrayPowerService[] = {
      // Command ID,              Call back functions
    { SS_SM_POWER_REQUEST_MSG,    SysMgrCallback<CSystemManager, &CSystemManager::OnPowerRequestCallback> },
    { SS_SM_WAKEUP_MODULES,       SysMgrCallback<CSystemManager, &CSystemManager::OnWakeupCallback> },
    { SS_SM_SYSTEM_MODE_INFO_REQ, SysMgrCallback<CSystemManager, &CSystemManager::OnSystemModeInfoRequest> },
    { SS_SM_INITCOMP_REP,         SysMgrCallback<CSystemManager, &CSystemManager::OnInitCompReportCallback> },
    { SS_SM_SHUTDOWN_MODULES,     SysMgrCallback<CSystemManager, &CSystemManager::OnShutdownModulesRequest> },
    { SS_SM_FWD_STARTUP_CONFIRMATION_MSG_REQ,
                            SysMgrCallback<CSystemManager, &CSystemManager::OnSetStartupConfirmationDataRequest> }
  };

  FrameworkunifiedProtocolCallbackHandler l_cbArraySystemManager[] = {
      // Command ID,        Call back functions
    { SS_SM_SendTriggerToSelf, SysMgrCallback<CSystemManager, &CSystemManager::OnLaunchGroupSelfTrigger> },
    { SS_SM_DEBUG_DUMP_RSPN, SysMgrCallback<CSystemManager,   &CSystemManager::OnDebugDumpResponseReceived> }
  };
  // LCOV_EXCL_BR_STOP

#define ATTACH_CBS_TO_DISPATCHER(L_ARRAY, CALLING_SERVICE_NAME)       \
    *l_eStatus_work = l_eStatus = FrameworkunifiedAttachCallbacksToDispatcher(hApp,           \
                          CALLING_SERVICE_NAME,   \
                          L_ARRAY,          \
                          static_cast<UI_32>_countof(L_ARRAY));     \
    if (eFrameworkunifiedStatusOK != l_eStatus) {                  \
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,                    \
       " Error: FrameworkunifiedAttachCallbacksToDispatcher(%s) errored: %d/'%s'",   \
       #L_ARRAY,                            \
       l_eStatus,                             \
       GetStr(l_eStatus).c_str());                    \
       *l_isInitFail = true;                       \
    }
// End of #define ATTACH_CBS_TO_DISPATCHER( CALLING_SERVICE_NAME, L_ARRAY )

  // LCOV_EXCL_BR_START 15: marco defined above
  // Note: This macro will exit this function if the embedded fnc errors.
  ATTACH_CBS_TO_DISPATCHER(l_cbArrayAnySource, FRAMEWORKUNIFIED_ANY_SOURCE);

  // Note: This macro will exit this function if the embedded fnc errors.
  ATTACH_CBS_TO_DISPATCHER(l_cbArrayGroupLaunchService, SS_GROUP_LAUNCH_TRIGGER);

  // Note: This macro will exit this function if the embedded fnc errors.
  ATTACH_CBS_TO_DISPATCHER(l_cbArrayNPPService, FRAMEWORKUNIFIED_NS_NPSERVICE);

  // Note: This macro will exit this function if the embedded fnc errors.
  ATTACH_CBS_TO_DISPATCHER(l_cbArrayPowerService, SERVICE_POWER);

  // Note: This macro will exit this function if the embedded fnc errors.
  ATTACH_CBS_TO_DISPATCHER(l_cbArraySystemManager, SERVICE_SYSMANAGER);
  // LCOV_EXCL_BR_STOP

  {
    char pathBuf[128];
    // LCOV_EXCL_BR_START 11:unexpected branch  // NOLINT(whitespace/line_length)
    snprintf(pathBuf, sizeof(pathBuf), "/proc/%d/oom_score_adj", getpid());
    std::ofstream fo(pathBuf);
    fo << OOM_SCORE_ADJ_MIN << endl;
    fo.close();
    // LCOV_EXCL_BR_STOP
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "set OOM_SCORE_ADJ_MIN(SS_SysManager/%d)", getpid());
  }

#undef ATTACH_CBS_TO_DISPATCHER

  // LCOV_EXCL_BR_START 4: nsfw error
  *l_eStatus_work = FrameworkunifiedSubscribeNotificationWithCallback(hApp, NTFY_BackupMgr_Availability,
          SysMgrCallback<CSystemManager, &CSystemManager::OnBackupMgrAvailCallback>);
  // LCOV_EXCL_BR_STOP
  if (eFrameworkunifiedStatusOK != *l_eStatus_work) {  // LCOV_EXCL_BR_LINE 4: nsfw error
    // LCOV_EXCL_START 4: nsfw error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("FrameworkunifiedSubscribeNotificationWithCallback()");
    *l_isInitFail = true;
    // LCOV_EXCL_STOP
  }

  if (eFrameworkunifiedStatusOK != (*l_eStatus_work = ErrorEventInit(hApp))) {  // LCOV_EXCL_BR_LINE 6: function do not return error
    // LCOV_EXCL_START 6: function do not return error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("ErrorEventInit()");
    *l_isInitFail = true;
    // LCOV_EXCL_STOP
  }

#ifndef SS_SM_SKIP_HEARTBEAT_INIT
  // LCOV_EXCL_BR_START 6: function do not return error, expect nsfw error
  if (eFrameworkunifiedStatusOK != (*l_eStatus_work = init_Heartbeat(hApp))) {
  // LCOV_EXCL_BR_STOP
    // LCOV_EXCL_START 6: function do not return error, expect nsfw error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("init_HeartBeat()");
    *l_isInitFail = true;
    // LCOV_EXCL_STOP
  }
#else
#warning Test code - NOT calling init_Heartbeat(hApp)
  FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: NOT calling init_Heartbeat(hApp)");
#endif
#undef SS_SM_SKIP_HEARTBEAT_INIT

  // LCOV_EXCL_BR_START 6: function do not return error, expect nsfw error
  if (eFrameworkunifiedStatusOK != (*l_eStatus_work = init_process_launcher(hApp))) {
  // LCOV_EXCL_BR_STOP
    // LCOV_EXCL_START 6: function do not return error, expect nsfw error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("init_process_launcher()");
    *l_isInitFail = true;
    // LCOV_EXCL_STOP
  }

  // LCOV_EXCL_BR_START 6: function do not return error, expect init error
  if (eFrameworkunifiedStatusOK != (*l_eStatus_work = start_process_launching(hApp))) {
  // LCOV_EXCL_BR_STOP
    // LCOV_EXCL_START 6: function do not return error, expect init error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("start_process_launching()");
    *l_isInitFail = true;
    // LCOV_EXCL_STOP
  }

  // LCOV_EXCL_BR_START 6: function do not return error, expect nsfw error
  if (eFrameworkunifiedStatusOK != (*l_eStatus_work = init_sysmem_monitor(hApp))) {
  // LCOV_EXCL_BR_STOP
    // LCOV_EXCL_START 6: function do not return error, expect nsfw error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("init_sysmem_monitor()");
    *l_isInitFail = true;
    // LCOV_EXCL_STOP
  }

  /// Internal thread monitoring start
  // LCOV_EXCL_BR_START 11:unexpected branch  // NOLINT(whitespace/line_length)
  m_GroupLaunchTimer->StartTimer(m_aTimerIDs[eSM_TIMER_HEARTBEAT_MONITOR],
      SS_HEARTBEAT_MONITOR_TIMER_CONFIG, 0,
      SS_HEARTBEAT_MONITOR_TIMER_CONFIG, 0);

  m_GroupLaunchTimer->StartTimer(
      m_aTimerIDs[eSM_TIMER_PROCESSLAUNCHER_MONITOR],
      SS_PROCESSLAUNCHER_MONITOR_TIMER_CONFIG, 0,
      SS_PROCESSLAUNCHER_MONITOR_TIMER_CONFIG, 0);

  m_GroupLaunchTimer->StartTimer(
      m_aTimerIDs[eSM_TIMER_NPP_STATUS_CHECK_MONITOR],
      SS_NPP_STATUS_CHECK_MONITOR_TIME_SEC, 0,
      SS_NPP_STATUS_CHECK_MONITOR_TIME_SEC, 0);
  // LCOV_EXCL_BR_STOP
}


/*****************************************************************************/
/**
 @ingroup: CSystemManager
 @brief: Initialization
 @note: .
 @param HANDLE hApp
 @return void
 */
/*****************************************************************************/
EFrameworkunifiedStatus CSystemManager::Initialize(HANDLE hApp) {
  bool l_isInitFail = false;
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  int ret;
//  PsmStartStatusT l_sts;
//  PsmComStsT      l_syscom_sts;
//  PsmDramStsT     l_dram_sts;

  // Need to call Lock System Initialize API once in the system
  ret = CL_LockSystemInit();
  if (ret != 0) {
    fprintf(stderr, "SS_SysManager/%s/Error: CL_LockSystemInit() errored: %d\n", __FUNCTION__, ret);
  }

  ret = CL_LockProcessInit();
  if (ret != 0) {
    fprintf(stderr, "SS_SysManager/%s/Error: CL_LockProcessInit() errored: %d\n", __FUNCTION__, ret);
    l_isInitFail = true;
  }

  SS_STATIC_ASERT(sizeof(LBM_RAM_t) <= SS_SYS_AREA_BOOT_MAX_SIZE);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  m_BootMemoryMapSize = SS_SYS_AREA_BOOT_MAX_SIZE;
//  if (EL_mem_getLength(EL_MEM_ID_SYS_BOOT) < m_BootMemoryMapSize) {
//    m_BootMemoryMapSize = static_cast<UI_32>(EL_mem_getLength(EL_MEM_ID_SYS_BOOT));
//  }
  m_BootMemory = &m_Lbm_Ram;
  if (m_BootMemory == NULL) {  // LCOV_EXCL_BR_LINE 200: m_BootMemory always not null
    // LCOV_EXCL_START 200: m_BootMemory always not null
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    m_BootMemoryMapSize = 0;
    fprintf(stderr, "SS_SysManager/%s/ERROR:EL_mem_getLength\n", __FUNCTION__);
    // LCOV_EXCL_STOP
  }

//  ret = PsmGetStartStatusPower(hApp, &l_sts);
//  if ((ret == 0) && ((l_sts >= PSM_START_STATUS_FACTRESET) && (l_sts < PSM_START_STATUS_MAX))) {
//    m_BootMemory->sts = m_PsmStartStatus_to_SysupCndMap[static_cast<UI_32>(l_sts)];
//  } else {
//    (void)fprintf(stderr, "SS_SysManager/%s/ERROR:start status not get\n", __FUNCTION__);
//  }
//  ret = PsmGetSyscomStatusPower(hApp, &l_syscom_sts);
//  if ((ret == 0) && ((l_syscom_sts >= PSM_COM_STS_NORMAL) && (l_syscom_sts < PSM_COM_STS_MAX))) {
//    m_BootMemory->syscomSts = m_PsmComSts_to_SubcpuStsMap[static_cast<UI_32>(l_syscom_sts)];
//  } else {
//    (void)fprintf(stderr, "SS_SysManager/%s/ERROR:communication status between mainsoc-sys not get\n", __FUNCTION__);
//  }
//  ret = PsmGetDramPowerSupplyAbnormalityCheckResultPower(hApp, &l_dram_sts);
//  if ((ret == 0) && ((l_dram_sts >= PSM_DRAM_STS_NORMAL) && (l_dram_sts < PSM_DRAM_STS_MAX))) {
//    m_BootMemory->pwSts             = m_PsmDramSts_to_SdramStsMap[static_cast<UI_32>(l_dram_sts)];
//    m_BootMemory->ramjudgeSts       = m_PsmDramSts_to_RamjudgeStsMap[static_cast<UI_32>(l_dram_sts)];
//    m_BootMemory->dram_self_refresh = m_PsmDramSts_to_SelfRefreshMap[static_cast<UI_32>(l_dram_sts)];
//  } else {
//    (void)fprintf(stderr, "SS_SysManager/%s/ERROR:dram power supply abnormality check result not get\n", __FUNCTION__);
//  }

  if (SYSUP_CND_FACTRESET == m_BootMemory->sts) {
    if (eFrameworkunifiedStatusOK != (l_eStatus = PerformMountResetAll())) {
      fprintf(stderr, "SS_SysManager/%s/Error: PerformMountResetAll() errored: %d\n", __FUNCTION__, l_eStatus);
    }
    SetDramStatusInfo(SYSUP_DRAM_NORMAL);
  } else {
    if (eFrameworkunifiedStatusOK != (l_eStatus = PerformMountNand())) {  // LCOV_EXCL_BR_LINE 200: PerformMountNand return always OK
      // LCOV_EXCL_START 200: PerformMountNand return always OK
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      fprintf(stderr, "SS_SysManager/%s/Error: PerformMountNand() errored: %d\n", __FUNCTION__, l_eStatus);
      l_isInitFail = true;
      // LCOV_EXCL_STOP
    }
  }

  BOOL isIllReset = (m_BootMemory->syscomSts == SUBCPU_STS_COMNG) ? TRUE : FALSE;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  bool isMountRamd = false;

  if (isIllReset || ("elog" == m_bootOpt)) {
    fprintf(stderr, "SS_SysManager/%s/Error: ILGRST LOG SAVE\n", __FUNCTION__);
    isMountRamd = StoreDebugLogs(SS_STORELOGS_ILLEGAL);
  } else {
    isMountRamd = StoreDebugLogs(SS_STORELOGS_ACCOFFON);
  }

  if (SDRAM_STS_BUPNG == m_BootMemory->pwSts) {
    l_eStatus = PerformMountResetRamd(hApp, isMountRamd);
    if (eFrameworkunifiedStatusOK != l_eStatus) {
      fprintf(stderr, "SS_SysManager/%s/Error: PerformMountResetRamd() errored: %d\n", __FUNCTION__, l_eStatus);
    }
  } else {
    if (isMountRamd == false) {
      if (eFrameworkunifiedStatusOK != (l_eStatus = PerformMountRamd())) {
//        fprintf(stderr, "SS_SysManager/%s/Error: PerformMountRamd() errored: %d\n", __FUNCTION__, l_eStatus);
        l_isInitFail = true;
      }
    }
  }

  FRAMEWORKUNIFIED_SET_ZONES();
  NsLogSetLogMethod(LPRINT | LMSGQ);
  // FRAMEWORKUNIFIEDLOG can not be used until this line

  m_ClProcessSigFd = CL_ProcessInit();  // LCOV_EXCL_BR_LINE 4: nsfw error
  if (m_ClProcessSigFd == -1) {  // LCOV_EXCL_BR_LINE 4: nsfw error
    // LCOV_EXCL_START 4: nsfw error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: CL_ProcessInit()");
    l_isInitFail = true;
    // LCOV_EXCL_STOP
  }

  ret = CL_MonitorInit(CL_MONITOR_INIT_SYSTEM);
  if(ret != 0) {
    fprintf(stderr, "SS_SysManager/%s/Error: CL_MonitorInit() errored: %d\n", __FUNCTION__, ret);
    l_isInitFail = true;
  }

  // Threads MUST NOT be started until this line
// ===== Initialization No.1
  Initialize_memory(hApp, &l_isInitFail, &l_eStatus, isIllReset);

// ===== Initialization No.2
  Initialize_callbacks(hApp, &l_isInitFail, &l_eStatus);

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");

  return l_eStatus;
}

EFrameworkunifiedStatus CSystemManager::SetEnvVariableForVupMode(BOOL bIsVupMode) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  const CHAR SmVupModeEnvVariable[] = "SM_VUP_MODE";
  const CHAR *l_pValue = bIsVupMode ? "y" : "n";  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  if (0 != setenv(SmVupModeEnvVariable, l_pValue, 1)) {  // LCOV_EXCL_BR_LINE 5: standard lib error
    // LCOV_EXCL_START 5: standard lib error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    SS_ASERT_ERRNO(0);
    l_eStatus = eFrameworkunifiedStatusFail;
    // LCOV_EXCL_STOP
  }

  return l_eStatus;
}

int CSystemManager::CreateRtUsb(void) {// LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  int pid = -1;
  int ret;

  ret = mkdir("/var/run", 0775);
  if ((ret < 0) && (errno != EEXIST)) {
    return -1;
  }
  pid = fork();
  switch (pid) {
    case 0: /* child process */
    {
      struct sched_param param;
      int policy;
      const char *exec;
      exec = const_cast<char *>(TR_USB_PATH);
      param.sched_priority = 0;
      policy = SCHED_OTHER;
      if (sched_setscheduler(getpid(), policy, &param) < 0) {
        SS_ASERT(0);
        _exit(-1);
      }
      if (setpriority(PRIO_PROCESS, getpid(), 0) < 0) {
        SS_ASERT(0);
        _exit(-1);
      }
      // exec
      if (execlp(exec, exec, NULL) < 0) {
        SS_ASERT(0);
        _exit(-1);
      }
      _exit(0);
    }
    break;

    case -1: /* error */
      SS_ASERT(0);
      goto ERROR;

    default: /* parent process */
      break;
  }

ERROR:
  return pid;
}// LCOV_EXCL_STOP

#define SYSLOGD_PATH "/usr/bin/syslogd"
#define SYSLOGD_NFS_PATH "/tmp/syslogd"
#define SYSLOG_LIMIT_FSIZE (249 * 1024);  // 249KB
int CSystemManager::CreateSyslogd(void) {// LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  int pid = -1;
  int ret;

  ret = mkdir("/var/run", 0775);
  if ((ret < 0) && (errno != EEXIST)) {
    return -1;
  }
  {
    // Suppress Warning : logrotate
    //  error: stat of /ramd/log/frameworkunifiedlog/syslog/syslog.log failed: No such file or directory
    int fd;
    fd = open("/ramd/log/frameworkunifiedlog/syslog/syslog.log", O_CREAT | O_RDWR | O_CLOEXEC, 0644);
    if (fd != -1) {
      AGL_ASSERT_NOT_TESTED();
      close(fd);
    }
  }

  pid = fork();
  switch (pid) {
  case 0:  // child process
  {
    struct sched_param param;
    int policy;
    const char *exec;
    struct rlimit rlim;
    sigset_t mask;
    const char* nfsenv = getenv("AGL_NFS");
    if (nfsenv && (strcmp(nfsenv, "y") == 0)) {
      exec = const_cast<char *>(SYSLOGD_NFS_PATH);
    } else {
      exec = const_cast<char *>(SYSLOGD_PATH);
    }
    // file size
    if (sigaddset(&mask, SIGXFSZ) < 0) {  // RLIMIT_FSIZE
      SS_ASERT(0);
      _exit(-1);
    }
    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) {
      SS_ASERT(0);
      _exit(-1);
    }
    getrlimit(RLIMIT_FSIZE, &rlim);
    rlim.rlim_cur = SYSLOG_LIMIT_FSIZE;
    if (setrlimit(RLIMIT_FSIZE, &rlim) < 0) {
      SS_ASERT(0);
      _exit(-1);
    }
    param.sched_priority = 0;
    policy = SCHED_OTHER;
    if (sched_setscheduler(getpid(), policy, &param) < 0) {
      SS_ASERT(0);
      _exit(-1);
    }
    if (setpriority(PRIO_PROCESS, getpid(), 0) < 0) {
      SS_ASERT(0);
      _exit(-1);
    }
    // exec
    if (execlp(exec, exec, "-n", "-f", "/usr/agl/conf/syslogd/syslog.conf", NULL) < 0) {
      SS_ASERT(0);
      _exit(-1);
    }
    _exit(0);
  }
    break;

  case -1:  // error
    SS_ASERT(0);
    goto ERROR;

  default:  // parent process
    break;
  }

  ERROR: return pid;
}// LCOV_EXCL_STOP

bool CSystemManager::StoreDebugLogs(SS_STORELOGS_OPE_TYPE type) {
  bool isMountRamd = false;
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  int pid;

  if (m_BootMemory->ramjudgeSts != RAMJUDGE_STS_NOMAL) {
//    fprintf(stderr, "SS_SysManager/%s/Error: RAM JUDGE NG\n", __FUNCTION__);
    goto ERROR;
  } else if (access(SS_SM_TEMP_FILE_FOR_STORE_LOGS, F_OK) == 0) {
    fprintf(stderr, "SS_SysManager/%s/Error: Skip StoreDebugLogs\n", __FUNCTION__);
    goto ERROR;
  }
  if (eFrameworkunifiedStatusOK != (l_eStatus = PerformMountRamd())) {
    fprintf(stderr, "SS_SysManager/%s/Error: PerformMountRamd() errored: %d\n", __FUNCTION__, l_eStatus);
    goto ERROR;
  }

  isMountRamd = true;

  if (type == SS_STORELOGS_ILLEGAL) {
  // In order to obtain the anomaly reboot count, nv information is acquired first when ILG is reset.
  // * Duplicate initialization and recovery operations to simplify design.
  // Therefore, SystemInitialize() must operate normally even if it is called more than once.
    DRAM_BACKUP_STATE l_bkupState =
        (SDRAM_STS_BUPNG == m_BootMemory->pwSts) ? DRAM_BACKUP_STATE_NG : DRAM_BACKUP_STATE_OK;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    ROM_AccessIf rom_access;
    if (eFrameworkunifiedStatusOK != (l_eStatus = rom_access.SystemInitialize(l_bkupState))) {
      fprintf(stderr, "SS_SysManager/%s/Error: rom_access.SystemInitialize(%d)\n", __FUNCTION__, l_eStatus);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
      goto ERROR;
    }

    rom_access.Initialize();  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

    UI_32 l_SigValue;

    if ((eFrameworkunifiedStatusOK != GetSignature(rom_access, &l_SigValue))
        || (l_SigValue != NVM_VALID_SIGNATURE)) {
      fprintf(stderr, "SS_SysManager/%s/Error: NVM SIGERR\n", __FUNCTION__);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    } else {
      UI_32 l_ErrLogCount;

      if (eFrameworkunifiedStatusOK != GetErrLogCount(rom_access, l_ErrLogCount)) {
        fprintf(stderr, "SS_SysManager/%s/Error: GetErrLogCount()\n", __FUNCTION__);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
      } else {
        if (l_ErrLogCount >= SS_SM_ERR_LOGGING_LIMIT) {
          fprintf(stderr, "SS_SysManager/%s/Error: Skip StoreDebugLogs by continuous error \n", __FUNCTION__);
          goto ERROR;
        }

        l_ErrLogCount++;
        if (eFrameworkunifiedStatusOK != SetErrLogCount(rom_access, l_ErrLogCount)) {
          fprintf(stderr, "SS_SysManager/%s/Error: SetErrLogCount()\n", __FUNCTION__);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
        }
      }
    }
  } else if (type == SS_STORELOGS_ACCOFFON) {
    if (0 != unlink(SS_LOGGER_SAVE_INTERFACEUNIFIEDLOG_FLAG)) {
      fprintf(stderr, "SS_SysManager/%s/Error: ACCOFFON LOG SAVE\n", __FUNCTION__);
    } else {
      goto ERROR;
    }
  }

  pid = fork();
  if (pid == 0) {
    int fd = open(SS_SM_TEMP_FILE_FOR_STORE_LOGS,
        O_CREAT | O_TRUNC | O_RDWR, 00664);
    if (-1 == fd) {  // LCOV_EXCL_BR_LINE 5:fd must not be -1
      fprintf(stderr, "SS_SysManager/%s/Error: Failed open %s errno: %d\n",
              __FUNCTION__, SS_SM_TEMP_FILE_FOR_STORE_LOGS, errno);
    } else {
      fsync(fd);
      close(fd);
    }
    FRAMEWORKUNIFIED_SET_ZONES();  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
    NsLogSetLogMethod(LPRINT | LMSGQ);

    if (eFrameworkunifiedStatusOK != (l_eStatus = SS_LoggerStoreLogs(type))) {  // LCOV_EXCL_BR_LINE 200: always return OK
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      fprintf(stderr, "SS_SysManager/%s/Error: SS_LoggerStoreDebugLogs() errored: %d\n", __FUNCTION__, l_eStatus);  // LCOV_EXCL_LINE 200: always return OK  // NOLINT(whitespace/line_length)
    }
    // Close accessing the RAMD by FRAMEWORKUNIFIEDLOG for RAMD initialization
    NsForceClose();

    if (0 != unlink(SS_SM_TEMP_FILE_FOR_STORE_LOGS)) {
      SS_ASERT_ERRNO(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    }

    exit(0);
  } else if (pid == -1) {  // LCOV_EXCL_BR_LINE 5:fork error case
    // LCOV_EXCL_START 5:fork error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    fprintf(stderr, "SS_SysManager/%s/Error: ASSERT %d\n", __FUNCTION__, __LINE__);
    // LCOV_EXCL_STOP
  } else {
    if (-1 == waitpid(pid, NULL, 0)) {    // LCOV_EXCL_BR_LINE 5:waitpid error case
      // LCOV_EXCL_START 5:waitpid error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      fprintf(stderr, "SS_SysManager/%s/Error: ASSERT %d\n", __FUNCTION__, __LINE__);
      // LCOV_EXCL_STOP
    }
  }
 ERROR:
  return isMountRamd;
}

#define SM_MNT_P_NAND_LOG        "/mnt_nv/mmcblk0p11"
#define SM_MNT_P_NAND_PTDATE     "/mnt_nv/mmcblk0p17"
#define SM_MNT_P_NAND_LOG2       "/mnt_nv/mmcblk0p18"
#define SM_MNT_P_NAND_NV         "/nv"
#define SM_MNT_P_NAND_NV_SYSTEM  "/nv_system"
#define SM_MNT_P_RAMD_SYSTEMMANAGERLOG     "/ramd/log/frameworkunifiedlog"
#define SM_MNT_P_RAMD_BKUP       "/ramd/bkup"

#define SM_MNT_P_RAMD_SDL        "/ramd/sdl"

EFrameworkunifiedStatus CSystemManager::PerformMountResetAll(void) {  // LCOV_EXCL_START 6: Because the condition cannot be set
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
//  MntErr l_eMnt;

//  l_eMnt = MntResetAll();
//  if (MNT_SUCCESS != l_eMnt) {
//    fprintf(stderr, "SS_SysManager/%s/Error: MntResetAll(%d)\n", __FUNCTION__, l_eMnt);
//    l_eStatus = eFrameworkunifiedStatusFail;
//  }

  return l_eStatus;
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus CSystemManager::PerformMountResetRamd(const HANDLE h_app, const bool isMountRamd) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
//  MntErr l_eMnt;
  bool l_reset = false;
  bool l_reset_sdl = false;
//  PsmDramStsT l_dram_sts;
  int ret;

//  l_eMnt = MntReset(SM_MNT_P_RAMD_SYSTEMMANAGERLOG);
//  if (MNT_SUCCESS != l_eMnt) {
//    fprintf(stderr, "SS_SysManager/%s/Error: MntReset(%s:%d)\n", __FUNCTION__, SM_MNT_P_RAMD_SYSTEMMANAGERLOG, l_eMnt);
//    l_eStatus = eFrameworkunifiedStatusFail;
//  }
  // Although RomAccessIf.progUpdate should be used to determine the initial start after updating Changing the
  // RomAccessIf initialization timings at the end of development is risky and should be judged by SYSUP_DRAM_DISCARD.
//  ret = PsmGetDramPowerSupplyAbnormalityCheckResultPower(h_app, &l_dram_sts);
//  if ((ret == 0) && ((l_dram_sts >= PSM_DRAM_STS_NORMAL) && (l_dram_sts < PSM_DRAM_STS_MAX))) {
//    m_BootMemory->pwSts             = m_PsmDramSts_to_SdramStsMap[static_cast<UI_32>(l_dram_sts)];
//    m_BootMemory->ramjudgeSts       = m_PsmDramSts_to_RamjudgeStsMap[static_cast<UI_32>(l_dram_sts)];
//    m_BootMemory->dram_self_refresh = m_PsmDramSts_to_SelfRefreshMap[static_cast<UI_32>(l_dram_sts)];
//  } else {
//    LOG_ERROR("PsmGetDramPowerSupplyAbnormalityCheckResultPower()");
//  }

  if ((SYSUP_DRAM_DISCARD == GetDramStatusInfo(h_app))
      && (m_BootMemory->ramjudgeSts == RAMJUDGE_STS_NOMAL
          && m_BootMemory->dram_self_refresh == SELF_REFRESH_OK)) {
    if (!isMountRamd) {
//      l_eMnt = MntMountEx(SM_MNT_P_RAMD_BKUP);
//      if (MNT_SUCCESS != l_eMnt) {
//        fprintf(stderr, "SS_SysManager/%s/Error: MntMountEX(%s:%d)\n", __FUNCTION__, SM_MNT_P_RAMD_BKUP, l_eMnt);
//        l_reset = true;
//        l_eStatus = eFrameworkunifiedStatusFail;
//      }

      if (CheckMountPoint(SM_MNT_P_RAMD_SDL)) {
//        l_eMnt = MntMountEx(SM_MNT_P_RAMD_SDL);
//        if (MNT_SUCCESS != l_eMnt) {
//          fprintf(stderr, "SS_SysManager/%s/Error: MntMountEX(%s:%d)\n", __FUNCTION__, SM_MNT_P_RAMD_SDL, l_eMnt);
//          l_reset_sdl = true;
//          l_eStatus = eFrameworkunifiedStatusFail;
//        }
      }

    }
  } else {
    l_reset = true;

    if (CheckMountPoint(SM_MNT_P_RAMD_SDL)) {
      l_reset_sdl = true;
    }

  }

  /* SM_MNT_P_RAMD_BKUP */
  if (l_reset) {
//    l_eMnt = MntReset(SM_MNT_P_RAMD_BKUP);
//    if (MNT_SUCCESS != l_eMnt) {  // LCOV_EXCL_BR_LINE 6: Due to Initialize processing
      // LCOV_EXCL_START 6: Due to Initialize processing
//      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//      fprintf(stderr, "SS_SysManager/%s/Error: MntReset(%s:%d)\n", __FUNCTION__, SM_MNT_P_RAMD_BKUP, l_eMnt);
//      l_eStatus = eFrameworkunifiedStatusFail;
      // LCOV_EXCL_STOP
//    }
  }

  /* SM_MNT_P_RAMD_SDL */
  // Additional correspondence of SDL is NA only : For common code, Add "Check Mount Point"
  if (l_reset_sdl) {
    if (CheckMountPoint(SM_MNT_P_RAMD_SDL)) {  // LCOV_EXCL_BR_LINE 6: l_reset_sdl==True guarantees CheckMountPoint is True  // NOLINT(whitespace/line_length)
//      l_eMnt = MntReset(SM_MNT_P_RAMD_SDL);
//      if (MNT_SUCCESS != l_eMnt) {
//        fprintf(stderr, "SS_SysManager/%s/Error: MntReset(%s:%d)\n", __FUNCTION__, SM_MNT_P_RAMD_SDL, l_eMnt);
//        l_eStatus = eFrameworkunifiedStatusFail;
//      }
    }
  }

  return l_eStatus;
}

EFrameworkunifiedStatus CSystemManager::PerformMountNand(void) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

// Delete in order to move mount to init quickly

  return l_eStatus;
}

EFrameworkunifiedStatus CSystemManager::PerformMountRamd(void) {

//#define SS_SM_MOUNT_RAMD_MAX (2)

  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

#if 0
  const char* l_mountlist[SS_SM_MOUNT_RAMD_MAX] = { SM_MNT_P_RAMD_SYSTEMMANAGERLOG, SM_MNT_P_RAMD_BKUP };
  MntErr l_eMnt[SS_SM_MOUNT_RAMD_MAX];

  MntMultiMountEx(l_mountlist, SS_SM_MOUNT_RAMD_MAX, l_eMnt);
  for (int i = 0; i < SS_SM_MOUNT_RAMD_MAX; i++) {
    if (MNT_SUCCESS != l_eMnt[i] && MNT_ERR_MOUNTED != l_eMnt[i]) {
//      fprintf(stderr, "SS_SysManager/%s/Error: MntMultiMountEx(%s:%d)\n",
//          __FUNCTION__, l_mountlist[i], l_eMnt[i]);
      MntErr l_eMntReset;
      l_eMntReset = MntReset(l_mountlist[i]);
      if (MNT_SUCCESS != l_eMntReset) {
//        fprintf(stderr, "SS_SysManager/%s/Error: MntReset(%s:%d)\n",
//            __FUNCTION__, l_mountlist[i], l_eMntReset);
        l_eStatus = eFrameworkunifiedStatusFail;
      }
    }
  }
#endif
  const char* l_mountlist[] = { SM_MNT_P_RAMD_SYSTEMMANAGERLOG,
                                SM_MNT_P_RAMD_BKUP };
  const char* l_mountlist_sdl[] = { SM_MNT_P_RAMD_SYSTEMMANAGERLOG,
                                    SM_MNT_P_RAMD_BKUP,
                                    SM_MNT_P_RAMD_SDL };
  const char** l_mlist = l_mountlist;
  int ramd_mount_max = sizeof (l_mountlist) / sizeof (l_mountlist[0]);

  // Additional correspondence of SDL is NA only : For common code, Add "Check Mount Point"
  if (CheckMountPoint(SM_MNT_P_RAMD_SDL)) {
    l_mlist = l_mountlist_sdl;
    ramd_mount_max = sizeof (l_mountlist_sdl) / sizeof (l_mountlist_sdl[0]);
  }

//  MntErr l_eMnt[ramd_mount_max];

//  MntMultiMountEx((const char**)l_mlist, ramd_mount_max, l_eMnt);
//  for (int i = 0; i < ramd_mount_max; i++) {
//    if (MNT_SUCCESS != l_eMnt[i] && MNT_ERR_MOUNTED != l_eMnt[i]) {
//      fprintf(stderr, "SS_SysManager/%s/Error: MntMultiMountEx(%s:%d)\n", __FUNCTION__, l_mlist[i], l_eMnt[i]);
//      MntErr l_eMntReset;
//      l_eMntReset = MntReset(l_mlist[i]);
//      if (MNT_SUCCESS != l_eMntReset) {
//        fprintf(stderr, "SS_SysManager/%s/Error: MntReset(%s:%d)\n", __FUNCTION__, l_mlist[i], l_eMntReset);
//        l_eStatus = eFrameworkunifiedStatusFail;
//      }
//    }
//  }

  return l_eStatus;
}

EFrameworkunifiedStatus CSystemManager::PerformUmountNand(void) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
//  MntErr l_eMnt;

//  l_eMnt = MntUmountEx(SM_MNT_P_NAND_PTDATE);
//  if (MNT_SUCCESS != l_eMnt) {
//    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: MntUmountEx(%s:%d)",
//        SM_MNT_P_NAND_PTDATE, l_eMnt);
//    l_eStatus = eFrameworkunifiedStatusFail;
//  }

//  l_eMnt = MntUmountEx(SM_MNT_P_NAND_NV);
//  if (MNT_SUCCESS != l_eMnt) {
//    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: MntUmountEx(%s:%d)",
//        SM_MNT_P_NAND_NV, l_eMnt);
//    l_eStatus = eFrameworkunifiedStatusFail;
//  }

//  l_eMnt = MntUmountEx(SM_MNT_P_NAND_LOG2);  // /nv/log2
//  if (MNT_SUCCESS != l_eMnt) {
//    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: MntUmountEx(%s:%d)",
//        SM_MNT_P_NAND_LOG2, l_eMnt);
//    l_eStatus = eFrameworkunifiedStatusFail;
//  }

//  l_eMnt = MntUmountEx(SM_MNT_P_NAND_LOG);  // /nv/log
//  if (MNT_SUCCESS != l_eMnt) {
//    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: MntUmountEx(%s:%d)",
//        SM_MNT_P_NAND_LOG, l_eMnt);
//    l_eStatus = eFrameworkunifiedStatusFail;
//  }

//  l_eMnt = MntUmountEx(SM_MNT_P_NAND_NV_SYSTEM);
//  if (MNT_SUCCESS != l_eMnt) {
//    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: MntUmountEx(/nv_system:%d)",
//        l_eMnt);
//    l_eStatus = eFrameworkunifiedStatusFail;
//  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus CSystemManager::PerformUmountRamd(void) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
//  MntErr l_eMnt;


  /* SM_MNT_P_RAMD_BKUP */
  PreLaunchModuleListIter l_itr;
  for (l_itr = m_PreLaunchModuleList.begin();
      m_PreLaunchModuleList.end() != l_itr; l_itr++) {
    if (("syslogd" == l_itr->name) && (-1 != l_itr->pid)) {  // LCOV_EXCL_BR_LINE 200:prelaunch mode is not valid at UT test //NOLINT (whitespace/line_length)
      // LCOV_EXCL_START 200 :prelaunch mode is not valid at UT test
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      if (0 != kill(l_itr->pid, SS_SM_KILL_SIGNAL)) {  // LCOV_EXCL_BR_LINE 5:kill error case
        SS_ASERT_ERRNO(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
      } else {
        SS_ASERT_ERRNO(l_itr->pid == waitpid(l_itr->pid, NULL, 0));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
      }
      // LCOV_EXCL_STOP
    }
  }

//  l_eMnt = MntUmountEx(SM_MNT_P_RAMD_BKUP);
//  if (MNT_ERR_BUSY == l_eMnt) {
//    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: MntUmountEx(%s) BUSY", SM_MNT_P_RAMD_BKUP);

    ModuleLaunchListIter l_ModuleListIter;
    l_eStatus = GetModuleIterator(MN_NS_BACKUPMGR, l_ModuleListIter);
    if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 200 : BACKUP cannot be error
      // LCOV_EXCL_START 200 : BACKUP cannot be error
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: %s not found", MN_NS_BACKUPMGR);  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
      // LCOV_EXCL_STOP
    } else if (0 >= l_ModuleListIter->pid) {
      SS_ASERT_ERRNO(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
      l_eStatus = eFrameworkunifiedStatusFail;
    } else {
      int ret;

      ret = kill(l_ModuleListIter->pid, SS_SM_TERMINATE_SIGNAL);
      if (0 != ret) {  // LCOV_EXCL_BR_LINE 5:kill error case
        // LCOV_EXCL_START 5: kill error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        SS_ASERT_ERRNO(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
        l_eStatus = eFrameworkunifiedStatusFail;
        // LCOV_EXCL_STOP
      } else {
        pid_t retPid;

        retPid = waitpid(l_ModuleListIter->pid, NULL, 0);
        if (l_ModuleListIter->pid != retPid) {  // LCOV_EXCL_BR_LINE 5:waitpid error case
          // LCOV_EXCL_START 5: kill error case
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          SS_ASERT_ERRNO(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
          l_eStatus = eFrameworkunifiedStatusFail;
          // LCOV_EXCL_STOP
        } else {
//          l_eMnt = MntUmountEx(SM_MNT_P_RAMD_BKUP);
//          if (MNT_SUCCESS != l_eMnt) {
//            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
//                " Error: MntUmountEx(%s:%d)",
//                SM_MNT_P_RAMD_BKUP, l_eMnt);
//            l_eStatus = eFrameworkunifiedStatusFail;
//          }
        }
      }
    }
//  } else if (MNT_SUCCESS != l_eMnt) {
//    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: MntUmountEx(%s:%d)", SM_MNT_P_RAMD_BKUP, l_eMnt);
//  }


  /* SM_MNT_P_RAMD_SDL */
  // Additional correspondence of SDL is NA only : For common code, Add "Check Mount Point"
  if (CheckMountPoint(SM_MNT_P_RAMD_SDL)) {
//    l_eMnt = MntUmountEx(SM_MNT_P_RAMD_SDL);
//    if (MNT_SUCCESS != l_eMnt) {
//      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: MntUmountEx(%s:%d)", SM_MNT_P_RAMD_SDL, l_eMnt);
//    }
  }


  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus CSystemManager::PerformForceUmountRamd(void) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  ModuleLaunchListIter l_ModuleListIter;

  l_eStatus = GetModuleIterator(MN_NS_BACKUPMGR, l_ModuleListIter);
  if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 200 : cannot be error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: %s not found", MN_NS_BACKUPMGR);  // LCOV_EXCL_LINE:200 cannot be error
  } else if (0 >= l_ModuleListIter->pid) {
    SS_ASERT_ERRNO(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    l_eStatus = eFrameworkunifiedStatusFail;
  } else {
    if (0 != kill(l_ModuleListIter->pid, SS_SM_TERMINATE_SIGNAL)) {
      SS_ASERT_ERRNO(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
      l_eStatus = eFrameworkunifiedStatusFail;
    } else {
      if (l_ModuleListIter->pid != waitpid(l_ModuleListIter->pid, NULL, 0)) {  // LCOV_EXCL_BR_LINE 5:waitpid error case  //NOLINT (whitespace/line_length)
        // LCOV_EXCL_START 5: waitpid error
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        SS_ASERT_ERRNO(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
        l_eStatus = eFrameworkunifiedStatusFail;
        // LCOV_EXCL_STOP
      } else {
//        MntErr l_eMnt = MntUmountEx2(SM_MNT_P_RAMD_BKUP, MNT_UMOUNT_NOLOG);
//        if (MNT_SUCCESS != l_eMnt) {
//          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: MntUmountEx(%s:%d)",
//              SM_MNT_P_RAMD_BKUP, l_eMnt);
//          l_eStatus = eFrameworkunifiedStatusFail;
//        }
      }
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus CSystemManager::PerformFinalUmount(BOOL bIsNeedLog) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
//  MntErr l_eMnt;
//  MntLogMode l_MntMode = MNT_LOGOFF;
  ModuleLaunchListIter l_ModuleListIter;

  if (eFrameworkunifiedStatusOK != GetModuleIterator(MN_SS_LOGGERSRV, l_ModuleListIter)) {  // LCOV_EXCL_BR_LINE 200:LOGSERVER is not valid at UT
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: %s not found", MN_SS_LOGGERSRV);  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
  } else if (0 >= l_ModuleListIter->pid) {  // LCOV_EXCL_BR_LINE 200:LOGSERVER is not valid at UT
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    SS_ASERT(0);  // LCOV_EXCL_LINE 200: LOGSERVER is not valid at UT
  } else {
    // LCOV_EXCL_START 200: LOGSERVER is not valid at UT
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    if (0 != kill(l_ModuleListIter->pid, SS_SM_KILL_SIGNAL)) {  // LCOV_EXCL_BR_LINE 5: standard lib error
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      SS_ASERT_ERRNO(0);  // LCOV_EXCL_LINE 5: standard lib error
    }
    // LCOV_EXCL_STOP
  }

  if (bIsNeedLog) {
//    l_MntMode = MNT_LOGON;
  }

//  l_eMnt = MntFinalUmount(l_MntMode);
//  if (MNT_SUCCESS != l_eMnt) {
//    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: MntFinalUmount(%d)", l_eMnt);
//    l_eStatus = eFrameworkunifiedStatusFail;
//  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

#define SS_SM_SCAC_DEVICE_NODE      "/dev/scac_driver"
#define SS_SM_SCAC_CMD_SHUTDOWN     0x3000

EFrameworkunifiedStatus CSystemManager::SecureChipOff() {
  static bool isOffDone = false;
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  int32_t fd;
  int32_t res;

  if (!isOffDone) {
    fd = open(SS_SM_SCAC_DEVICE_NODE, O_RDWR);
    if (fd < 0) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "open(%s) ERR", SS_SM_SCAC_DEVICE_NODE);
      l_eStatus = eFrameworkunifiedStatusFail;
    } else {
      res = ioctl(fd, SS_SM_SCAC_CMD_SHUTDOWN);
      if (res < 0) {  // LCOV_EXCL_BR_LINE 5: standard lib error
        // LCOV_EXCL_START 5: standard lib error
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "ioctl(SS_SM_SCAC_CMD_SHUTDOWN) ERR");
        l_eStatus = eFrameworkunifiedStatusFail;
        // LCOV_EXCL_STOP
      }

      close(fd);
    }

//    const EFrameworkunifiedStatus  l_ret = ResetSecurityIC();
//    if (eFrameworkunifiedStatusOK != l_ret) {
//      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "ResetSecurityIC ERR(%#X)", l_ret);
//      l_eStatus = eFrameworkunifiedStatusFail;
//    }

    isOffDone = true;
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "Skip Chip Off");
  }

  return l_eStatus;
}

UI_32 CSystemManager::InProgressStateToSendMsg() const {
  UI_32 l_iCmd = SS_SYSTEM_MANAGER_PROTOCOL_ENDING_INDEX;
  switch (m_SMCurrentState) {
  case SS_SM_APPS_PRE_START_IN_PROGRESS:
    l_iCmd = SS_SM_PRE_START;
    break;
  case SS_SM_APPS_PRE_STOP_IN_PROGRESS:
    l_iCmd = SS_SM_PRE_STOP;
    break;
  case SS_SM_APPS_BACKGROUND_START_IN_PROGRESS:
    l_iCmd = SS_SM_BACKGROUND_START;
    break;
  case SS_SM_APPS_BACKGROUND_STOP_IN_PROGRESS:
    l_iCmd = SS_SM_BACKGROUND_STOP;
    break;
  case SS_SM_APPS_START_IN_PROGRESS:
    l_iCmd = SS_SM_START;
    break;
  default:
    break;
  }
  return l_iCmd;
}

SMModuleState CSystemManager::InProgressStateToState() const {
  SMModuleState l_state = MODULE_STATE_INVALID;
  switch (m_SMCurrentState) {
  case SS_SM_APPS_PRE_START_IN_PROGRESS:
    l_state = MODULE_STATE_STARTED_PRE;
    break;
  case SS_SM_APPS_PRE_STOP_IN_PROGRESS:
    l_state = MODULE_STATE_STOPPED_PRE;
    break;
  case SS_SM_APPS_BACKGROUND_START_IN_PROGRESS:
    l_state = MODULE_STATE_STARTED_BACKGROUND;
    break;
  case SS_SM_APPS_BACKGROUND_STOP_IN_PROGRESS:
    l_state = MODULE_STATE_STOPPED_BACKGROUND;
    break;
  case SS_SM_APPS_START_IN_PROGRESS:
    l_state = MODULE_STATE_STARTED;
    break;
  default:
    break;
  }
  return l_state;
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup SetSystemModeInfoStruct
/// Read ( or initialize ) NVM and set the SystemModeInfo structure
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::SetSystemModeInfoStruct() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (m_UsingVMPlayer) {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " 'm_UsingVMPlayer' is True,"
        " using default SystemModeInfo values");
    m_SystemModeInfo.lastUserMode = epsumOFF;
    m_SystemModeInfo.limpHomeCutoff = epslhcDISABLED;
    m_SystemModeInfo.productionMode = epspmDISABLED;
    m_SystemModeInfo.transportMode = epstmDISABLED;
    m_SystemModeInfo.systemMode = epssinfNORMAL;
    m_DataResetMode = e_SS_SM_DATA_RESET_MODE_NONE;
    m_ResetCount = 0;
    m_ProgUpdateState = SS_SM_PROG_UPDATE_STATE_NONE;
    // Don't change the m_SystemModeInfo.startupStage.
    // It is set dynamically as System Manager initializes the _CWORD102_.
  } else {
    UI_32 SigValue;
    UI_32 * pSigValue = &SigValue;
    ROM_AccessIf l_cRomAccess;
    l_cRomAccess.Initialize();  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    bool isFast = true;

    if (eFrameworkunifiedStatusOK != (l_eStatus = GetSignature(l_cRomAccess, pSigValue))) {  // LCOV_EXCL_BR_LINE 6: For setting the initialization conditions at SystemManager startup // NOLINT(whitespace/line_length)
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      LOG_ERROR("GetSignature( l_cRomAccess, pSigValue)");  // LCOV_EXCL_LINE 6: For setting the initialization conditions at SystemManager startup
    } else {
      ESMBootModeInfo l_BootMode;
      if (SigValue != NVM_VALID_SIGNATURE) {
        FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "[SBX]NVM SIGERR");

        isFast = false;

        // LCOV_EXCL_BR_START 6: For setting the initialization conditions at SystemManager startup
        if (eFrameworkunifiedStatusOK != (l_eStatus =
            SetSignature(l_cRomAccess, NVM_VALID_SIGNATURE))) {
        // LCOV_EXCL_BR_STOP
          LOG_ERROR("SetSignature(NVM_VALID_SIGNATURE");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
        } else if (eFrameworkunifiedStatusOK
            != (l_eStatus = SetActiveFlashloader(l_cRomAccess, NEW_FLASHLOADER))) {
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          LOG_ERROR("SetActiveFlashloader(NEW_FLASHLOADER)");
        } else if (eFrameworkunifiedStatusOK
            != (l_eStatus = SetBootMode(l_cRomAccess, e_SS_SM_BOOT_MODE_APPLICATION))) {
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          LOG_ERROR("SetBootMode(e_SS_SM_BOOT_MODE_APPLICATION)");
        } else if (eFrameworkunifiedStatusOK
            != (l_eStatus = SetLastUserMode(l_cRomAccess, epsumON))) {
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          LOG_ERROR("SetLastUserMode(epsumON))");
        } else if (eFrameworkunifiedStatusOK
            != (l_eStatus = SetLimpHomeCutoffMode(l_cRomAccess, epslhcDISABLED))) {
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          LOG_ERROR("SetLimpHomeCutOffReqMode(epslhcDISABLED)");
        } else if (eFrameworkunifiedStatusOK
            != (l_eStatus = SetProductionMode(l_cRomAccess, epspmDISABLED))) {
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          LOG_ERROR("SetProductionMode(epspmDISABLED)");
        } else if (eFrameworkunifiedStatusOK
            != (l_eStatus = SetTransportMode(l_cRomAccess, epstmDISABLED))) {
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          LOG_ERROR("SetTransportMode(epstmDISABLED)");
        } else if (eFrameworkunifiedStatusOK
            != (l_eStatus = SetDataResetMode(l_cRomAccess, e_SS_SM_DATA_RESET_MODE_FACTORY))) {
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          LOG_ERROR("SetDataResetMode(e_SS_SM_DATA_RESET_MODE_FACTORY)");
        } else if (eFrameworkunifiedStatusOK
            != (l_eStatus = SetResetCount(l_cRomAccess, 0))) {
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          LOG_ERROR("SetResetCount(0)");
        } else if (eFrameworkunifiedStatusOK
            != (l_eStatus = SetLastIlgReset(l_cRomAccess, FALSE))) {
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          LOG_ERROR("SetLastIlgReset(LAST_ILGRESET_NORMAL)");
        } else if (eFrameworkunifiedStatusOK
            != (l_eStatus = SetProgUpdateState(l_cRomAccess,
                m_ProgUpdateState))) {
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          LOG_ERROR("SetProgUpdateState(NONE)");
        } else if (eFrameworkunifiedStatusOK
            != (l_eStatus = SetErrLogCount(l_cRomAccess, 0))) {
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          LOG_ERROR("SetErrLogCount(0)");
        } else if (eFrameworkunifiedStatusOK
            != (l_eStatus = SetNextWakeupType(l_cRomAccess,
                e_SS_SM_NEXT_WAKEUP_TYPE_NONE))) {
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          LOG_ERROR("SetNextWakeupType(NEXT_WAKEUP_TYPE_NONE)");
        } else {
          UI_8 buf[ROM_PRODUCT_PRIVATE_MAX];

          memset(buf, 0, sizeof(buf));

          if (eFrameworkunifiedStatusOK
              != (l_eStatus = SetProductPrivate(l_cRomAccess, buf))) {
            LOG_ERROR("SetProductPrivate(fill_0)");
          }
        }
      }  // End if NVM had not yet been initialized

      if (eFrameworkunifiedStatusOK == l_eStatus) {  // LCOV_EXCL_BR_LINE 6: Because the condition of the first if statement is satisfied, the branch condition cannot be satisfied.
        // LCOV_EXCL_START 6: Because the condition of the first if statement is satisfied, the branch condition cannot be satisfied.
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        // Get Boot Mode ( BootMode determines SystemModeInfo )
        if (eFrameworkunifiedStatusOK
            != (l_eStatus = GetBootMode(l_cRomAccess, l_BootMode))) {
          LOG_ERROR("GetBootMode()");
        } else if (eFrameworkunifiedStatusOK
            != (l_eStatus = GetLastUserMode(l_cRomAccess,
                m_SystemModeInfo.lastUserMode))) {
          LOG_ERROR("GetLastUserMode()");
        } else if (eFrameworkunifiedStatusOK
            != (l_eStatus = GetLimpHomeCutoffMode(l_cRomAccess,
                m_SystemModeInfo.limpHomeCutoff))) {
          LOG_ERROR("GetLimpHomeCutoff()");
        } else if (eFrameworkunifiedStatusOK
            != (l_eStatus = GetProductionMode(l_cRomAccess,
                m_SystemModeInfo.productionMode))) {
          LOG_ERROR("GetProductionMode()");
        } else if (eFrameworkunifiedStatusOK
            != (l_eStatus = GetTransportMode(l_cRomAccess,
                m_SystemModeInfo.transportMode))) {
          LOG_ERROR("GetTransportMode()");
        } else if (eFrameworkunifiedStatusOK
            != (l_eStatus = GetDataResetMode(l_cRomAccess,
                m_DataResetMode, isFast))) {  // Call using Fast
          LOG_ERROR("GetDataResetMode()");
        } else if (eFrameworkunifiedStatusOK
            != (l_eStatus = GetResetCount(l_cRomAccess,
                m_ResetCount))) {
          LOG_ERROR("GetResetCount()");
        } else if (eFrameworkunifiedStatusOK
            != (l_eStatus = GetProgUpdateState(l_cRomAccess,
                m_ProgUpdateState))) {
          LOG_ERROR("GetProgUpdateState()");
        } else if (eFrameworkunifiedStatusOK
            != (l_eStatus = GetNextWakeupType(l_cRomAccess,
                m_NextWakeupType))) {
          LOG_ERROR("GetNextWakeupType()");
        }
        // LCOV_EXCL_STOP
      }  // End else either 1) Signature already set or 2) NVM successfully
        // initialized
        // ToDo Jay 2012 September 24 Save SystemModeInfo ( ala Progamming /
        //    Application ) or get overlap wrt BootMode straightened out
        // Convert BootMode as read from NVM to SystemModeInfo as expected by
        // PowerShadow. ToDo: Investigate consolidating the two types.
      if (eFrameworkunifiedStatusOK == l_eStatus) {
        switch (l_BootMode) {
        case e_SS_SM_BOOT_MODE_APPLICATION:
          m_SystemModeInfo.systemMode = epssinfNORMAL;
          break;
        case e_SS_SM_BOOT_MODE_PROGRAMMING:
          m_SystemModeInfo.systemMode = epssinfPROGRAMMING;
          break;
        default:
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
              " Error: Unknown l_BootMode value: 0x%X",
              l_BootMode);
          l_eStatus = eFrameworkunifiedStatusInvldParam;
          break;
        }
      }
    }  // End else a successful read of the NVM Signature field
  }  // End else ! m_UsingVMPlayer

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::SetSystemModeInfoStruct()

///////////////////////////////////////////////////////////////////////////////
/// \ingroup ClearDramBackupInfo
///
/// \param [in]
///
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::ClearDramBackupInfo(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  int ret;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "PsmClrRamJudgeFlgPower()");
//  ret = PsmClrRamJudgeFlgPower(hApp);
  if (ret != 0) {  // LCOV_EXCL_BR_LINE 11: Excluded due to gcov constraints (others)
    SS_ASERT_ERRNO(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    l_eStatus = eFrameworkunifiedStatusFail;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup CloseApplication
/// Process request to close Application
///
/// \param [in]
///
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::CloseApplication(HANDLE hApp) {  // LCOV_EXCL_START 6: Because the condition cannot be set
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus l_eStatus;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (eFrameworkunifiedStatusOK
      != (l_eStatus = FrameworkunifiedPublishServiceAvailability(hApp, FALSE))) {
    LOG_ERROR("FrameworkunifiedPublishServiceAvailability(hApp,FALSE)");
  } else if (eFrameworkunifiedStatusOK != (l_eStatus =
      FrameworkunifiedUnRegisterServiceAvailabilityNotification(hApp))) {
    LOG_ERROR("FrameworkunifiedUnRegisterServiceAvailabilityNotification(hApp)");
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Successful");
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::CloseApplication(HANDLE hApp)
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////////////////////////
/// \ingroup SMStateStartCompleteEntry
///     entry for SM module state(SS_SM_APPS_START_COMPLETE)
///
/// \param
///
/// \return EFrameworkunifiedStatus
///////////////////////////////////////////////////////////////////////////////
VOID CSystemManager::SMStateStartCompleteEntry(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus;

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
      "All %d groups have been sent system_manager protocol message, sending Start Complete to _CWORD56_",
      m_SystemStarter.get_id());
  CALL_AND_LOG_STATUS_IF_ERRORED(  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
      send_power_request_complete_response(hApp, "Startup"));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  CALL_AND_LOG_STATUS(PublishPowerOnOffNotification(hApp));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  if (m_InitialBoot == true) {
    m_InitialBoot = false;

    const ESMServiceWakeupStatus l_svcWupStatus = e_SS_SM_SVC_WAKEUP_STATUS_COMPLETE;

    CALL_AND_LOG_STATUS(  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
        FrameworkunifiedNPPublishNotification(hApp, NTFY_SSServiceWakeupStatus,  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
            &l_svcWupStatus, sizeof(ESMServiceWakeupStatus)));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

    // If all modules are restarted, a passive warning will be issued
    // indicating that Heartbeat is already running.

    UI_32 l_list_num = static_cast<UI_32>(m_HBList.size());
    if (SS_MAX_NUM_MODULES < l_list_num) {
      l_list_num = SS_MAX_NUM_MODULES;
      FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__,
          " Warn: SS_MAX_NUM_MODULES '%d' <  m_HBList.size '%d'",
          SS_MAX_NUM_MODULES, l_list_num);
    }

    CHAR send_data[sizeof(l_list_num)
        + (l_list_num * SS_SM_HB_MAX_PROC_NAME_SIZE)
        + sizeof(m_SMConfig.HBConfig)];
    CHAR* p_prm = &send_data[0];

    memcpy(p_prm, reinterpret_cast<char *>(&m_SMConfig.HBConfig), sizeof(m_SMConfig.HBConfig));
    p_prm = p_prm + sizeof(m_SMConfig.HBConfig);
    memcpy(p_prm, reinterpret_cast<char *>(&l_list_num), sizeof(l_list_num));
    p_prm = p_prm + sizeof(l_list_num);

    for (UI_32 i = 0; i < l_list_num; i++) {
      snprintf(p_prm, SS_SM_HB_MAX_PROC_NAME_SIZE, "%s", m_HBList[i].c_str());
      p_prm = p_prm + SS_SM_HB_MAX_PROC_NAME_SIZE;
    }
    l_eStatus = SendRequestToHeartBeat(hApp, SS_HEARTBEAT_START, &send_data, static_cast<UI_32>(sizeof(send_data)));  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus, "SendRequestToHeartBeat()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

    {
      ROM_AccessIf rom_access;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
      rom_access.Initialize();  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

      if (e_SS_SM_DATA_RESET_MODE_NONE != m_DataResetMode) {
        l_eStatus = SetDataResetMode(rom_access, e_SS_SM_DATA_RESET_MODE_NONE);  // LCOV_EXCL_BR_LINE 11: Excluded due to gcov constraints (others)

        if (eFrameworkunifiedStatusOK != l_eStatus) {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: SetDataResetMode(%d)", l_eStatus);
        } else {
          m_DataResetMode = e_SS_SM_DATA_RESET_MODE_NONE;
        }
      }
      if (m_ProgUpdateState != SS_SM_PROG_UPDATE_STATE_NONE) {
        m_ProgUpdateState = SS_SM_PROG_UPDATE_STATE_NONE;
        l_eStatus = SetProgUpdateState(rom_access, m_ProgUpdateState);  // LCOV_EXCL_BR_LINE 11: Excluded due to gcov constraints (others)
        if (eFrameworkunifiedStatusOK != l_eStatus) {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: SetProgUpdateState(%d)", l_eStatus);
        }
      }

      m_isIlgReset = FALSE;

      l_eStatus = SetLastIlgReset(rom_access, m_isIlgReset);  // LCOV_EXCL_BR_LINE 11: Excluded due to gcov constraints (others)
      if (eFrameworkunifiedStatusOK != l_eStatus) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: SetLastIlgReset(%d)", l_eStatus);
      }

      if (e_SS_SM_NEXT_WAKEUP_TYPE_NONE != m_NextWakeupType) {
        l_eStatus = SetNextWakeupType(rom_access, e_SS_SM_NEXT_WAKEUP_TYPE_NONE);  // LCOV_EXCL_BR_LINE 11: Excluded due to gcov constraints (others)
        if (eFrameworkunifiedStatusOK != l_eStatus) {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: SetNextWakeupType(%d)", l_eStatus);
        }
      }
    }

    {
      RAM_AccessIf ram_access;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
      RAM_SM_INFO_t info;

      l_eStatus = ram_access.getRamInfo(&info);  // LCOV_EXCL_BR_LINE 11: Excluded due to gcov constraints (others)
      if (eFrameworkunifiedStatusOK != l_eStatus) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: ram_access.getRamInfo(%d)", l_eStatus);
      }

      info.needRenotifyStartPrm = FALSE;
      info.lastResetStatus = e_SS_SM_RESET_STATUS_NONE;
      info.lastWakeupType = epsstWARMSTART;
      info.lastDramBackupStatus = e_SS_SM_DRAM_BACKUP_OK;
      memset(info.order_name, 0x00, sizeof(info.order_name));

      l_eStatus = ram_access.setRamInfo(&info);  // LCOV_EXCL_BR_LINE 11: Excluded due to gcov constraints (others)
      if (eFrameworkunifiedStatusOK != l_eStatus) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: ram_access.setRamInfo(%d)", l_eStatus);
      }
    }

    if (access(SS_SM_TEMP_FILE_FOR_STORE_LOGS, F_OK) == 0) {
      SS_ASERT_ERRNO(0 == unlink(SS_SM_TEMP_FILE_FOR_STORE_LOGS));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    }
  }

  for (vector<std::string>::iterator protectedSvc =
      m_productCustomPrm.protectedSvcs.begin();
      protectedSvc != m_productCustomPrm.protectedSvcs.end();
      protectedSvc++) {
    ModuleLaunchListIter l_ModuleListIter;
    l_eStatus = GetModuleIterator(protectedSvc->c_str(), l_ModuleListIter);
    if (eFrameworkunifiedStatusOK == l_eStatus) {
      if ((l_ModuleListIter->IsModuleState(MODULE_STATE_STARTED))
          || (l_ModuleListIter->IsModuleState(MODULE_STATE_LAUNCHED)
              && !l_ModuleListIter->is_start_required)) {
        char pathBuf[128];

        snprintf(pathBuf, 128, "/proc/%d/oom_score_adj", l_ModuleListIter->pid);  // NOLINT
        std::ofstream fo(pathBuf);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
        fo << OOM_SCORE_ADJ_MIN << endl;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
        fo.close();  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "set OOM_SCORE_ADJ_MIN(%s/%d)",
            l_ModuleListIter->name.c_str(), l_ModuleListIter->pid);
      } else {
        SS_ASERT(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
      }
    }
  }

  for (vector<std::string>::iterator groupRelaunchSvc =
      m_productCustomPrm.groupRelaunchSvcs.begin();
      groupRelaunchSvc != m_productCustomPrm.groupRelaunchSvcs.end();
      groupRelaunchSvc++) {
    ModuleLaunchListIter l_ModuleListIter;
    l_eStatus = GetModuleIterator(groupRelaunchSvc->c_str(), l_ModuleListIter);
    if (eFrameworkunifiedStatusOK == l_eStatus) {
      if ((l_ModuleListIter->IsModuleState(MODULE_STATE_STARTED))
          || (l_ModuleListIter->IsModuleState(MODULE_STATE_LAUNCHED)
              && !l_ModuleListIter->is_start_required)) {
        GroupRelaunchModuleParams l_param;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
        l_param.name = l_ModuleListIter->name.c_str();
        m_GroupRelaunchModuleList.push_back(l_param);

        if (l_ModuleListIter->retry_cnt > m_GroupRelaunchLimit) {
          m_GroupRelaunchLimit = l_ModuleListIter->retry_cnt;
        }

        FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "GroupRelaunchSvcs:%s(%d)",
            l_ModuleListIter->name.c_str(), l_ModuleListIter->pid);
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "%s is not Launched", groupRelaunchSvc->c_str());
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "%s is not exist", groupRelaunchSvc->c_str());
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "GroupRelaunchLimit:%d", m_GroupRelaunchLimit);

  SendDeferMsg(hApp);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
}  // End of VOID CSystemManager::SMStateStartCompleteEntry()

///////////////////////////////////////////////////////////////////////////////
/// \ingroup SMStateEntry
///     entry for SM module state
///
/// \param
///
/// \return EFrameworkunifiedStatus
///////////////////////////////////////////////////////////////////////////////
VOID CSystemManager::SMStateEntry(HANDLE hApp, SS_SMModuleState l_SMState) {
  switch (l_SMState) {
  case SS_SM_APPS_START_COMPLETE:
  case SS_SM_APPS_PRE_RUN_COMPLETE:
  case SS_SM_APPS_BACKGROUND_RUN_COMPLETE:
    SMStateStartCompleteEntry(hApp);
    break;
  default:
    break;
  }
}  // End of VOID CSystemManager::SMStateEntry(SS_SMModuleState l_SMState)

///////////////////////////////////////////////////////////////////////////////
/// \ingroup SMStateExit
///     exit for SM module state
///
/// \param
///
/// \return EFrameworkunifiedStatus
///////////////////////////////////////////////////////////////////////////////
VOID CSystemManager::SMStateExit(HANDLE hApp, SS_SMModuleState l_SMState) {
  switch (l_SMState) {
  case SS_SM_APPS_START_COMPLETE:
    break;
  default:
    break;
  }

}  // End of VOID CSystemManager::SMStateExit(SS_SMModuleState l_SMState)

///////////////////////////////////////////////////////////////////////////////
/// \ingroup SMSetState
///     set module state
///
/// \param
///
/// \return EFrameworkunifiedStatus
///////////////////////////////////////////////////////////////////////////////
VOID CSystemManager::SMSetState(HANDLE hApp, SS_SMModuleState l_SMNewState) {
  // FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "%s -> %s",
      GetStr(m_SMCurrentState).c_str(), GetStr(l_SMNewState).c_str());

  if (l_SMNewState != m_SMCurrentState) {
    SMStateExit(hApp, m_SMCurrentState);

    m_SMCurrentState = l_SMNewState;

    SMStateEntry(hApp, m_SMCurrentState);
  }
  // FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup register_all_notification_callbacks
///     register all notifications and callbacks with NP_NPS
///   this function gets called when NP_NPS reply launch complete
///
/// \param
///
/// \return EFrameworkunifiedStatus
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::register_all_notification_callbacks(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  FrameworkunifiedNotificationsList userModeNotificationsBOOL[] = { {
      NTFY_SSSystemMgrPowerOnOff, sizeof(BOOL), eFrameworkunifiedStateVar }, {
      NTFY_SSSystemMgrUserMode, sizeof(BOOL), eFrameworkunifiedStateVar } };

  FrameworkunifiedNotificationsList userModeNotificationsStruct[] = { {
      NTFY_SSSystemMgrPowerOnOff,
      sizeof(T_SS_SM_UserModeOnOffNotification_Struct), eFrameworkunifiedStateVar }, {
      NTFY_SSSystemMgrUserMode,
      sizeof(T_SS_SM_UserModeOnOffNotification_Struct), eFrameworkunifiedStateVar } };

  FrameworkunifiedNotificationsList sm_notifications[] = { { NTFY_SSHUBootModes,
      sizeof(TSS_BootModeListStruct), eFrameworkunifiedStateVar }, {
      NTFY_SSSystemMgrStartUpType, sizeof(EPWR_SC_WAKEUP_TYPE),
      eFrameworkunifiedStateVar }, { NTFY_SSSystemMgrDataReset,
      sizeof(ESMDataResetType), eFrameworkunifiedStateVar }, {
      NTFY_SSSystemMgrShutdownStarted, 0, eFrameworkunifiedStateVar }, {
      NTFY_SSServiceWakeupStatus, sizeof(ESMServiceWakeupStatus),
      eFrameworkunifiedStateVar }, { NTFY_SSNeedAplRestart, 0, eFrameworkunifiedStateVar } };

  if (m_SMConfig.UMConfig.IsUserModeNotificationABOOL) {
    l_eStatus = FrameworkunifiedNPRegisterNotifications(hApp, userModeNotificationsBOOL,
        static_cast<UI_32>(_countof(userModeNotificationsBOOL)));
  } else {
    l_eStatus = FrameworkunifiedNPRegisterNotifications(hApp,
        userModeNotificationsStruct,
  static_cast<UI_32>(_countof(userModeNotificationsStruct)));
  }

  if (eFrameworkunifiedStatusOK != l_eStatus) {
    LOG_ERROR("FrameworkunifiedNPRegisterNotifications(userModeNotifications)");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  } else if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedNPRegisterNotifications(hApp, sm_notifications, static_cast<UI_32>(_countof(sm_notifications))))) {  // LCOV_EXCL_BR_LINE 4:NSFW error case  //NOLINT (whitespace/line_length)
    // LCOV_EXCL_START 4: nsfw error code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("FrameworkunifiedNPRegisterNotifications(sm_notifications)");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    // LCOV_EXCL_STOP
  } else if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedRegisterServiceAvailabilityNotification(hApp, NTFY_SSSystemMgrAvailability))) {  // LCOV_EXCL_BR_LINE 4:NSFW error case  //NOLINT (whitespace/line_length)
    // LCOV_EXCL_START 4: nsfw error code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("FrameworkunifiedRegisterServiceAvailabilityNotification("  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
        NTFY_SSSystemMgrAvailability ")");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    // LCOV_EXCL_STOP
  } else if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedPublishServiceAvailability(hApp, TRUE))) {  // LCOV_EXCL_BR_LINE 4:NSFW error case  //NOLINT (whitespace/line_length)
    // LCOV_EXCL_START 4: nsfw error code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("FrameworkunifiedPublishServiceAvailability('NTFY_SSSystemMgrAvailability ', TRUE)");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    // LCOV_EXCL_STOP
  } else {
    LOG_SUCCESS("FrameworkunifiedPublishServiceAvailability('NTFY_SSSystemMgrAvailability ', TRUE)");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

    REC_HIST_IF_SUCCESSFUL(NTFY_SSSystemMgrAvailability, m_PubCmdHist,  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    m_PubHistIter, "", l_eStatus);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup
///
/// \param
///
/// \return
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::init_Heartbeat(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  FrameworkunifiedProtocolCallbackHandler hb_protocol_callbacks[] = { {
    SS_HEARTBEAT_PERIODIC_RESP, SysMgrCallback<CSystemManager,
            &CSystemManager::OnHeartBeatThreadHeartbeatResponse> }, {
    SS_HEARTBEAT_ERROR_DETECTED, SysMgrCallback<CSystemManager,
            &CSystemManager::OnHeartBeatErrorDetected> }, {
    SS_HEARTBEAT_AVAIL_CHECK_RESP, SysMgrCallback<CSystemManager,
            &CSystemManager::OnCheckAvailResponse> } };  // LCOV_EXCL_BR_LINE 11:unexpected branch

  //  Subscribe to Notifications (All required notifications)
  if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedAttachCallbacksToDispatcher(hApp, SS_SMHeartbeat, hb_protocol_callbacks, static_cast<UI_32>(_countof(hb_protocol_callbacks))))) {  // LCOV_EXCL_BR_LINE 4:NSFW error case  //NOLINT (whitespace/line_length)
    // LCOV_EXCL_START 4: nsfw error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("FrameworkunifiedAttachCallbacksToDispatcher(hb_notification_handlers)");
    // LCOV_EXCL_STOP
  } else if (NULL == (m_hHeartbeatThread.m_ThreadHdl = FrameworkunifiedCreateChildThreadWithPriority(hApp, SS_SMHeartbeat, HBThreadStart, HBThreadStop, PR_SMHEARTBEAT))) {  // LCOV_EXCL_BR_LINE 4:NSFW error case  //NOLINT (whitespace/line_length)
    // LCOV_EXCL_START 4: nsfw error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_eStatus = eFrameworkunifiedStatusThreadNotExist;
    m_hHeartbeatThread.m_ThreadState = eSM_ThreadNotExist;
    LOG_ERROR("FrameworkunifiedCreateChildThreadWithPriority(hApp, SS_SMHeartbeat)");
    // LCOV_EXCL_STOP
  } else if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedStartChildThread(hApp, m_hHeartbeatThread.m_ThreadHdl, 0, NULL))) {  // LCOV_EXCL_BR_LINE 4:NSFW error case  //NOLINT (whitespace/line_length)
    // LCOV_EXCL_START 4: nsfw error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    m_hHeartbeatThread.m_ThreadState = eSM_ThreadNotExist;
    LOG_ERROR("FrameworkunifiedStartChildThread(SS_SMHeartbeat)");
    // LCOV_EXCL_STOP
  } else {
    m_hHeartbeatThread.m_ThreadState = eSMThreadIsFine;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus CSystemManager::init_sysmem_monitor(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  FrameworkunifiedProtocolCallbackHandler sysmem_protocol_callbacks[] = { {
      eSysMemThrdCmd_SYS_LOW_MEMORY, SysMgrCallback<CSystemManager,
            &CSystemManager::OnLowSystemMemory> } }; // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  // LCOV_EXCL_BR_START 4: For setting the initialization conditions at SystemManager startup
  if (eFrameworkunifiedStatusOK
      != (l_eStatus = FrameworkunifiedAttachCallbacksToDispatcher(
            hApp,
            m_SysMemMonitor.GetThreadName().c_str(), sysmem_protocol_callbacks,
  static_cast<UI_32>(_countof(sysmem_protocol_callbacks))))) {
  // LCOV_EXCL_BR_STOP
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("FrameworkunifiedAttachCallbacksToDispatcher()");  // LCOV_EXCL_LINE 4: For setting the initialization conditions at SystemManager startup
  } else {
    m_SysMemMonitor.SetSLMConfigData(m_SMConfig.SLMConfig);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    l_eStatus = m_SysMemMonitor.Initialize(hApp);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    LOG_ERROR_REC_HIST_IF_ERRORED  // LCOV_EXCL_BR_LINE 6: For setting the initialization conditions at SystemManager startup
    (l_eStatus, "m_SysMemMonitor.Initialize(hApp);");  // LCOV_EXCL_BR_LINE 6: For setting the initialization conditions at SystemManager startup
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup
///
/// \param
///
/// \return
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnGroupLaunchTimerExpiry(HANDLE hThread) {  // LCOV_EXCL_START 6: Because the condition cannot be set
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  UI_32 l_launchID = m_oSystemLauncher.get_id();

  FRAMEWORKUNIFIEDLOG(ZONE_PERFORMANCE, __FUNCTION__, " Received from group %d", l_launchID);
  REC_HIST_IF_SUCCESSFUL("SM_TIMER_GROUP_MONITOR", m_TimerCmdHist,
          m_TimerHistIter, "TIMER", l_eStatus);

  LogGroupModulesState(l_launchID,
          " Group launch timer expired, processing next group.");

  if (NULL == hThread) {
    l_eStatus = eFrameworkunifiedStatusInvldParam;
    LOG_ERROR("NULL == hThread");
  } else {
    //    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "Timer expired for group id,%d",m_oSystemLauncher.get_id());
    if (!m_oSystemLauncher.is_end()) {
      l_launchID = m_oSystemLauncher.advance_id();
      if (eFrameworkunifiedStatusOK != (l_eStatus = OnLaunchGroup(hThread))) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
            " Error: OnLaunchGroup(%d) errored: %d/'%s'", l_launchID,
            l_eStatus, GetStr(l_eStatus).c_str());
      }
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::OnGroupLaunchTimerExpiry( HANDLE hThread )
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////////////////////////
/// \ingroup
///
/// \param
///
/// \return
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::FindNameOfTerminatedProcess(SI_32 f_pid, SS_String &f_ModuleName) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusDbRecNotFound;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  GroupLaunchMapIter l_GroupIterator = m_MapProclaunchGrps.begin();
  if (l_GroupIterator == m_MapProclaunchGrps.end()) {  // LCOV_EXCL_BR_LINE 200: Group Map cannot be empty
    // LCOV_EXCL_START 200: Group Map cannot be empty
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error : Group Map empty");  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
    l_eStatus = eFrameworkunifiedStatusInvldParam;
    // LCOV_EXCL_STOP
  } else {
    BOOL l_bModuleFound = FALSE;
    for (; (FALSE == l_bModuleFound)
                && (l_GroupIterator != m_MapProclaunchGrps.end());
        l_GroupIterator++) {
      ModuleLaunchListIter l_ModuleListIterator =
            l_GroupIterator->second.modules.begin();
      for (; (FALSE == l_bModuleFound)
                    && (l_ModuleListIterator
                            != l_GroupIterator->second.modules.end());
            l_ModuleListIterator++) {
        if (l_ModuleListIterator->pid == f_pid) {
            l_bModuleFound = TRUE;
            f_ModuleName = l_ModuleListIterator->name;
            l_eStatus = eFrameworkunifiedStatusOK;
        }
      }
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
///////////////////////////////////////////////////////////////////////////////
/// \ingroup
///
/// \param
///
/// \return
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::GetBinaryNameOfProcess(SS_String f_ModuleQueueName,
    SS_String &f_ModuleBinaryName) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;
  ModuleLaunchListIter l_moduleIter;

  f_ModuleBinaryName = "";
  l_eStatus = GetModuleIterator(f_ModuleQueueName.c_str(), l_moduleIter);
  if (eFrameworkunifiedStatusOK != l_eStatus) {
    LOG_ERROR("GetModuleIterator()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  } else {
    size_t l_pos = l_moduleIter->path.find_last_of("\\/");
    if (std::string::npos == l_pos) {  // LCOV_EXCL_BR_LINE 5: stdlib error case.
      // LCOV_EXCL_START 5: stdlib error case.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
          " Error. Unable to parse binary name from path %s.",
          l_moduleIter->path.c_str());
      // LCOV_EXCL_STOP
    } else {
      f_ModuleBinaryName = l_moduleIter->path.substr(l_pos + 1);
      l_eStatus = eFrameworkunifiedStatusOK;
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup
///
/// \param
///
/// \return
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::init_process_launcher(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  UI_32 TODO = 0;

  FrameworkunifiedProtocolCallbackHandler process_launcher_protocol_callbacks[] = {
      // Command ID,                      Call back functions
      { ePLThrdCmd_LAUNCH_MODULE_RESP,    SysMgrCallback<CSystemManager,
                                          &CSystemManager::OnModuleLaunchResponse> },
      { ePLThrdCmd_TERMINATE_MODULE_RESP, SysMgrCallback<CSystemManager,
                                          &CSystemManager::OnTerminateModuleResponse> },
      { ePLThrdCmd_RELAUNCH_MODULE_RESP,  SysMgrCallback<CSystemManager,
                                          &CSystemManager::OnReLaunchModuleResponse> },
      { ePLThrdCmd_THREAD_STATUS_RESP,    SysMgrCallback<CSystemManager,
                                          &CSystemManager::OnProcessLauncherThreadHeartbeatResponse> } }; // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  // setup call backs for my children
  if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedAttachCallbacksToDispatcher(hApp, SS_SMLauncher, process_launcher_protocol_callbacks, static_cast<UI_32>(_countof(process_launcher_protocol_callbacks))))) {  // LCOV_EXCL_BR_LINE 4:NSFW error case  //NOLINT (whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("FrameworkunifiedAttachCallbacksToDispatcher()");  // LCOV_EXCL_LINE 4: For setting the initialization conditions at SystemManager startup
  ////////  Create Writer Child Threads handles the writing of data
  } else if (NULL == (m_hProcLauncherThread.m_ThreadHdl = FrameworkunifiedCreateChildThreadWithPriority(hApp, SS_SMLauncher, ProcessLauncherOnStart, ProcessLauncherOnStop, PR_SMPROCLAUNCH))) {  // LCOV_EXCL_BR_LINE 4:NSFW error case  //NOLINT (whitespace/line_length)
    // LCOV_EXCL_START 4: For setting the initialization conditions at SystemManager startup
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_eStatus = eFrameworkunifiedStatusNullPointer;
    m_hProcLauncherThread.m_ThreadState = eSM_ThreadNotExist;
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        " Error: FrameworkunifiedCreateChildThreadWithPriority(%s) errored: %d/'%s'",
        SS_SMLauncher, l_eStatus, GetStr(l_eStatus).c_str());
    // LCOV_EXCL_STOP
  } else if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedStartChildThread(hApp, m_hProcLauncherThread.m_ThreadHdl, sizeof(UI_32), &TODO))) {  // LCOV_EXCL_BR_LINE 4:NSFW error case  //NOLINT (whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    m_hProcLauncherThread.m_ThreadState =  // LCOV_EXCL_LINE 4: For setting the initialization conditions at SystemManager startup
        eSM_ThreadNotExist;  // LCOV_EXCL_LINE 4: For setting the initialization conditions at SystemManager startup
    LOG_ERROR("FrameworkunifiedStartChildThread()");  // LCOV_EXCL_LINE 4: For setting the initialization conditions at SystemManager startup
  } else {
    m_hProcLauncherThread.m_ThreadState = eSMThreadIsFine;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus CSystemManager::OnGroupLaunchWaitTimeout(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  SetCmdHist("SM_TIMER_GROUP_LAUNCH_WAIT_TIMER", m_TimerCmdHist, m_TimerHistIter, FrameworkunifiedGetMsgSrc(hApp)); // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  CALL_AND_LOG_STATUS(OnLaunchGroup(hApp));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup
///
/// \param
///
/// \return
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnModuleLaunchResponse(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  INTERFACEUNIFIEDLOG_RECEIVED_FROM(hApp);  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "from %s", FrameworkunifiedGetMsgSrc(hApp));

  GroupLaunchMapIter l_GroupIter;
  ModuleLaunchListIter l_ModuleIter;
  UI_32 l_launchID = m_oSystemLauncher.get_id();

  l_eStatus = PerformModuleLaunchRespProcessing(hApp, l_GroupIter, l_ModuleIter, "ML");
  if (eFrameworkunifiedStatusOK != l_eStatus) {
    LOG_ERROR("PerformModuleLaunchRespProcessing()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  } else if (l_GroupIter->second.id == l_launchID) {
    BOOL IsGroupLaunchComplete = TRUE;
    // Search the group for any module that has not completed launching.
    for (l_ModuleIter = l_GroupIter->second.modules.begin();
         IsGroupLaunchComplete && (l_ModuleIter != l_GroupIter->second.modules.end());
         l_ModuleIter++) {
      IsGroupLaunchComplete = !l_ModuleIter->IsModuleState(MODULE_STATE_LAUNCHING);
    }

    if (IsGroupLaunchComplete) {
      m_GroupLaunchTimer->StopTimer(m_aTimerIDs[eSM_TIMER_GROUP_MONITOR]);
      l_GroupIter->second.launch_complete = TRUE;
      FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "Group%d LaunchComp", l_GroupIter->second.id);

      if (!m_oSystemLauncher.is_end()) {
        l_launchID = m_oSystemLauncher.advance_id();
        FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "Next group:%d", l_launchID);
        if (FALSE == l_GroupIter->second.grp_wait_for_trigger) {
          UI_32 l_Sec = (l_GroupIter->second.grp_launch_wait) / (1000 * 1000);
          UI_32 l_mSec = 0;
          if (l_Sec > 0) {
              l_mSec = ((l_GroupIter->second.grp_launch_wait) % (1000 * 1000)) / 1000;
          } else {
              l_mSec = (l_GroupIter->second.grp_launch_wait) / 1000;
          }
          m_GroupLaunchTimer->StartTimer(
                  m_aTimerIDs[eSM_TIMER_GROUP_LAUNCH_WAIT_TIMER], l_Sec, l_mSec, 0, 0);
        } else {
          // Start the timer for group trigger... added for optimization
          m_GroupLaunchTimer->StartTimer(
                  m_aTimerIDs[eSM_TIMER_LAUNCH_GROUP_TRIGGER_PROC_RESP_TIMER],
                  SS_LAUNCH_GROUP_TRIGGER_TIMER_CONFIG, 0, 0, 0);
        }
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Group %d/%s LaunchIncomp", l_launchID,
          l_GroupIter->second.name.c_str());
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
        " '%s' ( Group %d/%s ) Launch Response received late - "
                "group id is now %d", l_ModuleIter->name.c_str(),
        l_GroupIter->second.id, l_GroupIter->second.name.c_str()
        , l_launchID);
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus CSystemManager::OnReLaunchModuleResponse(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  INTERFACEUNIFIEDLOG_RECEIVED_FROM(hApp);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  FRAMEWORKUNIFIEDLOG(ZONE_PERFORMANCE, __FUNCTION__, "from %s", FrameworkunifiedGetMsgSrc(hApp));

  GroupLaunchMapIter l_GroupIter;
  ModuleLaunchListIter l_ModuleIter;

  l_eStatus = PerformModuleLaunchRespProcessing(hApp, l_GroupIter, l_ModuleIter, "RL");
  LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus, "PerformModuleLaunchRespProcessing()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::OnReLaunchModuleResponse( HANDLE hApp )


EFrameworkunifiedStatus CSystemManager::PerformModuleLaunchRespProcessing(HANDLE hApp,
        GroupLaunchMapIter & f_GroupIter, ModuleLaunchListIter & f_ModuleIter,
        PCSTR p_sPPD_tag) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  T_ProcessLaunchResp l_LaunchRespData;

  // ReadMsg():                                                        *
  //     Check hApp ptr, msg size, msg reception, read msg if all ok.  *
  //     Report any errors found.                                      *
  //                                                                   *
  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg < T_ProcessLaunchResp > (hApp, l_LaunchRespData))) {  // LCOV_EXCL_BR_LINE 4: nsfw error  // NOLINT(whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("ReadMsg()");  // LCOV_EXCL_LINE 4: nsfw error
  } else {
    BOOL b_DidModuleLaunch = FALSE;
    f_GroupIter = l_LaunchRespData.groupIterator;
    f_ModuleIter = l_LaunchRespData.moduleIterator;
    SS_String l_ModulePath = l_LaunchRespData.path;

    FRAMEWORKUNIFIEDLOG(ZONE_PERFORMANCE, __FUNCTION__, "%s(%d)", l_LaunchRespData.name, l_LaunchRespData.pid);

    if (l_LaunchRespData.pid == -1) {
      f_ModuleIter->pid = -1;

      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: Process launch failed: %s, %s",
          f_ModuleIter->name.c_str(), l_ModulePath.c_str());

      /// TODO
      /// If PID is -1, launch had failed. Add logic to do necessary action.

      f_ModuleIter->SetModuleState(MODULE_STATE_LAUNCH_FAILED);
    } else if (l_LaunchRespData.pid == 0x7FFFFFFF) {
      FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, " Warning: '%s', '%s' already launched",
          f_ModuleIter->name.c_str(), l_ModulePath.c_str());

      b_DidModuleLaunch = TRUE;
    } else {
      f_ModuleIter->pid = l_LaunchRespData.pid;
      b_DidModuleLaunch = TRUE;
    }

    if (b_DidModuleLaunch) {
      //
      // Module state could already be MODULE_STATE_CONNECTED if
      // module process ran before process launcher thread completed;
      // don't overwrite/reset the MODULE_STATE_CONNECTED state.
      if (f_ModuleIter->IsModuleState(MODULE_STATE_LAUNCHING)) {
        f_ModuleIter->SetModuleState(MODULE_STATE_LAUNCHED);
      }

      l_eStatus = f_ModuleIter->GetPriority(f_ModuleIter->current_priority);
      if (eFrameworkunifiedStatusOK != l_eStatus) {
        FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, " Warning: f_ModuleIter->GetPriority( "
               "f_ModuleIter->current_priority ) returned %d/'%s'. "
               "Setting 'current_priority' = 'configuration_priority' %d.",
               l_eStatus, GetStr(l_eStatus).c_str(),
               f_ModuleIter->configuration_priority);
        f_ModuleIter->current_priority = f_ModuleIter->configuration_priority;
        l_eStatus = eFrameworkunifiedStatusOK;
      }
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::PerformModuleLaunchRespProcessing(

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnTerminateModuleResponse
///
/// \param HANDLE hApp
///
/// \return EFrameworkunifiedStatus
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnTerminateModuleResponse(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  T_ProcessLauncherTerminationResp l_TerminateRespData;
  INTERFACEUNIFIEDLOG_RECEIVED_FROM(hApp);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, " from %s", FrameworkunifiedGetMsgSrc(hApp));

  // ReadMsg():                                                        *
  //     Check hApp ptr, msg size, msg reception, read msg if all ok.  *
  //     Report any errors found.                                      *
  //                                                                   *
  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg < T_ProcessLauncherTerminationResp
                       > (hApp, l_TerminateRespData))) {
    LOG_ERROR("ReadMsg()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  } else {
    ModuleLaunchListIter l_ModuleIter = l_TerminateRespData.moduleIterator;

    SS_String l_ModulePath = m_BinaryFilesPath + l_ModuleIter->path;

    if ((0 == strcmp(l_ModulePath.c_str(), l_TerminateRespData.path))
        && (0 == strcmp(l_ModuleIter->arguments.c_str(),
                        l_TerminateRespData.args))) {
      l_ModuleIter->pid = 0;
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " %s Termination successful",
             l_ModuleIter->name.c_str());
    } else {
      if (0 != strcmp(l_ModulePath.c_str(), l_TerminateRespData.path)) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
            " Error: ModuleIter->path != l_TerminateRespData.path"
            " \" '%s' != '%s' \"", l_ModuleIter->path.c_str(),
            l_TerminateRespData.path);
      }
      if (0 != strcmp(l_ModuleIter->arguments.c_str(),
                      l_TerminateRespData.args)) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: ModuleIter->arguments != "
            "l_TerminateRespData.args, \" '%s' != '%s' \"",
            l_ModuleIter->arguments.c_str(), l_TerminateRespData.args);
      }
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::OnTerminateModuleResponse( HANDLE hApp )

///////////////////////////////////////////////////////////////////////
/// \ingroup ReadPathAndFileNameEnvironmentVariables
///
/// \param
///
/// \return VOID
///////////////////////////////////////////////////////////////////////////////
VOID CSystemManager::ReadPathFromEnvironmentVariables() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
  CHAR *l_pEnvVariable = NULL;

  // read Path from environment variable
  if (NULL == (l_pEnvVariable = std::getenv(BaseDirPathEnvVariable))) {
    // Environment variable not set, set it to default path
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Using Default path for Binary Files : %s", DefaultBasePath);
    m_BinaryFilesPath = DefaultBasePath;

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Using Default path for Config Files : %s%s",
        DefaultBasePath, DefaultConfigFilesPath);
    m_ConfigFilesPath = DefaultBasePath;
  } else {
    // set path of binary files
    m_BinaryFilesPath = l_pEnvVariable;

    // set path of configuration files
    m_ConfigFilesPath = l_pEnvVariable;
  }
  // set path of configuration files
  m_ConfigFilesPath += DefaultConfigFilesPath;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return;
}

///////////////////////////////////////////////////////////////////////
/// \ingroup GetConfigDataFileNameFromEnvironmentVariable
///
/// \param
///
/// \return SS_String
///////////////////////////////////////////////////////////////////////////////
SS_String CSystemManager::ReadLaunchConfigFileNameFromEnvironmentVariable() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  CHAR *l_pEnvVariable = NULL;
  SS_String l_FileName;

  // read Launch Config File Name from environment variable
  // LCOV_EXCL_BR_START 5: standard lib error
  if (NULL == (l_pEnvVariable = std::getenv(LaunchConfigFileNameEnvVariable))) {
  // LCOV_EXCL_BR_STOP
    // Environment variable not set, set it to default file name
    // LCOV_EXCL_START 5: standard lib error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Using Default Launch Configuration file : %s",
        DefaultLaunchConfigFileName);
    l_FileName = DefaultLaunchConfigFileName;
    // LCOV_EXCL_STOP
  } else {
    l_FileName = l_pEnvVariable;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Using Launch Configuration file: %s", l_FileName.c_str());

  SS_String retStr = m_ConfigFilesPath + l_FileName; // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  if (access(retStr.c_str(), F_OK) != 0) {  // LCOV_EXCL_BR_LINE 5: standard lib error
    // LCOV_EXCL_START 5: standard lib error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "no found %s", retStr.c_str());
    // For working even if the system installed into legacy path
    SS_String tmpStr = DefaultConfigFilesPathOld;
    tmpStr += l_FileName;
    // LCOV_EXCL_STOP
    if (access(tmpStr.c_str(), F_OK) == 0) {  // LCOV_EXCL_BR_LINE 5: standard lib error
      retStr = tmpStr;  // LCOV_EXCL_BR_LINE 5: standard lib error
    } else {
      SS_ASERT(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    }
  }
  return retStr;
}

///////////////////////////////////////////////////////////////////////
/// \ingroup GetConfigDataFileNameFromEnvironmentVariable
///
/// \param
///
/// \return SS_String
///////////////////////////////////////////////////////////////////////////////
SS_String CSystemManager::ReadConfigDataFileNameFromEnvironmentVariable() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  CHAR *l_pEnvVariable = NULL;
  SS_String l_FileName = "";

  // read SM Config Data File Name from environment variable
  // LCOV_EXCL_BR_START 5: standard lib error
  if (NULL == (l_pEnvVariable = std::getenv(SMConfigDataFileNameEnvVariable))) {
  // LCOV_EXCL_BR_STOP 5: standard lib error
    // LCOV_EXCL_START 5: standard lib error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    // Environment variable not set, set it to default file name
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Using Default Configuration Data file : %s", DefaultSMConfigFileName);
    l_FileName = DefaultSMConfigFileName;
    // LCOV_EXCL_STOP
  } else {
    l_FileName = l_pEnvVariable;
  }

  SS_String retStr = m_ConfigFilesPath + l_FileName; // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  if (access(retStr.c_str(), F_OK) != 0) {  // LCOV_EXCL_BR_LINE 5: standard lib error
    // LCOV_EXCL_START 5: standard lib error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "no found %s", retStr.c_str());
    // For working even if the system installed into legacy path
    SS_String tmpStr = DefaultConfigFilesPathOld;
    tmpStr += l_FileName;
    if (access(tmpStr.c_str(), F_OK) == 0) {
      retStr = tmpStr;
    } else {
      SS_ASERT(0);
    }
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return retStr;
}

///////////////////////////////////////////////////////////////////////
/// \ingroup ReadUsingVMPlayerEnvironmentVariable
///
/// \param
///
/// \return VOID
///////////////////////////////////////////////////////////////////////////////
VOID CSystemManager::ReadUsingVMPlayerEnvironmentVariable() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  CHAR *l_pEnvVariable = std::getenv(UsingVMPlayerVariable);
  m_UsingVMPlayer = (NULL != l_pEnvVariable);
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " %sunning on a VMPlayer",
      m_UsingVMPlayer ? "R" : "Not r");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return;
}  // End of VOID CSystemManager::ReadUsingVMPlayerEnvironmentVariable()

///////////////////////////////////////////////////////////////////////////////
/// \ingroup
///
/// \param
///
/// \return
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::start_process_launching(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  BOOL l_bIsGoodRc;
  SS_String l_SMConfigFile(ReadConfigDataFileNameFromEnvironmentVariable());
  SMConfigParams SMConfigData;
  SS_String l_LaunchConfigFile(ReadLaunchConfigFileNameFromEnvironmentVariable()); // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  SysMgrConfiguration l_launchConfig;

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Reading %s Configuration Data from %s",
      SERVICE_SYSMANAGER, l_SMConfigFile.c_str());

  l_bIsGoodRc = SMConfigData.LoadSMConfigParameters(m_SMConfig, l_SMConfigFile);

  if (!l_bIsGoodRc) {  // LCOV_EXCL_BR_LINE 6: For setting the initialization conditions at SystemManager startup
    // LCOV_EXCL_START 6: For setting the initialization conditions at SystemManager startup
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        " Error: SMConfigData.LoadSMConfigParameters(m_SMConfig, "
                " %s) returned FALSE", l_SMConfigFile.c_str());
    l_eStatus = eFrameworkunifiedStatusFileLoadError;
    // LCOV_EXCL_STOP
  } else {
    SMSetState(hApp, SS_SM_APPS_LAUNCH_IN_PROGRESS);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Reading Launch Configuration Data from %s",
        l_LaunchConfigFile.c_str());

    SS_String l_launchOrderName = "DEFAULT";  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    LaunchOrderedVector l_OrderList;

    {
      RAM_AccessIf ram_access;
      EFrameworkunifiedStatus l_eRamStatus;
      RAM_SM_INFO_t info;

      // LCOV_EXCL_BR_START 6: Because RamAccessIf::getRamInfo returns only eFrameworkunifiedStatusOK
      if (eFrameworkunifiedStatusOK !=
          (l_eRamStatus = ram_access.getRamInfo(&info))) {
      // LCOV_EXCL_BR_STOP
        // LCOV_EXCL_START 6: Because RamAccessIf::getRamInfo returns only eFrameworkunifiedStatusOK
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               " Error: ram_access.getRamInfo(%d)", l_eRamStatus);
        // LCOV_EXCL_STOP
      } else {
        if (info.order_name[0] != '\0') {  // LCOV_EXCL_BR_LINE 6: For setting the initialization conditions at SystemManager startup
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          l_launchOrderName = info.order_name;  // LCOV_EXCL_LINE 6: For setting the initialization conditions at SystemManager startup
        }
      }
    }

    l_bIsGoodRc = l_launchConfig.LoadParametersCfg(m_MapProclaunchGrps,
        m_MapProcNames, l_OrderList, l_launchOrderName, l_LaunchConfigFile);

    if (l_OrderList.size() != 0) {  // LCOV_EXCL_BR_LINE 6: For setting the initialization conditions at SystemManager startup
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "wakeup order:%s",
          l_launchOrderName.c_str());
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "wakeup order:DEFAULT");
    }

    if (!l_bIsGoodRc) {  // LCOV_EXCL_BR_LINE 6: For setting the initialization conditions at SystemManager startup
      // LCOV_EXCL_START 6: For setting the initialization conditions at SystemManager startup
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: l_launchConfig.LoadParametersCfg( "
          "m_MapProclaunchGrps, m_MapProcNames, %s ) returned FALSE",
          l_LaunchConfigFile.c_str());
      l_eStatus = eFrameworkunifiedStatusFileLoadError;
      // LCOV_EXCL_STOP
    } else {
      m_SystemLaunchProgress = SS_SM_INITIAL_GROUP;
      // l_launchConfig.PrintAllInfo( m_MapProclaunchGrps );

      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " System Launch Started");
      // Don't use DynamicLaunch because its dependency on the services table is complex
      SS_SOURCE source_type = SS_SOURCE_FM;

      m_oSystemLauncher.configure_dynamic_launch(source_type, m_MapProclaunchGrps, l_OrderList);
      m_oSystemLauncher.copyDynOrderedVector(m_SystemStarter);
      // m_SystemStarter.print_info();

      // start the timer upon successful launching of first process
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
          "Starting Module Connection Timeout timer with %d sec period",
          m_SMConfig.MCConfig.ModuleConnectionTimeOutSec);
      m_GroupLaunchTimer->StartTimer(
          m_aTimerIDs[eSM_TIMER_MODULE_CONNECT_WAIT_TIMER],
          m_SMConfig.MCConfig.ModuleConnectionTimeOutSec, 0,
          m_SMConfig.MCConfig.ModuleConnectionTimeOutSec, 0);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
      CALL_AND_LOG_STATUS(OnLaunchGroup(hApp));  // LCOV_EXCL_BR_LINE 6: For setting the initialization conditions at SystemManager startup
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}


EFrameworkunifiedStatus CSystemManager::GetLastSourceInfo(SS_SOURCE &source_type) {  // LCOV_EXCL_START 8: Because the condition cannot be set
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;

  CHAR LastSource[64 + 1] = { '\0' };
  l_eStatus = NPSynchronousReadPersistentData(SERVICE_AS_MODE,
              NTFY_SSLastSourceType, LastSource, static_cast<UI_32>(sizeof(LastSource) - 1));
  if (eFrameworkunifiedStatusOK != l_eStatus) {
    LOG_ERROR("NPSynchronousReadPersistentData("
              SERVICE_AS_MODE "," NTFY_SSLastSourceType);
  } else {
    std::string tempstr(LastSource);
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " String got from persistence %s", LastSource);
    UI_32 pos = static_cast<UI_32>(tempstr.find("_"));
    std::string tempstr2 = tempstr.substr(0, pos);
    UI_64 value = strtoull(tempstr2.c_str(), 0, 10);
    source_type = static_cast<SS_SOURCE>((UI_32)(value >> 32) & 0x0000FFFF);
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus CSystemManager::OnLaunchGroupTriggerProcessResponse(HANDLE hApp) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  INTERFACEUNIFIEDLOG_RECEIVED_FROM(hApp);
  UI_32 l_groupID;

  FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "from %s", FrameworkunifiedGetMsgSrc(hApp));
  SetCmdHist("SS_SM_GROUP_LAUNCH_TRIGGER", m_SMCmdHist, m_SMHistIter, FrameworkunifiedGetMsgSrc(hApp));
  // stop group launch trigger process response timer
  m_GroupLaunchTimer->StopTimer(m_aTimerIDs[eSM_TIMER_LAUNCH_GROUP_TRIGGER_PROC_RESP_TIMER]);

  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg < UI_32 > (hApp, l_groupID))) {
    LOG_ERROR("ReadMsg()");
  } else {
    SS_SOURCE source_type = SS_SOURCE_NA;
    l_eStatus = GetLastSourceInfo(source_type);

    if ((SS_SOURCE_AM == source_type) || (SS_SOURCE_FM == source_type)
     || (SS_SOURCE_FM_DAB == source_type) || (SS_SOURCE_SDARS == source_type)) {
      if (l_groupID == m_oSystemLauncher.get_id()) {
        CALL_AND_LOG_STATUS(OnLaunchGroup(hApp));
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
            " Error: Received Group ID %d != Sys Mgr Group ID %d", l_groupID,
            m_oSystemLauncher.get_id());
        l_eStatus = eFrameworkunifiedStatusInvldParam;
     }
    } else {  // media is last source
      CALL_AND_LOG_STATUS(OnLaunchGroup(hApp));
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus CSystemManager::OnLaunchGroupTriggerProcessResponseTimerExpiry(HANDLE hApp) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  INTERFACEUNIFIEDLOG_RECEIVED_FROM(hApp);
  REC_HIST_IF_SUCCESSFUL("SM_TIMER_LAUNCH_GROUP_TRIGGER", m_TimerCmdHist,
                         m_TimerHistIter, FrameworkunifiedGetMsgSrc(hApp), l_eStatus);
  CALL_AND_LOG_STATUS(OnLaunchGroup(hApp));

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup
///
/// \param
///
/// \return
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnLaunchGroupSelfTrigger(HANDLE hApp) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "from %s", FrameworkunifiedGetMsgSrc(hApp));

  REC_HIST_IF_SUCCESSFUL("SS_SM_SendTriggerToSelf", m_SMCmdHist, m_SMHistIter,
                         FrameworkunifiedGetMsgSrc(hApp), l_eStatus);

  CALL_AND_LOG_STATUS(OnLaunchGroup(hApp));

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////////////////////////
/// \ingroup
///
/// \param
///
/// \return
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnLaunchGroup(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  GroupLaunchMapIter l_GroupIter;
  UI_32 l_launch_ID = m_oSystemLauncher.get_id();

  //  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Launching group %d", l_launch_ID );

  l_GroupIter = m_MapProclaunchGrps.find(l_launch_ID);

  if (l_GroupIter->second.launch_complete == FALSE) {  // LCOV_EXCL_BR_LINE 6: Because it is not called for a condition that results in "TRUE"
    // FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__
    //     , " m_bIsNPP_ServicesStarted ? %s, group.id: %d, Launch_Group: %d"
    //     , m_bIsNPP_ServicesStarted ? "Yes" : "No"
    //     , l_GroupIter->second.id
    //     , m_SystemLaunchProgress );

    if ((m_bIsNPP_ServicesStarted && m_bIsBackupAvail) || (l_GroupIter->second.id <= 2)) {  // LCOV_EXCL_BR_LINE 6: Because it is not called for the condition that results in "FALSE" // NOLINT(whitespace/line_length)
      ModuleLaunchListIter l_ModuleListIterator = l_GroupIter->second.modules.begin();
      if (l_ModuleListIterator != l_GroupIter->second.modules.end()) {  // LCOV_EXCL_BR_LINE 6: For setting the initialization conditions at SystemManager startup // NOLINT(whitespace/line_length)
        // FRAMEWORKUNIFIEDLOG(ZONE_PERFORMANCE, __FUNCTION__, "GRPLCH,(start),%d,%s",
        //        l_GroupIter->second.id,l_GroupIter->second.name.c_str());

        FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "Launching Group%d/%s",
            l_GroupIter->second.id, l_GroupIter->second.name.c_str());
        SS_String l_ModulePath;
        for (; l_ModuleListIterator != l_GroupIter->second.modules.end(); l_ModuleListIterator++) {
          if (l_ModuleListIterator->IsModuleState(MODULE_STATE_INVALID)) {
            char l_cFormat[] = "SendRequestToLauncher(%s, "
                    "ePLThrdCmd_LAUNCH_MODULE_REQST)";
            char l_cBuf[sizeof(l_cFormat) + MAX_NAME_SIZE_APP];
            snprintf(l_cBuf, sizeof(l_cBuf), l_cFormat,
                    l_ModuleListIterator->name.c_str());

            l_eStatus = SendRequestToLauncher(hApp, l_ModuleListIterator,  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
                    ePLThrdCmd_LAUNCH_MODULE_REQST,  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
                    "ePLThrdCmd_LAUNCH_MODULE_REQST");  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

            if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 6: For setting the initialization conditions at SystemManager startup // NOLINT(whitespace/line_length)
              // LCOV_EXCL_START 6: For setting the initialization conditions at SystemManager startup // NOLINT(whitespace/line_length)
              AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
              l_ModuleListIterator->SetModuleState(
                      MODULE_STATE_LAUNCH_FAILED);
              SetCmdHist(l_cBuf, m_ErrHist, m_ErrHistIter,
                      GetStr(l_eStatus).c_str());
              // LCOV_EXCL_STOP
            } else {
              l_ModuleListIterator->SetModuleState(MODULE_STATE_LAUNCHING);
            }
          } else {
            l_ModulePath = (m_BinaryFilesPath + l_ModuleListIterator->path);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
                    " Not launching %s: module state is %s",
                    l_ModulePath.c_str(),
                    l_ModuleListIterator->ModuleStateStr().c_str());
          }
        }

        // LCOV_EXCL_BR_START 6: For setting the initialization conditions at SystemManager startup // NOLINT(whitespace/line_length)
        if (FALSE ==
            l_GroupIter->second.grp_wait_for_trigger) {
        // LCOV_EXCL_BR_STOP
          m_GroupLaunchTimer->StartTimer(m_aTimerIDs[eSM_TIMER_GROUP_MONITOR],
                  SS_GROUP_LAUNCH_TIMER_CONFIG, 0, 0, 0);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
        }
      }
    }

    if (m_oSystemLauncher.is_end()) {  // LCOV_EXCL_BR_LINE 6: For setting the initialization conditions at SystemManager startup // NOLINT(whitespace/line_length)
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "System Launch Completed");

      if (epssusSYSTEM_SERVICES_STARTED == m_SystemModeInfo.startupStage) {
        // If startupStage == SYSTEM_SERVICES_STARTED, a notification is requested because there are SVCs required for notification to the _CWORD56_.
        l_eStatus = SendSystemModeInfoResponse(hApp, epssusALL_SERVICES_LAUNCHED);
        LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus,  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
            "UpdateStartupStageState_SendSystemModeInfoResponse"  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
            "( epssusALL_SERVICES_LAUNCHED )");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
      }

      m_SystemModeInfo.startupStage = epssusALL_SERVICES_LAUNCHED;

      if (is_current_state(SS_SM_APPS_LAUNCH_IN_PROGRESS)) {
         SMSetState(hApp, SS_SM_APPS_LAUNCHED_READY_TO_START);
      }
      // SysMgrConfiguration config;
      // config.PrintAllInfo( m_MapProclaunchGrps );
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::OnLaunchGroup(HANDLE hApp)

///////////////////////////////////////////////////////////////////////////////
/// \ingroup    OnProcessLaunchTimerExpiry
///
/// \param      HANDLE hApp
///
/// \return     EFrameworkunifiedStatus
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnProcessLaunchTimerExpiry(HANDLE hApp) {  // LCOV_EXCL_START 8:  Because the condition cannot be set
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  // ToDo 2013_05_17 Jay When used ? Dead code ? Don't see a StartTimer call.
  REC_HIST_IF_SUCCESSFUL("SM_TIMER_PROCESS_LAUNCH_MONITOR", m_TimerCmdHist,
                         m_TimerHistIter, FrameworkunifiedGetMsgSrc(hApp), l_eStatus);
  m_GroupLaunchTimer->StopTimer(m_aTimerIDs[eSM_TIMER_PROCESS_LAUNCH_MONITOR]);
  return eFrameworkunifiedStatusOK;
}
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////////////////////////
/// \ingroup    OnHeartBeatMonitorTimerExpiry
///
/// \param      HANDLE hApp
///
/// \return     EFrameworkunifiedStatus
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnHeartBeatMonitorTimerExpiry(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_PERIODIC_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_PERIODIC_INFO, __FUNCTION__, " Received from %s", FrameworkunifiedGetMsgSrc(hApp));
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
  REC_HIST_IF_SUCCESSFUL("SM_TIMER_HEARTBEAT_MONITOR", m_TimerCmdHist,  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
                         m_TimerHistIter, FrameworkunifiedGetMsgSrc(hApp), l_eStatus);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)

  // LCOV_EXCL_BR_START 4: nsfw error
  if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedSendChild(hApp, m_hHeartbeatThread.m_ThreadHdl,
                      SS_HEARTBEAT_PERIODIC_STATUS_REQ, 0, NULL))) {
  // LCOV_EXCL_BR_STOP 4: nsfw error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("FrameworkunifiedSendChild(m_hHeartbeatThread.m_ThreadHdl, "  // LCOV_EXCL_LINE 4: nsfw error
              "SS_HEARTBEAT_PERIODIC_STATUS_REQ)");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  } else {
    m_hHeartbeatThread.m_ThreadState = eSMWaitForHeartbeat;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_PERIODIC_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup SystemManagerDebugDump
///     implement post mortem function
///
/// \param
///
/// \return void
///////////////////////////////////////////////////////////////////////////////

VOID CSystemManager::SystemManagerDebugDump(HANDLE hApp) {
  std::stringstream l_debugDumpBuf;
  cmdHistIter i;

  // LCOV_EXCL_BR_START 11:unexpected branch  // NOLINT(whitespace/line_length)
  l_debugDumpBuf << FrameworkunifiedGetAppName(hApp) << "/" << endl
      << " <SYSTEM MANAGER DUMP DATA> " << endl << endl << " ***internals*** " << endl
      << endl << " boot mode " << GetStr(m_BootMode).c_str() << endl << " SM state: "
      << GetStr(m_SMCurrentState).c_str() << endl << " SM  startup reason: "
      << GetStr(m_StartUpReason).c_str() << endl << " SM shutdown reason: "
      << GetStr(m_shutdownTrigger).c_str() << endl << " LUM: "
      << GetStr(m_lastUserMode).c_str() << endl << " TPM: "
      << GetStr(m_SystemModeInfo.transportMode).c_str() << endl << " LPH: "
      << GetStr(m_SystemModeInfo.limpHomeCutoff).c_str() << endl << " PMode: "
      << GetStr(m_SystemModeInfo.productionMode).c_str() << endl << " ActiveGroup: "
      << m_ActiveGroupId << endl << " m_MaxShutdownTime: " << m_MaxShutdownTime
      << endl << " NPP: " << (m_bIsNPP_ServicesStarted ? "Started" : "notStarted")
      << endl
      << endl << " ***Error history***" << endl;
  // LCOV_EXCL_BR_STOP

  for (i = m_ErrHist.begin(); i != m_ErrHist.end(); ++i) {
    l_debugDumpBuf << endl << " " << i->m_cmd.c_str() << " @ " << i->m_time  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
        << "ms EC:" << i->m_sender.c_str() << endl;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  }

  l_debugDumpBuf << endl << " ***_CWORD56_ command history***" << endl;

  for (i = m__CWORD56_CmdHist.begin(); i != m__CWORD56_CmdHist.end(); ++i) {
    l_debugDumpBuf << endl << " " << i->m_cmd.c_str() << " @ " << i->m_time << "ms" << endl;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  }

  l_debugDumpBuf << endl << " ***Timer history***" << endl;

  for (i = m_TimerCmdHist.begin(); i != m_TimerCmdHist.end(); ++i) {
    l_debugDumpBuf << endl << " " << i->m_cmd.c_str() << " @ " << i->m_time << "ms" << endl;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  }

  l_debugDumpBuf << endl << " ***SM message history***" << endl;

  for (i = m_SMCmdHist.begin(); i != m_SMCmdHist.end(); ++i) {
    l_debugDumpBuf << endl << " " << i->m_cmd.c_str() << " @ " << i->m_time << "ms "  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
        << i->m_sender.c_str() << endl;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  }

  l_debugDumpBuf << endl << " ***Publishshing history***" << endl;

  for (i = m_PubCmdHist.begin(); i != m_PubCmdHist.end(); ++i) {
    l_debugDumpBuf << endl << " " << i->m_cmd.c_str() << " @ " << i->m_time << "ms "  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
        << i->m_sender.c_str() << endl;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  }

  l_debugDumpBuf << endl << " ***SM start table***" << endl;    // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  // LCOV_EXCL_BR_START 11:unexpected branch  // NOLINT(whitespace/line_length)
  l_debugDumpBuf << endl
    << " index | name | start required? | isOKFlag | Module state | start sent "
        "| start resposne received |start delta | stop sent | reply received | stop delta | PID"
    << endl;
  // LCOV_EXCL_BR_STOP

  for (UI_32 k = 1u; (k <= m_MapProclaunchGrps.size()); k++) {
    GroupLaunchMapIter l_GroupIterator = m_MapProclaunchGrps.find(k);

    if (l_GroupIterator != m_MapProclaunchGrps.end()) {
      ModuleLaunchListIter l_ModuleListIterator = l_GroupIterator->second.modules.begin();

      for (int i = 1u; l_ModuleListIterator != l_GroupIterator->second.modules.end();
            l_ModuleListIterator++, i++) {
      // LCOV_EXCL_BR_START 11:unexpected branch  // NOLINT(whitespace/line_length)
        std::string sIsOKFlag = GetModuleCondition(l_ModuleListIterator) ? " " :  // 'blank' means OK
                "X";  // 'X' means state mismatch

        l_debugDumpBuf << " " << std::setw(2) << std::right << i << " | "
            << std::setw(24) << std::left << l_ModuleListIterator->name.c_str()
            << " | " << std::setw(5) << std::left
            << (l_ModuleListIterator->is_start_required ? "TRUE" : "FALSE")

            << " | " << sIsOKFlag.c_str() << " | " << std::setw(24) << std::left
            << l_ModuleListIterator->ModuleStateStr().c_str() << " | "
            << l_ModuleListIterator->m_startReason.GetInitial() << " | "
            << l_ModuleListIterator->m_startReason.GetFinal() << " | "
            << std::setw(3) << std::right
            << static_cast<SI_16>(l_ModuleListIterator->m_startReason.GetDelta())
            << "ms" << " | " << l_ModuleListIterator->m_stopReason.GetInitial()
            << " | " << l_ModuleListIterator->m_stopReason.GetFinal() << " | "
            << std::setw(3)
            << static_cast<SI_16>(l_ModuleListIterator->m_stopReason.GetDelta())
            << "ms" << " | " << l_ModuleListIterator->pid << endl;
      // LCOV_EXCL_BR_STOP
      }
    }
  }

  l_debugDumpBuf << endl << " ***Heartbeat information***" << endl;

  l_debugDumpBuf << endl << " Entire State:" << m_HBReport.eEntireState << endl;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  l_debugDumpBuf << endl << " Number of Modules:" << m_HBReport.nNumOfModules << endl;

  for (UI_32 j = 0; j < m_HBReport.nNumOfModules; j++) {  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    // LCOV_EXCL_START 6: Because the applicable variable cannot be changed from the external API (only evaluated by the initial value)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    if (0 != m_HBReport.tModuleList[j].HeartBeatRetryCount) {
      l_debugDumpBuf << endl << " " << m_HBReport.tModuleList[j].ProcQueueName << ","
          << m_HBReport.tModuleList[j].ProcHBState << ", "
          << m_HBReport.tModuleList[j].HeartBeatRetryCount << endl;
    }
    // LCOV_EXCL_STOP
  }

  std::string l_debugDumpStr = l_debugDumpBuf.str();

  SendDebugDumpResponseToSystemManager(l_debugDumpStr);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup   OnProcessLaunchMonitorTimerExpiry
///           This sends request to launcher thread to confirm that it
///           is still responsive.
///
/// \param [in] hApp
///         HANDLE - Handle of the Client Application
///
/// \return Status
///         EFrameworkunifiedStatus - success or error
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnProcessLaunchMonitorTimerExpiry(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Received from %s", FrameworkunifiedGetMsgSrc(hApp));
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  REC_HIST_IF_SUCCESSFUL("TIMER_PROCESSLAUNCHER_MONITOR", m_TimerCmdHist,  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
        m_TimerHistIter, FrameworkunifiedGetMsgSrc(hApp), l_eStatus);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)

  // LCOV_EXCL_BR_START 4: nsfw error
  if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedSendChild(hApp, m_hProcLauncherThread.m_ThreadHdl,
        ePLThrdCmd_THREAD_STATUS_REQST, 0, NULL))) {
  // LCOV_EXCL_BR_STOP 4: nsfw error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("FrameworkunifiedSendChild(m_hProcLauncherThread.m_ThreadHdl,"  // LCOV_EXCL_LINE 4: nsfw error
        "ePLThrdCmd_THREAD_STATUS_REQST)");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  } else {
    m_hProcLauncherThread.m_ThreadState = eSMWaitForHeartbeat;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  SendRequestToLauncher
///           This function is used for sending request to launcher thread from
///           System Manager.
///
/// \param HANDLE hApp
///      ModuleLaunchListIter f_ModuleListIterator => iterator node from map for the module
///        ESMPLThreadCommandIds f_CommandId         => command id to send
///        SS_String f_ModulePath                    => Path of the Module
///
/// \return Status
///         EFrameworkunifiedStatus - success or error
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::PerformLaunchProcedure(HANDLE hApp,
  ModuleLaunchListIter f_ModuleIterator, SS_String & f_stopCompName) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  SS_String s_ModuleStatus2;

  // Register for Stop Complete notification
  if (!f_ModuleIterator->shutdown_critical) {
    s_ModuleStatus2 = "is not shutdown_critical";
  // LCOV_EXCL_BR_START 6: For setting the initialization conditions at SystemManager startup

  } else if (eFrameworkunifiedStatusOK != (l_eStatus =
      FrameworkunifiedSubscribeNotificationWithCallback(
                hApp,
                f_stopCompName.c_str(),
                SysMgrCallback<CSystemManager, &CSystemManager::OnModuleStopCompleteNotification>))) {
  // LCOV_EXCL_BR_STOP
    // LCOV_EXCL_START 6: For setting the initialization conditions at SystemManager startup
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
         " Error: FrameworkunifiedSubscribeNotificationWithCallback(hApp, %s) errored: %d/'%s'",
    f_stopCompName.c_str(), l_eStatus, GetStr(l_eStatus).c_str());

    s_ModuleStatus2 = "is shutdown_critical but errored";
    // LCOV_EXCL_STOP
  } else {
    s_ModuleStatus2 = f_stopCompName + " is now registered";  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  }

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "%s", s_ModuleStatus2.c_str());

  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup   PerformTerminateProcedure
///           Perform Procedure for Service Terminate.
///
/// \param HANDLE hApp
///        ModuleLaunchListIter f_ModuleListIterator => iterator node from map for the module
///        SS_String & f_availabilityName            => Availability Name
///        SS_String & f_stopCompName                => Stop Complete Notification Name
///
/// \return Status
///         EFrameworkunifiedStatus - success or error
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::PerformTerminateProcedure(HANDLE hApp,
        ModuleLaunchListIter f_ModuleIterator, SS_String & f_availabilityName,
        SS_String & f_stopCompName) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  // close session
  f_ModuleIterator->SetModuleState(MODULE_STATE_INVALID);
  f_ModuleIterator->pid = 0;
  if (NULL != f_ModuleIterator->hsession) {  // LCOV_EXCL_BR_LINE 200:hsession must not be NULL.
    l_eStatus = FrameworkunifiedMcClose(f_ModuleIterator->hsession);  // LCOV_EXCL_BR_LINE 4: nsfw error
    if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 4: nsfw error
      // LCOV_EXCL_START 4: nsfw error
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: FrameworkunifiedMcClose(%s) errored: %d/'%s'",
          f_ModuleIterator->name.c_str(), l_eStatus, GetStr(l_eStatus).c_str());
      // LCOV_EXCL_STOP
    }
    f_ModuleIterator->hsession = NULL;
  }

  // Availability
  if (f_ModuleIterator->is_start_required) {
    // Initialize Availability
    HANDLE pPublishMq = McOpenSender(FRAMEWORKUNIFIED_NS_NPSERVICE);  // LCOV_EXCL_BR_LINE 4: nsfw error
    if (NULL == pPublishMq) {  // LCOV_EXCL_BR_LINE 4: nsfw error
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      SS_ASERT(0);  // LCOV_EXCL_LINE 4: nsfw error
    } else {
      ServiceAvailability tServiceAvailability;

      snprintf(tServiceAvailability.cServiceName, MAX_NAME_SIZE_APP, "%s",
          f_ModuleIterator->name.c_str());
      tServiceAvailability.eServiceAvailability = eFrameworkunifiedServiceNotAvailable;

      l_eStatus = NPPublishNotification(pPublishMq, f_ModuleIterator->name.c_str(),  // LCOV_EXCL_BR_LINE 4: nsfw error
                  f_availabilityName.c_str(), &tServiceAvailability,  // LCOV_EXCL_BR_LINE 4: nsfw error
        sizeof(tServiceAvailability));  // LCOV_EXCL_BR_LINE 4: nsfw error
      if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 4: nsfw error
        // LCOV_EXCL_START 4: nsfw error
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: NPPublishNotification(%s)",
            f_availabilityName.c_str());
        // LCOV_EXCL_STOP
      }

      l_eStatus = McClose(pPublishMq);  // LCOV_EXCL_BR_LINE 4: nsfw error
      if (eFrameworkunifiedStatusOK != l_eStatus) {
        // LCOV_EXCL_START 4: nsfw error
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        SS_ASERT(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
        // LCOV_EXCL_STOP
      }
    }
  }

  if (f_ModuleIterator->shutdown_critical) {
    // Unregister Stop Complete notification
    if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedUnsubscribeNotificationWithCallback(hApp, f_stopCompName.c_str()))) {  // LCOV_EXCL_BR_LINE 4: nsfw error  // NOLINT(whitespace/line_length)
      // LCOV_EXCL_START 4: nsfw error
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
          " Error: FrameworkunifiedUnsubscribeNotificationWithCallback(hApp, %s) errored: %d/'%s'",
          f_stopCompName.c_str(), l_eStatus, GetStr(l_eStatus).c_str());
      // LCOV_EXCL_STOP
    } else {
      SS_String s_ModuleStatus2 = f_stopCompName + " is now unregistered ";
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "%s", s_ModuleStatus2.c_str());
    }

    if (SS_SM_APPS_STOPPING_AT__CWORD56__REQ == m_SMCurrentState
        || SS_SM_APPS_STOPPING_AT_INTERNAL_REQ == m_SMCurrentState) {
      // When the shutdown_critical service terminates during the termination sequence, NAVI_DET is set to acquire the LOG at the next startup.
      // In the event of a Relaunch, NAVI_DET should be set because the likelihood of recovery is low.
        fprintf(stderr, "SS_SysManager/%s:will %s dead in Stopping \n", __FUNCTION__,
            f_ModuleIterator->name.c_str());
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "will %s dead in Stopping ",
            f_ModuleIterator->name.c_str());
        {
          RAM_AccessIf ram_access;
          RAM_SM_INFO_t info;
          SS_ASERT(eFrameworkunifiedStatusOK == ram_access.getRamInfo(&info));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
          info.isErrorReset = TRUE;
          SS_ASERT(eFrameworkunifiedStatusOK == ram_access.setRamInfo(&info));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
        }
    }
  }

  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  SendRequestToLauncher
///           This function is used for sending request to launcher thread from
///           System Manager.
///
/// \param HANDLE hApp
///      ModuleLaunchListIter f_ModuleListIterator => iterator node from map for the module
///        ESMPLThreadCommandIds f_CommandId         => command id to send
///        SS_String f_ModulePath                    => Path of the Module
///
/// \return Status
///         EFrameworkunifiedStatus - success or error
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::SendRequestToLauncher(HANDLE hApp,
        ModuleLaunchListIter f_ModuleIterator, ESMPLThreadCommandIds f_CommandId,
        SS_String f_CommandIdStr) {
  EFrameworkunifiedStatus l_eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  SS_String l_ModulePath = m_BinaryFilesPath + f_ModuleIterator->path;

  T_ProcessLauncherLaunchReq l_ModuleLaunchReq = { };
  strcpy(l_ModuleLaunchReq.name, f_ModuleIterator->name.c_str());  // NOLINT
  strcpy(l_ModuleLaunchReq.path, l_ModulePath.c_str());  // NOLINT
  strncpy(l_ModuleLaunchReq.args, f_ModuleIterator->arguments.c_str(),  // NOLINT
  sizeof(l_ModuleLaunchReq.args) - 1);
  strcpy(l_ModuleLaunchReq.logging_mask,  // NOLINT
  f_ModuleIterator->logging_msk_str.c_str());
  l_ModuleLaunchReq.priority = f_ModuleIterator->configuration_priority;
  l_ModuleLaunchReq.groupIterator = m_MapProclaunchGrps.find(
  f_ModuleIterator->group_id);
  l_ModuleLaunchReq.moduleIterator = f_ModuleIterator;

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
      "Queue:%s, path:%s, args:%s, lmsk:%s, prio:%d, cmd:%s", l_ModuleLaunchReq.name,
  l_ModuleLaunchReq.path, l_ModuleLaunchReq.args, l_ModuleLaunchReq.logging_mask,
  l_ModuleLaunchReq.priority, f_CommandIdStr.c_str());

  if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedSendChild(hApp, m_hProcLauncherThread.m_ThreadHdl, f_CommandId, sizeof(l_ModuleLaunchReq), reinterpret_cast<void *>(&l_ModuleLaunchReq)))) {  // LCOV_EXCL_BR_LINE 4: Because an initialization condition is set at SystemManager startup and the branching condition cannot be satisfied // NOLINT(whitespace/line_length)
    // LCOV_EXCL_START 4: Because an initialization condition is set at SystemManager startup and the branching condition cannot be satisfied
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        " Error: FrameworkunifiedSendChild( %s, %s ) errored: %d/'%s'", l_ModulePath.c_str(),
        f_CommandIdStr.c_str(), l_eStatus, GetStr(l_eStatus).c_str());
    // LCOV_EXCL_STOP
  } else {
    SS_String l_availabilityName = f_ModuleIterator->name + "/Availability";  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    SS_String l_stopCompName = f_ModuleIterator->name + "/StopComp";  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    // Perform command-specific processing
    switch (f_CommandId) {  // LCOV_EXCL_BR_LINE 6: Because the case is fixed on the call from the SystemManager::Initialize
      // LCOV_EXCL_START 6: Because the case is fixed on the call from the SystemManager::Initialize
      case ePLThrdCmd_RELAUNCH_MODULE_REQST:
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        l_eStatus = PerformTerminateProcedure(hApp, f_ModuleIterator,
                    l_availabilityName, l_stopCompName);
        if (eFrameworkunifiedStatusOK != l_eStatus) {
          SS_ASERT(0);
        }

        l_eStatus = PerformLaunchProcedure(hApp, f_ModuleIterator, l_stopCompName);
        if (eFrameworkunifiedStatusOK != l_eStatus) {
          SS_ASERT(0);
        }
        break;
      // LCOV_EXCL_STOP

      case ePLThrdCmd_LAUNCH_MODULE_REQST:
        l_eStatus = PerformLaunchProcedure(hApp, f_ModuleIterator, l_stopCompName);
        if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 6: For setting the initialization conditions at SystemManager startup
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          SS_ASERT(0);  // LCOV_EXCL_LINE 6: For setting the initialization conditions at SystemManager startup
        }
        break;

      case ePLThrdCmd_TERMINATE_MODULE_REQST:
        l_eStatus = PerformTerminateProcedure(hApp, f_ModuleIterator,
                    l_availabilityName, l_stopCompName);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
        if (eFrameworkunifiedStatusOK != l_eStatus) {
          SS_ASERT(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
        }
        break;

      // LCOV_EXCL_START 6: Because the case is fixed on the call from the SystemManager::Initialize
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      case ePLThrdCmd_LAUNCH_MODULE_RESP:
      case ePLThrdCmd_MODULE_STATUS_REQST:
      case ePLThrdCmd_RELAUNCH_MODULE_RESP:
      case ePLThrdCmd_TERMINATE_MODULE_RESP:
      case ePLThrdCmd_THREAD_STATUS_REQST:
      case ePLThrdCmd_THREAD_STATUS_RESP:
      case ePLThrdCmd_NONE:
        break;
      // default:  Don't code a 'default' here - let the compiler
      // issue a warning ( set via -Wall or -Wswitch ) when the set of
      // enumerations changes - then the maintainer will
      // automagically know to update this switch statement.
      // LCOV_EXCL_STOP
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::SendRequestToLauncher(

///////////////////////////////////////////////////////////////////////////////
/// \ingroup RemoveModuleEntryFromHB
///          This function is called to send module information to heart beat thread
///          which is to be removed from its list of services to be monitored.
///
/// \param   HANDLE hApp,
///      const CHAR *f_ModuleName  => Module Name
///
/// \return Status
///         EFrameworkunifiedStatus - success or error
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::RemoveModuleEntryFromHB(HANDLE hApp, const CHAR *f_ModuleName) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  HBListIter l_HBIter = std::find(m_HBList.begin(), m_HBList.end(), f_ModuleName);
  if (l_HBIter != m_HBList.end()) {
    m_HBList.erase(l_HBIter);
  }

  TSMRequestMessage l_ModuleDetails;
  strncpy(l_ModuleDetails.pstModuleName, f_ModuleName,
          sizeof(l_ModuleDetails.pstModuleName) - 1);
  l_ModuleDetails.pstModuleName[sizeof(l_ModuleDetails.pstModuleName) - 1] = '\0';
  l_eStatus = SendRequestToHeartBeat(hApp, SS_HEARTBEAT_DELETE_MODULE_ENTRY,
                &l_ModuleDetails, sizeof(TSMRequestMessage));

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnHeartBeatThreadHeartbeatResponse
///          This function gets called when heart beat thread replies the
///          heart beat query sent by the System Manager.
/// \param [in] hApp
///         HANDLE - Handle of the Client Application
///
/// \return Status
///         EFrameworkunifiedStatus - success or error
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnHeartBeatThreadHeartbeatResponse(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_PERIODIC_INFO, __FUNCTION__, "HeartBeat HeartBeat Received");
  m_hHeartbeatThread.m_ThreadState = eSMThreadIsFine;
  return eFrameworkunifiedStatusOK;
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnCheckAvailResponse
///          This function gets service's availability status from HBThread
/// \param [in] hApp
///         HANDLE - Handle of the Client Application
///
/// \return Status
///         EFrameworkunifiedStatus - success or error
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnCheckAvailResponse(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  THbAvailCheck result;

  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg < THbAvailCheck > (hApp, result))) {
    LOG_ERROR("ReadMsg()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_tempaltes.h // NOLINT(whitespace/line_length)
  } else if (result.isOk == FALSE) {
  }
  return eFrameworkunifiedStatusOK;
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnProcessLauncherThreadHeartbeatResponse
///          This function gets called when launcher thread replies the
///          heart beat query sent by the System Manager.
/// \param [in] hApp
///         HANDLE - Handle of the Client Application
///
/// \return Status
///         EFrameworkunifiedStatus - success or error
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnProcessLauncherThreadHeartbeatResponse(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_PERIODIC_INFO, __FUNCTION__, "ProcessLaunch HeartBeat Received");
  m_hProcLauncherThread.m_ThreadState = eSMThreadIsFine;
  return eFrameworkunifiedStatusOK;
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup RegisterCallbacksForRequester
/// Register callbacks for External Protocol commands
///
/// \param [in]
///
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::RegisterCallbacksForRequester(HANDLE hApp, PCSTR f_pRequester) {
  EFrameworkunifiedStatus l_eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // Attach Session Protocol messages.
  FrameworkunifiedProtocolCallbackHandler l_sm_protocol_handlers[] = {
    // Command ID,              Call back functions
     { SS_SM_START_COMPL_RSPN,
         SysMgrCallback<CSystemManager, &CSystemManager::OnModuleStartCompleteResponse> },
     { SS_SM_STOP_COMPL_RSPN,
         SysMgrCallback<CSystemManager, &CSystemManager::OnModuleStopCompleteResponse> },
     { SS_SM_PRE_START_COMPL_RSPN,
         SysMgrCallback<CSystemManager, &CSystemManager::OnModuleStartCompleteResponse> },
     { SS_SM_PRE_STOP_COMPL_RSPN,
         SysMgrCallback<CSystemManager, &CSystemManager::OnModuleStartCompleteResponse> },
     { SS_SM_BACKGROUND_START_COMPL_RSPN,
         SysMgrCallback<CSystemManager, &CSystemManager::OnModuleStartCompleteResponse> },
     { SS_SM_BACKGROUND_STOP_COMPL_RSPN,
         SysMgrCallback<CSystemManager, &CSystemManager::OnModuleStartCompleteResponse> },
    { SS_SM_GET_START_EXT_INFO, SysMgrCallback<CSystemManager,
                                &CSystemManager::OnGetStartExtInfo> },
    { SS_SM_GET_STOP_EXT_INFO,  SysMgrCallback<CSystemManager,
                                &CSystemManager::OnGetStopExtInfo> },
    { SS_SM_CRNT_STATE_QUERY,   SysMgrCallback<CSystemManager,
                                &CSystemManager::OnCurrentSMStateQuery> },
    { SS_SM_BOOT_MODE_SET_REQ,  SysMgrCallback<CSystemManager,
                                &CSystemManager::OnSetBootModeRequest> },
    { SS_SM_DATA_RESET_MODE_SET_REQ, SysMgrCallback<CSystemManager,
                                &CSystemManager::OnSetDataResetModeRequest> },
    { SS_SM_PROG_UPDATE_STATE_SET_REQ, SysMgrCallback<CSystemManager,
                                &CSystemManager::OnSetProgUpdateStateRequest> },
    { SS_SM_DEBUG_VAR_CODE_DATA_SET_REQ, SysMgrCallback<CSystemManager,
                                &CSystemManager::OnVarCodeSetRequest> },
    { SS_SM_SET_BOOTLOADER_INFO, SysMgrCallback<CSystemManager,
                                &CSystemManager::OnSetBootloaderInfoRequest> },

    { SS_SM_GET_BOOTLOADER_INFO, SysMgrCallback<CSystemManager,
                                &CSystemManager::OnGetBootloaderInfoRequest> },

    { SS_SM__CWORD56__HEARTBEAT_REQ, SysMgrCallback<CSystemManager,
                                &CSystemManager::On_CWORD56_HeartBeatRequest> },
    { SS_SM_WAKEUP_ORDER_SET_REQ, SysMgrCallback<CSystemManager,
                                &CSystemManager::OnSetWakeupOrderRequest> },
    { SS_SM_NEXT_WAKEUP_TYPE_SET_REQ, SysMgrCallback<CSystemManager,
                                &CSystemManager::OnSetNextWakeupTypeRequest> },
      // Error Event Logging
    { SS_SM_ERROR_EVENT_LOGGING_START_REQ, SysMgrCallback<CSystemManager,
                                &CSystemManager::OnLoggingStartRequest> },
    { SS_SM_ERROR_EVENT_ARTIFACT_REQ, SysMgrCallback<CSystemManager,
                                &CSystemManager::OnErrorEventArtifactRequest> },
    { SS_SM_ERROR_EVENT_LOGGING_COMPLETE, SysMgrCallback<CSystemManager,
                                &CSystemManager::OnErrorEventLoggingComplete> },
    { SS_SM_DEBUG_DUMP_RSPN,    SysMgrCallback<CSystemManager,
                                &CSystemManager::OnDebugDumpResponseReceived> },
    { SS_SM_PROPAGATE_SYSTEM_ERROR, SysMgrCallback<CSystemManager,
                                &CSystemManager::OnPropagateSystemError> },
    { SS_SM_BOOT_MICRO_RESET_NTF,   SysMgrCallback<CSystemManager,
                                &CSystemManager::OnBootMicroResetNotification> },
    { SS_SM_BOOT_MICRO_LOG_RSP, SysMgrCallback<CSystemManager,
                                &CSystemManager::OnBootMicroLogResponse> },
    { SS_SM_USER_INVOKED_LOG_REQ, SysMgrCallback<CSystemManager,
                                &CSystemManager::OnUserInvokedLoggingRequest> },
    { SS_SM_ERROR_EVENT_EEL_EXPORT_REQ, SysMgrCallback<CSystemManager,
                                &CSystemManager::OnEelExportRequest> },
    { SS_SM_ERROR_EVENT_INTERFACEUNIFIED_EMMC_LOGS_REQ, SysMgrCallback<CSystemManager,
                                &CSystemManager::OnSystemmanagerEmmcLogsRequest> },
    { SS_SM_ERROR_EVENT_CLR_LOGS_REQ, SysMgrCallback<CSystemManager,
                                &CSystemManager::OnSystemmanagerClearLogsRequest> },
    { SS_SM_ERROR_EVENT_DIAG_LOG_REQ, SysMgrCallback<CSystemManager,
                                &CSystemManager::OnDiagLoggingRequest> },
    { SS_SM_ERROR_EVENT_CAN_LOG_REQ, SysMgrCallback<CSystemManager,
                                &CSystemManager::OnCANLoggingRequest> } };  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

    l_eStatus = FrameworkunifiedAttachCallbacksToDispatcher(hApp, f_pRequester,
                l_sm_protocol_handlers, static_cast<UI_32>(_countof(l_sm_protocol_handlers)));  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  if (eFrameworkunifiedStatusOK != l_eStatus) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        " Error: FrameworkunifiedAttachCallbacksToDispatcher(%s) errored: %d/'%s'", f_pRequester,
        l_eStatus, GetStr(l_eStatus).c_str());
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::RegisterCallbacksForRequester( HANDLE hApp, PCSTR f_pRequester)

///////////////////////////////////////////////////////////////////////////////
/// \ingroup open_session_with_sm_test_client
/// Process request to open session with sm test client
///
/// \param [in]
///
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::open_session_with_sm_test_client(HANDLE hApp, PCSTR pRequester) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;
  UI_32 const l_ui32DefaultSessionID = 0;
  OpenSessionAck tOpenAck = { };

/*
 * Here pRequester is SMTesteClient with whom session is opened and its handle is stored in m_hPowerServiceSession handle.
 * This case comes only while testing SystemManager through SMTestClient (PowerService should not be running).
 */
  // Validate session handle
  if (NULL == m_hPowerServiceSession) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        " Error: Session Handle NULL, calling FrameworkunifiedMcOpenSender for %s", pRequester);

    m_hPowerServiceSession = FrameworkunifiedMcOpenSender(hApp, pRequester);
  //
  // Set the Session Handle for Framework so FrameworkunifiedSendResponse won't error due to a null
  // session handle.
  //
    // LCOV_EXCL_BR_START 4:NSFW's error
    if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedSetSessionHandle(hApp,
        pRequester,                  // [in] PCSTR - Name of the associated service name
        m_hPowerServiceSession))) {  // [in] HANDLE - Session handle
    // LCOV_EXCL_BR_STOP
      // LCOV_EXCL_START 4:NSFW's error
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
          " Error: FrameworkunifiedSetSessionHandle(%s) errored: %d/'%s'", pRequester,
          l_eStatus, GetStr(l_eStatus).c_str());
      FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
      return l_eStatus;
      // LCOV_EXCL_STOP
    }
  }

  if (m_hPowerServiceSession == NULL) {  // LCOV_EXCL_BR_LINE 200:m_hPowerServiceSession must not be NULL
    // LCOV_EXCL_START 200:m_hPowerServiceSession must not be NULL
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        " Error: Invalid Session Handle to service (%s) ", pRequester);
    // LCOV_EXCL_STOP
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Session opened with %s", pRequester);

    // Copy session name to data structure
    strncpy(tOpenAck.cSessionName, FrameworkunifiedGetAppName(hApp),
            sizeof(tOpenAck.cSessionName) - 1);
    tOpenAck.eStatus = eFrameworkunifiedStatusOK;
    tOpenAck.sessionId = l_ui32DefaultSessionID;

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "About to send Open Session ACK");
    // send OpenSession ACK
    // LCOV_EXCL_BR_START 4:NSFW's error
    if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedSendMsg(m_hPowerServiceSession,
            PROTOCOL_OPEN_SESSION_ACK, sizeof(OpenSessionAck),
            (PVOID) & tOpenAck))) {
    // LCOV_EXCL_BR_STOP
      // LCOV_EXCL_START 4:NSFW's error
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      LOG_ERROR("FrameworkunifiedSendMsg(PROTOCOL_OPEN_SESSION_ACK)");

      // Close the message queue handle
      if (NULL != m_hPowerServiceSession) {
        CALL_AND_LOG_STATUS_IF_ERRORED(FrameworkunifiedMcClose(m_hPowerServiceSession));
        m_hPowerServiceSession = NULL;
      }
      // LCOV_EXCL_STOP
    } else {
      CALL_AND_LOG_STATUS_IF_ERRORED(RegisterCallbacksForRequester(hApp, pRequester));
    }
  }  // end of else block of if(l_ModuleListIterator->hsession == NULL)

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnOpenSession
/// Process request to open session
///
/// \param [in]
///
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnOpenSession(HANDLE hApp) noexcept {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  INTERFACEUNIFIEDLOG_RECEIVED_FROM(hApp);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  UI_32 const l_ui32DefaultSessionID = 0;
  UI_32 const l_ui32DefaultSessionType = 0;
  OpenSessionAck tOpenAck = { };
  PCSTR pRequester = FrameworkunifiedGetMsgSrc(hApp);
  FRAMEWORKUNIFIEDLOG(ZONE_PERFORMANCE, __FUNCTION__, "%s", pRequester);

  ModuleLaunchListIter l_ModuleListIter;

  REC_HIST_IF_SUCCESSFUL("PROTOCOL_OPEN_SESSION_REQ", m_SMCmdHist, m_SMHistIter,  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
    FrameworkunifiedGetMsgSrc(hApp), l_eStatus);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)

  l_eStatus = GetModuleIterator(pRequester, l_ModuleListIter);

  if (eFrameworkunifiedStatusOK != l_eStatus) {
    // Special handling for open session request from SMTestClient.
    if ((0 == strcmp(TC_SysManager, pRequester))) {
      open_session_with_sm_test_client(hApp, pRequester);
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: %s not found in Group Launch Map",
          pRequester);
    }
  } else {
    /**********************************************************************/
    if (0 != l_ModuleListIter->pid) {
      UI_32 l_ClientPriority;
      l_eStatus = l_ModuleListIter->GetPriority(l_ClientPriority);

      if (eFrameworkunifiedStatusOK != l_eStatus) {
        LOG_ERROR("l_ModuleListIter->GetPriority( l_ClientPriority )");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
      } else if (l_ClientPriority != l_ModuleListIter->configuration_priority) {
        FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, " Changing %s's priority from %d to %d",
            pRequester, l_ClientPriority,
            l_ModuleListIter->configuration_priority);
        CALL_AND_LOG_STATUS_IF_ERRORED( // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
            l_ModuleListIter->SetPriority( // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
                    l_ModuleListIter->configuration_priority));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
      }
    }
    /**********************************************************************/

    // ToDo Jay 2013 Jan 09 REPLACE WITH CORRECT SERVICE / SESSION HANDLE
    //                      LOGIC
    // validate session handle
    if (NULL == l_ModuleListIter->hsession) {
      // LCOV_EXCL_BR_START 4: nsfw error
      if (NULL == (l_ModuleListIter->hsession = FrameworkunifiedMcOpenSender(hApp, pRequester))) {
      // LCOV_EXCL_BR_STOP 4: nsfw error
        // LCOV_EXCL_START 4: nsfw error
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
            " Error: FrameworkunifiedMcOpenSender(App %s, %s) returned NULL",
            FrameworkunifiedGetAppName(hApp), pRequester);
        return eFrameworkunifiedStatusInvldHandle;
        // LCOV_EXCL_STOP
      }
    } else {
    /*
     * Currently System Manager does not support multiple sessions from same client.
     * When a client gets restarted after Crash or after missing HeartBeat, System Manager
     * would receive OpenSession request from that client even when System Manager already
     * has a open session with that client.
     */
      FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__,
          " Warning: Repeated Session Opening Request from %s", pRequester);
    }
    if (eFrameworkunifiedStatusOK != (l_eStatus = RegisterCallbacksForRequester(hApp, pRequester))) {
      // Close the message queue handle
      if (NULL != l_ModuleListIter->hsession) {  // LCOV_EXCL_BR_LINE 6: Due to NULL checking
        if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedMcClose(l_ModuleListIter->hsession))) {
          LOG_ERROR("FrameworkunifiedMcClose()");  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
        }
        l_ModuleListIter->hsession = NULL;
      }
      FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
      return l_eStatus;
    }

    // Copy session name to data structure
    strncpy(tOpenAck.cSessionName, FrameworkunifiedGetAppName(hApp), sizeof(tOpenAck.cSessionName) - 1);
    tOpenAck.eStatus = eFrameworkunifiedStatusOK;
    tOpenAck.sessionId = l_ui32DefaultSessionID;
    tOpenAck.sessionType = l_ui32DefaultSessionType;

    // send OpenSession ACK
    // LCOV_EXCL_BR_START 4: nsfw error
    if (eFrameworkunifiedStatusOK
        != (l_eStatus = FrameworkunifiedSendMsg(l_ModuleListIter->hsession,
            SS_SM_PROTOCOL_OPEN_SESSION_ACK, sizeof(OpenSessionAck),
            (PVOID) & tOpenAck))) {  // LCOV_EXCL_BR_STOP
      // LCOV_EXCL_START 4: nsfw error
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
          " Error: FrameworkunifiedSendMsg( %s, SS_SM_PROTOCOL_OPEN_SESSION_ACK)"
          " errored: %d/'%s'", l_ModuleListIter->name.c_str(), l_eStatus,
          GetStr(l_eStatus).c_str());

      if (NULL != l_ModuleListIter->hsession) {
        // Close the message queue handle
        if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedMcClose(l_ModuleListIter->hsession))) {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
              " Error: FrameworkunifiedMcClose(%s) errored: %d/'%s'",
              l_ModuleListIter->name.c_str(), l_eStatus,
              GetStr(l_eStatus).c_str());
          FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
        }
        l_ModuleListIter->hsession = NULL;
      }
      // LCOV_EXCL_STOP
    } else {
      // Store PowerService session handle in class so it can be accessed faster
      // to use opened session while replying to PowerService.
      if ((0 == strcmp(SERVICE_POWER, pRequester))) {
        m_hPowerServiceSession = l_ModuleListIter->hsession;
      }

      l_ModuleListIter->SetModuleState(MODULE_STATE_CONNECTED);

      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " SM State: %s, %s.is_start_required: %s",
          GetStr(m_SMCurrentState).c_str(), l_ModuleListIter->name.c_str(),
          GetStr(l_ModuleListIter->is_start_required).c_str());

      if (l_ModuleListIter->is_start_required
          && ((SS_SM_APPS_START_IN_PROGRESS == m_SMCurrentState)
          || (SS_SM_APPS_START_COMPLETE == m_SMCurrentState))) {
          T_SS_SM_START_DataStructType f_startupData(m_StartUpReason,
            m_PowerType_to_SSBoolEnumMap[m_Wake.powerupType], m_DataResetMode,
            m_startUpConfirmationMsg.securityStatus,
            m_startUpConfirmationMsg.wakeupType, m_DramBackupStatus,
            m_ResetStatus, m_ResetCount);
          const UI_32 l_iCmd = InProgressStateToSendMsg();
          if (l_iCmd != SS_SYSTEM_MANAGER_PROTOCOL_ENDING_INDEX) {
            CALL_AND_LOG_STATUS_IF_ERRORED(l_ModuleListIter->SendMsgAndUpdateState(l_iCmd, &f_startupData));  // LCOV_EXCL_BR_LINE 11: Gcov constraints (because exception-handling routes are automatically generated) // NOLINT(whitespace/line_length)
          }
      }
    }
  }

  if (eFrameworkunifiedStatusOK == l_eStatus) {
    std::string l_Subscriber;
    l_Subscriber = FrameworkunifiedGetMsgSrc(hApp);

    // check if the heart beat subscriber is already in list
    HBListIter l_HBIter = std::find(m_HBList.begin(), m_HBList.end(), l_Subscriber);
    if (l_HBIter == m_HBList.end()) {
      m_HBList.push_back(l_Subscriber);
    }
    if (m_SMCurrentState == SS_SM_APPS_START_COMPLETE) {
      TSMRequestMessage l_ModuleDetails;
      snprintf(l_ModuleDetails.pstModuleName, sizeof(l_ModuleDetails.pstModuleName),
          "%s", l_Subscriber.c_str());
      l_eStatus = SendRequestToHeartBeat(hApp, SS_HEARTBEAT_APPEND_MODULE_ENTRY,
          &l_ModuleDetails, sizeof(TSMRequestMessage));
      LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus, "SendRequestToHeartBeat()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////
/// ConstructGetResultResponse
///
///
//////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::add_state_information_to_response(CHAR *f_MessageResponse) const {
  const CHAR ReadyToLaunchAppDescription[] = "_READY_TO_LAUNCH:";
  const CHAR StartInProgressDescription[] = "_START_IN_PROGRESS:";
  const CHAR LaunchInProgressDescription[] = "_LAUNCH_IN_PROGRESS:";
  const CHAR LaunchedReadyToStartDescription[] = "_READY_TO_START:";
  const CHAR StopInProgressDescription[] = "_STOP_IN_PROGRESS:";
  const CHAR StartCompleteDescription[] = "_START_COMPLETED:";
  const CHAR WaitingForCriticalAppsToStopDescription[] = "_WAITING_FOR_CRITICAL_APPS_TO_STOP:";
  const CHAR PreStartInProgressDescription[] = "_PRE_START_IN_PROGRESS:";
  const CHAR PreStopInProgressDescription[] = "_PRE_STOP_IN_PROGRESS:";
  const CHAR PreRunCompleteDescription[] = "_PRE_RUN_COMPLETE:";
  const CHAR BackgroundStartInProgressDescription[] = "_BACKGROUND_START_IN_PROGRESS:";
  const CHAR BackgroundStopInProgressDescription[] = "_BACKGROUND_STOP_IN_PROGRESS:";
  const CHAR BackgroundRunCompleteDescription[] = "_BACKGROUND_RUN_COMPLETE:";
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  CHAR l_Buffer[5];
  UI_32 l_Index = 0;

  std::map<SS_SMModuleState, std::string> StateMap;   // State Map
  StateMap[SS_SM_READY_TO_LAUNCH_APP] = ReadyToLaunchAppDescription;
  StateMap[SS_SM_APPS_LAUNCH_IN_PROGRESS] = LaunchInProgressDescription;
  StateMap[SS_SM_APPS_LAUNCHED_READY_TO_START] = LaunchedReadyToStartDescription;
  StateMap[SS_SM_APPS_START_IN_PROGRESS] = StartInProgressDescription;
  StateMap[SS_SM_APPS_START_COMPLETE] = StartCompleteDescription;
  StateMap[SS_SM_APPS_STOPPING_AT__CWORD56__REQ] = StopInProgressDescription;
  StateMap[SS_SM_APPS_STOPPING_AT_INTERNAL_REQ] = StopInProgressDescription;
  StateMap[SS_SM_WAITING_FOR_CRITICAL_APPS_AT_INTERNAL_REQ] =
      WaitingForCriticalAppsToStopDescription;
  StateMap[SS_SM_WAITING_FOR_CRITICAL_APPS_AT__CWORD56__REQ] =
      WaitingForCriticalAppsToStopDescription;
  StateMap[SS_SM_APPS_PRE_START_IN_PROGRESS] = PreStartInProgressDescription;
  StateMap[SS_SM_APPS_PRE_STOP_IN_PROGRESS] = PreStopInProgressDescription;
  StateMap[SS_SM_APPS_PRE_RUN_COMPLETE] = PreRunCompleteDescription;
  StateMap[SS_SM_APPS_BACKGROUND_START_IN_PROGRESS] = BackgroundStartInProgressDescription;
  StateMap[SS_SM_APPS_BACKGROUND_STOP_IN_PROGRESS] = BackgroundStopInProgressDescription;
  StateMap[SS_SM_APPS_BACKGROUND_RUN_COMPLETE] = BackgroundRunCompleteDescription;

  itoa(m_SMCurrentState, l_Buffer, 16);
  while (l_Buffer[l_Index]) {
    l_Buffer[l_Index] = toupper(l_Buffer[l_Index]);
    l_Index++;
  }

  if (SS_SM_MAX_RESP_MSG_SIZE > (strlen(f_MessageResponse) + sizeof(l_Buffer)
      + StateMap[m_SMCurrentState].length() + 1)) {
    strncat(f_MessageResponse, l_Buffer, sizeof(l_Buffer));
    strncat(f_MessageResponse, StateMap[m_SMCurrentState].c_str(),
    StateMap[m_SMCurrentState].length());
  } else {
    eStatus = eFrameworkunifiedStatusFail;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

///////////////////////////////////////////////////////////////////////
/// ConstructGetResultResponse
///
///
//////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::construct_get_result_response(CHAR *f_MessageResponse) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  const EFrameworkunifiedStatus  eStatus = add_state_information_to_response(f_MessageResponse);
  if (eFrameworkunifiedStatusOK == eStatus) {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Response Constructed");
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

///////////////////////////////////////////////////////////////////////
/// REC_HIST_IF_SUCCESSFUL / SetCmdHist
///
///
//////////////////////////////////////////////////////////////////////
void CSystemManager::SetCmdHist(std::string cmd, cmdHist &hist, cmdHistIter &it, std::string sender) {
  UI_64 l_time = 0XDEAD;
  CTimeSpan* timerClass = new (std::nothrow) CTimeSpan();  // LCOV_EXCL_BR_LINE 5: Because new cannot fail
  if (NULL == timerClass) {  // LCOV_EXCL_BR_LINE 5: Because new cannot fail
    // LCOV_EXCL_START 5: Because new cannot fail
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        " 'new (std::nothrow) CTimeSpan()' returned NULL, "
        "'it->m_time' being set to 0xDEAD");
    // LCOV_EXCL_STOP
  } else {
    l_time = timerClass->GetTimeMilliseconds();
  }

  pthread_mutex_lock(&sm_hist_mutex);

  it->m_time = l_time;
  it->m_cmd = cmd;
  it->m_sender = sender;
  it++;

  if (it != hist.end()) {  // LCOV_EXCL_BR_LINE 5: std: because it is a member of a vector and it is difficult to create a failing condition
    it = hist.begin();
  }

  pthread_mutex_unlock(&sm_hist_mutex);

  delete timerClass;  // LCOV_EXCL_BR_LINE 5: Because the delete cannot fail
}

///////////////////////////////////////////////////////////////////////
/// OnCurrentPowerStateQuery
///
///
//////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnCurrentSMStateQuery(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  INTERFACEUNIFIEDLOG_RECEIVED_FROM(hApp);
  FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "Received from %s", FrameworkunifiedGetMsgSrc(hApp));

  SS_SMCurrentState l_CurrentState;

  REC_HIST_IF_SUCCESSFUL("SS_SM_CRNT_STATE_QUERY", m_SMCmdHist, m_SMHistIter,
                         FrameworkunifiedGetMsgSrc(hApp), l_eStatus);
  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg < SS_SMCurrentState > (hApp, l_CurrentState))) {
    LOG_ERROR("ReadMsg()");  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  } else {
    construct_get_result_response(l_CurrentState.respMsgString);

    ModuleLaunchListIter l_ModuleListIter;
    PCSTR l_moduleName = FrameworkunifiedGetMsgSrc(hApp);
    if (eFrameworkunifiedStatusOK != (l_eStatus = GetModuleIterator(l_moduleName, l_ModuleListIter))) {  // LCOV_EXCL_BR_LINE 200: can not be other value  // NOLINT (whitespace/line_length)
      // LCOV_EXCL_START 200: can not be other value
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
          " Error: Module %s not found in Group Launch Map", l_moduleName);
      // LCOV_EXCL_STOP
    } else if (l_ModuleListIter->hsession != NULL) {  // LCOV_EXCL_BR_LINE 200: can not be NULL // NOLINT (whitespace/line_length)
      if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedSendMsg(l_ModuleListIter->hsession,  // LCOV_EXCL_BR_LINE 4: nsfw error
                SS_SM_CRNT_STATE_QUERY_RSPN, sizeof(SS_SMCurrentState),
                (PVOID) & l_CurrentState))) {
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        LOG_ERROR("FrameworkunifiedSendMsg(SS_SM_CRNT_STATE_QUERY_RSPN)");  // LCOV_EXCL_LINE 4: nsfw error
      }
    } else {
      // LCOV_EXCL_START 200: can not be NUL
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " NULL == hsession; %s not connected yet",
          l_ModuleListIter->name.c_str());
      // LCOV_EXCL_STOP
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus CSystemManager::GetVersionNumber(SS_VersionNumberType & f_VersionNumber) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;

  CSMVersion cVersion;
  if (eFrameworkunifiedStatusOK != (l_eStatus = cVersion.get_version(f_VersionNumber))) {  // LCOV_EXCL_BR_LINE 6: For setting the initialization conditions at SystemManager startup // NOLINT(whitespace/line_length)
    LOG_ERROR("cVersion.get_version()");  // LCOV_EXCL_BR_LINE 6: For setting the initialization conditions at SystemManager startup // NOLINT(whitespace/line_length)
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::GetVersionNumber( SS_VersionNumberType

EFrameworkunifiedStatus CSystemManager::GetBuildInfo(std::string &f_BuildInfoStr) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;

  CSMVersion cVersion;
  if (eFrameworkunifiedStatusOK != (l_eStatus = cVersion.get_build_info(f_BuildInfoStr))) {  // LCOV_EXCL_BR_LINE 6: For setting the initialization conditions at SystemManager startup // NOLINT(whitespace/line_length)

    LOG_ERROR("cVersion.get_build_info()");  // LCOV_EXCL_BR_LINE 6:For setting the initialization conditions at SystemManager startup // NOLINT(whitespace/line_length)
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::GetBuildInfo(std::string)

EFrameworkunifiedStatus CSystemManager::GetBootMode(ROM_AccessIf & f_cRomAccess, ESMBootModeInfo & f_BootMode) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (m_UsingVMPlayer) {  // LCOV_EXCL_BR_LINE 6: For setting the initialization conditions at SystemManager startup

    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    f_BootMode = e_SS_SM_BOOT_MODE_APPLICATION;  // LCOV_EXCL_LINE 6: For setting the initialization conditions at SystemManager startup
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
           " 'm_UsingVMPlayer' is True");
  } else {
    EBOOT_MODE l_boot_mode;
    // LCOV_EXCL_BR_START 6: Because RomAccessIf APIs cannot satisfy the fail condition
    if (eFrameworkunifiedStatusOK != (l_eStatus =
        f_cRomAccess.GetBootMode(&l_boot_mode))) {
    // LCOV_EXCL_BR_STOP
      // LCOV_EXCL_START 6: Because RomAccessIf APIs cannot satisfy the fail condition
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      f_BootMode = e_SS_SM_BOOT_MODE_INVALID;
      LOG_ERROR("f_cRomAccess.GetBootMode()");  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    } else if (l_boot_mode == APPLICATION_MODE) {
      f_BootMode = e_SS_SM_BOOT_MODE_APPLICATION;
      // LCOV_EXCL_STOP
    } else {
      f_BootMode = e_SS_SM_BOOT_MODE_PROGRAMMING;
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " BootMode is '%s'",
  GetStr(f_BootMode).c_str());  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::GetBootMode(

EFrameworkunifiedStatus CSystemManager::GetLastUserMode(ROM_AccessIf & f_cRomAccess, EPWR_USER_MODE_TYPE& f_lastUserMode) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  EUSER_MODE l_RomData;

  if (m_UsingVMPlayer) {  // LCOV_EXCL_BR_LINE 6: For setting the initialization conditions at SystemManager startup
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
           " 'm_UsingVMPlayer' is True");
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    f_lastUserMode = epsumON;  // LCOV_EXCL_LINE 6: For setting the initialization conditions at SystemManager startup
  } else {
    if (eFrameworkunifiedStatusOK != (l_eStatus = f_cRomAccess.GetLastUserMode(&l_RomData))) {  // LCOV_EXCL_BR_LINE 6: Because RomAccessIf APIs cannot satisfy the fail condition
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      LOG_ERROR("f_cRomAccess.GetLastUserMode()");  // LCOV_EXCL_LINE 6: Because RomAccessIf APIs cannot satisfy the fail condition
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      f_lastUserMode = epsumINVALID;  // LCOV_EXCL_LINE 6: Because RomAccessIf APIs cannot satisfy the fail condition
    } else {
      f_lastUserMode = m_ROM_to_SSUserModeEnumMap[l_RomData];
    }
  }
  if (eFrameworkunifiedStatusOK == l_eStatus) {  // LCOV_EXCL_LINE 6: Because RomAccessIf APIs cannot satisfy the fail condition
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Read LastUserMode of 'User%s'",
            f_lastUserMode == epsumON ? "On" : "Off");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::GetLastUserMode(

EFrameworkunifiedStatus CSystemManager::SaveLastUserModeToNVM(EPWR_USER_MODE_TYPE f_eUserModeState) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  if (m_UsingVMPlayer) {  // LCOV_EXCL_BR_LINE 6: For setting the initialization conditions at SystemManager startup
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__,
           " Warning: 'm_UsingVMPlayer' is True; "
        "unable to save LastUserMode to NVM");
  } else {
    // Initialize ROM access object
    ROM_AccessIf f_cRomAccess;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    f_cRomAccess.Initialize();  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    EUSER_MODE l_RomData = m_SS_to_ROMUserModeEnumMap[f_eUserModeState];  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

    if (eFrameworkunifiedStatusOK != (l_eStatus = f_cRomAccess.SetLastUserMode(l_RomData))) {
      LOG_ERROR("f_cRomAccess.SetLastUserMode()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Successfully wrote LastUserMode '%s' to NVM",
          l_RomData == USER_OFF ? "USER_OFF" : "USER_ON");
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::SaveLastUserModeToNVM(

EFrameworkunifiedStatus CSystemManager::GetLimpHomeCutoffMode(ROM_AccessIf & f_cRomAccess, EPWR_LHC_TYPE& f_limpHomeCutoff) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  ECONTROL_MODE l_RomData;

  if (m_UsingVMPlayer) {  // LCOV_EXCL_BR_LINE 6: For setting the initialization conditions at SystemManager startup
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
           " 'm_UsingVMPlayer' is True");
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    f_limpHomeCutoff = epslhcDISABLED;  // LCOV_EXCL_LINE 6: For setting the initialization conditions at SystemManager startup
  } else {
    if (eFrameworkunifiedStatusOK != (l_eStatus = f_cRomAccess.GetLimpHomeCutOffReqMode(&l_RomData))) {  // LCOV_EXCL_BR_LINE 6: Because RomAccessIf APIs cannot satisfy the fail condition // NOLINT(whitespace/line_length)

      LOG_ERROR("f_cRomAccess.GetLimpHomeCutOffReqMode()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      f_limpHomeCutoff = epslhcINVALID;  // LCOV_EXCL_LINE 6: Because RomAccessIf APIs cannot satisfy the fail condition
    } else {
      f_limpHomeCutoff = m_Control_to_LimpHomeCutoffMode_map[l_RomData];
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " LimpHomeCutoff is %s",
  GetStr(f_limpHomeCutoff).c_str());

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::GetLimpHomeCutoffMode(

EFrameworkunifiedStatus CSystemManager::GetProductionMode(ROM_AccessIf & f_cRomAccess, EPWR_PROD_MODE_TYPE& f_productionMode) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  ECONTROL_MODE l_RomData;

  if (m_UsingVMPlayer) {  // LCOV_EXCL_BR_LINE 6: For setting the initialization conditions at SystemManager startup
    // LCOV_EXCL_START 6: For setting the initialization conditions at SystemManager startup
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " 'm_UsingVMPlayer' is True");
    f_productionMode = epspmDISABLED;
    // LCOV_EXCL_STOP
  } else {
    if (eFrameworkunifiedStatusOK != (l_eStatus = f_cRomAccess.GetProductionMode(&l_RomData))) {  // LCOV_EXCL_BR_LINE 6: Because RomAccessIf APIs cannot satisfy the fail condition // NOLINT(whitespace/line_length)
      LOG_ERROR("f_cRomAccess.GetProductionMode()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      f_productionMode = epspmINVALID;  // LCOV_EXCL_LINE 6: RomAccessIf APIs cannot satisfy the fail condition
    } else {
      f_productionMode = m_Control_to_ProductionMode_map[l_RomData];
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Production Mode is %s",
  GetStr(f_productionMode).c_str());  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::GetProductionMode(

EFrameworkunifiedStatus CSystemManager::GetSignature(ROM_AccessIf & f_cRomAccess, UI_32 * pData) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (m_UsingVMPlayer) {  // LCOV_EXCL_BR_LINE 6: For setting the initialization conditions at SystemManager startup
    // LCOV_EXCL_START 6: For setting the initialization conditions at SystemManager startup
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: 'm_UsingVMPlayer' is True");
    l_eStatus = eFrameworkunifiedStatusInvldParam;
    // LCOV_EXCL_STOP
  } else {
    if (eFrameworkunifiedStatusOK != (l_eStatus = f_cRomAccess.GetSignature(pData))) {  // LCOV_EXCL_BR_LINE 6: Because RomAccessIf APIs cannot satisfy the fail condition // NOLINT(whitespace/line_length)
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      LOG_ERROR("f_cRomAccess.GetSignature()");  // LCOV_EXCL_LINE 6: Because RomAccessIf APIs cannot satisfy the fail condition
    }
  }
  if (eFrameworkunifiedStatusOK == l_eStatus) {  // LCOV_EXCL_BR_LINE 6: Because RomAccessIf APIs cannot satisfy the fail condition
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " NVM Signature is %llx",
        static_cast<long long unsigned int>(*pData));
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::GetSignature(ROM_AccessIf & f_cRomAccess

EFrameworkunifiedStatus CSystemManager::GetTransportMode(ROM_AccessIf & f_cRomAccess, EPWR_TRANSPORT_MODE_TYPE& f_transportMode) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  ECONTROL_MODE l_RomData;

  if (m_UsingVMPlayer) {  // LCOV_EXCL_BR_LINE 6: For setting the initialization conditions at SystemManager startup
    // LCOV_EXCL_START 6: For setting the initialization conditions at SystemManager startup
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: 'm_UsingVMPlayer' is True");
    l_eStatus = eFrameworkunifiedStatusInvldParam;
    // LCOV_EXCL_STOP
  } else {
    if (eFrameworkunifiedStatusOK != (l_eStatus = f_cRomAccess.GetTransportMode(&l_RomData))) {  // LCOV_EXCL_BR_LINE 6: Because RomAccessIf APIs cannot satisfy the fail condition // NOLINT(whitespace/line_length)
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      LOG_ERROR  // LCOV_EXCL_LINE 6: Because RomAccessIf APIs cannot satisfy the fail condition
      ("f_cRomAccess.GetTransportMode()");  // LCOV_EXCL_LINE 6: Because RomAccessIf APIs cannot satisfy the fail condition
    } else {
      f_transportMode = m_Control_to_TransportMode_map[l_RomData];
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Transport Mode is %s",
  GetStr(f_transportMode).c_str());

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::GetTransportMode(

EFrameworkunifiedStatus CSystemManager::GetDataResetMode(ROM_AccessIf & f_cRomAccess,
        ESMDataResetModeInfo& f_dataResetMode, bool isFast) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  EDATARESET_MODE l_DataResetMode;

  if (m_UsingVMPlayer) {  // LCOV_EXCL_BR_LINE 6: For setting the initialization conditions at SystemManager startup
    // LCOV_EXCL_START 6: For setting the initialization conditions at SystemManager startup
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: 'm_UsingVMPlayer' is True");
    l_eStatus = eFrameworkunifiedStatusInvldParam;
    // LCOV_EXCL_STOP
  } else {
    if (isFast) {  // LCOV_EXCL_BR_LINE 6: Because the condition cannot be set
      l_eStatus = f_cRomAccess.GetDataResetModeFast(&l_DataResetMode);
    } else {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_eStatus = f_cRomAccess.GetDataResetMode(&l_DataResetMode);  // LCOV_EXCL_LINE 6: Because the condition cannot be set
    }

    if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 6: Because RomAccessIf APIs cannot satisfy the fail condition
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      LOG_ERROR  // LCOV_EXCL_LINE 6: Because RomAccessIf APIs cannot satisfy the fail condition
      ("f_cRomAccess.GetDataResetMode()");  // LCOV_EXCL_LINE 6: Because RomAccessIf APIs cannot satisfy the fail condition
    } else {
      switch (l_DataResetMode) {  // LCOV_EXCL_BR_LINE 6: Because RomAccessIf APIs cannot satisfy the fail condition
        case DATARESET_NONE:
          f_dataResetMode = e_SS_SM_DATA_RESET_MODE_NONE;
          break;
        // LCOV_EXCL_START 6: Because values other than DATARESET NONE cannot be returned by RomAccessIf APIs
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        case DATARESET_USER:
          f_dataResetMode = e_SS_SM_DATA_RESET_MODE_USER;
          break;
        case DATARESET_FACTORY:
          f_dataResetMode = e_SS_SM_DATA_RESET_MODE_FACTORY;
          break;
        default:
          f_dataResetMode = e_SS_SM_DATA_RESET_MODE_NONE;
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: reset_mode = %d", l_DataResetMode);
          break;
        // LCOV_EXCL_STOP
      }
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Data Reset Mode is %s",
         f_dataResetMode == e_SS_SM_DATA_RESET_MODE_USER ? "DATARESET_USER" :
         f_dataResetMode == e_SS_SM_DATA_RESET_MODE_FACTORY ? "DATARESET_FACTORY" : "DATARESET_NONE");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
// End of EFrameworkunifiedStatus CSystemManager::GetDataResetMode(

EFrameworkunifiedStatus CSystemManager::GetResetCount(ROM_AccessIf & f_cRomAccess, UI_32& f_rebootCount) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (m_UsingVMPlayer) {  // LCOV_EXCL_BR_LINE 6: For setting the initialization conditions at SystemManager startup
    // LCOV_EXCL_START 6: For setting the initialization conditions at SystemManager startup
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: 'm_UsingVMPlayer' is True");
    l_eStatus = eFrameworkunifiedStatusInvldParam;
    // LCOV_EXCL_STOP
  } else {
    if (eFrameworkunifiedStatusOK != (l_eStatus = f_cRomAccess.GetResetCount(&f_rebootCount))) {  // LCOV_EXCL_BR_LINE 6: Because RomAccessIf APIs cannot satisfy the fail condition // NOLINT(whitespace/line_length)
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      LOG_ERROR("f_cRomAccess.GetResetCount()");  // LCOV_EXCL_LINE 6: Because RomAccessIf APIs cannot satisfy the fail condition
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Reboot Count is %d", f_rebootCount);
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
// End of EFrameworkunifiedStatus CSystemManager::GetResetCount(

EFrameworkunifiedStatus CSystemManager::GetLastIlgReset(ROM_AccessIf & f_cRomAccess, BOOL& f_lastIlgReset) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (m_UsingVMPlayer) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: 'm_UsingVMPlayer' is True");
    l_eStatus = eFrameworkunifiedStatusInvldParam;
  } else {
    ELASTILGRESET_MODE l_lastIlgReset;

    // LCOV_EXCL_BR_START 6: Because RomAccessIf APIs cannot satisfy the fail condition
    if (eFrameworkunifiedStatusOK
        != (l_eStatus = f_cRomAccess.GetLastIlgReset(&l_lastIlgReset))) {
    // LCOV_EXCL_BR_STOP
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      LOG_ERROR("f_cRomAccess.GetLastIlgReset()");  // LCOV_EXCL_LINE 6: Because RomAccessIf APIs cannot satisfy the fail condition
    }

    f_lastIlgReset = (l_lastIlgReset == LAST_ILGRESET_NG) ? TRUE : FALSE;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  }

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " LastIlgRerset is %s", f_lastIlgReset ? "LAST_ILGRESET_NORMAL" : "LAST_ILGRESET_NG");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
// End of EFrameworkunifiedStatus CSystemManager::GetLastIlgReset(

EFrameworkunifiedStatus CSystemManager::GetProgUpdateState(ROM_AccessIf & f_cRomAccess, SMProgUpdateState& f_progUpdateState) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (m_UsingVMPlayer) {  // LCOV_EXCL_BR_LINE 6: For setting the initialization conditions at SystemManager startup
    // LCOV_EXCL_START 6: For setting the initialization conditions at SystemManager startup
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error:m_UsingVMPlayer is True");
    l_eStatus = eFrameworkunifiedStatusInvldParam;
    // LCOV_EXCL_STOP
  } else {
    // LCOV_EXCL_BR_START 6: Because RomAccessIf APIs cannot satisfy the fail condition
    if (eFrameworkunifiedStatusOK
        != (l_eStatus = f_cRomAccess.GetProgUpdateState(&f_progUpdateState))) {
    // LCOV_EXCL_BR_STOP
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      LOG_ERROR  // LCOV_EXCL_LINE 6: Because RomAccessIf APIs cannot satisfy the fail condition
      ("f_cRomAccess.GetProgUpdateState()");  // LCOV_EXCL_LINE 6: Because RomAccessIf APIs cannot satisfy the fail condition
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
// End of EFrameworkunifiedStatus CSystemManager::GetProgUpdateState(

EFrameworkunifiedStatus CSystemManager::GetErrLogCount(ROM_AccessIf & f_cRomAccess, UI_32& f_errLogCount) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (m_UsingVMPlayer) {  // LCOV_EXCL_BR_LINE 6:Because the applicable variable cannot be changed from the external API (only evaluated by the initial value)
  // LCOV_EXCL_START 6: Because the applicable variable cannot be changed from the external API (only evaluated by the initial value)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error:m_UsingVMPlayer is True");
    l_eStatus = eFrameworkunifiedStatusInvldParam;
    // LCOV_EXCL_STOP
  } else {
    // LCOV_EXCL_BR_START 6: Because RomAccessIf APIs cannot satisfy the fail condition
    if (eFrameworkunifiedStatusOK
        != (l_eStatus = f_cRomAccess.GetErrLogCount(&f_errLogCount))) {
    // LCOV_EXCL_BR_STOP
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      LOG_ERROR("f_cRomAccess.GetErrLogCount()");  // LCOV_EXCL_LINE 6:Because RomAccessIf APIs cannot satisfy the fail condition
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
// End of EFrameworkunifiedStatus CSystemManager::GetErrLogCount

EFrameworkunifiedStatus CSystemManager::GetNextWakeupType(ROM_AccessIf & f_cRomAccess, ESMNextWakeupType& f_wakeupType) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (m_UsingVMPlayer) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error:m_UsingVMPlayer is True");
    l_eStatus = eFrameworkunifiedStatusInvldParam;
  } else {
    ENEXT_WAKEUP_TYPE l_wakeupType;

    if (eFrameworkunifiedStatusOK != (l_eStatus = f_cRomAccess.GetNextWakeupType(&l_wakeupType))) {
      LOG_ERROR("f_cRomAccess.GetNextWakeupType()");  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    } else {
      switch (l_wakeupType) {
        case NEXT_WAKEUP_TYPE_NONE:
          f_wakeupType = e_SS_SM_NEXT_WAKEUP_TYPE_NONE;
          break;
        case NEXT_WAKEUP_TYPE_COLD:
          f_wakeupType = e_SS_SM_NEXT_WAKEUP_TYPE_COLD;
          break;
        case NEXT_WAKEUP_TYPE_HOT:
          f_wakeupType = e_SS_SM_NEXT_WAKEUP_TYPE_HOT;
          break;
        default:
          f_wakeupType = e_SS_SM_NEXT_WAKEUP_TYPE_NONE;
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: wakeupType = %d", l_wakeupType);
          break;
      }
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
// End of EFrameworkunifiedStatus CSystemManager::GetNextWakeupType(

EFrameworkunifiedStatus CSystemManager::SetActiveFlashloader(ROM_AccessIf & f_cRomAccess, EACTIVE_FLASHLOADER f_FlashloaderMode) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  if (m_UsingVMPlayer) {  // LCOV_EXCL_BR_LINE 6: SetSystemModeInfoStruct() has checked
    // LCOV_EXCL_START 6: SetSystemModeInfoStruct() has checked
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: 'm_UsingVMPlayer' is True");
    l_eStatus = eFrameworkunifiedStatusInvldParam;
    // LCOV_EXCL_STOP
  } else if (eFrameworkunifiedStatusOK != (l_eStatus = f_cRomAccess.SetActiveFlashloader(f_FlashloaderMode))) {
    LOG_ERROR("f_cRomAccess.SetActiveFlashloader()");  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " NVM ActiveFlashloader initialized to %s",
        f_FlashloaderMode == NEW_FLASHLOADER ? "NEW_FLASHLOADER" : "OLD_FLASHLOADER");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::SetActiveFlashloader(

EFrameworkunifiedStatus CSystemManager::SetBootMode(ROM_AccessIf & f_cRomAccess, ESMBootModeInfo f_BootMode) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  if (m_UsingVMPlayer) {  // LCOV_EXCL_BR_LINE 6: SetSystemModeInfoStruct() has checked
    // LCOV_EXCL_START 6: SetSystemModeInfoStruct() has checked
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: 'm_UsingVMPlayer' is True");
    l_eStatus = eFrameworkunifiedStatusInvldParam;
    // LCOV_EXCL_STOP
  } else {
    EBOOT_MODE l_RomBootMode;
    switch (f_BootMode) {  // LCOV_EXCL_BR_LINE 200:only e_SS_SM_BOOT_MODE_APPLICATION is called
      case e_SS_SM_BOOT_MODE_APPLICATION:
        l_RomBootMode = APPLICATION_MODE;
        break;
      case e_SS_SM_BOOT_MODE_PROGRAMMING:
        // LCOV_EXCL_START 200:only e_SS_SM_BOOT_MODE_APPLICATION is called
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        l_RomBootMode = PROGRAMMING_MODE;
        break;
        // LCOV_EXCL_STOP
      default:
        // LCOV_EXCL_START 200:only e_SS_SM_BOOT_MODE_APPLICATION is called
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        // Shouldn't ever get here, but hey, in case we do...
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
            " Error: Unknown ESMBootModeInfo f_BootMode: 0x%X", f_BootMode);
        l_eStatus = eFrameworkunifiedStatusInvldParam;
        // LCOV_EXCL_STOP
    }

    if (eFrameworkunifiedStatusOK == l_eStatus) {  // LCOV_EXCL_BR_LINE 200: l_eStatus is always eFrameworkunifiedStatusOK
      if (eFrameworkunifiedStatusOK != (l_eStatus = f_cRomAccess.SetBootMode(l_RomBootMode))) {
        LOG_ERROR("f_cRomAccess.SetBootMode()");  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " NVM BootMode initialized to %s",
            l_RomBootMode == APPLICATION_MODE ? "APPLICATION_MODE" : "PROGRAMMING_MODE");
      }
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::SetBootMode(

EFrameworkunifiedStatus CSystemManager::SetLastUserMode(ROM_AccessIf & f_cRomAccess, EPWR_USER_MODE_TYPE f_lastUserMode) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  EUSER_MODE l_RomMode = f_lastUserMode == epsumOFF ? USER_OFF : USER_ON;
  if (m_UsingVMPlayer) {  // LCOV_EXCL_BR_LINE 6: SetSystemModeInfoStruct() has checked
    // LCOV_EXCL_START 6: SetSystemModeInfoStruct() has checked
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: 'm_UsingVMPlayer' is True");
    l_eStatus = eFrameworkunifiedStatusInvldParam;
    // LCOV_EXCL_STOP
  } else if (eFrameworkunifiedStatusOK != (l_eStatus = f_cRomAccess.SetLastUserMode(l_RomMode))) {
    LOG_ERROR("f_cRomAccess.SetLastUserMode()");
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " NVM LastUserMode initialized to %s",
        f_lastUserMode == epsumOFF ? "USER_OFF" : "USER_ON");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus SetLastUserMode(ROM_AccessIf & f_cRomAccess

EFrameworkunifiedStatus CSystemManager::SetLimpHomeCutoffMode(ROM_AccessIf & f_cRomAccess, EPWR_LHC_TYPE f_controlMode) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  ECONTROL_MODE l_RomControlMode;
  if (m_UsingVMPlayer) {  // LCOV_EXCL_BR_LINE 6: SetSystemModeInfoStruct() has checked
    // LCOV_EXCL_START 6: SetSystemModeInfoStruct() has checked
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: 'm_UsingVMPlayer' is True");
    l_eStatus = eFrameworkunifiedStatusInvldParam;
    // LCOV_EXCL_STOP
  } else {
    l_RomControlMode = f_controlMode == epslhcDISABLED ? DISABLE_MODE : ENABLE_MODE;
    if (eFrameworkunifiedStatusOK
        != (l_eStatus = f_cRomAccess.SetLimpHomeCutOffReqMode(l_RomControlMode))) {
      LOG_ERROR("f_cRomAccess.SetLimpHomeCutOffReqMode()");  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " NVM LimpHomeCutOff initialized to %s",
          f_controlMode == epslhcDISABLED ? "DISABLE_MODE" : "ENABLE_MODE");
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of CSystemManager::SetLimpHomeCutoffMode(ROM_AccessIf & f_cRomAccess

EFrameworkunifiedStatus CSystemManager::SetProductionMode(ROM_AccessIf & f_cRomAccess, EPWR_PROD_MODE_TYPE f_productionMode) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  ECONTROL_MODE l_RomControlMode;
  if (m_UsingVMPlayer) {  // LCOV_EXCL_BR_LINE 6: SetSystemModeInfoStruct() has checked
    // LCOV_EXCL_START 6: SetSystemModeInfoStruct() has checked
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: 'm_UsingVMPlayer' is True");
    l_eStatus = eFrameworkunifiedStatusInvldParam;
    // LCOV_EXCL_STOP
  } else {
    l_RomControlMode = f_productionMode == epspmDISABLED ? DISABLE_MODE : ENABLE_MODE;
    if (eFrameworkunifiedStatusOK
        != (l_eStatus = f_cRomAccess.SetProductionMode(l_RomControlMode))) {
      LOG_ERROR("f_cRomAccess.SetProductionMode()");  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " NVM ProductionMode initialized to %s",
           f_productionMode == epspmDISABLED ? "DISABLE_MODE" : "ENABLE_MODE");
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of CSystemManager::SetLimpHomeCutoff(ROM_AccessIf & f_cRomAccess

EFrameworkunifiedStatus CSystemManager::SetSignature(ROM_AccessIf & f_cRomAccess, UI_32 f_Data) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (m_UsingVMPlayer) {  // LCOV_EXCL_BR_LINE 6: For setting the initialization conditions at SystemManager startup
    // LCOV_EXCL_START 6: For setting the initialization conditions at SystemManager startup
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           " Error: 'm_UsingVMPlayer' is True");
    l_eStatus = eFrameworkunifiedStatusInvldParam;
    // LCOV_EXCL_STOP
  } else {
    // LCOV_EXCL_BR_START 6: Because RomAccessIf APIs cannot satisfy the fail condition
    if (eFrameworkunifiedStatusOK !=
        (l_eStatus = f_cRomAccess.SetSignature(f_Data))) {
    // LCOV_EXCL_BR_STOP
      LOG_ERROR("f_cRomAccess.SetSignature()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    }
  }

  if (eFrameworkunifiedStatusOK == l_eStatus) {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " NVM Signature is set to %llx",
    static_cast<long long unsigned int>(f_Data));  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of

EFrameworkunifiedStatus CSystemManager::SetTransportMode(ROM_AccessIf & f_cRomAccess, EPWR_TRANSPORT_MODE_TYPE f_transportMode) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  ECONTROL_MODE l_RomControlMode;
  if (m_UsingVMPlayer) {  // LCOV_EXCL_BR_LINE 6: SetSystemModeInfoStruct() has checked
    // LCOV_EXCL_START 6: SetSystemModeInfoStruct() has checked
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: 'm_UsingVMPlayer' is True");
    l_eStatus = eFrameworkunifiedStatusInvldParam;
    // LCOV_EXCL_STOP
  } else {
    l_RomControlMode = f_transportMode == epstmDISABLED ? DISABLE_MODE : ENABLE_MODE;
    if (eFrameworkunifiedStatusOK
        != (l_eStatus = f_cRomAccess.SetTransportMode(l_RomControlMode))) {
      LOG_ERROR("f_cRomAccess.SetTransportMode()");  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " NVM TransportMode initialized to %s",
          f_transportMode == epstmDISABLED ? "DISABLE_MODE" : "ENABLE_MODE");
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of CSystemManager::SetLimpHomeCutoff(ROM_AccessIf & f_cRomAccess

EFrameworkunifiedStatus CSystemManager::SetDataResetMode(ROM_AccessIf & f_cRomAccess, ESMDataResetModeInfo f_dataResetMode) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  EDATARESET_MODE l_DataResetMode;

  if (m_UsingVMPlayer) {  // LCOV_EXCL_BR_LINE 6: Because the applicable variable cannot be changed from the external API (only evaluated by the initial value)
    // LCOV_EXCL_START 6: Because the applicable variable cannot be changed from the external API (only evaluated by the initial value)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           " Error: 'm_UsingVMPlayer' is True");
    l_eStatus = eFrameworkunifiedStatusInvldParam;
    // LCOV_EXCL_STOP
  } else {
    switch (f_dataResetMode) {
      case e_SS_SM_DATA_RESET_MODE_NONE:
        l_DataResetMode = DATARESET_NONE;
        break;
      case e_SS_SM_DATA_RESET_MODE_USER:
        l_DataResetMode = DATARESET_USER;
        break;
      case e_SS_SM_DATA_RESET_MODE_FACTORY:
        l_DataResetMode = DATARESET_FACTORY;
        break;
      default:
        l_DataResetMode = DATARESET_NONE;
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: reset_mode = %d", f_dataResetMode);
        break;
    }

    if (eFrameworkunifiedStatusOK != (l_eStatus = f_cRomAccess.SetDataResetMode(l_DataResetMode))) {
      LOG_ERROR("f_cRomAccess.SetDataResetMode()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " NVM DataResetMode set to %s",
          f_dataResetMode == e_SS_SM_DATA_RESET_MODE_USER ? "DATARESET_USER" :
          f_dataResetMode == e_SS_SM_DATA_RESET_MODE_FACTORY ? "DATARESET_FACTORY" : "DATARESET_NONE");
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of CSystemManager::SetDataResetMode(ROM_AccessIf & f_cRomAccess

EFrameworkunifiedStatus CSystemManager::SetResetCount(ROM_AccessIf & f_cRomAccess, UI_32 f_resetCount) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  if (m_UsingVMPlayer) {  // LCOV_EXCL_BR_LINE 6: Because the applicable variable cannot be changed from the external API (only evaluated by the initial value)
    // LCOV_EXCL_START 6: Because the applicable variable cannot be changed from the external API (only evaluated by the initial value)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: 'm_UsingVMPlayer' is True");
    l_eStatus = eFrameworkunifiedStatusInvldParam;
    // LCOV_EXCL_STOP
  } else {
    if (eFrameworkunifiedStatusOK != (l_eStatus = f_cRomAccess.SetResetCount(f_resetCount))) {  // LCOV_EXCL_BR_LINE 6: Because the applicable variable cannot be changed from the external API (only evaluated by the initial value) // NOLINT(whitespace/line_length)
      LOG_ERROR("f_cRomAccess.SetResetCount()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    } else {
     FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
             " NVM ResetCount set to %d", f_resetCount);
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of CSystemManager::SetResetCount(ROM_AccessIf & f_cRomAccess

EFrameworkunifiedStatus CSystemManager::SetLastIlgReset(ROM_AccessIf & f_cRomAccess, BOOL f_lastIlgReset) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (m_UsingVMPlayer) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: 'm_UsingVMPlayer' is True");
    l_eStatus = eFrameworkunifiedStatusInvldParam;
  } else {
    // LCOV_EXCL_BR_START 6: For setting the initialization conditions at SystemManager startup
    ELASTILGRESET_MODE l_lastIlgReset =
      f_lastIlgReset ? LAST_ILGRESET_NG : LAST_ILGRESET_NORMAL;
    // LCOV_EXCL_BR_STOP

    if (eFrameworkunifiedStatusOK
        != (l_eStatus =
            f_cRomAccess.SetLastIlgReset(l_lastIlgReset))) {
      LOG_ERROR("f_cRomAccess.SetLastIlgReset()");
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " NVM LastIlgReset is set to %s",
          f_lastIlgReset ? "LAST_ILGRESET_NG" : "LAST_ILGRESET_NORMAL");
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of CSystemManager::SetLastIlgReset(ROM_AccessIf & f_cRomAccess

EFrameworkunifiedStatus CSystemManager::SetProgUpdateState(ROM_AccessIf & f_cRomAccess, SMProgUpdateState f_progUpdateState) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (m_UsingVMPlayer) {  // LCOV_EXCL_BR_LINE 6: Because the applicable variable cannot be changed from the external API (only evaluated by the initial value)
    // LCOV_EXCL_START 6: Because the applicable variable cannot be changed from the external API (only evaluated by the initial value)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "Error:m_UsingVMPlayer is True");
    l_eStatus = eFrameworkunifiedStatusInvldParam;
    // LCOV_EXCL_STOP
  } else {
    if (eFrameworkunifiedStatusOK
        != (l_eStatus = f_cRomAccess.SetProgUpdateState(f_progUpdateState))) {
      LOG_ERROR("f_cRomAccess.SetProgUpdateState()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "NVM ProgUpdateState : %#x", f_progUpdateState);
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of CSystemManager::SetProgUpdateState(ROM_AccessIf & f_cRomAccess

EFrameworkunifiedStatus CSystemManager::SetErrLogCount(ROM_AccessIf & f_cRomAccess, UI_32 f_errLogCount) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (m_UsingVMPlayer) {  // LCOV_EXCL_BR_LINE 6: Because the applicable variable cannot be changed from the external API (only evaluated by the initial value)
    // LCOV_EXCL_START 6: Because the applicable variable cannot be changed from the external API (only evaluated by the initial value)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "Error:m_UsingVMPlayer is True");
    l_eStatus = eFrameworkunifiedStatusInvldParam;
    // LCOV_EXCL_STOP
  } else {
    if (eFrameworkunifiedStatusOK != (l_eStatus =
        f_cRomAccess.SetErrLogCount(f_errLogCount))) {
      LOG_ERROR("f_cRomAccess.SetErrLogCount()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
             "NVM ErrLogCount : %#x", f_errLogCount);
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of CSystemManager::SetErrLogCount(ROM_AccessIf & f_cRomAccess

EFrameworkunifiedStatus CSystemManager::SetNextWakeupType(ROM_AccessIf & f_cRomAccess, ESMNextWakeupType f_wakeupType) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (m_UsingVMPlayer) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error:m_UsingVMPlayer is True");
    l_eStatus = eFrameworkunifiedStatusInvldParam;
  } else {
    ENEXT_WAKEUP_TYPE l_wakeupType;

    switch (f_wakeupType) {
      case e_SS_SM_NEXT_WAKEUP_TYPE_NONE:
        l_wakeupType = NEXT_WAKEUP_TYPE_NONE;
        break;
      case e_SS_SM_NEXT_WAKEUP_TYPE_COLD:
        l_wakeupType = NEXT_WAKEUP_TYPE_COLD;
        break;
      case e_SS_SM_NEXT_WAKEUP_TYPE_HOT:
        l_wakeupType = NEXT_WAKEUP_TYPE_HOT;
        break;
      default:
        l_wakeupType = NEXT_WAKEUP_TYPE_NONE;
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: wakeupType = %d", f_wakeupType);
        break;
    }

    if (eFrameworkunifiedStatusOK
        != (l_eStatus = f_cRomAccess.SetNextWakeupType(l_wakeupType))) {
      LOG_ERROR("f_cRomAccess.SetNextWakeupType()");  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "NVM NextWakeupType : %#x", f_wakeupType);
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of CSystemManager::SetNextWakeupType(ROM_AccessIf & f_cRomAccess

EFrameworkunifiedStatus CSystemManager::SetProductPrivate(ROM_AccessIf & f_cRomAccess, UI_8 buf[ROM_PRODUCT_PRIVATE_MAX]) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (m_UsingVMPlayer) {  // LCOV_EXCL_BR_LINE 6: SetSystemModeInfoStruct() has checked
    // LCOV_EXCL_START 6: SetSystemModeInfoStruct() has checked
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: 'm_UsingVMPlayer' is True");
    l_eStatus = eFrameworkunifiedStatusInvldParam;
    // LCOV_EXCL_STOP
  } else {
    if (eFrameworkunifiedStatusOK != (l_eStatus = f_cRomAccess.SetProductPrivate(buf))) {
      LOG_ERROR("f_cRomAccess.SetProductPrivate()");  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " NVM ProductPrivate is set");
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of CSystemManager::SetProductPrivate(ROM_AccessIf & f_cRomAccess

LBM_UINT32 CSystemManager::GetDramStatusInfo(const HANDLE h_app) {
  LBM_UINT32 dramStatus = SYSUP_DRAM_NORMAL;
//  PsmSysupDramT l_dram1 = PSM_SYSUP_DRAM_NORMAL;
//  PsmSysupDramT l_dram2 = PSM_SYSUP_DRAM_NORMAL;
//  PsmSysupDramT l_dram3 = PSM_SYSUP_DRAM_NORMAL;
  int ret;

//  ret = PsmGetDramBackupStatusPower(h_app, &l_dram1, &l_dram2, &l_dram3);
//  if ((ret == 0)
//      && ((l_dram1 >= PSM_SYSUP_DRAM_NORMAL) && (l_dram1 < PSM_SYSUP_DRAM_MAX))
//      && ((l_dram2 >= PSM_SYSUP_DRAM_NORMAL) && (l_dram2 < PSM_SYSUP_DRAM_MAX))
//      && ((l_dram3 >= PSM_SYSUP_DRAM_NORMAL) && (l_dram3 < PSM_SYSUP_DRAM_MAX))) {
//    m_BootMemory->dram1 = m_PsmSysupDram_to_SysupDramMap[static_cast<UI_32>(l_dram1)];
//    m_BootMemory->dram2 = m_PsmSysupDram_to_SysupDramMap[static_cast<UI_32>(l_dram2)];
//    m_BootMemory->dram3 = m_PsmSysupDram_to_SysupDramMap[static_cast<UI_32>(l_dram3)];
//    if ((m_BootMemory->dram1 == m_BootMemory->dram2)
//        || (m_BootMemory->dram1 == m_BootMemory->dram3)) {
//      dramStatus = m_BootMemory->dram1;
//    } else if (m_BootMemory->dram2 == m_BootMemory->dram3) {
//      dramStatus = m_BootMemory->dram2;
//      SS_ASERT(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
//    } else {
//      dramStatus = SYSUP_DRAM_NORMAL;
//      SS_ASERT(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
//    }
//  } else {
//    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "PsmGetDramBackupStatusPower()");
//  }

  return dramStatus;
}

void CSystemManager::SetDramStatusInfo(LBM_UINT32 f_state) {
  m_BootMemory->dram1 = m_BootMemory->dram2 = m_BootMemory->dram3 = f_state;
  return;
}

EFrameworkunifiedStatus CSystemManager::OnSetStartupConfirmationDataRequest(HANDLE hApp) noexcept {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  INTERFACEUNIFIEDLOG_RECEIVED_FROM(hApp);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  FRAMEWORKUNIFIEDLOG(ZONE_PERFORMANCE, __FUNCTION__, "from %s", FrameworkunifiedGetMsgSrc(hApp));
  EFrameworkunifiedStatus l_eStatus;
  Pwr_ServiceSetInterface tServiceSetIf;
  // ReadMsg():                                                        *
  //     Check hApp ptr, msg size, msg reception, read msg if all ok.  *
  //     Report any errors found.                                      *
  //                                                                   *
  if (eFrameworkunifiedStatusOK
      != (l_eStatus = ReadMsg < Pwr_ServiceSetInterface > (hApp, tServiceSetIf))) {  // LCOV_EXCL_BR_LINE 4:NSFW's error
    // LCOV_EXCL_START 4:NSFW's error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("ReadMsg()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    // LCOV_EXCL_STOP
  } else {
    m_startUpConfirmationMsg = tServiceSetIf.data.startupConfirmationMsg;
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " %s Startup Confirmation data:", FrameworkunifiedGetMsgSrc(hApp));
    SYSTEMMANAGERLOG_StartupConfirmationMsg(m_startUpConfirmationMsg);  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
//    DGCODE_RET_API  l_ret = DGCODE_RET_ERROR;

//    l_ret = Diag_StartupAllStatusOfDTC();
//    if (l_ret == DGCODE_RET_ERROR) {
//      LOG_ERROR("Diag_StartupAllStatusOfDTC()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
//    }

    if ((e_SS_SM_NEXT_WAKEUP_TYPE_COLD == m_NextWakeupType) && (epsstCOLDSTART != m_startUpConfirmationMsg.wakeupType)) {  //  LCOV_EXCL_BR_LINE 6: Because the above formula cannot be set to true // NOLINT(whitespace/line_length)
      // LCOV_EXCL_START 6: Because the condition cannot be set
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "WakeupType Change to Cold by Svc Request");
      m_startUpConfirmationMsg.wakeupType = epsstCOLDSTART;
      // LCOV_EXCL_STOP
    }

    RAM_WAKEUP_STATE l_ramWakeupState =
      (SDRAM_STS_BUPNG == m_BootMemory->pwSts) ? RAM_WAKEUP_STATE_BACKUP_NG : RAM_WAKEUP_STATE_DONT_CARE; // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    if (epsstWARMSTART != m_startUpConfirmationMsg.wakeupType) {
      l_ramWakeupState = RAM_WAKEUP_STATE_BATTERY_DOWN;
    }
    RAM_SM_INFO_t info;
    {
      RAM_AccessIf ram_access(l_ramWakeupState);

      if (eFrameworkunifiedStatusOK != (l_eStatus = ram_access.getRamInfo(&info))) {  //  LCOV_EXCL_BR_LINE 200:  As they are not returned except to eFrameworkunifiedStatusOK // NOLINT(whitespace/line_length)
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               " Error: ram_access.getRamInfo(%d)", l_eStatus);
      }

      if (epsstWARMSTART != m_startUpConfirmationMsg.wakeupType) {
        // To standardize the behavior when DRAM is backed up/not backed up, standardize the FALSE.
        m_isImmediateReset = FALSE;
        info.isImmediateReset = TRUE;
        if (eFrameworkunifiedStatusOK != (l_eStatus = ram_access.setRamInfo(&info))) {  //  LCOV_EXCL_BR_LINE 200: As they are not returned except to eFrameworkunifiedStatusOK // NOLINT(whitespace/line_length)
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                 " Error: ram_access.setRamInfo(%d)", l_eStatus);
        }
      }
    }

    m_needReNotifyStartPrm = info.needRenotifyStartPrm;
    m_ResetStatus = (((m_isIlgReset == TRUE) || (info.isErrorReset == TRUE)) ?
                     e_SS_SM_RESET_STATUS_NG :
                    ((m_isImmediateReset == TRUE) ?
                     e_SS_SM_RESET_STATUS_IMMEDIATE : e_SS_SM_RESET_STATUS_NONE));
    if (e_SS_SM_RESET_STATUS_NG == m_ResetStatus) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __PRETTY_FUNCTION__,
          "ResetStatus NG   illRst:%s lastErr:%s",
          m_isIlgReset ? "TRUE" : "FALSE",
          info.isErrorReset ? "ERROR" : "NOT ERROR");
    }

    // Count the number of abnormal REBOOTs based on the ResetStatus judged from the startup information (not counted when NG is retransmitted).
    if (e_SS_SM_RESET_STATUS_NG == m_ResetStatus) {
      ROM_AccessIf rom_access;
#ifndef SS_SM_SKIP_PUT_ROB_SYSTEM_RESET
      const uint64_t l_SystemResetRobCode = 0x2219;
//      DGCODE_ROB_SSR_INFO  l_info;

//      memset(&l_info, 0x00, sizeof(DGCODE_ROB_SSR_INFO));

//      l_ret = Diag_PutRoBInfo(hApp, l_SystemResetRobCode, l_info);
//      if (l_ret == DGCODE_RET_ERROR) {
//        LOG_ERROR("Diag_PutDiagCodeWithSSR_Custom()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
//      }
#endif

      m_ResetCount++;
      rom_access.Initialize();
      // Count the number of abnormal REBOOTs based on the ResetStatus judged from the startup information (not counted when NG is retransmitted).
      if (eFrameworkunifiedStatusOK != (l_eStatus =
          SetResetCount(rom_access, m_ResetCount))) {
      // LCOV_EXCL_BR_STOP
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: SetResetCount(%d)", l_eStatus);
      }
    }

    // LCOV_EXCL_BR_START 11:unexpected branch  // NOLINT(whitespace/line_length)
    m_DramBackupStatus =
      ((epsstWARMSTART != m_startUpConfirmationMsg.wakeupType)
            || (e_SS_SM_DATA_RESET_MODE_FACTORY == m_DataResetMode)
            || (SDRAM_STS_BUPNG == m_BootMemory->pwSts)
            || (GetDramStatusInfo(hApp) == SYSUP_DRAM_DISCARD)) ?
            e_SS_SM_DRAM_BACKUP_NG : e_SS_SM_DRAM_BACKUP_OK;
    // LCOV_EXCL_BR_STOP

    if (e_SS_SM_DRAM_BACKUP_NG == m_DramBackupStatus) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __PRETTY_FUNCTION__,
          "DramBackup NG  wakeupType:%s dataResetMode:%s pwSts:%s",
          (epsstWARMSTART == m_startUpConfirmationMsg.wakeupType) ? "WARM" : "COLD",
          (e_SS_SM_DATA_RESET_MODE_FACTORY == m_DataResetMode) ? "FACTORY" : "NotFactory",
          (SDRAM_STS_NOMAL == m_BootMemory->pwSts) ? "OK" : "NG");
    }

    if (m_needReNotifyStartPrm) {  //  LCOV_EXCL_BR_LINE 6: Because the condition cannot be satisfied from the external API
      // LCOV_EXCL_BR_START 6: Because both sides are not FALSE
      if ((e_SS_SM_DRAM_BACKUP_NG == m_DramBackupStatus)
          || (e_SS_SM_DRAM_BACKUP_NG == info.lastDramBackupStatus)) {
      // LCOV_EXCL_BR_STOP
        m_DramBackupStatus = e_SS_SM_DRAM_BACKUP_NG;
      }
    }

    // To ensure notification to SVCs when DRAM backups are destroyed,
    // Saving DramBackupStatus to RamAccessIf Prior to dramStatus Initialization
    info.lastDramBackupStatus = m_DramBackupStatus;
    {
      RAM_AccessIf ram_access(l_ramWakeupState);
      if (eFrameworkunifiedStatusOK != (l_eStatus = ram_access.setRamInfo(&info))) {  //  LCOV_EXCL_BR_LINE 200: As they are not returned except to eFrameworkunifiedStatusOK // NOLINT(whitespace/line_length)
      // NOLINT(whitespace/line_length)
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               " Error: ram_access.setRamInfo(%d)", l_eStatus);
      }
    }

    SetDramStatusInfo(SYSUP_DRAM_NORMAL);

    if (m_needReNotifyStartPrm) {  //  LCOV_EXCL_BR_LINE 6: Because the condition cannot be satisfied from the external API
      if ((e_SS_SM_RESET_STATUS_NG == info.lastResetStatus)
          || (e_SS_SM_RESET_STATUS_NG == m_ResetStatus)) {
        m_ResetStatus = e_SS_SM_RESET_STATUS_NG;
      } else if ((e_SS_SM_RESET_STATUS_IMMEDIATE == info.lastResetStatus)
          || (e_SS_SM_RESET_STATUS_IMMEDIATE == m_ResetStatus)) {
        m_ResetStatus = e_SS_SM_RESET_STATUS_IMMEDIATE;
      } else {
        m_ResetStatus = e_SS_SM_RESET_STATUS_NONE;
      }

      if ((epsstWARMSTART != info.lastWakeupType) || (epsstWARMSTART != m_startUpConfirmationMsg.wakeupType)) {  //  LCOV_EXCL_BR_LINE 6: Because the above condition cannot be satisfied // NOLINT(whitespace/line_length)
        m_startUpConfirmationMsg.wakeupType = epsstCOLDSTART;
      }
    }

    info.lastResetStatus = m_ResetStatus;
    info.lastWakeupType = m_startUpConfirmationMsg.wakeupType;
    info.needRenotifyStartPrm = TRUE;
    info.isErrorReset = FALSE;
    {
      RAM_AccessIf ram_access(l_ramWakeupState);
      if (eFrameworkunifiedStatusOK != (l_eStatus = ram_access.setRamInfo(&info))) {  //  LCOV_EXCL_BR_LINE 200:As there are no cases other than eFrameworkunifiedStatusOK // NOLINT(whitespace/line_length)
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               " Error: ram_access.setRamInfo(%d)", l_eStatus);
      }
    }

    CALL_AND_LOG_STATUS(ClearDramBackupInfo(hApp));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

    FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "\n"
      "start prm\n"
      "dramBackupStatus = %s\n"
      "wakeupType       = %s\n"
      "dataResetMode    = %s\n"
      "resetStatus      = %s\n"
      "resetCount       = %d\n"
      "progUpdateState  = %d",
      (m_DramBackupStatus == e_SS_SM_DRAM_BACKUP_OK) ? "OK" : "NG",
      (m_startUpConfirmationMsg.wakeupType == epsstWARMSTART) ? "WARM" : "COLD",
      (m_DataResetMode == e_SS_SM_DATA_RESET_MODE_FACTORY) ? "FACT" :
      (m_DataResetMode == e_SS_SM_DATA_RESET_MODE_USER) ? "USER" :
      (m_DataResetMode == e_SS_SM_DATA_RESET_MODE_PROGUPDATE) ? "PROG" : "NONE",
      (m_ResetStatus == e_SS_SM_RESET_STATUS_IMMEDIATE) ? "IMM" :
      (m_ResetStatus == e_SS_SM_RESET_STATUS_NONE) ? "NONE" : "NG", m_ResetCount,
      m_ProgUpdateState);

    if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedNPPublishNotification(hApp, NTFY_SSSystemMgrStartUpType, &m_startUpConfirmationMsg.wakeupType, sizeof(EPWR_SC_WAKEUP_TYPE)))) {  // LCOV_EXCL_BR_LINE 4:NSFW error case  //NOLINT (whitespace/line_length)
      // LCOV_EXCL_START 4: nsfw error
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
          " Error: FrameworkunifiedNPPublishNotification(%s, %s) errored: %d/'%s'",
          NTFY_SSSystemMgrStartUpType,
          GetStr(m_startUpConfirmationMsg.wakeupType).c_str(), l_eStatus,
          GetStr(l_eStatus).c_str());
      // LCOV_EXCL_STOP
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " FrameworkunifiedNPPublishNotification(%s, %s) successful",
          NTFY_SSSystemMgrStartUpType,
          GetStr(m_startUpConfirmationMsg.wakeupType).c_str());

      // LCOV_EXCL_BR_START 200: As there are no cases other than eFrameworkunifiedStatusOK
      REC_HIST_IF_SUCCESSFUL("SM/StartUpType",
                             m_PubCmdHist, m_PubHistIter, "", l_eStatus);
      // LCOV_EXCL_BR_STOP
    }

    if (eFrameworkunifiedStatusOK != (l_eStatus = PublishBootModeNotification(hApp))) {  // LCOV_EXCL_BR_LINE 4:NSFW error case  //NOLINT (whitespace/line_length)
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      LOG_ERROR("PublishBootModeNotification()");  // LCOV_EXCL_LINE 4: nsfw error
    }
  }

  EFrameworkunifiedStatus l_responseStatus = l_eStatus;

  ModuleLaunchListIter l_ModuleListIter;
  PCSTR l_moduleName = FrameworkunifiedGetMsgSrc(hApp);
  if (eFrameworkunifiedStatusOK != (l_eStatus = GetModuleIterator(l_moduleName, l_ModuleListIter))) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        " Error: Module %s not found in Group Launch Map", l_moduleName);
  } else if (l_ModuleListIter->hsession != NULL) {
    if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedSendMsg(l_ModuleListIter->hsession,  // LCOV_EXCL_BR_LINE 4:NSFW's error
            SS_SM_FWD_START_CONFIRMATION_MSG_RESP, sizeof(EFrameworkunifiedStatus),
            (PVOID) & l_responseStatus))) {
      // LCOV_EXCL_START 4:NSFW's error
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      LOG_ERROR("FrameworkunifiedSendMsg(SS_SM_FWD_START_CONFIRMATION_MSG_RESP)");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
      // LCOV_EXCL_STOP
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " NULL == hsession; %s not connected yet", l_ModuleListIter->name.c_str());
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSystemManager::OnSetStartupConfirmationDataRequest( HANDLE hApp )

EFrameworkunifiedStatus CSystemManager::GetTimeStamp(std::string& TimeStamp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  CHAR format[256] = { '\0' };
  CHAR l_format[256] = { '\0' };
  struct timeb timebuf;
  uint32_t time;
  uint8_t status;
  struct tm theTime;
  ftime(&timebuf);
//  Clock_getSystemTimeY2K38(&time, &status);
//  CLOCK_RETURN clock_ret = Clock_CnvSecToDateY2K38(&time, &theTime);

//  if (CLOCK_OK != clock_ret) {
//    return eFrameworkunifiedStatusNullPointer;
//  }

  sprintf(l_format, "GMT Time,%04d-%02d-%02d %02d:%02d:%02d.%hu\n",  // NOLINT
  theTime.tm_year + 1900, theTime.tm_mon + 1, theTime.tm_mday,
  theTime.tm_hour, theTime.tm_min, theTime.tm_sec, timebuf.millitm);

  struct timespec timeSpec;

  if (0 != clock_gettime(CLOCK_MONOTONIC, &timeSpec)) {
    SS_ASERT_ERRNO(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  } else {
    time_t timeInSecs = timeSpec.tv_sec;
    sprintf(format, "Elaps Time,%ld:%02ld:%02ld\n", (timeInSecs / 3600),  // NOLINT
            ((timeInSecs % 3600) / 60), (timeInSecs % 60));
  }

  TimeStamp.clear();
  TimeStamp.assign(format);
  TimeStamp.append(l_format);

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

VOID CSystemManager::SYSTEMMANAGERLOG_StartupConfirmationMsg(EPWR_SC_MSG_STRUCT & f_scMsg) {
  std::stringstream l_enumStr;
  l_enumStr << endl;
  l_enumStr << " HostProcSoftVer:" << hex << std::setw(16) << std::setfill('0')
      << m_VersionNumberStruct.m_VersionNumber << endl;
  l_enumStr << "  _CWORD56_ Boot Mode:" << GetStr(f_scMsg._CWORD56_BootMode) << endl; // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  l_enumStr << "  wakeupType    :" << GetStr(f_scMsg.wakeupType); // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "%s", l_enumStr.str().c_str());
}  // End of VOID CSystemManager::SYSTEMMANAGERLOG_StartupConfirmationMsg( EPWR_SC_MSG_STRUCT & f_scMsg )

VOID CSystemManager::LogESystemmanagerStatusEnums() {
  #define INITIAL_INDEX eFrameworkunifiedStatusEmptyMediaList
  #define FINAL_INDEX   eFrameworkunifiedStatusErrNoEINTR

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  std::stringstream l_enumStr;
  for (int i = INITIAL_INDEX; i <= FINAL_INDEX; i++) {
    // LCOV_EXCL_BR_START 11:unexpected branch  // NOLINT(whitespace/line_length)
    l_enumStr << "\n" << "0x" << std::setfill('0') << std::setw(8) << hex << i
      << "/" << std::setfill(' ') << std::setw(2) << dec << i << ": "
      << GetStr(static_cast<EFrameworkunifiedStatus>(i));
    // LCOV_EXCL_BR_STOP
  }

  std::string l_logMsg = l_enumStr.str();
  SystemmanagerLogString(ZONE_INFO, __FUNCTION__, l_logMsg); // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");

#undef INITIAL_INDEX
#undef FINAL_INDEX
}  // End of VOID CSystemManager::LogESystemmanagerStatusEnums()

VOID CSystemManager::LogProtocolIDs() {
#define INITIAL_INDEX SS_SYSTEM_MANAGER_PROTOCOL_BEGINNING_INDEX
#define FINAL_INDEX   SS_SYSTEM_MANAGER_PROTOCOL_ENDING_INDEX

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  std::stringstream l_enumStr;
  for (int i = INITIAL_INDEX; i <= FINAL_INDEX; i++) {
    // LCOV_EXCL_BR_START 11:unexpected branch  // NOLINT(whitespace/line_length)
    l_enumStr << "\n" << "0x" << std::setfill('0') << std::setw(8) << hex << i
      << "/" << std::setfill(' ') << std::setw(2) << dec << i << ": "
      << GetStr(static_cast<SS_SystemManagerProtocol>(i));
    // LCOV_EXCL_BR_STOP
  }

  std::string l_logMsg = l_enumStr.str();
  SystemmanagerLogString(ZONE_INFO, __FUNCTION__, l_logMsg);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");

#undef INITIAL_INDEX
#undef FINAL_INDEX
}  // End of VOID CSystemManager::LogProtocolIDs()

VOID CSystemManager::SystemmanagerLogString(TFrameworkunifiedZone f_zone, PCSTR f_func, std::string &f_text) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  UI_32 l_offset = 0;
  UI_32 l_msgLenMax = MAX_QUEUE_MSG_SIZE - 512;  // Leave space for the appending of
                                                 // the NSLog date/time/service/line string.

  if (f_text.at(f_text.length() - 1) == '\n') {  // LCOV_EXCL_BR_LINE 6: For setting the initialization conditions at SystemManager startup
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    f_text.erase(f_text.length() - 1);  // LCOV_EXCL_LINE 6: For setting the initialization conditions at SystemManager startup
  }

  while (l_offset < f_text.size()) {
    UI_32 l_charsRem = static_cast<UI_32>(f_text.size() - l_offset);
    UI_32 l_msgLen = (l_charsRem > l_msgLenMax) ?  // LCOV_EXCL_BR_LINE 6: For setting the initialization conditions at SystemManager startup
        l_msgLenMax : l_charsRem;  // LCOV_EXCL_BR_LINE 6: For setting the initialization conditions at SystemManager startup
    std::string l_msgStr = f_text.substr(l_offset, l_msgLen);
    size_t l_newLinePos = l_msgStr.find_last_of("\n", l_msgLen);
    if ((l_newLinePos != std::string::npos) && (l_charsRem > l_msgLenMax)) {  // LCOV_EXCL_BR_LINE 6: For setting the initialization conditions at SystemManager startup // NOLINT(whitespace/line_length)
      // LCOV_EXCL_START 6: For setting the initialization conditions at SystemManager startup
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_msgStr = f_text.substr(l_offset, l_newLinePos);
      l_offset += static_cast<UI_32>(l_newLinePos);
      // LCOV_EXCL_STOP
    } else {
      l_offset += l_msgLen;
    }

    FRAMEWORKUNIFIEDLOG(f_zone, f_func, l_msgStr.c_str());
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}  // LCOV_EXCL_BR_LINE 10: Final line

// End of /SS_SystemManager/src/ss_system_manager.cpp
