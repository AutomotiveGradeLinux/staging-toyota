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
 *  VehicleSens_Did_GpsClockFreq_g.cpp
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
static VEHICLESENS_DATA_MASTER gGpsClockFreq_g;    // NOLINT(readability/nolint)

/*---------------------------------------------------------------------------------*
 * Function                                                                                         *
 *---------------------------------------------------------------------------------*/
/**
 * @brief
 *   Initialization of GPS clock frequency data master
 */
void VehicleSensInitGpsClockFreqG(void) {
    uint32_t ul_gps_clock_freq;

    memset(&gGpsClockFreq_g, 0x00, sizeof(VEHICLESENS_DATA_MASTER));

    /** Data ID setting */
    gGpsClockFreq_g.ul_did    = POSHAL_DID_GPS_CLOCK_FREQ;
    /** Data size setting */
    gGpsClockFreq_g.us_size    = sizeof(uint32_t);
    /** Data content setting */
    ul_gps_clock_freq = 0xFFFFFFFF;
    memcpy(&gGpsClockFreq_g.uc_data[0], &ul_gps_clock_freq, sizeof(ul_gps_clock_freq));

    return;
}

/**
 * @brief
 *   GPS clock frequency data master SET process
 *
 * @param[in]  uint32_t*
 *
 * @return     u_int8
 */
u_int8 VehicleSensSetGpsClockFreqG(const uint32_t *p_data) {
    u_int8  uc_ret;
    VEHICLESENS_DATA_MASTER      *pst_master;

    pst_master = &gGpsClockFreq_g;

    /** With the contents of the current data master,Compare received data */
    uc_ret = VehicleSensmemcmp(pst_master->uc_data, p_data, sizeof(uint32_t));

    /** Received data is set in the data master. */
    pst_master->ul_did    = POSHAL_DID_GPS_CLOCK_FREQ;
    pst_master->us_size    = sizeof(uint32_t);
    pst_master->uc_rcvflag  = VEHICLE_RCVFLAG_ON;
    memset(pst_master->uc_data, 0x00, sizeof(pst_master->uc_data));
    memcpy(pst_master->uc_data, p_data, sizeof(uint32_t));

    return(uc_ret);
}

/**
 * @brief
 *   GPS clock frequency data master GET processing
 *
 * @param[out]  SENSOR_MSG_GPSDATA_DAT*
 */
void VehicleSensGetGpsClockFreqG(SENSOR_MSG_GPSDATA_DAT *pst_data) {
    const VEHICLESENS_DATA_MASTER *pst_master;

    pst_master = &gGpsClockFreq_g;

    /* Store the data master in the specified destination. */
    pst_data->ul_did    = pst_master->ul_did;
    pst_data->us_size    = pst_master->us_size;
    pst_data->uc_rcv_flag  = pst_master->uc_rcvflag;
    memcpy(pst_data->uc_data, pst_master->uc_data, pst_master->us_size);

    return;
}
