/*
 * @copyright Copyright (c) 2016-2020 TOYOTA MOTOR CORPORATION.
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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  NS_UtilityCenter
/// \brief    This file contains declaration of APIs to calcuate 16-bit and 32-bit CRC checksum of file.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @file ns_util_crc.h
 */

#ifndef NS_NS_UTILITYCENTER_INC_NS_UTIL_CRC_H_  // NOLINT  (build/header_guard)
#define NS_NS_UTILITYCENTER_INC_NS_UTIL_CRC_H_

#include <native_service/frameworkunified_types.h>

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup native_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup framework_unified
 *  @ingroup native_service
 *  @{
 */
/** @addtogroup utility
 *  @ingroup framework_unified
 *  @{
 */

////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NS_UtilityCenter
/// \~english @par Brief
///       Calculate the CRC value of the file
/// \~english @param [in] f_c_file_name
///       PCSTR - the file to be calculated
/// \~english @param [out] f_ui_check_sum
///       UI_16 - the CRC value of the file
/// \~english @retval eFrameworkunifiedStatusOK : Success
/// \~english @retval eFrameworkunifiedStatusFileLoadError : File Load Error
/// \~english @retval eFrameworkunifiedStatusInvldParam : Invalid Param
///
/// \~english @par Prerequisite
///       - None
/// \~english @par Change of internal state
///       - None
/// \~english @par Conditions of processing failure
///       - Failed to open the file. [eFrameworkunifiedStatusFileLoadError]
///       - The pointer f_c_file_name is NULL. [eFrameworkunifiedStatusInvldParam]
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       The function calculates the CRC value(16 bit) of the file f_c_file_name.
/// \~english @see
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CalculateCRC16(PCSTR f_c_file_name, UI_16 & f_ui_check_sum);  // NOLINT (readability/nolint)

////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NS_UtilityCenter
/// \~english @par Brief
///       Calculate the CRC value of the file
/// \~english @param [in] f_c_file_name
///       PCSTR - the file to be calculated
/// \~english @param [out] f_ui_check_sum
///       UI_32 - the CRC value of the file
/// \~english @retval eFrameworkunifiedStatusOK : Success
/// \~english @retval eFrameworkunifiedStatusFileLoadError : File Load Error
/// \~english @retval eFrameworkunifiedStatusInvldParam : Invalid Param
///
/// \~english @par Prerequisite
///       - None
/// \~english @par Change of internal state
///       - None
/// \~english @par Conditions of processing failure
///       - Failed to open the file. [eFrameworkunifiedStatusFileLoadError]
///       - The pointer f_c_file_name is NULL. [eFrameworkunifiedStatusInvldParam]
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       The function calculates the CRC value(32 bit) of the file f_c_file_name.
/// \~english @see
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CalculateCRC32(PCSTR f_c_file_name, UI_32 & f_ui_check_sum);  // NOLINT (readability/nolint)

#endif  // NS_NS_UTILITYCENTER_INC_NS_UTIL_CRC_H_  // NOLINT  (build/header_guard)

/** @}*/  // end of utility
/** @}*/  // end of framework_unified
/** @}*/  // end of native_service
/** @}*/  // end of BaseSystem
