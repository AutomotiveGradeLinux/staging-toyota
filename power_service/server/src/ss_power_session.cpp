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
/// \ingroup  tag_Power
/// \brief    This file supports power service session management.
///
///////////////////////////////////////////////////////////////////////////////
#include "ss_power_session.h"
#include <system_service/ss_services.h>
#include <system_service/ss_templates.h>
#include <system_service/ss_power_service.h>
#include <system_service/ss_power_service_protocol.h>
#include <system_service/ss_power_service_local.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_service_protocol.h>

#include "ss_power_powerservicelog.h"


PowerSessionHandler::PowerSessionHandler() {
  // TODO(my_username) Auto-generated constructor stub
}

PowerSessionHandler::~PowerSessionHandler() {  // LCOV_EXCL_START 14 Resident process, not called by NSFW
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  // TODO(my_username) Auto-generated destructor stub
}
// LCOV_EXCL_STOP 14 Resident process, not called by NSFW

EFrameworkunifiedStatus PowerSessionHandler::CloseSession(HANDLE h_app) {
  EFrameworkunifiedStatus l_eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  INTERFACEUNIFIEDLOG_RECEIVED_FROM(h_app);

  // find the subscriber...
  PwSessionIter iter = m_mapSessions.find(FrameworkunifiedGetMsgSrc(h_app));

  // the iter is set to the end then the subscriber is not in the map
  if (m_mapSessions.end() == iter) {  // LCOV_EXCL_BR_LINE 11:unexpected branch
    // LCOV_EXCL_START 11:unexpected branch
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(
        ZONE_ERR,
        __FUNCTION__,
        " Error: Subscriber '%s' was not found in the map; the Close " "Session Request is being ignored!",
        FrameworkunifiedGetMsgSrc(h_app));
    l_eStatus = eFrameworkunifiedStatusDbRecNotFound;
  } else if (NULL == iter->second.hsession) {  // LCOV_EXCL_BR_LINE 11:unexpected branch
    l_eStatus = eFrameworkunifiedStatusInvldHandle;
    LOG_ERROR("Client's PwSessionInfo handle");
    // LCOV_EXCL_STOP 11:unexpected branch
  } else {
    CloseSessionAck closeSessionAck;
    closeSessionAck.sessionId = FrameworkunifiedGetSessionId(iter->second.hsession);
    closeSessionAck.eStatus = eFrameworkunifiedStatusOK;

    // Send Ack to subscriber
    if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedSendMsg(iter->second.hsession, PROTOCOL_CLOSE_SESSION_ACK, sizeof(CloseSessionAck), (PVOID) &closeSessionAck))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
      // LCOV_EXCL_START 4: NSFW error case.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(
          ZONE_ERR,
          __FUNCTION__,
          " Error: FrameworkunifiedSendMsg(%s, PROTOCOL_CLOSE_SESSION_ACK) " "errored: %d/'%s'",
          iter->second.sz_name.c_str(), l_eStatus, GetStr(l_eStatus).c_str());
      // LCOV_EXCL_STOP 4: NSFW error case.
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
             " FrameworkunifiedSendMsg(%s, PROTOCOL_CLOSE_SESSION_ACK) successful",
             iter->second.sz_name.c_str());
      iter->second.frunning = FALSE;

      //
      // ToDo Jay 2012 November 05 1) Can't detach just basic callbacks;
      //      client could be Supervisor or System. This has been solved
      //      in another Power Service Change Set. Need to get all
      //      Change Sets merged and delivered.
      UI_32 ss_power_basic_session_ids[] = { PROTOCOL_CLOSE_SESSION_REQ };

      // Detach callback : Power System Session Requests
      if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedDetachCallbacksFromDispatcher(h_app, iter->second.sz_name.c_str(), ss_power_basic_session_ids, _countof(ss_power_basic_session_ids)))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
        // LCOV_EXCL_START 4: NSFW error case.
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        LOG_ERROR("FrameworkunifiedDetachCallbacksFromDispatcher()");
        // LCOV_EXCL_STOP 4: NSFW error case.
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
               " FrameworkunifiedDetachCallbacksFromDispatcher() successful");
      }

      CALL_AND_LOG_STATUS(FrameworkunifiedDestroySession(h_app, iter->second.hsession));
      iter->second.hsession = NULL;
      m_mapSessions.erase(iter);
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus PowerSessionHandler::StartComplete(HANDLE h_app) {  // LCOV_EXCL_START 8: can not be called
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus l_eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  StartCompleteAck ack;

  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg<StartCompleteAck>(h_app, ack))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("ReadMsg()");
  } else {
    // find the subscriber...
    PwSessionIter iter = m_mapSessions.find(FrameworkunifiedGetMsgSrc(h_app));

    // the iter is set to the end then the subscriber is not in the map
    if (m_mapSessions.end() != iter) {
      iter->second.sz_servicename = ack.szServiceName;
      iter->second.frunning = TRUE;

      if (AllClientsInGroupStarted(iter->second.ui_groupid)) {
        // NOP
      }
    } else {
      FRAMEWORKUNIFIEDLOG(
          ZONE_ERR,
          __FUNCTION__,
          "Subscriber: %s , was not found in the map, the close session is being ignored!",
          FrameworkunifiedGetMsgSrc(h_app));
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
// LCOV_EXCL_STOP 8: can not be called
VOID PowerSessionHandler::Print() {
  // find the subscriber...
  PwSessionIter iter = m_mapSessions.begin();
  for (; iter != m_mapSessions.end(); iter++) {
    FRAMEWORKUNIFIEDLOG(ZONE_DEBUG_DUMP, __FUNCTION__,
           " Power Service is session-connected to '%s';  running: %s",
           iter->second.sz_name.data(), (iter->second.frunning ? "YES" : "NO"));
  }
}

// LCOV_EXCL_START 8: can not be called
BOOL PowerSessionHandler::WakeupComplete(RequiredModuleList & wakeup_modules) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  BOOL rtn = TRUE;  // set the return TRUE until we find a missing module
  RequiredModuleListIter iter = wakeup_modules.begin();
  for (; iter != wakeup_modules.end(); iter++) {
    PwSessionIter psi = m_mapSessions.find(*iter);
    if (m_mapSessions.end() == psi) {
      rtn = FALSE;  // Module Not Found in Power Service Session Handler
      FRAMEWORKUNIFIEDLOG(
          ZONE_INFO,
          __FUNCTION__,
          "Required Wakeup: %s has not connected to Power Service, Wakeup NOT COMPLETE!",
          iter->data());
    } else if (FALSE == psi->second.frunning) {
      rtn = FALSE;  // Module Found in Power Service Session Handler but hasn't started yet
      FRAMEWORKUNIFIEDLOG(
          ZONE_INFO,
          __FUNCTION__,
          "Required Wakeup: %s has connected to Power Service but is not running (FrameworkunifiedStart Response missing),"
          " Wakeup NOT COMPLETE!",
          iter->data());
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return rtn;
}
// LCOV_EXCL_STOP 8: can not be called

BOOL PowerSessionHandler::ShutdownComplete(
    RequiredModuleList & shutdown_modules) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  BOOL rtn = TRUE;  // set the return TRUE until we find a missing module
  RequiredModuleListIter iter = shutdown_modules.begin();
  for (; iter != shutdown_modules.end(); iter++) {
    PwSessionIter psi = m_mapSessions.find(*iter);
    if (m_mapSessions.end() == psi) {
      FRAMEWORKUNIFIEDLOG(
          ZONE_INFO,
          __FUNCTION__,
          "Required Shutdown: %s is not in Power Service session list, Shutdown pending!",
          iter->data());
    } else if (TRUE == psi->second.frunning) {
      rtn = FALSE;  // Module Found in Power Service Session Handler but hasn't stopped yet
      FRAMEWORKUNIFIEDLOG(
          ZONE_INFO,
          __FUNCTION__,
          "Required Shutdown: %s is connected to Power Service but is running (Stop Response missing),"
          " Shutdown NOT COMPLETE!",
          iter->data());
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return rtn;
}

EFrameworkunifiedStatus PowerSessionHandler::SendToSupervisor(UI_32 cmd, UI_32 length,
                                                 PVOID data) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (NULL == data && length > 0) {  // LCOV_EXCL_BR_LINE 6: it will not be true at the other place
    // LCOV_EXCL_START 6: it will not be true at the other place
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("NULL == data && length > 0");
    l_eStatus = eFrameworkunifiedStatusInvldBuf;
    // LCOV_EXCL_STOP 6: it will not be true at the other place
  } else {
    // get first client from the map...
    PwSessionIter iter = m_mapSessions.begin();
    BOOL l_bFoundSupervisor = FALSE;

    for (; iter != m_mapSessions.end() && !l_bFoundSupervisor; iter++) {
      l_bFoundSupervisor = (epsstSUPERVISOR == iter->second.esessiontype);
      if (l_bFoundSupervisor) {
        l_eStatus = FrameworkunifiedSendMsg(iter->second.hsession, cmd, length, data);
        LOG_STATUS(l_eStatus, "FrameworkunifiedSendMsg()");
      }
    }
    if (!l_bFoundSupervisor) {
      FRAMEWORKUNIFIEDLOG(
          ZONE_ERR, __FUNCTION__,
          "Error: Did not find a Supervisor session; did not " "send cmd %d !",
          cmd);
      l_eStatus = eFrameworkunifiedStatusDbRecNotFound;
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

BOOL PowerSessionHandler::AllClientsInGroupStarted(UI_32 f_ungrpid) {  // LCOV_EXCL_START 8: can not be called
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  BOOL fStatus = TRUE;
  BOOL fGrpFnd = FALSE;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  PwSessionIter iter = m_mapSessions.begin();

  if (iter == m_mapSessions.end())
    fStatus = TRUE;

  for (; iter != m_mapSessions.end() && fStatus; iter++) {
    if (f_ungrpid == iter->second.ui_groupid) {
      fGrpFnd = TRUE;
      if (FALSE == iter->second.frunning) {
        fStatus = FALSE;
      }
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "- return flag: %s",
         ((fStatus && fGrpFnd) ? "TRUE" : "FALSE"));
  return (fStatus && fGrpFnd);
}
// LCOV_EXCL_STOP 8: can not be called
