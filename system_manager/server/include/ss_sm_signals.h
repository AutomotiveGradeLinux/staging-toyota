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
/// \brief    This file supports process termination.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef SS_SM_SIGNALS_H_  // NOLINT
#define SS_SM_SIGNALS_H_

#include <signal.h>

#define  SS_SM_ABORT_SIGNAL     (SIGABRT)
#define  SS_SM_KILL_SIGNAL      (SIGKILL)
#define  SS_SM_TERMINATE_SIGNAL (SIGTERM)

#endif  // SS_SM_SIGNALS_H_  // NOLINT
