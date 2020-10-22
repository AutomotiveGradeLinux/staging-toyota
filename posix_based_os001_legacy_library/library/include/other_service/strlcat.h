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

#ifndef OTHERSERVICE_POSIXBASEDOS001LEGACYLIBRARY_POSIXBASEDOS001_STRLCAT_H_  // NOLINT(build/header_guard)
#define OTHERSERVICE_POSIXBASEDOS001LEGACYLIBRARY_POSIXBASEDOS001_STRLCAT_H_  // NOLINT(build/header_guard)

#include <strings.h>

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @file strlcat.h
 * @brief \~english This file provides strlcat API to concatenate strings
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
/// \ingroup strlcat
/// \~english @par Brief
///     Concatenate strings
/// \~english @param [in/out] dst
///     char * - Destination string
/// \~english @param [in] src
///     const char * - Concatenated string
/// \~english @param [in] siz
///     size_t - Size of dst
/// \~english @retval String length after concatenation
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
///     Appends src to string dst of size siz (unlike strncat, siz is the full size of dst, not space left).
///     At most siz-1 characters will be copied. Always NUL terminates (unless siz <= strlen(dst)).
///     Returns strlen(src) + MIN(siz, strlen(initial dst)). If retval >= siz, truncation occurred.
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////
size_t strlcat(char *dst, const char *src, size_t siz);  // NOLINT(readability/nolint)

/** @}*/  // end of posix_based_os001_legacy_library
/** @}*/  // end of other_service
/** @}*/  // end of BaseSystem
#ifdef __cplusplus
}
#endif

#endif  // OTHERSERVICE_POSIXBASEDOS001LEGACYLIBRARY_POSIXBASEDOS001_STRLCAT_H_
