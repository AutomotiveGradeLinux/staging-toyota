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

#ifndef HAL_API_INPUT_HAL_H_
#define HAL_API_INPUT_HAL_H_

#include <native_service/frameworkunified_types.h>

#include "peripheral_service/aglinput.h"

/**
 * @file input_hal.h
 */

/** @addtogroup switchhandler
 *  @{
 */
/** @addtogroup input_hal
 *  @ingroup switchhandler
 *  @{
 */

/************************************************************************
*           Macro definitions                                           *
************************************************************************/
/**
 * \~english  Return value type
 */
enum HalInputRetType {
    /**
     * \~english  Success
     */
    HAL_INPUT_RET_NORMAL = 0,
    /**
     * \~english  Process abnormality
     */
    HAL_INPUT_RET_ERROR,
    /**
     * \~english  Not support
     */
    HAL_INPUT_RET_NOT_SUPPORT,
};
/**
 * \~english  Touch panel IC type
 */
enum HalInputTouchDeviceType {
    /**
     * \~english  Touch panel IC is invalid
     */
    HAL_INPUT_TOUCH_DEVICE_INVALID = 0,
    /**
     * \~english  Touch panel IC is ILITEK
     */
    HAL_INPUT_TOUCH_DEVICE_ILITEK,
};

/**
 * \~english  Radio band type
 */
enum HalInputBandType {
    /**
     * \~english  Radio band is AM
     */
    HAL_INPUT_BAND_TYPE_AM = 0,
    /**
     * \~english  Radio band isn't AM
     */
    HAL_INPUT_BAND_TYPE_NOT_AM,
};

/**
 * \~english  Radio reception status
 */
enum HalInputTuneStatus {
    /**
     * \~english  Radio is in normal state
     */
    HAL_INPUT_TUNE_STATUS_NORMAL = 0,
    /**
     * \~english  Radio is in search state
     */
    HAL_INPUT_TUNE_STATUS_SERACH,
};

/**
 * \~english  Sensitivity Level
 */
enum HalInputTouchSensitivityLevel {
    /**
     * \~english  Sensitivity level low
     */
    HAL_INPUT_TOUCH_SENSITIVITY_LEVEL_LOW = 1,
    /**
     * \~english  Sensitivity level middle
     */
    HAL_INPUT_TOUCH_SENSITIVITY_LEVEL_MIDDLE,
    /**
     * \~english  Sensitivity level high
     */
    HAL_INPUT_TOUCH_SENSITIVITY_LEVEL_HIGH,
    /**
     * \~english  Sensitivity level none
     */
    HAL_INPUT_TOUCH_SENSITIVITY_LEVEL_NONE,
};

/**
 * \~english  input device type
 */
enum HalInputDeviceType {
  /**
   * \~english The device type invalid
   */
  HAL_INPUT_DEVICE_INVALID,
  /**
   * \~english The device is keyboard
   */
  HAL_INPUT_DEVICE_KEYBOARD,
  /**
   * \~english The device is touch
   */
  HAL_INPUT_DEVICE_TOUCH,
  /**
   * \~english The device is ESC-KEY
   */
  HAL_INPUT_DEVICE_TOUCH_ESCKEY,
  /**
   * \~english The device is steering SW
   */
  HAL_INPUT_DEVICE_STEERING,
  /**
   * \~english The device is tablet finger
   */
  HAL_INPUT_DEVICE_TABLET_FINGER,
  /**
   * \~english The device is rotary key
   */
  HAL_INPUT_DEVICE_ROTARY_KEY,
};

/**
 * \~english  Don't need to config touch panel
 */
#define HAL_INPUT_TOUCH_CONFIG_OFF      (0)
/**
 * \~english  Touch panel configed
 */
#define HAL_INPUT_TOUCH_CONFIG_ON       (1)

/**
 * \~english  Touch panel touch press
 */
#define HAL_INPUT_TOUCH_PRESS           (0)
/**
 * \~english  Touch panel touch release
 */
#define HAL_INPUT_TOUCH_RELEASE         (1)

/**
 * \~english  Don't report touch panel's touch event
 */
#define HAL_INPUT_TOUCH_UNREPORT        (0)
/**
 * \~english  Report touch panel's touch event
 */
#define HAL_INPUT_TOUCH_REPORT          (1)

/**
 * \~english  Test all case
 */
#define HAL_INPUT_TOUCH_SELFTEST_ID_ALL         (0xFE)
/**
 * \~english  Selftest data length
 */
#define HAL_INPUT_TOUCH_SELFTEST_DATA_LEN       (5)
/**
 * \~english  Selftest mode not support
 */
#define HAL_INPUT_TOUCH_SELFTEST_NOT_SUPPORT    (0xEE)

/**
 * \~english  max number of input_event per package.
 */
#define HAL_INPUT_EVENT_COUNT 64

/**
 * \~english  Thread name defined
 */
#define HAL_INPUT_SOURCE_NAME   "input_hal_mon"

/**
 * \~english  Notify input event from touch panel,
 *            The data of the notification please refer the following two type.\n
 *            @ref HAL_INPUT_TOUCH_PRESS \n
 *            @ref HAL_INPUT_TOUCH_RELEASE \n
 *            Please use IF of NSFW as follows to receive this event.
 * \~ @code
 * l_eStatus = FrameworkunifiedAttachCallbackToDispatcher(happ, HAL_INPUT_SOURCE_NAME, HAL_INPUT_NOTIFY_TOUCH, fpOnCmd);
 * @endcode
 * \~english note:FrameworkunifiedAttachCallbacksToDispatcher is available, too.
 */
#define HAL_INPUT_NOTIFY_TOUCH            100
/**
 * \~english  Notify input event from touch panel ESC-KEY,
 *            The data format of the notification please refer to @ref EventsPackageInput.\n
 *            Please use IF of NSFW as follows to receive this event.
 * \~ @code
 * l_eStatus = FrameworkunifiedAttachCallbackToDispatcher(happ, HAL_INPUT_SOURCE_NAME, HAL_INPUT_NOTIFY_ESC_KEY, fpOnCmd)
 * @endcode
 * \~english note:FrameworkunifiedAttachCallbacksToDispatcher is available, too.
 */
#define HAL_INPUT_NOTIFY_ESC_KEY          101
/**
 * \~english  Notify input event from key board,
 *            The data format of the notification please refer to @ref EventsPackageInput.\n
 *            Please use IF of NSFW as follows to receive this event.
 * \~ @code
 * l_eStatus = FrameworkunifiedAttachCallbackToDispatcher(happ, HAL_INPUT_SOURCE_NAME, HAL_INPUT_NOTIFY_KEY_BOARD, fpOnCmd)
 * @endcode
 * \~english note:FrameworkunifiedAttachCallbacksToDispatcher is available, too.
 */
#define HAL_INPUT_NOTIFY_KEY_BOARD        102
/**
 * \~english  Notify input event from steering,
 *            The data format of the notification please refer to @ref EventsPackageInput.\n
 *            Please use IF of NSFW as follows to receive this event.
 * \~ @code
 * l_eStatus = FrameworkunifiedAttachCallbackToDispatcher(happ, HAL_INPUT_SOURCE_NAME, HAL_INPUT_NOTIFY_STEERING, fpOnCmd)
 * @endcode
 * \~english note:FrameworkunifiedAttachCallbacksToDispatcher is available, too.
 */
#define HAL_INPUT_NOTIFY_STEERING         104
/**
 * \~english  Notify input event from tablet finger,
 *            The data format of the notification please refer to @ref EventsPackageInput.\n
 *            Please use IF of NSFW as follows to receive this event.
 * \~ @code
 * l_eStatus = FrameworkunifiedAttachCallbackToDispatcher(happ, HAL_INPUT_SOURCE_NAME, HAL_INPUT_NOTIFY_TABLET_FINGER, fpOnCmd)
 * @endcode
 * \~english note:FrameworkunifiedAttachCallbacksToDispatcher is available, too.
 */
#define HAL_INPUT_NOTIFY_TABLET_FINGER    105
/**
 * \~english  Notify input event from rotary-key,
 *            The data format of the notification please refer to @ref EventsPackageInput.\n
 *            Please use IF of NSFW as follows to receive this event.
 * \~ @code
 * l_eStatus = FrameworkunifiedAttachCallbackToDispatcher(happ, HAL_INPUT_SOURCE_NAME, HAL_INPUT_NOTIFY_ROTARY_KEY, fpOnCmd)
 * @endcode
 * \~english note:FrameworkunifiedAttachCallbacksToDispatcher is available, too.
 */
#define HAL_INPUT_NOTIFY_ROTARY_KEY    106

/**
 * \~english  Initialzing touch result notify,
 *            The data format of the notification please refer to @ref TouchInitFinishInput.\n
 *            Please use IF of NSFW as follows to receive this event.
 * \~ @code
 * l_eStatus = FrameworkunifiedAttachCallbackToDispatcher(happ, HAL_INPUT_SOURCE_NAME, HAL_INPUT_NOTIFY_TOUCH_INIT_FINISH, fpOnCmd)
 * @endcode
 * \~english note:FrameworkunifiedAttachCallbacksToDispatcher is available, too.
 */
#define HAL_INPUT_NOTIFY_TOUCH_INIT_FINISH   120

/**
 * @struct RadioInfoTouch
 * \~english @par Brief
 *          Radio information
 */
struct RadioInfoTouch {
    /**
     * \~english  Radio band type
     * \~english Please ref to @ref HalInputBandType
     */
    int16_t     band;
    /**
     * \~english  Frequency of the tuner[kHz]
     *      (Use only at the time of AM reception)
     */
    uint16_t    freq;
    /**
     * \~english  Reception status (Use only at the time of AM reception)
     * \~english Please refer to @ref HalInputBandType
     */
    int32_t     status;
};

/**
 * @struct SelftestTouch
 * \~english @par Brief
 *          Touch panel selftest mode result
 */
struct SelftestTouch {
    /**
     * \~english  Touch panel selftest mode result
     */
    unsigned char r_code;
    /**
     * \~english  Touch panel selftest mode data.
     *     The datail info is depend on hardware spec.
     */
    unsigned char r_data[HAL_INPUT_TOUCH_SELFTEST_DATA_LEN];
};

/**
 * @struct EventsPackageInput
 * \~english @par Brief
 *          Input event package defined
 */
struct EventsPackageInput {
  /**
   * \~english device type
   */
  int device_type;
  /**
   * \~english event count
   */
  int count;
  /**
   * \~english input event data.
   */
  struct input_event event[HAL_INPUT_EVENT_COUNT];
};

/**
 * @struct TouchInitFinishInput
 * \~english @par Brief
 *          The result of initializing touch
 */
struct TouchInitFinishInput {
  /**
   * \~english  If initializing the touch successful, the result was set HAL_INPUT_RET_NORMAL.
   *            If an error occurred, the result was set HAL_INPUT_RET_ERROR.
   */
  int   result;
};
/************************************************************************
*           Function prototype                                            *
************************************************************************/

////////////////////////////////////////////////////////////////////////////////
/// \ingroup input_hal
/// \~english @par Brief
///       initializing input_hal.
/// \~english @param [in] app_name
///       const char* - the app name for receive input event.
/// \~english @retval HAL_INPUT_RET_NORMAL : Success
/// \~english @retval HAL_INPUT_RET_ERROR : Process abnormality
/// \~english @par Prerequisite
///       - Have called @ref InitTouch()
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur
/// \~english @par Conditions of processing failure
///       - None
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync
/// \~english @par Detail
///       - Listen input devices and send input event.
///       - After initializing touch panel, the event(@ref HAL_INPUT_NOTIFY_TOUCH_INIT_FINISH) will be sent.
///       - The API can be used by 1 process.
/// \~english @see None.
////////////////////////////////////////////////////////////////////////////////
int InitInput(const char* app_name);

////////////////////////////////////////////////////////////////////////////////
/// \ingroup input_hal
/// \~english @par Brief
///       finalize input_hal
/// \~english @param none
/// \~english @retval HAL_INPUT_RET_NORMAL : Success
/// \~english @par Prerequisite
///       - None
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur
/// \~english @par Conditions of processing failure
///       - None
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync
/// \~english @par Detail
///       - This API is to finalize input_hal.
///       - The API can be used by 1 process.
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////
int DeInitInput();

////////////////////////////////////////////////////////////////////////////////
/// \ingroup input_hal
/// \~english @par Brief
///       Init those operating function of touch panel driver
/// \~english @param none
/// \~english @retval HAL_INPUT_RET_NORMAL : Success
/// \~english @par Prerequisite
///       - Touch panel function exist
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur
/// \~english @par Conditions of processing failure
///       - None
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync
/// \~english @par Detail
///       - The API is a block I/F which initializes those operating functions of touch panel driver.
///       - The API can be used by 1 process.
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////
int InitTouch();

////////////////////////////////////////////////////////////////////////////////
/// \ingroup input_hal
/// \~english @par Brief
///       Make touch panel start work
/// \~english @param none
/// \~english @retval HAL_INPUT_RET_NORMAL : Success
/// \~english @retval HAL_INPUT_RET_ERROR : Process abnormality
/// \~english @par Prerequisite
///       - Have called @ref InitTouch()
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur
/// \~english @par Conditions of processing failure
///       - Haven't called @ref InitTouch()
///         - @ref HAL_INPUT_RET_ERROR
///       - Inner io error
///         - @ref HAL_INPUT_RET_ERROR
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync
/// \~english @par Detail
///       - This API is to prepare touch panel and make it start work.
///       - The API can be used by 1 process.
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////
int StartTouch();

////////////////////////////////////////////////////////////////////////////////
/// \ingroup input_hal
/// \~english @par Brief
///       Execute touch panel self test
/// \~english @param [in] id
///        int    - selftest id(Pass HAL_INPUT_TOUCH_SELFTEST_ID_ALL :
///                 Execute all test(disconnection check))
/// \~english @param [out] result
///        void*  - Touch panel selftest mode check result
/// \~english @note Result struct SelftestTouch depend on hardware spec
/// \~english @retval HAL_INPUT_RET_NORMAL : Success
/// \~english @retval HAL_INPUT_RET_ERROR : Process abnormality
/// \~english @retval HAL_INPUT_RET_NOT_SUPPORT : Not support
/// \~english @par Prerequisite
///       - Have called @ref InitTouch()
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur
/// \~english @par Conditions of processing failure
///       - Haven't called @ref InitTouch()
///         - @ref HAL_INPUT_RET_ERROR
///       - Not support this function
///         - @ref HAL_INPUT_RET_NOT_SUPPORT
///       - Param result is NULL
///         - @ref HAL_INPUT_RET_ERROR
///       - Inner io error
///         - @ref HAL_INPUT_RET_ERROR
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync
/// \~english @par Detail
///       - This API is to execute touch panel self test and get test result.
///       - The API can be used by 1 process.
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////
int SelfTestTouch(int id, void *result);

////////////////////////////////////////////////////////////////////////////////
/// \ingroup input_hal
/// \~english @par Brief
///       Set whether the driver sends touch panel data or not.
/// \~english @param [in] status
///       int    - Touch panel whether report/unreport event\n
///              HAL_INPUT_TOUCH_REPORT : Report touch panel's touch event\n
///              HAL_INPUT_TOUCH_UNREPORT : Don't report touch panel's touch event\n
/// \~english @retval HAL_INPUT_RET_NORMAL : Success
/// \~english @retval HAL_INPUT_RET_ERROR : Process abnormality
/// \~english @retval HAL_INPUT_RET_NOT_SUPPORT : Not support
/// \~english @note It means this API isn't support when return
///                 @ref HAL_INPUT_RET_NOT_SUPPORT.
///                 The user need to implement it
///                 if don't need to report touch event.
/// \~english @par Prerequisite
///       - Have called @ref InitTouch()
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur
/// \~english @par Conditions of processing failure
///       - Haven't called @ref InitTouch()
///         - @ref HAL_INPUT_RET_ERROR
///       - Not support this function
///         - @ref HAL_INPUT_RET_NOT_SUPPORT
///       - Status is none of @ref HAL_INPUT_TOUCH_REPORT /
///         @ref HAL_INPUT_TOUCH_UNREPORT
///         - @ref HAL_INPUT_RET_ERROR
///       - Inner io error
///         - @ref HAL_INPUT_RET_ERROR
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync
/// \~english @par Detail
///       - This API is to set whether the driver sends touch panel data or not.
///       - The API can be used by 1 process.
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////
int LockTouch(int status);

////////////////////////////////////////////////////////////////////////////////
/// \ingroup input_hal
/// \~english @par Brief
///       Suspend touch panel
/// \~english @param none
/// \~english @retval HAL_INPUT_RET_NORMAL : Success
/// \~english @retval HAL_INPUT_RET_ERROR : Process abnormality
/// \~english @retval HAL_INPUT_RET_NOT_SUPPORT : Not support
/// \~english @note It means this API isn't support when return
///                 @ref HAL_INPUT_RET_NOT_SUPPORT.
///                 The user need to implement it
///                 if don't need to report touch event.
/// \~english @par Prerequisite
///       - Have called @ref InitTouch()
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur
/// \~english @par Conditions of processing failure
///       - Haven't called @ref InitTouch()
///         - @ref HAL_INPUT_RET_ERROR
///       - Not support this function
///         - @ref HAL_INPUT_RET_NOT_SUPPORT
///       - Inner io error
///         - @ref HAL_INPUT_RET_ERROR
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync
/// \~english @par Detail
///       - This API is to suspend touch panel.
///       - The API can be used by 1 process.
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////
int SuspendTouch();

////////////////////////////////////////////////////////////////////////////////
/// \ingroup input_hal
/// \~english @par Brief
///       Set touch panel sensitivity level
/// \~english @param [in] level
///       int    - Sensitivity level.
/// \~english @retval HAL_INPUT_RET_NORMAL : Success
/// \~english @retval HAL_INPUT_RET_ERROR : Process abnormality
/// \~english @retval HAL_INPUT_RET_NOT_SUPPORT : Not support
/// \~english @par Prerequisite
///       - Have called @ref InitTouch()
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur
/// \~english @par Conditions of processing failure
///       - Haven't called @ref InitTouch()
///         - @ref HAL_INPUT_RET_ERROR
///       - Not support this function
///         - @ref HAL_INPUT_RET_NOT_SUPPORT
///       - Sensitivity level is none of @ref HalInputTouchSensitivityLevel
///         - @ref HAL_INPUT_RET_ERROR
///       - Inner io error
///         - @ref HAL_INPUT_RET_ERROR
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync
/// \~english @par Detail
///       - This API is to set touch panel sensitivity level.
///       - The API can be used by 1 process.
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////
int SetSensitivityLevelTouch(int level);

////////////////////////////////////////////////////////////////////////////////
/// \ingroup input_hal
/// \~english @par Brief
///       Get touch panel sensitivity level
/// \~english @param [out] level
///       int*    - Sensitivity level.
/// \~english @retval HAL_INPUT_RET_NORMAL : Success
/// \~english @retval HAL_INPUT_RET_ERROR : Process abnormality
/// \~english @retval HAL_INPUT_RET_NOT_SUPPORT : Not support
/// \~english @par Prerequisite
///       - Have called @ref InitTouch()
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur
/// \~english @par Conditions of processing failure
///       - Haven't called @ref InitTouch()
///         - @ref HAL_INPUT_RET_ERROR
///       - Not support this function
///         - @ref HAL_INPUT_RET_NOT_SUPPORT
///       - Param level is NULL
///         - @ref HAL_INPUT_RET_ERROR
///       - Inner io error
///         - @ref HAL_INPUT_RET_ERROR
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync
/// \~english @par Detail
///       - This API is to get touch panel sensitivity level.
///       - The API can be used by 1 process.
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////
int GetSensitivityLevelTouch(int *level);

////////////////////////////////////////////////////////////////////////////////
/// \ingroup input_hal
/// \~english @par Brief
///       Notify radio scan frequency
/// \~english @param [in] info
///       RadioInfoTouch*    - SCAN frequence info
/// \~english @retval HAL_INPUT_RET_NORMAL : Success
/// \~english @retval HAL_INPUT_RET_ERROR : Process abnormality
/// \~english @retval HAL_INPUT_RET_NOT_SUPPORT : Not support
/// \~english @par Prerequisite
///       - Have called @ref InitTouch()
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur
/// \~english @par Conditions of processing failure
///       - Haven't called @ref InitTouch()
///         - @ref HAL_INPUT_RET_ERROR
///       - Not support this function
///         - @ref HAL_INPUT_RET_NOT_SUPPORT
///       - Param info is NULL
///         - @ref HAL_INPUT_RET_ERROR
///       - Inner io error
///         - @ref HAL_INPUT_RET_ERROR
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync
/// \~english @par Detail
///       - This API is to notify radio scan frequency.
///       - The API can be used by 1 process.
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////
int NotifyRadioScanFreqTouch(struct RadioInfoTouch *info);

////////////////////////////////////////////////////////////////////////////////
/// \ingroup input_hal
/// \~english @par Brief
///       Get panel resolution
/// \~english @param [in] reso_h
///       int*      - horizontal resolution
/// \~english @param [in] reso_v
///       int*      - vertical resolution
/// \~english @retval HAL_INPUT_RET_NORMAL : Success
/// \~english @retval HAL_INPUT_RET_ERROR : Process abnormality
/// \~english @par Prerequisite
///       - Have called @ref InitTouch()
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur
/// \~english @par Conditions of processing failure
///       - Param reso_h or reso_v is NULL
///         - @ref HAL_INPUT_RET_ERROR
///       - Inner io error
///         - @ref HAL_INPUT_RET_ERROR
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync
/// \~english @par Detail
///       - This API is to get horizontal and vertical resolution.
///       - The API can be used by 1 process.
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////
int GetPanelSpecResolutionInput(int *reso_h, int *reso_v);

/** @}*/  // end of input_hal
/** @}*/  // end of switchhandler

#endif  // HAL_API_INPUT_HAL_H_
