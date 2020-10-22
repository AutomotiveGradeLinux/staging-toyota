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
/// \ingroup  tag_NSTimer
/// \brief    Contains the internal structures that used by ns_timer
///
///
///
///////////////////////////////////////////////////////////////////////////////

#ifndef FRAMEWORK_UNIFIED_CLIENT_NS_TIMER_INCLUDE_NS_TIMER_HANDLE_H_
#define FRAMEWORK_UNIFIED_CLIENT_NS_TIMER_INCLUDE_NS_TIMER_HANDLE_H_
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <native_service/frameworkunified_types.h>

typedef void (*pfCb)(UI_16 cmd);

/// \brief Struct that defines internal function callback for a timer
typedef struct _TimerNotifyInfo {
  UI_16  iCmd;
  PSTR   q_name;
} TTimerInfo, *PTimerInfo;

/// \brief Struct that defines internal handle object for a timer
typedef struct _NSTimerHandle {
  int timerfd;
  struct itimerspec itime;
  PTimerInfo tTimerInfo;
  UI_8 timerState;
} NSTimerHandle, *PNSTimerHandle;

#endif  // FRAMEWORK_UNIFIED_CLIENT_NS_TIMER_INCLUDE_NS_TIMER_HANDLE_H_
