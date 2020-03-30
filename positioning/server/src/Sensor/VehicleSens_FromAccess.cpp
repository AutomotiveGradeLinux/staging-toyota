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

/*******************************************************************************
 * File name        :VehicleSens_FromAccess.cpp
 *    System name        :PastModel002
 *    Subsystem name    :Vehicle sensor process
 *    Program name    :Functions for accessing vehicle sensor FROM
 ******************************************************************************/

#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_FromAccess.h"
#include "ClockDataMng.h"

/********************************************************************************
 *            Global variable                                                        *
 ********************************************************************************/

/********************************************************************************
 *            prototype declalation                                                  *
 ********************************************************************************/
static RET_API VehicleSensReadNV(NV_DATA_VEHICLESENS *p_nv_data);

/********************************************************************************
 *            Definition                                                            *
 ********************************************************************************/
#define VEHICLESENS_FROM_ACCESS_DEBUG 0

/*******************************************************************************
* MODULE    : VehicleSensFromAccessInitialize
* ABSTRACT  : Initializing process
* FUNCTION  :
* ARGUMENT  : None
* NOTE      :
* RETURN    : None
******************************************************************************/
void VehicleSensFromAccessInitialize(void) {
    /* The result of tag registration for non-volatile memory access is stored in g_nv_access_available, so no confirmation is required. */
    (void)VehicleSensRegistNvTag();

}

/*******************************************************************************
* MODULE    : VehicleSensRegistNvTag
* ABSTRACT  : Tag registration process
* FUNCTION  : Registering Tag Name to Identify Data Storage Destination
* ARGUMENT  : None
* NOTE      :
* RETURN    : RET_NORMAL    :Successful registration
*           : RET_ERROR    :Registration failure
******************************************************************************/
RET_API VehicleSensRegistNvTag(void) {
    RET_API            lret = RET_NORMAL;
    return lret;
}

/*******************************************************************************
* MODULE    : VehicleSensReadNV
* ABSTRACT  : Get local time Lonlat
* FUNCTION  : Reading local time Lonlat from non-volatile memory
* ARGUMENT  : NV_DATA_VEHICLESENS * p_nv_data : Pointer to data acquired from non-volatile memory
* NOTE      : None
* RETURN    : RET_NORMAL    :Read success
*           : RET_ERROR    :Failed to read
******************************************************************************/
static RET_API VehicleSensReadNV(NV_DATA_VEHICLESENS * p_nv_data) {    // LCOV_EXCL_START 8 : dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API                lret = RET_NORMAL;
    FILE                *fp;
    NV_DATA_VEHICLESENS    nv_data_tmp;
    BOOL                ret_read    = FALSE;
    BOOL                ret_read2    = FALSE;

    u_int32                loop;
    u_int8                CK_A = 0;
    u_int8                CK_B = 0;
    u_int8                *ptr;

    fp = fopen(NV_FILE_VEHICLESENS_TEMP, "rb");

    /* Read File1 */
    if (fp != NULL) {
        if ((fread(reinterpret_cast<void *>(p_nv_data), sizeof(u_int8), sizeof(NV_DATA_VEHICLESENS), fp)) == \
            sizeof(NV_DATA_VEHICLESENS)) {
            /* Checksum processing */
            ptr = reinterpret_cast<u_int8 *>(p_nv_data);    /* #QAC confirmation Rule11.4 1Byte accesses for checksums */
            CK_A = 0;
            CK_B = 0;

            /* The 2Byte portion from the end of the checksum data is excluded because the checksum storage area. */
            for (loop = 0; loop < (sizeof(NV_DATA_VEHICLESENS) - 2); loop++) {
                CK_A = static_cast<u_int8>(CK_A + ptr[loop]);
                CK_B = static_cast<u_int8>(CK_B + CK_A);
            }


            if ((p_nv_data->cka == CK_A) && (p_nv_data->ckb == CK_B)) {
                ret_read = TRUE;

#if VEHICLESENS_FROM_ACCESS_DEBUG
                FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "NV read, status, year, month, day, hour, min, sec, lat, lon, "\
                    "timediff, update_counter, cka, ckb ");
                FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "NV read, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d ",
                                 p_nv_data->localtime.status,
                                 p_nv_data->localtime.year,
                                 p_nv_data->localtime.month,
                                 p_nv_data->localtime.day,
                                 p_nv_data->localtime.hour,
                                 p_nv_data->localtime.min,
                                 p_nv_data->localtime.sec,
                                 p_nv_data->lonlat.latitude,
                                 p_nv_data->lonlat.longitude,
                                 p_nv_data->timediff,
                                 p_nv_data->update_counter,
                                 p_nv_data->cka,
                                 p_nv_data->ckb);
#endif /* VEHICLESENS_FROM_ACCESS_DEBUG */
            } else {
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Checksum1 failed.");
                ret_read = FALSE;
            }
        } else {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "fread1 failed.");
        }
        fclose(fp);
    } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "fopen1 failed.");
    }

    /* Read File2 */
    fp = fopen(NV_FILE_VEHICLESENS2_TEMP, "rb");

    if (fp != NULL) {
        if ((fread(reinterpret_cast<void *>(&nv_data_tmp), sizeof(u_int8), sizeof(NV_DATA_VEHICLESENS), fp)) == \
            sizeof(NV_DATA_VEHICLESENS)) {
            /* Checksum processing */
            ptr = reinterpret_cast<u_int8 *>(&nv_data_tmp);    /* #QAC confirmation Rule11.4 1Byte accesses for checksums */
            CK_A = 0;
            CK_B = 0;

            /* The 2Byte portion from the end of the checksum data is excluded because the checksum storage area. */
            for (loop = 0; loop < (sizeof(NV_DATA_VEHICLESENS) - 2); loop++) {
                CK_A = static_cast<u_int8>(CK_A + ptr[loop]);
                CK_B = static_cast<u_int8>(CK_B + CK_A);
            }


            if ((nv_data_tmp.cka == CK_A) && (nv_data_tmp.ckb == CK_B)) {
                ret_read2 = TRUE;

#if VEHICLESENS_FROM_ACCESS_DEBUG
                FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "NV read2, status, year, month, day, hour, min, sec, lat, lon, "\
                    "timediff, update_counter, cka, ckb ");
                FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "NV read2, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d ",
                                 nv_data_tmp.localtime.status,
                                 nv_data_tmp.localtime.year,
                                 nv_data_tmp.localtime.month,
                                 nv_data_tmp.localtime.day,
                                 nv_data_tmp.localtime.hour,
                                 nv_data_tmp.localtime.min,
                                 nv_data_tmp.localtime.sec,
                                 nv_data_tmp.lonlat.latitude,
                                 nv_data_tmp.lonlat.longitude,
                                 nv_data_tmp.timediff,
                                 nv_data_tmp.update_counter,
                                 nv_data_tmp.cka,
                                 nv_data_tmp.ckb);
#endif /* VEHICLESENS_FROM_ACCESS_DEBUG */
            } else {
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Checksum2 failed.");
                ret_read2 = FALSE;
            }
        } else {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "fread2 failed.");
        }
        fclose(fp);
    } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "fopen2 failed.");
    }


    /* Check the read results of File 1 and File 2. */
    if ((ret_read == TRUE) && (ret_read2 == TRUE)) {
        /* Whether File 1 or File 2 is the most recent file,Check with the update count counter */
        if (p_nv_data->update_counter < nv_data_tmp.update_counter) {
            (void)memcpy(reinterpret_cast<void *>(p_nv_data), reinterpret_cast<void *>(&nv_data_tmp), \
                sizeof(NV_DATA_VEHICLESENS));
        }
        lret = RET_NORMAL;
    } else if (ret_read == TRUE) {
        /* Use file 1 */
        lret = RET_NORMAL;
    } else if (ret_read2 == TRUE) {
        /* Use file 2 */
        (void)memcpy(reinterpret_cast<void *>(p_nv_data), reinterpret_cast<void *>(&nv_data_tmp), \
            sizeof(NV_DATA_VEHICLESENS));
        lret = RET_NORMAL;
    } else {
        /* Read failed for both file 1 and file 2 */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "fread failed.");
        lret = RET_ERROR;
    }

    return lret;
}

/*******************************************************************************
* MODULE    : VehicleSensReadNVLocalTime
* ABSTRACT  : Local time acquisition at shutdown
* FUNCTION  : Read local time at shutdown from non-volatile memory
* ARGUMENT  : LOCALTIME * local_time : Local time at shutdown
* NOTE      : None
* RETURN    : RET_NORMAL    :Successful acquisition
*           : RET_ERROR    :Failed to acquire
******************************************************************************/
RET_API VehicleSensReadNVLocalTime(LOCALTIME * local_time) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API                lret;
    NV_DATA_VEHICLESENS    nv_data;

    lret = VehicleSensReadNV(&nv_data);

    if (lret == RET_NORMAL) {
        (void)memcpy(reinterpret_cast<void *>(local_time), (const void *)(&nv_data.localtime), sizeof(LOCALTIME));
    }

    return lret;
}

/*******************************************************************************
* MODULE    : VehicleSensReadNVLonLat
* ABSTRACT  : Obtain position at shutdown
* FUNCTION  : Read the shutdown position from the non-volatile memory
* ARGUMENT  : LONLAT * lonlat : Shutdown position
* NOTE      : None
* RETURN    : RET_NORMAL    :Successful acquisition
*           : RET_ERROR    :Failed to acquire
******************************************************************************/
RET_API VehicleSensReadNVLonLat(LONLAT * lonlat) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API                lret;
    NV_DATA_VEHICLESENS    nv_data;

    lret = VehicleSensReadNV(&nv_data);

    if (lret == RET_NORMAL) {
        (void)memcpy(reinterpret_cast<void *>(lonlat), (const void *)(&nv_data.lonlat), sizeof(LONLAT));
    }

    return lret;
}

/*******************************************************************************
* MODULE    : VehicleSensReadNVTimeDiff
* ABSTRACT  : Shutdown time difference acquisition
* FUNCTION  : Reading the shutdown time difference from the non-volatile memory
* ARGUMENT  : int32 * time_diff : Shutdown position
* NOTE      : None
* RETURN    : RET_NORMAL    :Successful acquisition
*           : RET_ERROR    :Failed to acquire
******************************************************************************/
RET_API VehicleSensReadNVTimeDiff(int32 * time_diff) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API                lret;
    NV_DATA_VEHICLESENS    nv_data;

    lret = VehicleSensReadNV(&nv_data);

    if (lret == RET_NORMAL) {
        *time_diff = nv_data.timediff;
    }

    return lret;
}

/*******************************************************************************
* MODULE    : VehicleSensStoreLonlat
* ABSTRACT  : Store location data in non-volatile memory
* FUNCTION  :
* ARGUMENT  : LONLAT * plonlat : Position data
* NOTE      :
* RETURN    : None
******************************************************************************/
void VehicleSensStoreLonlat(LONLAT * plonlat) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

    return;
}

/*******************************************************************************
* MODULE    : VehicleSensWriteNVLocaltime
* ABSTRACT  : Local Time Write
* FUNCTION  : Write local time
* ARGUMENT  : LOCALTIME * local_time : Local time
* NOTE      :
* RETURN    : RET_NORMAL    :Successful write
*           : RET_ERROR    :Writing failure
******************************************************************************/
RET_API VehicleSensWriteNVLocaltime(LOCALTIME * local_time, int32 * time_diff) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API lret = RET_NORMAL;

    return lret;
}

// LCOV_EXCL_STOP
