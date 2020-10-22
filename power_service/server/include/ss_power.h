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
/// \ingroup  tag_PowerService
/// \brief    Handles Power Service Business logic
///
///////////////////////////////////////////////////////////////////////////////
#ifndef POWER_SERVICE_SERVER_INCLUDE_SS_POWER_H_
#define POWER_SERVICE_SERVER_INCLUDE_SS_POWER_H_

#include <system_service/ss_power_service.h>
#include <system_service/ss_power_service_protocol.h>
#include <system_service/ss_string_maps.h>
#include <system_service/ss_templates.h>
#include <native_service/frameworkunified_types.h>
#include <map>
#include <vector>
#include <string>
#include <exception>

#include "ss_power_session.h"
#include "ss_power_config.h"
#include "ss_power_state_machine.h"
#include "ss_power_powerservicelog.h"

typedef enum _SS_PSState {
  SS_PS_READY_TO_WAKEUP = 0x01,
  SS_PS_WAKEUP_INITIATED,
  SS_PS_WAKEUP_COMPLETE,
  SS_PS_POWER_ON_COMPLETE,
  SS_PS_POWER_OFF_INITIATED,
  SS_PS_POWER_OFF_COMPLETE,
  SS_PS_SHUTDOWN_INITIATED,
  SS_PS_SHUTDOWN_COMPLETE
  // This is not real state, used only to know total states
  ,
  SS_PS_STATE_MAX
} SS_PSState;

typedef struct {
  std::string m_cmd;
  UI_64 m_time;
  std::string m_sender;
} Ts_cmdHist;

typedef std::vector<Ts_cmdHist> cmdHist;
typedef cmdHist::iterator cmdHistIter;

class Power {
 public:
  Power();
  virtual ~Power();
  EFrameworkunifiedStatus Initialize(HANDLE h_app);
  static Power & GetInstance();

  /// Session Request Handlers
  EFrameworkunifiedStatus OnOpenSession(HANDLE h_app);
  EFrameworkunifiedStatus OnCloseSession(HANDLE h_app);

  /// Session Response\Ack Handlers
  EFrameworkunifiedStatus OnWakeUpComplete(HANDLE h_app);
  EFrameworkunifiedStatus OnShutdownComplete(HANDLE h_app);
  EFrameworkunifiedStatus OnPrintConnections(HANDLE h_app);
  EFrameworkunifiedStatus OnPrintStack(HANDLE h_app);

  /// SSM Handlers

  /// Supervisor Handlers
  EFrameworkunifiedStatus OnPowerRequestMsg(HANDLE h_app);
  EFrameworkunifiedStatus OnSetPowerState(HANDLE h_app);
  EFrameworkunifiedStatus OnSetCommWakeUp(HANDLE h_app);
  EFrameworkunifiedStatus OnSetCommSleep(HANDLE h_app);
  EFrameworkunifiedStatus OnShutdownRequestMsg(HANDLE h_app);

  EFrameworkunifiedStatus OnPowerOnOffNotification(HANDLE h_app);
  EFrameworkunifiedStatus OnStartModules(HANDLE h_app);
  EFrameworkunifiedStatus OnShutdownModules(HANDLE h_app);

  EFrameworkunifiedStatus OnHysteresisTimeout(HANDLE h_app);

  // System Handlers
  EFrameworkunifiedStatus OnSystemLaunchComplete(HANDLE h_app);
  EFrameworkunifiedStatus OnSystemShutdownComplete(HANDLE h_app);
  EFrameworkunifiedStatus OnWakeUpTimeout(HANDLE h_app);
  EFrameworkunifiedStatus OnShutdownTimeout(HANDLE h_app);

  // Get Result query from power test client
  EFrameworkunifiedStatus OnCurrentPowerStateQuery(HANDLE h_app);

  EFrameworkunifiedStatus OnSetVoltageState(HANDLE h_app);
  EFrameworkunifiedStatus OnSetCrankState(HANDLE h_app);

  EFrameworkunifiedStatus OnSystemModeInfoRequest(HANDLE h_app);
  EFrameworkunifiedStatus OnSystemModeInfoResponse(HANDLE h_app);

  EFrameworkunifiedStatus OnInitCompReport(HANDLE h_app);

  EFrameworkunifiedStatus OnSystemMgrConnectionEstablished(HANDLE h_app);

  //
  // Start Confirmation callback functions
  //
  EFrameworkunifiedStatus OnSendStartupConfirmationRequest(HANDLE h_app);
  EFrameworkunifiedStatus OnSendStartupConfirmationResponse(HANDLE h_app);

  //
  // User Mode Protocol callback functions
  //
  EFrameworkunifiedStatus OnUserModeResponse(HANDLE h_app);

  //
  // Shutdown Condition Notification Protocol function
  //
  EFrameworkunifiedStatus OnPublishShutdownPopupRequest(HANDLE h_app);

  //
  // Power Popup Notification Protocol function
  //
  EFrameworkunifiedStatus OnPublishPowerPopupRequest(HANDLE h_app);

  //
  // HeartBeat Protocol callback functions
  //
  EFrameworkunifiedStatus On_CWORD56_HeartBeatRequest(HANDLE h_app);
  EFrameworkunifiedStatus OnSM_CWORD56_HeartBeatResponse(HANDLE h_app);

  //
  // Hard Reset Protocol callback functions
  //
  EFrameworkunifiedStatus OnCpuResetRequest(HANDLE h_app);

  //
  // Remote Data Reset Protocol callback functions
  //
  EFrameworkunifiedStatus OnRemoteDataResetRequest(HANDLE h_app);
  EFrameworkunifiedStatus OnRemoteDataResetResponse(HANDLE h_app);

  cmdHist m__CWORD56_RepHist;
  cmdHistIter m__CWORD56_RepIter;
  cmdHist m_PubCmdHist;
  cmdHistIter m_PubHistIter;
  cmdHist m_ErrHist;
  cmdHistIter m_ErrHistIter;
  cmdHist m_VCmdHist;
  cmdHistIter m_VHistIter;

  void SSPowerDebugDump(HANDLE h_app);

 private:
#define LOG_POWERSERVICELOG_CNT(caseid, Cnt) \
      FRAMEWORKUNIFIEDLOG_CNT(ZONE_INFO, Cnt, 0); \
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " case " #caseid \
        ": FRAMEWORKUNIFIEDLOG_CNT(ZONE_INFO, " #Cnt ", 0)");

#define LOG_POWERSERVICELOG_EVT(caseid, Evt, Data0) \
      FRAMEWORKUNIFIEDLOG_EVT(ZONE_INFO, Evt, 1, Data0); \
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " case " #caseid \
        ": FRAMEWORKUNIFIEDLOG_EVT(ZONE_INFO, " #Evt ", " #Data0 ")");

#define LOG_STATUS_IF_ERRORED_PWR_SM_WITH_HIST_LOGGING(l_eStatus, pStr)          \
    if (eFrameworkunifiedStatusOK != l_eStatus) {                  \
        LOG_ERROR(pStr);                               \
        SetCmdHist(pStr, m_ErrHist, m_ErrHistIter, GetStr(static_cast<EFrameworkunifiedStatus>(l_eStatus)).c_str()); \
    }  // End of #define LOG_STATUS_IF_ERRORED(l_eStatus,pStr)

#define SS_PWR_LOG_HIST(cmd, hist, it, sender, l_eStatus) \
    if (eFrameworkunifiedStatusOK == l_eStatus) {             \
        SetCmdHist((static_cast< std::string> (cmd)), (static_cast<cmdHist&> (hist)), \
                   (static_cast<cmdHistIter&> (it)), (static_cast< std::string> (sender)));      \
    }
  //**************************************************************************
  // Counter Logging enums & typedefs                                        *
  //                                                                         *
  //  Counter Event IDs                                                      *
  typedef enum {                                                          // *
    CT_Startup_CanSignal = 0x0114,                                   // *
    CT_Startup_DiagMsg = 0x0118,                                   // *
                                                                   // *
    CT_Shutdown_CanSignal = 0x0121,                                   // *
    CT_Shutdown_Timeout = 0x0124,                                   // *
    CT_Shutdown_Temperature = 0x0125,                                   // *
    CT_Shutdown_Error = 0x0126,                                   // *
    CT_Shutdown_Voltage = 0x0127                                    // *
  } EPWR_COUNTER_EVENT_ID_TYPE;       // *
  //                                                                         *
  // End of Counter Logging enums & typedefs                                 *
  //**************************************************************************

  //**************************************************************************
  // Event Logging enums & typedefs                                          *
  //                                                                         *
  //  Event ID                                                               *
  typedef enum {                                                          // *
    // Startup Event IDs                                                   *
    Ei_StartUp_Cansignal = 0x0101,                           // *
    Ei_StartUp_HK_On = 0x0102,                           // *
    //                                0x0103 not specified              // *
    //                                0x0104 not specified              // *
    //                                 0x0105 System Svcs not responsible// *
    //                                0x0106 not specified              // *
    Ei_StartUp_ActivationLine = 0x0107,                           // *
    Ei_StartUp_DiagMsg = 0x0108,                           // *
    //                                                                     *
    // Shutdown Event IDs                                                  *
    Ei_shutdown_CANsignal = 0x0201,                           // *
    Ei_shutdown_HkOFF = 0x0202,                           // *
    //                                0x0203 not specified              // *
    //                                0x0204 not specified              // *
    Ei_shutdown_Temperature = 0x0205,                           // *
    Ei_shutdown_NetworkError = 0x0206,                           // *
    Ei_shutdown_Voltage = 0x0207                            // *
  } EPWR_LOGGING_EVENT_ID_TYPE;              // *
  //                                                                         *
  //  Ei_StartUp_CanSignal byte0 Event Detail                                *
  typedef enum {                                                          // *
    epseisuedIGNITION = 0x00,                             // *
    epseisuedHK_ON = 0x01,                             // *
    epseisuedDOOR_MODULES = 0x02,                             // *
    //                                    0x03 not specified              // *
    //                                    0x04 not specified              // *
    epseisuedPASS = 0x05,                             // *
    epseisuedIHTM = 0x06,                             // *
    epseisuedMPM = 0x07,                             // *
    epseisuedSNA = 0xFF                              // *
  } EPWR_LOGGING_EVT_DETAIL_SU_CAN_TYPE;                                  // *

  //
  // Ei_StartUp_HkOn
  typedef enum {
    epseisuedHK_ON_Key = 0x00
  } EPWR_LOGGING_EVT_DETAIL_SU_KEY_TYPE;

  //
  // Ei_StartUp_ActivationLine
  typedef enum {
    epseisuedACV_LINE = 0x00
  } EPWR_LOGGING_EVT_DETAIL_SU_LINE_TYPE;

  //                                                                         *
  //  Ei_StartUp_DiagMsg byte0 Event Detail                                  *
  typedef enum {                                                          // *
    epseisuedHU = 0x01,                                          // *
    epseisuedCAN = 0x02,                                          // *
  } EPWR_LOGGING_EVT_DETAIL_SU_DIAG_TYPE;                                 // *
  //                                                                         *
  //  Ei_ShutDown_CanSignal byte0 Event Detail                               *
  typedef enum {                                                          // *
    epseisdcsIGN_STATE = 0x00,                                          // *
    epseisdcsHK_ON = 0x01                                           // *
  } EPWR_LOGGING_EVT_DETAIL_SD_CAN_TYPE;                                  // *
  //                                                                         *
  //  Ei_ShutDown_Temperature byte0 Event Detail                             *
  typedef enum {                                                          // *
    epseisdtempECU_LOW = 0x00,                                    // *
    epseisdtempECU_HIGH = 0x01,                                    // *
  } EPWR_LOGGING_EVT_DETAIL_SD_TEMP_TYPE;                                 // *
  //                                                                         *
  //  Ei_ShutDown_NetworkError byte0 Event Detail                            *
  typedef enum {                                                          // *
    epseisdneHMI_CAN = 0x00,                                    // *
    epseisdneHU_CAN = 0x01                                     // *
  } EPWR_LOGGING_EVT_DETAIL_SD_NETWORK_ERROR_TYPE;                        // *
  //                                                                         *
  //  Ei_ShutDown_Voltage byte0 Event Detail                                 *
  typedef enum {                                                          // *
    epseisdnvoltBATTERY_CUT_OFF = 0x00,                                // *
    epseisdnvoltLIMP_HOME = 0x01,                                // *
    epseisdnvoltUNKNOWN = 0xFF                                 // *
  } EPWR_LOGGING_EVT_DETAIL_SD_VOLTAGE_TYPE;                              // *
  //                                                                         *
  // End of Event Logging enums & typedefs                                   *
  //**************************************************************************

  PowerSessionHandler m_oSessionHandler;
  PowerConfigParams m_tConfigData;
  PowerStateMachine m_oStateMachine;
  UI_32 m_aTimerIDs[15];
  wakeInfo m_WakeUpData;   /// WakeUp data received from SPM
  SS_PSState m_PowerState;
  ePowerSrvVoltageStates m_VoltageState;
  ePowerSrvCrankStates m_CrankState;
  UI_32 m_MaxShutdownTimeout;
  std::map<SS_PSState, std::string> m_PPStateStrMap;

  // Notify Module on Voltage change detected.
  EFrameworkunifiedStatus PublishVoltageStateChange(HANDLE h_app);
  EFrameworkunifiedStatus ConstructPwrStateResponse(CHAR *f_MessageResponse);
  EFrameworkunifiedStatus AddStateInformationToResponse(CHAR *f_MessageResponse);
  EFrameworkunifiedStatus AddVoltageInformationToResponse(CHAR *f_MessageResponse);
  EFrameworkunifiedStatus AddCrankInformationToResponse(CHAR *f_MessageResponse);
  EFrameworkunifiedStatus RegisterAllCallbacksAndNofitications(HANDLE h_app);
  VOID SetPowerServiceState(SS_PSState f_NewState);
  EFrameworkunifiedStatus ValidateUserModeMessage(HANDLE h_app,
      EPWR_USER_MODE_TYPE &l_eUserModeState);  // NOLINT (runtime/references)

  pthread_mutex_t pwr_hist_mutex;

  void SetCmdHist(std::string cmd,
                  cmdHist &hist,  // NOLINT (runtime/references)
                  cmdHistIter &it,  // NOLINT (runtime/references)
                  std::string sender);
};

#endif  // POWER_SERVICE_SERVER_INCLUDE_SS_POWER_H_
