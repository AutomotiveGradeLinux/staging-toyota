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

/*******************************************************************************
* $Header::                                                                   $
* $Revision::                                                                 $
*******************************************************************************/

/****************************************************************************
 * File name        :_pbCSection.cpp
 * System name      :GPF
 * Subsystem name   :_CWORD64_API Critical Section Function
 * Title            :Critical Section Function Definition File
 ****************************************************************************/

#include <fcntl.h>
#include <sys/mman.h>
#include <vehicle_service/positioning_base_library.h>
#include "WPF_STD_private.h"
#include "_pbInternalProc.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* MODULE    : PbDeleteCriticalSection()
* ABSTRACT  : Specify a critical section object that is not owned and free all resources used by that object.
* NOTE      :
* ARGUMENT  : LPCRITICAL_SECTION lp_critical_section
* RETURN    : VOID defined
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
VOID PbDeleteCriticalSection(LPCRITICAL_SECTION lp_critical_section) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    int32                    lret = EOK;

    if (lp_critical_section == NULL) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                        "_CWORD64_api.dll:%s:LINE %d\r\n Parameter ERROR In PbDeleteCriticalSection\r\n",
                        LTEXT(__FILE__), __LINE__);
    } else {
        lret = pthread_mutex_destroy(lp_critical_section);
        if (lret != EOK) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                            "_CWORD64_api.dll:%s:LINE %d\r\n ERROR ... ErrNo[%d] In PbDeleteCriticalSection\r\n",
                            LTEXT(__FILE__), __LINE__, lret);
        }
    }

    return;
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* MODULE    : PbEnterCriticalSection()
* ABSTRACT  : Wait to take ownership of the specified critical section object
* NOTE      :
* ARGUMENT  : LPCRITICAL_SECTION lp_critical_section
* RETURN    : VOID defined
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
VOID PbEnterCriticalSection(LPCRITICAL_SECTION lp_critical_section) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    int32                    lret = EOK;

    if (lp_critical_section == NULL) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                        "_CWORD64_api.dll:%s:LINE %d\r\n Parameter ERROR In PbEnterCriticalSection\r\n",
                       LTEXT(__FILE__), __LINE__);
    } else {
        lret = pthread_mutex_lock(lp_critical_section);
        if (lret != EOK) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                        "_CWORD64_api.dll:%s:LINE %d\r\n ERROR ... ErrNo[%d] In PbEnterCriticalSection\r\n",
                        LTEXT(__FILE__), __LINE__, lret);
        }
    }

    return;
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* MODULE    : PbInitializeCriticalSection
* ABSTRACT  : Initialize the specified critical section object.
* NOTE      :
* ARGUMENT  : LPCRITICAL_SECTION lp_critical_section
* RETURN    : VOID defined
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
VOID PbInitializeCriticalSection(LPCRITICAL_SECTION lp_critical_section) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* MODULE    : PbLeaveCriticalSection
* ABSTRACT  : Discard ownership of the specified critical section object.
* NOTE      :
* ARGUMENT  : LPCRITICAL_SECTION lp_critical_section
* RETURN    : VOID defined
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
VOID PbLeaveCriticalSection(LPCRITICAL_SECTION lp_critical_section ) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    int32                    lret = EOK;

    if (lp_critical_section == NULL) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                               "_CWORD64_api.dll:%s:LINE %d\r\n Parameter ERROR In PbLeaveCriticalSection\r\n",
                       LTEXT(__FILE__), __LINE__);
    } else {
        lret = pthread_mutex_unlock(lp_critical_section);
        if (lret != EOK) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                                   "_CWORD64_api.dll:%s:LINE %d\r\n ERROR ... ErrNo[%d] In PbLeaveCriticalSection\r\n",
                           LTEXT(__FILE__), __LINE__, lret);
        }
    }

    return;
}
// LCOV_EXCL_STOP

