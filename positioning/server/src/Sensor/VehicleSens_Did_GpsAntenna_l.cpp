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

/*******************************************************************************
 *  File name    :VehicleSens_Did_GpsAntenna_l.cpp
 *  System name    :_CWORD107_
 *  Subsystem name  :Vehicle sensor process
 *  Program name  :Vehicle sensor data master(VEHICLE_DID_GPS_ANTENNA)
 *  Module configuration  :VehicleSensInitGpsAntennal()  Vehicle sensor GPS_ANTENNA initialization function
 *                  :VehicleSensSetGpsAntennal()    Vehicle Sensor GPS_ANTENNA SET Function
 *                  :VehicleSensGetGpsAntennal()    Vehicle Sensor GPS_ANTENNA GET Function
 ******************************************************************************/

#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"

/*************************************************/
/*           Global variable                      */
/*************************************************/
static  VEHICLESENS_DATA_MASTER  gstGpsAntenna_l;    // NOLINT(readability/nolint)

/*******************************************************************************
* MODULE    : VehicleSensInitGpsAntennal
* ABSTRACT  : Vehicle sensor GPS_ANTENNA initialization function
* FUNCTION  : GPS_ANTENNA data master initialization processing
* ARGUMENT  : void
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensInitGpsAntennal(void) {
    memset(&gstGpsAntenna_l, 0x00, sizeof(VEHICLESENS_DATA_MASTER));
//    gstGpsAntenna_l.ul_did    = POSHAL_DID_GPS_ANTENNA;
    gstGpsAntenna_l.us_size    = VEHICLE_DSIZE_GPS_ANTENNA;
    gstGpsAntenna_l.uc_data[0]  = VEHICLE_DINIT_GPS_ANTENNA;
}

/*******************************************************************************
* MODULE    : VehicleSensSetGpsAntennal
* ABSTRACT  : Vehicle Sensor GPS_ANTENNA SET Function
* FUNCTION  : Update the GPS_ANTENNA data master
* ARGUMENT  : *pst_data : Pointer to the message data received by the direct line
* NOTE      :
* RETURN    : VEHICLESENS_EQ  : No data change
*             VEHICLESENS_NEQ  : Data change
******************************************************************************/
//u_int8 VehicleSensSetGpsAntennal(const LSDRV_LSDATA *pst_data) {
//    u_int8 uc_ret;
//    VEHICLESENS_DATA_MASTER *pst_master;
//
//    pst_master = &gstGpsAntenna_l;
//
//    /* Compare data master and received data */
//    /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//    uc_ret = VehicleSensmemcmp(pst_master->uc_data, pst_data->uc_data, pst_data->uc_size);
//
//    /* Received data is set in the data master. */
//    pst_master->ul_did    = pst_data->ul_did;
//    pst_master->us_size    = pst_data->uc_size;  /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//    pst_master->uc_rcvflag  = VEHICLE_RCVFLAG_ON;
//    memset(pst_master->uc_data, 0x00, sizeof(pst_master->uc_data));
//    memcpy(pst_master->uc_data, pst_data->uc_data, pst_data->uc_size);  /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//
//    return(uc_ret);
//}

/*******************************************************************************
* MODULE    : VehicleSensGetGpsAntennal
* ABSTRACT  : Vehicle Sensor GPS_ANTENNA GET Function
* FUNCTION  : Provide the GPS_ANTENNA data master
* ARGUMENT  : *pst_data : Pointer to the data master acquisition destination
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensGetGpsAntennal(VEHICLESENS_DATA_MASTER *pst_data) {
    const VEHICLESENS_DATA_MASTER *pst_master;

    pst_master = &gstGpsAntenna_l;

    /* Store the data master in the specified destination. */
    pst_data->ul_did    = pst_master->ul_did;
    pst_data->us_size    = pst_master->us_size;
    pst_data->uc_rcvflag  = pst_master->uc_rcvflag;
    memcpy(pst_data->uc_data, pst_master->uc_data, pst_master->us_size);  /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
}
