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

//////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup tag_NSTimer
/// \brief
///
/// Timer class for setting timers.
///
//////////////////////////////////////////////////////////////////////////////////////////////////

#include <native_service/ns_timer_if.hpp>
#include <native_service/ns_message_center_if.h>
#include <string.h>

//////////////////////////////////////////////////////////////////////////////////////////////
///// Constructor
//////////////////////////////////////////////////////////////////////////////////////////////
NSTimer::NSTimer():
  m_hTimer(NULL),
  m_u64TimeInterval(0),
  m_bRepeatTimer(FALSE) {
}

//////////////////////////////////////////////////////////////////////////////////////////////
///// Destructor
//////////////////////////////////////////////////////////////////////////////////////////////
NSTimer::~NSTimer() {
  if (NULL != m_hTimer) {
    NS_TimerDelete(m_hTimer);
    m_hTimer = NULL;
  }
}


//////////////////////////////////////////////////////////////////////////////////////////////
///// SetNotifyMethod
//////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NSTimer::SetNotifyMethod(UI_16 notifyCmdId, PCSTR notifyToAppName) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  NSTimerInfo l_tTimerInfo = {};

  if ((NULL != notifyToAppName) && (strlen(notifyToAppName) <= MAX_SERVICE_NAME)) {
    HANDLE l_hSenderHandle = McOpenSender(notifyToAppName);

    if (NULL != l_hSenderHandle) {  // reset previous notify method, and set new
      // resets previous timer timeout action if any
      if (NULL != m_hTimer) {
        NS_TimerDelete(m_hTimer);
        m_hTimer = NULL;
      }

      l_tTimerInfo.iCmd = notifyCmdId;
      // Create timer. Do not start.
      if (NULL == (m_hTimer = NS_TimerCreate(l_tTimerInfo, CALLBACK_MESSAGE, l_hSenderHandle))) {
        l_eStatus = eFrameworkunifiedStatusFail;
      }

      McClose(l_hSenderHandle);
    } else {
      // just return fail. Don't reset any previous notify method
      l_eStatus = eFrameworkunifiedStatusFail;
    }
  } else {
    l_eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return l_eStatus;
}

//////////////////////////////////////////////////////////////////////////////////////////////
///// SetRepeatTimer
//////////////////////////////////////////////////////////////////////////////////////////////
VOID NSTimer::SetRepeatTimer(BOOL repeatTimer) {
  m_bRepeatTimer = repeatTimer;
}

//////////////////////////////////////////////////////////////////////////////////////////////
///// SetTime
//////////////////////////////////////////////////////////////////////////////////////////////
VOID NSTimer::SetTime(UI_32 seconds, UI_64 msecs) {
  m_u64TimeInterval = msecs + seconds * 1000ULL;  // internally, set interval as milliseconds
}

//////////////////////////////////////////////////////////////////////////////////////////////
///// SetTime
//////////////////////////////////////////////////////////////////////////////////////////////
VOID NSTimer::SetTime(UI_32 hrs, UI_32 mins, UI_32 seconds, UI_64 msecs) {
  // internally, set interval as milliseconds
  m_u64TimeInterval = msecs + seconds * 1000ULL + mins * 60000ULL + hrs * 3600000ULL;
}

//////////////////////////////////////////////////////////////////////////////////////////////
///// Start
//////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NSTimer::Start(UI_32 seconds, UI_64 msecs) {
  this->SetTime(seconds, msecs);
  return (this->Start());
}

//////////////////////////////////////////////////////////////////////////////////////////////
///// Start
//////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NSTimer::Start() {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  if (NULL != m_hTimer) {
    NSTimerInfo l_tTimerInfo = {};

    l_tTimerInfo.t_sec = WholeSeconds(static_cast<UI_32>(m_u64TimeInterval));  /// Converts to seconds
    l_tTimerInfo.t_nsec = MSToNS(RemainderMs(static_cast<UI_32>(m_u64TimeInterval)));  /// Converts to nano seconds

    if (TRUE == m_bRepeatTimer) {
      l_tTimerInfo.rpt_sec = l_tTimerInfo.t_sec;
      l_tTimerInfo.rpt_nsec = l_tTimerInfo.t_nsec;
    }
    l_eStatus = NS_TimerSetTime(m_hTimer, l_tTimerInfo);
  } else {
    l_eStatus = eFrameworkunifiedStatusFail;
  }

  return l_eStatus;
}

//////////////////////////////////////////////////////////////////////////////////////////////
///// Stop
//////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NSTimer::Stop() {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (NULL != m_hTimer) {
    NSTimerInfo l_tTimerInfo = {};
    l_eStatus = NS_TimerSetTime(m_hTimer, l_tTimerInfo);
  } else {
    l_eStatus = eFrameworkunifiedStatusFail;
  }

  return l_eStatus;
}

//////////////////////////////////////////////////////////////////////////////////////////////
///// IsRunning
//////////////////////////////////////////////////////////////////////////////////////////////
BOOL NSTimer::IsRunning() {
  BOOL l_bIsRunning = FALSE;

  if (NULL != m_hTimer) {
    NSTimerInfo l_tTimerInfo = {};
    NS_TimerGetTime(m_hTimer, &l_tTimerInfo);
    if ((0 != l_tTimerInfo.t_sec) || (0 != l_tTimerInfo.t_nsec) || (0 != l_tTimerInfo.rpt_sec) ||
        (0 != l_tTimerInfo.rpt_nsec)) {
      l_bIsRunning = TRUE;
    }
  }
  return l_bIsRunning;
}

//////////////////////////////////////////////////////////////////////////////////////////////
///// GetInterval
//////////////////////////////////////////////////////////////////////////////////////////////
UI_64 NSTimer::GetInterval() {
  return m_u64TimeInterval;
}

// EOF
