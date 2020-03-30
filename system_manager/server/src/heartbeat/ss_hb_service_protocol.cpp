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
/// \ingroup  tag_SystemManager
/// \brief    This file provides support for the application heartbeat system.
///
///////////////////////////////////////////////////////////////////////////////
#include <system_service/ss_heartbeat_service_protocol.h>
#include <system_service/ss_system_manager_if.h>
#include <system_service/ss_system_manager_if_local.h>
#include <stdlib.h>
#include <string.h>
#include <native_service/frameworkunified_framework_if.h>
#include <system_service/ss_services.h>
#include <system_service/ss_templates.h>

#include "ss_hb_thread.h"
#include "ss_sm_systemmanagerlog.h"

/*****************************************************************************
 @ingroup: SS_SystemManager
 @brief: HBOnTimerExpiry
 @note: .
 @param HANDLE  - Handle to message queue of HeartBeat Service.
 @return EFrameworkunifiedStatus
 *****************************************************************************/
EFrameworkunifiedStatus CHeartBeatThread::HBOnTimerExpiry(HANDLE f_hThread) {
  FRAMEWORKUNIFIEDLOG(ZONE_PERIODIC_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;

  // check heart beat replies received from clients.
  l_eStatus = HBCheckHeartBeatResponses(f_hThread);
  LOG_STATUS_IF_ERRORED(l_eStatus, "HBCheckHeartBeatResponses()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  // send heart beat requests to clients
  l_eStatus = HBSendRequest(f_hThread);
  LOG_STATUS_IF_ERRORED(l_eStatus, "HBSendRequest()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  FRAMEWORKUNIFIEDLOG(ZONE_PERIODIC_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

/*****************************************************************************
 @ingroup: SS_SystemManager
 @brief:   HBOnClientResponse
 @note: .
 @param HANDLE - Handle to message queue of HeartBeat Service.
 @return EFrameworkunifiedStatus OK or Fail
 *****************************************************************************/
EFrameworkunifiedStatus CHeartBeatThread::HBOnClientResponse(HANDLE f_hThread) {
  EFrameworkunifiedStatus l_eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_PERIODIC_FUNC, __FUNCTION__, "+");
  l_eStatus = m_oSessionHandler.HBProcessClientResponse(f_hThread);
  LOG_STATUS_IF_ERRORED(l_eStatus, "m_oSessionHandler.HBProcessClientResponse(f_hThread)");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  FRAMEWORKUNIFIEDLOG(ZONE_PERIODIC_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

/*****************************************************************************
 @ingroup: SS_SystemManager
 @brief: HBOnPeriodicStatusRequest
 @note: .
 @param  HANDLE - Handle to message queue of HeartBeat Service.
 @return EFrameworkunifiedStatus OK or Fail
 *****************************************************************************/
EFrameworkunifiedStatus CHeartBeatThread::HBOnPeriodicStatusRequest(HANDLE f_hThread) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_PERIODIC_FUNC, __FUNCTION__, "+");
  // LCOV_EXCL_BR_START 4: NSFW error case.
  if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedSendParent(f_hThread, SS_HEARTBEAT_PERIODIC_RESP, 0, NULL))) {
  // LCOV_EXCL_BR_STOP
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("FrameworkunifiedSendParent(SS_HEARTBEAT_PERIODIC_RESP)");  // LCOV_EXCL_LINE 4: NSFW error case.
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_PERIODIC_INFO, __FUNCTION__, "Successful");
  }

  FRAMEWORKUNIFIEDLOG(ZONE_PERIODIC_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

/*****************************************************************************
 @ingroup: SS_SystemManager
 @brief: HBOnPeriodicStatusRequest
 @note: .
 @param  HANDLE - Handle to message queue of HeartBeat Service.
 @return EFrameworkunifiedStatus OK or Fail
 *****************************************************************************/
EFrameworkunifiedStatus CHeartBeatThread::HBOnAvailCheckRequest(HANDLE f_hThread) {
  FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "+");

  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  THbAvailCheck check;
  l_eStatus = m_oSessionHandler.HBAvailableCheck(check);
  SS_ASERT(l_eStatus == eFrameworkunifiedStatusOK);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  // LCOV_EXCL_BR_START 4: NSFW error case.
  if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedSendParent(f_hThread, SS_HEARTBEAT_AVAIL_CHECK_RESP, sizeof(check), &check))) {
  // LCOV_EXCL_BR_STOP
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("FrameworkunifiedSendParent(SS_HEARTBEAT_AVAIL_CHECK_RESP)");  // LCOV_EXCL_LINE 4: NSFW error case.
  }

  return l_eStatus;
}

/*****************************************************************************
 @ingroup: SS_SystemManager
 @brief: HBOnRemoveModule
 @note: .
 @param  HANDLE - Handle to message queue of HeartBeat Service.
 @return EFrameworkunifiedStatus OK or Fail
 *****************************************************************************/
EFrameworkunifiedStatus CHeartBeatThread::HBOnRemoveModule(HANDLE f_hThread) {
  EFrameworkunifiedStatus l_eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  TSMRequestMessage tReqMsgData;
  // ReadMsg():                                                        *
  //     Check hApp ptr, msg size, msg reception, read msg if all ok.  *
  //     Report any errors found.                                      *
  //                                                                   *
  // LCOV_EXCL_BR_START 4: NSFW error case.
  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg <TSMRequestMessage> (f_hThread, tReqMsgData))) {
  // LCOV_EXCL_BR_STOP
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("ReadMsg()");  // LCOV_EXCL_LINE 4: NSFW error case.
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
        " %s is required to be Deleted from HeartBeat Monitor List", tReqMsgData.pstModuleName);

    // delete module entry from map
    l_eStatus = m_oSessionHandler.HBDeleteRegisteredClientEntry(f_hThread, tReqMsgData.pstModuleName);
    LOG_STATUS_IF_ERRORED(l_eStatus, "m_oSessionHandler.HBDeleteRegisteredProcessEntry()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

/*****************************************************************************
 @ingroup: SS_SystemManager
 @brief: HBOnAppendModule
 @note: .
 @param  HANDLE - Handle to message queue of HeartBeat Service.
 @return EFrameworkunifiedStatus OK or Fail
 *****************************************************************************/
EFrameworkunifiedStatus CHeartBeatThread::HBOnAppendModule(HANDLE f_hThread) {
  EFrameworkunifiedStatus l_eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  TSMRequestMessage tReqMsgData;
  // ReadMsg():                                                        *
  //     Check hApp ptr, msg size, msg reception, read msg if all ok.  *
  //     Report any errors found.                                      *
  //                                                                   *
  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg < TSMRequestMessage > (f_hThread, tReqMsgData))) {
    LOG_ERROR("ReadMsg()");
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
        " %s is required to be Appended from HeartBeat Monitor List", tReqMsgData.pstModuleName);

    // append module entry from map
    SubscriberName l_Subscriber = tReqMsgData.pstModuleName;
    l_eStatus = m_oSessionHandler.HBEntrySubscriber(l_Subscriber);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    LOG_STATUS_IF_ERRORED(l_eStatus, "m_oSessionHandler.HBEntrySubscriber()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
// EOF
