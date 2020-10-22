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

#include "CAN_Delivery.h"

#include <can_hal.h>
#include <native_service/frameworkunified_framework_if.h>
#include <peripheral_service/Canif_API.h>
#include <can_hal.h>
#include <utility>
#include <map>
#include <string>
#include "CAN_TxMsg.h"
#include "CAN_CommWatch.h"
#include "Canif_API_Local.h"
#include "API_Local_Common.h"
#include "Thread_Common.h"

static CAN_DeliveryEntryList g_map_delivery_list_can;
// Member of g_msg_rx_msg exist as long as communication exists.
static std::map<CANID, CanMessage *> g_can_rx_msg;

void CANDeliveryInit(void) {
  g_map_delivery_list_can.clear();
  return;
}

static CanMessage *CANMsgFind(CANID k) {
  std::map<CANID, CanMessage *>::iterator it;

  it = g_can_rx_msg.find(k);
  if (it == g_can_rx_msg.end())
    return NULL;
  return it->second;
}

static void CANMsgInsert(CANID k, CanMessage *msg) {
  CanMessage *latest = CANMsgFind(k);
  if (!latest) {
    latest = new CanMessage();
  }
  g_can_rx_msg[k] = latest;
  memcpy(latest, msg, sizeof(CanMessage));
}

static CAN_DeliveryEntryListIt CANDeliveryFind(CANID canid, std::string s) {
  std::pair<CAN_DeliveryEntryListIt, CAN_DeliveryEntryListIt> range;
  CAN_DeliveryEntryListPair p = std::make_pair(canid, s);
  CAN_DeliveryEntryListIt it;
  bool found = false;
  range = g_map_delivery_list_can.equal_range(canid);
  for (it = range.first; it != range.second; ++it) {
    if (*it == p) {
      found = true;
      break;
    }
  } 

  if (!found)
    it = g_map_delivery_list_can.end();

  return it;
}

bool CANDeliveryInsert(CANID canid, std::string s) {
  CAN_DeliveryEntryListIt it = CANDeliveryFind(canid, s);
  CAN_DeliveryEntryListPair p = std::make_pair(canid, s);
  bool inserted = false;

  if (it == g_map_delivery_list_can.end()) {
    g_map_delivery_list_can.insert(p);
    inserted = true;
  }

  return inserted;
}

static EFrameworkunifiedStatus CANDeliverySndCmdSingle(HANDLE h_app, CANID cmd,
                                          CanMessage *msg, std::string dest) {
  CAN_MSG_CANCMD st_delivery_msg;
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusFail;
  HANDLE h_client;
  uint16_t len = 0;

  // Create delivery data
  memset(&st_delivery_msg, 0, sizeof(st_delivery_msg));
  switch (cmd) {
  case CAN_CMDID_FUELCALC_RST_REQ_RX:
    len = CAN_TX_CMD_DELIVERY_SIZE + CAN_TX_CMD_FUELCALC_RST_SIZE;
    break;
  case CAN_CMDID_STARTUP_FIN_RESP_RX:
    len = CAN_TX_CMD_DELIVERY_SIZE + CAN_TX_CMD_STARTUP_FIN_SIZE;
    break;
  case CAN_CMDID_MRST_INFO_RESP_RX:
    len = CAN_TX_CMD_DELIVERY_SIZE + CAN_TX_CMD_MRST_INFO_SIZE;
    break;
  case CAN_CMDID_VERSION_RESP_RX:
    len = CAN_TX_CMD_DELIVERY_SIZE + CAN_TX_CMD_VERSION_SIZE;
    break;
  case CAN_CMDID_CONNECTION_NODE_RESP_RX:
    len = (uint16_t)(CAN_TX_CMD_DELIVERY_SIZE + msg->data[0] + 1);
    break;
  default:
    return e_status;
  }
  st_delivery_msg.hdr.hdr.rid = 0;
  st_delivery_msg.hdr.hdr.cid = (uint16_t)CID_CAN_CMD_DELIVERY;
  st_delivery_msg.hdr.hdr.msgbodysize = len;
  st_delivery_msg.data.cmd_id = (uint8_t)cmd;
  memcpy(&st_delivery_msg.data.data, msg->data,
                                     (size_t)(len - CAN_TX_CMD_DELIVERY_SIZE));

  h_client = CommonFindSender(h_app, dest);
  if (!h_client) {
    return e_status;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __func__, "cid=%x msgbodysize=%x cmd_id=%x",
         st_delivery_msg.hdr.hdr.cid, st_delivery_msg.hdr.hdr.msgbodysize,
         st_delivery_msg.data.cmd_id);
  FRAMEWORKUNIFIEDLOG(ZONE_DEBUG, __func__, "msg_data=%s",
         MessageDataOutputLog(st_delivery_msg.data.data, len).c_str());

  e_status = FrameworkunifiedSendMsg(h_client, CID_CAN_CMD_DELIVERY,
                                   sizeof(st_delivery_msg), &st_delivery_msg);
  if (e_status != eFrameworkunifiedStatusOK) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "FrameworkunifiedSendMsg Error(e_status:%d to:%s)",
                               e_status, dest.c_str());
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __func__,
           "CAN Delivery : dst = %s, CMD = %x, DLC = %x",
           dest.c_str(), cmd, msg->dlc);
  }
  return e_status;
}

static EFrameworkunifiedStatus CANDeliverySndMsgSingle(HANDLE h_app, CANID ul_canid,
        uint8_t uc_dlc, const uint8_t *puc_data,
        enum CanIfEchoBackFlags flag, std::string dest) {
  CAN_MSG_CANDATA st_delivery_msg;
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusFail;
  HANDLE h_client;
  uint8_t uc_size = uc_dlc;

  // Create delivery data
  memset(&st_delivery_msg, 0, sizeof(st_delivery_msg));
  st_delivery_msg.hdr.hdr.cid = CID_CAN_DATA_DELIVERY;
  st_delivery_msg.hdr.hdr.msgbodysize = (uint16_t)(CAN_TRX_CANID_SIZE + 
                                                 CAN_TRX_DLC_SIZE + 
                                                 (uint32_t)uc_size);
  st_delivery_msg.hdr.hdr.rid = 0;
  st_delivery_msg.data.can_id = ul_canid;
  st_delivery_msg.data.dlc = uc_size;
  st_delivery_msg.echoback = flag;
  if (uc_size > (uint8_t)CAN_DATA_SIZE)
    uc_size = CAN_DATA_SIZE;
  memcpy(st_delivery_msg.data.data, puc_data, (size_t)uc_size);

  h_client = CommonFindSender(h_app, dest);
  if (!h_client) {
    return e_status;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __func__, "can_id=%x dlc=%x echoback=%d",
         st_delivery_msg.data.can_id, st_delivery_msg.data.dlc,
         st_delivery_msg.echoback);
  FRAMEWORKUNIFIEDLOG(ZONE_DEBUG, __func__, "msg_data=%s",
         MessageDataOutputLog(st_delivery_msg.data.data, uc_size).c_str());

  e_status = FrameworkunifiedSendMsg(h_client, CID_CAN_DATA_DELIVERY,
                                   sizeof(st_delivery_msg), &st_delivery_msg);
  if (e_status != eFrameworkunifiedStatusOK) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "FrameworkunifiedSendMsg Error(e_status:%d to:%s)",
                               e_status, dest.c_str());
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __func__,
           "CAN Delivery : dst = %s, CANID = %x, DLC = %x",
           dest.c_str(), ul_canid, uc_dlc);
  }
  return e_status;
}

EFrameworkunifiedStatus CANDeliveryEntry(HANDLE h_app) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __func__, "@@@ Start communication CanRecv");

  uint16_t i;
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  CAN_DELIVERY_ENTRY rcv_msg;
  CAN_DeliveryEntryListIt it;
  bool fail = false;

  memset(&rcv_msg, 0, sizeof(rcv_msg));
  e_status = FrameworkunifiedGetMsgDataOfSize(h_app, &rcv_msg,
                                              sizeof(rcv_msg), eSMRRelease);
  if (e_status != eFrameworkunifiedStatusOK) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__,
          "CAN DeliveryEntry FrameworkunifiedGetMsgDataOfSize Error(%d)", e_status);
    if (e_status == eFrameworkunifiedStatusInvldBufSize) {
      FrameworkunifiedClearMsgData(h_app);
    }
    goto cleanup;
  }

  for (i = 0; i < rcv_msg.usCanNum; i++) {
    CANID canid = rcv_msg.ulCanid[i];
    CanMessage *latest = CANMsgFind(canid);

    if (CANDeliveryInsert(canid, rcv_msg.notifyName)) {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __func__, "(cnd:%lu) : CANID=0x%x, dst=%s",
        g_map_delivery_list_can.size(), canid, rcv_msg.notifyName);
    }

    if (latest) {
      switch (canid) {
      case CAN_CMDID_FUELCALC_RST_REQ_RX:
      case CAN_CMDID_STARTUP_FIN_RESP_RX:
      case CAN_CMDID_MRST_INFO_RESP_RX:
      case CAN_CMDID_VERSION_RESP_RX:
      case CAN_CMDID_CONNECTION_NODE_RESP_RX:
        if (eFrameworkunifiedStatusOK != CANDeliverySndCmdSingle(h_app, canid,
                                                 latest, rcv_msg.notifyName)) {
          fail = true;
        }
        break;
      default:
        if (eFrameworkunifiedStatusOK != CANDeliverySndMsgSingle(h_app, canid,
            static_cast<uint8_t>(latest->dlc), 
            latest->data, CANIF_PURERECV, rcv_msg.notifyName)) {
          fail = true;
        }
        break;
      }
    }
  }

  if (fail)
    e_status = eFrameworkunifiedStatusFail;

cleanup:
  return e_status;
}

static EFrameworkunifiedStatus CANDeliverySndMsgToClient(HANDLE h_app, CANID ul_canid, 
                        void *data, size_t size, PS_CommunicationProtocol cid) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusFail;
  HANDLE h_client = NULL;
  CAN_DeliveryEntryListIt it;
  std::pair<CAN_DeliveryEntryListIt, CAN_DeliveryEntryListIt> range;
  bool fail = false;

  it = g_map_delivery_list_can.find(ul_canid);
  range = g_map_delivery_list_can.equal_range(ul_canid);
  for (it = range.first; it != range.second; ++it) {
    h_client = CommonFindSender(h_app, it->second);
    if (!h_client) {
      continue;
    }

    e_status = FrameworkunifiedSendMsg(h_client, cid, (UI_32)size, data);
    if (e_status != eFrameworkunifiedStatusOK) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "FrameworkunifiedSendMsg Error(e_status:%d to:%s)",
                                              e_status, it->second.c_str());
      fail = true;
    }
  }
  return fail ? eFrameworkunifiedStatusFail : eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus CANDeliverySndMsg(HANDLE h_app, CANID ul_canid, uint8_t n_ta,
        uint8_t uc_dlc, const uint8_t *puc_data,
        PS_CommunicationProtocol cid, enum CanIfEchoBackFlags flag) {
  CAN_MSG_CANDATA st_delivery_msg;
  uint8_t uc_size = uc_dlc;

  // Create delivery data
  memset(&st_delivery_msg, 0, sizeof(st_delivery_msg));
  st_delivery_msg.hdr.hdr.cid = cid;
  st_delivery_msg.hdr.hdr.msgbodysize = (uint16_t)(CAN_TRX_CANID_SIZE + 
                                                 CAN_TRX_DLC_SIZE + 
                                                 (uint32_t)uc_size);
  st_delivery_msg.hdr.hdr.rid = 0;
  st_delivery_msg.data.can_id = ul_canid;
  st_delivery_msg.data.dlc = uc_size;
  st_delivery_msg.echoback = flag;
 
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __func__, "can_id=%x dlc=%x echoback=%d",
         st_delivery_msg.data.can_id, st_delivery_msg.data.dlc,
         st_delivery_msg.echoback);
  FRAMEWORKUNIFIEDLOG(ZONE_DEBUG, __func__, "msg_data=%s",
         MessageDataOutputLog(st_delivery_msg.data.data, uc_size).c_str());

 
  if (uc_size > (uint8_t)CAN_DATA_SIZE)
    uc_size = CAN_DATA_SIZE;
  memcpy(st_delivery_msg.data.data, puc_data, (size_t)uc_size);

  return CANDeliverySndMsgToClient(h_app, ul_canid, &st_delivery_msg,
                                   sizeof(st_delivery_msg), cid);
}

EFrameworkunifiedStatus CANCommandDeliveryRcvProcess(HANDLE h_app,
                                                 CanMessage *msg, uint8_t cmd) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusFail;
  CAN_MSG_CANCMD st_delivery_msg;
  uint16_t len = 0;

  memset(&st_delivery_msg, 0, sizeof(st_delivery_msg));
  switch (cmd) {
  case CAN_CMDID_FUELCALC_RST_REQ_RX:
    len = CAN_TX_CMD_DELIVERY_SIZE + CAN_TX_CMD_FUELCALC_RST_SIZE;
    break;
  case CAN_CMDID_STARTUP_FIN_RESP_RX:
    len = CAN_TX_CMD_DELIVERY_SIZE + CAN_TX_CMD_STARTUP_FIN_SIZE;
    break;
  case CAN_CMDID_MRST_INFO_RESP_RX:
    len = CAN_TX_CMD_DELIVERY_SIZE + CAN_TX_CMD_MRST_INFO_SIZE;
    break;
  case CAN_CMDID_VERSION_RESP_RX:
    len = CAN_TX_CMD_DELIVERY_SIZE + CAN_TX_CMD_VERSION_SIZE;
    break;
  case CAN_CMDID_CONNECTION_NODE_RESP_RX:
    len = (uint16_t)(CAN_TX_CMD_DELIVERY_SIZE + msg->data[0] + 1);
    break;
  default:
    return e_status;
  }
  st_delivery_msg.hdr.hdr.rid = 0;
  st_delivery_msg.hdr.hdr.cid = (uint16_t)CID_CAN_CMD_DELIVERY;
  st_delivery_msg.hdr.hdr.msgbodysize = len;
  st_delivery_msg.data.cmd_id = cmd;
  memcpy(&st_delivery_msg.data.data, msg->data,
                                     (size_t)(len - CAN_TX_CMD_DELIVERY_SIZE));
  CANMsgInsert(cmd, msg);

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __func__, "cid=%x msgbodysize=%x cmd_id=%x",
         st_delivery_msg.hdr.hdr.cid, st_delivery_msg.hdr.hdr.msgbodysize,
         st_delivery_msg.data.cmd_id);
  FRAMEWORKUNIFIEDLOG(ZONE_DEBUG, __func__, "msg_data=%s",
         MessageDataOutputLog(st_delivery_msg.data.data, len).c_str());

  return CANDeliverySndMsgToClient(h_app, msg->can_id, &st_delivery_msg,
                                 sizeof(st_delivery_msg), CID_CAN_CMD_DELIVERY);
}

EFrameworkunifiedStatus CANDeliveryRcvProcess(HANDLE h_app, CanMessage *msg) {
  if (!Canif_CheckCanID(msg->can_id)) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "CANID is invalid (%x)", msg->can_id);
    return eFrameworkunifiedStatusFail;
  }

  CANCommWatchClear(h_app, msg->can_id);
  CANMsgInsert(msg->can_id, msg);
  if (eFrameworkunifiedStatusOK != CANDeliverySndMsg(h_app, msg->can_id, CAN_NTA_INVALID,
                       (uint8_t)msg->dlc, msg->data, CID_CAN_DATA_DELIVERY)) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "CAN Delivery Error");
    return eFrameworkunifiedStatusFail;
  }

  return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus CANClearEntry(HANDLE h_app) {
  char notify_name[CANIF_NOTIFY_NAME_MAX_SIZE + 1] = {0};
  CAN_DeliveryEntryListIt it;
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;

  e_status = FrameworkunifiedGetMsgDataOfSize(h_app, &notify_name,
                                 CANIF_NOTIFY_NAME_MAX_SIZE, eSMRRelease);
  if (e_status != eFrameworkunifiedStatusOK) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__,
                "#CAN thread# FrameworkunifiedGetMsgDataOfSize Error(%d)", e_status);
    if (e_status == eFrameworkunifiedStatusInvldBufSize) {
      FrameworkunifiedClearMsgData(h_app);
    }
    return e_status;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_DEBUG, __func__, "notify_name=%s", notify_name);

  // To avoid destruction of iterator.
  it = g_map_delivery_list_can.begin();
  while (it != g_map_delivery_list_can.end()) {
    size_t n = strnlen(it->second.c_str(), CANIF_NOTIFY_NAME_MAX_SIZE);
    if (!strncmp(it->second.c_str(), notify_name, n)) {
      g_map_delivery_list_can.erase(it++);
    } else {
      ++it;
    }
  }

  return eFrameworkunifiedStatusOK;
}
