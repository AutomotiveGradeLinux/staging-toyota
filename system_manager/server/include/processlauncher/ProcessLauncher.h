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
/// \brief    This file provides support for process launching and termination.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef CPROCESSLAUNCHER_H_  // NOLINT
#define CPROCESSLAUNCHER_H_

#include <native_service/frameworkunified_types.h>
#include <system_service/ss_system_process.h>
#include <string>
#include "ss_system_manager.h"
#include "ss_sm_process_launcher.h"

/*
 *
 */
class CProcessLauncher {
 public:
  CProcessLauncher();
  explicit CProcessLauncher(void *);
  virtual ~CProcessLauncher();

  EFrameworkunifiedStatus PLOnCmdTerminateModule(HANDLE hThread);
  EFrameworkunifiedStatus PLOnCmdModuleStatus(HANDLE hThread);
  EFrameworkunifiedStatus PLOnCmdLaunchModule(HANDLE hThread);
  EFrameworkunifiedStatus PLOnCmdRelaunchModule(HANDLE hThread);
  EFrameworkunifiedStatus PLOnCmdHeartbeatStatusReq(HANDLE hThread);
  EFrameworkunifiedStatus PLOnCmdStop(HANDLE hThread);
  EFrameworkunifiedStatus PLOnCmdStart(HANDLE hThread);
 protected:
  ProcessMap m_mapOfProcesses;

 private:
  EFrameworkunifiedStatus PLCmdLaunchModule(HANDLE         hThread,
                               UI_32          f_protocol_ID,
                               std::string    f_protocol_str);
  EFrameworkunifiedStatus PLLaunchModule(HANDLE hThread, T_ProcessLaunchResp &f_LaunchRespData);  // NOLINT
  EFrameworkunifiedStatus PLTerminateModule(HANDLE hThread, T_ProcessLauncherTerminationResp *f_pTerminateRespData = NULL);
};
#endif  // CPROCESSLAUNCHER_H_  // NOLINT
