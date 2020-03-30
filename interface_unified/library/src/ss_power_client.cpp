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
/// \ingroup  tag_PowerServiceIf
/// \brief    This file supports Power Service client management.
///
///////////////////////////////////////////////////////////////////////////////

#include <inttypes.h>
#include <string.h>
#include <native_service/frameworkunified_application.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_framework_types.h>
#include <string>
#include "system_service/ss_power_service.h"
#include "system_service/ss_power_service_protocol.h"
#include "system_service/ss_power_service_notifications.h"
#include "system_service/ss_services.h"
#include "system_service/ss_system_if.h"
#include "system_service/ss_templates.h"
#include "ss_power_if_interfaceunifiedlog.h"

typedef struct {
  BOOL fAvailable;
  HANDLE hSession;
  HANDLE hService;
  std::string szServiceName;
} TPowerSession;

static EFrameworkunifiedStatus OnPowerCloseSesionAck(HANDLE hApp);
static EFrameworkunifiedStatus OnPowerOpenSessionAck(HANDLE hApp);
static EFrameworkunifiedStatus OnPowerAvailability(HANDLE hApp);
static EFrameworkunifiedStatus OnPowerAvailable(HANDLE hApp);
static EFrameworkunifiedStatus OnPowerUnavailable(HANDLE hApp);
static TPowerSession g_tPowerSession = { };

/// Base on the a user group affiliation we will call there start and stop methods
/// Note: there group and subgroup strings are empty we will call start on the every
/// first case of
EFrameworkunifiedStatus InterfaceunifiedSystemConnectToPowerSerivce(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG0(ZONE_FUNC, __FUNCTION__, "+");

  // Subscriptions
  FrameworkunifiedNotificationCallbackHandler g_aryPower_Notif_Cbs[] = {
      // Notifications name, Call back function
      { szNTFY_PowerAvailability, OnPowerAvailability }, };

  /// Avoid opening a session to the Power
  /// service Since it uses the framework as well
  if (0 != strcmp(SERVICE_POWER, FrameworkunifiedGetAppName(hApp))) {
    // Subscribe and attach call backs to notifications, to Notification Service!
    if (eFrameworkunifiedStatusOK
        != (eStatus = FrameworkunifiedSubscribeNotificationsWithCallback(
            hApp, g_aryPower_Notif_Cbs, _countof(g_aryPower_Notif_Cbs)))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "FrameworkunifiedAttachNotificationCallbacksToDispatcher Failed Status:0x%x ",
             eStatus);
    } else {
      /// Need to save the Group and Subgroup values
      g_tPowerSession.szServiceName = FrameworkunifiedGetAppName(hApp);

      /// Cleanup any other data members
      g_tPowerSession.fAvailable = FALSE;
      g_tPowerSession.hService = NULL;
      g_tPowerSession.hSession = NULL;
    }
  }

  FRAMEWORKUNIFIEDLOG0(ZONE_FUNC, __FUNCTION__, "-");

  return eStatus;
}

EFrameworkunifiedStatus OnPowerAvailability(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusFail;
  FRAMEWORKUNIFIEDLOG0(ZONE_FUNC, __FUNCTION__, "+");

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
         "Power Service Availability notification received ****");

  if (sizeof(BOOL) == FrameworkunifiedGetMsgLength(hApp)) {
    BOOL avalibility = FALSE;
    if (eFrameworkunifiedStatusOK
        == (eStatus = FrameworkunifiedGetMsgDataOfSize(hApp, &avalibility, sizeof(BOOL)))) {
      if (TRUE == avalibility) {
        eStatus = OnPowerAvailable(hApp);
      } else {
        eStatus = OnPowerUnavailable(hApp);
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "FrameworkunifiedAttachNotificationCallbacksToDispatcher Failed Status:0x%x ",
             eStatus);
    }
  }

  FRAMEWORKUNIFIEDLOG0(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus OnPowerAvailable(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusFail;

  FRAMEWORKUNIFIEDLOG0(ZONE_FUNC, __FUNCTION__, "+");

  // if the service is available before the close happens then close the session and service first.
  if (NULL != g_tPowerSession.hService && NULL != g_tPowerSession.hSession) {
    EFrameworkunifiedStatus interfaceunifiedRet = FrameworkunifiedCloseSession(g_tPowerSession.hService,
                                        g_tPowerSession.hSession);
    SS_ASERT(interfaceunifiedRet == eFrameworkunifiedStatusOK);
    g_tPowerSession.hSession = NULL;
    g_tPowerSession.hService = NULL;
  }

  PCSTR power_name = FrameworkunifiedGetMsgSrc(hApp);

  // opening the service
  if (NULL == (g_tPowerSession.hService = FrameworkunifiedOpenService(hApp, power_name))) {
    FRAMEWORKUNIFIEDLOG0(ZONE_ERR, __FUNCTION__, "Power: Failed to open service");
  } else {
    FrameworkunifiedProtocolCallbackHandler g_aryPower_Protocol_Cbs[] = {
    // Command ID,  Call back functions
        { PROTOCOL_OPEN_SESSION_ACK, OnPowerOpenSessionAck },
        { PROTOCOL_CLOSE_SESSION_ACK, OnPowerCloseSesionAck }, };

    /// Attach the valid callback for this service
    if (eFrameworkunifiedStatusOK
        == (eStatus = FrameworkunifiedAttachCallbacksToDispatcher(
            hApp, power_name, g_aryPower_Protocol_Cbs,
            _countof(g_aryPower_Protocol_Cbs)))) {
      EPWR_SESSION_TYPE tOpenSessionReq = epsstBASIC;
      /// opening the session with service
      if (eFrameworkunifiedStatusOK
          != (eStatus = FrameworkunifiedOpenSessionWithData(g_tPowerSession.hService,
                                               &tOpenSessionReq,
                                               sizeof(tOpenSessionReq)))) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedOpenSession Failed Status:0x%x ",
               eStatus);
      }

      FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "%s has received PowerService Available.",
             FrameworkunifiedGetAppName(hApp));
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "FrameworkunifiedAttachCallbacksToDispatcher Failed Status:0x%x ", eStatus);
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus OnPowerUnavailable(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  FRAMEWORKUNIFIEDLOG0(ZONE_FUNC, __FUNCTION__, "+");
  if (0 == strcmp(SERVICE_POWER, FrameworkunifiedGetAppName(hApp))) {
    if ((g_tPowerSession.hSession != NULL)
        && (g_tPowerSession.hService != NULL)) {
      if (eFrameworkunifiedStatusOK
          != (eStatus = FrameworkunifiedCloseSession(g_tPowerSession.hService,
                                        g_tPowerSession.hSession))) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedCloseSession Failed Status:0x%x",
               eStatus);
      }
      g_tPowerSession.hSession = NULL;
      g_tPowerSession.hService = NULL;
      FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__,
             "%s has received PowerService Unavailable.", FrameworkunifiedGetAppName(hApp));
    } else {
      FRAMEWORKUNIFIEDLOG(
          ZONE_ERR,
          __FUNCTION__,
          "Error INVALID HANDLE [g_tPowerSession.hSession :0x%" PRIxPTR "]; [g_tPowerSession.hService :0x%" PRIxPTR "] ",  // NOLINT (whitespace/line_length)
          (uintptr_t) g_tPowerSession.hSession, (uintptr_t) g_tPowerSession.hService);
    }
  } else {
    eStatus = eFrameworkunifiedStatusOK;
  }
  FRAMEWORKUNIFIEDLOG0(ZONE_FUNC, __FUNCTION__, "-");

  return eStatus;
}

///////////////////////////////////////////////////////////
/// Function: OnHeartBeatClientOpenSessionAck
/// HeartBeat Service OpenSession callback
///////////////////////////////////////////////////////////
EFrameworkunifiedStatus OnPowerOpenSessionAck(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusFail;
  FRAMEWORKUNIFIEDLOG0(ZONE_FUNC, __FUNCTION__, "+");

  g_tPowerSession.hSession = FrameworkunifiedGetOpenSessionHandle(hApp);
  if (NULL != g_tPowerSession.hSession) {
    g_tPowerSession.fAvailable = TRUE;
    eStatus = eFrameworkunifiedStatusOK;
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__,
           "%s Session to PowerService has been successfully opened.",
           FrameworkunifiedGetAppName(hApp));
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedGetOpenSessionHandle returned : %" PRIxPTR "",
           (uintptr_t) g_tPowerSession.hSession);
    g_tPowerSession.hSession = NULL;
  }

  FRAMEWORKUNIFIEDLOG0(ZONE_FUNC, __FUNCTION__, "-");

  return eStatus;
}

///////////////////////////////////////////////////////////
/// Function: OnHeartBeatClientCloseSessionAck
/// HeartBeat Service CloseSession callback
///////////////////////////////////////////////////////////
EFrameworkunifiedStatus OnPowerCloseSesionAck(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  FRAMEWORKUNIFIEDLOG0(ZONE_FUNC, __FUNCTION__, "+");

  // Close Service
  if (NULL != g_tPowerSession.hService) {
    eStatus = FrameworkunifiedCloseService(hApp, g_tPowerSession.hService);
    if (eFrameworkunifiedStatusOK != eStatus) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: FrameworkunifiedCloseService() errored %d/'%s",
             eStatus, GetStr(eStatus).c_str());
    }
    g_tPowerSession.hService = NULL;
    g_tPowerSession.hSession = NULL;
  }
  g_tPowerSession.fAvailable = FALSE;
  eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__,
         "%s Session to PowerService has been successfully closed.",
         FrameworkunifiedGetAppName(hApp));

  FRAMEWORKUNIFIEDLOG0(ZONE_FUNC, __FUNCTION__, "-");

  return eStatus;
}
