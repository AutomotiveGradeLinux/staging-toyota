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
/// \ingroup  tag_SystemServicesIf
/// \brief    This file supports the Device Detection service.
///
///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 */
#ifndef _SS_DEVICEDETECTION_SERVICE_PROTOCOL_LOCAL_H_  // NOLINT (build/header_guard)
#define _SS_DEVICEDETECTION_SERVICE_PROTOCOL_LOCAL_H_


#ifdef __cplusplus
extern "C" {
#endif

typedef enum _DeviceDetectionServerProtocol {
  SS_DEV_DETECT_GET_APP_INFO       = 0x10,  /// client to service: not really needed
  SS_DEV_DETECT_RETURN_APP_INFO    = 0x11,  /// service to client: not really needed
  SS_UDEV_DEV_DETECT_INFO       = 0x12,  /// UDEV to service: device detect info sent to service
  SS_UDEV_DEV_DETECT_FILEPATHNAME_INFO  = 0x13,  /// File pathname info cmd
  SS_REGISTER_FILEPATHNAME     = 0x14,  /// Register file pathname cmd
  SS_BSP_DEV_DETECT_ERR_INFO       = 0x15,  /// BSP / MCD device insert/removal/mount error report.
  SS_REGISTER_DETECT_EVENT     = 0x16,  /// Register DeviceDetection Event
  SS_UDEV_DEV_DEVICE_INFO     = 0x17,  /// UDEV to service: device info sent to service
  SS_UDEV_DEV_DETECT_NCM_INFO     = 0x18,  /// UDEV to service: detect ncm sent to service
  SS_UDEV_DEV_DETECT__CWORD57__INFO   = 0x19,  /// UDEV to service: detect _CWORD57_ sent to service
  SS_UDEV_DEV_DETECT_ACM_INFO      = 0x1a,  /// UDEV to service: detect ACM sent to service
  SS_UDEV_DEV_DETECT_MNG_INFO      = 0x1b,  /// UDEV to service: detect DeviceManagementModel sent to service
  SS_UDEV_DEV_DETECT_VEHICLE_CTRL_INFO  = 0x1c,  /// UDEV to service: detect Vehicle Control sent to service
  SS_UDEV_DEV_DETECT_DSRCCTL_INFO  = 0x1d,  /// UDEV to service: detect DSRC Application sent to service
  SS_UDEV_DEV_DETECT_DSRCAPP_INFO  = 0x1e,  /// UDEV to service: detect DSRC Control sent to service
  SS_UDEV_DEV_DETECT_IRVICS_DATA_INFO = 0x1f,  /// UDEV to service: detect IR-VICS Data sent to service
  SS_DETECT_USB_OVC_ERR     = 0x20,  /// USB Monitor OVC to service: detect OverCurrent Error
  SS_UDEV_DEV_DETECT_MTP_INFO  = 0x21,  /// UDEV to service: detect MTP Device
  SS_DEV_MOUNT_SD  = 0x22,      /// client to service: mount SD partion
  SS_DEV_UMOUNT_SD  = 0x23,    /// client to service: umount SD partion
  SS_DEV_REMOUNT_PARTITION  = 0x24,  /// client to service: Remount partion
  SS_UNREGISTER_DETECT_EVENT      = 0x25,  /// UnRegister DeviceDetection event
  SS_USB_VBUS_RESET    = 0x26,  /// client to service: USB Vbus Reset
  SS_UDEV_USB_AUTHENTICATION_ERR_INFO  = 0x27,  /// USB Monitor to service: USB Authentication Error Notify
  SS_UDEV_DEV_DETECT_USB_FUNC_INFO  = 0x28,  /// USB Monitor to service: detect USB Func to service
  SS_DEV_ROLE_SW_STATE_NOTIFY = 0x29,  /// client to service: notify RoleSw state
  SS_DEV_FORMAT_SD = 0x2a,  /// client to service: Format SD
  SS_INFO_FORMAT_SD =  0x2b,  /// SDDRV to service: Infomation Format SD result
}SS_DeviceDetectionServerProtocol;

#ifdef __cplusplus
}
#endif

#endif /* _SS_DEVICEDETECTION_SERVICE_PROTOCOL_LOCAL_H_ */  // NOLINT (build/header_guard)

