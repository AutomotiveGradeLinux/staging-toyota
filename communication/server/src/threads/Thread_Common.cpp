/*
 * Copyright (c) 2019 TOYOTA MOTOR CORPORATION
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
#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_timer.h>
#include <agl_thread.h>
//#include <can_hal.h>
#include "communication_communicationlog.h"
#include "communication_cid.h"
#include "Thread_Common.h"

static boost::atomic<int32_t> availabilityMask;
static boost::atomic<int32_t> availabilityCurrent;

void CommSetAvailabilityMask(int32_t mask) {
  availabilityMask |= mask;
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

static BOOL CommCheckAvailabilityMask(void) {
  return (availabilityMask == availabilityCurrent);
}

EFrameworkunifiedStatus CommonStartNotify(HANDLE h_app, PCSTR cmd) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  const char *result = "Unknown";
  const char *called = "Unknown";
  const BOOL avail = TRUE;
  UI_16 z = ZONE_ERR;
  HANDLE hdl = NULL;
  uint8_t c = 0;

  hdl = FrameworkunifiedMcOpenSender(h_app, LAN_SERVICE_MAIN);
  result = (NULL == hdl) ? "failed" : "success";
  z = (NULL == hdl) ? ZONE_INFO : ZONE_ERR;
  called = "FrameworkunifiedMcOpenSender";
  FRAMEWORKUNIFIEDLOG(z, __func__, "%s %s", called, result);
  if (e_status != eFrameworkunifiedStatusOK)
    return e_status;

  if (TRUE == CommCheckAvailabilityMask()) {
    e_status = FrameworkunifiedSendMsg(hdl, CID_COMMSYS_CAN_READY, c, &c);
    result = (e_status != eFrameworkunifiedStatusOK) ? "failed" : "success";
    z = (e_status != eFrameworkunifiedStatusOK) ? ZONE_INFO : ZONE_ERR;
    called = "FrameworkunifiedSendMsg";
    FRAMEWORKUNIFIEDLOG(z, __func__, "%s %s(%d)", called, result, e_status);
  }
  FrameworkunifiedMcClose(hdl);

  e_status = FrameworkunifiedNPPublishNotification(h_app, cmd, &avail , sizeof(avail));
  result = (e_status != eFrameworkunifiedStatusOK) ? "failed" : "success";
  z = (e_status != eFrameworkunifiedStatusOK) ? ZONE_INFO : ZONE_ERR;
  called = "FrameworkunifiedNPPublishNotification";
  FRAMEWORKUNIFIEDLOG(z, __func__, "%s(%s) %s(%d)", called, cmd, result, e_status);
  return e_status;
}

EFrameworkunifiedStatus CommonCanHalErrorNotify(const char *msg, void *ctxid) {
  FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "can_hal error : %s", msg);
  return eFrameworkunifiedStatusOK;
}

//EFrameworkunifiedStatus CommonCanHalThreadStart(HANDLE h_app,
//                                   const FrameworkunifiedProtocolCallbackHandler *cb,
//                                   UI_32 count, PCSTR cmd, CanHalParam *p) {
//  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
//  CANHAL_RET_API ret = CANHAL_RET_NORMAL;
//  const char *result = "Unknown";
//  const char *called = "Unknown";
//  UI_16 z = ZONE_ERR;
//
//  e_status = FrameworkunifiedAttachCallbacksToDispatcher(h_app, FRAMEWORKUNIFIED_ANY_SOURCE, cb, count);
//  result = (e_status != eFrameworkunifiedStatusOK) ? "failed" : "success";
//  z = (e_status != eFrameworkunifiedStatusOK) ? ZONE_INFO : ZONE_ERR;
//  called = "CommunicationAttachCallbacksFromDispatcher";
//  FRAMEWORKUNIFIEDLOG(z, __func__, "%s %s(%d)", called, result, e_status);
//  if (e_status != eFrameworkunifiedStatusOK)
//    return e_status;
//
////  ret = CanOpen(p);
//  result = (ret != CANHAL_RET_NORMAL) ? "failed" : "success";
//  z = (ret != CANHAL_RET_NORMAL) ? ZONE_INFO : ZONE_ERR;
//  called = "CanOpen";
//  FRAMEWORKUNIFIEDLOG(z, __func__, "%s %s(%d)", called, result, ret);
//  if (e_status != eFrameworkunifiedStatusOK)
//    return e_status;
//
//  e_status = FrameworkunifiedNPRegisterNotification(h_app, cmd,
//                                       sizeof(EFrameworkunifiedStatus), eFrameworkunifiedStateVar);
//  result = (e_status != eFrameworkunifiedStatusOK) ? "failed" : "success";
//  z = (e_status != eFrameworkunifiedStatusOK) ? ZONE_INFO : ZONE_ERR;
//  called = "FrameworkunifiedNPRegisterNotification";
//  FRAMEWORKUNIFIEDLOG(z, __func__, "%s(%s) %s(%d)", called, cmd, result, e_status);
//  if (e_status != eFrameworkunifiedStatusOK)
//    return e_status;
//
//  return e_status;
//}

//EFrameworkunifiedStatus CommonCanHalThreadStop(HANDLE h_app, const PUI_32 cb,
//                                   UI_32 count, PCSTR cmd, CanHalType type) {
//  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
//  CANHAL_RET_API ret = CANHAL_RET_NORMAL;
//  const BOOL avail = FALSE;
//  const char *result = "Unknown";
//  const char *called = "Unknown";
//  UI_16 z = ZONE_ERR;
//
//  e_status = FrameworkunifiedNPPublishNotification(h_app, cmd, &avail , sizeof(avail));
//  result = (e_status != eFrameworkunifiedStatusOK) ? "failed" : "success";
//  z = (e_status != eFrameworkunifiedStatusOK) ? ZONE_INFO : ZONE_ERR;
//  called = "FrameworkunifiedNPPublishNotification";
//  FRAMEWORKUNIFIEDLOG(z, __func__, "%s(%s) %s(%d)", called, cmd, result, e_status);
//  if (e_status != eFrameworkunifiedStatusOK)
//    return e_status;
//
//  e_status = FrameworkunifiedNPUnRegisterNotification(h_app, cmd);
//  result = (e_status != eFrameworkunifiedStatusOK) ? "failed" : "success";
//  z = (e_status != eFrameworkunifiedStatusOK) ? ZONE_INFO : ZONE_ERR;
//  called = "FrameworkunifiedNPUnRegisterNotification";
//  FRAMEWORKUNIFIEDLOG(z, __func__, "%s(%s) %s(%d)", called, cmd, result, e_status);
//  if (e_status != eFrameworkunifiedStatusOK)
//    return e_status;
//
//  e_status = FrameworkunifiedDetachCallbacksFromDispatcher(h_app, FRAMEWORKUNIFIED_ANY_SOURCE, cb, count);
//  result = (e_status != eFrameworkunifiedStatusOK) ? "failed" : "success";
//  z = (e_status != eFrameworkunifiedStatusOK) ? ZONE_INFO : ZONE_ERR;
//  called = "FrameworkunifiedDetachCallbacksFromDispatcher";
//  FRAMEWORKUNIFIEDLOG(z, __func__, "%s %s(%d)", called, result, e_status);
//  if (e_status != eFrameworkunifiedStatusOK)
//    return e_status;
//
////  ret = CanClose(type);
//  result = (ret != CANHAL_RET_NORMAL) ? "failed" : "success";
//  z = (ret != CANHAL_RET_NORMAL) ? ZONE_INFO : ZONE_ERR;
//  called = "CanClose";
//  FRAMEWORKUNIFIEDLOG(z, __func__, "%s %s(%d)", called, result, e_status);
//  if (e_status != eFrameworkunifiedStatusOK)
//    return e_status;
//
//  return e_status;
//}

