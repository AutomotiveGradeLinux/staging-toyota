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

/**
 * @file ss_devicedetection_service_protocol.h
 * @brief This file supports the Device Detection service.
 */

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup system_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup device_detection_service
 *  @ingroup system_service
 *  @{
 */

#ifndef _SS_DEVICEDETECTION_SERVICE_PROTOCOL_H_  // NOLINT (build/header_guard)
#define _SS_DEVICEDETECTION_SERVICE_PROTOCOL_H_

#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_service_protocol.h>
#include "system_service/ss_devicedetection_service.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \~english Event type (USB/SD/...etc  event)
 */
typedef enum _DeviceDetectionServerEvents {
  /**
   * \~english Notify whenever a USB MassStorage device is connected/removed
   */
  SS_DEV_DETECT_ANY_USB_EV = 121,
  /**
   * \~english Notify whenever a SD device is connected/removed
   */
  SS_DEV_DETECT_ANY_SD_EV = 122,
  /**
   * \~english Notify all events for all devices
   */
  SS_DEV_DETECT_FILEPATHNAME_EV = 123,
  /**
   * \~english Notify whenever a Navi Emmc device is connected/removed
   */
  SS_DEV_DETECT_ANY_EMMC_NAV_EV = 124,
  /**
   * \~english Notify whenever a USB device is connected/removed
   */
  SS_DEV_INFO_ANY_USB_EV = 125,
  /**
   * \~english Notify whenever a DISC(block) device is connected/removed
   */
  SS_DEV_DETECT_ANY_DISC_EV = 126,
  /**
   * \~english Notify whenever a USB NCM device is connected/removed
   */
  SS_DEV_DETECT_ANY_USB_NCM_EV = 127,
  /**
   * \~english Notify whenever a USB _CWORD57_ device is connected/removed
   */
  SS_DEV_DETECT_ANY_USB__CWORD57__EV = 128,
  /**
   * \~english Notify whenever a USB ACM device is connected/removed (for DCM)
   */
  SS_DEV_DETECT_ANY_USB_ACM_EV = 129,
  /**
   * \~english Notify whenever a USB Device Management device is connected/removed (for DCM)
   */
  SS_DEV_DETECT_ANY_USB_DEV_MNG_EV = 130,
  /**
   * \~english Notify whenever a USB Vics Control device is connected/removed (for DCM)
   */
  SS_DEV_DETECT_ANY_USB_VEHICLE_CTRL_EV = 131,
  /**
   * \~english Notify whenever a USB DSRC Application device is connected/removed (for DSRC)
   */
  SS_DEV_DETECT_ANY_USB_DSRC_APP_EV = 132,
  /**
   * \~english Notify whenever a USB DSRC Control device is connected/removed (for DSRC)
   */
  SS_DEV_DETECT_ANY_USB_DSRC_CTRL_EV = 133,
  /**
   * \~english Notify whenever a USB IR Vics Data device is connected/removed (for DSRC)
   */
  SS_DEV_DETECT_ANY_USB_IR_VICS_DATA_EV = 134,
  /**
   * \~english Notify whenever USB Over Current is Detected
   */
  SS_DEV_ERR_USB_OVER_CURRENT_EV = 135,
  /**
   * \~english Notify whenever USB MTP Device is Detected
   */
  SS_DEV_DETECT_ANY_USB_MTP_EV = 136,
  /**
   * \~english Notify whenever USB Authentication Error Notify
   */
  SS_DEV_INFO_ANY_USB_NOTIFY_EV = 137,
  /**
   * \~english Notify whenever a USB _CWORD57_ device is connected/removed after RoleSW.
   */
  SS_DEV_DETECT_ANY_USB__CWORD57__ROLE_EV = 138,
  /**
   * \~english Notify whenever SD is formated.
   */
  SS_DEV_INFO_SD_FORMAT_COMP_EV = 139,
  /**
   * \~english Notify whenever a USB NCM device is connected/removed (for DCM)
   */
  SS_DEV_DETECT_ANY_USB_DCM_NCM_EV = 140,
} SS_DeviceDetectionServerEvents;

#ifdef __cplusplus
}
#endif

#endif /* _SS_DEVICEDETECTION_SERVICE_PROTOCOL_H_ */  // NOLINT (build/header_guard)

/** @}*/  // end of device_detection_service
/** @}*/  // end of system_service
/** @}*/  // end of BaseSystem
