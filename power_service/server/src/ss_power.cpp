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

#include "ss_power.h"
#include <inttypes.h>
#include <system_service/ss_system_manager_notifications.h>
#include <system_service/ss_templates.h>
#include <system_service/ss_services.h>
#include <system_service/ss_sm_client_if.h>
#include <system_service/ss_system_manager_if.h>
#include <system_service/ss_system_manager_protocol.h>
#include <system_service/ss_power_service_protocol.h>
#include <system_service/ss_power_service_notifications.h>
#include <system_service/ss_power_service.h>
#include <system_service/ss_power_service_notifications_local.h>
#include <system_service/ss_power_service_local.h>
#include <native_service/frameworkunified_timer.h>
#include <native_service/frameworkunified_application.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/ns_np_service_protocol.h>
#include <other_service/PosixBasedOS001ClockCycleApi.h>
#include <utility>
#include <string>

#include "ss_power_powerservicelog.h"

using namespace std;  // NOLINT (build/namespaces)

static Power g_PowerSubsystem;

/**
 * @brief
 *
 * @param
 * @param
 *
 * @return
 */
template<typename C, eFrameworkunifiedStatus (C::*M)(HANDLE)> EFrameworkunifiedStatus PowerCallback(
    HANDLE h_app) {

  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;

  C * pObj = static_cast<C *>(&g_PowerSubsystem);

  if (pObj) {
    l_eStatus = (pObj->*M)(h_app);
  }

  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////
/// GetInstance
/// Return global instance of Power object. Power is singleton class
///
//////////////////////////////////////////////////////////////////////
Power & Power::GetInstance() {
  return g_PowerSubsystem;
}

///////////////////////////////////////////////////////////////////////
/// Power
/// Constructor of Power class
///
//////////////////////////////////////////////////////////////////////
Power::Power()
    : m__CWORD56_RepHist(SS_PWR__CWORD56__REP_HIST_SIZE),
      m_PubCmdHist(SS_PWR_PUB_CMD_HIST_SIZE),
      m_ErrHist(SS_PWR_ERR_HIST_SIZE),
      m_VCmdHist(SS_PWR_V_HIST_SIZE),
      m_PowerState(SS_PS_READY_TO_WAKEUP),
      m_VoltageState(epsvsINVALID),
      m_CrankState(epscsINVALID),
      m_MaxShutdownTimeout(0) {
  m_PPStateStrMap[SS_PS_READY_TO_WAKEUP] = "SS_PS_READY_TO_WAKEUP";
  m_PPStateStrMap[SS_PS_WAKEUP_INITIATED] = "SS_PS_WAKEUP_INITIATED";
  m_PPStateStrMap[SS_PS_WAKEUP_COMPLETE] = "SS_PS_WAKEUP_COMPLETE";
  m_PPStateStrMap[SS_PS_POWER_ON_COMPLETE] = "SS_PS_POWER_ON_COMPLETE";
  m_PPStateStrMap[SS_PS_POWER_OFF_INITIATED] = "SS_PS_POWER_OFF_INITIATED";
  m_PPStateStrMap[SS_PS_POWER_OFF_COMPLETE] = "SS_PS_POWER_OFF_COMPLETE";
  m_PPStateStrMap[SS_PS_SHUTDOWN_INITIATED] = "SS_PS_SHUTDOWN_INITIATED";
  m_PPStateStrMap[SS_PS_SHUTDOWN_COMPLETE] = "SS_PS_SHUTDOWN_COMPLETE";
  m_PPStateStrMap[SS_PS_STATE_MAX] = "SS_PS_STATE_MAX";

  m__CWORD56_RepIter = m__CWORD56_RepHist.begin();
  m_PubHistIter = m_PubCmdHist.begin();
  m_ErrHistIter = m_ErrHist.begin();
  m_VHistIter = m_VCmdHist.begin();
  bzero(&m_WakeUpData, sizeof(m_WakeUpData));
}

///////////////////////////////////////////////////////////////////////
/// ~Power
/// Destructor of Power class
///
//////////////////////////////////////////////////////////////////////
Power::~Power() {  // LCOV_EXCL_START 14 Resident process, not called by NSFW
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
}
// LCOV_EXCL_STOP 14 Resident process, not called by NSFW

///////////////////////////////////////////////////////////////////////
/// Initialize
///
///
//////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus Power::Initialize(HANDLE h_app) {
  EFrameworkunifiedStatus l_eStatus;
  BOOL l_SpecifiedCfgLoaded;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  SetPowerServiceState(SS_PS_READY_TO_WAKEUP);

  pthread_mutex_init(&pwr_hist_mutex, NULL);

  /// Open and parse the configuration data for Power
  const CHAR configFileName[] = "/usr/agl/conf/BS/ss/power_service/rodata/gpf_ss_ps_config.cfg";

  PowerConfiguration config(configFileName);
  l_SpecifiedCfgLoaded = config.LoadParameters(m_tConfigData);
  if (FALSE == l_SpecifiedCfgLoaded) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
    FRAMEWORKUNIFIEDLOG(
        ZONE_ERR, __FUNCTION__,
        "Reading configuration from file: %s failed. Using default parameters.",
        configFileName);
    config.LoadDefaultParameters(m_tConfigData);
  }

  config.PrintConfigInfo(m_tConfigData);

  /// init PowerStateMachine
  m_oStateMachine.initialize(h_app, m_tConfigData);

  // Register all callbacks to dispatcher and notifications
  l_eStatus = RegisterAllCallbacksAndNofitications(h_app);
  LOG_STATUS_IF_ERRORED_PWR_SM_WITH_HIST_LOGGING(
      l_eStatus, "RegisterAllCallbacksAndNofitications()");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////
/// RegisterAllCallbacksAndNofitications
///
///
//////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus Power::RegisterAllCallbacksAndNofitications(HANDLE h_app) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedRegisterServiceAvailabilityNotification(h_app, szNTFY_PowerAvailability))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR(
        "FrameworkunifiedRegisterServiceAvailabilityNotification( "szNTFY_PowerAvailability" )");
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
    return l_eStatus;
    // LCOV_EXCL_STOP 4: NSFW error case.
  } else {
    l_eStatus = FrameworkunifiedPublishServiceAvailability(h_app, FALSE);
    if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
      // LCOV_EXCL_START 4: NSFW error case.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      LOG_ERROR("FrameworkunifiedPublishServiceAvailability()");
    }
    // LCOV_EXCL_STOP 4: NSFW error case.

    SS_PWR_LOG_HIST("FrameworkunifiedPublishServiceAvailability()", m_PubCmdHist,
                    m_PubHistIter, "", l_eStatus);
  }

  // Attach Notification callbacks to dispatcher
  FrameworkunifiedNotificationsList
  ss_power_notifications[] = {
    { szNTFY_PowerLVI1, sizeof(PowerSrvLVIStatus), eFrameworkunifiedStateVar}
    , {szNTFY_PowerLVI2, sizeof(PowerSrvLVIStatus), eFrameworkunifiedStateVar}
    , {szNTFY_PowerLevel, sizeof(PowerSrvLevelType), eFrameworkunifiedStateVar}
    , {szNTFY_ShutdownPopup, sizeof(EPWR_SHUTDOWN_POPUP_TYPE), eFrameworkunifiedStateVar}
    , {szNTFY_PowerPopup, sizeof(EPWR_POWER_POPUP_TYPE), eFrameworkunifiedStateVar}
  };
  //   Register Notifications
  if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedNPRegisterNotifications(h_app, ss_power_notifications, _countof(ss_power_notifications)))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("FrameworkunifiedNPRegisterNotifications(ss_power_notifications)");
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
    return l_eStatus;
  }
  // LCOV_EXCL_STOP 4: NSFW error case.
  // Subscribe and attach call back for notification
  if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedSubscribeNotificationWithCallback(h_app, NTFY_SSSystemMgrPowerOnOff, PowerCallback<Power, &Power::OnPowerOnOffNotification>))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("FrameworkunifiedSubscribeNotificationWithCallback()");
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
    return l_eStatus;
  }
  // LCOV_EXCL_STOP 4: NSFW error case.

//*************************

/// < Attach to the following session handles.
  FrameworkunifiedProtocolCallbackHandler ss_power_session_req_handlers[] = {
      /// session handles.
          { PROTOCOL_OPEN_SESSION_REQ, PowerCallback<Power,
              &Power::OnOpenSession> }, { PROTOCOL_CLOSE_SESSION_REQ,
              PowerCallback<Power, &Power::OnCloseSession> }, {
              SS_POWER_STATE_CHANGE_REQ, PowerCallback<Power,
                  &Power::OnSetPowerState> }, { SS_POWER_POWER_REQUEST_MSG,
              PowerCallback<Power, &Power::OnPowerRequestMsg> }, {
              SS_POWER_COMM_WAKEUP,
              PowerCallback<Power, &Power::OnSetCommWakeUp> }, {
              SS_POWER_SHUTDOWN_REQ,
              PowerCallback<Power, &Power::OnSetCommSleep> }, {
              SS_POWER_SHUTDOWN_REQUEST_MSG, PowerCallback<Power,
                  &Power::OnShutdownRequestMsg> }

          /// Wake-up / Shutdown Protocol commands for System Manager.
          , { SS_SM_WAKEUP_MODULES_CMPL_RSPN, PowerCallback<Power,
              &Power::OnWakeUpComplete> }, { SS_SM_SHUTDOWN_MODULES_CMPL_RSPN,
              PowerCallback<Power, &Power::OnShutdownComplete> }
          //
          , { SS_POWER_PRINT_CONNECTIONS, PowerCallback<Power,
              &Power::OnPrintConnections> }, { SS_POWER_PRINT_STACK,
              PowerCallback<Power, &Power::OnPrintStack> }
          /// Set Voltage & Crank states from shadow?
          , { SS_POWER_VOLTAGE_STATE, PowerCallback<Power,
              &Power::OnSetVoltageState> }, { SS_POWER_CRANK_STATE,
              PowerCallback<Power, &Power::OnSetCrankState> }
          /// Current Power Service State inquiry from Test Client
          , { SS_POWER_CRNT_STATE_QUERY, PowerCallback<Power,
              &Power::OnCurrentPowerStateQuery> }, {
              SS_POWER_SYSTEM_MODE_INFO_REQ, PowerCallback<Power,
                  &Power::OnSystemModeInfoRequest> }, {
              SS_SM_SYSTEM_MODE_INFO_RSPN, PowerCallback<Power,
                  &Power::OnSystemModeInfoResponse> }
          /// InitComp report to SystemManager
          , { SS_POWER_INITCOMP_REP, PowerCallback<Power,
              &Power::OnInitCompReport> }
          //
          // Startup Confirmation Request ( from Power Shadow ) and
          // Startup Confirmation Response ( from System Manager )
          , { SS_POWER_FWD_START_CONFIRMATION_MSG_REQ, PowerCallback<Power,
              &Power::OnSendStartupConfirmationRequest> }, {
              SS_POWER_FWD_START_CONFIRMATION_MSG_RESP, PowerCallback<Power,
                  &Power::OnSendStartupConfirmationResponse> }
          //
          // User Mode Request ( from Power Shadow ) and
          // User Mode Response ( from System Manager )
          , { SS_SM_USER_MODE_SET_RESP, PowerCallback<Power,
              &Power::OnUserModeResponse> }

          //
          // Heartbeat Request ( from Power Shadow )
          , { SS_POWER_HEARTBEAT_REQ, PowerCallback<Power,
              &Power::On_CWORD56_HeartBeatRequest> }, { SS_SM__CWORD56__HEARTBEAT_RSPN,
              PowerCallback<Power, &Power::OnSM_CWORD56_HeartBeatResponse> }

          // System Manager to Power Services
          , { SS_SM_CPU_RESET_REQ, PowerCallback<Power,
              &Power::OnCpuResetRequest> }, { SS_SM_REMOTE_DATA_RESET_REQ,
              PowerCallback<Power, &Power::OnRemoteDataResetRequest> } };

  // Attach callback
  if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedAttachCallbacksToDispatcher(h_app, FRAMEWORKUNIFIED_ANY_SOURCE, ss_power_session_req_handlers, _countof(ss_power_session_req_handlers)))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("FrameworkunifiedAttachCallbacksToDispatcher( ss_power_session_req_handlers )");
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
    return l_eStatus;
  }
  // LCOV_EXCL_STOP 4: NSFW error case.

  //*************************

  // Publish szNTFY_PowerAvailable so that other services can start connecting via sessions.

  l_eStatus = RegisterSMSessionAckCallback(
      PowerCallback<Power, &Power::OnSystemMgrConnectionEstablished>);
  LOG_STATUS(l_eStatus, "RegisterSMSessionAckCallback()");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////
/// OnSetCommWakeUp
///
///
//////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus Power::OnSetCommWakeUp(HANDLE h_app) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  Pwr_ServiceSetInterface l_CanStatesData;

  // ReadMsg():                                                        *
  //     Check h_app ptr, msg size, msg reception, read msg if all ok.  *
  //     Report any errors found.                                      *
  //                                                                   *
  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg<Pwr_ServiceSetInterface>(h_app, l_CanStatesData))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("ReadMsg()");
    // LCOV_EXCL_STOP 4: NSFW error case.
  } else if (l_CanStatesData.data.commwake.state == epscnCANWAKEUP) {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
           "Received WakeUp from %s, Processing Ignored !!",
           FrameworkunifiedGetMsgSrc(h_app));
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////
/// OnSetCommSleep
///
///
//////////////////////////////////////////////////////////////////////
// LCOV_EXCL_START 200: can not be called from power class
EFrameworkunifiedStatus Power::OnSetCommSleep(HANDLE h_app) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus l_eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  Pwr_ServiceSetInterface l_CanStatesData;

  // ReadMsg():                                                        *
  //     Check h_app ptr, msg size, msg reception, read msg if all ok.  *
  //     Report any errors found.                                      *
  //                                                                   *
  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg<Pwr_ServiceSetInterface>(h_app, l_CanStatesData))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("ReadMsg()");  // LCOV_EXCL_LINE 4: NSFW error case.
  } else if (l_CanStatesData.data.commwake.state != epscnCANSLEEP) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "Error: %s: l_CanStatesData.data.commwake.state != epscnCANSLEEP",
           FrameworkunifiedGetMsgSrc(h_app));
    l_eStatus = eFrameworkunifiedStatusInvldParam;
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Received Sleep from %s",
           FrameworkunifiedGetMsgSrc(h_app));

    // Send to System Manager to WakeUp Modules
    l_eStatus = SendShutdownToSystemManager(&l_CanStatesData);
    if (l_eStatus != eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      LOG_ERROR("SendShutdownToSystemManager()");  // LCOV_EXCL_LINE 4: NSFW error case.
    } else {
      SetPowerServiceState(SS_PS_SHUTDOWN_INITIATED);
      LOG_SUCCESS("SendShutdownToSystemManager()");
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
// LCOV_EXCL_STOP 200: can not be called from power class

///////////////////////////////////////////////////////////////////////
/// SetPowerServiceState
///
///
//////////////////////////////////////////////////////////////////////
VOID Power::SetPowerServiceState(SS_PSState f_NewState) {
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Changing State from '%s' to '%s'",
         m_PPStateStrMap[m_PowerState].c_str(),
         m_PPStateStrMap[f_NewState].c_str());

  m_PowerState = f_NewState;
}

///////////////////////////////////////////////////////////////////////
/// OnPowerOnOffNotification
///
///
//////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus Power::OnPowerOnOffNotification(HANDLE h_app) {
  EFrameworkunifiedStatus l_eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  T_SS_SM_UserModeOnOffNotification_StructType l_userModeOnOffStruct;
  INTERFACEUNIFIEDLOG_RECEIVED_FROM(h_app);

  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg<T_SS_SM_UserModeOnOffNotification_StructType>(h_app, l_userModeOnOffStruct))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("ReadMsg()");
    // LCOV_EXCL_STOP 4: NSFW error case.
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
           " User Mode is '%s', User Mode Change Reason is '%s'",
           GetStr(l_userModeOnOffStruct.isUserModeOn).c_str(),
           GetStr(l_userModeOnOffStruct.userModeChangeReason).c_str());
    SetPowerServiceState(
        l_userModeOnOffStruct.isUserModeOn ?
            SS_PS_POWER_ON_COMPLETE : SS_PS_POWER_OFF_COMPLETE);
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////
/// ProcessSystemWakeUp
/// On receipt of this notification send wake up notification to
/// System Manager to wake up (START) all modules
///
//////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus Power::OnSetPowerState(HANDLE h_app) {
  EFrameworkunifiedStatus l_eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  Pwr_ServiceSetInterface l_serviceSetIf;

  // ReadMsg():                                                        *
  //     Check h_app ptr, msg size, msg reception, read msg if all ok.  *
  //     Report any errors found.                                      *
  //                                                                   *
  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg<Pwr_ServiceSetInterface>(h_app, l_serviceSetIf))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("ReadMsg()");
    // LCOV_EXCL_STOP 4: NSFW error case.
  } else {
    switch (l_serviceSetIf.data.wake.powerupType) {
      case epswsPWRON:
      case epswsPWROFF:
        m_WakeUpData = l_serviceSetIf.data.wake;

        FRAMEWORKUNIFIEDLOG(
            ZONE_INFO, __FUNCTION__, " Received Power %s from %s, level 0x%X",
            l_serviceSetIf.data.wake.powerupType == epswsPWRON ? "On" : "Off",
            FrameworkunifiedGetMsgSrc(h_app), m_WakeUpData.up.level);
        // Send to System Manager to WakeUp Modules
        l_eStatus = SendWakeUpToSystemManager(&m_WakeUpData);
        if (l_eStatus == eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
          SetPowerServiceState(SS_PS_WAKEUP_INITIATED);
          LOG_SUCCESS("SendWakeUpToSystemManager()");
        } else {
          // LCOV_EXCL_START 4: NSFW error case.
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          LOG_ERROR("SendWakeUpToSystemManager()");
          // LCOV_EXCL_STOP 4: NSFW error case.
        }
        break;
      default:
        l_eStatus = eFrameworkunifiedStatusInvldParam;
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               " Error: Invalid PowerOnOff State received: 0x%X",
               l_serviceSetIf.data.wake.powerupType);
        break;
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////
///
/// Session Request Handlers
///
//////////////////////////////////////////////////////////////////////

EFrameworkunifiedStatus Power::OnOpenSession(HANDLE h_app) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  INTERFACEUNIFIEDLOG_RECEIVED_FROM(h_app);
  l_eStatus = m_oSessionHandler.OpenSesion(h_app);

  /// < Test Code TODO: Remove ME!!!
  // m_oSessionHandler.Print();

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////
///
///
///
//////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus Power::OnCloseSession(HANDLE h_app) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  INTERFACEUNIFIEDLOG_RECEIVED_FROM(h_app);
  l_eStatus = m_oSessionHandler.CloseSession(h_app);
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////
///
///
///
//////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus Power::OnWakeUpComplete(HANDLE h_app) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  wakeInfo l_Wake;

  // ReadMsg():                                                        *
  //     Check h_app ptr, msg size, msg reception, read msg if all ok.  *
  //     Report any errors found.                                      *
  //                                                                   *
  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg<wakeInfo>(h_app, l_Wake))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("ReadMsg()");
    // LCOV_EXCL_STOP 4: NSFW error case.
  } else if (l_Wake.up.level == m_WakeUpData.up.level) {
    SetPowerServiceState(SS_PS_WAKEUP_COMPLETE);

    /// < send the state machine a wakeup event.
    m_oStateMachine.onEvent(h_app, m_oSessionHandler,
                            PowerStateMachine::epsmeWAKEUP);

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
           " Sending SS_POWER_STATE_CHANGE_RESP(%s) to PSM",
           l_Wake.powerupType == epswsPWRON ? "User_On" : "User_Off");

    // send Wake-up complete response to Supervisor
    m_oSessionHandler.SendToSupervisor(SS_POWER_STATE_CHANGE_RESP,
                                       sizeof(l_Wake), (PVOID) &l_Wake);
    SS_PWR_LOG_HIST("SS_POWER_STATE_CHANGE_RESP", m__CWORD56_RepHist, m__CWORD56_RepIter,
                    "SS_PS_WAKEUP_COMPLETE", l_eStatus);

    /// < Test Code TODO: Remove ME!!!
    // m_oSessionHandler.Print();
  } else {
    l_eStatus = eFrameworkunifiedStatusInvldParam;
    FRAMEWORKUNIFIEDLOG(
        ZONE_ERR,
        __FUNCTION__,
        "Error: Power WakeUp Threshold (%d) and SM WakeUp Threshold (%d) Mismatch",
        m_WakeUpData.up.level, l_Wake.up.level);
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////
///
///
///
//////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus Power::OnShutdownComplete(HANDLE h_app) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  SetPowerServiceState(SS_PS_SHUTDOWN_COMPLETE);

  m_oStateMachine.onEvent(h_app, m_oSessionHandler,
                          PowerStateMachine::epsmeSHUTDOWN);

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Sending SS_POWER_SHUTDOWN_RESP to PSM");
  // send Shutdown complete response to Power Shadow / Supervisor
  m_oSessionHandler.SendToSupervisor(SS_POWER_SHUTDOWN_RESP, 0, (PVOID) NULL);

  SS_PWR_LOG_HIST("SS_POWER_SHUTDOWN_RESP", m__CWORD56_RepHist, m__CWORD56_RepIter,
                  "SS_PS_SHUTDOWN_COMPLETE", l_eStatus);

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus Power::OnShutdownComplete(HANDLE h_app)

///////////////////////////////////////////////////////////////////////
///
///
///
//////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus Power::OnPrintConnections(HANDLE h_app) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  m_oSessionHandler.Print();
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////
///
///
///
//////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus Power::OnPrintStack(HANDLE h_app) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  m_oSessionHandler.Print();
  // m_oStateMachine.Print();
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////
///
///
///
//////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus Power::OnHysteresisTimeout(HANDLE h_app) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  m_oStateMachine.onEvent(h_app, m_oSessionHandler,
                          PowerStateMachine::epsmeLVI1_HYSTERESIS_TM_OUT);
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
///////////////////////////////////////////////////////////////////////
///
///
///
//////////////////////////////////////////////////////////////////////

EFrameworkunifiedStatus PowerStateMachine::initialize(HANDLE h_app,
                                         PowerConfigParams & refConfigParms) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  /// Set all hysteresis objects with the configuration data obtained from PowerService.xml
  m_oShutdownHysteresis.set(refConfigParms.shutdown);
  m_oLowVoltage1Hysteresis.set(refConfigParms.lvi1);
  m_oLowVoltage2Hysteresis.set(refConfigParms.lvi2);

  /// Copy the list of required modules.
  m_lstWakeupModules = refConfigParms.wakeup_modules;
  m_lstShutdownModules = refConfigParms.shutdown_modules;
  m_lstLvi2Modules = refConfigParms.lvi2_modules;

  m_hHysteresisTimer = FrameworkunifiedAttachTimerCallback(
      h_app, 0, 0, PowerCallback<Power, &Power::OnHysteresisTimeout>);
  if (NULL == m_hHysteresisTimer) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "Error: FrameworkunifiedAttachTimerCallback() returned NULL");
    l_eStatus = eFrameworkunifiedStatusInvldHandle;
  }
  // LCOV_EXCL_STOP 4: NSFW error case.

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////
///
///
///
//////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus PowerSessionHandler::OpenSesion(HANDLE h_app) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // check if the subscriber is already in map
  PwSessionIter iter = m_mapSessions.find(FrameworkunifiedGetMsgSrc(h_app));

  // the iter is set to the end then the subscriber is not in the map
  if (m_mapSessions.end() == iter) {
    string subscriber(FrameworkunifiedGetMsgSrc(h_app));
    PwSessionInfo newEntry;
    newEntry.frunning = FALSE;
    newEntry.hsession = FrameworkunifiedMcOpenSender(h_app, subscriber.c_str());
    newEntry.sz_name = subscriber;
    newEntry.sz_servicename = "";  // we will get this on the start complete.
    newEntry.esessiontype = epsstBASIC;
    newEntry.ui_groupid = 0;

    if (NULL != newEntry.hsession) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
      //
      // Set the Session Handle for Framework so FrameworkunifiedSendResponse won't error due to a null
      // session handle.
      //
      // LCOV_EXCL_BR_START 4: NSFW error case.
      if (eFrameworkunifiedStatusOK
          // [in] PCSTR - Name of the associated service name
          // [in] HANDLE - Session handle
          != (l_eStatus = FrameworkunifiedSetSessionHandle(h_app, subscriber.c_str(), newEntry.hsession))) {  // NOLINT[whitespace/line_length]
      // LCOV_EXCL_BR_STOP
        // LCOV_EXCL_START 4: NSFW error case.
        LOG_ERROR("FrameworkunifiedSetSessionHandle()");
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
        return l_eStatus;
        // LCOV_EXCL_STOP 4: NSFW error case.
      }

      OpenSessionAck openSessionAck;
      memset(&openSessionAck, 0, sizeof(openSessionAck));
      strcpy(openSessionAck.cSessionName, SERVICE_POWER);  // NOLINT (runtime/printf)
      openSessionAck.eStatus = eFrameworkunifiedStatusOK;
      openSessionAck.sessionId = 0;  /// Only one session handles all the heartbeat clients

      // Send Ack to subscriber
      l_eStatus = FrameworkunifiedSendMsg(newEntry.hsession, PROTOCOL_OPEN_SESSION_ACK,
                             sizeof(OpenSessionAck), (PVOID) &openSessionAck);
      char l_cBuf[100];
      snprintf(l_cBuf, sizeof(l_cBuf),
               "FrameworkunifiedSendMsg(%s, PROTOCOL_OPEN_SESSION_ACK)", subscriber.c_str());
      LOG_STATUS(l_eStatus, l_cBuf);
    }

    // If this is not a basic module then we will need to register some other protocols
    if (0 != FrameworkunifiedGetMsgLength(h_app)) {
      EPWR_SESSION_TYPE tOpenSessionReq;
      if (sizeof(EPWR_SESSION_TYPE) == FrameworkunifiedGetMsgLength(h_app)) {
        if (eFrameworkunifiedStatusOK
            == (l_eStatus = FrameworkunifiedGetMsgDataOfSize(h_app, (PVOID) &tOpenSessionReq,
                                                sizeof(EPWR_SESSION_TYPE)))) {
          switch (tOpenSessionReq) {
            case epsstBASIC: {
              FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
                     "Subscriber : %s , open a session using type: BASIC",
                     subscriber.c_str());
              FrameworkunifiedProtocolCallbackHandler ss_power_session_basic_handlers[] = { {
                  PROTOCOL_OPEN_SESSION_REQ, PowerCallback<Power,
                      &Power::OnOpenSession> }, { PROTOCOL_CLOSE_SESSION_REQ,
                  PowerCallback<Power, &Power::OnCloseSession> } };

              // Attach callback : Power Session Requests
              if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedAttachCallbacksToDispatcher(h_app, subscriber.c_str(), ss_power_session_basic_handlers, _countof(ss_power_session_basic_handlers)))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
                // LCOV_EXCL_START 4: NSFW error case.
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                LOG_ERROR(
                    "FrameworkunifiedAttachCallbacksToDispatcher(ss_power_session_basic_handlers)");
                FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
                return l_eStatus;
              }
              // LCOV_EXCL_STOP 4: NSFW error case.

              FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
                     "Subscriber '%s' opened a type 'BASIC' session",
                     subscriber.c_str());
            }
              break;
            case epsstSUPERVISOR: {
              FrameworkunifiedProtocolCallbackHandler ss_power_sprv_session_handlers[] = { {
                  PROTOCOL_OPEN_SESSION_REQ, PowerCallback<Power,
                      &Power::OnOpenSession> }, { PROTOCOL_CLOSE_SESSION_REQ,
                  PowerCallback<Power, &Power::OnCloseSession> }, {
                  SS_POWER_FWD_START_CONFIRMATION_MSG_REQ, PowerCallback<Power,
                      &Power::OnSendStartupConfirmationRequest> }, {
                  SS_POWER_STATE_CHANGE_REQ, PowerCallback<Power,
                      &Power::OnSetPowerState> }, { SS_POWER_SHUTDOWN_REQ,
                  PowerCallback<Power, &Power::OnShutdownRequestMsg> }
                  , { SS_POWER_SYSTEM_MODE_INFO_REQ, PowerCallback<Power,
                      &Power::OnSystemModeInfoRequest> }, {
                      SS_POWER_PUBLISH_SHUTDOWN_CONDITION_REQ, PowerCallback<
                          Power, &Power::OnPublishShutdownPopupRequest> }, {
                      SS_POWER_PUBLISH_POWER_POPUP_REQ, PowerCallback<Power,
                          &Power::OnPublishPowerPopupRequest> }, {
                      SS_POWER_POWER_REQUEST_MSG, PowerCallback<Power,
                          &Power::OnPowerRequestMsg> } };

              // Attach callback : Power Supervisor Session Requests
              if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedAttachCallbacksToDispatcher(h_app, subscriber.c_str(), ss_power_sprv_session_handlers, _countof(ss_power_sprv_session_handlers)))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
                // LCOV_EXCL_START 4: NSFW error case.
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                LOG_ERROR(
                    "FrameworkunifiedAttachCallbacksToDispatcher(ss_power_sprv_session_handlers)");
                FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
                return l_eStatus;
              }
              // LCOV_EXCL_STOP 4: NSFW error case.

              FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
                     "Subscriber '%s' opened a 'SUPERVISOR' session",
                     subscriber.c_str());
              newEntry.esessiontype = tOpenSessionReq;

              static BOOL first_supervisor = TRUE;
              if (first_supervisor) {
                AttachCallbackToSystemManager(  // LCOV_EXCL_BR_LINE 4: NSFW error case.
                    h_app, SS_SM_SYSTEM_MODE_INFO_RSPN,
                    PowerCallback<Power, &Power::OnSystemModeInfoResponse>);
                first_supervisor = FALSE;
              }
            }
              break;
            case epsstSYSTEM: {
              FrameworkunifiedProtocolCallbackHandler ss_power_system_session_handlers[] = {
                  { PROTOCOL_OPEN_SESSION_REQ, PowerCallback<Power,
                      &Power::OnOpenSession> }, { PROTOCOL_CLOSE_SESSION_REQ,
                      PowerCallback<Power, &Power::OnCloseSession> }, {
                      SS_POWER_SYSTEM_LAUNCH_COMPLETE, PowerCallback<Power,
                          &Power::OnSystemLaunchComplete> }, {
                      SS_POWER_SHUTDOWN_RESP, PowerCallback<Power,
                          &Power::OnSystemShutdownComplete> } };

              // Attach callback : Power System Session Requests
              if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedAttachCallbacksToDispatcher(h_app, subscriber.c_str(), ss_power_system_session_handlers, _countof(ss_power_system_session_handlers)))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
                // LCOV_EXCL_START 4: NSFW error case.
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                LOG_ERROR(
                    "FrameworkunifiedAttachCallbacksToDispatcher(ss_power_session_handlers)");
                FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
                return l_eStatus;
              }
              // LCOV_EXCL_STOP 4: NSFW error case.

              FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
                     "Subscriber '%s' opened a 'SYSTEM' session",
                     subscriber.c_str());
              newEntry.esessiontype = tOpenSessionReq;
            }
              break;
            case epsstUNKNOWN: {
              FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                     "Subscriber '%s' tried to open a type 'UNKNOWN' session",
                     subscriber.c_str());
              FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
              return eFrameworkunifiedStatusInvldParam;
            }
              break;
          }
        }
      }
    }

    // insert this newEntry into the map
    m_mapSessions.insert(make_pair(subscriber, newEntry));
  } else {
    // This Subscriber is already in our map, just send and ack to keep them quote.
    FRAMEWORKUNIFIEDLOG(
        ZONE_INFO, __FUNCTION__,
        "Subscriber : %s , is already in the map, will Just send back an ACK",
        iter->second.sz_name.c_str());
    if (NULL != iter->second.hsession) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
      OpenSessionAck openSessionAck;
      memset(&openSessionAck, 0, sizeof(openSessionAck));
      strcpy(openSessionAck.cSessionName, SERVICE_POWER);  // NOLINT (runtime/printf)
      openSessionAck.eStatus = eFrameworkunifiedStatusOK;
      // I can't think of a reason why we would need more that one session per subscriber.
      openSessionAck.sessionId = 0;

      // Send Ack to subscriber
      l_eStatus = FrameworkunifiedSendMsg(iter->second.hsession, PROTOCOL_OPEN_SESSION_ACK,
                             sizeof(OpenSessionAck), (PVOID) &openSessionAck);
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
             "FrameworkunifiedSendMsg Success to : %s , status: 0x%x",
             iter->second.sz_name.c_str(), l_eStatus);
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__,
             "What!  NULL == iter->second.hsession TODO: Need to handle this!");
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////
///
///
///
//////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus PowerSessionHandler::StopComplete(HANDLE h_app) {  // LCOV_EXCL_START 8: can not be called
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // ToDo Jay 2012 November 02 Can this be deleted ?
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
         " No-op function - It's here, but it doesn't do anything");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
// LCOV_EXCL_STOP 8: can not be called

///////////////////////////////////////////////////////////////////////
/// ProcessVoltageNotifyRequest
///
///
//////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus Power::PublishVoltageStateChange(HANDLE h_app) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  PowerSrvLVIStatus l_LVIStatus;
  PowerSrvLevelType l_PowerLevel;

  switch (m_VoltageState) {  // LCOV_EXCL_BR_LINE 6: double check
    case epsvsNORMAL:
      l_PowerLevel = epspltNORMAL;
l_eStatus = FrameworkunifiedNPPublishNotification(h_app, szNTFY_PowerLevel, &l_PowerLevel, sizeof(PowerSrvLevelType));
                        LOG_STATUS_IF_ERRORED_PWR_SM_WITH_HIST_LOGGING(
          l_eStatus,
          "FrameworkunifiedNPPublishNotification(" szNTFY_PowerLevel ", epspltNORMAL)");

      l_LVIStatus = ePwSrvLVI_Status_InActive;
l_eStatus = FrameworkunifiedNPPublishNotification(h_app, szNTFY_PowerLVI1, &l_LVIStatus, sizeof(PowerSrvLVIStatus));
                        LOG_STATUS_IF_ERRORED_PWR_SM_WITH_HIST_LOGGING(
          l_eStatus,
          "FrameworkunifiedNPPublishNotification(" szNTFY_PowerLVI1 ", ePwSrvLVI_Status_InActive)");

l_eStatus = FrameworkunifiedNPPublishNotification(h_app, szNTFY_PowerLVI2, &l_LVIStatus, sizeof(PowerSrvLVIStatus));
                        LOG_STATUS_IF_ERRORED_PWR_SM_WITH_HIST_LOGGING(
          l_eStatus,
          "FrameworkunifiedNPPublishNotification(" szNTFY_PowerLVI2 ", ePwSrvLVI_Status_InActive)");
      break;

    case epsvsLVI1:
      l_PowerLevel = epspltEMERGENCY;
l_eStatus = FrameworkunifiedNPPublishNotification(h_app, szNTFY_PowerLevel, &l_PowerLevel, sizeof(PowerSrvLevelType));
                        LOG_STATUS_IF_ERRORED_PWR_SM_WITH_HIST_LOGGING(
          l_eStatus,
          "FrameworkunifiedNPPublishNotification(" szNTFY_PowerLevel ", epspltEMERGENCY)");

      l_LVIStatus = ePwSrvLVI_Status_Active;
l_eStatus = FrameworkunifiedNPPublishNotification(h_app, szNTFY_PowerLVI1, &l_LVIStatus, sizeof(PowerSrvLVIStatus));
                        LOG_STATUS_IF_ERRORED_PWR_SM_WITH_HIST_LOGGING(
          l_eStatus,
          "FrameworkunifiedNPPublishNotification(" szNTFY_PowerLVI1 ", ePwSrvLVI_Status_Active)");

      l_LVIStatus = ePwSrvLVI_Status_InActive;
l_eStatus = FrameworkunifiedNPPublishNotification(h_app, szNTFY_PowerLVI2, &l_LVIStatus, sizeof(PowerSrvLVIStatus));
                        LOG_STATUS_IF_ERRORED_PWR_SM_WITH_HIST_LOGGING(
          l_eStatus,
          "FrameworkunifiedNPPublishNotification(" szNTFY_PowerLVI2 ", ePwSrvLVI_Status_InActive)");
      break;

    case epsvsLVI2:
      l_PowerLevel = epspltEMERGENCY;
l_eStatus = FrameworkunifiedNPPublishNotification(h_app, szNTFY_PowerLevel, &l_PowerLevel, sizeof(PowerSrvLevelType));
                        LOG_STATUS_IF_ERRORED_PWR_SM_WITH_HIST_LOGGING(
          l_eStatus,
          "FrameworkunifiedNPPublishNotification(" szNTFY_PowerLevel ", epspltEMERGENCY)");

      l_LVIStatus = ePwSrvLVI_Status_InActive;
l_eStatus = FrameworkunifiedNPPublishNotification(h_app, szNTFY_PowerLVI1, &l_LVIStatus, sizeof(PowerSrvLVIStatus));
                        LOG_STATUS_IF_ERRORED_PWR_SM_WITH_HIST_LOGGING(
          l_eStatus,
          "FrameworkunifiedNPPublishNotification(" szNTFY_PowerLVI1 ", ePwSrvLVI_Status_InActive)");

      l_LVIStatus = ePwSrvLVI_Status_Active;
l_eStatus = FrameworkunifiedNPPublishNotification(h_app, szNTFY_PowerLVI2, &l_LVIStatus, sizeof(PowerSrvLVIStatus));
                        LOG_STATUS_IF_ERRORED_PWR_SM_WITH_HIST_LOGGING(
          l_eStatus,
          "FrameworkunifiedNPPublishNotification(" szNTFY_PowerLVI2 ", ePwSrvLVI_Status_Active)");
      break;

    case epsvsINVALID:
      // LCOV_EXCL_START 6: double check
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_eStatus = eFrameworkunifiedStatusInvldParam;
      LOG_ERROR(" epsvsINVALID == m_VoltageState");
      break;
      // LCOV_EXCL_STOP 6: double check

    default:
      // LCOV_EXCL_START 6: double check
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_eStatus = eFrameworkunifiedStatusInvldParam;
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Error: Invalid 'm_VoltageState' value detected: 0x%X/%d",
             m_VoltageState, m_VoltageState);
      break;
      // LCOV_EXCL_STOP 6: double check
  }

  SS_PWR_LOG_HIST("PublishVoltageStateChange()", m_PubCmdHist, m_PubHistIter,
                  "", l_eStatus);

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
///////////////////////////////////////////////////////////////////////
/// OnSystemModeInfoResponse
///
//////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus Power::OnSystemModeInfoResponse(HANDLE h_app) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  SystemModeInfo l_SystemModeInfo;

  std::memset(&l_SystemModeInfo, 0, sizeof(SystemModeInfo));

  // ReadMsg():                                                        *
  //     Check h_app ptr, msg size, msg reception, read msg if all ok.  *
  //     Report any errors found.                                      *
  //                                                                   *
  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg<SystemModeInfo>(h_app, l_SystemModeInfo))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("ReadMsg()");  // LCOV_EXCL_LINE 4: NSFW error case.
    // LCOV_EXCL_STOP 4: NSFW error case.
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
           "Sending SS_POWER_SYSTEM_MODE_INFO_RESP to PSM");
    // send Wake-up complete response to System Power Manager
    l_eStatus = m_oSessionHandler.SendToSupervisor(
        SS_POWER_SYSTEM_MODE_INFO_RESP, sizeof(SystemModeInfo),
        (PVOID) &l_SystemModeInfo);

    SS_PWR_LOG_HIST("SS_POWER_SYSTEM_MODE_INFO_RESP", m__CWORD56_RepHist,
                    m__CWORD56_RepIter, "", l_eStatus);

    LOG_STATUS_IF_ERRORED_PWR_SM_WITH_HIST_LOGGING(
        l_eStatus, "m_oSessionHandler.SendToSupervisor( "
        "SS_POWER_SYSTEM_MODE_INFO_RESP)");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////
/// OnSystemModeInfoRequest
///
//////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus Power::OnSystemModeInfoRequest(HANDLE h_app) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (eFrameworkunifiedStatusOK != (l_eStatus = SendSystemModeRequestToSystemManager())) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("SendSystemModeRequestToSystemManager()");
    // LCOV_EXCL_STOP 4: NSFW error case.
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////
/// OnInitCompReport
///
//////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus Power::OnInitCompReport(HANDLE h_app) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (eFrameworkunifiedStatusOK != (l_eStatus = SendInitCompReportToSystemManager())) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("SendInitCompReportToSystemManager()");
    // LCOV_EXCL_STOP 4: NSFW error case.
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////
/// OnCurrentPowerStateQuery
///
//////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus Power::OnCurrentPowerStateQuery(HANDLE h_app) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  SS_PSCurrentState l_CurrentState;
  std::memset(&l_CurrentState, 0, sizeof(SS_PSCurrentState));

  // ReadMsg():                                                        *
  //     Check h_app ptr, msg size, msg reception, read msg if all ok.  *
  //     Report any errors found.                                      *
  //                                                                   *
  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg<SS_PSCurrentState>(h_app, l_CurrentState))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("ReadMsg()");
    // LCOV_EXCL_STOP 4: NSFW error case.
  } else if (eFrameworkunifiedStatusOK != (l_eStatus = ConstructPwrStateResponse(l_CurrentState.printRespmsg))) {  // LCOV_EXCL_BR_LINE 8: dead code  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 8: dead code.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("ConstructPwrStateResponse(l_CurrentState.printRespmsg)");
    // LCOV_EXCL_STOP
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Response is %s",
           l_CurrentState.printRespmsg);
    // Send Power State to client

    HANDLE l_handle = FrameworkunifiedOpenService(h_app, FrameworkunifiedGetMsgSrc(h_app));
    if (NULL != l_handle) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
      l_eStatus = FrameworkunifiedSendMsg(l_handle, SS_POWER_CRNT_STATE_QUERY_RSPN,
                             sizeof(SS_PSCurrentState),
                             (PVOID) &l_CurrentState);

      LOG_STATUS_IF_ERRORED_PWR_SM_WITH_HIST_LOGGING(
          l_eStatus, "FrameworkunifiedSendMsg(SS_POWER_CRNT_STATE_QUERY_RSPN)");

      l_eStatus = FrameworkunifiedCloseService(h_app, l_handle);
      LOG_STATUS_IF_ERRORED_PWR_SM_WITH_HIST_LOGGING(l_eStatus,
                                                     "FrameworkunifiedCloseService()");
    } else {
      // LCOV_EXCL_START 4: NSFW error case.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_eStatus = eFrameworkunifiedStatusNullPointer;
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error. FrameworkunifiedOpenService(%s) returned NULL",
             FrameworkunifiedGetMsgSrc(h_app));
      // LCOV_EXCL_STOP 4: NSFW error case.
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////
/// OnSetVoltageState
///
//////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus Power::OnSetVoltageState(HANDLE h_app) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  std::string stLvl = "INVALID";
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  Pwr_ServiceSetInterface l_VoltageState;
  // ReadMsg():                                                        *
  //     Check h_app ptr, msg size, msg reception, read msg if all ok.  *
  //     Report any errors found.                                      *
  //                                                                   *
  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg<Pwr_ServiceSetInterface>(h_app, l_VoltageState))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("ReadMsg()");
    // LCOV_EXCL_STOP 4: NSFW error case.
  } else {
    m_VoltageState = static_cast<ePowerSrvVoltageStates>(l_VoltageState.data
        .voltage.state);
    if (eFrameworkunifiedStatusOK != (l_eStatus = PublishVoltageStateChange(h_app))) {  // LCOV_EXCL_BR_LINE 4:NSFW error case.
      // LCOV_EXCL_START 4: NSFW error case.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      LOG_ERROR("PublishVoltageStateChange()");
      // LCOV_EXCL_STOP 4: NSFW error case.
    }

    SS_PWR_LOG_HIST("OnSetVoltageState", m_PubCmdHist, m_PubHistIter,
                    "PublishVoltageStateChange()", l_eStatus);

    switch (l_VoltageState.data.voltage.state) {
      case epsvsINVALID: {
        stLvl = "INVALID";
      }
        break;

      case epsvsNORMAL: {
        stLvl = "NORMAL";
      }
        break;

      case epsvsLVI1: {
        stLvl = "LVI1";
      }
        break;

      case epsvsLVI2: {
        stLvl = "LVI2";
      }
        break;

      default:
        break;
    }

    SS_PWR_LOG_HIST("OnSetVoltageState()", m_VCmdHist, m_VHistIter, stLvl,
                    l_eStatus);
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////
/// OnSetCrankState
///
//////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus Power::OnSetCrankState(HANDLE h_app) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  Pwr_ServiceSetInterface l_CrankState;

  // ReadMsg():                                                        *
  //     Check h_app ptr, msg size, msg reception, read msg if all ok.  *
  //     Report any errors found.                                      *
  //                                                                   *
  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg<Pwr_ServiceSetInterface>(h_app, l_CrankState))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("ReadMsg()");
    // LCOV_EXCL_STOP 4: NSFW error case.
  } else {
    m_CrankState = static_cast<ePowerSrvCrankStates>(l_CrankState.data.crank
        .state);
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus Power::OnSystemMgrConnectionEstablished(HANDLE h_app) {
  EFrameworkunifiedStatus l_eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  l_eStatus = FrameworkunifiedPublishServiceAvailability(h_app, TRUE);

  SS_PWR_LOG_HIST("FrameworkunifiedPublishServiceAvailability()", m_PubCmdHist,
                  m_PubHistIter, "", l_eStatus);

  LOG_STATUS(l_eStatus,
             "FrameworkunifiedPublishServiceAvailability(" szNTFY_PowerAvailability ",TRUE)");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus Power::OnUserModeResponse(HANDLE h_app) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  EPWR_USER_MODE_TYPE l_eUserModeState = epsumINVALID;
  Pwr_ServiceSetInterface tServiceSetIf;

  l_eStatus = ValidateUserModeMessage(h_app, l_eUserModeState);
  if (eFrameworkunifiedStatusOK != l_eStatus) {
    LOG_ERROR("ValidateUserModeMessage(&l_eUserModeState)");
  } else {
    PCSTR p_sStateName = l_eUserModeState == epsumON ? "epsumON" : "epsumOFF";
    tServiceSetIf.data.user_mode.mode = l_eUserModeState;

    l_eStatus = m_oSessionHandler.SendToSupervisor(
        SS_POWER_USER_MODE_SET_RESP, sizeof(Pwr_ServiceSetInterface),
        (PVOID) &tServiceSetIf);

    if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
      // LCOV_EXCL_START 4: NSFW error case.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(
          ZONE_ERR,
          __FUNCTION__,
          "Error: m_oSessionHandler.SendToSupervisor(" " SS_POWER_USER_MODE_SET_RESP, %s) errored: %d/'%s'",
          p_sStateName, l_eStatus, GetStr(l_eStatus).c_str());
      // LCOV_EXCL_STOP
    } else {
      SS_PWR_LOG_HIST("SS_POWER_USER_MODE_SET_RESP", m__CWORD56_RepHist,
                      m__CWORD56_RepIter, p_sStateName, l_eStatus);
      FRAMEWORKUNIFIEDLOG(
          ZONE_INFO,
          __FUNCTION__,
          "  m_oSessionHandler.SendToSupervisor(" " SS_POWER_USER_MODE_SET_RESP, %s) successful ",
          p_sStateName);
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus Power::OnUserModeResponse( HANDLE h_app )

EFrameworkunifiedStatus Power::ValidateUserModeMessage(
    HANDLE h_app, EPWR_USER_MODE_TYPE &l_eUserModeState) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  Pwr_ServiceSetInterface tServiceSetIf;
  EPWR_USER_MODE_TYPE l_my_eUserModeState;

  // ReadMsg():                                                        *
  //     Check h_app ptr, msg size, msg reception, read msg if all ok.  *
  //     Report any errors found.                                      *
  //                                                                   *
  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg<Pwr_ServiceSetInterface>(h_app, tServiceSetIf))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("ReadMsg()");
    // LCOV_EXCL_STOP 4: NSFW error case.
  } else {
    l_my_eUserModeState = tServiceSetIf.data.user_mode.mode;
    switch (l_my_eUserModeState) {
      case epsumINVALID:
        l_eStatus = eFrameworkunifiedStatusInvldParam;
        LOG_ERROR("l_eUserModeState == epsumINVALID");
        break;

      case epsumOFF:
      case epsumON:
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Validated '%s'",
               l_my_eUserModeState == epsumON ? "epsumON" : "epsumOFF");
        l_eUserModeState = l_my_eUserModeState;
        l_eStatus = eFrameworkunifiedStatusOK;
        break;

      default:
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Error: Unknown 'l_my_eUserModeState' value: 0x%x",
               l_my_eUserModeState);
        l_eStatus = eFrameworkunifiedStatusInvldParam;
        break;
    }  // End switch
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus Power::ValidateUserModeMessage( HANDLE h_app, EPWR_USER_MODE_TYPE &l_eUserModeState )

//*****************************************************************************
// Start Confirmation Protocol callback functions
//
EFrameworkunifiedStatus Power::OnSendStartupConfirmationRequest(HANDLE h_app) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  EFrameworkunifiedStatus l_responseStatus = eFrameworkunifiedStatusOK;
  Pwr_ServiceSetInterface tServiceSetIf;

  // ReadMsg():                                                        *
  //     Check h_app ptr, msg size, msg reception, read msg if all ok.  *
  //     Report any errors found.                                      *
  //                                                                   *
  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg<Pwr_ServiceSetInterface>(h_app, tServiceSetIf))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("ReadMsg()");
    // LCOV_EXCL_STOP 4: NSFW error case.
  } else {
    StartupConfirmationMsgStrut l_startupConfirmationMsg = tServiceSetIf.data
        .startupConfirmationMsg;
    l_eStatus = SendStartupConfirmationToSystemManager(
        l_startupConfirmationMsg);
    LOG_STATUS_IF_ERRORED_PWR_SM_WITH_HIST_LOGGING(
        l_eStatus, "SendStartupConfirmationToSystemManager()");
  }

  if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 200:interface_unified if can not be error.
    // LCOV_EXCL_START 200:interface_unified if can not be error.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_responseStatus = l_eStatus;  // Save the current status as-is to
                                   // send in the message response to the
                                   // requester.
    l_eStatus = m_oSessionHandler.SendToSupervisor(
        SS_POWER_FWD_START_CONFIRMATION_MSG_RESP, sizeof(EFrameworkunifiedStatus),
        (PVOID) &l_responseStatus);

    SS_PWR_LOG_HIST("SS_POWER_FWD_START_CONFIRMATION_MSG_RESP", m__CWORD56_RepHist,
                    m__CWORD56_RepIter, "", l_eStatus);

    LOG_STATUS(
        l_eStatus,
        "m_oSessionHandler.SendToSupervisor( " "SS_POWER_FWD_START_CONFIRMATION_MSG_RESP)");
    // LCOV_EXCL_STOP 200:interface_unified if can not be error.
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
           " SendStartupConfirmationToSystemManager() successful");
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus Power::OnSendStartupConfirmationRequest( HANDLE h_app )

EFrameworkunifiedStatus Power::OnSendStartupConfirmationResponse(HANDLE h_app) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  EFrameworkunifiedStatus l_responseStatus;

  // ReadMsg():                                                        *
  //     Check h_app ptr, msg size, msg reception, read msg if all ok.  *
  //     Report any errors found.                                      *
  //                                                                   *
  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg<EFrameworkunifiedStatus>(h_app, l_responseStatus))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("ReadMsg()");
    // LCOV_EXCL_STOP 4: NSFW error case.
  } else {
    l_eStatus = m_oSessionHandler.SendToSupervisor(
        SS_POWER_FWD_START_CONFIRMATION_MSG_RESP, sizeof(EFrameworkunifiedStatus),
        (PVOID) &l_responseStatus);

    SS_PWR_LOG_HIST("SS_POWER_FWD_START_CONFIRMATION_MSG_RESP", m__CWORD56_RepHist,
                    m__CWORD56_RepIter, "", l_eStatus);

    LOG_STATUS(
        l_eStatus,
        "m_oSessionHandler.SendToSupervisor(" " SS_POWER_FWD_START_CONFIRMATION_MSG_RESP)");
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus Power::OnSendStartupConfirmationResponse( HANDLE h_app )
//
// End of Start Confirmation Protocol callback functions
//*****************************************************************************

//*****************************************************************************
// Start HeartBeat Protocol callback functions
// Theory of operation: Forward HB request to SM. If SM crashes, then the HB
// will cease between SM and the _CWORD56_.  Consequently, the  _CWORD56_ will reset the
// system.
EFrameworkunifiedStatus Power::On_CWORD56_HeartBeatRequest(HANDLE h_app) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  EPWR_HB_REQ_MSG_STRUCT l_HbReq;

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "_CWORD56_ HeartBeat Request received.");

  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg<EPWR_HB_REQ_MSG_STRUCT>(h_app, l_HbReq))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("ReadMsg()");
    // LCOV_EXCL_STOP 4: NSFW error case.
  } else if (eFrameworkunifiedStatusOK != (l_eStatus = Send_CWORD56_HeartBeatRequestToSystemManager(l_HbReq))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("Send_CWORD56_HeartBeatRequestToSystemManager()");
    // LCOV_EXCL_STOP 4: NSFW error case.
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus Power::OnSM_CWORD56_HeartBeatResponse(HANDLE h_app) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Received from SM.");

  l_eStatus = m_oSessionHandler.SendToSupervisor(SS_POWER_HEARTBEAT_RESP, 0,
  NULL);

  SS_PWR_LOG_HIST("SS_POWER_HEARTBEAT_RESP", m__CWORD56_RepHist, m__CWORD56_RepIter, "",
                  l_eStatus);

  LOG_STATUS(l_eStatus,
             "m_oSessionHandler.SendToSupervisor( SS_POWER_HEARTBEAT_RESP)");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

//
// End of Heartbeat Protocol callback functions
//*****************************************************************************

//*****************************************************************************
// Start CPU Reset Request Protocol callback functions
//
EFrameworkunifiedStatus Power::OnCpuResetRequest(HANDLE h_app) {  // SS_SM_CPU_RESET_REQ
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  TSystemManagerCpuResetInfo l_SmCpuResetInfo;  // SS Type
  SS_Pwr_CpuResetMsgStruct l_PsCpuResetInfo;  // SS Type translated to PSM type.

  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg<TSystemManagerCpuResetInfo>(h_app, l_SmCpuResetInfo))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("ReadMsg()");
    // LCOV_EXCL_STOP 4: NSFW error case.
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "CPU Reset Type: %d",
           l_SmCpuResetInfo.resetReason);

    switch (l_SmCpuResetInfo.resetReason) {
      case e_SS_SM_CPU_RESET_REASON_GENERIC_ERR:
      case e_SS_SM_CPU_RESET_REASON_CRITICAL_ERR:
        l_PsCpuResetInfo.resetReason = epsCpuResetReasonGeneric;
        break;

      case e_SS_SM_CPU_RESET_REASON_DSP_ERR:
        l_PsCpuResetInfo.resetReason = epsCpuResetReasonFatalError;
        break;

      case e_SS_SM_CPU_RESET_REASON_USER_FORCE_RESET:
        l_PsCpuResetInfo.resetReason = epsCpuResetReasonUserForceReset;
        break;

      case e_SS_SM_CPU_RESET_REASON_NORMAL:
        l_PsCpuResetInfo.resetReason = epsCpuResetReasonNormalReset;
        break;
      default:
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: Reset reason '%d' not defined",
               l_SmCpuResetInfo.resetReason);
        FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
        return (eFrameworkunifiedStatusInvldParam);
    }

    snprintf(l_PsCpuResetInfo.messageStr, PWR_RESET_MSG_STR_SIZE, "%s",
             l_SmCpuResetInfo.messageStr);

    l_eStatus = m_oSessionHandler.SendToSupervisor(SS_POWER_HARD_RESET_REQ,
                                                   sizeof(l_PsCpuResetInfo),
                                                   (PVOID) &l_PsCpuResetInfo);

    SS_PWR_LOG_HIST("SS_POWER_HARD_RESET_REQ", m__CWORD56_RepHist, m__CWORD56_RepIter, "",
                    l_eStatus);

    LOG_STATUS_IF_ERRORED_PWR_SM_WITH_HIST_LOGGING(
        l_eStatus, "m_oSessionHandler.SendToSupervisor( "
        "SS_POWER_HARD_RESET_REQ)");
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
//
// End of CPU Reset Request Protocol callback functions
//*****************************************************************************

///////////////////////////////////////////////////////////////////////
/// SetCmdHist
///
///
//////////////////////////////////////////////////////////////////////
void Power::SetCmdHist(std::string cmd, cmdHist &hist, cmdHistIter &it,
                       std::string sender) {
  pthread_mutex_lock(&pwr_hist_mutex);

  UI_64 l_clkcycles = ClockCycle();
  /* find out how many cycles per millisecond */
  UI_64 l_totalmsec = l_clkcycles / 1000;

  it->m_time = l_totalmsec;
  it->m_cmd = cmd;
  it->m_sender = sender;
  it++;

  if (it == hist.end()) {
    it = hist.begin();
  }

  pthread_mutex_unlock(&pwr_hist_mutex);
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup SSPowerDebugDump
///     implement post mortem function
///
/// \param
///
/// \return void
///////////////////////////////////////////////////////////////////////////////
VOID Power::SSPowerDebugDump(HANDLE h_app) {  // LCOV_EXCL_START 7:debug code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  char l_debugDumpData[SS_PWR_DEBUG_DUMP_MAX_SIZE];
  UI_16 l_byteCount = 0;
  cmdHistIter i;
  memset((void*) l_debugDumpData, 0x00, sizeof(l_debugDumpData));  // NOLINT (readability/casting)

  l_byteCount = snprintf(l_debugDumpData, SS_PWR_DEBUG_DUMP_MAX_SIZE,
                         ("\n <SS POWER DUMP DATA> \n"));

  l_byteCount += snprintf(l_debugDumpData + l_byteCount,
  SS_PWR_DEBUG_DUMP_MAX_SIZE - l_byteCount,
                          "\n ***Error history***\n");

  for (i = m_ErrHist.begin(); i != m_ErrHist.end(); ++i) {
    l_byteCount += snprintf(l_debugDumpData + l_byteCount,
    SS_PWR_DEBUG_DUMP_MAX_SIZE - l_byteCount,
                            "\n %s @ %llums EC:%s \n", i->m_cmd.c_str(),
                            i->m_time, i->m_sender.c_str());
  }

  l_byteCount += snprintf(l_debugDumpData + l_byteCount,
  SS_PWR_DEBUG_DUMP_MAX_SIZE - l_byteCount,
                          "\n ***_CWORD56_ Reply history***\n");

  for (i = m__CWORD56_RepHist.begin(); i != m__CWORD56_RepHist.end(); ++i) {
    l_byteCount += snprintf(l_debugDumpData + l_byteCount,
    SS_PWR_DEBUG_DUMP_MAX_SIZE - l_byteCount,
                            "\n %s @ %llums\n", i->m_cmd.c_str(), i->m_time);
  }

  l_byteCount += snprintf(l_debugDumpData + l_byteCount,
  SS_PWR_DEBUG_DUMP_MAX_SIZE - l_byteCount,
                          "\n ***Voltage history***\n");

  for (i = m_VCmdHist.begin(); i != m_VCmdHist.end(); ++i) {
    l_byteCount += snprintf(l_debugDumpData + l_byteCount,
    SS_PWR_DEBUG_DUMP_MAX_SIZE - l_byteCount,
                            "\n %s @ %llums %s \n", i->m_cmd.c_str(), i->m_time,
                            i->m_sender.c_str());
  }

  l_byteCount += snprintf(l_debugDumpData + l_byteCount,
  SS_PWR_DEBUG_DUMP_MAX_SIZE - l_byteCount,
                          "\n ***Publishshing history***\n");

  for (i = m_PubCmdHist.begin(); i != m_PubCmdHist.end(); ++i) {
    l_byteCount += snprintf(l_debugDumpData + l_byteCount,
    SS_PWR_DEBUG_DUMP_MAX_SIZE - l_byteCount,
                            "\n %s @ %llums \n", i->m_cmd.c_str(), i->m_time);
  }

  SSDEBUGDUMP("%s", (const char *) &l_debugDumpData);
}
// LCOV_EXCL_STOP 7:debug code

//*****************************************************************************
// Start Remote Data Reset Protocol callback functions
// From SM to PS
EFrameworkunifiedStatus Power::OnRemoteDataResetRequest(HANDLE h_app) {  // SS_SM_REMOTE_DATA_RESET_REQ
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  ESMDataResetType l_eSmDataResetType;   // SS Type
  epsCpuResetReason l_ePsCpuResetReason;  // SS Type translated to PSM CPU reset reason.
  BOOL l_bResetRequired;

  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg<ESMDataResetType>(h_app, l_eSmDataResetType))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("ReadMsg()");
    // LCOV_EXCL_STOP 4: NSFW error case.
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Remote Data Reset Type: %d",
           l_eSmDataResetType);
    l_bResetRequired = FALSE;
    switch (l_eSmDataResetType) {
      case e_SS_SM_DATA_RESET_TYPE_USER:
        l_ePsCpuResetReason = epsCpuResetReasonUserDataReset;
        l_bResetRequired = TRUE;
        break;

      case e_SS_SM_DATA_RESET_TYPE_FACTORY:
        l_ePsCpuResetReason = epsCpuResetReasonFactoryDataReset;
        l_bResetRequired = TRUE;
        break;

      case e_SS_SM_DATA_RESET_TYPE_CONFIGURATION:  // No reset action required.
        l_bResetRequired = FALSE;
        break;

      default:
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Error: Remote Reset Data type : %d not defined",
               l_eSmDataResetType);
        FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
        return (eFrameworkunifiedStatusInvldParam);
    }

    if (TRUE == l_bResetRequired) {
      SS_Pwr_CpuResetMsgStruct l_PsCpuResetInfo = { };
      l_PsCpuResetInfo.resetReason = l_ePsCpuResetReason;
      l_eStatus = m_oSessionHandler.SendToSupervisor(SS_POWER_HARD_RESET_REQ,
                                                     sizeof(l_PsCpuResetInfo),
                                                     (PVOID) &l_PsCpuResetInfo);

      if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
        // LCOV_EXCL_START 4: NSFW error case.
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(
            ZONE_ERR,
            __FUNCTION__,
            "Error: m_oSessionHandler.SendToSupervisor( " "SS_POWER_HARD_RESET_REQ, Reason '%d') errored:%d/'%s'",
            l_ePsCpuResetReason, l_eStatus, GetStr(l_eStatus).c_str());
        // LCOV_EXCL_STOP
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
               "SS_POWER_HARD_RESET_REQ Reason '%d' sent to PSMShadow",
               l_ePsCpuResetReason);
      }
      SS_PWR_LOG_HIST("SS_POWER_HARD_RESET_REQ", m__CWORD56_RepHist, m__CWORD56_RepIter,
                      "", l_eStatus);
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
//
// End of Remote Data Reset Protocol callback functions
//*****************************************************************************

// EOF of /SS_PowerService/src/ss_power.cpp

