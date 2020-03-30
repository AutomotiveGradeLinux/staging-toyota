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
 *	Module configuration	:CANTxStart()				  CANDataTransmission start message processing
 *                    CANTxStartMsgCheck()			  CANDataSend start message check processing
 *                    CANFreqTxStop()			  CANDataPeriodic transmission stop message processing
 *                    CANFreqTxTimeupMsg()			  CANDataPeriodic transmission time-up message processing
 *                    CANTxRsltReceiveMsg()		  CANDataReceive message processing of transmission result
 *                    CANTxRsltReceiveMsgCheck()	   CANDataSend result received message check processing
 ******************************************************************************/
/*!-----------------------------------------------------------------------------
 * @file    CAN_Transmission.cpp
 * @~english
 * @brief   CAN Transmission process
 */
#include "CAN_Transmission.h"
#include <string.h>  // NOLINT(build/include)
#include <fcntl.h>
#include "CAN_TransmissionData.h"
#include "CAN_TxMsg.h"
#include "CAN_TimerCtrl.h"
#include "CAN_Delivery.h"
#include "CAN_DeliveryData.h"
#include "Canif_API_Local.h"
#include "ICR_Common.h"
#include <native_service/frameworkunified_framework_if.h>
//#include "can_hal.h"

/*************************************************/
/*           Global variable                      */
/*************************************************/

/*******************************************************************************
 * MODULE    : CANTxStartMsg
 * ABSTRACT  : CANDataTransmission start message processing
 * FUNCTION  : CANDataStarting transmission when a transmission start message is received 
 * ARGUMENT  : *pst_rcv_msg			: Received Message Reference Pointer
 * NOTE      :
 * RETURN    : RET_CAN_NORMAL			: Normal completion
 *			   RET_CAN_ERROR_BUFFULL	: Registered FULL
 *			   RET_CAN_ERROR_TIMER		: Timer acquisition failed
 ******************************************************************************/
EFrameworkunifiedStatus CANTxStart(HANDLE h_app) {
  RET_CAN l_ret = RET_CAN_NORMAL; /* Return value of this function */
  uint8_t uc_index;               /* Indexed */
  uint8_t uc_can_rid;              /* CAN uint8_t */
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  CAN_TRANSMISSION_START_MSG_DAT rcv_msg;

  FRAMEWORKUNIFIEDLOG(ZONE_CAN_DEBUG, __func__, "##### CAN COMMWATCH START  #####");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  //NOLINT (readability/naming)

  e_status = FrameworkunifiedGetMsgDataOfSize(h_app, &rcv_msg, sizeof(rcv_msg), eSMRRelease);
  if (e_status != eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "#CAN_thread# FrameworkunifiedGetMsgDataOfSize Error");
    if (e_status == eFrameworkunifiedStatusInvldBufSize) {
      FrameworkunifiedClearMsgData(h_app);
    }
    l_ret = RET_CAN_ERROR_CANCEL;
    goto exit;
  }
  // LCOV_EXCL_STOP 4: NSFW error case.

  /****** Data section check ******/
  l_ret = CANTxStartMsgCheck(&rcv_msg, &uc_index);
  if (RET_CAN_NORMAL != l_ret) {
    goto exit;
  }

  /****** Message processing ******/
  if ((uint16_t)0 != rcv_msg.usFreq) {
    /* Periodic transmission */
    CANFreqTransEntry(uc_index, &rcv_msg);            /* CANDataRegular transmission registration */
    if (FALSE == CANFreqTransStartTxMsg(h_app, uc_index)) { /* Periodic transmission start message transmission */
      l_ret = RET_CAN_ERROR_CREATE_EVENT;             /* Event generation failure */
      goto exit;
    }
  } else {
    if ((uint8_t)CAN_RID_NOTUSE_CODE == uc_index) {
      uc_can_rid = (uint8_t)CAN_RID_NOTUSE_CODE;
    } else {
      uc_can_rid = (uint8_t)((uint32_t)uc_index + CAN_TXRSLT_CANRID_1TIME_START);
    }
    /* Send one transmission start message */
    if (FALSE == CAN1TimeTransStartTxMsg(h_app, uc_can_rid, &rcv_msg.stCandata)) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "#CAN_thread# CAN Transmission ICR Error");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  //NOLINT (readability/naming)
      CANDebugLogOut(CAN_TRANCE_ICR_ERR, &rcv_msg); /* Transmission error log output */
      l_ret = RET_CAN_ERROR_CREATE_EVENT;
      goto exit;
    }
  }

  /****** CANDataRegister one transmission result management table ******/
  if (((uint16_t)0 == rcv_msg.usFreq) && ((uint8_t)CAN_RID_NOTUSE_CODE != rcv_msg.ucRid)) {
    e_status = CAN1TimeTransEntry(uc_index, &rcv_msg);          /* CANDataRegister 1 transmission result */
    if (eFrameworkunifiedStatusOK != e_status) {  // LCOV_EXCL_BR_LINE 8: uc_index can't GL CAN_1TIME_TRSNSMISSION_RSLT_NUM, so e_status is always eFrameworkunifiedStatusOK  // NOLINT (whitespace/line_length)
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 8: same with above
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "ERROR !!! CAN1TimeTransEntry failed : e_status=%d, uc_index=%d", e_status, uc_index);  // LCOV_EXCL_LINE 8: same with above  //NOLINT (readability/naming)
      l_ret = RET_CAN_ERROR_PARAM;  // LCOV_EXCL_LINE 8: same with above
    }
  }

exit:
  FRAMEWORKUNIFIEDLOG(ZONE_CAN_DEBUG, __func__, "END(l_ret=%x)", l_ret);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  //NOLINT (readability/naming)
  if (l_ret == RET_CAN_NORMAL)
    return eFrameworkunifiedStatusOK;
  else
    return eFrameworkunifiedStatusFail;
  // return RET_CAN_NORMAL;
}

/*******************************************************************************
 * MODULE    : CANTxStartMsgCheck
 * ABSTRACT  : CANDataSend start message check processing
 * FUNCTION  : CANDataCheck the data contents when a transmission start message is received 
 * ARGUMENT  : *pst_msg_data	: Receive message data section reference pointer
 *             *puc_index	: Index (pointer output)
 * NOTE      :
 * RETURN    : RET_CAN_NORMAL			:Normal completion
 *			   RET_CAN_ERROR_BUFFULL	:Registered FULL
 *			   RET_CAN_ERROR_TIMER		:Timer acquisition failure
 ******************************************************************************/
RET_CAN CANTxStartMsgCheck(const CAN_TRANSMISSION_START_MSG_DAT *pst_msg_data, uint8_t *puc_index) {
  RET_CAN l_ret = RET_CAN_NORMAL; /* Return value of the function */
  uint8_t uc_index = 0;           /* Indexed */

  FRAMEWORKUNIFIEDLOG(ZONE_CAN_DEBUG, __func__, "##### START  #####");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  //NOLINT (readability/naming)
  /* #106  Periodic transmission */
  if ((uint16_t)0 != pst_msg_data->usFreq) {
    /* CANDataConfirmation of periodic transmission management table registration destination exists */
    if (TRUE == CANFreqTransEntryCheck(pst_msg_data->stCandata.can_id, &uc_index)) {/* Periodic transmission timer start */
      l_ret = CANFreqTransTimerStart(uc_index, pst_msg_data->usFreq, CAN_PROTOCOL_TYPE_CAN);
    } else {  /* No registration destination */
      l_ret = RET_CAN_ERROR_BUFFULL; /* Registered FULL */
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "#CAN_thread# CAN FreqTrans BufferFull");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  //NOLINT (readability/naming)
    }
  } else {/* One-time transmission */
    if ((uint8_t)CAN_RID_NOTUSE_CODE == pst_msg_data->ucRid) {/* When transmission results are unnecessary */
      uc_index = (uint8_t)CAN_RID_NOTUSE_CODE; /* Transmission result non-notification code */
    } else { /* When transmission result is required */
      if (FALSE == CAN1TimeTransEntryCheck(&uc_index)) {
        l_ret = RET_CAN_ERROR_BUFFULL; /* One-time transmission results notification lists are FULL. */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "#CAN_thread# CAN 1TimeTransRslt BufferFull");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  //NOLINT (readability/naming)
      }
    }
  }
  *puc_index = uc_index;
  FRAMEWORKUNIFIEDLOG(ZONE_CAN_DEBUG, __func__, "##### END  #####");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  //NOLINT (readability/naming)
  return (l_ret);
}

/*******************************************************************************
 * MODULE    : CANFreqTxStop
 * ABSTRACT  : CANDataPeriodic transmission stop message processing
 * FUNCTION  : CANDataStop periodic transmission when a periodic transmission stop message is received 
 * ARGUMENT  : *pst_rcv_msg			: Received Message Reference Pointer
 * NOTE      :
 * RETURN    : RET_CAN_NORMAL				: Parameter Normal
 *			   RET_CAN_ERROR_UNDEF			: Undefined ID
 *			   RET_CAN_ERROR_PID			: Thread ID mismatch
 *			   RET_CAN_ERROR_TIMER			: Failed to stop timer
 *			   RET_CAN_ERROR_CREATE_EVENT	: Event generation failure
 ******************************************************************************/
EFrameworkunifiedStatus CANFreqTxStop(HANDLE h_app) {
  RET_CAN l_ret = RET_CAN_ERROR_UNDEF; /* Return value of this function */
  int32_t i;                          /* Generic counters */
  uint8_t uc_index = 0xFF;             /* Indexed */
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  CAN_FREQ_TRANS_STOP_MSG_DAT rcv_msg;

  FRAMEWORKUNIFIEDLOG(ZONE_CAN_DEBUG, __func__, "##### START  #####");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  //NOLINT (readability/naming)
  e_status = FrameworkunifiedGetMsgDataOfSize(h_app, &rcv_msg, sizeof(rcv_msg), eSMRRelease);
  if (e_status != eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "#CAN_thread# FrameworkunifiedGetMsgDataOfSize Error");
    if (e_status == eFrameworkunifiedStatusInvldBufSize) {
      FrameworkunifiedClearMsgData(h_app);
    }
    l_ret = RET_CAN_ERROR_CANCEL;
    goto exit;
  }
  // LCOV_EXCL_STOP 4: NSFW error case.

  /****** Data checking ******/
  for (i = 0; i < (int32_t)CAN_FREQ_TRSNSMISSION_LIST_NUM; i++) {
    /* CANDataPeriodic transmission control table registration status checking process (CAN ID retrieval) */
    if (TRUE != CANFreqTransCanidEntryCheck((uint8_t)i, rcv_msg.ulCanid)) {
      continue;
    }

    if (TRUE == CANFreqTransIdEntryCheck((uint8_t)i, rcv_msg.notifyName)) {
      l_ret = RET_CAN_NORMAL; /* Normal completion */
      uc_index = (uint8_t)i;  /* Index acquisition */
      break;
    } else {
      l_ret = RET_CAN_ERROR_SNDID; /* Distribution destination ID mismatch */
    }
  }

  if (l_ret == RET_CAN_ERROR_UNDEF) { /* CAN ID undefined */
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "#CAN_thread# CAN FreqTxStop CanID Error");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  //NOLINT (readability/naming)
    goto exit;
  }
  if (l_ret == RET_CAN_ERROR_SNDID) { /* Distribution destination ID mismatch */
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "#CAN_thread# CAN FreqTxStop SNDID Error");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  //NOLINT (readability/naming)
    goto exit;
  }

  /****** Message processing ******/
  l_ret = CANFreqTransTimerStop(uc_index, CAN_PROTOCOL_TYPE_CAN); /* Periodic transmission timer stop */
  if (RET_CAN_NORMAL != l_ret) {  // LCOV_EXCL_BR_LINE 200:the function is always returned to RET_CAN_NORMAL
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    goto exit;  // LCOV_EXCL_LINE 200:the function is always returned to RET_CAN_NORMAL
  }

  CANFreqTransStop(uc_index); /* CANDataPeriodic transmission stop */

exit:
  FRAMEWORKUNIFIEDLOG(ZONE_CAN_DEBUG, __func__, "END(l_ret=%x)", l_ret);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  //NOLINT (readability/naming)
  if (l_ret == RET_CAN_NORMAL)
    return eFrameworkunifiedStatusOK;
  else
    return eFrameworkunifiedStatusFail;
}
/*******************************************************************************
 * MODULE    : CANFreqTxTimeupMsg
 * ABSTRACT  : CANDataPeriodic transmission time-up message processing
 * FUNCTION  : Start transmission when the periodic timer for periodic transmission expires 
 * ARGUMENT  : *pst_rcv_msg           : Received Message Reference Pointer
 * NOTE      :
 * RETURN    : RET_CAN_NORMAL       : Normal completion
 *             RET_CAN_ERROR_CANCEL : Data destruction
 ******************************************************************************/
RET_CAN CANFreqTxTimeupMsg(HANDLE h_app, uint16_t timer_seq) {
  RET_CAN l_ret = RET_CAN_NORMAL; /* Return value of this function */
  uint8_t uc_index;               /* Indexed */

  FRAMEWORKUNIFIEDLOG(ZONE_CAN_DEBUG, __func__, "##### START  #####");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  //NOLINT (readability/naming)
  /* Check Timer Sequence Range(0x8000 ~ 0x80FF) */
  if (FREQ_TRANS_SEQNO_CODE == (timer_seq & ~TIMER_SEQNO_MASK_CODE)) {
    uc_index = (uint8_t)(timer_seq & TIMER_SEQNO_MASK_CODE);
    if (FALSE == CANFreqTransIndexEntryCheck(uc_index)) {  // LCOV_EXCL_BR_LINE 6:Double check code. in CANFreqTransEntryCheck  //NOLINT (readability/naming)
      l_ret = RET_CAN_ERROR_CANCEL; /* Data destruction */
    } else {
      /* CANDataPeriodic transmission start message transmission */
      if (FALSE == CANFreqTransStartTxMsg(h_app, uc_index)) {
        l_ret = RET_CAN_ERROR_CANCEL; /* Data destruction */
      }
    }
  } else {
    l_ret = RET_CAN_ERROR_CANCEL; /* Data destruction */
  }

  FRAMEWORKUNIFIEDLOG(ZONE_CAN_DEBUG, __func__, "##### END  #####");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  //NOLINT (readability/naming)
  return (l_ret);
}

/*******************************************************************************
 * MODULE    : CANTxRsltReceiveMsg
 ******************************************************************************/
/*!-----------------------------------------------------------------------------
 * @~english
 * @brief    CAN data transmission result reply processing
 *
 * It notifies the corresponding thread when the informative message
 * of the CAN data transmission result is received.
 *
 * CANDataWhen a transmission result notification message is received, a notification is sent to the relevant thread... 
 *
 * @~english
 * @note
 *   -# Transmission result status check
 *   -# Transmission result notification registration check
 *   -# Transmission result table check
 *   -# Transmission result status acquisition
 *   -# Message sending
 *   -# Transmission result table deletion
 *
 * @~english
 * @return  Reception result
 * @retval  <RET_CAN_NORMAL>        Normality
 * @retval  <RET_CAN_ERROR_CANCEL>  Data annulment
 *
 * @~english
 * @param[in]     <pst_rcv_msg> Reception message reference pointer
 */
RET_CAN CANTxRsltReceiveMsg(HANDLE h_app, CAN_MSG_DATA *pst_rcv_msg) {  // LCOV_EXCL_START 8:this IF is called in CANIcrSndStsProcess, which will not be called // NOLINT (whitespace/line_length)
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  RET_CAN l_ret = RET_CAN_NORMAL; /* Return value of this function */
  RET_CAN l_ret_func; /* Lower-order function return value */
  BOOL b_ret_func; /* Lower-order function return value */
//  T_ICR_CMDSNDCNF_STS *pst_msg_data;                      /* Receive data structure pointer */
  uint8_t us_sts;                                        /* Status */

  FRAMEWORKUNIFIEDLOG(ZONE_CAN_DEBUG, __func__, "##### START  #####");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  //NOLINT (readability/naming)
//  pst_msg_data = reinterpret_cast<T_ICR_CMDSNDCNF_STS *>(reinterpret_cast<void *>(&pst_rcv_msg->ucData[0]));

  /****** Transmission result status check ******/
  l_ret_func = CANTxRsltReceiveMsgCheck(pst_rcv_msg); /* Receive command (transmission result) check processing */

  /****** Send result notification registration check ******/
  if (RET_CAN_NORMAL == l_ret_func) {
//    b_ret_func = CANTxRsltEntryCheck(pst_rcv_msg->stHead.hdr.rid);
  }

  /****** Message processing ******/
  if ((RET_CAN_NORMAL == l_ret_func) && (TRUE == b_ret_func)) {
//    b_ret_func = CANTxRsltSndCheck(pst_rcv_msg->stHead.hdr.rid, (uint32_t)pst_msg_data->cnf_sts);
    /* Determine whether to perform transmission result notification */
    if (TRUE == b_ret_func) {
//      switch (pst_msg_data->cnf_sts) {
//        case ICR_SND_SUCCESS:
//          us_sts = CAN_SUCCESS;
//          break;
//        case ICR_SND_ERR_RETRYOUT:
//          us_sts = CAN_RETRYOUT;
//          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "#CAN_thread# CAN Transmission SYSCOM Error");
//          CANDebugLogOut(CAN_TRANCE_SYSCOM_ERR, reinterpret_cast<void *>(pst_rcv_msg));
//          break;
//        case ICR_SND_ERR_BUFFERFULL:
//          us_sts = CAN_BUFFERFUL;
//          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "#CAN_thread# CAN Transmission SYSCOM Error");
//          CANDebugLogOut(CAN_TRANCE_SYSCOM_ERR, reinterpret_cast<void *>(pst_rcv_msg));
//          break;
//        default:
//          l_ret = RET_CAN_ERROR_CANCEL; /* Data destruction */
//          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "#CAN_thread# CAN Transmission SYSCOM Error");
//          CANDebugLogOut(CAN_TRANCE_SYSCOM_ERR, reinterpret_cast<void *>(pst_rcv_msg));
//          break;
//      }
      if (RET_CAN_NORMAL == l_ret) {
//        b_ret_func = CANTxRsltSndMsg(h_app, pst_rcv_msg->stHead.hdr.rid, us_sts);
        /* CANDataSend result notification message */
        /* Transmission result judgment */
        if (FALSE == b_ret_func) {
          /* Transmission result notification transmission failure */
          l_ret = RET_CAN_ERROR_CANCEL; /* Data destruction */
        }
      }
    } else {
      /* No need to send result notification */
    }

    /****** Delete various transmission results management tables ******/
    if (RET_CAN_NORMAL == l_ret) {
//      CANTxRsltFin(pst_rcv_msg->stHead.hdr.rid); /* CANDataTransmission result notification completed */
    }
  } else {
    /* Transmission result status error */
    l_ret = RET_CAN_ERROR_CANCEL; /* Data destruction */
  }
  FRAMEWORKUNIFIEDLOG(ZONE_CAN_DEBUG, __func__, "##### END  #####");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  //NOLINT (readability/naming)
  return (l_ret);
}
// LCOV_EXCL_STOP 8

/*******************************************************************************
 * MODULE    : CANTxRsltReceiveMsgCheck
 ******************************************************************************/
/*!-----------------------------------------------------------------------------
 * @~english
 * @brief    CAN data transmission result reception message check processing
 *
 * CAN data transmission result reception message check processing
 *
 * CANDataCheck the data contents when a transmission result notification message is received. 
 *
 * @~english
 * @note
 *   -# Transmission result status check
 *   -# RID abnormality check
 *
 * @~english
 * @return  Check result
 * @retval  <RET_CAN_NORMAL>        Normality
 * @retval  <RET_CAN_ERROR_CANCEL>  Data annulment
 *
 * @~english
 * @param[in]     <pst_rcv_msg> Reception message reference pointer
 */
/*------------------------------------------------------------------------------
 * RETURN    : RET_CAN_NORMAL			: Parameter Normal
 *             RET_CAN_ERROR_CANCEL		: Data destruction
 *----------------------------------------------------------------------------*/
RET_CAN CANTxRsltReceiveMsgCheck(CAN_MSG_DATA *pst_rcv_msg) {  // LCOV_EXCL_START 8:this IF is called in CANIcrSndStsProcess, which will not be called // NOLINT (whitespace/line_length)
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  RET_CAN l_ret = RET_CAN_NORMAL;   /* Return value of this function */
//  T_ICR_CMDSNDCNF_STS *pst_msg_data; /* Receive data structure pointer */

  FRAMEWORKUNIFIEDLOG(ZONE_CAN_DEBUG, __func__, "##### START  #####");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  //NOLINT (readability/naming)
//  pst_msg_data = reinterpret_cast<T_ICR_CMDSNDCNF_STS *>(reinterpret_cast<void *>(&pst_rcv_msg->ucData[0]));
  /* Type conversion */

  /****** Transmission result status check ******/
//  if ((ICR_SND_SUCCESS != pst_msg_data->cnf_sts) &&
//      (ICR_SND_ERR_RETRYOUT != pst_msg_data->cnf_sts) &&
//      (ICR_SND_ERR_BUFFERFULL != pst_msg_data->cnf_sts)) {
//    l_ret = RET_CAN_ERROR_CANCEL; /* Data destruction */
//  }
  /****** RID anomaly check ******/
  /* NG if the ID is out of the table to be searched */
  /* The table range is from 0 (periodic-transmission-management-table + number-of-transmission-results-once-management-tables). */
//  if (((uint8_t)CAN_TXRSLT_CANRID_END < pst_rcv_msg->stHead.hdr.rid)) {
//    l_ret = RET_CAN_ERROR_CANCEL; /* Data destruction */
//  }

  FRAMEWORKUNIFIEDLOG(ZONE_CAN_DEBUG, __func__, "##### END  #####");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  //NOLINT (readability/naming)
  return (l_ret);
}
// LCOV_EXCL_STOP 8

/*******************************************************************************
 * MODULE    : CAN_CWORD29_TxMsg
 * ABSTRACT  : CANData(_CWORD29_) outgoing message processing
 * FUNCTION  : CANData(_CWORD29_) Start sending a message when it is received 
 * ARGUMENT  : *pst_rcv_msg					: Received Message Reference Pointer
 * NOTE      :
 * RETURN    : RET_CAN_NORMAL				: Normal completion
 *			       RET_CAN_ERROR_BUFFULL		: Registered FULL
 ******************************************************************************/
EFrameworkunifiedStatus CAN_CWORD29_TxMsg(HANDLE h_app) {
  CAN__CWORD29__TRANS_MSG pst_msg_data;          /* Pointer to send start data structure */
  uint8_t uc_can_rid;                         /* Send Resource ID */
  uint8_t uc_index;                           /* Indexed */
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;

  FRAMEWORKUNIFIEDLOG(ZONE_CAN_DEBUG, __func__, "##### CAN_CWORD29_TxMsg START  #####");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  //NOLINT (readability/naming)

  e_status = FrameworkunifiedGetMsgDataOfSize(h_app, &pst_msg_data, sizeof(pst_msg_data), eSMRRelease);
  if (e_status != eFrameworkunifiedStatusOK) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "#CAN_thread# FrameworkunifiedGetMsgDataOfSize Error");
    if (e_status == eFrameworkunifiedStatusInvldBufSize) {
      FrameworkunifiedClearMsgData(h_app);
    }
    return eFrameworkunifiedStatusFail;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_CAN_DEBUG, __func__, "##### Opc=%04x Name=%s Rid=%02x #####", pst_msg_data.opc, pst_msg_data.notifyName, pst_msg_data.ucRid);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  //NOLINT (readability/naming)

  /* Register transmission result notification? */
  if ((uint8_t)CAN_RID_NOTUSE_CODE == pst_msg_data.ucRid) {/* When transmission results are unnecessary */
    uc_index = (uint8_t)CAN_RID_NOTUSE_CODE; /* Transmission result non-notification code */
  } else { /* When transmission result is required */
    if (FALSE == CAN_CWORD29_TransEntryCheck(&uc_index)) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "#CAN_thread# CAN _CWORD29_TransRslt BufferFull");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  //NOLINT (readability/naming)
      return eFrameworkunifiedStatusFail;
    }
    e_status = CAN_CWORD29_TransEntry(uc_index, &pst_msg_data);          /* Save CAN_CWORD29_ Transmission Result */
    if (eFrameworkunifiedStatusOK != e_status) {  // LCOV_EXCL_BR_LINE 6:Excluded due to value never returned at present
      // LCOV_EXCL_START 6:Excluded due to value never returned at present
      AGL_ASSERT_NOT_TESTED();
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "ERROR !!! CAN_CWORD29_TransEntry failed : e_status=%d, uc_index=%d", e_status, uc_index);
      return eFrameworkunifiedStatusFail;
      // LCOV_EXCL_STOP 6:Excluded due to value never returned at present
    }
  }

  if ((uint8_t)CAN_RID_NOTUSE_CODE == uc_index) {
    uc_can_rid = (uint8_t)CAN_RID_NOTUSE_CODE;
  } else {
    uc_can_rid = (uint8_t)((uint32_t)uc_index + CAN_TXRSLT_CANRID__CWORD29__START);
  }

  /****** Message transmission processing ******/
//  CanMessage_CWORD29_ st_can_data;
//
//  st_can_data.opc = pst_msg_data.opc;
//  st_can_data.rid = uc_can_rid;
//  st_can_data.dlc = (uint8_t)pst_msg_data.ulDataSize;
//  memcpy(st_can_data.data, pst_msg_data.data, (st_can_data.dlc));

//  if (CanSend_CWORD29_(&st_can_data) != CANHAL_RET_NORMAL) {
//    return eFrameworkunifiedStatusFail;
//  }

  return eFrameworkunifiedStatusOK;
}

/*!-----------------------------------------------------------------------------
 * @~english
 * @brief    Start transmission of CAN Command
 *
 * @~english
 * @return  Return value
 * @retval  <eFrameworkunifiedStatusOK>        OK
 * @retval  <eFrameworkunifiedStatusFail>      Abnormal termination
 *
 * @~english
 * @param[in]     <h_app>    Application handle
 */
EFrameworkunifiedStatus CANTxBitStart(HANDLE h_app) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  CAN_TRANS_START_MSG_DAT rcv_msg;

  FRAMEWORKUNIFIEDLOG(ZONE_CAN_DEBUG, __func__, "##### CANTxBitStart Start");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  //NOLINT (readability/naming)

  e_status = FrameworkunifiedGetMsgDataOfSize(h_app, &rcv_msg, sizeof(rcv_msg), eSMRRelease);
  if (e_status != eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "FrameworkunifiedGetMsgDataOfSize Error eFrameworkunifiedStatus=%d", e_status);
    if (e_status == eFrameworkunifiedStatusInvldBufSize) {
      FrameworkunifiedClearMsgData(h_app);
    }
    goto exit;
  }
  // LCOV_EXCL_STOP 4: NSFW error case.

  /* Merge CAN Data and update Trnasmission management table */
  if (TRUE == CANTransDataEntry(&rcv_msg)) {

    /* Interval Data registered to Timer management table */
    if ((uint32_t)0 < rcv_msg.freq) {
      CANFreqTimerEntry(rcv_msg.id, rcv_msg.freq);
    }

    /* Transmission CAN Data */
    if (FALSE == CANTransStartTxMsg(h_app, rcv_msg.id)) {
      e_status = eFrameworkunifiedStatusFail;
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "CANTransStartTxMsg Error CAN ID=0x%04X", rcv_msg.id);
      goto exit;
    }

    /* Reset Timer */
    if ((uint32_t)0 == rcv_msg.freq) {
      CANFreqTimerResetMap(rcv_msg.id);
    }
  } else if (FALSE == CAN_SubIDTransStartTxMsg(h_app, &rcv_msg)) {
    e_status = eFrameworkunifiedStatusFail;
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "Trans NG CAN ID=0x%04X DATA[0]=0x%02X", rcv_msg.id, (rcv_msg.dat.dat[0] & rcv_msg.mask.dat[0]));
  }

exit:
  FRAMEWORKUNIFIEDLOG(ZONE_CAN_DEBUG, __func__, "END(eFrameworkunifiedStatus=%x)", e_status);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  //NOLINT (readability/naming)

  return e_status;
}

/*!-----------------------------------------------------------------------------
 * @~english
 * @brief    Stop transmission CAN Command
 *
 * @~english
 * @return  Return value
 * @retval  <eFrameworkunifiedStatusOK>        OK
 * @retval  <eFrameworkunifiedStatusFail>      Abnormal termination
 *
 * @~english
 * @param[in]     <h_app>    Application handle
 */
EFrameworkunifiedStatus CANTxBitStop(HANDLE h_app) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  CANID rcv_msg;

  FRAMEWORKUNIFIEDLOG(ZONE_CAN_DEBUG, __func__, "##### CAN_FreqTxBitStop Start");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  //NOLINT (readability/naming)
  e_status = FrameworkunifiedGetMsgDataOfSize(h_app, &rcv_msg, sizeof(rcv_msg), eSMRRelease);
  if (e_status != eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "FrameworkunifiedGetMsgDataOfSize Error eFrameworkunifiedStatus=%d", e_status);
    if (e_status == eFrameworkunifiedStatusInvldBufSize) {
      FrameworkunifiedClearMsgData(h_app);
    }
    e_status = eFrameworkunifiedStatusFail;
    goto exit;
  }
  // LCOV_EXCL_STOP 4: NSFW error case.

  /* Check correct CAN ID */
  if (FALSE == CANEntryTransCanidCheck(rcv_msg)) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "CANEntryTransCanidCheck Error CAN ID=0x%04X", rcv_msg);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  //NOLINT (readability/naming)
    e_status = eFrameworkunifiedStatusFail;
    goto exit;
  }

  /* Stop periodic transmission */
  if (RET_CAN_NORMAL != CANFreqTransTimerStopMap(rcv_msg)) {  // LCOV_EXCL_BR_LINE 200:the function is always returned to RET_CAN_NORMAL //NOLINT (readability/naming)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    e_status = eFrameworkunifiedStatusFail;  // LCOV_EXCL_LINE 200:the function is always returned to RET_CAN_NORMAL
  }

exit:
  FRAMEWORKUNIFIEDLOG(ZONE_CAN_DEBUG, __func__, "END(eFrameworkunifiedStatus=%x)", e_status);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  //NOLINT (readability/naming)

  return e_status;
}
// LCOV_EXCL_BR_LINE 10:THE_END_LINE_OF_THE_FILE
