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
/// \brief    This file supports the power service state machine.
///
///////////////////////////////////////////////////////////////////////////////

#include "ss_power_state_machine.h"
#include <system_service/ss_power_service_protocol.h>
#include <system_service/ss_power_service.h>
#include <system_service/ss_power_service_notifications.h>
#include <system_service/ss_power_service_local.h>
#include <system_service/ss_power_service_notifications_local.h>
#include <system_service/ss_services.h>
#include <native_service/ns_timer_if.h>
#include <native_service/frameworkunified_framework_if.h>


#include "ss_power_session.h"
#include "ss_power_powerservicelog.h"

/// Define static members of the class.
PowerStateMachine::Wakeup PowerStateMachine::WakeupState;
PowerStateMachine::WakeupActive PowerStateMachine::WakeupActiveState;
PowerStateMachine::WakeupPending PowerStateMachine::WakeupPendingState;
PowerStateMachine::LowVoltage1 PowerStateMachine::LowVoltage1State;
PowerStateMachine::LowVoltage1Active PowerStateMachine::LowVoltage1ActiveState;
PowerStateMachine::LowVoltage2 PowerStateMachine::LowVoltage2State;
PowerStateMachine::LowVoltage2Active PowerStateMachine::LowVoltage2ActiveState;
PowerStateMachine::Shutdown PowerStateMachine::ShutdownState;
PowerStateMachine::ShutdownActive PowerStateMachine::ShutdownActiveState;
PowerStateMachine::NormalVoltage PowerStateMachine::NormalVoltageState;
PowerStateMachine::SoftwareUpdate PowerStateMachine::SoftwareUpdateState;

/// Constructor
PowerStateMachine::PowerStateMachine()
    : m_pCurrentState(&WakeupState),
      m_pPreviousState(NULL),
      m_pOnHysteresisTimeoutState(NULL),
      m_oShutdownHysteresis(),
      m_oLowVoltage1Hysteresis(),
      m_oLowVoltage2Hysteresis(),
      m_hHysteresisTimer(NULL),
      m_tStateInfo() {
}

/// Deconstructor
PowerStateMachine::~PowerStateMachine() {  // LCOV_EXCL_START 14 Resident process, not called by NSFW
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
}
// LCOV_EXCL_STOP 14 Resident process, not called by NSFW

void PowerStateMachine::onEvent(HANDLE h_app, PowerSessionHandler & oSession,
                                ePowerStateMachineEvents evt) {
  FRAMEWORKUNIFIEDLOG0(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_POWER_STATEMACHINE, __FUNCTION__,
         "(ENTRY) Power StateMachine Current State: %s, Previous State: %s",
         m_pCurrentState->name(),
         (m_pPreviousState == NULL ? "Ivalid" : m_pPreviousState->name()));

  switch (evt) {  // LCOV_EXCL_BR_LINE 8: dead code
    case PowerStateMachine::epsmeWAKEUP:
      FRAMEWORKUNIFIEDLOG0(ZONE_POWER_STATEMACHINE, __FUNCTION__, "Power StateMachine Event Received: epsmeWAKEUP");
      m_pCurrentState->onWakeup(*this, h_app, oSession);
      break;
    case PowerStateMachine::epsmeSTART_COMPLETE:  // LCOV_EXCL_START 8: no one send the event
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG0(ZONE_POWER_STATEMACHINE, __FUNCTION__, "Power StateMachine Event Received: epsmeSTART_COMPLETE");
      m_pCurrentState->onStartComplete(*this, h_app, oSession);
      break;
    // LCOV_EXCL_STOP 8: no one send the event
    case PowerStateMachine::epsmeSHUTDOWN:
      FRAMEWORKUNIFIEDLOG0(ZONE_POWER_STATEMACHINE, __FUNCTION__, "Power StateMachine Event Received: epsmeSHUTDOWN");
      m_pCurrentState->onShutdown(*this, h_app, oSession);
      break;
    case PowerStateMachine::epsmeSTOP_COMPLETE:  // LCOV_EXCL_START 8: no one send the event
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG0(ZONE_POWER_STATEMACHINE, __FUNCTION__, "Power StateMachine Event Received: epsmeSTOP_COMPLETE");
      m_pCurrentState->onStopComplete(*this, h_app, oSession);
      break;
    case PowerStateMachine::epsmeLVI1_ENCOUNTERED:
      FRAMEWORKUNIFIEDLOG0(ZONE_POWER_STATEMACHINE, __FUNCTION__, "Power StateMachine Event Received: epsmeLVI1_ENCOUNTERED");
      m_pCurrentState->onLowVoltage1Encountered(*this, h_app, oSession);
      break;
    case PowerStateMachine::epsmeLVI2_ENCOUNTERED:
      FRAMEWORKUNIFIEDLOG0(ZONE_POWER_STATEMACHINE, __FUNCTION__, "Power StateMachine Event Received: epsmeLVI2_ENCOUNTERED");
      m_pCurrentState->onLowVoltage2Encountered(*this, h_app, oSession);
      break;
    case PowerStateMachine::epsmeNORMAL_VOLTAGE_ENCOUNTERED:
      FRAMEWORKUNIFIEDLOG0(ZONE_POWER_STATEMACHINE, __FUNCTION__,
              "Power StateMachine Event Received: epsmeNORMAL_VOLTAGE_ENCOUNTERED");
      m_pCurrentState->onNormalVoltageEncountered(*this, h_app, oSession);
      break;
    case PowerStateMachine::epsmeSOFTWARE_UPDATE:
      FRAMEWORKUNIFIEDLOG0(ZONE_POWER_STATEMACHINE, __FUNCTION__, "Power StateMachine Event Received: epsmeSOFTWARE_UPDATE");
      m_pCurrentState->onSoftwareUpdate(*this, h_app, oSession);
      break;
    case PowerStateMachine::epsmeSHUTDOWN_HYSTERESIS_ABORTED:
      FRAMEWORKUNIFIEDLOG0(ZONE_POWER_STATEMACHINE, __FUNCTION__,
              "Power StateMachine Event Received: epsmeSHUTDOWN_HYSTERESIS_ABORTED");
    case PowerStateMachine::epsmeLVI1_HYSTERESIS_ABORTED:
      FRAMEWORKUNIFIEDLOG0(ZONE_POWER_STATEMACHINE, __FUNCTION__, "Power StateMachine Event Received: epsmeLVI1_HYSTERESIS_ABORTED");
    case PowerStateMachine::epsmeLVI2_HYSTERESIS_ABORTED:
      FRAMEWORKUNIFIEDLOG0(ZONE_POWER_STATEMACHINE, __FUNCTION__, "Power StateMachine Event Received: epsmeLVI2_HYSTERESIS_ABORTED");
      m_pCurrentState->onHysteresisAborted(*this, h_app, oSession);
      break;
    // LCOV_EXCL_STOP 8: no one send the event
    case PowerStateMachine::epsmeSHUTDOWN_HYSTERESIS_TM_OUT:
      FRAMEWORKUNIFIEDLOG0(ZONE_POWER_STATEMACHINE, __FUNCTION__,  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
              "Power StateMachine Event Received: epsmeSHUTDOWN_HYSTERESIS_TM_OUT");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
    case PowerStateMachine::epsmeLVI1_HYSTERESIS_TM_OUT:
      FRAMEWORKUNIFIEDLOG0(ZONE_POWER_STATEMACHINE, __FUNCTION__, "Power StateMachine Event Received: epsmeLVI1_HYSTERESIS_TM_OUT");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
    case PowerStateMachine::epsmeLVI2_HYSTERESIS_TM_OUT:
      FRAMEWORKUNIFIEDLOG0(ZONE_POWER_STATEMACHINE, __FUNCTION__, "Power StateMachine Event Received: epsmeLVI2_HYSTERESIS_TM_OUT");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
      m_pCurrentState->onHysteresisTimeout(*this, h_app, oSession);
      break;
    default:
      FRAMEWORKUNIFIEDLOG0(ZONE_POWER_STATEMACHINE, __FUNCTION__, "Power StateMachine Event Received: WAS INVALID");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
      break;
  }  // ZONE_POWER_STATEMACHINE

  FRAMEWORKUNIFIEDLOG0(ZONE_FUNC, __FUNCTION__, "-");
  FRAMEWORKUNIFIEDLOG(ZONE_POWER_STATEMACHINE, __FUNCTION__,
         "(EXIT) Power StateMachine Current State: %s, Previous State: %s",
         m_pCurrentState->name(),
         (m_pPreviousState == NULL ? "Ivalid" : m_pPreviousState->name()));
}

PCSTR PowerStateMachine::name() {  // LCOV_EXCL_START 8: do not be called
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  if (NULL != m_pCurrentState) {
    return m_pCurrentState->name();
  }

  return "Invalid Current State is NULL...";
}
// LCOV_EXCL_STOP 8: do not be called

/// Switch to the next State and save the previous state.
void PowerStateMachine::NextState(Base_State & refState, HANDLE h_app,
                                  PowerSessionHandler & oSession) {
  m_pPreviousState = m_pCurrentState;
  m_pPreviousState->onExit(*this, h_app, oSession);
  m_pCurrentState = &refState;
  m_pCurrentState->onEntry(*this, h_app, oSession);
}

// LCOV_EXCL_START 8: do not be called
void PowerStateMachine::OnVoltage(Base_State * pState, HANDLE h_app,
                                  PowerSessionHandler & oSession) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  if (sizeof(Pwr_ServiceSetInterface) == FrameworkunifiedGetMsgLength(h_app)) {
    Pwr_ServiceSetInterface tServiceIf;

    if (eFrameworkunifiedStatusOK
        == FrameworkunifiedGetMsgDataOfSize(h_app, (PVOID) &tServiceIf,
                               sizeof(Pwr_ServiceSetInterface))) {
      /// Need to store the voltage information
      m_tStateInfo.voltage = tServiceIf.data.voltage.state;

      if (NULL != pState) {
        NextState(*pState, h_app, oSession);
      }
    }
  }
}
// LCOV_EXCL_STOP 8: do not be called
void PowerStateMachine::GotoStateOnHysteresisTimeout(Base_State & refState) {
  m_pOnHysteresisTimeoutState = &refState;
}

void PowerStateMachine::startHysteresisTimer(PowerStateHysteresis & hys_info) {
  /// clear the try counter.
  hys_info.clearTryCounter();

  /// bump the hysteresis try counter (prime the counter).
  hys_info.bumbTryCounter();

  /// setup the hysteresis timer
  NSTimerInfo turn_on_hysteresis = { WholeSeconds(hys_info.getTimeout()),
      MSToNS(RemainderMs(hys_info.getTimeout())), 0, 0, 0 };

  /// start the hysteresis timer
  NS_TimerSetTime(m_hHysteresisTimer, turn_on_hysteresis);
}

void PowerStateMachine::reStartHysteresisTimer(
    PowerStateHysteresis & hys_info) {
  /// bump the hysteresis try counter.
  hys_info.bumbTryCounter();

  /// setup the hysteresis timer (one shot)
  NSTimerInfo turn_on_hysteresis = { WholeSeconds(hys_info.getTimeout()),
      MSToNS(RemainderMs(hys_info.getTimeout())), 0, 0, 0 };
  /// start the hysteresis timer
  NS_TimerSetTime(m_hHysteresisTimer, turn_on_hysteresis);
}

void PowerStateMachine::stopHysteresisTimer(PowerStateHysteresis & hys_info) {
  NSTimerInfo turn_off_hysteresis = { 0, 0, 0, 0, 0 };
  NS_TimerSetTime(m_hHysteresisTimer, turn_off_hysteresis);
  /// clear the try counter.
  hys_info.clearTryCounter();
}

// LCOV_EXCL_START 8: can not be called
void PowerStateMachine::publishPowerLVIStatus(HANDLE h_app, PCSTR nNotifNm,
                                              UI_32 value) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  PowerSrvLVIStatus eLviStatus = static_cast<PowerSrvLVIStatus>(value);
  if (eFrameworkunifiedStatusOK
      != (eStatus = FrameworkunifiedNPPublishNotification(h_app, nNotifNm, &eLviStatus,
                                             sizeof(eLviStatus)))) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           " Error: FrameworkunifiedNPPublishNotification( %s ) errored: 0x%X", nNotifNm,
           eStatus);
  }
}
// LCOV_EXCL_STOP 8: can not be called
void PowerStateMachine::publishPowerLevelType(HANDLE h_app, UI_32 value) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  PowerSrvLevelType eLevelType = static_cast<PowerSrvLevelType>(value);
  if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedNPPublishNotification (h_app, szNTFY_PowerLevel, &eLevelType, sizeof (eLevelType)))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           " Error: FrameworkunifiedNPPublishNotification %s Failed Status:0x%x ",
           szNTFY_PowerLevel, eStatus);
  // LCOV_EXCL_STOP 4: NSFW error case.
  }
}

// LCOV_EXCL_START 8: no one send the event
void PowerStateMachine::Base_State::onStartComplete(
    PowerStateMachine &, HANDLE h_app, PowerSessionHandler & oSession) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  oSession.StartComplete(h_app);
}
// LCOV_EXCL_STOP 8: do not be called

// LCOV_EXCL_START 8: no one send the event
void PowerStateMachine::Base_State::onStopComplete(
    PowerStateMachine &, HANDLE h_app, PowerSessionHandler & oSession) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  oSession.StopComplete(h_app);
}
// LCOV_EXCL_STOP 8: no one send the event

// LCOV_EXCL_START 8: no one call this function
void PowerStateMachine::Base_State::onLaunchComplete(
    PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FrameworkunifiedDeferMessage(h_app);
}
// LCOV_EXCL_STOP 8: do not be called

// LCOV_EXCL_START 8: no one send the event
void PowerStateMachine::Base_State::onShutdownComplete(
    PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FrameworkunifiedDeferMessage(h_app);
}
// LCOV_EXCL_STOP 8: no one call this function
PCSTR PowerStateMachine::Wakeup::name() {
  return "Wakeup";
}

// LCOV_EXCL_START 8: no one send the event
void PowerStateMachine::Wakeup::onLowVoltage1Encountered(
    PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  u.OnVoltage(&LowVoltage1State, h_app, oSession);
  FRAMEWORKUNIFIEDLOG(ZONE_POWER_STATEMACHINE, __FUNCTION__, "LOW VOLTAGE 1 ENCOUNTERED %s",
         FrameworkunifiedGetMsgSrc(h_app));
}
// LCOV_EXCL_STOP 8: do not be called

// LCOV_EXCL_START 8: no one send the event
void PowerStateMachine::Wakeup::onLowVoltage2Encountered(
    PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  u.OnVoltage(&LowVoltage2State, h_app, oSession);
  FRAMEWORKUNIFIEDLOG(ZONE_POWER_STATEMACHINE, __FUNCTION__, "LOW VOLTAGE 2 ENCOUNTERED %s",
         FrameworkunifiedGetMsgSrc(h_app));
}
// LCOV_EXCL_STOP 8: no one send the event to call this function

// LCOV_EXCL_START 8: no one send the event to call this function
void PowerStateMachine::Wakeup::onNormalVoltageEncountered(
    PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  u.OnVoltage(NULL, h_app, oSession);
  FRAMEWORKUNIFIEDLOG(ZONE_POWER_STATEMACHINE, __FUNCTION__,
         "NORMAL ENCOUNTERED %s (no switch)", FrameworkunifiedGetMsgSrc(h_app));
}
// LCOV_EXCL_STOP 8: no one send the event
void PowerStateMachine::Wakeup::onShutdown(PowerStateMachine &u, HANDLE h_app,
                                           PowerSessionHandler & oSession) {
  u.NextState(ShutdownState, h_app, oSession);
}

// LCOV_EXCL_START 8: no one send the event
void PowerStateMachine::Wakeup::onSoftwareUpdate(
    PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  u.NextState(SoftwareUpdateState, h_app, oSession);
}
// LCOV_EXCL_STOP 8: no one send the event

// LCOV_EXCL_START 8: no one send the event
void PowerStateMachine::Wakeup::onWakeup(PowerStateMachine &u, HANDLE h_app,
                                         PowerSessionHandler & oSession) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  Pwr_ServiceSetInterface tServiceIf;
  if (sizeof(Pwr_ServiceSetInterface) == FrameworkunifiedGetMsgLength(h_app)) {
    if (eFrameworkunifiedStatusOK
        == FrameworkunifiedGetMsgDataOfSize(h_app, (PVOID) &tServiceIf,
                               sizeof(Pwr_ServiceSetInterface))) {
      if (epswsPWRON == tServiceIf.data.wake.powerupType) {
        /// Need to store the wakeup \ shutdown information
        u.m_tStateInfo.wake = tServiceIf.data.wake.powerupType;
        u.m_tStateInfo.level = tServiceIf.data.wake.up.level;
        u.m_tStateInfo.factor = tServiceIf.data.wake.up.factor;

        u.NextState(WakeupPendingState, h_app, oSession);
      }
    }
  }
}
// LCOV_EXCL_STOP 8: no one send the event

PCSTR PowerStateMachine::WakeupActive::name() {
  return "Wakeup Active";
}

// LCOV_EXCL_START 8: no one send the event
void PowerStateMachine::WakeupActive::onLowVoltage1Encountered(
    PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  u.OnVoltage(&LowVoltage1State, h_app, oSession);
  FRAMEWORKUNIFIEDLOG(ZONE_POWER_STATEMACHINE, __FUNCTION__, "LOW VOLTAGE 1 ENCOUNTERED %s",
         FrameworkunifiedGetMsgSrc(h_app));
}
// LCOV_EXCL_STOP 8: no one send the event to call this function

// LCOV_EXCL_START 8: no one send the event to call this function
void PowerStateMachine::WakeupActive::onLowVoltage2Encountered(
    PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  u.OnVoltage(&LowVoltage2State, h_app, oSession);
  FRAMEWORKUNIFIEDLOG(ZONE_POWER_STATEMACHINE, __FUNCTION__, "LOW VOLTAGE 2 ENCOUNTERED %s",
         FrameworkunifiedGetMsgSrc(h_app));
}
// LCOV_EXCL_STOP 8: no one send the event
void PowerStateMachine::WakeupActive::onShutdown(
    PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
  u.NextState(ShutdownState, h_app, oSession);
}

PCSTR PowerStateMachine::WakeupPending::name() {
  return "Wakeup Pending";
}

// LCOV_EXCL_START 8: no one send the event
void PowerStateMachine::WakeupPending::onNormalVoltageEncountered(
    PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  u.OnVoltage(&NormalVoltageState, h_app, oSession);
  FRAMEWORKUNIFIEDLOG(ZONE_POWER_STATEMACHINE, __FUNCTION__, "NORMAL ENCOUNTERED %s",
         FrameworkunifiedGetMsgSrc(h_app));
}
// LCOV_EXCL_STOP 8: no one send the event to call this function

// LCOV_EXCL_START 8: no one send the event to call this function
void PowerStateMachine::WakeupPending::onStartComplete(
    PowerStateMachine &, HANDLE h_app, PowerSessionHandler & oSession) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  oSession.StartComplete(h_app);
}
// LCOV_EXCL_STOP 8: no one send the event

// LCOV_EXCL_START 8: no one call this function
void PowerStateMachine::WakeupPending::onLaunchComplete(
    PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  oSession.SendToSupervisor(SS_POWER_WAKEUP_COMPLETE, 0, NULL);
  if (epsvsNORMAL == u.m_tStateInfo.voltage) {
    u.NextState(NormalVoltageState, h_app, oSession);
  }
}
// LCOV_EXCL_STOP 8: no one call this function

// LCOV_EXCL_START 8: no one send the event
void PowerStateMachine::WakeupPending::onWakeup(
    PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  if (oSession.WakeupComplete(u.m_lstWakeupModules)) {
    oSession.SendToSupervisor(SS_POWER_WAKEUP_COMPLETE, 0, NULL);
  }
}
// LCOV_EXCL_STOP 8: no one send the event

void PowerStateMachine::WakeupPending::onEntry(PowerStateMachine &u,
                                               HANDLE h_app,
                                               PowerSessionHandler & oSession) {
  if (!FrameworkunifiedIsDeferQueueEmpty(h_app)) {
    FrameworkunifiedRetrieveDeferMessage(h_app);
  }
}

PCSTR PowerStateMachine::Shutdown::name() {
  return "Shutdown";
}

// LCOV_EXCL_START 8: no one send the event
void PowerStateMachine::Shutdown::onLowVoltage1Encountered(
    PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  u.GotoStateOnHysteresisTimeout(LowVoltage1State);
  u.OnVoltage(NULL, h_app, oSession);
  FRAMEWORKUNIFIEDLOG(ZONE_POWER_STATEMACHINE, __FUNCTION__,
         "LOW VOLTAGE 1 ENCOUNTERED %s (no switch waiting for hysteresis to)",
         FrameworkunifiedGetMsgSrc(h_app));
}
// LCOV_EXCL_STOP 8: no one send the event to call this function

// LCOV_EXCL_START 8: no one send the event to call this function
void PowerStateMachine::Shutdown::onLowVoltage2Encountered(
    PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  u.GotoStateOnHysteresisTimeout(LowVoltage2State);
  u.OnVoltage(NULL, h_app, oSession);
  FRAMEWORKUNIFIEDLOG(ZONE_POWER_STATEMACHINE, __FUNCTION__,
         "LOW VOLTAGE 2 ENCOUNTERED %s (no switch waiting for hysteresis to)",
         FrameworkunifiedGetMsgSrc(h_app));
}
// LCOV_EXCL_STOP 8: no one send the event to call this function

// LCOV_EXCL_START 8: no one send the event to call this function
void PowerStateMachine::Shutdown::onHysteresisAborted(
    PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  u.NextState(*(u.m_pPreviousState), h_app, oSession);
}
// LCOV_EXCL_STOP 8: no one send the event
void PowerStateMachine::Shutdown::onHysteresisTimeout(
    PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
  if (!u.m_oShutdownHysteresis.maxTries()) {
    FRAMEWORKUNIFIEDLOG(ZONE_POWER_STATEMACHINE, __FUNCTION__,
           "epsmeSHUTDOWN_TM_OUT tries: %d", u.m_oShutdownHysteresis.getTries());

    u.reStartHysteresisTimer(u.m_oShutdownHysteresis);
  } else {
    // Move directorly to next state.
    u.NextState(*(u.m_pOnHysteresisTimeoutState), h_app, oSession);
  }
}

void PowerStateMachine::Shutdown::onEntry(PowerStateMachine &u, HANDLE h_app,
                                          PowerSessionHandler & oSession) {
  if (u.m_oShutdownHysteresis.getEnabled()) {
    FRAMEWORKUNIFIEDLOG(ZONE_POWER_STATEMACHINE, __FUNCTION__,
           "Starting epsmeSHUTDOWN_HYSTERESIS_TM_OUT timeout: %d tries: %d",
           u.m_oShutdownHysteresis.getTimeout(),
           u.m_oShutdownHysteresis.getTries());
    // Set the goto state state.
    u.GotoStateOnHysteresisTimeout(ShutdownActiveState);
    // Start the hysteresis timer for low voltage 2
    u.startHysteresisTimer(u.m_oShutdownHysteresis);
  } else {
    FRAMEWORKUNIFIEDLOG0(ZONE_POWER_STATEMACHINE, __FUNCTION__,
            "SHUTDOWN_HYSTERESIS is not enabled. Going directly to ShutdownActiveState");
    // Move directly to next state.
    u.NextState(ShutdownActiveState, h_app, oSession);
  }
}

void PowerStateMachine::Shutdown::onExit(PowerStateMachine &u, HANDLE h_app,
                                         PowerSessionHandler & oSession) {
  u.stopHysteresisTimer(u.m_oShutdownHysteresis);
  FRAMEWORKUNIFIEDLOG0(ZONE_POWER_STATEMACHINE, __FUNCTION__, "(EXITING) Shutdown State");
}

// LCOV_EXCL_START 8: no one send the event
void PowerStateMachine::Shutdown::onWakeup(PowerStateMachine &u, HANDLE h_app,
                                           PowerSessionHandler & oSession) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  u.GotoStateOnHysteresisTimeout(WakeupPendingState);
}
// LCOV_EXCL_STOP 8: no one send the event

PCSTR PowerStateMachine::ShutdownActive::name() {
  return "Shutdown Active";
}

// LCOV_EXCL_START 8: no one send the event
void PowerStateMachine::ShutdownActive::onLowVoltage1Encountered(
    PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  u.OnVoltage(&LowVoltage1State, h_app, oSession);
  FRAMEWORKUNIFIEDLOG(ZONE_POWER_STATEMACHINE, __FUNCTION__, "LOW VOLTAGE 1 ENCOUNTERED %s",
         FrameworkunifiedGetMsgSrc(h_app));
}
// LCOV_EXCL_STOP 8: no one send the event to call this function

// LCOV_EXCL_START 8: no one send the event to call this function
void PowerStateMachine::ShutdownActive::onLowVoltage2Encountered(
    PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  u.OnVoltage(&LowVoltage2State, h_app, oSession);
  FRAMEWORKUNIFIEDLOG(ZONE_POWER_STATEMACHINE, __FUNCTION__, "LOW VOLTAGE 2 ENCOUNTERED %s",
         FrameworkunifiedGetMsgSrc(h_app));
}
// LCOV_EXCL_STOP 8: no one send the event
void PowerStateMachine::ShutdownActive::onEntry(
    PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
  u.publishPowerLevelType(h_app, epspltSHUTDOWN);

  if (oSession.ShutdownComplete(u.m_lstShutdownModules)) {
    oSession.SendToSupervisor(SS_POWER_SHUTDOWN_COMPLETE, 0, NULL);
  }
}

void PowerStateMachine::ShutdownActive::onShutdown(
    PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
  if (oSession.ShutdownComplete(u.m_lstShutdownModules)) {
    oSession.SendToSupervisor(SS_POWER_SHUTDOWN_COMPLETE, 0, NULL);
  }
}

PCSTR PowerStateMachine::LowVoltage1::name() {  // LCOV_EXCL_START 8: can not be LowVoltage1status
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  return "Low Voltage I";
}
// LCOV_EXCL_STOP 8: can not be LowVoltage1status

// LCOV_EXCL_START 8: no one send the event
void PowerStateMachine::LowVoltage1::onHysteresisAborted(
    PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  u.NextState(*(u.m_pPreviousState), h_app, oSession);
}
// LCOV_EXCL_STOP 8: no one send the event

// LCOV_EXCL_START 8: can not be LowVoltage1status
void PowerStateMachine::LowVoltage1::onHysteresisTimeout(
    PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  if (!u.m_oLowVoltage1Hysteresis.maxTries()) {
    FRAMEWORKUNIFIEDLOG(ZONE_POWER_STATEMACHINE, __FUNCTION__,
           "epsmeLOW_VOLTAGE_HYSTERESIS_TM_OUT tries: %d",
           u.m_oLowVoltage1Hysteresis.getTries());

    u.reStartHysteresisTimer(u.m_oLowVoltage1Hysteresis);
  } else {
    // Move directorly to next state.
    u.NextState(*(u.m_pOnHysteresisTimeoutState), h_app, oSession);
  }
}
// LCOV_EXCL_STOP 8: no one send the event to call this function

// LCOV_EXCL_START 8: no one send the event to call this function
void PowerStateMachine::LowVoltage1::onWakeup(PowerStateMachine &u, HANDLE h_app,
                                              PowerSessionHandler & oSession) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  u.GotoStateOnHysteresisTimeout(NormalVoltageState);
  u.OnVoltage(NULL, h_app, oSession);
  FRAMEWORKUNIFIEDLOG(ZONE_POWER_STATEMACHINE, __FUNCTION__,
         "NORMAL VOLTAGE ENCOUNTERED %s (no switch waiting for hysteresis to)",
         FrameworkunifiedGetMsgSrc(h_app));
}
// LCOV_EXCL_STOP 8: can not be LowVoltage1status

// LCOV_EXCL_START 8: no one send the event
void PowerStateMachine::LowVoltage1::onLowVoltage2Encountered(
    PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  u.GotoStateOnHysteresisTimeout(LowVoltage2State);
  u.OnVoltage(NULL, h_app, oSession);
  FRAMEWORKUNIFIEDLOG(ZONE_POWER_STATEMACHINE, __FUNCTION__,
         "LOW VOLTAGE 2 ENCOUNTERED %s (no switch waiting for hysteresis to)",
         FrameworkunifiedGetMsgSrc(h_app));
}
// LCOV_EXCL_STOP 8: no one send the event to call this function

// LCOV_EXCL_START 8: no one send the event to call this function
void PowerStateMachine::LowVoltage1::onNormalVoltageEncountered(
    PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  u.GotoStateOnHysteresisTimeout(NormalVoltageState);
  u.OnVoltage(NULL, h_app, oSession);
  FRAMEWORKUNIFIEDLOG(ZONE_POWER_STATEMACHINE, __FUNCTION__,
         "NORMAL VOLTAGE ENCOUNTERED %s (no switch waiting for hysteresis to)",
         FrameworkunifiedGetMsgSrc(h_app));
}
// LCOV_EXCL_STOP 8: no one send the event

// LCOV_EXCL_START 8: can not be LowVoltage1status
void PowerStateMachine::LowVoltage1::onEntry(PowerStateMachine &u, HANDLE h_app,
                                             PowerSessionHandler & oSession) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  if (u.m_oLowVoltage1Hysteresis.getEnabled()) {
    FRAMEWORKUNIFIEDLOG(ZONE_POWER_STATEMACHINE, __FUNCTION__,
           "Starting epsmeLOW_VOLTAGE_HYSTERESIS_TM_OUT timeout: %d tries: %d",
           u.m_oLowVoltage1Hysteresis.getTimeout(),
           u.m_oLowVoltage1Hysteresis.getTries());
    // Set the goto state state.
    u.GotoStateOnHysteresisTimeout(LowVoltage1ActiveState);
    // Start the hysteresis timer for low voltage 1
    u.startHysteresisTimer(u.m_oLowVoltage1Hysteresis);
  } else {
    FRAMEWORKUNIFIEDLOG0(ZONE_POWER_STATEMACHINE, __FUNCTION__,
            "LOW_VOLTAGE_HYSTERESIS is not enabled. Going directly to LowVoltage1ActiveState");
    // Move directly to next state.
    u.NextState(LowVoltage1ActiveState, h_app, oSession);
  }
}
// LCOV_EXCL_STOP 8: no one send the event to call this function

// LCOV_EXCL_START 8: no one send the event to call this function
void PowerStateMachine::LowVoltage1::onExit(PowerStateMachine &u, HANDLE h_app,
                                            PowerSessionHandler & oSession) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  u.stopHysteresisTimer(u.m_oLowVoltage1Hysteresis);
  FRAMEWORKUNIFIEDLOG0(ZONE_POWER_STATEMACHINE, __FUNCTION__, "(EXITING) LowVoltage1 State");
}
// LCOV_EXCL_STOP 8: no one send the event to call this function

// LCOV_EXCL_START 8: no one send the event to call this function
PCSTR PowerStateMachine::LowVoltage1Active::name() {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  return "Low Voltage I Active";
}
// LCOV_EXCL_STOP 8: can not be LowVoltage1status

// LCOV_EXCL_START 8: no one send the event
void PowerStateMachine::LowVoltage1Active::onLowVoltage2Encountered(
    PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  u.OnVoltage(&LowVoltage2State, h_app, oSession);
  FRAMEWORKUNIFIEDLOG(ZONE_POWER_STATEMACHINE, __FUNCTION__, "LOW VOLTAGE 2 ENCOUNTERED %s",
         FrameworkunifiedGetMsgSrc(h_app));
}
// LCOV_EXCL_STOP 8: no one send the event to call this function

// LCOV_EXCL_START 8: no one send the event to call this function
void PowerStateMachine::LowVoltage1Active::onNormalVoltageEncountered(
    PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  u.OnVoltage(&NormalVoltageState, h_app, oSession);
  FRAMEWORKUNIFIEDLOG(ZONE_POWER_STATEMACHINE, __FUNCTION__, "NORMAL VOLTAGE ENCOUNTERED %s",
         FrameworkunifiedGetMsgSrc(h_app));
}
// LCOV_EXCL_STOP 8: no one send the event

// LCOV_EXCL_START 8: can not be LowVoltage1Activestatus
void PowerStateMachine::LowVoltage1Active::onEntry(
    PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  u.publishPowerLevelType(h_app, epspltEMERGENCY);
u.publishPowerLVIStatus(h_app, szNTFY_PowerLVI1, ePwSrvLVI_Status_Active);
}
// LCOV_EXCL_STOP 8: no one send the event to call this function

// LCOV_EXCL_START 8: no one send the event to call this function
void PowerStateMachine::LowVoltage1Active::onExit(
  PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
u.publishPowerLVIStatus(h_app, szNTFY_PowerLVI1, ePwSrvLVI_Status_InActive);
}
// LCOV_EXCL_STOP 8: can not be LowVoltage1Activestatus

PCSTR PowerStateMachine::LowVoltage2::name() {  // LCOV_EXCL_START 8: can not be LowVoltage2status
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
return "Low Voltage II";
}
// LCOV_EXCL_STOP 8: can not be LowVoltage2status

// LCOV_EXCL_START 8: no one send the event
void PowerStateMachine::LowVoltage2::onHysteresisAborted(
PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
u.NextState(*(u.m_pPreviousState), h_app, oSession);
}
// LCOV_EXCL_STOP 8: no one send the event

// LCOV_EXCL_START 8: can not be LowVoltage2status
void PowerStateMachine::LowVoltage2::onHysteresisTimeout(
PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
if (!u.m_oLowVoltage2Hysteresis.maxTries()) {
FRAMEWORKUNIFIEDLOG(ZONE_POWER_STATEMACHINE, __FUNCTION__,
       "epsmeLOW_VOLTAGE2_HYSTERESIS_TM_OUT tries: %d",
       u.m_oLowVoltage2Hysteresis.getTries());

u.reStartHysteresisTimer(u.m_oLowVoltage2Hysteresis);
} else {
// Move directorly to next state.
u.NextState(*(u.m_pOnHysteresisTimeoutState), h_app, oSession);
}
}
// LCOV_EXCL_STOP 8: no one send the event to call this function

// LCOV_EXCL_START 8: no one send the event to call this function
void PowerStateMachine::LowVoltage2::onEntry(PowerStateMachine &u, HANDLE h_app,
                                             PowerSessionHandler & oSession) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
if (u.m_oLowVoltage2Hysteresis.getEnabled()) {
FRAMEWORKUNIFIEDLOG(ZONE_POWER_STATEMACHINE, __FUNCTION__,
       "Starting epsmeLOW_VOLTAGE_HYSTERESIS_TM_OUT timeout: %d tries: %d",
       u.m_oLowVoltage2Hysteresis.getTimeout(),
       u.m_oLowVoltage2Hysteresis.getTries());
// Set the goto state state.
u.GotoStateOnHysteresisTimeout(LowVoltage2ActiveState);
// Start the hysteresis timer for low voltage 2
u.startHysteresisTimer(u.m_oLowVoltage2Hysteresis);
} else {
FRAMEWORKUNIFIEDLOG0(ZONE_POWER_STATEMACHINE, __FUNCTION__,
        "LOW_VOLTAGE2_HYSTERESIS is not enabled. Going directly to LowVoltage2ActiveState");
// Move directly to next state.
u.NextState(LowVoltage2ActiveState, h_app, oSession);
}
}
// LCOV_EXCL_STOP 8: no one send the event to call this function

// LCOV_EXCL_START 8: no one send the event to call this function
void PowerStateMachine::LowVoltage2::onExit(PowerStateMachine &u, HANDLE h_app,
                                            PowerSessionHandler & oSession) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
u.stopHysteresisTimer(u.m_oLowVoltage2Hysteresis);
FRAMEWORKUNIFIEDLOG0(ZONE_POWER_STATEMACHINE, __FUNCTION__, "(EXITING) LowVoltage2 State");
}
// LCOV_EXCL_STOP 8: no one send the event to call this function

// LCOV_EXCL_START 8: no one send the event to call this function
void PowerStateMachine::LowVoltage2::onWakeup(PowerStateMachine &u, HANDLE h_app,
                                              PowerSessionHandler & oSession) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
u.GotoStateOnHysteresisTimeout(WakeupPendingState);
}
// LCOV_EXCL_STOP 8: can not be LowVoltage2status

// LCOV_EXCL_START 8: no one send the event
void PowerStateMachine::LowVoltage2::onLowVoltage1Encountered(
PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
u.GotoStateOnHysteresisTimeout(LowVoltage1State);
u.OnVoltage(NULL, h_app, oSession);
FRAMEWORKUNIFIEDLOG(ZONE_POWER_STATEMACHINE, __FUNCTION__,
       "LOW VOLTAGE 1 ENCOUNTERED %s (no switch waiting for hysteresis to)",
       FrameworkunifiedGetMsgSrc(h_app));
}
// LCOV_EXCL_STOP 8: no one send the event to call this function

// LCOV_EXCL_START 8: no one send the event to call this function
void PowerStateMachine::LowVoltage2::onNormalVoltageEncountered(
PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
u.GotoStateOnHysteresisTimeout(NormalVoltageState);
u.OnVoltage(NULL, h_app, oSession);
FRAMEWORKUNIFIEDLOG(ZONE_POWER_STATEMACHINE, __FUNCTION__,
       "NORMAL VOLTAGE ENCOUNTERED %s (no switch waiting for hysteresis to)",
       FrameworkunifiedGetMsgSrc(h_app));
}
// LCOV_EXCL_STOP 8: no one send the event

PCSTR PowerStateMachine::LowVoltage2Active::name() {  // LCOV_EXCL_START 8: can not be LowVoltage2Activestatus
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
return "Low Voltage II Active";
}
// LCOV_EXCL_STOP 8: can not be LowVoltage2Activestatus

// LCOV_EXCL_START 8: no one send the event
void PowerStateMachine::LowVoltage2Active::onLowVoltage1Encountered(
PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
u.OnVoltage(&LowVoltage1State, h_app, oSession);
FRAMEWORKUNIFIEDLOG(ZONE_POWER_STATEMACHINE, __FUNCTION__, "LOW VOLTAGE 1 ENCOUNTERED %s",
       FrameworkunifiedGetMsgSrc(h_app));
}
// LCOV_EXCL_STOP 8: no one send the event to call this function

// LCOV_EXCL_START 8: no one send the event to call this function
void PowerStateMachine::LowVoltage2Active::onNormalVoltageEncountered(
PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
u.OnVoltage(&NormalVoltageState, h_app, oSession);
FRAMEWORKUNIFIEDLOG(ZONE_POWER_STATEMACHINE, __FUNCTION__, "NORMAL VOLTAGE ENCOUNTERED %s",
       FrameworkunifiedGetMsgSrc(h_app));
}
// LCOV_EXCL_STOP 8: no one send the event

// LCOV_EXCL_START 8: can not be LowVoltage2Activestatus
void PowerStateMachine::LowVoltage2Active::onEntry(
PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
u.publishPowerLevelType(h_app, epspltEMERGENCY);
u.publishPowerLVIStatus(h_app, szNTFY_PowerLVI2, ePwSrvLVI_Status_Active);
}
// LCOV_EXCL_STOP 8: no one send the event to call this function

// LCOV_EXCL_START 8: no one send the event to call this function
void PowerStateMachine::LowVoltage2Active::onExit(
PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
u.publishPowerLVIStatus(h_app, szNTFY_PowerLVI2, ePwSrvLVI_Status_InActive);
oSession.SendToSupervisor(SS_POWER_LVI2_SHUTDOWN_COMPLETE, 0, NULL);
}
// LCOV_EXCL_STOP 8: can not be LowVoltage2Activestatus

PCSTR PowerStateMachine::NormalVoltage::name() {  // LCOV_EXCL_START 8: can not be NormalVoltageState
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
return "Normal Voltage";
}
// LCOV_EXCL_STOP 8: can not be NormalVoltageState

// LCOV_EXCL_START 8: no one send the event
void PowerStateMachine::NormalVoltage::onLowVoltage1Encountered(
PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
u.OnVoltage(&LowVoltage1State, h_app, oSession);
FRAMEWORKUNIFIEDLOG(ZONE_POWER_STATEMACHINE, __FUNCTION__, "LOW VOLTAGE 1 ENCOUNTERED %s",
       FrameworkunifiedGetMsgSrc(h_app));
}
// LCOV_EXCL_STOP 8: no one send the event to call this function

// LCOV_EXCL_START 8: no one send the event to call this function
void PowerStateMachine::NormalVoltage::onLowVoltage2Encountered(
PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
u.OnVoltage(&LowVoltage2State, h_app, oSession);
FRAMEWORKUNIFIEDLOG(ZONE_POWER_STATEMACHINE, __FUNCTION__, "LOW VOLTAGE 2 ENCOUNTERED %s",
       FrameworkunifiedGetMsgSrc(h_app));
}
// LCOV_EXCL_STOP 8: no one send the event

// LCOV_EXCL_START 8: can not be NormalVoltageState
void PowerStateMachine::NormalVoltage::onShutdown(
PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
u.NextState(ShutdownState, h_app, oSession);
}
// LCOV_EXCL_STOP 8: can not be NormalVoltageState

// LCOV_EXCL_START 8: no one send the event
void PowerStateMachine::NormalVoltage::onSoftwareUpdate(
PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
u.NextState(SoftwareUpdateState, h_app, oSession);
}
// LCOV_EXCL_STOP 8: no one send the event

// LCOV_EXCL_START 8: can not be NormalVoltageState
void PowerStateMachine::NormalVoltage::onEntry(PowerStateMachine &u,
                                               HANDLE h_app,
                                               PowerSessionHandler & oSession) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
u.publishPowerLevelType(h_app, epspltNORMAL);
}
// LCOV_EXCL_STOP 8: can not be NormalVoltageState

PCSTR PowerStateMachine::SoftwareUpdate::name() {  // LCOV_EXCL_START 8: can not be SoftwareUpdatestatus
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
return "Software Update";
}
// LCOV_EXCL_STOP 8: can not be SoftwareUpdatestatus

// LCOV_EXCL_START 8: no one send the event
void PowerStateMachine::SoftwareUpdate::onLowVoltage1Encountered(
PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
u.OnVoltage(&LowVoltage1State, h_app, oSession);
FRAMEWORKUNIFIEDLOG(ZONE_POWER_STATEMACHINE, __FUNCTION__, "LOW VOLTAGE 1 ENCOUNTERED %s",
       FrameworkunifiedGetMsgSrc(h_app));
}
// LCOV_EXCL_STOP 8: no one send the event to call this function

// LCOV_EXCL_START 8: no one send the event to call this function
void PowerStateMachine::SoftwareUpdate::onLowVoltage2Encountered(
PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
u.OnVoltage(&LowVoltage2State, h_app, oSession);
FRAMEWORKUNIFIEDLOG(ZONE_POWER_STATEMACHINE, __FUNCTION__, "LOW VOLTAGE 2 ENCOUNTERED %s",
       FrameworkunifiedGetMsgSrc(h_app));
}
// LCOV_EXCL_STOP 8: no one send the event

// LCOV_EXCL_START 8: can not be SoftwareUpdatestatus
void PowerStateMachine::SoftwareUpdate::onShutdown(
PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
}
// LCOV_EXCL_STOP 8: no one send the event to call this function

// LCOV_EXCL_START 8: no one send the event to call this function
void PowerStateMachine::SoftwareUpdate::onEntry(
PowerStateMachine &u, HANDLE h_app, PowerSessionHandler & oSession) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
}
// LCOV_EXCL_STOP 8: no one send the event to call this function

// LCOV_EXCL_START 8: no one send the event to call this function
void PowerStateMachine::SoftwareUpdate::onExit(PowerStateMachine &u,
                                               HANDLE h_app,
                                               PowerSessionHandler & oSession) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
}
// LCOV_EXCL_STOP 8: can not be SoftwareUpdatestatus
