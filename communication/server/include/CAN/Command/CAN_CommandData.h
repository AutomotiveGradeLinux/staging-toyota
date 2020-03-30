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

#ifndef COMMUNICATION_SERVER_INCLUDE_CAN_COMMAND_CAN_COMMANDDATA_H_
#define COMMUNICATION_SERVER_INCLUDE_CAN_COMMAND_CAN_COMMANDDATA_H_
/******************************************************************************
 * FILE  :CAN_CommandData.h
 * SYSTEM  :_CWORD107_
 * SUBSYSTEM :
 ******************************************************************************/

#include <native_service/frameworkunified_types.h>

/************************************************************************
*   Macro definitions     *
************************************************************************/
/* Number of table elements */
#define CAN_COMMAND_DELIVERY_SNDID_NUM 16                         /* CAN Command Delivery Registered Delivery Destination ID Max. */
#define CAN_COMMAND_DELIVERY_NUM 6  /* Max. number of CAN command delivery control tables (6 for CAN command) */
#define CAN_COMMAND_TXRSLT_SNDID_NUM 16                           /* Maximum number of registered delivery destination IDs for CAN command transmission results */
#define CAN_COMMAND_TXRSLT_NUM (6 * CAN_COMMAND_TXRSLT_SNDID_NUM) /* Maximum number of CAN command transmission result management tables */

/* Control method */
#define CAN_CMD_DELIVERY_CTRL 0 /* Delivery control type */
#define CAN_CMD_1TIME_TRANCE 1  /* One-time communication */
#define CAN_CMD_TRANCE 2        /* Continuous communication type */

/* Controlling flag */
#define CAN_CMD_RST_NOT_RCV 0 /* Not received */
#define CAN_CMD_RST_RCV 1     /* Received */

/* Resource ID Mask */
#define CAN_CMD_RID_MASK 0x80

/************************************************************************
*   Struct definitions     *
************************************************************************/
/***************************************************
* TAG       : CAN_COMMAND_DELIVERY_LIST_DAT
* ABSTRACT  : CAN command delivery management table structure (1 item)
*             Use in arrays when used 
*             (CAN internal data management table)
****************************************************/
typedef struct {
  uint8_t uc_ctrl;                                                         /* Control method */
  uint8_t uc_flag;                                                         /* Controlling flag */
  uint8_t notify_name_num;                                                  /* Number of registered shipping destinations */
  uint8_t reserve;                                                        /* ----- */
  // uint32_t notifyIdList[CAN_COMMAND_DELIVERY_SNDID_NUM]; /* Addresses for delivery ID */
  char notify_name_list[CAN_COMMAND_DELIVERY_SNDID_NUM][MAX_NAME_SIZE_APP]; /* Destination thread name */
} CAN_COMMAND_DELIVERY_LIST_DAT;

/***************************************************
* TAG       : CAN_COMMAND_TXRSLT_LIST_DAT
* ABSTRACT  : CAN command transmission result management table structure (1 item)
*             Use in arrays when used 
*             (CAN internal data management table)
****************************************************/
typedef struct {
  char notify_name[MAX_NAME_SIZE_APP]; /* Destination thread name */
                                      // uint32_t notifyId; /* Addresses for delivery ID */
  uint8_t uc_rid;                      /* Resources ID */
  uint8_t uc_cmd_id;                    /* CAN command ID */
} CAN_COMMAND_TXRSLT_LIST_DAT;

/************************************************************************
*   Function prototype      *
************************************************************************/
BOOL CANCommandDeliveryEntryCheck(PCSTR name, uint8_t);
void CANCommandDeliveryEntry(PCSTR name, uint8_t);
void CANCommandDeliveryDataGet(uint8_t, CAN_COMMAND_DELIVERY_LIST_DAT *);
void CANCommandDeliveryDelete(uint8_t);
void CANCommandDeliveryDeletesingle(uint8_t, PCSTR);
BOOL CANCommandTxRsltEntryCheck(uint8_t, uint8_t *);
void CANCommandTxRsltEntry(uint8_t, PCSTR name, uint8_t);
void CANCommandTxRsltDataGet(uint8_t, char *, uint8_t *, uint8_t *);
void CANCommandTxRsltDelete(uint8_t);
BOOL CANCommandTxRsltCheck(uint8_t);
BOOL CANCommandFuelCalcRstReqCheck(void);
void CANCommandFuelCalcRstReq(void);
void CANCommandDataInit(void);

#endif  // COMMUNICATION_SERVER_INCLUDE_CAN_COMMAND_CAN_COMMANDDATA_H_
