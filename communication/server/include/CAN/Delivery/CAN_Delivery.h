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

#ifndef COMMUNICATION_SERVER_INCLUDE_CAN_DELIVERY_CAN_DELIVERY_H_
#define COMMUNICATION_SERVER_INCLUDE_CAN_DELIVERY_CAN_DELIVERY_H_
/******************************************************************************
 * FILE		:CAN_Delivery.h
 * SYSTEM		:_CWORD107_
 * SUBSYSTEM	:
 ******************************************************************************/

#include <peripheral_service/Canif_API.h>
#include "CAN_Thread.h"

/************************************************************************
*			Macro definitions													*
************************************************************************/
#define CAN_CSUM_CHECKTBL_STOP_CODE 0xFFFFFFFFUL
/* CANDataStop code of the table for which checksum checksumming is to be performed */

/************************************************************************
*			Struct definitions													*
************************************************************************/

/************************************************************************
*			Function prototype	  										*
************************************************************************/
RET_CAN CANDataReceiveMsg(HANDLE h_app, CAN_MSG_DATA*);            /* CANDataProcess of Received Messages */
//RET_CAN CANDlcCheck(const T_ICR_CMD_DATA*); /* DLC check process */
CANID CANCanidIfToUserCvt(uint8_t*);                            /* CAN ID Conversion Process 1 */
CANID CANPacCanidIfToUserCvt(uint8_t*);
RET_CAN CANDeliveryRcvData(HANDLE h_app, CANID ul_canid, uint8_t n_ta,
                           uint8_t uc_dlc, uint8_t* puc_data_pos, uint8_t opc);
RET_CAN CANDeliveryRcv_CWORD29_Data(HANDLE h_app, const uint16_t us_opc, uint32_t uc_dlc, uint8_t *puc_data_pos);
#endif  // COMMUNICATION_SERVER_INCLUDE_CAN_DELIVERY_CAN_DELIVERY_H_
