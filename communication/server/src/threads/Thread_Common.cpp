/*
 * Copyright (c) 2019-2020 TOYOTA MOTOR CORPORATION
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

#include <stdint.h>
#include <boost/atomic.hpp>
#include <pthread.h>

#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_timer.h>
#include <agl_thread.h>
#include <can_hal.h>
#include "communication_communicationlog.h"
#include "communication_cid.h"
#include "Thread_Common.h"

static std::map<std::string, HANDLE> g_map_sender;
static pthread_mutex_t g_sender_mutex = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;
static boost::atomic<int32_t> availabilityCurrent;

void CommonInit(void) {
  pthread_mutex_lock(&g_sender_mutex);
  g_map_sender.clear();
  pthread_mutex_unlock(&g_sender_mutex);
}

HANDLE CommonFindSender(HANDLE h_app, std::string s) {
  HANDLE ret = NULL;
  std::string app(FrameworkunifiedGetAppName(h_app));
  std::string key = s + app;
  std::map<std::string, HANDLE>::iterator it = g_map_sender.find(key);

  pthread_mutex_lock(&g_sender_mutex);

  if (it != g_map_sender.end()) {
    ret = g_map_sender[key];
    goto cleanup;
  }

  ret = FrameworkunifiedMcOpenSender(h_app, s.c_str());
  if (ret) {
    g_map_sender[key] = ret;
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "FrameworkunifiedMcOpenSender Error(to:%s)", s.c_str());
  }

cleanup:
  pthread_mutex_unlock(&g_sender_mutex);
  return ret;
}

void CommSetAvailabilityCurrent(int32_t current) {
  availabilityCurrent |= current;
}

void CommClrAvailabilityCurrent(int32_t current) {
  availabilityCurrent &= (~current);
}

BOOL CommGetAvailabilityCurrent(int32_t current) {
  return !(!(availabilityCurrent & current));
}

EFrameworkunifiedStatus CommonStartNotify(HANDLE h_app, PCSTR cmd) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  const char *result = "Unknown";
  const char *called = "Unknown";
  const BOOL avail = TRUE;
  UI_16 z = ZONE_ERR;

  e_status = FrameworkunifiedNPPublishNotification(h_app, cmd, &avail , sizeof(avail));
  result = (e_status != eFrameworkunifiedStatusOK) ? "failed" : "success";
  z = (e_status != eFrameworkunifiedStatusOK) ? ZONE_INFO : ZONE_ERR;
  called = "FrameworkunifiedNPPublishNotification";
  FRAMEWORKUNIFIEDLOG(z, __func__, "%s(%s) %s(%d)", called, cmd, result, e_status);
  return e_status;
}

EFrameworkunifiedStatus CommonCanHalErrorNotify(HANDLE h_app) {
  char msg[CANHAL_ERROR_MESSAGE_LEN] = {0};
  EFrameworkunifiedStatus err = eFrameworkunifiedStatusOK;

  err = FrameworkunifiedGetMsgDataOfSize(h_app, &msg, sizeof(msg), eSMRRelease);
  if (err != eFrameworkunifiedStatusOK) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedGetMsgDataOfSize is failed, err=%d", err);
  }

  if (err == eFrameworkunifiedStatusInvldBufSize) {
    FrameworkunifiedClearMsgData(h_app);
    return eFrameworkunifiedStatusFail;
  } else if (err != eFrameworkunifiedStatusOK) {
    return eFrameworkunifiedStatusFail;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "can_hal error : %s", msg);
  return err;
}

EFrameworkunifiedStatus CommonThreadStart(HANDLE h_app, const FrameworkunifiedProtocolCallbackHandler *cb,
                      UI_32 count, PCSTR cmd, EFrameworkunifiedStatus (*open_func)(HANDLE)) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  const char *result = "Unknown";
  const char *called = "Unknown";
  UI_16 z = ZONE_ERR;

  e_status = FrameworkunifiedAttachCallbacksToDispatcher(h_app, FRAMEWORKUNIFIED_ANY_SOURCE, cb, count);
  result = (e_status != eFrameworkunifiedStatusOK) ? "failed" : "success";
  z = (e_status != eFrameworkunifiedStatusOK) ? ZONE_INFO : ZONE_ERR;
  called = "FrameworkunifiedAttachCallbacksFromDispatcher";
  FRAMEWORKUNIFIEDLOG(z, __func__, "%s %s(%d)", called, result, e_status);
  if (e_status != eFrameworkunifiedStatusOK)
    return e_status;

  e_status = open_func(h_app);
  if (e_status != eFrameworkunifiedStatusOK)
    return e_status;

  e_status = FrameworkunifiedNPRegisterNotification(h_app, cmd,
                                       sizeof(EFrameworkunifiedStatus), eFrameworkunifiedStateVar);
  result = (e_status != eFrameworkunifiedStatusOK) ? "failed" : "success";
  z = (e_status != eFrameworkunifiedStatusOK) ? ZONE_INFO : ZONE_ERR;
  called = "FrameworkunifiedNPRegisterNotification";
  FRAMEWORKUNIFIEDLOG(z, __func__, "%s(%s) %s(%d)", called, cmd, result, e_status);
  if (e_status != eFrameworkunifiedStatusOK)
    return e_status;

  return e_status;
}

EFrameworkunifiedStatus CommonThreadStop(HANDLE h_app, const PUI_32 cb,
                     UI_32 count, PCSTR cmd, EFrameworkunifiedStatus (*close_func)(HANDLE)) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  const BOOL avail = FALSE;
  const char *result = "Unknown";
  const char *called = "Unknown";
  UI_16 z = ZONE_ERR;

  e_status = FrameworkunifiedNPPublishNotification(h_app, cmd, &avail , sizeof(avail));
  result = (e_status != eFrameworkunifiedStatusOK) ? "failed" : "success";
  z = (e_status != eFrameworkunifiedStatusOK) ? ZONE_INFO : ZONE_ERR;
  called = "FrameworkunifiedNPPublishNotification";
  FRAMEWORKUNIFIEDLOG(z, __func__, "%s(%s) %s(%d)", called, cmd, result, e_status);
  if (e_status != eFrameworkunifiedStatusOK)
    return e_status;

  e_status = FrameworkunifiedNPUnRegisterNotification(h_app, cmd);
  result = (e_status != eFrameworkunifiedStatusOK) ? "failed" : "success";
  z = (e_status != eFrameworkunifiedStatusOK) ? ZONE_INFO : ZONE_ERR;
  called = "FrameworkunifiedNPUnRegisterNotification";
  FRAMEWORKUNIFIEDLOG(z, __func__, "%s(%s) %s(%d)", called, cmd, result, e_status);
  if (e_status != eFrameworkunifiedStatusOK)
    return e_status;

  e_status = FrameworkunifiedDetachCallbacksFromDispatcher(h_app, FRAMEWORKUNIFIED_ANY_SOURCE, cb, count);
  result = (e_status != eFrameworkunifiedStatusOK) ? "failed" : "success";
  z = (e_status != eFrameworkunifiedStatusOK) ? ZONE_INFO : ZONE_ERR;
  called = "FrameworkunifiedDetachCallbacksFromDispatcher";
  FRAMEWORKUNIFIEDLOG(z, __func__, "%s %s(%d)", called, result, e_status);
  if (e_status != eFrameworkunifiedStatusOK)
    return e_status;

  e_status = close_func(h_app);
  if (e_status != eFrameworkunifiedStatusOK){
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: close_func() failed");
    return e_status;
  }

  return e_status;
}

