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

#ifndef OTHERSERVICE_POSIXBASEDOS001LEGACYLIBRARY_POSIXBASEDOS001_STRLCPY_H_  // NOLINT(build/header_guard)
#define OTHERSERVICE_POSIXBASEDOS001LEGACYLIBRARY_POSIXBASEDOS001_STRLCPY_H_  // NOLINT(build/header_guard)

#include <strings.h>

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @file strlcpy.h
 * @brief \~english This file provides API to copy string
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
/// \ingroup strlcpy
/// \~english @par Brief
///     Copy string
/// \~english @param [in/out] dst
///     char * - Destination string
/// \~english @param [in] src
///     const char * - Copy string
/// \~english @param [in] siz
///     size_t - Size of dst
/// \~english @retval Length of copied character string
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
///     Copy src to string dst of size siz.  At most siz-1 characters will be copied.
///     Always NUL terminates (unless siz == 0).
///     Returns strlen(src); if retval >= siz, truncation occurred.
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////
size_t strlcpy(char *dst, const char *src, size_t siz);  // NOLINT(readability/nolint)

/** @}*/  // end of posix_based_os001_legacy_library
/** @}*/  // end of other_service
/** @}*/  // end of BaseSystem
#ifdef __cplusplus
}
#endif

#endif  // OTHERSERVICE_POSIXBASEDOS001LEGACYLIBRARY_POSIXBASEDOS001_STRLCPY_H_
