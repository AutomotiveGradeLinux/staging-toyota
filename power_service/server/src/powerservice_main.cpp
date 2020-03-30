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
/// \brief    Application entry point.
///
///////////////////////////////////////////////////////////////////////////////

#include <system_service/ss_services.h>
#include <system_service/ss_version.h>
#include <system_service/ss_system_if.h>
#include <native_service/ns_version_if.h>
#include <native_service/frameworkunified_dispatcher.h>
#include <native_service/frameworkunified_application.h>
#include <native_service/ns_message_center_if.h>

#include "ss_power_powerservicelog.h"

CFrameworkunifiedVersion g_FrameworkunifiedVersion(MAJORNO, MINORNO, REVISION);

/// \brief: Name of the Queue that will be
/// used to read data from.
const CHAR AppName[] = SERVICE_POWER;

/// \brief: public structure that is used
/// for logging by NSLogger.
FRAMEWORKUNIFIEDLOGPARAM g_FrameworkunifiedLogParams = {
FRAMEWORKUNIFIEDLOGOPTIONS, {
ZONE_TEXT_10, ZONE_TEXT_11, ZONE_TEXT_12,
ZONE_TEXT_13, ZONE_TEXT_14, ZONE_TEXT_15,
ZONE_TEXT_16, ZONE_TEXT_17, ZONE_TEXT_18,
ZONE_TEXT_19, ZONE_TEXT_20, ZONE_TEXT_21,
ZONE_TEXT_22, ZONE_TEXT_23, ZONE_TEXT_24,
ZONE_TEXT_25, ZONE_TEXT_26, ZONE_TEXT_27,
ZONE_TEXT_28, ZONE_TEXT_29, ZONE_TEXT_30,
ZONE_TEXT_31 }, FRAMEWORKUNIFIEDLOGZONES };

/**
 *int main(int argc, char *argv[])
 * @brief Main function for SS_Power subsystem.
 *
 * @param argc number of arguments passed to SS_Power
 * @param argv array of arguments passed to SS_Power
 *
 * @return int status of applications exit.
 */
int main(int argc, char *argv[]) {
  FRAMEWORKUNIFIED_SET_ZONES();
  FrameworkunifiedDefaultCallbackHandler cbFuncs;
  FRAMEWORKUNIFIED_MAKE_DEFAULT_CALLBACK(cbFuncs);

  EFrameworkunifiedStatus eStatus = FrameworkunifiedDispatcherWithArguments(AppName, argc, argv,
                                                  &cbFuncs);
  return eStatus;
}

