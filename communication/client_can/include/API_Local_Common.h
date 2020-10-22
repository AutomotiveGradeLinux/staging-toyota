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

#ifndef COMMUNICATION_CLIENT_CAN_INCLUDE_API_LOCAL_COMMON_H_
#define COMMUNICATION_CLIENT_CAN_INCLUDE_API_LOCAL_COMMON_H_
#include <string.h>
enum CANIF_PROTOCOL_IDX {
  CANIF_CAN = 0,
  NUM_OF_CANIF_PROTOCOL
};

static inline const char *Canif_PidxTosname(enum CANIF_PROTOCOL_IDX idx) {
  switch (idx) {
  case CANIF_CAN:
    return LAN_SERVICE_CAN;
    break;
  default:
    return "Unknown";
    break;
  }
}

#define CANIF_NOTIFY_NAME_MAX_SIZE 15
static inline bool Canif_CheckNotifyName(PCSTR notify_name) {
  size_t n = CANIF_NOTIFY_NAME_MAX_SIZE + 1;

  if (!notify_name)
    return false;

  return (strnlen(notify_name, n) < n);
}

static inline bool Canif_CheckNotifyNameWithoutNullCheck(PCSTR notify_name) {
  size_t n = CANIF_NOTIFY_NAME_MAX_SIZE + 1;
  return (strnlen(notify_name, n) < n);
}

static inline void Canif_CopyNotifyName(char *dest, PCSTR notify_name) {
  strncpy(dest, notify_name,
          strnlen(notify_name, CANIF_NOTIFY_NAME_MAX_SIZE));
}
#endif
