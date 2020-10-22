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
/// \brief    This file supports error event logging configuration.
///
///////////////////////////////////////////////////////////////////////////////
#ifndef LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_ERROR_EVENT_CFG_H_
#define LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_ERROR_EVENT_CFG_H_
#include <system_service/ss_system_manager_if.h>
#include <vector>
#include <map>
#include <string>

#include "ss_logger_cfg.h"

typedef UI_32 ARTIFACT_BIT_MASK;
typedef UI_32 EVENT_BIT_MASK;

#define  NBR_ARTIFACT_BITS  (sizeof(ARTIFACT_BIT_MASK) * 8)

enum EVENT_BIT {
  EVENT_BIT_NONE = 0x00000000,
  EVENT_BIT_POPUP = 1 << (UI_32) 1,
  EVENT_BIT_CAN_NOTIFICATION = 1 << (UI_32) 2,
  EVENT_BIT_DIAG = 1 << (UI_32) 3,
};

struct TLoggingErrorEventInformation {
  ARTIFACT_BIT_MASK ArtifactBitMask;
  EVENT_BIT_MASK EventBitMask;
};
struct TLoggingArtifactCfg {
  EErrorEventType ErrorType;
  TLoggingErrorEventInformation Information;
};
struct TLoggerErrorEvent {
  TLoggerErrorEvent()
      : EventType(eErrorEventTypeMaxValue),
        ModuleName(""),
        isNeedReboot(FALSE),
        EventBitMask(EVENT_BIT_NONE) {  // LCOV_EXCL_BR_LINE 11:Unexpected branch
  }
  TLoggerErrorEvent(SMErrorEventNtfData l_SMErrorEvent) {  // NOLINT (runtime/explicit)
    EventType = l_SMErrorEvent.EventType;
    ModuleName = l_SMErrorEvent.ModuleName;
    isNeedReboot = l_SMErrorEvent.isNeedReboot;
    EventBitMask = EVENT_BIT_NONE;
  }
  EErrorEventType EventType;
  std::string ModuleName;
  BOOL isNeedReboot;
  EVENT_BIT_MASK EventBitMask;
};
struct TLoggingArtifact;

typedef EFrameworkunifiedStatus (*TLoggingCbFunction)(HANDLE, TLoggingArtifact);

enum ECallbackType {
  eCbTypePath = 0,
  eCbTypeFunction,
  eCbTypeInvalid
};

struct TLoggingArtifactCallback {
  ECallbackType Type;
  std::string Path;
  TLoggingCbFunction Function;
};

struct TLoggingArtifactInformation {
  std::string OwnerServiceName;
  UI_32 RequestTimeoutMs;
  TLoggingArtifactCallback Cb;
  BOOL Remove;
};
struct TLoggingArtifact {
  EArtifactId ArtifactId;
  TLoggingArtifactInformation Information;
};

struct TLoggingArtifactResponse {
  EArtifactId ArtifactId;
  std::string Filepath;
  BOOL Remove;
};

//////////////////////////////////////////////////////////////////////////////////////////
/////  External function definition for Logger callbacks
//////////////////////////////////////////////////////////////////////////////////////////
extern EFrameworkunifiedStatus SSLogger_SendtoSM(HANDLE f_hApp, TLoggingArtifact f_artifact);
extern EFrameworkunifiedStatus SSLogger_SendtoSelf(HANDLE f_hApp,
                                      TLoggingArtifact f_artifact);

typedef std::map<EErrorEventType, TLoggingErrorEventInformation> TArtifactBitMaskMap;
typedef std::map<EArtifactId, TLoggingArtifactInformation> TArtifactMap;
typedef std::vector<TLoggingArtifactResponse> TArtifactResponseVec;

class CErrorEventCfg {
 public:
  CErrorEventCfg(void);

  ~CErrorEventCfg(void);

  EFrameworkunifiedStatus Initialize(CLoggerCfg* p_logger_cfg);

  void GetArtifactRequestVec(
      EErrorEventType f_eventType,
      std::vector<TLoggingArtifact> &f_refArtifactRequestVec);  // NOLINT (runtime/references)
  EVENT_BIT_MASK GetEventsForErrorEvent(EErrorEventType f_eventType);
 private:
  UI_32 ValidateConfiguration();
  TArtifactBitMaskMap m_errorEventTypeToArtifactBitMaskMap;
  TArtifactMap m_errorArtifactMap;
  UI_32 m_ext_log_num;

  static TLoggingArtifactCfg m_loggerErrorEventCfgTbl[eErrorEventTypeMaxValue];
  static TLoggingArtifact m_loggerErrorArtifactCfgTbl[eArtifactIdMaxValue];
};

#endif  // LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_ERROR_EVENT_CFG_H_
