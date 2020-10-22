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
#include <system_service/ss_heartbeat_notifications.h>
#include <system_service/ss_heartbeat_service_protocol.h>
#include <system_service/ss_power_service_notifications.h>
#include <system_service/ss_services.h>
#include <system_service/ss_templates.h>

#include "ss_hb_thread.h"
#include "ss_sm_systemmanagerlog.h"
#include "ss_system_manager.h"

/// Total Timers used by Heart Beat Thread
/// This value is used for creating timers
#define HBMaxTimers       1
#define HBTimerInterval   1

template<typename C, eFrameworkunifiedStatus (C::*M)(HANDLE)> EFrameworkunifiedStatus HBThreadCallback(HANDLE hThread) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  C * pObj = static_cast<C *>(FrameworkunifiedGetThreadSpecificData(hThread));
  if (pObj) {  // LCOV_EXCL_BR_LINE 4:pObj must not be NULL
    eStatus = (pObj->*M)(hThread);
  }
  return eStatus;
}

/*****************************************************************************
 @ingroup: SS_SystemManager
 @brief: CHeartBeatThread
 @note: Constructor
*****************************************************************************/
CHeartBeatThread::CHeartBeatThread(HANDLE f_hThread) :
      m_oTimerCtrl(HBMaxTimers)
    , m_HBTimerID(0)
    , m_NextChkIndex(0)
    , m_hThread(f_hThread) {  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  std::memset(&m_HBConfigParams, 0, sizeof(m_HBConfigParams));
}

/*****************************************************************************
 @ingroup: SS_SystemManager
 @brief: ~CHeartBeatThread
 @note: Destructor
*****************************************************************************/
CHeartBeatThread::~CHeartBeatThread() {  // LCOV_EXCL_START 14: Resident process, not called by NSFW
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
}
// LCOV_EXCL_STOP

/*****************************************************************************
 @ingroup: SS_SystemManager
 @brief: HBPublishAvailabilityStatus
 @note: .
 @param HANDLE  - Handle to message queue of HeartBeat Service.
 @param BOOL    - The Availability Status to be published
 @return EFrameworkunifiedStatus
*****************************************************************************/
EFrameworkunifiedStatus CHeartBeatThread::HBPublishAvailabilityStatus(HANDLE hThread, BOOL f_bAvailabiltyStatus) {// LCOV_EXCL_START 14: Resident process, not called by NSFW
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // Send availability notification to users
  l_eStatus = FrameworkunifiedPublishServiceAvailability(hThread, f_bAvailabiltyStatus);
  char l_cBuf[100] = { 0 };
  snprintf(l_cBuf,
           sizeof(l_cBuf),
           "FrameworkunifiedPublishServiceAvailability(%s)",
           GetStr(f_bAvailabiltyStatus).c_str());
  LOG_STATUS(l_eStatus, l_cBuf);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}// LCOV_EXCL_STOP

/*****************************************************************************
 @ingroup: SS_SystemManager
 @brief: HBOnStartThread
 @note: .
 @param HANDLE  - Handle to message queue of HeartBeat Service.
 @return EFrameworkunifiedStatus
*****************************************************************************/
EFrameworkunifiedStatus CHeartBeatThread::HBOnStartThread(HANDLE hThread) {
  EFrameworkunifiedStatus l_eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  UI_32 l_DataLen = FrameworkunifiedGetMsgLength(hThread);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  if ((sizeof(UI_32) + (SS_MAX_NUM_MODULES * SS_SM_HB_MAX_PROC_NAME_SIZE) + sizeof(m_HBConfigParams)) < l_DataLen) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           " Error: message buffer sizes invalid:received %d", l_DataLen);
    return eFrameworkunifiedStatusInvldParam;
  }

  CHAR l_Data[l_DataLen];  // NOLINT
  // LCOV_EXCL_BR_START 4: NSFW error case.
  if (eFrameworkunifiedStatusOK !=
      (l_eStatus = FrameworkunifiedGetMsgDataOfSize(hThread, (PVOID) & l_Data, static_cast<UI_32>(sizeof(l_Data)), eSMRRelease))) {
  // LCOV_EXCL_BR_STOP
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
            " Error: FrameworkunifiedGetMsgDataOfSize() errored: 0x%x", l_eStatus);
    return l_eStatus;
  }

  CHAR* p_Data = &l_Data[0];
  UI_32 list_num;
  memcpy(&m_HBConfigParams, p_Data, sizeof(m_HBConfigParams));
  p_Data = p_Data + sizeof(m_HBConfigParams);
  memcpy(&list_num, p_Data, sizeof(list_num));
  p_Data = p_Data + sizeof((list_num));

  for (UI_32 i = 0; i < list_num; i++) {
    SubscriberName l_Subscriber = p_Data;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    // LCOV_EXCL_BR_START 4: NSFW error case.
    if (eFrameworkunifiedStatusOK != m_oSessionHandler.HBEntrySubscriber(l_Subscriber)) {
    // LCOV_EXCL_BR_STOP
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: HBEntrySubscriber");
    }
    p_Data = p_Data + SS_SM_HB_MAX_PROC_NAME_SIZE;
  }

  if (0 == m_HBConfigParams.ApplicationHeartBeatIntervalInitial) {
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__,
         " Warning: Heart beat disabled via configuration file interval "
         "parameter set to 0.");
  } else if (0 != m_HBTimerID) {
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__,
        " Warning: Heart Beat Timer ID '%d' already created, ignoring "
        "repeated Heart Beat Start command.", m_HBTimerID);
  } else {
    // LCOV_EXCL_BR_START 4: NSFW error case.
    if (eFrameworkunifiedStatusOK !=
        (l_eStatus = FrameworkunifiedRegisterServiceAvailabilityNotification(hThread, NTFY_HeartBeatAvailability))) {
    // LCOV_EXCL_BR_STOP
      // LCOV_EXCL_START 4: NSFW error case.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      LOG_ERROR("FrameworkunifiedRegisterServiceAvailabilityNotification(" NTFY_HeartBeatAvailability ")");
      // LCOV_EXCL_STOP
    }
    else {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
          "Service availability %s Registered SOURCE : %s",
          NTFY_HeartBeatAvailability,
          FrameworkunifiedGetAppName(hThread));

      // Publish Heart Beat Availability
      // LCOV_EXCL_BR_START 4: NSFW error case.
      if (eFrameworkunifiedStatusOK != (l_eStatus = HBPublishAvailabilityStatus(hThread, TRUE))) {
      // LCOV_EXCL_BR_STOP
        // LCOV_EXCL_START 4: NSFW error case.
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        LOG_ERROR("HBPublishAvailabilityStatus(TRUE)");
        // LCOV_EXCL_STOP
      }
    }
    // Create Heart Beat app monitor
    m_HBTimerID = m_oTimerCtrl.CreateTimer(HBThreadCallback<CHeartBeatThread, &CHeartBeatThread::HBOnTimerExpiry>);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    if (0 == m_HBTimerID) {  // LCOV_EXCL_BR_LINE 8: m_HBTimerID can't be 0
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: Heart Beat Timer ID creation failed.");
      // return eFrameworkunifiedStatusFail;
    } else {
      // Start Heart Beat app monitor
      m_oTimerCtrl.StartTimer(m_HBTimerID,
                              m_HBConfigParams.ApplicationHeartBeatIntervalInitial,
                              0,
                              HBTimerInterval,
                              0);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Initial Heart Beat Timer created for %d seconds interval",
                                      m_HBConfigParams.ApplicationHeartBeatIntervalInitial);
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Repetitive Heart Beat Timer created for %d seconds interval",
              m_HBConfigParams.ApplicationHeartBeatIntervalRepeat);
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

/*****************************************************************************
 @ingroup: SS_SystemManager
 @brief: HBOnStopThread
 @note: .
 @param HANDLE  - Handle to message queue of HeartBeat Service.
 @return EFrameworkunifiedStatus
*****************************************************************************/
EFrameworkunifiedStatus CHeartBeatThread::HBOnStopThread(HANDLE hThread) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  if (0 != m_HBTimerID) {
    // Stop HeartBeat timer
    m_oTimerCtrl.StopTimer(m_HBTimerID);
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Timer '%d' stopped", m_HBTimerID);
  }
  else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: 'm_HBTimerID' is '0'; unable to stop timer");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

/*****************************************************************************
 @ingroup: SS_SystemManager
 @brief: HeartBeatTimerInit
 @note:
 @param HANDLE  - Handle to message queue of HeartBeat Service.
 @return EFrameworkunifiedStatus
*****************************************************************************/
EFrameworkunifiedStatus CHeartBeatThread::HeartBeatTimerInit(HANDLE hThread) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  m_oTimerCtrl.Initialize(hThread);

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

/*****************************************************************************
 @ingroup: SS_SystemManager
 @brief: HeartBeatTimersDelete
 @note:
 @param void
 @return EFrameworkunifiedStatus
*****************************************************************************/
EFrameworkunifiedStatus CHeartBeatThread::HeartBeatTimersDelete() {// LCOV_EXCL_START 14: Resident process, not called by NSFW
  EFrameworkunifiedStatus l_eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (0 == m_HBTimerID) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: 'm_HBTimerID' is '0'; unable to delete timer");
    l_eStatus = eFrameworkunifiedStatusInvldParam;
  } else {
    // Delete Heart Beat timer
    UI_32 l_TimerID = m_oTimerCtrl.DeleteTimer(m_HBTimerID);
    if (0 == l_TimerID) {
      l_eStatus = eFrameworkunifiedStatusDbRecNotFound;
      LOG_ERROR("m_oTimerCtrl.DeleteTimer(m_HBTimerID)");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    } else {
      l_eStatus = eFrameworkunifiedStatusOK;
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Successful");
    }
    m_HBTimerID = 0;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}// LCOV_EXCL_STOP

/*****************************************************************************
 @ingroup: SS_SystemManager
 @brief: HBCheckHeartBeatResponses
 @note:  This function checks heartbeat responses and generates a summary
         report for System Manager if one or more modules have failed to
         reply within the configured maximum number of heartbeat retries.
 @param  HANDLE  - Handle to message queue of HeartBeat Service.
 @return EFrameworkunifiedStatus
*****************************************************************************/
EFrameworkunifiedStatus CHeartBeatThread::HBCheckHeartBeatResponses(HANDLE hThread) {
  EFrameworkunifiedStatus l_eStatus;
  THbReportData f_tReportData;
  FRAMEWORKUNIFIEDLOG(ZONE_PERIODIC_FUNC, __FUNCTION__, "+");

  l_eStatus = m_oSessionHandler.HBCheckResponses(f_tReportData,
                                                 m_HBConfigParams.MaxHeartBeatRetryCount,
                                                 m_HBConfigParams.ApplicationHeartBeatIntervalRepeat,
                                                 m_NextChkIndex);
  if (l_eStatus == eFrameworkunifiedStatusFail) {  // Send a heartbeat report ONLY if a heartbeat failure is detected.
    // send report to system manager
    l_eStatus = FrameworkunifiedSendParent(hThread,
                              SS_HEARTBEAT_ERROR_DETECTED,
                              sizeof(f_tReportData),
                              &f_tReportData);
    LOG_STATUS_IF_ERRORED(l_eStatus, "FrameworkunifiedSendParent(SS_HEARTBEAT_ERROR_DETECTED)");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  }
  FRAMEWORKUNIFIEDLOG0(ZONE_PERIODIC_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

/*****************************************************************************
 @ingroup: SS_SystemManager
 @brief: HBSendRequest
 @note: .
 @param  HANDLE  - Handle to message queue of HeartBeat Service.
 @return EFrameworkunifiedStatus
*****************************************************************************/
EFrameworkunifiedStatus CHeartBeatThread::HBSendRequest(HANDLE hThread) {
  EFrameworkunifiedStatus l_eStatus;

  CALL_AND_LOG_STATUS_IF_ERRORED(  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    m_oSessionHandler.HBSendRequest(hThread, m_HBConfigParams.ApplicationHeartBeatIntervalRepeat, m_NextChkIndex));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  m_NextChkIndex++;
  if (m_HBConfigParams.ApplicationHeartBeatIntervalRepeat <= m_NextChkIndex) {
    m_NextChkIndex = 0;
  }

  return l_eStatus;
}

/*****************************************************************************
 @ingroup: SS_SystemManager
 @brief: HBOnPrintConnections
 @note: .Prints all active sessions connected to HeartBeat
 @param HANDLE hThread
 @return EFrameworkunifiedStatus
*****************************************************************************/
EFrameworkunifiedStatus CHeartBeatThread::HBOnPrintConnections(HANDLE hThread) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  m_oSessionHandler.HBPrintConnection();

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

/*****************************************************************************
 @ingroup: SS_SystemManager
 @brief: HBOnPrintStack
 @note: . Prints all sessions connected to HeartBeat
 @param HANDLE  - Handle to message queue of HeartBeat Service.
 @return EFrameworkunifiedStatus
*****************************************************************************/
EFrameworkunifiedStatus CHeartBeatThread::HBOnPrintStack(HANDLE hThread) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  m_oSessionHandler.HBPrintStack(m_HBConfigParams.MaxHeartBeatRetryCount);

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
