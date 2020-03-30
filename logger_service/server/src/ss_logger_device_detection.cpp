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
/// \ingroup  tag_NS_InterfaceunifiedLogCapture
/// \brief    This file supports USB logging threads.
///
///////////////////////////////////////////////////////////////////////////////

// System Headers
#include <errno.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#ifdef AGL_STUB
#else
#include <sys/usbdi.h>
#endif
#include <limits.h>
#include <string.h>
#include <native_service/ns_message_center_if.h>
#include <ss_logger_fs_directory.h>
#include <boost/bind.hpp>

// Pasa Logs
#include <loggerservicedebug_loggerservicelog.h>
#include <ss_logger_device_detection.h>
#include <loggerservicedebug_thread_if.h>
#include <native_service/frameworkunified_framework_if.h>
#include <system_service/ss_logger_service.h>
#include <system_service/ss_devicedetection_service_notifications.h>
#include <system_service/ss_devicedetection_service_ifc.h>
#include <system_service/ss_sm_client_if.h>
#include <string>
#include "ss_logger_common.h"

//
const SI_32 INVALID_FD = -1;

// Max command line length
const UI_32 MAX_SYS_CMD_LN_LENGTH = 120;

CLoggerDeviceDetection::CLoggerDeviceDetection() :
    m_pLoggerCfg(NULL),
    m_loggerUtil(NULL) {
}

CLoggerDeviceDetection::~CLoggerDeviceDetection() {  // LCOV_EXCL_START 14:global instance
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus CLoggerDeviceDetection::Initialize(HANDLE f_hApp, CLoggerCfg * f_pLoggerCfg) {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
    EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
    m_pLoggerCfg = f_pLoggerCfg;
    // Device Detection Obj initialization
    if (m_devDetect.Initialize(f_hApp)) {  // LCOV_EXCL_BR_LINE 200:To ensure success
            if (eFrameworkunifiedStatusOK != (l_eStatus = m_devDetect.NotifyOnDeviceDetectionAvailability(
                    boost::bind(&CLoggerDeviceDetection::DD_ServiceAvailabilityCallBack, this, _1) ))) {
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "ServiceAvailabilityCallback registration failed");
            }

            if (eFrameworkunifiedStatusOK != (l_eStatus = m_devDetect.NotifyOnOpenSessionAck(
                    boost::bind(&CLoggerDeviceDetection::DD_OpenSessionAckCallBack, this, _1) ))) {
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "OpenSessionAckCallback registration failed");
            }

            if (eFrameworkunifiedStatusOK != (l_eStatus = m_devDetect.NotifyOnCloseSessionAck(
                    boost::bind(&CLoggerDeviceDetection::DD_CloseSessionAckCallBack, this, _1) ))) {
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "CloseSessionCallback registration failed");
            }
    } else {
            // LCOV_EXCL_START 200:To ensure success
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Device Detection Object Initialization failed!");
            // LCOV_EXCL_STOP
    }

    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
    return l_eStatus;
}

//////////////////////////////////////////
//  Function : ServiceAvailabilityCallBack
//////////////////////////////////////////
EFrameworkunifiedStatus CLoggerDeviceDetection::DD_ServiceAvailabilityCallBack(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (FrameworkunifiedIsServiceAvailable(hApp)) {
    if (eFrameworkunifiedStatusOK != (eStatus = m_devDetect.OpenSessionRequest())) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Open session request failed");
    }
  } else {
    if (eFrameworkunifiedStatusOK != (eStatus = m_devDetect.CloseSessionRequest())) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Close session request failed");
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

//////////////////////////////////////////
//  Function : OpenSessionAckCallBack
//////////////////////////////////////////
EFrameworkunifiedStatus CLoggerDeviceDetection::DD_OpenSessionAckCallBack(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (eFrameworkunifiedStatusOK == (eStatus = m_devDetect.DecodeOpenSessionResponse())) {
    if (eFrameworkunifiedStatusOK != (eStatus = m_devDetect.RegisterForDeviceDetectionEvent(SS_DEV_DETECT_ANY_USB_EV,
                         boost::bind(&CLoggerDeviceDetection::DD_MediaDetectCallBack, this, _1)))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Registration for SS_DEV_DETECT_ANY_USB_EV failed");
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Decode open session response failed");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}
//////////////////////////////////////////
//  Function : OpenSessionAckCallBack
//////////////////////////////////////////
EFrameworkunifiedStatus CLoggerDeviceDetection::DD_CloseSessionAckCallBack(HANDLE hApp) {  // LCOV_EXCL_START 200: can not called from devicedetection_service   // NOLINT[whitespace/line_length]
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  CloseSessionAck tCloseSessionAck;

  if (hApp) {
    if (eFrameworkunifiedStatusOK == (eStatus = FrameworkunifiedGetMsgDataOfSize(hApp, &tCloseSessionAck, sizeof (tCloseSessionAck)))) {
      if (eFrameworkunifiedStatusOK  == tCloseSessionAck.eStatus) {
        UI_32 l_uiSessionId = tCloseSessionAck.sessionId;
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "session  %d closed successfully", l_uiSessionId);
      }
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus CLoggerDeviceDetection::DD_CloseSessionWithDevDetectionSrv(HANDLE hApp) {  // LCOV_EXCL_START 200: can not called from devicedetection_service   // NOLINT[whitespace/line_length]
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (eFrameworkunifiedStatusOK == (eStatus = m_devDetect.
      NotifyOnCloseSessionAck(boost::bind(&CLoggerDeviceDetection::DD_CloseSessionAckCallBack, this, _1)))) {
    if (eFrameworkunifiedStatusOK != (eStatus = m_devDetect.CloseSessionRequest())) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to send CloseSessionRequest");
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to register closeSessionAckCallback");
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus CLoggerDeviceDetection::DD_MediaDetectCallBack(HANDLE hApp) {  // LCOV_EXCL_START 200: can not called from devicedetection_service   // NOLINT[whitespace/line_length]
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus         l_eStatus = eFrameworkunifiedStatusOK;
  std::string        l_devstr;
    std::string        l_filePathStr;
    SS_MediaDetectInfo l_tMediaDetectInfo;


  if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedGetMsgDataOfSize(hApp, &l_tMediaDetectInfo, sizeof (l_tMediaDetectInfo)))) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __PRETTY_FUNCTION__, "FrameworkunifiedGetMsgDataOfSize Failed Status:0x%x ", l_eStatus);
  } else {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Device = %d",       l_tMediaDetectInfo.dev_type);
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Availability = %d", l_tMediaDetectInfo.bIsDeviceAvailable);
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " device path = %s",  l_tMediaDetectInfo.deviceMountpath);
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " File path = %s",    l_tMediaDetectInfo.filepathName);

        if (l_tMediaDetectInfo.dev_type == eUSB) {
      if (l_tMediaDetectInfo.bIsDeviceAvailable == TRUE && m_pLoggerCfg != NULL) {
        std::string l_usbMountPath;
        l_usbMountPath = l_tMediaDetectInfo.deviceMountpath;
        m_pLoggerCfg->setUsb0MountPath(l_usbMountPath);
        m_loggerUtil->SetUsbMountPath(l_usbMountPath);
        }
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}
// LCOV_EXCL_STOP
