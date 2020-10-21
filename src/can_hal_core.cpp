/*
 * @copyright Copyright (c) 2017-2020 TOYOTA MOTOR CORPORATION.
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

#include "can_hal.h"
#include "can_hal_core.h"
#include "can_hal_internal.h"
#include "can_hal_frameworkunifiedlog.h"
#include "can_hal_stm.h"
extern "C" {
  #include "can_mng_api.h"
}
#include <native_service/ns_message_center_if.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_multithreading.h>
#include <stdio.h>
#include <pthread.h>
#include <string>

static CanCtlApiObj g_driver_obj;        // can driver handler

typedef struct MessageWrapper {
  HANDLE h_app;
  void *message;
} MessageWrapper;

// define frame
enum CANHAL_FRAME {
  CANHAL_FRAME_SF = 0x00,
  CANHAL_FRAME_FF,
  CANHAL_FRAME_CF,
  CANHAL_FRAME_NONE = 0xFF,
};

// Declaration of the local functions
static EFrameworkunifiedStatus CanSendData(HANDLE h_app);
static UINT8 PackageSendData(const CanMessage *send_msg,
                            UINT8 *send_cmd);
static UINT8 PackageRecvData(const UINT8 *recv_data, CanMessage *message);
static uint32_t GetCanId(const UINT8 *recv_data);

#define CANHAL_VALID_NUM_INDEX   0x00
#define CANHAL_CANID_HI_INDEX    0x00
#define CANHAL_CANID_LO_INDEX    0x01
#define CANHAL_DLC_INDEX         0x02
#define CANHAL_DATA_START_INDEX  0x03

#define CANHAL_RECV_DATA_LENGTH_INVALID 0xFF

#define CANHAL_RECV_DATABLOCK_SIZE                (0x0B)
#define CANHAL_RECV_NTA_INDEX                     (0x03)

// Register table for framework callback
static const FrameworkunifiedProtocolCallbackHandler kCanHalPcbhs[] = {
  { TX_INTERNAL, CanSendData },
};

void *CanHalPackMessage(HANDLE h_app, const void *msg, ssize_t msg_sz,
                        ssize_t *packed_sz) {
  *packed_sz = msg_sz + sizeof(ssize_t) + sizeof(HANDLE);
  void *p = malloc(*packed_sz);
  if (p) {
    char *_p = (char *)p;
    // Set HANDLE to new buffer.
    memcpy(_p, &h_app, sizeof(HANDLE));
    // Set message size to new buffer
    _p += sizeof(HANDLE);
    *(ssize_t *)_p = msg_sz;
    // Set message body to new buffer
    _p += (sizeof(ssize_t));
    memcpy(_p, msg, msg_sz);
  }
  return p;
}

void CanHalUnPackMessage(void *packed, HANDLE *h_app, void **msg,
                         ssize_t *msg_sz) {
  char *_p = (char *)packed;
  *h_app = *(HANDLE *)_p;
  _p += sizeof(HANDLE);
  *msg_sz = *((ssize_t *)_p);
  _p += sizeof(ssize_t);
  *msg = _p;
}

void CanHalDestroyPackedMessage(void *packed) {
  free(packed);
}

// Start function of the receive thread
void *CanRecvRun(void *arg) {
  UINT8 data_start_index = 0;
  UINT8 loop_num = 0;
  UINT32 ret = 0;
  CanCtlApiCmd recv_cmd;
  memset(&recv_cmd, 0x00, sizeof(CanCtlApiCmd));
  CanMessage recv_data_can;
  memset(&recv_data_can, 0x00, sizeof(recv_data_can));
  enum CanHalType *type = (enum CanHalType *)arg;

  while (1) {
    // call driver API to receive the can data
    ret = CanCtlApiRcvCmd(&g_driver_obj, &recv_cmd);
    if (CAN_CTL_RET_SUCCESS != ret) {
      continue;
    }

    /**
     * Receive data in the following structure:
     *           -----------------
     * BYTE 01  | num of can data |
     *           -----------------
     * BYTE 02  | CAN ID (Hi)     |
     *           -----------------
     * BYTE 03  | CAN ID (Lo)     |
     *           -----------------
     * BYTE 04  | DLC             |
     *           -----------------
     * BYTE 05  | DATA #1         |
     *           -----------------
     *          | ...             |
     *           -----------------
     * BYTE 12  | DATA #8         |
     *           -----------------
     * BYTE 13  | CAN ID (Hi)     |
     *           -----------------
     * BYTE 14  | CAN ID (Lo)     |
     *           -----------------
     * BYTE 15  | DLC             |
     *           -----------------
     * BYTE 16  | DATA #1         |
     *           -----------------
     *          | ...             |
     *           -----------------
     * BYTE 23  | DATA #8         |
     *           -----------------
     *          | ...             |
     *           -----------------
     * BYTE 255 |                 |
     *           -----------------
     *
     * BYTE 0 for the invalid number of the can data
     * CAN ID (Hi) and (Lo) combine for the CAN ID
     * CAN ID (Hi) byte's low 7 bits is the high 7 bits of the CAN ID
     * CAN ID (Lo) byte's high 4 bits is the low 4 bits of the CAN ID
     * DLC for the length of the following data
     * DATA #1 ~ #8 for the receive data, its actual length changes
     *   according to the previous DLC byte
     */
    UINT8 total_data_num = recv_cmd.data[CANHAL_VALID_NUM_INDEX];
    data_start_index = 1;

    for (loop_num = 0; loop_num < total_data_num; loop_num++) {
      uint32_t can_id;
      can_id = GetCanId(&(recv_cmd.data[data_start_index]));

      // normal can
      // Return value of the PackageRecvData is the total length
      //   of one can data package, so add to the data_start_index
      //   for the next can data package.
      ret = PackageRecvData(&(recv_cmd.data[data_start_index]),
                            &recv_data_can);
      if (CANHAL_RECV_DATA_LENGTH_INVALID == ret) {
        continue;
      }
      data_start_index += ret;
      CanHalReceiveNotify(*type, &(recv_data_can), sizeof(recv_data_can));
    }

    // reset buffer
    memset(&recv_cmd, 0x00, sizeof(CanCtlApiCmd));
    usleep(1000);
  }

  return NULL;
}

// get can id from recive buffer
static uint32_t GetCanId(const UINT8 *recv_data) {
  uint32_t can_id;
  can_id = (recv_data[CANHAL_CANID_HI_INDEX] & 0x7F);
  can_id <<= 4;
  can_id |= ((recv_data[CANHAL_CANID_LO_INDEX] & 0xF0) >> 4);
  return can_id;
}

static CANHAL_RET_API CanOpenCoreCAN(void) {
  CanCtlRcvId recv_can_id;
  memset(recv_can_id.id, 0xFF, sizeof(recv_can_id.id));
  if (CAN_CTL_RET_SUCCESS != CanCtlApiOpen(&g_driver_obj))
    return CANHAL_RET_ERR_ERR;

  if (CAN_CTL_RET_SUCCESS != CanCtlApiSetRcvId(&g_driver_obj, &recv_can_id)) {
    return CANHAL_RET_ERR_ERR;
  }

  return CANHAL_RET_NORMAL;
}

CANHAL_RET_API CanOpenCore(CanHalType type) {
  CANHAL_RET_API ret = CANHAL_RET_ERR_ERR;
  switch (type) {
  case CAN_HAL_TYPE_CAN:
    ret = CanOpenCoreCAN();
    break;
  default:
    // Do nothing
    break;
  }
  return ret;
}

// Initialize the sending thread
EFrameworkunifiedStatus CanSendThreadStart(HANDLE h_app) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  intptr_t ptr;
  e_status = FrameworkunifiedAttachCallbacksToDispatcher(h_app,
                                            FRAMEWORKUNIFIED_ANY_SOURCE,
                                            kCanHalPcbhs,
                                            _countof(kCanHalPcbhs));
  if (e_status != eFrameworkunifiedStatusOK)
    goto finish;
  
  e_status = FrameworkunifiedGetMsgDataOfSize(h_app, &ptr, sizeof(ptr));
  if (e_status != eFrameworkunifiedStatusOK)
    goto finish;

  *((HANDLE *)ptr) = FrameworkunifiedMcOpenSender(h_app, FrameworkunifiedGetAppName(h_app));
  if (!(*(HANDLE *)ptr))
    e_status = eFrameworkunifiedStatusFail;

finish:
  return e_status;
}

// Clean the sending thread
EFrameworkunifiedStatus CanSendThreadStop(HANDLE h_app) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  intptr_t ptr;
  e_status = FrameworkunifiedGetMsgDataOfSize(h_app, &ptr, sizeof(ptr));
  if (e_status != eFrameworkunifiedStatusOK)
    goto finish;

  FrameworkunifiedMcClose(*((HANDLE*)ptr));
finish:
  return e_status;
}

static CANHAL_RET_API CanCloseCoreCAN(void) {
  CanCtlApiClose(&g_driver_obj);
  return CANHAL_RET_NORMAL;
}

// Stop the can hal
CANHAL_RET_API CanCloseCore(CanHalType type) {
  CANHAL_RET_API ret = CANHAL_RET_NORMAL;
  switch (type) {
  case CAN_HAL_TYPE_CAN:
    ret = CanCloseCoreCAN();
    if (ret != CANHAL_RET_NORMAL)
      goto finish;
    break;
  default:
    goto finish;
    break;
  }
finish:
  return ret;
}

// Callback for the sending thread to send the can data
static EFrameworkunifiedStatus CanSendData(HANDLE h_app) {
  UINT8 ret = 0;
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  HANDLE hb = NULL;
  CanMessage mb;
  ssize_t sz = 0;
  void *send_msg = NULL;
  HANDLE unpacked_h_app;
  CanMessage *unpacked_msg;
  ssize_t unpacked_sz;

  send_msg = CanHalPackMessage(hb, (const void *)&mb, sizeof(mb), &sz);
  if (!send_msg)
    return eFrameworkunifiedStatusFail;

  e_status = FrameworkunifiedGetMsgDataOfSize(h_app, send_msg, sz, eSMRRelease);
  if (eFrameworkunifiedStatusOK != e_status) {
    if (eFrameworkunifiedStatusInvldBufSize == e_status) {
      FrameworkunifiedClearMsgData(h_app);
    }
    return eFrameworkunifiedStatusFail;
  }

  CanHalUnPackMessage(send_msg, &unpacked_h_app,
                      (void **)&unpacked_msg, &unpacked_sz);

  CanCtlApiCmd send_can_data;
  memset(&send_can_data, 0, sizeof(CanCtlApiCmd));
  send_can_data.data[CANHAL_VALID_NUM_INDEX] = 1;
  ret = PackageSendData(unpacked_msg, &(send_can_data.data[1]));
  send_can_data.len = ret + 1;
  CanSendResult send_result;
  send_result.can_id = unpacked_msg->can_id;
  send_result.rid = unpacked_msg->rid;

  if (CAN_CTL_RET_SUCCESS == CanCtlApiSndCmd(&g_driver_obj, &send_can_data)) {
    send_result.result = CAN_SEND_RESULT_SUCCESS;
  } else {
    send_result.result = CAN_SEND_RESULT_FAILURE;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Send status is %d.", send_result.result);

  if (CANHAL_RET_NORMAL != CanHalSendStatus(CAN_HAL_TYPE_CAN,
                           unpacked_h_app, &send_result, sizeof(send_result))) {
    CanHalDestroyPackedMessage(send_msg);
    return eFrameworkunifiedStatusFail;
  }

  CanHalDestroyPackedMessage(send_msg);
  return eFrameworkunifiedStatusOK;
}

/**
 * param [in]  send_msg
 * param [out] send_can_data
 */
static UINT8 PackageSendData(const CanMessage *send_msg,
                             UINT8 *send_can_data) {
  // package according to the rule of the data structure,
  //   refer to the line 108 and 109
  send_can_data[CANHAL_CANID_HI_INDEX] = (send_msg->can_id & 0x7F0) >> 4;
  send_can_data[CANHAL_CANID_LO_INDEX] = (send_msg->can_id & 0x0F) << 4;
  send_can_data[CANHAL_DLC_INDEX] = send_msg->dlc;
  memcpy(&(send_can_data[CANHAL_DATA_START_INDEX]),
         send_msg->data,
         send_msg->dlc);
  return (send_msg->dlc + CANHAL_DATA_START_INDEX);
}

/**
 * param [in]  recv_data
 * param [out] message
 */
static UINT8 PackageRecvData(const UINT8 *recv_data, CanMessage *message) {
  if (CAN_NORMAL_MESSAGE_LEN < recv_data[CANHAL_DLC_INDEX]) {
    // receive data's length is invalid
    return CANHAL_RECV_DATA_LENGTH_INVALID;
  }
  // package according to the rule of the data structure,
  //   refer to the line 108 and 109
  message->can_id = recv_data[CANHAL_CANID_HI_INDEX] & 0x7F;
  message->can_id <<= 4;
  message->can_id |= (recv_data[CANHAL_CANID_LO_INDEX] & 0xF0) >> 4;
  message->dlc = recv_data[CANHAL_DLC_INDEX];
  memcpy(message->data, &(recv_data[CANHAL_DATA_START_INDEX]), CAN_NORMAL_MESSAGE_LEN);

  return CANHAL_RECV_DATABLOCK_SIZE;
}

