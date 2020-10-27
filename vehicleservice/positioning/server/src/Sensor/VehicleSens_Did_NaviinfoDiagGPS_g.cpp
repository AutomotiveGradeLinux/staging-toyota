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
@file       VehicleSens_Did_NaviinfoDiagGPS_g.cpp
@detail     Management of GPS Information Master for Diag
******************************************************************************/
#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"

/*************************************************
 *           Global variable                      *
 *************************************************/
static  VEHICLESENS_DATA_MASTER_GPS_FORMAT  gstNaviinfoDiagGPS_g;    // NOLINT(readability/nolint)

/****************************************************************************
@brief      VehicleSensInitNaviinfoDiagGPSg<BR>
            Initialization of GPS Data Master for Diag
@outline    Initializing GPS Data Master for Diag
@param[in]  none
@param[out] none
@return     none
@retval     none
*******************************************************************************/
void VehicleSensInitNaviinfoDiagGPSg(void) {
    memset(&gstNaviinfoDiagGPS_g, 0x00, sizeof(VEHICLESENS_DATA_MASTER_GPS_FORMAT));

    /** Data ID setting */
    gstNaviinfoDiagGPS_g.ul_did    = VEHICLE_DID_NAVIINFO_DIAG_GPS;
    /** Data size setting */
    gstNaviinfoDiagGPS_g.us_size    = sizeof(NAVIINFO_DIAG_GPS);
}

/****************************************************************************
@brief      VehicleSensSetNaviinfoDiagGPSg<BR>
            GPS Information Master SET Processing for Diag
@outline    Update the GPS Data Master for Diag
@param[in]  NAVIINFO_DIAG_GPS*  pst_diag_data     : GPS information for Diag
@param[out] none
@return     u_int8
@retval     VEHICLESENS_EQ  : No data change
@retval     VEHICLESENS_NEQ : Data change
*******************************************************************************/
u_int8 VehicleSensSetNaviinfoDiagGPSg(const NAVIINFO_DIAG_GPS *pst_diag_data) {
    u_int8  uc_ret;
    VEHICLESENS_DATA_MASTER_GPS_FORMAT  *pst_master;

    pst_master = &gstNaviinfoDiagGPS_g;

    /** With the contents of the current data master,Compare received data */
    uc_ret = VehicleSensmemcmp(pst_master->uc_data, pst_diag_data, sizeof(NAVIINFO_DIAG_GPS));

    /** Received data is set in the data master. */
    pst_master->ul_did    = VEHICLE_DID_NAVIINFO_DIAG_GPS;
    pst_master->us_size    = sizeof(NAVIINFO_DIAG_GPS);
    pst_master->uc_rcvflag  = VEHICLE_RCVFLAG_ON;
    memcpy(pst_master->uc_data, pst_diag_data, sizeof(NAVIINFO_DIAG_GPS));

    return(uc_ret);
}

/****************************************************************************
@brief      VehicleSensGetNaviinfoDiagGPSg<BR>
            GPS Information Master GET Processing for Diag
@outline    Provide a master GPS information for Diag
@param[in]  none
@param[out] SENSOR_MSG_GPSDATA_DAT *pst_data : Pointer to the data master acquisition destination
@return     none
@retval     none
*******************************************************************************/
void VehicleSensGetNaviinfoDiagGPSg(SENSOR_MSG_GPSDATA_DAT *pst_data) {
    const VEHICLESENS_DATA_MASTER_GPS_FORMAT *pst_master;

    pst_master = reinterpret_cast<VEHICLESENS_DATA_MASTER_GPS_FORMAT*>(&gstNaviinfoDiagGPS_g);

    /* Store the data master in the specified destination. */
    pst_data->ul_did    = pst_master->ul_did;
    pst_data->us_size    = pst_master->us_size;
    pst_data->uc_rcv_flag  = pst_master->uc_rcvflag;
    memcpy(pst_data->uc_data, pst_master->uc_data, pst_master->us_size);  /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
}
