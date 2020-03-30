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
 *  SYSTEM    :_CWORD107_
 *  SUBSYSTEM  :EXL process
 *  Module configuration  :CANDeliverySndMsg( )          CANDataDelivery message transmission processing
 *                    CANFreqTransStartTxMsg( )      CANDataPeriodic transmission start message transmission processing
 *                    CAN1TimeTransStartTxMsg( )      CANDataSend one transmission start message processing
 *                    CANCommWatchSndMsg( )        CANDataTransmission of communication interruption detection/recovery message
 *                    CANTxRsltSndMsg( )          CANDataTransmission result notification message transmission processing
 *                    CANCanidUserToIfCvt( )        CAN ID Conversion Process 2
 *            CANCommandDeliverySndMsg( )      CAN command delivery message transmission processing
 *            CANCommandTxRsltSndMsg( )      CAN command transmission result notification message transmission processing
 *            CANCommandTransmissionSndMsg( )    CAN command message transmission processing
 *            CANCommandFuelCalcRstReqSndMsg( )  CAN section flame reset request reception message transmission processing
 *            CANCommandidOpcToUserCvt( )    CAN Command ID Conversion Processing 1
 *            CANCommandidUserToOpcCvt( )    CAN command ID conversion process 2
 ******************************************************************************/
/*!-----------------------------------------------------------------------------
 * @file    CAN_TxMsg.cpp
 * @~english
 * @brief   CAN Thread Message transmission process
 */
#include "CAN_TxMsg.h"

#include <string.h>  // NOLINT (build/include)
#include <other_service/strlcpy.h>
#include <native_service/frameworkunified_types.h>
#include <agl_thread.h>
#include <native_service/frameworkunified_framework_if.h>
#include <peripheral_service/Canif_API.h>
//#include <can_hal.h>
#include <map>
#include <utility>
#include <string>
#include "ICR_Common.h"

#include "CAN_Thread.h"
#include "CAN_DeliveryData.h"
#include "CAN_TransmissionData.h"
#include "CAN_CommWatchData.h"
#include "CAN_CommandData.h"

/*************************************************/
/*           Prototype declaration                    */
/*************************************************/

/*************************************************/
/*           Global variable                      */
/*************************************************/
static std::map<std::string, HANDLE_DAT> g_map_handler;
extern std::multimap<CANID, std::string> g_map_delivery_list;
extern std::multimap<uint16_t, std::string> g_map__CWORD29__delivery_list;
extern CAN_MSG_CANGWDATA g_cangw_msg_data; /* CANdata to be sent to the CANGW */
extern BOOL g_isaccoff;              // Declared in communication_application.cpp
/*******************************************************************************
 * MODULE    : CANDeliverySndMsg
 * ABSTRACT  : CANDataDelivery message transmission processing
 * FUNCTION  : CANDataSending a Delivery Message 
 * ARGUMENT  : can_id        : CAN ID
 *             uc_dlc        : DLC
 *             *puc_data        : CANDataPart pointer
 *             uc_opc        : OPC
 *             n_ta          : N_TA
 * NOTE      :
 * RETURN    : TRUE        : Message transmission success
 *         FALSE      : Message transmission failure
 ******************************************************************************/
BOOL CANDeliverySndMsg(HANDLE h_app, CANID can_id, uint8_t uc_dlc,
                       const uint8_t *puc_data, uint8_t uc_opc, uint8_t n_ta) {
  BOOL ret = TRUE;
  CAN_MSG_CANDATA st_delivery_msg;
  uint8_t uc_size;
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  HANDLE h_client;
  BOOL q_full = FALSE;
  uint16_t can_id_shift = 0;
  std::multimap<CANID, std::string>::iterator it;
  std::pair<std::multimap<CANID, std::string>::iterator, std::multimap<CANID, std::string>::iterator> rang;

  /* Initialization of delivery-message-structure to user */
  (void)memset(reinterpret_cast<void *>(&st_delivery_msg), (int32_t)0, (size_t)sizeof(st_delivery_msg));

  /****** Header section creation (excluding response destination process No.) ******/
  switch (uc_opc) {
    default:                                                       /* CAN_OPC_RX */
//      st_delivery_msg.hdr.hdr.cid = (uint16_t)CID_CAN_DATA_DELIVERY; /* CANDataDelivery notice */
      break;
  }
  /* Message data size */
//  st_delivery_msg.hdr.hdr.msgbodysize = (uint16_t)(CAN_TRX_CANID_SIZE + CAN_TRX_DLC_SIZE + 
//                                                 CAN_TRX_DLC_RESERVE_SIZE + (uint32_t)uc_dlc);
//  st_delivery_msg.hdr.hdr.rid = 0;     /* Resource ID (not used) */

  /****** Creation of data section ******/
  st_delivery_msg.data.can_id = can_id; /* CAN ID */
  st_delivery_msg.data.dlc = uc_dlc;      /* DLC */
  if ((uint8_t)CAN_DATA_SIZE < uc_dlc) {
    AGL_ASSERT_NOT_TESTED();
    uc_size = (uint8_t)CAN_DATA_SIZE;
  } else {
    uc_size = uc_dlc;
  }
  memcpy(st_delivery_msg.data.data, puc_data, (size_t)uc_size);

  it = g_map_delivery_list.find(can_id);
  if (it == g_map_delivery_list.end()) {
    /* Delivery destination unregistered log output */
    FRAMEWORKUNIFIEDLOG(ZONE_CAN_DEBUG, __func__, "#CAN_thread# CAN ID 0x%x not found in CAN Delivery entry table", can_id);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT (whitespace/line_length)
  }

  /****** Messaging ******/
  rang = g_map_delivery_list.equal_range(can_id);
  for (it = rang.first; it != rang.second; ++it) {
    // If the delivery destination is other than CANGW, datum is sent.
    if (TN_CANGW_M_RCV != it->second) {
      if (g_map_handler.count(it->second) == 0) {
        h_client = FrameworkunifiedMcOpenSender(h_app, it->second.c_str());
        if (h_client == NULL) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          ret = FALSE;  // LCOV_EXCL_LINE 4: NSFW error case
          continue;  // LCOV_EXCL_LINE 4: NSFW error case
        }
        g_map_handler[it->second].handle = h_client;
        g_map_handler[it->second].is_q_full = FALSE;
      } else {
        h_client = g_map_handler[it->second].handle;
        q_full = g_map_handler[it->second].is_q_full;
      }

      if (g_isaccoff && q_full) {
        /* Once queue full has occured during ACC OFF, do nothing */
      } else {
//        e_status = FrameworkunifiedSendMsg(h_client, st_delivery_msg.hdr.hdr.cid, sizeof(st_delivery_msg), &st_delivery_msg);
        if (eFrameworkunifiedStatusMsgQFull == e_status) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
          /* eFrameworkunifiedStatusMsgQFull */
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          g_map_handler[it->second].is_q_full = TRUE;  // LCOV_EXCL_LINE 4: NSFW error case
        }
        if (e_status != eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          ret = FALSE;  // LCOV_EXCL_LINE 4: NSFW error case
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "#CAN_thread#  FrameworkunifiedSendMsg Error(e_status:%d to:%s)", e_status, it->second.c_str());  // LCOV_EXCL_LINE 4: NSFW error case // NOLINT (whitespace/line_length)
        }
      }
    } else {  // If the destination is CANGW, the data is stored in the buffers.
      can_id_shift = (uint16_t)(can_id << 4);
      g_cangw_msg_data.data[g_cangw_msg_data.num].can_id_high = (uint8_t)(can_id_shift >> 8);
      g_cangw_msg_data.data[g_cangw_msg_data.num].can_id_low = (uint8_t)(can_id_shift & 0xFF);
      if (CANGW_DLC_MAX_SIZE < uc_dlc) {  // LCOV_EXCL_BR_LINE 6: already checked in CANHalRcvProcess
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        g_cangw_msg_data.data[g_cangw_msg_data.num].dlc = CANGW_DLC_MAX_SIZE;  // LCOV_EXCL_LINE 6: same as above
      } else {
        g_cangw_msg_data.data[g_cangw_msg_data.num].dlc = uc_dlc;
      }
      memcpy(g_cangw_msg_data.data[g_cangw_msg_data.num].data, puc_data, (size_t)CANGW_DLC_MAX_SIZE);
      g_cangw_msg_data.num++;
    }
  }

  return (ret);
}

/*******************************************************************************
 * MODULE    : CANDelivery_CWORD29_SndMsg
 * ABSTRACT  : CAN_CWORD29_ delivery messaging process
 * FUNCTION  : Send CAN_CWORD29_ delivery messages 
 * ARGUMENT  : h_app         : HANDLE
 *             us_opc        : OPC
 *             uc_dlc        : DLC
 *             *puc_data     : CANDataPart pointer
 * NOTE      :
 * RETURN    : TRUE        : Message transmission success
 *             FALSE       : Message transmission failure
 ******************************************************************************/
BOOL CANDelivery_CWORD29_SndMsg(HANDLE h_app, const uint16_t us_opc, uint32_t uc_dlc, const uint8_t *puc_data) {
  BOOL ret = TRUE;
  CAN_MSG_CAN__CWORD29_DATA st_delivery_msg;
  uint32_t uc_size;
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  HANDLE h_client;
  BOOL q_full = FALSE;
  std::multimap<uint16_t, std::string>::iterator it;
  std::pair<std::multimap<uint16_t, std::string>::iterator, std::multimap<uint16_t, std::string>::iterator> rang;
  /* Initialization of delivery-message-structure to user */
  (void)memset(reinterpret_cast<void *>(&st_delivery_msg), (int32_t)0, (size_t)sizeof(st_delivery_msg));

  /****** Creation of data section ******/
  st_delivery_msg.opc    = us_opc;      /* OPC */
  st_delivery_msg.dlc    = uc_dlc;      /* DLC */
  if ((uint32_t)CAN__CWORD29__MEM_SZ < uc_dlc) {
    uc_size = (uint32_t)CAN__CWORD29__MEM_SZ;
  } else {
    uc_size = uc_dlc;
  }
  memcpy(st_delivery_msg.data, puc_data, (size_t)uc_size);
  it = g_map__CWORD29__delivery_list.find(static_cast<const uint16_t>(us_opc));
  if (it == g_map__CWORD29__delivery_list.end()) {
    /* Delivery destination unregistered log output */
    FRAMEWORKUNIFIEDLOG(ZONE_CAN_DEBUG, __func__, "#CAN_thread# OPC 0x%x not found in CAN Delivery entry table", us_opc);
  }

  /****** Messaging ******/
  rang = g_map__CWORD29__delivery_list.equal_range(static_cast<const uint16_t>(us_opc));
  for (it = rang.first; it != rang.second; ++it) {
    if (g_map_handler.count(it->second) == 0) {
      h_client = FrameworkunifiedMcOpenSender(h_app, it->second.c_str());
      if (h_client == NULL) {
        ret = FALSE;
        continue;
      }
      g_map_handler[it->second].handle = h_client;
      g_map_handler[it->second].is_q_full = FALSE;
    } else {
      h_client = g_map_handler[it->second].handle;
      q_full = g_map_handler[it->second].is_q_full;
    }

    if (g_isaccoff && q_full) {
      /* Once queue full has occured during ACC OFF, do nothing */
    } else {
      e_status = FrameworkunifiedSendMsg(h_client, CID_CAN_DIAG_CWORD29__DELIVERY, sizeof(CAN_MSG_CAN__CWORD29_DATA), &st_delivery_msg);
      if (eFrameworkunifiedStatusMsgQFull == e_status) {
        /* eFrameworkunifiedStatusMsgQFull */
        g_map_handler[it->second].is_q_full = TRUE;
      }
      if (e_status != eFrameworkunifiedStatusOK) {
        ret = FALSE;
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "#CAN_thread#  FrameworkunifiedSendMsg Error(e_status:%d to:%s)", e_status, it->second.c_str()); // NOLINT (whitespace/line_length)
      }
    }
  }
  return (ret);
}

/*******************************************************************************
 * MODULE    : CANDeliverySndMsgToCANGW
 * ABSTRACT  : CAN-data-delivery-messaging process to the CANGW
 * FUNCTION  : CANDataSending Delivery Messages to a CANGW 
 * ARGUMENT  : h_app        : HANDLE
 *             cangw_data   : CAN_MSG_CANGWDATA
 * NOTE      :
 * RETURN    : TRUE        : Message transmission success
 *             FALSE       : Message transmission failure
 ******************************************************************************/
BOOL CANDeliverySndMsgToCANGW(HANDLE h_app, CAN_MSG_CANGWDATA *cangw_data) {
  BOOL ret = TRUE;
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  HANDLE h_client;
  BOOL q_full = FALSE;
  uint8_t data_count;

  // Memset unused space
  data_count = cangw_data->num;
  memset(reinterpret_cast<void *>(&cangw_data->data[data_count]), (int32_t)0,
         (size_t)sizeof(cangw_data->data[data_count]) * (CANGW_SND_NUM_MAX - data_count));

  if (g_map_handler.count(TN_CANGW_M_RCV) == 0) {
    h_client = FrameworkunifiedMcOpenSender(h_app, TN_CANGW_M_RCV);
    if (h_client == NULL) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      ret = FALSE;  // LCOV_EXCL_LINE 4: NSFW error case
      goto exit;  // LCOV_EXCL_LINE 4: NSFW error case
    }
    g_map_handler[TN_CANGW_M_RCV].handle = h_client;  // LCOV_EXCL_BR_LINE 200 unexpect branch
    g_map_handler[TN_CANGW_M_RCV].is_q_full = FALSE;  // LCOV_EXCL_BR_LINE 200 unexpect branch
  } else {
    h_client = g_map_handler[TN_CANGW_M_RCV].handle;   // LCOV_EXCL_BR_LINE 200 unexpect branch
    q_full = g_map_handler[TN_CANGW_M_RCV].is_q_full;  // LCOV_EXCL_BR_LINE 200 unexpect branch
  }

  if (g_isaccoff && q_full) {
    /* Once queue full has occured during ACC OFF, do nothing */
  } else {
    e_status = FrameworkunifiedSendMsg(h_client, CID_CAN_CANGW_DELIVERY, sizeof(CAN_MSG_CANGWDATA), cangw_data);
    if (eFrameworkunifiedStatusMsgQFull == e_status) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
      /* eFrameworkunifiedStatusMsgQFull */
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      g_map_handler[TN_CANGW_M_RCV].is_q_full = TRUE;  // LCOV_EXCL_LINE 4: NSFW error case
    }
    if (e_status != eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      ret = FALSE;  // LCOV_EXCL_LINE 4: NSFW error case
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "#CAN_thread#  FrameworkunifiedSendMsg Error(e_status:%d to:%s)", e_status, TN_CANGW_M_RCV);  // LCOV_EXCL_LINE 4: NSFW error case // NOLINT (whitespace/line_length)
    }
  }
exit:
  return (ret);
}

/*******************************************************************************
 * MODULE    : CANFreqTransStartTxMsg
 * ABSTRACT  : CANDataPeriodic transmission start message transmission processing
 * FUNCTION  : CANDataSending Periodic Send Data as a Send Message 
 * ARGUMENT  : uc_index        : Indexed
 * NOTE      :
 * RETURN    : TRUE        : Message transmission success
 *         FALSE      : Message transmission failure
 ******************************************************************************/
BOOL CANFreqTransStartTxMsg(HANDLE h_app, uint8_t uc_index) {
  BOOL ret = TRUE;                 /* Return value of this function */
  CAN_FREQ_TRANSMISSION_DAT st_data; /* Periodic send data acquisition buffer */

  /****** Data acquisition ******/
  CANFreqTransDataGet(uc_index, &st_data); /* Periodic transmission data acquisition */

  /****** Messaging ******/
  if ((uint8_t)0 == st_data.uc_resp_num) {
    uc_index = (uint8_t)CAN_RID_NOTUSE_CODE;
  }

  /* CANDataSend one transmission start message processing */
  if (FALSE == CAN1TimeTransStartTxMsg(h_app, uc_index, &st_data.st_can_data)) {
    ret = FALSE;      /* Return value of this function */
  }

  return (ret);
}

/*******************************************************************************
 * MODULE    : CAN1TimeTransStartTxMsg
 ******************************************************************************/
/*!-----------------------------------------------------------------------------
 * @~english
 * @brief    The transmission of CAN data transmission start message
 *
 * @~english
 * The transmission of CAN data transmission start message
 *
 * @~english
 * @note Details of process
 *   -# Creating of data division
 *   -# Message sending
 *
 * @~english
 * @return  Message transmission success/failure
 * @retval  <1>  Message transmission success
 * @retval  <0>  Message transmission failure
 *
 * @~english
 * @param[in]     <uc_can_rid> Index
 * @param[in]     <*pst_data> Pointer of transmission data
 * @param[in]     <uc_opc>    Operation code
 * @param[in]     <n_ta>     Target address
 */
BOOL CAN1TimeTransStartTxMsg(HANDLE h_app, uint8_t uc_can_rid, const CAN_DATA *pst_data) {
  BOOL b_ret = TRUE;
//  CanMessage st_can_data;
//
//  if (pst_data != NULL) {  // LCOV_EXCL_BR_LINE 200 (the caller always use the pointer of object)
//    st_can_data.can_id = pst_data->can_id;
//    st_can_data.dlc = pst_data->dlc;
//    st_can_data.rid = uc_can_rid;
//    if (pst_data->dlc < 8) {
//      memcpy(st_can_data.data, pst_data->data, pst_data->dlc);
//    }

//    if (CanSend(&st_can_data, CAN_HAL_TYPE_CAN) != CANHAL_RET_NORMAL) {
//      b_ret = FALSE;
//    }
//  }
  return b_ret;
}

/*******************************************************************************
 * MODULE    : CANCommWatchSndMsg
 * ABSTRACT  : CANDataTransmission of communication interruption detection/recovery message
 * FUNCTION  : CANDataSend disconnection detection messages or recovery messages 
 * ARGUMENT  : uc_index      : Indexed
 * NOTE      :
 * RETURN    : None
 ******************************************************************************/
void CANCommWatchSndMsg(HANDLE h_app, uint8_t uc_index) {
  DID ul_did;                            /* Data ID */
  uint8_t uc_comm_stop;                   /* Communication status */
  CAN_MSG_COMM_WATCHSTS st_comm_watch_sts; /* Communication Disruption Detection/Recovery Message Structures to the user */
                                        //  int32_t          ret_api;  /* Return Values of System API Functions */
  char notify_name[MAX_NAME_SIZE_APP];
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  HANDLE h_client;

  /* Message structure initialization */
  memset(reinterpret_cast<void *>(&st_comm_watch_sts), (int32_t)0, (size_t)sizeof(st_comm_watch_sts));
  /* Buffer initialization */

  /* Acquisition of the communication status of the target index */
  CANCommWatchDataGet(uc_index, notify_name, &ul_did, &uc_comm_stop, CAN_PROTOCOL_TYPE_CAN);

  /* Message creation */
  /* Creation of header part (CAN -> Monitoring Requester)*/
//  if ((uint8_t)CAN_COMM_NORMAL == uc_comm_stop) { /* Command ID */
    /* During communication -> Send data communication recovery notification message */
//    st_comm_watch_sts.hdr.hdr.cid = (uint16_t)CID_CAN_COMM_RESTORE;
//  } else {
    /* When communication is interrupted -> Send data communication interruption notification message */
//    st_comm_watch_sts.hdr.hdr.cid = (uint16_t)CID_CAN_COMM_STOP;
//  }
//  st_comm_watch_sts.hdr.hdr.msgbodysize = sizeof(CAN_MSG_COMM_WATCHSTS_DAT); /* Message data size */
//  st_comm_watch_sts.hdr.hdr.rid = 0;                                         /* Resource ID (not used) */
  st_comm_watch_sts.data.ulDid = ul_did;                                     /* Creation of data section */

  if (g_map_handler.count(notify_name) == 0) {
    h_client = FrameworkunifiedMcOpenSender(h_app, notify_name);
    if (h_client == NULL) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      goto exit;  // LCOV_EXCL_LINE 4: NSFW error case
    }
    g_map_handler[notify_name].handle = h_client;  // LCOV_EXCL_BR_LINE 200 unexpect branch
    g_map_handler[notify_name].is_q_full = FALSE;  // LCOV_EXCL_BR_LINE 200 unexpect branch
  } else {
    h_client = g_map_handler[notify_name].handle;  // LCOV_EXCL_BR_LINE 200 unexpect branch
  }
//  e_status = FrameworkunifiedSendMsg(h_client, st_comm_watch_sts.hdr.hdr.cid, sizeof(st_comm_watch_sts), &st_comm_watch_sts);
  if (e_status != eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    CANCommWatchDelete(uc_index, CAN_PROTOCOL_TYPE_CAN);  // LCOV_EXCL_LINE 4: NSFW error case
    goto exit;  // LCOV_EXCL_LINE 4: NSFW error case
  }
exit:
  return;
}

/*******************************************************************************
 * MODULE    : CANTxRsltSndMsg
 * ABSTRACT  : CANDataTransmission result notification message transmission processing
 * FUNCTION  : CANDataSend a transmission result notification message 
 * ARGUMENT  : uc_can_rid      : Resource ID from CAN
 *             uc_status      : Transmission result status
 * NOTE      :
 * RETURN    : TRUE        : Message transmission success
 *         FALSE      : Message transmission failure
 ******************************************************************************/
BOOL CANTxRsltSndMsg(HANDLE h_app, uint8_t uc_can_rid, uint8_t uc_status) {
  BOOL ret = TRUE;             /* Return value of this function */
  CAN_MSG_SENDSTS st_txrslt_msg;  /* Transmission result notification message structure */
  CAN_1TIME_TXRSLT_DAT st_data;  /* Transmission result data */
  int32_t ret_api = RET_NORMAL; /* Return Values of System API Functions */
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  HANDLE h_client;

  if (static_cast<const uint8_t>(CAN_TXRSLT_CANRID__CWORD29__START) <= uc_can_rid) {
    return (FALSE);
  }

  memset(reinterpret_cast<void *>(&st_txrslt_msg), 0, sizeof(CAN_MSG_SENDSTS));
  /* Message structure initialization */
  /****** Data acquisition ******/
  CANTxRsltDataGet(uc_can_rid, &st_data);

  /****** Creation of header section ******/
//  st_txrslt_msg.hdr.hdr.cid = static_cast<uint16_t>(CID_CAN_TX_RESULT);                   /* Command ID */
//  st_txrslt_msg.hdr.hdr.msgbodysize = static_cast<uint16_t>(sizeof(CAN_MSG_SENDSTS_DAT)); /* Message data size */

  /****** Creation of data section ******/
//  st_txrslt_msg.hdr.hdr.rid = st_data.uc_rid;    /* Resources ID */
  st_txrslt_msg.data.ulCanid = st_data.ul_canid; /* CAN ID */
  st_txrslt_msg.data.ucStatus = uc_status;      /* Transmission result status */

  FRAMEWORKUNIFIEDLOG(ZONE_CAN_DEBUG, __func__, "Rid:%02x Canid:%08x Status:%02x", st_data.uc_rid, st_data.ul_canid, uc_status);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT (whitespace/line_length)
  /****** Messaging ******/
  if (g_map_handler.count(st_data.notify_name) == 0) {
    h_client = FrameworkunifiedMcOpenSender(h_app, reinterpret_cast<const char *>(st_data.notify_name));
    if (h_client == reinterpret_cast<HANDLE>(NULL)) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "FrameworkunifiedMcOpenSender Error(name=%s)", st_data.notify_name);
      ret_api = RET_ERROR;
    }
    g_map_handler[st_data.notify_name].handle = h_client;  // LCOV_EXCL_BR_LINE 200: unexpect branch
    g_map_handler[st_data.notify_name].is_q_full = FALSE;  // LCOV_EXCL_BR_LINE 200: unexpect branch
  } else {
    h_client = g_map_handler[st_data.notify_name].handle;  // LCOV_EXCL_BR_LINE 200: unexpect branch
  }

//  e_status = FrameworkunifiedSendMsg(h_client, st_txrslt_msg.hdr.hdr.cid,
//                        static_cast<UI_32>(sizeof(CAN_MSG_SENDSTS)), reinterpret_cast<PCVOID>(&st_txrslt_msg));
  if (e_status != eFrameworkunifiedStatusOK) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "FrameworkunifiedSendMsg Error");
    ret_api = RET_ERROR;
  }

  if (RET_NORMAL != ret_api) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "FrameworkunifiedSendMsg Error(ret_api=%d)", ret_api);
    /* Check for message transmission and reception errors */
    CANMsgErrChk(ret_api, 0);
    ret = FALSE; /* Message transmission failure */
  }

  return (ret);
}

/*******************************************************************************
 * MODULE    : CAN_CWORD29_TxRsltSndMsg
 * ABSTRACT  : _CWORD29_ data transmission result notification message transmission process
 * FUNCTION  : Send _CWORD29_ transmission result notification messages 
 * ARGUMENT  : uc_can_rid     : Resource ID from CAN
 *             uc_status      : Transmission result status
 * NOTE      :
 * RETURN    : TRUE           : Message transmission success
 *             FALSE          : Message transmission failure
 ******************************************************************************/
BOOL CAN_CWORD29_TxRsltSndMsg(const HANDLE h_app, const uint8_t uc_can_rid, const uint8_t uc_status) {
  BOOL ret = TRUE;                       /* Return value of this function */
  CAN_MSG__CWORD29__SENDSTS st_txrslt_msg;  /* _CWORD29_ send result notification messaging structures */
  CAN__CWORD29__TXRSLT_DAT st_data;         /* _CWORD29_ transmission result data */
  int32_t ret_api = RET_NORMAL;          /* Return Values of System API Functions */
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  HANDLE h_client;

  if (reinterpret_cast<HANDLE>(NULL) == h_app) {
    return (FALSE);
  }

  memset(reinterpret_cast<void *>(&st_txrslt_msg), 0, sizeof(CAN_MSG__CWORD29__SENDSTS));
  /* Message structure initialization */
  /****** Data acquisition ******/
  CAN_CWORD29_TxRsltDataGet(uc_can_rid, &st_data);

  /****** Creation of header section ******/
//  st_txrslt_msg.hdr.hdr.cid = CID_CAN__CWORD29__TX_RESULT;  /* Command ID */
  /* Message data size */
//  st_txrslt_msg.hdr.hdr.msgbodysize = static_cast<uint16_t>(sizeof(CAN_MSG__CWORD29__SENDSTS_DAT));

  /****** Creation of data section ******/
//  st_txrslt_msg.hdr.hdr.rid = st_data.uc_rid;    /* Resources ID */
  st_txrslt_msg.data.usOpc = st_data.opc;        /* OPC */
  st_txrslt_msg.data.ucStatus = uc_status;       /* Transmission result status */

  FRAMEWORKUNIFIEDLOG(ZONE_CAN_DEBUG, __func__, "Rid:%02x OPC:%08x Status:%02x", st_data.uc_rid, st_data.opc, uc_status);
  /****** Messaging ******/
  if (g_map_handler.count(reinterpret_cast<const char *>(st_data.notify_name)) == 0) {
    h_client = FrameworkunifiedMcOpenSender(h_app, st_data.notify_name);
    if (h_client == reinterpret_cast<HANDLE>(NULL)) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "FrameworkunifiedMcOpenSender Error(name=%s)", st_data.notify_name);
      ret_api = RET_ERROR;
    }
    g_map_handler[st_data.notify_name].handle = h_client;
    g_map_handler[st_data.notify_name].is_q_full = FALSE;
  } else {
    h_client = g_map_handler[st_data.notify_name].handle;
  }

//  e_status = FrameworkunifiedSendMsg(h_client, static_cast<UI_32>(st_txrslt_msg.hdr.hdr.cid), sizeof(CAN_MSG__CWORD29__SENDSTS),
//                        &st_txrslt_msg);
  if (e_status != eFrameworkunifiedStatusOK) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "FrameworkunifiedSendMsg Error");
    ret_api = RET_ERROR;
  }

  /****** NSFW error case ******/
  if (RET_NORMAL != ret_api) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "FrameworkunifiedSendMsg Error(ret_api=%d)", ret_api);
    /* Check for message transmission and reception errors */
    CANMsgErrChk(ret_api, 0);
    /* Message transmission failure */
    ret = FALSE;  // LCOV_EXCL_LINE 8:Due to dead code
  }

  return (ret);
}

/*******************************************************************************
 * MODULE    : CANCanidUserToIfCvt
 * ABSTRACT  : CAN ID Conversion Process 2
 * FUNCTION  : Convert CAN ID for USER to CAN ID for interfaces 
 * ARGUMENT  : can_id      : CAN ID
 *         *puc_buf      : CAN ID Array Pointers (Data-Out)
 * NOTE      :
 * RETURN    : None
 ******************************************************************************/
void CANCanidUserToIfCvt(CANID can_id, uint8_t *puc_buf) {
  int32_t i;                   /* Generic counters */
  uint8_t *puc;                /* Generic pointer */
  uint32_t ul1; /* General Variables */
  uint32_t ul2; /* General Variables */
  uint32_t ul3; /* General Variables */

  /*  Concept of conversion
  Input data format (32-bit data)
    Bit assignment    [31:29]    [28:0]
    CAN ID free [2:0] CAN ID [28:0]
  Output data format (8-bit data with the following structure)
    Base Address  +0  :ext_flag,    CAN ID[10:4]
            +1  :CAN ID[3:0], Empty[1:0], CAN ID[28:27]
            +2  :CAN ID[26:19]
            +3  :CAN ID[18:11]
*/
  ul1 = (uint32_t)can_id;        /* Type conversion */
  ul2 = (ul1 << 20) & 0x7FF00000; /* Insert data of [10:0] */
  ul3 = (ul1 >> 11) & 0x0003FFFF; /* Populate [28:11] */
  ul2 |= ul3;                     /* Combine */
  /* Check of ext_flag */
  if (0 != ul3) {
    ul2 |= CAN_ID_EXT_FLAG; /* Add Extended Flag */
  } else {
    /* Do nothing */
  }
  puc = puc_buf; /* Get array pointers */
  for (i = 0; i < 4; i++) {
    ul3 = ul2 >> (uint32_t)((3 - i) * 8);
    *puc = (uint8_t)ul3; /* Data storage */
    puc++;
  }
  return;
}

/*******************************************************************************
 * MODULE    : CANCommandDeliverySndMsg
 * ABSTRACT  : CAN command delivery message transmission processing
 * FUNCTION  : CAN command delivery (CAN thread-> user)
 * ARGUMENT  :  uc_cmd_id    : CAN command ID
 *        puc_data    : Receive data reference pointer
 * NOTE      :
 * RETURN    :  TRUE    : Message transmission success
 *        FALSE    : Message transmission failure
 ******************************************************************************/
BOOL CANCommandDeliverySndMsg(HANDLE h_app, uint8_t uc_cmd_id, const uint8_t *puc_data) {
  BOOL ret = TRUE;               /* Return value of this function */
  CAN_MSG_CANCMD st_snd_msg;        /* CAN Command Delivery Notification Message Structure */
  int32_t l_cnt = 0;               /* Counter */
  uint32_t ul = 0; /* General Variables */
                                  //  int32_t        ret_api;          /* Return Values of System API Functions */
  CAN_COMMAND_DELIVERY_LIST_DAT list;
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  HANDLE h_client;

  /* Get Shipping ID List */
  (void)CANCommandDeliveryDataGet(uc_cmd_id, &list);

  /* Messaging */
  for (l_cnt = 0; l_cnt < (int32_t)list.notify_name_num; l_cnt++) {
    /* Data size setting */
    switch (uc_cmd_id) { // LCOV_EXCL_BR_LINE 6:Excluded because the value is not currently passed
      case CAN_CMDID_FUELCALC_RST_REQ_RX:
        ul = CAN_TX_CMD_DELIVERY_SIZE + CAN_TX_CMD_FUELCALC_RST_SIZE;
        break;
      case CAN_CMDID_STARTUP_FIN_RESP_RX:
        ul = CAN_TX_CMD_DELIVERY_SIZE + CAN_TX_CMD_STARTUP_FIN_SIZE;
        break;
      case CAN_CMDID_MRST_INFO_RESP_RX:
        ul = CAN_TX_CMD_DELIVERY_SIZE + CAN_TX_CMD_MRST_INFO_SIZE;
        break;
      case CAN_CMDID_VERSION_RESP_RX:
        ul = CAN_TX_CMD_DELIVERY_SIZE + CAN_TX_CMD_VERSION_SIZE;
        break;
      case CAN_CMDID_CONNECTION_NODE_RESP_RX:
        ul = CAN_TX_CMD_DELIVERY_SIZE + (uint32_t)puc_data[0] + 1;
        break;
      default:
        // LCOV_EXCL_START 6 Excluded because the value is not currently passed
        AGL_ASSERT_NOT_TESTED();
        ul = (uint32_t)sizeof(st_snd_msg.data);
        break;
        // LCOV_EXCL_STOP6 Excluded because the value is not currently passed
    }

    (void)memset(reinterpret_cast<void *>(&st_snd_msg), (int32_t)0, (size_t)sizeof(st_snd_msg));
    /* Message structure initialization */
    /****** Creation of header section ******/
//    st_snd_msg.hdr.hdr.rid = 0;                                                 /* Resource ID (not used) */
//    st_snd_msg.hdr.hdr.cid = (uint16_t)CID_CAN_CMD_DELIVERY;                    /* Command ID */
//    st_snd_msg.hdr.hdr.msgbodysize = (uint16_t)ul; /* Message data size */

    /****** Creation of data section ******/
    st_snd_msg.data.cmd_id = uc_cmd_id;                                                            /* CAN command ID */
    memcpy(&st_snd_msg.data.data[0], puc_data, (ul - CAN_TX_CMD_DELIVERY_SIZE)); /* Received data */

    /****** Messaging ******/
    if (g_map_handler.count(list.notify_name_list[l_cnt]) == 0) {
      h_client = FrameworkunifiedMcOpenSender(h_app, list.notify_name_list[l_cnt]);
      if (h_client == NULL) {
        ret = FALSE;
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "FrameworkunifiedMcOpenSender Error(notify_name=%s)", list.notify_name_list[l_cnt]);
        break;
      }
      g_map_handler[list.notify_name_list[l_cnt]].handle = h_client;
      g_map_handler[list.notify_name_list[l_cnt]].is_q_full = FALSE;
    } else {
      h_client = g_map_handler[list.notify_name_list[l_cnt]].handle;
    }
    e_status = FrameworkunifiedSendMsg(h_client, CID_CAN_CMD_DELIVERY, sizeof(st_snd_msg), &st_snd_msg);
    if (e_status != eFrameworkunifiedStatusOK) {
      CANCommandDeliveryDeletesingle(uc_cmd_id, list.notify_name_list[l_cnt]);
      ret = FALSE;
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "FrameworkunifiedSendMsg Error(e_status=%d)", e_status);
      break;
    }
  }

  return (ret);
}

/*******************************************************************************
 * MODULE    : CANCommandTxRsltSndMsg
 * ABSTRACT  : CAN command transmission result notification message transmission processing
 * FUNCTION  : Send CAN command send result notification (CAN thread-> user)
 * ARGUMENT  :  uc_can_rid  : Resource ID from CAN
 *        uc_status  : Transmission result status
 * NOTE      :
 * RETURN    :
 * RETURN    :  TRUE    : Message transmission success
 *        FALSE    : Message transmission failure
 ******************************************************************************/
BOOL CANCommandTxRsltSndMsg(HANDLE h_app, uint8_t uc_can_rid, uint8_t uc_status) {
  BOOL ret = TRUE;             /* Return value of this function */
  CAN_CMD_MSG_SENDSTS st_snd_msg; /* CAN command transmission result notification message structure */
  uint8_t uc_rid;                /* Resources ID */
  uint8_t uc_cmd_id;              /* CAN command ID */
                                //  int32_t        ret_api;  /* Return Values of System API Functions */
  char notify_name[MAX_NAME_SIZE_APP];
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  HANDLE h_client;

  /* Data acquisition */
  CANCommandTxRsltDataGet(uc_can_rid, notify_name, &uc_rid, &uc_cmd_id);

  memset(reinterpret_cast<void *>(&st_snd_msg), (int32_t)0, (size_t)sizeof(st_snd_msg));
  /* Message structure initialization */
  /****** Creation of header section ******/
//  st_snd_msg.hdr.hdr.rid = uc_rid;                                 /* Resources ID */
//  st_snd_msg.hdr.hdr.cid = (uint16_t)CID_CAN_CMD_TX_RESULT;       /* Command ID */
//  st_snd_msg.hdr.hdr.msgbodysize = (size_t)sizeof(st_snd_msg.data); /* Message data size */

  /****** Creation of data section ******/
  st_snd_msg.data.cmd_id = (uint32_t)uc_cmd_id; /* CAN command ID */
  st_snd_msg.data.status = uc_status;                              /* Transmission result status */

  /****** Messaging ******/
  if (g_map_handler.count(notify_name) == 0) {
    h_client = FrameworkunifiedMcOpenSender(h_app, notify_name);
    if (h_client == NULL) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      ret = FALSE;  // LCOV_EXCL_LINE 4: NSFW error case
      return (ret);  // LCOV_EXCL_LINE 4: NSFW error case
    }
    g_map_handler[notify_name].handle = h_client;  // LCOV_EXCL_BR_LINE 200: unexpect branch
    g_map_handler[notify_name].is_q_full = FALSE;  // LCOV_EXCL_BR_LINE 200: unexpect branch
  } else {
    h_client = g_map_handler[notify_name].handle;  // LCOV_EXCL_BR_LINE 200: unexpect branch
  }
  e_status = FrameworkunifiedSendMsg(h_client, CID_CAN_CMD_TX_RESULT, sizeof(st_snd_msg), &st_snd_msg);
  if (e_status != eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    CANCommandTxRsltDelete(uc_can_rid);  // LCOV_EXCL_LINE 4: NSFW error case
    ret = FALSE;  // LCOV_EXCL_LINE 4: NSFW error case
    return (ret);  // LCOV_EXCL_LINE 4: NSFW error case
  }
  return (ret);
}

/*******************************************************************************
 * MODULE    : CANCommandTransmissionSndMsg
 ******************************************************************************/
/*!-----------------------------------------------------------------------------
 * @~english
 * @brief    Transmission of CAN command
 *
 * @~english
 * Transmission of CAN command (DISP Microcomputer -> SYS Microcomputer)
 *
 * @~english
 * @note Details of process
 *   -# Creating of data division
 *   -# Message sending
 *
 * @~english
 * @return  Message transmission success/failure
 * @retval  <1>  Message transmission success
 * @retval  <0>  Message transmission failure
 *
 * @~english
 * @param[in]     <uc_opc>    Operation code
 * @param[in]     <uc_can_rid> Resource ID
 */
BOOL CANCommandTransmissionSndMsg(uint8_t uc_opc, uint8_t uc_can_rid) {  // LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  BOOL ret = TRUE;      /* Return value of this function */
//  T_ICR_CMD st_tx_can_data; /* Transmit data structure */

//  (void)memset(reinterpret_cast<void *>(&st_tx_can_data), (int32_t)0, (size_t)sizeof(st_tx_can_data));
  /* Message structure initialization */
  /****** Creation of data section ******/
//  st_tx_can_data.cmd_data.phyadr_from = CAN_TX_PHYS_ADRS;     /* Physical address */
//  st_tx_can_data.cmd_data.service_type = CAN_TX_COMM_KIND;    /* Communication type 0x00: Individual */
//  st_tx_can_data.cmd_data.lgadr_from = CAN_TX_FROM_ADRS;      /* Source Logical Address */
//  st_tx_can_data.cmd_data.lgadr_to = CAN_TX_TO_ADRS;          /* Destination logical address */
//  st_tx_can_data.cmd_data.opc = uc_opc;                        /* Operation code */
//  st_tx_can_data.cmd_data.d_length = (uint16_t)CAN_CMDSND_DATA_SIZE; /* Data length (data section only) */

  /****** Messaging ******/
  return (ret);
}
// LCOV_EXCL_STOP

/*******************************************************************************
 * MODULE    : CANCommandFuelCalcRstReqSndMsg
 * ABSTRACT  : CAN section flame reset request reception message transmission processing
 * FUNCTION  : Send CAN interval flame reset request reception message (CAN thread-> user)
 * ARGUMENT  :  notifyId    : Message Destination ID
 * NOTE      :
 * RETURN    :  TRUE    : Message transmission success
 *        FALSE    : Message transmission failure
 ******************************************************************************/
BOOL CANCommandFuelCalcRstReqSndMsg(HANDLE h_app, PCSTR notify_name) {
  BOOL ret = TRUE;        /* Return value of this function */
  CAN_MSG_CANCMD st_snd_msg; /* CAN Command Delivery Notification Message Structure */
                           //  int32_t      ret_api = RET_NORMAL;  /* Return Values of System API Functions */
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  HANDLE h_client;

  (void)memset(reinterpret_cast<void *>(&st_snd_msg), (int32_t)0, (size_t)sizeof(st_snd_msg));
  /* Message structure initialization */
  /****** Creation of header section ******/
//  st_snd_msg.hdr.hdr.rid = 0;                                                               /* Resource ID (not used) */
//  st_snd_msg.hdr.hdr.cid = (uint16_t)CID_CAN_CMD_DELIVERY;                                  /* Command ID */
//  st_snd_msg.hdr.hdr.msgbodysize = CAN_TX_CMD_DELIVERY_SIZE + CAN_TX_CMD_FUELCALC_RST_SIZE; /* Message data size */

  /****** Creation of data section ******/
  st_snd_msg.data.cmd_id = CAN_CMDID_FUELCALC_RST_REQ_RX; /* CAN command ID */

  if (g_map_handler.count(notify_name) == 0) {
    h_client = FrameworkunifiedMcOpenSender(h_app, notify_name);
    if (h_client == NULL) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      ret = FALSE;  // LCOV_EXCL_LINE 4: NSFW error case
      return (ret);  // LCOV_EXCL_LINE 4: NSFW error case
    }
    g_map_handler[notify_name].handle = h_client;  // LCOV_EXCL_BR_LINE 11:except,C++ STL
    g_map_handler[notify_name].is_q_full = FALSE;  // LCOV_EXCL_BR_LINE 11:except,C++ STL
  } else {
    h_client = g_map_handler[notify_name].handle;  // LCOV_EXCL_BR_LINE 11:except,C++ STL
  }
  e_status = FrameworkunifiedSendMsg(h_client, CID_CAN_CMD_DELIVERY, sizeof(st_snd_msg), &st_snd_msg);
  if (e_status != eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    ret = FALSE;  // LCOV_EXCL_LINE 4: NSFW error case
    return (ret);  // LCOV_EXCL_LINE 4: NSFW error case
  }

  return (ret);
}

/*******************************************************************************
 * MODULE    : CANCommandidOpcToUserCvt
 * ABSTRACT  : CAN Command ID Conversion Processing 1
 * FUNCTION  : Convert the opcode to the corresponding CAN command ID 
 * ARGUMENT  :  uc_opc    : Opcode
 *        puc_cmd_id  : CAN command ID return pointer
 * NOTE      :
 * RETURN    :  TRUE    : Predefined opcodes
 *        FALSE    : Undefined opcode
 ******************************************************************************/
BOOL CANCommandidOpcToUserCvt(uint8_t uc_opc, uint8_t *puc_cmd_id) {
  BOOL ret = TRUE; /* Return value of this function */

  switch (uc_opc) {
    case CAN_OPC_COMMAND_STARTUP_FIN_RESP_RX: /* CAN startup completion notice received */
      *puc_cmd_id = CAN_CMDID_STARTUP_FIN_RESP_RX;
      break;
    case CAN_OPC_COMMAND_MRST_INFO_RESP_RX: /* CAN Master Reset Information Notification Reception */
      *puc_cmd_id = CAN_CMDID_MRST_INFO_RESP_RX;
      break;
    case CAN_OPC_COMMAND_VERSION_RESP_RX: /* CAN Version Response Reception */
      *puc_cmd_id = CAN_CMDID_VERSION_RESP_RX;
      break;
    case CAN_OPC_COMMAND_CONNECTION_NODE_RESP_RX: /* Receive CAN Connection Node Notification Response */
      *puc_cmd_id = CAN_CMDID_CONNECTION_NODE_RESP_RX;
      break;
    case CAN_OPC_COMMAND_FUELCALC_RST_REQ_RX: /* RECEIVE REQUEST FREE OF CAN SECTION */
      *puc_cmd_id = CAN_CMDID_FUELCALC_RST_REQ_RX;
      break;
    default:
      ret = FALSE; /* Undefined opcode */
      break;
  }

  return (ret);
}

/*******************************************************************************
 * MODULE    : CANCommandidUserToOpcCvt
 * ABSTRACT  : CAN command ID conversion process 2
 * FUNCTION  : Convert the CAN command ID to the corresponding opcode 
 * ARGUMENT  :  uc_cmd_id    : CAN command ID
 *        puc_opc    : Opcode return pointer
 * NOTE      :
 * RETURN    :  TRUE    : Predefined ID
 *        FALSE    : Undefined ID
 ******************************************************************************/
BOOL CANCommandidUserToOpcCvt(uint8_t uc_cmd_id, uint8_t *puc_opc) {
  BOOL ret = TRUE; /* Return value of this function */

  switch (uc_cmd_id) {
    case CAN_CMDID_STARTUP_FIN_REQ_TX: /* CAN start completion notification request transmission */
      *puc_opc = CAN_OPC_COMMAND_STARTUP_FIN_REQ_TX;
      break;
    case CAN_CMDID_MRST_INFO_REQ_TX: /* CAN master reset information notification request transmission */
      *puc_opc = CAN_OPC_COMMAND_MRST_INFO_REQ_TX;
      break;
    case CAN_CMDID_VERSION_REQ_TX: /* CAN Version Request Send */
      *puc_opc = CAN_OPC_COMMAND_VERSION_REQ_TX;
      break;
    case CAN_CMDID_CONNECTION_NODE_REQ_TX: /* CAN connection node notification request transmission */
      *puc_opc = CAN_OPC_COMMAND_CONNECTION_NODE_REQ_TX;
      break;
    case CAN_CMDID_FUELCALC_REQ_TX: /* CAN section flame reset response transmission */
      *puc_opc = CAN_OPC_COMMAND_FUELCALC_REQ_TX;
      break;
    default:
      ret = FALSE; /* Undefined ID */
      break;
  }

  return (ret);
}

/*!-----------------------------------------------------------------------------
 * @~english
 * @brief    Transmission CAN Command
 *
 * @~english
 * @return  Return value
 * @retval  <TRUE>        Transmission success
 * @retval  <FALSE>       Transmission failed
 *
 * @~english
 * @param[in]     <id>        CAN ID
 * @param[in]     <dlc>       DLC
 * @param[in]     <pst_data>   CAN Data structure
 */
BOOL CANTransStartTxMsgCore(HANDLE h_app, CANID id, uint8_t dlc, CAN_DATA_BIT *pst_data) {
  BOOL b_ret = FALSE;
//  CanMessage st_can_data;
//
//  if (pst_data != NULL) {  // LCOV_EXCL_BR_LINE 200: puc_data is a pointer of global variable
//    st_can_data.can_id = id;
//    st_can_data.rid = CAN_RID_NOTUSE_CODE;
//    st_can_data.dlc = dlc;
//    memcpy(st_can_data.data, pst_data->dat, dlc);

//    if (CanSend(&st_can_data, CAN_HAL_TYPE_CAN) != CANHAL_RET_NORMAL) {
//      b_ret = FALSE;
//    } else {
//      b_ret = TRUE;
//    }
//  }

  return b_ret;
}  // LCOV_EXCL_BR_LINE 10: end of line unknown branch
