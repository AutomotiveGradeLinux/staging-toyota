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

#ifndef COMMUNICATION_SERVER_INCLUDE_CAN_TXMSG_CAN_TXMSG_H_
#define COMMUNICATION_SERVER_INCLUDE_CAN_TXMSG_CAN_TXMSG_H_
#include <vector>
#include <map>
#include <string>
#include <can_hal.h>
#include "CAN_Thread.h"
#include "API_Local_Common.h"
#include "Canif_API_Local.h"


#define CAN_OPC_PAC_RX 0x00
#define CAN_OPC_COMMAND_STARTUP_FIN_REQ_TX 0x00      // CAN start completion notification request transmission
#define CAN_OPC_COMMAND_MRST_INFO_REQ_TX 0x01        // CAN master reset information notification request transmission
#define CAN_OPC_COMMAND_VERSION_REQ_TX 0x02          // CAN Version Request Send
#define CAN_OPC_COMMAND_CONNECTION_NODE_REQ_TX 0x03  // CAN connection node notification request transmission
#define CAN_OPC_COMMAND_BUS_STATUS_REQ_TX 0x04       // CAN bus status notification request transmission
#define CAN_OPC_COMMAND_FUELCALC_REQ_TX 0x05         // CAN section flame reset response transmission
#define CAN_OPC_COMMAND_STARTUP_FIN_RESP_RX 0x06     // CAN startup completion notice received
#define CAN_OPC_COMMAND_MRST_INFO_RESP_RX 0x07       // CAN Master Reset Information Notification Reception
#define CAN_OPC_COMMAND_VERSION_RESP_RX 0x08         // CAN Version Response Reception
#define CAN_OPC_COMMAND_CONNECTION_NODE_RESP_RX 0x09 // Receive CAN Connection Node Notification Response
#define CAN_OPC_COMMAND_BUS_STATUS_RESP_RX 0x10      // CAN Bus Status Notification Response Reception
#define CAN_OPC_COMMAND_FUELCALC_RST_REQ_RX 0x11     // RECEIVE REQUEST FREE OF CAN SECTION

typedef struct {
  uint8_t rid;
  char notify_name[CANIF_NOTIFY_NAME_MAX_SIZE + 1];
} CAN_SEND_STATUS_DAT;

void CANTxMsgInit(void);
EFrameworkunifiedStatus CANTxMsg(HANDLE h_app);
EFrameworkunifiedStatus CANTxMsgBit(HANDLE h_app);
void CANCommandSetFuelCalcRstReq(void);
EFrameworkunifiedStatus CANTxMsgCommand(HANDLE h_app);
EFrameworkunifiedStatus CANSndStsProcess(HANDLE h_app, CanSendResult *rcv_msg,
                                        PS_CommunicationProtocol cid); 
#endif  // COMMUNICATION_SERVER_INCLUDE_CAN_TXMSG_CAN_TXMSG_H_
