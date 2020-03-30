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
/// \brief    This file supports notifying CAN abou HK and CAN error events
///
///////////////////////////////////////////////////////////////////////////////
#ifndef LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_ERROR_EVENT_CAN_EVT_REPLY_H_
#define LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_ERROR_EVENT_CAN_EVT_REPLY_H_

#include <native_service/frameworkunified_types.h>
#include <system_service/ss_logger_service_protocol.h>

#include "ss_logger_cfg.h"
#include "ss_logger_error_event_cfg.h"
#include "ss_logger_common.h"
class CLoggerErrorEventCANEvtReply {
 public:
  CLoggerErrorEventCANEvtReply();
  virtual ~CLoggerErrorEventCANEvtReply();
  EFrameworkunifiedStatus Initialize(CLoggerCfg* f_pLoggerCfg);

  BOOL IsEventNeeded(TLoggerErrorEvent f_event);
  EFrameworkunifiedStatus PublishStartEvent(HANDLE f_hApp, TLoggerErrorEvent f_event);
  EFrameworkunifiedStatus PublishEndEvent(HANDLE f_hApp, TLoggerErrorEvent f_event,
                             UI_32 f_triggerNumber, uint32_t f_time);

  EFrameworkunifiedStatus PublishErrorEvent(HANDLE f_hApp, TLoggerErrorEvent f_event,
                               BOOL f_isValid, UI_32 f_triggerNumber,
                               uint32_t f_time);
 private:
};

#endif  // LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_ERROR_EVENT_CAN_EVT_REPLY_H_
