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
 * @file ss_access_if_private.h
 * @brief \~english This file contains declaration of structures RAM_AccessPrivate and ROM_AccessPrivate
 */
/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup system_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup config
 *  @ingroup system_service
 *  @{
 */
#ifndef CONFIG_LIBRARY_SYSTEM_MANAGER_CONFIG_INCLUDE_SYSTEM_SERVICE_SS_ACCESS_IF_PRIVATE_H_
#define CONFIG_LIBRARY_SYSTEM_MANAGER_CONFIG_INCLUDE_SYSTEM_SERVICE_SS_ACCESS_IF_PRIVATE_H_

#include <system_service/ss_sm_rom_access.h>
#include <system_service/ss_sm_ram_access.h>

//  Should be design the data so that the default value is 0.

//  Product private area for DRAM BACKUP

/**
 * @struct RAM_AccessPrivate
 * @brief \~english RAM Access Private data define
 */
typedef struct {
  union {
    //! \~english _CWORD80_ error define
    struct {
      /*
       *  Note.
       *  This feature needs to be defined depending on the product specification by the vendor.
       */
    };
    //! \~english RAM product private max
    uint8_t max[RAM_PRODUCT_PRIVATE_MAX];
  };
} RAM_AccessPrivate;

/**
 * @struct ROM_AccessPrivate
 * @brief \~english ROM Access Private data define
 */
typedef struct {
  union {
    //  Since data is accessed with 32 bit width at the low level, data must be allocated using 32 bits algin
    struct {
      /*
       *  Note.
       *  This feature needs to be defined depending on the product specification by the vendor.
       */
    };
    //! \~english ROM product private max
    uint8_t max[ROM_PRODUCT_PRIVATE_MAX];
  };
} ROM_AccessPrivate;

#endif  // CONFIG_LIBRARY_SYSTEM_MANAGER_CONFIG_INCLUDE_SYSTEM_SERVICE_SS_ACCESS_IF_PRIVATE_H_
/** @}*/
/** @}*/
/** @}*/
