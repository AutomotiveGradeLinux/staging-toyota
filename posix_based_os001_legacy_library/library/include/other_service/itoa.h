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

#ifndef OTHERSERVICE_POSIXBASEDOS001LEGACYLIBRARY_POSIXBASEDOS001_ITOA_H_  // NOLINT(build/header_guard)
#define OTHERSERVICE_POSIXBASEDOS001LEGACYLIBRARY_POSIXBASEDOS001_ITOA_H_  // NOLINT(build/header_guard)

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @file itoa.h
 * @brief \~english This file provides itoa API to convert integer value to null terminated string
 */

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup other_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup posix_based_os001_legacy_library
 *  @ingroup other_service
 *  @{
 */

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup itoa
/// \~english @par Brief
///     Integer value to null terminated string.
/// \~english @param [in] value
///     int - value Number to convert
/// \~english @param [out] buff
///     char - buff Result string
/// \~english @param [in] radix
///     int - radix Radix of value
/// \~english @retval String pointer
/// \~english @par Prerequisite
///     None
/// \~english @par Change of internal state
///     None
/// \~english @par Conditions of processing failure
///     None
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync
/// \~english @par Detail
///     None
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////
char* itoa(int value, char buff[], int radix);  // NOLINT(readability/nolint)

/** @}*/  // end of posix_based_os001_legacy_library
/** @}*/  // end of other_service
/** @}*/  // end of BaseSystem
#ifdef __cplusplus
}
#endif

#endif  // OTHERSERVICE_POSIXBASEDOS001LEGACYLIBRARY_POSIXBASEDOS001_ITOA_H_
