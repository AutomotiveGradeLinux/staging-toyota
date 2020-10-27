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
 *  File name    :VehicleSens_Did_GyroZFst_l.cpp
 *  Subsystem name  :Vehicle sensor process
 *  Program name  :Vehicle sensor data master(POSHAL_DID_GYRO_Z_FST)
 *  Module configuration  :VehicleSensInitGyroZFstl()    Vehicle sensor GYRO (initial sensor) initialization functions
 *                  :VehicleSensSetGyroZFstl()    Vehicle sensor GYRO (initial sensor) SET-function
 *                  :VehicleSensSetGyroZFstG()    Vehicle sensor GYRO (initial sensor) SET-function
 *                  :VehicleSensGetGyroZFstl()    Vehicle sensor GYRO (initial sensor) GET-function
 ******************************************************************************/

#include "VehicleSens_DataMaster.h"

#if CONFIG_SENSOR_EXT_VALID        /* Initial Sensor Support */
/*************************************************/
/*           Global variable                     */
/*************************************************/
static  VEHICLESENS_DATA_MASTER_FST  gstGyroZFst_l;   // NOLINT(readability/nolint)

/*******************************************************************************
* MODULE    : VehicleSensInitGyroZFstl
* ABSTRACT  : Vehicle Sensor GYRO_Z Initialization Functions
* FUNCTION  : GYRO data master initialization process
* ARGUMENT  : void
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensInitGyroZFstl(void) {
    u_int16  *pus;

    memset(&gstGyroZFst_l, 0x00, sizeof(VEHICLESENS_DATA_MASTER_FST));
    gstGyroZFst_l.ul_did        = POSHAL_DID_GYRO_Z_FST;
    gstGyroZFst_l.us_size       = 0;
    gstGyroZFst_l.partition_flg = 0;

    pus = reinterpret_cast<u_int16 *>(gstGyroZFst_l.uc_data);
    memset(reinterpret_cast<void*>(pus), VEHICLE_DINIT_GYRO_Z, VEHICLE_DSIZE_GYRO_Z_FST);
}

/*******************************************************************************
* MODULE    : VehicleSensSetGyroZFstl
* ABSTRACT  : Vehicle Sensor GYRO_Z SET Functions
* FUNCTION  : Update the GYRO data master
* ARGUMENT  : *pst_data : Pointer to the message data received by the direct line
* NOTE      :
* RETURN    : VEHICLESENS_EQ  : No data change
*             VEHICLESENS_NEQ  : Data change
******************************************************************************/
u_int8 VehicleSensSetGyroZFstl(const LSDRV_LSDATA_FST *pst_data) {  // LCOV_EXCL_START 8: dead code.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    u_int8 uc_ret;
    VEHICLESENS_DATA_MASTER_FST *pst_master;

    pst_master = &gstGyroZFst_l;

    /* Compare data master and received data */
    uc_ret = VehicleSensmemcmp(pst_master->uc_data,
            pst_data->uc_data, pst_data->uc_size);

    /* Received data is set in the data master. */
    pst_master->ul_did        = pst_data->ul_did;
    pst_master->us_size       = pst_data->uc_size;
    pst_master->partition_flg = 0;
    pst_master->uc_rcvflag    = VEHICLE_RCVFLAG_ON;
    memset(pst_master->uc_data, 0x00, sizeof(pst_master->uc_data));
    memcpy(pst_master->uc_data, pst_data->uc_data, pst_data->uc_size);

    return(uc_ret);
}
// LCOV_EXCL_STOP

/*******************************************************************************
* MODULE    : VehicleSensSetGyroZFstG
* ABSTRACT  : Vehicle Sensor GYRO_Z SET Functions
* FUNCTION  : Update the GYRO data master
* ARGUMENT  : *pst_data : Pointer to the message data received by the direct line
* NOTE      :
* RETURN    : VEHICLESENS_EQ  : No data change
*             VEHICLESENS_NEQ  : Data change
******************************************************************************/
u_int8 VehicleSensSetGyroZFstG(const LSDRV_LSDATA_FST_GYRO_Z *pst_data) {
    static u_int8   uc_ret  = VEHICLESENS_EQ;
    VEHICLESENS_DATA_MASTER_FST *pst_master;

    u_int8  partition_max;    /* Total number of partitions */
    u_int8  partition_num;    /* Data number                */

    partition_max = pst_data->uc_partition_max;
    partition_num = pst_data->uc_partition_num;

    pst_master = &gstGyroZFst_l;

    if (partition_max == 1) {
        /* Compare data master and received data */
        uc_ret = VehicleSensmemcmp(pst_master->uc_data,
                pst_data->uc_data, pst_data->uc_size);

        /* Received data is set in the data master. */
        pst_master->ul_did        = pst_data->ul_did;
        pst_master->us_size       = pst_data->uc_size;
        pst_master->uc_rcvflag    = VEHICLE_RCVFLAG_ON;
        pst_master->partition_flg = 0;
        memset(pst_master->uc_data, 0x00, sizeof(pst_master->uc_data));
        memcpy(pst_master->uc_data, pst_data->uc_data, pst_data->uc_size);
    } else if (partition_max == 2) {
        if (partition_num == 1) {
            /* Compare data master and received data */
            uc_ret = VehicleSensmemcmp(pst_master->uc_data, pst_data->uc_data, pst_data->uc_size);

            /* Received data is set in the data master. */
            pst_master->ul_did        = pst_data->ul_did;
            pst_master->us_size       = static_cast<u_int16>(pst_master->us_size + pst_data->uc_size);
            pst_master->partition_flg = 1;
            memcpy(pst_master->uc_data, pst_data->uc_data, pst_data->uc_size);
        } else if (partition_num == 2) {
            /* Compare data master and received data */
            if (uc_ret == VEHICLESENS_EQ) {
                uc_ret = VehicleSensmemcmp(&pst_master->uc_data[VEHICLE_DSIZE_GYRO_Z_FST],
                                            pst_data->uc_data, pst_data->uc_size);
            }

            /* Received data is set in the data master. */
            pst_master->ul_did        = pst_data->ul_did;
            pst_master->us_size       = static_cast<u_int16>(pst_master->us_size + pst_data->uc_size);
            pst_master->uc_rcvflag    = VEHICLE_RCVFLAG_ON;
            pst_master->partition_flg = 1;
            memcpy(&pst_master->uc_data[VEHICLE_DSIZE_GYRO_Z_FST], pst_data->uc_data, pst_data->uc_size);
        } else {}
    } else {}

    return(uc_ret);
}

/*******************************************************************************
* MODULE    : VehicleSensGetGyroZFstl
* ABSTRACT  : Vehicle Sensor GYRO_Z GET Functions
* FUNCTION  : Provide a GYRO data master
* ARGUMENT  : *pst_data : Pointer to the data master acquisition destination
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensGetGyroZFstl(VEHICLESENS_DATA_MASTER_FST *pst_data) {
    const VEHICLESENS_DATA_MASTER_FST *pst_master;

    pst_master = &gstGyroZFst_l;

    /* Store the data master in the specified destination. */
    pst_data->ul_did        = pst_master->ul_did;
    pst_data->us_size       = pst_master->us_size;
    pst_data->uc_rcvflag    = pst_master->uc_rcvflag;
    pst_data->partition_flg = pst_master->partition_flg;
    memcpy(pst_data->uc_data, pst_master->uc_data, pst_master->us_size);
}
#endif
