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

/******************************************************************************
@file       SensorLocation_API.cpp
@detail     SensorLocation_API Functions
@lib        libSensorLocation_API.so
******************************************************************************/

/*****************************************************************************
 *            Include                                                        *
 *****************************************************************************/
#include <vehicle_service/positioning_base_library.h>
#include "SensorLocation_API.h"
#include "Vehicle_API.h"
#include "Vehicle_API_Dummy.h"
#include "SensorLocation_API_private.h"

static RET_API SensorLocationGetLonLatOnShutdownGetData(LONLAT *lonlat);

/*******************************************************************************
* MODULE    : SensorLocationGetLonLatOnShutdownGetData
* ABSTRACT  : Obtain position at shutdown
* FUNCTION  : Gets the location at shutdown from shared memory
* ARGUMENT  : LONLAT * lonlat    : Latitude/Longitude
* NOTE      :
* RETURN    : RET_NORMAL                : Successful acquisition
*           : RET_ERROR                : Failed to acquire
*           : RET_ERRPARAM                : Parameter error
******************************************************************************/
static RET_API SensorLocationGetLonLatOnShutdownGetData(LONLAT *lonlat) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API    ret = RET_NORMAL;
    RET_API    ret_api;
    LONLAT     *share_mem;            /* Store Shared Memory Address */
    u_int32    share_mem_size;            /* Size of the linked shared memory */

    if (lonlat == NULL) {
        ret = RET_ERRPARAM;
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "lonlat is NULL\r\n");
    } else {
        /* Link to shared memory */
        ret_api = _pb_LinkShareData(const_cast<char *>(LONLAT_SHARE_NAME),
                                    reinterpret_cast<void **>(&share_mem), &share_mem_size);
        /* #QAC confirmation Rule11.4 Use structure for member reference(Cast according to shared memory link IF) */

        if (ret_api == RET_NORMAL) {
            /* Link to shared memory successful */

            /* Get shutdown location from shared memory */
            *lonlat = *share_mem;
        } else {
            /* Failed to link to shared memory */
            ret = RET_ERROR;
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Can't link shared memory\r\n");
        }
    }

    return ret;
}
// LCOV_EXCL_STOP

/******************************************************************************
@brief      SensorLocationGetLonLatOnShutdown<BR>
            Obtain position at shutdown
@outline    Get location information at shutdown.
@param[in]  none
@param[out] LONLAT* lonlat: Pointer to the acquired latitude/longitude information storage destination
@return     SENSORLOCATION_RET_API
@retval     SENSORLOCATION_RET_NORMAL           : Normal completion
@retval     SENSORLOCATION_RET_ERROR_INNER      : Internal error
*******************************************************************************/
SENSORLOCATION_RET_API SensorLocationGetLonLatOnShutdown(LONLAT *lonlat) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    SENSORLOCATION_RET_API    ret;
    RET_API                   ret_api;    /* API return value */
    RET_API                   ret_tmp;
    SemID                     sem_id;     /* Semaphore ID */

    if (lonlat != NULL) {
        /* Get Semaphore ID */
        sem_id = _pb_CreateSemaphore(const_cast<char *>(LONLAT_SEMAPHO_NAME));

        if (sem_id != 0) {    /* Because the return value of _pb_CreateSemaphore is not defined in #define,Evaluate directly */
            /* Semaphore ID successfully acquired */

            /* Semaphore Lock */
            ret_api = _pb_SemLock(sem_id);

            if (ret_api == RET_NORMAL) {
                /* Semaphore lock successful */

                /* Obtain position at shutdown */
                ret_tmp = SensorLocationGetLonLatOnShutdownGetData(lonlat);

                if (ret_tmp == RET_NORMAL) {
                    /* Position acquisition at shutdown successful */
                    ret = SENSORLOCATION_RET_NORMAL;
                } else {
                    /* Location acquisition at shutdown failed */
                    ret = SENSORLOCATION_RET_ERROR_INNER;
                    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "SensorLocationGetLonLatOnShutdownGetData failed");
                }

                /* Semaphore unlock */
                ret_api = _pb_SemUnlock(sem_id);
                if (ret_api != RET_NORMAL) {
                    /* Semaphore unlock failure */
                    ret = SENSORLOCATION_RET_ERROR_INNER;
                    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_SemUnlock failed");
                }
            } else {
                /* Semaphore lock failed */
                ret = SENSORLOCATION_RET_ERROR_INNER;
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_SemLock failed");
            }
        } else {
            /* Semaphore ID acquisition failure */
            ret = SENSORLOCATION_RET_ERROR_INNER;
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "sem_id == 0");
        }
    } else {
        ret = SENSORLOCATION_RET_ERROR_INNER;
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "lonlat == NULL");
    }

    return ret;
}
// LCOV_EXCL_STOP
