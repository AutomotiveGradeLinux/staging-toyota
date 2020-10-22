
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

 public:
  BOOL IsAGLUnit(void) const { return is_agl_unit; }
  BOOL IsAGLResetHistoryDisable(void) const { return disable_agl_resethistory; }
  BOOL IsNonAGLResetHistoryDisable(void) const { return disable_nonagl_resethistory; }
  void SetAGLUnit(BOOL f_is_agl_unit) { is_agl_unit = f_is_agl_unit; }
  void SetAGLResetHistoryDisable(BOOL f_disable_agl_resethistory) {
    disable_agl_resethistory = f_disable_agl_resethistory;
  }
  void SetNonAGLResetHisoryDisable(BOOL f_disable_nonagl_resethistory) {
    disable_nonagl_resethistory = f_disable_nonagl_resethistory;
  }

 private:
  BOOL        is_agl_unit;
  BOOL        disable_agl_resethistory;
  BOOL        disable_nonagl_resethistory;
} LaunchInfo;

LaunchInfo g_arrLaunchTableCfg[] = {
};
#endif  // SS_SM_LAUNCH_CONFIG_H_  // NOLINT
