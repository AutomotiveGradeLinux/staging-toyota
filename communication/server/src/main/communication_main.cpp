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

#include <errno.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <poll.h>

#include <native_service/frameworkunified_dispatcher.h>
#include <native_service/frameworkunified_application.h>
#include <native_service/frameworkunified_multithreading.h>

#include <native_service/ns_version_if.h>
#include <system_service/ss_system_if.h>
#include <system_service/ss_sm_client_if.h>
#include "communication_communicationlog.h"
#include "communication_version.h"
#include "communication_cid.h"

#include "CAN_Thread.h"
#include "Thread_Common.h"

CFrameworkunifiedVersion g_FrameworkunifiedVersion(MAJORNO, MINORNO, REVISION);

FRAMEWORKUNIFIEDLOGPARAM g_FrameworkunifiedLogParams = {
  FRAMEWORKUNIFIEDLOGOPTIONS,
  {ZONE_TEXT_10, ZONE_TEXT_11, ZONE_TEXT_12,
   ZONE_TEXT_13, ZONE_TEXT_14, ZONE_TEXT_15,
   ZONE_TEXT_16, ZONE_TEXT_17, ZONE_TEXT_18,
   ZONE_TEXT_19, ZONE_TEXT_20, ZONE_TEXT_21,
   ZONE_TEXT_22, ZONE_TEXT_23, ZONE_TEXT_24,
   ZONE_TEXT_25, ZONE_TEXT_26, ZONE_TEXT_27,
   ZONE_TEXT_28, ZONE_TEXT_29, ZONE_TEXT_30,
   ZONE_TEXT_31},
  FRAMEWORKUNIFIEDLOGZONES};

extern HANDLE g_can_thread;
static bool capture_log_flg = FALSE;

#define COMMSYS_SND_ERR_CNT_MAX 10

static void MessageHandler(HANDLE h_app) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  e_status = FrameworkunifiedDispatchProcessWithoutLoop(h_app);
  if (e_status != eFrameworkunifiedStatusOK) {
    // Ignore Error. Logging only.
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "FrameworkunifiedDispatchProcessWithoutLoop: %d", e_status);
  }
}

static void TimeoutHandlerCAN(HANDLE h_app) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  uint32_t dummy = 0;
  static uint32_t snd_err_cnt_can = 0;

  if (CommGetAvailabilityCurrent(CAN_AVAILABILITY)) {
    e_status = FrameworkunifiedSendChild(h_app, g_can_thread,
                            CID_COMMSYS_TIMEOUT, 0, &dummy);
    if (e_status != eFrameworkunifiedStatusOK) {
      snd_err_cnt_can++;
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__,
                       "FrameworkunifiedSendChild(timeout for can) faild: %d", e_status);

      if (capture_log_flg)
        return;

      if (snd_err_cnt_can < COMMSYS_SND_ERR_CNT_MAX)
        return;

      SendUserInvokedLoggingRequestToSystemManager(
                     e_SS_SM_CAPTURE_DTC_LOGS, "Send CAN TimeoutNtfy Error");
      capture_log_flg = true;
    } else {
      snd_err_cnt_can = 0;
    }
  }
}

int main(int argc, char *argv[]) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  FrameworkunifiedDefaultCallbackHandler cb_funcs;
  FRAMEWORKUNIFIED_MAKE_DEFAULT_CALLBACK(cb_funcs);
  HANDLE h_app;
  FRAMEWORKUNIFIED_SET_ZONES();
  struct pollfd fds[2];
  int ret;

  e_status = FrameworkunifiedCreateDispatcherWithoutLoop(LAN_SERVICE_MAIN, 
                                        h_app, argc, argv, &cb_funcs, TRUE);
  if (e_status != eFrameworkunifiedStatusOK) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "FrameworkunifiedCreateDispatcherWithoutLoop: %d", e_status);
    return EXIT_FAILURE;
  }

  e_status = FrameworkunifiedGetDispatcherFD(h_app, &fds[0].fd);
  if (e_status != eFrameworkunifiedStatusOK) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "FrameworkunifiedGetDispatcherFD: %d", e_status);
    return EXIT_FAILURE;
  }

  if ((fds[1].fd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC)) == -1) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "timerfd_create: %d", errno);
    return EXIT_FAILURE;
  }

  struct itimerspec tm;
  tm.it_value.tv_sec = 0;
  tm.it_value.tv_nsec = 100 * 1000 * 1000;
  tm.it_interval.tv_sec = 0;
  tm.it_interval.tv_nsec = 100 * 1000 * 1000;
  if (timerfd_settime(fds[1].fd, 0, &tm, NULL) == -1) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "timerfd_settime: %d", errno);
    return EXIT_FAILURE;
  }
  fds[0].events = POLLIN;
  fds[1].events = POLLIN;

  while (1) {
    ret = poll(fds, sizeof(fds) / sizeof(struct pollfd), -1);
    if (ret < 0) {
      if (errno == EINTR) {
        continue;
      }
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "poll errno:%d", errno);
      continue;
    }

    if ((fds[0].revents & POLLIN) != 0) {
      MessageHandler(h_app);
      continue;
    }

    if ((fds[1].revents & POLLIN) != 0) {
      uint64_t exp;
      read(fds[1].fd, &exp, sizeof(uint64_t));
      TimeoutHandlerCAN(h_app);
      continue;
    }
  }
  return EXIT_SUCCESS;
}