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
/// \brief    Native Services Timer Interface code
///
///
///
///////////////////////////////////////////////////////////////////////////////

#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/timerfd.h>
#include <sys/prctl.h>

#include <native_service/ns_timer_if.h>
#include <ns_timer_handle.h>
#include <native_service/ns_message_center_if.h>
#include <ns_timer_internal.h>
#include <native_service/ns_logger_if.h>
#include <native_service/ns_resource_controler.h>

#define INVALID_TIMERFD    -1

#define RUNNING_STATE       0
#define DELETING_STATE      1
#define DELETED_STATE       2

#define MAX_FD_EPOLL 10

int epollFd;    // To listen to multiple timerfd events.
int eventFd;    // To use not complete at epoll_wait
pthread_t timerTh_id = 0;  // Thread ID of the TimerMonitoringThread
pthread_mutex_t m_mtx = PTHREAD_MUTEX_INITIALIZER;

const UI_32 MS_IN_SEC = 1000;
const UI_64 NS_IN_MS  = 1000000;

BOOL DebugFlag = FALSE;

static PVOID TimerMonitoringThread(PVOID args) {
  struct epoll_event events[MAX_FD_EPOLL];
  int nfds;   // The number of events received
  int n;      // Loop counter
  PNSTimerHandle hTimer;
  char *p, name[32];
  uint64_t exp;

  // Thread naming
#define NSTIMER_APPEND_NAME "_T"
#ifndef NSTIMER_SIZE_PROCESSNAME
#define NSTIMER_SIZE_PROCESSNAME 15  // Profiler analysis tool name length limitations
#endif
  prctl(PR_GET_NAME, name);
  name[NSTIMER_SIZE_PROCESSNAME] = '\0';
  if (strlen(name) + strlen(NSTIMER_APPEND_NAME) > NSTIMER_SIZE_PROCESSNAME) {
    p = name + NSTIMER_SIZE_PROCESSNAME - strlen(NSTIMER_APPEND_NAME);
  } else {
    p = name + strlen(name);
  }
  strcpy(p, NSTIMER_APPEND_NAME);
  prctl(PR_SET_NAME, name);

  for (;;) {
    nfds = epoll_wait(epollFd, events, MAX_FD_EPOLL, -1);
    if (-1 != nfds) {
      for (n = 0; n < nfds; ++n) {
        hTimer = (PNSTimerHandle)events[n].data.ptr;
        if (NULL != hTimer) {
          if (-1 == read(hTimer->timerfd, &exp, sizeof(uint64_t))) {
            if (errno != EAGAIN) {
              // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
              FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Failed to read in timeout : fd=%d, errno=%d",
                                                                hTimer->timerfd, errno);
              // LCOV_EXCL_BR_STOP
            }
            continue;
          }

          if (RUNNING_STATE == hTimer->timerState) {
            if (NULL != hTimer->tTimerInfo) {
              EFrameworkunifiedStatus eStatus;
              HANDLE hReceiver = NULL;
              if (frameworkunifiedAcquireResouce(FRAMEWORKUNIFIED_RES_TIMER, hTimer->tTimerInfo->q_name, (long *)&hReceiver) < 0) {
                if ((hReceiver = McOpenSender(hTimer->tTimerInfo->q_name)) == NULL) {
                  // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
                  FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : McOpenSender is Failed");
                  // LCOV_EXCL_BR_STOP
                  continue;
                } else {
                  if (frameworkunifiedRegistResouce(FRAMEWORKUNIFIED_RES_TIMER, hTimer->tTimerInfo->q_name, (long)hReceiver, 1) < 0) {
                    // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
                    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : frameworkunifiedRegistResouce is Failed : q_name=%s",
                                                                hTimer->tTimerInfo->q_name);
                    // LCOV_EXCL_BR_STOP
                  }
                }
              }

              eStatus = McSendWithPriority(hReceiver,
                                           TIMER_QUE,
                                           hTimer->tTimerInfo->iCmd,
                                           0,
                                           NULL,
                                           eFrameworkunifiedMsgPrioEmergency,
                                           0);
              if (eFrameworkunifiedStatusOK != eStatus) {
                // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
                FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : McSendWithPriority to %s is Failed, eStatus=%d", hTimer->tTimerInfo->q_name,
                       eStatus);
                // LCOV_EXCL_BR_STOP
              }
            } else {
              // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
              FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR(RUNNING_STATE) : tTimerInfo is NULL");
              // LCOV_EXCL_BR_STOP
            }
          } else if (DELETED_STATE == hTimer->timerState) {
            if (-1 == epoll_ctl(epollFd, EPOLL_CTL_DEL, hTimer->timerfd, events)) {
              // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
              FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : epoll_ctl(DEL) Failed, fd=%d, errno=%d",
                     hTimer->timerfd, errno);
              // LCOV_EXCL_BR_STOP
            }

            if (-1 == close(hTimer->timerfd)) {
              // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
              FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : close(fd=%d) Failed, errno=%d", hTimer->timerfd, errno);
              // LCOV_EXCL_BR_STOP
            }

            hTimer->timerfd = INVALID_TIMERFD;

            if (NULL != hTimer->tTimerInfo) {
              HANDLE hReceiver;
              if (frameworkunifiedAcquireResouce(FRAMEWORKUNIFIED_RES_TIMER, hTimer->tTimerInfo->q_name, (long *)&hReceiver) >= 0) {
                if (frameworkunifiedReleaseResouce(FRAMEWORKUNIFIED_RES_TIMER, hTimer->tTimerInfo->q_name) <= 0) {
                  frameworkunifiedUnregistResouce(FRAMEWORKUNIFIED_RES_TIMER, hTimer->tTimerInfo->q_name);
                  McClose(hReceiver);
                }
              }
              free(hTimer->tTimerInfo->q_name);
              free(hTimer->tTimerInfo);   // delete the timer info
              hTimer->tTimerInfo = NULL;
            } else {
              // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
              FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR(DELETED_STATE) : tTimerInfo is NULL");
              // LCOV_EXCL_BR_STOP
            }

            free(hTimer);   // delete the handle
            events[n].data.ptr = NULL;
          } else {
            // do nothing
          }
        } else {
          // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
          FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : hTimer is NULL");
          // LCOV_EXCL_BR_STOP
        }
      }
    } else {
      if (errno == EINTR) {
        // signal interrupt
      } else {
        // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : epoll_wait Failed, errno=%d", errno);
        // LCOV_EXCL_BR_STOP
      }
    }
  }
  return NULL;
}

static EFrameworkunifiedStatus CreateTimerMonitoringThread() {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  struct epoll_event ev;  // Info struct to associate with multiwaiting FD
  int ret;

  pthread_mutex_lock(&m_mtx);

  // Create the TimerMonitoringThread If the thread has not been generated.
  if (0 == timerTh_id) {
    epollFd = epoll_create1(EPOLL_CLOEXEC);
    if (-1 != epollFd) {
      eventFd = eventfd(0, EFD_CLOEXEC);
      if (-1 != eventFd) {
        ev.events = EPOLLIN;
        ev.data.fd = eventFd;
        if (-1 != epoll_ctl(epollFd, EPOLL_CTL_ADD, eventFd, &ev)) {
          if (0 != (ret = pthread_create(&timerTh_id, NULL, TimerMonitoringThread, NULL))) {
            eStatus = eFrameworkunifiedStatusFail;
            // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
            FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Failed to pthread_create : errno %d", ret);
            // LCOV_EXCL_BR_STOP
          }
        } else {
          eStatus = eFrameworkunifiedStatusFail;
          // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
          FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : epoll_ctl(eventFd=%d, ADD) Failed, status=%d, errno=%d", eventFd, eStatus,
                 errno);
          // LCOV_EXCL_BR_STOP
        }
      } else {
        eStatus = eFrameworkunifiedStatusFail;
        // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR :eventfd Failed, status=%d, errno=%d", eStatus, errno);
        // LCOV_EXCL_BR_STOP
      }
    } else {
      eStatus = eFrameworkunifiedStatusFail;
      // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : epoll_create1 Failed, status=%d, errno=%d",
             eStatus, errno);
      // LCOV_EXCL_BR_STOP
    }

  }

  if (TRUE == DebugFlag) {  // LCOV_EXCL_BR_LINE 7: debug code
    // LCOV_EXCL_START 7: debug code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NS_IMP_INFO, __FUNCTION__, "[DEBUG] sleep 3 Sec START.");
    sleep(3);
    FRAMEWORKUNIFIEDLOG(ZONE_NS_IMP_INFO, __FUNCTION__, "[DEBUG] sleep 3 Sec END.");
    // LCOV_EXCL_STOP
  }
  pthread_mutex_unlock(&m_mtx);

  return eStatus;
}

HANDLE NS_TimerCreate(NSTimerInfo timer_info, eNSTimerCallbackMechanism cbMech, HANDLE sndMqHndl) {
  PNSTimerHandle hTimer = NULL;

  if ((NULL != sndMqHndl) && (cbMech == CALLBACK_MESSAGE)) {
    EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

    PTimerInfo pTimerInfo = NULL;
    struct epoll_event ev;  // Info struct to associate with multiwaiting FD
    int timerfd = INVALID_TIMERFD;

    eStatus = CreateTimerMonitoringThread();
    if (eFrameworkunifiedStatusOK == eStatus) {
      hTimer = (PNSTimerHandle)malloc(sizeof(NSTimerHandle));
      if (NULL != hTimer) {  // LCOV_EXCL_BR_LINE 5: malloc's error case
        pTimerInfo = (PTimerInfo)malloc(sizeof(TTimerInfo));

        if (NULL != pTimerInfo) {  // LCOV_EXCL_BR_LINE 5: malloc's error case
          hTimer->timerState = RUNNING_STATE;

          // set the timer info structure
          PCSTR pName = McGetQueueName(sndMqHndl);
          if (NULL != pName) {
            pTimerInfo->q_name = strdup(pName);
          } else {
            pTimerInfo->q_name = NULL;
            eStatus = eFrameworkunifiedStatusNullPointer;
            // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
            FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : McGetQueueName(sndMqHndl) Failed, status=%d", eStatus);
            // LCOV_EXCL_BR_STOP
          }

          if (eFrameworkunifiedStatusOK == eStatus) {
            pTimerInfo->iCmd = timer_info.iCmd;

            hTimer->tTimerInfo = pTimerInfo;
            if (INVALID_TIMERFD != (timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC|TFD_NONBLOCK))) {
              ev.events = EPOLLIN;
              ev.data.ptr = hTimer;
              if (-1 != epoll_ctl(epollFd, EPOLL_CTL_ADD, timerfd, &ev)) {
                hTimer->timerfd = timerfd;
                if (eFrameworkunifiedStatusOK == NS_TimerSetTime(hTimer, timer_info)) {
                  // set the interval in timer handle
                  hTimer->itime.it_value.tv_sec = (__time_t)timer_info.t_sec;
                  hTimer->itime.it_value.tv_nsec = (__syscall_slong_t)timer_info.t_nsec;
                  hTimer->itime.it_interval.tv_sec = (__time_t)timer_info.rpt_sec;
                  hTimer->itime.it_interval.tv_nsec = (__syscall_slong_t)timer_info.rpt_nsec;

                } else {
                  eStatus = eFrameworkunifiedStatusErrOther;

                  // if timer interval is not set or memory is not allocated for hTimer
                  close(timerfd);
                  timerfd = INVALID_TIMERFD;
                }
              } else {
                eStatus = eFrameworkunifiedStatusFail;
                // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
                FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : epoll_ctl(timerfd=%d, ADD) Failed, status=%d, errno=%d", timerfd, eStatus,
                       errno);
                // LCOV_EXCL_BR_STOP
              }
            } else {
              eStatus = eFrameworkunifiedStatusErrOther;
            }
          }
        } else {
          // LCOV_EXCL_START 5: malloc's error case
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          eStatus = eFrameworkunifiedStatusNullPointer;
          FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : (PTimerInfo)malloc(sizeof(TTimerInfo)) Failed, status=%d", eStatus);
          // LCOV_EXCL_STOP
        }

        if (eFrameworkunifiedStatusOK != eStatus) {
          if (NULL != pTimerInfo) {
            if (NULL != pTimerInfo->q_name) {
              free(pTimerInfo->q_name);
              pTimerInfo->q_name = NULL;
            }

            free(pTimerInfo);
            pTimerInfo = NULL;
          }

          if (NULL != hTimer) {
            free(hTimer);
            hTimer = NULL;
          }
        }
      } else {
        // LCOV_EXCL_START 5: malloc's error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : malloc(sizeof(NSTimerHandle)) Failed");
        // LCOV_EXCL_STOP
      }
    } else {
      // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : CreateTimerMonitoringThread Failed, status=%d", eStatus);
      // LCOV_EXCL_BR_STOP
    }
  } else {
    // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : Invalid param");
    // LCOV_EXCL_BR_STOP
  }

  return hTimer;
}

EFrameworkunifiedStatus NS_TimerDelete(HANDLE hTimer) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  if (NULL != hTimer) {
    PNSTimerHandle tTimerHndl = (PNSTimerHandle)hTimer;
    /**
     * @todo
     *  Dropping by typing the service handle instead of the timer handle
     */
    if (DELETED_STATE != tTimerHndl->timerState) {
      if (NULL != tTimerHndl->tTimerInfo) {
        tTimerHndl->timerState = DELETING_STATE;
        NSTimerInfo timer_info = {1, 0, tTimerHndl->tTimerInfo->iCmd, 0, 0};

        if (eFrameworkunifiedStatusOK != (eStatus = NS_TimerSetTime(tTimerHndl, timer_info))) {
          // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
          FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, "ERROR : NS_TimerSetTime for Timer delete Failed, status=%d", eStatus);
          // LCOV_EXCL_BR_STOP
        }

        tTimerHndl->timerState = DELETED_STATE;
      } else {
        eStatus = eFrameworkunifiedStatusInvldParam;
        // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : tTimerInfo is NULL, status=%d", eStatus);
        // LCOV_EXCL_BR_STOP
      }
    } else {
      eStatus = eFrameworkunifiedStatusFail;
      // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : Timer is not RUNNING State, timerStatus=%d, status=%d",
             tTimerHndl->timerState, eStatus);
      // LCOV_EXCL_BR_STOP
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
    // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : hTimer is NULL, status=%d", eStatus);
    // LCOV_EXCL_BR_STOP
  }

  return eStatus;
}

EFrameworkunifiedStatus NS_TimerSetTime(HANDLE hTimer, NSTimerInfo timer_info) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (NULL != hTimer) {
    PNSTimerHandle tTimerHndl = (PNSTimerHandle)hTimer;
    struct itimerspec itime;

    if (DELETED_STATE != tTimerHndl->timerState) {
      // set periodic interval values
      itime.it_value.tv_sec = (__time_t)timer_info.t_sec;
      itime.it_value.tv_nsec = (__syscall_slong_t)timer_info.t_nsec;

      // set periodic interval values
      itime.it_interval.tv_sec = (__time_t)timer_info.rpt_sec;
      itime.it_interval.tv_nsec = (__syscall_slong_t)timer_info.rpt_nsec;

      if (-1 != timerfd_settime(tTimerHndl->timerfd, 0, &itime, NULL)) {
        // updated the value of the timer values in the handler
        tTimerHndl->itime = itime;
      } else {
        eStatus = eFrameworkunifiedStatusFail;
        // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : timerfd_settime is failed, status=%d, errno=%d", eStatus, errno);
        // LCOV_EXCL_BR_STOP
      }
    } else {
      eStatus = eFrameworkunifiedStatusFail;
      // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : Timer is not RUNNING State, status=%d", eStatus);
      // LCOV_EXCL_BR_STOP
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
    // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : hTimer is NULL, status=%d", eStatus);
    // LCOV_EXCL_BR_STOP
  }

  return eStatus;
}

EFrameworkunifiedStatus NS_TimerGetTime(HANDLE hTimer, NSTimerInfo *timer_info) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if ((NULL != hTimer) && (NULL != timer_info)) {
    PNSTimerHandle tTimerHndl = (PNSTimerHandle)hTimer;
    struct itimerspec itime;

    if (DELETED_STATE != tTimerHndl->timerState) {
      if (-1 != timerfd_gettime(tTimerHndl->timerfd, &itime)) {
        // set periodic interval values

//        timer_info->t_sec = (UI_32)itime.it_value.tv_sec;
        timer_info->t_sec = itime.it_value.tv_sec;
        timer_info->t_nsec = (UI_64)itime.it_value.tv_nsec;

        // set periodic interval values

//        timer_info->rpt_sec = (UI_32)itime.it_interval.tv_sec;
        timer_info->rpt_sec = itime.it_interval.tv_sec;
        timer_info->rpt_nsec = (UI_64)itime.it_interval.tv_nsec;
      } else {
        eStatus = eFrameworkunifiedStatusFail;
        // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : timerfd_gettime is failed, status=%d, errno=%d", eStatus, errno);
        // LCOV_EXCL_BR_STOP
      }
    } else {
      eStatus = eFrameworkunifiedStatusFail;
      // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : Timer is not RUNNING State, status=%d", eStatus);
      // LCOV_EXCL_BR_STOP
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
    // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR :  Invalid param, status=%d", eStatus);
    // LCOV_EXCL_BR_STOP
  }

  return eStatus;
}


//UI_32 WholeSeconds(UI_32 ms) {
//  return ms / MS_IN_SEC;
//}
time_t WholeSeconds(UI_32 ms) {
  return (time_t)(ms / MS_IN_SEC);
}


UI_32 RemainderMs(UI_32 ms) {
  return ms % MS_IN_SEC;
}

UI_64 MSToNS(UI_32 ms) {
  return (UI_64)ms * NS_IN_MS;
}

void NS_TimerDebugOn(BOOL FlagState) {  // LCOV_EXCL_START 7: debug code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  if (DebugFlag != FlagState) {
    DebugFlag = FlagState;
    if (TRUE == DebugFlag) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_IMP_INFO, __FUNCTION__, "NS_Timer debugging is enabled.");
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_IMP_INFO, __FUNCTION__, "NS_Timer debugging is disabled.");
    }
  }
}
// LCOV_EXCL_STOP
