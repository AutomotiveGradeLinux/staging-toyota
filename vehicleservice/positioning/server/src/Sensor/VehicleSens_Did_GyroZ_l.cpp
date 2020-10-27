/*
 * @copyright Copyright (c) 2019-2020 TOYOTA MOTOR CORPORATION.
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
 *  File name    :VehicleSens_Did_GyroZ_l.cpp
 *  Subsystem name  :Vehicle sensor process
 *  Program name  :Vehicle sensor data master(POSHAL_DID_GYRO_Z)
 *  Module configuration  :VehicleSensInitGyroZl()    Vehicle Sensor GYRO Initialization Functions
 *                  :VehicleSensSetGyroZl()    Vehicle Sensor GYRO SET Functions
 *                  :VehicleSensGetGyroZl()    Vehicle Sensor GYRO GET Functions
 ******************************************************************************/

#include "VehicleSens_DataMaster.h"

/*************************************************/
/*           Global variable                      */
/*************************************************/
static  VEHICLESENS_DATA_MASTER  gstGyroZ_l;    // NOLINT(readability/nolint)

/*******************************************************************************
* MODULE    : VehicleSensInitGyroZl
* ABSTRACT  : Vehicle Sensor GYRO_Z Initialization Functions
* FUNCTION  : GYRO data master initialization process
* ARGUMENT  : void
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensInitGyroZl(void) {
    memset(&gstGyroZ_l, 0x00, sizeof(VEHICLESENS_DATA_MASTER));
    gstGyroZ_l.ul_did     = POSHAL_DID_GYRO_Z;
    gstGyroZ_l.us_size    = VEHICLE_DSIZE_GYRO_Z;
    gstGyroZ_l.uc_rcvflag = VEHICLE_RCVFLAG_OFF;
}

/*******************************************************************************
* MODULE    : VehicleSensSetGyroZl
* ABSTRACT  : Vehicle Sensor GYRO_Z SET Functions
* FUNCTION  : Update the GYRO data master
* ARGUMENT  : *pst_data : Pointer to the message data received by the direct line
* NOTE      :
* RETURN    : VEHICLESENS_EQ  : No data change
*             VEHICLESENS_NEQ  : Data change
******************************************************************************/
u_int8 VehicleSensSetGyroZl(const LSDRV_LSDATA *pst_data) {
    u_int8 uc_ret;
    VEHICLESENS_DATA_MASTER *pst_master;

    pst_master = &gstGyroZ_l;

    /* Compare data master and received data */
    uc_ret = VehicleSensmemcmp(pst_master->uc_data, pst_data->uc_data, pst_data->uc_size);

    /* Received data is set in the data master. */
    pst_master->ul_did     = pst_data->ul_did;
    pst_master->us_size    = pst_data->uc_size;
    pst_master->uc_rcvflag = VEHICLE_RCVFLAG_ON;
    pst_master->uc_snscnt  = pst_data->uc_sns_cnt;
    (void)memcpy(reinterpret_cast<void *>(pst_master->uc_data),
        (const void *)(pst_data->uc_data), (size_t)pst_data->uc_size);

    return(uc_ret);
}

/*******************************************************************************
* MODULE    : VehicleSensSetGyroZlG
* ABSTRACT  : Vehicle Sensor GYRO_Z SET Functions
* FUNCTION  : Update the GYRO data master
* ARGUMENT  : *pst_data : Pointer to the message data received by the direct line
* NOTE      :
* RETURN    : VEHICLESENS_EQ  : No data change
*             VEHICLESENS_NEQ  : Data change
******************************************************************************/
u_int8 VehicleSensSetGyroZlG(const LSDRV_LSDATA_G *pst_data) {
    u_int8 uc_ret;
    VEHICLESENS_DATA_MASTER *pst_master;

    pst_master = &gstGyroZ_l;

    /* Compare data master and received data */
    uc_ret = VehicleSensmemcmp(pst_master->uc_data, pst_data->uc_data, pst_data->uc_size);

    /* Received data is set in the data master. */
    pst_master->ul_did     = pst_data->ul_did;
    pst_master->us_size    = pst_data->uc_size;
    pst_master->uc_rcvflag = VEHICLE_RCVFLAG_ON;
    pst_master->uc_snscnt  = pst_data->uc_sns_cnt;
    (void)memcpy(reinterpret_cast<void *>(pst_master->uc_data),
        (const void *)(pst_data->uc_data), (size_t)(pst_data->uc_size));

    return(uc_ret);
}

/*******************************************************************************
* MODULE    : VehicleSensGetGyroZl
* ABSTRACT  : Vehicle Sensor GYRO_Z GET Functions
* FUNCTION  : Provide a GYRO data master
* ARGUMENT  : *pst_data : Pointer to the data master acquisition destination
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensGetGyroZl(VEHICLESENS_DATA_MASTER *pst_data) {
    const VEHICLESENS_DATA_MASTER *pst_master;

    pst_master = &gstGyroZ_l;

    /* Store the data master in the specified destination. */
    pst_data->ul_did     = pst_master->ul_did;
    pst_data->us_size    = pst_master->us_size;
    pst_data->uc_rcvflag = pst_master->uc_rcvflag;
    pst_data->uc_snscnt  = pst_master->uc_snscnt;
    (void)memcpy(reinterpret_cast<void *>(pst_data->uc_data),
        (const void *)(pst_master->uc_data), (size_t)(pst_master->us_size));
}
