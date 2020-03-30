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
 *  File name    :VehicleSens_Did_GpsAntennaStatus.cpp
 *  System name    :PastModel002
 *  Subsystem name  :Vehicle sensor process
 *  Program name  :Vehicle sensor data master(VEHICLE_DID_GPS_ANTENNA)
 *  Module configuration  :VehicleSensInitGpsAntennaStatus()  Vehicle sensor GPS_ANTENNA_STATUS initialization function
 *                  :VehicleSensSetGpsAntennaStatus()    Vehicle sensor GPS_ANTENNA_STATUS SET function
 *                  :VehicleSensGetGpsAntennaStatus()    Vehicle sensor GPS_ANTENNA_STATUS GET function
 ******************************************************************************/

#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"

/*************************************************/
/*           Global variable                      */
/*************************************************/
static  VEHICLESENS_DATA_MASTER_GPS_ANTENNA_STATUS  gstGpsAntennaStatus;    // NOLINT(readability/nolint)

/*******************************************************************************
* MODULE    : VehicleSensInitGpsAntennaStatus
* ABSTRACT  : Vehicle sensor GPS_ANTENNA_STATUS initialization function
* FUNCTION  : GPS_ANTENNA_STATUS data master initialization processing
* ARGUMENT  : void
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensInitGpsAntennaStatus(void) {
    (void)memset(reinterpret_cast<void *>(&gstGpsAntennaStatus), 0, sizeof(gstGpsAntennaStatus));
//    gstGpsAntennaStatus.ul_did    = POSHAL_DID_GPS_ANTENNA;
    gstGpsAntennaStatus.us_size    = VEHICLE_DSIZE_GPS_ANTENNA;
    gstGpsAntennaStatus.uc_rcvflag  = VEHICLE_RCVFLAG_OFF;
    gstGpsAntennaStatus.uc_sensor_cnt  = 0U;
    gstGpsAntennaStatus.uc_data    = VEHICLE_DINIT_GPS_ANTENNA;
}

/*******************************************************************************
* MODULE    : VehicleSensSetGpsAntennaStatus
* ABSTRACT  : Vehicle sensor GPS_ANTENNA_STATUS SET function
* FUNCTION  : Update the GPS_ANTENNA_STATUS data master
* ARGUMENT  : *pst_data : Pointer to received message data
* NOTE      :
* RETURN    : VEHICLESENS_EQ  : No data change
*           : VEHICLESENS_NEQ  : Data change
******************************************************************************/
//u_int8 VehicleSensSetGpsAntennaStatus(const LSDRV_MSG_LSDATA_DAT_GPS_ANTENNA_STATUS *pst_data) {  // LCOV_EXCL_START 8: dead code  // NOLINT(whitespace/line_length)
//    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//    u_int8 uc_ret = VEHICLESENS_EQ;
//    VEHICLESENS_DATA_MASTER_GPS_ANTENNA_STATUS *pst_master;
//
//    if (pst_data == NULL) {
//        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "pst_data == NULL\r\n");
//    } else {
//        pst_master = &gstGpsAntennaStatus;
//
//        /* Compare data master and received data */
//        uc_ret = VehicleSensmemcmp(reinterpret_cast<void *>(&(pst_master->uc_data)),
//            (const void *)(&(pst_data->uc_data)), (size_t)(pst_data->uc_size));
//
//        /* Received data is set in the data master. */
//        pst_master->ul_did    = pst_data->ul_did;
//        pst_master->us_size    = (u_int16)pst_data->uc_size;
//        pst_master->uc_rcvflag  = VEHICLE_RCVFLAG_ON;
//        pst_master->uc_sensor_cnt  = pst_data->uc_sns_cnt;
//        (void)memcpy(reinterpret_cast<void *>(&(pst_master->uc_data)),
//            (const void *)(&(pst_data->uc_data)), sizeof(pst_master->uc_data));
//    }
//    return(uc_ret);
//}
// LCOV_EXCL_STOP

/*******************************************************************************
* MODULE    : VehicleSensGetGpsAntennaStatus
* ABSTRACT  : Vehicle sensor GPS_ANTENNA_STATUS GET function
* FUNCTION  : Provide the GPS_ANTENNA_STATUS data master
* ARGUMENT  : *pst_data : Pointer to the data master acquisition destination
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensGetGpsAntennaStatus(VEHICLESENS_DATA_MASTER_GPS_ANTENNA_STATUS *pst_data) {  // LCOV_EXCL_START 8: dead code  // NOLINT(whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    const VEHICLESENS_DATA_MASTER_GPS_ANTENNA_STATUS *pst_master;
    if (pst_data == NULL) {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "pst_data == NULL\r\n");
    } else {
        pst_master = &gstGpsAntennaStatus;

        /* Store the data master in the specified destination. */
        pst_data->ul_did      = pst_master->ul_did;
        pst_data->us_size      = pst_master->us_size;
        pst_data->uc_rcvflag    = pst_master->uc_rcvflag;
        pst_data->uc_sensor_cnt  = pst_master->uc_sensor_cnt;
        (void)memcpy(reinterpret_cast<void *>(&(pst_data->uc_data)),
            (const void *)(&(pst_master->uc_data)), sizeof(pst_data->uc_data));
    }
}
// LCOV_EXCL_STOP
