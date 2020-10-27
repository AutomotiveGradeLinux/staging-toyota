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
/// \ingroup  tag_SystemManagerIf
/// \brief    This file provides support for the System Manager client interface.
///
///////////////////////////////////////////////////////////////////////////////

#include "system_service/ss_system_timer.h"
#include <native_service/ns_timer_if.h>
#include <native_service/ns_message_center_if.h>
#include <utility>
#include "ss_system_timer_local.h"

Timer::Timer() :
  m_bInit(FALSE), m_hApp(NULL), m_hSnd(NULL), m_hTmr(NULL) {
  bzero(&m_tTi, sizeof(m_tTi));
}

Timer::Timer(HANDLE hdl, UI_32 id, CbFuncPtr CbFn):
  m_bInit(FALSE), m_hApp(NULL), m_hSnd(NULL), m_hTmr(NULL) {
  bzero(&m_tTi, sizeof(m_tTi));
  Initialize(hdl, id, CbFn);
}

Timer::~Timer() {
  if (m_bInit) {
    Shutdown();
  }
}  // LCOV_EXCL_BR_LINE 10:Because destructor

BOOL Timer::Initialize(HANDLE hApp, UI_32 id, CbFuncPtr CbFn) {
  if (!m_bInit && hApp && id && CbFn) {
    m_hApp = hApp;
    m_tTi.iCmd = id;
    m_tTi.t_sec = 0;
    m_tTi.t_nsec = 0;
    m_tTi.rpt_sec = 0;
    m_tTi.rpt_nsec = 0;
    m_CbFn = CbFn;
    m_hSnd = McOpenSender(FrameworkunifiedGetAppName(m_hApp));
    m_hTmr = NS_TimerCreate(m_tTi, CALLBACK_MESSAGE, m_hSnd);
    m_bInit = TRUE;
  }
  return m_bInit;
}

void Timer::Reinitialize(UI_32 id) {
  if (!m_bInit && id) {
    m_tTi.iCmd = id;

    m_tTi.t_sec = 0;
    m_tTi.t_nsec = 0;
    m_tTi.rpt_sec = 0;
    m_tTi.rpt_nsec = 0;

    m_hSnd = McOpenSender(FrameworkunifiedGetAppName(m_hApp));
    m_hTmr = NS_TimerCreate(m_tTi, CALLBACK_MESSAGE, m_hSnd);
    m_bInit = TRUE;
  }
  return;
}

void Timer::Shutdown() {
  if (m_bInit) {
    Stop();
    NS_TimerDelete(m_hTmr);
    m_hTmr = NULL;
    McClose(m_hSnd);
    m_hSnd = NULL;
    m_bInit = FALSE;
  }
}

BOOL Timer::SetTime(UI_32 ss, UI_32 sms, UI_32 rs, UI_32 rms) {
  if (m_bInit) {
    m_tTi.t_sec = ss;
    m_tTi.t_nsec = (ss || sms) ? sms*NS_PER_MS : 1;  // LCOV_EXCL_BR_LINE 11:Unexpected branch
    m_tTi.rpt_sec = rs;
    m_tTi.rpt_nsec = rms*NS_PER_MS;
  }
  return m_bInit;
}

BOOL Timer::Start(UI_32 ss, UI_32 sms, UI_32 rs, UI_32 rms) {
  if (m_bInit) {
    SetTime(ss, sms, rs, rms);
    FrameworkunifiedAttachCallbackToDispatcher(m_hApp, TIMER_SERVICE_NAME, m_tTi.iCmd, m_CbFn);
    NS_TimerSetTime(m_hTmr, m_tTi);
  }
  return m_bInit;
}

BOOL Timer::Start() {
  if (m_bInit) {
    FrameworkunifiedAttachCallbackToDispatcher(m_hApp, TIMER_SERVICE_NAME, m_tTi.iCmd, m_CbFn);
    m_tTi.t_nsec = (m_tTi.t_sec || m_tTi.t_nsec) ? m_tTi.t_nsec : 1;  // LCOV_EXCL_BR_LINE 11:Unexpected branch
    NS_TimerSetTime(m_hTmr, m_tTi);
  }
  return m_bInit;
}

BOOL Timer::Stop() {
  if (m_bInit) {
    NSTimerInfo ti = m_tTi;
    ti.t_sec = 0;
    ti.t_nsec = 0;
    ti.rpt_sec = 0;
    ti.rpt_nsec = 0;
    NS_TimerSetTime(m_hTmr, ti);
    FrameworkunifiedDetachCallbackFromDispatcher(m_hApp, TIMER_SERVICE_NAME, ti.iCmd);
  }
  return m_bInit;
}

// LCOV_EXCL_START 6:Because the condition cannot be set
TimerCtrl::TimerCtrl()
  : m_hApp(NULL), m_nTimersMax(DEFAULT_NTIMERS) {
}

TimerCtrl::TimerCtrl(UI_32 ntimers)
  : m_hApp(NULL), m_nTimersMax(ntimers) {
}

void TimerCtrl::Initialize(HANDLE hApp) {
  if ( NULL != hApp ) {
    m_hApp = hApp;
    if (m_aTimers.empty() && m_rTimers.empty()) {
      for (UI_32 i = DEFAULT_TIMERID; i < DEFAULT_TIMERID+m_nTimersMax; ++i) {
        m_aTimers.push_back(i);
      }
    }
  }
}

void TimerCtrl::Shutdown() {
  m_aTimers.clear();
  while (m_rTimers.begin() != m_rTimers.end()) {
    Timer* p_timer = m_rTimers.begin()->second;
    p_timer->Shutdown();
    delete p_timer;
    m_rTimers.erase(m_rTimers.begin());
  }
}

UI_32 TimerCtrl::CreateTimer(CbFuncPtr CbFn) {
  if (!m_aTimers.empty()) {
    UI_32 id = m_aTimers.front();
    m_aTimers.pop_front();
    Timer* p_timer = new Timer(m_hApp, id, CbFn);
    m_rTimers.insert(std::pair<UI_32, Timer*>(id, p_timer));
    return id;
  }
  return 0;
}

UI_32 TimerCtrl::DeleteTimer(UI_32 id) {
  if (m_rTimers.find(id) != m_rTimers.end()) {
    Timer* p_timer = m_rTimers.find(id)->second;
    p_timer->Shutdown();
    delete p_timer;
    m_rTimers.erase(m_rTimers.find(id));
    m_aTimers.push_back(id);
    return id;
  }
  return 0;
}

UI_32 TimerCtrl::SetTimer(UI_32 id, UI_32 ss, UI_32 sms, UI_32 rs, UI_32 rms) {
  if (m_rTimers.find(id) != m_rTimers.end()) {
    Timer* p_timer = m_rTimers.find(id)->second;
    p_timer->SetTime(ss, sms, rs, rms);
    return id;
  }
  return 0;
}
UI_32 TimerCtrl::StartTimer(UI_32 id, UI_32 ss, UI_32 sms, UI_32 rs, UI_32 rms) {
  if (m_rTimers.find(id) != m_rTimers.end()) {
    Timer* p_timer = m_rTimers.find(id)->second;
    p_timer->Start(ss, sms, rs, rms);
    return id;
  }
  return 0;
}

UI_32 TimerCtrl::StartTimerMulti(UI_32 id, UI_32 ss, UI_32 sms, UI_32 rs, UI_32 rms, UI_32 subId) {
  if (m_rTimers.find(id) != m_rTimers.end()) {
    Timer* p_timer = m_rTimers.find(id)->second;
    p_timer->Shutdown();
    p_timer->Reinitialize(id + subId);
    p_timer->Start(ss, sms, rs, rms);
    return id;
  }
  return 0;
}

UI_32 TimerCtrl::StartTimer(UI_32 id) {
  if (m_rTimers.find(id) != m_rTimers.end()) {
    Timer* p_timer = m_rTimers.find(id)->second;
    p_timer->Start();
    return id;
  }
  return 0;
}

UI_32 TimerCtrl::StopTimer(UI_32 id) {
  if (m_rTimers.find(id) != m_rTimers.end()) {
    Timer* p_timer = m_rTimers.find(id)->second;
    p_timer->Stop();
    return id;
  }
  return 0;
}
// LCOV_EXCL_STOP
