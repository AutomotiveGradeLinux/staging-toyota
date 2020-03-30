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
 *  VehicleSens_Did_WknRollover_g.cpp
 * @brief
 */

/*---------------------------------------------------------------------------------*
 * Include Files                                                                   *
 *---------------------------------------------------------------------------------*/
#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"

/*---------------------------------------------------------------------------------*
 * Global Value                                                                    *
 *---------------------------------------------------------------------------------*/
static  VEHICLESENS_DATA_MASTER  gstWknRollover_g;    // NOLINT(readability/nolint)

/*---------------------------------------------------------------------------------*
 * Function                                                                        *
 *---------------------------------------------------------------------------------*/
/**
 * @brief
 *   GPS rollover standard week number data master initialization processing
 */
void VehicleSensInitWknRolloverG(void) {
    SENSOR_WKNROLLOVER  st_wkn_rollover;

    memset(&gstWknRollover_g, 0x00, sizeof(VEHICLESENS_DATA_MASTER));

    /** Data ID setting */
//    gstWknRollover_g.ul_did    = POSHAL_DID_GPS_WKNROLLOVER;
    /** Data size setting */
    gstWknRollover_g.us_size    = sizeof(SENSOR_WKNROLLOVER);
    /** Data content setting */
    memset(&st_wkn_rollover, 0x00, sizeof(st_wkn_rollover));
    memcpy(&gstWknRollover_g.uc_data[0], &st_wkn_rollover, sizeof(st_wkn_rollover));

    return;
}

/**
 * @brief
 *   GPS Rollover Standard Week Number Data Master SET Process
 *
 * @param[in]  SENSOR_WKNROLLOVER*
 *
 * @return     u_int8
 */
u_int8 VehicleSensSetWknRolloverG(const SENSOR_WKNROLLOVER *pst_wkn_rollover) {
    u_int8  uc_ret;
    VEHICLESENS_DATA_MASTER      *pst_master;

    pst_master = &gstWknRollover_g;

    /** With the contents of the current data master,Compare received data */
    uc_ret = VehicleSensmemcmp(pst_master->uc_data, pst_wkn_rollover, sizeof(SENSOR_WKNROLLOVER));

    /** Received data is set in the data master. */
//    pst_master->ul_did    = POSHAL_DID_GPS_WKNROLLOVER;
    pst_master->us_size    = sizeof(SENSOR_WKNROLLOVER);
    pst_master->uc_rcvflag  = VEHICLE_RCVFLAG_ON;
    memset(pst_master->uc_data, 0x00, sizeof(pst_master->uc_data));
    memcpy(pst_master->uc_data, pst_wkn_rollover, sizeof(SENSOR_WKNROLLOVER));

    return(uc_ret);
}

/**
 * @brief
 *   GPS Rollover Standard Week Number Data Master GET Processing
 *
 * @param[out]  SENSOR_MSG_GPSDATA_DAT*
 */
//void VehicleSensGetWknRolloverG(SENSOR_MSG_GPSDATA_DAT *pst_data) {
//    const VEHICLESENS_DATA_MASTER *pst_master;
//
//    pst_master = &gstWknRollover_g;
//
//    /* Store the data master in the specified destination. */
//    pst_data->ul_did    = pst_master->ul_did;
//    pst_data->us_size    = pst_master->us_size;
//    pst_data->uc_rcv_flag  = pst_master->uc_rcvflag;
//    memcpy(pst_data->uc_data, pst_master->uc_data, pst_master->us_size);
//
//    return;
//}
