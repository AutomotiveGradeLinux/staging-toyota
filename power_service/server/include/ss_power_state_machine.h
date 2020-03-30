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
/// \ingroup  tag_PowerService
/// \brief    This file supports the power service state machine.
///
///////////////////////////////////////////////////////////////////////////////
#ifndef POWER_SERVICE_SERVER_INCLUDE_SS_POWER_STATE_MACHINE_H_
#define POWER_SERVICE_SERVER_INCLUDE_SS_POWER_STATE_MACHINE_H_

#include <system_service/ss_power_service.h>
#include "ss_power_state_hysteresis.h"

class PowerSessionHandler;

class PowerStateMachine {
  class Base_State {
   public:
    virtual void onWakeup(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                          PowerSessionHandler & oSession) {  // NOLINT (runtime/references)
    }
    virtual void onStartComplete(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                                 PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    virtual void onLaunchComplete(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                                  PowerSessionHandler & oSession);  // NOLINT (runtime/references)

    // LCOV_EXCL_START 200: virtual function
    virtual void onShutdown(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                            PowerSessionHandler & oSession) {  // NOLINT (runtime/references)
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    }
    // LCOV_EXCL_STOP 200: virtual function
    virtual void onStopComplete(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                                PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    virtual void onShutdownComplete(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                                    PowerSessionHandler & oSession);  // NOLINT (runtime/references)

    // LCOV_EXCL_START 200: virtual function
    virtual void onNormalVoltageEncountered(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                                            PowerSessionHandler & oSession) {  // NOLINT (runtime/references)
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    }
    // LCOV_EXCL_STOP 200: virtual function

    // LCOV_EXCL_START 200: virtual function
    virtual void onLowVoltage1Encountered(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                                          PowerSessionHandler & oSession) {  // NOLINT (runtime/references)
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    }
    // LCOV_EXCL_STOP 200: virtual function

    // LCOV_EXCL_START 200: virtual function
    virtual void onLowVoltage2Encountered(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                                          PowerSessionHandler & oSession) {  // NOLINT (runtime/references)
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    }
    // LCOV_EXCL_STOP 200: virtual function

    // LCOV_EXCL_START 200: virtual function
    virtual void onSoftwareUpdate(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                                  PowerSessionHandler & oSession) {  // NOLINT (runtime/references)
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    }
    // LCOV_EXCL_STOP 200: virtual function

    // LCOV_EXCL_START 200: virtual function
    virtual void onHysteresisAborted(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                                     PowerSessionHandler & oSession) {  // NOLINT (runtime/references)
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    }
    // LCOV_EXCL_STOP 200: virtual function

    // LCOV_EXCL_START 200: virtual function
    virtual void onHysteresisTimeout(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                                     PowerSessionHandler & oSession) {  // NOLINT (runtime/references)
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    }
    // LCOV_EXCL_STOP 200: virtual function
    virtual PCSTR name() = 0;

    /// State Entry and Exit methods

    // LCOV_EXCL_START 200: virtual function
    virtual void onEntry(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                         PowerSessionHandler & oSession) {  // NOLINT (runtime/references)
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    }
    // LCOV_EXCL_STOP 200: virtual function

    // LCOV_EXCL_START 200: virtual function
    virtual void onExit(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                        PowerSessionHandler & oSession) {  // NOLINT (runtime/references)
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    }
    // LCOV_EXCL_STOP 200: virtual function
  };

  friend class Base_State;

  class Wakeup : public Base_State {
   public:
    void onWakeup(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                  PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    void onShutdown(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                    PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    void onNormalVoltageEncountered(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                                    PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    void onLowVoltage1Encountered(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                                  PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    void onLowVoltage2Encountered(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                                  PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    void onSoftwareUpdate(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                          PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    virtual PCSTR name();

    // LCOV_EXCL_START 8: fist status, no entry
    void onEntry(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                 PowerSessionHandler & oSession) {  // NOLINT (runtime/references)
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    }
    // LCOV_EXCL_STOP 8: fist status, no entry
    void onExit(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                PowerSessionHandler & oSession) {  // NOLINT (runtime/references)
    }
  };

  friend class Wakeup;

  class WakeupActive : public Wakeup {
   public:
    void onShutdown(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                    PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    void onLowVoltage1Encountered(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                                  PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    void onLowVoltage2Encountered(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                                  PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    PCSTR name();

    void onEntry(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                 PowerSessionHandler & oSession) {  // NOLINT (runtime/references)
    }
    void onExit(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                PowerSessionHandler & oSession) {  // NOLINT (runtime/references)
    }
  };

  friend class WakeupActive;

  class WakeupPending : public Wakeup {
   public:
    void onWakeup(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                  PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    void onStartComplete(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                         PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    void onLaunchComplete(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                          PowerSessionHandler & oSession);  // NOLINT (runtime/references)

    void onNormalVoltageEncountered(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                                    PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    PCSTR name();

    void onEntry(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                 PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    void onExit(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                PowerSessionHandler & oSession) {  // NOLINT (runtime/references)
    }
  };

  friend class WakeupPending;

  class Shutdown : public Base_State {
   public:
    void onWakeup(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                  PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    void onShutdown(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                    PowerSessionHandler & oSession) {  // NOLINT (runtime/references)
    }
    void onLowVoltage1Encountered(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                                  PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    void onLowVoltage2Encountered(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                                  PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    void onHysteresisAborted(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                             PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    void onHysteresisTimeout(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                             PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    PCSTR name();

    void onEntry(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                 PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    void onExit(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                PowerSessionHandler & oSession);  // NOLINT (runtime/references)
  };

  friend class Shutdown;

  class ShutdownActive : public Base_State {
   public:
    void onShutdown(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                    PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    void onLowVoltage1Encountered(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                                  PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    void onLowVoltage2Encountered(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                                  PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    PCSTR name();

    void onEntry(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                 PowerSessionHandler & oSession);  // NOLINT (runtime/references)
  };

  friend class ShutdownActive;

  class LowVoltage1 : public Base_State {
   public:
    void onWakeup(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                  PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    void onNormalVoltageEncountered(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                                    PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    void onLowVoltage2Encountered(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                                  PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    void onHysteresisAborted(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                             PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    void onHysteresisTimeout(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                             PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    PCSTR name();

    void onEntry(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                 PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    void onExit(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                PowerSessionHandler & oSession);  // NOLINT (runtime/references)
  };

  friend class LowVoltage1;

  class LowVoltage1Active : public Base_State {
   public:
    void onNormalVoltageEncountered(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                                    PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    void onLowVoltage2Encountered(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                                  PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    PCSTR name();

    void onEntry(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                 PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    void onExit(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                PowerSessionHandler & oSession);  // NOLINT (runtime/references)
  };

  friend class LowVoltage1Active;

  class LowVoltage2 : public Base_State {
   public:
    void onWakeup(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                  PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    void onNormalVoltageEncountered(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                                    PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    void onLowVoltage1Encountered(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                                  PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    void onHysteresisAborted(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                             PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    void onHysteresisTimeout(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                             PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    PCSTR name();

    void onEntry(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                 PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    void onExit(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                PowerSessionHandler & oSession);  // NOLINT (runtime/references)
  };

  friend class LowVoltage2;

  class LowVoltage2Active : public Base_State {
   public:
    void onNormalVoltageEncountered(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                                    PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    void onLowVoltage1Encountered(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                                  PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    PCSTR name();

    void onEntry(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                 PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    void onExit(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                PowerSessionHandler & oSession);  // NOLINT (runtime/references)
  };

  friend class LowVoltage2Active;

  class NormalVoltage : public Base_State {
   public:
    void onShutdown(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                    PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    void onLowVoltage1Encountered(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                                  PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    void onLowVoltage2Encountered(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                                  PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    void onSoftwareUpdate(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                          PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    PCSTR name();

    void onEntry(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                 PowerSessionHandler & oSession);  // NOLINT (runtime/references)
  };

  friend class NormalVoltage;

  class SoftwareUpdate : public Base_State {
   public:
    void onShutdown(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                    PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    void onLowVoltage1Encountered(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                                  PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    void onLowVoltage2Encountered(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                                  PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    PCSTR name();

    void onEntry(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                 PowerSessionHandler & oSession);  // NOLINT (runtime/references)
    void onExit(PowerStateMachine &u, HANDLE h_app,  // NOLINT (runtime/references)
                PowerSessionHandler & oSession);  // NOLINT (runtime/references)
  };

  friend class SoftwareUpdate;

 public:
  typedef enum _Power_State_Machine_Events {
    epsmeWAKEUP = epswsPWRON,
    epsmeSHUTDOWN = epscnCANSLEEP,
    epsmeLVI1_ENCOUNTERED = epsvsLVI1,
    epsmeLVI2_ENCOUNTERED = epsvsLVI2,
    epsmeNORMAL_VOLTAGE_ENCOUNTERED = epsvsNORMAL,
    epsmeSTART_COMPLETE = 0xD1,
    epsmeSTOP_COMPLETE = 0xD2,
    epsmeSOFTWARE_UPDATE = 0xE1,
    epsmeSHUTDOWN_HYSTERESIS_ABORTED = 0xF1,
    epsmeLVI1_HYSTERESIS_ABORTED = 0xF2,
    epsmeLVI2_HYSTERESIS_ABORTED = 0xF3,
    epsmeSHUTDOWN_HYSTERESIS_TM_OUT = 0xFA,
    epsmeLVI1_HYSTERESIS_TM_OUT = 0xFB,
    epsmeLVI2_HYSTERESIS_TM_OUT = 0xFC,
  } ePowerStateMachineEvents;

  PowerStateMachine();
  virtual ~PowerStateMachine();
  void onEvent(HANDLE h_app, PowerSessionHandler & oSession,  // NOLINT (runtime/references)
               ePowerStateMachineEvents evt);
  EFrameworkunifiedStatus initialize(HANDLE h_app, PowerConfigParams & refConfigParms);  // NOLINT (runtime/references)
  PCSTR name();

 private:
  static Wakeup WakeupState;
  static WakeupActive WakeupActiveState;
  static WakeupPending WakeupPendingState;
  static LowVoltage1 LowVoltage1State;
  static LowVoltage1Active LowVoltage1ActiveState;
  static LowVoltage2 LowVoltage2State;
  static LowVoltage2Active LowVoltage2ActiveState;
  static Shutdown ShutdownState;
  static ShutdownActive ShutdownActiveState;
  static NormalVoltage NormalVoltageState;
  static SoftwareUpdate SoftwareUpdateState;

  Base_State *m_pCurrentState;
  Base_State *m_pPreviousState;
  Base_State *m_pOnHysteresisTimeoutState;
  PowerStateHysteresis m_oShutdownHysteresis;
  PowerStateHysteresis m_oLowVoltage1Hysteresis;
  PowerStateHysteresis m_oLowVoltage2Hysteresis;
  RequiredModuleList m_lstWakeupModules;
  RequiredModuleList m_lstShutdownModules;
  RequiredModuleList m_lstLvi2Modules;
  HANDLE m_hHysteresisTimer;

  typedef struct _state_info {
    EPWR_POWER_STATE_TYPE wake;
    EPWR_VOLTAGE_STATE_TYPE voltage;
    EPWR_WAKEUP_LEVEL_TYPE level;
    EPWR_WAKEUP_FACTOR_TYPE factor;
    EPWR_CRANK_STATE_TYPE crank;
  } Pwr_StateInfo;

  Pwr_StateInfo m_tStateInfo;

  PowerStateMachine(PowerStateMachine&);
  PowerStateMachine & operator=(PowerStateMachine &);

  /// Helper methods
  void NextState(Base_State & refState, HANDLE h_app,  // NOLINT (runtime/references)
                 PowerSessionHandler & oSession);  // NOLINT (runtime/references)
  void GotoStateOnHysteresisTimeout(Base_State & refState);  // NOLINT (runtime/references)
  void startHysteresisTimer(PowerStateHysteresis & hys_info);  // NOLINT (runtime/references)
  void reStartHysteresisTimer(PowerStateHysteresis & hys_info);  // NOLINT (runtime/references)
  void stopHysteresisTimer(PowerStateHysteresis & hys_info);  // NOLINT (runtime/references)
  void publishPowerLVIStatus(HANDLE h_app, PCSTR nNotifNm, UI_32 value);
  void publishPowerLevelType(HANDLE h_app, UI_32 value);

  void OnVoltage(Base_State * pState, HANDLE h_app,
                 PowerSessionHandler & oSession);  // NOLINT (runtime/references)
};

#endif  // POWER_SERVICE_SERVER_INCLUDE_SS_POWER_STATE_MACHINE_H_
