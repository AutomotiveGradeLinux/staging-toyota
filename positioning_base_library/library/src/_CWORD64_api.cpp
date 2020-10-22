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
 *	_CWORD64_api.cpp
 */

/*---------------------------------------------------------------------------------*
 * Include Files                                                                   *
 *---------------------------------------------------------------------------------*/
#include <vehicle_service/positioning_base_library.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/cl_lock.h>
#include <native_service/cl_lockid.h>
#include "tchar.h"
#include "WPF_STD_private.h"



/*---------------------------------------------------------------------------------*
 * Function Prototype                                                              *
 *---------------------------------------------------------------------------------*/
extern RET_API MutexInit(void);
extern RET_API SemaphoreInit(void);
extern RET_API MemoryInit(void);
extern RET_API EventInit(void);
extern RET_API MsgInit(void);
extern RET_API ErrTrapInit(void);
extern RET_API TimerInit(HANDLE h_app);
extern void MsgTerm(void);

static void BaseCreateMutex(void);
static void BaseLockMutex(void);
static void BaseUnlockMutex(void);

/*---------------------------------------------------------------------------------*
 * Grobal Value                                                                    *
 *---------------------------------------------------------------------------------*/
/** Handle for locking Base API setups */
static HANDLE g_h_mtx = NULL;

/*---------------------------------------------------------------------------------*
 * Function                                                                        *
 *---------------------------------------------------------------------------------*/
/**
 * @brief
 *   Setup Base API
 *
 *   Perform the processing required to use the Base API.(Call per process)
 *
 * @param[in]  h_app Application handle
 *
 * @return  RET_NORMAL Normal completion
 */
RET_API _pb_Setup_CWORD64_API(HANDLE h_app) {    // NOLINT(readability/nolint)  API
    RET_API result;
    static int8 i = 0;
    int32 ret;

    if (i != 0) {
        /* If the process has already initialized */
        /* nop */
    } else {
        /* Interprocess lock initialization(CLS) */
        ret = CL_LockProcessInit();  // LCOV_EXCL_BR_LINE 4: nsfw error
        if (ret != 0) {  // LCOV_EXCL_BR_LINE 4: nsfw error
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "CL_LockProcessInit ERROR!! [ret=%d]", ret);
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            _pb_Exit();  // LCOV_EXCL_LINE 4: nsfw error
            /* don't arrive here. */
        }

        BaseCreateMutex(); /* Create Mutex */
        BaseLockMutex(); /* Get Mutex */

        /* Recheck during getting Mutex */
        if (i == 0) {  // LCOV_EXCL_BR_LINE 6: i can not be other value
            /* Not initialized */
            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "_pb_Setup_CWORD64_API Entered.");

            /* Mutex function initialization */
            result = MutexInit();
            if (result != RET_NORMAL) {  // LCOV_EXCL_BR_LINE 200: can not be not normal
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "MutexInit ERROR!! [result=%d]", result);
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                _pb_Exit();  // LCOV_EXCL_LINE 200: function can not be not normal
            }

            /* Semaphore function initialization */
            result = SemaphoreInit();
            if (result != RET_NORMAL) {  // LCOV_EXCL_BR_LINE 200: can not be not normal
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "SemaphoreInit ERROR!! [result=%d]", \
                    result);
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                _pb_Exit();  // LCOV_EXCL_LINE 200: function can not be not normal
            }

            /* Memory function initialization */
            result = MemoryInit();
            if (result != RET_NORMAL) {  // LCOV_EXCL_BR_LINE 200: can not be not normal
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "MemoryInit ERROR!! [result=%d]", result);
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                _pb_Exit();  // LCOV_EXCL_LINE 200: can not be not normal
            }

            /* Event function initialization */
            result = EventInit();
            if (result != RET_NORMAL) {  // LCOV_EXCL_BR_LINE 200: can not be not normal
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "EventInit ERROR!! [result=%d]", result);
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                _pb_Exit();  // LCOV_EXCL_LINE 200: function can not be not normal
            }

            /* Message function initialization */
            result = MsgInit();
            if (result != RET_NORMAL) {  // LCOV_EXCL_BR_LINE 200: can not be not normal
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "MsgInit ERROR!! [result=%d]", result);
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                _pb_Exit();  // LCOV_EXCL_LINE 200: can not be not normal
            }

            result = ErrTrapInit();
            if (result != RET_NORMAL) {  // LCOV_EXCL_BR_LINE 200: can not be not normal
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "ErrTrapInit ERROR!! [result=%d]", result);
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                _pb_Exit();  // LCOV_EXCL_LINE 200: can not be not normal
            }

            /* For Positioning processes */
            if (_tcscmp("Positioning", FrameworkunifiedGetAppName(h_app)) == 0) {  // LCOV_EXCL_BR_LINE 200: can not be other value
                /* Timer function initialization */
                result = TimerInit(h_app);
                if (result != RET_NORMAL) {  // LCOV_EXCL_BR_LINE 200: can not be not normal
                    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, \
                    "TimerInit ERROR!! [result=%d]", result);
                    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                    _pb_Exit();  // LCOV_EXCL_LINE 200: can not be not normal
                }
            }

            i = 1;
        }

        BaseUnlockMutex(); /* Release Mutex */
    }

    /* Set application handle */
    _pb_SetAppHandle(h_app);

    return RET_NORMAL;
}

/**
 * @brief
 *   Teardown Base API
 *
 *   Base API termination processing (Call per process)
 *
 * @param[in]  none
 *
 * @return none
 */
void _pb_Teardown_CWORD64_API(void) {   // NOLINT(readability/nolint)  API  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    /* Interprocess Lock Control(CLS) */


    /*
       Resources cannot be released unless it is guaranteed that other processes do not call the Positioning API.
       Even if the resource is not released at the time of termination, it will not lead to illegal operation such as memory leak.
       Comment out release processing.
     */

    /* Mutex */
    /* Semaphore */
    /* Memory */
    /* Event */
    /* Message */

    /* Timer */

    return;
}
// LCOV_EXCL_STOP


/**
 * @brief
 *   Create Mutex for Base API setups
 */
static void BaseCreateMutex(void) {
    g_h_mtx = (HANDLE)CL_LockMap(LOCK_POS_MTX_1);  // LCOV_EXCL_BR_LINE 4: nsfw error
    if (g_h_mtx == NULL) {  // LCOV_EXCL_BR_LINE 4: nsfw error
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "CL_LockMap ERROR [g_h_mtx:%p]", g_h_mtx);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 4: nsfw error
        /* don't arrive here. */
    }

    return;
}

/**
 * @brief
 *   Get Mutex for Base API setups
 */
static void BaseLockMutex(void) {
    int32 ret;

    ret = CL_LockGet(g_h_mtx);  // LCOV_EXCL_BR_LINE 4: nsfw error
    if (ret != 0) {  // LCOV_EXCL_BR_LINE 4: nsfw error
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "CL_LockGet ERROR [g_h_mtx:%p, ret:%d]", \
            g_h_mtx, ret);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 4: nsfw error
        /* don't arrive here. */
    }

    return;
}

/**
 * @brief
 *   Release Mutex for Base API Setup
 */
static void BaseUnlockMutex(void) {
    int32 ret;

    ret = CL_LockRelease(g_h_mtx);  // LCOV_EXCL_BR_LINE 4: nsfw error
    if (ret != 0) {  // LCOV_EXCL_BR_LINE 4: nsfw error
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "CL_LockRelease ERROR [g_h_mtx:%p, ret:%d]", \
            g_h_mtx, ret);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 4: nsfw error
        /* don't arrive here. */
    }

    return;
}

