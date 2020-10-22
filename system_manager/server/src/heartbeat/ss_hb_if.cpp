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
/// \ingroup  tag_SystemManager
/// \brief    This file provides support for the application heartbeat system.
///
///////////////////////////////////////////////////////////////////////////////
#include <native_service/frameworkunified_application.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_service_protocol.h>
#include <ss_hb_thread.h>
#include <system_service/ss_sm_client_if.h>
#include <system_service/ss_heartbeat_notifications.h>
#include <system_service/ss_heartbeat_service_protocol.h>
#include <system_service/ss_templates.h>
#include <new>
#include "ss_sm_systemmanagerlog.h"

template<typename C, eFrameworkunifiedStatus (C::*M)(HANDLE)> EFrameworkunifiedStatus HBThreadCallback(HANDLE hThread) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusNullPointer;
  C * pObj = static_cast<C *>(FrameworkunifiedGetThreadSpecificData(hThread));
  if (pObj) {  // LCOV_EXCL_BR_LINE 4:pObj must not be NULL
    eStatus = (pObj->*M)(hThread);
  }
  return eStatus;
}

static VOID DeletePObj(CHeartBeatThread * pObj) {  // LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  delete pObj;
}
// LCOV_EXCL_STOP

static VOID DeleteHBTimer(CHeartBeatThread * pObj) {  // LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus l_eStatus = pObj->HeartBeatTimersDelete();
  LOG_STATUS_IF_ERRORED(l_eStatus, "pObj->HeartBeatTimersDelete()");
}
// LCOV_EXCL_STOP

#define DetachParentCbThreadProtocols(hThread, handler)                           \
{                                                                                 \
  EFrameworkunifiedStatus l_eStatus = DetachParentCallbacksFromInterfaceunifiedDispatcher(                  \
                            hThread,                                              \
                            handler,                                              \
                            static_cast<UI_32>(_countof(handler)));               \
  LOG_STATUS_IF_ERRORED(l_eStatus, "DetachParentCallbacksFromInterfaceunifiedDispatcher()")    \
}

#define DetachCbProtocols(hThread, f_pStr, f_protocolHandlers, hSession)          \
{                                                                                 \
  EFrameworkunifiedStatus l_eStatus =                                                          \
                  DetachCallbacksFromInterfaceunifiedDispatcher(                               \
                            hThread,                                              \
                            f_pStr,                                               \
                            f_protocolHandlers,                                   \
                            static_cast<UI_32>(_countof(f_protocolHandlers)),     \
                            hSession);                                            \
  LOG_STATUS_IF_ERRORED(l_eStatus, "DetachCallbacksFromInterfaceunifiedDispatcher()")          \
}

/*****************************************************************************/
/**
 @ingroup: SS_SystemManager
 @brief: HBThreadStart: function would be called at the time of starting of the thread
 @note: .
 @param HANDLE  - Handle to message queue of HeartBeat Service.
 @return EFrameworkunifiedStatus
 */
/*****************************************************************************/
EFrameworkunifiedStatus HBThreadStart(HANDLE hThread) {
  EFrameworkunifiedStatus l_eStatus;
  FRAMEWORKUNIFIEDLOG0(ZONE_FUNC, __FUNCTION__, "+");

  FrameworkunifiedProtocolCallbackHandler HBThreadCommonHandlers[] = {
    // Command ID,                       Call back functions
  { SS_HEARTBEAT_RESPONSE,           HBThreadCallback<CHeartBeatThread, &CHeartBeatThread::HBOnClientResponse> },
  { SS_HEARTBEAT_PRINT_CONNECTIONS,  HBThreadCallback<CHeartBeatThread, &CHeartBeatThread::HBOnPrintConnections> },
  { SS_HEARTBEAT_PRINT_STACK,        HBThreadCallback<CHeartBeatThread, &CHeartBeatThread::HBOnPrintStack> } };  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  FrameworkunifiedProtocolCallbackHandler HBThreadProtocolHandlers[] = {
    //    Command ID,              Call back functions
  { SS_HEARTBEAT_START,               HBThreadCallback<CHeartBeatThread, &CHeartBeatThread::HBOnStartThread> },
  { SS_HEARTBEAT_STOP,                HBThreadCallback<CHeartBeatThread, &CHeartBeatThread::HBOnStopThread> },
  { SS_HEARTBEAT_DELETE_MODULE_ENTRY, HBThreadCallback<CHeartBeatThread, &CHeartBeatThread::HBOnRemoveModule> },
  { SS_HEARTBEAT_APPEND_MODULE_ENTRY, HBThreadCallback<CHeartBeatThread, &CHeartBeatThread::HBOnAppendModule> },
  { SS_HEARTBEAT_PERIODIC_STATUS_REQ, HBThreadCallback<CHeartBeatThread,
                                                                &CHeartBeatThread::HBOnPeriodicStatusRequest> },
  { SS_HEARTBEAT_AVAIL_CHECK_REQ,     HBThreadCallback<CHeartBeatThread, &CHeartBeatThread::HBOnAvailCheckRequest> }
  };  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  CHeartBeatThread * pObj = new (std::nothrow) CHeartBeatThread(hThread);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  if (NULL == pObj) {  // LCOV_EXCL_BR_LINE 5: Standard lib
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_eStatus = eFrameworkunifiedStatusNullPointer;  // LCOV_EXCL_LINE 5: Standard lib
    LOG_ERROR("new(std::nothrow) CHeartBeatThread(hThread)");  // LCOV_EXCL_LINE 5: Standard lib
  // LCOV_EXCL_BR_START 6:always return eFrameworkunifiedStatusOK
  } else if (eFrameworkunifiedStatusOK != (l_eStatus = pObj->HeartBeatTimerInit(hThread))) {
  // LCOV_EXCL_BR_STOP
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("pObj->HeartBeatTimerInit(hThread)");  // LCOV_EXCL_LINE 6:always return eFrameworkunifiedStatusOK
    DeletePObj(pObj);  // LCOV_EXCL_LINE 6:always return eFrameworkunifiedStatusOK
  // LCOV_EXCL_BR_START 4: NSFW error case.
  } else if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedAttachParentCallbacksToDispatcher(hThread,
              HBThreadProtocolHandlers, static_cast<UI_32>(_countof(HBThreadProtocolHandlers))))) {
  // LCOV_EXCL_BR_STOP
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("FrameworkunifiedAttachParentCallbacksToDispatcher(HBThreadProtocolHandlers)");
    DeleteHBTimer(pObj);
    DeletePObj(pObj);
    // LCOV_EXCL_STOP
  // LCOV_EXCL_BR_START 4: NSFW error case.
  } else if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedAttachParentCallbacksToDispatcher(hThread,
                HBThreadCommonHandlers, static_cast<UI_32>(_countof(HBThreadCommonHandlers))))) {
  // LCOV_EXCL_BR_STOP
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("FrameworkunifiedAttachParentCallbacksToDispatcher(HBThreadCommonHandlers)");
    DetachParentCbThreadProtocols(hThread, HBThreadProtocolHandlers);
    DeleteHBTimer(pObj);
    DeletePObj(pObj);
    // LCOV_EXCL_STOP
  } else if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedAttachCallbacksToDispatcher(hThread, FRAMEWORKUNIFIED_ANY_SOURCE, HBThreadCommonHandlers, static_cast<UI_32>(_countof(HBThreadCommonHandlers)), NULL))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT(whitespace/line_length)
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("FrameworkunifiedAttachCallbacksToDispatcher("
            "FRAMEWORKUNIFIED_ANY_SOURCE, HBThreadCommonHandlers)");
    DetachParentCbThreadProtocols(hThread, HBThreadCommonHandlers);
    DetachParentCbThreadProtocols(hThread, HBThreadProtocolHandlers);
    DeleteHBTimer(pObj);
    DeletePObj(pObj);
    // LCOV_EXCL_STOP
  } else {
    l_eStatus = FrameworkunifiedSetThreadSpecificData(hThread, pObj);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
      // LCOV_EXCL_START 4: NSFW error case.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      LOG_ERROR("FrameworkunifiedSetThreadSpecificData(hThread, pObj)");
      DetachCbProtocols(hThread, FRAMEWORKUNIFIED_ANY_SOURCE, HBThreadCommonHandlers,
              NULL);
      DetachParentCbThreadProtocols(hThread, HBThreadCommonHandlers);
      DetachParentCbThreadProtocols(hThread, HBThreadProtocolHandlers);
      DeleteHBTimer(pObj);
      DeletePObj(pObj);
      // LCOV_EXCL_STOP
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Successful");
    }
  }
  FRAMEWORKUNIFIEDLOG0(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

/*****************************************************************************/
/**
 @ingroup: SS_SystemManager
 @brief: HBThreadStop: function would be called at the time of stopping of the thread
 @note: .
 @param HANDLE  - Handle to message queue of HeartBeat Service.
 @return EFrameworkunifiedStatus
 */
/*****************************************************************************/
EFrameworkunifiedStatus HBThreadStop(HANDLE hThread) {  // LCOV_EXCL_START 14: Resident process, not called by NSFW
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus l_eStatus;

  FRAMEWORKUNIFIEDLOG0(ZONE_FUNC, __FUNCTION__, "+");

  CHeartBeatThread * pObj = static_cast<CHeartBeatThread *>(FrameworkunifiedGetThreadSpecificData(hThread));
  if (NULL == pObj) {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
    LOG_ERROR("FrameworkunifiedGetThreadSpecificData(hThread)");
  } else {
    // delete timers
    CALL_AND_LOG_STATUS_IF_ERRORED(pObj->HeartBeatTimersDelete());
    CALL_AND_LOG_STATUS_IF_ERRORED(pObj->HBPublishAvailabilityStatus(hThread, FALSE));
    DeletePObj(pObj);
  }

  FrameworkunifiedSetThreadSpecificData(hThread, NULL);
  CALL_AND_LOG_STATUS_IF_ERRORED(FrameworkunifiedDetachServiceFromDispatcher(hThread, FrameworkunifiedGetAppName(hThread)));

  FRAMEWORKUNIFIEDLOG0(ZONE_FUNC, __FUNCTION__, "-");

  return l_eStatus;
}// LCOV_EXCL_STOP
