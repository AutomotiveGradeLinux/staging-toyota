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

#ifndef __SS_SYSTEM_MANAGER_H__  // NOLINT
#define __SS_SYSTEM_MANAGER_H__

#include <system_service/ss_heartbeat_service_protocol.h>
#include <system_service/ss_system_manager_if.h>
#include <system_service/ss_error_event_common_local.h>
#include <system_service/ss_system_timer.h>
#include <crashdetector/ss_crashdetector.h>
#include <system_service/ss_system_manager_protocol.h>
#include <system_service/ss_power_service_protocol.h>
#include <system_service/ss_sm_rom_access.h>
#include <heartbeat/ss_hb_if.h>
#include <other_service/PosixBasedOS001ClockCycleApi.h>
#include <system_service/ss_string_maps.h>
#include <system_service/ss_templates.h>
#include <system_service/ss_boot_map.h>
#include <system_service/ss_system_manager_conf.h>
#include <system_service/ss_system_manager_protocol_local.h>
#include <system_service/ss_logger_store_logs.h>
#include <native_service/ns_logger_if.h>
#include <native_service/frameworkunified_types.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <inttypes.h>
#include <map>
#include <string>
#include <vector>
#include <queue>
#include <sstream>
#include <set>
#include <list>
#include <utility>

#include "ss_sm_config.h"
#include "ss_sm_systemmanagerlog.h"
#include "ss_system_thread_info.h"
#include "ss_system_memory_monitor.h"
#include "ss_sm_dynamic_launcher.h"

#define SM_SHUTDOWN_COMPLETE_MONITOR_TIMEOUT 60

typedef enum {
  eSM_TIMER_GROUP_MONITOR = 0x00
  , eSM_TIMER_PROCESS_LAUNCH_MONITOR
  , eSM_TIMER_HEARTBEAT_MONITOR
  , eSM_TIMER_PROCESSLAUNCHER_MONITOR
  , eSM_TIMER_CLIENT_START_MONITOR
  , eSM_TIMER_NPP_STATUS_CHECK_MONITOR
  , eSM_TIMER__CWORD56__HEARTBEAT_RESPONSE
  , eSM_TIMER_LAUNCH_GROUP_TRIGGER_PROC_RESP_TIMER
  , eSM_TIMER_GROUP_LAUNCH_WAIT_TIMER
  , eSM_TIMER_MODULE_CONNECT_WAIT_TIMER
  , eSM_TIMER_START_RESP_MONITOR_WAIT_TIMER
  , eSM_TIMER_SHUTDOWN_COMPLETE_MONITOR
  , eSM_TIMER_CLIENT_STOP_MONITOR
  , eSM_TIMERS_END
} ESMTimerIDs;

typedef enum {
  eSM_ERROR_EVENT_TIMER_LOGGER_START_REQ
  , eSM_ERROR_EVENT_TIMER_DEBUG_DUMP_RSPN
  , eSM_ERROR_EVENT_TIMER_BOOT_MICRO_LOG_RSPN
  , eSM_ERROR_EVENT_TIMER_CORE_FILE_POLL
  , eSM_ERROR_EVENT_TIMER_END
} ESMErrorEventTimers;

typedef enum {
  eErrorEventResetTypeNone = 0,
  eErrorEventResetTypeHard
} EErrorEventResetType;

typedef enum _ESMInternalProtocol {
  SS_SM_SendTriggerToSelf = SS_SM_PROTOCOL_INTERFACE_END  ///< System Manager to System Manager
} ESMInternalProtocol;

typedef enum _SS_SMGroupHirarchy {
  SS_SM_INVALID_GROUP = 0
  , SS_SM_INITIAL_GROUP
  , SS_SM_SECOND_GROUP
  , SS_SM_THIRD_GROUP
} SS_SMGroupHirarchy;

typedef enum _SS_SMModuleState {
  SS_SM_READY_TO_LAUNCH_APP = 0x00
  , SS_SM_APPS_LAUNCH_IN_PROGRESS
  , SS_SM_APPS_LAUNCHED_READY_TO_START
  , SS_SM_APPS_START_IN_PROGRESS
  , SS_SM_APPS_START_COMPLETE
  , SS_SM_APPS_STOPPING_AT__CWORD56__REQ
  , SS_SM_APPS_STOPPING_AT_INTERNAL_REQ
  , SS_SM_WAITING_FOR_CRITICAL_APPS_AT__CWORD56__REQ
  , SS_SM_WAITING_FOR_CRITICAL_APPS_AT_INTERNAL_REQ
  , SS_SM_APPS_PRE_START_IN_PROGRESS
  , SS_SM_APPS_PRE_STOP_IN_PROGRESS
  , SS_SM_APPS_PRE_RUN_COMPLETE
  , SS_SM_APPS_BACKGROUND_START_IN_PROGRESS
  , SS_SM_APPS_BACKGROUND_STOP_IN_PROGRESS
  , SS_SM_APPS_BACKGROUND_RUN_COMPLETE
} SS_SMModuleState;

typedef struct _SMProcessExitInfo {
  _SMProcessExitInfo() {
    this->pid                = 0;
    this->exitValue          = 0;
    this->signalNumber       = 0;
    this->binaryFileName[0]  = '\0';
  }

  UI_32  pid;
  SI_32  exitValue;
  SI_32  signalNumber;
  CHAR   binaryFileName[MAX_PATH_LENGTH];
} SMProcessExitInfo;

typedef struct _SMLoggingInfo {  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  _SMLoggingInfo() {
    this->pid               = 0;
    this->exitValue         = 0;
    this->signalNumber      = 0;
    this->binaryFileName[0] = '\0';
    this->messageStr        = "";
    this->suffixStr         = "";
    this->path              = "";
    this->resetReason       = e_SS_SM_CPU_RESET_REASON_INVALID;
  }

  UI_32              pid;
  SI_32              exitValue;
  SI_32              signalNumber;
  CHAR               binaryFileName[MAX_PATH_LENGTH];
  std::string        messageStr;
  std::string        suffixStr;
  std::string        path;
  ESMCpuResetReason  resetReason;
} SMLoggingInfo;

struct ERROR_EVENT_INFO {  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  ERROR_EVENT_INFO() :
    m_eventEnqueueTimeStamp(""),
    m_moduleQueueName(""),
    m_eventType(eErrorEventTypeMaxValue),
    m_prio(eErrorEventPrioDefault),
    m_uniqueCycles(ClockCycle()),  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    m_errorEventResetType(eErrorEventResetTypeNone) {  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  }

  std::string      m_eventEnqueueTimeStamp;
  std::string      m_moduleQueueName;
  EErrorEventType  m_eventType;
  SMLoggingInfo    m_loggingInfo;
  EErrorEventPrio  m_prio;
  UI_64            m_uniqueCycles;
  EErrorEventResetType m_errorEventResetType;
};

struct ERROR_EVENT_COMPARE {
  bool operator() (const ERROR_EVENT_INFO& lhs,
           const ERROR_EVENT_INFO& rhs) const {
    if ((eErrorEventPrioDefault == lhs.m_prio) &&
        (eErrorEventPrioDefault == rhs.m_prio)) {
      return (lhs.m_uniqueCycles < rhs.m_uniqueCycles);
    } else {
      return (lhs.m_prio > rhs.m_prio);
    }
  }
};

typedef  std::set<ERROR_EVENT_INFO, ERROR_EVENT_COMPARE> ERROR_EVENT_QUEUE;
typedef  ERROR_EVENT_QUEUE::iterator                     ERROR_EVENT_QUEUE_ITER;
typedef  std::pair<ERROR_EVENT_QUEUE_ITER, BOOL>         ERROR_EVENT_QUEUE_RET;

typedef EFrameworkunifiedStatus (*HndlrCb)(HANDLE hApp);
typedef EFrameworkunifiedStatus (*FncCbType2)(HANDLE hApp, UI_32 f_UI_32);
typedef BOOL       (*FncCbType3)(UI_32 f_UI_32);

typedef std::pair<SS_SystemManagerProtocol, Pwr_ServiceSetInterface> SM_POWER_EVENT_INFO;
typedef std::queue<SM_POWER_EVENT_INFO>                              SM_POWER_EVENT_QUEUE;

typedef struct {
  std::string m_cmd;
  UI_64       m_time;
  std::string m_sender;
} Ts_cmdHist;

typedef std::list<std::string> ErrorGroupList;
typedef ErrorGroupList::iterator ErrorGroupListIter;

typedef std::vector<Ts_cmdHist> cmdHist;
typedef cmdHist::iterator cmdHistIter;

typedef std::vector<std::string> HBList;
typedef HBList::iterator HBListIter;

class CVersionNumInfo {
 public:
  CVersionNumInfo() : m_VersionNumber(0), m_eSystemmanagerStatus(eFrameworkunifiedStatusErrOther) {}
  CVersionNumInfo(SS_VersionNumberType f_vn, EFrameworkunifiedStatus f_eStatus)
                : m_VersionNumber(f_vn), m_eSystemmanagerStatus(f_eStatus) {}

  // System Manager's local data store; write once on initialization,
  // read as needed at clients' request
  SS_VersionNumberType m_VersionNumber;
  EFrameworkunifiedStatus           m_eSystemmanagerStatus;    // Validity / status of version number
};

#define SS_SM_RELAUNCH_NO_LIMIT -1
typedef struct _PreLaunchModuleParams {
  _PreLaunchModuleParams() {
    this->LaunchFunc     = NULL;
    this->name           = "";
    this->binaryFileName = "";
    this->critical       = FALSE;
    this->relaunchLimit  =  0;
    this->pid            = -1;
    this->relaunchCount  =  0;
  }

  int          (*LaunchFunc)();  // Launching function
  std::string  name;             // Process name
  std::string  binaryFileName;   // Binary file name
  BOOL         critical;         // Reboot attribute
  int          relaunchLimit;    // Maximum number of Relaunch
  int          pid;              // pid
  int          relaunchCount;    // Count of Relaunching
} PreLaunchModuleParams;

typedef std::list<PreLaunchModuleParams> PreLaunchModuleList;
typedef PreLaunchModuleList::iterator PreLaunchModuleListIter;

typedef struct _GroupRelaunchModuleParams {
  _GroupRelaunchModuleParams() {
    this->name    = "";
    this->bIsKilled = FALSE;
  }

  std::string  name;      // Process name
  BOOL     bIsKilled;     // Flag indicates while forced quit process
} GroupRelaunchModuleParams;

typedef std::list<GroupRelaunchModuleParams> GroupRelaunchModuleList;
typedef GroupRelaunchModuleList::iterator GroupRelaunchModuleListIter;


class CSystemManager {
 public:
  static std::string m_bootOpt;   // BOOT OPTION

  CSystemManager();
  virtual ~CSystemManager();

  static CSystemManager & GetInstance();
  EFrameworkunifiedStatus        Initialize(HANDLE hApp);
  void       Initialize_memory(HANDLE hApp, bool* l_isInitFail, EFrameworkunifiedStatus* l_eStatus, BOOL isIllReset);
  void       Initialize_callbacks(HANDLE hApp, bool *l_isInitFail, EFrameworkunifiedStatus *l_eStatus);
  /// < Process Launcher Thread Callbacks
  EFrameworkunifiedStatus OnModuleLaunchResponse(HANDLE hApp);
  EFrameworkunifiedStatus OnTerminateModuleResponse(HANDLE hApp);
  EFrameworkunifiedStatus OnReLaunchModuleResponse(HANDLE hApp);
  EFrameworkunifiedStatus PerformModuleLaunchRespProcessing(HANDLE         hApp,
                         GroupLaunchMapIter   & f_GroupIter,  // NOLINT
                         ModuleLaunchListIter & f_ModuleIter,  // NOLINT
                         PCSTR        p_sPPD_tag);
  EFrameworkunifiedStatus OnModuleStatusResponse(HANDLE hApp);
  EFrameworkunifiedStatus OnGroupLaunchTimerExpiry(HANDLE hApp);
  EFrameworkunifiedStatus OnLaunchGroup(HANDLE hApp);
  EFrameworkunifiedStatus OnLaunchGroupSelfTrigger(HANDLE hApp);
  EFrameworkunifiedStatus OnProcessLaunchTimerExpiry(HANDLE hApp);
  EFrameworkunifiedStatus OnHeartBeatMonitorTimerExpiry(HANDLE hApp);
  EFrameworkunifiedStatus OnProcessLaunchMonitorTimerExpiry(HANDLE hApp);
  EFrameworkunifiedStatus OnHeartBeatThreadHeartbeatResponse(HANDLE hApp);
  EFrameworkunifiedStatus OnCheckAvailResponse(HANDLE f_hApp);

  EFrameworkunifiedStatus OnProcessLauncherThreadHeartbeatResponse(HANDLE hApp);
  EFrameworkunifiedStatus OnClientStartMonitorTimerExpiry(HANDLE hApp);
  EFrameworkunifiedStatus OnClientStopMonitorTimerExpiry(HANDLE hApp);
  EFrameworkunifiedStatus OnNPPStatusCheckMonitorTimerExpiry(HANDLE hApp);
  EFrameworkunifiedStatus OnPowerServiceAvailableNotify(HANDLE hApp);
  EFrameworkunifiedStatus OnLVI1Notify(HANDLE hApp);
  EFrameworkunifiedStatus OnLVI2Notify(HANDLE hApp);

  EFrameworkunifiedStatus OnModuleStartCompleteResponse(HANDLE hApp);  /// Process start complete from client
  EFrameworkunifiedStatus OnModuleStopCompleteResponse(HANDLE hApp) noexcept;    /// Process stop Complete from client
  EFrameworkunifiedStatus OnGetStartExtInfo(HANDLE hApp) noexcept;
  EFrameworkunifiedStatus OnGetStopExtInfo(HANDLE hApp) noexcept;
  EFrameworkunifiedStatus OnModuleStopCompleteNotification(HANDLE hApp);
  EFrameworkunifiedStatus ProcessModuleStopCompleteResponse(HANDLE hApp,
                         PCSTR  f_moduleName);
  EFrameworkunifiedStatus ModuleCompleteResponse(HANDLE         hApp,
                     ModuleLaunchListIter f_ModuleListIter,
                     SMModuleState    f_moduleState,
                     FncCbType3       f_isGroupDoneFnc,
                     FncCbType2       f_groupDoneFnc,
                     PCSTR        f_sCompleteTypeText);

  EFrameworkunifiedStatus ProcessGroupOnModuleStopResponse(HANDLE hApp, UI_32 f_groupID);



  EFrameworkunifiedStatus OnLaunchGroupTriggerProcessResponseTimerExpiry(HANDLE hApp);

  EFrameworkunifiedStatus OnNppStopComplete(HANDLE hApp);      /// Process stop Complete from NPPService
  EFrameworkunifiedStatus OnOpenSession(HANDLE hApp) noexcept;     /// Process open session request
  EFrameworkunifiedStatus OnCloseSession(HANDLE hApp);         /// Process close session acknowledgment

  EFrameworkunifiedStatus CloseApplication(HANDLE hApp);
  EFrameworkunifiedStatus OnGetAppInfo(HANDLE hApp);

  /// < Heartbeat Thread Callbacks
  EFrameworkunifiedStatus OnHeartBeatErrorReport(HANDLE hApp);

  EFrameworkunifiedStatus OnCurrentSMStateQuery(HANDLE hApp);

  EFrameworkunifiedStatus OnNPPReadyStatusCallback(HANDLE hApp);
  EFrameworkunifiedStatus OnNPPReadyEventCallback(HANDLE hApp);
  EFrameworkunifiedStatus OnWakeupCallback(HANDLE hApp);
  EFrameworkunifiedStatus OnPowerRequestCallback(HANDLE hApp) noexcept;
  EFrameworkunifiedStatus OnBackupMgrAvailCallback(HANDLE hApp);
  EFrameworkunifiedStatus OnShutdownModulesRequest(HANDLE hApp);
  EFrameworkunifiedStatus OnSystemModeInfoRequest(HANDLE hApp) noexcept;
  EFrameworkunifiedStatus ClearDramBackupInfo(HANDLE hApp);
  EFrameworkunifiedStatus OnInitCompReportCallback(HANDLE hApp) noexcept;
  EFrameworkunifiedStatus SetEnvVariableForVupMode(BOOL bIsVupMode);
  static int CreateSyslogd();
  static int CreateRtUsb();
  EFrameworkunifiedStatus PerformMountResetAll();
  EFrameworkunifiedStatus PerformMountResetRamd(const HANDLE h_app, const bool isMountRamd);
  EFrameworkunifiedStatus PerformMountNand();
  EFrameworkunifiedStatus PerformMountRamd();
  EFrameworkunifiedStatus PerformUmountNand();
  EFrameworkunifiedStatus PerformUmountRamd();
  EFrameworkunifiedStatus PerformForceUmountRamd();
  EFrameworkunifiedStatus PerformFinalUmount(BOOL bIsNeedLog);
  EFrameworkunifiedStatus SecureChipOff();
  EFrameworkunifiedStatus OnRebootNotification(HANDLE hApp);
  EFrameworkunifiedStatus CallStopCompleteHandler(HANDLE hApp, BOOL bISDetectTimeout = FALSE);
  EFrameworkunifiedStatus OnVarCodeSetRequest(HANDLE hApp) noexcept;
  EFrameworkunifiedStatus OnSetBootloaderInfoRequest(HANDLE hApp) noexcept;

  EFrameworkunifiedStatus OnGetBootloaderInfoRequest( HANDLE hApp );
  EFrameworkunifiedStatus On_CWORD56_HeartBeatRequest(HANDLE hApp) noexcept;
  EFrameworkunifiedStatus On_CWORD56_HeartBeatResponseIntervalTimerExpiry(HANDLE hApp);
  EFrameworkunifiedStatus OnGroupLaunchWaitTimeout(HANDLE hApp);
  EFrameworkunifiedStatus OnModuleConnectWaitTimeout(HANDLE hApp);
  EFrameworkunifiedStatus OnStartRespMonitorTimeout(HANDLE hApp);
  EFrameworkunifiedStatus OnShutdownCompleteMonitorTimeout(HANDLE hApp);
  EFrameworkunifiedStatus PerformCpuReset(HANDLE hApp, ESMCpuResetReason f_eSmCpuResetReason, std::string f_messageStr = "") noexcept;  // NOLINT
  EFrameworkunifiedStatus OnCpuResetRequest(HANDLE hApp) noexcept;
  EFrameworkunifiedStatus OnSetBootModeRequest(HANDLE hApp) noexcept;
  EFrameworkunifiedStatus OnSetDataResetModeRequest(HANDLE hApp) noexcept;
  EFrameworkunifiedStatus OnSetProgUpdateStateRequest(HANDLE hApp) noexcept;
  EFrameworkunifiedStatus OnLaunchGroupTriggerProcessResponse(HANDLE hApp);
  EFrameworkunifiedStatus OnSetWakeupOrderRequest(HANDLE hApp) noexcept;
  EFrameworkunifiedStatus OnSetNextWakeupTypeRequest(HANDLE hApp) noexcept;

  LBM_UINT32 GetDramStatusInfo(const HANDLE h_app);
  void     SetDramStatusInfo(LBM_UINT32 f_state);

  //
  // Start Confirmation, Get _CWORD56_ Version callback functions
  EFrameworkunifiedStatus OnSetStartupConfirmationDataRequest(HANDLE hApp) noexcept;

  void SystemManagerDebugDump(HANDLE hApp);


 private:
  CThreadInfo              m_hHeartbeatThread;            // Handle to Heartbeat Thread
  CThreadInfo              m_hProcLauncherThread;         // Handle to Process Launcher Thread
  UI_32                    m_SystemLaunchProgress;        // Launch Progress of system
  TimerCtrl *              m_GroupLaunchTimer;            // Launch Monitor Timer
  UI_32                    m_aTimerIDs[eSM_TIMERS_END];   // TimerIDs returned by create timer
  PreLaunchModuleList      m_PreLaunchModuleList;         // List of modules that shuuld be launched preliminarily.
  GroupRelaunchModuleList  m_GroupRelaunchModuleList;     // List of modules that need to relaunch
  UI_32                    m_GroupRelaunchCount;          // Count for group relaunch
  UI_32                    m_GroupRelaunchLimit;          // Limit for group relaunch
  GroupLaunchMap           m_MapProclaunchGrps;           // Map of modules that should be launched.
  ProcessNameMap           m_MapProcNames;                // Map of module binary names to queue names.
  HANDLE                   m_hPowerServiceSession;
  UI_32                    m_ActiveGroupId;               // Modules of Group to whom START/STOP has been sent
  wakeInfo                 m_Wake;                        // WakeUp data from power
  EPWR_WAKEUP_FACTOR_TYPE  m_StartUpReason;               // System Manager preserve start up reason
  LBM_RAM_t                m_Lbm_Ram;
  LBM_RAM_t               *m_BootMemory;                  // Boot If
  UI_32                    m_BootMemoryMapSize;           // Size of Boot If
  ESMDataResetModeInfo     m_DataResetMode;               // Data Reset Mode
  SMProgUpdateState        m_ProgUpdateState;             // ProgUpdate State
  ESMNextWakeupType        m_NextWakeupType;              // Next Progupdate State
  ESMDramBackupStatus      m_DramBackupStatus;            // Dram Backup Status
  BOOL                     m_isIlgReset;                  // Error Detected by SYS-ucon
  ESMResetStatus           m_ResetStatus;                 // Reset Status
  UI_32                    m_ResetCount;                  // Reset Count
  T_SS_SM_INIT_HOOK_OUT_PARAM m_productCustomPrm;         // Product customized parameters

  SS_SMModuleState         m_SMCurrentState;              // Current state of System Manager
  UI_32                    m_MaxShutdownTime;             // Calculate Max shutdown time based on info from xml
  UI_32                    m_NbrDebugDumpRspnRecv;        // Total number of debug dump responses received.
  BOOL                     m_isRcvModeInfo;
  SystemModeInfo           m_SystemModeInfo;  // System Mode Info, SM to read data from persistent file at start
  ESMBootModeInfo          m_BootMode;                    // Application or Programming
  std::vector<std::string> ListOfServicesAuthorizedToSetBootMode;
  SS_String                m_BinaryFilesPath;             // Store binary files path
  SS_String                m_ConfigFilesPath;             // Store config files path
  ConfigurationData        m_SMConfig;                    // Configuration Data of System Manager and its threads
  HndlrCb                  m_pfStopCompleteHandler;
  CVersionNumInfo          m_VersionNumberStruct;
  SS_String                m_BuildInfoStr;

  BOOL                     m_isRstPending;
  TSystemManagerCpuResetInfo m_rstPendingInfo;

  SM_POWER_EVENT_QUEUE     m_dqDeferMsg;
  BOOL                     m_UsingVMPlayer;               // Running on a VMPlayer ?
  UI_32                    NVM_VALID_SIGNATURE;
  StartupConfirmationMsgStrut  m_startUpConfirmationMsg;
  EPWR_USER_MODE_TYPE      m_lastUserMode;
  EPWR_SHUTDOWN_TRIGGER_TYPE   m_shutdownTrigger;         // Set from Shutdown_Request
                                                          // msg from _CWORD56_, sent to
                                                          // all modules as part of
                                                          // SS_SM_STOP msg.
  BOOL                     m_isImmediateReset;
  BOOL                     m_isImmResetReq;
  BOOL                     m_needReNotifyStartPrm;
  CHAR                    *m_pVarCodeStr;
  GroupLaunchMapIter       m_SSLGroupLaunchMapIterator;
  ModuleLaunchListIter     m_ModuleLaunchListIter;
  BOOL                     m_bIsNPP_ServicesStarted;
  BOOL                     m_bIsBackupAvail;
  //
  DynamicLauncher          m_oSystemLauncher;
  DynamicModuleStart       m_SystemStarter;
  //
  BOOL                     m_NPPStopSent;
  ePwrServiceUserModeChangeReasonType m_userModeChangeReason;

  HBList                   m_HBList;

  cmdHist                  m__CWORD56_CmdHist;
  cmdHistIter              m__CWORD56_HistIter;
  cmdHist                  m_SMCmdHist;
  cmdHistIter              m_SMHistIter;
  cmdHist                  m_TimerCmdHist;
  cmdHistIter              m_TimerHistIter;
  cmdHist                  m_PubCmdHist;
  cmdHistIter              m_PubHistIter;
  cmdHist                  m_ErrHist;
  cmdHistIter              m_ErrHistIter;

  pthread_mutex_t sm_hist_mutex;

  bool  m_InitialBoot;

  void SetCmdHist(std::string cmd, cmdHist &hist, cmdHistIter &it, std::string sender);  // NOLINT

  // ILGRST or ACCOFFON Logging
  bool     StoreDebugLogs(SS_STORELOGS_OPE_TYPE type);

  UI_32 InProgressStateToSendMsg() const;
  SMModuleState InProgressStateToState() const;

  //************************************************************************
  // Error Event Logging                                                   *
  //
  int              m_ClProcessSigFd;
  CSysMemoryMonitor        m_SysMemMonitor;

  UI_32              m_FreeMemAvailable;
  off_t              m_coreFileSizeBytes;
  THbReportData          m_HBReport;
  eSMBootMicroResetReason    m_BootMicroResetReason;
  BOOL               m_errorEventQueueLocked;
  ERROR_EVENT_QUEUE        m_errorEventQueue;
  Timer              m_errorEventTimers[eSM_ERROR_EVENT_TIMER_END];
  BOOL               m_isPrevErrEventCompleted;
  EFrameworkunifiedStatus           m_errorEventResult;
  EArtifactId          m_requestedArtifactId;
  ERROR_EVENT_QUEUE_ITER     m_errorEventCurrentIter;
  //                                                                       *
  // End of Error Event Logging                                            *
  //************************************************************************


  ////******************************************************************////
  ////     enum<=>enum maps                                             ////
  //
  //
  //  Control Mode <=> Limp Home Cutoff Mode enum maps
  std::map<ECONTROL_MODE, EPWR_LHC_TYPE> m_Control_to_LimpHomeCutoffMode_map;
  std::map<EPWR_LHC_TYPE, ECONTROL_MODE> m_LimpHomeCutoff_to_ControlMode_map;
  //
  //  Control Mode <=> Production Mode enum maps
  std::map<ECONTROL_MODE, EPWR_PROD_MODE_TYPE> m_Control_to_ProductionMode_map;
  std::map<EPWR_PROD_MODE_TYPE, ECONTROL_MODE> m_Production_to_ControlMode_map;
  //
  //  Control Mode <=> Transport Mode enum maps
  std::map<ECONTROL_MODE, EPWR_TRANSPORT_MODE_TYPE> m_Control_to_TransportMode_map;
  std::map<EPWR_TRANSPORT_MODE_TYPE, ECONTROL_MODE> m_Transport_to_ControlMode_map;
  //
  //  PowerService SystemModeInfo <=> SystemServices BootMode enum maps
  std::map<EPWR_SYSTEM_MODE_INFO, ESMBootModeInfo> m_PSSMI_to_SSBootMode_map;
  std::map<ESMBootModeInfo, EPWR_SYSTEM_MODE_INFO> m_SSBootMode_to_PSSMI_map;
  //
  //  EPWR_POWER_STATE_TYPE <=> BOOL
  std::map<EPWR_POWER_STATE_TYPE, BOOL>    m_PowerType_to_SSBoolEnumMap;
  std::map<EPWR_POWER_STATE_TYPE, EPWR_USER_MODE_TYPE>
                         m_PowerType_to_SSUserModeEnumMap;
  std::map<BOOL, EPWR_POWER_STATE_TYPE>    m_SSBool_to_PowerTypeEnumMap;
  //
  //  User Mode
  std::map<EUSER_MODE, EPWR_USER_MODE_TYPE>  m_ROM_to_SSUserModeEnumMap;
  std::map<EPWR_USER_MODE_TYPE, EUSER_MODE>  m_SS_to_ROMUserModeEnumMap;
  std::map<EPWR_USER_MODE_TYPE, BOOL>    m_SSUserMode_to_SSBoolEnumMap;
  std::map<BOOL, EPWR_USER_MODE_TYPE>    m_SSBool_to_SSUserModeEnumMap;

  std::map<UI_32, LBM_UINT32>   m_PsmSysupDram_to_SysupDramMap;
  std::map<UI_32, LBM_UINT32>   m_PsmStartStatus_to_SysupCndMap;
  std::map<UI_32, LBM_UINT32>   m_PsmComSts_to_SubcpuStsMap;
  std::map<UI_32, LBM_UINT32>   m_PsmDramSts_to_SdramStsMap;
  std::map<UI_32, LBM_UINT32>   m_PsmDramSts_to_RamjudgeStsMap;
  std::map<UI_32, LBM_UINT32>   m_PsmDramSts_to_SelfRefreshMap;
  //
  ////     End of enum<=>enum maps                                      ////
  ////******************************************************************////

  EFrameworkunifiedStatus init_process_launcher(HANDLE hApp);
  EFrameworkunifiedStatus start_process_launching(HANDLE hApp);
  EFrameworkunifiedStatus init_Heartbeat(HANDLE hApp);
  EFrameworkunifiedStatus init_crash_detector(HANDLE hApp);
  EFrameworkunifiedStatus init_sysmem_monitor(HANDLE hApp);
  EFrameworkunifiedStatus SendRequestToHeartBeat(HANDLE hAPP, EHBProtocolMessages, VOID *, UI_32);
  EFrameworkunifiedStatus send_launch_status(HANDLE hApp, UI_32 u32LaunchState);

  EFrameworkunifiedStatus register_all_notification_callbacks(HANDLE hApp);
  VOID SMStateStartCompleteEntry(HANDLE hApp);
  VOID SMStateEntry(HANDLE hApp, SS_SMModuleState l_SMState);
  VOID SMStateExit(HANDLE hApp, SS_SMModuleState l_SMState);
  VOID SMSetState(HANDLE hApp, SS_SMModuleState l_SMState);

  // Wakeup from Power
  EFrameworkunifiedStatus BeginStartup(HANDLE hApp);
  EFrameworkunifiedStatus start_all_modules_of_group(HANDLE hApp, UI_32 f_ui32GroupNumber) noexcept;
  EFrameworkunifiedStatus send_power_request_complete_response(HANDLE hApp,
                           std::string pStr) noexcept;
  BOOL     have_all_services_start_completed(const SMModuleState f_moduleState);

  // Power Off from Power
  EFrameworkunifiedStatus PublishPowerOnOffNotification(HANDLE hApp) noexcept;

  // Shutdown from Power
  EFrameworkunifiedStatus stop_all_modules_of_group(HANDLE hApp);
  UI_32    calculate_max_shutdown_time();
  BOOL     is_service_shutdown_ready(ModuleLaunchListIter &modIte);  // NOLINT
  BOOL     have_critical_services_stopped();
  VOID     check_all_groups_have_stopped(HANDLE hApp);
  EFrameworkunifiedStatus perform_force_reset(HANDLE hApp);
  EFrameworkunifiedStatus send_shutdown_complete_response(HANDLE hApp);
  EFrameworkunifiedStatus SendShutdownResponseMessage(HANDLE hApp);


  EFrameworkunifiedStatus ValidateUserModeMessage(HANDLE hApp, EPWR_USER_MODE_TYPE &l_eUserModeState);  // NOLINT

  VOID SetStartGroup();

  VOID ReadPathFromEnvironmentVariables();
  SS_String ReadConfigDataFileNameFromEnvironmentVariable();
  SS_String ReadLaunchConfigFileNameFromEnvironmentVariable();


  VOID ReadUsingVMPlayerEnvironmentVariable();

  EFrameworkunifiedStatus GetBootMode(ROM_AccessIf & f_cRomAccess
                        , ESMBootModeInfo & f_BootMode);  // NOLINT
  EFrameworkunifiedStatus GetLastUserMode(ROM_AccessIf & f_cRomAccess  // NOLINT
                           , EPWR_USER_MODE_TYPE& f_lastUserMode);  // NOLINT
  EFrameworkunifiedStatus SaveLastUserModeToNVM(EPWR_USER_MODE_TYPE l_eUserModeState);
  EFrameworkunifiedStatus GetLimpHomeCutoffMode(ROM_AccessIf & f_cRomAccess
                                 , EPWR_LHC_TYPE& f_limpHomeCutoff);  // NOLINT
  EFrameworkunifiedStatus GetProductionMode(ROM_AccessIf & f_cRomAccess
                             , EPWR_PROD_MODE_TYPE& f_productionMode);  // NOLINT
  EFrameworkunifiedStatus GetSignature(ROM_AccessIf & f_cRomAccess
                        , UI_32 * pData);
  EFrameworkunifiedStatus GetTransportMode(ROM_AccessIf & f_cRomAccess
                            , EPWR_TRANSPORT_MODE_TYPE& f_transportMode);  // NOLINT
  EFrameworkunifiedStatus GetDataResetMode(ROM_AccessIf & f_cRomAccess
                            , ESMDataResetModeInfo& f_dataResetMode, bool isFast = false);  // NOLINT
  EFrameworkunifiedStatus GetResetCount(ROM_AccessIf & f_cRomAccess
                            , UI_32& f_resetCount);  // NOLINT
  EFrameworkunifiedStatus GetLastIlgReset(ROM_AccessIf & f_cRomAccess
                            , BOOL& f_lastIlgReset);  // NOLINT
  EFrameworkunifiedStatus GetProgUpdateState(ROM_AccessIf & f_cRomAccess
                            , SMProgUpdateState& f_progUpdateState);  // NOLINT
  EFrameworkunifiedStatus GetErrLogCount(ROM_AccessIf & f_cRomAccess
                            , UI_32& f_errLogCount);  // NOLINT
  EFrameworkunifiedStatus GetNextWakeupType(ROM_AccessIf & f_cRomAccess
                            , ESMNextWakeupType& f_wakeupType);  // NOLINT
  EFrameworkunifiedStatus SetActiveFlashloader(ROM_AccessIf & f_cRomAccess
                                 , EACTIVE_FLASHLOADER f_FlashloaderMode);
  EFrameworkunifiedStatus SetBootMode(ROM_AccessIf & f_cRomAccess
                        , ESMBootModeInfo f_BootMode);
  EFrameworkunifiedStatus SetLastUserMode(ROM_AccessIf & f_cRomAccess
                           , EPWR_USER_MODE_TYPE f_lastUserMode);
  EFrameworkunifiedStatus SetLimpHomeCutoffMode(ROM_AccessIf & f_cRomAccess
                                 , EPWR_LHC_TYPE f_limpHomeCutoff);
  EFrameworkunifiedStatus SetProductionMode(ROM_AccessIf & f_cRomAccess
                             , EPWR_PROD_MODE_TYPE f_productionMode);
  EFrameworkunifiedStatus SetSignature(ROM_AccessIf & f_cRomAccess
                        , UI_32 Data);
  EFrameworkunifiedStatus SetTransportMode(ROM_AccessIf & f_cRomAccess
                            , EPWR_TRANSPORT_MODE_TYPE f_transportMode);
  EFrameworkunifiedStatus SetDataResetMode(ROM_AccessIf & f_cRomAccess
                            , ESMDataResetModeInfo f_dataResetMode);
  EFrameworkunifiedStatus SetResetCount(ROM_AccessIf & f_cRomAccess
                            , UI_32 f_resetCount);
  EFrameworkunifiedStatus SetLastIlgReset(ROM_AccessIf & f_cRomAccess
                            , BOOL f_lastIlgReset);
  EFrameworkunifiedStatus SetProgUpdateState(ROM_AccessIf & f_cRomAccess
                            , SMProgUpdateState f_progUpdateState);
  EFrameworkunifiedStatus SetErrLogCount(ROM_AccessIf & f_cRomAccess
                            , UI_32 f_errLogCount);
  EFrameworkunifiedStatus SetNextWakeupType(ROM_AccessIf & f_cRomAccess
                            , ESMNextWakeupType f_wakeupType);
  EFrameworkunifiedStatus SetProductPrivate(ROM_AccessIf & f_cRomAccess
                            , UI_8 buf[ROM_PRODUCT_PRIVATE_MAX]);
  EFrameworkunifiedStatus PublishBootModeNotification(HANDLE hApp) noexcept;
  EFrameworkunifiedStatus ReturnBootModeAndStatus(HANDLE hApp
                                    , SS_SystemManagerProtocol protocolID
                                    , PCSTR                    p_sIDName
                                    , ESMBootModeInfo          f_BootMode
                                    , EFrameworkunifiedStatus               eFrameworkunifiedStatus) noexcept;
  EFrameworkunifiedStatus ValidateBootModeMessage(HANDLE hApp
                                    , ESMBootModeInfo &l_eBootModeState) noexcept;  // NOLINT
  EFrameworkunifiedStatus ValidateRequester(HANDLE hApp
                              , std::vector<std::string> &L
                              , PCSTR p_sActionName);

  VOID       InitiateAllGroupsShutdown(HANDLE hApp);
  VOID       SendDeferMsg(HANDLE hApp);
  EFrameworkunifiedStatus GetVersionNumber(SS_VersionNumberType &f_VersionNumber);  // NOLINT
  EFrameworkunifiedStatus GetBuildInfo(std::string &f_BuildInfoStr);  // NOLINT
  EFrameworkunifiedStatus RegisterCallbacksForRequester(HANDLE hApp, PCSTR pRequester);
  EFrameworkunifiedStatus SetSystemModeInfoStruct();

  EFrameworkunifiedStatus PerformLaunchProcedure(HANDLE hApp
                                  , ModuleLaunchListIter f_ModuleIterator
                                  , SS_String & f_stopCompName);  // NOLINT

  EFrameworkunifiedStatus PerformTerminateProcedure(HANDLE hApp
                                 , ModuleLaunchListIter f_ModuleIterator
                                 , SS_String & f_availabilityName
                                 , SS_String & f_stopCompName);  // NOLINT

  EFrameworkunifiedStatus SendRequestToLauncher(HANDLE        hApp
                   , ModuleLaunchListIter  f_ModuleIterator
                   , ESMPLThreadCommandIds f_CommandId
                   , SS_String       f_CommandIdStr);

  EFrameworkunifiedStatus FindNameOfTerminatedProcess(SI_32 pid, SS_String &f_ModuleName);  // NOLINT

  EFrameworkunifiedStatus GetBinaryNameOfProcess(SS_String  f_ModuleQueueName,
                    SS_String &f_ModuleBinaryName);  // NOLINT

  EFrameworkunifiedStatus RemoveModuleEntryFromHB(HANDLE hApp, const CHAR *f_ModuleName);

  EFrameworkunifiedStatus PublishUserModeNotification(HANDLE hApp) noexcept;
  EFrameworkunifiedStatus PerformPowerOnOffUserModePublication(
                  HANDLE hApp,
                  const char * p_NotificationStr) noexcept;
  EFrameworkunifiedStatus SendSystemModeInfoResponse(
          HANDLE hApp,
          EPWR_STARTUP_STAGE_TYPE f_startupStage) noexcept;

  VOID     SYSTEMMANAGERLOG_StartupConfirmationMsg(EPWR_SC_MSG_STRUCT & f_scMsg);  // NOLINT
  VOID     LogGroupModulesState(UI_32 f_groupId, std::string pStr) noexcept;
  VOID     LogAllGroupModulesState(std::string pStr) noexcept;

  EFrameworkunifiedStatus ProcessGroupAsStarted(HANDLE hApp
                  , GroupLaunchMapIter f_GroupIterator);
  EFrameworkunifiedStatus ProcessGroupAsStarted(HANDLE hApp
                  , UI_32 f_groupId);
  EFrameworkunifiedStatus GetModuleIterator(PCSTR          f_moduleName
                , ModuleLaunchListIter & f_moduleIter) noexcept;  // NOLINT
  BOOL     IsGroupStarted(UI_32 f_groupId);
  BOOL     IsGroupStopped(UI_32 f_groupId);
  EFrameworkunifiedStatus ResetModulesStateToConnected(HANDLE hApp);

  EFrameworkunifiedStatus GetTimeStamp(std::string& TimeStamp);  // NOLINT
  std::string GetErrorEventInString(UI_32 &ErrorEventType);  // NOLINT
  EFrameworkunifiedStatus GetLastSourceInfo(SS_SOURCE &source_type);  // NOLINT
  //
  // Return True when Module is in correct / appropriate state wrt SM state
  // return false otherwise
  BOOL     GetModuleCondition(ModuleLaunchListIter & f_ModuleIter);  // NOLINT

  VOID LogESystemmanagerStatusEnums();
  VOID LogProtocolIDs();

  VOID SystemmanagerLogString(TFrameworkunifiedZone   f_zone,
            PCSTR    f_func,
            std::string &f_text);  // NOLINT

  inline BOOL is_current_state(SS_SMModuleState SMState) {
    return (m_SMCurrentState == SMState);
  }

  //**************************************************************************
  // Get results from TestClient                                             *
  //                                                                         *
  EFrameworkunifiedStatus add_state_information_to_response(CHAR *f_MessageResponse) const;
  EFrameworkunifiedStatus construct_get_result_response(CHAR *f_MessageResponse);      // *
  EFrameworkunifiedStatus open_session_with_sm_test_client(HANDLE hApp,                // *
                                              PCSTR pRequester);          // *
  //                                                                         *
  // End of 'Get results from TestClient'                                    *
  //**************************************************************************

  //************************************************************************
  // Error Event Logging                                                   *
  //                                                                       *
  EFrameworkunifiedStatus  ErrorEventInit(HANDLE                f_hApp);

  EFrameworkunifiedStatus  OnHeartBeatErrorDetected(HANDLE                f_hApp);
  EFrameworkunifiedStatus  OnProcessTermDetected(HANDLE                f_hApp);
  BOOL        isPreLaunchedProcess(int                   f_pid);
  EFrameworkunifiedStatus  OnPreLaunchedProcessTermDetected(HANDLE                f_hApp,
                                               SMProcessExitInfo    &f_procExitInfo,  // NOLINT
                                               BOOL                  f_bIsExited);
  EFrameworkunifiedStatus  OnProcessCrashDetected(HANDLE                f_hApp,
                                     SMProcessExitInfo    &f_procExitInfo);  // NOLINT
  EFrameworkunifiedStatus  OnProcessExitDetected(HANDLE                f_hApp,
                                    SMProcessExitInfo    &f_procExitInfo);  // NOLINT

  EFrameworkunifiedStatus  OnUserInvokedLoggingRequest(HANDLE                f_hApp);

  EFrameworkunifiedStatus  OnEelExportRequest(HANDLE                f_hApp);

  EFrameworkunifiedStatus  OnSystemmanagerEmmcLogsRequest(HANDLE                f_hApp);
  EFrameworkunifiedStatus  OnSystemmanagerClearLogsRequest(HANDLE                f_hApp);

  EFrameworkunifiedStatus  OnDiagLoggingRequest(HANDLE                f_hApp);
  EFrameworkunifiedStatus  OnCANLoggingRequest(HANDLE                f_hApp);

  EFrameworkunifiedStatus  OnPropagateSystemError(HANDLE                f_hApp);

  EFrameworkunifiedStatus  OnCaptureAllLogsRequest(HANDLE                f_hApp);
  EFrameworkunifiedStatus  OnCaptureScreenShotRequest(HANDLE                f_hApp);
  EFrameworkunifiedStatus  OnCaptureSystemmanagerLogsRequest(HANDLE                f_hApp);


  EFrameworkunifiedStatus  OnBootMicroResetNotification(HANDLE                f_hApp);

  EFrameworkunifiedStatus  OnObtainBootMicroLog(HANDLE                f_hApp);
  EFrameworkunifiedStatus  RequestBootMicroLog(HANDLE                f_hApp);

  EFrameworkunifiedStatus  OnBootMicroLogResponse(HANDLE                f_hApp);

  EFrameworkunifiedStatus  OnErrorEventBootMicroLogResponseTimeout(HANDLE                f_hApp);

  EFrameworkunifiedStatus  OnLowSystemMemory(HANDLE                f_hApp);
  EFrameworkunifiedStatus  OnObtainSystemmanagerSystemDataCsv(HANDLE                f_hApp);
  EFrameworkunifiedStatus  OnObtainShowMemTxt(HANDLE                f_hApp);

  EFrameworkunifiedStatus  OnErrorEventCoreFilePollTimeout(HANDLE                f_hApp);

  EFrameworkunifiedStatus  OnObtainDebugDumpLog(HANDLE                f_hApp);
  EFrameworkunifiedStatus  SendDebugDumpRequestToAllModules(HANDLE                f_hApp);

  EFrameworkunifiedStatus  OnDebugDumpResponseReceived(HANDLE                f_hApp);

  EFrameworkunifiedStatus  OnDebugDumpCompleteTimeout(HANDLE                f_hApp);

  EFrameworkunifiedStatus  ErrorEventEnqueue(HANDLE                f_hApp,
                                EErrorEventType       f_eventType,
                                std::string          &f_moduleQueueName,  // NOLINT
                                EErrorEventResetType  f_resetType   = eErrorEventResetTypeNone,
                                const SMLoggingInfo  &f_loggingInfo = SMLoggingInfo()) noexcept;

  EFrameworkunifiedStatus  PerformResetAfterLogging(HANDLE                f_hApp,
                                       ESMCpuResetReason     f_eCpuResetReason,
                                       std::string           f_messageStr = "");

  EFrameworkunifiedStatus  ErrorEventProcessNext(HANDLE                f_hApp);

  EFrameworkunifiedStatus  OnLoggingStartRequest(HANDLE                f_hApp);
  EFrameworkunifiedStatus  OnErrorEventLoggerStartRequestTimeout(HANDLE                f_hApp);

  EFrameworkunifiedStatus  OnErrorEventArtifactRequest(HANDLE                f_hApp);

  EFrameworkunifiedStatus  SendLogArtifactResponseToLogger(HANDLE                f_hApp,
                                              EArtifactId           f_artifactId,
                                              std::string           f_artifactFilePathAndName);

  EFrameworkunifiedStatus  OnErrorEventLoggingComplete(HANDLE                f_hApp);

  EFrameworkunifiedStatus  ActOnModuleFailure(HANDLE                f_hApp,
                                 SS_String             f_moduleName,
                                 BOOL                 &f_refResetRequired);  // NOLINT
  std::string ErrorEventCreateHmiDisplayString(void);

  //                                                                       *
  // End of Error Event Logging                                            *
  //************************************************************************

  UI_32 m_SystemManagerPriority;
};
#define REC_HIST(pStrCmd, hist, it, pStrSender)    \
  SetCmdHist(pStrCmd, hist, it, pStrSender);

#define REC_HIST_IF_SUCCESSFUL(pStr, hist, it, sender, l_eStatus) \
  if (eFrameworkunifiedStatusOK == l_eStatus) {     \
    REC_HIST(pStr, hist, it, sender) \
  }

#define LOG_ERROR_REC_HIST(l_eStatus, pStr) { \
    LOG_ERROR(pStr);                     \
    REC_HIST(pStr, m_ErrHist, m_ErrHistIter, GetStr(l_eStatus).c_str()); \
  }
#define LOG_STATUS_REC_HIST_IF_ERRORED(l_eStatus, pStr)   \
  if (eFrameworkunifiedStatusOK != l_eStatus) { \
    LOG_ERROR(pStr);                 \
    REC_HIST(pStr, m_ErrHist, m_ErrHistIter, GetStr(l_eStatus).c_str()); \
  } else {                           \
    LOG_SUCCESS(pStr);               \
  }  // End of #define LOG_STATUS(l_eStatus,pStr)

#define LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus, pStr) \
  if (eFrameworkunifiedStatusOK != l_eStatus) {                  \
    LOG_ERROR(pStr);                                  \
    REC_HIST(pStr, m_ErrHist, m_ErrHistIter, GetStr(l_eStatus).c_str()); \
  }  // End of #define LOG_STATUS_IF_ERRORED(l_eStatus,pStr)

#endif  // __SS_SYSTEM_MANAGER_H__  // NOLINT
