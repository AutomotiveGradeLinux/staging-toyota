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

#ifndef COMMUNICATION_SERVER_INCLUDE_CAN_TXMSG_CAN_TXMSG_H_
#define COMMUNICATION_SERVER_INCLUDE_CAN_TXMSG_CAN_TXMSG_H_
/******************************************************************************
 * FILE  :CAN_TxMsg.h
 * SYSTEM  :_CWORD107_
 * SUBSYSTEM :
 ******************************************************************************/

#include "CAN_Thread.h"

/************************************************************************
*   Macro definitions             *
************************************************************************/
#define CAN_TX_COMM_KIND 0x00           /* CANDataCommunication type for transmission */
#define CAN_TX_PHYS_ADRS 0x0000         /* CANDataPhysical address for transmission */
#define CAN_TX_FROM_ADRS 0x00           /* CANDataSource address for sending (wildcard) */
#define CAN_TX_TO_ADRS 0x00             /* CANDataDestination address for sending (wildcard) */

/****** OPC ******/
#define CAN_OPC_RX 0x00               /* CANData reception notice */
#define CAN_OPC_PAC_RX 0x00           /* CAN Packaging Data Reception Notification */
#define CAN_OPC_TX 0x00               /* CANDataReport request */

#define CAN_OPC_COMMAND_STARTUP_FIN_REQ_TX 0x00      /* CAN start completion notification request transmission */
#define CAN_OPC_COMMAND_MRST_INFO_REQ_TX 0x01        /* CAN master reset information notification request transmission */
#define CAN_OPC_COMMAND_VERSION_REQ_TX 0x02          /* CAN Version Request Send */
#define CAN_OPC_COMMAND_CONNECTION_NODE_REQ_TX 0x03  /* CAN connection node notification request transmission */
#define CAN_OPC_COMMAND_BUS_STATUS_REQ_TX 0x04       /* CAN bus status notification request transmission */
#define CAN_OPC_COMMAND_FUELCALC_REQ_TX 0x05         /* CAN section flame reset response transmission */
#define CAN_OPC_COMMAND_STARTUP_FIN_RESP_RX 0x06     /* CAN startup completion notice received */
#define CAN_OPC_COMMAND_MRST_INFO_RESP_RX 0x07       /* CAN Master Reset Information Notification Reception */
#define CAN_OPC_COMMAND_VERSION_RESP_RX 0x08         /* CAN Version Response Reception */
#define CAN_OPC_COMMAND_CONNECTION_NODE_RESP_RX 0x09 /* Receive CAN Connection Node Notification Response */
#define CAN_OPC_COMMAND_BUS_STATUS_RESP_RX 0x10      /* CAN Bus Status Notification Response Reception */
#define CAN_OPC_COMMAND_FUELCALC_RST_REQ_RX 0x11     /* RECEIVE REQUEST FREE OF CAN SECTION */

/************************************************************************
*   Struct definitions             *
************************************************************************/
/***************************************************
* TAG       : CAN_TXMSG_DAT
* ABSTRACT  : CANDataTransmission data section structure
*             (Data structure passed from tha CAN  to router)
****************************************************/
typedef struct {                          /* Type definition of send command */
  uint8_t reserve;                        /* Reserved */
  uint8_t uc_length;                       /* Data length */
  uint8_t uc_kind_padrs;                   /* Data type/Physical address */
  uint8_t uc_p_adrs;                       /* Physical address */
  uint8_t uc_from_adrs;                    /* Source address */
  uint8_t uc_to_adrs;                      /* Forwarding address */
  uint8_t uc_opc;                          /* OPC */
  uint8_t uc_operand[CAN_TX_OPERAND_SIZE]; /* Operand*/
} CAN_TXMSG_DAT;

/***************************************************
* TAG       : CAN_TXMSG
* ABSTRACT  : CANDataTransmission data section structure
*             (Data structure passed from tha CAN  to router)
****************************************************/
typedef struct {                      /* For main processing */
//  T_APIMSG_MSGBUF_HEADER_COMM st_head; /* Header */
  CAN_TXMSG_DAT st_data;               /* Data portion */
} CAN_TXMSG;

/***************************************************
* TAG       : HANDLE_DAT
* ABSTRACT  : MapHandler data-section structures
****************************************************/
typedef struct {
  HANDLE handle;
  BOOL is_q_full; /* Queue full judgment flag */
} HANDLE_DAT;

/************************************************************************
*   Function prototype             *
************************************************************************/
BOOL CANDeliverySndMsg(HANDLE h_app, CANID canid, uint8_t, const uint8_t *, uint8_t, uint8_t);
BOOL CANDelivery_CWORD29_SndMsg(HANDLE h_app, const uint16_t us_opc, uint32_t uc_dlc, const uint8_t *puc_data);
/* CANDataDelivery Messages Sending Process (Except for CANGW) */
BOOL CANDeliverySndMsgToCANGW(HANDLE h_app, CAN_MSG_CANGWDATA *cangw_data);
/* CAN-data-delivery-messaging process to the CANGW */
BOOL CANFreqTransStartTxMsg(HANDLE h_app, uint8_t);                /* CANDataPeriodic transmission start message transmission processing */
BOOL CAN1TimeTransStartTxMsg(HANDLE h_app, uint8_t, const CAN_DATA *); /* CANDataSend one transmission start message processing */
void CANCommWatchSndMsg(HANDLE h_app, uint8_t); /* CANDataTransmission of communication interruption detection/recovery message */
BOOL CANTxRsltSndMsg(HANDLE h_app, uint8_t, uint8_t);          /* CANDataTransmission result notification message transmission processing */
/* _CWORD29_ data transmission result notification message transmission process */
BOOL CAN_CWORD29_TxRsltSndMsg(const HANDLE h_app, const uint8_t uc_can_rid, const uint8_t uc_status);
void CANCanidUserToIfCvt(CANID canid, uint8_t *);            /* CAN ID Conversion Process 2 */

BOOL CANCommandDeliverySndMsg(HANDLE h_app, uint8_t, const uint8_t *);   /* CAN command delivery message transmission processing */
BOOL CANCommandTxRsltSndMsg(HANDLE h_app, uint8_t, uint8_t);  /* CAN command transmission result notification message transmission processing */
BOOL CANCommandTransmissionSndMsg(uint8_t, uint8_t);               /* CAN command message transmission processing */
BOOL CANCommandFuelCalcRstReqSndMsg(HANDLE h_app, PCSTR name);  /* CAN section flame reset request reception message transmission processing */
BOOL CANCommandidOpcToUserCvt(uint8_t, uint8_t *);               /* CAN Command ID Conversion Processing 1 */
BOOL CANCommandidUserToOpcCvt(uint8_t, uint8_t *);               /* CAN command ID conversion process 2 */

BOOL CANTransStartTxMsgCore(HANDLE h_app, CANID canid, uint8_t, CAN_DATA_BIT *);

#endif  // COMMUNICATION_SERVER_INCLUDE_CAN_TXMSG_CAN_TXMSG_H_
