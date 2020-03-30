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

/****************************************************************************
@file       VehicleSens_Did_LocationAltitude_g.cpp
@detail     Altitude information data master management(NMEA information)
******************************************************************************/
#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"
#include "CommonDefine.h"

/*************************************************
 *           Global variable                      *
 *************************************************/
static  VEHICLESENS_DATA_MASTER  gstLocationAltitude_g;    // NOLINT(readability/nolint)

/****************************************************************************
@brief      VehicleSensInitLocationAltitudeG<BR>
            Altitude information data master initialization processing(NMEA information)
@outline    Initialize the altitude information data master
@param[in]  none
@param[out] none
@return     none
@retval     none
*******************************************************************************/
void VehicleSensInitLocationAltitudeG(void) {
//    SENSORLOCATION_ALTITUDEINFO_DAT  st_altitude;

    memset(&gstLocationAltitude_g, 0x00, sizeof(VEHICLESENS_DATA_MASTER));

    /** Data ID setting */
    gstLocationAltitude_g.ul_did    = VEHICLE_DID_LOCATION_ALTITUDE;
    /** Data size setting */
//    gstLocationAltitude_g.us_size    = sizeof(SENSORLOCATION_ALTITUDEINFO_DAT);
    /** Data content setting */
//    memset(&st_altitude, 0x00, sizeof(st_altitude));
//    st_altitude.getMethod = SENSOR_GET_METHOD_GPS;
//    st_altitude.SyncCnt   = 0x00;
//    st_altitude.isEnable  = SENSORLOCATION_STATUS_DISABLE;
//    memcpy(&gstLocationAltitude_g.uc_data[0], &st_altitude, sizeof(st_altitude));
}

/****************************************************************************
@brief      VehicleSensSetLocationAltitudeG<BR>
            Altitude information data master SET processing(NMEA information)
@outline    Update the altitude information data master
@param[in]  u_int8    ucSensCnt : Sensor counter value
@param[in]  u_int8*    pucDGGA   : Double precision GGAInformation(_CWORD82_ NMEA)
@param[out] none
@return     u_int8
@retval     VEHICLESENS_EQ  : No data change
@retval     VEHICLESENS_NEQ : Data change
*******************************************************************************/
//u_int8 VehicleSensSetLocationAltitudeG(const SENSORLOCATION_ALTITUDEINFO_DAT *pst_altitude) {
//    u_int8  uc_ret;
//    VEHICLESENS_DATA_MASTER      *pst_master;
//
//    pst_master = &gstLocationAltitude_g;
//
//    /** With the contents of the current data master,Compare received data */
//    uc_ret = VehicleSensmemcmp(pst_master->uc_data, pst_altitude, sizeof(SENSORLOCATION_ALTITUDEINFO_DAT));
//
//    /** Received data is set in the data master. */
//    pst_master->ul_did    = VEHICLE_DID_LOCATION_ALTITUDE;
//    pst_master->us_size    = sizeof(SENSORLOCATION_ALTITUDEINFO_DAT);
//    pst_master->uc_rcvflag  = VEHICLE_RCVFLAG_ON;
//    memset(pst_master->uc_data, 0x00, sizeof(pst_master->uc_data));
//    memcpy(pst_master->uc_data, pst_altitude, sizeof(SENSORLOCATION_ALTITUDEINFO_DAT));
//
//    return(uc_ret);
//}

/****************************************************************************
@brief      VehicleSensGetLocationAltitudeG<BR>
            Altitude Information Data Master GET Processing(NMEA information)
@outline    Provide an altitude information data master
@param[in]  none
@param[out] VEHICLESENS_DATA_MASTER *pst_data : Pointer to the data master acquisition destination
@return     none
@retval     none
*******************************************************************************/
void VehicleSensGetLocationAltitudeG(VEHICLESENS_DATA_MASTER *pst_data) {
    const VEHICLESENS_DATA_MASTER *pst_master;

    pst_master = &gstLocationAltitude_g;

    /* Store the data master in the specified destination. */
    pst_data->ul_did    = pst_master->ul_did;
    pst_data->us_size    = pst_master->us_size;
    pst_data->uc_rcvflag  = pst_master->uc_rcvflag;
    memcpy(pst_data->uc_data, pst_master->uc_data, pst_master->us_size);  /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
}
