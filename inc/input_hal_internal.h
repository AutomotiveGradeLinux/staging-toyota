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

#ifndef INC_INPUT_HAL_INTERNAL_H_
#define INC_INPUT_HAL_INTERNAL_H_

/*
 * Touch panel operation function info
 */
struct TouchHal {
    /**
     * \~english  Make touch panel start work.
     */
    int (* start)(void);
    /**
     * \~english  Get touch panel device horizontal resolution.
     */
    int (* get_reso_h)(int *);
    /**
     * \~english  Get touch panel device vertical resolution.
     */
    int (* get_reso_v)(int *);
    /**
     * \~english  Get whether X axis is inversion.
     */
    int (* get_reverse_axis_x)(bool *);
    /**
     * \~english  Get whether Y axis is inversion.
     */
    int (* get_reverse_axis_y)(bool *);
    /**
     * \~english  Config touch panel.
     */
    int (* config)(const char *, int, int);
    /**
     * \~english  Get touch panel device name.
     */
    int (* get_touch_devicename)(char*, size_t);
    /**
     * \~english  Get touch panel key device name.
     */
    int (* get_key_devicename)(char*, size_t);
    /**
     * \~english  Execute touch panel self test.
     */
    int (* selftest)(int, void *);
    /**
     * \~english  Get touch panel config status.
     */
    int (* get_config_status)(int *);
    /**
     * \~english  Set whether the driver sends touch panel data or not.
     */
    int (* set_touch_lock)(int);
    /**
     * \~english  Request touch panel touch suspend.
     */
    int (* set_touch_suspend)(void);
    /**
     * \~english  Set touch panel sensitivity level.
     */
    int (* set_sensitivity_level)(int);
    /**
     * \~english  Get touch panel sensitivity level.
     */
    int (* get_sensitivity_level)(int *);
    /**
     * \~english  Notify radio scan frequency.
     */
    int (* notify_radio_scan_frequency)(struct RadioInfoTouch *);
};


/**
 * Config touch panel
 */
int ConfigTouch(const char *path , int resolution_h, int resolution_v);

/**
 * Get touch panel device name
 */
int GetPanelNameTouch(char* name, size_t buf_length);

/**
 * Get touch panel key device name
 */
int GetKeyNameTouch(char* name, size_t buf_length);

/**
 * Get touch panel config status
 */
int GetConfigStatusTouch(int *status);

#endif  // INC_INPUT_HAL_INTERNAL_H_
