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

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_SS_LoggerService
/// \brief    This file contains the callback for Notifications and Protocol
///           commands.
///
///////////////////////////////////////////////////////////////////////////////
#include "ss_logger_service_callbacks.h"
#include <string.h>
#include <boost/bind.hpp>
#include <errno.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_multithreading.h>
#include <system_service/ss_logger_service.h>
#include <ss_logger_common.h>
#include <system_service/ss_logger_service_notifications.h>
#include <system_service/ss_logger_service_protocol.h>
#include <native_service/ns_config_parser_if.h>
#include <native_service/nslogutil_cmd_if.h>
#include <system_service/ss_sm_client_if.h>
#include <queue>
#include <new>
#include <string>
#include "ss_logger_error_event.h"
#include "loggerservicedebug_loggerservicelog.h"
#include "loggerservicedebug_thread_if.h"
extern CErrorEvent g_errorEventHandler;
extern HANDLE g_thrdEvntLogWriter;

// Global variables
STCanCurrentDateTime g_stDateAndTime = { 0 };
UEvtLoggerCommonInfo g_uEvtLoggerCommonInfo;
UI_16 g_u16DiagId = 0;

CLoggerServiceCallbacks::CLoggerServiceCallbacks()
    : m_pLoggerCfg(NULL),
      m_pErrorEvent(NULL) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FrameworkunifiedProtocolCallbackHandler l_protocolHandlers[kNUMBER_PROTOCOL_HANDLERS] =
      { { SS_LOGGER_MILEAGE_DATA, boost::bind(
          &CLoggerServiceCallbacks::LoggerServer_SetMileageData, this, _1) },
          { SS_LOGGER_SET_PARAMS, boost::bind(
              &CLoggerServiceCallbacks::LoggerServer_SetLoggerParams, this, _1) },
          { SS_LOGGER_UDP_LOGGING, boost::bind(
              &CLoggerServiceCallbacks::LoggerServer_UDPLogging, this, _1) },
          { SS_LOGGERCOPYEVENTUSB, boost::bind(
              &CLoggerServiceCallbacks::LoggerServer_OnCopyEvntLog, this,_1) },
          { SS_LOGGERCLEAREVENT, boost::bind(
              &CLoggerServiceCallbacks::LoggerServer_OnClearEvntLog, this, _1) },
          { SS_LOGGER_READ_STATL_COUNTER, boost::bind(
              &CLoggerServiceCallbacks::LoggerServer_OnReadStatCounter, this, _1) },
          { SS_LOGGER_EVENT_COMMONINFO, boost::bind(
              &CLoggerServiceCallbacks::LoggerServer_UpdateCommonInfo, this, _1) },
          { SS_LOGGER__CWORD56__EVENT_INFO, boost::bind(
              &CLoggerServiceCallbacks::LoggerServer_Handle_CWORD56_Events, this, _1) },
          { SS_LOGGER_RESET_STATL_COUNTER, boost::bind(
              &CLoggerServiceCallbacks::LoggerServer_OnResetStatCounter, this, _1) },
          { SS_LOGGER_SET_VIN, boost::bind(
              &CLoggerServiceCallbacks::LoggerServer_OnSetVIN, this, _1) },
          { SS_LOGGERCOPYEMERGENCYLOGS, boost::bind(
              &CLoggerServiceCallbacks::LoggerServer_OnCopyEmergencyLog, this, _1) },
          { eSSLoggerCANProtocolIDCANTrigger, boost::bind(
              &CLoggerServiceCallbacks::LoggerServer_OnCANTrigger, this, _1) },
          { eSSLoggerCANProtocolIDDTCTrigger, boost::bind(
              &CLoggerServiceCallbacks::LoggerServer_OnDTCTrigger, this, _1) },
          { SS_LOGGER_SHUTDOWN_COMPLETE, boost::bind(
              &CLoggerServiceCallbacks::LoggerServer_On_SHTDWN_Complete, this, _1) },
          { SS_LOGGER_ENG_READ_NUMOFEVENTS, boost::bind(
              &CLoggerServiceCallbacks::LoggerServer_OnReadNumberOfEvents, this, _1) },
          { SS_LOGGER_SET_DIAGID, boost::bind(
              &CLoggerServiceCallbacks::LoggerServer_OnSetDiagID, this, _1) },
          { SS_LOGGER_UPLOAD_EVENTLOG, boost::bind(
              &CLoggerServiceCallbacks::LoggerServer_UploadEventLog, this, _1) } };  // LCOV_EXCL_BR_LINE 11:Unexpected branch

  for (UI_8 i = 0u; i < kNUMBER_PROTOCOL_HANDLERS; i++) {
    m_protocolHandlers[i].iCmd = l_protocolHandlers[i].iCmd;
    m_protocolHandlers[i].callBack = l_protocolHandlers[i].callBack;  // LCOV_EXCL_BR_LINE 11:Unexpected branch
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

CLoggerServiceCallbacks::~CLoggerServiceCallbacks() {  // LCOV_EXCL_START 14:globle instance
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus CLoggerServiceCallbacks::Initialize(HANDLE f_hApp,
                                               CLoggerCfg *f_pLoggerCfg,
                                               CErrorEvent* f_pErrorEvent) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  m_pLoggerCfg = f_pLoggerCfg;
  m_pErrorEvent = f_pErrorEvent;

  if ((NULL == m_pLoggerCfg) || (NULL == m_pErrorEvent)) {  // LCOV_EXCL_BR_LINE 200:As it is always TRUE
    // LCOV_EXCL_START 200:As it is always TRUE
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "NULL pointer passed to Initialize function!!!!!");
    l_eStatus = eFrameworkunifiedStatusNullPointer;
    // LCOV_EXCL_STOP
  } else {
    // Attach callback : Open Session Request
    if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedAttachCallbackToDispatcher(f_hApp, FRAMEWORKUNIFIED_ANY_SOURCE, PROTOCOL_OPEN_SESSION_REQ, boost::bind(&CLoggerServiceCallbacks::LoggerServerOnOpenSession, this, _1)))) {  // LCOV_EXCL_BR_LINE 4:NSFW  // NOLINT[whitespace/line_length]
      // LCOV_EXCL_START 4:NSFW
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(
          ZONE_ERR,
          __FUNCTION__,
          "FrameworkunifiedAttachCallbackToDispatcher PROTOCOL_OPEN_SESSION_REQ (FRAMEWORKUNIFIED_ANY_SOURCE) Failed Status:0x%x ",
          l_eStatus);
      // LCOV_EXCL_STOP
    } else if ( eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedRegisterServiceAvailabilityNotification(f_hApp, NTFY_SS_LoggerService_Availability))) {  // LCOV_EXCL_BR_LINE 4:NSFW  // NOLINT[whitespace/line_length]
      // LCOV_EXCL_START 4:NSFW
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Failed to set service availability notification:0x%x ", l_eStatus);
      // LCOV_EXCL_STOP
    } else {
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

//////////////////////////////////////////
//  Function : DevDetectionServerOnOpenSession
//  Callback for PROTOCOL_OPEN_SESSION_REQ
//////////////////////////////////////////
EFrameworkunifiedStatus CLoggerServiceCallbacks::LoggerServerOnOpenSession(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  HANDLE l_hClientService = NULL;
  HANDLE l_hSession = NULL;

  // Get the requester name
  PCSTR pRequester = FrameworkunifiedGetMsgSrc(hApp);
  OpenSessionAck tOpenAck;

  if (pRequester) {  // LCOV_EXCL_BR_LINE 4:NSFW
    l_hClientService = FrameworkunifiedMcOpenSender(hApp, pRequester);

    if (l_hClientService) {  // LCOV_EXCL_BR_LINE 4:NSFW
      // Create a session
      l_hSession = FrameworkunifiedCreateSession(hApp, pRequester);
      UI_32 l_nSessionId = 0;
      if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedAttachCallbacksToDispatcher( hApp, pRequester, m_protocolHandlers, kNUMBER_PROTOCOL_HANDLERS, l_hSession))) {  // LCOV_EXCL_BR_LINE 4:NSFW  // NOLINT[whitespace/line_length]
        // LCOV_EXCL_START 4:NSFW
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(
            ZONE_ERR,
            __PRETTY_FUNCTION__,
            "FrameworkunifiedAttachCallbacksToDispatcher ss_logger_server_protocol_handlers Failed Status:0x%x ",
            l_eStatus);
        // LCOV_EXCL_STOP
      }

      l_eStatus = this->m_pErrorEvent->OnOpenSession(hApp, pRequester,
                                                     l_hSession);
      if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 200:To ensure success
        // LCOV_EXCL_START 200:To ensure success
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               " Error. m_pErrorEvent->OnOpenSession(%s, %p) returned: %d.",
               pRequester, l_hSession, l_eStatus);
        // LCOV_EXCL_STOP
      }

      // Attach callback : close Session Request
      if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedAttachCallbackToDispatcher(hApp, pRequester, PROTOCOL_CLOSE_SESSION_REQ, boost::bind(&CLoggerServiceCallbacks::LoggerServerOnCloseSession, this, _1)))) {  // LCOV_EXCL_BR_LINE 4:NSFW  // NOLINT[whitespace/line_length]
        // LCOV_EXCL_START 4:NSFW
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(
            ZONE_ERR,
            __FUNCTION__,
            "FrameworkunifiedAttachCallbackToDispatcher " "PROTOCOL_CLOSE_SESSION_REQ (FRAMEWORKUNIFIED_ANY_SOURCE) Failed Status:0x%x ",
            l_eStatus);
        // LCOV_EXCL_STOP
      }

      l_nSessionId = FrameworkunifiedGetSessionId(l_hSession);

      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Session id is %d", l_nSessionId);  // LCOV_EXCL_BR_LINE 15:macro
      // sending the session open
      tOpenAck.eStatus = eFrameworkunifiedStatusOK;
      tOpenAck.sessionId = l_nSessionId;
      tOpenAck.sessionType = 1;

      // Copy session name to data structure
      strncpy(tOpenAck.cSessionName, FrameworkunifiedGetAppName(hApp),
              MAX_QUEUE_NAME_SIZE - 1);
      tOpenAck.cSessionName[MAX_QUEUE_NAME_SIZE - 1] = '\0';

      // send OpenSession ACK
      if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedSendMsg(l_hClientService, PROTOCOL_OPEN_SESSION_ACK, sizeof(OpenSessionAck), (PVOID) &tOpenAck))) {  // LCOV_EXCL_BR_LINE 4:NSFW  // NOLINT[whitespace/line_length]
        // LCOV_EXCL_START 4:NSFW
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __PRETTY_FUNCTION__,
               "FrameworkunifiedSendMsg PROTOCOL_OPEN_SESSION_ACK Failed Status:0x%x ",
               l_eStatus);
        // LCOV_EXCL_STOP
      } else {
        l_eStatus = FrameworkunifiedSetSessionHandle(hApp, FrameworkunifiedGetMsgSrc(hApp), l_hSession);
        LOG_STATUS_IF_ERRORED(l_eStatus, "FrameworkunifiedSetSessionHandle()");  // LCOV_EXCL_BR_LINE 15:macro
      }
    }
    l_eStatus = FrameworkunifiedMcClose(l_hClientService);
    LOG_STATUS_IF_ERRORED(l_eStatus, "FrameworkunifiedMcClose()");  // LCOV_EXCL_BR_LINE 15:macro
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");

  return l_eStatus;
}

//////////////////////////////////////////
//  Function : DevDetectionServerOnCloseSession
//  Callback for PROTOCOL_CLOSE_SESSION_REQ
//////////////////////////////////////////
EFrameworkunifiedStatus CLoggerServiceCallbacks::LoggerServerOnCloseSession(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  HANDLE hSession = NULL;
  HANDLE l_hClientService = NULL;
  // Get the requester name
  PCSTR pRequester = FrameworkunifiedGetMsgSrc(hApp);
  CloseSessionAck tCloseAck;
  CloseSessionReq tClose;

  if (pRequester) {  // LCOV_EXCL_BR_LINE 4:NSFW
    l_hClientService = FrameworkunifiedMcOpenSender(hApp, pRequester);
    if (l_hClientService) {  // LCOV_EXCL_BR_LINE 4:NSFW
      // Read the data from the message
      if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedGetMsgDataOfSize(hApp, &tClose, sizeof(tClose), eSMRRelease))) {  // LCOV_EXCL_BR_LINE 4:NSFW  // NOLINT[whitespace/line_length]
        // LCOV_EXCL_START 4:NSFW
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: FrameworkunifiedGetMsgDataOfSize Failed");

        if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedMcClose(l_hClientService))) {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: FrameworkunifiedMcClose failed");
        }
        FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
        return eStatus;
        // LCOV_EXCL_STOP
      }

      hSession = FrameworkunifiedGetSessionHandle(hApp, pRequester, tClose.sessionId);

      // Create a list of commands to be detached from the dispatcher
      UI_32 cmd_array[kNUMBER_PROTOCOL_HANDLERS] = { 0 };
      for (UI_32 i = 0; i < kNUMBER_PROTOCOL_HANDLERS; i++) {
        cmd_array[i] = m_protocolHandlers[i].iCmd;
      }

      if (NULL != hSession) {  // LCOV_EXCL_BR_LINE 4:NSFW
        // Detach callbacks associated with client
        if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedDetachCallbacksFromDispatcher(hApp, pRequester, cmd_array, kNUMBER_PROTOCOL_HANDLERS, hSession))) {  // LCOV_EXCL_BR_LINE 4:NSFW  // NOLINT[whitespace/line_length]
          // LCOV_EXCL_START 4:NSFW
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(
              ZONE_ERR,
              __PRETTY_FUNCTION__,
              "FrameworkunifiedAttachCallbacksToDispatcher ss_logger_server_protocol_handlers Failed Status:0x%x ",
              eStatus);
          // LCOV_EXCL_STOP
        }

        eStatus = m_pErrorEvent->OnCloseSession(hApp, pRequester, hSession);
        if (eFrameworkunifiedStatusOK != eStatus) {  // LCOV_EXCL_BR_LINE 200:To ensure success
          // LCOV_EXCL_START 200:To ensure success
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                 " Error. m_pErrorEvent->OnCloseSession(%s, %p) returned: %d.",
                 pRequester, hSession, eStatus);
          // LCOV_EXCL_STOP
        }

        // API to destroy the session handle and remove it from the Application Framework.
        // LCOV_EXCL_BR_START 4:NSFW
        if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedDestroySession(hApp,  // Application Handle
            hSession))) {  // Session Handle
        // LCOV_EXCL_BR_STOP
          // LCOV_EXCL_START 4:NSFW
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Session destroy failed");
          // LCOV_EXCL_STOP
        }
      }
      // sending the session close ack
      tCloseAck.eStatus = eFrameworkunifiedStatusOK;
      tCloseAck.sessionId = tClose.sessionId;

      // send CloseSession ACK
      if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedSendMsg(l_hClientService, PROTOCOL_CLOSE_SESSION_ACK, sizeof(CloseSessionAck), (PVOID) &tCloseAck))) {  // LCOV_EXCL_BR_LINE 4:NSFW  // NOLINT[whitespace/line_length]
        // LCOV_EXCL_START 4:NSFW
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __PRETTY_FUNCTION__,
               "FrameworkunifiedSendMsg PROTOCOL_CLOSE_SESSION_ACK Failed Status:0x%x ",
               eStatus);
        // LCOV_EXCL_STOP
      }
      if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedMcClose(l_hClientService))) {  // LCOV_EXCL_BR_LINE 4:NSFW
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: FrameworkunifiedMcClose failed");  // LCOV_EXCL_LINE 15:macro
      }
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

//////////////////////////////////////////
//  Function : LoggerServer_SetMileageData
//  Callback for SS_LOGGER_MILEAGE_DATA
//////////////////////////////////////////
EFrameworkunifiedStatus CLoggerServiceCallbacks::LoggerServer_SetMileageData(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  STLOGGER_CANDIAGSTAT MileageInfo_t;
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  memset(&MileageInfo_t, 0x00, sizeof(STLOGGER_CANDIAGSTAT));
  if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedGetMsgDataOfSize(hApp, &MileageInfo_t, sizeof(MileageInfo_t), eSMRRelease))) {  // LCOV_EXCL_BR_LINE 4:NSFW  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 4:NSFW
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " FrameworkunifiedGetMsgDataOfSize Failed");
    l_eStatus = eFrameworkunifiedStatusFail;
    // LCOV_EXCL_STOP
  } else {
    UI_32 l_mileage = (MileageInfo_t.Odo_MSB_H << 24
        | MileageInfo_t.Odo_MSB_L << 16 | MileageInfo_t.Odo_LSB_H << 8
        | MileageInfo_t.Odo_LSB_L);
    // if value set to SNA
    if (0xFFFFFFFF != l_mileage) {
      this->m_pErrorEvent->SetMileage(l_mileage);
      // Immediate Persist Event Log
      if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedSendChild(hApp, g_thrdEvntLogWriter, eThrdCmdMileageData, sizeof(UI_32), &l_mileage))) {  // LCOV_EXCL_BR_LINE 4:NSFW  // NOLINT[whitespace/line_length]
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedSendChild msg failed:%d", l_eStatus);  // LCOV_EXCL_LINE 15:macro
      }
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

//////////////////////////////////////////
//  Function : LoggerServer_SetLoggerParams
//  Callback for SS_LOGGER_SET_PARAMS
//////////////////////////////////////////
EFrameworkunifiedStatus CLoggerServiceCallbacks::LoggerServer_SetLoggerParams(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  STLoggerSetParams l_stLoggerSetParams = { eInvalid_LoggerState, eDevUSB1 };

  if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedGetMsgDataOfSize(hApp, &l_stLoggerSetParams, sizeof(l_stLoggerSetParams), eSMRRelease))) {  // LCOV_EXCL_BR_LINE 4:NSFW  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 4:NSFW
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: FrameworkunifiedGetMsgDataOfSize Failed");
    eStatus = eFrameworkunifiedStatusFail;
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
    return eStatus;
    // LCOV_EXCL_STOP
  } else {
    eStatus = m_pLoggerCfg->SetLoggingParams(hApp, l_stLoggerSetParams);
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

//////////////////////////////////////////
//  Function : LoggerServer_SetLoggerParams
//  Callback for SS_LOGGER_SET_PARAMS
//////////////////////////////////////////
EFrameworkunifiedStatus CLoggerServiceCallbacks::LoggerServer_UDPLogging(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  ELOGGER_STAT l_eUDPLoggingStatus = eInvalid_LoggerState;

  if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedGetMsgDataOfSize(hApp, &l_eUDPLoggingStatus, sizeof(l_eUDPLoggingStatus), eSMRRelease))) {  // LCOV_EXCL_BR_LINE 4:NSFW  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 4:NSFW
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: FrameworkunifiedGetMsgDataOfSize Failed");
    l_eStatus = eFrameworkunifiedStatusFail;
    // LCOV_EXCL_STOP
  } else {
    l_eStatus = m_pLoggerCfg->SetUDPLogging(hApp, l_eUDPLoggingStatus);
    LOG_STATUS_IF_ERRORED(  // LCOV_EXCL_BR_LINE 15:macro
        l_eStatus,
        "m_pLoggerCfg->SetUDPLogging()");
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

EFrameworkunifiedStatus CLoggerServiceCallbacks::LoggerServer_UpdateCommonInfo(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedGetMsgDataOfSize(hApp, &g_uEvtLoggerCommonInfo.u_stEvtLoggerCommonInfo, sizeof(g_uEvtLoggerCommonInfo.u_stEvtLoggerCommonInfo), eSMRRelease))) {  // LCOV_EXCL_BR_LINE 4:NSFW  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 4:NSFW
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " FrameworkunifiedGetMsgDataOfSize Failed");
    eStatus = eFrameworkunifiedStatusFail;
    // LCOV_EXCL_STOP
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus CLoggerServiceCallbacks::LoggerServer_Handle_CWORD56_Events(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  STEventLoggerEventInfo l_stEventLoggingEventInfo;
  UI_16 l_evtid = 0;

  if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedGetMsgDataOfSize(hApp, &l_stEventLoggingEventInfo, sizeof(l_stEventLoggingEventInfo), eSMRRelease))) {  // LCOV_EXCL_BR_LINE 4:NSFW  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 4:NSFW
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " FrameworkunifiedGetMsgDataOfSize Failed");
    eStatus = eFrameworkunifiedStatusFail;
    // LCOV_EXCL_STOP
  }
  l_evtid = static_cast<UI_16>(((l_stEventLoggingEventInfo.EventGroup & 0x00FF) << 8)
      | (l_stEventLoggingEventInfo.EventIdentifier & 0x00FF));
  FRAMEWORKUNIFIEDLOG_EVT(ZONE_INFO, l_evtid, 4, l_stEventLoggingEventInfo.EventData[3],
             l_stEventLoggingEventInfo.EventData[2],
             l_stEventLoggingEventInfo.EventData[1],
             l_stEventLoggingEventInfo.EventData[0]);

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus CLoggerServiceCallbacks::LoggerServer_OnCopyEmergencyLog(
    HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  EDevNumber l_eDevNumber;
  std::string l_path = "";
  std::string l_srcName = "";

  if (NULL != f_hApp) {  // LCOV_EXCL_BR_LINE 6: it can't be null.
    PCSTR l_tmp_srcname = FrameworkunifiedGetMsgSrc(f_hApp);
    if (NULL != l_tmp_srcname) {  // LCOV_EXCL_BR_LINE 6: it can't be null.
      l_srcName = l_tmp_srcname;
    }

    if (eFrameworkunifiedStatusOK
        == (l_eStatus = FrameworkunifiedGetMsgDataOfSize(f_hApp, &l_eDevNumber,
                                            sizeof(l_eDevNumber), eSMRRelease))) {
      switch (l_eDevNumber) {
        case eEEL_USB1:
          l_path = m_pLoggerCfg->m_usb0MountPath;
          break;

        case eEEL_USB2:
          l_path = m_pLoggerCfg->m_usb1MountPath;
          break;

        case eEEL_SD:
          l_path = m_pLoggerCfg->m_sdMountPath;
          break;

        default:
          /*send back error response wrong param*/
          l_eStatus = eFrameworkunifiedStatusInvldParam;
          break;
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. Could not get msg Data.");
    }

    HANDLE l_hSession = NULL;
    if (NULL == (l_hSession = FrameworkunifiedGetCurrentSessionHandle(f_hApp))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
      // LCOV_EXCL_START 4: NSFW error case.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_eStatus = eFrameworkunifiedStatusNullPointer;
      LOG_ERROR("FrameworkunifiedGetCurrentSessionHandle()");
      // LCOV_EXCL_STOP
    } else {
      m_pErrorEvent->SetDiagSessionHandle(l_hSession);
    }
    if (0 == l_srcName.length()) {  // LCOV_EXCL_BR_LINE 6: it can't be 0.
      // LCOV_EXCL_START 6: it can't be 0.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             " Error. Could not get name of source service.");
    // LCOV_EXCL_STOP
      l_eStatus = eFrameworkunifiedStatusInvldHandle;
    } else if (eFrameworkunifiedStatusOK != l_eStatus) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             " Error. Invalid parameter l_eDevNumber(%d).", l_eDevNumber);
      l_eStatus = m_pErrorEvent->sendDiagEventErrorResponse(eNO_ERROR_INFO,
                                                            l_srcName);
      LOG_STATUS_IF_ERRORED(l_eStatus, "m_pErrorEvent->sendDiagEventErrorResponse(l_srcName)");
      l_eStatus = eFrameworkunifiedStatusInvldParam;
    } else if (eFrameworkunifiedStatusOK
        != (l_eStatus = m_pErrorEvent->SetDiagEventSourceName(l_srcName))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             " Error. m_pErrorEvent->SetDiagEventSourceName() returned %d.", l_eStatus);
      l_eStatus = m_pErrorEvent->sendDiagEventErrorResponse(eNO_ERROR_INFO,
                                                            l_srcName);
      LOG_STATUS_IF_ERRORED(l_eStatus, "m_pErrorEvent->sendDiagEventErrorResponse(l_srcName)");
      l_eStatus = eFrameworkunifiedStatusFail;
    } else if (eFrameworkunifiedStatusOK != (l_eStatus =
        SendDiagLoggingRequestToSystemManager(l_path))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
      // LCOV_EXCL_START 4: NSFW error case.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      /* call System Manager interface*/
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             " Error. SendDiagLoggingRequestToSystemManager() returned %d.", l_eStatus);
      l_eStatus = m_pErrorEvent->sendDiagEventErrorResponse(eNO_ERROR_INFO);
      LOG_STATUS_IF_ERRORED(l_eStatus, "m_pErrorEvent->sendDiagEventErrorResponse()");
      l_eStatus = eFrameworkunifiedStatusFail;
      // LCOV_EXCL_STOP
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "Log Request passed to SystemManager.");
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

EFrameworkunifiedStatus CLoggerServiceCallbacks::LoggerServer_OnCANTrigger(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  l_eStatus = SendCANLoggingRequestToSystemManager();
  LOG_STATUS_IF_ERRORED(l_eStatus, "SendCANLoggingRequestToSystemManager()");  // LCOV_EXCL_BR_LINE 15:macro
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

EFrameworkunifiedStatus CLoggerServiceCallbacks::LoggerServer_OnDTCTrigger(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  UI_32 l_dtc;
  if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedGetMsgDataOfSize(f_hApp, &l_dtc, sizeof(l_dtc), eSMRRelease))) {  // LCOV_EXCL_BR_LINE 4:NSFW  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 4:NSFW
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. Could not get msg Data.");
    // LCOV_EXCL_STOP
  } else {
    l_eStatus = SendDTCLoggingRequestToSystemManager(l_dtc);
    LOG_STATUS_IF_ERRORED(l_eStatus, "SendDTCLoggingRequestToSystemManager(dtc)");  // LCOV_EXCL_BR_LINE 15:macro
    // Immediate Persist Event Log
    if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedSendChild(f_hApp, g_thrdEvntLogWriter, eThrdCmdImmPersistEvtLog, 0x00, NULL))) {  // LCOV_EXCL_BR_LINE 4:NSFW  // NOLINT[whitespace/line_length]
      // LCOV_EXCL_START 4:NSFW
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "FrameworkunifiedSendChild msg failed:%X", l_eStatus);
      // LCOV_EXCL_STOP
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

EFrameworkunifiedStatus CLoggerServiceCallbacks::LoggerServer_On_SHTDWN_Complete(
    HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  l_eStatus = LoggerService_OnStop(f_hApp);
  LOG_STATUS_IF_ERRORED(l_eStatus, "LoggerService_OnStop()");  // LCOV_EXCL_BR_LINE 15:macro
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////
/// Function :LoggerServer_OnSetVIN
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CLoggerServiceCallbacks::LoggerServer_OnSetVIN(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  STVIN_NUMBER l_stVIN_Number;

  if (eFrameworkunifiedStatusOK == (l_eStatus = FrameworkunifiedGetMsgDataOfSize(hApp, &l_stVIN_Number, sizeof(l_stVIN_Number), eSMRRelease))) {  // LCOV_EXCL_BR_LINE 4:NSFW  // NOLINT[whitespace/line_length]
    if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedSendChild(hApp, g_thrdEvntLogWriter, eThrdCmdSetVINnumber, sizeof(STVIN_NUMBER), &l_stVIN_Number))) {  // LCOV_EXCL_BR_LINE 4:NSFW  // NOLINT[whitespace/line_length]
      // LCOV_EXCL_START 4:NSFW
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "FrameworkunifiedSendChild msg failed:%X", l_eStatus);
      // LCOV_EXCL_STOP
    }
  } else {
    // LCOV_EXCL_START 4:NSFW
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "FrameworkunifiedGetMsgDataOfSize failed errval: %X",
           l_eStatus);
    // LCOV_EXCL_STOP
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////
/// Function :LoggerServer_OnCopyEvntLog
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CLoggerServiceCallbacks::LoggerServer_OnCopyEvntLog(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  TWriteFilesToUsbCmdData l_stCmdData;
  HANDLE l_hSession = NULL;

  if (hApp != NULL) {  // LCOV_EXCL_BR_LINE 6: it can not be null
    PCSTR l_tmp_srcname = FrameworkunifiedGetMsgSrc(hApp);
    if (NULL != l_tmp_srcname) {  // LCOV_EXCL_BR_LINE 6: it can not be null
      l_stCmdData.stSessiondata.strSrcName = l_tmp_srcname;
    }
    if (l_stCmdData.stSessiondata.strSrcName.empty()) {  // LCOV_EXCL_BR_LINE 6: it can not be empty
      // LCOV_EXCL_START 6: it can not be empty
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_eStatus = eFrameworkunifiedStatusNullPointer;
      LOG_ERROR("FrameworkunifiedGetMsgSrc()");
      // LCOV_EXCL_STOP
    } else if (NULL == (l_hSession = FrameworkunifiedGetCurrentSessionHandle(hApp))) {  // LCOV_EXCL_BR_LINE 6: it can not be null
      // LCOV_EXCL_START 6: it can not be null
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_eStatus = eFrameworkunifiedStatusNullPointer;
      LOG_ERROR("FrameworkunifiedGetCurrentSessionHandle()");
      // LCOV_EXCL_STOP
    } else {
      l_stCmdData.stSessiondata.session_id = FrameworkunifiedGetSessionId(l_hSession);
      if (eFrameworkunifiedStatusOK
          != (l_eStatus = ReadMsg<EEvtLogUSBDevNumber>(hApp, l_stCmdData.eDevId))) {
        LOG_ERROR("ReadMsg()");
      } else {
        if (eFrameworkunifiedStatusOK
            != (l_eStatus = FrameworkunifiedSendChild(hApp, g_thrdEvntLogWriter,  // LCOV_EXCL_BR_LINE 4:NSFW
                                         eThrdCmdWriteEvntFilesToUsb,
                                         sizeof(TWriteFilesToUsbCmdData),
                                         &l_stCmdData))) {
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          LOG_ERROR("FrameworkunifiedSendChild()");  // LCOV_EXCL_LINE 4:NSFW
        }
      }
    }
  } else {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Invalid App handler");  // LCOV_EXCL_LINE 6: it can not be null
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////
/// Function :LoggerServer_OnClearEvntLog
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CLoggerServiceCallbacks::LoggerServer_OnClearEvntLog(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (hApp != NULL) {  // LCOV_EXCL_BR_LINE 200:As it is not always NULL
    TSessionData l_sessiondata;
    HANDLE l_hSession = NULL;
    PCSTR l_tmp_srcname = FrameworkunifiedGetMsgSrc(hApp);  // LCOV_EXCL_BR_LINE 11:Unexpected branch
    if (NULL != l_tmp_srcname) {  // LCOV_EXCL_BR_LINE 4:NSFW
      l_sessiondata.strSrcName = l_tmp_srcname;
    }
    if (l_sessiondata.strSrcName.empty()) {  // LCOV_EXCL_BR_LINE 4:As it is guaranteed by NSFW and is not always an empty string
      // LCOV_EXCL_START 4:As it is guaranteed by NSFW and is not always an empty string
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_eStatus = eFrameworkunifiedStatusNullPointer;
      LOG_ERROR("FrameworkunifiedGetMsgSrc()");
      // LCOV_EXCL_STOP
    } else if (NULL == (l_hSession = FrameworkunifiedGetCurrentSessionHandle(hApp))) {  // LCOV_EXCL_BR_LINE 4:NSFW
      // LCOV_EXCL_START 4:NSFW
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_eStatus = eFrameworkunifiedStatusNullPointer;
      LOG_ERROR("FrameworkunifiedGetCurrentSessionHandle()");
      // LCOV_EXCL_STOP
    } else {
      l_sessiondata.session_id = FrameworkunifiedGetSessionId(l_hSession);  // LCOV_EXCL_BR_LINE 11:Unexpected branch
      if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedSendChild(hApp, g_thrdEvntLogWriter, eThrdCmdClearEvntLogs, sizeof(TSessionData), &l_sessiondata))) {  // LCOV_EXCL_BR_LINE 4:NSFW  // NOLINT[whitespace/line_length]
        // LCOV_EXCL_START 4:NSFW
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        LOG_ERROR("FrameworkunifiedSendChild()");
        // LCOV_EXCL_STOP
      }
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////
/// Function :LoggerServer_OnReadStatCounter
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CLoggerServiceCallbacks::LoggerServer_OnReadStatCounter(
    HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (hApp != NULL) {  // LCOV_EXCL_BR_LINE 6: it can not be null
    TStatisticalCountersCmd l_stCmdData;
    HANDLE l_hSession = NULL;

    PCSTR l_tmp_srcname = FrameworkunifiedGetMsgSrc(hApp);
    if (NULL != l_tmp_srcname) {  // LCOV_EXCL_BR_LINE 6: it can not be null
      l_stCmdData.stSessiondata.strSrcName = l_tmp_srcname;
    }
    if (l_stCmdData.stSessiondata.strSrcName.empty()) {  // LCOV_EXCL_BR_LINE 6: it can not be empty
      // LCOV_EXCL_START 6: it can not be empty
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_eStatus = eFrameworkunifiedStatusNullPointer;
      LOG_ERROR("FrameworkunifiedGetMsgSrc()");
      // LCOV_EXCL_STOP
    } else if (NULL == (l_hSession = FrameworkunifiedGetCurrentSessionHandle(hApp))) {  // LCOV_EXCL_BR_LINE 6: it can not be null
      // LCOV_EXCL_START 6: it can not be empty
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_eStatus = eFrameworkunifiedStatusNullPointer;
      LOG_ERROR("FrameworkunifiedGetCurrentSessionHandle()");
      // LCOV_EXCL_STOP
    } else {
      l_stCmdData.stSessiondata.session_id = FrameworkunifiedGetSessionId(l_hSession);
      if (eFrameworkunifiedStatusOK
          != (l_eStatus = ReadMsg<EStatCounterGroupID>(hApp,
                                                       l_stCmdData.eGroupId))) {
        LOG_ERROR("ReadMsg()");
      } else {
        if (eFrameworkunifiedStatusOK
            != (l_eStatus = FrameworkunifiedSendChild(hApp, g_thrdEvntLogWriter,  // LCOV_EXCL_BR_LINE 4:NSFW
                                         eThrdCmdStatisticalCounter,
                                         sizeof(TStatisticalCountersCmd),
                                         &l_stCmdData))) {
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          LOG_ERROR("FrameworkunifiedSendChild()");// LCOV_EXCL_LINE 4:NSFW
        }
      }
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////
/// Function :LoggerServer_OnResetStatCounter
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CLoggerServiceCallbacks::LoggerServer_OnResetStatCounter(
    HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (hApp != NULL) {  // LCOV_EXCL_BR_LINE 6: it can not be null
    TStatisticalCountersCmd l_stCmdData;
    HANDLE l_hSession = NULL;
    PCSTR l_tmp_srcname = FrameworkunifiedGetMsgSrc(hApp);
    if (NULL != l_tmp_srcname) {  // LCOV_EXCL_BR_LINE 6: it can not be null
      l_stCmdData.stSessiondata.strSrcName = l_tmp_srcname;
    }
    if (l_stCmdData.stSessiondata.strSrcName.empty()) {  // LCOV_EXCL_BR_LINE 6: it can not be empty
      // LCOV_EXCL_START 6: it can not be empty
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_eStatus = eFrameworkunifiedStatusNullPointer;
      LOG_ERROR("FrameworkunifiedGetMsgSrc()");
      // LCOV_EXCL_STOP
    } else if (NULL == (l_hSession = FrameworkunifiedGetCurrentSessionHandle(hApp))) {  // LCOV_EXCL_BR_LINE 6: it can not be null
      // LCOV_EXCL_START 6: it can not be empty
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_eStatus = eFrameworkunifiedStatusNullPointer;
      LOG_ERROR("FrameworkunifiedGetCurrentSessionHandle()");
      // LCOV_EXCL_STOP
    } else {
      l_stCmdData.stSessiondata.session_id = FrameworkunifiedGetSessionId(l_hSession);
      if (eFrameworkunifiedStatusOK
          != (l_eStatus = ReadMsg<EStatCounterGroupID>(hApp,
                                                       l_stCmdData.eGroupId))) {
        LOG_ERROR("ReadMsg()");
      } else {
        if (eFrameworkunifiedStatusOK
            != (l_eStatus = FrameworkunifiedSendChild(hApp, g_thrdEvntLogWriter,  // LCOV_EXCL_BR_LINE 4:NSFW
                                         eThrdCmdResetStatisticalCntrs,
                                         sizeof(TStatisticalCountersCmd),
                                         &l_stCmdData))) {
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          LOG_ERROR("FrameworkunifiedSendChild()");  // LCOV_EXCL_LINE 4:NSFW
        }
      }
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////
/// Function :LoggerServer_OnReadNumberOfEvents
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CLoggerServiceCallbacks::LoggerServer_OnReadNumberOfEvents(
    HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  TSessionData l_sessiondata;
  HANDLE l_hSession = NULL;

  l_sessiondata.strSrcName = FrameworkunifiedGetMsgSrc(hApp);
  if (l_sessiondata.strSrcName.empty()) {  // LCOV_EXCL_BR_LINE 6: it can not be empty
    // LCOV_EXCL_START 6: it can not be empty
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_eStatus = eFrameworkunifiedStatusNullPointer;
    LOG_ERROR("FrameworkunifiedGetMsgSrc()");
    // LCOV_EXCL_STOP
  } else if (NULL == (l_hSession = FrameworkunifiedGetCurrentSessionHandle(hApp))) {  // LCOV_EXCL_BR_LINE 6: it can not be null
    // LCOV_EXCL_START 6: it can not be empty
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_eStatus = eFrameworkunifiedStatusNullPointer;
    LOG_ERROR("FrameworkunifiedGetCurrentSessionHandle()");
    // LCOV_EXCL_STOP
  } else {
    l_sessiondata.session_id = FrameworkunifiedGetSessionId(l_hSession);
    if (eFrameworkunifiedStatusOK
        != (l_eStatus = FrameworkunifiedSendChild(hApp, g_thrdEvntLogWriter,  // LCOV_EXCL_BR_LINE 4:NSFW
                                     eThrdCmdGetNumberOfEventsLogged,
                                     sizeof(TSessionData), &l_sessiondata))) {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      LOG_ERROR("FrameworkunifiedSendChild()");  // LCOV_EXCL_LINE 4:NSFW
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////
/// Function :LoggerServer_OnSetDiagID
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CLoggerServiceCallbacks::LoggerServer_OnSetDiagID(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  UI_16 l_DiagId = 0;

  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg<UI_16>(hApp, l_DiagId))) {  // LCOV_EXCL_BR_LINE 200:To ensure success
    // LCOV_EXCL_START 200:To ensure success
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("ReadMsg()");
    // LCOV_EXCL_STOP
  } else {
    g_u16DiagId = l_DiagId;
  }
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////
/// Function :LoggerServer_UploadEventLog
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CLoggerServiceCallbacks::LoggerServer_UploadEventLog(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  TSessionData l_sessiondata;
  HANDLE l_hSession = NULL;

  PCSTR l_tmp_srcname = FrameworkunifiedGetMsgSrc(hApp);
  if (NULL != l_tmp_srcname) {  // LCOV_EXCL_BR_LINE 6: it can not be null
    l_sessiondata.strSrcName = l_tmp_srcname;
  }
  if (l_sessiondata.strSrcName.empty()) {  // LCOV_EXCL_BR_LINE 6: it can not be empty
    // LCOV_EXCL_START 6: it can not be empty
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_eStatus = eFrameworkunifiedStatusNullPointer;
    LOG_ERROR("FrameworkunifiedGetMsgSrc()");
    // LCOV_EXCL_STOP
  } else if (NULL == (l_hSession = FrameworkunifiedGetCurrentSessionHandle(hApp))) {  // LCOV_EXCL_BR_LINE 6: it can not be null
    // LCOV_EXCL_START 6: it can not be null
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_eStatus = eFrameworkunifiedStatusNullPointer;
    LOG_ERROR("FrameworkunifiedGetCurrentSessionHandle()");
    // LCOV_EXCL_STOP
  } else {
    l_sessiondata.session_id = FrameworkunifiedGetSessionId(l_hSession);
    if (eFrameworkunifiedStatusOK
        != (l_eStatus = FrameworkunifiedSendChild(hApp, g_thrdEvntLogWriter,  // LCOV_EXCL_BR_LINE 4:NSFW
                                     eThrdCmdUploadEventLog,
                                     sizeof(TSessionData), &l_sessiondata))) {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      LOG_ERROR("FrameworkunifiedSendChild()");  // LCOV_EXCL_LINE 4:NSFW
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}
