/*
 * @copyright Copyright (c) 2016-2019 TOYOTA MOTOR CORPORATION.
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
 * @file nor_backup.h
 * @brief This file provides API for get nor flash information
 */

#ifndef ROM_ACCESS_LIBRARY_LIBRARY_INCLUDE_SYSTEM_SERVICE_NOR_BACKUP_H_
#define ROM_ACCESS_LIBRARY_LIBRARY_INCLUDE_SYSTEM_SERVICE_NOR_BACKUP_H_

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup system_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup rom_access_library
 *  @ingroup system_service
 *  @{
 */
#if defined(__cplusplus)
extern "C" {
#endif  // __cplusplus

/**
 * \~english Common return value: Normal return value of API
 */
#define RET_DEV_NORMAL          0

/**
 * \~english Common return value: Parameter error return value of API
 */
#define RET_DEV_ERR_PARAM       3

/**
 * \~english Common return value: Error of getting device info
 */
#define RET_DEV_ERROR           4

/**
 * \~english Read API return value: Internal error
 */
#define RET_DEV_RD_ERR_OTHER    1

/**
 * \~english Read API return value: Acquire buffer error(malloc)
 */
#define RET_DEV_RD_ERR          2

/**
 * \~english Read API return value: error type 1
 */
#define RET_DEV_RD_ERR_1        -1

/**
 * \~english Read API return value: error type 2
 */
#define RET_DEV_RD_ERR_2        -2

/**
 * \~english Read API return value: error type 3
 */
#define RET_DEV_RD_ERR_3        -3

/**
 * \~english Read API return value: error type 4
 */
#define RET_DEV_RD_ERR_4        -4

/**
 * \~english Write API return value: Other error
 */
#define RET_DEV_WT_ERR_OTHER    1

/**
 * \~english Write API return value: Internal error
 */
#define RET_DEV_WT_ERR          2

/**\ingroup mtdn_backup_Read
 * \~english @par Brief
 *      Read data from norflash device.
 * \~english @param [in] path_name
 *      const char*           - Device path to read(such as "/dev/mtd1ro")
 * \~english @param [in] i_id
 *      int                   - Main block ID(0, 2, 4, ...)
 * \~english @param [in] i_offset
 *      int                   - Data offset
 * \~english @param [in] i_size
 *      int                   - Data size to read
 * \~english @param [in] p_buff
 *      Void*                 - Buffer for storing read data
 * \~english @retval RET_DEV_NORMAL OK
 * \~english @retval RET_DEV_ERROR Get device error
 * \~english @retval RET_DEV_ERR_PARAM Parameter error
 * \~english @retval RET_DEV_RD_ERR_OTHER Internal error
 * \~english @retval RET_DEV_RD_ERR Acquire buffer error(malloc)
 * \~english @retval RET_DEV_RD_ERR_1 Error type 1
 * \~english @retval RET_DEV_RD_ERR_2 Error type 2
 * \~english @retval RET_DEV_RD_ERR_3 Error type 3
 * \~english @retval RET_DEV_RD_ERR_4 Error type 4
 * \~english @par Preconditions
 *      - There is no preconditions for this API.
 * \~english @par Change of internal state
 *      - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *      - Parameter path_name is NULL. [RET_DEV_ERR_PARAM]
 *      - Parameter p_buff is NULL. [RET_DEV_ERR_PARAM]
 *      - Value of parameter i_id is less than 0. [RET_DEV_ERR_PARAM]
 *      - Value of parameter i_offset is less than 0 or value of parameter i_size
 *        is not greater than 0. [RET_DEV_ERR_PARAM]
 *      - Device specified by path_name is invalid. [RET_DEV_ERR_PARAM]
 *      - Sum value of i_offset and i_size is greater than block size - 16byte.
 *        [RET_DEV_ERR_PARAM]
 *      - Failed to get device info. [RET_DEV_ERROR]
 *      - Failed to acquire the send/receive buffer(malloc). [RET_DEV_RD_ERR]
 *      - Internal IO error(open, read, close, ioctl, lseek, other error).
 *        [RET_DEV_RD_ERR_OTHER]
 *      - Check text error of main block and backup block. [RET_DEV_RD_ERR_1]
 *      - Check text error of main block and checksum error of backup block.
 *        [RET_DEV_RD_ERR_2]
 *      - Checksum error of main block and check text error of backup block.
 *        [RET_DEV_RD_ERR_3]
 *      - Checksum error of main block and backup block. [RET_DEV_RD_ERR_4]
 * \~english @par Detail
 *      - This API gets data from main or backup block according to specified\n
 *      norflash device pathname and main block ID.
 * \~english @par
 *      - When this API is executed, read data from main block first. If failed to\n
 *      read main block, return RET_DEV_RD_ERR_OTHER. If read main block success,\n
 *      but check main block data error, turn to read data from backup block which\n
 *      block ID is i_id + 1.
 * \~english @par Classification
 *      Public
 * \~english @par Type
 *      Sync Only
 * \~english @see mtdn_backup_Write
 */
int mtdn_backup_Read(const char * path_name, int i_id, int i_offset, int i_size, void *p_buff);

/**\ingroup mtdn_backup_Write
 * \~english @par Brief
 *      Write data to norflash device.
 * \~english @param [in] path_name
 *      const char*           - Device path to write(such as "/dev/mtd")
 * \~english @param [in] i_id
 *      int                   - Main block ID(0, 2, 4, ...)
 * \~english @param [in] i_offset
 *      int                   - Data offset
 * \~english @param [in] i_size
 *      int                   - Data size to write
 * \~english @param [in] p_buff
 *      Void*                 - Buffer for storing written data
 * \~english @retval RET_DEV_NORMAL OK
 * \~english @retval RET_DEV_ERROR Get device error
 * \~english @retval RET_DEV_ERR_PARAM Parameter error
 * \~english @retval RET_DEV_WT_ERR_OTHER Other error
 * \~english @retval RET_DEV_WT_ERR Internal error
 * \~english @par Preconditions
 *      - There is no preconditions for this API.
 * \~english @par Change of internal state
 *      - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *      - Parameter path_name is NULL. [RET_DEV_ERR_PARAM]
 *      - Parameter p_buff is NULL. [RET_DEV_ERR_PARAM]
 *      - Value of parameter i_id is less than 0. [RET_DEV_ERR_PARAM]
 *      - Value of parameter i_offset is less than 0 or value of parameter i_size
 *        is not greater than 0. [RET_DEV_ERR_PARAM]
 *      - Device specified by path_name is invalid. [RET_DEV_ERR_PARAM]
 *      - Sum value of i_offset and i_size is greater than block size - 16byte.
 *        [RET_DEV_ERR_PARAM]
 *      - Failed to get device info. [RET_DEV_ERROR]
 *      - Internal processing error(Other error). [RET_DEV_WT_ERR_OTHER]
 *      - Internal IO error(open, write, close, ioctl, malloc, lseek).
 *        [RET_DEV_WT_ERR]
 * \~english @par Detail
 *      - This API writes data to main and backup blocks according to specified\n
 *      norflash device pathname and main block ID.
 * \~english @par
 *      - When this API is executed, p_buff data will be write to main and backup\n
 *      blocks. If failed to write any block, return RET_DEV_WT_ERR.
 * \~english @par Classification
 *      Public
 * \~english @par Type
 *      Sync Only
 * \~english @see mtdn_backup_Read
 */
int mtdn_backup_Write(const char * path_name, int i_id, int i_offset, int i_size, void *p_buff);

#if defined(__cplusplus)
}
#endif  // __cplusplus

/** @}*/  // end of rom_access_library
/** @}*/  // end of system_service
/** @}*/  // end of BaseSystem
#endif  // ROM_ACCESS_LIBRARY_LIBRARY_INCLUDE_SYSTEM_SERVICE_NOR_BACKUP_H_
