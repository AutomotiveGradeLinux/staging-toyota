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
/// \ingroup  tag_SystemManagerIf
/// \brief    This file provides support for the System Manager client
///           heartbeat service interface.
///
///////////////////////////////////////////////////////////////////////////////
#include <string.h>
#include <native_service/frameworkunified_application.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_framework_types.h>
#include <new>
#include "system_service/ss_heartbeat_service_protocol.h"
#include "system_service/ss_heartbeat_notifications.h"
#include "system_service/ss_heartbeat_if.h"
#include "system_service/ss_services.h"
#include "system_service/ss_sm_thread_names.h"
#include "system_service/ss_templates.h"
#include "ss_system_if_interfaceunifiedlog.h"

CHeartBeatServiceIf * pHeartBeatServiceIf = NULL;

template <typename C, eFrameworkunifiedStatus(C::*M)(HANDLE)>

// LCOV_EXCL_START 6:Because process initialization
EFrameworkunifiedStatus HeartBeatIfCallback(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusNullPointer;
  C * pObj = static_cast<C *>(pHeartBeatServiceIf);
  if (pObj) {
    l_eStatus = (pObj->*M)(hApp);
  }
  return l_eStatus;
}
// LCOV_EXCL_STOP

/*****************************************************************************
    @ingroup: SS_SystemManager
    @brief:  InterfaceunifiedSystemConnectToHeartBeatService
    @note: .  Called from framework for every app to start connection to HeartBeat
    @param HANDLE - Handle to message queue of HeartBeat Service.
    @return EFrameworkunifiedStatus OK or Fail
*****************************************************************************/
EFrameworkunifiedStatus InterfaceunifiedSystemConnectToHeartBeatService(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  /// Avoid opening a session to the HeartBeat
  /// service Since it uses the framework as well
  /// Avoid opening a session to System Manager For testing
  // TODO(my_username): Remove system manager check
  if ((0 != strcmp(SERVICE_SYSMANAGER, FrameworkunifiedGetAppName(hApp)))) {
    FrameworkunifiedProtocolCallbackHandler     g_aryHeartBeat_Protocol_Cbs[] = {
      // Command ID,              Call back functions
      { SS_HEARTBEAT_REQUEST,       HeartBeatIfCallback<CHeartBeatServiceIf, &CHeartBeatServiceIf::OnHeartBeatRequest>}
    };  // LCOV_EXCL_BR_LINE 11:Unexpected branch

    /// Attach the valid callback for this service
    if ( eFrameworkunifiedStatusOK !=  // LCOV_EXCL_BR_LINE 6:Because process initialization
        (l_eStatus = FrameworkunifiedAttachCallbacksToDispatcher(
                                    hApp,
                                    SS_SMHeartbeat,
                                    g_aryHeartBeat_Protocol_Cbs,
                                    _countof(g_aryHeartBeat_Protocol_Cbs)))) {
      LOG_ERROR("FrameworkunifiedAttachCallbacksToDispatcher()");
    } else {
      // LCOV_EXCL_BR_LINE 5:Because constructor
      pHeartBeatServiceIf =
          new(std::nothrow) CHeartBeatServiceIf(FALSE, NULL, FrameworkunifiedGetAppName(hApp));
    }  // LCOV_EXCL_BR_LINE 11:Unexpected branch

  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Heartbeat service name duplicated.");
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");

  return l_eStatus;  // LCOV_EXCL_BR_LINE 11:Unexpected branch
}

/*****************************************************************************
    @ingroup: SS_SystemManager
    @brief:  CHeartBeatServiceIf constructor
    @param  -
*****************************************************************************/
CHeartBeatServiceIf::CHeartBeatServiceIf(BOOL avail, HANDLE service, PCSTR name) {
  g_tHeartBeatSession.szServiceName = name;  // LCOV_EXCL_BR_LINE 11:Unexpected branch
  g_tHeartBeatSession.fAvailable = avail;
  g_tHeartBeatSession.hService = service;
}

// LCOV_EXCL_START 10:Because destructor
/*****************************************************************************
    @ingroup: SS_SystemManager
    @brief:  CHeartBeatServiceIf destructor
*****************************************************************************/
CHeartBeatServiceIf::~CHeartBeatServiceIf() {
  if (pHeartBeatServiceIf != NULL) {
    delete pHeartBeatServiceIf;
    pHeartBeatServiceIf = NULL;
  }
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START 6:Because process initialization
/*****************************************************************************
    @ingroup: SS_SystemManager
    @brief:  OnHeartBeatRequest
    @note: .
    @param HANDLE - Handle to message queue of HeartBeat Service.
    @return EFrameworkunifiedStatus OK or Fail
*****************************************************************************/
EFrameworkunifiedStatus CHeartBeatServiceIf::OnHeartBeatRequest(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  BOOL l_availability;

  if (g_tHeartBeatSession.hService == NULL) {
    if ( NULL == (g_tHeartBeatSession.hService = FrameworkunifiedMcOpenSender(hApp, SS_SMHeartbeat)) ) {
      l_eStatus = eFrameworkunifiedStatusNullPointer;
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        " Error: FrameworkunifiedMcOpenSender(%s) returned NULL",
        SS_SMHeartbeat);
      return l_eStatus;
    }
  }

  if ( (0 == strcmp(SS_SMHeartbeat, FrameworkunifiedGetMsgSrc(hApp))) ) {
    if (NULL != g_tHeartBeatSession.hService) {
      l_availability = FrameworkunifiedGetSelfAvailability(hApp);

      if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedSendMsg(g_tHeartBeatSession.hService, SS_HEARTBEAT_RESPONSE,
                                                  sizeof(l_availability), &l_availability))) {
        LOG_ERROR("FrameworkunifiedSendMsg");
      }
    } else {
      l_eStatus = eFrameworkunifiedStatusInvldHandle;
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Invalid handle[_g_hHeartBeatSessionHandle = :NULL]");
    }
  }
  return l_eStatus;
}
// LCOV_EXCL_STOP
