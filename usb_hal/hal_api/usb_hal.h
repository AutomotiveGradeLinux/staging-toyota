/*
 * @copyright Copyright (c) 2017-2020 TOYOTA MOTOR CORPORATION.
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
 * @file usb_hal.h
 * @brief usb_hal API
 */

#ifndef HAL_API_USB_HAL_H_
#define HAL_API_USB_HAL_H_

#include "native_service/frameworkunified_types.h"

/** @addtogroup device_detection_service
 *  @{
 */
/** @addtogroup usb_hal
 *  @ingroup device_detection_service
 *  @{
 */

/**
 * \~english Definition of Host/Function mode of USB OTG port.
 */
enum UsbRoleType {
  /**
   * \~english Host mode
   */
  USB_ROLE_HOST = 0,
  /**
   * \~english Function mode
   */
  USB_ROLE_FUNCTION,
};

/**
 * \ingroup GetUsbRoleSwitch
 * \~english @par Brief
 *       Get USB role
 * \~english @param [out] usb_role
 *       UsbRoleType*   - USB role
 * \~english @retval eFrameworkunifiedStatusOK OK
 * \~english @retval eFrameworkunifiedStatusNullPointer NULL Pointer
 * \~english @retval eFrameworkunifiedStatusFail Abnormal Error
 * \~english @par Prerequisite
 *       - When State value is "status faile",  Can not  get  host/funtcion mode of USB OTG port.
 * \~english @par Change of internal state
 *       - None.
 * \~english @par Conditions of processing failure
 *       - Input parameter error
 *         - eFrameworkunifiedStatusNullPointer
 *       - Internal process error(open/read)
 *         - eFrameworkunifiedStatusFail
 *       - Internal process error(State value is "status faile")
 *         - eFrameworkunifiedStatusFail
 * \~english @par Classification
 *       Public
 * \~english @par Type
 *       Sync
 * \~english @par Detail
 *       - Get Host/Funtcion mode of USB OTG port.
 *       - The API can not be used by multiple processes, threads at the same time.
 * \~english @see SetUsbRoleSwitch
 */
EFrameworkunifiedStatus GetUsbRoleSwitch(UsbRoleType* usb_role);

/**
 * \ingroup SetUsbRoleSwitch
 * \~english @par Brief
 *       Set USB role.
 * \~english @param [in] usb_role
 *       UsbRoleType   - USB role
 * \~english @retval eFrameworkunifiedStatusOK OK
 * \~english @retval eFrameworkunifiedStatusInvldParam Invalid Parameter
 * \~english @retval eFrameworkunifiedStatusFail Abnormal Error
 * \~english @par Prerequisite
 *       - When State value is "status faile", Can not  set host/funtcion mode of USB OTG port.
 * \~english @par Change of internal state
 *       - Change "USB role is host mode"or"USB role is host mode"
 * \~english @par Conditions of processing failure
 *       - Input parameter error
 *         - eFrameworkunifiedStatusInvldParam
 *       - Internal process error(open/read/write)
 *         - eFrameworkunifiedStatusFail
 *       - Internal process error(State value is "status faile")
 *         - eFrameworkunifiedStatusFail
 * \~english @par Classification
 *       Public
 * \~english @par Type
 *       Sync
 * \~english @par Detail
 *       - Set Host/Funtcion mode of USB OTG port.
 *       - The API can not be used by multiple processes, threads at the same time.
 * \~english @see GetUsbRoleSwitch
 */
EFrameworkunifiedStatus SetUsbRoleSwitch(UsbRoleType usb_role);

/**
 * \~english USB authentication error information
 */
enum UsbAuthenticationError {
  /**
   * \~english USB authentication no error
   */
  USB_AUTHENTICATION_ERROR_NONE = 0,
  /**
   * \~english The connected device has no response
   */
  USB_AUTHENTICATION_ERROR_ENUMERATE,
  /**
   * \~english The device connected to bus-powered HUB is overcurrent
   */
  USB_AUTHENTICATION_ERROR_HUB_OVERCURRENT,
  /**
   * \~english HUB tier levels
   */
  USB_AUTHENTICATION_ERROR_HUB_TIER_LEVELS,
  /**
   * \~english The number of connected devices is more than supported
   */
  USB_AUTHENTICATION_ERROR_CONNECTION_EXCESS
};

/**
 * \ingroup CheckUsbAuthenticationError
 * \~english @par Brief
 *       Check USB authentication error
 * \~english @param [out] usb_auth_error
 *       UsbAuthenticationError*   - USB authentication error
 * \~english @retval eFrameworkunifiedStatusOK OK
 * \~english @retval eFrameworkunifiedStatusNullPointer NULL Pointer
 * \~english @retval eFrameworkunifiedStatusFail Abnormal Error
 * \~english @par Prerequisite
 *       - When State value is "status faile",  Can not  check USB authentication error .
 * \~english @par Change of internal state
 *       - None.
 * \~english @par Conditions of processing failure
 *       - Input parameter error
 *         - eFrameworkunifiedStatusNullPointer
 *       - Internal process error(open/read)
 *         - eFrameworkunifiedStatusFail
 *       - Internal process error(State value is "status faile")
 *         - eFrameworkunifiedStatusFail
 * \~english @par Classification
 *       Public
 * \~english @par Type
 *       Sync
 * \~english @par Detail
 *       - Check USB authentication error information.
 *       - The API can not be used by multiple processes, threads at the same time.
 * \~english @see None.
 */
EFrameworkunifiedStatus CheckUsbAuthenticationError(UsbAuthenticationError* usb_auth_error);

/**
 * \~english Definition of USB port number.
 *       - The USB port number is defined according to the RefHW,
 *         modification will be necessary if there's any change about USB port.
 */
enum UsbPortNumber {
  /**
   * \~english USB0(IF-BOX)
   */
  USB_PORT_NUMBER_0 = 0,
  /**
   * \~english USB1(DCM)
   */
  USB_PORT_NUMBER_1,
  /**
   * \~english USB2(DSRC/ETC)
   */
  USB_PORT_NUMBER_2,
  /**
   * \~english USB3(External deck)
   */
  USB_PORT_NUMBER_3,
  /**
   * \~english USB4(Drive recorder)
   */
  USB_PORT_NUMBER_4,
  /**
   * \~english Maximum of USB port number.
   */
  USB_PORT_NUMBER_MAX
};

/**
 * \ingroup ResetUsbVbus
 * \~english @par Brief
 *       USB VBus Reset.
 * \~english @param [in] usb_port_no
 *       UsbPortNumber   - USB port number
 * \~english @retval eFrameworkunifiedStatusOK OK
 * \~english @retval eFrameworkunifiedStatusInvldParam Invalid Parameter
 * \~english @retval eFrameworkunifiedStatusFail Abnormal Error
 * \~english @par Prerequisite
 *       - When State value is "status faile",  Can not  USB VBus reset .
 * \~english @par Change of internal state
 *       - "USB power ON" from the "USB power OFF".
 * \~english @par Conditions of processing failure
 *       - Input parameter error
 *         - eFrameworkunifiedStatusInvldParam
 *       - Internal process error(open/read/write)
 *         - eFrameworkunifiedStatusFail
 *       - Internal process error(State value is "status faile")
 *         - eFrameworkunifiedStatusFail
 *         - Not change internal state.
 * \~english @par Classification
 *       Public
 * \~english @par Type
 *       Sync
 * \~english @par Detail
 *       - USB VBus Reset.
 *       - The API is a block I/F which costs a while due to the sleep time between power off and power on.
 *       - The API can not be used by multiple processes, threads at the same time.
 * \~english @see IsUsbPortPowered, PowerOnUsbPort, PowerOffUsbPort
 */
EFrameworkunifiedStatus ResetUsbVbus(UsbPortNumber usb_port_no);

/**
 * \ingroup IsUsbPortPowered
 * \~english @par Brief
 *       Check the power state of USB port.
 * \~english @param [in] usb_port_no
 *       UsbPortNumber   - USB port number
 * \~english @param [out] is_powered
 *       bool*\n
 *               - true : Power is on
 *               - false : Power is off
 * \~english @retval eFrameworkunifiedStatusOK OK
 * \~english @retval eFrameworkunifiedStatusInvldParam Invalid Parameter
 * \~english @retval eFrameworkunifiedStatusNullPointer NULL Pointer
 * \~english @retval eFrameworkunifiedStatusFail Abnormal Error
 * \~english @par Prerequisite
 *       - None.
 * \~english @par Change of internal state
 *       - None.
 * \~english @par Conditions of processing failure
 *       - Input parameter error
 *         - eFrameworkunifiedStatusInvldParam
 *         - eFrameworkunifiedStatusNullPointer
 *       - Internal process error(open/read)
 *         - eFrameworkunifiedStatusFail
 * \~english @par Classification
 *       Public
 * \~english @par Type
 *       Sync
 * \~english @par Detail
 *       - Check the power state of USB port.
 *       - The API can not be used by multiple processes, threads at the same time.
 * \~english @see ResetUsbVbus, PowerOnUsbPort, PowerOffUsbPort
 */
EFrameworkunifiedStatus IsUsbPortPowered(UsbPortNumber usb_port_no, bool* is_powered);

/**
 * \ingroup PowerOnUsbPort
 * \~english @par Brief
 *       Power on USB port.
 * \~english @param [in] usb_port_no
 *       UsbPortNumber   - USB port number
 * \~english @retval eFrameworkunifiedStatusOK OK
 * \~english @retval eFrameworkunifiedStatusInvldParam Invalid Parameter
 * \~english @retval eFrameworkunifiedStatusFail Abnormal Error
 * \~english @par Prerequisite
 *       - When State value is "status faile",  Can not  power on USB port.
 * \~english @par Change of internal state
 *       - Change state "USB power on"
 * \~english @par Conditions of processing failure
 *       - Input parameter error
 *         - eFrameworkunifiedStatusInvldParam
 *       - Internal process error(open/read/write)
 *         - eFrameworkunifiedStatusFail
 *       - Internal process error(State value is "status faile")
 *         - eFrameworkunifiedStatusFail
 *         - Not change internal state.
 * \~english @par Classification
 *       Public
 * \~english @par Type
 *       Sync
 * \~english @par Detail
 *       - Power on USB port.
 *       - The API can not be used by multiple processes, threads at the same time.
 * \~english @see ResetUsbVbus, IsUsbPortPowered, PowerOffUsbPort
 */
EFrameworkunifiedStatus PowerOnUsbPort(UsbPortNumber usb_port_no);

/**
 * \ingroup PowerOffUsbPort
 * \~english @par Brief
 *       Power off USB port.
 * \~english @param [in] usb_port_no
 *       UsbPortNumber   - USB port number
 * \~english @retval eFrameworkunifiedStatusOK OK
 * \~english @retval eFrameworkunifiedStatusInvldParam Invalid Parameter
 * \~english @retval eFrameworkunifiedStatusFail Abnormal Error
 * \~english @par Prerequisite
 *       - None.
 * \~english @par Change of internal state
 *       - Change state "USB power off"
 * \~english @par Conditions of processing failure
 *       - Input parameter error
 *         - eFrameworkunifiedStatusInvldParam
 *       - Internal process error(open/read/write)
 *         - eFrameworkunifiedStatusFail
 * \~english @par Classification
 *       Public
 * \~english @par Type
 *       Sync
 * \~english @par Detail
 *       - Power off USB port.
 *       - The API can not be used by multiple processes, threads at the same time.
 * \~english @see ResetUsbVbus, IsUsbPortPowered, PowerOnUsbPort
 */
EFrameworkunifiedStatus PowerOffUsbPort(UsbPortNumber usb_port_no);

/**
 * \~english USB abnormal status
 */
enum UsbAbnormalStatus {
  /**
   * \~english USB is not abnormal
   */
  USB_ABNORMAL_STATUS_NONE = 0,
  /**
   * \~english USB is overcurrent
   */
  USB_ABNORMAL_STATUS_OVERCURRENT
};

/**
 * \ingroup CheckUsbAbnormalStatus
 * \~english @par Brief
 *       Check the abnormal status of USB port
 * \~english @param [in] usb_port_no
 *       UsbPortNumber   - USB port number
 * \~english @param [out] usb_abnormal_status
 *       UsbAbnormalStatus*   - USB abnormal status
 * \~english @retval eFrameworkunifiedStatusOK OK
 * \~english @retval eFrameworkunifiedStatusInvldParam Invalid Parameter
 * \~english @retval eFrameworkunifiedStatusNullPointer NULL Pointer
 * \~english @retval eFrameworkunifiedStatusFail Abnormal Error
 * \~english @par Prerequisite
 *       - None.
 * \~english @par Change of internal state
 *       - change state "usb port status faile" or "usb port status ok".
 * \~english @par Conditions of processing failure
 *       - Input parameter error
 *         - eFrameworkunifiedStatusInvldParam
 *         - eFrameworkunifiedStatusNullPointer
 *       - Internal process error(open/read)
 *         - eFrameworkunifiedStatusFail
 * \~english @par Classification
 *       Public
 * \~english @par Type
 *       Sync
 * \~english @par Detail
 *       - Check the abnormal status of USB port, including overcurrent and so on.
 *       - The API can not be used by multiple processes, threads at the same time.
 * \~english @see None.
 */
EFrameworkunifiedStatus CheckUsbAbnormalStatus(UsbPortNumber usb_port_no, UsbAbnormalStatus* usb_abnormal_status);

/** @}*/  // end of usb_hal
/** @}*/  // end of device_detection_service

#endif  // HAL_API_USB_HAL_H_
