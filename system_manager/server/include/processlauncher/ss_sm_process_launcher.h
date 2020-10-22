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

#ifndef SS_SM_PROCESS_LAUNCHER_H_  // NOLINT
#define SS_SM_PROCESS_LAUNCHER_H_

#include <native_service/frameworkunified_types.h>
#include <system_service/ss_system_manager_protocol.h>
#include <system_service/ss_system_manager_protocol_local.h>

#include "ss_sm_config.h"

///////////////////////////////////////////////////////////////////////////////
/// System manager launches the modules using the process launcher.
/// The system manager launches the modules by sending the data about the process to be launched by
/// the message ePLThrdCmd_LAUNCH_MODULE_REQST.
/// SM => PL
///////////////////////////////////////////////////////////////////////////////
typedef struct {
  CHAR                 name[SS_SM_MAX_MODULE_NAME_LENGTH];
  CHAR                 path[SS_SM_MAX_MODULE_PATH_LENGTH];
  CHAR                 args[SS_SM_MAX_MODULE_ARGS_LENGTH];
  UI_32                priority;
  CHAR                 logging_mask[SS_SM_MAX_MODULE_LOG_MASK_LENGTH];
  GroupLaunchMapIter   groupIterator;
  ModuleLaunchListIter moduleIterator;
  int                  cpu_assign;
} T_ProcessLauncherLaunchReq;

///////////////////////////////////////////////////////////////////////////////
/// After successfully launching an module, the process launcher sends the
/// pid of the launched process along with the process name and arguments
/// by the message ePLThrdCmd_LAUNCH_MODULE_RESP.
/// System manager will verify the process name and the arguments and update
/// the pid in its process table.
///
/// This message will be sent for every module that is launched by process launcher.
/// Launch success returns a number > 0 for ProcessID, which is the PID of the
/// newly created process.
/// Launch failure returns -1 for ProcessID.
/// PL => SM
///////////////////////////////////////////////////////////////////////////////
/// The response of the module relaunch message from process launcher to system
/// manager ePLThrdCmd_RELAUNCH_MODULE_RESP is defined by the below structure
/// A process ID value of > 0 indicates the success of the operation.
/// The process name and args are used to ensure correct mapping of requests
/// and responses.
/// PL => SM
///////////////////////////////////////////////////////////////////////////////
typedef struct {
  SI_32                pid;
  CHAR                 name[SS_SM_MAX_MODULE_NAME_LENGTH];
  CHAR                 path[SS_SM_MAX_MODULE_PATH_LENGTH];
  CHAR                 args[SS_SM_MAX_MODULE_ARGS_LENGTH];
  UI_32                priority;
  GroupLaunchMapIter   groupIterator;
  ModuleLaunchListIter moduleIterator;
} T_ProcessLaunchResp;

///////////////////////////////////////////////////////////////////////////////
/// System manager terminates the process when it detects there is something
/// wrong with the process. The system manager send the message
/// ePLThrdCmd_TERMINATE_MODULE_REQST to process launcher as required.
/// The request data is defined as below
/// SM => PL
///////////////////////////////////////////////////////////////////////////////
typedef T_ProcessLauncherLaunchReq T_ProcessLauncherTerminationReq;

///////////////////////////////////////////////////////////////////////////////
/// The response of the module termination response message from process
/// launcher to system manager
/// ePLThrdCmd_TERMINATE_MODULE_RESP is defined by the below structure
/// A ProcessID value of 0 indicates the success of the operation.
/// The process name and args are used to ensure correct mapping of requests
/// and responses.
/// PL => SM
///////////////////////////////////////////////////////////////////////////////
typedef T_ProcessLaunchResp T_ProcessLauncherTerminationResp;

///////////////////////////////////////////////////////////////////////////////
/// The system manager requests a module relaunch by the message
/// ePLThrdCmd_RELAUNCH_MODULE_REQST.
/// The message data is described by the below structure
/// The old process ID is passed sent in the request and a new process ID is
/// expected in the response.
/// The process launcher launches the module and does internal retries for this
/// kind of request in case of failure.
/// SM => PL
///////////////////////////////////////////////////////////////////////////////
typedef T_ProcessLauncherLaunchReq T_ProcessLauncherRelaunchReq;

#endif  // SS_SM_PROCESS_LAUNCHER_H_  // NOLINT
