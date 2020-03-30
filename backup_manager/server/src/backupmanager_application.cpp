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

#include <native_service/ns_backup.h>
#include <agl_thread.h>
#include <system_service/ss_system_if.h>
#include <system_service/ss_system_manager_notifications.h>
#include <native_service/frameworkunified_multithreading.h>

//#include <nv_hal.h>
#include <signal.h>
#include <cerrno>
#include <cstdio>
#include "bkup_api.h"
#include "bkup_param.h"
#include "bkup_backupmanagerlog.h"
#include "bkup_process.h"

pthread_t g_work_thread_id = 0;
pthread_t g_delay_thread_id = 0;
pthread_t g_nand_thread_id = 0;
HANDLE g_msg_handle_thread = NULL;

typedef void (* signal_handler)(int);

void SignalHandlerFuncForMsgHandleThread(int signum) {
  pthread_exit(0);
}

EFrameworkunifiedStatus MsgHandleThreadStart(HANDLE h_app) {
  signal_handler p_signal = SignalHandlerFuncForMsgHandleThread;
  signal(SIGUSR1, p_signal);

  g_work_thread_id = pthread_self();

  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  // LCOV_EXCL_BR_START 4: NSFW error case
  if ((e_status = FrameworkunifiedAttachCallbackToDispatcher(h_app, FRAMEWORKUNIFIED_ANY_SOURCE, BACKUP_CID, BkupHandler)) != eFrameworkunifiedStatusOK) {
  // LCOV_EXCL_BR_STOP 4: NSFW error case
    // LCOV_EXCL_START 4: NSFW error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "FrameworkunifiedAttachCallbacksToDispatcher e_status:%d\n", e_status);
    exit(EXIT_FAILURE);
    // LCOV_EXCL_STOP 4: NSFW error case
  }

  return e_status;
}

EFrameworkunifiedStatus MsgHandleThreadStop(HANDLE h_app) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  return e_status;
}

EFrameworkunifiedStatus FrameworkunifiedOnInitialization(HANDLE h_app) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  pthread_attr_t t_attr;
  pthread_t t;

  // LCOV_EXCL_BR_START 4: NSFW error case
  if ((e_status = FrameworkunifiedRegisterServiceAvailabilityNotification(h_app, NTFY_BackupMgr_Availability)) != eFrameworkunifiedStatusOK) {
  // LCOV_EXCL_BR_STOP 4: NSFW error case
    // LCOV_EXCL_START 4: NSFW error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "FrameworkunifiedRegisterServiceAvailabilityNotification e_status:%d\n", e_status);
    exit(EXIT_FAILURE);
    // LCOV_EXCL_STOP 4: NSFW error case
  }

  if (pthread_attr_init(&t_attr) < 0) {  // LCOV_EXCL_BR_LINE 5:pthread_attr_init's error case.
    // LCOV_EXCL_START 5:pthread_attr_init's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "pthread_attr_init:%s", strerror(errno));
    exit(EXIT_FAILURE);
    // LCOV_EXCL_STOP 5:pthread_attr_init's error case.
  }

  // LCOV_EXCL_BR_START 5:pthread_attr_setinheritsched's error case.
  if (pthread_attr_setinheritsched(&t_attr, PTHREAD_INHERIT_SCHED) < 0) {
  // LCOV_EXCL_BR_STOP 5:pthread_attr_setinheritsched's error case.
    // LCOV_EXCL_START 5:pthread_attr_setinheritsched's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "pthread_attr_setinheritsched:%s", strerror(errno));
    exit(EXIT_FAILURE);
    // LCOV_EXCL_STOP 5:pthread_attr_setinheritsched's error case.
  }

  if (pthread_create(&t, &t_attr, BkupNandThread, NULL) < 0) {  // LCOV_EXCL_BR_LINE 5:pthread_create's error case.
    // LCOV_EXCL_START 5:pthread_create's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "pthread_create:%s", strerror(errno));
    exit(EXIT_FAILURE);
    // LCOV_EXCL_STOP 5:pthread_create's error case.
  }

  if (pthread_create(&t, &t_attr, BkupDelayThread, NULL) < 0) {  // LCOV_EXCL_BR_LINE 5:pthread_create's error case.
    // LCOV_EXCL_START 5:pthread_create's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "pthread_create:%s", strerror(errno));
    exit(EXIT_FAILURE);
    // LCOV_EXCL_STOP 5:pthread_create's error case.
  }

//  if ((e_status = InitNv()) != eFrameworkunifiedStatusOK) {
//    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "InitNv e_status:%d", e_status);
//    exit(EXIT_FAILURE);
//  }

  if (BckupParamInit() < 0) {  // LCOV_EXCL_BR_LINE 5:C API's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    exit(EXIT_FAILURE);  // LCOV_EXCL_LINE 5:C API's error case.
  }

  // LCOV_EXCL_BR_START 4: NSFW error case
  if ((e_status = FrameworkunifiedAttachCallbackToDispatcher(h_app, FRAMEWORKUNIFIED_ANY_SOURCE, BACKUP_CID, BkupHandler)) != eFrameworkunifiedStatusOK) {
  // LCOV_EXCL_BR_STOP 4: NSFW error case
    // LCOV_EXCL_START 4: NSFW error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "FrameworkunifiedAttachCallbacksToDispatcher e_status:%d\n", e_status);
    exit(EXIT_FAILURE);
    // LCOV_EXCL_STOP 4: NSFW error case
  }


  g_msg_handle_thread = FrameworkunifiedCreateChildThread(h_app, SERVICE_BACKUP_MANAGER_MSG_HANDLE_THREAD, MsgHandleThreadStart, MsgHandleThreadStop);  // LCOV_EXCL_BR_LINE 11:Unexpected branch  // NOLINT (whitespace/line_length)
  if (g_msg_handle_thread == NULL) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    // LCOV_EXCL_START 4: NSFW error case.
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        "Fail to create SERVICE_BACKUP_MANAGER_MSG_HANDLE_THREAD");
    // LCOV_EXCL_STOP 4: NSFW error case.
  } else {
    if (eFrameworkunifiedStatusOK != (e_status = FrameworkunifiedStartChildThread(h_app, g_msg_handle_thread, 0, NULL))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT (whitespace/line_length)
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Fail to Start SERVICE_BACKUP_MANAGER_MSG_HANDLE_THREAD. Status:%#x", e_status);  // LCOV_EXCL_LINE 4: NSFW error case.  // NOLINT (whitespace/line_length)
    }
  }

  // LCOV_EXCL_BR_START 4: NSFW error case
  if ((e_status = FrameworkunifiedSubscribeNotificationWithCallback(h_app,
                                                       NTFY_SSSystemMgrPowerOnOff,
                                                       bkup_power_handler)) != eFrameworkunifiedStatusOK) {
  // LCOV_EXCL_BR_STOP 4: NSFW error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "FrameworkunifiedSubscribeNotificationWithCallback eStatus:%d", e_status);  // LCOV_EXCL_LINE 4: NSFW error case.  // NOLINT (whitespace/line_length)
  }

  e_status = BkupInitHandler(h_app);  // LCOV_EXCL_BR_LINE 11:unexpected branch

  // LCOV_EXCL_BR_START 4: NSFW error case
  if ((e_status = FrameworkunifiedPublishServiceAvailability(h_app, TRUE)) != eFrameworkunifiedStatusOK) {
  // LCOV_EXCL_BR_STOP 4: NSFW error case
    // LCOV_EXCL_START 4: NSFW error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "FrameworkunifiedPublishServiceAvailability e_status:%d\n", e_status);
    exit(EXIT_FAILURE);
    // LCOV_EXCL_STOP 4: NSFW error case
  }

  return e_status;
}

// LCOV_EXCL_START 14 Resident process, not called by NSFW
EFrameworkunifiedStatus FrameworkunifiedOnDestroy(HANDLE h_app) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __func__, "called");
  return eFrameworkunifiedStatusOK;
}
// LCOV_EXCL_STOP 14 Resident process, not called by NSFW

EFrameworkunifiedStatus FrameworkunifiedOnStart(HANDLE h_app) {
  // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __func__, "called");
  // LCOV_EXCL_BR_STOP 15:marco defined in "native_service/ns_logger_if.h"
  return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus FrameworkunifiedOnStop(HANDLE h_app) {
  EFrameworkunifiedStatus e_status = BkupTerminateHandler(h_app);

  if (FrameworkunifiedPublishServiceAvailability(h_app, FALSE) != eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
    // LCOV_EXCL_START 15:marco defined in "native_service/ns_logger_if.h"
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "FrameworkunifiedPublishServiceAvailability fail\n");
    // LCOV_EXCL_STOP 15:marco defined in "native_service/ns_logger_if.h"
  }

  // LCOV_EXCL_BR_START 4: NSFW error case
  if (eFrameworkunifiedStatusOK == (FrameworkunifiedStopChildThread(h_app, g_msg_handle_thread, 0, NULL))) {
  // LCOV_EXCL_BR_STOP 4: NSFW error case
    FrameworkunifiedDestroyChildThread(h_app, g_msg_handle_thread);
    g_msg_handle_thread = NULL;
  }

  int ret2 = pthread_kill(g_work_thread_id, SIGUSR1);
  if (ESRCH == ret2) {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__,
        "thread is already exit !");
  } else if (EINVAL == ret2) {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__,
        "signal is invalid !");
  } else {
    pthread_join(g_work_thread_id, NULL);
  }

  ret2 = pthread_kill(g_nand_thread_id, SIGUSR1);
  if (ESRCH == ret2) {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__,
        "thread is already exit !");
  } else if (EINVAL == ret2) {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__,
        "signal is invalid !");
  } else {
    pthread_join(g_nand_thread_id, NULL);
  }

  ret2 = pthread_kill(g_delay_thread_id, SIGUSR1);
  if (ESRCH == ret2) {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__,
        "thread is already exit !");
  } else if (EINVAL == ret2) {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__,
        "signal is invalid !");
  } else {
    pthread_join(g_delay_thread_id, NULL);
  }

  return e_status;
}

EFrameworkunifiedStatus FrameworkunifiedOnPreStart(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __func__, "called");
  return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus FrameworkunifiedOnPreStop(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __func__, "called");
  return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus FrameworkunifiedOnBackgroundStart(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __func__, "called");
  return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus FrameworkunifiedOnBackgroundStop(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __func__, "called");
  return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus FrameworkunifiedOnDebugDump(HANDLE h_app) {  // LCOV_EXCL_START 7:debug code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __func__, "called");
  return eFrameworkunifiedStatusOK;
}
// LCOV_EXCL_STOP 7:debug code

EFrameworkunifiedStatus FrameworkunifiedCreateStateMachine(HANDLE h_app) {  // LCOV_EXCL_START 7:debug code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __func__, "called");
  return eFrameworkunifiedStatusOK;
}
// LCOV_EXCL_STOP 7:debug code
