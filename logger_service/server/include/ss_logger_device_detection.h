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
#ifndef LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_DEVICE_DETECTION_H_
#define LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_DEVICE_DETECTION_H_

#include <system_service/ss_devicedetection_service_notifications.h>
#include <system_service/ss_devicedetection_service_ifc.h>
#include "ss_logger_cfg.h"
#include "ss_logger_util.h"

extern const char * USB_INSERTED_RULE;

// USB device is ejected
extern const CHAR USB_EJECTED_RULE[];

// The trigger event file (/log/retrievelog.evt) file is found on USB
extern const CHAR USB_CAPTURE_LOG_EVENT_RULE[];

// When the command to update system with script is executed all the console
// output will be redirected to this file on /tmp folder. later it will be
// moved to USB stick
extern const CHAR CAPTURE_LOG_FILENAME[];

//
extern const SI_32 INVALID_FD;
class CLoggerDeviceDetection {
 public:
  CLoggerDeviceDetection();
  ~CLoggerDeviceDetection();

  EFrameworkunifiedStatus Initialize(HANDLE hApp, CLoggerCfg *f_pLoggerCfg);
  EFrameworkunifiedStatus DD_ServiceAvailabilityCallBack(HANDLE hApp);
  EFrameworkunifiedStatus DD_OpenSessionAckCallBack(HANDLE hApp);
  EFrameworkunifiedStatus DD_CloseSessionAckCallBack(HANDLE hApp);
  EFrameworkunifiedStatus DD_CloseSessionWithDevDetectionSrv(HANDLE hApp);
  EFrameworkunifiedStatus DD_MediaDetectCallBack(HANDLE hApp);

  EFrameworkunifiedStatus OnStop(void);
 private:
  CLoggerCfg *m_pLoggerCfg;

  /// Device Detection Class Instance
  DeviceDetectionServiceIf m_devDetect;

  CLoggerUtil *m_loggerUtil;
};
#endif  // LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_DEVICE_DETECTION_H_
