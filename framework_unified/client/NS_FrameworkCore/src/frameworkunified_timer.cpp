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


#include <native_service/frameworkunified_timer.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/ns_timer_if.h>
#include "frameworkunified_framework_core.h"

const UI_32 TIMER_CALLBACK_CMD_ID = 42;  // The answer to life, the universe, and everything
const PCSTR TIMER_SERVICE_NAME = "TIMER";


HANDLE FrameworkunifiedAttachTimerCallback(HANDLE hApp, UI_32 initMS, UI_32 repeatMS, CbFuncPtr CbFn) {
  HANDLE timer = NULL;

  if ((frameworkunifiedCheckValidAppHandle(hApp)) && (NULL != CbFn)) {
    FrameworkunifiedAttachCallbackToDispatcher(hApp, TIMER_SERVICE_NAME, TIMER_CALLBACK_CMD_ID, CbFn);

    CFrameworkunifiedFrameworkApp *pApp = const_cast<CFrameworkunifiedFrameworkApp *>(reinterpret_cast<const CFrameworkunifiedFrameworkApp *>(hApp));
    NSTimerInfo ti = { WholeSeconds(initMS),
                       MSToNS(RemainderMs(initMS)),
                       TIMER_CALLBACK_CMD_ID,
                       WholeSeconds(repeatMS),
                       MSToNS(RemainderMs(repeatMS))
                     };
    timer = NS_TimerCreate(ti, CALLBACK_MESSAGE, pApp->hAppSndMsgQ);
  }

  return timer;
}

EFrameworkunifiedStatus FrameworkunifiedDetachTimerCallback(HANDLE hApp, HANDLE hTimer) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && NULL != hTimer) {
    EFrameworkunifiedStatus sDel = NS_TimerDelete(hTimer);
    hTimer = NULL;

    EFrameworkunifiedStatus sDet = FrameworkunifiedDetachCallbackFromDispatcher(hApp,
                                                      TIMER_SERVICE_NAME,
                                                      TIMER_CALLBACK_CMD_ID);

    // out of coverage
    // Since the above frameworkunifiedCheckValidAppHandle(hApp) && NULL != hTimer have been confirmed,
    // the condition will not be satisfied.
    if (eFrameworkunifiedStatusOK != sDel || eFrameworkunifiedStatusOK != sDet) {
      eStatus = eFrameworkunifiedStatusFail;
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }
  return eStatus;
}
