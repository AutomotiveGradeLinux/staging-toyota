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
#ifndef _cl_lock_h_  // NOLINT(build/header_guard)
#define _cl_lock_h_  // NOLINT(build/header_guard)

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file cl_lock.h
 * @brief \~english This file contains the base api of cl_clock.
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

////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CL_LockSystemInit
/// \~english @par Brief
///        Initialize the system lock.
/// \~english @retval 0  Success
/// \~english @retval -1 Error
/// \~english @par Prerequisite
///        - Prerequisites are nothing.
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - On system call shm_open error, -1 is returned.
///        - On system call ftrancate error, -1 is returned.
///        - On system call mmap error, -1 is returned.
///        - On system call pthread_mutexattr_setpshared error, -1 is returned.
/// \~english @par Detail
///        This function will generate the Lock file, and initialize all the pthread_mutex_t slots.\n
///        This function must to be called in the system once, which is recommended on the SystemManager start.\n
/// \~english @par
///        Lock file is formed with lots of slots. Every slot occupies 4KB.
///        The slot layout is:
///        0     ~ 4Byte  : field of PID
///        4Byte ~ 28Byte : field of pthread_mutex_t
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Sync only
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////
int32_t CL_LockSystemInit(void);  // NOLINT(readability/nolint)

////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CL_LockProcessInit
/// \~english @par Brief
///        Initialize the process Lock file.
/// \~english @retval 0  Success
/// \~english @retval -1 Error
/// \~english @par Prerequisite
///        Should call CL_LockSystemInit() to generate Lock file.
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - On system call shm_open error, -1 is returned.
/// \~english @par Detail
///        Open the Lock file.\n
///        If process want to use the Lock, this function should be called.\n
///        It is recommended that this function should be called since start main function.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Sync only
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////
int32_t CL_LockProcessInit(void);  // NOLINT(readability/nolint)

////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CL_LockMap
/// \~english @par Brief
///        Mapping the Lock information address.
/// \~english @param [in] lid
///        int32_t - LockID of the Lock used.(0~LID_NUM) \n
/// \~english @retval addr Lock address
/// \~english @retval MAP_FAILED Error (errno)
/// \~english @par Prerequisite
///        Should call CL_LockSystemInit(), CL_LockProcessInit() to open the Lock file.
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - If LockID(lid) < 0, errno is set as EINVAL, and MAP_FAILED is returned
///        - If LockID(lid) > LID_NUM, errno is set as EINVAL, and MAP_FAILED is returned
///        - If systemcall mmap is failure, errno is set euqal with the errno of mmap, and MAP_FAILED is returned
/// \~english @par Detail
///        Mapping the Lock information to the memory.\n
///        The related LockID should be assigned, when apply the share memory.\n
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Sync only
/// \~english @see
///        CL_LockUnmap
////////////////////////////////////////////////////////////////////////////////////
void *CL_LockMap(int32_t lid);  // NOLINT(readability/nolint)

////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CL_LockUnmap
/// \~english @par Brief
///        Unmapping the Lock information.
/// \~english @param [in] addr
///        void* - Lock information address.
/// \~english @retval 0  Success
/// \~english @retval -1 Error
/// \~english @par Prerequisite
///        Should call CL_LockSystemInit(), CL_LockProcessInit() before this function is called
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - If systemcall munmap is failure, errno is set euqal with the errno of munmap, -1 is returned
/// \~english @par Detail
///        0 would be returned, even if not get the Lock.
///        The addr is the Lock information address that acquired from CL_LockMap.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Sync only
/// \~english @see
///        CL_LockMap
////////////////////////////////////////////////////////////////////////////////////
int32_t CL_LockUnmap(void *addr);  // NOLINT(readability/nolint)

////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CL_LockGet
/// \~english @par Brief
///        Get the Lock. Block until get the Lock.
/// \~english @param [in] addr
///        void* - Lock information address.
/// \~english @retval 0       Success
/// \~english @retval EINVAL  Invalid parameter
/// \~english @retval EDEADLK Mutex already locked
/// \~english @par Prerequisite
///        Should call CL_LockMap to get the Lock information address.
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - EINVAL as the parameter of Lock information address is NULL
///        - EINVAL as the parameter of Lock information address is MAP_FAILED
///        - EINVAL as the mutex does not be initialized in the systemcall pthread_mutex_lock failure.
///        - EDEADLK as the current thread already owns the mutex in the systemcall pthread_mutex_lock failure.
/// \~english @par Detail
///        Get the Lock until other threads release it.
///        It will be deadlock if the current thread already owns the mutex.
///        The addr is the Lock information address that should be acquired from CL_LockMap.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Sync only
/// \~english @see
///        CL_LockRelease
////////////////////////////////////////////////////////////////////////////////////
int32_t CL_LockGet(void *addr);  // NOLINT(readability/nolint)

////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CL_LockNowait
/// \~english @par Brief
///        Try to get Lock, shall immediately return.
/// \~english @param [in] addr
///        void* - Lock information address.
/// \~english @retval 0       Success
/// \~english @retval EINVAL  Invalid parameter
/// \~english @retval EBUSY   Busy
/// \~english @par Prerequisite
///        Should call CL_LockMap to get the Lock information address.
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - EINVAL as input parameter addr, the Lock information address is NULL
///        - EINVAL as input parameter addr, the Lock information address if MAP_FAILED
///        - EINVAL as the mutex does not be initialized in the systemcall pthread_mutex_trylock failure.
///        - EBUSY as the mutex could not be acquired as it was already locked in the
///          systemcall pthread_mutex_trylock failure.
/// \~english @par Detail
///        The addr is the Lock information address that should be acquired from CL_LockMap.
///        Get the Lock information from the pointed share memory.
///        Shall immediately return while not acquire the Lock.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Sync only
/// \~english @see
///        CL_LockRelease
////////////////////////////////////////////////////////////////////////////////////
int32_t CL_LockNowait(void *addr);  // NOLINT(readability/nolint)

////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CL_LockRelease
/// \~english @par Brief
///        Release the Lock.
/// \~english @param [in] addr
///        addr   - Lock information address.
/// \~english @retval 0       Success
/// \~english @retval EINVAL  Invalid parameter
/// \~english @retval EPERM   The current thread does not own the mutex
/// \~english @par Prerequisite
///        - Should call CL_LockMap to get the Lock information address.
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - EINVAL as input parameter addr, the Lock information address is NULL
///        - EINVAL as input parameter addr, the Lock information address if MAP_FAILED
///        - EINVAL as the mutex does not be initialized in the systemcall pthread_mutex_unlock failure.
///        - EPERM as the current thread does not own the mutex in the systemcall pthread_mutex_unlock failure.
/// \~english @par Detail
///        0 would be returned, even if not get the Lock.
///        The addr is the Lock information address that should be acquired from CL_LockMap.
///        Release the Lock related with the share memory.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Sync only
/// \~english @see
///        CL_LockGet, CL_LockNowait
////////////////////////////////////////////////////////////////////////////////////
int CL_LockRelease(void *addr);  // NOLINT(readability/nolint)

#ifdef __cplusplus
}
#endif


/** @}*/  // end of common_library
/** @}*/  // end of NativeService
/** @}*/  // end of BaseSystem
#endif  // #ifndef _cl_lock_h_  // NOLINT(build/header_guard)
