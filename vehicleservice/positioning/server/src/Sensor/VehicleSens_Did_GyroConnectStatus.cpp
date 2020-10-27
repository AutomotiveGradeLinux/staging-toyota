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
 *  File name    :VehicleSens_Did_GyroConnectStatus.cpp
 *  System name    :PastModel002
 *  Subsystem name  :Vehicle sensor process
 *  Program name  :Vehicle sensor data master(VEHICLE_DID_GYRO_CONNECT_STATUS)
 *  Module configuration  :VehicleSensInitGyroConnectStatus()    Vehicle Sensor GYRO CONNECT STATUS Initialization Functions
 *                  :VehicleSensSetGyroConnectStatus()    Vehicle Sensor GYRO CONNECT STATUS SET Functions
 *                  :VehicleSensGetGyroConnectStatus()    Vehicle Sensor GYRO CONNECT STATUS GET Functions
 ******************************************************************************/

#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"

/*************************************************/
/*           Global variable                      */
/*************************************************/
static  VEHICLESENS_DATA_MASTER_GYRO_CONNECT_STATUS  gstGyroConnectStatus;    // NOLINT(readability/nolint)

/*******************************************************************************
* MODULE    : VehicleSensInitGyroConnectStatus
* ABSTRACT  : Vehicle sensor GYRO_CONNECT_STATUS initialization function
* FUNCTION  : GYRO_CONNECT_STATUS data master initialization processing
* ARGUMENT  : void
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensInitGyroConnectStatus(void) {
    (void)memset(reinterpret_cast<void *>(&(gstGyroConnectStatus)), static_cast<int>(0x00),
                        sizeof(VEHICLESENS_DATA_MASTER_GYRO_CONNECT_STATUS));
    gstGyroConnectStatus.ul_did  = VEHICLE_DID_GYRO_CONNECT_STATUS;
    gstGyroConnectStatus.us_size  = VEHICLE_DSIZE_GYRO_CONNECT_STATUS;
    gstGyroConnectStatus.uc_data  = VEHICLE_DINIT_GYRO_CONNECT_STATUS;
}

/*******************************************************************************
* MODULE    : VehicleSensSetGyroConnectStatus
* ABSTRACT  : Vehicle sensor GYRO_CONNECT_STATUS SET function
* FUNCTION  : Update the GYRO_CONNECT_STATUS data master
* ARGUMENT  : *pst_data : Pointer to received message data
* NOTE      :
* RETURN    : VEHICLESENS_EQ  : No data change
*             VEHICLESENS_NEQ  : Data change
******************************************************************************/
u_int8 VehicleSensSetGyroConnectStatus(const LSDRV_MSG_LSDATA_DAT_GYRO_CONNECT_STATUS *pst_data) {  // LCOV_EXCL_START 8: dead code.  // NOLINT(whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    u_int8 uc_ret = VEHICLESENS_EQ;
    VEHICLESENS_DATA_MASTER_GYRO_CONNECT_STATUS *pst_master;

    if (pst_data == NULL) {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "pst_data == NULL\r\n");
    } else {
        pst_master = &gstGyroConnectStatus;

        /* Compare data master and received data */
        uc_ret = VehicleSensmemcmp(reinterpret_cast<void *>(&(pst_master->uc_data)),
            (const void *)(&(pst_data->uc_data)), (size_t)(pst_data->uc_size));

        /* Received data is set in the data master. */
        pst_master->ul_did    = pst_data->ul_did;
        pst_master->us_size    = (u_int16)pst_data->uc_size;
        pst_master->uc_rcvflag  = VEHICLE_RCVFLAG_ON;
        (void)memcpy(reinterpret_cast<void *>(&(pst_master->uc_data)),
            (const void *)(&(pst_data->uc_data)), sizeof(pst_master->uc_data));
    }

    return(uc_ret);
}

/*******************************************************************************
* MODULE    : VehicleSensGetGyroConnectStatus
* ABSTRACT  : Vehicle Sensor GYRO TROUBLE GET Functions
* FUNCTION  : Provide a GYRO TROUBLE data master
* ARGUMENT  : *pst_data : Pointer to the data master acquisition destination
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensGetGyroConnectStatus(VEHICLESENS_DATA_MASTER_GYRO_CONNECT_STATUS *pst_data) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    const VEHICLESENS_DATA_MASTER_GYRO_CONNECT_STATUS *pst_master;

    if (pst_data == NULL) {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "pst_data == NULL\r\n");
    } else {
        pst_master = &gstGyroConnectStatus;

        /* Store the data master in the specified destination. */
        pst_data->ul_did    = pst_master->ul_did;
        pst_data->us_size    = pst_master->us_size;
        pst_data->uc_rcvflag  = pst_master->uc_rcvflag;
        (void)memcpy(reinterpret_cast<void *>(&(pst_data->uc_data)),
            (const void *)(&(pst_master->uc_data)), sizeof(pst_data->uc_data));
    }
}
// LCOV_EXCL_STOP
