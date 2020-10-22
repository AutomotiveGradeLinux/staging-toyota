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
 *  VehicleSens_Did_LocationLonLat_n.cpp
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
static  VEHICLESENS_DATA_MASTER  gstLocationLonLat_n;    // NOLINT(readability/nolint)

/**
 * @brief
 *  Latitude and longitudeInitialization of information data master(NAVI information)
 *
 *  Latitude and longitudeInitialize the information data master
 *
 * @param[in]   none
 * @param[out]  none
 * @return      none
 * @retval      none
 */
void VehicleSensInitLocationLonLatN(void) {
    SENSORLOCATION_LONLATINFO_DAT  st_lonlat;

    memset(&gstLocationLonLat_n, 0x00, sizeof(VEHICLESENS_DATA_MASTER));

    /** Data ID setting */
    gstLocationLonLat_n.ul_did  = VEHICLE_DID_LOCATION_LONLAT_NAVI;

    /** Data size setting */
    gstLocationLonLat_n.us_size  = sizeof(SENSORLOCATION_LONLATINFO_DAT);

    /** Data content setting */
    memset(&st_lonlat, 0x00, sizeof(st_lonlat));
    st_lonlat.getMethod = SENSOR_GET_METHOD_NAVI;
    st_lonlat.SyncCnt   = 0x00;
    st_lonlat.isEnable  = SENSORLOCATION_STATUS_DISABLE;
    st_lonlat.posSts    = 0x00;
    st_lonlat.posAcc    = 0x00;
    st_lonlat.Longitude = 0x00;
    st_lonlat.Latitude  = 0x00;
    memcpy(&gstLocationLonLat_n.uc_data[0], &st_lonlat, sizeof(st_lonlat));

    return;
}

/**
 * @brief
 *  Latitude and longitudeInformation data master SET process(NAVI information)
 *
 *  Latitude and longitudeUpdate the information data master
 *
 * @param[in]  VEHICLESENS_DATA_MASTER *pst_data     : Pointer to the data master acquisition destination
 * @param[out] none
 * @return     u_int8
 * @retval     VEHICLESENS_EQ  : No data change
 * @retval     VEHICLESENS_NEQ : Data change
 */
u_int8 VehicleSensSetLocationLonLatN(const SENSORLOCATION_LONLATINFO_DAT *pst_lonlat) {
    u_int8            uc_ret;
    VEHICLESENS_DATA_MASTER    *pst_master;

    pst_master = &gstLocationLonLat_n;

    /** With the contents of the current data master,Compare received data */
    uc_ret = VehicleSensmemcmp(pst_master->uc_data, pst_lonlat, sizeof(SENSORLOCATION_LONLATINFO_DAT));

    /** Received data is set in the data master. */
    pst_master->ul_did    = VEHICLE_DID_LOCATION_LONLAT_NAVI;
    pst_master->us_size    = sizeof(SENSORLOCATION_LONLATINFO_DAT);
    pst_master->uc_rcvflag  = VEHICLE_RCVFLAG_ON;
    memcpy(pst_master->uc_data, pst_lonlat, sizeof(SENSORLOCATION_LONLATINFO_DAT));

    return(uc_ret);
}

/**
 * @brief
 *  Latitude and longitudeInformation Data Master GET Processing(NAVI information)
 *
 *  Latitude and longitudeProvide an information data master
 *
 * @param[in]  none
 * @param[out] VEHICLESENS_DATA_MASTER *pst_data : Pointer to the data master acquisition destination
 * @return     none
 * @retval     none
 */
void VehicleSensGetLocationLonLatN(VEHICLESENS_DATA_MASTER *pst_data) {
    const VEHICLESENS_DATA_MASTER *pst_master;

    pst_master = &gstLocationLonLat_n;

    /** Store the data master in the specified destination. */
    pst_data->ul_did    = pst_master->ul_did;
    pst_data->us_size    = pst_master->us_size;
    pst_data->uc_rcvflag  = pst_master->uc_rcvflag;
    memcpy(pst_data->uc_data, pst_master->uc_data, pst_master->us_size);

    return;
}

/**
 * @brief
 *  Latitude and longitudeInformation Data Master GET Processing(NAVI information)
 *
 *  Latitude and longitudeProvide an information data master(Unit:10^-7th degree)
 *
 * @param[out] VEHICLESENS_DATA_MASTER *pst_data : Pointer to the data master acquisition destination
 */
void VehicleSensGetLocationLonLatnUnitCnv(VEHICLESENS_DATA_MASTER *pst_data) {
    const VEHICLESENS_DATA_MASTER *pst_master;
    SENSORLOCATION_LONLATINFO_DAT  st_lonlat;
    int32_t l_lon;
    int32_t l_lat;
    int64_t ll_tmp;

    pst_master = &gstLocationLonLat_n;

    /* Perform unit conversion[1/128Second] -> [10^-7 degree] */
    memcpy(&st_lonlat, pst_master->uc_data, sizeof(st_lonlat));

    /* Longitude */
    l_lon = st_lonlat.Longitude;
    ll_tmp = (int64_t)l_lon * 10000000;
    st_lonlat.Longitude = (int32_t)(ll_tmp / (128 * 60 * 60));

    /* Latitude */
    l_lat = st_lonlat.Latitude;
    ll_tmp = (int64_t)l_lat * 10000000;
    st_lonlat.Latitude = (int32_t)(ll_tmp / (128 * 60 * 60));

    /** Store the data master in the specified destination. */
    pst_data->ul_did    = pst_master->ul_did;
    pst_data->us_size    = pst_master->us_size;
    pst_data->uc_rcvflag  = pst_master->uc_rcvflag;
    memcpy(pst_data->uc_data, &st_lonlat, sizeof(st_lonlat));

    return;
}
