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

/*
 * This file has been generated automatically.
 * User hand written code entry is allowed only inside protected zones.
 */

/*
 * This file has been generated automatically.
 * User hand written code entry is allowed only inside protected zones.
 */

#include <native_service/frameworkunified_application.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_service_protocol.h>
#include <native_service/frameworkunified_multithreading.h>
#include <peripheral_service/communication_notifications.h>
#include <peripheral_service/Canif_API.h>
#include <stdio.h>
#include <signal.h>
#include <string>
#include <sstream>
#include <iostream>
#include "communication_cid.h"
#include "communication_communicationlog.h"
#include "Thread_Common.h"

extern EFrameworkunifiedStatus CANThreadStart(HANDLE h_app);
extern EFrameworkunifiedStatus CANThreadStop(HANDLE h_app);

HANDLE g_can_thread = NULL;

EFrameworkunifiedStatus FrameworkunifiedOnInitialization(HANDLE h_app) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;

  CommonInit();

  // API to Register Notification for Service Availability.
  e_status = FrameworkunifiedRegisterServiceAvailabilityNotification(h_app, 
                                              NTFY_Communication_Availability);
  if (eFrameworkunifiedStatusOK != e_status) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
          "Failed to Register Service Availability Notification.");
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, 
          "Service Availability Notification registered successfully.");
  }

  // API to Publish Service Availability Notification.
  e_status = FrameworkunifiedPublishServiceAvailability(h_app, FALSE);
  if (eFrameworkunifiedStatusOK != e_status) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
          "Failed to Publish Service Availability Notification.");
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, 
          "Service Availability Notification published successfully.");
  }

  g_can_thread = FrameworkunifiedCreateChildThread(h_app, LAN_SERVICE_CAN,
                                        CANThreadStart, CANThreadStop);
  if (g_can_thread == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Fail to create CAN");
  }

  // API to Publish Service Availability Notification.
  e_status = FrameworkunifiedPublishServiceAvailability(h_app, TRUE);
  if (eFrameworkunifiedStatusOK != e_status) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
          "Failed to Publish Service Availability Notification.");
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__,
          "Service Availability Notification published successfully.");
  }

  return e_status;
}

EFrameworkunifiedStatus FrameworkunifiedOnWakeup(HANDLE h_app) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  return e_status;
}

EFrameworkunifiedStatus FrameworkunifiedOnShutdown(HANDLE h_app) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  return e_status;
}

EFrameworkunifiedStatus FrameworkunifiedOnEShutdown(HANDLE h_app) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  return e_status;
}

EFrameworkunifiedStatus FrameworkunifiedOnDestroy(HANDLE h_app) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  return e_status;
}

EFrameworkunifiedStatus FrameworkunifiedOnDebugDump(HANDLE h_app) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  return e_status;
}

EFrameworkunifiedStatus FrameworkunifiedCreateStateMachine(HANDLE h_app) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  return e_status;
}

static EFrameworkunifiedStatus CommThreadCheckAndStart(HANDLE p, HANDLE c,
                  UI_32 len, PCVOID data, BOOL current) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  if (TRUE != current) {
    e_status = FrameworkunifiedStartChildThread(p, c, len, data);
  }
  return e_status;
}

static EFrameworkunifiedStatus CommThreadCheckAndStop(HANDLE p, HANDLE c,
                  UI_32 len, PCVOID data, BOOL current) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  if (TRUE == current) {
    e_status = FrameworkunifiedStopChildThread(p, c, len, data);
  }
  return e_status;
}

EFrameworkunifiedStatus FrameworkunifiedOnStart(HANDLE h_app) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "[START]");

  e_status = CommThreadCheckAndStart(h_app, g_can_thread,
                         0, NULL, CommGetAvailabilityCurrent(CAN_AVAILABILITY));
  if (eFrameworkunifiedStatusOK != e_status) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
          "Fail to Start CAN Thread. Status:%#x", e_status);
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "[END]");
  return e_status;
}

EFrameworkunifiedStatus FrameworkunifiedOnStop(HANDLE h_app) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;

  e_status = CommThreadCheckAndStop(h_app, g_can_thread,
                         0, NULL, CommGetAvailabilityCurrent(CAN_AVAILABILITY));
  if (eFrameworkunifiedStatusOK != e_status) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
          "Fail to Stop CAN Thread. Status:%#x", e_status);
  } else {
    g_can_thread = NULL;
  }

  return e_status;
}

EFrameworkunifiedStatus FrameworkunifiedOnPreStart(HANDLE h_app) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "[START]");

  e_status = CommThreadCheckAndStart(h_app, g_can_thread,
                         0, NULL, CommGetAvailabilityCurrent(CAN_AVAILABILITY));
  if (eFrameworkunifiedStatusOK != e_status) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
          "Fail to Start CAN Thread. Status:%#x", e_status);
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "[END]");
  return e_status;
}

EFrameworkunifiedStatus FrameworkunifiedOnBackgroundStart(HANDLE h_app) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "[START]");

  e_status = CommThreadCheckAndStart(h_app, g_can_thread,
                        0, NULL, CommGetAvailabilityCurrent(CAN_AVAILABILITY));
  if (eFrameworkunifiedStatusOK != e_status) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "Fail to Start CAN Thread. Status:%#x", e_status);
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "[END]");
  return e_status;
}

EFrameworkunifiedStatus FrameworkunifiedOnPreStop(HANDLE h_app) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "[START]");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "[END]");
  return e_status;
}

EFrameworkunifiedStatus FrameworkunifiedOnBackgroundStop(HANDLE h_app) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "[START]");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "[END]");
  return e_status;
}

std::string MessageDataOutputLog(uint8_t *msg_data, uint32_t len){
  std::string data_log;
  int data_buf;
  for(uint32_t i = 0; i < len; i++){
    std::ostringstream ss;
    data_buf = (int)msg_data[i];
    ss << std::hex << data_buf;
    data_log = ss.str() + " " + data_log;
  }
  return data_log;
}
