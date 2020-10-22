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
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <native_service/frameworkunified_framework_if.h>
#include <peripheral_service/Canif_API.h>

#include "API_Local_Common.h"
#include "Canif_API_Local.h"
#include "Canif_TransmissionData.h"
#include "com_error_type.h"

static __thread HANDLE g_sender = NULL;

static EFrameworkunifiedStatus CanifMsgToSrv(HANDLE h, UI_32 cmd, size_t len, PCVOID data) {
  if (data == NULL)
    return eFrameworkunifiedStatusFail;

  if (len > UINT32_MAX)
    return eFrameworkunifiedStatusFail;

  if (g_sender == NULL) {
    g_sender = FrameworkunifiedMcOpenSender(h, LAN_SERVICE_CAN);
    if (g_sender == NULL)
      return eFrameworkunifiedStatusFail;
  }
  return FrameworkunifiedSendMsg(g_sender, cmd, (UI_32)len, data);
}

CANIF_RET_API Canif_DeliveryEntry(HANDLE h_app, PCSTR notify_name, 
                                  uint8_t can_num, CANID *p_can_id) {

  CANIF_RET_API l_ret = CANIF_RET_ERROR_PARAM;
  CAN_DELIVERY_ENTRY pst_delivery_entry;
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  PS_Communication_InternalProtocol cmd = CID_CANIF_DELIVERY_ENTRY;
  void *tx = reinterpret_cast<void *>(&pst_delivery_entry);
  size_t len = sizeof(pst_delivery_entry);
  memset(tx, 0x0, len);
  int32_t i;

  if (NULL == h_app)
    goto cleanup;

  if (NULL == p_can_id)
    goto cleanup;

  if (!Canif_CheckNotifyName(notify_name))
    goto cleanup;

  if (0 == can_num)
    goto cleanup;

  if (CAN_DELIVERY_CANID_ENTRY_MAX < can_num)
    goto cleanup;

  for (i = 0; i < (int32_t)can_num; i++) {
    if (!Canif_CheckCanID(p_can_id[i]))
      goto cleanup;
  }

  Canif_CopyNotifyName(pst_delivery_entry.notifyName, notify_name);
  pst_delivery_entry.usCanNum = (uint16_t)can_num;
  memcpy(pst_delivery_entry.ulCanid, p_can_id, sizeof(CANID) * can_num);

  e_status = CanifMsgToSrv(h_app, cmd, len, tx);
  if (e_status != eFrameworkunifiedStatusOK) {
    l_ret = CANIF_RET_ERROR_CANCEL;
    goto cleanup;
  }

  l_ret = CANIF_RET_NORMAL;
cleanup:
  return l_ret;
}

CANIF_RET_API Canif_TransmissionStart(HANDLE h_app, PCSTR notify_name, 
                                uint8_t rid, uint16_t freq, CAN_DATA *p_data) {

  CANIF_RET_API l_ret = CANIF_RET_ERROR_PARAM;
  CAN_TRANSMISSION_START_MSG_DAT pst_transmission_start;
  PS_Communication_InternalProtocol cmd = CID_CANIF_TX_START;
  void *tx = reinterpret_cast<void *>(&pst_transmission_start);
  size_t len = sizeof(pst_transmission_start);
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  PCSTR _notify_name = notify_name;
  memset(tx, 0x0, len);

  if (h_app == NULL)
    goto cleanup;

  if (p_data == NULL)
    goto cleanup;

  if (rid == 0xFF) {
    _notify_name = "";  // using dummy name;
  }

  if (!Canif_CheckNotifyName(_notify_name))
    goto cleanup;

  if (CAN_TXDATA_SIZE < (p_data->dlc))
    goto cleanup;

  if (!Canif_CheckCanID(p_data->can_id))
    goto cleanup;

  if (!Canif_InitDataIsDefined21PF(p_data->can_id))
    goto cleanup;
  
  Canif_CopyNotifyName(pst_transmission_start.notifyName, _notify_name);
  pst_transmission_start.ucRid = rid;
  pst_transmission_start.usFreq = freq;
  memcpy(reinterpret_cast<void *>(&pst_transmission_start.stCandata),
         reinterpret_cast<int8_t *>(p_data), (size_t)sizeof(CAN_DATA));
  e_status = CanifMsgToSrv(h_app, cmd, len, tx);
  if (e_status != eFrameworkunifiedStatusOK) {
    l_ret = CANIF_RET_ERROR_CANCEL;
    goto cleanup;
  }

  l_ret = CANIF_RET_NORMAL;
cleanup:
  return l_ret;
}

CANIF_RET_API Canif_CommandCtrl(HANDLE h_app, PCSTR notify_name, uint8_t rid,
                                uint32_t cmd_id) {

  CANIF_RET_API l_ret = CANIF_RET_ERROR_PARAM;
  CAN_CMD_CTRL_MSG_DAT pst_cmd_ctrl_msg;
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  PS_Communication_InternalProtocol cmd = CID_CANIF_CMD_CTRL;
  void *tx = reinterpret_cast<void *>(&pst_cmd_ctrl_msg);
  size_t len = sizeof(pst_cmd_ctrl_msg);
  PCSTR _notify_name = notify_name;
  memset(tx, 0x0, len);

  if (!Canif_IsCommand(cmd_id))
    goto cleanup;

  if (h_app == NULL)
    goto cleanup;

  if (rid == 0xFF) {
    _notify_name = "";  // using dummy name;
  }

  if (!Canif_CheckNotifyName(_notify_name))
    goto cleanup;

  Canif_CopyNotifyName(pst_cmd_ctrl_msg.notifyName, _notify_name);
  pst_cmd_ctrl_msg.ucRid = rid;
  pst_cmd_ctrl_msg.ucCmdid = (uint8_t)cmd_id;

  e_status = CanifMsgToSrv(h_app, cmd, len, tx);
  if (e_status != eFrameworkunifiedStatusOK) {
    l_ret = CANIF_RET_ERROR_CANCEL;
    goto cleanup;
  }

  l_ret = CANIF_RET_NORMAL;
cleanup:
  return l_ret;
}

CANIF_RET_API Canif_CommWatch(HANDLE h_app, PCSTR notify_name, CANID can_id, 
                              DID did, uint16_t watch_time) {

  CANIF_RET_API l_ret = CANIF_RET_ERROR_PARAM;
  CAN_COMM_WATCHEXT_MSG_DAT pst_comm_watch_msg;
  PS_Communication_InternalProtocol cmd = CID_CANIF_COMM_WATCH;
  void *tx = reinterpret_cast<void *>(&pst_comm_watch_msg);
  size_t len = sizeof(pst_comm_watch_msg);
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  memset(tx, 0x0, len);

  if (h_app == NULL)
    goto cleanup;

  if (!Canif_CheckNotifyName(notify_name))
    goto cleanup;

  if (!Canif_CheckCanID(can_id))
    goto cleanup;

  Canif_CopyNotifyName(pst_comm_watch_msg.notifyName, notify_name);
  pst_comm_watch_msg.ulCanid = can_id;
  pst_comm_watch_msg.ulDid = did;
  pst_comm_watch_msg.ucIg = CAN_IG_COOPERATION_OFF;
  pst_comm_watch_msg.usWatchTime = watch_time;

  e_status = CanifMsgToSrv(h_app, cmd, len, tx);
  if (e_status != eFrameworkunifiedStatusOK) {
    l_ret = CANIF_RET_ERROR_CANCEL;
    goto cleanup;
  }

  l_ret = CANIF_RET_NORMAL;
cleanup:
  return l_ret;
}

CANIF_RET_API Canif_TransStart(HANDLE h_app, CANID can_id,
                      CAN_DATA_MASK *mask, CAN_DATA_BIT *dat, uint32_t freq) {
  CANIF_RET_API l_ret = CANIF_RET_ERROR_PARAM;
  CAN_TRANS_START_MSG_DAT pst_trans_start;
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  PS_Communication_InternalProtocol cmd = CID_CANIF_TX_BIT_START;
  void *tx = reinterpret_cast<void *>(&pst_trans_start);
  size_t len = sizeof(pst_trans_start);
  memset(tx, 0x0, len);

  if (h_app == NULL)
    goto cleanup;

  if (!Canif_CheckCanID(can_id))
    goto cleanup;

  if (!Canif_InitDataIsDefined21PF(can_id))
    goto cleanup;

  if (mask == NULL)
    goto cleanup;

  if (dat == NULL)
    goto cleanup;

  if (0xFFFF < freq)
    goto cleanup; 

  pst_trans_start.id = can_id;
  pst_trans_start.freq = freq;
  memcpy(&pst_trans_start.mask.dat, mask->dat, sizeof(mask->dat));
  memcpy(&pst_trans_start.dat.dat, dat->dat, sizeof(dat->dat));

  e_status = CanifMsgToSrv(h_app, cmd, len, tx);
  if (e_status != eFrameworkunifiedStatusOK) {
    l_ret = CANIF_RET_ERROR_CANCEL;
    goto cleanup;
  }

  l_ret = CANIF_RET_NORMAL;
cleanup:
  return l_ret;
}

CANIF_RET_API Canif_DeliveryEraseAll(HANDLE h_app, PCSTR notify_name) {
  CANIF_RET_API l_ret = CANIF_RET_ERROR_PARAM;
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  HANDLE handle[NUM_OF_CANIF_PROTOCOL];
  int i = 0;

  for (i = 0; i < NUM_OF_CANIF_PROTOCOL; i++) {
    handle[i] = NULL;
  }

  if (NULL == h_app)
    goto cleanup;

  if (!Canif_CheckNotifyName(notify_name))
    goto cleanup; 

  l_ret = CANIF_RET_ERROR_CANCEL;
  for (i = 0; i < NUM_OF_CANIF_PROTOCOL; i++) {
    handle[i] = FrameworkunifiedMcOpenSender(h_app,
                                             Canif_PidxTosname((enum CANIF_PROTOCOL_IDX)i));
    if (!handle[i])
      goto cleanup;
  }

  for (i = 0; i < NUM_OF_CANIF_PROTOCOL; i++) {
    e_status = FrameworkunifiedSendMsg(handle[i], CID_CANIF_DELIVERY_ERASE,
                                       CANIF_NOTIFY_NAME_MAX_SIZE, (PCVOID)notify_name);
    if (e_status != eFrameworkunifiedStatusOK)
      goto cleanup;
  }

  l_ret = CANIF_RET_NORMAL;
cleanup:
  for (i = 0; i < NUM_OF_CANIF_PROTOCOL; i++) {
    if (handle[i] != NULL)
      FrameworkunifiedMcClose(handle[i]);
  }
  return l_ret;
}

