/*
 * @copyright Copyright (c) 2016-2020 TOYOTA MOTOR CORPORATION.
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
#include "CAN_TxMsg.h"
#include "CAN_Delivery.h"
#include <string.h>
#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_framework_if.h>
#include <peripheral_service/Canif_API.h>
#include <can_hal.h>
#include <map>
#include <string>
#include "CAN_Thread.h"
#include "Thread_Common.h"
#include "Canif_API_Local.h"
#include "CAN_TxMsg.h"
#include "Canif_TransmissionData.h"

static std::map<CANID, CAN_SEND_STATUS_DAT> g_m_send_sts;
static bool can_cmd_rst_rcv = false;
static std::map<CANID, CAN_TRANS_START_TABLE_VAL *> g_map_trans_data;

void CANTxMsgInit(void) {
  g_m_send_sts.clear();
  uint32_t i;

  for (i = 0; i < _countof(Can_TransInitData_21PF); i++) {
    CANID canid = Can_TransInitData_21PF[i].canid;
    CAN_TRANS_START_TABLE_VAL *val = &(Can_TransInitData_21PF[i].val);
    g_map_trans_data[canid] = val;
  }

  return;
}

static bool CANCommandFuelCalcRstReqCheck(void) {
  return can_cmd_rst_rcv;
}

void CANCommandSetFuelCalcRstReq(void) {
  can_cmd_rst_rcv = true;
}

static void CANTxMsgErrorLog(const char *reason, const char *func,
                                         CANID id, uint16_t len, HANDLE h_app) {
  FRAMEWORKUNIFIEDLOG(ZONE_ERR, func, "%s Frame(0x%x) length=%hu from %s is ignored.", 
    reason, id, len, FrameworkunifiedGetMsgSrc(h_app));
}

static EFrameworkunifiedStatus CANCommandResponse(HANDLE h_app, CAN_MSG_CANCMD *msg, 
                                                 size_t len, char *notifyName) {
  HANDLE h_client = NULL;
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusFail;

  h_client = CommonFindSender(h_app, notifyName);
  if (!h_client) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "Cannot open sender handle.");
    return e_status;
  }

  msg->hdr.hdr.cid = (uint16_t)CID_CAN_CMD_DELIVERY;
  e_status = FrameworkunifiedSendMsg(h_client, CID_CAN_CMD_DELIVERY, (uint32_t)len, msg); 
  if (e_status != eFrameworkunifiedStatusOK)    
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "FrameworkunifiedSendMsg is failed.");

  return e_status;
}

static EFrameworkunifiedStatus CANCommandVersionReq(HANDLE h_app, char *notifyName) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusFail;
  CANHAL_RET_API ret = CANHAL_RET_NORMAL;
  std::string version_info;
  CAN_MSG_CANCMD msg;

  memset(&msg, 0, sizeof(msg));

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __func__, "@@@ Call CanGetVersion for can_hal");
  ret = CanGetVersion(h_app, &version_info);
  if (ret != CANHAL_RET_NORMAL) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "Cannot get version from can_hal."); 
    return e_status;
  }

  if (CAN_TX_CMD_VERSION_SIZE != version_info.length()) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "Invalid length of version."); 
    return e_status;
  }

  msg.hdr.hdr.rid = 0;
  msg.hdr.hdr.msgbodysize = CAN_TX_CMD_DELIVERY_SIZE + CAN_TX_CMD_VERSION_SIZE;
  msg.data.cmd_id = CAN_CMDID_VERSION_RESP_RX;
  strncpy((char *)msg.data.data, version_info.c_str(), CAN_TX_CMD_VERSION_SIZE);
  return CANCommandResponse(h_app, &msg, sizeof(msg), notifyName);
}

static EFrameworkunifiedStatus CANCommandFuelcalcRstReq(HANDLE h_app, char *notifyName) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusFail;
  if (CANCommandFuelCalcRstReqCheck()) {
    // CAN_CMDID_FUELCALC_RST_REQ_RX is already received from can_hal.
    // so response it in direct.
    CAN_MSG_CANCMD msg;
    memset(&msg, 0, sizeof(msg));
    msg.hdr.hdr.rid = 0;
    msg.hdr.hdr.msgbodysize = CAN_TX_CMD_DELIVERY_SIZE +
                                                  CAN_TX_CMD_FUELCALC_RST_SIZE;
    msg.data.cmd_id = CAN_CMDID_FUELCALC_RST_REQ_RX;
    e_status = CANCommandResponse(h_app, &msg, sizeof(msg), notifyName);
  } else {
    CANDeliveryInsert(CAN_CMDID_FUELCALC_RST_REQ_RX, notifyName); 
    e_status = eFrameworkunifiedStatusOK;
  }
  return e_status;
}

static EFrameworkunifiedStatus CANCommandOther(HANDLE h_app, char *notifyName, 
                                                    uint8_t cmd, uint8_t rid) {
  CanMessage msg;
  CANID wait = 0;
  CANID can_id = (CANID)cmd;

  memset(&msg, 0, sizeof(msg));
  switch (cmd) {
  case CAN_CMDID_STARTUP_FIN_REQ_TX:
    wait = CAN_CMDID_STARTUP_FIN_RESP_RX;
    break;
  case CAN_CMDID_MRST_INFO_REQ_TX:
    wait = CAN_CMDID_MRST_INFO_RESP_RX;
    break;
  case CAN_CMDID_CONNECTION_NODE_REQ_TX:
    wait = CAN_CMDID_CONNECTION_NODE_RESP_RX;
    break;
  case CAN_CMDID_FUELCALC_REQ_TX:
    wait = CAN_CMDID_FUELCALC_RST_REQ_RX;
    break;
  default:
    CANTxMsgErrorLog("Unsupported cmd requested", __func__, cmd, 0, h_app);
    return eFrameworkunifiedStatusFail;
  }

  if (cmd != CAN_CMDID_FUELCALC_REQ_TX) {
    CANDeliveryInsert(wait, notifyName);
  }

  if (rid != CAN_RID_NOTUSE_CODE) {
    // cmd can be assumed same as can_id.
    if (0 < g_m_send_sts.count(can_id)) {
      if (g_m_send_sts[can_id].notify_name[0] != 0x00) {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __func__, 
          "Not yet deliver SendSts but updated(SendID=%x, CAN ID=%x, Nane=%s)",
          g_m_send_sts[can_id].rid, can_id, g_m_send_sts[can_id].notify_name);
      }
    }
    g_m_send_sts[can_id].rid = rid;
    memcpy(g_m_send_sts[can_id].notify_name, notifyName,
         sizeof(g_m_send_sts[can_id].notify_name));
  }

  msg.can_id = (CANID)cmd;
  msg.rid = rid;
  msg.dlc = (uint8_t)CAN_CMDSND_DATA_SIZE;

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __func__, "@@@ Call CanSend for can_hal");
  if (CANHAL_RET_NORMAL != CanSend(h_app, &msg, CAN_HAL_TYPE_CAN)) {
    CANTxMsgErrorLog("CAN CanSend failed", __func__, msg.can_id, 
                    (uint16_t)msg.dlc, h_app);
    return eFrameworkunifiedStatusFail;
  }

  return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus CANTxMsgCommand(HANDLE h_app) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __func__, "@@@ Start communication CanCommandCtl");

  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusFail;
  CAN_CMD_CTRL_MSG_DAT rcv_msg;

  memset(&rcv_msg, 0, sizeof(rcv_msg));

  e_status = FrameworkunifiedGetMsgDataOfSize(h_app, &rcv_msg, sizeof(rcv_msg), eSMRRelease);
  if (e_status != eFrameworkunifiedStatusOK) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "FrameworkunifiedGetMsgDataOfSize Error(%d)", e_status);
    if (e_status == eFrameworkunifiedStatusInvldBufSize)
      FrameworkunifiedClearMsgData(h_app);
    return e_status; 
  }

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __func__, "notify_name=%s ulEvtId=%x ucRid=%x ucCmdid=%x",
         rcv_msg.notifyName, rcv_msg.ulEvtId, rcv_msg.ucRid, rcv_msg.ucCmdid);

  switch (rcv_msg.ucCmdid) {
  case CAN_CMDID_VERSION_REQ_TX:
    e_status = CANCommandVersionReq(h_app, rcv_msg.notifyName);
    break;
  case CAN_CMDID_FUELCALC_RST_REQ_DELIVERY:
    e_status = CANCommandFuelcalcRstReq(h_app, rcv_msg.notifyName);
    break;
  default:
    e_status = CANCommandOther(h_app,
                  rcv_msg.notifyName, rcv_msg.ucCmdid, rcv_msg.ucRid);
    break;
  }
  return e_status; 
}

static CAN_TRANS_START_TABLE_VAL *CANTxMsgFindMap(CAN_TRANS_START_MSG_DAT *msg) {
  std::map<CANID, CAN_TRANS_START_TABLE_VAL *>::iterator it;
  it = g_map_trans_data.find(msg->id);
  if (it == g_map_trans_data.end())
    return NULL;
  return it->second;
}

static CAN_TRANS_START_TABLE_VAL *CanSendFind(CAN_TRANS_START_MSG_DAT *msg) {
  return CANTxMsgFindMap(msg);
}

EFrameworkunifiedStatus CANTxMsgBit(HANDLE h_app) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __func__, "@@@ Start communication CanSend(bit)");

  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusFail;
  CAN_TRANS_START_MSG_DAT rcv_msg; 
  CanMessage snd_msg;
  CAN_TRANS_START_TABLE_VAL *store;
  int32_t i = 0;
  uint8_t *store_p;
  uint8_t *data_p;
  uint8_t *mask_p;

  memset(&rcv_msg, 0, sizeof(rcv_msg));
  memset(&snd_msg, 0, sizeof(snd_msg));

  e_status = FrameworkunifiedGetMsgDataOfSize(h_app, &rcv_msg, sizeof(rcv_msg), eSMRRelease);
  if (e_status != eFrameworkunifiedStatusOK) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "FrameworkunifiedGetMsgDataOfSize Error(%d)", e_status);
    if (e_status == eFrameworkunifiedStatusInvldBufSize)
      FrameworkunifiedClearMsgData(h_app);
    goto cleanup;
  }

  e_status = eFrameworkunifiedStatusFail;

  if (TRUE != CommGetAvailabilityCurrent(CAN_AVAILABILITY)) {
    CANTxMsgErrorLog("CAN is not ready", __func__, 0, 0, h_app);
    goto cleanup;
  }

  store = CanSendFind(&rcv_msg);
  if (!store) {
    CANTxMsgErrorLog("No initial value", __func__, 
                                        rcv_msg.id, sizeof(rcv_msg.dat), h_app);
    goto cleanup;
  }

  store_p = store->dat.dat;
  data_p = rcv_msg.dat.dat;
  mask_p = rcv_msg.mask.dat;
  for (i = 0; i < store->dlc; i++) {
    *store_p &= (uint8_t)(~(*mask_p));
    *store_p |= ((*mask_p) & (*data_p));
    ++store_p;
    ++data_p;
    ++mask_p;
  }

  snd_msg.can_id = rcv_msg.id;
  snd_msg.dlc = store->dlc;
  snd_msg.rid = CAN_RID_NOTUSE_CODE;
  memcpy(snd_msg.data, store->dat.dat, snd_msg.dlc);

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __func__, 
         "@@@ Call CanSend for can_hal : "
         "can_id=%x dlc=%d rid=%x", snd_msg.can_id, snd_msg.dlc, snd_msg.rid);
  FRAMEWORKUNIFIEDLOG(ZONE_DEBUG, __func__, "msg_data=%s",
         MessageDataOutputLog(snd_msg.data, snd_msg.dlc).c_str());

  if (CANHAL_RET_NORMAL != CanSend(h_app, &snd_msg, CAN_HAL_TYPE_CAN)) {
    CANTxMsgErrorLog("CAN CanSend failed", __func__, snd_msg.can_id, 
                  (uint16_t)snd_msg.dlc, h_app);
    goto cleanup;
  }

  // echoback
  e_status = CANDeliverySndMsg(h_app, snd_msg.can_id, 0, (uint8_t)snd_msg.dlc,
                               (const uint8_t *)snd_msg.data, 
                                CID_CAN_DATA_DELIVERY, CANIF_ECHOBACK);
cleanup:
  return e_status; 
}

EFrameworkunifiedStatus CANTxMsg(HANDLE h_app) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __func__, "@@@ Start communication CanSend");

  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusFail;
  CAN_TRANSMISSION_START_MSG_DAT rcv_msg;
  CanMessage snd_msg;
  memset(&rcv_msg, 0, sizeof(rcv_msg));
  memset(&snd_msg, 0, sizeof(snd_msg));

  e_status = FrameworkunifiedGetMsgDataOfSize(h_app, &rcv_msg, sizeof(rcv_msg), eSMRRelease);
  if (e_status != eFrameworkunifiedStatusOK) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "FrameworkunifiedGetMsgDataOfSize Error(%d)", e_status);
    if (e_status == eFrameworkunifiedStatusInvldBufSize)
      FrameworkunifiedClearMsgData(h_app);
    goto cleanup;
  }

  e_status = eFrameworkunifiedStatusFail;

  if (TRUE != CommGetAvailabilityCurrent(CAN_AVAILABILITY)) {
    CANTxMsgErrorLog("CAN is not ready", __func__, 0, 0, h_app);
    goto cleanup;
  }

  snd_msg.can_id = rcv_msg.stCandata.can_id;
  snd_msg.dlc = rcv_msg.stCandata.dlc;
  snd_msg.rid = rcv_msg.ucRid;
  memcpy(snd_msg.data, rcv_msg.stCandata.data, snd_msg.dlc); 

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __func__, "@@@ Call CanSend for can_hal : "
         "can_id=%x dlc=%d rid=%x", snd_msg.can_id, snd_msg.dlc, snd_msg.rid);
  FRAMEWORKUNIFIEDLOG(ZONE_DEBUG, __func__, "msg_data=%s",
         MessageDataOutputLog(snd_msg.data, snd_msg.dlc).c_str());

  if (CANHAL_RET_NORMAL != CanSend(h_app, &snd_msg, CAN_HAL_TYPE_CAN)) {
    CANTxMsgErrorLog("CAN CanSend failed", __func__, snd_msg.can_id, 
                  (uint16_t)snd_msg.dlc, h_app);
    goto cleanup;
  }
  
  // echoback
  e_status = CANDeliverySndMsg(h_app, snd_msg.can_id, 0, (uint8_t)snd_msg.dlc,
                               (const uint8_t *)snd_msg.data, 
                                CID_CAN_DATA_DELIVERY, CANIF_ECHOBACK);
  if (rcv_msg.ucRid == (uint8_t)CAN_RID_NOTUSE_CODE)
    goto cleanup;

  if (0 < g_m_send_sts.count(snd_msg.can_id)) {
    if (g_m_send_sts[snd_msg.can_id].notify_name[0] != 0x00) {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __func__, 
        "Not yet deliver SendSts but updated(SendID=%x, CAN ID=%x, Nane=%s)",
        g_m_send_sts[snd_msg.can_id].rid, snd_msg.can_id, 
        g_m_send_sts[snd_msg.can_id].notify_name);
    }
  }

  g_m_send_sts[snd_msg.can_id].rid = rcv_msg.ucRid;
  memcpy(g_m_send_sts[snd_msg.can_id].notify_name,
         rcv_msg.notifyName,
         sizeof(g_m_send_sts[snd_msg.can_id].notify_name));
cleanup:
  return e_status;
}

EFrameworkunifiedStatus CANSndStsProcess(HANDLE h_app, CanSendResult *rcv_msg,
                                        PS_CommunicationProtocol cid) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  CAN_MSG_SENDSTS snd_msg;
  CANID can_id = rcv_msg->can_id;
  HANDLE sender = NULL;
  memset(&snd_msg, 0, sizeof(snd_msg));

  if (0 == g_m_send_sts.count(can_id))
    goto cleanup;

  if (0x00 == (g_m_send_sts[can_id].notify_name[0]))
    goto cleanup;
  
  if (rcv_msg->rid != g_m_send_sts[can_id].rid)
    goto cleanup;

  snd_msg.hdr.hdr.cid = (uint16_t)cid;
  snd_msg.hdr.hdr.msgbodysize = (uint16_t)(sizeof(snd_msg.data));
  snd_msg.hdr.hdr.rid = rcv_msg->rid;
  snd_msg.data.ulCanid = can_id;
  snd_msg.data.ucStatus = 
      (CAN_SEND_RESULT_SUCCESS == rcv_msg->result) ? CAN_SUCCESS : CAN_RETRYOUT;

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __func__, "cid=%x msgbodysize=%d rid=%x can_id=%x ucStatus=%x",
         snd_msg.hdr.hdr.cid, snd_msg.hdr.hdr.msgbodysize, snd_msg.hdr.hdr.rid,
         snd_msg.data.ulCanid, snd_msg.data.ucStatus);

  sender = CommonFindSender(h_app, g_m_send_sts[can_id].notify_name);
  if (!sender) {
    CANTxMsgErrorLog("Could not open sender.", __func__, can_id, 
                                                        sizeof(snd_msg), h_app);
    goto cleanup;
  }

  e_status = FrameworkunifiedSendMsg(sender, cid, sizeof(snd_msg), &snd_msg);
  if (eFrameworkunifiedStatusOK != e_status) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "FrameworkunifiedSendMsg Error(e_status:%d to:%s)",
          e_status, g_m_send_sts[can_id].notify_name);
    g_m_send_sts[can_id].notify_name[0] = 0x00;
    g_m_send_sts[can_id].rid = 0x00;
    goto cleanup;
  }
  g_m_send_sts[can_id].notify_name[0] = 0x00;
  g_m_send_sts[can_id].rid = 0x00;

  e_status = eFrameworkunifiedStatusOK;
cleanup:
  return e_status;
}
