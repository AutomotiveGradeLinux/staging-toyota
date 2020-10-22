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
/// \brief    This file supports power service configuration.
///
///////////////////////////////////////////////////////////////////////////////
#include <string>
#include "ss_power_config.h"

// define all the configuration parmeters that will be used to get data
const char * PowerConfiguration::kPowerLogicPlugin = "PowerLogicPlugin.dll";
const char * PowerConfiguration::kLVI1Timeout = "LVI1.Timeouts_Timeout";
const char * PowerConfiguration::kLVI1HysteresisTimeout =
    "LVI1.Hysteresis_Timeout";
const char * PowerConfiguration::kLVI1HysteresisEnabled =
    "LVI1.Hysteresis_Enabled";
const char * PowerConfiguration::kLVI1HysteresisTries = "LVI1.Hysteresis_Tries";
const char * PowerConfiguration::kLVI2Timeout = "LVI2.Timeouts_Timeout";
const char * PowerConfiguration::kLVI2HysteresisTimeout =
    "LVI2.Hysteresis_Timeout";
const char * PowerConfiguration::kLVI2HysteresisEnabled =
    "LVI2.Hysteresis_Enabled";
const char * PowerConfiguration::kLVI2HysteresisTries = "LVI2.Hysteresis_Tries";
const char * PowerConfiguration::kShutdownTimeout = "Shutdown.Timeouts_Timeout";
const char * PowerConfiguration::kShutdownHysteresisTimeout =
    "Shutdown.Hysteresis_Timeout";
const char * PowerConfiguration::kShutdownHysteresisEnabled =
    "Shutdown.Hysteresis_Enabled";
const char * PowerConfiguration::kShutdownHysteresisTries =
    "Shutdown.Hysteresis_Tries";
/// < defines the names in the cfg file that are used to get required modules names
const char * PowerConfiguration::kRequiredWakeupModules =
    "Wakeup.RequiredModules.";  /// < once all the modules have been wakened up send wake-up complete
const char * PowerConfiguration::kRequiredShutdownModules =
    "Shutdown.RequiredModules.";  /// < once all the modules have been shutdown send shutdown complete
const char * PowerConfiguration::kRequiredLvi2Modules =
    "LVI2.RequiredModules.";  /// < once all the modules have been wakened up send wake-up complete

PowerConfiguration::PowerConfiguration(std::string f_configfilename)
    : m_ConfigFileName(f_configfilename) {
}

PowerConfiguration::~PowerConfiguration() {
}

BOOL PowerConfiguration::LoadParameters(PowerConfigParams & params) {
  EFrameworkunifiedStatus l_eStatus;
  BOOL l_rtnCode = FALSE;

  CNSConfigReader *l_pReaderCfg = new CNSConfigReader();
  if (NULL == l_pReaderCfg) {  // LCOV_EXCL_BR_LINE 5:new error case
    // LCOV_EXCL_START 5:new error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           " Error. new CNSConfigReader() returned NULL pointer.");
    // LCOV_EXCL_STOP 5:new error case
  } else {
    if (eFrameworkunifiedStatusOK != (l_eStatus = l_pReaderCfg->Parse(m_ConfigFileName))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             " Error. l_pReaderCfg->Open(%s) returned NULL pointer.",
             m_ConfigFileName.c_str());
    } else if (eFrameworkunifiedStatusOK
        != (l_eStatus = l_pReaderCfg->GetInt(kShutdownTimeout,
                                             params.shutdown.timeout))) {
      LOG_STATUS_IF_ERRORED(l_eStatus, "l_pReaderCfg->GetInt(ShutdownTimeout)");
    } else if (eFrameworkunifiedStatusOK
        != (l_eStatus = l_pReaderCfg->GetInt(kLVI1Timeout, params.lvi1.timeout))) {
      LOG_STATUS_IF_ERRORED(l_eStatus, "l_pReaderCfg->GetInt(LVI1Timeout)");
    } else if (eFrameworkunifiedStatusOK
        != (l_eStatus = l_pReaderCfg->GetInt(kLVI2Timeout, params.lvi2.timeout))) {
      LOG_STATUS_IF_ERRORED(l_eStatus, "l_pReaderCfg->GetInt(LVI2Timeout)");
    } else if (eFrameworkunifiedStatusOK
        != (l_eStatus = l_pReaderCfg->GetInt(kShutdownHysteresisTimeout,
                                             params.shutdown.hysteresis.timeout))) {
      LOG_STATUS_IF_ERRORED(l_eStatus,
                            "l_pReaderCfg->GetInt(ShutdownHysteresisTimeout)");
    } else if (eFrameworkunifiedStatusOK
        != (l_eStatus = l_pReaderCfg->GetInt(kLVI1HysteresisTimeout,
                                             params.lvi1.hysteresis.timeout))) {
      LOG_STATUS_IF_ERRORED(l_eStatus,
                            "l_pReaderCfg->GetInt(LVI1HysteresisTimeout)");
    } else if (eFrameworkunifiedStatusOK
        != (l_eStatus = l_pReaderCfg->GetInt(kLVI2HysteresisTimeout,
                                             params.lvi2.hysteresis.timeout))) {
      LOG_STATUS_IF_ERRORED(l_eStatus,
                            "l_pReaderCfg->GetInt(LVI2HysteresisTimeout)");
    } else if (eFrameworkunifiedStatusOK
        != (l_eStatus = l_pReaderCfg->GetInt(kShutdownHysteresisTries,
                                             params.shutdown.hysteresis.tries))) {
      LOG_STATUS_IF_ERRORED(l_eStatus,
                            "l_pReaderCfg->GetInt(ShutdownHysteresisTries)");
    } else if (eFrameworkunifiedStatusOK
        != (l_eStatus = l_pReaderCfg->GetInt(kLVI1HysteresisTries,
                                             params.lvi1.hysteresis.tries))) {
      LOG_STATUS_IF_ERRORED(l_eStatus,
                            "l_pReaderCfg->GetInt(LVI1HysteresisTries)");
    } else if (eFrameworkunifiedStatusOK
        != (l_eStatus = l_pReaderCfg->GetInt(kLVI2HysteresisTries,
                                             params.lvi2.hysteresis.tries))) {
      LOG_STATUS_IF_ERRORED(l_eStatus,
                            "l_pReaderCfg->GetInt(LVI2HysteresisTries)");
    } else if (eFrameworkunifiedStatusOK
        != (l_eStatus = l_pReaderCfg->GetBool(
            kShutdownHysteresisEnabled, params.shutdown.hysteresis.enabled))) {
      LOG_STATUS_IF_ERRORED(l_eStatus,
                            "l_pReaderCfg->GetInt(ShutdownHysteresisEnabled)");
    } else if (eFrameworkunifiedStatusOK
        != (l_eStatus = l_pReaderCfg->GetBool(kLVI1HysteresisEnabled,
                                              params.lvi1.hysteresis.enabled))) {
      LOG_STATUS_IF_ERRORED(l_eStatus,
                            "l_pReaderCfg->GetInt(LVI1HysteresisEnabled)");
    } else if (eFrameworkunifiedStatusOK
        != (l_eStatus = l_pReaderCfg->GetBool(kLVI2HysteresisEnabled,
                                              params.lvi2.hysteresis.enabled))) {
      LOG_STATUS_IF_ERRORED(l_eStatus,
                            "l_pReaderCfg->GetInt(LVI2HysteresisEnabled)");
    } else if (eFrameworkunifiedStatusOK
        != (l_eStatus = l_pReaderCfg->GetString(kPowerLogicPlugin,
                                                params.power_logic_plugin))) {
      LOG_STATUS_IF_ERRORED(l_eStatus, "l_pReaderCfg->GetInt(PowerLogicPlugin)");
    } else {
      l_rtnCode = TRUE;
    }

    delete l_pReaderCfg;
  }

  return (l_rtnCode);
}

void PowerConfiguration::LoadDefaultParameters(PowerConfigParams & params) {
  params.power_logic_plugin = "ss_powerlogic.dll";
  params.shutdown.timeout = 600;  // thats 5mins
  params.shutdown.hysteresis.enabled = FALSE;  // There should be hysteresis for Shutdown
  params.shutdown.hysteresis.timeout = 0;  // Don't Care since disabled.
  params.shutdown.hysteresis.tries = 0;  // Don't Care since disabled.
  params.lvi1.timeout = 2;  // thats 2secs
  params.lvi1.hysteresis.enabled = TRUE;  // Enable LVI1 hysteresis
  params.lvi1.hysteresis.tries = 2;  // check twice before switching to LVI1
  params.lvi1.hysteresis.timeout = 500;  // thats 500 msecs
  params.lvi2.timeout = 2;  // thats 2secs
  params.lvi2.hysteresis.enabled = TRUE;  // Enable LVI2 hysteresis
  params.lvi2.hysteresis.tries = 2;  // check twice before switching to LVI2
  params.lvi2.hysteresis.timeout = 100;  // thats 100 msecs
}

void PowerConfiguration::PrintConfigInfo(PowerConfigParams & f_params) {
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "shutdown.timeout: %d",
         f_params.shutdown.timeout);
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "lvi1.timeout:  %d", f_params.lvi1.timeout);
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "lvi2.timeout: %d", f_params.lvi2.timeout);
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "shutdown.hysteresis.timeout: %d",
         f_params.shutdown.hysteresis.timeout);
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "lvi1.hysteresis.timeout: %d",
         f_params.lvi1.hysteresis.timeout);
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "lvi2.hysteresis.timeout: %d",
         f_params.lvi2.hysteresis.timeout);
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "shutdown.hysteresis.tries: %d",
         f_params.shutdown.hysteresis.tries);
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "lvi1.hysteresis.tries: %d",
         f_params.lvi1.hysteresis.tries);
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "lvi1.hysteresis.tries: %d",
         f_params.lvi1.hysteresis.tries);
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "shutdown.hysteresis.enabled: %d",
         f_params.shutdown.hysteresis.enabled);
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "lvi1.hysteresis.enabled: %d",
         f_params.lvi1.hysteresis.enabled);
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "lvi2.hysteresis.enabled: %d",
         f_params.lvi2.hysteresis.enabled);
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "power_logic_plugin: %s",
         f_params.power_logic_plugin.c_str());
  return;
}
