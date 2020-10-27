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
/// \brief    This file supports error event logging.
///
///////////////////////////////////////////////////////////////////////////////
#include "ss_logger_error_event.h"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <boost/bind.hpp>
#include <unistd.h>
#include <sys/wait.h>
#include <native_service/frameworkunified_framework_if.h>
#include <system_service/ss_sm_client_if.h>
#include <system_service/ss_client_names.h>
#include <system_service/ss_logger_service_protocol.h>
#include <system_service/ss_logger_service.h>
#include <system_service/ss_services.h>
#include <system_service/ss_templates.h>
#include <stub/el_mem.h>
#include <native_service/cl_process.h>
#include <stub/Clock_API.h>
#include <string>
#include "loggerservicedebug_loggerservicelog.h"
#include "loggerservicedebug_thread_if.h"
#include "ss_logger_service_callbacks.h"
#include "ss_logger_error_event_archive.h"
#include "ss_logger_common.h"
#include "ss_logger_types.h"
#include "ss_logger_fs_directory.h"

CErrorEvent::CErrorEvent()
    : m_hApp(NULL),
      m_bIsPrevEventCompleted(true),
      m_errorEventNtfData(),
      m_pLoggerCfg(NULL),
      m_pReaderWriterControl(NULL),
      m_ServiceName(""),
      m_time(0),
      m_currentEventTriggerNumber(0),
      m_diagsessionhandle(NULL),
      m_thrdEvntLogWriter(NULL),
      m_sfd(-1) {  // LCOV_EXCL_BR_LINE 11:Unexpected branch
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}  // LCOV_EXCL_BR_LINE 11:Unexpected branch

CErrorEvent::~CErrorEvent() {  // LCOV_EXCL_START 14:globle instance
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus CErrorEvent::Initialize(HANDLE f_hApp, CLoggerCfg *f_pLoggerCfg,
                                   CReaderWriterControl *f_pReaderWriterControl,
                                   HANDLE f_thrdEvntLogWriter,
                                   std::string f_strEvntLogQueWorkerName) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  Timer *l_pTimer;

  m_sfd = CL_ProcessInit();
  if (m_sfd == -1) {  // LCOV_EXCL_BR_LINE 8:Because the process initialization is guaranteed to be successful
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. m_sfd CL_ProcessInit() failed \n.");  // LCOV_EXCL_LINE 8:Because the process initialization is guaranteed to be successful
  }

  SS_LoggerServerEvents l_errorEventStateEvents[] = {
      SS_LOGGER_SCREENCAPTURE_EVT };

  FrameworkunifiedProtocolCallbackHandler l_smCallbacks[] = {
  // Logging Protocol Callbacks
      { SS_SM_EVENT_ERROR_TO_SSL, boost::bind(&CErrorEvent::OnStartLogging,
                                              this, _1) }, {
          SS_SM_ERROR_EVENT_LOGGING_START_RSPN, boost::bind(
              &CErrorEvent::OnLogStartResponse, this, _1) }, {
          SS_SM_ERROR_EVENT_ARTIFACT_RSPN, boost::bind(
              &CErrorEvent::OnArtifactResponse, this, _1) } };  // LCOV_EXCL_BR_LINE 11:Unexpected branch

  FrameworkunifiedProtocolCallbackHandler l_selfCallbacks[] = {
  // Logging Protocol Callbacks
      { SS_LOGGER_ERROR_EVENT_ARTIFACT_REQ, boost::bind(
          &CErrorEvent::OnArtifactRequest, this, _1) }, {
          SS_SM_ERROR_EVENT_ARTIFACT_RSPN, boost::bind(
              &CErrorEvent::OnArtifactResponse, this, _1) } };  // LCOV_EXCL_BR_LINE 11:Unexpected branch

  FrameworkunifiedProtocolCallbackHandler l_requesterCallbacks[] = { {
      SS_LOGGER_SCREENCAPTURE_EVT_ACK, boost::bind(
          &CErrorEvent::OnObtainScreenShotResponse, this, _1) } };  // LCOV_EXCL_BR_LINE 11:Unexpected branch

  FrameworkunifiedProtocolCallbackHandler l_storageThreadCallbacks[] = { {
      eLoggerStorageThreadCmdOK, boost::bind(&CErrorEvent::OnStorageResponseOk,
                                             this, _1) }, {
      eLoggerStorageThreadCmdWriteFailed, boost::bind(
          &CErrorEvent::OnStorageResponseWriteFailed, this, _1) }, {
      eLoggerStorageThreadCmdNotFound, boost::bind(
          &CErrorEvent::OnStorageResponseNotFound, this, _1) }, {
      eLoggerStorageThreadCmdNoWritten, boost::bind(
          &CErrorEvent::OnStorageResponseNoWritten, this, _1) } };  // LCOV_EXCL_BR_LINE 11:Unexpected branch
  for (UI_32 i = 0; i < _countof(l_requesterCallbacks); i++) {
    m_requesterCallbacksVec.push_back(l_requesterCallbacks[i]);  // LCOV_EXCL_BR_LINE 11:Unexpected branch
  }

  if (NULL == f_hApp) {  // LCOV_EXCL_BR_LINE 6:As it is not always NULL
    // LCOV_EXCL_START 6:As it is not always NULL
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_eStatus = eFrameworkunifiedStatusInvldHandle;
    FRAMEWORKUNIFIEDLOG(
        ZONE_ERR, __FUNCTION__,
        " Error. Argument f_hApp passed NULL pointer. Initialization failed.");
    // LCOV_EXCL_STOP
  } else if (NULL == (m_thrdEvntLogWriter = f_thrdEvntLogWriter)) {  // LCOV_EXCL_BR_LINE 6:As it is not always NULL
    // LCOV_EXCL_START 6:As it is not always NULL
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_eStatus = eFrameworkunifiedStatusInvldHandle;
    FRAMEWORKUNIFIEDLOG(
        ZONE_ERR, __FUNCTION__,
        " Error. Argument f_hApp passed NULL pointer. Initialization failed.");
    // LCOV_EXCL_STOP
  } else if (eFrameworkunifiedStatusOK != (l_eStatus = m_errorEventCfg.Initialize(f_pLoggerCfg))) {  // LCOV_EXCL_BR_LINE 200:To ensure success
    // LCOV_EXCL_START 200:To ensure success
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           " Error. m_errorEventCfg.Initialize() returned: %d.", l_eStatus);
    // LCOV_EXCL_STOP
  } else if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedAttachCallbacksToDispatcher(f_hApp, SERVICE_LOGGER, l_selfCallbacks, _countof(l_selfCallbacks)))) {  // LCOV_EXCL_BR_LINE 4:NSFW  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 4:NSFW
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(
        ZONE_ERR,
        __FUNCTION__,
        " Error. FrameworkunifiedAttachCallbacksToDispatcher(SERVICE_SYSMANAGER) returned: %d.",
        l_eStatus);
    // LCOV_EXCL_STOP
  } else if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedAttachCallbacksToDispatcher(f_hApp, SERVICE_SYSMANAGER, l_smCallbacks, _countof(l_smCallbacks)))) {  // LCOV_EXCL_BR_LINE 4:NSFW  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 4:NSFW
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(
        ZONE_ERR,
        __FUNCTION__,
        " Error. FrameworkunifiedAttachCallbacksToDispatcher(SERVICE_SYSMANAGER) returned: %d.",
        l_eStatus);
    // LCOV_EXCL_STOP
  } else if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedDefineStateEvents(f_hApp, &l_errorEventStateEvents, static_cast<UI_32>(_countof(l_errorEventStateEvents))))) {  // LCOV_EXCL_BR_LINE 4:NSFW  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 4:NSFW
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(
        ZONE_ERR,
        __FUNCTION__,
        " Error. FrameworkunifiedDefineStateEvents(SS_LOGGER_SCREENCAPTURE_EVT) returned: %d",
        l_eStatus);
    // LCOV_EXCL_STOP
  } else if (NULL == f_pLoggerCfg) {  // LCOV_EXCL_BR_LINE 200:As it is not always NULL
    // LCOV_EXCL_START 200:As it is not always NULL
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_eStatus = eFrameworkunifiedStatusNullPointer;
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. f_pLoggerCfg passed NULL pointer.");
    // LCOV_EXCL_STOP
  } else if (eFrameworkunifiedStatusOK != (l_eStatus = m_loggerPopups.Initialize(f_pLoggerCfg))) {  // LCOV_EXCL_BR_LINE 200:To ensure success  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 200:To ensure success
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           " Error. m_loggerPopups.Initialize(f_pLoggerCfg) returned: %d.",
           l_eStatus);
    // LCOV_EXCL_STOP
  } else if (NULL == f_pReaderWriterControl) {  // LCOV_EXCL_BR_LINE 200:As it is not always NULL
    // LCOV_EXCL_START 200:As it is not always NULL
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_eStatus = eFrameworkunifiedStatusNullPointer;
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           " Error. Argument f_pReaderWriterControl passed NULL pointer.");
    // LCOV_EXCL_STOP
  } else if (eFrameworkunifiedStatusOK != (l_eStatus = m_loggerStorage.Initialize(f_hApp, AppName, f_pLoggerCfg))) {  // LCOV_EXCL_BR_LINE 200:To ensure success  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 200:To ensure success
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           " Error. m_loggerPopups.Initialize(f_pLoggerCfg) returned: %d.",
           l_eStatus);
    // LCOV_EXCL_STOP
  } else if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedAttachCallbacksToDispatcher( f_hApp, ERROR_EVENT_STORAGE_QUEUE_NAME, l_storageThreadCallbacks, _countof(l_storageThreadCallbacks)))) {  // LCOV_EXCL_BR_LINE 4:NSFW  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 4:NSFW
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(
        ZONE_ERR,
        __FUNCTION__,
        " Error. FrameworkunifiedAttachCallbacksToDispatcher(ERROR_EVENT_STORAGE_QUEUE_NAME) returned: %d.",
        l_eStatus);
    // LCOV_EXCL_STOP
  } else if (eFrameworkunifiedStatusOK != (l_eStatus = m_loggerCanEvent.Initialize(f_pLoggerCfg))) {  // LCOV_EXCL_BR_LINE 200:To ensure success  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 200:To ensure success
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           " Error. m_loggerCanEvent.Initialize() returned: %d.", l_eStatus);
    // LCOV_EXCL_STOP
  } else {
    m_hApp = f_hApp;
    m_pLoggerCfg = f_pLoggerCfg;
    m_pReaderWriterControl = f_pReaderWriterControl;
    l_eStatus = m_loggerUtil.Initialize(m_pLoggerCfg);  // LCOV_EXCL_BR_LINE 11:Unexpected branch
    LOG_STATUS_IF_ERRORED(l_eStatus,  // LCOV_EXCL_BR_LINE 5:macro
                          "CLoggerUtil::m_loggerUtil.Initialize(m_pLoggerCfg)");

    l_pTimer = &m_errorEventTimers[kSSL_ERROR_EVENT_TIMER_LOGGING_START_RSPN];
    l_pTimer->Initialize(
        f_hApp, SS_LOGGER_ERROR_EVENT_TIMER_ID_LOGGING_START_RSPN,
        boost::bind(&CErrorEvent::OnLogStartResponseTimeout, this, _1));  // LCOV_EXCL_BR_LINE 11:Unexpected branch

    l_pTimer = &m_errorEventTimers[kSSL_ERROR_EVENT_TIMER_ARTIFACT_RSPN];
    l_pTimer->Initialize(
        f_hApp, SS_LOGGER_ERROR_EVENT_TIMER_ID_ARTIFACT_RESPONSE,
        boost::bind(&CErrorEvent::OnArtifactResponseTimeout, this, _1));  // LCOV_EXCL_BR_LINE 11:Unexpected branch
    m_ServiceName = "";
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////
//  Function : RegisterSessionErrorEvent
//  brief    : Register session event. This function is called within the
//             context of the System Manager session acknowledgment handler.
///////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CErrorEvent::RegisterSessionErrorEvent(HANDLE f_hSession) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  l_eStatus = FrameworkunifiedRegisterEvent(f_hSession, SS_SM_EVENT_ERROR_TO_SSL);
  LOG_STATUS_IF_ERRORED(l_eStatus, "FrameworkunifiedRegisterEvent(SS_SM_EVENT_ERROR_TO_SSL)");  // LCOV_EXCL_BR_LINE 5:macro

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////
//  Function : OnOpenSession
//  brief    : Called when a requester opens a session with SS_Logger
//             service.  This function attaches error event callbacks
//             to the dispatcher with the source set to the requester.
///////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CErrorEvent::OnOpenSession(HANDLE f_hApp, PCSTR f_pRequesterName,
                                      HANDLE f_hSession) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  l_eStatus = FrameworkunifiedAttachCallbacksToDispatcher(f_hApp, f_pRequesterName,
                                             &m_requesterCallbacksVec[0],
                                             static_cast<UI_32>(m_requesterCallbacksVec.size()),
                                             f_hSession);

  LOG_STATUS_IF_ERRORED(l_eStatus, "FrameworkunifiedAttachCallbacksToDispatcher(f_hSession)");  // LCOV_EXCL_BR_LINE 5:macro

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////
//  Function : OnOpenSession
//  brief    : Called when a requester opens a session with SS_Logger
//             service.  This function attaches error event callbacks
//             to the dispatcher with the source set to the requester.
///////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CErrorEvent::OnCloseSession(HANDLE f_hApp, PCSTR f_pRequesterName,
                                       HANDLE f_hSession) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  UI_32 l_cmdIdArr[m_requesterCallbacksVec.size()]; // NOLINT (runtime/arrays)

  for (UI_32 i = 0; i < m_requesterCallbacksVec.size(); i++) {
    l_cmdIdArr[i] = m_requesterCallbacksVec[i].iCmd;
  }

  l_eStatus = FrameworkunifiedDetachCallbacksFromDispatcher(f_hApp, f_pRequesterName,
                                               l_cmdIdArr,
                                               static_cast<UI_32>(m_requesterCallbacksVec.size()),
                                               f_hSession);

  LOG_STATUS_IF_ERRORED(l_eStatus, "FrameworkunifiedAttachCallbacksToDispatcher()");  // LCOV_EXCL_BR_LINE 5:macro

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////
//  Function : OnStartLogging
//  brief    : Called by SS_Logger and external services to begin logging
//             artifact collection. This function sends a logging request
//             to System Manager. See SS_SM_EVENT_ERROR_TO_SSL.
///////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CErrorEvent::OnStartLogging(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  SMErrorEventNtfData l_eventErrorNtfData;

  INTERFACEUNIFIEDLOG_RECEIVED_FROM(f_hApp);  // LCOV_EXCL_BR_LINE 5:macro

  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg<SMErrorEventNtfData>(f_hApp, l_eventErrorNtfData))) {  // LCOV_EXCL_BR_LINE 200:To ensure success  // NOLINT[whitespace/line_length]
    // when failed to ReadMsg, we can't know event-type for action, so nothing is able to do.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("ReadMsg()");  // LCOV_EXCL_LINE 200:To ensure success
  } else {
    if (TRUE == this->m_pLoggerCfg->IsLoggingEnabled()) {  // LCOV_EXCL_BR_LINE 200:As it is always TRUE
      if (TRUE == m_bIsPrevEventCompleted) {
        TLoggerErrorEvent l_eventData = l_eventErrorNtfData;

        if (IsNeedVerify(l_eventData.EventType)) {
          // if need output log to external storage, check whether device is authenticated or not.
          VerifyExtStorage(l_eventData);
        }

        uint8_t status;
        Clock_getSystemTimeY2K38(&m_time, &status);
        m_currentEventTriggerNumber = m_pLoggerCfg->GetUserInvokedCounter();
        l_eStatus = m_loggerUtil.checkDestinationAvailable(l_eventData);
        if (eFrameworkunifiedStatusOK == l_eStatus) {
          m_bIsPrevEventCompleted = false; /* See timer start / timeout handler below.             */

          m_artifactResponseVec.clear();

          m_errorEventNtfData = l_eventErrorNtfData; /* Overwrite m_eventErrorNtfData when logging complete. */
          m_errorEventNtfData.EventBitMask = m_errorEventCfg
              .GetEventsForErrorEvent(l_eventErrorNtfData.EventType);
          l_eStatus = SendLogStartRequestToSystemManager(
              m_errorEventNtfData.EventType);
          LOG_STATUS_IF_ERRORED(l_eStatus,
                                "SendLogStartRequestToSystemManager()");
          l_eStatus = m_loggerCanEvent.PublishStartEvent(f_hApp,
                                                         m_errorEventNtfData);
          LOG_STATUS_IF_ERRORED(
              l_eStatus,
              "m_loggerCanEvent.PublishStartEvent(f_hApp,m_errorEventNtfData);");

          bool result =
              m_errorEventTimers[kSSL_ERROR_EVENT_TIMER_LOGGING_START_RSPN]
                  .Start(m_loggingStartRspnToSec, 0, 0, 0);
          if (FALSE == result) {  // LCOV_EXCL_BR_LINE 200:As it is always TRUE
            // LCOV_EXCL_START 200:As it is always TRUE
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(
                ZONE_ERR,
                __FUNCTION__,
                " Error. Failed to start timer kSSL_ERROR_EVENT_TIMER_LOGGING_START_RSPN.");
            // LCOV_EXCL_STOP
          }
        } else {
          FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__,
                 " Warning. Destination not available. ");
          m_errorEventNtfData = l_eventErrorNtfData; /* Overwrite m_eventErrorNtfData when logging complete. */
          m_errorEventNtfData.EventBitMask = m_errorEventCfg
              .GetEventsForErrorEvent(l_eventErrorNtfData.EventType);
          l_eStatus = OnStorageResponseNotFound(f_hApp);
          LOG_STATUS_IF_ERRORED(l_eStatus, "OnStorageResponseNotFound(f_hApp)");
        }
      } else {
        // Not-Need Resp to SM, because of keep logging sequance.
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               " Error. Logging already in progress. Event dropped.");
      }
    } else {
      // LCOV_EXCL_START 200:As it is always TRUE
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      if (l_eventErrorNtfData.isNeedReboot == TRUE) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "System will reboot soon, so stop logging function.");
        StopLoggingFunction(f_hApp);
      }
      FRAMEWORKUNIFIEDLOG(
          ZONE_INFO,
          __FUNCTION__,
          "Logging is disabled! Dropping Event. Sending Complete to SS_SystemManager");
      l_eStatus = SendLogCompleteRequestToSystemManager(eFrameworkunifiedStatusFail);
      LOG_STATUS_IF_ERRORED(l_eStatus,
                            "SendLogCompleteRequestToSystemManager()");
      // LCOV_EXCL_STOP
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////
//  Function : OnLogStartResponse
//  brief    : Called by System Manager after the acknowledgment of the
//             logging session start request.  This function starts the
//             artifact collection process. See
//             SS_SM_ERROR_EVENT_LOGGING_START_RSPN.
///////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CErrorEvent::OnLogStartResponse(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  m_errorEventTimers[kSSL_ERROR_EVENT_TIMER_LOGGING_START_RSPN].Stop();
  m_errorEventCfg.GetArtifactRequestVec(m_errorEventNtfData.EventType,
                                        m_artifactRequestVec);

  l_eStatus = RequestNextArtifact(f_hApp);
  LOG_STATUS_IF_ERRORED(l_eStatus, "RequestNextArtifact()");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////
//  Function : OnLogStartResponseTimeout
//  brief    : Called when / if SM fails to respond to a logging start
//             request and the logging star response timer expires.
///////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CErrorEvent::OnLogStartResponseTimeout(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  FRAMEWORKUNIFIEDLOG(
      ZONE_ERR,
      __FUNCTION__,
      " Error. SM failed to respond to a logging start request in the required amount of time.");

  m_bIsPrevEventCompleted = TRUE;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////
//  Function : RequestNextArtifact
//  brief    : This function requests the next artifact from the service
//             that owns the artifact.  If no artifacts remain, this
//             function performs initiates artifact post processing.
///////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CErrorEvent::RequestNextArtifact(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  UI_32 l_timeoutSec;
  UI_32 l_timeoutMs;

  if (0 != m_artifactRequestVec.size()) {
    TLoggingArtifact &l_nextArtifact = m_artifactRequestVec.back();
    l_eStatus = m_loggerPopups.ShowStartPopup(
        f_hApp, m_errorEventNtfData, &m_artifactRequestVec,
        m_loggerUtil.getDestination(m_errorEventNtfData));

    LOG_STATUS_IF_ERRORED(
        l_eStatus,
        "m_loggerPopups.ShowStartPopup(f_hApp,m_errorEventNtfData,&m_artifactRequestVec )");

    TEXT(__FUNCTION__,
         "Requesting next artifact ID:%d from:%s, with timeout:%d ms.",
         l_nextArtifact.ArtifactId,
         l_nextArtifact.Information.OwnerServiceName.c_str(),
         l_nextArtifact.Information.RequestTimeoutMs);

    l_timeoutSec = l_nextArtifact.Information.RequestTimeoutMs / 1000;
    l_timeoutMs = l_nextArtifact.Information.RequestTimeoutMs % 1000;
    bool result =
        m_errorEventTimers[kSSL_ERROR_EVENT_TIMER_ARTIFACT_RSPN].Start(
            l_timeoutSec, l_timeoutMs, 0, 0);
    if (FALSE == result) {  // LCOV_EXCL_BR_LINE 200:As it is always TRUE
      // LCOV_EXCL_START 200:As it is always TRUE
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Failed to start timer kSSL_ERROR_EVENT_TIMER_ARTIFACT_RSPN.");
      // LCOV_EXCL_STOP
    }
    switch (l_nextArtifact.Information.Cb.Type) {
      case eCbTypePath: {
        l_eStatus = CheckPathForArtifact(f_hApp, l_nextArtifact);
      }
        break;

      case eCbTypeFunction: {
        if (l_nextArtifact.Information.Cb.Function == NULL) {  // LCOV_EXCL_BR_LINE 200:As it is not always NULL
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          l_eStatus = eFrameworkunifiedStatusNullPointer;  // LCOV_EXCL_LINE 200:As it is not always NULL
        } else {
          l_eStatus = l_nextArtifact.Information.Cb.Function(f_hApp,
                                                             l_nextArtifact);
          LOG_STATUS_IF_ERRORED(
              l_eStatus,
              "l_nextArtifact.Information.Cb.Function(f_hApp, l_nextArtifact)");
        }
      }
        break;

      case eCbTypeInvalid:
      default:
      {
        FRAMEWORKUNIFIEDLOG(
            ZONE_ERR,
            __FUNCTION__,
            " Error. Unknown artifact or artifact owner for artifact ID:"
            " %d. Invalid logging artifact configuration likely.",
            l_nextArtifact.ArtifactId);
      }
        break;
    }
  } else {
    if ((m_errorEventNtfData.EventType == eErrorEventTypeUserInvokedClearLogs)
        || (m_errorEventNtfData.EventType
            == eErrorEventTypeUserInvokedCollectNaviLog)) {
      l_eStatus = this->OnStorageResponseOk(f_hApp);
      return (l_eStatus);
    }
    uint8_t status;
    Clock_getSystemTimeY2K38(&m_time, &status);
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
           " Info. All logging error event artifacts have been collected.");
    l_eStatus = m_loggerUtil.checkDestinationAvailable(m_errorEventNtfData);
    LOG_STATUS_IF_ERRORED(
        l_eStatus,
        "m_loggerUtil.checkDestinationAvailable(m_errorEventNtfData)");
    if (eFrameworkunifiedStatusOK == l_eStatus) {
      /* Check one last time after all logs have been collected   */
      /* if we should show popup for archiving and writing to USB */
      l_eStatus = m_loggerPopups.ShowStartPopup(
          f_hApp, m_errorEventNtfData, &m_artifactRequestVec,
          m_loggerUtil.getDestination(m_errorEventNtfData));
      LOG_STATUS_IF_ERRORED(
          l_eStatus,
          "m_loggerPopups.ShowStartPopup(f_hApp,m_errorEventNtfData,&m_artifactRequestVec )");

      l_eStatus = m_loggerUtil.getFilePathAndName(f_hApp, m_errorEventNtfData,
                                                  m_time, m_archiveDestination);
      LOG_STATUS_IF_ERRORED(
          l_eStatus,
          "m_loggerUtil.getFilePathAndName()");

      std::string destination_path = m_loggerUtil.getDestination(
          m_errorEventNtfData);
      if (destination_path.compare(m_pLoggerCfg->m_emmcOutputPath) == 0) {
        if (eFrameworkunifiedStatusOK
            != m_loggerUtil.deleteOldEmmcLog(m_archiveDestination,
                                             m_errorEventNtfData.EventType)) {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                 " Error. m_loggerUtil.deleteOldEmmcLog()");
        }
      }

      l_eStatus = this->m_loggerStorage.Start(m_archiveDestination,
                                              this->m_artifactResponseVec,
                                              m_time);
      LOG_STATUS_IF_ERRORED(
          l_eStatus,
          "m_loggerStorage.Start()");
      if (l_eStatus != eFrameworkunifiedStatusOK) {
        l_eStatus = this->OnStorageResponseWriteFailed(f_hApp);
        LOG_STATUS_IF_ERRORED(l_eStatus,
                              "this->OnStorageResponseWriteFailed(f_hApp)");
      }
    } else {
      l_eStatus = this->OnStorageResponseNotFound(f_hApp);
      LOG_STATUS_IF_ERRORED(
          l_eStatus,
          "this->OnStorageResponseNotFound(f_hApp)");
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////
//  Function : OnArtifactResponse
//  brief    : This function is called when an artifact has been made ready
//             by the owning service.
///////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CErrorEvent::OnArtifactResponse(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  ARTIFACT_RESPONSE l_artifactResponse;

  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg<ARTIFACT_RESPONSE>(f_hApp, l_artifactResponse))) {  // LCOV_EXCL_BR_LINE 200:To ensure success  // NOLINT[whitespace/line_length]
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("ReadMsg()");  // LCOV_EXCL_LINE 200:To ensure success
  } else {
    m_errorEventTimers[kSSL_ERROR_EVENT_TIMER_ARTIFACT_RSPN].Stop();

    // Prevent processing of artifacts that arrive after the error event logging session has completed.
    if (m_artifactRequestVec.size() > 0) {
      TEXT(__FUNCTION__, "Artifact ID:%d received.",
           l_artifactResponse.ArtifactId);

      TLoggingArtifact &l_artifactCurr = m_artifactRequestVec.back();
      if (l_artifactCurr.ArtifactId != l_artifactResponse.ArtifactId) {
        TEXT(
            __FUNCTION__,
            "Failure collecting current artifact. Expected artifact ID:%d, received artifact ID:%d."
            " Ignoring received artifact.",
            l_artifactCurr.ArtifactId, l_artifactResponse.ArtifactId);
      } else {
        TLoggingArtifactResponse l_response;
        l_response.ArtifactId = l_artifactResponse.ArtifactId;
        l_response.Filepath = l_artifactResponse.FilePathAndName;
        l_response.Remove = l_artifactCurr.Information.Remove;
        m_artifactResponseVec.push_back(l_response);
        m_artifactRequestVec.pop_back();  // Remove received artifact from request vector.
        l_eStatus = RequestNextArtifact(f_hApp);
        LOG_STATUS_IF_ERRORED(l_eStatus, "RequestNextArtifact()");
      }
    } else {
      FRAMEWORKUNIFIEDLOG(
          ZONE_ERR,
          __FUNCTION__,
          " Error. Received spurious artifact ID: %d. No error event logging session active.",
          l_artifactResponse.ArtifactId);  // LCOV_EXCL_BR_LINE 5:macro
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////
//  Function : OnArtifactResponseTimeout
//  brief    : This function is called when a requested artifact response
//             fails to arrive in the required amount of time.
///////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CErrorEvent::OnArtifactResponseTimeout(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (m_artifactRequestVec.size() > 0) {  // LCOV_EXCL_BR_LINE 200:As it cannot always be 0
    TLoggingArtifact &l_artifactCurr = m_artifactRequestVec.back();

    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           " Error. Artifact response timer expired for artifact ID: %d.",
           l_artifactCurr.ArtifactId);

    // Remove artifact from request vector.
    m_artifactRequestVec.pop_back();

    RequestNextArtifact(f_hApp);
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////
//  Function : OnArtifactRequest
//  brief    : This function services an artifact request generated by
//             the error event sub system.  The requested artifact will
//             be one of the artifacts owned by SSL.
///////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CErrorEvent::OnArtifactRequest(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  EArtifactId l_requestedArtifactId;

  if (eFrameworkunifiedStatusOK
      != (l_eStatus = ReadMsg<EArtifactId>(f_hApp, l_requestedArtifactId))) {
    LOG_ERROR("ReadMsg()");
  } else {
    switch (l_requestedArtifactId) {
      case eArtifactIdInterfaceunifiedDebugLog:
        l_eStatus = OnObtainLoggerserviceLogRequest(f_hApp);
        LOG_STATUS_IF_ERRORED(l_eStatus, "OnObtainLoggerserviceLogRequest()");
        break;

      case eArtifactIdTransmitLog:
        l_eStatus = OnObtainTransmitLogRequest(f_hApp);
        LOG_STATUS_IF_ERRORED(l_eStatus, "OnObtainTransmitLogRequest()");
        break;

      case eArtifactIdPerformanceLog:
        l_eStatus = OnObtainPerformanceLogRequest(f_hApp);
        LOG_STATUS_IF_ERRORED(l_eStatus, "OnObtainPerformanceLogRequest()");
        break;

      case eArtifactIdScreenShot:
        l_eStatus = OnObtainScreenShotRequest(f_hApp);
        LOG_STATUS_IF_ERRORED(l_eStatus, "OnObtainScreenShotRequest()");
        break;

      case eArtifactIdKernelLog:
        l_eStatus = OnObtainKernelLogInfoRequest(f_hApp);
        LOG_STATUS_IF_ERRORED(l_eStatus, "OnObtainKernelLogInfoRequest()");
        break;

      case eArtifactIdDRInitialLog:
        l_eStatus = OnObtainDRInitialLogRequest(f_hApp);
        LOG_STATUS_IF_ERRORED(l_eStatus, "OnObtainDRInitialLogRequest()");
        break;

      case eArtifactIdDRLocationLog:
        l_eStatus = OnObtainDRLocationLogRequest(f_hApp);
        LOG_STATUS_IF_ERRORED(l_eStatus, "OnObtainDRLocationLogRequest()");
        break;

      case eArtifactIdClearAllLog:
        l_eStatus = OnClearAllLogRequest(f_hApp);
        LOG_STATUS_IF_ERRORED(l_eStatus, "OnClearAllLogRequest()");
        break;

      case eArtifactIdNaviLog:
        l_eStatus = OnNaviLogRequest(f_hApp);
        LOG_STATUS_IF_ERRORED(l_eStatus, "OnNaviLogRequest()");
        break;

      default:
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               " Error. Unsupported logging artifact requested: %d.",
               l_requestedArtifactId);
        break;
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////
//  Function : OnStorageResponseOk
//  brief    : This function services response from the storage thread. It
//             is called when the logs have been stored successfully.
///////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CErrorEvent::OnStorageResponseOk(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  l_eStatus = this->m_loggerPopups.ShowEndPopup(f_hApp, m_errorEventNtfData,
                                                m_archiveDestination);
  LOG_STATUS_IF_ERRORED(
      l_eStatus,
      "this->m_loggerPopups.ShowEndPopup(f_hApp,m_errorEventNtfData,m_archiveDestination)");

  if (m_errorEventNtfData.EventType == eErrorEventTypeDiagEvent) {
    l_eStatus = sendDiagEventResponse();
    LOG_STATUS_IF_ERRORED(l_eStatus, "sendDiagEventResponse()");
  }
  if (m_errorEventNtfData.isNeedReboot == TRUE) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "System will reboot soon, so stop logging function.");
    StopLoggingFunction(f_hApp);
  }
  m_bIsPrevEventCompleted = true;
  l_eStatus = SendLogCompleteRequestToSystemManager(eFrameworkunifiedStatusOK);

  l_eStatus = m_loggerCanEvent.PublishEndEvent(f_hApp, m_errorEventNtfData,
                                               m_currentEventTriggerNumber,
                                               m_time);
  LOG_STATUS_IF_ERRORED(l_eStatus, "m_loggerCanEvent.PublishEndEvent();");
  LOG_STATUS_IF_ERRORED(l_eStatus, "SendLogCompleteRequestToSystemManager()");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////
//  Function : OnStorageResponseWriteFailed
//  brief    : This function services response from the storage thread. It
//             is called when the logs failed to be written to the target
//             destination.
///////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CErrorEvent::OnStorageResponseWriteFailed(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  l_eStatus = m_loggerPopups.ShowErrorPopup(f_hApp, m_errorEventNtfData,
                                            (UI_16) eWriteToDeviceFailed);
  LOG_STATUS_IF_ERRORED(
      l_eStatus, "m_loggerPopups.ShowErrorPopup(f_hApp, eWriteToDeviceFailed)");

  if (m_errorEventNtfData.EventType == eErrorEventTypeDiagEvent) {
    sendDiagEventErrorResponse(eDEVICE_WRITE_ERROR);
  }
  if (m_errorEventNtfData.isNeedReboot == TRUE) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "System will reboot soon, so stop logging function.");
    StopLoggingFunction(f_hApp);
  }
  m_bIsPrevEventCompleted = true;
  l_eStatus = SendLogCompleteRequestToSystemManager(eFrameworkunifiedStatusFail);

  l_eStatus = m_loggerCanEvent.PublishErrorEvent(f_hApp, m_errorEventNtfData,
  TRUE,
                                                 m_currentEventTriggerNumber,
                                                 m_time);
  LOG_STATUS_IF_ERRORED(l_eStatus, "m_loggerCanEvent.PublishErrorEvent();");

  LOG_STATUS_IF_ERRORED(l_eStatus, "SendLogCompleteRequestToSystemManager()");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////
//  Function : OnStorageResponseNotFound
//  brief    : This function services response from the storage thread. It
//             is called when the logs failed to be written to the target
//             destination because the target was not found
///////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CErrorEvent::OnStorageResponseNotFound(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  l_eStatus = m_loggerPopups.ShowErrorPopup(f_hApp, m_errorEventNtfData,
                                            (UI_16) eSelectedDeviceNotFound);
  LOG_STATUS_IF_ERRORED(
      l_eStatus,
      "m_loggerPopups.ShowErrorPopup(f_hApp, eSelectedDeviceNotFound)");

  if (m_errorEventNtfData.EventType == eErrorEventTypeDiagEvent) {
    l_eStatus = sendDiagEventErrorResponse(eDEVICE_NOT_AVAILABLE);
    LOG_STATUS_IF_ERRORED(l_eStatus, "sendDiagEventErrorResponse(eDEVICE_NOT_AVAILABLE)");
  }
  if (m_errorEventNtfData.isNeedReboot == TRUE) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "System will reboot soon, so stop logging function.");
    StopLoggingFunction(f_hApp);
  }
  m_bIsPrevEventCompleted = true;
  l_eStatus = SendLogCompleteRequestToSystemManager(eFrameworkunifiedStatusFail);

  l_eStatus = m_loggerCanEvent.PublishErrorEvent(f_hApp, m_errorEventNtfData,
  TRUE,
                                                 m_currentEventTriggerNumber,
                                                 m_time);
  LOG_STATUS_IF_ERRORED(l_eStatus, "m_loggerCanEvent.PublishErrorEvent();");

  LOG_STATUS_IF_ERRORED(l_eStatus, "SendLogCompleteRequestToSystemManager()");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////
//  Function : OnStorageResponseNoWritten
//  brief    : This function services response from the storage thread. It
//             is called when no logs where found.
///////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CErrorEvent::OnStorageResponseNoWritten(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  l_eStatus = m_loggerPopups.ShowErrorPopup(f_hApp, m_errorEventNtfData,
                                            (UI_16) eNoLogToStore);
  LOG_STATUS_IF_ERRORED(l_eStatus,
                        "m_loggerPopups.ShowErrorPopup(f_hApp, eNoLogToStore)");

  if (m_errorEventNtfData.EventType == eErrorEventTypeDiagEvent) {
    l_eStatus = sendDiagEventErrorResponse(eNO_ERROR_INFO);
    LOG_STATUS_IF_ERRORED(l_eStatus, "sendDiagEventErrorResponse(eNO_ERROR_INFO)");
  }
  if (m_errorEventNtfData.isNeedReboot == TRUE) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "System will reboot soon, so stop logging function.");
    StopLoggingFunction(f_hApp);
  }
  m_bIsPrevEventCompleted = true;
  l_eStatus = SendLogCompleteRequestToSystemManager(eFrameworkunifiedStatusOK);

  l_eStatus = m_loggerCanEvent.PublishErrorEvent(f_hApp, m_errorEventNtfData,
  TRUE,
                                                 m_currentEventTriggerNumber,
                                                 m_time);
  LOG_STATUS_IF_ERRORED(l_eStatus, "m_loggerCanEvent.PublishErrorEvent();");

  LOG_STATUS_IF_ERRORED(l_eStatus, "SendLogCompleteRequestToSystemManager()");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

EFrameworkunifiedStatus CErrorEvent::sendDiagEventErrorResponse(EELL_ErrorCode f_errCode) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  if (this->m_ServiceName.length() > 0) {
    l_eStatus = sendDiagEventErrorResponse(f_errCode, this->m_ServiceName);
    this->m_ServiceName = "";
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

EFrameworkunifiedStatus CErrorEvent::sendDiagEventErrorResponse(EELL_ErrorCode f_errCode,
                                                   std::string f_destName) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedSendMsg(m_diagsessionhandle, SS_LOGGERCOPYEMERGENCYLOGS_ERROR_RESP, sizeof(EELL_ErrorCode), &f_errCode))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "SS_LOGGERCOPYEMERGENCYLOGS_ERROR_RESP Msg send failed = %x",
           l_eStatus);
    // LCOV_EXCL_STOP
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

EFrameworkunifiedStatus CErrorEvent::sendDiagEventResponse(void) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedSendMsg(m_diagsessionhandle, SS_LOGGERCOPYEMERGENCYLOGS_SUCCESS_RESP, 0, NULL))) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "SS_LOGGERCOPYEMERGENCYLOGS_SUCCESS_RESP Msg send failed = %x",
           l_eStatus);
  }

  m_ServiceName = "";

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

EFrameworkunifiedStatus CErrorEvent::SetDiagEventSourceName(std::string f_serviceName) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;
  if ((f_serviceName.length() > 0)
      && (this->m_ServiceName.length() == 0)) {
    this->m_ServiceName = f_serviceName;
    l_eStatus = eFrameworkunifiedStatusOK;
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           " Error. f_serviceName(%s), m_serviceName(%s)",
           f_serviceName.c_str(), m_ServiceName.c_str());
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

void CErrorEvent::SetDiagSessionHandle(HANDLE f_sessionhandle) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  m_diagsessionhandle = f_sessionhandle;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

void CErrorEvent::SetMileage(UI_32 f_mileage) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  this->m_loggerUtil.SetMilage(f_mileage);
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

// LCOV_EXCL_START 200: 
EFrameworkunifiedStatus CErrorEvent::SaveNaviLog(EPWR_SHUTDOWN_TRIGGER_TYPE errType) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  UI_32 l_logMaxNum = 0;
  std::string l_Filename = "";

  uint32_t l_time;
  uint8_t status;
  Clock_getSystemTimeY2K38(&l_time, &status);
  l_eStatus = m_loggerUtil.getEmmcNaviLogParams(l_time, errType, l_Filename,
                                                l_logMaxNum);

  if (eFrameworkunifiedStatusOK != l_eStatus) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           " Error: getEmmcNaviLogParams NaviLog update failed %d", l_eStatus);
  } else {
    if (l_logMaxNum == 0) {
      /*OutputNaviLogNmlMax or OutputNaviLogErrMax = 0. */
      FRAMEWORKUNIFIEDLOG(
          ZONE_WARN,
          __FUNCTION__,
          " Warning: Not update NaviLog. OutputNaviLogNmlMax or OutputNaviLogErrMax = 0");
    } else {
      FILE *fp;
      void *p;
      p = EL_mem_exram_mmap_simple(EL_MEM_ID_NAVI_LOG, PROT_READ | PROT_WRITE,
      MAP_SHARED,
                                   EL_MEM_CACHE_INVALID);
      if (p == MAP_FAILED) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Error : EL_mem_exram_mmap_simple MAP_FAILD");
        l_eStatus = eFrameworkunifiedStatusFail;
      } else {
        fp = fopen(l_Filename.c_str(), "wb");
        if (fp == NULL) {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Could not open file %s",
                 l_Filename.c_str());
          l_eStatus = eFrameworkunifiedStatusAccessError;
        } else {
          fwrite(p, sizeof(unsigned char), EL_mem_getLength(EL_MEM_ID_NAVI_LOG),
                 fp);
          fclose(fp);
        }
        if (0 != EL_mem_exram_munmap(p, EL_mem_getLength(EL_MEM_ID_NAVI_LOG))) {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error : EL_mem_exram_munmap");
        }
      }
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus CErrorEvent::VerifyExtStorage(TLoggerErrorEvent event) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;
  std::string f_path;

  f_path = m_pLoggerCfg->getUsb0MountPath();

  if (eFrameworkunifiedStatusOK == l_eStatus) {
    l_eStatus = eFrameworkunifiedStatusFail;

    if (eFrameworkunifiedStatusFail == l_eStatus) {
      // timeout or error
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error : get secure log failed or timeout");
    }
  } else {
    // timeout
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error : ticket verify timeout.");
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

bool CErrorEvent::IsNeedVerify(EErrorEventType type) {
  bool ret = false;
  switch (type) {
    case eErrorEventTypeUserInvokedCollectScreenShot:
    case eErrorEventTypeUserInvokedCollectAllLogs:
    case eErrorEventTypeUserInvokedCollectInterfaceunifiedLogs:
    case eErrorEventTypeInterfaceunifiedEmmcLogs:
    case eErrorEventTypeEelExport:
    case eErrorEventTypeDiagEvent:
    case eErrorEventTypeCanEvent:
      ret = true;
      break;
    default:
      break;
  }
  return ret;
}
