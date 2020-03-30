/*
 * @copyright Copyright (c) 2016-2019 TOYOTA MOTOR CORPORATION.
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

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_TE_Calculator
/// \brief
///
///
///
///
///
///////////////////////////////////////////////////////////////////////////////

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
#include "communication_cid.h"
#include "communication_communicationlog.h"
#include "ICR_Common.h"
#include "Thread_Common.h"

#define NTFY_CommSH4A_Availability "CommSH4A/Availability"
#define TIMER_SEM_NAME "TIMER_SEM"
#define INT_SEM_NAME "INT_SEM"
#define SENDRECV_SEM_NAME "SENDRECV_SEM"

/*------------------------------------------------------------------------------
 * function declaration
 *----------------------------------------------------------------------------*/
EFrameworkunifiedStatus CommSysCanReady(HANDLE h_app);
EFrameworkunifiedStatus CallbackCommSH4AAvailability(HANDLE h_app);
EFrameworkunifiedStatus CANThreadStart(HANDLE h_app);
EFrameworkunifiedStatus CANThreadStop(HANDLE h_app);

/*------------------------------------------------------------------------------
 * static variables
 *----------------------------------------------------------------------------*/
HANDLE g_can_thread = NULL;

BOOL g_can_ready = FALSE;
BOOL g_commsh4a_ready = FALSE;

BOOL g_isaccoff = FALSE;  // CAN_TxMsg.cpp needs this

static const FrameworkunifiedProtocolCallbackHandler kCommPcbhs[] = {  // LCOV_EXCL_BR_LINE 11: unexpected branch
    {CID_COMMSYS_CAN_READY, CommSysCanReady},
};

static const FrameworkunifiedNotificationCallbackHandler kCommsysNotificationHandlers[] = {  // LCOV_EXCL_BR_LINE 11: unexpected branch  // NOLINT (whitespace/line_length)
    {NTFY_CommSH4A_Availability, CallbackCommSH4AAvailability},
};

EFrameworkunifiedStatus CommSysReadyCheck(HANDLE h_app) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  e_status = FrameworkunifiedPublishServiceAvailability(h_app, TRUE);
  if (eFrameworkunifiedStatusOK != e_status) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__,
              "Failed to Publish Service Availability Notification.");
  }
  return e_status;
}

EFrameworkunifiedStatus CallbackCommSH4AAvailability(HANDLE h_app) {
  EFrameworkunifiedStatus e_status;
  ServiceAvailability tServiceAvailability;

  // Read the data from the message
  e_status = FrameworkunifiedGetMsgDataOfSize(h_app, &tServiceAvailability, sizeof(tServiceAvailability), eSMRRelease);
  if (e_status != eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    if (e_status == eFrameworkunifiedStatusInvldBufSize) {
      EFrameworkunifiedStatus e_statusClear = FrameworkunifiedClearMsgData(h_app);
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "FrameworkunifiedClearMsgData (e_status = %d)", e_statusClear);
    }
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "FrameworkunifiedGetMsgDataOfSize Failed");
    return e_status;
  }
  // LCOV_EXCL_STOP 4: NSFW error case.

  // Service Availability
  if (eFrameworkunifiedServiceAvailable != tServiceAvailability.eServiceAvailability) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_DEBUG, __func__, "[%s] Availability is FALSE.", tServiceAvailability.cServiceName);  // LCOV_EXCL_LINE 4: NSFW error case.  // NOLINT (whitespace/line_length)
    return e_status;
  } else {
    g_commsh4a_ready = TRUE;
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __func__, "[%s] Availability is TRUE.", tServiceAvailability.cServiceName);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT (whitespace/line_length)
  }

  CommSysReadyCheck(h_app);
  return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus CommSysCanReady(HANDLE h_app) {
  g_can_ready = TRUE;
  CommSysReadyCheck(h_app);
  return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus FrameworkunifiedOnInitialization(HANDLE h_app) {
  // Return Status
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  /*UserCodeStartCommunicationOnInitialization*/

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

  // Attach callback api to dispatcher for to notify availability
  e_status = FrameworkunifiedAttachCallbacksToDispatcher(h_app, FRAMEWORKUNIFIED_ANY_SOURCE,
                                            kCommPcbhs, _countof(kCommPcbhs));
  if (e_status != eFrameworkunifiedStatusOK) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__,
      "FrameworkunifiedAttachCallbacksToDispatcher Error(%d)", e_status);
    exit(EXIT_FAILURE);
  }

  // FIXME : Following code is really needed?
  // Subscribe to service availability
  e_status = FrameworkunifiedSubscribeNotificationsWithCallback(h_app, 
          kCommsysNotificationHandlers, _countof(kCommsysNotificationHandlers));
  if (eFrameworkunifiedStatusOK != e_status) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "Failed to FrameworkunifiedSubscribeNotificationsWithCallback.");
    exit(EXIT_FAILURE);
  }

  g_can_thread = FrameworkunifiedCreateChildThread(h_app, LAN_SERVICE_CAN,
                                        CANThreadStart, CANThreadStop);
  if (g_can_thread == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Fail to create CAN");
  }

  /*UserCodeEndCommunicationOnInitialization*/
  return e_status;
}

// LCOV_EXCL_START 8: (dead code there is no function in wakeup)
EFrameworkunifiedStatus FrameworkunifiedOnWakeup(HANDLE h_app) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  return e_status;
}
// LCOV_EXCL_STOP 8: (dead code there is no function in wakeup)

// LCOV_EXCL_START 14 Resident process, not called by NSFW
EFrameworkunifiedStatus FrameworkunifiedOnShutdown(HANDLE h_app) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  return e_status;
}
// LCOV_EXCL_STOP 14 Resident process, not called by NSFW

// LCOV_EXCL_START 14 Resident process, not called by NSFW
EFrameworkunifiedStatus FrameworkunifiedOnEShutdown(HANDLE h_app) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  return e_status;
}
// LCOV_EXCL_STOP 14 Resident process, not called by NSFW

// LCOV_EXCL_START 14 Resident process, not called by NSFW
EFrameworkunifiedStatus FrameworkunifiedOnDestroy(HANDLE h_app) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  return e_status;
}
// LCOV_EXCL_STOP 14 Resident process, not called by NSFW

// LCOV_EXCL_START 7: Debug Code
EFrameworkunifiedStatus FrameworkunifiedOnDebugDump(HANDLE h_app) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  return e_status;
}
// LCOV_EXCL_STOP 7: Debug Code

// LCOV_EXCL_START 8:(dead code do not use StateMachine)
EFrameworkunifiedStatus FrameworkunifiedCreateStateMachine(HANDLE h_app) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  return e_status;
}
// LCOV_EXCL_STOP 8:(dead code do not use StateMachine)

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

  e_status = FrameworkunifiedPublishServiceAvailability(h_app, FALSE);
  if (eFrameworkunifiedStatusOK != e_status) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
          "Failed to Publish Service Availability Notification.");
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__,
          "Service Availability Notification published successfully.");
  }

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
  g_isaccoff = TRUE;

  e_status = CommThreadCheckAndStop(h_app, g_can_thread,
                         0, NULL, CommGetAvailabilityCurrent(CAN_AVAILABILITY));
  if (eFrameworkunifiedStatusOK != e_status) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
          "Fail to Stop CAN Thread. Status:%#x", e_status);
  } else {
    g_can_thread = NULL;
  }

  return e_status;
}  // LCOV_EXCL_LINE 10: final line

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
}  // LCOV_EXCL_BR_LINE 10:Gcov constraints (last row)
