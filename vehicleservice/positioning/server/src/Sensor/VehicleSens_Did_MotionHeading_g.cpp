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
@file       VehicleSens_Did_MotionHeading_g.cpp
@detail     Orientation Information Data Master Management(NMEA information)
******************************************************************************/
#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"
#include "CommonDefine.h"

/*************************************************
 *           Global variable                      *
 *************************************************/
static  VEHICLESENS_DATA_MASTER  gstMotionHeading_g;  // NOLINT(readability/nolint)

/****************************************************************************
@brief      VehicleSensInitMotionHeadingG<BR>
            Orientation information data master initialization process(NMEA information)
@outline    Initialize the orientation information data master
@param[in]  none
@param[out] none
@return     none
@retval     none
*******************************************************************************/
void VehicleSensInitMotionHeadingG(void) {
    SENSORMOTION_HEADINGINFO_DAT  st_heading;

    memset(&gstMotionHeading_g, 0x00, sizeof(VEHICLESENS_DATA_MASTER));

    /** Data ID setting */
    gstMotionHeading_g.ul_did    = VEHICLE_DID_MOTION_HEADING;
    /** Data size setting */
    gstMotionHeading_g.us_size    = sizeof(SENSORMOTION_HEADINGINFO_DAT);
    /** Data content setting */
    memset(&st_heading, 0x00, sizeof(st_heading));
    st_heading.getMethod = SENSOR_GET_METHOD_GPS;
    st_heading.SyncCnt   = 0x00;
    st_heading.isEnable  = SENSORMOTION_STATUS_DISABLE;
    memcpy(&gstMotionHeading_g.uc_data[0], &st_heading, sizeof(st_heading));
}

/****************************************************************************
@brief      VehicleSensSetMotionHeadingG<BR>
            Compass Data Master SET Processing(NMEA information)
@outline    Update the orientation information data master
@param[in]  SENSORMOTION_HEADINGINFO_DAT*    pst_heading   : Bearing information
@param[out] none
@return     u_int8
@retval     VEHICLESENS_EQ  : No data change
@retval     VEHICLESENS_NEQ : Data change
*******************************************************************************/
u_int8 VehicleSensSetMotionHeadingG(const SENSORMOTION_HEADINGINFO_DAT *pst_heading) {
    u_int8  uc_ret;
    VEHICLESENS_DATA_MASTER      *pst_master;

    pst_master = &gstMotionHeading_g;

    /** With the contents of the current data master,Compare received data */
    uc_ret = VehicleSensmemcmp(pst_master->uc_data, pst_heading,
                    sizeof(SENSORMOTION_HEADINGINFO_DAT));

    /** Received data is set in the data master. */
    pst_master->ul_did    = VEHICLE_DID_MOTION_HEADING;
    pst_master->us_size    = sizeof(SENSORMOTION_HEADINGINFO_DAT);
    pst_master->uc_rcvflag  = VEHICLE_RCVFLAG_ON;
    memset(pst_master->uc_data, 0x00, sizeof(pst_master->uc_data));
    memcpy(pst_master->uc_data, pst_heading, sizeof(SENSORMOTION_HEADINGINFO_DAT));

    return(uc_ret);
}

/****************************************************************************
@brief      VehicleSensGetMotionHeadingG<BR>
            Compass Data Master GET Processing(NMEA information)
@outline    Provide an orientation information data master
@param[in]  none
@param[out] VEHICLESENS_DATA_MASTER *pst_data : Pointer to the data master acquisition destination
@return     none
@retval     none
*******************************************************************************/
void VehicleSensGetMotionHeadingG(VEHICLESENS_DATA_MASTER *pst_data) {
    const VEHICLESENS_DATA_MASTER *pst_master;

    pst_master = &gstMotionHeading_g;

    /* Store the data master in the specified destination. */
    pst_data->ul_did    = pst_master->ul_did;
    pst_data->us_size    = pst_master->us_size;
    pst_data->uc_rcvflag  = pst_master->uc_rcvflag;
    memcpy(pst_data->uc_data, pst_master->uc_data, pst_master->us_size);  /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
}
