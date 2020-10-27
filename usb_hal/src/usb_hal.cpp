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

/*
 * Information.
 * This source code is a sample source code .
 * Implementation of the function must be performed by the vendor.
 */

#include "usb_hal.h"
#include "usb_hal_internal.h"
#include "usb_hal_debug.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

/**
 * USB role switch information table
 */
const UsbRoleSwitchInfo kUsbRoleSwitchTable = {USB_ROLE_FILE, USB_HOST_STRING, USB_FUNCTION_STRING};

/**
 * Read content of USB role switch file, then determine HOST/FUNCTION role
 * according to the specified HOST/FUNCTION value.
 */
static EFrameworkunifiedStatus GetUsbRoleType(const char* file_path, const char* usb_host_value, const char* usb_function_value,
                                 UsbRoleType* usb_role) {
  if (!file_path || !usb_host_value || !usb_function_value || !usb_role) {
    USB_ERROR_LOG("parameter is NULL.");
    return eFrameworkunifiedStatusFail;
  }

  // 1. open role switch file
  int fd = open(file_path, O_RDONLY);
  if (-1 == fd) {
    USB_ERROR_LOG("open [%s] failed.(%d, %s)", file_path, errno, strerror(errno));
    return eFrameworkunifiedStatusFail;
  }

  // 2. read file content
  char file_content[FILE_CONTENT_LENGTH];
  memset(file_content, 0, sizeof(file_content));
  ssize_t ret = read(fd, file_content, sizeof(file_content) - 1);
  int read_error = errno;

  // 3. close file
  close(fd);

  if (-1 == ret) {
    USB_ERROR_LOG("read failed.(%d, %s)", read_error, strerror(read_error));
    return eFrameworkunifiedStatusFail;
  }

  // 4. compare file content to HOST/FUNCTION value
  if (!strcmp(usb_host_value, file_content)) {
    *usb_role = USB_ROLE_HOST;
  } else if (!strcmp(usb_function_value, file_content)) {
    *usb_role = USB_ROLE_FUNCTION;
  } else {
    USB_ERROR_LOG("content error.[%s]", file_content);
    return eFrameworkunifiedStatusFail;
  }

  return eFrameworkunifiedStatusOK;
}

/**
 * Write the specified role value to USB role switch file.
 */
static EFrameworkunifiedStatus SetUsbRoleValue(const char* file_path, const char* role_value) {
  if (!file_path || !role_value) {
    USB_ERROR_LOG("parameter is NULL.");
    return eFrameworkunifiedStatusFail;
  }

  // 1. open role switch file
  int fd = open(file_path, O_WRONLY | O_TRUNC);
  if (-1 == fd) {
    USB_ERROR_LOG("open [%s] failed.(%d, %s)", file_path, errno, strerror(errno));
    return eFrameworkunifiedStatusFail;
  }

  // 2. write role file
  ssize_t ret = write(fd, role_value, strlen(role_value));
  int write_error = errno;

  // 3. close file
  close(fd);

  // 4. check write result
  if (static_cast<ssize_t>(strlen(role_value)) != ret) {
    USB_ERROR_LOG("write [%s] failed, ret=%zd.(%d, %s)", role_value, ret, write_error, strerror(write_error));
    return eFrameworkunifiedStatusFail;
  } else {
    return eFrameworkunifiedStatusOK;
  }
}

/**
 * Check whether the specified USB port supports power control.
 */
static bool CheckSupportPowerControl(UsbPortNumber usb_port_no) {
  if (USB_PORT_NUMBER_MAX <= usb_port_no) {
    return false;
  }

  return true;
}

/**
 * Check whether the specified gpio port is available.
 */
static bool CheckGpioPortAvailable(UsbGpioPort gpio_port) {
  if (USB_GPIO_PORT_MAX <= gpio_port) {
    USB_ERROR_LOG("port %d is invalid.", gpio_port);
    return false;
  }

  if (!kUsbGpioInfo[gpio_port].port_name || (0 == strlen(kUsbGpioInfo[gpio_port].port_name))) {
    USB_ERROR_LOG("port %d is not available.", gpio_port);
    return false;
  }

  return true;
}

/**
 * Open gpio export file.
 */
static int OpenExportFile() {
  return open(kUsbGpioExportFile, O_WRONLY);
}

/**
 * Open the direction file of the specified gpio port.
 */
static int OpenDirectionFile(const char* port_name) {
  if (!port_name || (0 == strlen(port_name))) {
    USB_ERROR_LOG("port name is invalid.");
    return -1;
  }

  char dir_file[FILE_PATH_LENGTH];
  // /sys/class/gpio/gpio123/direction
  snprintf(dir_file, sizeof(dir_file), "%s%s%s", kUsbGpioFilePrefix, port_name, kUsbGpioDirectionFile);
  return open(dir_file, O_RDWR);
}

/**
 * Open the value file of the specified gpio port.
 */
static int OpenValueFile(const char* port_name, bool ro) {
  if (!port_name || (0 == strlen(port_name))) {
    USB_ERROR_LOG("port name is invalid.");
    return -1;
  }

  char value_file[FILE_PATH_LENGTH];
  // /sys/class/gpio/gpio123/value
  snprintf(value_file, sizeof(value_file), "%s%s%s", kUsbGpioFilePrefix, port_name, kUsbGpioValueFile);
  return open(value_file, ro ? O_RDONLY : O_RDWR);
}

/**
 * Initialize gpio port value.
 */
static EFrameworkunifiedStatus InitializeGpioPort(UsbGpioPort gpio_port) {
  if (!CheckGpioPortAvailable(gpio_port)) {
    return eFrameworkunifiedStatusFail;
  }

  char port_file[FILE_PATH_LENGTH];
  snprintf(port_file, sizeof(port_file), "%s%s", kUsbGpioFilePrefix, kUsbGpioInfo[gpio_port].port_name);

  // check if port is exported already
  if (0 == access(port_file, F_OK)) {
    return eFrameworkunifiedStatusOK;
  }

  // open the export file
  int export_fd = OpenExportFile();
  if (-1 == export_fd) {
    USB_ERROR_LOG("open [%s] failed.(%d, %s)", "export", errno, strerror(errno));
    return eFrameworkunifiedStatusFail;
  }

  // do export by writing the export file
  ssize_t ret = write(export_fd, kUsbGpioInfo[gpio_port].port_name, strlen(kUsbGpioInfo[gpio_port].port_name));
  int write_error = errno;

  // close the export file
  close(export_fd);

  // check write result
  if (static_cast<ssize_t>(strlen(kUsbGpioInfo[gpio_port].port_name)) != ret) {
    USB_ERROR_LOG("write [%s] failed, ret=%zd.(%d, %s)", kUsbGpioInfo[gpio_port].port_name, ret, write_error,
                  strerror(write_error));
    return eFrameworkunifiedStatusFail;
  }

  // open the direction file
  int direction_fd = OpenDirectionFile(kUsbGpioInfo[gpio_port].port_name);
  if (-1 == direction_fd) {
    USB_ERROR_LOG("open direction file failed.(%d, %s)", errno, strerror(errno));
    return eFrameworkunifiedStatusFail;
  }

  // set direction
  const char* direction_value;
  if (!kUsbGpioInfo[gpio_port].is_output) {
    // for input port, set direction as "in"
    direction_value = kUsbGpioDirectionIn;
  } else {
    if (kUsbGpioHighValue == kUsbGpioInfo[gpio_port].default_value) {
      // for output port, if default value is high, set direction as "high"
      direction_value = kUsbGpioDirectionHigh;
    } else if (kUsbGpioLowValue == kUsbGpioInfo[gpio_port].default_value) {
      // for output port, if default value is low, set direction as "low"
      direction_value = kUsbGpioDirectionLow;
    } else {
      USB_ERROR_LOG("unknown default value[%c]", kUsbGpioInfo[gpio_port].default_value);
      return eFrameworkunifiedStatusFail;
    }
  }

  ret = write(direction_fd, direction_value, strlen(direction_value));
  write_error = errno;
  close(direction_fd);
  if (static_cast<ssize_t>(strlen(direction_value)) != ret) {
    USB_ERROR_LOG("write direction %s failed, ret=%zd.(%d, %s)", direction_value, ret, write_error,
                  strerror(write_error));
    return eFrameworkunifiedStatusFail;
  }

  return eFrameworkunifiedStatusOK;
}

/**
 * Get gpio port value.
 */
static EFrameworkunifiedStatus GetGpioPortValue(UsbGpioPort port_name, bool* port_value) {
  if ((USB_GPIO_PORT_MAX <= port_name) || !port_value) {
    USB_ERROR_LOG("parameter is invalid.");
    return eFrameworkunifiedStatusFail;
  }

  // initialize gpio port
  EFrameworkunifiedStatus result = InitializeGpioPort(port_name);
  if (eFrameworkunifiedStatusOK != result) {
    return result;
  }

  // open port value file
  int fd = OpenValueFile(kUsbGpioInfo[port_name].port_name, true);
  if (-1 == fd) {
    USB_ERROR_LOG("open value file failed.(%d, %s)", errno, strerror(errno));
    return eFrameworkunifiedStatusFail;
  }

  // read file content
  char file_content = '\0';
  ssize_t ret = read(fd, &file_content, sizeof(file_content));
  int read_error = errno;

  // close file
  close(fd);

  if (sizeof(file_content) != ret) {
    USB_ERROR_LOG("read failed.(%d, %s)", read_error, strerror(read_error));
    return eFrameworkunifiedStatusFail;
  }

  // compare file content to high/low value
  if (kUsbGpioHighValue == file_content) {
    *port_value = (kUsbGpioInfo[port_name].active_low ? false : true);
  } else if (kUsbGpioLowValue == file_content) {
    *port_value = (kUsbGpioInfo[port_name].active_low ? true : false);
  } else {
    USB_ERROR_LOG("content error.[%c]", file_content);
    return eFrameworkunifiedStatusFail;
  }

  return eFrameworkunifiedStatusOK;
}

/**
 * Set gpio port value.
 */
static EFrameworkunifiedStatus SetGpioPortValue(UsbGpioPort port_name, GpioPortValue port_value) {
  if (USB_GPIO_PORT_MAX <= port_name) {
    USB_ERROR_LOG("parameter is invalid.");
    return eFrameworkunifiedStatusFail;
  }

  // initialize gpio port
  EFrameworkunifiedStatus result = InitializeGpioPort(port_name);
  if (eFrameworkunifiedStatusOK != result) {
    return result;
  }

  // check direction is output
  if (!kUsbGpioInfo[port_name].is_output) {
    USB_ERROR_LOG("%d not an output port.", port_name);
    return eFrameworkunifiedStatusFail;
  }

  // open port value file
  int fd = OpenValueFile(kUsbGpioInfo[port_name].port_name, false);
  if (-1 == fd) {
    USB_ERROR_LOG("open value file failed.(%d, %s)", errno, strerror(errno));
    return eFrameworkunifiedStatusFail;
  }

  // write value
  bool port_active = (GPIO_PORT_ON == port_value ? true : false);
  bool write_low = (kUsbGpioInfo[port_name].active_low ? port_active : !port_active);
  char write_value = (write_low ? kUsbGpioLowValue : kUsbGpioHighValue);
  ssize_t ret = write(fd, &write_value, sizeof(write_value));
  int write_error = errno;

  // close file
  close(fd);

  // check write result
  if (sizeof(write_value) != ret) {
    USB_ERROR_LOG("write [%c] failed, ret=%zd.(%d, %s)", write_value, ret, write_error, strerror(write_error));
    return eFrameworkunifiedStatusFail;
  } else {
    return eFrameworkunifiedStatusOK;
  }
}

/**
 * Check overcurrent.
 */
static EFrameworkunifiedStatus CheckUsbOvercurrent(UsbPortNumber usb_port_no, bool* is_ovc) {
  if ((USB_PORT_NUMBER_MAX <= usb_port_no) || !is_ovc) {
    return eFrameworkunifiedStatusFail;
  }

  return GetGpioPortValue(kUsbOvercurrentGpio[usb_port_no], is_ovc);
}

EFrameworkunifiedStatus GetUsbRoleSwitch(UsbRoleType* usb_role) {
  // check NULL pointer
  if (!usb_role) {
    USB_ERROR_LOG("parameter is NULL.");
    return eFrameworkunifiedStatusNullPointer;
  }

  return GetUsbRoleType(kUsbRoleSwitchTable.file_path, kUsbRoleSwitchTable.usb_host_value,
                        kUsbRoleSwitchTable.usb_function_value, usb_role);
}

EFrameworkunifiedStatus SetUsbRoleSwitch(UsbRoleType usb_role) {
  if ((USB_ROLE_HOST != usb_role) && (USB_ROLE_FUNCTION != usb_role)) {
    USB_ERROR_LOG("parameter error.[%d]", usb_role);
    return eFrameworkunifiedStatusInvldParam;
  }

  // check current usb role:
  // allow to switch only when usb_role is different from current usb role.
  UsbRoleType current_role;
  if (eFrameworkunifiedStatusOK != GetUsbRoleSwitch(&current_role)) {
    return eFrameworkunifiedStatusFail;
  }

  if (current_role == usb_role) {
    return eFrameworkunifiedStatusOK;
  }

  // check usb role type
  //  - HOST: write HOST value to role file
  //  - FUNCTION: write FUNCTION value to role file
  return SetUsbRoleValue(kUsbRoleSwitchTable.file_path, (USB_ROLE_HOST == usb_role)
                                                            ? kUsbRoleSwitchTable.usb_host_value
                                                            : kUsbRoleSwitchTable.usb_function_value);
}

EFrameworkunifiedStatus CheckUsbAuthenticationError(UsbAuthenticationError* usb_auth_error) {
  // check NULL pointer
  if (!usb_auth_error) {
    USB_ERROR_LOG("parameter is NULL.");
    return eFrameworkunifiedStatusNullPointer;
  }

  // Empty Stub implementations
  *usb_auth_error = USB_AUTHENTICATION_ERROR_NONE;
  return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus ResetUsbVbus(UsbPortNumber usb_port_no) {
  // check support power control
  if (!CheckSupportPowerControl(usb_port_no)) {
    return eFrameworkunifiedStatusInvldParam;
  }

  // step 1: power off USB port
  EFrameworkunifiedStatus result = SetGpioPortValue(kUsbPowerEnableGpio[usb_port_no], GPIO_PORT_OFF);
  if (eFrameworkunifiedStatusOK != result) {
    USB_ERROR_LOG("power off USB%d failed.", usb_port_no);
    return result;
  }

  // step 2: sleep 1.1 sec
  usleep(1100 * 1000);

  // step 3: power on USB port
  result = SetGpioPortValue(kUsbPowerEnableGpio[usb_port_no], GPIO_PORT_ON);
  if (eFrameworkunifiedStatusOK != result) {
    USB_ERROR_LOG("power on USB%d failed.", usb_port_no);
  }
  return result;
}

EFrameworkunifiedStatus IsUsbPortPowered(UsbPortNumber usb_port_no, bool* is_powered) {
  if (!is_powered) {
    return eFrameworkunifiedStatusNullPointer;
  }

  // check support power control
  if (!CheckSupportPowerControl(usb_port_no)) {
    return eFrameworkunifiedStatusInvldParam;
  }

  return GetGpioPortValue(kUsbPowerEnableGpio[usb_port_no], is_powered);
}

EFrameworkunifiedStatus PowerOnUsbPort(UsbPortNumber usb_port_no) {
  // check support power control
  if (!CheckSupportPowerControl(usb_port_no)) {
    return eFrameworkunifiedStatusInvldParam;
  }
  bool current_power = false;
  if (eFrameworkunifiedStatusOK != GetGpioPortValue(kUsbPowerEnableGpio[usb_port_no], &current_power)) {
    return eFrameworkunifiedStatusFail;
  }

  if (current_power) {
    return eFrameworkunifiedStatusOK;
  }
  return SetGpioPortValue(kUsbPowerEnableGpio[usb_port_no], GPIO_PORT_ON);
}

EFrameworkunifiedStatus PowerOffUsbPort(UsbPortNumber usb_port_no) {
  // check support power control
  if (!CheckSupportPowerControl(usb_port_no)) {
    return eFrameworkunifiedStatusInvldParam;
  }
  bool current_power = false;
  if (eFrameworkunifiedStatusOK != GetGpioPortValue(kUsbPowerEnableGpio[usb_port_no], &current_power)) {
    return eFrameworkunifiedStatusFail;
  }

  if (!current_power) {
    return eFrameworkunifiedStatusOK;
  }

  return SetGpioPortValue(kUsbPowerEnableGpio[usb_port_no], GPIO_PORT_OFF);
}

EFrameworkunifiedStatus CheckUsbAbnormalStatus(UsbPortNumber usb_port_no, UsbAbnormalStatus* usb_abnormal_status) {
  if (USB_PORT_NUMBER_MAX <= usb_port_no) {
    return eFrameworkunifiedStatusInvldParam;
  }

  // check NULL pointer
  if (!usb_abnormal_status) {
    return eFrameworkunifiedStatusNullPointer;
  }

  // check overcurrent
  bool is_ovc = false;
  EFrameworkunifiedStatus result = CheckUsbOvercurrent(usb_port_no, &is_ovc);
  if (eFrameworkunifiedStatusOK == result) {
    if (is_ovc) {
      *usb_abnormal_status = USB_ABNORMAL_STATUS_OVERCURRENT;
    } else {
      *usb_abnormal_status = USB_ABNORMAL_STATUS_NONE;
    }
  }
  return result;
}
