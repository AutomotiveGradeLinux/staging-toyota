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
 *  File name    :VehicleSens_Did_GyroTrouble.cpp
 *  System name    :PastModel002
 *  Subsystem name  :Vehicle sensor process
 *  Program name  :Vehicle sensor data master(VEHICLE_DID_GYRO_TROUBLE)
 *  Module configuration  :VehicleSensInitGyroTrouble()    Vehicle Sensor GYRO TROUBLE Initialization Functions
 *                  :VehicleSensSetGyroTrouble()    Vehicle Sensor GYRO TROUBLE SET Functions
 *                  :VehicleSensGetGyroTrouble()    Vehicle Sensor GYRO TROUBLE GET Functions
 ******************************************************************************/

#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"

#define VEHICLE_SENS_DID_GYRO_TROUBLE_DEBUG_FACTORY 0

/*************************************************/
/*           Global variable                      */
/*************************************************/
static  VEHICLESENS_DATA_MASTER_GYRO_TROUBLE  gstGyroTrouble;    // NOLINT(readability/nolint)

/*******************************************************************************
* MODULE    : VehicleSensInitGyroTrouble
* ABSTRACT  : Vehicle sensor GYRO_TROUBLE initialization function
* FUNCTION  : GYRO_TROUBLE data master initialization processing
* ARGUMENT  : void
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensInitGyroTrouble(void) {
    (void)memset(reinterpret_cast<void *>(&(gstGyroTrouble)),
            static_cast<int>(0x00), sizeof(VEHICLESENS_DATA_MASTER_GYRO_TROUBLE));
    gstGyroTrouble.ul_did  = VEHICLE_DID_GYRO_TROUBLE;
    gstGyroTrouble.us_size  = VEHICLE_DSIZE_GYRO_TROUBLE;
    gstGyroTrouble.uc_data  = VEHICLE_DINIT_GYRO_TROUBLE;
}

/*******************************************************************************
* MODULE    : VehicleSensSetGyroTrouble
* ABSTRACT  : Vehicle Sensor GYRO_TROUBLE SET Function
* FUNCTION  : Update the GYRO_TROUBLE data master
* ARGUMENT  : *pst_data : Pointer to received message data
* NOTE      :
* RETURN    : VEHICLESENS_EQ  : No data change
*             VEHICLESENS_NEQ  : Data change
******************************************************************************/
u_int8 VehicleSensSetGyroTrouble(const LSDRV_MSG_LSDATA_DAT_GYRO_TROUBLE *pst_data) {  // LCOV_EXCL_START 8: dead code.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    u_int8 uc_ret = VEHICLESENS_EQ;
    VEHICLESENS_DATA_MASTER_GYRO_TROUBLE *pst_master;

    if (pst_data == NULL) {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "pst_data == NULL\r\n");
    } else {
        pst_master = &gstGyroTrouble;

        /* Compare data master and received data */
        uc_ret = VehicleSensmemcmp(&(pst_master->uc_data), &(pst_data->uc_data), pst_data->uc_size);

        /* Received data is set in the data master. */
        pst_master->ul_did    = pst_data->ul_did;
        pst_master->us_size    = (u_int16)pst_data->uc_size;
        pst_master->uc_rcvflag  = VEHICLE_RCVFLAG_ON;
        pst_master->uc_data    = pst_data->uc_data;
        (void)memcpy(reinterpret_cast<void *>(&(pst_master->uc_data)),
            (const void *)(&(pst_data->uc_data)), sizeof(pst_master->uc_data));
    }

#if VEHICLE_SENS_DID_GYRO_TROUBLE_DEBUG_FACTORY
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "#[DIAG] pst_data->ul_did == 0x%x", pst_data->ul_did);
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "#[DIAG] gstGyroTrouble.ul_did == 0x%x\r\n", gstGyroTrouble.ul_did);
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "#[DIAG] (u_int8)pst_data->ucSize == 0x%x", (u_int8)pst_data->uc_size);
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "#[DIAG] gstGyroTrouble.us_size == 0x%x\r\n", gstGyroTrouble.us_size);
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "#[DIAG] pst_data->uc_data == 0x%x", pst_data->uc_data);
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "#[DIAG] gstGyroTrouble.uc_data == 0x%x\r\n", gstGyroTrouble.uc_data);
#endif

    return(uc_ret);
}

/*******************************************************************************
* MODULE    : VehicleSensGetGyroTrouble
* ABSTRACT  : Vehicle Sensor GYRO TROUBLE GET Functions
* FUNCTION  : Provide a GYRO TROUBLE data master
* ARGUMENT  : *pst_data : Pointer to the data master acquisition destination
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensGetGyroTrouble(VEHICLESENS_DATA_MASTER_GYRO_TROUBLE *pst_data) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    const VEHICLESENS_DATA_MASTER_GYRO_TROUBLE *pst_master;

    if (pst_data == NULL) {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "pst_data == NULL\r\n");
    } else {
        pst_master = &gstGyroTrouble;

        /* Store the data master in the specified destination. */
        pst_data->ul_did    = pst_master->ul_did;
        pst_data->us_size    = pst_master->us_size;
        pst_data->uc_rcvflag  = pst_master->uc_rcvflag;
        (void)memcpy(reinterpret_cast<void *>(&(pst_data->uc_data)),
            (const void *)(&(pst_master->uc_data)), sizeof(pst_data->uc_data));
    }
}
// LCOV_EXCL_STOP
