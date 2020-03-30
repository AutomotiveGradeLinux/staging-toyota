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

/*******************************************************************************
 * FILE  :CAN_CommWatchData.cpp
 * SYSTEM  :_CWORD107_
 * SUBSYSTEM :EXL process
 * PROGRAM :CAN thread CAN data communication interruption monitoring data management
 * Module configuration :Can_CommWatchData_Init() CAN Data Communication Disruption Monitoring Management Data Initialization Process
 *     :CANCommWatchCanidDidEntryCheck()  CANDataCommunication Disruption Monitoring Control Table Registration Status Checking Process (CAN ID and DataID Retrieval)
 *     :CANCommWatchTimerSeqNoEntryCheck() CANDataCommunication Disruption Monitoring Management Table Registration Status Checking Process (Timer Sequence Number Search)
 *     :CANCommWatchEntryCheck()    CANDataCommunication Disruption Monitoring Management Table Registration Destination Confirmation Process
 *     :CANCommWatchEntry()     CANDataCommunication Disruption Monitoring Management Table Registration Process
 *     :CANCommWatchDelete()     CANDataCommunication Disruption Monitoring Management Table Deletion Processing
 *     :CANCommWatchCtrl()     Communication interruption state control processing
 *     :CANCommWatchDataGet()    Communication interruption data acquisition process
 *     :CANCommWatchTimerSeqNoGet()   Acquisition of Communication Disruption Monitor Timer Sequence Number
 *     :CANCommWatchTimerSeqNoRenwal()  Communication interruption monitoring timer sequence number update processing
 *     :CANCommWatchSndCheck()    CANDataCommunication Disruption Monitoring Management Table Delivery Destination ID Confirmation Process
 *     :CANCommWatchIgcoopGet()    IG linkage information acquisition processing of the communication disruption monitoring management table
 ******************************************************************************/
#include "CAN_CommWatchData.h"

#include <string.h>  // NOLINT (build/include)
#include <native_service/frameworkunified_types.h>
#include <other_service/strlcpy.h>

#include <peripheral_service/Canif_API.h>
#include "CAN_Thread.h"

/*************************************************/
/*           Global variables                      */
/*************************************************/
/* Timer sequence number increment counter */
static uint16_t g_us_comm_watch_timer_seq_no = COMM_WATICH_TIMER_SEQ_NO_MIN;
CAN_COMM_WATCH_DAT g_st_comm_watch_dat[CAN_PROTOCOL_TYPE_TERMINATE][COMM_WATCH_LIST_NUM]; /* Communication disruption monitoring management table */
uint16_t g_us_comm_watch_dat_cnt[CAN_PROTOCOL_TYPE_TERMINATE];  /* Number of effective registrations of communication disruption monitoring management table */

/*******************************************************************************
 * MODULE    : CANCommWatchDataInit
 * ABSTRACT  : CANDataCommunication Disruption Monitoring Management Data Initialization Process
 * FUNCTION  : CANDataInitializing management data for communication disruption monitoring 
 * ARGUMENT  : void
 * NOTE      : 
 * RETURN    : void
 ******************************************************************************/
void CANCommWatchDataInit(void) {
  for (int i = 0; i < CAN_PROTOCOL_TYPE_TERMINATE; i++) {
    /* Communication disruption monitoring management table */
    memset(g_st_comm_watch_dat[i], 0x00, sizeof(CAN_COMM_WATCH_DAT) * COMM_WATCH_LIST_NUM);
    g_us_comm_watch_dat_cnt[i] = 0;            /* Number of effective registrations of communication disruption monitoring management table */
  }
  return;
}

/*******************************************************************************
 * MODULE    : CAN_CommWatchCanidDidIDEntryCheck
 * ABSTRACT  : CANDataCommunication Disruption Monitoring Control Table Registration Status Checking Process (CAN ID and DataID Retrieval)
 * FUNCTION  : Compares the specified indexes of the communication interruption monitoring control table with the CAN ID and data IDs. 
 * ARGUMENT  : uc_index  :Indexed
 *      can_id  :CAN ID
 *      ul_did  :Data ID
 * NOTE      : 
 * RETURN    : TRUE  :Data consistency
 *    FALSE  :Data mismatch
 ******************************************************************************/
BOOL CANCommWatchCanidDidEntryCheck(uint8_t uc_index, CANID can_id, DID ul_did, CAN_PROTOCOL_TYPE mode) {
  BOOL ret = FALSE;
  CAN_COMM_WATCH_DAT *ptr = &g_st_comm_watch_dat[mode][uc_index];

  /* Is the specified index monitoring interrupted? */
  if ((uint16_t)0 != ptr->us_watch_time) {
    /* The CANID and data identifier of the named index data matches the arguments? */
    if (can_id == ptr->ul_can_id) {
      if (ul_did == ptr->ul_did) {
        ret = TRUE;
      }
    }
  }

  return (ret);
}

/*******************************************************************************
 * MODULE    : CANCommWatchTimerSeqNoEntryCheck
 * ABSTRACT  : CANDataCommunication Disruption Monitoring Management Table Registration Status Checking Process (Timer Sequence Number Search)
 * FUNCTION  : Returns the index of the communication interruption monitoring management table with the specified timer sequence number 
 * ARGUMENT  : us_timer_seq_no :Search timer sequence number
 *      puc_index  :Index return pointer
 *      puc_comm_stop :Communication status return pointer
 * NOTE      : 
 * RETURN    : TRUE  :Registered
 *    FALSE  :No registration
 ******************************************************************************/
BOOL CANCommWatchTimerSeqNoEntryCheck(uint16_t us_timer_seq_no, uint8_t *puc_index,
                                      uint8_t *puc_comm_stop, CAN_PROTOCOL_TYPE mode) {
  uint32_t ul_cnt;
  BOOL ret = FALSE;
  CAN_COMM_WATCH_DAT *ptr = &g_st_comm_watch_dat[mode][0];

  /* Timer Sequence Number Search */
  for (ul_cnt = 0; ul_cnt < COMM_WATCH_LIST_NUM; ul_cnt++) {
    /* Is monitoring interrupted? */
    if ((uint16_t)0 != ptr[ul_cnt].us_watch_time) {
      /* Match Timer Sequence Number */
      if (us_timer_seq_no == ptr[ul_cnt].us_timer_seq_no) {
        ret = TRUE;
        *puc_index = (uint8_t)ul_cnt;
        *puc_comm_stop = ptr[ul_cnt].uc_comm_stop;

        break;
      }
    }
  }

  return (ret);
}

/*******************************************************************************
 * MODULE    : CANCommWatchEntryCheck
 * ABSTRACT  : CANDataCommunication Disruption Monitoring Management Table Registration Destination Confirmation Process
 * FUNCTION  : Returns the registerable index in the communication disconnection monitoring management table 
 * ARGUMENT  : can_id  :CAN ID
 *      ul_did  :Data ID
 *      notifyId  :Addresses for delivery ID
 *      puc_index  :Index return pointer
 * NOTE      : 
 * RETURN    : TRUE  :Can be registered
 *    FALSE  :Unable to register
 ******************************************************************************/
BOOL CANCommWatchEntryCheck(CANID can_id, DID ul_did, PCSTR notify_name, uint8_t *puc_index, CAN_PROTOCOL_TYPE mode) {
  uint32_t ul_cnt;
  BOOL ret = FALSE;
  CAN_COMM_WATCH_DAT *ptr = &g_st_comm_watch_dat[mode][0];

  /* Search duplicate data */
  for (ul_cnt = 0; ul_cnt < COMM_WATCH_LIST_NUM; ul_cnt++) {
    /* CANID, DataID, Shipping ID Matching? */
    if ((can_id == ptr[ul_cnt].ul_can_id) &&
        (ul_did == ptr[ul_cnt].ul_did) &&
        (strcmp(notify_name, ptr[ul_cnt].notify_name) == 0)) {
      ret = TRUE;
      *puc_index = (uint8_t)ul_cnt;

      break;
    }
  }
  if (FALSE == ret) {
    /* Search for free table */
    for (ul_cnt = 0; ul_cnt < COMM_WATCH_LIST_NUM; ul_cnt++) {
      /* Free (communication interruption time = 0)? */
      if ((uint16_t)0 == ptr[ul_cnt].us_watch_time) {
        ret = TRUE;
        *puc_index = (uint8_t)ul_cnt;
        break;
      }
    }
  }

  return (ret);
}

/*******************************************************************************
 * MODULE    : CANCommWatchEntry
 * ABSTRACT  : CANDataCommunication Disruption Monitoring Management Table Registration Process
 * FUNCTION  : Register data in the communication interruption monitoring management table 
 * ARGUMENT  : uc_index  :Index of registration destination
 *      ul_did  :Data to register ID
 *      notifyId  :Process ID to register
 *      ulCanid  :CAN ID to register
 *      usWatchTime :Communication interruption time to register
 *      ucIg   :IG linkage flag to be registered
 *      us_timer_seq_no :Timer sequence number to register
 * NOTE      : 
 * RETURN    : void
 ******************************************************************************/
void CANCommWatchEntry(uint8_t uc_index, const CAN_COMM_WATCHEXT_MSG_DAT *pst_msg_data,
                       uint16_t us_timer_seq_no, uint8_t uc_comm_stop, CAN_PROTOCOL_TYPE mode) {
  CAN_COMM_WATCH_DAT *ptr = &g_st_comm_watch_dat[mode][uc_index];

  /* Confirm whether overwriting or addition of communication disruption monitoring management table */
  if ((uint16_t)0 == ptr->us_watch_time) {
    /* In case of additional registration, update the number of registered data */
    g_us_comm_watch_dat_cnt[mode]++;
  }
  /* Register new monitoring data in communication interruption monitoring management table */
  strlcpy(ptr->notify_name, pst_msg_data->notifyName, sizeof(ptr->notify_name)); /* Store Destination Thread Name */
  ptr->ul_did = pst_msg_data->ulDid;                                             /* Data ID storage */
  ptr->ul_can_id = pst_msg_data->ulCanid;                                        /* CAN ID storage */
  ptr->us_watch_time = pst_msg_data->usWatchTime;                                /* Communication interruption time storage */
  ptr->uc_comm_stop = uc_comm_stop;                                              /* Communication state storage  */
  ptr->uc_ig = pst_msg_data->ucIg;                                               /* IG linkage information storage */
  ptr->us_timer_seq_no = us_timer_seq_no;                                         /* Store timer sequence number */
  /* Log output */
  // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"  //NOLINT (readability/naming)
  FRAMEWORKUNIFIEDLOG(ZONE_CAN_DEBUG, __func__,
         "##### COMMWaitTable [Index:0x%04x, SndId:0x%s, Did:0x%08x, CanID:0x%08x, Tim:0x%04x, Comm:0x%04x, IG:0x%04x, TimSeq:0x%04x] ##### ",  // NOLINT (whitespace/line_length)
         uc_index, ptr->notify_name, ptr->ul_did, ptr->ul_can_id,
         ptr->us_watch_time, ptr->uc_comm_stop, ptr->uc_ig, ptr->us_timer_seq_no);
  // LCOV_EXCL_BR_STOP 15:marco defined in "native_service/ns_logger_if.h"  //NOLINT (readability/naming)
  return;
}

/*******************************************************************************
 * MODULE    : CANCommWatchDelete
 * ABSTRACT  : CANDataCommunication Disruption Monitoring Management Table Deletion Processing
 * FUNCTION  : Delete the data in the communication disconnection monitoring management table. 
 * ARGUMENT  : uc_index  :Destination index
 * NOTE      : 
 * RETURN    : void
 ******************************************************************************/
void CANCommWatchDelete(uint8_t uc_index, CAN_PROTOCOL_TYPE mode) {
  CAN_COMM_WATCH_DAT *ptr = &g_st_comm_watch_dat[mode][uc_index];

  /* To 0 all members of the data for the specified index */
  memset(ptr->notify_name, 0x00, sizeof(ptr->notify_name)); /* Initialization of delivery destination thread name */
  ptr->ul_did = 0;                                          /* Data ID initialization */
  ptr->ul_can_id = 0;                                       /* CAN ID initialization */
  ptr->us_watch_time = 0;                                   /* Communication interruption time initialization */
  ptr->uc_comm_stop = 0;                                    /* Communication status initialization */
  ptr->uc_ig = 0;                                           /* Initialization of IG linkage information */
  ptr->us_timer_seq_no = 0;                                 /* Initialization of timer sequence number */

  if ((uint16_t)0 != g_us_comm_watch_dat_cnt[mode]) {
    g_us_comm_watch_dat_cnt[mode]--;                        /* Updating the number of registered data */
  }
  /* Log output */
  FRAMEWORKUNIFIEDLOG(ZONE_CAN_DEBUG, __func__, "##### COMMWaitTableDel [Index:0x%04x] #####", uc_index);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  //NOLINT (readability/naming)

  return;
}

/*******************************************************************************
 * MODULE    : CANCommWatchCtrl
 * ABSTRACT  : Communication interruption state control processing
 * FUNCTION  : Setting communication interruption status 
 * ARGUMENT  : uc_index  :Index to set
 *      uc_comm_stop :Communication status to be set
 * NOTE      : 
 * RETURN    : void
 ******************************************************************************/
void CANCommWatchCtrl(uint8_t uc_index, uint8_t uc_comm_stop, CAN_PROTOCOL_TYPE mode) {
  CAN_COMM_WATCH_DAT *ptr = &g_st_comm_watch_dat[mode][uc_index];

  /* Change communication status */
  ptr->uc_comm_stop = uc_comm_stop;
  /* Log output */
  // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"  //NOLINT (readability/naming)
  FRAMEWORKUNIFIEDLOG(ZONE_CAN_DEBUG, __func__,
         "##### COMMWaitTable [Index:0x%04x, SndId:0x%s, Did:0x%08x, CanID:0x%08x, Tim:0x%04x, Comm:0x%04x, IG:0x%04x, TimSeq:0x%04x] #####",  // NOLINT (whitespace/line_length)
         uc_index, ptr->notify_name, ptr->ul_did, ptr->ul_can_id,
         ptr->us_watch_time, ptr->uc_comm_stop, ptr->uc_ig, ptr->us_timer_seq_no);
  // LCOV_EXCL_BR_STOP 15:marco defined in "native_service/ns_logger_if.h"  //NOLINT (readability/naming)
  return;
}

/*******************************************************************************
 * MODULE    : CANCommWatchDataGet
 * ABSTRACT  : Communication interruption data acquisition process
 * FUNCTION  : Returns the disconnected data for the specified index 
 * ARGUMENT  : uc_index  :Indexed
 *  pNotifyId :Destination ID Return Pointer
 *  pul_did  :Data ID return pointer
 *  puc_comm_stop :Communication status return pointer
 * NOTE      : 
 * RETURN    : void
 ******************************************************************************/
void CANCommWatchDataGet(uint8_t uc_index, char *p_notify_name, DID *pul_did,
                         uint8_t *puc_comm_stop, CAN_PROTOCOL_TYPE mode) {
  CAN_COMM_WATCH_DAT *ptr = &g_st_comm_watch_dat[mode][uc_index];

  snprintf(p_notify_name, strlen(ptr->notify_name) + 1, "%s", ptr->notify_name);
  *pul_did = ptr->ul_did;            /* Data ID */
  *puc_comm_stop = ptr->uc_comm_stop; /* Communication status */

  return;
}

/*******************************************************************************
 * MODULE    : CANCommWatchTimerSeqNoGet
 * ABSTRACT  : Acquisition of Communication Disruption Monitor Timer Sequence Number
 * FUNCTION  : Returns the timer sequence number at the specified index 
 * ARGUMENT  : uc_index   :Indexed
 * NOTE      : 
 * RETURN    : Timer Sequence Number
 ******************************************************************************/
uint16_t CANCommWatchTimerSeqNoGet(uint8_t uc_index, CAN_PROTOCOL_TYPE mode) {
  uint16_t us_timer_seq_no;
  CAN_COMM_WATCH_DAT *ptr = &g_st_comm_watch_dat[mode][uc_index];

  /* Timer sequence number of the target index */
  us_timer_seq_no = ptr->us_timer_seq_no;

  return (us_timer_seq_no);
}

/*******************************************************************************
 * MODULE    : CANCommWatchTimerSeqNoRenwal
 * ABSTRACT  : Communication interruption monitoring timer sequence number update processing
 * FUNCTION  : Updates the timer sequence number at the specified index 
 * ARGUMENT  : uc_index   :Indexed
 *      pus_watch_time  :Communication interruption time return pointer
 * NOTE      : 
 * RETURN    : Timer Sequence Number
 ******************************************************************************/
uint16_t CANCommWatchTimerSeqNoRenwal(uint8_t uc_index, uint16_t *pus_watch_time, CAN_PROTOCOL_TYPE mode) {
  uint16_t us_timer_seq_no;
  CAN_COMM_WATCH_DAT *ptr = &g_st_comm_watch_dat[mode][uc_index];

  /* Updating the timer sequence number of the target index */
  ptr->us_timer_seq_no = g_us_comm_watch_timer_seq_no;

  /* Timer sequence number increment counter + 1 */
  g_us_comm_watch_timer_seq_no++;

  /* Check whether it is within the range of the sequence number used for communication interruption monitoring */
  if (g_us_comm_watch_timer_seq_no > (uint16_t)COMM_WATICH_TIMER_SEQ_NO_MAX) {
    g_us_comm_watch_timer_seq_no = COMM_WATICH_TIMER_SEQ_NO_MIN;
  }

  /* Return value setting */
  us_timer_seq_no = ptr->us_timer_seq_no;
  *pus_watch_time = ptr->us_watch_time;
  /* Log output */
  // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"  //NOLINT (readability/naming)
  FRAMEWORKUNIFIEDLOG(ZONE_CAN_DEBUG, __func__,
         "##### COMMWaitTable [Index:0x%04x, SndId:0x%s, Did:0x%08x, CanID:0x%08x, Tim:0x%04x, Comm:0x%04x, IG:0x%04x, TimSeq:0x%04x] #####",  // NOLINT (whitespace/line_length)
         uc_index, ptr->notify_name, ptr->ul_did, ptr->ul_can_id,
         ptr->us_watch_time, ptr->uc_comm_stop, ptr->uc_ig, ptr->us_timer_seq_no);
  // LCOV_EXCL_BR_STOP 15:marco defined in "native_service/ns_logger_if.h"  //NOLINT (readability/naming)
  return (us_timer_seq_no);
}

/*******************************************************************************
 * MODULE    : CANCommWatchSndCheck
 * ABSTRACT  : CANDataCommunication Disruption Monitoring Management Table Delivery Destination ID Confirmation Process
 * FUNCTION  : Compare the specified index of the communication interruption monitoring management table with the delivery destination ID. 
 * ARGUMENT  : uc_index  :Indexed
 *      notifyId :Addresses for delivery ID
 * NOTE      : 
 * RETURN    : TRUE  :Data consistency
 *    FALSE  :Data mismatch
 ******************************************************************************/
BOOL CANCommWatchSndCheck(uint8_t uc_index, PCSTR notify_name, CAN_PROTOCOL_TYPE mode) {
  BOOL ret = FALSE;
  CAN_COMM_WATCH_DAT *ptr = &g_st_comm_watch_dat[mode][uc_index];

  /* Destination ID of specified index data matches argument? */
  if (strcmp(notify_name, ptr->notify_name) == 0) {
    ret = TRUE;
  }
  return (ret);
}

/*******************************************************************************
 * MODULE    : CANCommWatchIgcoopGet
 * ABSTRACT  : IG linkage information acquisition processing of the communication disruption monitoring management table
 * FUNCTION  : Returns the IG linkage value of the specified index 
 * ARGUMENT  : uc_index  :Indexed
 * NOTE      : 
 * RETURN    : CAN_IG_COOPERATION_OFF  :IG no collaboration
 *    CAN_IG_COOPERATION_ON  :IG Has collaborations
 ******************************************************************************/

uint8_t CANCommWatchIgcoopGet(uint8_t uc_index, CAN_PROTOCOL_TYPE mode) {  // LCOV_EXCL_START 8:the function is not used
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  uint8_t uc_ig_cooperation;
  CAN_COMM_WATCH_DAT *ptr = &g_st_comm_watch_dat[mode][uc_index];

  /* IG linkage value of the target index */
  uc_ig_cooperation = ptr->uc_ig;

  return (uc_ig_cooperation);
}
// LCOV_EXCL_STOP

/*******************************************************************************
 * MODULE    : CANCommWatchBufferOut
 * ABSTRACT  : CAN communication interruption table log output processing
 * FUNCTION  : Output CAN communication disruption table log 
 * ARGUMENT  : FILE *fp_log  : File pointer of the log output file
 * NOTE      : 
 * RETURN    : 
 ******************************************************************************/

void CANCommWatchBufferOut(FILE *fp_log, CAN_PROTOCOL_TYPE mode) {  // LCOV_EXCL_START 8:the function is not used
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  int32_t usage_rate;
  uint16_t i;
  uint16_t data_cnt = 0;
  CAN_COMM_WATCH_DAT *ptr = &g_st_comm_watch_dat[mode][0];

  if (NULL != fp_log) {
    /* Output of communication disruption management table */
    (void)fprintf(fp_log, "CAN CommWatch Buffer \n");
    (void)fprintf(fp_log, "CanID:     SNDID:   DID:       Time:  State:  IG_mode:  TimeSeq:\n");
    for (i = 0; i < (uint16_t)COMM_WATCH_LIST_NUM; i++) {
      /* Free (communication interruption time = 0)? */
      if ((uint16_t)0 != ptr[i].us_watch_time) {
        (void)fprintf(fp_log, "%08x   %s   %08x   %04x   %04x    %04x      %04x\n",
                      ptr[i].ul_can_id,
                      ptr[i].notify_name,
                      ptr[i].ul_did,
                      ptr[i].us_watch_time,
                      ptr[i].uc_comm_stop,
                      ptr[i].uc_ig,
                      ptr[i].us_timer_seq_no);
        data_cnt++;
      }
    }
    usage_rate = (((int32_t)data_cnt * 100) / COMM_WATCH_LIST_NUM);
    (void)fprintf(fp_log, "BUFFER_Use: %04d  BUFFER_Max: %04d  Usage_Rate: %04d \n",
                  data_cnt, COMM_WATCH_LIST_NUM, usage_rate);
    if (usage_rate >= CAN_USAGE_RATE_THRESHOLD) {
      (void)fprintf(fp_log, "Warning: Buffer utilization exceeds a threshold.\n");
    }
  }
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START 7: debug code
EFrameworkunifiedStatus CANCommWatchAllClearDebug(HANDLE h_app) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  if (access("/tmp/communication_debug", F_OK) == 0) {
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __func__, "g_st_comm_watch_dat[CAN].");
    /* Communication disruption monitoring management table */
    memset(g_st_comm_watch_dat[0], 0x00, sizeof(CAN_COMM_WATCH_DAT) * COMM_WATCH_LIST_NUM);
    g_us_comm_watch_dat_cnt[0] = 0;      /* Number of effective registrations of communication disruption monitoring management table */
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "/tmp/communication_debug Not found.");
  }
  return eFrameworkunifiedStatusOK;
}
// LCOV_EXCL_STOP 7: debug code
