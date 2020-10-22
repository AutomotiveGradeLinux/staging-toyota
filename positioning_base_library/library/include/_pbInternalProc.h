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

#ifndef POSITIONING_BASE_LIBRARY_LIBRARY_INCLUDE__PBINTERNALPROC_H_
#define POSITIONING_BASE_LIBRARY_LIBRARY_INCLUDE__PBINTERNALPROC_H_

#define POSITIONINGBASELIBRARY_NON_FD       (-1)           //!< \~english invalid fd
#define POSITIONINGBASELIBRARY_MAP_NON_INIT (0)            //!< \~english init flag for mmap

#ifdef __cplusplus
extern "C" {
#endif

    int32    InitMutexObject(pthread_mutex_t *p_st_mutex);

    HANDLE    CreateSharedMemory(TCHAR* name, DWORD size);
    HANDLE    OpenSharedMemory(TCHAR* name, DWORD size);
    void    DeleteAllSharedMemory(void);
    void*    GetSharedMemoryPtr(HANDLE h_shm);
    void    CloseSharedMemory(HANDLE h_shm);
    void    DeleteSharedMemory(TCHAR* name);
    HANDLE    OpenSharedMemoryAtPhysical(DWORD physical_address, DWORD size, DWORD protect);

#ifdef __cplusplus
}
#endif

#endif  // POSITIONING_BASE_LIBRARY_LIBRARY_INCLUDE__PBINTERNALPROC_H_
