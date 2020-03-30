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
/// \ingroup  tag_SS_LoggerService
/// \brief    This file supports error event logging.
///
///////////////////////////////////////////////////////////////////////////////
#ifndef LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_ERROR_EVENT_H_
#define LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_ERROR_EVENT_H_

#include <time.h>
#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_framework_if.h>
#include <system_service/ss_system_types.h>
#include <system_service/ss_system_timer.h>
#include <system_service/ss_system_manager_protocol.h>
#include <string>
#include <vector>
#include "ss_logger_popups.h"
#include "ss_logger_common.h"
#include "ss_logger_reader_writer_control.h"
#include "ss_logger_error_event_cfg.h"
#include "ss_logger_cfg.h"
#include "ss_logger_util.h"
#include "readerWriter/reader_writer.h"
#include "readerWriter/reader_writer_cfg.h"
#include "ss_logger_error_event_storage.h"
#include "ss_logger_error_event_can_evt_reply.h"

class CErrorEvent {
 public:
  CErrorEvent();
  ~CErrorEvent();

  EFrameworkunifiedStatus Initialize(HANDLE f_hApp, CLoggerCfg *f_pLoggerCfg,
                        CReaderWriterControl *f_pReaderWriterControl,
                        HANDLE f_thrdEvntLogWriter,
                        std::string f_strEvntLogQueWorkerName);

  EFrameworkunifiedStatus RegisterSessionErrorEvent(HANDLE f_hApp);

  EFrameworkunifiedStatus OnOpenSession(HANDLE f_hApp, PCSTR f_pRequesterName,
                           HANDLE f_hSession);

  EFrameworkunifiedStatus OnCloseSession(HANDLE f_hApp, PCSTR f_pRequesterName,
                            HANDLE f_hSession);

  EFrameworkunifiedStatus StartLogging(HANDLE f_hApp, EErrorEventType f_eventType);

  EFrameworkunifiedStatus sendDiagEventErrorResponse(EELL_ErrorCode f_errCode);

  EFrameworkunifiedStatus sendDiagEventErrorResponse(EELL_ErrorCode f_errCode,
                                        std::string f_destName);

  EFrameworkunifiedStatus sendDiagEventResponse(void);

  EFrameworkunifiedStatus SetDiagEventSourceName(std::string f_serviceName);

  void SetMileage(UI_32 f_mileage);

  void SetDiagSessionHandle(HANDLE hSession);

  EFrameworkunifiedStatus SaveNaviLog(EPWR_SHUTDOWN_TRIGGER_TYPE errorType);
  EFrameworkunifiedStatus CreateKernelLog(HANDLE f_hApp, SS_LOGGER_KLOG_OPE_TYPE type);

 private:
  typedef enum {
    kSSL_ERROR_EVENT_TIMER_LOGGING_START_RSPN,
    kSSL_ERROR_EVENT_TIMER_ARTIFACT_RSPN,
    kSSL_ERROR_EVENT_TIMER_END
  } ESSLErrorEventTimers;

  // Logging Protocol Function Prototypes
  EFrameworkunifiedStatus OnStartLogging(HANDLE f_hApp);
  EFrameworkunifiedStatus OnLogStartResponse(HANDLE f_hApp);
  EFrameworkunifiedStatus OnLogStartResponseTimeout(HANDLE f_hApp);
  EFrameworkunifiedStatus RequestNextArtifact(HANDLE f_hApp);
  EFrameworkunifiedStatus OnArtifactResponse(HANDLE f_hApp);
  EFrameworkunifiedStatus OnArtifactResponseTimeout(HANDLE f_hApp);

  // Protocol functions sent to self.
  EFrameworkunifiedStatus OnArtifactRequest(HANDLE f_hApp);

  // Error Event Artifact Request Function Prototypes
  EFrameworkunifiedStatus SendLogArtifactResponseToSelf(
      HANDLE f_hApp, EArtifactId f_artifactId,
      std::string f_artifactFilePathAndName);

  EFrameworkunifiedStatus OnObtainLoggerserviceLogRequest(HANDLE f_hApp);
  EFrameworkunifiedStatus OnObtainTransmitLogRequest(HANDLE f_hApp);
  EFrameworkunifiedStatus OnObtainPerformanceLogRequest(HANDLE f_hApp);
  EFrameworkunifiedStatus OnObtainScreenShotRequest(HANDLE f_hApp);
  EFrameworkunifiedStatus OnObtainScreenShotResponse(HANDLE f_hApp);
  EFrameworkunifiedStatus OnObtainKernelLogInfoRequest(HANDLE f_hApp);
  EFrameworkunifiedStatus OnObtainDRInitialLogRequest(HANDLE f_hApp);
  EFrameworkunifiedStatus OnObtainDRLocationLogRequest(HANDLE f_hApp);
  EFrameworkunifiedStatus OnStorageResponseOk(HANDLE f_hApp);
  EFrameworkunifiedStatus OnStorageResponseWriteFailed(HANDLE f_hApp);
  EFrameworkunifiedStatus OnStorageResponseNotFound(HANDLE f_hApp);
  EFrameworkunifiedStatus OnStorageResponseNoWritten(HANDLE f_hApp);
  EFrameworkunifiedStatus CheckPathForArtifact(HANDLE f_hApp, TLoggingArtifact f_artifact);
  EFrameworkunifiedStatus OnClearAllLogRequest(HANDLE f_hApp);
  EFrameworkunifiedStatus OnNaviLogRequest(HANDLE f_hApp);

  // verify function for external storage
  EFrameworkunifiedStatus VerifyExtStorage(TLoggerErrorEvent event);
  bool IsNeedVerify(EErrorEventType type);

  HANDLE m_hApp;
  bool m_bIsPrevEventCompleted;
  CErrorEventCfg m_errorEventCfg;
  TLoggerErrorEvent m_errorEventNtfData;
  Timer m_errorEventTimers[kSSL_ERROR_EVENT_TIMER_END];
  std::vector<TLoggingArtifact> m_artifactRequestVec;
  TArtifactResponseVec m_artifactResponseVec;
  std::vector<FrameworkunifiedProtocolCallbackHandler> m_requesterCallbacksVec;
  std::string m_archiveDestination;
  CLoggerUtil m_loggerUtil;
  CLoggerPopups m_loggerPopups;
  CLoggerCfg* m_pLoggerCfg;
  CLoggerErrorEventStorage m_loggerStorage;
  CLoggerErrorEventCANEvtReply m_loggerCanEvent;
  static const UI_32 m_loggingStartRspnToSec = 1;
  static const UI_32 m_screenShotRspnToSec = 2;
  CReaderWriterControl* m_pReaderWriterControl;

  std::string m_ServiceName;
  uint32_t m_time;
  UI_32 m_currentEventTriggerNumber;
  HANDLE m_diagsessionhandle;

  HANDLE m_thrdEvntLogWriter;

 public:
  int m_sfd;
};

EFrameworkunifiedStatus SSLogger_SendtoSM(HANDLE f_hApp, TLoggingArtifact f_artifact);
EFrameworkunifiedStatus SSLogger_SendtoSelf(HANDLE f_hApp, TLoggingArtifact f_artifact);

#endif  // LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_ERROR_EVENT_H_

