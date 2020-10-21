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

/*
 * Information.
 * This source code is a sample source code .
 * Implementation of the function must be performed by the vendor.
 */

#include "can_hal_core.h"
#include "can_hal_stm.h"
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>

CANHAL_RET_API CanOpen(HANDLE h_app, CanHalType type) {
  CANHAL_RET_API ret = CANHAL_RET_ERR_ERR;

  if (!TypeIsValid(type)) {
    return CANHAL_RET_ERR_PARAM;
  }

  if (!h_app) {
    return CANHAL_RET_ERR_PARAM;
  }

  if (IsCanHalOpened(type)) {
    return CANHAL_RET_NORMAL;
  }

  if (IsDeviceEnabled(type)) {
    return CANHAL_RET_NORMAL;
  }

  ret = CanOpenCore(type);
  if (CANHAL_RET_NORMAL != ret) {
    return ret;
  }

  ret = CanHalCreateInternalThread(h_app, type);
  if (CANHAL_RET_NORMAL != ret) {
    CanCloseCore(type);
    return ret;
  }

  SetDeviceStateEnable(type);
  SetCanHalStateOpen(type);
  InvokeStateCallback(type);
  return ret;
}

CANHAL_RET_API CanClose(HANDLE h_app, CanHalType type) {
  CANHAL_RET_API ret = CANHAL_RET_ERR_ERR;
  if (!TypeIsValid(type)) {
    return CANHAL_RET_ERR_PARAM;
  }

  if (!h_app) {
    return CANHAL_RET_ERR_PARAM;
  }

  if (!IsCanHalOpened(type)) {
    return CANHAL_RET_NORMAL;
  }

  if (IsDeviceEnabled(type)) {
    ret = CanCloseCore(type);
    if (CANHAL_RET_NORMAL != ret)
      return ret;
  }
    
  ret = CanHalDestroyInternalThread(h_app, type);
  if (CANHAL_RET_NORMAL != ret)
    return ret;

  SetDeviceStateDisable(type);
  SetCanHalStateClose(type);
  InvokeErrorCallback(h_app, type);

  return ret;
}

static CANHAL_RET_API CanSendCommonCheck(const void *message, 
                                                        enum CanHalType type) {
  if (!TypeIsValid(type)) {
    return CANHAL_RET_ERR_PARAM;
  }

  if (!message) {
    return CANHAL_RET_ERR_PARAM;
  }

  if (!IsCanHalOpened(type)) {
    return CANHAL_RET_ERR_STATE;
  }

  if (!IsDeviceEnabled(type)) {
    return CANHAL_RET_ERR_STATE;
  }

  return CANHAL_RET_NORMAL;
}

CANHAL_RET_API CanSend(HANDLE h_app,
                             const CanMessage *message, enum CanHalType type) {
  CANHAL_RET_API ret = CANHAL_RET_ERR_ERR;
  void *_message = NULL;
  ssize_t sz = 0;

  ret = CanSendCommonCheck(message, type);
  if (ret != CANHAL_RET_NORMAL)
    return ret;

  if (!h_app) {
    return CANHAL_RET_ERR_PARAM;
  }

  if (CAN_NORMAL_MESSAGE_LEN < message->dlc) {
    return CANHAL_RET_ERR_PARAM;
  }

  _message = CanHalPackMessage(h_app, (const void *)message,
                               sizeof(CanMessage), &sz);
  if (!_message)
    return CANHAL_RET_ERR_ERR;

  ret = CanHalInternalSend(type, _message, sz);
  CanHalDestroyPackedMessage(_message);
  return ret;
}

CANHAL_RET_API CanGetVersion(HANDLE h_app, std::string *p_version) {
  CanHalType type = CAN_HAL_TYPE_CAN;
  if (!h_app) {
    return CANHAL_RET_ERR_PARAM;
  }

  if (!IsCanHalOpened(type)) {
    return CANHAL_RET_ERR_STATE;
  }

  if (!IsDeviceEnabled(type)) {
    return CANHAL_RET_ERR_STATE;
  }

  if (NULL == p_version) {
    return CANHAL_RET_ERR_PARAM;
  }

  *p_version = "FFFF";
  return CANHAL_RET_NORMAL;
}
