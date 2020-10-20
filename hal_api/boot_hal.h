/*
 * @copyright Copyright (c) 2018-2020 TOYOTA MOTOR CORPORATION.
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

#ifndef HAL_API_BOOT_HAL_H_
#define HAL_API_BOOT_HAL_H_

#include <stdint.h>
#include <native_service/frameworkunified_types.h>

/**
 * @file boot_hal.h
 * @brief This file provides API for get boot info from boot device, set boot info to boot device,
 *        set next boot mode to coldstart and get current boot mode
 */

/** @addtogroup reproservice
 *  @{
 */
/** @addtogroup boot_hal
 *  @ingroup reprogramming_service
 *  @{
 */

/**
 * \~english BootHalBootMode
 */
enum BootHalBootMode{
  /**
   * \~english cold start
   */
  COLD_START = 0,
  /**
   * \~english hot start
   */
  HOT_START = 1,
};

/**
 * \~english BootHalUpTableID
 */
enum BootHalUpTableID{
  /**
   * \~english UBOOT
   */
  BOOTHAL_SIDE_IDX_UBOOT = 0,
  /**
   * \~english Secure loader
   */
  BOOTHAL_SIDE_IDX_SECURE = 1,
  /**
   * \~english MAIN software
   */
  BOOTHAL_SIDE_IDX_MAINSOFT = 2,
  /**
   * \~english Max num of modules(include reserves)
   */
  BOOTHAL_NUMTARGS = 20
};

/**
 * \~english BootHalStatus
 */
typedef enum BootHalStatus {
/**
 * \~english Success
 */
  BOOTHAL_RET_SUCCESS = 0,
/**
 * \~english No permission to operate
 */
  BOOTHAL_RET_ERR_PERMISSION = 1,
/**
 * \~english Time out
 */
  BOOTHAL_RET_ERR_TIMEOUT = 2,
/**
 * \~english Status error
 */
  BOOTHAL_RET_ERR_STATUS = 3,
/**
 * \~english Write error
 */
  BOOTHAL_RET_ERR_WRITE = 4,
/**
 * \~english Paramater error
 */
  BOOTHAL_RET_ERR_PARAM = 5,
/**
 * \~english Internal IO error(open, read, write, strstr, close)
 */
  BOOTHAL_RET_ERR_FAIL = 6,
} BootHalStatus;

/**
 * \~english Not rewrite.
 */
#define BOOTHAL_SIDE_NONE 0
/**
 * \~english Rewrites side A.
 */
#define BOOTHAL_SIDE_A 1
/**
 * \~english Rewrites side B.
 */
#define BOOTHAL_SIDE_B 2
/**
 * \~english Max parameter num
 */
#define BOOTHAL_NUMSIDE_MAX 3

/**\ingroup setSide
 * \~english @par Brief
 *      This API set boot info to boot device, the caller control this API not be called at the
 *      same time
 * \~english @param [in] upTableID
 *      unsigned int           - Boot info
 * \~english @retval BOOTHAL_RET_SUCCESS OK
 * \~english @retval BOOTHAL_RET_ERR_PARAM Parameter error
 * \~english @retval BOOTHAL_RET_ERR_WRITE Write error
 * \~english @retval BOOTHAL_RET_ERR_FAIL Internal IO error
 * \~english @par Preconditions
 *      - There is no preconditions for this API.
 * \~english @par Change of internal state
 *      - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *      - If the parameter upTableId is NULL. [BOOTHAL_RET_ERR_PARAM]
 *      - If write /dev/mmcblk0boot1 return -1. [BOOTHAL_RET_ERR_WRITE]
 *      - Internal IO error(open, read, lseek). [BOOTHAL_RET_ERR_FAIL]
 * \~english @par Detail
 *      - This API set boot info.
 *      - The API can only be used by 1 process.
 * \~english @par Classification
 *      Public
 * \~english @par Type
 *      Sync
 * \~english @see getSide
 */
BootHalStatus setSide(unsigned int upTableID[]);

/**\ingroup getSide
 * \~english @par Brief
 *      Get boot info from boot device
 * \~english @param [out] upTableID
 *      unsigned int              - boot info
 * \~english @retval BOOTHAL_RET_SUCCESS OK
 * \~english @retval BOOTHAL_RET_ERR_PARAM Parameter error
 * \~english @retval BOOTHAL_RET_ERR_FAIL Internal IO error
 * \~english @par Preconditions
 *      - There is no preconditions for this API.
 * \~english @par Change of internal state
 *      - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *      - If the parameter upTableId is NULL. [BOOTHAL_RET_ERR_PARAM]
 *      - Internal IO error.[BOOTHAL_RET_ERR_FAIL]
 * \~english @par Detail
 *      - This API get boot info from boot device.
 *      - The API can only be used by 1 process.
 * \~english @par Classification
 *      Public
 * \~english @par Type
 *      Sync
 * \~english @see setSide
 */
BootHalStatus getSide(unsigned int upTableID[]);

/**\ingroup setBootColdStart
 * \~english @par Brief
 *     Set next boot to ColdStart mode.
 * \~english @retval BOOTHAL_RET_SUCCESS OK
 * \~english @retval BOOTHAL_RET_ERR_FAIL set next boot start mode failed
 * \~english @par Preconditions
 *      - There is no preconditions for this API.
 * \~english @par Change of internal state
 *      - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *      - Internal IO error.[BOOTHAL_RET_ERR_FAIL]
 * \~english @par Detail
 *      - This API Set next boot to ColdStart mode.
 *      - The API can only be used by 1 process.
 * \~english @par Classification
 *      Public
 * \~english @par Type
 *      Sync
 * \~english @see getBootMode
 */
BootHalStatus setBootColdStart(void);

/**\ingroup getBootMode
 * \~english @par Brief
 *      Get boot mode.
 * \~english @param [out] mode
 *      unsigned int              - boot mode(COLD_START or HOT_START)
 * \~english @retval BOOTHAL_RET_SUCCESS OK
 * \~english @retval BOOTHAL_RET_ERR_PARAM Parameter error
 * \~english @par Preconditions
 *      - There is no preconditions for this API.
 * \~english @par Change of internal state
 *      - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *      - If the parameter mode is NULL. [BOOTHAL_RET_ERR_PARAM]
 * \~english @par Detail
 *      - This API get boot mode.
 *      - The API can only be used by 1 process.
 * \~english @par Classification
 *      Public
 * \~english @par Type
 *      Sync
 * \~english @see setBootColdStart
 */
BootHalStatus getBootMode(uint32_t *mode);

/** @}*/  // end of boot_hal
/** @}*/  // end of reprogramming_service

#endif  // HAL_API_BOOT_HAL_H_
