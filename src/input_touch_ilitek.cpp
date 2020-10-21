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
#include "input_touch_ilitek.h"

#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "input_hal.h"

// Touch panel device name
#define HAL_INPUT_ILITEK_TOUCH_DEVICE_NAME "ILITEK ILITEK Multi-Touch"
// Touch panel key device name
#define HAL_INPUT_ILITEK_KEY_DEVICE_NAME ""

// Touch panel horizontal resolution value
#define HAL_INPUT_TOUCH_RESOLUTION_HORIZONTAL   4816

// Touch panel vertical resolution value
#define HAL_INPUT_TOUCH_RESOLUTION_VERTICAL     2992

/*
 * Make touch panel start work
 */
static int InputTouchStart() {
  return HAL_INPUT_RET_NORMAL;
}

/*
 * Config touch panel
 */
static int InputTouchConfig(const char *path,
                            int resolution_h, int resolution_v) {
  if (NULL == path) {
    return HAL_INPUT_RET_ERROR;
  }

  if (-1 == ::access(path, F_OK)) {
    return HAL_INPUT_RET_ERROR;
  }
  return HAL_INPUT_RET_NORMAL;
}

/*
 * Get touch panel device name
 */
static int InputTouchGetDeviceName(char* name, size_t buf_length) {
  if (NULL == name) {
    return HAL_INPUT_RET_ERROR;
  }

  if (buf_length < (strlen(HAL_INPUT_ILITEK_TOUCH_DEVICE_NAME) + 1)) {
    return HAL_INPUT_RET_ERROR;
  }

  snprintf(name, buf_length, "%s", HAL_INPUT_ILITEK_TOUCH_DEVICE_NAME);
  return HAL_INPUT_RET_NORMAL;
}

/*
 * Get touch panel key device name
 */
static int InputTouchGetKeyName(char* name, size_t buf_length) {
  if (NULL == name) {
    return HAL_INPUT_RET_ERROR;
  }

  if (buf_length < (strlen(HAL_INPUT_ILITEK_KEY_DEVICE_NAME) + 1)) {
    return HAL_INPUT_RET_ERROR;
  }

  snprintf(name, buf_length, "%s", HAL_INPUT_ILITEK_KEY_DEVICE_NAME);
  return HAL_INPUT_RET_NORMAL;
}

/*
 * Get touch panel device horizontal resolution
 */
static int InputTouchGetDeviceHResolution(int *resolution) {
  if (NULL == resolution) {
    return HAL_INPUT_RET_ERROR;
  }
  *resolution = HAL_INPUT_TOUCH_RESOLUTION_HORIZONTAL;
  return HAL_INPUT_RET_NORMAL;
}

/*
 * Get touch panel device vertical resolution
 */
static int InputTouchGetDeviceVResolution(int *resolution) {
  if (NULL == resolution) {
    return HAL_INPUT_RET_ERROR;
  }
  *resolution = HAL_INPUT_TOUCH_RESOLUTION_VERTICAL;
  return HAL_INPUT_RET_NORMAL;
}

/*
 * Get whether X axis is inversion
 */
static int InputTouchGetXAxisReverse(bool* is_reverse) {
  if (NULL == is_reverse) {
    return HAL_INPUT_RET_ERROR;
  }
  *is_reverse = false;
  return HAL_INPUT_RET_NORMAL;
}

/*
 * Get whether Y axis is inversion
 */
static int InputTouchGetYAxisReverse(bool* is_reverse) {
  if (NULL == is_reverse) {
    return HAL_INPUT_RET_ERROR;
  }
  *is_reverse = false;
  return HAL_INPUT_RET_NORMAL;
}

/*
 * Execute touch panel self test
 */
static int InputTouchSelftest(int id, void *result) {
  if (NULL == result) {
    return HAL_INPUT_RET_ERROR;
  }
  return HAL_INPUT_RET_NOT_SUPPORT;
}

/*
 * Get touch panel config status
 */
static int InputTouchGetConfigStatus(int *status) {
  if (NULL == status) {
    return HAL_INPUT_RET_ERROR;
  }
  *status = HAL_INPUT_TOUCH_CONFIG_OFF;
  return HAL_INPUT_RET_NORMAL;
}

/*
 * Set whether the driver sends touch panel data or not
 */
static int InputTouchSetTouchLock(int lock) {
  if ((HAL_INPUT_TOUCH_UNREPORT == lock) ||
    (HAL_INPUT_TOUCH_REPORT == lock)) {
    return HAL_INPUT_RET_NOT_SUPPORT;
  }
  return HAL_INPUT_RET_ERROR;
}

/*
 * Suspend touch panel
 */
static int InputTouchSetTouchSuspend() {
  return HAL_INPUT_RET_NOT_SUPPORT;
}

/*
 * Set touch panel sensitivity level
 */
static int InputTouchSetSensitivityLevel(int level) {
  if ((HAL_INPUT_TOUCH_SENSITIVITY_LEVEL_LOW == level) ||
    (HAL_INPUT_TOUCH_SENSITIVITY_LEVEL_MIDDLE == level) ||
    (HAL_INPUT_TOUCH_SENSITIVITY_LEVEL_HIGH == level) ||
    (HAL_INPUT_TOUCH_SENSITIVITY_LEVEL_NONE == level) ) {
    return HAL_INPUT_RET_NOT_SUPPORT;
  }
  return HAL_INPUT_RET_ERROR;
}

/*
 * Get touch panel sensitivity level
 */
static int InputTouchGetSensitivityLevel(int *level) {
  if (NULL == level) {
    return HAL_INPUT_RET_ERROR;
  }

  return HAL_INPUT_RET_NOT_SUPPORT;
}

/*
 * Notify radio scan frequency
 */
static int InputTouchNotifyRadioScanFrequency(struct RadioInfoTouch *info) {
  if (NULL == info) {
    return HAL_INPUT_RET_ERROR;
  }
  return HAL_INPUT_RET_NOT_SUPPORT;
}

/*
 * Init touch panel operation function
 */
int InputTouchIlitekInit(struct TouchHal *touch) {
  touch->start = InputTouchStart;
  touch->config = InputTouchConfig;
  touch->get_touch_devicename = InputTouchGetDeviceName;
  touch->get_key_devicename = InputTouchGetKeyName;
  touch->get_reso_h = InputTouchGetDeviceHResolution;
  touch->get_reso_v = InputTouchGetDeviceVResolution;
  touch->get_reverse_axis_x = InputTouchGetXAxisReverse;
  touch->get_reverse_axis_y = InputTouchGetYAxisReverse;
  touch->selftest = InputTouchSelftest;
  touch->get_config_status = InputTouchGetConfigStatus;
  touch->set_touch_lock = InputTouchSetTouchLock;
  touch->set_touch_suspend = InputTouchSetTouchSuspend;
  touch->set_sensitivity_level = InputTouchSetSensitivityLevel;
  touch->get_sensitivity_level = InputTouchGetSensitivityLevel;
  touch->notify_radio_scan_frequency = InputTouchNotifyRadioScanFrequency;
  return HAL_INPUT_RET_NORMAL;
}
