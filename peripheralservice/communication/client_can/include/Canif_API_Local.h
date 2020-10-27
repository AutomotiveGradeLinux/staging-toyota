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

#ifndef COMMUNICATION_CLIENT_CAN_INCLUDE_CANIF_API_LOCAL_H_
#define COMMUNICATION_CLIENT_CAN_INCLUDE_CANIF_API_LOCAL_H_

#define CAN_ID_MASK_CODE 0xE0000000UL  // CAN ID Mask Codes
static inline bool Canif_CheckCanID(CANID id) {
  if (id == 0)
    return false;

  // upper 3 bit from MSB should be 0x0
  if ((CAN_ID_MASK_CODE & id) != 0x0)
    return false;

  return true;
}

static inline bool Canif_IsCommand(uint32_t cmd_id) {
  switch (cmd_id) {
  case CAN_CMDID_FUELCALC_RST_REQ_DELIVERY:
  case CAN_CMDID_STARTUP_FIN_REQ_TX:
  case CAN_CMDID_MRST_INFO_REQ_TX:
  case CAN_CMDID_VERSION_REQ_TX:
  case CAN_CMDID_CONNECTION_NODE_REQ_TX:
  case CAN_CMDID_FUELCALC_REQ_TX:
    return true;
  default:
    return false;
  }
}

#endif  // COMMUNICATION_CLIENT_CAN_INCLUDE_CANIF_API_LOCAL_H_
