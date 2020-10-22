/*
 * @copyright Copyright (c) 2019-2020 TOYOTA MOTOR CORPORATION.
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
#ifndef COMMUNICATION_SERVER_INCLUDE_THREADS_THREAD_COMMON_H_
#define COMMUNICATION_SERVER_INCLUDE_THREADS_THREAD_COMMON_H_
#include <stdint.h>
#include <can_hal.h>
#include <string>
#include <sstream>
#include <iostream>

const int32_t CAN_AVAILABILITY = 0x1;

void CommonInit(void);
HANDLE CommonFindSender(HANDLE h_app, std::string s);
BOOL CommGetAvailabilityCurrent(int32_t current);
void CommSetAvailabilityCurrent(int32_t current);
void CommClrAvailabilityCurrent(int32_t current);
EFrameworkunifiedStatus CommonStartNotify(HANDLE h_app, PCSTR cmd);
EFrameworkunifiedStatus CommonThreadStart(HANDLE h_app, const FrameworkunifiedProtocolCallbackHandler *cb,
                      UI_32 count, PCSTR cmd, EFrameworkunifiedStatus (*open_func)(HANDLE));
EFrameworkunifiedStatus CommonThreadStop(HANDLE h_app, const PUI_32 cb, UI_32 count,
                                  PCSTR cmd, EFrameworkunifiedStatus (*close_func)(HANDLE));
EFrameworkunifiedStatus CommonCanHalErrorNotify(HANDLE h_app);

static inline EFrameworkunifiedStatus ConvRet(CANHAL_RET_API ret) {
  return ((ret != CANHAL_RET_NORMAL) ? eFrameworkunifiedStatusFail : eFrameworkunifiedStatusOK);
}
std::string MessageDataOutputLog(uint8_t *msg_data, uint32_t len);
#endif