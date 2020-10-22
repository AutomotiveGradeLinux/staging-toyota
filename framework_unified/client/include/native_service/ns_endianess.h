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

/**
 * @file ns_endianess.h
 * @brief \~english Provide api for convert value
 */

#ifndef NS_NATIVESERVICES_INC_UTILITY_NS_ENDIANESS_H_  // NOLINT  (build/header_guard)
#define NS_NATIVESERVICES_INC_UTILITY_NS_ENDIANESS_H_

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
///       Convert the UI_16(unsigned 16bit) value to big-endian.
/// \~english @param [in] f_value
///       UI_16 - The value to be converted
/// \~english @retval the big-endian value of the input param f_value
///
/// \~english @par Prerequisite
///       - NULL
/// \~english @par Change of internal state
///       - NULL
/// \~english @par Conditions of processing failure
///       - NULL
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       The function converts the UI_16(unsigned 16bit) value to big-endian.
/// \~english @see
////////////////////////////////////////////////////////////////////////////////////////////////
UI_16 ConvertEndian_UI16(UI_16 f_value);  // NOLINT (readability/nolint)

////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NS_UtilityCenter
/// \~english @par Brief
///       Convert the SI_16(signed 16bit) value to big-endian.
/// \~english @param [in] f_value
///       SI_16 - The value to be converted
/// \~english @retval the big-endian value of the input param f_value
///
/// \~english @par Prerequisite
///       - NULL
/// \~english @par Change of internal state
///       - NULL
/// \~english @par Conditions of processing failure
///       - NULL
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       The function converts the SI_16(signed 16bit) value to big-endian.
/// \~english @see
////////////////////////////////////////////////////////////////////////////////////////////////
SI_16 ConvertEndian_SI16(SI_16 f_value);  // NOLINT (readability/nolint)

////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NS_UtilityCenter
/// \~english @par Brief
///        Convert the UI_32(unsigned 32bit) value to big-endian.
/// \~english @param [in] f_value
///       UI_32 - The value to be converted
/// \~english @retval the big-endian value of the input param f_value
///
/// \~english @par Prerequisite
///       - NULL
/// \~english @par Change of internal state
///       - NULL
/// \~english @par Conditions of processing failure
///       - NULL
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///        The function convers the UI_32(unsigned 32bit) value to big-endian.
/// \~english @see
////////////////////////////////////////////////////////////////////////////////////////////////
UI_32 ConvertEndian_UI32(UI_32 f_value);  // NOLINT (readability/nolint)

////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NS_UtilityCenter
/// \~english @par Brief
///       Convert the SI_32(signed 32bit) value to big-endian.
/// \~english @param [in] f_value
///       SI_32 - The value to be converted
/// \~english @retval the big-endian value of the input param f_value
///
/// \~english @par Prerequisite
///       - NULL
/// \~english @par Change of internal state
///       - NULL
/// \~english @par Conditions of processing failure
///       - NULL
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///        The function convers the SI_32(signed 32bit) value to big-endian.
/// \~english @see
////////////////////////////////////////////////////////////////////////////////////////////////
SI_32 ConvertEndian_SI32(SI_32 f_value);  // NOLINT (readability/nolint)

////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NS_UtilityCenter
/// \~english @par Brief
///       Convert the UI_64(unsigned 64bit) value to big-endian.
/// \~english @param [in] f_value
///       UI_64 - The value to be converted
/// \~english @retval the big-endian value of the input param f_value
///
/// \~english @par Prerequisite
///       - NULL
/// \~english @par Change of internal state
///       - NULL
/// \~english @par Conditions of processing failure
///       - NULL
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///        The function convers the UI_64(unsigned 64bit) value to big-endian.
/// \~english @see
////////////////////////////////////////////////////////////////////////////////////////////////
UI_64 ConvertEndian_UI64(UI_64 f_value);  // NOLINT (readability/nolint)

////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NS_UtilityCenter
/// \~english @par Brief
///       Convert the SI_64(signed 64bit) value to big-endian.
/// \~english @param [in] f_value
///       SI_64 - The value to be converted
/// \~english @retval the big-endian value of the input param f_value
///
/// \~english @par Prerequisite
///       - NULL
/// \~english @par Change of internal state
///       - NULL
/// \~english @par Conditions of processing failure
///       - NULL
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///        The function convers the SI_64(signed 64bit) value to big-endian.
/// \~english @see
////////////////////////////////////////////////////////////////////////////////////////////////
SI_64 ConvertEndian_SI64(SI_64 f_value);  // NOLINT (readability/nolint)

#endif  // NS_NATIVESERVICES_INC_UTILITY_NS_ENDIANESS_H_  // NOLINT  (build/header_guard)

/** @}*/  // end of utility
/** @}*/  // end of framework_unified
/** @}*/  // end of native_service
/** @}*/  // end of BaseSystem
