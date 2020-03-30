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

/**
 * @file
 *    MsgBapi.cpp
 * @brief system common functions
  */

/*---------------------------------------------------------------------------------*
 * Include Files                                                                                   *
 *---------------------------------------------------------------------------------*/
#include <vehicle_service/positioning_base_library.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <dirent.h>
#include "_pbInternalProc.h"
#include "WPF_STD_private.h"

/*---------------------------------------------------------------------------------*
 * Function                                                                                        *
 *---------------------------------------------------------------------------------*/
/**
 * @brief
 *   Create Shared Memory
 *
 *   Create a file mapping object with the specified shared memory name and size, then <br>
 *   maps it to the invoking process space. If the shared memory that has already been <br>
 *   created is specified, an error is returned.<br>
 *   The shared memory management information allocates a heap area each time it is <br>
 *   created, and return this address as a shared memory handle.
 *
 * @param[in]  TCHAR* name
 * @param[in]  DWORD size
 *
 * @return HANDLE<br>
 *              Except NULL    Handle of the created shared memory(Management information pointer)<br>
 *              NULL        ABEND
 */
HANDLE CreateSharedMemory(TCHAR* name, DWORD size) {
    int32            lret = EOK;
    RET_API            ret_api = RET_ERROR;
    int32            fd = POSITIONINGBASELIBRARY_NON_FD;
    SHARED_MEMORY*    p_shm = NULL;
    char c_file_path[PATH_MAX + 1] = SHARED_MEMORY_DIRECTORY;

    if ((name != NULL) && (size != 0)) {  // LCOV_EXCL_BR_LINE 6:name can not be NULL, size can not be 0
        /* Allocate the shared memory management information area in the heap. */
        p_shm = reinterpret_cast<SHARED_MEMORY *>(malloc(sizeof(SHARED_MEMORY)));
        if (p_shm == NULL) {  // LCOV_EXCL_BR_LINE 5: malloc's error case
        } else {
            p_shm->h_heap = NULL;
            p_shm->phy_addr = 0;
            p_shm->h_map = NULL;
            p_shm->size = size;

            /* Create directory */
            (void)mkdir(SHARED_MEMORY_DIRECTORY, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);

            /* Create shared memory with the specified size and name.(Error if it already exists) */
            (void)strncat(c_file_path, (const char*)name, 128);
            fd = open(c_file_path, O_RDWR | O_CREAT | O_EXCL, S_IRWXU | S_IRWXG | S_IRWXO);  // LCOV_EXCL_BR_LINE 5: standard lib error  // NOLINT(whitespace/line_length)
            if (fd > POSITIONINGBASELIBRARY_NON_FD)   /* Coverity CID: 18775 compliant */ {  // LCOV_EXCL_BR_LINE 5: standard lib error  // NOLINT(whitespace/line_length)
                /* Set size */
                lret = ftruncate(fd, size);
                if (lret == EOK) {  // LCOV_EXCL_BR_LINE 5: ftruncate's error case
                    /* Map the created shared memory to the invoking process space. */
                    p_shm->p_memory = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
                    if (p_shm->p_memory != MAP_FAILED) {  // LCOV_EXCL_BR_LINE 5: standard lib error
                        ret_api = RET_NORMAL;
                    }
                }
            }
        }
    }

    if (fd > POSITIONINGBASELIBRARY_NON_FD)   /* Coverity CID: 18775 compliant */ {
        close(fd);
    }

    if (ret_api == RET_NORMAL) {  // LCOV_EXCL_BR_LINE 200: can not be not normal
        /* Indicate that it is the creator of the shared memory */
        p_shm->owner = TRUE;
    } else {
        // LCOV_EXCL_START 200: can not be not normal
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        /* In case of an error */
        if (fd != POSITIONINGBASELIBRARY_NON_FD) {
            /* Delete shared memory object if created */
            shm_unlink(name);
        }

        if (p_shm != NULL) {
            free(p_shm);
        }

        p_shm = NULL;
        // LCOV_EXCL_STOP
    }

    return (HANDLE)p_shm;
}

/**
 * @brief
 *   Open Shared Memory
 *
 *   Create a file mapping object with the specified shared memory name and size, <br>
 *   then map it to the invoking process space. Since this is not shared memory <br>
 *   creation processing, an error is returned if it is not a file mapping object.<br>
 *   The shared memory management information allocates a heap area each time <br>
 *   it is created, and return this address as a shared memory handle.
 *
 * @param[in]  TCHAR* name
 * @param[in]  DWORD size
 *
 * @return HANDLE<br>
 *              Except NULL    Handle of the acquired shared memory(Management information pointer)<br>
 *              NULL        ABEND
 */
HANDLE OpenSharedMemory(TCHAR* name, DWORD size) {
    RET_API            ret_api = RET_ERROR;
    int32            fd = POSITIONINGBASELIBRARY_NON_FD;
    SHARED_MEMORY*    p_shm = NULL;
    char c_file_path[PATH_MAX + 1] = SHARED_MEMORY_DIRECTORY;

    if ((name != NULL) && (size != 0)) {  // LCOV_EXCL_BR_LINE 6:name can not be NULL, size can not be 0
        /* Allocate the shared memory management information area in the heap. */
        p_shm = reinterpret_cast<SHARED_MEMORY *>(malloc(sizeof(SHARED_MEMORY)));
        if (p_shm == NULL) {  // LCOV_EXCL_BR_LINE 5: malloc's error case
        } else {
            p_shm->h_heap = NULL;
            p_shm->phy_addr = 0;
            p_shm->h_map = NULL;
            p_shm->size = size;

            /* Open shared memory with the specified size and name(Error if not exists) */
            (void)strncat(c_file_path, (const char*)name, 128);
            fd = open(c_file_path, O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);  // LCOV_EXCL_BR_LINE 5: standard lib error
            if (fd > POSITIONINGBASELIBRARY_NON_FD)   /* Coverity CID: 18774 compliant */ {
                /* Map open shared memory to invoking process space. */
                p_shm->p_memory = reinterpret_cast<SHARED_MEMORY *>(mmap(0, size, PROT_READ | PROT_WRITE, \
                    MAP_SHARED | POSITIONINGBASELIBRARY_MAP_NON_INIT, fd, 0));
                if (p_shm->p_memory != MAP_FAILED) {  // LCOV_EXCL_BR_LINE 5: standard lib error
                    ret_api = RET_NORMAL;
                }
            }
        }
    }

    if (fd > POSITIONINGBASELIBRARY_NON_FD)   /* Coverity CID: 18774 compliant */ {
        close(fd);
    }

    if (ret_api == RET_NORMAL) {
        /* Indicate that the opener of the shared memory not the creator of it. */
        p_shm->owner = FALSE;
    } else {
        /* In case of an error */
        if (p_shm != NULL) {
            free(p_shm);
        }

        p_shm = NULL;
    }

    return (HANDLE)p_shm;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : DeleteAllSharedMemory()
 * ABSTRACT  : Delete all shared memory objects
 * NOTE      : Delete all shared memory objects
 *           : However, the heap area allocated at the time of shared memory creation or open is not released.
 * ARGUMENT  : None
 * RETURN    : None
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void
DeleteAllSharedMemory(void) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    int32            lret = EOK;
    char*            cwd = NULL;
    char            sz_cwd[PATH_MAX + 1] = {0};
    DIR*            p_dir = NULL;
    struct dirent*     p_dir_ent = NULL;

    /* Get path of current directory */
    cwd = getcwd(sz_cwd, PATH_MAX + 1);

    if (cwd != NULL) {
        /* Move to shared memory directory */
        lret = chdir(SHARED_MEMORY_DIRECTORY);

        if (lret == EOK) {
            /* Open shared memory directory */
            p_dir = opendir(SHARED_MEMORY_DIRECTORY);
            if (p_dir != NULL) {
                for (;;) {
                    /* Get directory entry */
                    p_dir_ent = readdir(p_dir);
                    if (p_dir_ent == NULL) {
                        break;
                    }

                    if ((_pb_strcmp(p_dir_ent->d_name, ".") != 0) &&
                            (_pb_strcmp(p_dir_ent->d_name, "..") != 0)) {
                        /* Delete shared memory objects */
                        shm_unlink(p_dir_ent->d_name);
                    }
                }

                /* Close shared memory directory */
                closedir(p_dir);
            }

            /* Go to original directory */
            lret = chdir(cwd);
            if (lret != EOK) /* Coverity CID: 18816 compliant */ {
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "chdir ERROR [lret:%d]", lret);
            }
        }
    }

    /* Delete shared memory directory */
    lret = rmdir(SHARED_MEMORY_DIRECTORY);

    return;
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : GetSharedMemoryPtr()
 * ABSTRACT  : Shared memory start address acquisition processing
 * NOTE      : Return the start address of the shared memory that is allocated/mapped at the 
 *           : time of shared memory creation or open
 * ARGUMENT  : HANDLE        h_shm        Shared memory handle(Management information pointer)
 * RETURN    : void*        Except NULL    Pointer to the acquired shared memory
 *           :                NULL        ABEND
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void*
GetSharedMemoryPtr(HANDLE h_shm) {
    void            *p_vret = NULL;
    SHARED_MEMORY    *p_shm = reinterpret_cast<SHARED_MEMORY *>(h_shm);

    if (p_shm == NULL)      /* If the shared memory handle that is not created or not opened is specified, */ {  // LCOV_EXCL_BR_LINE 200: p_shm can not be NULL  // NOLINT(whitespace/line_length)
    } else {
        p_vret = p_shm->p_memory;
    }

    return p_vret;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : CloseSharedMemory()
 * ABSTRACT  : Shared memory close processing
 * NOTE      : Close the shared memory that is allocated/mapped at the time of its creation or open
 * ARGUMENT  : HANDLE        h_shm        Shared memory handle(Management information pointer)
 * RETURN    : NONE
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void
CloseSharedMemory(HANDLE h_shm) {
    SHARED_MEMORY*     p_shm = reinterpret_cast<SHARED_MEMORY *>(h_shm);
    int32            lret = EOK;

    if (p_shm == NULL)                  /* If the shared memory handle that is not created or not opened is specified, */ {
    } else if (p_shm->phy_addr != 0)      /* If the physical memory area address is specified,(Currently never comes here) */ {
        if (p_shm->p_memory != NULL) {
            /* Release physical memory area */
            if (lret != EOK) {
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, \
                    "_CWORD64_api.dll:%s:LINE %d\r\n munmap_device_memory ERROR In PbFreePhysicalMem\r\n", \
                LTEXT(__FILE__), __LINE__);
                _pb_Exit();    /* System recovery processing(Exception execution) */
            }
        }
    } else {
        /* Release the shared memory object mapped to invoking process space */
        if (p_shm->p_memory != NULL) {
            lret = munmap(p_shm->p_memory, p_shm->size);
            p_shm->p_memory = NULL;
        }

        /* Release the shared memory management information allocated in the heap */
        free(p_shm);
    }

    return;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : DeleteSharedMemory()
 * ABSTRACT  : Shared memory object delete processing
 * NOTE      : Delete the shared memory object that is specified name
 * ARGUMENT  : TCHAR*        name        Pointer to the name of the shared memory to be deleted
 * RETURN    : None
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void
DeleteSharedMemory(TCHAR* name) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    int32            lret = EOK;
    char*            cwd = NULL;
    char            sz_cwd[PATH_MAX + 1] = {0};

    if (name != NULL) {
        /* Get path of current directory */
        cwd = getcwd(sz_cwd, PATH_MAX + 1);

        if (cwd != NULL) {
            /* Move to shared memory directory */
            lret = chdir(SHARED_MEMORY_DIRECTORY);

            if (lret == EOK) {
                /* Delete shared memory object */
                lret = shm_unlink(name);

                /* Go to original directory */
                lret = chdir(cwd);
            }
        }
    }

    return;
}
// LCOV_EXCL_STOP


HANDLE
OpenSharedMemoryAtPhysical(DWORD physical_address, DWORD size, DWORD protect) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return NULL;
}
// LCOV_EXCL_STOP

