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

/****************************************************************************
@file       VehicleSens_Did_LocationLonLat_g.cpp
@detail     Latitude and longitudeManagement of information data master(NMEA information)
******************************************************************************/
#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"
#include "CommonDefine.h"

/*************************************************
 *           Global variable                      *
 *************************************************/
static  VEHICLESENS_DATA_MASTER  gstLocationLonLat_g;  // NOLINT(readability/nolint)

/****************************************************************************
@brief      VehicleSensInitLocationLonLatG<BR>
            Latitude and longitudeInitialization of information data master(NMEA information)
@outline    Latitude and longitudeInitialize the information data master
@param[in]  none
@param[out] none
@return     none
@retval     none
*******************************************************************************/
void VehicleSensInitLocationLonLatG(void) {
    SENSORLOCATION_LONLATINFO_DAT  st_lonlat;

    memset(&gstLocationLonLat_g, 0x00, sizeof(VEHICLESENS_DATA_MASTER));

    /** Data ID setting */
    gstLocationLonLat_g.ul_did    = VEHICLE_DID_LOCATION_LONLAT;
    /** Data size setting */
    gstLocationLonLat_g.us_size    = sizeof(SENSORLOCATION_LONLATINFO_DAT);
    /** Data content setting */
    memset(&st_lonlat, 0x00, sizeof(st_lonlat));
    st_lonlat.getMethod = SENSOR_GET_METHOD_GPS;
    st_lonlat.SyncCnt  = 0x00;
    st_lonlat.isEnable = SENSORLOCATION_STATUS_DISABLE;
    memcpy(&gstLocationLonLat_g.uc_data[0], &st_lonlat, sizeof(st_lonlat));
}

/****************************************************************************
@brief      VehicleSensSetLocationLonLatG<BR>
            Latitude and longitudeInformation data master SET process(NMEA information)
@outline    Latitude and longitudeUpdate the information data master
@param[in]  SENSORLOCATION_LONLATINFO_DAT *    pst_lonlat : Latitude and longitude information
@param[out] none
@return     u_int8
@retval     VEHICLESENS_EQ  : No data change
@retval     VEHICLESENS_NEQ : Data change
*******************************************************************************/
u_int8 VehicleSensSetLocationLonLatG(const SENSORLOCATION_LONLATINFO_DAT *pst_lonlat) {
    u_int8  uc_ret;
    VEHICLESENS_DATA_MASTER      *pst_master;

    pst_master = &gstLocationLonLat_g;

    /** With the contents of the current data master,Compare received data */
    uc_ret = VehicleSensmemcmp(pst_master->uc_data, pst_lonlat, sizeof(SENSORLOCATION_LONLATINFO_DAT));

    /** Received data is set in the data master. */
    pst_master->ul_did    = VEHICLE_DID_LOCATION_LONLAT;
    pst_master->us_size    = sizeof(SENSORLOCATION_LONLATINFO_DAT);
    pst_master->uc_rcvflag  = VEHICLE_RCVFLAG_ON;
    memset(pst_master->uc_data, 0x00, sizeof(pst_master->uc_data));
    memcpy(pst_master->uc_data, pst_lonlat, sizeof(SENSORLOCATION_LONLATINFO_DAT));

    return(uc_ret);
}

/****************************************************************************
@brief      VehicleSensGetLocationLonLatG<BR>
            Latitude and longitudeInformation Data Master GET Processing(NMEA information)
@outline    Latitude and longitudeProvide an information data master
@param[in]  none
@param[out] VEHICLESENS_DATA_MASTER *pst_data : Pointer to the data master acquisition destination
@return     none
@retval     none
*******************************************************************************/
void VehicleSensGetLocationLonLatG(VEHICLESENS_DATA_MASTER *pst_data) {
    const VEHICLESENS_DATA_MASTER *pst_master;

    pst_master = &gstLocationLonLat_g;

    /* Store the data master in the specified destination. */
    pst_data->ul_did    = pst_master->ul_did;
    pst_data->us_size    = pst_master->us_size;
    pst_data->uc_rcvflag  = pst_master->uc_rcvflag;
    memcpy(pst_data->uc_data, pst_master->uc_data, pst_master->us_size);  /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
}
