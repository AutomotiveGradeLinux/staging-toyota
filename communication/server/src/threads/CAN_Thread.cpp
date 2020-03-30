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
 * SYSTEM  :_CWORD107_
 * SUBSYSTEM :EXL process
 * Module configuration :CAN_Thread()    CAN thread domain processing
 *                    CANMainThreadInit()  CAN thread initialization processing
 *                    CANTimeOutMsg()   Timeout message processing
 *                    CANCommonTimerStart() CAN thread common timer start processing
 *                    CANBufferLogOut()  CAN thread buffer log output processing
 *                    CANDebugLogOut()   Debug log output processing
 ******************************************************************************/
/*!-----------------------------------------------------------------------------
 * @file    CAN_Thread.cpp
 * @~english
 * @brief   CAN Thread
 */
#include "CAN_Thread.h"

extern "C" {
#include <poll.h>  //NOLINT (build/include)
#include <sys/timerfd.h>
}
#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_timer.h>
#include <peripheral_service/Canif_API.h>
#include <peripheral_service/communication_notifications.h>
//#include <can_hal.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <agl_thread.h>
#include <string>
#include "CAN_Delivery.h"
#include "CAN_DeliveryData.h"
#include "CAN_Transmission.h"
#include "CAN_TransmissionData.h"
#include "CAN_CommWatch.h"
#include "CAN_CommWatchData.h"
#include "CAN_Command.h"
#include "CAN_CommandData.h"
#include "CAN_TxMsg.h"
#include "CAN_TimerCtrl.h"

#include "communication_communicationlog.h"
#include "communication_cid.h"
#include "Thread_Common.h"

#define NTFY_PSM_INITCOMP MN_PS_PSMSHADOW"/InitComp"

//static EFrameworkunifiedStatus CANHalRcvProcess(CanMessage *p);
//static EFrameworkunifiedStatus CANHal_CWORD29_RcvProcess(CanMessage_CWORD29_ *p);
//static EFrameworkunifiedStatus CANHalSndStsProcess(CanSendResult *p);
//static EFrameworkunifiedStatus CANHal_CWORD29_SndStsProcess(CanSendResult_CWORD29_ *p);
static EFrameworkunifiedStatus CANIcrCmdRcvProcess(HANDLE h_app);
static EFrameworkunifiedStatus CANIcrSndStsProcess(HANDLE h_app);

/*************************************************/
/*           Global variable                      */
/*************************************************/
static const FrameworkunifiedProtocolCallbackHandler kCanPcbhs[] = {  // LCOV_EXCL_BR_LINE 11: unexpected branch
//    {CID_ICR_CMD_RCV,  CANIcrCmdRcvProcess},
//    {CID_ICR_CMD_SND_CNF, CANIcrSndStsProcess},
    {CID_CANIF_DELIVERY_ENTRY, CANDeliveryEntry},
    {CID_CANIF__CWORD29__DELIVERY_ENTRY, CAN_CWORD29_DeliveryEntry},
    {CID_COMMSYS_TIMEOUT, CANCallbackForTimeOut},
#ifdef CAN_DEBUG
    {CID_CANIF_DELETE_DELIVERY_ENTRY, CANAllDeleteDeliveryEntry},
#endif
    {CID_CANIF_TX_START, CANTxStart},
    {CID_CANIF_TX_STOP, CANFreqTxStop},
    {CID_CANIF_TX_BIT_START, CANTxBitStart},
    {CID_CANIF_TX_BIT_STOP, CANTxBitStop},
    {CID_CANIF_COMM_WATCH, CANCommWatch},
    {CID_CANIF__CWORD29__TX_START, CAN_CWORD29_TxMsg},
    {CID_CANIF_CMD_CTRL, CANCommandTransmission},
};

static UI_32 kCanPcbhsD[] = {  // LCOV_EXCL_BR_LINE 11: unexpected branch
//    CID_ICR_CMD_RCV,
//    CID_ICR_CMD_SND_CNF,
    CID_CANIF_DELIVERY_ENTRY,
    CID_CANIF__CWORD29__DELIVERY_ENTRY,
    CID_COMMSYS_TIMEOUT,
#ifdef CAN_DEBUG
    CID_CANIF_DELETE_DELIVERY_ENTRY,
#endif
    CID_CANIF_TX_START,
    CID_CANIF_TX_STOP,
    CID_CANIF_TX_BIT_START,
    CID_CANIF_TX_BIT_STOP,
    CID_CANIF_COMM_WATCH,
    CID_CANIF__CWORD29__TX_START,
    CID_CANIF_CMD_CTRL
};

static const FrameworkunifiedProtocolCallbackHandler kCanDebugPcbhs[] = {  // LCOV_EXCL_BR_LINE 11: unexpected branch
    {CID_CANIF_COMMWATCH_CLEAR_FOR_DEBUG, CANCommWatchAllClearDebug},
};

static UI_32 kCanDebugPcbhsD[] = {  // LCOV_EXCL_BR_LINE 11: unexpected branch
    CID_CANIF_COMMWATCH_CLEAR_FOR_DEBUG,
};

//static CanHalParam canParam;

//static EFrameworkunifiedStatus CANStateCallback(DEVICE_STATE state, void *ctxid) {
//  HANDLE h_app = canParam.callbacks.can.cmn.ctxid_state;
//  CommSetAvailabilityCurrent(CAN_AVAILABILITY);
//  return CommonStartNotify(h_app, NTFY_Communication_CAN_ISAVAILABLE);
//}

EFrameworkunifiedStatus CANThreadStart(HANDLE h_app) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;

  CommClrAvailabilityCurrent(CAN_AVAILABILITY);
  CANDeliveryDataInit();
  CANTransmissionDataInit();
  CANCommWatchDataInit();
  CANCommandDataInit();
  CANCommWatchInit();
  CANTimerTblInit();

//  canParam.type = CAN_HAL_TYPE_CAN;
//  canParam.callbacks.can.cmn.ctxid_state = h_app;
//  canParam.callbacks.can.cmn.StateCallback = CANStateCallback;
//  canParam.callbacks.can.cmn.ctxid_error = h_app;
//  canParam.callbacks.can.cmn.ErrorCallback = CommonCanHalErrorNotify;
//  canParam.callbacks.can.cmn.ctxid_tx = h_app;
//  canParam.callbacks.can.cmn.SendResultCallback = CANHalSndStsProcess;
//  canParam.callbacks.can.cmn.ctxid_rx = h_app;
//  canParam.callbacks.can.cmn.ReceiveCallback = CANHalRcvProcess;
//  canParam.callbacks.can.ctxid_tx__CWORD118_ = h_app;
//  canParam.callbacks.can.SendResultCallback_CWORD29_ = CANHal_CWORD29_SndStsProcess;
//  canParam.callbacks.can.ctxid_rx__CWORD118_ = h_app;
//  canParam.callbacks.can.ReceiveCallback_CWORD29_ = CANHal_CWORD29_RcvProcess;
//  e_status = CommonCanHalThreadStart(h_app, kCanPcbhs, _countof(kCanPcbhs),
//                          NTFY_Communication_CAN_ISAVAILABLE, &canParam);

  e_status = FrameworkunifiedAttachCallbacksToDispatcher(h_app,
                        "CAN_TEST", kCanDebugPcbhs, _countof(kCanDebugPcbhs));
  if (e_status != eFrameworkunifiedStatusOK) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__,
            "FrameworkunifiedAttachCallbacksToDispatcher Error(%d)", e_status);
    return e_status;
  }

  return e_status;
}

EFrameworkunifiedStatus CANThreadStop(HANDLE h_app) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;

  CommClrAvailabilityCurrent(CAN_AVAILABILITY);
//  e_status = CommonCanHalThreadStop(h_app, kCanPcbhsD, _countof(kCanPcbhsD),
//                              NTFY_Communication_CAN_ISAVAILABLE,
//                              CAN_HAL_TYPE_CAN);

  e_status = FrameworkunifiedDetachCallbacksFromDispatcher(h_app, "CAN_TEST",
                              kCanDebugPcbhsD, _countof(kCanDebugPcbhsD));
  if (e_status != eFrameworkunifiedStatusOK) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__,
           "CommunicationDettachCallbacksFromDispatcher Error(%d)", e_status);
    return e_status;
  }

  return e_status;
}

//static EFrameworkunifiedStatus CANHalRcvProcess(CanMessage *p) {
//  RET_CAN ret = RET_CAN_ERROR_CANCEL;
////  T_ICR_CMD_DATA *p_can_data = NULL;
//  CanMessage canhal_recv_data;
//  CAN_MSG_DATA st_recv_msg;
//  BOOL bcmdopc = FALSE; /* CAN command OPC judgment */
//  int32_t lcnt;         /* General Variables */
//  HANDLE h_app = canParam.callbacks.can.cmn.ctxid_rx;
//
//  memcpy(&canhal_recv_data, p, sizeof(canhal_recv_data));
//  if (canhal_recv_data.dlc > CAN_MESSAGE_LEN) {
//    return eFrameworkunifiedStatusFail;
//  }
//
//  const uint8_t ucOpcTable[] = {
//      /* OPC search table */
//      CAN_CMDID_STARTUP_FIN_RESP_RX,     /* CAN startup completion notice received */
//      CAN_CMDID_MRST_INFO_RESP_RX,       /* CAN Master Reset Information Notification Reception */
//      CAN_CMDID_VERSION_RESP_RX,         /* CAN Version Response Reception */
//      CAN_CMDID_CONNECTION_NODE_RESP_RX, /* Receive CAN Connection Node Notification Response */
//      CAN_CMDID_FUELCALC_RST_REQ_RX,     /* RECEIVE REQUEST FREE OF CAN SECTION */
//  };
//
//  bcmdopc = FALSE;
//  for (lcnt = 0; lcnt < (int32_t)((sizeof(ucOpcTable)) / (sizeof(uint8_t))); lcnt++) {
//    if ((int32_t)canhal_recv_data.can_id == (int32_t)ucOpcTable[lcnt]) {
//      bcmdopc = TRUE;
//      break;
//    }
//  }
//
//  if (FALSE == bcmdopc) {
////    p_can_data = reinterpret_cast<T_ICR_CMD_DATA *>(&st_recv_msg.ucData[0]);
////    p_can_data->opc = CAN_OPC_RX;
////    CANCanidUserToIfCvt(canhal_recv_data.can_id, p_can_data->data);
////    p_can_data->data[CAN_MSGBUF_DLC_POS] = static_cast<uint8_t>(canhal_recv_data.dlc);
////    memcpy(&(p_can_data->data[CAN_MSGBUF_DATA_POS]),
////           canhal_recv_data.data,
////           canhal_recv_data.dlc);
//    ret = CANDataReceiveMsg(h_app, &st_recv_msg);
//  } else {
////    p_can_data = reinterpret_cast<T_ICR_CMD_DATA *>(&st_recv_msg.ucData[0]);
////    CANCommandidUserToOpcCvt((uint8_t)canhal_recv_data.can_id, &p_can_data->opc);
//
//    switch (canhal_recv_data.can_id) {  // LCOV_EXCL_LINE 6: double check by ucOpcTable
//      case CAN_CMDID_STARTUP_FIN_RESP_RX:
////        p_can_data->opc = CAN_OPC_COMMAND_STARTUP_FIN_RESP_RX;
//        break;
//      case CAN_CMDID_MRST_INFO_RESP_RX:
////        p_can_data->opc = CAN_OPC_COMMAND_MRST_INFO_RESP_RX;
//        break;
//      case CAN_CMDID_VERSION_RESP_RX:
////        p_can_data->opc = CAN_OPC_COMMAND_VERSION_RESP_RX;
//        break;
//      case CAN_CMDID_CONNECTION_NODE_RESP_RX:
////        p_can_data->opc = CAN_OPC_COMMAND_CONNECTION_NODE_RESP_RX;
//        break;
//      case CAN_CMDID_FUELCALC_RST_REQ_RX:
////        p_can_data->opc = CAN_OPC_COMMAND_FUELCALC_RST_REQ_RX;
//        break;
//        // LCOV_EXCL_START 6:Excluded due to value never returned at present
//      default:
//        return eFrameworkunifiedStatusFail;
//        // LCOV_EXCL_STOP 6:Excluded due to value never returned at present
//    }
//
////    CANCanidUserToIfCvt(canhal_recv_data.can_id, p_can_data->data);
////    memcpy(&(p_can_data->data[0]),
////           canhal_recv_data.data,
////           canhal_recv_data.dlc);
//    ret = CANCommandDelivery(h_app, &st_recv_msg);
//  }
//
//  if (RET_CAN_NORMAL != ret) {
//    return eFrameworkunifiedStatusFail;
//  } else {
//    return eFrameworkunifiedStatusOK;
//  }
//}

//static EFrameworkunifiedStatus CANHal_CWORD29_RcvProcess(CanMessage_CWORD29_ *p) {
//  RET_CAN ret = RET_CAN_ERROR_CANCEL;
//  CanMessage_CWORD29_ canhal_recv_data;
//  HANDLE h_app = canParam.callbacks.can.ctxid_rx__CWORD118_;
//
//  memcpy(&canhal_recv_data, p, sizeof(canhal_recv_data));
//  if (canhal_recv_data.dlc > CAN_L_MESSAGE_LEN) {
//    return eFrameworkunifiedStatusFail;
//  }
//
//  ret = CANDeliveryRcv_CWORD29_Data(h_app, canhal_recv_data.opc, canhal_recv_data.dlc, canhal_recv_data.data);
//
//  if (RET_CAN_NORMAL != ret) {
//    return eFrameworkunifiedStatusFail;
//  } else {
//    return eFrameworkunifiedStatusOK;
//  }
//}

//static EFrameworkunifiedStatus CANHalSndStsProcess(CanSendResult *p) {
//  BOOL bcmdopc = FALSE; /* CAN command OPC judgment */
//  int32_t lcnt;
//  BOOL ret = TRUE;
//  uint8_t ui_can_rid;
//  uint8_t uc_sts;
//  CanSendResult canhal_recv_data;
//  HANDLE h_app = canParam.callbacks.can.cmn.ctxid_tx;
//
//  memcpy(&canhal_recv_data, p, sizeof(canhal_recv_data));
//  ui_can_rid = canhal_recv_data.rid;
//  if (CAN_RID_NOTUSE_CODE == ui_can_rid) {
//    return eFrameworkunifiedStatusOK;
//  }
//
//  if (CAN_SEND_RESULT_SUCCESS == canhal_recv_data.result) {
//    uc_sts = CAN_SUCCESS;
//  } else {
//    uc_sts = CAN_RETRYOUT;
//  }
//
//  const uint8_t ucOpcTable[] = {
//      /* OPC search table */
//      CAN_CMDID_MRST_INFO_REQ_TX,       /* CAN master reset information notification request transmission */
//      CAN_CMDID_CONNECTION_NODE_REQ_TX, /* CAN connection node notification request transmission */
//      CAN_CMDID_FUELCALC_REQ_TX,        /* CAN section flame reset response transmission */
//  };
//
//  bcmdopc = FALSE;
//  for (lcnt = 0; lcnt < (int32_t)((sizeof(ucOpcTable)) / (sizeof(uint8_t))); lcnt++) {
//    if ((int32_t)canhal_recv_data.can_id == (int32_t)ucOpcTable[lcnt]) {
//      bcmdopc = TRUE;
//      break;
//    }
//  }
//
//  if (bcmdopc) {
//    ret = CANCommandTxRsltSndMsg(h_app, ui_can_rid, uc_sts);
//
//    if (TRUE == ret) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
//      CANCommandTxRsltDelete(ui_can_rid);
//    }
//  } else {
//    ret = CANTxRsltSndMsg(h_app, ui_can_rid, uc_sts);
//
//    if (TRUE == ret) {
//      CANTxRsltFin(ui_can_rid); /* CANDataTransmission result notification completed */
//    }
//  }
//
//  if (FALSE == ret) {
//    return eFrameworkunifiedStatusFail;
//  } else {
//    return eFrameworkunifiedStatusOK;
//  }
//}

//static EFrameworkunifiedStatus CANHal_CWORD29_SndStsProcess(CanSendResult_CWORD29_ *p) {
//  BOOL ret = TRUE;
//  uint8_t ui_can_rid;
//  uint8_t uc_sts;
//  CanSendResult_CWORD29_ canhal_recv_data;
//  HANDLE h_app = canParam.callbacks.can.ctxid_tx__CWORD118_;
//
//  memcpy(&canhal_recv_data, p, sizeof(canhal_recv_data));
//  ui_can_rid = canhal_recv_data.rid;
//  if (CAN_RID_NOTUSE_CODE == ui_can_rid) {
//    return eFrameworkunifiedStatusOK;
//  }
//
//  if (CAN_SEND_RESULT_SUCCESS == canhal_recv_data.result) {
//    uc_sts = CAN_SUCCESS;
//  } else {
//    uc_sts = CAN_RETRYOUT;
//  }
//
//  ret = CAN_CWORD29_TxRsltSndMsg(h_app, ui_can_rid, uc_sts);
//  if (TRUE == ret) {  // LCOV_EXCL_LINE 11:Excluded due to gcov constraints (others)
//    CAN_CWORD29_TxRsltFin(ui_can_rid); /* CANDataTransmission result notification completed */
//    return eFrameworkunifiedStatusOK;
//  } else {
//    return eFrameworkunifiedStatusFail;
//  }
//}

static EFrameworkunifiedStatus  CANIcrCmdRcvProcess(HANDLE h_app) {
  RET_CAN l_ret = RET_CAN_INIT_SUCCESS; /* Return value of the function */
  BOOL bCmdOpc = FALSE;                /* CAN command OPC judgment */
  int32_t l_cnt;                        /* General Variables */
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
//  T_ICR_CMD_RCV rcv_msg;

  const uint8_t ucOpcTable[] = {
      /* OPC search table */
      CAN_OPC_COMMAND_STARTUP_FIN_RESP_RX,     /* CAN startup completion notice received */
      CAN_OPC_COMMAND_MRST_INFO_RESP_RX,       /* CAN Master Reset Information Notification Reception */
      CAN_OPC_COMMAND_VERSION_RESP_RX,         /* CAN Version Response Reception */
      CAN_OPC_COMMAND_CONNECTION_NODE_RESP_RX, /* Receive CAN Connection Node Notification Response */
      CAN_OPC_COMMAND_BUS_STATUS_RESP_RX,      /* CAN Bus Status Notification Response Reception */
      CAN_OPC_COMMAND_FUELCALC_RST_REQ_RX,     /* RECEIVE REQUEST FREE OF CAN SECTION */
  };

//  e_status = FrameworkunifiedGetMsgDataOfSize(h_app, &rcv_msg, sizeof(rcv_msg), eSMRRelease);
  if (e_status != eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
  // LCOV_EXCL_START 4: NSFW error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "#CAN_thread# FrameworkunifiedGetMsgDataOfSize Error");
    if (e_status == eFrameworkunifiedStatusInvldBufSize) {
      FrameworkunifiedClearMsgData(h_app);
    }
    l_ret = RET_CAN_ERROR_CANCEL;
    goto exit;
  }
  // LCOV_EXCL_STOP

//  FRAMEWORKUNIFIEDLOG(ZONE_CAN_DEBUG, __func__, "from:%02x to:%02x opc:%02x", rcv_msg.cmd_data.lgadr_from, rcv_msg.cmd_data.lgadr_to, rcv_msg.cmd_data.opc);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT (whitespace/line_length)
//  if (rcv_msg.cmd_data.lgadr_from == (uint8_t)CAN_TX_TO_ADRS) {
    bCmdOpc = FALSE;
    for (l_cnt = 0; l_cnt < (int32_t)((sizeof(ucOpcTable)) / (sizeof(uint8_t))); l_cnt++) {
//      if ((int32_t)rcv_msg.cmd_data.opc == (int32_t)ucOpcTable[l_cnt]) {  // LCOV_EXCL_BR_LINE 8: cmd recv is CANHalRcvProcess  // NOLINT (whitespace/line_length)
        bCmdOpc = TRUE;
        break;
//      }
    }

    if (FALSE == bCmdOpc) {  // LCOV_EXCL_BR_LINE 8: cmd recv is CANHalRcvProcess
      FRAMEWORKUNIFIEDLOG(ZONE_CAN_DEBUG, __func__, "##### CAN RCVDATA #####");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT (whitespace/line_length)
//      l_ret = CANDataReceiveMsg(h_app, reinterpret_cast<CAN_MSG_DATA *>(&rcv_msg)); /* CANDataProcess of Received Messages */
    } else {
      // LCOV_EXCL_START 8: same as above
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_CAN_DEBUG, __func__, "##### CAN RCVCMD #####");
//      l_ret = CANCommandDelivery(h_app, reinterpret_cast<CAN_MSG_DATA *>(&rcv_msg)); /* CAN command delivery process */
      // LCOV_EXCL_STOP
    }

//  }
exit:
  if (l_ret == RET_CAN_NORMAL)
    return eFrameworkunifiedStatusOK;
  else
    return eFrameworkunifiedStatusFail;
}

static EFrameworkunifiedStatus CANIcrSndStsProcess(HANDLE h_app) {  // LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  RET_CAN l_ret = RET_CAN_INIT_SUCCESS; /* Return value of the function */
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
//  T_ICR_CMDSNDCNF rcv_msg;

//  e_status = FrameworkunifiedGetMsgDataOfSize(h_app, &rcv_msg, sizeof(rcv_msg), eSMRRelease);
  if (e_status != eFrameworkunifiedStatusOK) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "#CAN_thread# FrameworkunifiedGetMsgDataOfSize Error");
    if (e_status == eFrameworkunifiedStatusInvldBufSize) {
      FrameworkunifiedClearMsgData(h_app);
    }
    e_status = eFrameworkunifiedStatusFail;
    goto exit;
  }

//  if (((uint32_t)CAN_CMD_RID_MASK & (uint32_t)rcv_msg.msg_hdr.hdr.rid) != 0) {
    /* CAN command transmission result notification processing */
//    l_ret = CANCommandTxRslt(h_app, reinterpret_cast<CAN_MSG_DATA *>(&rcv_msg));
//  } else {
    /* CANDataReceive message processing of transmission result */
//    l_ret = CANTxRsltReceiveMsg(h_app, reinterpret_cast<CAN_MSG_DATA *>(&rcv_msg));
//  }
exit:
  if (l_ret == RET_CAN_NORMAL)
    return eFrameworkunifiedStatusOK;
  else
    return eFrameworkunifiedStatusFail;
}
// LCOV_EXCL_STOP 8: dead code

/*******************************************************************************
 * MODULE    : CANTimeOutMsg
 * ABSTRACT  : Timeout message processing
 * FUNCTION  : Timeout message processing
 * ARGUMENT  : *pst_rcv_msg   : Received Message Reference Pointer
 * NOTE      : 
 * RETURN    : None
 ******************************************************************************/
EFrameworkunifiedStatus CANCallbackForTimeOut(HANDLE h_app) {
  CANTimeOutMsg(h_app, CAN_TIMER_SEQNO_CODE);
  return eFrameworkunifiedStatusOK;
}

/*******************************************************************************
 * MODULE    : CANTimeOutMsg
 * ABSTRACT  : Timeout message processing
 * FUNCTION  : Timeout message processing
 * ARGUMENT  : *pst_rcv_msg   : Received Message Reference Pointer
 * NOTE      : 
 * RETURN    : None
 ******************************************************************************/
void CANTimeOutMsg(HANDLE h_app, uint16_t timer_seq) {
  CANFreqTransTimeOut(h_app, timer_seq, CAN_PROTOCOL_TYPE_CAN);    /* CAN periodic transmission timeout check   */
  CANFreqTransTimeOutMap(h_app);
  CANCommWatchTimeOut(h_app, CAN_PROTOCOL_TYPE_CAN);    /* Confirmation of CAN communication interruption monitoring timeout  */
}

/*******************************************************************************
 * MODULE    : CANBufferLogOut
 * ABSTRACT  : CAN thread buffer log output processing
 * FUNCTION  : CAN thread buffer log output processing
 * ARGUMENT  : None
 * NOTE      : 
 * RETURN    : None
 ******************************************************************************/
void CANBufferLogOut(void) {  // LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FILE *afp_log = NULL;

  afp_log = fopen(CAN_BUFFER_LOGOUT_NAME, "r"); /* Opening Log Files in Read Mode */

  if (NULL != afp_log) {
    /* When a log file is opened   */
    (void)fclose(afp_log);                        /* Close the log file   */
    afp_log = fopen(CAN_BUFFER_LOGOUT_NAME, "a"); /* Opening Log Files in Append Mode */

    if (NULL != afp_log) {
      /* When a log file is opened   */
      CANDeliveryBufferOut(afp_log); /* CAN shipping table log output   */
      (void)fprintf(afp_log, "\n\n\n\n\n");
      CANCycleTransBufferOut(afp_log); /* CAN Periodic Transmission Table Log Output  */
      (void)fprintf(afp_log, "\n\n\n\n\n");
      CANCommWatchBufferOut(afp_log, CAN_PROTOCOL_TYPE_CAN); /* CAN communication disruption table log output  */
      (void)fprintf(afp_log, "\n\n\n\n\n");

      (void)fclose(afp_log);                                     /* Close the log file    */
    }
  }
}
// LCOV_EXCL_STOP

/*******************************************************************************
 * MODULE    : CANDebugLogOut
 * ABSTRACT  : Debug log output processing
 * FUNCTION  : Output a debug log
 * ARGUMENT  : u_int16 err_code Error code
 *           : void* msg_ptr  Message start pointers
 * NOTE      :
 * RETURN    :
 ******************************************************************************/
void CANDebugLogOut(uint16_t err_code, void *msg_ptr) {
  CAN_MSG_DATA *can_msg;
  uint16_t i;
  uint16_t data_sz;
  uint8_t *data_p;
  int8_t out_cha[100];

  can_msg = reinterpret_cast<CAN_MSG_DATA *>(msg_ptr);
  data_p = reinterpret_cast<uint8_t *>(msg_ptr);
//  data_sz = (uint16_t)((uint32_t)can_msg->stHead.hdr.msgbodysize + sizeof(T_APIMSG_MSGBUF_HEADER_COMM));
  if (0x30 < (uint32_t)data_sz) {
    data_sz = 0x30;
  }
  for (i = 0; i < data_sz; i++) {
    (void)snprintf(reinterpret_cast<char *>(&out_cha[i * 2]), sizeof(char), "%02x", data_p[i]);
  }
  if ((uint16_t)CAN_DELIVERY_ERR == err_code) {  // LCOV_EXCL_BR_LINE 8: dead code(CAN_DELIVERY_ERR is impossible)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_CAN_DEBUG, __func__, "#CAN_thread# ERRCODE:%04x %s", err_code, &out_cha[0]);  // LCOV_EXCL_LINE 8:dead code  //NOLINT (readability/naming)
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "#CAN_thread# ERRCODE:%04x %s", err_code, &out_cha[0]);
  }
}  // LCOV_EXCL_LINE 10: final line
