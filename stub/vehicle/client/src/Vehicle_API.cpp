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

/*
 * Information.
 * This source code is a sample source code .
 * Implementation of the function must be performed by the vendor.
 */

#include "Vehicle_API.h"

VEHICLE_RET_API Vehicle_DeliveryEntry(HANDLE h_app, PCSTR notify_name,
                                      uint32_t ul_did, uint8_t uc_ctrl_flg,
                                      uint8_t uc_delivery_timing) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
  return VEHICLE_RET_NORMAL;
}

int32_t Vehicle_GetVehicleData(HANDLE h_app, uint32_t ul_did, void *p_dest_data,
                               uint16_t us_dest_size) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
  return 1;
}

