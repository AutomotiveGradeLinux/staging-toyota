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


#ifndef OTHERSERVICE_POSIXBASEDOS001LEGACYLIBRARY_POSIXBASEDOS001_CLOCK_CYCLE_API_H_  // NOLINT(build/header_guard)
#define OTHERSERVICE_POSIXBASEDOS001LEGACYLIBRARY_POSIXBASEDOS001_CLOCK_CYCLE_API_H_  // NOLINT(build/header_guard)

#include <stdint.h>

/******************************************************************************/
/* define */

/******************************************************************************/
/* typedef */

/******************************************************************************/
/* struct */


/******************************************************************************/
/* function */
#ifdef __cplusplus
extern "C" {
#endif
/**
 * @file PosixBasedOS001ClockCycleApi.h
 * @brief \~english This file provides ClockCycle API to get number of clock cycles.
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
/// \ingroup ClockCycle
/// \~english @par Brief
///     Get the number of clock cycles.
/// \~english @retval Microsecond
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
uint64_t ClockCycle(void);

/** @}*/  // end of posix_based_os001_legacy_library
/** @}*/  // end of other_service
/** @}*/  // end of BaseSystem
#ifdef __cplusplus
}
#endif

#endif  // OTHERSERVICE_POSIXBASEDOS001LEGACYLIBRARY_POSIXBASEDOS001_CLOCK_CYCLE_API_H_
