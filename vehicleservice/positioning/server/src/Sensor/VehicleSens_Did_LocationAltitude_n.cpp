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
 *  VehicleSens_Did_LocationAltitude_n.cpp
 * @brief
 */
/*---------------------------------------------------------------------------------*
 * Include Files                                                                   *
 *---------------------------------------------------------------------------------*/
#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"
#include "SensorLocation_API.h"
#include "CommonDefine.h"

/*---------------------------------------------------------------------------------*
 * Global Value                                                                    *
 *---------------------------------------------------------------------------------*/
static  VEHICLESENS_DATA_MASTER  gstLocationAltitude_n;    // NOLINT(readability/nolint)

/**
 * @brief
 *  Altitude information data master initialization processing(NAVI information)
 *
 *  Initialize the altitude information data master
 *
 * @param[in]   none
 * @param[out]  none
 * @return      none
 * @retval      none
 */
void VehicleSensInitLocationAltitudeN(void) {
    SENSORLOCATION_ALTITUDEINFO_DAT  st_altitude;

    memset(&gstLocationAltitude_n, 0x00, sizeof(VEHICLESENS_DATA_MASTER));

    /** Data ID setting */
    gstLocationAltitude_n.ul_did   = VEHICLE_DID_LOCATION_ALTITUDE_NAVI;

    /** Data size setting */
    gstLocationAltitude_n.us_size  = sizeof(SENSORLOCATION_ALTITUDEINFO_DAT);

    /** Data content setting */
    memset(&st_altitude, 0x00, sizeof(st_altitude));
    st_altitude.getMethod = SENSOR_GET_METHOD_NAVI;
    st_altitude.SyncCnt   = 0x00;
    st_altitude.isEnable  = SENSORLOCATION_STATUS_DISABLE;
    st_altitude.Altitude  = 0x00;
    memcpy(&gstLocationAltitude_n.uc_data[0], &st_altitude, sizeof(st_altitude));

    return;
}

/**
 * @brief
 *  Altitude information data master SET processing(NAVI information)
 *
 *  Update the altitude information data master
 *
 * @param[in]  VEHICLESENS_DATA_MASTER *pst_data     : Pointer to the data master acquisition destination
 * @param[out] none
 * @return     u_int8
 * @retval     VEHICLESENS_EQ  : No data change
 * @retval     VEHICLESENS_NEQ : Data change
 */
u_int8 VehicleSensSetLocationAltitudeN(const SENSORLOCATION_ALTITUDEINFO_DAT *pst_altitude) {
    u_int8            uc_ret;
    VEHICLESENS_DATA_MASTER    *pst_master;

    pst_master = &gstLocationAltitude_n;

    /** With the contents of the current data master,Compare received data */
    uc_ret = VehicleSensmemcmp(pst_master->uc_data, pst_altitude, sizeof(SENSORLOCATION_ALTITUDEINFO_DAT));

    /** Received data is set in the data master. */
    pst_master->ul_did    = VEHICLE_DID_LOCATION_ALTITUDE_NAVI;
    pst_master->us_size    = sizeof(SENSORLOCATION_ALTITUDEINFO_DAT);
    pst_master->uc_rcvflag  = VEHICLE_RCVFLAG_ON;
    memcpy(pst_master->uc_data, pst_altitude, sizeof(SENSORLOCATION_ALTITUDEINFO_DAT));

    return(uc_ret);
}

/**
 * @brief
 *  Altitude Information Data Master GET Processing(NAVI information)
 *
 *  Provide an altitude information data master
 *
 * @param[in]  none
 * @param[out] VEHICLESENS_DATA_MASTER *pst_data : Pointer to the data master acquisition destination
 * @return     none
 * @retval     none
 */
void VehicleSensGetLocationAltitudeN(VEHICLESENS_DATA_MASTER *pst_data) {
    const VEHICLESENS_DATA_MASTER *pst_master;

    pst_master = &gstLocationAltitude_n;

    /** Store the data master in the specified destination. */
    pst_data->ul_did    = pst_master->ul_did;
    pst_data->us_size    = pst_master->us_size;
    pst_data->uc_rcvflag  = pst_master->uc_rcvflag;
    memcpy(pst_data->uc_data, pst_master->uc_data, pst_master->us_size);

    return;
}
