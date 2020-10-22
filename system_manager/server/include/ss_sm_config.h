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
/// \brief    This file provides support for System Manager configuration.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef SS_SM_CONFIG_H_  // NOLINT
#define SS_SM_CONFIG_H_

#include <native_service/ns_config_parser_if.h>
#include <system_service/ss_system_types.h>
#include <system_service/ss_system_manager_if.h>
#include <system_service/ss_system_manager_if_local.h>
#include <system_service/ss_system_manager_protocol.h>
#include <system_service/ss_string_maps.h>
#include <list>
#include <map>
#include <string>
#include <vector>
#include "ss_sm_process_launcher_protocol.h"


/// All timer values are **position-dependent** in the timer calls ! These
/// defines are currently all used as Start and Restart Seconds parameters.
/// \param [in] id
///         UI_32 - timer id that you want to start
/// \param [in] ss
///         UI_32 - Start time in seconds
/// \param [in] sms
///         UI_32 - Start time in milliseconds
/// \param [in] rs
///         UI_32 - Repeat time in seconds
/// \param [in] rms
///         UI_32 - Repeat time in milliseconds
#define SS_GROUP_LAUNCH_TIMER_CONFIG               (3)
#define SS_HEARTBEAT_MONITOR_TIMER_CONFIG          (10)
#define SS_PROCESSLAUNCHER_MONITOR_TIMER_CONFIG    (10)
#define SS_CLIENT_START_MONITOR_TIMER_CONFIG       (3)
#define SS_CLIENT_STOP_MONITOR_TIMER_CONFIG        (1)
#define SS_NPP_STATUS_CHECK_MONITOR_TIME_SEC       (5)
#define SS_LAUNCH_GROUP_TRIGGER_TIMER_CONFIG       (10)
// size of the internal debug dump buffer
#define SS_SM_DEBUG_DUMP_MAX_SIZE                   (12288u)
// number of _CWORD56_ command history log entries for the debug dump
#define SS_SM__CWORD56__CMD_HIST_SIZE                    (10u)
// number of timer history log entries for the debug dump
#define SS_SM_TIMER_CMD_HIST_SIZE                   (10u)
// number of message history log entries for the debug dump
#define SS_SM_CMD_HIST_SIZE                         (20u)
// number of publishing history log entries for the debug dump
#define SS_SM_PUB_CMD_HIST_SIZE                     (5u)
// number of error history log entries for the debug dump
#define SS_SM_ERR_HIST_SIZE                         (5u)

// When occurs repeat to abnormal rebooting, logging cause of error at first time and next time.
// When it occurs "ILG_RESET", Maximum number of log is 4 so that the log will be stored at the just after the startup.
// Need reconsideration if maximum number of abnormal logging is less than 4 using SSL.
#define SS_SM_ERR_LOGGING_LIMIT     (8)

// Maximum number of continuous processing of "NaviReset" by errors.
// It try to recover by NaviDet when it exceeds this limit. 
#define SS_SM_ERR_NAVI_RESET_LIMIT  (5)

typedef enum {
  MODULE_STATE_INVALID = 0      /// Initial state of all modules
  , MODULE_STATE_SKIPPED        /// Non-critical module has crossed max re-launch limit
                                ///  or hSession was null when attempted to start
  , MODULE_STATE_LAUNCHING      /// Module launch has been requested
  , MODULE_STATE_LAUNCHED       /// Module has been launched but a session
                                /// connection has NOT been established
  , MODULE_STATE_LAUNCH_FAILED  /// Oops
  , MODULE_STATE_CONNECTED      /// A session connection has been established
  , MODULE_STATE_START_SENT     /// SM has sent START to the module
  , MODULE_STATE_START_FAILED   /// FrameworkunifiedSendMsg(SS_SM_START) failed
  , MODULE_STATE_STARTED        /// Module has replied START Complete to SM
  , MODULE_STATE_STOP_SENT      /// SM has sent STOP to the module
  , MODULE_STATE_STOP_FAILED    /// FrameworkunifiedSendMsg(SS_SM_STOP) failed
  , MODULE_STATE_STOPPED        /// Module has replied STOP Complete to SM
  , MODULE_STATE_START_PRE_SENT           //!< SM has sent Pre-boot start to the module
  , MODULE_STATE_START_PRE_FAILED         //!< FrameworkunifiedSendMsg(SS_SM_PRE_START) failed
  , MODULE_STATE_STARTED_PRE              //!< Module has replied Pre-boot start Complete to SM
  , MODULE_STATE_STOP_PRE_SENT            //!< SM has sent Pre-boot stop to the module
  , MODULE_STATE_STOP_PRE_FAILED          //!< FrameworkunifiedSendMsg(SS_SM_PRE_STOP) failed
  , MODULE_STATE_STOPPED_PRE              //!< Module has replied Pre-boot stopComplete to SM
  , MODULE_STATE_START_BACKGROUND_SENT    //!< SM has sent Background-boot start to the module
  , MODULE_STATE_START_BACKGROUND_FAILED  //!< FrameworkunifiedSendMsg(SS_SM_BACKGROUND_START) failed
  , MODULE_STATE_STARTED_BACKGROUND       //!< Module has replied Background-boot start Complete to SM
  , MODULE_STATE_STOP_BACKGROUND_SENT     //!< SM has sent Background-boot stop to the module
  , MODULE_STATE_STOP_BACKGROUND_FAILED   //!< FrameworkunifiedSendMsg(SS_SM_BACKGROUND_STOP) failed
  , MODULE_STATE_STOPPED_BACKGROUND       //!< Module has replied Background-boot stop Complete to SM
} SMModuleState;
//
//    Module State string map type
typedef std::map<SMModuleState, std::string>  SMModuleStateStrMapType;

typedef  enum {
  MODULE_DEBUG_DUMP_STATE_NOT_REQUESTED = 0
  , MODULE_DEBUG_DUMP_STATE_REQUEST_SENT
  , MODULE_DEBUG_DUMP_STATE_RESPONSE_RECEIVED
} SMModuleDebugDumpState;

typedef std::map<SMModuleDebugDumpState, std::string>  SMModuleDebugDumpStateStrMapType;

/*
<?xml version="1.0" encoding="UTF-8"?>
<Configuration>
    <!-- System Service Launch control elements -->
    <SystemManager>
        <Launch>
            <Group1>
                <group_name>CoreModules</group_name>
                <group_id>1</group_id>
                <Member1>
                    <name>TRUE</name>
                    <path>1</path>
                    <priority>200</priority>
                    <critical>TRUE</critical>
                    <retry_cnt>1</retry_cnt>
                    <arguments></arguments>
                    <logging_mask></logging_msk>
                    <restart>group</restart>
                    <shudown_critical></shudown_critical>
                    <shudown_wait_time></shudown_wait_time>
                </Member1>
                <Member2>
                    <name>TRUE</name>
                    <path>1</path>
                    <priority>200</priority>
                    <critical>TRUE</critical>
                    <retry_cnt>1</retry_cnt>
                    <arguments></arguments>
                    <logging_mask></logging_msk>
                    <restart>me</restart>
                    <shudown_critical></shudown_critical>
                    <shudown_wait_time></shudown_wait_time>
                </Member2>
            </Group1>
        </Launch>
    </SystemManager>
</Configuration>
*/

//******************************************************************************
//* CTimeSpan class                                                            *
//*                                                                            *
class CTimeSpan {
 public:
  CTimeSpan(): initial(0), final(0), delta(0) {}
  CTimeSpan(UI_64 f_initial)  // NOLINT
             : initial(f_initial), final(0), delta(0) {}
  ~CTimeSpan() {}
  /*
   * Set Initial time, reset Final and Delta to zero, return time
   */
  UI_64 Begin();
  /*
   * Set Final time, calculate Delta time, return time
   */
  UI_64 End();
  UI_64 GetInitial();
  UI_64 GetFinal();
  UI_64 GetDelta();
  UI_64 GetTimeMilliseconds();

 private:
  UI_64   initial;
  UI_64   final;
  UI_64   delta;
};  // End of class CTimeSpan
//******************************************************************************


typedef enum {
  NotStopped,
  StoppedByModuleStopComplete,
  StoppedByModuleStopFail,
  StoppedByGroupStopTimeout,
  StoppedByCriticalAppsTimeout,
  StoppedByModulePreStopComplete,
  StoppedByModulePreStopFail,
  StoppedByModuleBackgroundStopComplete,
  StoppedByModuleBackgroundStopFail
} EStopReasonType;

typedef enum {
  NotStarted,
  StartedByModuleStartComplete,
  StartedByModuleStartFail,
  StartedByModulePreStartComplete,
  StartedByModulePreStartFail,
  StartedByModuleBackgroundStartComplete,
  StartedByModuleBackgroundStartFail
} EStartReasonType;

//******************************************************************************
//* CTimeEvent class                                                           *
//*                                                                            *
template <typename EEventType>
class CTimeEvent : public CTimeSpan {
 public:
  CTimeEvent() {
      bzero(&m_eventReason, sizeof(m_eventReason));
  }
  ~CTimeEvent() {}

  VOID            SetReason(EEventType f_eventReason)
                  { m_eventReason = f_eventReason; }
 private:
  EEventType       m_eventReason;
};  // End of class CTimeEvent
//******************************************************************************

typedef enum {
  NotRelaunched,
  RelaunchSafe,
  RelaunchErr
} ERelaunchStatusType;


typedef  CTimeEvent<EStartReasonType> CStartReason;
typedef  CTimeEvent<EStopReasonType>  CStopReason;
typedef CHAR TUserNameBuffer[128];  // TODO(yaomingwu): Stuff. Does Posix define a useful
                                    // constant representing max length of user id?

class ModuleLaunchParams {
 public:
  // Move to private to prevent direct access
  // SMModuleState        module_state;
  //
  SS_String               name;
  SS_String               path;
  SS_String               arguments;
  SS_String               restart;
  UI_32                   configuration_priority;
  int                     cpu_assign;
  UI_32                   previous_priority;
  UI_32                   current_priority;
  BOOL                    critical;
  UI_32                   retry_cnt;
  SS_String               logging_msk_str;
  BOOL                    is_start_required;
  BOOL                    shutdown_critical;
  UI_32                   shutdown_wait_time;
  UI_32                   fast_shutdown_wait_time;
  SI_32                   pid;
  HANDLE                  hsession;
  UI_32                   relaunch_count;
  ERelaunchStatusType     relaunch_status;
  UI_32                   group_id;
  TUserNameBuffer         unix_user_name;

  BOOL                    m_serviceAvailability;  // Not set because it does not watch Availability for speeding up start up process.

  CStartReason            m_startReason;
  CStopReason             m_stopReason;

  ModuleLaunchParams();
  ~ModuleLaunchParams();
  SMModuleState           GetModuleState(void);
  BOOL                    IsModuleState(SMModuleState f_moduleState);
  BOOL                    IsModuleConnected() const;
  VOID                    SetModuleState(SMModuleState f_moduleState, BOOL f_bLog = TRUE);
  std::string             ModuleStateStr(void);

  SMModuleDebugDumpState  GetModuleDebugDumpState(void);
  std::string             GetModuleDebugDumpStateStr(void);
  VOID                    SetModuleDebugDumpState(SMModuleDebugDumpState f_moduleDebugDumpState, BOOL f_bLog = TRUE);
  EFrameworkunifiedStatus SendMsgAndUpdateState(
                            const UI_32 f_iCmd, const T_SS_SM_START_DataStructType* const f_startData);
  EFrameworkunifiedStatus SendMsgAndUpdateState(T_SS_SM_STOP_DataStruct const* f_stopData);

  EFrameworkunifiedStatus GetPriority(UI_32& f_Priority);  // NOLINT
  EFrameworkunifiedStatus SetPriority(UI_32 f_Priority);

  BOOL                    IsAGLUnit(void) const { return is_agl_unit; }
  BOOL                    IsAGLResetHistoryDisable(void) const { return disable_agl_resethistory; }
  BOOL                    IsNonAGLResetHistoryDisable(void) const { return disable_nonagl_resethistory; }
  void                    SetAGLUnit(BOOL f_is_agl_unit) { is_agl_unit = f_is_agl_unit; }
  void                    SetAGLResetHistoryDisable(BOOL f_disable_agl_resethistory) {
    disable_agl_resethistory = f_disable_agl_resethistory;
  }
  void                    SetNonAGLResetHisoryDisable(BOOL f_disable_nonagl_resethistory) {
    disable_nonagl_resethistory = f_disable_nonagl_resethistory;
  }

 private:
  SMModuleState                    m_moduleState;
  SMModuleStateStrMapType          m_ModuleStateStrMap;
  SMModuleDebugDumpState           m_ModuleDebugDumpState;
  SMModuleDebugDumpStateStrMapType m_ModuleDebugDumpStateStrMap;
  BOOL                             is_agl_unit;
  BOOL                             disable_agl_resethistory;
  BOOL                             disable_nonagl_resethistory;
};

typedef std::list<ModuleLaunchParams> ModuleLaunchList;
typedef ModuleLaunchList::iterator ModuleLaunchListIter;

typedef struct _GroupLaunchInfo {  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  BOOL                launch_complete;
  BOOL                start_complete;
  BOOL                stop_complete;
  SS_String           name;
  UI_32               id;
  UI_32               grp_launch_wait;
  UI_32               grp_wait_for_trigger;
  ModuleLaunchList    modules;
} GroupLaunchInfo;

typedef std::list<GroupLaunchInfo> GroupLaunchList;
typedef GroupLaunchList::iterator GroupLaunchListIter;

typedef std::map<UI_32, GroupLaunchInfo> GroupLaunchMap;
typedef GroupLaunchMap::iterator GroupLaunchMapIter;

typedef std::list<GroupLaunchList> GroupList;
typedef GroupList::iterator GroupListIter;

// Process Binary Name (key) to ModuleLaunchList iterator (ModuleLaunchParams).
typedef std::map<std::string, ModuleLaunchListIter>ProcessNameMap;
typedef ProcessNameMap::iterator ProcessNameMapIter;

// Config Parameters specific to Heart Beat Thread
typedef struct _HBConfigParams {
  SI_32 MaxHeartBeatRetryCount;           /// Retry heart beat check before error report is sent to SM
  SI_32 ApplicationHeartBeatIntervalInitial;  /// Initial Timer interval
                                              /// between two heart beat requests & response check
  SI_32 ApplicationHeartBeatIntervalRepeat;  /// Repeat Timer interval between two heart beat requests & response check
} HBConfigParameters;

typedef  struct _CDConfigParams {
  BOOL CrashDetectorEnabled;
} CDConfigParameters;

typedef  struct _SLMConfigParams {  // SLM - System Low Memory
  SI_32 SLMTimerValue;
  SI_32 SLMMaxRetryCount;
  SI_32 SLMThresholdValue;
  SI_32 SLMSystemmanagerLogIntervalMs;
} SLMConfigParameters;

typedef struct _UserMModeConfigParams {
  BOOL IsUserModeNotificationABOOL;
} UserModeConfigParameters;

typedef struct _ModuleConnectTimeConfigParams {
  SI_32 ModuleConnectionNumTimesToCheck;
  SI_32 ModuleConnectionTimeOutSec;
  SI_32 ModuleStartRespTimeOutSec;
} ModuleConfigParams;

typedef struct _CriticalAppsMaxShutdownTime {
  SI_32 CriticalAppsMaxShutdownTimeFastSleep;
  SI_32 CriticalAppsMaxShutdownTimeNormalReset;
} CriticalAppsMaxShutdownTime;

typedef struct _ConfigurationData {
  // Parameters specific to System Manager

  // Parameters specific to Heart Beat Thread
  HBConfigParameters HBConfig;

  // Parameters specific to Launcher Thread

  // Parameters specific to Crash Detector Thread
  CDConfigParameters CDConfig;

  // Parameters specific to System Low Memory

  // Parameters specific to Sys Low Memory Thread
  SLMConfigParameters SLMConfig;

  // Parameters specific to User Mode Notification Data Structure
  UserModeConfigParameters  UMConfig;

  // Parameters specific to Module connection timeout with SM
  ModuleConfigParams MCConfig;

  // Parameters specific to Critical Apps Max shutdown time
  CriticalAppsMaxShutdownTime CAMSTConfig;
} ConfigurationData;

typedef std::vector<UI_32> LaunchOrderedVector;
typedef LaunchOrderedVector::iterator LaunchOrderedIter;

class SMConfigParams {
 public:
  SMConfigParams();
  virtual ~SMConfigParams();

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
  BOOL LoadSMConfigParameters(ConfigurationData& f_ConfigParams,  // NOLINT
                              SS_String          f_FileName);

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
  BOOL GetSMConfigInformation(ConfigurationData& f_ConfigParams,  // NOLINT
                              SS_String          f_FileName);

  ////////////////////////////////////////////////////////////////////////////////////////////
  ///  PrintConfigInfo
  ///  \brief Print the configuration data stored in config structure
  ///
  /// \param [in] configata
  ///         ConfigurationData & - Ref to structure that get populated.
  ///
  /// \return VOID
  ////////////////////////////////////////////////////////////////////////////////////////////
  VOID PrintConfigInfo(ConfigurationData const& f_ConfigParams);
};

class SysMgrConfiguration {
 public:
  ////////////////////////////////////////////////////////////////////////////////////////////
  ///  SysMgrConfiguration
  ///  \brief
  ///
  /// \return NA
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  SysMgrConfiguration();

  ////////////////////////////////////////////////////////////////////////////////////////////
  ///  ~SysMgrConfiguration
  ///  \brief
  ///
  /// \return NA
  ////////////////////////////////////////////////////////////////////////////////////////////
  virtual ~SysMgrConfiguration();

//  ////////////////////////////////////////////////////////////////////////////////////////////
//  ///  LoadParameters
//  ///  \brief
//  ///
//  /// \param [in] groups
//  ///         GroupLaunchList & - Ref to GroupLaunchList that get populated.
//  ///
//  /// \return BOOL
//  ///         BOOL - TRUE or FALSE
//  ////////////////////////////////////////////////////////////////////////////////////////////
//  BOOL LoadParameters(GroupLaunchMap & groups_map);

  ////////////////////////////////////////////////////////////////////////////////////////////
  ///  LoadParametersCfg
  ///  \brief
  ///
  /// \param [in] groups
  ///         GroupLaunchList & - Ref to GroupLaunchList that gets populated.
  ///         ProcessNameMap  & - Ref to ProcessNameMap  that gets populated.
  ///         LaunchOrderedVector  & - Ref to LaunchOrderedVector  that gets populated.
  ///
  /// \param [in] f_launchOrderName   - launch order name.
  ///
  /// \return BOOL
  ///         BOOL - TRUE or FALSE
  ////////////////////////////////////////////////////////////////////////////////////////////
  BOOL LoadParametersCfg(GroupLaunchMap&      groups_map,  // NOLINT
                         ProcessNameMap&      f_processNameMap,  // NOLINT
                         LaunchOrderedVector& f_OrderList,  // NOLINT
                         SS_String&           f_launchOrderName,  // NOLINT
                         SS_String&           f_launchCfgFn);  // NOLINT


  ////////////////////////////////////////////////////////////////////////////////////////////
  ///  PrintGroupInfo
  ///  \brief Print method for GroupLaunchInfo only prints the group info not the module info
  ///
  /// \param [in] refGrpInfo
  ///         GroupLaunchInfo & - Ref to a GroupLaunchInfo object that will be printed.
  ///
  /// \return VOID
  ////////////////////////////////////////////////////////////////////////////////////////////
  VOID PrintGroupInfo(GroupLaunchInfo& refGrpInfo);  // NOLINT

  ////////////////////////////////////////////////////////////////////////////////////////////
  ///  PrintModuleInfo
  ///  \brief Print method for a Module's Params.
  ///
  /// \param [in] refMbrInfo
  ///         ModuleLaunchParams & - Ref to ModuleLaunchParams object that will be printed.
  ///
  /// \return VOID
  ////////////////////////////////////////////////////////////////////////////////////////////
  VOID PrintModuleInfo(ModuleLaunchParams& refMbrInfo);  // NOLINT

  ////////////////////////////////////////////////////////////////////////////////////////////
  ///  PrintAllInfo
  ///  \brief Print all the information in the GroupLaunchList.
  ///
  /// \param [in] refGrpList
  ///         GroupLaunchList & - Ref to GroupLaunchList object that will be printed.
  ///
  /// \return VOID
  ////////////////////////////////////////////////////////////////////////////////////////////
  VOID PrintAllInfo(GroupLaunchMap& refGrpMap);  // NOLINT

 protected:
  // define all the configuration parameters that will be used to get data

  CNSConfigReader * l_pReaderCfg;

  SysMgrConfiguration(const SysMgrConfiguration&) : l_pReaderCfg(NULL) {}
  SysMgrConfiguration & operator = (const SysMgrConfiguration &);

 private:
  BOOL ParseBoolParameter(PCSTR f_value, BOOL f_default = FALSE) const;
};

#endif  // SS_SM_CONFIG_H_  // NOLINT
