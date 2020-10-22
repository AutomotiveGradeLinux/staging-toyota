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
/// \brief    This file supports power service session management.
///
///////////////////////////////////////////////////////////////////////////////
#ifndef POWER_SERVICE_SERVER_INCLUDE_SS_POWER_SESSION_H_
#define POWER_SERVICE_SERVER_INCLUDE_SS_POWER_SESSION_H_

#include <system_service/ss_power_service.h>
#include <native_service/frameworkunified_types.h>
#include <map>
#include <string>
#include "ss_power_config.h"

typedef std::string SubscriberName;  // The name of the module that is trying to open a session with Power

/*
 *
 */
class PowerSessionHandler {
 public:
  PowerSessionHandler();
  virtual ~PowerSessionHandler();
  EFrameworkunifiedStatus OpenSesion(HANDLE h_app);
  EFrameworkunifiedStatus CloseSession(HANDLE h_app);
  EFrameworkunifiedStatus StartComplete(HANDLE h_app);
  EFrameworkunifiedStatus StopComplete(HANDLE h_app);

  VOID Print();

  // Methods that do the heavy lifting
  BOOL WakeupComplete(RequiredModuleList & wakeup_modules);  // NOLINT (runtime/references)
  BOOL ShutdownComplete(RequiredModuleList & shutdown_modules);  // NOLINT (runtime/references)
  EFrameworkunifiedStatus SendToSupervisor(UI_32 cmd, UI_32 length, PVOID data);
  BOOL AllClientsInGroupStarted(UI_32 f_ungrpid);
  BOOL ClientPresent(PCSTR f_sclientname);

 private:
  struct PwSessionInfo {
    std::string sz_name;
    std::string sz_servicename;
    HANDLE hsession;
    BOOL frunning;
    EPWR_SESSION_TYPE esessiontype;
    UI_32 ui_groupid;
  };

  typedef std::map<SubscriberName, PwSessionInfo> PwSessionMap;
  typedef PwSessionMap::iterator PwSessionIter;

  PwSessionMap m_mapSessions;
};

#endif  // POWER_SERVICE_SERVER_INCLUDE_SS_POWER_SESSION_H_
