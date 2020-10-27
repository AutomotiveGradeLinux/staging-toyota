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
 *    _pbMutex.cpp
 */

/*---------------------------------------------------------------------------------*
 * Include Files                                                                   *
 *---------------------------------------------------------------------------------*/

#ifndef PT_PB_MUTEX_STUB__CWORD71_

#include <vehicle_service/positioning_base_library.h>
#include "_pbInternalProc.h"
#include <native_service/cl_lock.h>
#include <native_service/cl_lockid.h>
#include "WPF_STD_private.h"
#include "tchar.h"

/*---------------------------------------------------------------------------------*
 * Define                                                                          *
 *---------------------------------------------------------------------------------*/
/* Mutex control table name (Shared Memory) */
#define MUTEX_CTRL_TBL_NAME        __TEXT("POS_BASE_MUTEX_TABLE")

/*
   Max. available Lock ID
  Note : Lock ID application is 1 (for BaseAPI initialization) + 1 (for Mutex function control) + 30 (for provision) = 32
*/
#define MAX_CTRL_MUTEX_NUM        (30)

#define FULL_CTRL_MUTEX_NUM (MAX_CTRL_MUTEX_NUM - 4)  /** Mutex control data threshold (no free) */
#define WARN_CTRL_MUTEX_NUM (MAX_CTRL_MUTEX_NUM - 10) /** Mutex control data threshold (warning) */

/*
    ID needs to be aplicated

    If you increase or decrease the IDs, define maximum number of managed LOCK IDs and
    the default setting of "Lock ID Resource List" must also be changed.
*/
#define MUTEX_LOCK_ID_0        LOCK_POS_MTX_2 /* Mutex management-information-exclusive control */
#define MUTEX_LOCK_ID_1        LOCK_POS_MTX_3
#define MUTEX_LOCK_ID_2        LOCK_POS_MTX_4
#define MUTEX_LOCK_ID_3        LOCK_POS_MTX_5
#define MUTEX_LOCK_ID_4        LOCK_POS_MTX_6
#define MUTEX_LOCK_ID_5        LOCK_POS_MTX_7
#define MUTEX_LOCK_ID_6        LOCK_POS_MTX_8
#define MUTEX_LOCK_ID_7        LOCK_POS_MTX_9
#define MUTEX_LOCK_ID_8        LOCK_POS_MTX_10
#define MUTEX_LOCK_ID_9        LOCK_POS_MTX_11
#define MUTEX_LOCK_ID_10    LOCK_POS_MTX_12
#define MUTEX_LOCK_ID_11    LOCK_POS_MTX_13
#define MUTEX_LOCK_ID_12    LOCK_POS_MTX_14
#define MUTEX_LOCK_ID_13    LOCK_POS_MTX_15
#define MUTEX_LOCK_ID_14    LOCK_POS_MTX_16
#define MUTEX_LOCK_ID_15    LOCK_POS_MTX_17
#define MUTEX_LOCK_ID_16    LOCK_POS_MTX_18
#define MUTEX_LOCK_ID_17     LOCK_POS_MTX_19
#define MUTEX_LOCK_ID_18    LOCK_POS_MTX_20
#define MUTEX_LOCK_ID_19    LOCK_POS_MTX_21
#define MUTEX_LOCK_ID_20    LOCK_POS_MTX_22
#define MUTEX_LOCK_ID_21    LOCK_POS_MTX_23
#define MUTEX_LOCK_ID_22    LOCK_POS_MTX_24
#define MUTEX_LOCK_ID_23    LOCK_POS_MTX_25
#define MUTEX_LOCK_ID_24    LOCK_POS_MTX_26
#define MUTEX_LOCK_ID_25    LOCK_POS_MTX_27
#define MUTEX_LOCK_ID_26    LOCK_POS_MTX_28
#define MUTEX_LOCK_ID_27    LOCK_POS_MTX_29
#define MUTEX_LOCK_ID_28    LOCK_POS_MTX_30
#define MUTEX_LOCK_ID_29    LOCK_POS_MTX_31
#define MUTEX_LOCK_ID_30    LOCK_POS_MTX_32

typedef int32 LOCK_ID;

/*---------------------------------------------------------------------------------*
 * Structure                                                                       *
 *---------------------------------------------------------------------------------*/
/*!
  @brief Mutex control information(Interprocess common-details)
 */
typedef struct {
    LOCK_ID      lock_id;            /**< Lock ID                                       */
    TCHAR        name[NAME_MAX]; /**< Mutex Name                                       */
    int32        ref_cnt;            /**< Reference Counter                               */
    uint32_t    thread_id;       /**< Thread IDs while Lock is being acquired (Initial value:0) Note: For debugging */
    BOOL        is_rel_fail;      /**< Release failure information (Failed:TRUE) Note: For debugging  */
    BOOL        is_forbid_access; /**< Deletion occurred during Lock acquisition (Deletion occurred:TRUE) Note: For debugging  */
} MUTEX_CTRL_SHARED_DETAIL;

/*!
  @brief Mutex control information (Interprocess common)
 */
typedef struct {
    MUTEX_CTRL_SHARED_DETAIL   detail[MAX_CTRL_MUTEX_NUM]; /**< Common Infomation  */
    uint32_t    use_cnt;                                    /**< Use Counter        */
    uint32_t    rsv_cnt;                                    /**< Reserve Counter    */
} MUTEX_CTRL_SHARED_INFO;

/*!
   @brief Mutex control information
*/
typedef struct {
    HANDLE    handle[MAX_CTRL_MUTEX_NUM];    /**< Mutex Handle        */
    int32    ref_cnt[MAX_CTRL_MUTEX_NUM];    /**< Reference Counter     */
    MUTEX_CTRL_SHARED_INFO* info;        /**< Common Infomation    */
} MUTEX_CTRL_INFO;

/*---------------------------------------------------------------------------------*
 * Grobal Value                                                                    *
 *---------------------------------------------------------------------------------*/
/**
    Mutex control table pointer (Partial shared memory)
    Note: Access to this instance shall be made through the operation module.
 */
static MUTEX_CTRL_INFO g_mutex_ctrl_tbl;   // NOLINT(readability/nolint)  global class instance

static HANDLE    g_h_mtx; /** Mutex control-information-locking Mutex handle */
static HANDLE    g_h_shm; /** Shared memory handle */   // Coverity CID: 18788 compliant


/** Lock ID Resource List */
static const LOCK_ID kLockIdList[MAX_CTRL_MUTEX_NUM] = {
    MUTEX_LOCK_ID_1,
    MUTEX_LOCK_ID_2,
    MUTEX_LOCK_ID_3,
    MUTEX_LOCK_ID_4,
    MUTEX_LOCK_ID_5,
    MUTEX_LOCK_ID_6,
    MUTEX_LOCK_ID_7,
    MUTEX_LOCK_ID_8,
    MUTEX_LOCK_ID_9,
    MUTEX_LOCK_ID_10,
    MUTEX_LOCK_ID_11,
    MUTEX_LOCK_ID_12,
    MUTEX_LOCK_ID_13,
    MUTEX_LOCK_ID_14,
    MUTEX_LOCK_ID_15,
    MUTEX_LOCK_ID_16,
    MUTEX_LOCK_ID_17,
    MUTEX_LOCK_ID_18,
    MUTEX_LOCK_ID_19,
    MUTEX_LOCK_ID_20,
    MUTEX_LOCK_ID_21,
    MUTEX_LOCK_ID_22,
    MUTEX_LOCK_ID_23,
    MUTEX_LOCK_ID_24,
    MUTEX_LOCK_ID_25,
    MUTEX_LOCK_ID_26,
    MUTEX_LOCK_ID_27,
    MUTEX_LOCK_ID_28,
    MUTEX_LOCK_ID_29,
    MUTEX_LOCK_ID_30,
};

/*
   Mutex property information (Information for each process)
   _pb_CreateMutex calls -> 1
   DeleteMutex calls -> 0
   Only "1" can be used in the process.
 */
BOOL g_is_mutex_owner_tbl[MAX_CTRL_MUTEX_NUM];

/*---------------------------------------------------------------------------------*
 * Local Function Prototype                                                        *
 *---------------------------------------------------------------------------------*/
/* Mutex Control Table Manipulation Functions */
static void        MutexSetLockIdOfCtrlTbl(u_int32 idx, LOCK_ID l_id);         /* Set Lock ID     */
static LOCK_ID    MutexGetLockIdOfCtrlTbl(u_int32 idx);                        /* Get Lock ID     */
static void        MutexSetMutexNameOfCtrlTbl(u_int32 idx, LPCTSTR name);        /* Set Mutex name        */
static void        MutexSetMutexHandleOfCtrlTbl(u_int32 idx, HANDLE handle);     /* Set Mutex handle  */
static HANDLE    MutexGetMutexHandleOfCtrlTbl(u_int32 idx);                     /* Get Mutex Handle  */
static void     MutexSetTidOfCtrlTbl(uint32_t idx, uint32_t tid);          /* Set thread ID while Lock is being acquired     */
static uint32_t MutexGetTidOfCtrlTbl(uint32_t idx);                        /* Get thread ID while Lock is being acquired     */
static void     MutexSetIsRelFailOfCtrlTbl(uint32_t idx, BOOL flag);       /* Set release failure information        */
static void     MutexSetIsForbidAccessOfCtrlTbl(uint32_t idx, BOOL flag);  /* Set deletion occurrence while Lock is being acquired   */
static u_int32    MutexGetIdxOfCtrlTbl(HANDLE h_mutex);       /* Get index for access                   */
static void        MutexIncRefCntOfCtrlTbl(u_int32 idx);         /* Increment Mutex reference counter     */
static void        MutexDecRefCntOfCtrlTbl(u_int32 idx);         /* Decrement Mutex reference counter       */
static int32    MutexGetRefCntOfCtrlTbl(u_int32 idx);         /* Get Mutex reference counter            */
static int32    MutexGetRefCntLocalOfCtrlTbl(u_int32 idx);      /* Get mutex reference counter (in-process) */
static u_int32    MutexSearchNameOfCtrlTbl(LPCTSTR name);      /* Search mutex name       */
static u_int32    MutexSearchEmptyOfCtrlTbl(void);              /* Search unused area            */
static void     MutexIncUseCntOfCtrlInfo(void);             /* Increment Mutex using counter     */
static void     MutexDecUseCntOfCtrlInfo(void);             /* Decrement Mutex using counter     */
static void     MutexIncRsvCntOfCtrlInfo(void);             /* Increment Mutex reserved counter     */
static void     MutexDecRsvCntOfCtrlInfo(void);             /* Decrement Mutex reserved counter     */

/* Mutex ownership-information manipulation functions */
static void    MutexSetFlagOfOwnerTbl(u_int32 idx, BOOL flag); /* Set ownership-information */
static BOOL MutexGetFlagOfOwnerTbl(u_int32 idx);             /* Get ownership-information */

/* Mutex manipulation functions for accessing Mutex control tables */
static void MutexCreateMutex(void);    /* Create Mutex */
static void MutexLockMutex(void);        /* Get Mutex */
static void MutexUnlockMutex(void);    /* Release Mutex */
static void MutexDeleteMutex(void);    /* Delete Mutex */

/*---------------------------------------------------------------------------------*
 * Function                                                                                        *
 *---------------------------------------------------------------------------------*/
/**
 * @brief
 *   Initialize the mutex function
 *
 *   Call the in-process initialization API for locking between processes that provide CLS.<br>
 *   Create shared memory for a Mutex control table.<br>
 *   Create a mutex for accessing Mutex control info.<br>
 *   If an error occurs during internal processing of this API, subsequent normal operations cannot be performed, so call _pb_Exit().
 *
 * @return    RET_NORMAL
 */
RET_API MutexInit(void) {
    RET_API ret_api = RET_NORMAL;
    MUTEX_CTRL_SHARED_INFO **pp_tbl;
    u_int32    idx;

    memset(g_mutex_ctrl_tbl.handle, 0, sizeof(g_mutex_ctrl_tbl.handle));
    memset(g_mutex_ctrl_tbl.ref_cnt, 0, sizeof(g_mutex_ctrl_tbl.ref_cnt));

    /* Set pointer to Mutex control table */
    pp_tbl = &(g_mutex_ctrl_tbl.info);

    MutexCreateMutex(); /* Create Mutex for accessing Mutex control info */

    MutexLockMutex(); /* Get Mutex for accessing Mutex control info */

    /* Open the Mutex control table shared memory */
    g_h_shm = OpenSharedMemory((TCHAR*)(MUTEX_CTRL_TBL_NAME),  // NOLINT(readability/casting)
            sizeof(MUTEX_CTRL_SHARED_INFO));

    /* If called for the first time within all processes, an error occurs and the following processing is performed. */
    if (g_h_shm == NULL) {  // LCOV_EXCL_BR_LINE 200: can not be other val
        /* Create shared memory for Mutex control table */
        // LCOV_EXCL_BR_LINE 200: can not return NULL
        g_h_shm = CreateSharedMemory((TCHAR*)MUTEX_CTRL_TBL_NAME,  // NOLINT(readability/casting)
                                    sizeof(MUTEX_CTRL_SHARED_INFO));
        if (g_h_shm == NULL) /* In case of an error */ {  // LCOV_EXCL_BR_LINE 200: can not return NULL
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_CreateShareData ERROR [hShm:%p]", g_h_shm);
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            _pb_Exit();  // LCOV_EXCL_LINE 200: cannot return NULL
            /* don't arrive here. */
        }
    }

    MutexUnlockMutex(); /* Release of Mutex for accessing Mutex control info */

    /* Set addresses of the acquired shared memory as a pointer of the Mutex control table. */
    *pp_tbl = reinterpret_cast<MUTEX_CTRL_SHARED_INFO*>(GetSharedMemoryPtr(g_h_shm));

    /* Initialize various information of control table */
    for (idx = 0; idx < MAX_CTRL_MUTEX_NUM; idx++) {
        /* Set Lock ID in Mutex control table(Overwrite from the second process onwards) */
        MutexSetLockIdOfCtrlTbl(idx, kLockIdList[idx]);

        MutexSetFlagOfOwnerTbl(idx, FALSE);
    }

    return ret_api;
}

/**
 * @brief
 *   Terminate
 */
RET_API MutexTerm(void) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API ret_api = RET_NORMAL;

    CloseSharedMemory(g_h_shm);

    MutexDeleteMutex();

    return ret_api;
}
// LCOV_EXCL_STOP

/**
 * @brief
 *   Create Mutex
 *
 *   Create a named mutex that can be locked between processes.
 *
 * @param[in]    lp_mutex_attributes    Security descriptor(Unused)
 * @param[in]    b_initial_owner        Initial owner flag(Unused)
 * @param[in]    lp_name                Mutex name
 *
 * @return Except NULL Handle of the created mutex (Management information pointer)<br>
 *         NULL       ABEND
 */
HANDLE _pb_CreateMutex(LPSECURITY_ATTRIBUTES lp_mutex_attributes,   // NOLINT(readability/nolint) WPF_SYSAPI.h API
    BOOL b_initial_owner, LPCTSTR lp_name) {
    int32    idx;            /* For accessing Mutex control table */
    HANDLE   h_mutex = NULL; /* Mutex handle      */
    void    *p_addr;
    int32    errno;
    LOCK_ID  lock_id;
    size_t   len;

    /* Null check */
    if (lp_name == NULL) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argument ERROR!!");
    } else {
        len = _tcslen(lp_name);
        if (len >= NAME_MAX) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argument ERROR!! " \
                "Length of mutex name is too long(>=%d). [len:%zu]", NAME_MAX, len);
        } else {
            MutexLockMutex(); /* Get Mutex for accessing Mutex control info */

            /* Retrieve whether the specified mutex name exists in the Mutex control table */
            idx = MutexSearchNameOfCtrlTbl(lp_name);

            /* Already assigned */
            if (idx != MAX_CTRL_MUTEX_NUM) {
                /* Get mutex handle from Mutex control table */
                h_mutex = MutexGetMutexHandleOfCtrlTbl(idx);

                /* First-time creation within a process */
                if (h_mutex == NULL) {
                    /* Get Lock ID */
                    lock_id = MutexGetLockIdOfCtrlTbl(idx);

                    /* Lock information mapping (CLS) */
                    p_addr = CL_LockMap(lock_id);  // LCOV_EXCL_BR_LINE 4: nsfw error
                    /* In case of an error */
                    if (p_addr == MAP_FAILED) {  // LCOV_EXCL_BR_LINE 4: nsfw error
                        /* Output an error log and return an error value */
                        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, \
                            "CL_LockMap ERROR!! [p_addr:%p, errno:%d]", p_addr, errno);
                    } else {
                        /* Successful allocation */
                        /* Set Mutex handle in the mutex management table */
                        MutexSetMutexHandleOfCtrlTbl(idx, (HANDLE)p_addr);

                        h_mutex = (HANDLE)p_addr;
                    }
                }

                if (h_mutex != NULL) {  // LCOV_EXCL_BR_LINE 200: h_mutex can not be NULL
                    /* Increment Mutex reference counter */
                    MutexIncRefCntOfCtrlTbl(idx);

                    FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, 
                                   "### MUTEX TABLE INFORMATION # (+) idx:%d HANDLE:%p, " \
                                   "ref_cnt:%d, LOCK_ID:%d, NAME:%s, ref_cnt:%d, tid:%d, fail:%d, forbid:%d",
                                   idx,
                                   g_mutex_ctrl_tbl.handle[idx],
                                   g_mutex_ctrl_tbl.ref_cnt[idx],
                                   g_mutex_ctrl_tbl.info->detail[idx].lock_id,
                                   g_mutex_ctrl_tbl.info->detail[idx].name,
                                   g_mutex_ctrl_tbl.info->detail[idx].ref_cnt,
                                   g_mutex_ctrl_tbl.info->detail[idx].thread_id,
                                   g_mutex_ctrl_tbl.info->detail[idx].is_rel_fail,
                                   g_mutex_ctrl_tbl.info->detail[idx].is_forbid_access);
                }
            } else {
                /* For a new assignment */
                /* Get the free space in the mutex management table. */
                idx = MutexSearchEmptyOfCtrlTbl();

                /* Get Lock ID */
                lock_id = MutexGetLockIdOfCtrlTbl(idx);

                /* Lock information mapping (CLS) */
                p_addr = CL_LockMap(lock_id);

                /* In case of an error */
                if (p_addr == MAP_FAILED) {  // LCOV_EXCL_BR_LINE 4: nsfw error
                    /* Output an error log and return an error value. */
                    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "CL_LockMap ERROR!! " \
                        "[p_addr:%p, errno:%d] In _pb_CreateMutex", p_addr, errno);
                } else {
                    /* Successful allocation */
                    /* Set Mutex Hanlde in the mutex management table */
                    MutexSetMutexHandleOfCtrlTbl(idx, (HANDLE)p_addr);

                    /* Set the mutex name in the mutex management table */
                    MutexSetMutexNameOfCtrlTbl(idx, lp_name);

                    /* Increment Mutex reference counter */
                    MutexIncRefCntOfCtrlTbl(idx);

                    /* Increment mutex using counter */
                    MutexIncUseCntOfCtrlInfo();

                    h_mutex = (HANDLE)p_addr;

                    FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, 
                                   "### MUTEX TABLE INFORMATION # (++) idx:%d HANDLE:%p, ref_cnt:%d, " \
                                   "LOCK_ID:%d, NAME:%s, ref_cnt:%d, tid:%d, fail:%d, forbid:%d",
                                   idx,
                                   g_mutex_ctrl_tbl.handle[idx],
                                   g_mutex_ctrl_tbl.ref_cnt[idx],
                                   g_mutex_ctrl_tbl.info->detail[idx].lock_id,
                                   g_mutex_ctrl_tbl.info->detail[idx].name,
                                   g_mutex_ctrl_tbl.info->detail[idx].ref_cnt,
                                   g_mutex_ctrl_tbl.info->detail[idx].thread_id,
                                   g_mutex_ctrl_tbl.info->detail[idx].is_rel_fail,
                                   g_mutex_ctrl_tbl.info->detail[idx].is_forbid_access);
                }
            }

            if (h_mutex != NULL) {  // LCOV_EXCL_BR_LINE 200: can not be NULL
                /* Update Mutex ownership */
                MutexSetFlagOfOwnerTbl(idx, TRUE);
            }

            /* Release the Mutex for accessing Mutex control info */
            MutexUnlockMutex();
        }
    }

    return h_mutex;
}

/**
 * @brief
 *   Delete the mutex
 *
 *   Decrement the Mutex reference counter and delete it when it reaches zero.
 *
 * @param[in]    h_mutex    Mutex handle (CreateMutex return value)
 *
 * @return WAIT_OBJECT_0    Normal completion<br>
 *              WAIT_FAILED        ABEND
 */
DWORD PbDeleteMutex(HANDLE h_mutex) {
    DWORD lret = WAIT_OBJECT_0;
    u_int32 idx;
    int32    ref_cnt;
    int32    ret;
    BOOL    is_owner;
    uint32_t tid;

    /* Null check */
    if (h_mutex == NULL) {  // LCOV_EXCL_BR_LINE 6: h_mutex cannot be NULL
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,  "Argment ERROR");
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        lret = WAIT_FAILED; /* ABEND */  // LCOV_EXCL_LINE 6: h_mutex cannot be NULL
    } else {
        MutexLockMutex(); /* Get Mutex for accessing Mutex control info */

        /* Retrieve the index of the management table containing the Mutex handles */
        idx = MutexGetIdxOfCtrlTbl(h_mutex);
        /* When the specified Mutex handle is not registered */
        if (idx == MAX_CTRL_MUTEX_NUM) {  // LCOV_EXCL_BR_LINE 200: idx can no be MAX_CTRL_MUTEX_NUM
            FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, "Argment ERROR [h_mutex:%p]", h_mutex);
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            lret = WAIT_FAILED; /* ABEND */    // LCOV_EXCL_LINE 200: idx can no be MAX_CTRL_MUTEX_NUM
        } else {
            /* If it is registered */
            /* Determinate its ownership */
            is_owner = MutexGetFlagOfOwnerTbl(idx);
            if (is_owner == TRUE) {
                /* Get Mutex reference counter */
                ref_cnt = MutexGetRefCntOfCtrlTbl(idx);

                /* No clients are using the specified Mutex. */
                if ((ref_cnt - 1) <= 0) {
                    /* Lock information unmapping (CLS) */
                    ret = CL_LockUnmap(reinterpret_cast<void*>(h_mutex));  // LCOV_EXCL_BR_LINE 4: nsfw error
                    /* When an error occurs */
                    if (ret == -1) {  // LCOV_EXCL_BR_LINE 4: nsfw error
                        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "CL_LockUnmap ERROR");
                        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                        lret = WAIT_FAILED; /* ABEND */  // LCOV_EXCL_LINE 4: nsfw error
                    } else {
                        /* If successful */
                        /* Remove Mutex hanlde from Mutex control table */
                        MutexSetMutexHandleOfCtrlTbl(idx, NULL);

                        /* Delete the mutex name from the Mutex control table */
                        MutexSetMutexNameOfCtrlTbl(idx, "");

                        /* Update Mutex ownership */
                        MutexSetFlagOfOwnerTbl(idx, FALSE);

                        /* Decrement Mutex reference counter */
                        MutexDecRefCntOfCtrlTbl(idx);

                        /* Decrement mutex using counter */
                        MutexDecUseCntOfCtrlInfo();

                        tid = MutexGetTidOfCtrlTbl(idx);
                        if (tid != 0) {  // LCOV_EXCL_BR_LINE 200: tid can not be 0
                            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                            MutexSetIsForbidAccessOfCtrlTbl(idx, TRUE);  // LCOV_EXCL_LINE 200: tid can not be 0
                        }

                        /* Initialize debug information */
                        MutexSetTidOfCtrlTbl(idx, 0);
                        MutexSetIsRelFailOfCtrlTbl(idx, FALSE);

                        FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, 
                                       "### MUTEX TABLE INFORMATION # (--) idx:%d HANDLE:%p, ref_cnt(local):%d, " \
                                       "LOCK_ID:%d, NAME:%s, ref_cnt:%d, tid:%d, fail:%d, forbid:%d",
                                       idx,
                                       g_mutex_ctrl_tbl.handle[idx],
                                       g_mutex_ctrl_tbl.ref_cnt[idx],
                                       g_mutex_ctrl_tbl.info->detail[idx].lock_id,
                                       g_mutex_ctrl_tbl.info->detail[idx].name,
                                       g_mutex_ctrl_tbl.info->detail[idx].ref_cnt,
                                       g_mutex_ctrl_tbl.info->detail[idx].thread_id,
                                       g_mutex_ctrl_tbl.info->detail[idx].is_rel_fail,
                                       g_mutex_ctrl_tbl.info->detail[idx].is_forbid_access);
                    }
                } else {
                    /* If exists */
                    /* Determine if any clients are referencing in the process */
                    ref_cnt = MutexGetRefCntLocalOfCtrlTbl(idx);
                    if ((ref_cnt - 1) <= 0) {  // LCOV_EXCL_BR_LINE 200: ref_cnt can not bigger than 1
                        // LCOV_EXCL_START 200: ref_cnt can not bigger than 1
                        //AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                        /* Lock information unmapping (CLS) */
                        ret = CL_LockUnmap(reinterpret_cast<void*>(h_mutex));
                        /* When an error occurs */
                        if (ret == -1) {
                            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "CL_LockUnmap ERROR");
                            lret = WAIT_FAILED; /* ABEND */
                        } else {
                            /* If successful */
                            /* Remove Mutex Hanlde from the Mutex control table */
                            MutexSetMutexHandleOfCtrlTbl(idx, NULL);

                            /* Update Mutex ownership */
                            MutexSetFlagOfOwnerTbl(idx, FALSE);
                        }
                        // LCOV_EXCL_STOP
                    }

                    if (lret == WAIT_OBJECT_0) {
                        /* Decrement Mutex refernce counter */
                        MutexDecRefCntOfCtrlTbl(idx);

                        FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, 
                                       "### MUTEX TABLE INFORMATION # (-) idx:%d HANDLE:%p, ref_cnt(local):%d, " \
                                       "LOCK_ID:%d, NAME:%s, ref_cnt:%d, tid:%d, fail:%d, forbid:%d",
                                       idx,
                                       g_mutex_ctrl_tbl.handle[idx],
                                       g_mutex_ctrl_tbl.ref_cnt[idx],
                                       g_mutex_ctrl_tbl.info->detail[idx].lock_id,
                                       g_mutex_ctrl_tbl.info->detail[idx].name,
                                       g_mutex_ctrl_tbl.info->detail[idx].ref_cnt,
                                       g_mutex_ctrl_tbl.info->detail[idx].thread_id,
                                       g_mutex_ctrl_tbl.info->detail[idx].is_rel_fail,
                                       g_mutex_ctrl_tbl.info->detail[idx].is_forbid_access);
                    }
                }

            } else {  // LCOV_EXCL_BR_LINE 200: is_owner can not be FALSE
                /* If it is not the owner */
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                lret = WAIT_FAILED;  // LCOV_EXCL_LINE 200: is_owner can not be FALSE
            }
        }

        MutexUnlockMutex(); /* Release of Mutex for accessing Mutex control info */
    }

    return lret;
}

/**
 * @brief
 *   Mutex Lock
 *
 *   Take ownership of the mutex and start locking.
 *
 * @param[in]    h_mutex    Mutex handle(CreateMutex return value)
 * @param[in]    timeout    Timeout (Millisecond)
 *
 * @return WAIT_OBJECT_0    Succeeded to get ownership<br>
 *              WAIT_TIMEOUT    Failed to get ownership (Timeout)<br>
 *              WAIT_FAILED        Failed to get ownership (Error)
 */
DWORD PbMutexLock(HANDLE h_mutex, DWORD timeout) {
    DWORD    lret = WAIT_OBJECT_0;
    int32    ret;
    DWORD    time_out_cnt = 0;
    u_int32    idx;
    BOOL    is_owner;
    uint32_t tid;

    /* Null check */
    if (h_mutex == NULL) {  // LCOV_EXCL_BR_LINE 6: h_mutex cannot be NULL
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argment ERROR");
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        lret = WAIT_FAILED; /* ABEND */  // LCOV_EXCL_LINE 6: h_mutex cannot be NULL
    } else {
        /* Retrieve the index of the management table containing the Mutex handles */
        idx = MutexGetIdxOfCtrlTbl(h_mutex);
        /* When the specified Mutex handle is not registered */
        if (idx == MAX_CTRL_MUTEX_NUM) {  // LCOV_EXCL_BR_LINE 6: idx cannot be MAX_CTRL_MUTEX_NUM
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argment ERROR [h_mutex:%p, idx:%d]", \
                    h_mutex, idx);
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            lret = WAIT_FAILED; /* ABEND */  // LCOV_EXCL_LINE 6: idx cannot be MAX_CTRL_MUTEX_NUM
        } else {
            /* If it is registered */
            /* Determinate ownership */
            is_owner = MutexGetFlagOfOwnerTbl(idx);
            if (is_owner == TRUE) {
                tid = PbGetTid();

                /* No timeout specified */
                if (timeout == INFINITE) {  // LCOV_EXCL_BR_LINE 6: timeout will not be other case
                    /* Get Lock (CLS) */
                    ret = CL_LockGet(reinterpret_cast<void*>(h_mutex));  // LCOV_EXCL_BR_LINE 4: nsfw error
                    if (ret != 0) /* In case of ABEND */ {  // LCOV_EXCL_BR_LINE 4: nsfw error
                        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, \
                            "CL_LockGet ERROR [h_mutex:%p, ret:%d]", h_mutex, ret);
                        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                        lret = WAIT_FAILED; /* ABEND */  // LCOV_EXCL_LINE 4: nsfw error
                    } else {
                        /* Set thread ID during lock */
                        MutexSetTidOfCtrlTbl(idx, tid);
                    }
                } else {
                    // LCOV_EXCL_START 6: timeout will not be other case
                    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                    /* Timeout specified */
                    while (1) {
                        /* Get Lock (CLS) */
                        ret = CL_LockNowait(reinterpret_cast<void*>(h_mutex));
                        if (ret != 0) /* In case of ABEND */ {
                            /* Invalid parameter */
                            if (ret != EBUSY) {
                                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, \
                                    "CL_LockGet ERROR [h_mutex:%p, ret:%d]", h_mutex, ret);
                                lret = WAIT_FAILED; /* ABEND */
                                break;
                            }

                            time_out_cnt++;
                            if (time_out_cnt <= timeout) {
                                usleep(1000);
                            } else {
                                /* Timeout error */
                                lret = WAIT_TIMEOUT;
                                break;
                            }
                        } else {
                            /* Successful acquisition */
                            /* Set thread ID during lock */
                            MutexSetTidOfCtrlTbl(idx, tid);

                            break;
                        }
                    }
                    // LCOV_EXCL_STOP
                }
            } else {  // LCOV_EXCL_BR_LINE 200: is_owner can not be FALSE
                /* If it is not the owner */
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                lret = WAIT_FAILED;  // LCOV_EXCL_LINE 200: is_owner can not be FALSE
            }
        }
    }

    return lret;
}

/**
 * @brief
 *   Mutex Unlock
 *
 *   Release the mutex ownership and terminate the lock.
 *
 * @param[in]    h_mutex    Mutex handle(CreateMutex return value)
 *
 * @return TURE    Normal<br>
 *              FALSE    Error
 */
BOOL PbMutexUnlock(HANDLE h_mutex) {
    BOOL     bret = FALSE;
    int32    ret;
    u_int32  idx;
    BOOL    is_owner;
    uint32_t tid;

    /* Null check */
    if (h_mutex == NULL) {  // LCOV_EXCL_BR_LINE 6: h_mutex can not be NULL
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argment ERROR");
    } else {
        /* Retrieve the index of the management table containing the Mutex handle */
        idx = MutexGetIdxOfCtrlTbl(h_mutex);
        /* When the specified Mutex handle is not registered */
        if (idx == MAX_CTRL_MUTEX_NUM) {  // LCOV_EXCL_BR_LINE 200: idx can not be MAX_CTRL_MUTEX_NUM
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, \
                "MutexGetIdxOfCtrlTbl ERROR [h_mutex:%p, idx:%d]", h_mutex, idx);
        } else {
            /* If it is registered */
            /* Determinate ownership */
            is_owner = MutexGetFlagOfOwnerTbl(idx);
            if (is_owner == TRUE) {  // LCOV_EXCL_BR_LINE 4: nsfw error
                /* Release thread ID setting during lock */
                MutexSetTidOfCtrlTbl(idx, 0);

                /* Release the Lock */
                ret = CL_LockRelease(reinterpret_cast<void*>(h_mutex));
                if (ret == 0) /* If successful */ {  // LCOV_EXCL_BR_LINE 4: nsfw error
                    bret = TRUE;

                } else {  // LCOV_EXCL_BR_LINE 4: nsfw error
                    // LCOV_EXCL_START 4: nsfw error
                    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                    /* Failed */
                    tid = PbGetTid();

                    /* Retry to set the thread ID during lock */
                    MutexSetTidOfCtrlTbl(idx, tid);

                    /* Add the release failure information */
                    MutexSetIsRelFailOfCtrlTbl(idx, TRUE);

                    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, \
                        "CL_LockRelease ERROR [h_mutex:%p, ret:%d]", h_mutex, ret);
                    // LCOV_EXCL_STOP
                }
            }
        }
    }

    return bret;
}

/*---------------------------------------------------------------------------------*
 * Local Function                                                                  *
 *---------------------------------------------------------------------------------*/
/**
 * @brief
 *   Set the Lock ID (Mutex control table)
 *
 *   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 *
 * @param[in]    idx    Control table accessor
 * @param[in]    l_id    Lock ID
 */
static void MutexSetLockIdOfCtrlTbl(u_int32 idx, LOCK_ID l_id) {
    /* check index */
    if (idx >= MAX_CTRL_MUTEX_NUM) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argment ERROR [idx:%d, l_id:%d]", idx, l_id);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater
        /* don't arrive here. */
    } else {
        g_mutex_ctrl_tbl.info->detail[idx].lock_id = l_id;
    }

    return;
}

/**
 * @brief
 *   Get the Lock ID (Mutex control table)
 *
 *   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 *
 * @param[in]    idx    Control table accessor
 */
static LOCK_ID MutexGetLockIdOfCtrlTbl(u_int32 idx) {
    /* check index */
    if (idx >= MAX_CTRL_MUTEX_NUM) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argment ERROR [idx:%d]", idx);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater
        /* don't arrive here. */
    }

    return g_mutex_ctrl_tbl.info->detail[idx].lock_id;
}

/**
 * @brief
 *   Set the Mutex name (Mutex control table)
 *
 *   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 *
 * @param[in]    idx        Control table accessor
 * @param[in]    name    Mutex name
 */
static void MutexSetMutexNameOfCtrlTbl(u_int32 idx, LPCTSTR name) {
    /* check index */
    if (idx >= MAX_CTRL_MUTEX_NUM) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argment ERROR [idx:%d, name:%s]", idx, name);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater
        /* don't arrive here. */
    } else {
        _tcscpy(g_mutex_ctrl_tbl.info->detail[idx].name, name);
    }

    return;
}

/**
 * @brief
 *   Set the Mutex handle (Mutex control table)
 *
 *   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 *
 * @param[in]    idx        Control table accessor
 * @param[in]    handle    Mutex handle
 */
static void MutexSetMutexHandleOfCtrlTbl(u_int32 idx, HANDLE handle) {
    /* check index */
    if (idx >= MAX_CTRL_MUTEX_NUM) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argment ERROR [idx:%d, handle:%p]", idx, handle);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater
        /* don't arrive here. */
    } else {
        g_mutex_ctrl_tbl.handle[idx] = handle;
    }

    return;
}

/**
 * @brief
 *   Get the Mutex handle (Mutex control table)
 *
 *   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 *
 * @param[in]    idx    Control table accessor
 *
 * @return Mutex handle
 */
static HANDLE MutexGetMutexHandleOfCtrlTbl(u_int32 idx) {
    HANDLE handle = NULL;

    /* check index */
    if (idx >= MAX_CTRL_MUTEX_NUM) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argment ERROR [idx:%d]", idx);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater
        /* don't arrive here. */
    } else {
        handle = g_mutex_ctrl_tbl.handle[idx];
    }

    return handle;
}

/**
 * @brief
 *   Set the thread ID during lock acquisition (Mutex control table)
 *
 * @param[in]    idx    Control table accessor
 * @param[in]    tid    Thread ID
 * @note   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 */
static void MutexSetTidOfCtrlTbl(uint32_t idx, uint32_t tid) {
    /* check index */
    if (idx >= MAX_CTRL_MUTEX_NUM) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argument ERROR [idx:%d]", idx);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater
        /* don't arrive here. */
    } else {
        g_mutex_ctrl_tbl.info->detail[idx].thread_id = tid;
    }

    return;
}


/**
 * @brief
 *   Get the thread ID during lock acquisition (Mutex control table)
 *
 * @param[in]    idx    Control table accessor
 *
 * @return Thread ID
 *
 * @note   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 */
static uint32_t MutexGetTidOfCtrlTbl(uint32_t idx) {
    uint32_t tid = 0;

    /* check index */
    if (idx >= MAX_CTRL_MUTEX_NUM) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argument ERROR [idx:%d]", idx);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater
        /* don't arrive here. */
    } else {
        tid = g_mutex_ctrl_tbl.info->detail[idx].thread_id;
    }

    return tid;
}


/**
 * @brief
 *   Set the release failure information (Mutex control table)
 *
 * @param[in]    idx     Control table accessor
 * @param[in]   flag   Failed to release:TURE
 * @note   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 */
static void MutexSetIsRelFailOfCtrlTbl(uint32_t idx, BOOL flag) {
    /* check index */
    if (idx >= MAX_CTRL_MUTEX_NUM) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argument ERROR [idx:%d]", idx);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater
        /* don't arrive here. */
    } else {
        g_mutex_ctrl_tbl.info->detail[idx].is_rel_fail = flag;
    }

    return;
}

/**
 * @brief
 *   Set deletion-occurrence information during Lock acquisition (Mutex control table)
 *
 * @param[in]    idx     Control table accessor
 * @param[in]   flag  When deletion occurs while Lock is being acquired:TRUE
 * @note   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 */
static void MutexSetIsForbidAccessOfCtrlTbl(uint32_t idx, BOOL flag) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    /* check index */
    if (idx >= MAX_CTRL_MUTEX_NUM) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argument ERROR [idx:%d]", idx);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater
        /* don't arrive here. */
    } else {
        g_mutex_ctrl_tbl.info->detail[idx].is_forbid_access = flag;
    }

    return;
}
// LCOV_EXCL_STOP

/**
 * @brief
 *   Get index for access (Mutex control table)
 *
 *   Return the index for accessing the area of the Mutex control table in which <br>
 *   the specified mutex handle is registered. If the specified handle is not <br>
 *   registered, the maximum number of mutex management (MAX_MUTEX_CTRL_NUM) is returned.
 *
 * @param[in]    h_mutex Mutex handle
 *
 * @return Index for access (Specified handle is already registered)<br>
 *              Maximum mutex management value (Specified handle is not registered)
 */
static u_int32 MutexGetIdxOfCtrlTbl(HANDLE h_mutex) {
    u_int32 idx;
    HANDLE handle;

    for (idx = 0; idx < MAX_CTRL_MUTEX_NUM; idx++) {
        /* Get handle from Mutex control table */
        handle = MutexGetMutexHandleOfCtrlTbl(idx);

        if (handle == h_mutex) {
            break;
        }
    }

    return idx;
}

/**
 * @brief
 *   Increment the Mutex reference counter (Mutex control table)
 *
 *   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 *
 * @param[in]    idx Control table accessor
 */
static void MutexIncRefCntOfCtrlTbl(u_int32 idx) {
    /* check index */
    if (idx >= MAX_CTRL_MUTEX_NUM) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argment ERROR [idx:%d]", idx);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater
        /* don't arrive here. */
    }

    g_mutex_ctrl_tbl.info->detail[idx].ref_cnt++;
    g_mutex_ctrl_tbl.ref_cnt[idx]++;

    return;
}

/**
 * @brief
 *   Decrement the Mutex reference counter (Mutex control table)
 *
 *   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 *
 * @param[in]    idx Control table accessor
 */
static void MutexDecRefCntOfCtrlTbl(u_int32 idx) {
    /* check index */
    if (idx >= MAX_CTRL_MUTEX_NUM) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argment ERROR [idx:%d]", idx);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater
        /* don't arrive here. */
    }

    g_mutex_ctrl_tbl.info->detail[idx].ref_cnt--;
    g_mutex_ctrl_tbl.ref_cnt[idx]--;

    return;
}

/**
 * @brief
 *   Get the Mutex Reference Counter (Mutex control table)
 *
 *   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 *
 * @param[in]    idx Control table accessor
 *
 * @return Reference counter value
 */
static int32 MutexGetRefCntOfCtrlTbl(u_int32 idx) {
    int32 ret = 0;

    /* check index */
    if (idx >= MAX_CTRL_MUTEX_NUM) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argment ERROR [idx:%d]", idx);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater
        /* don't arrive here. */
    } else {
        ret = g_mutex_ctrl_tbl.info->detail[idx].ref_cnt;
    }

    return ret;
}

/**
 * @brief
 *   Get mutex reference counter (in-process)(Mutex control table)
 *
 *   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 *
 * @param[in]    idx Control table accessor
 *
 * @return Reference counter value
 */
static int32 MutexGetRefCntLocalOfCtrlTbl(u_int32 idx) {
    /* check index */
    if (idx >= MAX_CTRL_MUTEX_NUM) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argment ERROR [idx:%d]", idx);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater
        /* don't arrive here. */
    }

    return g_mutex_ctrl_tbl.ref_cnt[idx];
}

/**
 * @brief
 *   Search mutex name (Mutex control table)
 *
 *   Retrieve whether the specified mutex is already registered in the Mutex control table.<br>
 *   If it is registered, the access index is returned.If it is not registered,<br>
 *   return the maximum mutex management value(MAX_CTRL_MUTEX_NUM).
 *
 * @param[in]    name Mutex name
 *
 * @return Index for access(If it is registered)<br>
 *              Maximum mutex management value (Not registered)
 */
static u_int32 MutexSearchNameOfCtrlTbl(LPCTSTR name) {
    int32    ret;
    u_int32 idx;

    for (idx = 0; idx < MAX_CTRL_MUTEX_NUM; idx++) {
        ret = _tcscmp(g_mutex_ctrl_tbl.info->detail[idx].name, name);

        /* If there is a match */
        if (ret == 0) {
            break;
        }
    }

    return idx;
}

/**
 * @brief
 *   Search unused area (Mutex control table)
 *
 *   Return the lowest-numbered index for accessing unused space in the Mutex control table.<br>
 *   Return the maximum value((MAX_CTRL_MUTEX_NUM)) of message queue management <br>
 *   when no unused area exists. 
 *
 * @return Index for access (Unused area exists)<br>
 *              Maximum mutex management value (No unused area)
 */
static u_int32 MutexSearchEmptyOfCtrlTbl(void) {
    u_int32    idx;

    idx = MutexSearchNameOfCtrlTbl("");

    return idx;
}

/**
 * @brief
 *   Set the Mutex ownership flag (Ownership-information table)
 *
 * @param[in]    idx    Control table accessor
 * @param[in]    flag ownership-information
 */
static void MutexSetFlagOfOwnerTbl(u_int32 idx, BOOL flag) {
    /* check index */
    if (idx >= MAX_CTRL_MUTEX_NUM) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argment ERROR [idx:%d, flag:%d]", idx, flag);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater
        /* don't arrive here. */
    } else {
        g_is_mutex_owner_tbl[idx] = flag;
    }

    return;
}

/**
 * @brief
 *   Get the Mutex ownership flag (Ownership-information table)
 *
 * @param[in]    idx    Control table accessor
 *
 * @return    TRUE    Owned
 *            FALSE    Not owned
 */
static BOOL MutexGetFlagOfOwnerTbl(u_int32 idx) {
    BOOL    bret = FALSE;

    /* check index */
    if (idx >= MAX_CTRL_MUTEX_NUM) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argment ERROR [idx:%d]", idx);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater
        /* don't arrive here. */
    } else {
        bret = g_is_mutex_owner_tbl[idx];
    }

    return bret;
}

/**
 * @brief
 *   Create Mutex for accessing the Mutex control table
 */
static void MutexCreateMutex(void) {
    g_h_mtx = (HANDLE)CL_LockMap(MUTEX_LOCK_ID_0);  // LCOV_EXCL_BR_LINE 4: nsfw error
    if (g_h_mtx == NULL) {    // LCOV_EXCL_BR_LINE 4: nsfw error
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "CL_LockMap ERROR [g_h_mtx:%p]", g_h_mtx);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 4: nsfw error
        /* don't arrive here. */
    }

    return;
}

/**
 * @brief
 *   Delete Mutex for accessing Mutex control table
 */
static void MutexDeleteMutex(void) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    int32 ret;

    ret = CL_LockUnmap(g_h_mtx);
    if (ret != 0) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, \
            "CL_LockUnmap ERROR [g_h_mtx:%p, ret:%d]", g_h_mtx, ret);
        _pb_Exit();
        /* don't arrive here. */
    }

    return;
}
// LCOV_EXCL_STOP

/**
 * @brief
 *   Get Mutex for accessing Mutex control table
 */
static void MutexLockMutex(void) {
    int32 ret;

    ret = CL_LockGet(g_h_mtx);  // LCOV_EXCL_BR_LINE 4: nsfw error
    if (ret != 0) {  // LCOV_EXCL_BR_LINE 4: nsfw error
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, \
            "CL_LockGet ERROR [g_h_mtx:%p, ret:%d]", g_h_mtx, ret);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 4: nsfw error
        /* don't arrive here. */
    }

    return;
}

/**
 * @brief
 *   Release Mutex for accessing Mutex control table
 */
static void MutexUnlockMutex(void) {
    int32 ret;

    ret = CL_LockRelease(g_h_mtx);  // LCOV_EXCL_BR_LINE 4: nsfw error
    if (ret != 0) {  // LCOV_EXCL_BR_LINE 4: nsfw error
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, \
            "CL_LockRelease ERROR [g_h_mtx:%p, ret:%d]", g_h_mtx, ret);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 4: nsfw error
        /* don't arrive here. */
    }

    return;
}

/**
 * @brief
 *   Get dump information
 *
 * @param[out]    p_buf      Dump info
 * @param[in/out] p_len      Buffer size
 */
void _pb_GetDebugMutexMngTbl(void* p_buf, uint8_t* p_len) {
    static uint8_t buf[DEBUG_DUMP_MAX_SIZE];
    static uint8_t buf_tmp[512];
    static uint8_t buf_info[DEBUG_DUMP_MAX_SIZE];
    uint32_t        i;
    uint8_t         cnt = 0;

    if ((p_buf != NULL) && (p_len != NULL)) {
        memset(&buf[0], 0x00, sizeof(buf));
        memset(&buf_info[0], 0x00, sizeof(buf_info));
        for (i = 0; i < MAX_CTRL_MUTEX_NUM; i++) {
            memset(&buf_tmp[0], 0x00, sizeof(buf_tmp));
            snprintf(reinterpret_cast<char *>(&buf_tmp[0]), sizeof(buf_tmp),
                    "\n  [%02d]id:%10d, nm:%40s, ref_cnt:%10d, tid:%03d, fail:%01d, " \
                    "forbid:%01d, handle:%10p, ref_cnt(local):%10d",
                    i,
                    g_mutex_ctrl_tbl.info->detail[i].lock_id,
                    g_mutex_ctrl_tbl.info->detail[i].name,
                    g_mutex_ctrl_tbl.info->detail[i].ref_cnt,
                    g_mutex_ctrl_tbl.info->detail[i].thread_id,
                    g_mutex_ctrl_tbl.info->detail[i].is_rel_fail,
                    g_mutex_ctrl_tbl.info->detail[i].is_forbid_access,
                    g_mutex_ctrl_tbl.handle[i],
                    g_mutex_ctrl_tbl.ref_cnt[i]);
            strncat(reinterpret_cast<char *>(&buf_info[0]), reinterpret_cast<char *>(&buf_tmp[0]), \
                strlen(reinterpret_cast<char *>(&buf_tmp[0])));
            if (((i + 1) % 10) == 0) {
                cnt++;
                memset(&buf[0], 0x00, sizeof(buf));
                snprintf(reinterpret_cast<char *>(&buf[0]), sizeof(buf),
                        "Mutex-%d%s",
                        cnt,
                        &buf_info[0]);
                memcpy(p_buf, &buf[0], sizeof(buf));
                p_buf = reinterpret_cast<void *>((reinterpret_cast<uint8_t *>(p_buf)) + sizeof(buf));
                memset(&buf_info[0], 0x00, sizeof(buf_info));
                if (cnt >= *p_len) {
                    break;
                }
            }
        }
        if (cnt < *p_len) {
            if (buf_info[0] != 0x00) {
                cnt++;
                memset(&buf[0], 0x00, sizeof(buf));
                snprintf(reinterpret_cast<char *>(&buf[0]), sizeof(buf),
                        "Mutex-%d%s",
                        cnt,
                        &buf_info[0]);
                memcpy(p_buf, &buf[0], sizeof(buf));
            }
            *p_len = cnt;
        }
    }
}

/**
 * @brief
 *   Increment the Mutex using counter
 *
 * @param[in] none
 */
static void MutexIncUseCntOfCtrlInfo(void) {
    g_mutex_ctrl_tbl.info->use_cnt++;
    return;
}

/**
 * @brief
 *   Decrement Mutex using counter
 *
 * @param[in] none
 */
static void MutexDecUseCntOfCtrlInfo(void) {
    g_mutex_ctrl_tbl.info->use_cnt--;
    return;
}

/**
 * @brief
 *   Increment Mutex reserved counter
 *
 * @param[in] none
 */
static void MutexIncRsvCntOfCtrlInfo(void) {
    g_mutex_ctrl_tbl.info->rsv_cnt++;
    return;
}

/**
 * @brief
 *   Decrement Mutex reserved counter
 *
 * @param[in] none
 */
static void MutexDecRsvCntOfCtrlInfo(void) {
    g_mutex_ctrl_tbl.info->rsv_cnt--;
    return;
}

/**
 * @brief
 *   Determine resources ready (Mutex control data)
 *
 * @param[in]  none
 *
 * @return BOOL
 * @retval TRUE  : Normal
 * @retval FALSE : Error (Resource shortage)
 */
BOOL _pb_GetMutexResource(void) {
    BOOL ret = TRUE;
    uint32_t cnt;

    MutexLockMutex();

    /* Increment Mutex reserved counter */
    MutexIncRsvCntOfCtrlInfo();

    cnt = g_mutex_ctrl_tbl.info->use_cnt + g_mutex_ctrl_tbl.info->rsv_cnt;
    if (cnt >= FULL_CTRL_MUTEX_NUM) {
        ret = FALSE;
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, \
            "Lack of resources [FATAL][use_cnt:%d rsv_cnt:%d]", g_mutex_ctrl_tbl.info->use_cnt, \
            g_mutex_ctrl_tbl.info->rsv_cnt);
    } else if (cnt >= WARN_CTRL_MUTEX_NUM) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, \
            "Lack of resources [WARN][use_cnt:%d rsv_cnt:%d]", g_mutex_ctrl_tbl.info->use_cnt, \
            g_mutex_ctrl_tbl.info->rsv_cnt);
    }

    MutexUnlockMutex();

    return ret;
}

/**
 * @brief
 *   Release resources (Mutex control data)
 *
 * @param[in]  none
 *
 * @return none
 */
void _pb_ReleaseMutexResource(void) {
    MutexLockMutex();

    /* Decrement Mutex reserved counter */
    MutexDecRsvCntOfCtrlInfo();

    MutexUnlockMutex();

    return;
}
#endif /* PT_PB_MUTEX_STUB__CWORD71_  */
