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

#ifndef COMMUNICATION_SERVER_INCLUDE_CAN_COMMWATCH_CAN_COMMWATCHDATA_H_
#define COMMUNICATION_SERVER_INCLUDE_CAN_COMMWATCH_CAN_COMMWATCHDATA_H_
/******************************************************************************
 * FILE  :CAN_CommWatchData.h
 * SYSTEM  :_CWORD107_
 * SUBSYSTEM :
 ******************************************************************************/

#include <peripheral_service/Canif_API.h>
#include "CAN_Thread.h"

/************************************************************************
*   Macro definitions             *
************************************************************************/
/* #001 start */
#define COMM_WATICH_TIMER_SEQ_NO_MIN 0x0001 /* Minimum value of timer sequence number */
/* #100 start */
#define COMM_WATICH_TIMER_SEQ_NO_MAX 0x0FFF /* Maximum value of timer sequence number */
/* #001 end */
/* #100 end */

/* Flag Relationship */
#define CAN_COMM_OFF 0x00 /* Monitoring OFF  */
#define CAN_COMM_NORMAL 0x01 /* Communicating     */
#define CAN_COMM_STOP 0x02 /* Interruption of communication */

/************************************************************************
*   Struct definitions             *
************************************************************************/
/***************************************************
* TAG       : CAN_COMM_WATCH_DAT
* ABSTRACT  : Communication interruption monitoring management table structure (1 item)
*             Use in arrays when used 
*             (CAN internal data management table) Not used because API is used in the TBD_Z first game
****************************************************/
typedef struct {                      /* Type definition of communication disconnection monitoring management table (1 case) */
  CANID ul_can_id;                      /* CAN ID */
  DID ul_did; /* Data ID */
  uint16_t us_watch_time;               /* Communication interruption monitoring time (in units of 100ms) */
                                      // uint32_t notifyId;  /* Addresses for delivery ID */
  uint8_t uc_comm_stop;                 /* Communication status */
  uint8_t uc_ig;                       /* IG linkage */
  uint16_t us_timer_seq_no;              /* Timer Sequence Number*/
  char notify_name[MAX_NAME_SIZE_APP]; /* Destination thread name */
} CAN_COMM_WATCH_DAT;

/************************************************************************
*   Variable prototype             *
************************************************************************/
/* Communication disruption monitoring management table */
extern CAN_COMM_WATCH_DAT g_st_comm_watch_dat[CAN_PROTOCOL_TYPE_TERMINATE][COMM_WATCH_LIST_NUM];
/* Number of effective registrations of communication disruption monitoring management table */
extern uint16_t g_us_comm_watch_dat_cnt[CAN_PROTOCOL_TYPE_TERMINATE];

/************************************************************************
*   Function prototype             *
************************************************************************/
void CANCommWatchDataInit(void);
BOOL CANCommWatchCanidDidEntryCheck(uint8_t, CANID canid, DID did, CAN_PROTOCOL_TYPE);
BOOL CANCommWatchTimerSeqNoEntryCheck(uint16_t, uint8_t *, uint8_t *, CAN_PROTOCOL_TYPE);
BOOL CANCommWatchEntryCheck(CANID canid, DID did, PCSTR namel, uint8_t *, CAN_PROTOCOL_TYPE);
void CANCommWatchEntry(uint8_t uc_index, const CAN_COMM_WATCHEXT_MSG_DAT *pst_msg_data,
                       uint16_t us_timer_seq_no, uint8_t uc_comm_stop, CAN_PROTOCOL_TYPE);
void CANCommWatchDelete(uint8_t, CAN_PROTOCOL_TYPE);
void CANCommWatchCtrl(uint8_t, uint8_t, CAN_PROTOCOL_TYPE);
void CANCommWatchDataGet(uint8_t, char *, DID *, uint8_t *, CAN_PROTOCOL_TYPE);
uint16_t CANCommWatchTimerSeqNoGet(uint8_t, CAN_PROTOCOL_TYPE);
uint16_t CANCommWatchTimerSeqNoRenwal(uint8_t, uint16_t *, CAN_PROTOCOL_TYPE);
BOOL CANCommWatchSndCheck(uint8_t, PCSTR name, CAN_PROTOCOL_TYPE);
uint8_t CANCommWatchIgcoopGet(uint8_t uc_index, CAN_PROTOCOL_TYPE);
void CANCommWatchBufferOut(FILE *fp_log, CAN_PROTOCOL_TYPE);
inline BOOL CANCommWatchCanidEntryCheck(uint8_t uc_index, CANID ul_can_id, uint8_t *puc_comm_stop,
                                        uint16_t *pus_chk_cnt, CAN_PROTOCOL_TYPE);

/* for debug */
EFrameworkunifiedStatus CANCommWatchAllClearDebug(HANDLE h_app);
/*******************************************************************************
 * MODULE    : CANCommWatchCanidEntryCheck
 * ABSTRACT  : CANDataCommunication Disruption Monitoring Control Table Registration Status Checking Process (CAN ID Search)
 * FUNCTION  : Compare the communication discontinuity monitoring control data indicated by the index with the specified CAN ID. 
 * ARGUMENT  : uc_index  :Indexed
 *      can_id  :CAN ID
 *      puc_comm_stop :Communication status return pointer
 *      pusChkCnt :Pointer to number of valid data checks for table registration
 * NOTE      : 
 * RETURN    : TRUE  :Data consistency
 *    FALSE  :Data mismatch
 ******************************************************************************/

inline BOOL CANCommWatchCanidEntryCheck(uint8_t uc_index, CANID can_id, uint8_t *puc_comm_stop,
                                         uint16_t *pusChkCnt, CAN_PROTOCOL_TYPE mode) {
  BOOL ret = FALSE;
  CAN_COMM_WATCH_DAT *ptr = &g_st_comm_watch_dat[mode][uc_index];

  /* Is the specified index monitoring interrupted? */
  if ((uint16_t)0 != ptr->us_watch_time) {
    /* Specified index-data CANID matches the arguments? */
    if (can_id == ptr->ul_can_id) {
      ret = TRUE;
      *puc_comm_stop = ptr->uc_comm_stop;
    }
    /* Updating the number of effective data checks for table registration */
    *pusChkCnt = (uint16_t)(*pusChkCnt + (uint16_t)1);
  }

  return (ret);
}

#endif  // COMMUNICATION_SERVER_INCLUDE_CAN_COMMWATCH_CAN_COMMWATCHDATA_H_
