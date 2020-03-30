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

#ifndef TASK_MANAGER_SERVER_INCLUDE_TSKM_MAIN_H_
#define TASK_MANAGER_SERVER_INCLUDE_TSKM_MAIN_H_

#include <system_service/ss_boot_map.h>

#include "system_service/tskm_svc.h"
#include "system_service/tskm_local_type.h"
#include "tskm_comm.h"
#include "tskm_type_local.h"

typedef struct {
  // State
  TSKM_BOOL_t isExec;                  // Executing flag
  TSKM_STATE_t state;                  // TSKM state

  // API
  TSKM_BOOL_t inApi;                   // API running
  TSKM_ERR_t apiRet;                   // API return
  TSKM_BOOL_t isReboot;                // REBOOT request

  TSKM_BOOL_t isOnStartDone;           // System start process completion flag
  TSKM_BOOL_t isOnStopDone;            // System stop process completion flag

  // Communication
  TSKM_SRV_SOCK_CTX_t sock;            // priLib socket
  int sigFd;                           // signalFd(process-termination-detection)
  HANDLE hApp;                         // DispatcherHandle
  int nsFd;                            // For receiving from the NSFW
  int iFd;                             // inotify FDs
  int resmFd;                          // EV FDs of RESM

  // Non-volatile info
  TSKM_NV_INFO_t nvInfo;               // Non-volatile information

  // Startup info
  T_SS_SM_START_DataStructType bootInfo;          // for Pri notification
  T_SS_SM_START_ExtDataStructType extBootInfo;    // for Pri notification

  // Analysis support
  STATEMGR__CWORD69_LOG* p__CWORD69_Log;         // BUGBUG: Required or not

  // Services management
  TSKM_SVCS_CTX_t svcs;

  // Gradual starting state management
  TSKM_GSTEP_CTX_t wakeup;

  // Gradual terminating state management
  TSKM_GSTEP_CTX_t down;
} TSKM_MAIN_CTX_t;

EFrameworkunifiedStatus OnStart(const HANDLE hApp);  // System start process
TSKM_MAIN_CTX_t* tskm_getMainCtx(void);

#endif  // TASK_MANAGER_SERVER_INCLUDE_TSKM_MAIN_H_
