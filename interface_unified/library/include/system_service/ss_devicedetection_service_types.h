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
 * @file ss_devicedetection_service_types.h
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

#ifndef SS_DEVDETECT_TYPES_H_  // NOLINT (build/header_guard)
#define SS_DEVDETECT_TYPES_H_

/**
 * \~english Deive type(eUSB,eSD...)
 */
typedef enum _DeviceType {
  /**
   * \~english USB
   */
  eUSB,
  /**
   * \~english SD
   */
  eSD,
  /**
   * \~english EMMC
   */
  eMMC,
  /**
   * \~english DISC
   */
  eDISC,
  /**
   * \~english MTP
   */
  eMTP
} EDEVICE_TYPE;

/**
 * \~english Action result
 */
typedef enum _SsDevError {
  /**
   * \~english success
   */
  SS_DEV_OK,
  /**
   * \~english error
   */
  SS_DEV_ERR,
  /**
   * \~english mount error
   */
  SS_DEV_ERR_MOUNT,
  /**
   * \~english device busy
   */
  SS_DEV_ERR_BUSY,
} SS_DEV_ERROR;

/**
 * \~english USB port
 */
typedef enum _USBPort {
  /**
   * \~english USB0(IF-BOX)
   */
  SS_DEV_USB_PORT0,     // IF-BOX
  /**
   * \~english USB1(DCM)
   */
  SS_DEV_USB_PORT1,     // DCM
  /**
   * \~english USB2(DSRC/ETC)
   */
  SS_DEV_USB_PORT2,     // DSRC/ETC
  /**
   * \~english USB3(External deck)
   */
  SS_DEV_USB_PORT3,     // External DECK
  /**
   * \~english USB4(Drive recorder)
   */
  SS_DEV_USB_PORT4,     // Supplies drive recorder 
  /**
   * \~english USB number max.
   */
  SS_DEV_USB_MAX        // MAX
} SS_DEV_USBPORT;

/**
 * \~english notify code
 */
typedef enum SsNotifyCode {
  /**
   * \~english Unsupport USB device
   */
  SS_DEV_UNSUPPORT_USB,    // Un support USB device
  /**
   * \~english The connected device has no response.
   */
  SS_DEV_NON_RESPONSE_USB,  // Enemuration Error
  /**
   * \~english It detects a USB device of current excess that has been connected to the bus power HUB.
   */
  SS_DEV_HIGH_POWER_USB,    // It detects a USB device of current excess that
          // has been connected to the bus power HUB.
  /**
   * \~english Over support USB(3 or more units)
   */
  SS_DEV_OVER_SUPPORT_USB,  // Over support USB(3 or more units)
  /**
   * \~english  Over number of USB HUB(2 or more units)
   */
  SS_DEV_OVER_NUM_USB_HUB,  // Over number of USB HUB(2 or more units)
} SS_NOTIFY_CODE;

/**
 * \~english RoleSw state
 */
typedef enum _SsDevRoleSwState {
  /**
   * \~english start
   */
  SS_DEV_ROLE_SW_START,
  /**
   * \~english begin end
   */
  SS_DEV_ROLE_SW_BEGIN_END,
  /**
   * \~english end
   */
  SS_DEV_ROLE_SW_END,
  /**
   * \~english restart
   */
  SS_DEV_ROLE_SW_RESTART,
} SS_DEV_ROLE_SW_STATE;

/**
 * \~english Disc Type(eCD, eDVD, eBD)
 */
typedef enum _DiscType {
  /**
   * \~english CD
   */
  eCD,
  /**
   * \~english DVD
   */
  eDVD,
  /**
   * \~english BD
   */
  eBD,
} EDISC_TYPE;

#endif /* SS_DEVDETECT_TYPES_H_ */  // NOLINT (build/header_guard)

/** @}*/  // end of device_detection_service
/** @}*/  // end of system_service
/** @}*/  // end of BaseSystem
