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
/// \brief    This file contains the callback for Notifications and Protocol
///           commands.
///
///////////////////////////////////////////////////////////////////////////////
#ifndef LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_SERVICE_CALLBACKS_H_
#define LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_SERVICE_CALLBACKS_H_

#include <stdlib.h>
#include <stdio.h>
#include <native_service/frameworkunified_framework_types.h>
#include <system_service/ss_error_event_common.h>
#include "ss_logger_error_event.h"
#include "ss_logger_reader_writer_control.h"
#include "ss_logger_cfg.h"

class CLoggerServiceCallbacks {
 private:
  static const UI_32 kNUMBER_PROTOCOL_HANDLERS = 17u;
  FrameworkunifiedProtocolCallbackHandler m_protocolHandlers[kNUMBER_PROTOCOL_HANDLERS];
  CLoggerCfg *m_pLoggerCfg;
  CErrorEvent *m_pErrorEvent;

 public:
  CLoggerServiceCallbacks();
  ~CLoggerServiceCallbacks();

  EFrameworkunifiedStatus Initialize(HANDLE f_hApp, CLoggerCfg *f_pLoggerCfg,
                        CErrorEvent *f_pErrorEvent);

  EFrameworkunifiedStatus LoggerServerOnOpenSession(HANDLE hApp);

  EFrameworkunifiedStatus LoggerServerOnCloseSession(HANDLE hApp);

  EFrameworkunifiedStatus LoggerServer_SetMileageData(HANDLE hApp);

  EFrameworkunifiedStatus LoggerServer_SetLoggerParams(HANDLE hApp);

  EFrameworkunifiedStatus LoggerServer_UDPLogging(HANDLE hApp);

  EFrameworkunifiedStatus LoggerServer_SetCurrentDateTime(HANDLE hApp);

  EFrameworkunifiedStatus LoggerServer_UpdateCommonInfo(HANDLE hApp);

  EFrameworkunifiedStatus LoggerServer_Handle_CWORD56_Events(HANDLE hApp);

  EFrameworkunifiedStatus LoggerServer_LogResetEvents(HANDLE hApp);

  EFrameworkunifiedStatus LoggerServer_OnCopyEmergencyLog(HANDLE hApp);

  EFrameworkunifiedStatus LoggerServer_OnCANTrigger(HANDLE f_hApp);

  EFrameworkunifiedStatus LoggerServer_OnDTCTrigger(HANDLE f_hApp);

  EFrameworkunifiedStatus LoggerServer_On_SHTDWN_Complete(HANDLE f_hApp);

  EFrameworkunifiedStatus LoggerServer_OnSetVIN(HANDLE hApp);

  EFrameworkunifiedStatus LoggerServer_OnCopyEvntLog(HANDLE hApp);

  EFrameworkunifiedStatus LoggerServer_OnClearEvntLog(HANDLE hApp);

  EFrameworkunifiedStatus LoggerServer_OnReadNumberOfEvents(HANDLE hApp);

  EFrameworkunifiedStatus LoggerServer_OnReadStatCounter(HANDLE hApp);

  EFrameworkunifiedStatus LoggerServer_OnResetStatCounter(HANDLE hApp);

  EFrameworkunifiedStatus LoggerServer_OnSetDiagID(HANDLE hApp);

  EFrameworkunifiedStatus LoggerServer_UploadEventLog(HANDLE hApp);

  HANDLE RetrieveRegisteredClientHandle(HANDLE hApp, PCSTR pRequester);
};

EFrameworkunifiedStatus LoggerService_OnStop(HANDLE hApp);

#endif  // LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_SERVICE_CALLBACKS_H_

