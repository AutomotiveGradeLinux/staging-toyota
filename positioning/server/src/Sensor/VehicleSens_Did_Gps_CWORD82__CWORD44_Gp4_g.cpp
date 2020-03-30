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
 *  File name    :VehicleSens_Did_Gps_CWORD82_Nmea_g.cpp
 *  System name    :_CWORD72_
 *  Subsystem name  :Vehicle sensor process
 *  Program name  :Vehicle sensor data master(VEHICLE_DID_GPS__CWORD82__FULLBINARY)
 *  Module configuration  :VehicleSensInitGps_CWORD82_FullBinaryG()  Vehicle sensor GPS__CWORD82__FULLBINARY initialization function
 *              :VehicleSensSetGps_CWORD82_FullBinaryG()  Vehicle sensor GPS__CWORD82__FULLBINARY SET function
 *              :VehicleSensGetGps_CWORD82_FullBinaryG()  Vehicle sensor GPS__CWORD82__FULLBINARY GET function
 ******************************************************************************/

#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"
//#include "gps_hal.h"

/*************************************************/
/*           Global variable                      */
/*************************************************/
static  VEHICLESENS_DATA_MASTER_GPS_FORMAT  gstGps_CWORD82__CWORD44_Gp4_g;    // NOLINT(readability/nolint)

/*******************************************************************************
* MODULE    : VehicleSensInitGps_CWORD82__CWORD44_Gp4G
* ABSTRACT  : Vehicle sensor GPS__CWORD44__GP4 initialization function
* FUNCTION  : GPS__CWORD82___CWORD44__GP4 data master initialization processing
* ARGUMENT  : void
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensInitGps_CWORD82__CWORD44_Gp4G(void) {
    memset(&gstGps_CWORD82__CWORD44_Gp4_g, 0x00, sizeof(VEHICLESENS_DATA_MASTER_GPS_FORMAT));
//    gstGps_CWORD82__CWORD44_Gp4_g.ul_did  = POSHAL_DID_GPS__CWORD82___CWORD44_GP4;
    /* Initialize with _CWORD82_ only and size fixed VEHICLE_DSIZE_GPS_NMEA_PJRDC_GP_4 */
    /* GPS antenna connection information(1byte) + Sensor Counter(1byte) + VEHICLE_DSIZE_GPS_NMEA_PJRDC_GP_4 */
    gstGps_CWORD82__CWORD44_Gp4_g.us_size  = (VEHICLE_DSIZE_GPS_ANTENNA + VEHICLE_DSIZE_SNS_COUNTER) \
    + (VEHICLE_DSIZE_GPS_NMEA_PJRDC_GP_4);
}

/*******************************************************************************
* MODULE    : VehicleSensSetGps_CWORD82__CWORD44_Gp4G
* ABSTRACT  : Vehicle sensor GPS_NMEA SET function
* FUNCTION  : Update the GPS__CWORD82___CWORD44__GP4 data master
* ARGUMENT  : *pst_data : Pointer to CAN received message data
* NOTE      :
* RETURN    : VEHICLESENS_EQ   : No data change
*             VEHICLESENS_NEQ  : Data change
*******************************************************************************/
//u_int8 VehicleSensSetGps_CWORD82__CWORD44_Gp4G(const SENSOR_MSG_GPSDATA_DAT *pst_data) {  // LCOV_EXCL_START 8: dead code
//    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//    u_int8 uc_ret;
//    VEHICLESENS_DATA_MASTER_GPS_FORMAT *pst_master;
//
//    pst_master = &gstGps_CWORD82__CWORD44_Gp4_g;
//
//    /* Compare data master and received data */
//    uc_ret = VehicleSensmemcmp(pst_master->uc_data, pst_data->uc_data, pst_data->us_size);
//
//    /* Received data is set in the data master. */
//    pst_master->ul_did        = pst_data->ul_did;
//    pst_master->us_size       = (u_int8)pst_data->us_size;
//    pst_master->uc_rcvflag    = VEHICLE_RCVFLAG_ON;
//    memcpy(pst_master->uc_data, pst_data->uc_data, pst_data->us_size);
//
//    return(uc_ret);
//}

/*******************************************************************************
* MODULE    : VehicleSensGetGps_CWORD82__CWORD44_Gp4G
* ABSTRACT  : Vehicle sensor GPS__CWORD44__GP4 GET function
* FUNCTION  : Provide the GPS__CWORD82___CWORD44__GP4 data master
* ARGUMENT  : *pst_data : Pointer to the data master acquisition destination
* NOTE      :
* RETURN    : void
******************************************************************************/
//void VehicleSensGetGps_CWORD82__CWORD44_Gp4G(SENSOR_MSG_GPSDATA_DAT *pst_data) {
//    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//    const VEHICLESENS_DATA_MASTER_GPS_FORMAT *pst_master;
//
//    pst_master = &gstGps_CWORD82__CWORD44_Gp4_g;
//
//    /* Store the data master in the specified destination. */
//    pst_data->ul_did    = pst_master->ul_did;
//    pst_data->us_size    = pst_master->us_size;
//    pst_data->uc_rcv_flag  = pst_master->uc_rcvflag;
//    memcpy(pst_data->uc_data, pst_master->uc_data, pst_master->us_size);  /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//}
// LCOV_EXCL_STOP
