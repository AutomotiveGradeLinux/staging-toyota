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
 * Module configuration :CANCommandTransmission()   CAN command transmission processing
 *     :CANCommandTxRslt()    CAN command transmission result notification processing
 *     :CANCommandDelivery()    CAN command delivery process
 ******************************************************************************/
/*!-----------------------------------------------------------------------------
 * @file    CAN_Command.cpp
 * @~english
 * @brief   CAN Command process
 */
#include "CAN_Command.h"

#include <native_service/frameworkunified_types.h>  // NOLINT (build/include)
#include <native_service/frameworkunified_framework_if.h>
#include <peripheral_service/Canif_API.h>
//#include <can_hal.h>
#include <string>
#include "CAN_Thread.h"
#include "CAN_Delivery.h"
#include "CAN_CommandData.h"
#include "CAN_TxMsg.h"
#include "ICR_Common.h"

/*************************************************/
/*           Global variable                      */
/*************************************************/

/*******************************************************************************
 * MODULE    : CANCommandTransmission
 * ABSTRACT  : CAN command transmission processing
 * FUNCTION  : Control the transmission of CAN commands 
 * ARGUMENT  : pst_rcv_msg     : Received Message Reference Pointer
 * NOTE      :
 * RETURN    : RET_CAN_NORMAL    : Normal completion
 *    RET_CAN_ERROR_CREATE_EVENT : Event generation failure
 *    RET_CAN_ERROR_BUFFULL  : Delivery registration FULL or result notification registration FULL
 ******************************************************************************/
EFrameworkunifiedStatus CANCommandTransmission(HANDLE h_app) {
  RET_CAN ret = RET_CAN_NORMAL; /* Return value of this function */
  BOOL b_dlvry_flag = TRUE;        /* Delivery registration flag */
  BOOL b_tx_rslt_flag = TRUE;       /* Notification registration flag */
  BOOL b_com_flag = TRUE;          /* Command transmission flag */
  BOOL b_snd_msg_flag = FALSE;      /* RST request MSG send flag */
  uint8_t uc_can_rid = CAN_RID_NOTUSE_CODE;
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  CAN_CMD_CTRL_MSG_DAT rcv_msg;

  e_status = FrameworkunifiedGetMsgDataOfSize(h_app, &rcv_msg, sizeof(rcv_msg), eSMRRelease);
  if (e_status != eFrameworkunifiedStatusOK) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "#CAN_thread# FrameworkunifiedGetMsgDataOfSize Error");
    if (e_status == eFrameworkunifiedStatusInvldBufSize) {
      FrameworkunifiedClearMsgData(h_app);
    }
    ret = RET_CAN_ERROR_CANCEL;
    goto exit;
  }

  if ((uint8_t)CAN_CMDID_VERSION_REQ_TX == rcv_msg.ucCmdid) {
    CAN_MSG_CANCMD data = {0x00};
    std::string version_info;
//    CANHAL_RET_API ret;

//    ret = CanGetVersion(&version_info);
//    if (CANHAL_RET_NORMAL != ret) {
//      return eFrameworkunifiedStatusFail;
//    }

    if (CAN_TX_CMD_VERSION_SIZE != version_info.length()) {
      return eFrameworkunifiedStatusFail;
    }

    memcpy(data.data.data, version_info.c_str(), CAN_TX_CMD_VERSION_SIZE);

//    data.hdr.hdr.cid = CID_CAN_CMD_DELIVERY;
//    data.hdr.hdr.rid = 0;
//    data.hdr.hdr.msgbodysize = CAN_TX_CMD_DELIVERY_SIZE + CAN_TX_CMD_VERSION_SIZE;
    data.data.cmd_id = CAN_CMDID_VERSION_RESP_RX;

    HANDLE client = FrameworkunifiedMcOpenSender(h_app, rcv_msg.notifyName);  // LCOV_EXCL_BR_LINE 11:except,C++ STL
    if (NULL == client) {
      return eFrameworkunifiedStatusFail;
    }

    e_status = FrameworkunifiedSendMsg(client, CID_CAN_CMD_DELIVERY, sizeof(CAN_MSG_CANCMD), &data);  // LCOV_EXCL_BR_LINE 200: unexpect branch  //NOLINT (readability/naming)
    if (eFrameworkunifiedStatusOK != e_status) {
      return eFrameworkunifiedStatusFail;
    }

    return eFrameworkunifiedStatusOK;
  }

  /* Reset request delivery request */
  if ((uint8_t)CAN_CMDID_FUELCALC_RST_REQ_DELIVERY == rcv_msg.ucCmdid) { /* #104 */
    b_tx_rslt_flag = FALSE;

    /* Check reset reception status */
    if (TRUE == CANCommandFuelCalcRstReqCheck()) {
      /* Cost reset request received in CAN section */
      b_dlvry_flag = FALSE;
      b_snd_msg_flag = TRUE;
    } else {
      /* CAN section flammability reset request not received */
      b_com_flag = FALSE;
      if (FALSE == CANCommandDeliveryEntryCheck(rcv_msg.notifyName, rcv_msg.ucCmdid)) {
        ret = RET_CAN_ERROR_BUFFULL;             /* CAN-command delivery control table FULL */
        goto exit;
      }
    }
  } else {
    /* Delivery Registration? */
    if ((uint8_t)CAN_CMDID_FUELCALC_REQ_TX == rcv_msg.ucCmdid) { /* #104 */
      /* Not registered for delivery */
      b_dlvry_flag = FALSE;
    } else {
      /* Register for delivery */
      if (FALSE == CANCommandDeliveryEntryCheck(rcv_msg.notifyName, rcv_msg.ucCmdid)) {
        ret = RET_CAN_ERROR_BUFFULL;           /* CAN-command delivery control table FULL */
        goto exit;
      }
    }

    /* Register transmission result notification? */
    if ((uint8_t)CAN_RID_NOTUSE_CODE == rcv_msg.ucRid) { /* #104 */
      /* Not to register transmission result notification */
      b_tx_rslt_flag = FALSE;
    } else {
      /* Register transmission result notification */
      if (FALSE == CANCommandTxRsltEntryCheck(rcv_msg.ucCmdid, &uc_can_rid)) {
        ret = RET_CAN_ERROR_BUFFULL;                                         /* CAN-command-send-result-management-table FULL */
        goto exit;
      }
    }
  }

  if (TRUE == b_snd_msg_flag) {
    /* CAN section flame reset request message transmission */
    CANCommandFuelCalcRstReqSndMsg(h_app, rcv_msg.notifyName);
  } else {
    if (TRUE == b_dlvry_flag) {
      /* CAN command delivery registration */
      CANCommandDeliveryEntry(rcv_msg.notifyName, rcv_msg.ucCmdid);
    }
    if (TRUE == b_tx_rslt_flag) {
      /* CAN command transmission result notification registration */
      CANCommandTxRsltEntry(uc_can_rid, rcv_msg.notifyName, rcv_msg.ucRid);
    }
    if (TRUE == b_com_flag) {
//      CanMessage st_can_data;
//
//      st_can_data.can_id = rcv_msg.ucCmdid;
//      st_can_data.rid = uc_can_rid;
//      st_can_data.dlc = (uint8_t)CAN_CMDSND_DATA_SIZE;

//      if (CANHAL_RET_NORMAL != CanSend(&st_can_data, CAN_HAL_TYPE_CAN)) {
//        ret = RET_CAN_ERROR_PARAM;
//        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "#CAN_thread# CAN Transmission CANHAL Error");  // LCOV_EXCL_BR_LINE 15: marco defined in "native_service/ns_logger_if.h" // NOLINT (whitespace/line_length)
//      }
    }
  }

exit:
  FRAMEWORKUNIFIEDLOG(ZONE_CAN_DEBUG, __func__, "ret=%x", ret);  // LCOV_EXCL_BR_LINE 15: marco defined in "native_service/ns_logger_if.h" // NOLINT (whitespace/line_length)
  if (ret == RET_CAN_NORMAL)
    return eFrameworkunifiedStatusOK;
  else
    return eFrameworkunifiedStatusFail;
  // return RET_CAN_NORMAL;
}

/*******************************************************************************
 * MODULE    : CANCommandTxRslt
 *******************************************************************************/
/*!-----------------------------------------------------------------------------
 * @~english
 * @brief    CAN command transmission result notification processing
 *
 * CAN command transmission result notification processing
 *
 * Notify the result of CAN command transmission 
 *
 * @~english
 * @note
 *   -# Transmission result status acquisition
 *   -# Transmission result notification registration detection
 *   -# Message sending
 *   -# Result notification registration deletion
 *
 * @~english
 * @return  Transmission result
 * @retval  <RET_CAN_NORMAL>        Normality
 * @retval  <RET_CAN_ERROR_CANCEL>  Data annulment
 *
 * @~english
 * @param[in]     <pst_rcv_msg> Reception message reference pointer
 */
RET_CAN CANCommandTxRslt(HANDLE h_app, CAN_MSG_DATA *pst_rcv_msg) {  // LCOV_EXCL_START 8:this IF is called in CANIcrSndStsProcess, and CAN command TX to can_hal, but not ICR // NOLINT (whitespace/line_length)
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  RET_CAN ret = RET_CAN_NORMAL;   /* Return value of this function */
//  T_ICR_CMDSNDCNF_STS *pst_msg_data; /* Receive data structure pointer */
  uint8_t uc_status;                /* Reception status */

  /* Type conversion */
//  pst_msg_data = reinterpret_cast<T_ICR_CMDSNDCNF_STS *>(reinterpret_cast<void *>(&pst_rcv_msg->ucData[0]));

  /* Acquisition of transmission result status */
//  switch (pst_msg_data->cnf_sts) {
//    case ICR_SND_SUCCESS:
//      uc_status = CAN_SUCCESS; /* Transmission completion */
//      break;
//    case ICR_SND_ERR_RETRYOUT:
//      uc_status = CAN_RETRYOUT;                                                   /* Re-tryout */
//      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "#CAN_thread# CAN Transmission SYSCOM Error");
//      CANDebugLogOut(CAN_TRANCE_SYSCOM_ERR, reinterpret_cast<void *>(pst_rcv_msg));
//      break;
//    case ICR_SND_ERR_BUFFERFULL:
//      uc_status = CAN_BUFFERFUL;                                                  /* Transmit Buffer Full */
//      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "#CAN_thread# CAN Transmission SYSCOM Error");
//      CANDebugLogOut(CAN_TRANCE_SYSCOM_ERR, reinterpret_cast<void *>(pst_rcv_msg));
//      break;
//    default:
//      ret = RET_CAN_ERROR_CANCEL;                                               /* Data destruction */
//      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "#CAN_thread# CAN Transmission SYSCOM Error");
//      CANDebugLogOut(CAN_TRANCE_SYSCOM_ERR, reinterpret_cast<void *>(pst_rcv_msg));
//      break;
//  }

  if (RET_CAN_NORMAL == ret) {
    /* Send result notification registration detection */
//    if (FALSE == CANCommandTxRsltCheck(pst_rcv_msg->stHead.hdr.rid)) {
//      ret = RET_CAN_ERROR_CANCEL; /* Data destruction */
//    }
  }

  if (RET_CAN_NORMAL == ret) {
    /* Send Result Notification Message */
//    CANCommandTxRsltSndMsg(h_app, pst_rcv_msg->stHead.hdr.rid, uc_status);

    /* Delete registered result notification */
//    CANCommandTxRsltDelete(pst_rcv_msg->stHead.hdr.rid);
  }

  return (ret);
}
// LCOV_EXCL_STOP

/*******************************************************************************
 * MODULE    : CANCommandDelivery
 ******************************************************************************/
/*!-----------------------------------------------------------------------------
 * @~english
 * @brief    CAN command delivery processing
 *
 * CAN command delivery processing
 *
 * Deliver CAN commands 
 *
 * @~english
 * @note
 *   -# CAN command ID conversion
 *   -# Control flag operation (The reset demand is received).
 *   -# Message sending
 *   -# Delivery registration deletion
 *
 * @~english
 * @return  Delivery result
 * @retval  <RET_CAN_NORMAL>        Normality
 * @retval  <RET_CAN_ERROR_CANCEL>  Data annulment
 *
 * @~english
 * @param[in]     <pst_rcv_msg> Reception message reference pointer
 */
RET_CAN CANCommandDelivery(HANDLE h_app, CAN_MSG_DATA *pst_rcv_msg) {
  RET_CAN ret = RET_CAN_NORMAL; /* Return value of this function */
//  T_ICR_CMD_DATA *pst_msg_data;    /* Receive data structure pointer */
  uint8_t uc_cmd_id;               /* Receive CAN command ID */

  /* Type conversion */
//  pst_msg_data = reinterpret_cast<T_ICR_CMD_DATA *>(reinterpret_cast<void *>(&pst_rcv_msg->ucData[0]));

  /* OPC-> CAN command ID conversion */
//  if (FALSE == CANCommandidOpcToUserCvt(pst_msg_data->opc, &uc_cmd_id)) {  // LCOV_EXCL_BR_LINE 6: double check
//    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//    ret = RET_CAN_ERROR_CANCEL; /* Data destruction */  // LCOV_EXCL_LINE 6: double check
//  }

  if (RET_CAN_NORMAL == ret) {  // LCOV_EXCL_BR_LINE 200: ret will not assigned to other value forever
    /* Set the control flag to received if CAN command section flagreset request */
    if ((uint8_t)CAN_CMDID_FUELCALC_RST_REQ_RX == uc_cmd_id) {
      /* Receive Reset Request */
      (void)CANCommandFuelCalcRstReq();
    }

    /* Send Delivery Message */
//    if (!CANCommandDeliverySndMsg(h_app, uc_cmd_id, &pst_msg_data->data[0])) {
//      ret = RET_CAN_ERROR_PARAM;
//    }

    /* Delete Delivery Registration */
    CANCommandDeliveryDelete(uc_cmd_id);
  }

  return (ret);
}
