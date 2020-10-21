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

#include <stub/CommSubCpu_API.h>

COMMSUBCPU_RET_API CommSubCpu_Init() {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return COMMSUBCPU_RET_NORMAL;
}

COMMSUBCPU_RET_API CommSubCpu_DeInit() {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return COMMSUBCPU_RET_NORMAL;
}

COMMSUBCPU_RET_API  CommSubCpu_DeliveryMsgEntry(PCSTR name, uint8_t from, uint8_t to, uint8_t opc,
                                uint8_t  callback_id, uint16_t cid) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return COMMSUBCPU_RET_NORMAL;
}


COMMSUBCPU_RET_API CommSubCpu_Transmission(PCSTR name, T_ICR_CMD *snd_cmd, uint8_t req_id) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return COMMSUBCPU_RET_NORMAL;
}
