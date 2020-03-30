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

/***************************************************************************
@file      VehicleSens_Did_Nav_Velned_g.cpp
@detail      Master vehicle sensor data(VEHICLE_DID_GPS_UBLOX_NAV_VELNED)
*****************************************************************************/

#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"

/*************************************************/
/*           Global variable                      */
/*************************************************/
static  VEHICLESENS_DATA_MASTER_GPS_UBLOX_FORMAT  gstNavVelned_g;    // NOLINT(readability/nolint)

/***************************************************************************
@brief      NAV-VELNED vehicle sensor initialization function
@outline    NAV-VELNED initialization process data master
@type      Completion return type
@param[in]    none
@threshold    none
@return      void
@retval      none
@trace
*****************************************************************************/
void VehicleSensInitNavVelnedG(void) {
    memset(&gstNavVelned_g, 0x00, sizeof(VEHICLESENS_DATA_MASTER_GPS_UBLOX_FORMAT));
    gstNavVelned_g.ul_did    = VEHICLE_DID_GPS_UBLOX_NAV_VELNED;
    gstNavVelned_g.us_size    = VEHICLE_DSIZE_GPS_UBLOX_NAV_VELNED + VEHICLE_DSIZE_GPS_UBLOX_HEADER_SIZE;
    gstNavVelned_g.uc_data[0]  = VEHICLE_DINIT_GPS_UBLOX_NAV_VELNED;
}

/***************************************************************************
@brief      NAV-VELNED SET vehicle sensor function
@outline    To update the master data NAV-VELNED.
@type      Completion return type
@param[in]    SENSOR_MSG_GPSDATA_DAT *pst_data  :  A pointer to the received data message or the direct line
@threshold    none
@return      u_int8
@retval      VEHICLESENS_EQ  :   No data changes
@retval      VEHICLESENS_NEQ  :   With data changes
@trace
*****************************************************************************/
//u_int8 VehicleSensSetNavVelnedG(const SENSOR_MSG_GPSDATA_DAT *pst_data) {  // LCOV_EXCL_START 8: dead code
//    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//    u_int8 uc_ret;
//    VEHICLESENS_DATA_MASTER_GPS_UBLOX_FORMAT *pst_master;
//
//    pst_master = &gstNavVelned_g;
//
//    /** Compare data master and received data */
//    uc_ret = VehicleSensmemcmp(pst_master->uc_data, pst_data->uc_data, pst_data->us_size);
//
//    /** Received data is set in the data master. */
//    pst_master->ul_did    = pst_data->ul_did;
//    pst_master->us_size    = pst_data->us_size;
//    pst_master->uc_rcvflag  = VEHICLE_RCVFLAG_ON;
//    pst_master->uc_sensor_cnt  = pst_data->uc_sns_cnt;
//    pst_master->uc_gpscnt_flag  = pst_data->uc_gps_cnt_flag;
//
//    memcpy(pst_master->uc_data, pst_data->uc_data, pst_data->us_size);
//
//    return(uc_ret);
//}

/***************************************************************************
@brief      Vehicle sensor function NAV-VELNED GET
@outline    Master Data provides the NAV-VELNED
@type      Completion return type
@param[in]    SENSOR_MSG_GPSDATA_DAT *pst_data  :  Where to get a pointer to the data master
@threshold    none
@return      void
@retval      none
@trace
*****************************************************************************/
//void VehicleSensGetNavVelnedG(SENSOR_MSG_GPSDATA_DAT *pst_data) {
//    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//    const VEHICLESENS_DATA_MASTER_GPS_UBLOX_FORMAT *pst_master;
//
//    pst_master = &gstNavVelned_g;
//
//    /** Store the data master in the specified destination. */
//    pst_data->ul_did      = pst_master->ul_did;
//    pst_data->us_size      = pst_master->us_size;
//    pst_data->uc_rcv_flag    = pst_master->uc_rcvflag;
//    pst_data->uc_sns_cnt    = pst_master->uc_sensor_cnt;
//    pst_data->uc_gps_cnt_flag  = pst_master->uc_gpscnt_flag;
//    memcpy(pst_data->uc_data, pst_master->uc_data, pst_master->us_size);
//}
// LCOV_EXCL_STOP
