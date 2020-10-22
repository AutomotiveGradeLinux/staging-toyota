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
/// \ingroup  tag_SystemManagerIf
/// \brief    This file provides support for the System Manager AGL framework
///           interface.
///
///////////////////////////////////////////////////////////////////////////////

#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_framework_if.h>
#include <string.h>
#include "system_service/ss_system_if.h"
#include "system_service/interfaceunified_system_application.h"
#include "system_service/ss_services.h"
#include "system_service/ss_test_clients.h"
#include "system_service/ss_templates.h"
#include "ss_system_if_interfaceunifiedlog.h"

HANDLE  g_SystemIf_hApp = NULL;

EFrameworkunifiedStatus FrameworkunifiedSSFrameworkInterface(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  g_SystemIf_hApp = hApp;
  PCSTR pAppNameStr = FrameworkunifiedGetAppName(hApp);

  if (0 != strcmp(SERVICE_SYSMANAGER, pAppNameStr) &&
      0 != strcmp(TC_Power, pAppNameStr)) {
    l_eStatus = InterfaceunifiedSystemConnectToSystemManagerService(hApp);
    LOG_STATUS(l_eStatus, "InterfaceunifiedSystemConnectToSystemManagerService()");

    if (0 != strcmp(TC_SysManager, pAppNameStr)) {
      l_eStatus = InterfaceunifiedSystemConnectToHeartBeatService(hApp);
      LOG_STATUS(l_eStatus, "InterfaceunifiedSystemConnectToHeartBeatService()");
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // LCOV_EXCL_BR_LINE 10:Because the last line
