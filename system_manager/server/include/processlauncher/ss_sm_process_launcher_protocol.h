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
/// \ingroup  tag_SystemManager
/// \brief    This file provides support for process launching and termination.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef SS_SM_PROCESS_LAUNCHER_PROTOCOL_H_  // NOLINT
#define SS_SM_PROCESS_LAUNCHER_PROTOCOL_H_

#include <native_service/frameworkunified_types.h>
#include <system_service/ss_system_manager_protocol_local.h>
#include <system_service/ss_system_manager_protocol.h>

typedef enum _ESMPLThreadCommandIds {
  //// <
  ePLThrdCmd_LAUNCH_MODULE_REQST = 0x543,  ///< SM --> SM.ProcessLauncher

  //// <
  ePLThrdCmd_LAUNCH_MODULE_RESP,           ///< SM.ProcessLauncher --> SM

  //// <
  ePLThrdCmd_TERMINATE_MODULE_REQST,       ///< SM --> SM.ProcessLauncher

  //// <
  ePLThrdCmd_TERMINATE_MODULE_RESP,        ///< SM.ProcessLauncher --> SM

  //// <
  ePLThrdCmd_RELAUNCH_MODULE_REQST,        ///< SM --> SM.ProcessLauncher

  //// <
  ePLThrdCmd_RELAUNCH_MODULE_RESP,         ///< SM.ProcessLauncher --> SM

  //// <
  ePLThrdCmd_MODULE_STATUS_REQST,          ///< SM --> SM.ProcessLauncher

  //// <
  ePLThrdCmd_THREAD_STATUS_REQST,          ///< SM --> SM.ProcessLauncher

  //// <
  ePLThrdCmd_THREAD_STATUS_RESP,           ///< SM.ProcessLauncher --> SM

  ePLThrdCmd_NONE,                         ///< Not sent. Used for initialization.
} ESMPLThreadCommandIds;


#endif  // SS_SM_PROCESS_LAUNCHER_PROTOCOL_H_  // NOLINT
