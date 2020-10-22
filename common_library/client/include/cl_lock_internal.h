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

#ifndef _cl_lock_internal_h_  // NOLINT(build/header_guard)
#define _cl_lock_internal_h_  // NOLINT(build/header_guard)

#include <native_service/cl_lockid.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file cl_lock_internal.h
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

#define LOCKFILE_NAME "lockfile"  // lock file name
#define SLOT_SIZE   4096  // slot size
#define LOCKFILE_SIZE SLOT_SIZE * LID_NUM  // the lock file size

#define LOCK_TIMEOUT    3   // timeout(sec) for CL_LockTimedGet

/** @}*/  // end of common_library
/** @}*/  // end of NativeService
/** @}*/  // end of BaseSystem
#ifdef __cplusplus
}
#endif

#endif  // #ifndef _cl_lock_internal_h_  // NOLINT(build/header_guard)
