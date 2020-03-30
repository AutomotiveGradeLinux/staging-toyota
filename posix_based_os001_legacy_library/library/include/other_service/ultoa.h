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

#ifndef OTHERSERVICE_POSIXBASEDOS001LEGACYLIBRARY_POSIXBASEDOS001_ULTOA_H_  // NOLINT(build/header_guard)
#define OTHERSERVICE_POSIXBASEDOS001LEGACYLIBRARY_POSIXBASEDOS001_ULTOA_H_  // NOLINT(build/header_guard)

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @file ultoa.h
 * @brief \~english This file provides ultoa API to convert unsigned long type to NULL terminated character string
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
/// \ingroup ultoa
/// \~english @par Brief
///     Convert unsigned long type to NULL terminated character string.
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
/// \~english @par Detail
///     None
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////
char *ultoa(unsigned long value, char *buf, int radix);  // NOLINT (readability/nolint)

/** @}*/  // end of posix_based_os001_legacy_library
/** @}*/  // end of other_service
/** @}*/  // end of BaseSystem
#ifdef __cplusplus
}
#endif

#endif  // OTHERSERVICE_POSIXBASEDOS001LEGACYLIBRARY_POSIXBASEDOS001_ULTOA_H_
