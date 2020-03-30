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
#ifndef _cl_sem_h_  // NOLINT(build/header_guard)
#define _cl_sem_h_  // NOLINT(build/header_guard)

#include <semaphore.h>

#ifdef __cplusplus
#define EXT_C extern "C"
#else
#define EXT_C
#endif  // __cplusplus

/**
 * @file cl_sem.h
 * @brief \~english This file contains the api of sem wait.
 */

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup native_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup common_library
 *  @ingroup native_service
 *  @{
 */

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup Sem
/// \~english @par Brief
///      Sem wait
/// \~english @param [in] semid
///     sem_t * - the pointer to the union sem_t
/// \~english @param [in] timeout
///     unsigned int - the count of timeout
/// \~english @retval 0  Success
/// \~english @retval not 0 Error
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
EXT_C int CL_SemWait(sem_t *semid, unsigned int timeout);  // NOLINT(readability/nolint)

/** @}*/  // end of common_library
/** @}*/  // end of NativeService
/** @}*/  // end of BaseSystem
#endif  // #ifndef _cl_sem_h_  // NOLINT(build/header_guard)
