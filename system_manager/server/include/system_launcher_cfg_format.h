
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
/// \brief    This file provides support for System Manager process launching.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef SS_SM_LAUNCH_CONFIG_H_  // NOLINT
#define SS_SM_LAUNCH_CONFIG_H_

#include <native_service/frameworkunified_types.h>

typedef struct _tLaunchParams {
  const PCHAR group_name;
  UI_32     group_id;
  UI_32     group_launch_wait;
  const PCHAR name;
  const PCHAR binary_name;
  UI_32     priority;
  BOOL    critical;
  UI_32     retry_cnt;
  const PCHAR arguments;
  const PCHAR logging_msk_str;
  const PCHAR restart;
  BOOL    is_start_required;
  BOOL    shutdown_critical;
  UI_32     shutdown_wait_time;
  UI_32     fast_shutdown_wait_time;
} LaunchInfo;

LaunchInfo g_arrLaunchTableCfg[] = {
};
#endif  // SS_SM_LAUNCH_CONFIG_H_  // NOLINT
