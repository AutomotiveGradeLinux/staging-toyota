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

#ifndef COMMUNICATION_SERVER_INCLUDE_THREADS_CAN_THREAD_H_
#define COMMUNICATION_SERVER_INCLUDE_THREADS_CAN_THREAD_H_
/*!-----------------------------------------------------------------------------
 * @file    CAN_Thread.h
 * @~english
 * @brief   CAN Thread main block header file
 *----------------------------------------------------------------------------*/
#include <peripheral_service/Canif_API.h>
#include <stdlib.h>

#include "com_error_type.h"
#include "ICR_Common.h"
#include "communication_communicationlog.h"

/************************************************************************
*      Macro definitions                          *
************************************************************************/
/* Data size relationship */

#define CAN_TRX_HEADER_SIZE 7     /* Size of send/receive header section (data length excluding operands) */
#define CAN_TRX_HEADER_LEN_SIZE 5 /* Size included in the data length calculation of the send/receive header section */

#define CAN_TRX_CANID_SIZE 4                                                     /* Transmit/Receive CAN ID Size */
#define CAN_TRX_DLC_SIZE 1                                                       /* Transmit/Receive DLC Data Size */
#define CAN_TRX_DLC_RESERVE_SIZE 3          /* Reserved data size after transmit/receive DLC */

#define CAN_RX_CANDATA_SIZE CAN_DATA_SIZE /* Reserve CANData(DATA#1 ~ #x)size */
/* Reserve the structure if it is not a multiple of 4. */
#define CAN_TX_CANDATA_SIZE CAN_DATA_SIZE /* Send CANData(DATA#1 ~ #x)size */
/* Reserve the structure if it is not a multiple of 4. */
#define CAN_RX_OPERAND_SIZE (CAN_TRX_CANID_SIZE + CAN_TRX_DLC_SIZE + CAN_RX_CANDATA_SIZE)
/* Receive operand size */
#define CAN_RX_COMMAND_SIZE (CAN_TRX_HEADER_SIZE + CAN_RX_OPERAND_SIZE)
/* Receive command size */
#define CAN_TX_OPERAND_SIZE (CAN_TRX_CANID_SIZE + CAN_TRX_DLC_SIZE + CAN_TX_CANDATA_SIZE)  // NOLINT(whitespace/line_length)
/* Send operand size */
#define CAN_TX_COMMAND_SIZE (CAN_TRX_HEADER_SIZE + CAN_TX_OPERAND_SIZE)
/* Send command size */
#define CAN_TX_CMD_DELIVERY_SIZE 4     /* Transmitting CAN Command Delivery Data Size Common Block */
#define CAN_TX_CMD_FUELCALC_RST_SIZE 0 /* Transmit CAN Command Delivery Data Size CAN Section Burn Cost Reset Request Receive Data */
#define CAN_TX_CMD_STARTUP_FIN_SIZE 3  /* Transmit CAN Command Delivery Data Size CAN Startup Completion Notification Data */
#define CAN_TX_CMD_MRST_INFO_SIZE 33   /* Transmit CAN Command Delivery Data Size CAN Master Reset Information Notification Receive Data */
#define CAN_TX_CMD_VERSION_SIZE 4      /* Transmit CAN Command Delivery Data Size CAN Version Response Receive Data */
#define CAN_TX_CMD_BUS_STATUS_SIZE 1   /* Transmit CAN Command Delivery Data Size Receive CAN Bus Status Notification Response */

#define CAN_CMDSND_DATA_SIZE 0 /* Command transmission data section size (CAN command control) */

/* Data location relationship */
#define CAN_MSGBUF_CANID_POS 0   /* How many operands the CAN ID contains? */
#define CAN_MSGBUF_DLC_POS 4     /* No. of operand from which DLC contains data? */
#define CAN_MSGBUF_DATA_POS 5    /* How many operands does the DATA contain? */

#define CAN_MSGBUF_PAC_CANID_POS 1 /* How many operands the CAN ID contains? */
#define CAN_MSGBUF_PAC_DLC_POS 3   /* No. of operand from which DLC contains data? */
#define CAN_MSGBUF_PAC_DATA_POS 4  /* How many operands does the DATA contain? */

/* Error definitions */
#define RET_CAN_INIT_SUCCESS RET_NORMAL /* Success in initialization */
#define RET_CAN_INIT_ERROR RET_ERROR    /* Master Clear failed */

#define RET_CAN_NORMAL CANIF_RET_NORMAL                         /* Normal completion */
#define RET_CAN_ERROR_CREATE_EVENT CANIF_RET_ERROR_CREATE_EVENT /* Event generation failure */
#define RET_CAN_ERROR_PARAM CANIF_RET_ERROR_PARAM               /* Specified parameter is invalid */
#define RET_CAN_ERROR_UNDEF CANIF_RET_ERROR_UNDEF               /* Unregistered ID */
#define RET_CAN_ERROR_SNDID CANIF_RET_ERROR_PID                 /* Discrepancy of Destination ID */
#define RET_CAN_ERROR_BUFFULL CANIF_RET_ERROR_BUFFULL           /* Registered FULL (Delivery/Periodic Transmission/Communication Disruption Monitor) */
#define RET_CAN_ERROR_CANIDFULL CANIF_RET_ERROR_CANIDFULL       /* CAN ID type is FULL */
#define RET_CAN_ERROR_TIMER CANIF_RET_ERROR_TIMER               /* Timer acquisition failed */
#define RET_CAN_ERROR_CANCEL CANIF_RET_ERROR_CANCEL             /* Abnormal termination (discard/cancel data) */

/* CANDataControl code used in the transmission/reception relationship */
#define CAN_ID_EXT_FLAG 0x80000000UL /* CAN ID Extended Identity Codes */

/* Timer Relationship */
#define CAN_TIMER_SEQNO_CODE 0x0001       /* CAN thread timer sequence number    */
#define CAN_TIMER_SEQNO_BUFFLOGOUT 0x0002 /* Buffer log output timer No.      */

#define COMM_WATCH_SEQNO_CODE 0x0000 /* Timer Sequence Number Identification Flag of Communication Disruption */
#define FREQ_TRANS_SEQNO_CODE 0x8000 /* Timer Sequence Number Identification Flag for Periodic Transmission */
#define CAN_TIMER_DISTINCTION_CODE 0xF000   /* Timer Sequence Number Identification Flag */

#define CAN_COMTIM_OFF 0x00 /* Timer not started            */
#define CAN_COMTIM_ON 0x01  /* Timer activation state            */

#define CAN_FREQ_TRSNSMISSION_LIST_NUM 32 /* Maximum number of periodic transmission management tables     */
#define COMM_WATCH_LIST_NUM 64            /* Maximum number of communication disruption monitoring management tables     */


/* Maximum number of CAN periodic transmission timer management data    */
#define CAN_FREQTRANS_TIMER_DATA CAN_FREQ_TRSNSMISSION_LIST_NUM
/* Maximum number of CAN disruption monitoring timer management data   */
#define CAN_COMMWATCH_TIMER_DATA COMM_WATCH_LIST_NUM

#define CAN_BUFFER_LOGOUT_NAME "/fs/U0/can_buffer_log.txt" /*     */

#define CAN_USAGE_RATE_THRESHOLD 80 /* Buffer Utilization Threshold (80%)         */

#define CAN_DELIVERY_DLCERR 0xfeff   /* CAN delivery error (DLC)        */
#define CAN_DELIVERY_CSUMERR 0xfefe  /* CAN delivery errors (CSUM)        */
#define CAN_DELIVERY_ERR 0xfefd      /* CAN delivery error (delivery unregistered)    */
#define CAN_TRANCE_ICR_ERR 0xfefc    /* CAN transmission error (ICR)        */
#define CAN_TRANCE_SYSCOM_ERR 0xfefb /* CAN transmission errors (SYSCOM)      */

/*********************************************************************
*                       Type definition
***********************************************************************/
typedef CANIF_RET_API RET_CAN;

typedef enum {
  CAN_PROTOCOL_TYPE_CAN = 0, /* CAN */
  /* -------------------------------------------------------------------- */
  CAN_PROTOCOL_TYPE_TERMINATE /* Mode termination */
} CAN_PROTOCOL_TYPE;

/************************************************************************
*      Function prototype                        *
************************************************************************/
RET_CAN CANMainThreadInit(void);
EFrameworkunifiedStatus CANCallbackForTimeOut(HANDLE);
void CANTimeOutMsg(HANDLE h_app, uint16_t);
void CANCommonTimerStart(void);
void CANBufferLogOut(void);
void CANDebugLogOut(uint16_t, void*);
void CANFreqTransTimeOutMap(void);

/*******************************************************************************
 * MODULE    : CANMsgErrChk
 * ABSTRACT  : Message Send/Receive Error Check Processing
 * FUNCTION  : Check for message transmission and reception errors and perform error processing.
 * ARGUMENT  : RET_API ret_api Message send/receive return value
 *           : u_int16 communication_code  Registration code
 * NOTE      :
 * RETURN    : 
 ******************************************************************************/

inline void CANMsgErrChk(int32_t ret_api, uint16_t communication_code) {
  if (RET_ERROR == ret_api) {  /* In case of an error  */
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "CANMsgErrChk Called.");
    exit(EXIT_FAILURE);
  }
}
#endif  // COMMUNICATION_SERVER_INCLUDE_THREADS_CAN_THREAD_H_
