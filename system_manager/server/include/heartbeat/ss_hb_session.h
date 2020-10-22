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
/// \ingroup  tag_SystemManager
/// \brief    This file provides support for the application heartbeat system.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef SS_HEARTBEAT_SESSION_H_  // NOLINT
#define SS_HEARTBEAT_SESSION_H_

#include <native_service/frameworkunified_types.h>
#include <system_service/ss_heartbeat_service_protocol.h>
#include <system_service/ss_system_manager_if.h>
#include <system_service/ss_system_manager_if_local.h>
#include <map>
#include <string>

#include "ss_hb_if.h"

// The name of the module that is trying to open a session with Heart Beat
typedef std::string SubscriberName;

/*
 *
 */
class CHeartBeatSessionHandler {
 public:
  CHeartBeatSessionHandler();
  virtual ~CHeartBeatSessionHandler();

  EFrameworkunifiedStatus HBProcessClientResponse(HANDLE hApp);

  EFrameworkunifiedStatus HBEntrySubscriber(SubscriberName &f_Subscriber);  // NOLINT
  EFrameworkunifiedStatus HBSendRequest(HANDLE hApp, SI_32 f_HeartBeatIntervalRepeat, SI_32 f_ChkIndex);
  EFrameworkunifiedStatus HBCheckResponses(THbReportData &f_tReportData, UI_32 f_MaxHeartBeatRetryCount,  // NOLINT
        SI_32 f_HeartBeatIntervalRepeat, SI_32 f_ChkIndex);

  EFrameworkunifiedStatus HBDeleteRegisteredClientEntry(HANDLE f_hThread, PSTR pQueueName);
  EFrameworkunifiedStatus HBAvailableCheck(THbAvailCheck &check);  // NOLINT

  VOID HBPrintConnection();
  VOID HBPrintStack(UI_32 f_MaxHeartBeatRetryCount);

 private:
  class HbSessionInfo {
   public:
    std::string szName;
    HANDLE hSession;
    BOOL   fRunning;
    BOOL   fHeartBeatRequestSent;
    BOOL   fHeartBeatResponseReceived;
    UI_8   HeartBeatRetryCount;
    BOOL   fHeartBeatTimedOut;
    BOOL   fisAvailability;

    // Copy constructor
    HbSessionInfo(const HbSessionInfo &f_SessionInfo) {
      szName                     = f_SessionInfo.szName;
      hSession                   = f_SessionInfo.hSession;
      fRunning                   = f_SessionInfo.fRunning;
      fHeartBeatRequestSent      = f_SessionInfo.fHeartBeatRequestSent;
      fHeartBeatResponseReceived = f_SessionInfo.fHeartBeatResponseReceived;
      HeartBeatRetryCount        = f_SessionInfo.HeartBeatRetryCount;
      fHeartBeatTimedOut         = f_SessionInfo.fHeartBeatTimedOut;
      fisAvailability            = f_SessionInfo.fisAvailability;
    }

    explicit HbSessionInfo(SubscriberName f_Subscriber);
  };

  typedef std::map<SubscriberName, HbSessionInfo> HbSessionMap;
  typedef HbSessionMap::iterator HbSessionIter;

  HbSessionMap m_mapHbSessions;
};
#endif  // SS_HEARTBEAT_SESSION_H_  // NOLINT
