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

/**
 * @file
 *    _pbOther.cpp
 */

#include <stdio.h>
#include <unistd.h>
#include <asm/unistd.h>
#include <native_service/frameworkunified_types.h>

#include <vehicle_service/positioning_base_library.h>
#include "_pbInternalProc.h"
#include <other_service/VP_GetEnv.h>
#include "WPF_STD_private.h"
#include "tchar.h"



/*---------------------------------------------------------------------------------*
 * Define                                                                          *
 *---------------------------------------------------------------------------------*/
/* Shared memory */
#define POS_BASE_OTHER_PROC_ID    "POS_BASE_OTHER_PROC_ID"

#define MAX_OTHER_PROC_NUM      (32)     /** Maximum number of the management information to translate the process name to PNO      */
#define MAX_NUM_CTRL_TID        (16)     /** Maximum number of tje TID management for thread in Process       */

#define OTHER_PNO_BASE          (0x9000) /** Base number of local process            */

#define THREAD_NAME_LEN_MAX     (32)

#define FULL_OTHER_PROC_NUM     (MAX_OTHER_PROC_NUM - 4)  /** Threshold of the management information to translate the process name to PNO (no free) */
#define WARN_OTHER_PROC_NUM     (MAX_OTHER_PROC_NUM - 10) /** Threshold of the management information to translate the process name to PNO (warning) */

typedef void* (*_CWORD64_PROCMNG_START_ROUTINE)(void*);

/*---------------------------------------------------------------------------------*
 * Structure                                                                       *
 *---------------------------------------------------------------------------------*/
/*!
  @brief    Process identification information
 */
typedef struct {
    PNO     pno;                        /**< Process number    */
    char    name[THREAD_NAME_LEN_MAX];    /**< Process name        */
} PROC_ID;

/*!
  @brief    Process information
 */
typedef struct {
    PROC_ID  id[MAX_OTHER_PROC_NUM]; /**< Process identification information */
    uint32_t use_cnt;                 /**< Used number           */
    uint32_t rsv_cnt;                 /**< Reserved number       */
} PROC_INFO;

/*---------------------------------------------------------------------------------*
 * Grobal Value                                                                    *
 *---------------------------------------------------------------------------------*/
static HANDLE        g_h_app[MAX_NUM_CTRL_TID];       /** Application handle       */
static HANDLE        g_h_mtx;                        /** Shared-information-locking Mutex handle */
static HANDLE        g_h_shm;                        /** Shared memory handle */   // Coverity CID: 18787 compliant

static PROC_INFO*    g_p_proc_id_tbl;    /** Process Name-PNO Translation Table    */

/*---------------------------------------------------------------------------------*
 * Internal Function Prototype                                                     *
 *---------------------------------------------------------------------------------*/
/* Process number to PNO translation table manipulation functions */
static void        OtherSetPnoOfCnvTbl(u_int32 idx, PNO pno);
static void        OtherSetNameOfCnvTbl(u_int32 idx, PCSTR name);
static PNO        OtherGetPnoOfCnvTbl(u_int32 idx);
static PCSTR    OtherGetNameOfCnvTbl(u_int32 idx);
static u_int32    OtherSearchPnoOfCnvTbl(PNO pno);
static u_int32    OtherSearchNameOfCnvTbl(PCSTR name);
static void     OtherIncUseCntOfCnvTbl(void);
static void     OtherIncRsvCntOfCnvTbl(void);
static void     OtherDecRsvCntOfCnvTbl(void);

static void OtherCreateMutex(void);
static void OtherDeleteMutex(void);
static void OtherLockMutex(void);
static void OtherUnlockMutex(void);

/**
 * @brief
 *  Initialize other funtion
 *
 * @return RET_NORMAL    Normal completion
 */
RET_API ErrTrapInit(void) {
    RET_API    ret_api = RET_NORMAL;
    PROC_INFO **pp_tbl;
    u_int16    idx;

    pp_tbl = &g_p_proc_id_tbl; /* Set a pointer to a table to translate the process name to PNO */

    OtherCreateMutex(); /* Create Mutex for accessing shared info */
    OtherLockMutex(); /* Acquire Mutex for accessing shared info */

    /* Open shared memory for a table to translate the process name to PNO */
    g_h_shm = OpenSharedMemory(const_cast<char*>(POS_BASE_OTHER_PROC_ID), sizeof(PROC_INFO));

    /* If called for the first time within all processes, an error occurs and the following processing is performed. */
    if (g_h_shm == NULL) {  // LCOV_EXCL_BR_LINE 200: can not be not NULL
        /* Create shared memory for a table to translate the name to PNO */
        g_h_shm = CreateSharedMemory(const_cast<char*>(POS_BASE_OTHER_PROC_ID), sizeof(PROC_INFO));
        /* In case of an error */
        if (g_h_shm == NULL) {  // LCOV_EXCL_BR_LINE 200: can not be NULL
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_CreateShareData ERROR " \
                "[g_h_shm:%p]", g_h_shm);
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            _pb_Exit();  // // LCOV_EXCL_LINE 200: can not be NULL
            /* don't arrive here. */
        }
    }

    OtherUnlockMutex(); /* Release Mutex for accessing shared info */

    /* Set the acquired shared memory address as a pointer for a table to translate the process name to PNO */
    *pp_tbl = reinterpret_cast<PROC_INFO*>(GetSharedMemoryPtr(g_h_shm));

    /* Table initialization */
    for (idx = 0; idx < MAX_OTHER_PROC_NUM; idx++) {
        /* Set PNO into the table to translate the process name to PNO (Overwrite from the second process onwards) */
        OtherSetPnoOfCnvTbl(idx, static_cast<PNO>(OTHER_PNO_BASE + idx));
    }

    return ret_api;
}

/**
 * @brief
 *  Terminate other function
 */
void ErrTrapTerm(void) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    CloseSharedMemory(g_h_shm);

    OtherDeleteMutex();
}
// LCOV_EXCL_STOP

/**
 * @brief
 *   Create Thread
 *
 * @param[in]  lp_thread_attributes    Not used
 * @param[in]  dw_stack_size            Initial stack size
 * @param[in]  lp_start_address        Address of the effective function of the thread
 * @param[in]  lp_parameter            Thread arguments
 * @param[in]  dw_creation_flags        Not used
 * @param[in]  lp_thread_id            Thread identifier
 * @param[in]  pno                    PNO
 * @param[in]  priority                Thread priority
 *
 * @return Non-zero:Normal status,  0:When an error occurs
 */
HANDLE _pb_CreateThread(LPSECURITY_ATTRIBUTES lp_thread_attributes, DWORD dw_stack_size, LPTHREAD_START_ROUTINE lp_start_address, LPVOID lp_parameter, DWORD dw_creation_flags, LPDWORD lp_thread_id, PNO pno, int32 priority) {  // LCOV_EXCL_START 8:dead code  // NOLINT(whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    pthread_attr_t    st_thread_attr;
    pthread_t        ul_thread_id = 0;
    sched_param        st_thread_param = {0};
    HANDLE            handle = NULL;
    int32            lret = EOK;
    BOOL            bret = FALSE;

    /* null check */
    if (lp_thread_id  == NULL) {
        // no op
    } else {
        /* Initializing Attributes */
        lret = pthread_attr_init(&st_thread_attr);

        /* When the attribute initialization is successful */
        if (lret == EOK) {
            /* Do not inherit parent scheduling policies */
            lret = pthread_attr_setinheritsched(&st_thread_attr, PTHREAD_EXPLICIT_SCHED);
        }

        /* If you successfully configure policy inheritance */
        if (lret == EOK) {
            /* Scheduling settings */
            lret = pthread_attr_setschedpolicy(&st_thread_attr, SCHED_RR);
        }

        /* Successful Scheduling settings */
        if (lret == EOK) {
            /* Create a thread with the lowest priority so that the spawned thread */
            /* do not run until they are ready for processing                      */
            st_thread_param.sched_priority    = 1;
            lret = pthread_attr_setschedparam(&st_thread_attr, &st_thread_param);
        }

        /* If the priority setting is successful */
        if (lret == EOK) {
            lret = pthread_create(&ul_thread_id,
                                   NULL,
                                   (_CWORD64_PROCMNG_START_ROUTINE)lp_start_address,
                                   lp_parameter);
        }

        /* Successful pthread_create */
        if (lret == EOK) {
            bret = TRUE;
        }
    }

    /* When priority setting is successful */
    if (bret != FALSE) {
        /* Return value setting */
        handle         = (HANDLE)ul_thread_id;
        *lp_thread_id    = ul_thread_id;
    } else {
        /* Error log output */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "lret ERROR [lret:%d]", lret);
    }

    return handle;
}
// LCOV_EXCL_STOP

/**
 * @brief
 *   Get the thread ID
 *
 * @return Thread ID
 */
uint32_t PbGetTid(void) {
    uint32_t ul_tid;

    ul_tid = (uint32_t)syscall(__NR_gettid);

    return ul_tid;
}

/**
 * @brief
 *   Get the local thread ID
 *
 *   Local thread ID = [0, 1, 2, ...]<br>
 *   The local thread ID is unique in the process, and dynamically assigned in the order in which <br>
 *   this API was called during the process execution. <br>
 *
 * @return Local thread ID
 */
uint32_t PbGetLocalTid(void) {
    static uint32_t g_tid[MAX_NUM_CTRL_TID] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
            0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
            0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
            0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF
                                              };  /** In-process thread ID management table */

    uint32_t ul_tid;
    uint32_t ul_idx;
    uint32_t ul_local_tid = 0xFFFFFFFF;

    ul_tid = PbGetTid();

    OtherLockMutex(); /* Get Mutex for accessing shared info */

    for (ul_idx = 0; ul_idx < MAX_NUM_CTRL_TID; ul_idx++) {
        if (g_tid[ul_idx] == ul_tid) {
            ul_local_tid = ul_idx;
        }
    }

    if (ul_local_tid == 0xFFFFFFFF) {
        for (ul_idx = 0; ul_idx < MAX_NUM_CTRL_TID; ul_idx++) {
            if (g_tid[ul_idx] == 0xFFFFFFFF) {
                g_tid[ul_idx] = ul_tid;
                ul_local_tid = ul_idx;

                break;
            }
        }

        if (ul_local_tid == 0xFFFFFFFF) {
            /* forbidden */
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Local Tid buffer is overfull!!");
            _pb_Exit();

            /* don't arrive here. */
        }
    }

    OtherUnlockMutex(); /* Release Mutex for accessing shared info */

    return ul_local_tid;
}


/**
 * @brief
 *   Get Application Handle
 *
 *    Get the application handle of the invoking thread.
 *
 * @return Non-zero:Normal status,  0:When an error occurs
 */
HANDLE _pb_GetAppHandle(void) {     // NOLINT(readability/nolint)  WPF_SYSAPI.h  API
    const uint32_t offset = PbGetLocalTid();

    return g_h_app[offset];
}


/**
 * @brief
 *   Set Application Handle
 *
 *    Set the application handle of the invoking thread.
 *
 * @param[in]  name Process name
 */
void _pb_SetAppHandle(HANDLE h_app) {     // NOLINT(readability/nolint)  WPF_SYSAPI.h  API
    const uint32_t offset = PbGetLocalTid();

    OtherLockMutex(); /* Get Mutex for accessing shared info */

    g_h_app[offset] = h_app;

    OtherUnlockMutex(); /* Release Mutex for accessing shared info */

    return;
}


/**
 * @brief
 *   Convert process name to pno
 *
 *   Translate process name to PNO.<br>
 *   If the process name specified in the argument is the first name to <br>
 *   be translated by this API, a new PNO is assigned and returned. <br>
 *   If NULL is specified, 0 is returned.
 *
 * @param[in]  name Process name
 *
 * @return Process number
 */
PNO _pb_CnvName2Pno(PCSTR name) {     // NOLINT(readability/nolint)  WPF_SYSAPI.h  API
    u_int32 idx;
    PNO pno = 0;
    size_t len;

    /* null check */
    if (name == NULL) {
        pno = 0;
    } else {
        len = _tcslen(name);
        if (len >= THREAD_NAME_LEN_MAX) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argument ERROR!! " \
                "Length of thread name is too long(>=%d). [len:%zu]", THREAD_NAME_LEN_MAX, len);
        } else {
            OtherLockMutex();

            idx = OtherSearchNameOfCnvTbl(name);

            if (idx != MAX_OTHER_PROC_NUM) {
                pno = OtherGetPnoOfCnvTbl(idx);
            } else {
                idx = OtherSearchNameOfCnvTbl("");
                OtherSetNameOfCnvTbl(idx, name);
                /* Increment using-counter */
                OtherIncUseCntOfCnvTbl();
                pno = OtherGetPnoOfCnvTbl(idx);
            }

            OtherUnlockMutex();
        }
    }

    return pno;
}

/**
 * @brief
 *   Convert pno to process name
 *
 *    Translate PNO to the process name.
 *   Return the process name set by _pb_CnvName2Pno to the PNO argument.
 *   If a non-PNO value is given by _pb_CnvName2Pno is specified,
 *   NULL is returned.
 *
 * @param[in]  pno Process number
 *
 * @return Process name
 */
PCSTR _pb_CnvPno2Name(PNO pno) {     // NOLINT(readability/nolint)  WPF_SYSAPI.h  API
    u_int32 idx;
    PCSTR name = NULL;

    OtherLockMutex();

    idx = OtherSearchPnoOfCnvTbl(pno);

    if (idx != MAX_OTHER_PROC_NUM) {
        name = OtherGetNameOfCnvTbl(idx);
    }

    OtherUnlockMutex();

    return name;
}


/**
 * @brief
 *   Get environment variables
 *
 * @param[in]   Environment variable name
 * @param[in]   Pointer to environment variable value
 */
void GetEnv(const char* p_env_str, char* p_env_buff) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    VP_GetEnv(p_env_str, p_env_buff);

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "VP_GetEnv:%s=%s", p_env_str, p_env_buff);

    return;
}
// LCOV_EXCL_STOP

/*---------------------------------------------------------------------------------*
 * Local Function                                                                  *
 *---------------------------------------------------------------------------------*/
/**
 * @brief
 *   PNO setting(The table to translate the process name to PNO)
 *
 *   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 *
 * @param[in]    u_int32 idx        Table accessor
 * @param[in]    PNO    pno    Process number
 *
 * @return none
 */
static void OtherSetPnoOfCnvTbl(u_int32 idx, PNO pno) {
    /* check index */
    if (idx >= MAX_OTHER_PROC_NUM) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argment ERROR [idx:%d, pno:%d]", idx, pno);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater
        /* don't arrive here. */
    }

    g_p_proc_id_tbl->id[idx].pno = pno;

    return;
}

/**
 * @brief
 *   Set process name (The table to translate the process name to PNO)
 *
 *   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 *
 * @param[in]    u_int32    idx        Table accessor
 * @param[in]    PCSTR        name    Process name
 *
 * @return none
 */
static void OtherSetNameOfCnvTbl(u_int32 idx, PCSTR name) {
    /* check index */
    if (idx >= MAX_OTHER_PROC_NUM) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argment ERROR [idx:%d, name:%s]", idx, name);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater
        /* don't arrive here. */
    } else {
        _tcscpy(g_p_proc_id_tbl->id[idx].name, name);
    }

    return;
}

/**
 * @brief
 *   Get PNO (The table to translate the process name to PNO)
 *
 *   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 *
 * @param[in]    u_int32    idx    Table accessor
 *
 * @return PNO
 */
static PNO OtherGetPnoOfCnvTbl(u_int32 idx) {
    /* check index */
    if (idx >= MAX_OTHER_PROC_NUM) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argment ERROR [idx:%d]", idx);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater
        /* don't arrive here. */
    }

    return g_p_proc_id_tbl->id[idx].pno;
}

/**
 * @brief
 *   Get process name (The table to translate the process name to PNO)
 *
 *   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 *
 * @param[in]    u_int32    idx    Table accessor
 *
 * @return PCSTR
 */
static PCSTR OtherGetNameOfCnvTbl(u_int32 idx) {
    /* check index */
    if (idx >= MAX_OTHER_PROC_NUM) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argment ERROR [idx:%d]", idx);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater
        /* don't arrive here. */
    }

    return g_p_proc_id_tbl->id[idx].name;
}

/**
 * @brief
 *   Retrieve PNO (The table to translate the process name to PNO)
 *
 *   If the PNO specified in the argument exists in the table, return the index for access.
 *   If not exists, return the maximum number of local PNO controls (MAX_OTHER_PROC_NUM).
 *
 * @param[in]    PNO    pno    Process number
 *
 * @return u_int32    Table accessor
 */
static u_int32 OtherSearchPnoOfCnvTbl(PNO pno) {
    u_int32 idx;
    PNO lPno;

    for (idx = 0; idx < MAX_OTHER_PROC_NUM; idx++) {
        lPno = OtherGetPnoOfCnvTbl(idx);

        if (lPno == pno) {
            break;
        }
    }

    return idx;
}

/**
 * @brief
 *   Retrieve process name (The table to translate the process name to PNO)
 *
 *   If the process specified by the argument exists in the table, return the index for access.
 *   If not exists, return the maximum number of local PNO controls (MAX_OTHER_PROC_NUM).
 *
 * @param[in]    PCSTR name Process name
 *
 * @return u_int32    Table accessor
 */
static u_int32 OtherSearchNameOfCnvTbl(PCSTR name) {
    int32    ret;
    u_int32 idx;

    for (idx = 0; idx < MAX_OTHER_PROC_NUM; idx++) {
        ret = _tcscmp(g_p_proc_id_tbl->id[idx].name, name);

        /* If there is a match */
        if (ret == 0) {
            break;
        }
    }

    return idx;
}


/**
 * @brief
 *   Create Mutex for accessing shared info
 *
 * @param[in]    none
 *
 * @return none
 */
static void OtherCreateMutex(void) {
    g_h_mtx = _pb_CreateMutex(NULL, 0, "Other_Mutex");
    if (g_h_mtx == NULL) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_CreateMutex ERROR [g_h_mtx:%p]", g_h_mtx);
        _pb_Exit();
        /* don't arrive here. */
    }

    return;
}

/**
 * @brief
 *   Delete Mutex for accessing shared info
 *
 * @param[in]  none
 *
 * @return none
 */
static void OtherDeleteMutex(void) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    DWORD    ret;
    ret = PbDeleteMutex(g_h_mtx);
    if (ret != WAIT_OBJECT_0) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "PbDeleteMutex ERROR " \
            "[ret:%lu, g_h_mtx:%p]", ret, g_h_mtx);
        _pb_Exit();
        /* don't arrive here. */
    }

    return;
}
// LCOV_EXCL_STOP

/**
 * @brief
 *   Get Mutex for accessing shared info
 *
 * @param[in]  none
 *
 * @return none
 */
static void OtherLockMutex(void) {
    DWORD    ret;
    ret = PbMutexLock(g_h_mtx, INFINITE);
    if (ret != WAIT_OBJECT_0) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "PbMutexLock ERROR " \
            "[ret:%lu, g_h_mtx:%p]", ret, g_h_mtx);
        _pb_Exit();
        /* don't arrive here. */
    }

    return;
}

/**
 * @brief
 *   Open Mutex for Accessing Shared Info
 *
 * @param[in]  none
 *
 * @return none
 */
static void OtherUnlockMutex(void) {
    BOOL    ret;
    ret = PbMutexUnlock(g_h_mtx);
    if (ret != TRUE) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "PbMutexUnlock ERROR " \
            "[ret:%d, g_h_mtx:%p]", ret, g_h_mtx);
        _pb_Exit();
        /* don't arrive here. */
    }

    return;
}

/**
 * @brief
 *   Get dump information
 *
 * @param[out] pBuf      Dump info
 */
void _pb_GetDebugOtherMngTbl(void* pBuf) {
    static uint8_t  buf[DEBUG_DUMP_MAX_SIZE];
    static uint8_t  bufTmp[64];
    uint32_t         i;

    if (pBuf != NULL) {
        memset(&buf[0], 0x00, sizeof(buf));
        snprintf(reinterpret_cast<char *>(&buf[0]), sizeof(buf), "Other");
        if (g_p_proc_id_tbl == NULL) {  // LCOV_EXCL_BR_LINE 200: g_p_proc_id_tbl can not be NULL
            // LCOV_EXCL_START 200: g_p_proc_id_tbl can not be NULL
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            strncat(reinterpret_cast<char *>(&buf[0]), "\n NULL", strlen("\n NULL"));
            // LCOV_EXCL_STOP
        } else {
            for (i = 0; i < MAX_OTHER_PROC_NUM; i++) {
                memset(&bufTmp[0], 0x00, sizeof(bufTmp));
                snprintf(reinterpret_cast<char *>(&bufTmp[0]), sizeof(bufTmp),
                        "\n [%02d] pno:0x%04x, name:%s",
                        i,
                        g_p_proc_id_tbl->id[i].pno,
                        g_p_proc_id_tbl->id[i].name);
                strncat(reinterpret_cast<char *>(&buf[0]), reinterpret_cast<char *>(&bufTmp[0]), \
                    strlen(reinterpret_cast<char *>(&bufTmp[0])));
            }
        }
        memcpy(pBuf, &buf[0], sizeof(buf));
    }
}

/**
 * @brief
 *   Increment the usage counter of the table to translate process name to PNO
 *
 * @param[in] none
 */
static void OtherIncUseCntOfCnvTbl(void) {
    g_p_proc_id_tbl->use_cnt++;
    return;
}

/**
 * @brief
 *   Increment the counter to reserve the table to translate the process name to PNO
 *
 * @param[in] none
 */
static void OtherIncRsvCntOfCnvTbl(void) {
    g_p_proc_id_tbl->rsv_cnt++;
    return;
}

/**
 * @brief
 *   Decrement the counter to reserve the table to translate the process name to PNO
 *
 * @param[in] none
 */
static void OtherDecRsvCntOfCnvTbl(void) {
    g_p_proc_id_tbl->rsv_cnt--;
    return;
}

/**
 * @brief
 *   Determine resources ready (The table to translate the process name to PNO)
 *
 * @param[in]  none
 *
 * @return BOOL
 * @retval TRUE  : Normal
 * @retval FALSE : Error (Resource shortage)
 */
BOOL _pb_GetOtherResource(void) {
    BOOL ret = TRUE;
    uint32_t cnt;

    OtherLockMutex();

    /* Increment reserved counter */
    OtherIncRsvCntOfCnvTbl();

    cnt = g_p_proc_id_tbl->use_cnt + g_p_proc_id_tbl->rsv_cnt;
    if (cnt >= FULL_OTHER_PROC_NUM) {
        ret = FALSE;
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Lack of resources " \
            "[FATAL][use_cnt:%d rsv_cnt:%d]", g_p_proc_id_tbl->use_cnt, g_p_proc_id_tbl->rsv_cnt);
    } else if (cnt >= WARN_OTHER_PROC_NUM) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Lack of resources " \
            "[WARN][use_cnt:%d rsv_cnt:%d]", g_p_proc_id_tbl->use_cnt, g_p_proc_id_tbl->rsv_cnt);
    }

    OtherUnlockMutex();

    return ret;
}

/**
 * @brief
 *   Release resources (The table to translate process name to PNO)
 *
 * @param[in]  none
 *
 * @return none
 */
void _pb_ReleaseOtherResource(void) {
    OtherLockMutex();

    /* Decrement reserved counter */
    OtherDecRsvCntOfCnvTbl();

    OtherUnlockMutex();

    return;
}
