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

/**
 * @file commusb_notifications.h
 * @~english
 * @brief   commusb_notifications I/F
 */
#ifndef COMMUNICATION_USB_CLIENT_INCLUDE_PERIPHERAL_SERVICE_COMMUSB_NOTIFICATIONS_H_
#define COMMUNICATION_USB_CLIENT_INCLUDE_PERIPHERAL_SERVICE_COMMUSB_NOTIFICATIONS_H_

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup peripheral_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup communication_usb
 *  @ingroup peripheral_service
 *  @{
 */

/**
 * \~english PS_CommUSB service availability notify.
 * \~ @code
 * l_eStatus = FrameworkunifiedSubscribeNotificationWithCallback(hApp, NTFY_CommUSB_Availability, OnRcvAvailable_USBEx);
 * @endcode
 */
#define NTFY_CommUSB_Availability "PS_CommUSB/Availability"

/**
 * \~english USB Device Enumeration complete notify.
 * \~ @code
 * l_eStatus = FrameworkunifiedSubscribeNotificationWithCallback(hApp, NTFY_CommUSB_DeviceValidity, OnRcvDeviceValidity);
 * @endcode
 */
#define NTFY_CommUSB_DeviceValidity "PS_CommUSB/DeviceValidity"

/** @}*/  // end of communication_usb
/** @}*/  // end of peripheral_service
/** @}*/  // end of BaseSystem
#endif  // COMMUNICATION_USB_CLIENT_INCLUDE_PERIPHERAL_SERVICE_COMMUSB_NOTIFICATIONS_H_
