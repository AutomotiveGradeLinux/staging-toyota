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
/// \brief    This file contains declaration of common APIs for NS_UtilityCenter.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @file ns_utility_if_internal.h
 */

#ifndef FRAMEWORK_UNIFIED_CLIENT_NS_UTILITYCENTER_INCLUDE_NS_UTILITY_IF_INTERNAL_H_
#define FRAMEWORK_UNIFIED_CLIENT_NS_UTILITYCENTER_INCLUDE_NS_UTILITY_IF_INTERNAL_H_

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

////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NS_UtilityCenter
/// \~english @par Brief
///       Get the number of leading zeros in a given 16bit value.
/// \~english @param [in] f_ui_val
///       UI_16 - the value to be counted
/// \~english @retval the number of leading zeros
///
/// \~english @par Prerequisite
///       - None
/// \~english @par Change of internal state
///       - None
/// \~english @par Conditions of processing failure
///       - None
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       The function returns the number of leading zeros in a given 16bit value.
/// \~english @see NS_CountLeadingZeros_32Bit NS_CountLeadingZeros_64Bit
////////////////////////////////////////////////////////////////////////////////////////////////
SI_16 NS_CountLeadingZeros_16Bit(UI_16 f_ui_val);  // NOLINT (readability/nolint)

////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NS_UtilityCenter
/// \~english @par Brief
///       Get the number of leading zeros in a given 32bit value.
/// \~english @param [in] f_ui_val
///       UI_32 - the value to be counted
/// \~english @retval the number of leading zeros
///
/// \~english @par Prerequisite
///       - None
/// \~english @par Change of internal state
///       - None
/// \~english @par Conditions of processing failure
///       - None
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       The function returns the number of leading zeros in a given 32bit value.
/// \~english @see NS_CountLeadingZeros_16Bit NS_CountLeadingZeros_64Bit
////////////////////////////////////////////////////////////////////////////////////////////////
SI_16 NS_CountLeadingZeros_32Bit(UI_32 f_ui_val);  // NOLINT (readability/nolint)

////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NS_UtilityCenter
/// \~english @par Brief
///       Get the number of leading zeros in a given 64bit value.
/// \~english @param [in] f_ui_val
///       UI_64 - the value to be counted
/// \~english @retval the number of leading zeros
///
/// \~english @par Prerequisite
///       - None
/// \~english @par Change of internal state
///       - None
/// \~english @par Conditions of processing failure
///       - None
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       The function returns the number of leading zeros in a given 64bit value.
/// \~english @see NS_CountLeadingZeros_16Bit NS_CountLeadingZeros_32Bit
////////////////////////////////////////////////////////////////////////////////////////////////
SI_16 NS_CountLeadingZeros_64Bit(UI_64 f_ui_val);  // NOLINT (readability/nolint)

////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NS_UtilityCenter
/// \~english @par Brief
///       Get the number of trailing zeros in a given 16bit value.
/// \~english @param [in] f_ui_val
///       UI_16 - the value to be counted
/// \~english @retval the number of trailing zeros
///
/// \~english @par Prerequisite
///       - None
/// \~english @par Change of internal state
///       - None
/// \~english @par Conditions of processing failure
///       - None
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       The function returns the number of trailing zeros in a given 16bit value.
/// \~english @see NS_CountTrailingZeros_32Bit NS_CountTrailingZeros_64Bit
////////////////////////////////////////////////////////////////////////////////////////////////
SI_16 NS_CountTrailingZeros_16Bit(UI_16 f_ui_val);  // NOLINT (readability/nolint)

////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NS_UtilityCenter
/// \~english @par Brief
///       Get the number of trailing zeros in a given 32bit value.
/// \~english @param [in] f_ui_val
///       UI_32 - the value to be counted
/// \~english @retval the number of trailing zeros
///
/// \~english @par Prerequisite
///       - None
/// \~english @par Change of internal state
///       - None
/// \~english @par Conditions of processing failure
///       - None
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       The function returns the number of trailing zeros in a given 32bit value.
/// \~english @see NS_CountTrailingZeros_16Bit NS_CountTrailingZeros_64Bit
////////////////////////////////////////////////////////////////////////////////////////////////
SI_16 NS_CountTrailingZeros_32Bit(UI_32 f_ui_val);  // NOLINT (readability/nolint)

////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NS_UtilityCenter
/// \~english @par Brief
///       Get the number of trailing zeros in a given 64bit value.
/// \~english @param [in] f_ui_val
///       UI_64 - the value to be counted
/// \~english @retval the number of trailing zeros
///
/// \~english @par Prerequisite
///       - None
/// \~english @par Change of internal state
///       - None
/// \~english @par Conditions of processing failure
///       - None
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       The function returns the number of trailing zeros in a given 64bit value.
/// \~english @see NS_CountTrailingZeros_16Bit NS_CountTrailingZeros_32Bit
////////////////////////////////////////////////////////////////////////////////////////////////
SI_16 NS_CountTrailingZeros_64Bit(UI_64 f_ui_val);  // NOLINT (readability/nolint)

/** @}*/  // end of framework_unified

/** @}*/  // end of native_service

/** @}*/  // end of BaseSystem

#endif  // FRAMEWORK_UNIFIED_CLIENT_NS_UTILITYCENTER_INCLUDE_NS_UTILITY_IF_INTERNAL_H_
