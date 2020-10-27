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

/*******************************************************************************
 *  File name    :VehicleSens_Did_GpsCounter_g.cpp
 *  System name    :_CWORD107_
 *  Subsystem name  :Vehicle sensor process
 *  Program name  :Vehicle sensor data master(VEHICLE_DID_GPS_COUNTER)
 *  Module configuration  :VehicleSensInitGpsCounterg()    Vehicle sensor GPS_COUNTER initialization function
 *                  :VehicleSensSetGpsCounterg()    Vehicle Sensor GPS_COUNTER SET Function
 *                  :VehicleSensGetGpsCounterg()    Vehicle Sensor GPS_COUNTER GET Function
 ******************************************************************************/

#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"

/*************************************************/
/*           Global variable                      */
/*************************************************/
static  VEHICLESENS_DATA_MASTER  gstGpsCounter_g;    // NOLINT(readability/nolint)

/*******************************************************************************
* MODULE    : VehicleSensInitGpsCounterg
* ABSTRACT  : Vehicle sensor GPS_COUNTER initialization function
* FUNCTION  : GPS_COUNTER data master initialization process
* ARGUMENT  : void
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensInitGpsCounterg(void) {
    memset(&gstGpsCounter_g, 0x00, sizeof(VEHICLESENS_DATA_MASTER));
    gstGpsCounter_g.ul_did    = VEHICLE_DID_GPS_COUNTER;
    gstGpsCounter_g.us_size    = VEHICLE_DSIZE_GPS_COUNTER;
    gstGpsCounter_g.uc_data[0]  = VEHICLE_DINIT_GPS_COUNTER;
}

/*******************************************************************************
* MODULE    : VehicleSensSetGpsCounterg
* ABSTRACT  : Vehicle Sensor GPS_COUNTER SET Function
* FUNCTION  : Update the GPS_COUNTER data master
* ARGUMENT  : *pst_data : Pointer to CAN received message data
* NOTE      :
* RETURN    : VEHICLESENS_EQ  : No data change
*             VEHICLESENS_NEQ  : Data change
******************************************************************************/
u_int8 VehicleSensSetGpsCounterg(const SENSOR_MSG_GPSDATA_DAT *pst_data) {  // LCOV_EXCL_START 8: dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    u_int8 uc_ret;
    VEHICLESENS_DATA_MASTER *pst_master;

    pst_master = &gstGpsCounter_g;

    /* Compare data master and received data */
    uc_ret = VehicleSensmemcmp(pst_master->uc_data, pst_data->uc_data, pst_data->us_size);

    /* Received data is set in the data master. */
    pst_master->ul_did    = pst_data->ul_did;
    pst_master->us_size    = (u_int8)pst_data->us_size;
    pst_master->uc_rcvflag  = VEHICLE_RCVFLAG_ON;
    memcpy(pst_master->uc_data, pst_data->uc_data, pst_data->us_size);

    return(uc_ret);
}

/*******************************************************************************
* MODULE    : VehicleSensGetGpsCounterg
* ABSTRACT  : Vehicle Sensor GPS_COUNTER GET Function
* FUNCTION  : Provide the GPS_COUNTER data master
* ARGUMENT  : *pst_data : Pointer to the data master acquisition destination
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensGetGpsCounterg(SENSOR_MSG_GPSDATA_DAT *pst_data) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    const VEHICLESENS_DATA_MASTER *pst_master;

    pst_master = &gstGpsCounter_g;

    /* Store the data master in the specified destination. */
    pst_data->ul_did    = pst_master->ul_did;
    pst_data->us_size    = pst_master->us_size;
    pst_data->uc_rcv_flag  = pst_master->uc_rcvflag;
    memcpy(pst_data->uc_data, pst_master->uc_data, pst_master->us_size);
}
// LCOV_EXCL_STOP
