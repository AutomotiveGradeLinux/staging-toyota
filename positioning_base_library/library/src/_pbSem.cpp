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
 *    _pbSem.cpp
  */

#include <vehicle_service/positioning_base_library.h>
#include "_pbInternalProc.h"
#include "WPF_STD_private.h"
#include "tchar.h"

/*
 Constants and structure definitions
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#define MAX_PB_SEMAPHORES                        256
#define MAX_SEMAPHORE_NAME_LEN                    32
#define MAX_PB_SEMAPHORES_INPROC                16

/* Name storage table */
typedef struct {
    TCHAR             semaphore_name[MAX_SEMAPHORE_NAME_LEN + 1];    /* Semaphore name(Specified name of the user APP) */
    DWORD             ref_counter;                                    /* Reference Counter (Currently unused, always 1) */
} PB_SEMAPHORE;

/* Control information storage table */
typedef struct {
    HANDLE           h_heap;          /* Handle of the heap area allocated for expanding the self TBL (control-information-storage TBL)  */
    PB_SEMAPHORE*    p_sys_semaphore;  /* Address where the self name in the name storage table is stored (Top address of self name)    */
    DWORD            index;          /* Semaphore ID that is equal to the index of self TBL (control information strage TBL:p_handle_table) + 1   */
    HANDLE           h_semaphore;     /* Semaphore handle (Semaphore or Mutex according to the above DEBUG defines)     */
    HANDLE           h_mutex;        /* Mutex handle for locking when updating the self TBL (control information storage TBL)  */
} PB_SEMAPHORE_OPEN_HANDLE;

/* Semaphore information management table */
typedef struct {
    PB_SEMAPHORE_OPEN_HANDLE* p_handle_table[MAX_PB_SEMAPHORES];    /* Pointer to control information storage table      */
    HANDLE                    h_shared_memory;        /* Handle of shared memory allocated for name storage table to expand  */
    HANDLE                    h_mutex;                /* Mutex handle to lock when updating the name storage table */
    PB_SEMAPHORE*             p_semaphore_table;   /* Pointer to the name storage table (Allocate as many areas as the maximum number of registrations in shared memory)*/
} PB_SEMAPHORE_INSTANCE;

typedef struct      /* In-process semaphore management table */ {
    char                 semaphore_name[MAX_SEMAPHORE_NAME_LEN];        /* Semaphore name (Specified name of the user APP)   */
    HANDLE               h_heap;                           /* Heap handle of critical section structure area */
    CRITICAL_SECTION    *p_cs;                           /* Critical section pointer(Semaphore ID)    */
} PB_SEM_INPROC;

/*
 Internal function prototype declarations
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static DWORD    FindSemaphoreTable(PB_SEMAPHORE* p_semaphore_table, TCHAR* name, HANDLE h_mutex);
static DWORD    AllocNewSemaphoreTable(PB_SEMAPHORE* p_semaphore_table, TCHAR* name, HANDLE h_mutex);
static void     FreeSemaphoreTable(PB_SEMAPHORE* p_semaphore_table, int index, HANDLE h_mutex);

/*
 Global variable definitions
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static PB_SEMAPHORE_INSTANCE    g_instance;    // NOLINT(readability/nolint)  global class instance
/* CS for exclusive control of in-process semaphore management table */
CRITICAL_SECTION                g_sem_in_proc_tbl_mng_cs;
/* Pointer to the in-process semaphore management table */
PB_SEM_INPROC                   *g_p_sem_in_proc_mng = NULL;

/*
 *    Inline functions.
 */
inline void
MakeSemaphoreName(TCHAR* name, DWORD index) {
    wsprintf(name, __TEXT("POS_BASE_SEMAPHORE_SEM%05d"), static_cast<int32>(index));
}
inline void
MakeMutexName(TCHAR* name, DWORD index) {
    wsprintf(name, __TEXT("POS_BASE_SEMAPHORE_MUTEX%05d"), static_cast<int32>(index));
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : SemaphoreInit
 * ABSTRACT  : Semaphore initialization processing
 * NOTE      : This function is called when _CWORD64_api.dll is ATTACH from processes
 *           : and initializes the process.
 * ARGUMENT  : None
 * RETURN    : RET_API    RET_NORMAL        Normal completion
 *                        RET_ERRINIT        ABEND
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
SemaphoreInit(void) {
    RET_API                    ret_api = RET_NORMAL;
    PB_SEMAPHORE_INSTANCE *p_inst = &g_instance;
    TCHAR                    name[32] = {0};
    DWORD                    semaphore_table_size = 0;
    int32                     n = 0;
    PB_SEMAPHORE           *p_semaphore_table = NULL;
    BOOL                    b_create = FALSE;

    /* Initialize the semaphore information management table */
    for (n = 0; n < MAX_PB_SEMAPHORES; n++) {
        p_inst->p_handle_table[n] = NULL;            /* NULL initialize the control data storage table */
    }

    /* Initialize the semaphore name storage table */
    _tcscpy(name, __TEXT("POS_BASE_SEMAPHORE_TABLE"));
    semaphore_table_size = sizeof(PB_SEMAPHORE) * MAX_PB_SEMAPHORES;
    /* Open shared memory with the name _CWORD64__SEMAPHORE_TABLE */
    p_inst->h_shared_memory = OpenSharedMemory(name, semaphore_table_size);
    if (p_inst->h_shared_memory == NULL)              /* If shared memory does not exist */ {
        /* Create a shared memory with the name _CWORD64__SEMAPHORE_TABLE */
        p_inst->h_shared_memory = CreateSharedMemory(name, semaphore_table_size);
        if (p_inst->h_shared_memory == NULL)          /* If shared memory creation fails */ {
            ret_api = RET_ERRINIT;                /* Ends in error */
        } else {
            b_create = TRUE;                        /* Create shared memory */

            /* Allocate the created shared memory to the semaphore name storage table and initialize it. */
            // LCOV_EXCL_BR_START 200: cannot be null
            p_semaphore_table = reinterpret_cast<PB_SEMAPHORE*>(GetSharedMemoryPtr(p_inst->h_shared_memory));
            // LCOV_EXCL_BR_STOP
            if (p_semaphore_table == NULL) {    // LCOV_EXCL_BR_LINE 200: cannot be null
                // LCOV_EXCL_START 200: cannot be null
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                CloseSharedMemory(p_inst->h_shared_memory);
                DeleteSharedMemory(name);
                ret_api = RET_ERRINIT;            /* Ends in error */
                // LCOV_EXCL_STOP
            } else {
                for (n = 0; n < MAX_PB_SEMAPHORES; n++) {
                    p_semaphore_table[n].semaphore_name[0] = __TEXT('\0');    /* Initialize name */
                    p_semaphore_table[n].ref_counter = 0;                /* Initialize reference counter */
                }
            }
        }
    }

    if (ret_api == RET_NORMAL) {
        /* Save the address of the shared memory to the name storage table pointer of the semaphore information management table. */
        // LCOV_EXCL_BR_START 200: cannot be null
        p_inst->p_semaphore_table = reinterpret_cast<PB_SEMAPHORE*>(GetSharedMemoryPtr(p_inst->h_shared_memory));
        // LCOV_EXCL_BR_STOP
        if (p_inst->p_semaphore_table == NULL) {  // LCOV_EXCL_BR_LINE 200: cannot be null
            // LCOV_EXCL_START 200: cannot be null
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            CloseSharedMemory(p_inst->h_shared_memory);
            if (b_create != FALSE) {
                DeleteSharedMemory(name);
            }
            ret_api = RET_ERRINIT;            /* Ends in error */
            // LCOV_EXCL_STOP
        } else {
            /* Mutex creation process for semaphore-information-management table */
            _tcscpy(name, __TEXT("POS_BASE_SEMAPHORE_MUTEX"));
            /* Save the handle of the created Mutex in the Mutex handles for semaphore-information-management-table */
            p_inst->h_mutex = _pb_CreateMutex(NULL, FALSE, name);  // LCOV_EXCL_BR_LINE 200: can not be null
            if (p_inst->h_mutex == NULL) {  /* Failed to create a Mutex. */  // LCOV_EXCL_BR_LINE 200: can not be null
                // LCOV_EXCL_START 200: can not be null
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_CWORD64_api.dll:%s:LINE %d\r\n CreateMutex ERROR " \
                    "In SemaphoreInit\r\n", LTEXT(__FILE__), __LINE__);
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Mutex_Name : %s\r\n", name);
                _pb_Exit();
                // LCOV_EXCL_STOP
            }
        }
    }

    return ret_api;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : SemaphoreTerm
 * ABSTRACT  : Semaphore function termination processing
 * NOTE      : Called when the process ATTACH to _CWORD64_api.dll terminates, and then terminated.
 * ARGUMENT  : None
 * RETURN    : RET_API    RET_NORMAL        Always this value
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
SemaphoreTerm(void) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    PB_SEMAPHORE_INSTANCE *p_inst = &g_instance;

    if (p_inst->h_mutex != NULL) {
        PbDeleteMutex(p_inst->h_mutex);
        p_inst->h_mutex = NULL;
    }

    if (p_inst->h_shared_memory != NULL) {
        CloseSharedMemory(p_inst->h_shared_memory);
        p_inst->h_shared_memory = NULL;
    }

    return RET_NORMAL;
}
// LCOV_EXCL_STOP

/**
 * @brief
 *   Create Semaphore
 *
 *   Create a semaphore and return a semaphore ID.<br>
 *   For a semaphore that has already been created, return the same value of the semaphore ID when it has been created.
 *
 * @param[in]    *sem_name    Pointer to the semaphore name string to be created (NULL termination)
 *
 * @return Semaphore ID created other than 0<br>
 *              0       ABEND to create semaphore
 */
#ifdef _CWORD64_API_DOES_NOT_USE_UNICODE
SemID _pb_CreateSemaphore(char* sem_name)    // NOLINT(readability/nolint) WPF_SYSAPI.h API
#else
SemID _pb_CreateSemaphore(TCHAR* sem_name)   // NOLINT(readability/nolint) WPF_SYSAPI.h API
#endif  // _CWORD64_API_DOES_NOT_USE_UNICODE
{
    SemID                   ret_sem_id = 0;
    TCHAR                   *p_semaphore_name = NULL;
    PB_SEMAPHORE_INSTANCE   *p_inst = &g_instance;
    TCHAR                    name[MAX_SEMAPHORE_NAME_LEN + 1] = {0};
    PB_SEMAPHORE_OPEN_HANDLE *p_semaphore_open = NULL;
    DWORD                    index = 0;

    /* Check if the semaphore name is NULL */
    if (sem_name == NULL) {
    } else {
        p_semaphore_name = sem_name;

        /* Check if the semaphore name is specified */
        if (p_semaphore_name[0] == __TEXT('\0')) {
        } else if (_tcslen(p_semaphore_name) > MAX_SEMAPHORE_NAME_LEN) {
            /* Check whether the semaphore name is less than or equal to the maximum number of characters */
        } else {
            /* Allocate Heap control information storage table area to create a semaphore */
            p_semaphore_open = reinterpret_cast<PB_SEMAPHORE_OPEN_HANDLE *>(PbProcessHeapAlloc(0, \
                sizeof(PB_SEMAPHORE_OPEN_HANDLE)));  // LCOV_EXCL_BR_LINE 200: no branch
        }

        if (p_semaphore_open != NULL) {
            PbMutexLock(p_inst->h_mutex, INFINITE);  /* Mutex Lock from here */  // LCOV_EXCL_BR_LINE 200: no branch  // NOLINT(whitespace/line_length)

            /* Retrieve the name storage table expanded in the shared memory with the user-specified name. */
            index = FindSemaphoreTable(p_inst->p_semaphore_table, p_semaphore_name, p_inst->h_mutex);
            if (index != ((DWORD) - 1)) {
                /* The semaphore name specified for the user already exists. */
                /* Check if the control information storage table exists */
                if (p_inst->p_handle_table[index] != NULL) {
                    /* If the control information storage TBL exists,(If it has already been created by this process) */
                    /* release the previously allocated Heap because it is not needed. */
                    PbProcessHeapFree(0, p_semaphore_open);  // LCOV_EXCL_BR_LINE 200: no branch
                     /* Retrieve the pointer to the TBL storing the existing control information. */
                    p_semaphore_open = p_inst->p_handle_table[index];
                    PbMutexUnlock(p_inst->h_mutex);  /* Mutex release */  // LCOV_EXCL_BR_LINE 200: no branch
                    /* Convert from an index of array to a semaphore ID and return (If it has already been created in this process) */
                    ret_sem_id = static_cast<SemID>(index + 1);
                } else {
                    // LCOV_EXCL_START 200: p_handle_table can not be NULL
                    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                    /* If a semaphore with the established name exists but the control information table does not exist, link to it  or create it in the following processing. */
                    /* (A semaphore was created by another process or the created process was terminated.) */
                    /* Store the assigned semaphore ID in the control information storage table. */
                    p_semaphore_open->index = index;
                    /* Store the start address of the name storage table */
                    p_semaphore_open->p_sys_semaphore = &p_inst->p_semaphore_table[index];
                    /* Store the address of the control information TBL (Heap) into the semaphore information management TBL. */
                    p_inst->p_handle_table[index] = p_semaphore_open;

                    /* Create the object name of the semaphore from the position (index of array) of the control information storage TBL. */
                    /* "_CWORD64__SEMAPHORE_SEMAPHORExxxxx" : xxxxx is expanded with five-digit array index */
                    MakeSemaphoreName(name, index);

                    /* Create a semaphore object using Mutex and store its handle into the control information storage TBL. */
                    p_semaphore_open->h_semaphore = _pb_CreateMutex(NULL, FALSE, name);
                    if (p_semaphore_open->h_semaphore == NULL) {
                        /* If the semaphore object creation failed, */
                        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_CWORD64_api.dll:%s:LINE %d\r\n CreateMutex ERROR " \
                            "In _pb_CreateSemaphore\r\n", LTEXT(__FILE__), __LINE__);
                        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Mutex_Name : %s\r\n", name);
                        _pb_Exit();                                /* Make reset */
                    }

                    /* Create a Mutex name for locking the control information storage TBL from the position (index of array) of the control information storage TBL */
                    /* "_CWORD64__SEMAPHORE_MUTEXxxxxx" : xxxxx is expanded with five-digit array index */
                    MakeMutexName(name, index);
                    /* Create a Mutex for locking the control information storage TBL and store its handle into the control information storage TBL */
                    p_semaphore_open->h_mutex = _pb_CreateMutex(NULL, FALSE, name);
                    if (p_semaphore_open->h_mutex == NULL) {
                        /* If the creation of a Mutex for locking the control data storage TBL fails, */
                        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_CWORD64_api.dll:%s:LINE %d\r\n CreateMutex ERROR " \
                            "In _pb_CreateSemaphore\r\n", LTEXT(__FILE__), __LINE__);
                        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Mutex_Name : %s\r\n", name);
                        _pb_Exit();
                    }
                    /* Semaphore Lock for updating semaphore information control TBL */
                    PbMutexLock(p_semaphore_open->h_mutex, INFINITE);
                    p_semaphore_open->p_sys_semaphore->ref_counter = 1;
                    /* Semaphore UnLock upon completion of updating semaphore-information-management-TBL */
                    PbMutexUnlock(p_semaphore_open->h_mutex);
                    /* Semaphore UnLock upon completion of updating semaphore-information-management-TBL */
                    PbMutexUnlock(p_inst->h_mutex);

                    ret_sem_id = static_cast<SemID>(index + 1);    /* Convert from an index of array to a semaphore ID and return */
                    /* (A semaphore was created by another process or the created process was terminated.) */
                    // LCOV_EXCL_STOP
                }
            } else {
                /* If the semaphore name specified by the user does not exist, it is newly created in the following processing. */
                /* Free-space retrieval processing for the name storage table  */
                index = AllocNewSemaphoreTable(p_inst->p_semaphore_table, p_semaphore_name, p_inst->h_mutex);
                if (index == ((DWORD) - 1)) {  // LCOV_EXCL_BR_LINE 200: table buffer is enough, can not failed
                    /* If there is no free space in the name storage table */
                    /* Initialize the name storage TBL pointer of the control information storage TBL. (it may be No meaning due to release heap area in the following processing?) */
                    // LCOV_EXCL_START 200: table buffer is enough, can not failed
                    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                    p_semaphore_open->p_sys_semaphore = NULL;
                    /* Free the Heap area allocated for control information storage TBL */
                    PbProcessHeapFree(0, p_semaphore_open);
                    /* Semaphore UnLock to recover from errors */
                    PbMutexUnlock(p_inst->h_mutex);
                    // LCOV_EXCL_STOP
                } else {
                     /* Store the assigned semaphore ID in the control information storage table. */
                    p_semaphore_open->index = index;
                    /* Store this start address of the name storage table */
                    p_semaphore_open->p_sys_semaphore = &p_inst->p_semaphore_table[index];
                    p_semaphore_open->p_sys_semaphore->ref_counter = 1;            /* Reset reference counter. */
                     /* Store the control information TBL (Heap) address in the semaphore information management TBL. */
                    p_inst->p_handle_table[index] = p_semaphore_open;

                    /* Create the object name of the semaphore from the position (array index) of the control information storage TBL. */
                    /* "_CWORD64__SEMAPHORE_SEMAPHORExxxxx" : xxxxx is expanded with five-digit array index */
                    MakeSemaphoreName(name, index);
                    p_semaphore_open->h_semaphore = _pb_CreateMutex(NULL, FALSE, name);  // LCOV_EXCL_BR_LINE 200: cannot be null  // NOLINT(whitespace/line_length)
                    if (p_semaphore_open->h_semaphore == NULL) {  // LCOV_EXCL_BR_LINE 200: cannot be null
                        // LCOV_EXCL_START 200: can not be null
                        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_CWORD64_api.dll:%s:LINE %d\r\n CreateMutex ERROR " \
                            "In _pb_CreateSemaphore\r\n", LTEXT(__FILE__), __LINE__);
                        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Mutex_Name : %s\r\n", name);
                        _pb_Exit();
                        // LCOV_EXCL_STOP
                    }

                    MakeMutexName(name, index);
                    p_semaphore_open->h_mutex = _pb_CreateMutex(NULL, FALSE, name);  // LCOV_EXCL_BR_LINE 200: cannot be null  // NOLINT(whitespace/line_length)
                    if (p_semaphore_open->h_mutex == NULL) {  // LCOV_EXCL_BR_LINE 200: cannot be null
                        // LCOV_EXCL_START 200: can not be null
                        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_CWORD64_api.dll:%s:LINE %d\r\n CreateMutex ERROR " \
                            "In _pb_CreateSemaphore\r\n", LTEXT(__FILE__), __LINE__);
                        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Mutex_Name : %s\r\n", name);
                        _pb_Exit();
                        // LCOV_EXCL_STOP
                    }
                    /* Semaphore name registration */
                    _tcscpy(p_semaphore_open->p_sys_semaphore->semaphore_name, p_semaphore_name);

                    PbMutexUnlock(p_inst->h_mutex);  /* Mutex lock to new create semaphore ends here */  // LCOV_EXCL_BR_LINE 200: no branch  // NOLINT(whitespace/line_length)

                    ret_sem_id = static_cast<SemID>(index + 1);    /* Convert from an array index to a semaphore ID and return */
                }
            }
        }
    }

    return ret_sem_id;        /* Return the allocated semaphore ID. */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : PbDeleteSemaphore
 * ABSTRACT  : Semaphore deletion processing
 * NOTE      : Delete the semaphore specified by semaphore ID
 * ARGUMENT  : SemID        sem_id            Semaphore ID to be deleted
 * RETURN    : RET_API        RET_NORMAL        Normal completion
 *           :                RET_OSERROR        ABEND
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
PbDeleteSemaphore(SemID sem_id) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API                    ret_api = RET_NORMAL;
    int                     index = 0;
    PB_SEMAPHORE_INSTANCE    *p_inst = &g_instance;
    PB_SEMAPHORE_OPEN_HANDLE  *p_semaphore_open = NULL;
    DWORD                     dw_ret_sts = 0;
    DWORD                     ref_counter = 0;

    /* Parameter check */
    if (sem_id == 0) {
        /* Error if specified semaphore ID is zero */
        ret_api = RET_OSERROR;
    }

    if (ret_api == RET_NORMAL) {
        index = static_cast<int>(sem_id) - 1;  /* Calculate the index number of the semaphore table from the specified semaphore ID. */
        if (index >= MAX_PB_SEMAPHORES) {
            /* If the specified semaphore ID is out of range */
            ret_api = RET_OSERROR;
        }
    }

    if (ret_api == RET_NORMAL) {
        PbMutexLock(p_inst->h_mutex, INFINITE);    /* Need this exclusion? Seems to not be used exclusion at Locking/Unlocking... */
        p_semaphore_open = p_inst->p_handle_table[index];
        PbMutexUnlock(p_inst->h_mutex);        /* Need this exclusion? Seems to not be used exclusion at Locking/Unlocking... */
        if (p_semaphore_open == NULL)              /* If the specified semaphore ID is not registered in the table */ {
            ret_api = RET_OSERROR;
        }
    }

    if (ret_api == RET_NORMAL) {
        /* Return an error if the semaphore is locked */
        dw_ret_sts = PbMutexLock(p_semaphore_open->h_semaphore, 0);
        if (dw_ret_sts == WAIT_TIMEOUT) {
            ret_api = RET_OSERROR;
        }
    }

    if (ret_api == RET_NORMAL) {
        PbMutexUnlock(p_semaphore_open->h_semaphore);

        PbMutexLock(p_semaphore_open->h_mutex, INFINITE);
        if (p_semaphore_open->p_sys_semaphore->ref_counter > 0) {
            p_semaphore_open->p_sys_semaphore->ref_counter--;
        }

        ref_counter = p_semaphore_open->p_sys_semaphore->ref_counter;
        PbMutexUnlock(p_semaphore_open->h_mutex);

        if (ref_counter == 0) {
            PbMutexLock(p_inst->h_mutex, INFINITE);    /* Get Mutex */

            FreeSemaphoreTable(p_inst->p_semaphore_table, index, p_inst->h_mutex);
            p_semaphore_open->p_sys_semaphore = NULL;
            if (p_semaphore_open->h_semaphore != NULL) {
                PbDeleteMutex(p_semaphore_open->h_semaphore);
                p_semaphore_open->h_semaphore = NULL;
            }

            if (p_semaphore_open->h_mutex != NULL) {
                PbDeleteMutex(p_semaphore_open->h_mutex);
                p_semaphore_open->h_mutex = NULL;
            }

            PbProcessHeapFree(0, p_semaphore_open);

            p_inst->p_handle_table[index] = NULL;
            PbMutexUnlock(p_inst->h_mutex);            /* Release Mutex */
        }
    }

    return ret_api;
}
// LCOV_EXCL_STOP

/**
 * @brief
 *   Semaphore Lock
 *
 *   Get the semaphore with the specified semaphore ID. Do not return from this function until it is acquired.
 *
 * @param[in]    sem_id    Semaphore ID of the semaphore to be acquired
 *
 * @return RET_NORMAL    Normal completion<br>
 *              RET_OSERROR    ABEND
 */
RET_API _pb_SemLock(SemID sem_id) {    // NOLINT(readability/nolint) WPF_SYSAPI.h API
    RET_API                     ret_api = RET_OSERROR;
    PB_SEMAPHORE_INSTANCE       *p_inst = &g_instance;
    PB_SEMAPHORE_OPEN_HANDLE    *p_semaphore_open = NULL;
    int                         index = 0;
    DWORD                       result = 0;

    /* Parameter check */
    if (sem_id != 0) {
        /* The specified semaphore ID is non-zero */
        index = static_cast<int>(sem_id) - 1;    /* Calculate the index number of the semaphore table from the specified semaphore ID. */
        if (index >= MAX_PB_SEMAPHORES) {
            /* If the specified semaphore ID is out of range */
        } else {
            p_semaphore_open = p_inst->p_handle_table[index];
            if (p_semaphore_open != NULL) {
                /* If the specified semaphore ID is already registered in the table, */
                ret_api = RET_NORMAL;
            }
        }
    }

    if (ret_api == RET_NORMAL) {
        /* Wait forever until a semaphore is acquired */
        result = PbMutexLock(p_semaphore_open->h_semaphore, INFINITE);

        switch (result) {
            case WAIT_OBJECT_0: {
                ret_api = RET_NORMAL;
                break;
            }
            case WAIT_ABANDONED: {  // LCOV_EXCL_BR_LINE 200: function do not return this
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                ret_api = RET_OSERROR;  // LCOV_EXCL_LINE 200: function do not return this
                break;  // LCOV_EXCL_LINE 200: function do not return this
            }
            case WAIT_TIMEOUT: {  // LCOV_EXCL_BR_LINE 200: parameter INFINITE not return this
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                ret_api = RET_OSERROR;  // LCOV_EXCL_LINE 200: parameter INFINITE not return this
                break;  // LCOV_EXCL_LINE 200: parameter INFINITE not return this
            }
            default:
                ret_api = RET_OSERROR;
                break;
        }
    }

    return ret_api;
}

/**
 * @brief
 *   Semaphore Unlock
 *
 *   Release the semaphore specified by semaphore ID.
 *
 * @param[in]    sem_id    Semaphore ID of the semaphore to be released
 *
 * @return RET_NORMAL    Normal completion<br>
 *              RET_OSERROR    ABEND
 */
RET_API _pb_SemUnlock(SemID sem_id) {    // NOLINT(readability/nolint) WPF_SYSAPI.h API
    RET_API                        ret_api = RET_OSERROR;
    int                            index = 0;
    PB_SEMAPHORE_INSTANCE        *p_inst = &g_instance;
    PB_SEMAPHORE_OPEN_HANDLE    *p_semaphore_open = NULL;
    BOOL                        ok = FALSE;

    /* Parameter check */
    if (sem_id != 0) {
        /* The specified semaphore ID is non-zero */
        index = static_cast<int>(sem_id) - 1;    /* Calculate the index number of the semaphore table from the specified semaphore ID. */
        if (index >= MAX_PB_SEMAPHORES) {
            /* If the specified semaphore ID is out of range */
        } else {
            p_semaphore_open = p_inst->p_handle_table[index];
            if (p_semaphore_open != NULL) {
                /* If the specified semaphore ID is already registered in the table, */
                ret_api = RET_NORMAL;
            }
        }
    }

    if (ret_api == RET_NORMAL) {
        ok = PbMutexUnlock(p_semaphore_open->h_semaphore);  // LCOV_EXCL_BR_LINE 200: unlock can not failed
        if (ok == FALSE) {  // LCOV_EXCL_BR_LINE 200: unlock can not failed
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            ret_api = RET_OSERROR;  // LCOV_EXCL_LINE 200: unlock can not failed
        }
    }

    return ret_api;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : InitSemaphoreInProcess
 * ABSTRACT  : Semaphore initialization processing
 * NOTE      : Initialize to use semaphore that is valid only within a process
 * ARGUMENT  : None
 * RETURN    : RET_API        RET_NORMAL        Normal completion
 *           :                RET_OSERROR        ABEND
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
InitSemaphoreInProcess(void) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API     ret_api = RET_NORMAL;
    DWORD       pid;                       /* Process ID */
    TCHAR       wcs_share_data_name[32];       /* Name of shared data area for in-process semaphore management table */
    char        mbs_share_data_name[32];       /* Name of shared data area for in-process semaphore management table */
    char        *cp_addr = NULL;               /* For returning the start address of the shared data area */
    u_int32     dummy_size;                   /* For returning the size of shared data area */

    pid = getpid();                                /* Get process ID */

    memset(&wcs_share_data_name[0], 0, sizeof(wcs_share_data_name));
    memset(&mbs_share_data_name[0], 0, sizeof(mbs_share_data_name));
#ifdef UNDER_CE
    /* Create the name of shared data area for in-process semaphore management table */
    wsprintf(&wcs_share_data_name[0], __TEXT("SemInProc_%08x"), pid);
    wcstombs(&mbs_share_data_name[0], &wcs_share_data_name[0], sizeof(mbs_share_data_name));
#else
    /* Create the name of shared data area for in-process semaphore management table */
    wsprintf(&mbs_share_data_name[0], __TEXT("SemInProc_%08x"), static_cast<int32>(pid));
#endif

    /* Link to shared data area for semaphore management table in process */
    ret_api = _pb_LinkShareData(&mbs_share_data_name[0], reinterpret_cast<void**>(&cp_addr), &dummy_size);
    if (ret_api == RET_NORMAL) {
        /* Successful completion because _pb_InitSemaphoreInProcess has already been executed. */
    } else {
        /* Create shared data area for in-process semaphore management table */
        ret_api = _pb_CreateShareData(&mbs_share_data_name[0],
            static_cast<u_int32>(sizeof(PB_SEM_INPROC) * MAX_PB_SEMAPHORES_INPROC), \
            reinterpret_cast<void**>(&cp_addr));
        if (ret_api == RET_NORMAL) {
            /* Save top address of in-process semaphore management table */
            g_p_sem_in_proc_mng = reinterpret_cast<PB_SEM_INPROC *>(cp_addr);

            /* Initialization processing of the critical section object for in-process semaphore management table lock */
            PbInitializeCriticalSection(&g_sem_in_proc_tbl_mng_cs);
        } else {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " _CWORD64_api.dll:%s:LINE %d\r\n ## " \
                "ERROR:InitSemaphoreInProcess --> _pb_CreateShareData ##\r\n", LTEXT(__FILE__), __LINE__);
            ret_api = RET_OSERROR;
        }
    }

    return ret_api;
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : DeinitSemaphoreInProcess
 * ABSTRACT  : Semaphore function termination processing
 * NOTE      : Terminate semaphore function that is valid only within a process
 * ARGUMENT  : None
 * RETURN    : RET_API        RET_NORMAL        Normal completion
 *           :                RET_OSERROR        ABEND
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
DeinitSemaphoreInProcess(void) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API        ret_api = RET_NORMAL;
    PB_SEM_INPROC  *p_sem_in_proc;                /* Pointer to the in-process semaphore management table */
    DWORD          pid;                        /* Process ID */
    TCHAR          wcs_share_data_name[32];        /* Name of shared data area for in-process semaphore management table */
    char           mbs_share_data_name[32];        /* Name of shared data area for in-process semaphore management table */
    char           *cp_addr = NULL;                /* For returning the start address of the shared data area */
    u_int32        dummy_size;                    /* For returning the size of the shared data area */
    int            i;

    pid = getpid();                    /* Get process ID */

    memset(&wcs_share_data_name[0], 0, sizeof(wcs_share_data_name));
    memset(&mbs_share_data_name[0], 0, sizeof(mbs_share_data_name));
#ifdef UNDER_CE
    /* Create the name of shared data area for in-process semaphore management table */
    wsprintf(&wcs_share_data_name[0], __TEXT("SemInProc_%08x"), pid);
    wcstombs(&mbs_share_data_name[0], &wcs_share_data_name[0], sizeof(mbs_share_data_name));
#else/* Create the name of shared data area for in-process semaphore management table */
    wsprintf(&mbs_share_data_name[0], __TEXT("SemInProc_%08x"), static_cast<int32>(pid));
#endif

    /* Link to shared data area for in-process semaphore management table */
    ret_api = _pb_LinkShareData(&mbs_share_data_name[0], reinterpret_cast<void**>(&cp_addr), &dummy_size);
    if (ret_api != RET_NORMAL) {
        ret_api = RET_NORMAL;                            /* If the link fails, it is assumed to have been deleted and it completes normally. */
    } else {
        /* Get CS for exclusive control of in-process semaphore management table */
        PbEnterCriticalSection(&g_sem_in_proc_tbl_mng_cs);

        p_sem_in_proc = g_p_sem_in_proc_mng;            /* Get start address of in-process semaphore management table */
        if (p_sem_in_proc == NULL) {
            ret_api = RET_OSERROR;
        } else {
            /* Search in-process semaphore management table (delete all semaphores) */
            for (i = 0; i < MAX_PB_SEMAPHORES_INPROC; i++, p_sem_in_proc++) {
                if (p_sem_in_proc->p_cs != 0) {
                    PbDeleteCriticalSection(p_sem_in_proc->p_cs);    /* Delete critical section */
                    /* Release the Heap area allocated as the critical section structured area */
                    PbProcessHeapFree(0, p_sem_in_proc->p_cs);
                }
            }

            ret_api = PbDeleteShareData(&mbs_share_data_name[0]);        /* Delete shared data area */
            if (ret_api != RET_NORMAL) {
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " _CWORD64_api.dll:%s:LINE %d\r\n ## " \
                    "ERROR:DeinitSemaphoreInProcess --> PbDeleteShareData ##\r\n", LTEXT(__FILE__), __LINE__);
                ret_api = RET_OSERROR;
            } else {
                g_p_sem_in_proc_mng = NULL;
            }
        }

        PbLeaveCriticalSection(&g_sem_in_proc_tbl_mng_cs);        /* Release CS for exclusive control of in-process semaphore management table */

        if (ret_api == RET_NORMAL) {
            /* When the process is completed normally up to this point */
            PbDeleteCriticalSection(&g_sem_in_proc_tbl_mng_cs);        /* Delete critical section */
        }
    }

    return ret_api;
}
// LCOV_EXCL_STOP

/*****    Internal functions    *****/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : FindSemaphoreTable
 * ABSTRACT  : Name storage table retrieval processing
 * NOTE      : Retrieve the specified name storage table with the specified name,
 *           : and return its array index if the specified name exists in the table.
 *           : The specified Mutex is locked during this table retrieval processing.
 * ARGUMENT  : PB_SEMAPHORE*    p_semaphore_table        Pointer to the name storage TBL
 *             TCHAR*            name                Pointer to the name to be retrieved 
 *             HANDLE            h_mutex                Handle of the name storage TBL-locking Mutex
 * RETURN    : DWORD            othe than -1            Array index of containing the name string to be retrieved
 *           :                    -1                    Specified name does not exist
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static DWORD
FindSemaphoreTable(PB_SEMAPHORE* p_semaphore_table, TCHAR* name, HANDLE h_mutex) {
    DWORD ret = (DWORD) - 1;
    /* Loop up to the maximum number of entries and search the name storage table. */
    for (int n = 0; n < MAX_PB_SEMAPHORES; n++) {
        /* If there is a matching name, */
        if (_tcscmp(p_semaphore_table[n].semaphore_name, name) == 0) {
            ret = n;                    /* Return the index of the array in which the given name existed */
            break;
        }
    }
    /* UnLock the lock Mutex because the search for the name storage table has been completed. */
    /*    PbMutexUnlock(h_mutex);    */

    return ret;                /* Since no search name exists, an error value of-1 is returned. */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : AllocNewSemaphoreTable
 * ABSTRACT  : Name storage table free space retrieval processing
 * NOTE      : Retrieve the specified name storage table from the beginning,
 *           : return the array index of any free space.
 *           : [Note] Because the Mutex part inside this function has been deleted
 *           : to fix a bug caused by Mutex leaks, the whole function must be
 *           : locked by Mutex from the outside when this function is used.
 * ARGUMENT  : PB_SEMAPHORE    *p_semaphore_table
 *             TCHAR            *name
 *             HANDLE            h_mutex
 * RETURN    : DWORD
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static DWORD
AllocNewSemaphoreTable(PB_SEMAPHORE* p_semaphore_table, TCHAR* name, HANDLE h_mutex) {
    DWORD ret = (DWORD) - 1;
    /* Loop up to the maximum number of entries and search the name storage table. */
    for (int n = 0; n < MAX_PB_SEMAPHORES; n++) {
        /* If there is free space */
        if (p_semaphore_table[n].semaphore_name[0] == __TEXT('\0')) {
            ret = n;
            break;
        }
    }

    return ret;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : FreeSemaphoreTable
 * ABSTRACT  :
 * NOTE      :
 * ARGUMENT  : PB_SEMAPHORE*    p_semaphore_table
 *             int                index
 *             HANDLE            h_mutex
 * RETURN    : None
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static void
FreeSemaphoreTable(PB_SEMAPHORE* p_semaphore_table, int index, HANDLE h_mutex) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    p_semaphore_table[index].semaphore_name[0] = __TEXT('\0');
}
// LCOV_EXCL_STOP

/*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 End of File : _sysSem.cpp
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*/
