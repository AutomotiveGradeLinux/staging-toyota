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
/// \ingroup  system_manager
/// \file     ss_system_manager_if_local.h
/// \brief    This file supports the System Manager client interface.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef SS_SYSTEM_MANAGER_IF_LOCAL_H_  // NOLINT (build/header_guard)
#define SS_SYSTEM_MANAGER_IF_LOCAL_H_

#define SS_MAX_NUM_MODULES     (128)

// EXIT_SUCCESS         exit succeccfully
// EXIT_FAILURE         exit with failure
// SS_SM_EXIT_RELAUNCH  exit for relaunch request
#define SS_SM_EXIT_RELAUNCH    (2)

/// \ingroup system_manager
/// Message data payload structure from system manager to heartbeat
typedef struct T_SMRequestMsg {
  CHAR           pstModuleName[MAX_QUEUE_NAME_SIZE];
}TSMRequestMessage;

#endif /* SS_SYSTEM_MANAGER_IF_LOCAL_H_ */  // NOLINT (build/header_guard)
