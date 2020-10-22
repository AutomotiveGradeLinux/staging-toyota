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

#ifndef INC_USB_HAL_INTERNAL_H_
#define INC_USB_HAL_INTERNAL_H_

#define FILE_CONTENT_LENGTH (32)
#define FILE_PATH_LENGTH (128)

/**
 * USB role file
 */
#define USB_ROLE_FILE "/sys/devices/platform/soc/ee080200.usb-phy/role"

/**
 * USB host value
 */
#define USB_HOST_STRING "host\n"

/**
 * USB function value
 */
#define USB_FUNCTION_STRING "peripheral\n"

/**
 * gpio high value
 */
const char kUsbGpioHighValue = '1';

/**
 * gpio low value
 */
const char kUsbGpioLowValue = '0';

/**
 * gpio export file
 */
const char kUsbGpioExportFile[] = "/sys/class/gpio/export";

/**
 * gpio file prefix
 */
const char kUsbGpioFilePrefix[] = "/sys/class/gpio/gpio";

/**
 * gpio direction file
 */
const char kUsbGpioDirectionFile[] = "/direction";

/**
 * gpio direction in
 */
const char kUsbGpioDirectionIn[] = "in";

/**
 * gpio direction out high
 */
const char kUsbGpioDirectionHigh[] = "high";

/**
 * gpio direction out low
 */
const char kUsbGpioDirectionLow[] = "low";

/**
 * gpio value file
 */
const char kUsbGpioValueFile[] = "/value";

/**
 * structure of USB role switch information
 */
struct UsbRoleSwitchInfo {
  const char* file_path;           //!< USB role switch file path
  const char* usb_host_value;      //!< USB host value
  const char* usb_function_value;  //!< USB function value
};

/**
 * USB gpio port definition
 */
enum UsbGpioPort {
  USB_GPIO_PORT_USB0_PWEN = 0,  //!< USB0 power enable
  USB_GPIO_PORT_USB1_PWEN,      //!< USB1 power enable
  USB_GPIO_PORT_USB2_PWEN,      //!< USB2 power enable
  USB_GPIO_PORT_USB0_OVC,       //!< USB0 overcurrent
  USB_GPIO_PORT_USB1_OVC,       //!< USB1 overcurrent
  USB_GPIO_PORT_USB2_OVC,       //!< USB2 overcurrent
  USB_GPIO_PORT_MAX
};

/**
 * USB gpio port value definition
 */
enum GpioPortValue {
  GPIO_PORT_OFF = 0,  //!< gpio port on
  GPIO_PORT_ON        //!< gpio port off
};

/**
 * Length for USB gpio port name
 */
#define USB_PORT_NAME_LENGTH (8)

/**
 * USB gpio port information
 */
struct UsbGpioPortInfo {
  bool active_low;        //!< active low or high
  bool is_output;         //!< direction output or input
  const char* port_name;  //!< gpio port name
  char default_value;     //!< output port default value (high or low)
};

/**
 * USB gpio port config table
 */
const char kUsbGpioPortNameUsb0Pwen[] = "375";
const char kUsbGpioPortNameUsb1Pwen[] = "387";
const char kUsbGpioPortNameUsb2Pwen[] = "389";
const char kUsbGpioPortNameUsb0Ovc[] = "376";
const char kUsbGpioPortNameUsb1Ovc[] = "388";
const char kUsbGpioPortNameUsb2Ovc[] = "390";

const UsbGpioPortInfo kUsbGpioInfo[] = {
  { false, true, kUsbGpioPortNameUsb0Pwen, kUsbGpioHighValue },
  { false, true, kUsbGpioPortNameUsb1Pwen, kUsbGpioHighValue },
  { false, true, kUsbGpioPortNameUsb2Pwen, kUsbGpioHighValue },
  { false, false, kUsbGpioPortNameUsb0Ovc, kUsbGpioLowValue },
  { false, false, kUsbGpioPortNameUsb1Ovc, kUsbGpioLowValue },
  { false, false, kUsbGpioPortNameUsb2Ovc, kUsbGpioLowValue }
};

/**
 * gpio port list of USB power enable
 */
const UsbGpioPort kUsbPowerEnableGpio[] = {
  USB_GPIO_PORT_USB0_PWEN,
  USB_GPIO_PORT_USB1_PWEN,
  USB_GPIO_PORT_USB2_PWEN
};

/**
 * gpio port list of USB overcurrent
 */
const UsbGpioPort kUsbOvercurrentGpio[] = {
  USB_GPIO_PORT_USB0_OVC,
  USB_GPIO_PORT_USB1_OVC,
  USB_GPIO_PORT_USB2_OVC
};

#endif  // INC_USB_HAL_INTERNAL_H_
