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

#ifndef COMMUNICATION_SERVER_INCLUDE_CAN_TRANSMISSION_CAN_TRANSMISSIONDATA_H_
#define COMMUNICATION_SERVER_INCLUDE_CAN_TRANSMISSION_CAN_TRANSMISSIONDATA_H_
/******************************************************************************
 * FILE        :CAN_TransmissionData.h
 * SYSTEM        :_CWORD107_
 * SUBSYSTEM  :
 ******************************************************************************/
#include <native_service/frameworkunified_types.h>
#include <peripheral_service/Canif_API.h>
#include "Canif_API_Local.h"
/************************************************************************
*      Macro definitions                                                   *
************************************************************************/
/* Data size relationship */
/* Maximum number of transmission results management tables */
#define CAN_1TIME_TRSNSMISSION_RSLT_NUM 16
#define CAN__CWORD29__TRSNSMISSION_RSLT_NUM 16

/* Regular transmission, one transmission result management table relationship */
#define CAN_TXRSLT_CANRID_START 0x00 /* Send result CANRID starting No. */
/* Transmission result (1 transmission) CANRID starting number */
#define CAN_TXRSLT_CANRID_1TIME_START CAN_FREQ_TRSNSMISSION_LIST_NUM
/* Send result (_CWORD29_) CANRID starting number) */
#define CAN_TXRSLT_CANRID__CWORD29__START (CAN_FREQ_TRSNSMISSION_LIST_NUM + CAN_1TIME_TRSNSMISSION_RSLT_NUM)  // NOLINT(whitespace/line_length)
/* Send result CANRID end No. */
#define CAN_TXRSLT_CANRID_END (CAN_FREQ_TRSNSMISSION_LIST_NUM + CAN_1TIME_TRSNSMISSION_RSLT_NUM + CAN__CWORD29__TRSNSMISSION_RSLT_NUM - 1)  // NOLINT(whitespace/line_length)

/* Others */
#define CAN_FREQ_TXRSLT_NUM 1 /* Number of notifications of periodic transmission results */

#define LAN_TYPE_19PF             "CAN_19PF"
/************************************************************************
*        Struct definitions                                                     *
************************************************************************/
/***************************************************
* TAG       : CAN_FREQ_TRANSMISSION_DAT
* ABSTRACT  : Periodic transmission management table structure (1 item)
*             Use in arrays when used 
*             (CAN internal data management table)
****************************************************/
typedef struct {                      /* Type definitions for the periodic transmission management table (1 item) */
  uint16_t us_freq;                    /* Periodic transmission period (in 100ms) */
                                      // uint32_t notifyId; /* Thread ID */
  uint8_t uc_rid;                      /* Resources ID */
  uint8_t uc_resp_num;                  /* Number of transmission result notifications (number of remaining notifications) */
  uint8_t reserve[2];                 /* Reserved */
  CAN_DATA st_can_data;                 /* Transmitted data */
  char notify_name[MAX_NAME_SIZE_APP]; /* Destination thread name */
} CAN_FREQ_TRANSMISSION_DAT;

/***************************************************
* TAG       : CAN_1TIME_TXRSLT_DAT
* ABSTRACT  : One transmission result management table structure (1 item)
*             (CAN internal data management table)
****************************************************/
typedef struct {                      /* Defining the type of delivery destination CAN ID (1 item) */
  CANID ul_canid;                      /* CAN ID */
                                      // uint32_t notifyId; /* Addresses for delivery ID */
  uint8_t uc_rid;                      /* Resources ID */
  uint8_t reserve;                    /* Reserved */
  char notify_name[MAX_NAME_SIZE_APP]; /* Destination thread name */
} CAN_1TIME_TXRSLT_DAT;

/***************************************************
* TAG       : CAN_1TIME_TXRSLT_MSG_DAT
* ABSTRACT  : List of Messages Sent Once (1 item)
*             (CAN-internal Work)
****************************************************/
typedef struct {                      /* Defining the type of delivery destination CAN ID (1 item) */
  CANID ul_canid;                      /* CAN ID */
                                      // uint32_t notifyId; /* Addresses for delivery ID */
  uint8_t uc_rid;                      /* Resources ID */
  uint8_t uc_sts;                      /* Transmission result status */
  char notify_name[MAX_NAME_SIZE_APP]; /* Destination thread name */
} CAN_1TIME_TXRSLT_MSG_DAT;

/***************************************************
* TAG       : CAN_1TIME_TXRSLT_MSG_LIST_DAT
* ABSTRACT  : List of Messages Sent Once (All Messages)
*             (CAN-internal Work)
****************************************************/
typedef struct {                                                    /* Type definition of the delivery destination ID data to which the message is sent */
  int32_t i_num;                                                     /* Number of messages sent */
  CAN_1TIME_TXRSLT_MSG_DAT st_list[CAN_1TIME_TRSNSMISSION_RSLT_NUM]; /* List */
} CAN_1TIME_TXRSLT_MSG_LIST_DAT;

/***************************************************
* TAG       : CAN__CWORD29__TXRSLT_DAT
* ABSTRACT  : _CWORD29_ send result control table structure (1 item)
*             (CAN internal data management table)
****************************************************/
typedef struct {
  uint16_t opc;                        /* OPC */
  uint8_t uc_rid;                      /* Resources ID */
  char notify_name[MAX_NAME_SIZE_APP]; /* Destination thread name */
} CAN__CWORD29__TXRSLT_DAT;

/***************************************************
* TAG       : CAN_TRANS_START_TABLE_VAL
* ABSTRACT  : Transmit data management table structure (1 item)
*             (CAN internal data management table)
****************************************************/
typedef struct {
  uint8_t dlc;
  CAN_DATA_BIT dat;
} CAN_TRANS_START_TABLE_VAL;

/***************************************************
* TAG       : CAN_INIT_TABLE
* ABSTRACT  : Transmission data initial value management table structure
****************************************************/
typedef struct {
  CANID  canid;
  CAN_TRANS_START_TABLE_VAL  val;
} CAN_INIT_TABLE;

/************************************************************************
*			Function prototype	  										*
************************************************************************/
void CANTransmissionDataInit(void);
BOOL CANFreqTransEntryCheck(CANID canid, uint8_t *);
void CANFreqTransEntry(uint8_t, const CAN_TRANSMISSION_START_MSG_DAT *);
void CANFreqTransDataGet(uint8_t uc_index, CAN_FREQ_TRANSMISSION_DAT *pst_data);
BOOL CANFreqTransCanidEntryCheck(uint8_t, CANID);
BOOL CANFreqTransIdEntryCheck(uint8_t, PCSTR);
void CANFreqTransStop(uint8_t);
BOOL CANFreqTransIndexEntryCheck(uint8_t);
BOOL CAN1TimeTransEntryCheck(uint8_t *);
BOOL CAN_CWORD29_TransEntryCheck(uint8_t *);
EFrameworkunifiedStatus CAN1TimeTransEntry(uint8_t, const CAN_TRANSMISSION_START_MSG_DAT *);
EFrameworkunifiedStatus CAN_CWORD29_TransEntry(uint8_t, const CAN__CWORD29__TRANS_MSG *);
BOOL CANTxRsltEntryCheck(uint8_t);
void CANTxRsltDataGet(uint8_t, CAN_1TIME_TXRSLT_DAT *);
void CAN_CWORD29_TxRsltDataGet(const uint8_t, CAN__CWORD29__TXRSLT_DAT* const);
void CANTxRsltFin(uint8_t);
void CAN_CWORD29_TxRsltFin(const uint8_t);
BOOL CANTxRsltSndCheck(uint8_t, uint32_t);
void CANCycleTransBufferOut(FILE *fp_log);

BOOL CANTransDataEntry(CAN_TRANS_START_MSG_DAT *);
BOOL CANTransStartTxMsg(HANDLE h_app, CANID);
BOOL CANEntryTransCanidCheck(CANID);

BOOL CAN_SubIDTransStartTxMsg(HANDLE h_app, CAN_TRANS_START_MSG_DAT *);

#endif  // COMMUNICATION_SERVER_INCLUDE_CAN_TRANSMISSION_CAN_TRANSMISSIONDATA_H_
