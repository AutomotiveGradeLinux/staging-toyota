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

//////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NSFramework
/// \brief    File defines FrameworkunifiedMsgProfiler class which keeps profiler information for message dispatcher
//////////////////////////////////////////////////////////////////////////////////////////////////

/* For FRAMEWORKUNIFIEDLOG*/
#include <stdio.h>
#include <time.h>
#include <errno.h>

#include <native_service/ns_logger_if.h>
#include <native_service/ns_message_center_if.h>
/* For PROTOCOL_DIS_MSGPROFILER */
#include <native_service/frameworkunified_service_protocol.h>
/* For FrameworkunifiedGetAppName */
#include <native_service/frameworkunified_framework_if.h>

#include <string>
#include <vector>
/* Required for OpenSender, SendMessage, McSend, McOpenSender */
#include "ns_msg_queue.h"
#include "frameworkunified_msgprofiler.h"

BOOL FrameworkunifiedMsgProfiler::m_bMsgProfilerEnabled = FALSE;

//////////////////////////////////////////
// Constructor
//////////////////////////////////////////
// LCOV_EXCL_START 8: dead code
FrameworkunifiedMsgProfiler::FrameworkunifiedMsgProfiler(const std::string &f_cAppName)
  : m_cAppName(f_cAppName),
    m_ui32TotalNoOfMsgsReceived(0),
    m_ui64MinMsgProcessingTime(720575940),  // assign some big value
    m_ui64MaxMsgProcessingTime(0),
    m_ui64TotalMsgExecutionTime(0),
    m_ui64LastMsgReceivedTime(0),
    m_ui64AppInitTime(0),
    m_bLastMsgWasPrintProfile(FALSE) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 8: dead code
  m_ui64AppInitTime = GetClock();
}
// LCOV_EXCL_STOP

//////////////////////////////////////////
// Destructor
//////////////////////////////////////////
// LCOV_EXCL_START 8: dead code
FrameworkunifiedMsgProfiler::~FrameworkunifiedMsgProfiler() {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 8: dead code
}
// LCOV_EXCL_STOP

//////////////////////////////////////////
// Function : GetAppName
//////////////////////////////////////////
// LCOV_EXCL_START 8: dead code
std::string FrameworkunifiedMsgProfiler::GetAppName() {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 8: dead code
  return m_cAppName;
}
// LCOV_EXCL_STOP

//////////////////////////////////////////
// Function : MsgReceived
//////////////////////////////////////////
// LCOV_EXCL_START 8: dead code
VOID FrameworkunifiedMsgProfiler::MsgReceived() {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 8: dead code
  m_ui32TotalNoOfMsgsReceived++;
  m_ui64LastMsgReceivedTime = GetClock();
}
// LCOV_EXCL_STOP

//////////////////////////////////////////
// Function : MsgProcessed
//////////////////////////////////////////
// LCOV_EXCL_START 8: dead code
VOID FrameworkunifiedMsgProfiler::MsgProcessed() {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 8: dead code
  if (!m_bLastMsgWasPrintProfile) {
    UI_64 l_ui64MsgProcessedTime = GetClock() - m_ui64LastMsgReceivedTime;

    // set minimum execution time
    if (l_ui64MsgProcessedTime < m_ui64MinMsgProcessingTime) {
      m_ui64MinMsgProcessingTime = l_ui64MsgProcessedTime;
    }

    // set maximum execution time
    if (l_ui64MsgProcessedTime > m_ui64MaxMsgProcessingTime) {
      m_ui64MaxMsgProcessingTime = l_ui64MsgProcessedTime;
    }

    m_ui64TotalMsgExecutionTime += l_ui64MsgProcessedTime;
  } else {
    m_bLastMsgWasPrintProfile = FALSE;
  }

  // reset the last msg received time
  m_ui64LastMsgReceivedTime = 0;
}
// LCOV_EXCL_STOP

//////////////////////////////////////////
// Function : PrintProfileInfo
//////////////////////////////////////////
// LCOV_EXCL_START 8: dead code
EFrameworkunifiedStatus FrameworkunifiedMsgProfiler::PrintProfileInfo(HANDLE f_hApp) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 8: dead code
  HANDLE l_hProfileQ = OpenSender(FRAMEWORKUNIFIED_NS_MSGPROFILERUTIL);
  FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __PRETTY_FUNCTION__, "Inside");
  CHAR l_cData[MAX_QUEUE_MSG_SIZE] = {};
  m_bLastMsgWasPrintProfile = TRUE;
  // don't consider this message which is profiler specific. So decreasing the msg count by 1
  m_ui32TotalNoOfMsgsReceived--;
  if (NULL !=  l_hProfileQ) {
    UI_64 l_ui64TimeSinceAppStart = (GetClock() - m_ui64AppInitTime) / 1000;
    UI_64 l_ui64AvgMsgProcessingTime = m_ui64TotalMsgExecutionTime / m_ui32TotalNoOfMsgsReceived;

    // MSG FORMAT: Tick, App/ThreadName, TotalMsgs, Freq(msgs/sec),
    //      Min MsgProcessingtime(cs,ms),:, Avg MsgProcessingtime (cs,ms) ,:, Max MsgProcessingtime (cs,ms)
    snprintf(l_cData, MAX_QUEUE_MSG_SIZE, "%010lld, %-23s, %9d, %6.3f, "
             "%30lld,%-5lld ,:, %30lld,%-5lld ,:, %30lld,%-5lld\n",
             static_cast<long long int>(l_ui64TimeSinceAppStart),  // NOLINT  (readability/nolint)
             m_cAppName.c_str(),
             m_ui32TotalNoOfMsgsReceived,
             (F_64)m_ui32TotalNoOfMsgsReceived / (F_64)l_ui64TimeSinceAppStart,
             static_cast<long long int>(m_ui64MinMsgProcessingTime),  // NOLINT  (readability/nolint)
             static_cast<long long int>(m_ui64MinMsgProcessingTime),  // NOLINT  (readability/nolint)
             static_cast<long long int>(l_ui64AvgMsgProcessingTime),  // NOLINT  (readability/nolint)
             static_cast<long long int>(l_ui64AvgMsgProcessingTime),  // NOLINT  (readability/nolint)
             static_cast<long long int>(m_ui64MaxMsgProcessingTime),  // NOLINT  (readability/nolint)
             static_cast<long long int>(m_ui64MaxMsgProcessingTime));  // NOLINT  (readability/nolint)

    SendMessage(l_hProfileQ, MAX_QUEUE_MSG_SIZE, l_cData);

    if (eFrameworkunifiedStatusOK == CloseSender(l_hProfileQ)) {
      l_hProfileQ = NULL;
    }

    // inform all childs to print msg profiler information.
    std::vector<std::string>::iterator l_itChildList =  m_vChildNames.begin();

    HANDLE l_hChild = NULL;
    while (m_vChildNames.end() != l_itChildList) {
      if (!((*l_itChildList).empty())) {
        l_hChild = McOpenSender((*l_itChildList).c_str());
        if (NULL != l_hChild) {
          if (eFrameworkunifiedStatusOK != McSend(l_hChild, FrameworkunifiedGetAppName(f_hApp), PROTOCOL_DIS_MSGPROFILER, 0, NULL)) {
            FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __PRETTY_FUNCTION__, "Error sending PROTOCOL_DIS_MSGPROFILER to child");
          } else {
            FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __PRETTY_FUNCTION__, "Msg PROTOCOL_DIS_MSGPROFILER sent to child");
          }

          McClose(l_hChild);
          l_hChild = NULL;
        }
      }
      l_itChildList++;
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __PRETTY_FUNCTION__, "Exiting");
  return eFrameworkunifiedStatusOK;
}
// LCOV_EXCL_STOP

//////////////////////////////////////////
// Function : AddChildName
//////////////////////////////////////////
// LCOV_EXCL_START 8: dead code
VOID FrameworkunifiedMsgProfiler::AddChildName(const std::string &f_cChildName) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 8: dead code
  if (!f_cChildName.empty()) {
    m_vChildNames.push_back(f_cChildName);
  }
}
// LCOV_EXCL_STOP

//////////////////////////////////////////
// Function : GetClock
//////////////////////////////////////////
// LCOV_EXCL_START 8: dead code
UI_64 FrameworkunifiedMsgProfiler::GetClock() {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 8: dead code
  struct timespec tp;

  if (clock_gettime(CLOCK_MONOTONIC, &tp) == -1) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __PRETTY_FUNCTION__, "clock_gettime errno:%d", errno);
    return 0;
  }
  return (UI_64)tp.tv_sec * 1000 * 1000 + tp.tv_nsec / 1000;
}
// LCOV_EXCL_STOP

// EOF
