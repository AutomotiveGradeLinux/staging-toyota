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
#ifndef LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_POPUPS_H_
#define LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_POPUPS_H_
#include <native_service/frameworkunified_types.h>
#include <system_service/ss_logger_service_protocol.h>
#include <string>
#include <vector>

#include "ss_logger_cfg.h"
#include "ss_logger_error_event_cfg.h"
class CLoggerPopups {
 public:
  CLoggerPopups();
  virtual ~CLoggerPopups();
  EFrameworkunifiedStatus Initialize(CLoggerCfg* f_pLoggerCfg);
  BOOL IsPopupNeeded(TLoggerErrorEvent f_event);
  BOOL CanShowStartPopup(std::vector<TLoggingArtifact>* f_remainingArtifact);
  EFrameworkunifiedStatus ShowStartPopup(HANDLE f_hApp, TLoggerErrorEvent f_event,
                            std::vector<TLoggingArtifact>* f_remainingArtifact,
                            std::string f_dest);
  EFrameworkunifiedStatus ShowEndPopup(HANDLE f_hApp, TLoggerErrorEvent f_event,
                          std::string f_dest);
  EFrameworkunifiedStatus ShowErrorPopup(HANDLE f_hApp, TLoggerErrorEvent f_event,
                            UI_16 f_errCode);
  std::string getDeviceName(std::string f_dest);
 private:
  CLoggerCfg *m_pLoggerCfg;
  BOOL m_startShown;
};

#endif  // LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_POPUPS_H_
