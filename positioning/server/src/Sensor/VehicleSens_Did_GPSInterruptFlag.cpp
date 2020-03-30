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
@file      VehicleSens_Did_GPSInterruptFlag.cpp
@detail      Master vehicle sensor data(VEHICLE_DID_GPS_INTERRUPT_FLAG)
*****************************************************************************/

#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"

/*************************************************/
/*           Global variable                      */
/*************************************************/
static  VEHICLESENS_DATA_MASTER  gstGpsInterruptFlag;    // NOLINT(readability/nolint)

/***************************************************************************
@brief      VehicleSensInitGpsInterruptFlag<BR>
         Vehicle sensor SPEED_PULSE initialization function
@outline    SPEED_PULSE_FLAG initialization process data master
@type      Completion return type
@param[in]    none
@threshold    none
@return      void
@retval      none
@trace
*****************************************************************************/
void VehicleSensInitGpsInterruptFlag(void) {
    memset(&gstGpsInterruptFlag, 0x00, sizeof(VEHICLESENS_DATA_MASTER));
//    gstGpsInterruptFlag.ul_did    = POSHAL_DID_GPS_INTERRUPT_FLAG;
    gstGpsInterruptFlag.us_size    = VEHICLE_DSIZE_GPS_INTERRUPT_FLAG;

    gstGpsInterruptFlag.uc_data[0]  = VEHICLE_DINIT_GPS_INTERRUPT_FLAG;
}

/***************************************************************************
@brief      NAV-CLOCK SET vehicle sensor function
@outline    To update the master data NAV-CLOCK.
@type      Completion return type
@param[in]    SENSOR_MSG_GPSDATA_DAT *pst_data  :  The pointer to GPS incoming message data
@threshold    none
@return      u_int8
@retval      VEHICLESENS_EQ  :   No data changes
@retval      VEHICLESENS_NEQ  :   With data changes
@trace
*****************************************************************************/
//u_int8 VehicleSensSetGpsInterruptFlag(const LSDRV_LSDATA_G *pst_data) {  // LCOV_EXCL_START 8: dead code.
//    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//    u_int8 uc_ret;
//    VEHICLESENS_DATA_MASTER *pst_master;
//
//    pst_master = &gstGpsInterruptFlag;
//
//    /** Compare data master and received data */
//    uc_ret = VehicleSensmemcmp(pst_master->uc_data, pst_data->uc_data, pst_data->uc_size);
//
//    /** Received data is set in the data master. */
//    pst_master->ul_did    = pst_data->ul_did;
//    pst_master->us_size    = pst_data->uc_size;
//    pst_master->uc_rcvflag  = VEHICLE_RCVFLAG_ON;
//    pst_master->uc_snscnt    = pst_data->uc_sns_cnt;
//    (void)memcpy(reinterpret_cast<void *>(pst_master->uc_data),
//        (const void *)(pst_data->uc_data), (size_t)(pst_data->uc_size));
//
//    return(uc_ret);
//}

/***************************************************************************
@brief      Vehicle sensor function NAV-CLOCK GET
@outline    Master Data provides the NAV-CLOCK
@type      Completion return type
@param[in]    SENSOR_MSG_GPSDATA_DAT *pst_data  :  Where to get a pointer to the data master
@threshold    none
@return      void
@retval      none
@trace
*****************************************************************************/
void VehicleSensGetGpsInterruptFlag(VEHICLESENS_DATA_MASTER *pst_data) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    const VEHICLESENS_DATA_MASTER *pst_masterdata;

    pst_masterdata = &gstGpsInterruptFlag;

    /** Store the data master in the specified destination. */
    pst_data->ul_did    = pst_masterdata->ul_did;
    pst_data->us_size    = pst_masterdata->us_size;
    pst_data->uc_rcvflag  = pst_masterdata->uc_rcvflag;
    pst_data->uc_snscnt  = pst_masterdata->uc_snscnt;
    (void)memcpy(reinterpret_cast<void *>(pst_data->uc_data),
        (const void *)(pst_masterdata->uc_data), (size_t)(pst_masterdata->us_size));
}
// LCOV_EXCL_STOP
