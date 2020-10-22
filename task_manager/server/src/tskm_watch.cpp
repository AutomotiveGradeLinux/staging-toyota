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

#include "tskm_watch.h"
#include <agl_thread.h>

#include "tskm_debug.h"
#include "tskm_port_pf.h"
#include "tskm_comm.h"


#define WATCH_CYCLE 5

#define TIMER_THREAD_NAME "SS_TskmTimer"

/*******************************************************************
 *    Periodic Timer-Thread MAIN Function
 *******************************************************************/
void *
watchMain(void *arg) {
  int connFd = -1;

  connFd = tskm_cliSockConnect(TSKM_SOCKET_NAME);
  if (connFd < 0) {  // LCOV_EXCL_BR_LINE 5: system function(uinx) "socket" fail process
    // LCOV_EXCL_START 5: system function(uinx) "socket" fail process
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT(0);
    goto ERROR;
    // LCOV_EXCL_STOP 5
  }

  while (1) {
    int ret;
    TSKM_EVENT_INFO_t ev;

    sleep(WATCH_CYCLE);

    ev.event = TSKM_EV_LCL_REP_POLLING;
    ev.errCode = TSKM_E_OK;
    ret = tskm_sockSend(connFd, &ev);
    if (ret <= 0) {  // LCOV_EXCL_BR_LINE 5: system function(uinx) "send" fail process
    // LCOV_EXCL_START 5: system function(uinx) "send" fail process
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      TSKM_ASSERT(0);
      goto ERROR;
      // LCOV_EXCL_STOP 5
    }
  }

  // LCOV_EXCL_START 6: system function(uinx) call fail process
  ERROR:
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  if (connFd != -1) {
    tskm_sockDestory(connFd);
  }

  return 0;
  // LCOV_EXCL_STOP
}

/*******************************************************************
 *    Periodic Timer thread start
 *******************************************************************/
int tskm_watch_startTimer() {
  int ret;
  pthread_t thId = { 0 };

  ret = tskm_pf_createThread(watchMain, NULL, PR_SS_TSKMTIMER,
                             TIMER_THREAD_NAME, &thId);
  if (ret != 0) {  // LCOV_EXCL_BR_LINE 5: system function(uinx) "pthread_*" fail process
    // LCOV_EXCL_START 5: system function(uinx) "pthread_*" fail process
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT(0);
    goto ERROR;
    // LCOV_EXCL_STOP 5
  } else {
    TSKM_PRINTF(TSKM_LOG_STATE, "watch thread created.");
  }

  return 0;

  // LCOV_EXCL_LINE 6: system function(uinx) "pthread_*" fail process
  ERROR:
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  return -1;
  // LCOV_EXCL_STOP
}

