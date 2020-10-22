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
#include <system_service/ss_power_service_protocol.h>
#include <system_service/ss_power_service.h>
#include <native_service/frameworkunified_framework_if.h>
#include "ss_power_powerservicelog.h"

EFrameworkunifiedStatus Power::OnSystemLaunchComplete(HANDLE h_app) {
  FRAMEWORKUNIFIEDLOG0(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
         "Power Service received SS_POWER_SYSTEM_LAUNCH_COMPLETE from: %s",
         FrameworkunifiedGetMsgSrc(h_app));

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "- return EFrameworkunifiedStatus: 0x%X", eStatus);
  return eStatus;
}

// test from peripheralservice/power_supply_manager_shadow
EFrameworkunifiedStatus Power::OnSystemShutdownComplete(HANDLE h_app) {
  FRAMEWORKUNIFIEDLOG0(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
         "Power Service received SS_POWER_SYSTEM_SHUTDOWN_COMPLETE from: %s",
         FrameworkunifiedGetMsgSrc(h_app));

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "- return EFrameworkunifiedStatus: 0x%X", eStatus);
  return eStatus;
}
