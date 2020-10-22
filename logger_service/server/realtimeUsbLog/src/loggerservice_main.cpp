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
/// \brief    Application entry point.
///
///////////////////////////////////////////////////////////////////////////////
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <system_service/ss_system_if.h>
#include <native_service/frameworkunified_dispatcher.h>
#include <system_service/ss_string_maps.h>
#include <system_service/ss_version.h>
#include <native_service/ns_version_if.h>
#include <system_service/ss_system_types.h>
#include <system_service/ss_logger_store_logs.h>
#include <fstream>
#include <cstdlib>

BOOL   g_isExec = FALSE;

//////////////////////////////////////////
//  Function : FrameworkunifiedOnInitialization
//////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOnInitialization(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  eStatus = StartRtUsbLogThread(hApp);
  return eStatus;
}

//////////////////////////////////////////
//  Function : FrameworkunifiedOnDestroy
//////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOnDestroy(HANDLE hApp) {
  return eFrameworkunifiedStatusOK;
}

//////////////////////////////////////////
//  Function : FrameworkunifiedOnDummy
//////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOnDummy(HANDLE hApp) {
  return eFrameworkunifiedStatusOK;
}

//////////////////////////////////////////
//  Function : main
//////////////////////////////////////////
int main(int argc, char *argv[]) {
  HANDLE hApp;
  int nsFd;
  char clientName[16] = "realtimeUsbLog";
  FrameworkunifiedDefaultCallbackHandler cbFuncs;

  cbFuncs.onInitilization = FrameworkunifiedOnInitialization;
  cbFuncs.onDestroy = FrameworkunifiedOnDestroy;

  cbFuncs.onStart = FrameworkunifiedOnDummy;
  cbFuncs.onStop = FrameworkunifiedOnDummy;
  cbFuncs.onDebugDump = FrameworkunifiedOnDummy;
  cbFuncs.createStateMachine = FrameworkunifiedOnDummy;
  cbFuncs.ssFrameworkInterface = FrameworkunifiedOnDummy;

  FrameworkunifiedCreateDispatcherWithoutLoop(clientName, hApp, argc, argv, &cbFuncs, FALSE);

  FrameworkunifiedGetDispatcherFD(hApp, &nsFd);

  g_isExec = TRUE;
  while (g_isExec) {
    fd_set  fds;
    int maxFd = 0;

    FD_ZERO(&fds);

    FD_SET(nsFd, &fds);
    maxFd = nsFd;

    select(maxFd + 1, &fds, NULL, NULL, NULL);

    if (FD_ISSET(nsFd, &fds)) {
      FrameworkunifiedDispatchProcessWithoutLoop(hApp);
    }
  }

  FrameworkunifiedDestroyDispatcherWithoutLoop(hApp);
  return 0;
}
