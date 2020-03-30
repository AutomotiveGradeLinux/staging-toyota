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
/// \ingroup tag_NSFramework
/// \brief   This file has the function implementation for creating state machine child thread
///
///
//////////////////////////////////////////////////////////////////////////////////////////////////
#include <sys/prctl.h>

#include <native_service/frameworkunified_sm_hsmframework.h>
#include <native_service/frameworkunified_sm_framework_dispatch.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/ns_utility.hpp>
#include <native_service/ns_system_mode.h>
#include <native_service/frameworkunified_sm_multithreading.h>
#include <native_service/frameworkunified_multithreading.h>

#include <iostream>
#include <string>

#include "frameworkunified_framework_utility.h"
#include "frameworkunified_framework_core.h"
#include "frameworkunified_framework_internal.h"

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedCreateHSMDispatcherChild
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedCreateHSMDispatcherChild(PCSTR childName,
                                       PCSTR parentName,
                                       HANDLE &hChildApp,  // NOLINT  (readability/nolint)
                                       CbFuncPtr pOnThreadStart,
                                       CbFuncPtr pOnThreadStop) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if ((NULL != childName) && (NULL != parentName) && (strlen(childName) <= MAX_NAME_SIZE_APP) &&
      (strlen(parentName) <= MAX_NAME_SIZE_APP)) {
    if (eFrameworkunifiedStatusOK == (eStatus = FrameworkunifiedCreateHSMDispatcher(childName, hChildApp, TRUE))) {
      if (frameworkunifiedCheckValidAppHandle(hChildApp)) {
        CFrameworkunifiedFrameworkApp *pApp = reinterpret_cast< CFrameworkunifiedFrameworkApp * >(hChildApp);

        (reinterpret_cast<CFrameworkunifiedHSMChildFramework *>(pApp->m_pFrameworkunifiedStateMachine))->m_fpStartThread
          = pOnThreadStart;

        (reinterpret_cast<CFrameworkunifiedHSMChildFramework *>(pApp->m_pFrameworkunifiedStateMachine))->m_fpStopThread
          = pOnThreadStop;

        pApp->hParentSndMsgQ = McOpenSender(parentName);
        if (NULL == pApp->hParentSndMsgQ) {
            eStatus = eFrameworkunifiedStatusNullPointer;
            FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "McOpenSender failed");
            return eStatus;
        }
        pApp->uiSessionId = THREAD_SESSION_ID;

        memset(pApp->cParentAppName, 0, sizeof(pApp->cParentAppName));
        memcpy(pApp->cParentAppName, parentName, strlen(parentName));
      } else {
        eStatus = eFrameworkunifiedStatusNullPointer;
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "hChildApp is NULL");
      }
    } else {
      // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "FrameworkunifiedCreateHSMDispatcher error, status=%d", eStatus);
      // LCOV_EXCL_BR_STOP
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Invalid Param received");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

void *child_hsm_thread_proc(void *args) {
  PCData pcdata = *reinterpret_cast< PCData * >(args);   // Create a local copy of data

  try {
    EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

    HANDLE hFrameworkApp = NULL;

    if (eFrameworkunifiedStatusOK == (eStatus = FrameworkunifiedCreateHSMDispatcherChild(pcdata.childName.c_str(),
                                                               pcdata.parentName.c_str(),
                                                               hFrameworkApp,
                                                               pcdata.initFn,
                                                               pcdata.shdnFn))) {
      if (NULL != hFrameworkApp) {
        THApp hChildApp(hFrameworkApp);

        if (pcdata.CbCreateStateMachine) {
          pcdata.CbCreateStateMachine(hChildApp);
          FRAMEWORKUNIFIED_PRINT_HSM(hChildApp)
        }
        const FrameworkunifiedProtocolEvent pcbhs[] = { { SYSTEM_ON_INITIALIZATION,  FRAMEWORKUNIFIED_EVENT(evFrameworkunifiedStart) },
          { SYSTEM_ON_SHUTDOWN,    FRAMEWORKUNIFIED_EVENT(evFrameworkunifiedStop) },
          { SYSTEM_ON_DESTROY,         FRAMEWORKUNIFIED_EVENT(evFrameworkunifiedDestroy) }
        };

        if (eFrameworkunifiedStatusOK != FrameworkunifiedAttachHSMEventsToDispatcher(hChildApp,
                                                           pcdata.parentName.c_str(),
                                                           &pcbhs[ 0 ],
                                                           static_cast<UI_32>(_countof(pcbhs)),
                                                           NULL)) {
          FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error: Failed to attach hsm events to child thread %s",
            pcdata.childName.c_str());
        }

        // set child thread name
        char thread_name[16];
        strncpy(thread_name, pcdata.childName.c_str(), sizeof(thread_name));
        prctl(PR_SET_NAME, thread_name);
        thread_name[15] = '\0';

        setChildThreadSched(pcdata.schedPolicy, pcdata.schedPriority);

        (FrameworkunifiedGetStateMachine(hChildApp))->FrameworkunifiedStart();

        *pcdata.childStatus = eFrameworkunifiedStatusOK;
        if (IsValidWaitBarrier(pthread_barrier_wait(pcdata.barrier))) {
          RunChildDispatcher(hChildApp);
        }
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "hFrameworkApp is NULL");
      }
    } else {
      // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "FrameworkunifiedCreateHSMDispatcherChild error, status=%d", eStatus);
      // LCOV_EXCL_BR_STOP

      *pcdata.childStatus = eFrameworkunifiedStatusFail;
      pthread_barrier_wait(pcdata.barrier);
    }
  } catch (const THApp::Exception &) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error: Failed to create child %s", pcdata.childName.c_str());
  }

  return NULL;
}



////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedCreateHSMChildThread
////////////////////////////////////////////////////////////////////////////////////////////
HANDLE FrameworkunifiedCreateHSMChildThread(HANDLE hApp, PCSTR childName, CbFuncPtr CbInitialize,
                               CbFuncPtr CbShutdown, CbFuncPtr CbCreateStateMachine) {
  HANDLE hChildQ = NULL;
  FrameworkunifiedChildThreadAttr attr;

  if (frameworkunifiedCheckValidAppHandle(hApp) && NULL != childName && NULL != CbInitialize && NULL != CbShutdown &&
      NULL != CbCreateStateMachine) {
    CreateChildThreadAttrInit(&attr);
    hChildQ = CreateChildThread(hApp, childName, CbInitialize, CbShutdown, &attr, CbCreateStateMachine);
  }

  return hChildQ;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedCreateHSMChildThreadWithPriority
////////////////////////////////////////////////////////////////////////////////////////////
HANDLE FrameworkunifiedCreateHSMChildThreadWithPriority(HANDLE hApp, PCSTR childName, CbFuncPtr CbInitialize,
                                           CbFuncPtr CbShutdown, CbFuncPtr CbCreateStateMachine, SI_32 schedPrio) {
  HANDLE hChildQ = NULL;
  FrameworkunifiedChildThreadAttr attr;

  if (frameworkunifiedCheckValidAppHandle(hApp) && NULL != childName && NULL != CbInitialize && NULL != CbShutdown &&
      NULL != CbCreateStateMachine) {
    CreateChildThreadAttrInit(&attr);
    CreateChildThreadAttrSetSched(&attr, eFrameworkunifiedSchedPolicyRR, schedPrio);
    hChildQ = CreateChildThread(hApp, childName, CbInitialize, CbShutdown, &attr, CbCreateStateMachine);
  }
  return hChildQ;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedCreateHSMChildThreadWithAttribute
////////////////////////////////////////////////////////////////////////////////////////////
HANDLE FrameworkunifiedCreateHSMChildThreadWithAttribute(HANDLE hApp,
                                            PCSTR childName,
                                            CbFuncPtr CbInitialize,
                                            CbFuncPtr CbShutdown,
                                            CbFuncPtr CbCreateStateMachine,
                                            const FrameworkunifiedChildThreadAttr *attr) {
  HANDLE hChildQ = NULL;

  if (frameworkunifiedCheckValidAppHandle(hApp) && NULL != childName && NULL != CbInitialize && NULL != CbShutdown &&
      NULL != CbCreateStateMachine) {
    hChildQ = CreateChildThread(hApp, childName, CbInitialize, CbShutdown, attr, CbCreateStateMachine);
  }
  return hChildQ;
}
