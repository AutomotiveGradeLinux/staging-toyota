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

#ifndef COMMUNICATION_SERVER_INCLUDE_CAN_TRANSMISSION_CAN_TRANSMISSION_H_
#define COMMUNICATION_SERVER_INCLUDE_CAN_TRANSMISSION_CAN_TRANSMISSION_H_
/******************************************************************************
 * FILE		:CAN_Transmission.h
 * SYSTEM		:_CWORD107_
 * SUBSYSTEM	:
 ******************************************************************************/
#include <native_service/frameworkunified_types.h>
#include "CAN_Thread.h"

/************************************************************************
*			Macro definitions													*
************************************************************************/
#define TIMER_SEQNO_MASK_CODE (uint16_t)(0x00FF) /* Mask code for acquiring timer sequence number */

/************************************************************************
*			Struct definitions													*
************************************************************************/

/************************************************************************
*			Function prototype	  										*
************************************************************************/
/* CANDataTransmission start message processing */
EFrameworkunifiedStatus CANTxStart(HANDLE);
/* CANDataSend start message check processing */
RET_CAN CANTxStartMsgCheck(const CAN_TRANSMISSION_START_MSG_DAT *, uint8_t *);
/* CANDataPeriodic transmission stop message processing */
EFrameworkunifiedStatus CANFreqTxStop(HANDLE);
/* CANDataPeriodic transmission time-up message processing */
RET_CAN CANFreqTxTimeupMsg(HANDLE h_app, uint16_t);
/* CANDataReceive message processing of transmission result */
RET_CAN CANTxRsltReceiveMsg(HANDLE h_app, CAN_MSG_DATA *);
/* CANDataSend result received message check processing */
RET_CAN CANTxRsltReceiveMsgCheck(CAN_MSG_DATA *);
/* CANData(_CWORD29_) outgoing message processing */
EFrameworkunifiedStatus CAN_CWORD29_TxMsg(HANDLE h_app);
EFrameworkunifiedStatus CANTxBitStart(HANDLE); /* CANDataTransmission start message processing */
EFrameworkunifiedStatus CANTxBitStop(HANDLE);  /* CANDataPeriodic transmission stop message processing */

#endif  // COMMUNICATION_SERVER_INCLUDE_CAN_TRANSMISSION_CAN_TRANSMISSION_H_
