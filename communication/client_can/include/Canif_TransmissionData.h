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

#ifndef COMMUNICATION_CLIENT_CAN_INCLUDE_CANIF_TRANSMISSIONDATA_H_
#define COMMUNICATION_CLIENT_CAN_INCLUDE_CANIF_TRANSMISSIONDATA_H_

#define LAN_TYPE_21PF             "CAN_21PF"

typedef struct {
  uint8_t dlc;
  CAN_DATA_BIT dat;
} CAN_TRANS_START_TABLE_VAL;

typedef struct {
  CANID  canid;
  CAN_TRANS_START_TABLE_VAL  val;
} CAN_INIT_TABLE;

#include "Canif_TransmissionData21PF.h"

static inline bool Canif_InitDataIsDefined21PF(CANID id) {
  int32_t count;
  int i;

  count = sizeof(Can_TransInitData_21PF) /
          sizeof(Can_TransInitData_21PF[0]);
  for (i = 0; i < count; i++) {
    if (id == Can_TransInitData_21PF[i].canid)
      return true;
  }

  return false;
}

#endif  // COMMUNICATION_CLIENT_CAN_INCLUDE_CANIF_TRANSMISSIONDATA_H_
