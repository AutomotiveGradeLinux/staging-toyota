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
 *  VehicleSens_Did_GpsTimeRaw_g.cpp
 * @brief
 */

/*---------------------------------------------------------------------------------*
 * Include Files                                                                                   *
 *---------------------------------------------------------------------------------*/
#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"

/*---------------------------------------------------------------------------------*
 * Global Value                                                                                   *
 *---------------------------------------------------------------------------------*/
static  VEHICLESENS_DATA_MASTER  gstGpsTimeRaw_g;    // NOLINT(readability/nolint)

/*---------------------------------------------------------------------------------*
 * Function                                                                                         *
 *---------------------------------------------------------------------------------*/
/**
 * @brief
 *   GPS time information data master initialization processing
 */
void VehicleSensInitGpsTimeRawG(void) {
    SENSOR_GPSTIME_RAW  st_gps_time_raw;

    memset(&gstGpsTimeRaw_g, 0x00, sizeof(VEHICLESENS_DATA_MASTER));

    /** Data ID setting */
    gstGpsTimeRaw_g.ul_did    = POSHAL_DID_GPS_TIME_RAW;
    /** Data size setting */
    gstGpsTimeRaw_g.us_size    = sizeof(SENSOR_GPSTIME_RAW);
    /** Data content setting */
    memset(&st_gps_time_raw, 0x00, sizeof(st_gps_time_raw));
    memcpy(&gstGpsTimeRaw_g.uc_data[0], &st_gps_time_raw, sizeof(st_gps_time_raw));

    return;
}

/**
 * @brief
 *   Raw GPS Time Data Master SET Processing
 *
 * @param[in]  SENSOR_GPSTIME_RAW*
 *
 * @return     u_int8
 */
u_int8 VehicleSensSetGpsTimeRawG(const SENSOR_GPSTIME_RAW *pst_gps_time_raw) {
    u_int8  uc_ret;
    VEHICLESENS_DATA_MASTER      *pst_master;

    pst_master = &gstGpsTimeRaw_g;

    /** With the contents of the current data master,Compare received data */
    uc_ret = VehicleSensmemcmp(pst_master->uc_data, pst_gps_time_raw, sizeof(SENSOR_GPSTIME_RAW));

    /** Received data is set in the data master. */
    pst_master->ul_did    = POSHAL_DID_GPS_TIME_RAW;
    pst_master->us_size    = sizeof(SENSOR_GPSTIME_RAW);
    pst_master->uc_rcvflag  = VEHICLE_RCVFLAG_ON;
    memset(pst_master->uc_data, 0x00, sizeof(pst_master->uc_data));
    memcpy(pst_master->uc_data, pst_gps_time_raw, sizeof(SENSOR_GPSTIME_RAW));

    FRAMEWORKUNIFIEDLOG(ZONE_27, __FUNCTION__, 
        "year=%04d, month=%02d, date=%02d, hour=%02d, minute=%02d, second=%02d, tdsts=%d",
        pst_gps_time_raw->utc.year, pst_gps_time_raw->utc.month, pst_gps_time_raw->utc.date,
        pst_gps_time_raw->utc.hour, pst_gps_time_raw->utc.minute,
        pst_gps_time_raw->utc.second, pst_gps_time_raw->tdsts);
    return(uc_ret);
}

/**
 * @brief
 *   Raw GPS time information data master GET processing
 *
 * @param[out]  SENSOR_MSG_GPSDATA_DAT*
 */
void VehicleSensGetGpsTimeRawG(SENSOR_MSG_GPSDATA_DAT *pst_data) {
    const VEHICLESENS_DATA_MASTER *pst_master;

    pst_master = &gstGpsTimeRaw_g;

    /* Store the data master in the specified destination. */
    pst_data->ul_did    = pst_master->ul_did;
    pst_data->us_size    = pst_master->us_size;
    pst_data->uc_rcv_flag  = pst_master->uc_rcvflag;
    memcpy(pst_data->uc_data, pst_master->uc_data, pst_master->us_size);

    return;
}
