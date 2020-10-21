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

#ifndef _CAN_HAL_STM_H_
#define _CAN_HAL_STM_H_
#include "can_hal.h"
#ifdef __cplusplus
extern "C" {
#endif
bool TypeIsValid(enum CanHalType type);
bool IsCanHalOpened(enum CanHalType type);
void SetCanHalStateOpen(enum CanHalType type);
void SetCanHalStateClose(enum CanHalType type);
bool IsDeviceEnabled(enum CanHalType type);
void SetDeviceStateEnable(enum CanHalType type);
void SetDeviceStateDisable(enum CanHalType type);
CANHAL_RET_API InvokeStateCallback(enum CanHalType type);
CANHAL_RET_API InvokeErrorCallback(HANDLE h_app, enum CanHalType type);
CANHAL_RET_API CanHalInternalSend(enum CanHalType type, 
                                  const void *msg, ssize_t sz);
CANHAL_RET_API CanHalInternalSend_CWORD118_(enum CanHalType type, 
                                     const void *msg, ssize_t sz);
CANHAL_RET_API CanHalSendStatus(enum CanHalType type, HANDLE h_app,
                                     const void *msg, ssize_t sz);
CANHAL_RET_API CanHalReceiveNotify(enum CanHalType type,
                                     const void *msg, ssize_t sz);
CANHAL_RET_API CanHalCreateInternalThread(HANDLE h_app, enum CanHalType type);
CANHAL_RET_API CanHalDestroyInternalThread(HANDLE h_app, enum CanHalType type);
#ifdef __cplusplus
}
#endif
#endif
