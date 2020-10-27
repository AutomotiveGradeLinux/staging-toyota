/*
 * @copyright Copyright (c) 2020 TOYOTA MOTOR CORPORATION.
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

#ifndef _CAN_HAL_CORE_H_
#define _CAN_HAL_CORE_H_
#include <stdint.h>
#include "can_hal.h"
#ifdef __cplusplus
extern "C" {
#endif
CANHAL_RET_API CanOpenCore(CanHalType type);
CANHAL_RET_API CanCloseCore(CanHalType type);
EFrameworkunifiedStatus CanSendThreadStart(HANDLE h_app);
EFrameworkunifiedStatus CanSendThreadStop(HANDLE h_app);
void *CanRecvRun(void *arg);
void *CanHalPackMessage(HANDLE h_app, const void *msg, ssize_t msg_sz,
                        ssize_t *packed_sz);
void CanHalUnPackMessage(void *packed, HANDLE *h_app, void **msg,
                          ssize_t *msg_sz);
void CanHalDestroyPackedMessage(void *packed);
#ifdef __cplusplus
}
#endif
#endif
