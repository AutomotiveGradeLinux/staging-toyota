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

#ifndef TASK_MANAGER_INCLUDE_SHARE_TSKM_UTIL_H_
#define TASK_MANAGER_INCLUDE_SHARE_TSKM_UTIL_H_

#include  "system_service/tskm_local_type.h"
#include "tskm_main.h"

// inline MACRO
#define TSKM_ERR_CHK(ret, expect, lavel) \
  if (ret != expect) {TSKM_ASSERT(0); goto lavel;}

#define TSKM_ERR_CHK_DFT TSKM_ERR_CHK(tskmRet, TSKM_E_OK, ERROR)

#define TSKM_MAX(a, b) ((a) > (b) ? (a) : (b))

#endif  // TASK_MANAGER_INCLUDE_SHARE_TSKM_UTIL_H_

