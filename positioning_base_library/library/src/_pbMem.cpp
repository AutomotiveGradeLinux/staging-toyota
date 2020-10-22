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

/*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 File name      : _pbMem.cpp
 System name    : 05 Integration Platform
 Subsystem name : System common functions
 Title          : System API
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*/

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <vehicle_service/positioning_base_library.h>
#include "tchar.h"
#include "WPF_STD_private.h"
#include "_pbInternalProc.h"

/*
 Constants and data type definitions
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#define MAX_AREA_NAME_LEN        32
#define MAX_FILENAME_LEN        32
#define _CWORD64__MEM_MUTEX_NAME        __TEXT("POS_BASE_MEM_MUTEX")
#define MAX_ROMFILENAME_LEN        (32-4)        /* To the name appended with "_rom" at the time of registration */
#define PHYADDR_MAX                0x9FFFFFFF    /* Physical address of the boundary divided by 256 */
#define ROM_DATA_SIZE            0x00400000    /* Size of the virtual ROM data area(4MB) */
#define ALLOC_SIZE                0x00200000    /* 2MB (Minimum VirtualAlloc allocation of shared memory, */
/*      which is less than 2MB, is not mapped to shared memory) */
#define OFFSET_SIZE                0x00001000    /* 4KB (VirtualCopy Addressing Units) */

  /* Memory map information */
typedef struct TagMemMapItem {
    HANDLE                    h_heap;             /* Handle of heap.           */
    struct TagMemMapItem*     p_next;             /* Pointers to the next Chain memory map table    */
    struct TagMemMapItem*     p_prev;             /* Pointers to the previous Chain memory map table    */
    DWORD                     address;
    TCHAR                     name[MAX_AREA_NAME_LEN];    /* Name of shared data area(Not used when permit is processed)        */
    HANDLE                    h_shared_mem;                    /* Shared memory handle           */
} MEMMAP_ITEM;

/* Memory map information management table */
typedef struct {
    HANDLE                    h_heap;            /* Heap handle of the invoking table                */
    MEMMAP_ITEM*              p_head;            /* Pointer to the first memory map information            */
    MEMMAP_ITEM*              p_tail;            /* Pointer to the terminating memory map information            */
    DWORD                     num_of_items;        /* Chain memory map data                */
    HANDLE                    h_mutex;            /* _CWORD64__MEM_MUTEX Exclusive Mutex handles    */
} MEMMAP_LIST;

typedef struct {
    DWORD                    mem_size;
    DWORD                    reserv[3];
} _CWORD64_SHMHDR;

/* LINK ROM data top address/size storage ROM data */
typedef struct {
    u_int32                    *link_rom_addr;    /* ROM data start address                        */
    u_int32                    link_size;        /* ROM data size                                */
} LINK_ROM;

/*
 Internal function prototype declaration
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static void            UnlinkFromMemMapList(MEMMAP_ITEM* p_item);
static void            LinkToMemMapList(MEMMAP_ITEM* p_item);
static MEMMAP_ITEM*        FindMemMapItemByName(TCHAR* name);

/*
 Global variable
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
MEMMAP_LIST*    g_p_mem_map_list;

/**
 * @brief
 *   Memory API initialization
 *
 * @param[in]  none
 *
 * @return RET_API    RET_NORMAL Normal completion<br>
 *                        RET_ERROR     ABENDs
 */

RET_API MemoryInit(void) {
    RET_API        ret_api = RET_NORMAL;
    TCHAR        name[] = _CWORD64__MEM_MUTEX_NAME;

    // LCOV_EXCL_BR_START 5: standard lib error
    g_p_mem_map_list = reinterpret_cast<MEMMAP_LIST *>(PbProcessHeapAlloc(0, sizeof(MEMMAP_ITEM)));
    // LCOV_EXCL_BR_STOP
    if (g_p_mem_map_list == NULL) {  // LCOV_EXCL_BR_LINE 5: standard lib error
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        ret_api = RET_ERROR;  // LCOV_EXCL_LINE 5: standard lib error
    } else {
        /* Initialization of memory map information management table */
        g_p_mem_map_list->h_heap = NULL;
        g_p_mem_map_list->p_head = NULL;
        g_p_mem_map_list->p_tail = NULL;
        g_p_mem_map_list->num_of_items = 0;

        /* Create Mutex */
        g_p_mem_map_list->h_mutex = _pb_CreateMutex(NULL, TRUE, name);  // LCOV_EXCL_BR_LINE 200: no branch
        if (g_p_mem_map_list->h_mutex == NULL) {  // LCOV_EXCL_BR_LINE 200: can not be NULL
            // LCOV_EXCL_START 200:dead code
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_CreateMutex ERROR [name:%s]", name);
            _pb_Exit();    /* System recovery processing(Exception execution) */
            // LCOV_EXCL_STOP
        }
    }

    return ret_api;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : MemoryTerm
 * ABSTRACT  : Memory API termination processing
 * NOTE      : This function frees each allocated object.
 *           : This function must be called only once by DllMain() at process termination.
 * ARGUMENT  : None
 * RETURN    : Return RET_NORMAL
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
MemoryTerm(void) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API        ret_api = RET_NORMAL;

    if (g_p_mem_map_list == NULL) {
        ret_api = RET_ERROR;
    } else {
        /* Lock Mutex */
        PbMutexLock(g_p_mem_map_list->h_mutex, INFINITE);

        while (g_p_mem_map_list->num_of_items > 0) {
            MEMMAP_ITEM* p_item = g_p_mem_map_list->p_head;
            if (g_p_mem_map_list->num_of_items == 1) {
                /* Delete last one of memory map list */
                g_p_mem_map_list->p_head = NULL;
                g_p_mem_map_list->p_tail = NULL;
                g_p_mem_map_list->num_of_items = 0;
            } else {
                /* Deletes the beginning of the memory map list, with the next at the beginning. */
                g_p_mem_map_list->p_head = g_p_mem_map_list->p_head->p_next;
                g_p_mem_map_list->p_head->p_prev = NULL;
                g_p_mem_map_list->num_of_items--;
            }

            /* Releases the real memory specified in the memory map list. */
            if (p_item != NULL) {
                PbProcessHeapFree(0, p_item);
                p_item = NULL;
            }
        }

        /* Release Mutex */
        PbMutexUnlock(g_p_mem_map_list->h_mutex);

        /* Delete Mutex */
        PbDeleteMutex(g_p_mem_map_list->h_mutex);

        /* Free MEMMAP_LIST structure */
        PbProcessHeapFree(0, g_p_mem_map_list);
        g_p_mem_map_list = NULL;
    }

    return ret_api;
}
// LCOV_EXCL_STOP

/**
 * @brief
 *    Allocate Process Heap
 *
 *   Allocates a memory block from the process heap
 *
 * @param[in]    DWORD    dw_flags    Heap allocation scheme
 * @param[in]    SIZE_T    dw_bytes    Number of bytes to allocate
 *
 * @return    LPVOID    Except NULL    Pointer to the allocated memory block
 *                        NULL        Assignment Failed
 */
LPVOID PbProcessHeapAlloc(DWORD dw_flags, SIZE_T dw_bytes) {
    LPVOID        pv_ret = NULL;

    if ((dw_flags & HEAP_ZERO_MEMORY) == 0) {  // LCOV_EXCL_BR_LINE 200: dw_flags cannot have bit HEAP_ZERO_MEMORY
        /* Zero initialization not specified */
        pv_ret = malloc(dw_bytes);
    } else {
        /* Zero initialization specified */
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        pv_ret = calloc(dw_bytes, sizeof(char));  // LCOV_EXCL_LINE 200: dw_flags cannot have bit HEAP_ZERO_MEMORY
    }

    return pv_ret;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : PrivateHeapAlloc
 * ABSTRACT  : Allocate a block of memory from the private heap
 * NOTE      : Because there is no private-heap notion in PosixBasedOS001,
 *             Allocate from the process-heap using ProcessHeapAlloc
 * ARGUMENT  : DWORD    dw_flags        Controlling Heap Allocation Methods
 *           : SIZE_T    dw_bytes        Number of bytes to allocate
 * RETURN    : LPVOID    Except NULL    Pointer to the allocated memory block
 *           :             NULL        Assignment Failed
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
LPVOID
PrivateHeapAlloc(DWORD dw_flags, SIZE_T dw_bytes) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return PbProcessHeapAlloc(dw_flags, dw_bytes);
}
// LCOV_EXCL_STOP

/**
 * @brief
 *    Free Process Heap
 *
 *   Frees memory blocks allocated from the process heap
 *
 * @param[in]    DWORD    dw_flags    Heap free method(Unused)
 * @param[in]    LPVOID    lp_mem        Pointer to the memory to be freed
 *
 * @return    BOOL            Non-zero    Normal completion
 *                               0        ABENDs
 */
BOOL PbProcessHeapFree(DWORD dw_flags, LPVOID lp_mem) {
    free(lp_mem);

    return TRUE;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : PrivateHeapFree
 * ABSTRACT  : Free memory blocks allocated from private heap
 * NOTE      : Because there is no private-heap notion in PosixBasedOS001,
 *             Open using ProcessHeapFree
 * ARGUMENT  : DWORD    dw_flags    Heap release option(Not used by PosixBasedOS001)
 *           : LPVOID    lp_mem    Pointer to the memory to be freed
 * RETURN    : BOOL        Non-zero    Normal
 *           :             0        Abnormality
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
BOOL
PrivateHeapFree(DWORD dw_flags, LPVOID lp_mem) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return PbProcessHeapFree(dw_flags, lp_mem);
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : MemPermit
 * ABSTRACT  : Acquire access privilege to non-managed memory block of invoking process
 * NOTE      : To the data of the unmanaged memory block of invoking process
 *           : Allocate to accessible area of invoking process
 * ARGUMENT  : u_int32    mem_adr    Start address of the data area to which access privileges are granted
 *           : u_int32    size    Size of the data area to be granted access privileges
 *           : int32    perm    PERM_NONCACHE(Cache invalidation), PERM_CACHE(Cache enabled)
 *           : void**    ptr        Returns the start address of the actual data area that can be accessed
 * RETURN    : RET_API    RET_NORMAL    Normal completion
 *           :             RET_ERROR    ABENDs
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API MemPermit(u_int32 mem_adr, u_int32 size, int32 perm, void** ptr) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return RET_NORMAL;
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : MemProtect
 * ABSTRACT  : Revoking access privileges to invoking process unmanaged memory blocks
 * NOTE      : Free unmanaged memory block allocated to invoking process area
 * ARGUMENT  : u_int32    mem_adr        Start address of the data area from which the access privilege is to be revoked
 *           : u_int32    size        Size of the data area for which access privileges are to be revoked
 * RETURN    : RET_API    RET_NORMAL    Access privilege revocation successful
 *           :             RET_ERROR    Failed to revoke access privilege
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API MemProtect(u_int32 mem_adr, u_int32 size) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return RET_NORMAL;
}
// LCOV_EXCL_STOP

/**
 * @brief
 *   Create Share Data
 *
 * @param[in]    char*    area_name    Pointer to shared data area name string
 * @param[in]    u_int32 size        Size of the data portion
 * @param[out] void**    mem_ptr    Pointer to the start address of the shared data area
 *
 * @return RET_API
 *              RET_NORMAL    Normal
 *              RET_ERROR        Abnormality
 */
#ifdef _CWORD64_API_DOES_NOT_USE_UNICODE
RET_API _pb_CreateShareData(char* area_name, u_int32 size, void** mem_ptr)   // NOLINT(readability/nolint) WPF_SYSAPI.h
#else
RET_API _pb_CreateShareData(TCHAR* area_name, u_int32 size, void** mem_ptr)   // NOLINT(readability/nolint) WPF_SYSAPI.h
#endif  // _CWORD64_API_DOES_NOT_USE_UNICODE
{
    RET_API            ret_api = RET_NORMAL;
    TCHAR           *p_area_name = NULL;
    TCHAR            name[MAX_AREA_NAME_LEN] = {0};
    HANDLE            h_shm = NULL;
    _CWORD64_SHMHDR       *p_hdr = reinterpret_cast<_CWORD64_SHMHDR *>(MAP_FAILED);
    MEMMAP_ITEM       *p_item = NULL;
#ifdef UNDER_CE
    TCHAR            unicode_area_name[MAX_AREA_NAME_LEN] = {0};
#endif  // UNDER_CE

    if (mem_ptr == NULL) {
        ret_api = RET_ERROR;        /* Parameter error */
    } else if (area_name == NULL) {
        ret_api = RET_ERROR;        /* Parameter error */
    } else {
#ifdef _CWORD64_API_DOES_NOT_USE_UNICODE
#ifdef UNDER_CE
        if (area_name[0] == '\0') {
            ret_api = RET_ERROR;
        } else {
            /* Shared memory name character limit processing */
            if (strlen(area_name) >= MAX_AREA_NAME_LEN) {
                ret_api = RET_ERROR;
            } else {
                mbstowcs(unicode_area_name, area_name, MAX_AREA_NAME_LEN);
                p_area_name = unicode_area_name;
            }
        }
#else
        p_area_name = area_name;
        if (_tcslen(p_area_name) >= MAX_AREA_NAME_LEN) {
            ret_api = RET_ERROR;
        }
#endif
#else
        p_area_name = area_name;
        if (_tcslen(p_area_name) >= MAX_AREA_NAME_LEN) {
            ret_api = RET_ERROR;
        }
#endif
    }

    if (ret_api == RET_NORMAL) {
        if ((p_area_name[0] == __TEXT('\0')) || (size == 0)) {
            ret_api = RET_ERROR;    /* Parameter error */
        }
    }

    if (ret_api == RET_NORMAL) {
        _tcscpy(name, p_area_name);

        /* Lock Mutex */
        PbMutexLock(g_p_mem_map_list->h_mutex, INFINITE);  // LCOV_EXCL_BR_LINE 200: no branch

        p_item = FindMemMapItemByName(name);    /* Search memory map object by area name */
        if (p_item != NULL)              /* When there is already a generated name */ {
            *mem_ptr = NULL;
            ret_api = RET_ERROR;    /* Return in error because it has been generated */
        } else {
            /* Allocate MEMMAP_ITEM structure from heap */
            // LCOV_EXCL_BR_START 5: standard lib error
            p_item = reinterpret_cast<MEMMAP_ITEM *>(PbProcessHeapAlloc(0, sizeof(MEMMAP_ITEM)));
            // LCOV_EXCL_BR_STOP
            if (p_item == NULL) {  // LCOV_EXCL_BR_LINE 5: standard lib error
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                *mem_ptr = NULL;  // LCOV_EXCL_LINE 5: standard lib error
                ret_api = RET_ERROR;    /* Failed to allocate */  // LCOV_EXCL_LINE 5: standard lib error
            } else {
                p_item->h_heap = NULL;
                p_item->p_next = NULL;
                p_item->p_prev = NULL;
                p_item->address = 0;
                p_item->h_shared_mem = NULL;
                LinkToMemMapList(p_item);    /* Add to end of memory map list */    // LCOV_EXCL_BR_LINE 200: no branch

                /* Allocate shared memory */
                h_shm = CreateSharedMemory(name, size + sizeof(_CWORD64_SHMHDR));  // LCOV_EXCL_BR_LINE 200: can not be null
                if (h_shm == NULL) {  // LCOV_EXCL_BR_LINE 200: can not be null
                    // LCOV_EXCL_START 200: can not be null
                    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                    /* Remove the target memory-mapped object from the memory-mapped list */
                    UnlinkFromMemMapList(p_item);
                    PbProcessHeapFree(0, p_item);
                    p_item = NULL;
                    *mem_ptr = NULL;
                    ret_api = RET_ERROR;
                    // LCOV_EXCL_STOP
                } else {
                    p_item->h_shared_mem = h_shm;
                     /* Get Accessible Address */
                    // LCOV_EXCL_BR_START 200: can not be null
                    p_hdr = reinterpret_cast<_CWORD64_SHMHDR *>(GetSharedMemoryPtr(p_item->h_shared_mem));
                    // LCOV_EXCL_BR_STOP
                    if (p_hdr == NULL) {  // LCOV_EXCL_BR_LINE 200: can not be null
                        // LCOV_EXCL_START 200: can not be null
                        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                        /* Delete allocated shared memory */
                        CloseSharedMemory(h_shm);
                        DeleteSharedMemory(name);
                        /* Remove the target memory-mapped object from the memory-mapped list */
                        UnlinkFromMemMapList(p_item);
                        PbProcessHeapFree(0, p_item);
                        p_item = NULL;
                        *mem_ptr = NULL;
                        ret_api = RET_ERROR;
                        // LCOV_EXCL_STOP
                    } else {
                        p_hdr->mem_size = size;
                        *mem_ptr = reinterpret_cast<void*>(p_hdr + 1);    /* Start at the address following the header */
                        _tcscpy(p_item->name, name);    /* Name registration in the table */
                    }
                }
            }
        }

        /* Release Mutex */
        PbMutexUnlock(g_p_mem_map_list->h_mutex);  // LCOV_EXCL_BR_LINE 200: no branch
    } else {
        /* When an error occurs during parameter check */
        if (mem_ptr != NULL) {
            *mem_ptr = NULL;
        }
    }

    return ret_api;
}

/**
 * @brief
 *   Link Share Data
 *
 * @param[in]    char*    area_name    Pointer to shared data area name string
 * @param[out] void**    mem_ptr    Pointer to the start address of the shared data area
 * @param[out] u_int32* size        Size of the data portion
 *
 * @return RET_API
 *              RET_NORMAL    Normal
 *              RET_ERROR        Abnormality
 */
#ifdef _CWORD64_API_DOES_NOT_USE_UNICODE
RET_API _pb_LinkShareData(char* area_name, void** mem_ptr, u_int32* size)   // NOLINT(readability/nolint) WPF_SYSAPI.h
#else
RET_API _pb_LinkShareData(TCHAR* area_name, void** mem_ptr, u_int32* size)   // NOLINT(readability/nolint) WPF_SYSAPI.h
#endif   // _CWORD64_API_DOES_NOT_USE_UNICODE
{
    RET_API            ret_api = RET_NORMAL;
    TCHAR           *p_area_name = NULL;
    TCHAR            name[MAX_AREA_NAME_LEN] = {0};
    _CWORD64_SHMHDR       *p_hdr = reinterpret_cast<_CWORD64_SHMHDR *>(MAP_FAILED);
    MEMMAP_ITEM       *p_item = NULL;
    HANDLE            h_shm = NULL;
    HANDLE            h_shm_temp = NULL;
    DWORD            mem_size = 0;
#ifdef UNDER_CE
    TCHAR            unicode_area_name[MAX_AREA_NAME_LEN] = {0};
#endif

    if (mem_ptr == NULL) {
        ret_api = RET_ERROR;        /* Parameter error */
    } else if ((area_name == NULL) || (size == NULL)) {
        ret_api = RET_ERROR;        /* Parameter error */
    } else {
#ifdef _CWORD64_API_DOES_NOT_USE_UNICODE
#ifdef UNDER_CE
        if (area_name[0] == '\0') {
            ret_api = RET_ERROR;
        } else {
            /* Shared memory name character limit processing */
            if (strlen(area_name) >= MAX_AREA_NAME_LEN) {
                ret_api = RET_ERROR;
            } else {
                mbstowcs(unicode_area_name, area_name, MAX_AREA_NAME_LEN);
                p_area_name = unicode_area_name;
            }
        }
#else
        p_area_name = area_name;
        if (_tcslen(p_area_name) >= MAX_AREA_NAME_LEN) {
            ret_api = RET_ERROR;
        }
#endif
#else
        p_area_name = area_name;
        if (_tcslen(p_area_name) >= MAX_AREA_NAME_LEN) {
            ret_api = RET_ERROR;
        }
#endif
    }

    if (ret_api == RET_NORMAL) {
        if (p_area_name[0] == __TEXT('\0')) {
            ret_api = RET_ERROR;    /* Parameter error */
        }
    }

    if (ret_api == RET_NORMAL) {
        *size = 0;
        _tcscpy(name, p_area_name);
        mem_size = (u_int32) * size;

        /* Lock Mutex */
        PbMutexLock(g_p_mem_map_list->h_mutex, INFINITE);  // LCOV_EXCL_BR_LINE 200: no branch

        p_item = FindMemMapItemByName(name);            /* Search memory map object by area name */
        if (p_item != NULL) {
            /* When there is already a generated name */
            h_shm = p_item->h_shared_mem;
            // LCOV_EXCL_BR_START 200: can not be null
            p_hdr = reinterpret_cast<_CWORD64_SHMHDR*>(GetSharedMemoryPtr(h_shm));    /* Get Accessible Address */
            // LCOV_EXCL_BR_STOP
            if (p_hdr == NULL) {  // LCOV_EXCL_BR_LINE 200: can not be null
                // LCOV_EXCL_START 200: can not be null
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                *mem_ptr = NULL;
                ret_api = RET_ERROR;
                // LCOV_EXCL_STOP
            } else {
                /* Start at the address following the header */
                *mem_ptr = reinterpret_cast<void *>(p_hdr + 1);
                *size = static_cast<u_int32>(p_hdr->mem_size);
            }
        } else {
            /* When no memory map object has been created */
            /* Allocate MEMMAP_ITEM structure from heap */
            // LCOV_EXCL_BR_START 5: standard lib error
            p_item = reinterpret_cast<MEMMAP_ITEM*>(PbProcessHeapAlloc(0, sizeof(MEMMAP_ITEM)));
            // LCOV_EXCL_BR_STOP
            if (p_item == NULL) {  // LCOV_EXCL_BR_LINE 5: standard lib error
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                *mem_ptr = NULL;  // LCOV_EXCL_LINE 5: standard lib error
                ret_api = RET_ERROR;    /* Failed to allocate */  // LCOV_EXCL_LINE 5: standard lib error
            } else {
                p_item->h_heap = NULL;
                p_item->p_next = NULL;
                p_item->p_prev = NULL;
                p_item->address = 0;
                _tcscpy(p_item->name, name);
                p_item->h_shared_mem = NULL;
                LinkToMemMapList(p_item);    /* Add to end of memory map list */  // LCOV_EXCL_BR_LINE 200: no branch

                ret_api = RET_ERROR;        /* Process result initialization */
                /* Maps the shared data area to physical memory and returns its information pointer
                 * (Check if not found by table lookup, but other processes are already reserved) */
                h_shm_temp = OpenSharedMemory(p_item->name, sizeof(_CWORD64_SHMHDR));  // LCOV_EXCL_BR_LINE 200: no branch
                if (h_shm_temp != NULL) {
                    /* Get Accessible Address */
                    p_hdr = reinterpret_cast<_CWORD64_SHMHDR*>(GetSharedMemoryPtr(h_shm_temp));  // LCOV_EXCL_BR_LINE 200: can not be NULL  // NOLINT(whitespace/line_length)
                    if (p_hdr != NULL) {  // LCOV_EXCL_BR_LINE 200: can not be NULL
                        mem_size = p_hdr->mem_size;
                        ret_api = RET_NORMAL;
                    }
                    CloseSharedMemory(h_shm_temp);  // LCOV_EXCL_BR_LINE 200: no branch
                }

                if (ret_api == RET_NORMAL) {
                    ret_api = RET_ERROR;    /* Process result initialization */
                    /* Maps the shared data area to physical memory and returns its information pointer */
                    h_shm = OpenSharedMemory(p_item->name, mem_size);  // LCOV_EXCL_BR_LINE 200: no branch
                    if (h_shm != NULL) {
                        p_item->h_shared_mem = h_shm;
                        /* Get Accessible Address */
                        p_hdr = reinterpret_cast<_CWORD64_SHMHDR*>(GetSharedMemoryPtr(p_item->h_shared_mem));
                        if (p_hdr != NULL) {
                            *mem_ptr = reinterpret_cast<void*>(p_hdr + 1);    /* Start at the address following the header */
                            *size = static_cast<u_int32>(mem_size);
                            ret_api = RET_NORMAL;
                        }
                    }
                }

                if (ret_api != RET_NORMAL) {
                    UnlinkFromMemMapList(p_item);    /* Remove the target memory-mapped object from the memory-mapped list */

                    PbProcessHeapFree(0, p_item);  // LCOV_EXCL_BR_LINE 200: no branch
                    p_item = NULL;
                    *mem_ptr = NULL;
                    *size = 0;
                }
            }
        }

        /* Release Mutex */
        PbMutexUnlock(g_p_mem_map_list->h_mutex);  // LCOV_EXCL_BR_LINE 200: no branch
    } else {
        /* When an error occurs during parameter check */
        if (mem_ptr != NULL) {
            *mem_ptr = NULL;
        }
    }

    return ret_api;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : PbDeleteShareData
 * ABSTRACT  : Shared data area deletion processing
 * NOTE      : Deletes (releases) the shared memory area allocated under the name of the shared data area.
 * ARGUMENT  : char*        area_name        Pointer to shared data area name string
 * RETURN    : RET_API        RET_NORMAL        Normal
 *           :                RET_ERROR        The specified shared data does not exist
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
#ifdef _CWORD64_API_DOES_NOT_USE_UNICODE
PbDeleteShareData(char* area_name)  // LCOV_EXCL_START 8:dead code
#else
PbDeleteShareData(TCHAR* area_name)
#endif   // _CWORD64_API_DOES_NOT_USE_UNICODE
{
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API            ret_api = RET_NORMAL;
    TCHAR           *p_area_name = NULL;
    TCHAR            name[MAX_AREA_NAME_LEN] = {0};
    MEMMAP_ITEM       *p_item = NULL;
#ifdef UNDER_CE
    TCHAR            unicode_area_name[MAX_AREA_NAME_LEN] = {0};
#endif

    /* Parameter check */
    if (area_name == NULL)          /* If the name of the shared area is NULL */ {
        ret_api = RET_ERROR;
    } else {
#ifdef _CWORD64_API_DOES_NOT_USE_UNICODE
#ifdef UNDER_CE
        if (strlen(area_name) >= MAX_AREA_NAME_LEN) {
            ret_api = RET_ERROR;
        } else {
            mbstowcs(unicode_area_name, area_name, MAX_AREA_NAME_LEN);
            p_area_name = unicode_area_name;
        }
#else
        p_area_name = area_name;
        if (_tcslen(p_area_name) >= MAX_AREA_NAME_LEN) {
            ret_api = RET_ERROR;
        }
#endif
#else
        p_area_name = area_name;
        if (_tcslen(p_area_name) >= MAX_AREA_NAME_LEN) {
            ret_api = RET_ERROR;
        }
#endif
        if (ret_api == RET_NORMAL) {
            if (p_area_name[0] == __TEXT('\0')) {
                ret_api = RET_ERROR;
            }
        }
    }

    if (ret_api == RET_NORMAL) {
        /* Shared memory is allocated with the name of the shared area, or the memory map information management table is searched. */
        _tcscpy(name, p_area_name);

        /* Lock Mutex */
        PbMutexLock(g_p_mem_map_list->h_mutex, INFINITE);

        p_item = FindMemMapItemByName(name);        /* Search memory map object by area name */
        if (p_item == NULL)                          /* If the area name is not in the memory-map information management TBL */ {
            /* If the area name is not in the memory map information management TBL, it has not been created or deleted. */
            /* Release Mutex */
            PbMutexUnlock(g_p_mem_map_list->h_mutex);
            ret_api = RET_ERROR;
        } else {
            /* If the area name is in the memory map information management TBL */
            /* Removes the memory map information from the Chain of the memory map information management table. */
            UnlinkFromMemMapList(p_item);

            /* Releases the Mutex of the memory map information management table and releases the shared memory. */
            /* Release Mutex */
            PbMutexUnlock(g_p_mem_map_list->h_mutex);

            if (p_item->h_shared_mem != NULL) {
                CloseSharedMemory(p_item->h_shared_mem);
                p_item->h_shared_mem = NULL;
            }

            /* Frees memory-map information from the heap */
            PbProcessHeapFree(0, p_item);

            /* Deleting Shared Memory Objects */
            /*************************************************************/
            /* Note: Be sure to delete it, so Do not use DeleteShareData */
            /* if you want to link elsewhere.                            */
            /*************************************************************/
            DeleteSharedMemory(name);
        }
    }

    return ret_api;
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : LinkRomData
 * ABSTRACT  : ROM data link
 * NOTE      : Links data located in ROM.
 *           : Specifies the name of the file in which the data is to be stored, and points to the start address of the data.
 *           : Get pointer and data size.
 * ARGUMENT  : char        *filename    Pointer to the string of the name of the ROM data storage file
 *           : void        **mem_ptr    Pointer to the start address of the ROM data storage file
 *           : u_int32    *size        Pointer to the size of the data portion
 * RETURN    : RET_API    RET_NORMAL    Normal status
 *           :             RET_ERROR    Specified ROM data storage file does not exist
 *           :                         File name character string width error
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifdef _CWORD64_API_DOES_NOT_USE_UNICODE
RET_API LinkRomData(char* filename, void** mem_ptr, u_int32* size)  // LCOV_EXCL_START 8:dead code
#else
RET_API LinkRomData(TCHAR* filename, void** mem_ptr, u_int32* size)
#endif   // _CWORD64_API_DOES_NOT_USE_UNICODE
{
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return RET_NORMAL;    /* Coverity CID: 18772 compliant */
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : PbAccessPhysicalMem
 * ABSTRACT  : Access to Physical Memory Area Allocation Data
 * NOTE      : Access data allocated in the physical memory area.
 *           : The physical memory area is mapped to the shared memory area and mapped.
 *           : The start address is returned.
 * ARGUMENT  : u_int32    addr        Start address of the physical memory area
 *           : void        **mem_ptr    Pointer to the start address of the mapped shared area
 *           : u_int32    size        Size of the data
 *           : u_int32    mode        Access mode
 *           :                             ACCESS_MODE_READONLY :Read Only
 *           :                             ACCESS_MODE_READWRITE:Reading and Writing
 * RETURN    : RET_API    RET_NORMAL        Normal status
 *           :             RET_ERRPARAM    Parameter error
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
PbAccessPhysicalMem(u_int32 addr, void **mem_ptr, u_int32 size, u_int32 mode) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return RET_NORMAL;    /* Coverity CID: 18767 compliant */
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : PbFreePhysicalMem
 * ABSTRACT  : Access release processing to the physical memory area allocation data
 * NOTE      : Releases access to data allocated in a physical memory area
 *           : Releases the shared memory area Allocate by AccessPhysicalMem.
 * ARGUMENT  : u_int32    addr        Start address of the physical memory area to be released
 *           : void        *mem_ptr    Pointer to the start address of the mapped shared area
 *           : u_int32    size        Data size to be released
 * RETURN    : RET_API    RET_NORMAL        Normal status
 *           :             RET_ERRPARAM    Parameter error
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
PbFreePhysicalMem(u_int32 addr, void *mem_ptr, u_int32 size) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return RET_NORMAL;    /* Coverity CID: 18766 compliant */
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : FindMemMapItemByName
 * ABSTRACT  : Memory map information retrieval processing
 * NOTE      : Retrieves the memory map information by the specified shared data area name.
 * ARGUMENT  : TCHAR*            name        Pointer to shared data area name character string
 * RETURN    : MEMMAP_ITEM*        !NULL        Pointer to memory map information
 *           :                    NULL        Memory map information does not exist
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static MEMMAP_ITEM*
FindMemMapItemByName(TCHAR* name) {
    MEMMAP_ITEM* ret = NULL;    /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
    if (g_p_mem_map_list == NULL) {  // LCOV_EXCL_BR_LINE 200: g_p_mem_map_list can not be NULL
        /* If the memory map information management table is not allocated, */
        /* nop */
        /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
    } else if (g_p_mem_map_list->num_of_items == 0) {
        /* If no memory map information is allocated, */
        /* nop */
        /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
    } else {
        /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
        MEMMAP_ITEM* p_item = g_p_mem_map_list->p_head;
        /* Gets the pointer of the memory map Chain at the beginning of the file. */
        /* Loops until there is no memory map Chain */
        while (p_item != NULL) {
            if (_tcscmp(p_item->name, name) == 0) {
                /* If the name of the memory map information matches the name of the argument */
                /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
                /* Returns a pointer to the memory map information */
                ret = p_item;
                break;    /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
            }
            p_item = p_item->p_next;                    /* Retrieves the pointers of the memory map data Chain next time. */
        }
    }

    return(ret);   /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : LinkToMemMapList
 * NOTE      : Adding a Memory Map Object to the Memory Map List
 * ARGUMENT  : MEMMAP_ITEM*    p_item    Specify a memory-mapped object
 * RETURN    : Without
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static void
LinkToMemMapList(MEMMAP_ITEM* p_item) {
    BOOL    processing_complete = FALSE;   /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
    if (g_p_mem_map_list == NULL) {  // LCOV_EXCL_BR_LINE 200: g_p_mem_map_list can not be NULL
        // LCOV_EXCL_START 200: g_p_mem_map_list can not be NULL
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        processing_complete = TRUE;   /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
        // LCOV_EXCL_STOP
    }

    /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
    if ((processing_complete == FALSE) && (g_p_mem_map_list->num_of_items == 0)) {
        g_p_mem_map_list->p_head = p_item;
        g_p_mem_map_list->p_tail = p_item;
        g_p_mem_map_list->num_of_items = 1;
        processing_complete = TRUE;   /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
    }

    /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
    if (processing_complete == FALSE) {
        g_p_mem_map_list->p_tail->p_next = p_item;
        p_item->p_prev = g_p_mem_map_list->p_tail;
        p_item->p_next = NULL;
        g_p_mem_map_list->p_tail = p_item;
        g_p_mem_map_list->num_of_items++;
    }
    return;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : UnlinkFromMemMapList
 * ABSTRACT  : Memory map information deletion processing
 * NOTE      : Memory map information specified by the pointer is stored in the memory map information management table.
 *           : From the Chain structures.The memory map information is not released.
 * ARGUMENT  : MEMMAP_ITEM*        p_item        Pointer to the memory map information to be deleted
 * RETURN    : None
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static void
UnlinkFromMemMapList(MEMMAP_ITEM* p_item) {
    /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
    BOOL    processing_complete = FALSE;
    /* Parameter check */
    if (g_p_mem_map_list == NULL) {  // LCOV_EXCL_BR_LINE 200: g_p_mem_map_list can not be NULL
        /* If the memory map information management table is not allocated, */
        /* nop */
        /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
    } else if (p_item == NULL) {  // LCOV_EXCL_BR_LINE 200: p_item can not be NULL
        /* If a pointer to memory-mapped information is not specified, */
        /* nop */
        /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
    } else if (g_p_mem_map_list->num_of_items == 0) {  // LCOV_EXCL_BR_LINE 200: num_of_items can not be 0
        /* If no memory map information is allocated, */
        /* nop */
        /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
    } else {
        /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
        /* Chain detachment process when there is only one memory map data item */
        if (g_p_mem_map_list->num_of_items == 1) {
            if (g_p_mem_map_list->p_head == p_item) {
                /* If only one memory map is reserved */
                /* For the top memory map information of the memo map information management table */
                g_p_mem_map_list->p_head = NULL;            /* Initializes the top memory map to NULL. */
                g_p_mem_map_list->p_tail = NULL;            /* NULL initialization of the termination memory map data */
                g_p_mem_map_list->num_of_items = 0;        /* Initializes the control memory map information count to zero. */
            }
            /* Specified memory map information pointer does not exist  */
            processing_complete = TRUE;   /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
        }

        /* Memory-map-information-removal process at the top of the Chain */
        if ((processing_complete == FALSE) && (p_item == g_p_mem_map_list->p_head))  {
            /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
            /* If the specified memory map information is the first memory map information, */
            /* The leading memory map information pointer is changed to the next Chain pointer. */
            g_p_mem_map_list->p_head = g_p_mem_map_list->p_head->p_next;
            /* Change the previous Chain source of the first memory map to NULL */
            g_p_mem_map_list->p_head->p_prev = NULL;
            g_p_mem_map_list->num_of_items--;        /* Decrement the number of management memory map information */
            p_item->p_next = NULL;         /* Initialize the Chain destination of the removed memory map NULL. */
            processing_complete = TRUE;   /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
        }

        /* Memory-map data detachment process at the end of the Chain */
        if ((processing_complete == FALSE) && (p_item == g_p_mem_map_list->p_tail)) {
            /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
            /* If the pointed-to memory-map information is terminated, */
            /* Change the terminating memory-map info pointer to the previous Chain source */
            g_p_mem_map_list->p_tail = g_p_mem_map_list->p_tail->p_prev;
            /* Change the Chain destination of the terminated memory map to NULL. */
            g_p_mem_map_list->p_tail->p_next = NULL;
            /* Decrement the number of management memory map information */
            g_p_mem_map_list->num_of_items--;
            /* The previous Chain source of the removed memory map data is NULL initialized. */
            p_item->p_prev = NULL;
            processing_complete = TRUE;   /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
        }

        /* Checking the memory map info Chain for errors */
        if ((processing_complete == FALSE) &&
                (g_p_mem_map_list->num_of_items <= 2)) {
            /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
            /* No more than two memory maps are possible except at the beginning and end. */
            processing_complete = TRUE;   /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
        }

        /* Departure process other than the start and end of the memory map data Chain */
        if (processing_complete == FALSE) {
            /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
            p_item->p_prev->p_next = p_item->p_next;     /* Set the next Chain destination of the previous memory map to the next one */
            p_item->p_next->p_prev = p_item->p_prev;     /* Previous Chain of one memory map before previous */
            g_p_mem_map_list->num_of_items--;             /* Decrement the number of management memory map information */
            p_item->p_prev = NULL;                 /* The previous Chain source of the removed memory map data is NULL initialized. */
            p_item->p_next = NULL;                 /* Initialize the Chain destination of the removed memory map NULL. */
        }
    }
    return;
}

/**
 * @brief
 *   Obtain dump information
 *
 * @param[out] p_buf      Dump info
 */
void _pb_GetDebugMemoryMngTbl(void* p_buf) {    // NOLINT(readability/nolint) WPF_SYSAPI.h
    static uint8_t  buf[DEBUG_DUMP_MAX_SIZE];
    static uint8_t  buf_tmp[256];
    uint32_t        i = 0;

    if (p_buf != NULL) {
        memset(&buf[0], 0x00, sizeof(buf));
        snprintf(reinterpret_cast<char *>(&buf[0]), sizeof(buf), "Memory");

        if (g_p_mem_map_list == NULL) {  // LCOV_EXCL_BR_LINE 200: g_p_mem_map_list can not be NULL
            // LCOV_EXCL_START 200: g_p_mem_map_list can not be NULL
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            strncat(reinterpret_cast<char *>(&buf[0]), "\n NULL", strlen("\n NULL"));
            // LCOV_EXCL_STOP
        } else if (g_p_mem_map_list->num_of_items == 0) {  // LCOV_EXCL_BR_LINE 200: num_of_items can not be 0
            // LCOV_EXCL_START 200: num_of_items can not be 0
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            strncat(reinterpret_cast<char *>(&buf[0]), "\n num_of_items:0", strlen("\n num_of_items:0"));
            // LCOV_EXCL_STOP
        } else {
            memset(&buf_tmp[0], 0x00, sizeof(buf_tmp));
            snprintf(reinterpret_cast<char *>(&buf_tmp[0]), sizeof(buf_tmp),  // LCOV_EXCL_BR_LINE 5: c lib error case
                    "\n h_heap:%p, p_head:%s, p_tail:%s, num_of_items:%lu, h_mutex:%p",
                    g_p_mem_map_list->h_heap,
                    ((g_p_mem_map_list->p_head == NULL)?"NULL":"NOT NULL"),
                    ((g_p_mem_map_list->p_tail == NULL)?"NULL":"NOT NULL"),
                    g_p_mem_map_list->num_of_items,
                    g_p_mem_map_list->h_mutex);
            strncat(reinterpret_cast<char *>(&buf[0]), reinterpret_cast<char *>(&buf_tmp[0]), \
                strlen(reinterpret_cast<char *>(&buf_tmp[0])));
            MEMMAP_ITEM* p_item = g_p_mem_map_list->p_head;
            while (p_item != NULL) {
                memset(&buf_tmp[0], 0x00, sizeof(buf_tmp));
                snprintf(reinterpret_cast<char *>(&buf_tmp[0]), sizeof(buf_tmp),
                        "\n [%02d]h_heap:%10p, p_next:%s, p_prev:%s, name:%40s, hShrMem:%10p, adr:%lu",
                        i,
                        p_item->h_heap,
                        ((p_item->p_next == NULL)?"NULL    ":"NOT NULL"),
                        ((p_item->p_prev == NULL)?"NULL    ":"NOT NULL"),
                        p_item->name,
                        p_item->h_shared_mem,
                        p_item->address);
                i++;
                strncat(reinterpret_cast<char *>(&buf[0]), reinterpret_cast<char *>(&buf_tmp[0]), \
                    strlen(reinterpret_cast<char *>(&buf_tmp[0])));
                p_item = p_item->p_next;
            }
        }
        memcpy(p_buf, &buf[0], sizeof(buf));
    }
}
