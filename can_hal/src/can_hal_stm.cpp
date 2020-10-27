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

#include "can_hal_stm.h"
#include "can_hal_core.h"
#include "can_hal_internal.h"
#include <native_service/ns_message_center_if.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_multithreading.h>
#include <string>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <pthread.h>

// BSS section would be initialized to all 0.
struct CanHalStateMachine {
  CanHalType type;
  HANDLE h_app;
  bool can_hal_is_opened;
  bool device_is_enabled;
  struct {
    HANDLE tx;
    HANDLE tx_sender;
    uint32_t tx_cmd;
    const char *tx_name;
    bool rx_initialized;
    pthread_t rx;
    pthread_attr_t rx_attr;
  } internal;
} can_hal_stm[NR_CAN_HAL_TYPES];

bool TypeIsValid(enum CanHalType type) {
  if (CAN_HAL_TYPE_CAN == type)
    return true;

  return false;
}

bool IsCanHalOpened(enum CanHalType type) {
  return can_hal_stm[type].can_hal_is_opened;
}

void SetCanHalStateOpen(enum CanHalType type) {
  can_hal_stm[type].can_hal_is_opened = true;
}

void SetCanHalStateClose(enum CanHalType type) {
  can_hal_stm[type].can_hal_is_opened = false;
}

bool IsDeviceEnabled(enum CanHalType type) {
  return can_hal_stm[type].device_is_enabled;
}

void SetDeviceStateEnable(enum CanHalType type) {
  can_hal_stm[type].device_is_enabled = true;
}

void SetDeviceStateDisable(enum CanHalType type) {
  can_hal_stm[type].device_is_enabled = false;
}

CANHAL_RET_API CanHalDestroyInternalThread(HANDLE h_app, enum CanHalType type) {
  intptr_t ptr = (intptr_t)&(can_hal_stm[type].internal.tx_sender);

  if (can_hal_stm[type].internal.tx) {
    FrameworkunifiedStopChildThread(h_app, can_hal_stm[type].internal.tx, sizeof(ptr), &ptr);
    FrameworkunifiedDestroyChildThread(h_app, can_hal_stm[type].internal.tx);
    can_hal_stm[type].internal.tx = NULL;
  }

  if (can_hal_stm[type].internal.rx_initialized) {
    pthread_cancel(can_hal_stm[type].internal.rx);
    pthread_join(can_hal_stm[type].internal.rx, NULL);
    can_hal_stm[type].internal.rx_initialized = false;
  }
  can_hal_stm[type].h_app = NULL;
  return CANHAL_RET_NORMAL;
}

CANHAL_RET_API CanHalCreateInternalThread(HANDLE h_app, enum CanHalType type) {
  enum CANHAL_RET_API ret = CANHAL_RET_ERR_PARAM;
  EFrameworkunifiedStatus err = eFrameworkunifiedStatusOK;
  intptr_t ptr = (intptr_t)&(can_hal_stm[type].internal.tx_sender);

  can_hal_stm[type].h_app = h_app;
  can_hal_stm[type].internal.tx_name = CANHAL_CAN_SEND_THREAD;
  can_hal_stm[type].internal.tx = FrameworkunifiedCreateChildThread(h_app,
                can_hal_stm[type].internal.tx_name, 
                CanSendThreadStart, CanSendThreadStop);
  if (!can_hal_stm[type].internal.tx)
    goto cleanup;

  err = FrameworkunifiedStartChildThread(h_app,
                  can_hal_stm[type].internal.tx, 
                  sizeof(ptr), &ptr);
  if (err != eFrameworkunifiedStatusOK)
    goto cleanup;

  if (0 != pthread_attr_init(&(can_hal_stm[type].internal.rx_attr)))
    goto cleanup;

  if (0 != pthread_create(&(can_hal_stm[type].internal.rx),
                            &(can_hal_stm[type].internal.rx_attr), CanRecvRun,
                            (void *)&(can_hal_stm[type].type)))
    goto cleanup;

  can_hal_stm[type].internal.rx_initialized = true;
  ret = CANHAL_RET_NORMAL;
  return ret;
cleanup:
  if (can_hal_stm[type].internal.tx) {
    FrameworkunifiedStopChildThread(h_app, can_hal_stm[type].internal.tx, sizeof(ptr), &ptr);
    FrameworkunifiedDestroyChildThread(h_app, can_hal_stm[type].internal.tx);
    can_hal_stm[type].internal.tx = NULL;
  }

  return ret;
}

CANHAL_RET_API CanHalInternalSend_CWORD118_(enum CanHalType type,
                                     const void *msg, ssize_t sz) {
  EFrameworkunifiedStatus e_status = FrameworkunifiedSendMsg(can_hal_stm[type].internal.tx_sender,
                              TX_INTERNAL__CWORD118_, sz, msg);
  if (e_status != eFrameworkunifiedStatusOK) {
    return CANHAL_RET_ERR_ERR;
  }
  return CANHAL_RET_NORMAL;
}

static EFrameworkunifiedStatus FrameworkunifiedSendMsgOneshot(HANDLE h_app, UI_32 cmd,
                                    UI_32 l, PCVOID d) {
  HANDLE h_client = NULL;
  EFrameworkunifiedStatus err = eFrameworkunifiedStatusFail;

  h_client = FrameworkunifiedMcOpenSender(h_app, FrameworkunifiedGetAppName(h_app));
  if (!h_client)
    return err;

  err = FrameworkunifiedSendMsg(h_client, cmd, l, d);
  FrameworkunifiedMcClose(h_client);
  return err;
}

CANHAL_RET_API CanHalInternalSend(enum CanHalType type,
                                  const void *msg, ssize_t sz) {
  EFrameworkunifiedStatus e_status = FrameworkunifiedSendMsg(can_hal_stm[type].internal.tx_sender,
                              TX_INTERNAL, sz, msg);
  if (e_status != eFrameworkunifiedStatusOK) {
    return CANHAL_RET_ERR_ERR;
  }
  return CANHAL_RET_NORMAL;
}


CANHAL_RET_API InvokeStateCallback(enum CanHalType type) {
  HANDLE sender = can_hal_stm[type].h_app;
  uint32_t cmd = 0;
  EFrameworkunifiedStatus err = eFrameworkunifiedStatusOK;
  bool send = true;

  switch (type) {
  case CAN_HAL_TYPE_CAN:
    cmd = CID_CANHAL_CMD_CAN_READY;
    break;
  default:
    assert(0);
    break;
  }

  err = FrameworkunifiedSendMsgOneshot(sender, cmd, sizeof(send), &send);
  if (err != eFrameworkunifiedStatusOK)
    return CANHAL_RET_ERR_ERR;
  return CANHAL_RET_NORMAL;
}

CANHAL_RET_API InvokeErrorCallback(HANDLE h_app, enum CanHalType type) {
  HANDLE sender = h_app;
  uint32_t cmd = CID_CANHAL_CMD_ERROR_NOTIFY;
  char msg[CANHAL_ERROR_MESSAGE_LEN] = {0};
  ssize_t sz = sizeof(msg);
  EFrameworkunifiedStatus err = eFrameworkunifiedStatusOK;

  switch (type) {
  case CAN_HAL_TYPE_CAN:
    sprintf(msg, "Global CAN Stop");
    break;
  default:
    assert(0);
    break;
  }

  err = FrameworkunifiedSendMsgOneshot(sender, cmd, sz, msg);
  if (err != eFrameworkunifiedStatusOK)
    return CANHAL_RET_ERR_ERR;
  return CANHAL_RET_NORMAL;
}

CANHAL_RET_API CanHalSendStatus(enum CanHalType type, HANDLE h_app,
                                     const void *msg, ssize_t sz) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  e_status = FrameworkunifiedSendMsgOneshot(h_app, CID_CANHAL_CMD_CAN_SEND_STATUS, sz, msg);
  if (e_status != eFrameworkunifiedStatusOK) {
    return CANHAL_RET_ERR_ERR;
  }
  return CANHAL_RET_NORMAL;
}

CANHAL_RET_API CanHalReceiveNotify(enum CanHalType type,
                                     const void *msg, ssize_t sz) {
  EFrameworkunifiedStatus e_status;
  uint32_t cmd = 0;

  switch (type) {
  case CAN_HAL_TYPE_CAN:
    cmd = CID_CANHAL_CMD_CAN_RECV;
    break;
  default:
    assert(0);
    break;
  }

  e_status = FrameworkunifiedSendMsgOneshot(can_hal_stm[type].h_app, cmd, sz, msg);
  if (e_status != eFrameworkunifiedStatusOK) {
    return CANHAL_RET_ERR_ERR;
  }

  return CANHAL_RET_NORMAL;
}
