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
 * @file ss_devicedetection_service_notifications.h
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

#ifndef SS_DEVICE_DETECTION_SERVICE_NOTIFICATIONS_H_  // NOLINT (build/header_guard)
#define SS_DEVICE_DETECTION_SERVICE_NOTIFICATIONS_H_

#include "system_service/ss_services.h"

/**
 * \~english SS_DeviceDetection service availability notification.
 */
#define NTFY_SS_Device_Detection_Service_Availability    SS_DEV_DETECT_SRV"/Availability"

/**
 * \~english SS_DeviceDetection service USB error notification.
 */
#define NTFY_SS_Device_Detection_USB_Error                  SS_DEV_DETECT_SRV"/USBError"

/**
 * \~english SS_DeviceDetection service MMCSD error notification.
 */
#define NTFY_SS_Device_Detection_MMCSD_Error                SS_DEV_DETECT_SRV"/MMCSDError"

#endif /* SS_DEVICE_DETECTION_SERVICE_NOTIFICATIONS_H_ */  // NOLINT (build/header_guard)

/** @}*/  // end of device_detection_service
/** @}*/  // end of system_service
/** @}*/  // end of BaseSystem
