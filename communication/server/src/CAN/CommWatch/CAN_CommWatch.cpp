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
 * FILE  :CAN_CommWatch.cpp
 * SYSTEM  :_CWORD107_
 * SUBSYSTEM :EXL process
 * PROGRAM :CAN Thread CAN Data Communication Disruption Monitoring Management Process
 * Module configuration :CANCommWatchTimeoutMsg()  CANDataCommunication Disruption Timeout Message Processing
 *     :CAN_CommWatch()   CANDataCommunication Disruption Monitoring Message Processing
 *     :CANCommWatchClear()   CANDataCommunication interruption clear processing
 *     :CANVehicleInfoMsg()   Vehicle sensor information notification message processing
 *     :CANIgStatGet()    IG reception status acquisition processing
 ******************************************************************************/
#include "CAN_CommWatch.h"

#include <native_service/frameworkunified_types.h>  // NOLINT (build/include)
#include <native_service/frameworkunified_framework_if.h>

#include <peripheral_service/Canif_API.h>
#include "CAN_Thread.h"
#include "CAN_CommWatchData.h"
#include "CAN_TxMsg.h"
#include "CAN_TimerCtrl.h"

/*************************************************/
/*           Global variable                      */
/*************************************************/

static uint8_t g_uc_comm_watch_ig_stat; /* IG receive mode */

/*******************************************************************************
 * MODULE    : CANCommWatchInit
 * ABSTRACT  : CANDataCommunication Disruption Monitoring Initialization Process
 * FUNCTION  : CANDataInitialize communication interruption monitoring
 * ARGUMENT  : void
 * NOTE      : 
 * RETURN    : void
 ******************************************************************************/
void CANCommWatchInit(void) {
  g_uc_comm_watch_ig_stat = CAN_COMM_IG_NORCV; /* Initialization of IG reception status   */
}

/*******************************************************************************
 * MODULE    : CANCommWatchTimeoutMsg
 * ABSTRACT  : CANDataCommunication Disruption Timeout Message Processing
 * FUNCTION  : Send communication interruption message when communication interruption of monitored CAN data is detected. 
 * ARGUMENT  : us_timer_seq_no    : Timer sequence number that timed out
 * NOTE      : 
 * RETURN    : RET_CAN_NORMAL   :Normal completion
 *    : RET_CAN_ERROR_CANCEL  :Data destruction
 ******************************************************************************/
RET_CAN CANCommWatchTimeoutMsg(HANDLE h_app, uint16_t us_timer_seq_no) {
  RET_CAN ret = RET_CAN_ERROR_CANCEL; /* Return value of this function */
  uint8_t uc_index;                     /* Index of the corresponding timer sequence number */
  uint8_t uc_comm_stop;                  /* Communication status of the corresponding timer sequence number */

  /* Determine whether communication interruption monitoring of the corresponding sequence number is registered. */
  /* Timer Sequence Number Search*/
  if (TRUE == CANCommWatchTimerSeqNoEntryCheck(us_timer_seq_no, &uc_index, &uc_comm_stop, CAN_PROTOCOL_TYPE_CAN)) {
    /* Determining whether communication is in progress */
    if ((uint8_t)CAN_COMM_NORMAL == uc_comm_stop) {
      /* Change the communication status to "Communication interrupted" */
      (void)CANCommWatchCtrl(uc_index, CAN_COMM_STOP, CAN_PROTOCOL_TYPE_CAN); /* Communication interruption state control processing */

      /* Send Communication Disruption Detection Message */
      (void)CANCommWatchSndMsg(h_app, uc_index); /* Transmission of communication interruption detection/recovery message */

      ret = RET_CAN_NORMAL;
    }
  }

  return (ret);
}

/*******************************************************************************
 * MODULE    : CANCommWatch
 * ABSTRACT  : CANDataCommunication Disruption Monitoring Message Processing
 * FUNCTION  : CANDataRegister/delete communication disruption monitoring management table 
 * ARGUMENT  : pst_rcv_msg :Received Message Reference Pointer
 * NOTE      : 
 * RETURN    : RET_CAN_NORMAL   :Normal completion
 *    : RET_CAN_ERROR_SNDID  :Distribution destination ID mismatch
 *    : RET_CAN_ERROR_UNDEF  :Unregistered ID
 *    : RET_CAN_ERROR_TIMER  :Timer acquisition failed
 *    : RET_CAN_ERROR_BUFFULL :Communication Disruption Monitoring Registered Number FULL
 ******************************************************************************/
EFrameworkunifiedStatus CANCommWatch(HANDLE h_app) {
  return CANCommWatchCore(h_app, CAN_PROTOCOL_TYPE_CAN);
}

EFrameworkunifiedStatus CANCommWatchCore(HANDLE h_app, CAN_PROTOCOL_TYPE mode) {
  RET_CAN ret = RET_CAN_NORMAL;                /* Return value of this function */
  uint8_t uc_cnt;                                /* Generic counters */
  uint8_t uc_index;                              /* Indexed */
  uint8_t uc_comm_stop = CAN_COMM_OFF;            /* Communication status */
  BOOL b_id_flag = FALSE; /* Variables for Stopping Monitoring */    /* ID match flag */
  BOOL b_snd_id_flag = FALSE; /* Variables for Stopping Monitoring */ /* Destination ID match flag */
  uint16_t us_timer_seq_no; /* Variables for Starting Monitoring */   /* Acquisition Timer Sequence Number */
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  CAN_COMM_WATCHEXT_MSG_DAT rcv_msg;

  FRAMEWORKUNIFIEDLOG(ZONE_CAN_DEBUG, __func__, "##### CAN COMMWATCH START  #####");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  //NOLINT (readability/naming)

  e_status = FrameworkunifiedGetMsgDataOfSize(h_app, &rcv_msg, sizeof(rcv_msg), eSMRRelease);
  if (e_status != eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "#CAN_thread# FrameworkunifiedGetMsgDataOfSize Error");
    if (e_status == eFrameworkunifiedStatusInvldBufSize) {
      FrameworkunifiedClearMsgData(h_app);
    }
    ret = RET_CAN_ERROR_CANCEL;
    goto exit;
  }
  // LCOV_EXCL_STOP 4: NSFW error case.

  /* Monitor Registration/Suspension Determination */
  if ((uint16_t)0 == rcv_msg.usWatchTime) { /* Is the communication interruption time zero? */
    /*** Suspend Monitoring(Communication interruption time = 0) ***/

    /* Is the delivery destination ID and CANID, DID registered in the disruption monitoring table? */
    for (uc_cnt = 0; uc_cnt < (uint8_t)COMM_WATCH_LIST_NUM; uc_cnt++) {
      /* Are data with matching CANID and data IDs registered? ? */
      if (FALSE == CANCommWatchCanidDidEntryCheck(uc_cnt, rcv_msg.ulCanid, rcv_msg.ulDid, mode)) {
        continue;
      }
      b_id_flag = TRUE; /* Set the ID match flag */

      /* Does the shipping ID match? */
      if (FALSE == CANCommWatchSndCheck(uc_cnt, rcv_msg.notifyName, mode)) {
        continue;
      }
      b_snd_id_flag = TRUE; /* Set the delivery destination ID match flag  */
      uc_index = uc_cnt;   /* Store the index subject to monitoring termination. */
      break;
    }

    if (TRUE != b_id_flag) { /* No matching data */
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "#CAN_thread# CAN CommWatchStop CanID Error");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  //NOLINT (readability/naming)
      ret = RET_CAN_ERROR_UNDEF; /* Undefined ID error */
      goto exit;
    }
    if (TRUE != b_snd_id_flag) { /* Distribution destination ID mismatch */
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "#CAN_thread# CAN CommWatchStop SNDID Error");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  //NOLINT (readability/naming)
      ret = RET_CAN_ERROR_SNDID; /* Distribution destination ID mismatch */
      goto exit;
    }

    /* Suspend Monitoring */
    /* Communication interruption timer stop processing */
    if (RET_CAN_NORMAL == CANCommWatchTimerStop(uc_index, mode)) { // LCOV_EXCL_BR_LINE 200:the function is always returned to RET_CAN_NORMAL //NOLINT (readability/naming)
      /* Successful stop -> delete from communication disconnection monitoring management table */
      CANCommWatchDelete(uc_index, mode);
    }

  } else {
    /* Storage status can be checked? */
    /* No free space */
    if (TRUE != CANCommWatchEntryCheck(rcv_msg.ulCanid, rcv_msg.ulDid, rcv_msg.notifyName, &uc_index, mode)) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "#CAN_thread# CAN CommWatch BufferFull");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  //NOLINT (readability/naming)
      ret = RET_CAN_ERROR_BUFFULL;
      goto exit;
    }

    /* Timer stop */
    (void)CANCommWatchTimerStop(uc_index, mode);

    /* IG linkage OFF or IG linkage ON and IG reception in progress */
    if ((rcv_msg.ucIg == (uint8_t)CAN_IG_COOPERATION_OFF) || ((rcv_msg.ucIg == (uint8_t)CAN_IG_COOPERATION_ON) && (g_uc_comm_watch_ig_stat == (uint8_t)CAN_COMM_IG_ON))) {  // LCOV_EXCL_BR_LINE 200: rcv_msg.ucIg is always be CAN_IG_COOPERATION_OFF //NOLINT (readability/naming)
      /* Timer activation */
      /* Communication interruption monitoring timer start processing */
      if (RET_CAN_NORMAL != CANCommWatchTimerStart(uc_index, rcv_msg.usWatchTime, mode)) {  // LCOV_EXCL_BR_LINE 200:the function is always returned to RET_CAN_NORMAL //NOLINT (readability/naming)
        // LCOV_EXCL_START 200: the function is always returned to RET_CAN_NORMAL
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        ret = RET_CAN_ERROR_TIMER; /* Timer start failure -> Return value setting: Timer acquisition error */
        goto exit;
      }  // LCOV_EXCL_STOP 200: the function is always returned to RET_CAN_NORMAL
      /* Communication type = set during communication */
      uc_comm_stop = CAN_COMM_NORMAL;
    }

    /* Timer sequence number acquisition */
    us_timer_seq_no = CANCommWatchTimerSeqNoGet(uc_index, mode);
    /* CANDataCommunication Disruption Monitoring Management Table Registration Process */
    CANCommWatchEntry(uc_index, &rcv_msg, us_timer_seq_no, uc_comm_stop, mode);
  }

exit:
  FRAMEWORKUNIFIEDLOG(ZONE_CAN_DEBUG, __func__, "END(ret=%x)", ret);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  //NOLINT (readability/naming)
  if (ret == RET_CAN_NORMAL)
    return eFrameworkunifiedStatusOK;
  else
    return eFrameworkunifiedStatusFail;
  // return RET_CAN_NORMAL;
}

/*******************************************************************************
 * MODULE    : CANCommWatchClear
 * ABSTRACT  : CANDataCommunication interruption clear processing
 * FUNCTION  : Sends a communication recovery message and restarts the timer. 
 * ARGUMENT  : can_id  :Received CAN ID
 * NOTE      : 
 * RETURN    : void
 ******************************************************************************/
void CANCommWatchClear(HANDLE h_app, CANID can_id) {
  uint8_t uc_cnt;            /* Counter */
  uint8_t uc_comm_stop;       /* Communication status */
  BOOL ret;
  uint16_t us_tbl_chk_cnt = 0; /* Number of effective data checks for communication disruption table */

  /* Corresponding CAN ID retrieval in communication interruption monitoring control table */
  for (uc_cnt = 0; uc_cnt < (uint8_t)COMM_WATCH_LIST_NUM; uc_cnt++) {
    ret = CANCommWatchCanidEntryCheck(uc_cnt, can_id, &uc_comm_stop, &us_tbl_chk_cnt, CAN_PROTOCOL_TYPE_CAN);
    /* CAN ID matching ? */
    if (TRUE == ret) {
      /* Is communication interrupted? */
      if ((uint8_t)CAN_COMM_STOP == uc_comm_stop) {
        /* Change the communication state to "Communicating" */
        (void)CANCommWatchCtrl(uc_cnt, CAN_COMM_NORMAL, CAN_PROTOCOL_TYPE_CAN); /* Communication interruption state control processing */
        uc_comm_stop = CAN_COMM_NORMAL;
        /* Send communication disruption recovery message */
        (void)CANCommWatchSndMsg(h_app, uc_cnt); /* Transmission of communication interruption detection/recovery message */
      }

      /* Timer restart only when communication status is in progress */
      if (uc_comm_stop == (uint8_t)CAN_COMM_NORMAL) {
        /* Timer restart */
        (void)CANCommWatchTimerRenewal(uc_cnt, CAN_PROTOCOL_TYPE_CAN); /* Communication interruption monitoring timer update processing */
      }
    }
    /* Check for completion of retrieval of all valid table data */
    if (us_tbl_chk_cnt >= g_us_comm_watch_dat_cnt[CAN_PROTOCOL_TYPE_CAN]) {
      /* Completion of retrieval of all valid data for the table */
      break;
    }
  }

  return;
}

/*******************************************************************************
 * MODULE    : CANIgStatGet
 * ABSTRACT  : IG reception status acquisition processing
 * FUNCTION  : Obtain IG reception information
 * ARGUMENT  : void
 * NOTE      : 
 * RETURN    : IG Information Received
 ******************************************************************************/

uint8_t CANIgStatGet(void) {  // LCOV_EXCL_START 8: for dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  return (g_uc_comm_watch_ig_stat);
}
// LCOV_EXCL_STOP
