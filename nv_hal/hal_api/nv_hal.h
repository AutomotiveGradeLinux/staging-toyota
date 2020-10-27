/*
 * @copyright Copyright(c) 2018-2020 TOYOTA MOTOR CORPORATION.
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

#ifndef HAL_API_NV_HAL_H_
#define HAL_API_NV_HAL_H_

#include <stdint.h>
#include <native_service/frameworkunified_types.h>

/**
 * @file nv_hal.h
 * @brief This file provides API for accessing storage device.
 */

/** @addtogroup backup_manager
 *  @{
 */
/** @addtogroup nv_hal
 *  @ingroup backup_manager
 *  @{
 */

/**
 * \~english NvHalMedia
 */
enum NvHalMedia {
  /**
   * \~english CacheDram
   */
  NVHALMEDIA_CACHEDRAM = 0,
  /**
   * \~english BackupDram
   */
  NVHALMEDIA_BACKUPDRAM,
  /**
   * \~english nand
   */
  NVHALMEDIA_NAND,
  /**
   * \~english Max
   */
  NVHALMEDIA_MAX
};

/**\ingroup InitNv
 * \~english @par Brief
 *      Initialize Nv hal library.
 * \~english @retval eFrameworkunifiedStatusOK   : succsess initialize.
 * \~english @retval eFrameworkunifiedStatusFail : fail initialize
 * \~english @par Preconditions
 *      - There is no preconditions for this API.
 * \~english @par Change of internal state
 *      - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *      - Initialization processing failure.[eFrameworkunifiedStatusFail]
 * \~english @par Detail
 *      - Perform Nv_hal library initialization.
 *      - This process also performs deletion of temporary files accompanying system reset\n
 *        during write processing.
 *      - The API can only be used by 1 process.
 * \~english @par Classification
 *      Public
 * \~english @par Type
 *      Sync
 * \~english @see
 *      - None
 */
EFrameworkunifiedStatus InitNv(void);

/**\ingroup GetSizeNv
 * \~english @par Brief
 *      This API get data size which is stored in storage device.
 * \~english @param [in] media
 *      enum NvHalMedia           - Media type
 * \~english @param [in] filename
 *      const char*               - file name(Concatenated[category name/item name])
 * \~english @param [out] size
 *      uint32_t                  - Data size
 * \~english @par
 *      - Get decrypted data size as encryption data when item name in the argument of filename (file name) is added Prefix "ENC_".
 *       - Non encryption data : D_BK_CA_GID_D/D_BK_ID_GID_D
 *       - Encryption data     : D_BK_CA_GID_D/ENC_D_BK_ID_GID_D
 * \~english @retval eFrameworkunifiedStatusOK OK
 * \~english @retval eFrameworkunifiedStatusFileLoadError File not exists
 * \~english @retval eFrameworkunifiedStatusInvldParam Parameter error
 * \~english @par Preconditions
 *      - There is no preconditions for this API.
 * \~english @par Change of internal state
 *      - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *      - If the parameter media is less than NVHALMEDIA_CACHEDRAM or greater than\n
 *      NVHALMEDIA_NAND. [eFrameworkunifiedStatusInvldParam]
 *      - Parameter id is NULL. [eFrameworkunifiedStatusInvldParam]
 *      - Value of parameter size is 0. [eFrameworkunifiedStatusInvldParam]
 *      - Internal IO error(lstat).[eFrameworkunifiedStatusFileLoadError]
 * \~english @par Detail
 *      - This API get the data size according to specified media and ID.
 *      - The API can only be used by 1 process.
 * \~english @par Classification
 *      Public
 * \~english @par Type
 *      Sync
 * \~english @see WriteNv ReadNv
 */
EFrameworkunifiedStatus GetSizeNv(enum NvHalMedia media, const char *filename, uint32_t* size);



/**\ingroup ReadNv
 * \~english @par Brief
 *      Read data from storage device.
 * \~english @param [in] media
 *      enum NvHalMedia           - Media type
 * \~english @param [in] filename
 *      const char*               - file name(Concatenated[category name/item name])
 * \~english @param [out] buffer
 *      uint8_t*                  - Buffer for storing read data
 * \~english @param [in] size
 *      uint32_t                  - Data size
 * \~english @par
 *      - Decrypt and read data as encryption data when item name in the argument of filename (file name) is added Prefix "ENC_".
 *       - Non encryption data : D_BK_CA_GID_D/D_BK_ID_GID_D
 *       - Encryption data     : D_BK_CA_GID_D/ENC_D_BK_ID_GID_D
 * \~english @retval eFrameworkunifiedStatusOK OK
 * \~english @retval eFrameworkunifiedStatusInvldParam Parameter error
 * \~english @retval eFrameworkunifiedStatusFail Read data failed
 * \~english @par Preconditions
 *      - There is no preconditions for this API.
 * \~english @par Change of internal state
 *      - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *      - If the parameter media is less than NVHALMEDIA_CACHEDRAM or greater than\n
 *      NVHALMEDIA_NAND. [eFrameworkunifiedStatusInvldParam]
 *      - Parameter id is NULL. [eFrameworkunifiedStatusInvldParam]
 *      - Parameter buffer is NULL. [eFrameworkunifiedStatusInvldParam]
 *      - Value of parameter size is 0. [eFrameworkunifiedStatusInvldParam]
 *      - Internal IO error(open, pread, lstat, close).[eFrameworkunifiedStatusFail]
 *      - The value of parameter size is not equal to the file size specified by id.
 *      [eFrameworkunifiedStatusFail]
 * \~english @par Detail
 *      - This API reads data according to specified media and ID.
 *      - ID is managed by backup_manager.For details, refer to "Details about XML file" in external specification of backup_manager.
 *      - The API can only be used by 1 process.
 * \~english @par Classification
 *      Public
 * \~english @par Type
 *      Sync
 * \~english @see CheckNv WriteNv
 */
EFrameworkunifiedStatus ReadNv(enum NvHalMedia media, const char *filename, uint8_t *buffer, uint32_t size);

/**\ingroup WriteNv
 * \~english @par Brief
 *      Write data to storage device.
 * \~english @param [in] media
 *      enum NvHalMedia           - Media type
 * \~english @param [in] filename
 *      const char*               - filename(Concatenated[category name/item name])
 * \~english @param [in] buffer
 *      uint8_t*                  - Buffer for storing written data
 * \~english @param [in] size
 *      uint32_t                  - Data size
 * \~english @par
 *      - Encrypt and write data as encryption data when item name in the argument of filename (file name) is added Prefix "ENC_".
 *       - Non encryption data : D_BK_CA_GID_D/D_BK_ID_GID_D
 *       - Encryption data     : D_BK_CA_GID_D/ENC_D_BK_ID_GID_D
 * \~english @retval eFrameworkunifiedStatusOK OK
 * \~english @retval eFrameworkunifiedStatusInvldParam Parameter error
 * \~english @retval eFrameworkunifiedStatusFail Write data failed
 * \~english @par Preconditions
 *      - There is no preconditions for this API.
 * \~english @par Change of internal state
 *      - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *      - If the parameter media is less than NVHALMEDIA_CACHEDRAM or greater than\n
 *      NVHALMEDIA_NAND. [eFrameworkunifiedStatusInvldParam]
 *      - Parameter id is NULL. [eFrameworkunifiedStatusInvldParam]
 *      - Parameter buffer is NULL. [eFrameworkunifiedStatusInvldParam]
 *      - Value of parameter size is 0. [eFrameworkunifiedStatusInvldParam]
 *      - Internal IO error(open, pwrite, close, mkdir).[eFrameworkunifiedStatusFail]
 * \~english @par Detail
 *      - This API writes data according to specified media and ID.
 *      - filename is managed by backup_manager.For details, refer to "Details about XML file" in external specification of backup_manager.
 *      - The API can only be used by 1 process.
 * \~english @par Classification
 *      Public
 * \~english @par Type
 *      Sync
 * \~english @see CheckNv ReadNv
 */
EFrameworkunifiedStatus WriteNv(enum NvHalMedia media, const char *filename, uint8_t *buffer, uint32_t size);

/**\ingroup DeleteNv
 * \~english @par Brief
 *      Delete data which is stored in storage device.
 * \~english @param [in] media
 *      enum \ref NvHalMedia           - Media type
 * \~english @param [in] filename
 *      const char*               - filename(Concatenated[category name/item name])
 * \~english @par
 *      - Delete data as encryption data when item name in the argument of filename (file name) is added Prefix "ENC_".
 *       - Non encryption data : D_BK_CA_GID_D/D_BK_ID_GID_D
 *       - Encryption data     : D_BK_CA_GID_D/ENC_D_BK_ID_GID_D
 * \~english @retval eFrameworkunifiedStatusOK OK
 * \~english @retval eFrameworkunifiedStatusInvldParam Parameter error
 * \~english @retval eFrameworkunifiedStatusFail Delete data failed
 * \~english @par Preconditions
 *      - There is no preconditions for this API.
 * \~english @par Change of internal state
 *      - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *      - If the parameter media is less than \ref NVHALMEDIA_CACHEDRAM or greater than\n
 *        \ref NVHALMEDIA_NAND . [eFrameworkunifiedStatusInvldParam]
 *      - Parameter filename (filename) is NULL. [eFrameworkunifiedStatusInvldParam]
 *      - Internal IO error.[eFrameworkunifiedStatusFail]
 * \~english @par Detail
 *      - This API delete data according to specified media and filename.
 *      - This API delete encryption key if data is encrypted.
 *      - filename is managed by backup_manager.For details, refer to "Details about XML file" in external specification of backup_manager.
 *      - The API can only be used by 1 process.
 * \~english @par Classification
 *      Public
 * \~english @par Type
 *      Sync
 * \~english @see GetSizeNv ReadNv WriteNv
 */
EFrameworkunifiedStatus DeleteNv(enum NvHalMedia media, const char *filename);

/** @}*/  // end of nv_hal
/** @}*/  // end of backup_manager

#endif  // HAL_API_NV_HAL_H_
