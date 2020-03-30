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
/// \brief    This file supports power service configuration.
///
///////////////////////////////////////////////////////////////////////////////
#ifndef POWER_SERVICE_SERVER_INCLUDE_SS_POWER_CONFIG_H_
#define POWER_SERVICE_SERVER_INCLUDE_SS_POWER_CONFIG_H_


#include <system_service/ss_system_types.h>
#include <system_service/ss_templates.h>
#include <native_service/ns_config_parser_if.h>
#include <list>
#include <string>
#include "ss_power_powerservicelog.h"

typedef std::list<SS_String> RequiredModuleList;
typedef RequiredModuleList::iterator RequiredModuleListIter;

#define SS_PWR__CWORD56__REP_HIST_SIZE     (10u)
#define SS_PWR_PUB_CMD_HIST_SIZE      (5u)
#define SS_PWR_ERR_HIST_SIZE          (5u)
#define SS_PWR_V_HIST_SIZE            (10u)
#define SS_PWR_DEBUG_DUMP_MAX_SIZE    (4096u)

typedef struct _PowerConfigParams {  // NOLINT (readability/naming)  // LCOV_EXCL_BR_LINE 14:it will be called when poweron
  typedef struct {
    SI_32 timeout;
    struct {
      BOOL enabled;
      SI_32 timeout;
      SI_32 tries;
    } hysteresis;
  } PowerOffInfo;

  SS_String power_logic_plugin;
  PowerOffInfo lvi1;
  PowerOffInfo lvi2;
  PowerOffInfo shutdown;
  RequiredModuleList wakeup_modules;
  RequiredModuleList shutdown_modules;
  RequiredModuleList lvi2_modules;
} PowerConfigParams;

class PowerConfiguration {
 public:
  // define all the configuration parmeters that will be used to get data
  static const char * kPowerLogicPlugin;
  static const char * kLVI1Timeout;
  static const char * kLVI1HysteresisTimeout;
  static const char * kLVI1HysteresisEnabled;
  static const char * kLVI1HysteresisTries;
  static const char * kLVI2Timeout;
  static const char * kLVI2HysteresisTimeout;
  static const char * kLVI2HysteresisEnabled;
  static const char * kLVI2HysteresisTries;
  static const char * kShutdownTimeout;
  static const char * kShutdownHysteresisTimeout;
  static const char * kShutdownHysteresisEnabled;
  static const char * kShutdownHysteresisTries;

  /// < defines the names in the xml file that are used to get required modules names
  static const char * kRequiredWakeupModules;  /// < once all the modules have been wakened up send wake-up complete
  static const char * kRequiredShutdownModules;  /// < once all the modules have been shutdown send shutdown complete
  static const char * kRequiredLvi2Modules;  /// < once all the modules have been wakened up send wake-up complete

  ////////////////////////////////////////////////////////////////////////////////////////////
  ///  PowerConfiguration
  ///  \brief Here it is checked whether the new connection is to be connected or not, if yes then whether the old
  ///  connection is  to be disconnected or to be paused or to be played simultaneously.
  ///
  /// \param [in] f_configFileName  Configuration file name.
  ///
  /// \return NA
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  explicit PowerConfiguration(std::string f_configfilename);

  ////////////////////////////////////////////////////////////////////////////////////////////
  ///  ~PowerConfiguration
  ///  \brief Here it is checked whether the new connection is to be connected or not, if yes then whether the old
  ///  connection is  to be disconnected or to be paused or to be played simultaneously.
  ///
  /// \param [in] pRequestingSrc
  ///         Source* - Pointer to the requesting source.
  ///
  /// \return bool
  ///         bool - TRUE or FALSE
  ////////////////////////////////////////////////////////////////////////////////////////////
  virtual ~PowerConfiguration();

  ////////////////////////////////////////////////////////////////////////////////////////////
  ///  LoadParameters
  ///  \brief Load module parameters from configuration file.
  ///
  /// \param [in] params - Ref to variable which to store the loaded module parameters.
  ///
  /// \return bool
  ///         bool - TRUE or FALSE
  ////////////////////////////////////////////////////////////////////////////////////////////
  BOOL LoadParameters(PowerConfigParams & params);  // NOLINT (runtime/references)

  ////////////////////////////////////////////////////////////////////////////////////////////
  ///  LoadDefaultParameters
  ///  \brief Laod default power services parameters.
  ///
  /// \param [in] params - Ref to variable for which to store the default module parameters.
  ///
  /// \return none
  ////////////////////////////////////////////////////////////////////////////////////////////
  void LoadDefaultParameters(PowerConfigParams & params);  // NOLINT (runtime/references)

  ////////////////////////////////////////////////////////////////////////////////////////////
  ///  PrintConfigInfo
  ///  \brief Print configuration information.
  ///
  /// \param [in] params - Ref to variable containing the configuration parameters to load.params
  ///
  /// \return none.
  ////////////////////////////////////////////////////////////////////////////////////////////
  void PrintConfigInfo(PowerConfigParams & f_params);  // NOLINT (runtime/references)

 protected:
  PowerConfiguration(const PowerConfiguration &);
  PowerConfiguration & operator =(const PowerConfiguration &);  // NOLINT (runtime/references)

 private:
  std::string m_ConfigFileName;
};

#endif  // POWER_SERVICE_SERVER_INCLUDE_SS_POWER_CONFIG_H_
