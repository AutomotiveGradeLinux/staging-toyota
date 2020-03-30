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

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NSFramework
/// \brief    Framework wrapper over the service directory interface APIs
///
///
///
///////////////////////////////////////////////////////////////////////////////

#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/resource.h>
#include <sys/syscall.h>

#include <native_service/frameworkunified_dispatcher.h>
#include <native_service/frameworkunified_multithreading.h>
#include <native_service/ns_utility.hpp>
#include <native_service/ns_message_center_if.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/ns_system_mode.h>
#include <native_service/ns_logger_if.h>
#include <native_service/frameworkunified_sm_hsmframework.h>
#include <native_service/frameworkunified_sm_framework_dispatch.h>

#include <iostream>

#ifdef DISPATCHER_PROFILER
#include <boost/bind.hpp>
#include "frameworkunified_msgprofiler.h"
#endif

#include "frameworkunified_framework_core.h"
#include "frameworkunified_framework_utility.h"
#include "frameworkunified_framework_internal.h"
#include "frameworkunified_sm_multithreading_internal.h"

__thread HANDLE responseWaitQ = NULL;

////////////////////////////////////////////////////////////////////////////////////////////
/// IsValidWaitBarrier
////////////////////////////////////////////////////////////////////////////////////////////
bool IsValidWaitBarrier(int wbret) {
  return (PTHREAD_BARRIER_SERIAL_THREAD == wbret) ||
         (0                             == wbret);
}

////////////////////////////////////////////////////////////////////////////////////////////
/// DestroyThread
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus DestroyThread(HANDLE hApp) {
  // terminates the dispatcher loop to destroy the thread
  if (responseWaitQ != NULL) {
    /**
     * @todo
     * The responseWaitQ is set in the FrameworkunifiedInvokeSync and is closed in the callback process (DestroyThread) of the FrameworkunifiedDestroyChildThread.
     * Therefore, the responseWaitQ is not closed when FrameworkunifiedInvokeSync is used on the parent.
     */
    McClose(responseWaitQ);
    responseWaitQ = NULL;
  }
  return eFrameworkunifiedStatusExit;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// setChildThreadSched
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus setChildThreadSched(EFrameworkunifiedSchedPolicy policy, SI_32 priority) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  struct sched_param param;
  int get_policy, set_policy;
  int set_priority;

  if (pthread_getschedparam(pthread_self(), &get_policy, &param) != 0) {
    // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error: pthread_getschedparam errno:%d", errno);
    // LCOV_EXCL_BR_STOP
    return eFrameworkunifiedStatusFail;
  }

  if (policy == eFrameworkunifiedSchedPolicyInherit) {
    set_policy = get_policy;
  } else {
    if (policy == eFrameworkunifiedSchedPolicyFIFO) {
      set_policy = SCHED_FIFO;
    } else if (policy == eFrameworkunifiedSchedPolicyRR) {
      set_policy = SCHED_RR;
    } else {
      set_policy = SCHED_OTHER;
    }
  }

  if (priority == INHERIT_PARENT_THREAD_PRIO) {
    if (get_policy != set_policy) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, "Warning: Change policy(%d-%d) but inherit priority", get_policy, set_policy);

      if (set_policy == SCHED_OTHER) {
        set_priority = 0;
      } else {
        set_priority = sched_get_priority_min(set_policy);
      }
    } else {
      set_priority = param.sched_priority;
    }
  } else {
    set_priority = priority;
  }

  switch (set_policy) {
    case SCHED_OTHER:
      param.sched_priority = 0;
      if (pthread_setschedparam(pthread_self(), SCHED_OTHER, &param) != 0) {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error: pthread_setschedparam errno:%d", errno);
        eStatus = eFrameworkunifiedStatusFail;
      } else {
        if (setpriority(PRIO_PROCESS, static_cast<int>(syscall(__NR_gettid)), set_priority) != 0) {
          FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error: setpriority errno:%d", errno);
          eStatus = eFrameworkunifiedStatusFail;
        }
      }
      break;

    case SCHED_FIFO:
    case SCHED_RR:
      param.sched_priority = set_priority;
      if (pthread_setschedparam(pthread_self(), set_policy, &param) != 0) {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error: pthread_setschedparam errno:%d", errno);
        eStatus = eFrameworkunifiedStatusFail;
      }
      break;
  }

  return eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// child_thread_proc
////////////////////////////////////////////////////////////////////////////////////////////
void *child_thread_proc(void *args) {
  PCData pcdata = *reinterpret_cast< PCData * >(args);   // Create a local copy of data

  try {
    EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
    HANDLE hApp = NULL;

    if (eFrameworkunifiedStatusOK == (eStatus = FrameworkunifiedCreateDispatcherChild(hApp,
                                                            pcdata.childName.c_str(),
                                                            pcdata.parentName.c_str()))) {
      // LCOV_EXCL_BR_START 200: If FrameworkunifiedCreateDispatcherChild return eFrameworkunifiedStatusOK, hApp would also be valid.
      if (frameworkunifiedCheckValidAppHandle(hApp)) {
      // LCOV_EXCL_BR_STOP
        THApp hChildApp(hApp);

        const FrameworkunifiedProtocolCallbackHandler pcbhs[] = { { SYSTEM_ON_INITIALIZATION,    pcdata.initFn },
          { SYSTEM_ON_SHUTDOWN,          pcdata.shdnFn },
          { SYSTEM_ON_DESTROY,           DestroyThread }
        };  // LCOV_EXCL_BR_LINE 11:except branch

        if (eFrameworkunifiedStatusOK != FrameworkunifiedAttachCallbacksToDispatcher(hChildApp,
                                                           pcdata.parentName.c_str(),
                                                           &pcbhs[ 0 ],
                                                           static_cast<UI_32>(_countof(pcbhs)))) {
          FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__,
            "Error: Attaching child callbacks to dispatcher %s", pcdata.childName.c_str());
        }

        char thread_name[16];
        strncpy(thread_name, pcdata.childName.c_str(), sizeof(thread_name));
        prctl(PR_SET_NAME, thread_name);
        thread_name[15] = '\0';

        setChildThreadSched(pcdata.schedPolicy, pcdata.schedPriority);

        *pcdata.childStatus = eFrameworkunifiedStatusOK;
        if (IsValidWaitBarrier(pthread_barrier_wait(pcdata.barrier))) {
          RunChildDispatcher(hChildApp);
        }
      } else {
        // LCOV_EXCL_START 200: If FrameworkunifiedCreateDispatcherChild return eFrameworkunifiedStatusOK, hApp would also be valid.
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "hApp is NULL");
        // LCOV_EXCL_STOP
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "FrameworkunifiedCreateDispatcherChild error, status=%d", eStatus);

      *pcdata.childStatus = eFrameworkunifiedStatusFail;
      pthread_barrier_wait(pcdata.barrier);
    }
  } catch (const THApp::Exception &) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error: Failed to create child %s", pcdata.childName.c_str());
  }

  return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// CreateChildThread
////////////////////////////////////////////////////////////////////////////////////////////
HANDLE CreateChildThread(HANDLE hApp, PCSTR childName, CbFuncPtr CbInitialize, CbFuncPtr CbShutdown,
                         const FrameworkunifiedChildThreadAttr *attr, CbFuncPtr CbCreateStateMachine) {
  HANDLE hChildQ = NULL;
  pthread_attr_t tAttr;
  pthread_attr_t *pAttr = NULL;
  SI_32 l_iThrCreate = 0;
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && NULL != childName && strlen(childName) < LIMIT_NAME_SIZE_APP &&
      NULL != CbInitialize && NULL != CbShutdown && NULL != attr) {
    if (attr->schedPolicy < eFrameworkunifiedSchedPolicyInherit || attr->schedPolicy >= eFrameworkunifiedSchedPolicyMAX) {
      l_eStatus = eFrameworkunifiedStatusInvldParam;
    } else {
      if (EOK == pthread_attr_init(&tAttr)) {
        if (EOK != pthread_attr_setinheritsched(&tAttr, PTHREAD_INHERIT_SCHED)) {
          l_eStatus = eFrameworkunifiedStatusFail;
        } else {
          pAttr = &tAttr;
        }
      }
    }

    if (eFrameworkunifiedStatusOK == l_eStatus) {
      pthread_barrier_t barrier;
      if (EOK == pthread_barrier_init(&barrier, NULL, 2)) {
        PCData pc(&barrier, &l_eStatus, FrameworkunifiedGetAppName(hApp), childName, CbInitialize, CbShutdown,
                  attr->schedPolicy, attr->schedPriority, CbCreateStateMachine);
        pthread_t childThread = 0;

        if (NULL != CbCreateStateMachine) {
          l_iThrCreate = pthread_create(&childThread, pAttr, child_hsm_thread_proc, &pc);
        } else {
          l_iThrCreate = pthread_create(&childThread, pAttr, child_thread_proc, &pc);
        }

        if (EOK == l_iThrCreate) {
          if (IsValidWaitBarrier(pthread_barrier_wait(&barrier))) {
            if (eFrameworkunifiedStatusOK == l_eStatus) {
              hChildQ = McOpenSenderChild(childName, childThread);
            }
          }
        }
        pthread_barrier_destroy(&barrier);
      }
    }
#ifdef DISPATCHER_PROFILER
    if (TRUE == FrameworkunifiedMsgProfiler::m_bMsgProfilerEnabled) {
      CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
      if (NULL != pApp->m_pFrameworkunifiedMsgProfiler) {
        pApp->m_pFrameworkunifiedMsgProfiler->AddChildName(childName);
      }
    }
#endif
  }
  return hChildQ;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// CreateChildThreadAttrInit
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CreateChildThreadAttrInit(FrameworkunifiedChildThreadAttr *attr) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (attr == NULL) {
    eStatus = eFrameworkunifiedStatusNullPointer;
  } else {
    memset(attr, 0, sizeof(FrameworkunifiedChildThreadAttr));
    attr->schedPolicy = eFrameworkunifiedSchedPolicyInherit;
    attr->schedPriority = INHERIT_PARENT_THREAD_PRIO;
  }

  return eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// CreateChildThreadAttrSetSched
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CreateChildThreadAttrSetSched(FrameworkunifiedChildThreadAttr *attr, EFrameworkunifiedSchedPolicy policy, SI_32 priority) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (attr == NULL) {
    eStatus = eFrameworkunifiedStatusNullPointer;
  } else {
    if (policy < eFrameworkunifiedSchedPolicyInherit || policy >= eFrameworkunifiedSchedPolicyMAX) {
      eStatus = eFrameworkunifiedStatusInvldParam;
    } else {
      attr->schedPolicy = policy;
      attr->schedPriority = priority;
    }
  }

  return eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedCreateChildThreadAttrInit
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedCreateChildThreadAttrInit(FrameworkunifiedChildThreadAttr *attr) {
  return CreateChildThreadAttrInit(attr);
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedCreateChildThreadAttrSetSched
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedCreateChildThreadAttrSetSched(FrameworkunifiedChildThreadAttr *attr, EFrameworkunifiedSchedPolicy policy, SI_32 priority) {
  return CreateChildThreadAttrSetSched(attr, policy, priority);
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedCreateChildThread
////////////////////////////////////////////////////////////////////////////////////////////
HANDLE FrameworkunifiedCreateChildThread(HANDLE hApp, PCSTR childName, CbFuncPtr CbInitialize
                            , CbFuncPtr CbShutdown) {
  HANDLE hChildQ = NULL;
  FrameworkunifiedChildThreadAttr attr;

  if (frameworkunifiedCheckValidAppHandle(hApp) && NULL != childName && strlen(childName) < LIMIT_NAME_SIZE_APP &&
      NULL != CbInitialize && NULL != CbShutdown) {
    CreateChildThreadAttrInit(&attr);
    hChildQ = CreateChildThread(hApp, childName, CbInitialize, CbShutdown, &attr, NULL);
  }

  return hChildQ;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedCreateChildThreadWithPriority
////////////////////////////////////////////////////////////////////////////////////////////
HANDLE FrameworkunifiedCreateChildThreadWithPriority(HANDLE hApp, PCSTR childName, CbFuncPtr CbInitialize
                                        , CbFuncPtr CbShutdown, SI_32 schedPrio) {
  HANDLE hChildQ = NULL;
  FrameworkunifiedChildThreadAttr attr;

  if (frameworkunifiedCheckValidAppHandle(hApp) && NULL != childName && strlen(childName) < LIMIT_NAME_SIZE_APP  &&
      NULL != CbInitialize && NULL != CbShutdown) {
    CreateChildThreadAttrInit(&attr);
    CreateChildThreadAttrSetSched(&attr, eFrameworkunifiedSchedPolicyFIFO, schedPrio);
    hChildQ = CreateChildThread(hApp, childName, CbInitialize, CbShutdown, &attr, NULL);
  }

  return hChildQ;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedCreateChildThreadWithAttribute
////////////////////////////////////////////////////////////////////////////////////////////
HANDLE FrameworkunifiedCreateChildThreadWithAttribute(HANDLE hApp, PCSTR childName, CbFuncPtr CbInitialize
                                         , CbFuncPtr CbShutdown, const FrameworkunifiedChildThreadAttr *attr) {
  HANDLE hChildQ = NULL;

  if (frameworkunifiedCheckValidAppHandle(hApp) && NULL != childName  && strlen(childName) < LIMIT_NAME_SIZE_APP &&
      NULL != CbInitialize && NULL != CbShutdown && NULL != attr) {
    hChildQ = CreateChildThread(hApp, childName, CbInitialize, CbShutdown, attr, NULL);
  }

  return hChildQ;
}


////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedDestroyChildThread
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedDestroyChildThread(HANDLE hApp, HANDLE hChildQ) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && NULL != hChildQ) {
    eStatus = FrameworkunifiedSendChild(hApp, hChildQ, PROTOCOL_THREAD_DESTROY, 0, NULL);

    if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedJoinChild(hChildQ))) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "FrameworkunifiedJoinChild Error. status: %d", eStatus);
    }

    // close the child message queue handle
    eStatus = McClose(hChildQ);
    hChildQ = NULL;
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}


////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedStartChildThread
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedStartChildThread(HANDLE hApp, HANDLE hChildQ, UI_32 length, PCVOID data) {
  return FrameworkunifiedSendChild(hApp, hChildQ, SYSTEM_ON_INITIALIZATION, length, data);
}


////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedStopChildThread
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedStopChildThread(HANDLE hApp, HANDLE hChildQ, UI_32 length, PCVOID data) {
  return FrameworkunifiedSendChild(hApp, hChildQ, SYSTEM_ON_SHUTDOWN, length, data);
}


////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedSendChild
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSendChild(HANDLE hApp, HANDLE hChildQ, UI_32 iCmd, UI_32 length, PCVOID data) {
  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = reinterpret_cast< CFrameworkunifiedFrameworkApp * >(hApp);
    return McSend(hChildQ, &pApp->cAppName[ 0 ], iCmd, length, data);
  }
  return eFrameworkunifiedStatusInvldHandle;
}


////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedSendParent
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSendParent(HANDLE hChildApp, UI_32 iCmd, UI_32 length, PCVOID data) {
  if (frameworkunifiedCheckValidAppHandle(hChildApp)) {
    CFrameworkunifiedFrameworkApp *pApp = reinterpret_cast< CFrameworkunifiedFrameworkApp * >(hChildApp);
    return McSend(pApp->hParentSndMsgQ, &pApp->cAppName[ 0 ], iCmd, length, data);
  }
  return eFrameworkunifiedStatusInvldHandle;
}



////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedCreateDispatcherChild
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedCreateDispatcherChild(HANDLE &hChildApp,  // NOLINT  (readability/nolint)
                                    PCSTR childName,
                                    PCSTR parentName) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if ((NULL != childName) && (NULL != parentName) && (LIMIT_NAME_SIZE_APP > strlen(parentName)) &&
      (LIMIT_NAME_SIZE_APP > strlen(parentName))) {
    if (eFrameworkunifiedStatusOK == (eStatus = FrameworkunifiedCreateDispatcher(childName, hChildApp, TRUE))) {
      // LCOV_EXCL_BR_START 200: If FrameworkunifiedCreateDispatcher return eFrameworkunifiedStatusOK, hChildApp would also be valid.
      if (frameworkunifiedCheckValidAppHandle(hChildApp)) {
      // LCOV_EXCL_BR_STOP
        CFrameworkunifiedFrameworkApp *pApp = reinterpret_cast< CFrameworkunifiedFrameworkApp * >(hChildApp);

        memset(pApp->cParentAppName, 0, sizeof(pApp->cParentAppName));
        memcpy(pApp->cParentAppName, parentName, strlen(parentName));

        pApp->hParentSndMsgQ = McOpenSender(parentName);
        if (NULL == pApp->hParentSndMsgQ) {
          FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "McOpenSender is NULL");
          eStatus = eFrameworkunifiedStatusNullPointer;
          return eStatus;
        }
        pApp->uiSessionId = THREAD_SESSION_ID;
      } else {
        // LCOV_EXCL_START 200: If FrameworkunifiedCreateDispatcher return eFrameworkunifiedStatusOK, hChildApp would also be valid.
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "hChildApp is NULL");
        eStatus = eFrameworkunifiedStatusNullPointer;
        // LCOV_EXCL_STOP
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "FrameworkunifiedCreateDispatcher error, status=%d", eStatus);
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Invalid parameter received");
    eStatus = eFrameworkunifiedStatusInvldParam;
  }
  return eStatus;
}


////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedJoinChild
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedJoinChild(HANDLE hChildApp) {
  return McJoinChild(hChildApp);
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedGetChildThreadPriority
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedGetChildThreadPriority(HANDLE hChildApp, PSI_32 threadPrio) {
  return McGetChildThreadPriority(hChildApp, threadPrio);
}

EFrameworkunifiedStatus RunChildDispatcher(HANDLE hChildApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  int efd;
  // LCOV_EXCL_BR_START 6: RunChildDispatcher is a internal function, hChildApp would checked in child_thread_proc.
  if (frameworkunifiedCheckValidAppHandle(hChildApp)) {
  // LCOV_EXCL_BR_STOP
#ifdef DISPATCHER_PROFILER
    // Get the application handle
    CFrameworkunifiedFrameworkApp *pApp = reinterpret_cast< CFrameworkunifiedFrameworkApp * >(hChildApp);
    if (TRUE == FrameworkunifiedMsgProfiler::m_bMsgProfilerEnabled) {
      pApp->m_pFrameworkunifiedMsgProfiler = new(std::nothrow) FrameworkunifiedMsgProfiler(FrameworkunifiedGetAppName(hChildApp));
      if (NULL != pApp->m_pFrameworkunifiedMsgProfiler) {
        FrameworkunifiedAttachChildMsgProfilerCallbacksDispatcher(hChildApp);
      }
    }
#endif
    FrameworkunifiedGetDispatcherFD(hChildApp, &efd);

    while (eFrameworkunifiedStatusExit != l_eStatus) {
      l_eStatus = frameworkunifiedFdHandler(hChildApp, efd);
    }
  } else {
    // LCOV_EXCL_START 6: RunChildDispatcher is a internal function, hChildApp would checked in child_thread_proc.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_eStatus = eFrameworkunifiedStatusInvldHandle;
    // LCOV_EXCL_STOP
  }

  return l_eStatus;
}
