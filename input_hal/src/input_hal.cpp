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

#include "input_hal.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "input_hal_debug.h"
#include "input_hal_internal.h"
#include "input_touch_ilitek.h"
#include "input_udev_monitor.h"

// Touch panel operation  function info
static struct TouchHal g_input_touch_info = { 0 };

char* g_app_name = NULL;
static bool g_touch_inited = false;
static bool g_input_inited = false;

extern bool g_break_from_watch;
extern pthread_t g_udev_monitor_thread;

// Environment key name
#define HAL_INPUT_TARGET_BOARD "TARGET_BOARD"
// Reference board environment value of HAL_INPUT_TARGET_BOARD
#define HAL_INPUT_REF_BOARD_NAME "agl_reference"
// Invalid status of report touch panel's touch event
#define HAL_INPUT_TOUCH_REPORT_INVALID  (-1)

/*
 * Input device init.
 */
int InitInput(const char* app_name) {
  if (NULL == app_name) {
    INPUT_ERROR_LOG("param is error");
    return HAL_INPUT_RET_ERROR;
  }

  if (!g_touch_inited) {
    INPUT_ERROR_LOG("call InitTouch first.");
    return HAL_INPUT_RET_ERROR;
  }

  if (g_input_inited) {
    INPUT_ERROR_LOG("input inited.");
    return HAL_INPUT_RET_ERROR;
  }

  g_break_from_watch = false;
  if (NULL != g_app_name) {
    delete[] g_app_name;
  }

  g_app_name = new char[strlen(app_name) + 1];
  snprintf(g_app_name, strlen(app_name) + 1, "%s", app_name);
  if (HAL_INPUT_RET_ERROR == InputUdevMonitorThreadCreate()) {
    delete[] g_app_name;
    g_app_name = NULL;
    return HAL_INPUT_RET_ERROR;
  }

  g_input_inited = true;
  return HAL_INPUT_RET_NORMAL;
}

/*
 * Deinit input device
 */
int DeInitInput() {
  g_break_from_watch = true;
  void* ret_val = NULL;
  if (NULL != g_app_name) {
    delete[] g_app_name;
    g_app_name = NULL;
  }
  if (g_udev_monitor_thread != static_cast<pthread_t>(-1)) {
    pthread_join(g_udev_monitor_thread, &ret_val);
  }
  g_input_inited = false;
  return HAL_INPUT_RET_NORMAL;
}

/*
 * Init those operating function of touch panel driver
 */
int InitTouch() {
  int ret = InputTouchIlitekInit(&g_input_touch_info);
  g_touch_inited = true;
  return ret;
}

/*
 * Make touch panel start work
 */
int StartTouch() {
  int ret = HAL_INPUT_RET_ERROR;

  if (NULL != g_input_touch_info.start) {
    ret = g_input_touch_info.start();
  }

  return ret;
}

/*
 * Config touch panel
 */
int ConfigTouch(const char *path , int reso_h, int reso_v) {
  int ret = HAL_INPUT_RET_ERROR;

  if (NULL != g_input_touch_info.config) {
    ret = g_input_touch_info.config(path, reso_h, reso_v);
  }

  return ret;
}

/*
 * Get touch panel device name
 */
int GetPanelNameTouch(char* name, size_t buf_length) {
  int ret = HAL_INPUT_RET_ERROR;

  if (NULL != g_input_touch_info.get_touch_devicename) {
    ret = g_input_touch_info.get_touch_devicename(name, buf_length);
  }

  return ret;
}

/*
 * Get touch panel key device name
 */
int GetKeyNameTouch(char* name, size_t buf_length) {
  int ret = HAL_INPUT_RET_ERROR;

  if (NULL != g_input_touch_info.get_key_devicename) {
    ret = g_input_touch_info.get_key_devicename(name, buf_length);
  }

  return ret;
}

/*
 * Execute touch panel self test
 */
int SelfTestTouch(int id, void *result) {
  int ret = HAL_INPUT_RET_ERROR;

  if (NULL != g_input_touch_info.selftest) {
    ret = g_input_touch_info.selftest(id, result);
  }

  return ret;
}

/*
 * Get touch panel config status
 */
int GetConfigStatusTouch(int *status) {
  int ret = HAL_INPUT_RET_ERROR;

  if (NULL != g_input_touch_info.get_config_status) {
    ret = g_input_touch_info.get_config_status(status);
  }

  return ret;
}

/*
 * Set whether the driver sends touch panel data or not
 */
int LockTouch(int status) {
  static int input_touch_lock_status = HAL_INPUT_TOUCH_REPORT_INVALID;

  if (input_touch_lock_status == status) {
    return HAL_INPUT_RET_NORMAL;
  }

  int ret = HAL_INPUT_RET_ERROR;
  if (NULL != g_input_touch_info.set_touch_lock) {
    ret = g_input_touch_info.set_touch_lock(status);
    if (HAL_INPUT_RET_NORMAL == ret) {
      input_touch_lock_status = status;
    }
  }

  return ret;
}

/*
 * Suspend touch panel
 */
int SuspendTouch() {
  int ret = HAL_INPUT_RET_ERROR;

  if (NULL != g_input_touch_info.set_touch_suspend) {
    ret = g_input_touch_info.set_touch_suspend();
  }

  return ret;
}

/*
 * Set touch panel sensitivity level
 */
int SetSensitivityLevelTouch(int level) {
  int cur = HAL_INPUT_TOUCH_SENSITIVITY_LEVEL_NONE;

  int ret = GetSensitivityLevelTouch(&cur);
  if (HAL_INPUT_RET_NORMAL == ret) {
    if (cur == level) {
      // Don't need to update sensitivity level
      INPUT_LOG_TRACE("already set level=%d", level);
    } else {
      if (NULL != g_input_touch_info.set_sensitivity_level) {
        ret = g_input_touch_info.set_sensitivity_level(level);
      } else {
        ret = HAL_INPUT_RET_ERROR;
      }
    }
  }

  return ret;
}

/*
 * Get touch panel sensitivity level
 */
int GetSensitivityLevelTouch(int *level) {
  int ret = HAL_INPUT_RET_ERROR;

  if (NULL != g_input_touch_info.get_sensitivity_level) {
    ret = g_input_touch_info.get_sensitivity_level(level);
  }

  return ret;
}

/*
 * Notify radio scan frequency
 */
int NotifyRadioScanFreqTouch(struct RadioInfoTouch *info) {
  int ret = HAL_INPUT_RET_ERROR;

  if (NULL != g_input_touch_info.notify_radio_scan_frequency) {
    ret = g_input_touch_info.notify_radio_scan_frequency(info);
  }

  return ret;
}
