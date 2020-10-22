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

#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_service_protocol.h>
#include <system_service/ss_heartbeat_service_protocol.h>
#include <system_service/ss_services.h>
#include <system_service/ss_sm_thread_names.h>
#include <system_service/ss_templates.h>
#include <iostream>
#include <iomanip>
#include <utility>
#include <list>
#include <string>

#include "ss_sm_systemmanagerlog.h"
#include "ss_hb_thread.h"
#include "ss_hb_session.h"
#include "ss_system_manager.h"

using namespace std;  // NOLINT

CHeartBeatSessionHandler::CHeartBeatSessionHandler() {
}

CHeartBeatSessionHandler::~CHeartBeatSessionHandler() {  // LCOV_EXCL_START 14: Resident process, not called by NSFW
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
}
// LCOV_EXCL_STOP

/*****************************************************************************
  @ingroup: SS_SystemManager
  @brief: HbSessionInfo
  @note: HbSessionInfo constructor
  @param
  @return
 *****************************************************************************/
CHeartBeatSessionHandler::HbSessionInfo::HbSessionInfo(SubscriberName f_Subscriber) {
  fRunning = FALSE;
  fHeartBeatRequestSent = FALSE;
  fHeartBeatResponseReceived = FALSE;
  HeartBeatRetryCount = 0;
  fHeartBeatTimedOut = FALSE;
  hSession = NULL;
  szName = f_Subscriber;
  fisAvailability = FALSE;
}

/*****************************************************************************
  @ingroup: SS_SystemManager
  @brief: HBEntrySubscriber
  @note:
  @param
  @return
 *****************************************************************************/
EFrameworkunifiedStatus CHeartBeatSessionHandler::HBEntrySubscriber(SubscriberName &f_Subscriber) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  // check if the subscriber is already in map
  HbSessionIter l_SessionInfoIterator = m_mapHbSessions.find(f_Subscriber);

  // the l_SessionInfoIterator is set to the end then the subscriber is not in the map
  if (m_mapHbSessions.end() == l_SessionInfoIterator) {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Entry Subscriber : %s", f_Subscriber.c_str());
    HbSessionInfo l_NewHBClientSessionInfo(f_Subscriber.c_str());  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

    pair<HbSessionIter, bool> ret;
    // insert new subscriber entry into the session map
    ret = m_mapHbSessions.insert(std::make_pair(f_Subscriber, l_NewHBClientSessionInfo));
    if (!ret.second) {  // LCOV_EXCL_BR_LINE 6:l_NewHBClientSessionInfo must not be NULL.
      // LCOV_EXCL_START 6:l_NewHBClientSessionInfo must not be NULL.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_eStatus = eFrameworkunifiedStatusFail;
      SS_ASERT(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
      // LCOV_EXCL_STOP
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Subscriber '%s' is already in the map", f_Subscriber.c_str());
  }
  return l_eStatus;
}

/*****************************************************************************
  @ingroup: SS_SystemManager
  @brief: ProcessHBClientResponse
  @note: Process the response received from client, update relevant structure data
  @param HANDLE
  @return EFrameworkunifiedStatus
 *****************************************************************************/
EFrameworkunifiedStatus CHeartBeatSessionHandler::HBProcessClientResponse(HANDLE f_hThread) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_PERIODIC_INFO, __FUNCTION__, "+");
  BOOL l_availability = FALSE;

  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg<BOOL>(f_hThread, l_availability))) {
    LOG_ERROR("ReadMsg()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  }

  // find the subscriber...
  HbSessionIter l_SessionInfoIterator = m_mapHbSessions.find(FrameworkunifiedGetMsgSrc(f_hThread));

  // the l_SessionInfoIterator is set to the end then the subscriber is not in the map
  if (m_mapHbSessions.end() != l_SessionInfoIterator) {
    l_SessionInfoIterator->second.HeartBeatRetryCount = 0;
    l_SessionInfoIterator->second.fHeartBeatResponseReceived = TRUE;
    l_SessionInfoIterator->second.fHeartBeatRequestSent = FALSE;
    l_SessionInfoIterator->second.fisAvailability = l_availability;
    FRAMEWORKUNIFIEDLOG(ZONE_PERIODIC_INFO, __FUNCTION__,
        "HeartBeat received from module: %s ", l_SessionInfoIterator->first.c_str());
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_PERIODIC_INFO, __FUNCTION__,
        "Ignoring Heart Beat Response, Client: %s not found in the map!", FrameworkunifiedGetMsgSrc(f_hThread));
  }
  FRAMEWORKUNIFIEDLOG(ZONE_PERIODIC_INFO, __FUNCTION__, "-");
  return l_eStatus;
}

/*****************************************************************************
  @ingroup: SS_SystemManager
  @brief: HBPrintConnection
  @note: .Print registered client information
  @param VOID
  @return VOID
 *****************************************************************************/
VOID CHeartBeatSessionHandler::HBPrintConnection() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // find the subscriber...
  HbSessionIter l_SessionInfoIterator = m_mapHbSessions.begin();
  for (; l_SessionInfoIterator != m_mapHbSessions.end(); l_SessionInfoIterator++) {
    FRAMEWORKUNIFIEDLOG(ZONE_DEBUG_DUMP, __FUNCTION__,
        "HeartBeat Service is Connected to: %s via a Session: Running: %s ",
        l_SessionInfoIterator->second.szName.data(),
        (l_SessionInfoIterator->second.fRunning ? "YES" : "NO"));
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

/*****************************************************************************
  @ingroup: SS_SystemManager
  @brief: HBPrintStack
  @note: .Print registered client information
  @param VOID
  @return VOID
 *****************************************************************************/
VOID CHeartBeatSessionHandler::HBPrintStack(UI_32 f_MaxHeartBeatRetryCount) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // find the subscriber...
  HbSessionIter l_SessionInfoIterator = m_mapHbSessions.begin();
  for (; l_SessionInfoIterator != m_mapHbSessions.end(); l_SessionInfoIterator++) {
    if (FALSE == l_SessionInfoIterator->second.fHeartBeatResponseReceived) {
      FRAMEWORKUNIFIEDLOG(ZONE_DEBUG_DUMP, __FUNCTION__,
             "HeartBeat Service is Connected to: %s via a Session: Running: %s ",
             l_SessionInfoIterator->second.szName.data(),
             (l_SessionInfoIterator->second.fRunning ? "YES" : "NO"));
      FRAMEWORKUNIFIEDLOG(ZONE_DEBUG_DUMP, __FUNCTION__,
             "Retry count (%d) within limit (%d), HeartBeatTimedout = %s",
             l_SessionInfoIterator->second.HeartBeatRetryCount,
             f_MaxHeartBeatRetryCount,
             (l_SessionInfoIterator->second.fHeartBeatTimedOut == TRUE ?
              "TRUE" : "FALSE"));
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

/*****************************************************************************
  @ingroup: SS_SystemManager
  @brief: HBCheckResponses
  @note:  Check if all clients have replied to heartbeat query and generate report
  @param  THbReportData Report structure to be filled
  @param  UI_32         Heartbeat max retry count
  @return eFrameworkunifiedStatusFail if one or more modules have timed out, eFrameworkunifiedStatusOK otherwise.
 *****************************************************************************/
EFrameworkunifiedStatus CHeartBeatSessionHandler::HBCheckResponses(
        THbReportData &f_tReportData, UI_32 f_MaxHeartBeatRetryCount,
        SI_32 f_HeartBeatIntervalRepeat, SI_32 f_ChkIndex) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_PERIODIC_FUNC, __FUNCTION__, "+");

  HbSessionIter l_SessionInfoIterator = m_mapHbSessions.begin();
  memset(&f_tReportData, 0, sizeof(THbReportData));

  int index = 0;

  while (l_SessionInfoIterator != m_mapHbSessions.end()) {
    if (f_ChkIndex == (index % f_HeartBeatIntervalRepeat)) {
      // check if report queue is full
      // LCOV_EXCL_BR_START 6: f_tReportData.nNumOfModules can't more than SS_MAX_NUM_MODULES
      if (SS_MAX_NUM_MODULES <= f_tReportData.nNumOfModules) {
      // LCOV_EXCL_BR_STOP
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
          " Error: SS_MAX_NUM_MODULES '%d' <=  f_tReportData.nNumOfModules '%d'. See ss_system_manager_if.h.",
          SS_MAX_NUM_MODULES, f_tReportData.nNumOfModules);

        FRAMEWORKUNIFIEDLOG(ZONE_PERIODIC_FUNC, __FUNCTION__, "-");
        return eFrameworkunifiedStatusInvldBuf;
      }


      strncpy(f_tReportData.tModuleList[f_tReportData.nNumOfModules].ProcQueueName,
              l_SessionInfoIterator->first.c_str(),
              sizeof(f_tReportData.tModuleList[f_tReportData.nNumOfModules].ProcQueueName) - 1);
      f_tReportData.tModuleList[f_tReportData.nNumOfModules].HeartBeatRetryCount =
              l_SessionInfoIterator->second.HeartBeatRetryCount;
      f_tReportData.tModuleList[f_tReportData.nNumOfModules].ProcHBState = HB_STATUS_GOOD;

      if (TRUE == l_SessionInfoIterator->second.fHeartBeatResponseReceived) {
        l_SessionInfoIterator->second.fHeartBeatResponseReceived = FALSE;
        FRAMEWORKUNIFIEDLOG(ZONE_PERIODIC_INFO, __FUNCTION__,
               "[%s] Heart Beat Response Received !",
               l_SessionInfoIterator->second.szName.c_str());
      } else if (TRUE == l_SessionInfoIterator->second.fHeartBeatTimedOut) {
        f_tReportData.tModuleList[f_tReportData.nNumOfModules].ProcHBState = HB_STATUS_TIMEOUT;
      } else if ((TRUE == l_SessionInfoIterator->second.fHeartBeatRequestSent)
                 && (FALSE == l_SessionInfoIterator->second.fHeartBeatTimedOut)) {
        l_SessionInfoIterator->second.HeartBeatRetryCount++;

        if (l_SessionInfoIterator->second.HeartBeatRetryCount > f_MaxHeartBeatRetryCount) {
            l_eStatus = eFrameworkunifiedStatusFail;
            l_SessionInfoIterator->second.fHeartBeatTimedOut = TRUE;
            l_SessionInfoIterator->second.HeartBeatRetryCount = 0;

          f_tReportData.tModuleList[f_tReportData.nNumOfModules].ProcHBState = HB_STATUS_TIMEOUT;

          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
            " Error: Heart Beat Retry count of [%s] has crossed max limit (%d)."
            "Disabling sending HeartBeat query to it",
            l_SessionInfoIterator->second.szName.c_str(),
            f_MaxHeartBeatRetryCount);
        } else {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
            " Error: [%s] has missed a Heart Beat. Retry count (%d) is within max retry limit (%d)",
            l_SessionInfoIterator->second.szName.c_str(),
            l_SessionInfoIterator->second.HeartBeatRetryCount,
            f_MaxHeartBeatRetryCount);
        }
      }
    }

    f_tReportData.nNumOfModules++;
    ++l_SessionInfoIterator;
    index++;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_PERIODIC_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

/*****************************************************************************
  @ingroup: SS_SystemManager
  @brief: HBSendRequest
  @note: .Sent heart beat query to the registered clients
  @param void
  @return EFrameworkunifiedStatus
******************************************************************************/
EFrameworkunifiedStatus CHeartBeatSessionHandler::HBSendRequest(HANDLE f_hThread,
        SI_32 f_HeartBeatIntervalRepeat, SI_32 f_ChkIndex) {
    EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
    FRAMEWORKUNIFIEDLOG(ZONE_PERIODIC_FUNC, __FUNCTION__, "+");

  // Get session iterator
  HbSessionIter l_SessionInfoIterator = m_mapHbSessions.begin();

  if (l_SessionInfoIterator == m_mapHbSessions.end()) {
    FRAMEWORKUNIFIEDLOG(ZONE_PERIODIC_INFO, __FUNCTION__, "No Client has opened session with HeartBeat");
    FRAMEWORKUNIFIEDLOG(ZONE_PERIODIC_FUNC, __FUNCTION__, "-");
    return l_eStatus;
  }

  int index = 0;

  while (l_SessionInfoIterator != m_mapHbSessions.end()) {
    if (f_ChkIndex == (index % f_HeartBeatIntervalRepeat)) {
      if (l_SessionInfoIterator->second.hSession == NULL) {  // LCOV_EXCL_BR_LINE 200:hSession must not be NULL
        l_SessionInfoIterator->second.hSession = FrameworkunifiedMcOpenSender(f_hThread, l_SessionInfoIterator->first.c_str());
        if (NULL == l_SessionInfoIterator->second.hSession) {  // LCOV_EXCL_BR_LINE 200:hSession must not be NULL
          // LCOV_EXCL_START 200: hSession must not be NULL
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                 " Error: FrameworkunifiedMcOpenSender(%s) returned NULL",
                 l_SessionInfoIterator->first.c_str());
          // LCOV_EXCL_STOP
        } else {
          l_SessionInfoIterator->second.fRunning = TRUE;
        }
      }

      if ((NULL != l_SessionInfoIterator->second.hSession)
          && (FALSE == l_SessionInfoIterator->second.fHeartBeatTimedOut)) {
        if (eFrameworkunifiedStatusOK == (l_eStatus = FrameworkunifiedSendMsg(l_SessionInfoIterator->second.hSession, SS_HEARTBEAT_REQUEST, 0, NULL))) {  // LCOV_EXCL_BR_LINE 5:NSFW's error  // NOLINT(whitespace/line_length)
          /// TODO review if we are required to return failure if sending message to one process failed.
          l_SessionInfoIterator->second.fHeartBeatResponseReceived = FALSE;
          l_SessionInfoIterator->second.fHeartBeatRequestSent = TRUE;
          FRAMEWORKUNIFIEDLOG(ZONE_PERIODIC_INFO, __FUNCTION__,
                 "HeartBeat Request sent to : %s", l_SessionInfoIterator->first.c_str());
        } else {
          // LCOV_EXCL_START 5:NSFW's error
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                 " Error: FrameworkunifiedSendMsg(%s, SS_HEARTBEAT_REQUEST) errored: %d/'%s'",
                 l_SessionInfoIterator->first.c_str(), l_eStatus,
                 GetStr(l_eStatus).c_str());
          // LCOV_EXCL_STOP
        }
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_PERIODIC_INFO, __FUNCTION__,
               " HeartBeat Request was not sent to '%s', timeout is '%s'",
               l_SessionInfoIterator->first.c_str(),
               (l_SessionInfoIterator->second.fHeartBeatTimedOut == TRUE ? "TRUE" : "FALSE"));
      }
    }
    ++l_SessionInfoIterator;
    index++;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_PERIODIC_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

/*****************************************************************************
  @ingroup: SS_SystemManager
  @brief: HBDeleteRegisteredClientEntry
  @note: .
  @param
  @return EFrameworkunifiedStatus
******************************************************************************/
EFrameworkunifiedStatus CHeartBeatSessionHandler::HBDeleteRegisteredClientEntry(HANDLE f_hThread, PSTR pQueueName) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  SubscriberName tQueuename(pQueueName);

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Looking for %s.", tQueuename.c_str());

  HbSessionIter l_SessionInfoIterator = m_mapHbSessions.find(tQueuename);

  if (m_mapHbSessions.end() != l_SessionInfoIterator) {
    // Close the session handle
    if (NULL != l_SessionInfoIterator->second.hSession) {  // LCOV_EXCL_BR_LINE 200:hSession must not be NULL when tQueuename exists.  // NOLINT(whitespace/line_length)
      // LCOV_EXCL_BR_START 4: NSFW error case.
      if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedMcClose(l_SessionInfoIterator->second.hSession))) {
      // LCOV_EXCL_BR_STOP
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        LOG_ERROR("FrameworkunifiedMcClose()");  // LCOV_EXCL_LINE 4: NSFW error case.
      }
      l_SessionInfoIterator->second.hSession = NULL;
    }
    m_mapHbSessions.erase(tQueuename);
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Deleting '%s' Successful ", tQueuename.c_str());
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
            "%s not found in Heart beat client list", tQueuename.c_str());
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

/*****************************************************************************
  @ingroup: SS_SystemManager
  @brief: HBAvailableCheck
  @note: .
  @param
  @return EFrameworkunifiedStatus
******************************************************************************/
EFrameworkunifiedStatus CHeartBeatSessionHandler::HBAvailableCheck(THbAvailCheck &check) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  BOOL isOk = TRUE;

  std::list<std::string> ngList;
  for (HbSessionIter ite = m_mapHbSessions.begin(); ite != m_mapHbSessions.end(); ite++) {
    if (ite->second.fisAvailability == FALSE) {
      ngList.push_back(ite->first);
      // Notify Last Service as Representative
      snprintf(check.serviceName, SS_SM_HB_MAX_PROC_NAME_SIZE, "%s", ite->first.c_str());
      isOk = FALSE;
    }
  }

  if (isOk == FALSE) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "********* AVAILABILITY CHECK ERROR **********");
    fprintf(stderr, "HBOnAvailCheckRequest/********* AVAILABILITY CHECK ERROR **********\n");  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "********* AVAILABILITY CHECK OK **********");
    fprintf(stderr, "HBOnAvailCheckRequest/********* AVAILABILITY CHECK OK **********\n");  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  }

  for (std::list<std::string>::iterator ite = ngList.begin(); ite != ngList.end(); ite++) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "  %s", ite->c_str());
    fprintf(stderr, "  %s\n", ite->c_str());  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  }

  check.isOk = isOk;

  return l_eStatus;
}
