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

// libraries for replacing PosixBasedOS001 to Linux
// PosixBasedOS001TimeApi.h

#ifndef OTHERSERVICE_POSIXBASEDOS001LEGACYLIBRARY_POSIXBASEDOS001_TIMEAPI_H_  // NOLINT(build/header_guard)
#define OTHERSERVICE_POSIXBASEDOS001LEGACYLIBRARY_POSIXBASEDOS001_TIMEAPI_H_  // NOLINT(build/header_guard)

#include <sys/types.h>

/*******************************************************************************/
/* define */

/*******************************************************************************/
/* typedef */

/*******************************************************************************/
/* struct */

/*******************************************************************************/
/* function */
#ifdef __cplusplus
extern "C" {
#endif
/**
 * @file PosixBasedOS001TimeApi.h
 * @brief \~english This file provides delay API to delay for the specified time.
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
/// \ingroup delay
/// \~english @par Brief
///     Delay for the specified time.
/// \~english @param [in] duration
///     unsigned int - The number of milliseconds for which to suspend the calling thread from execution
/// \~english @retval EOK
/// \~english @par Prerequisite
///     None
/// \~english @par Change of internal state
///     None
/// \~english @par Conditions of processing failure
///     None
/// \~english @par Detail
///     If duration is more than 1000 milliseconds, return immediately.
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////
unsigned int delay(unsigned int duration);  // NOLINT(readability/nolint)

/** @}*/  // end of posix_based_os001_legacy_library
/** @}*/  // end of other_service
/** @}*/  // end of BaseSystem
#ifdef __cplusplus
}
#endif

#endif  // OTHERSERVICE_POSIXBASEDOS001LEGACYLIBRARY_POSIXBASEDOS001_TIMEAPI_H_
