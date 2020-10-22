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

#include "CAN_Thread.h"

extern "C" {
#include <poll.h>
#include <sys/timerfd.h>
}
#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_timer.h>
#include <peripheral_service/Canif_API.h>
#include <peripheral_service/communication_notifications.h>
#include <can_hal.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <agl_thread.h>
#include <string>
#include "CAN_Delivery.h"
#include "CAN_CommWatch.h"
#include "CAN_TxMsg.h"

#include "communication_communicationlog.h"
#include "communication_cid.h"
#include "Thread_Common.h"

static EFrameworkunifiedStatus CANStateCallback(HANDLE h_app);
static EFrameworkunifiedStatus CANHalRcvProcess(HANDLE h_app);
static EFrameworkunifiedStatus CANHalSndStsProcess(HANDLE h_app);

static const FrameworkunifiedProtocolCallbackHandler kCanPcbhs[] = {
    {CID_CANIF_DELIVERY_ENTRY, CANDeliveryEntry},
    {CID_CANIF_DELIVERY_ERASE, CANClearEntry},
    {CID_COMMSYS_TIMEOUT, CANCallbackForTimeOut},
    {CID_CANIF_TX_START, CANTxMsg},
    {CID_CANIF_TX_BIT_START, CANTxMsgBit},
    {CID_CANIF_COMM_WATCH, CANCommWatch},
    {CID_CANIF_CMD_CTRL, CANTxMsgCommand},
    {CID_CANHAL_CMD_CAN_READY, CANStateCallback},
    {CID_CANHAL_CMD_ERROR_NOTIFY, CommonCanHalErrorNotify},
    {CID_CANHAL_CMD_CAN_RECV, CANHalRcvProcess},
    {CID_CANHAL_CMD_CAN_SEND_STATUS, CANHalSndStsProcess},
};

static UI_32 kCanPcbhsD[] = {
    CID_CANIF_DELIVERY_ENTRY,
    CID_CANIF_DELIVERY_ERASE,
    CID_COMMSYS_TIMEOUT,
    CID_CANIF_TX_START,
    CID_CANIF_TX_BIT_START,
    CID_CANIF_COMM_WATCH,
    CID_CANIF_CMD_CTRL,
    CID_CANHAL_CMD_CAN_READY,
    CID_CANHAL_CMD_ERROR_NOTIFY,
    CID_CANHAL_CMD_CAN_RECV,
    CID_CANHAL_CMD_CAN_SEND_STATUS
};

static EFrameworkunifiedStatus CANStateCallback(HANDLE h_app) {
  bool result;
  EFrameworkunifiedStatus err = eFrameworkunifiedStatusOK;

  err = FrameworkunifiedGetMsgDataOfSize(h_app, &result, sizeof(result), eSMRRelease);
  if (err != eFrameworkunifiedStatusOK) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: FrameworkunifiedGetMsgDataOfSize failed");
  }
  if (err == eFrameworkunifiedStatusInvldBufSize) {
    FrameworkunifiedClearMsgData(h_app);
    return eFrameworkunifiedStatusFail;
  } else if (err != eFrameworkunifiedStatusOK) {
    return eFrameworkunifiedStatusFail;
  }

  CommSetAvailabilityCurrent(CAN_AVAILABILITY);
  return CommonStartNotify(h_app, NTFY_Communication_CAN_ISAVAILABLE);
}

static EFrameworkunifiedStatus CANThreadCanOpen(HANDLE h_app) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  const char *result = "Unknown";
  const char *called = "Unknown";
  UI_16 z = ZONE_ERR;

  e_status = ConvRet(CanOpen(h_app, CAN_HAL_TYPE_CAN));
  result = (e_status != eFrameworkunifiedStatusOK) ? "failed" : "success";
  z = (e_status != eFrameworkunifiedStatusOK) ? ZONE_INFO : ZONE_ERR;
  called = "CanOpen";
  FRAMEWORKUNIFIEDLOG(z, __func__, "%s %s", called, result);
  return e_status;
}

EFrameworkunifiedStatus CANThreadStart(HANDLE h_app) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;

  CommClrAvailabilityCurrent(CAN_AVAILABILITY);
  CANDeliveryInit();
  CANCommWatchInit();
  CANTxMsgInit();

  e_status = CommonThreadStart(h_app, kCanPcbhs, _countof(kCanPcbhs),
                          NTFY_Communication_CAN_ISAVAILABLE, CANThreadCanOpen);
  return e_status;
}

static EFrameworkunifiedStatus CANThreadCanClose(HANDLE h_app) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  const char *result = "Unknown";
  const char *called = "Unknown";
  UI_16 z = ZONE_ERR;

  e_status = ConvRet(CanClose(h_app, CAN_HAL_TYPE_CAN));
  result = (e_status != eFrameworkunifiedStatusOK) ? "failed" : "success";
  z = (e_status != eFrameworkunifiedStatusOK) ? ZONE_INFO : ZONE_ERR;
  called = "CanClose";
  FRAMEWORKUNIFIEDLOG(z, __func__, "%s %s", called, result);
  return e_status;
}

EFrameworkunifiedStatus CANThreadStop(HANDLE h_app) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;

  CommClrAvailabilityCurrent(CAN_AVAILABILITY);
  e_status = CommonThreadStop(h_app, kCanPcbhsD, _countof(kCanPcbhsD),
                              NTFY_Communication_CAN_ISAVAILABLE,
                              CANThreadCanClose);
  return e_status;
}

static EFrameworkunifiedStatus CANHalRcvProcess(HANDLE h_app) {
  CanMessage canhal_recv_data;
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusFail;
  uint8_t cmd = 0;

  e_status = FrameworkunifiedGetMsgDataOfSize(h_app, &canhal_recv_data,
                            sizeof(canhal_recv_data), eSMRRelease);
  if (e_status != eFrameworkunifiedStatusOK) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error : FrameworkunifiedGetMsgDataOfSize is failed, e_status=%d", e_status);
  }

  if (e_status == eFrameworkunifiedStatusInvldBufSize) {
    FrameworkunifiedClearMsgData(h_app);
    goto cleanup;
  } else if (e_status != eFrameworkunifiedStatusOK) {
    goto cleanup;
  }

  if (canhal_recv_data.dlc > CAN_MESSAGE_LEN) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error : Invalid data length, dlc=%d", canhal_recv_data.dlc);
    return eFrameworkunifiedStatusFail;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __func__, "can_id=%x dlc=%d rid=%x",
         canhal_recv_data.can_id, canhal_recv_data.dlc, canhal_recv_data.rid);
  FRAMEWORKUNIFIEDLOG(ZONE_DEBUG, __func__, "msg_data=%s",
         MessageDataOutputLog(canhal_recv_data.data, canhal_recv_data.dlc).c_str());

  cmd = (uint8_t)(canhal_recv_data.can_id & 0xFF);
  switch (canhal_recv_data.can_id) {
  case CAN_CMDID_STARTUP_FIN_RESP_RX:
    e_status = CANCommandDeliveryRcvProcess(h_app, &canhal_recv_data, cmd);
    break;
  case CAN_CMDID_MRST_INFO_RESP_RX:
    e_status = CANCommandDeliveryRcvProcess(h_app, &canhal_recv_data, cmd);
    break;
  case CAN_CMDID_VERSION_RESP_RX:
    e_status = CANCommandDeliveryRcvProcess(h_app, &canhal_recv_data, cmd);
    break;
  case CAN_CMDID_CONNECTION_NODE_RESP_RX:
    e_status = CANCommandDeliveryRcvProcess(h_app, &canhal_recv_data, cmd);
    break;
  case CAN_CMDID_FUELCALC_RST_REQ_RX:
    CANCommandSetFuelCalcRstReq();
    e_status = CANCommandDeliveryRcvProcess(h_app, &canhal_recv_data, cmd);
    break;
  default:
    e_status = CANDeliveryRcvProcess(h_app, &canhal_recv_data);
  }
cleanup:
  return e_status;
}

static EFrameworkunifiedStatus CANHalSndStsProcess(HANDLE h_app) {
  CanSendResult canhal_recv_data;
  EFrameworkunifiedStatus err = eFrameworkunifiedStatusOK;
  uint8_t ui_can_rid;
  PS_CommunicationProtocol cid;

  err = FrameworkunifiedGetMsgDataOfSize(h_app, &canhal_recv_data,
                            sizeof(canhal_recv_data), eSMRRelease);
  if (err != eFrameworkunifiedStatusOK) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error : FrameworkunifiedGetMsgDataOfSize is failed, err=%d", err);
  }

  if (err == eFrameworkunifiedStatusInvldBufSize) {
    FrameworkunifiedClearMsgData(h_app);
    return eFrameworkunifiedStatusFail;
  } else if (err != eFrameworkunifiedStatusOK) {
    return eFrameworkunifiedStatusFail;
  }

  ui_can_rid = canhal_recv_data.rid;
  if (CAN_RID_NOTUSE_CODE == ui_can_rid) {
    return eFrameworkunifiedStatusOK;
  }

  switch (canhal_recv_data.can_id) {
  case CAN_CMDID_MRST_INFO_REQ_TX:
  case CAN_CMDID_CONNECTION_NODE_REQ_TX:
  case CAN_CMDID_FUELCALC_REQ_TX:
    cid = CID_CAN_CMD_TX_RESULT;
    break;
  default:
    cid = CID_CAN_TX_RESULT;
    break;
  }

  return CANSndStsProcess(h_app, &canhal_recv_data, cid);
}

EFrameworkunifiedStatus CANCallbackForTimeOut(HANDLE h_app) {
  CANCommWatchTimeout(h_app);
  return eFrameworkunifiedStatusOK;
}
