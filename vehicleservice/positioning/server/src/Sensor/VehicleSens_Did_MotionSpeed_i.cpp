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

/**
 * @file
 *  VehicleSens_Did_MotionSpeed_i.cpp
 * @brief
 *  Vehicle Speed Information Data Master Management(Internal calculation information)
 */

/*---------------------------------------------------------------------------------*
 * Include Files                                                                   *
 *---------------------------------------------------------------------------------*/
#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"
#include "CommonDefine.h"

/*---------------------------------------------------------------------------------*
 * Grobal Value                                                                    *
 *---------------------------------------------------------------------------------*/
static VEHICLESENS_DATA_MASTER gstMotionSpeed_i;    // NOLINT(readability/nolint)

/*---------------------------------------------------------------------------------*
 * Function                                                                        *
 *---------------------------------------------------------------------------------*/
/**
 * @brief
 *   Speed information data master initialization process(Internal calculation information)
 */
void VehicleSensInitMotionSpeedI(void) {
    SENSORMOTION_SPEEDINFO_DAT  st_speed;

    memset(&gstMotionSpeed_i, 0x00, sizeof(VEHICLESENS_DATA_MASTER));

    /** Data ID setting */
    gstMotionSpeed_i.ul_did    = VEHICLE_DID_MOTION_SPEED_INTERNAL;
    /** Data size setting */
    gstMotionSpeed_i.us_size    = sizeof(SENSORMOTION_SPEEDINFO_DAT);
    /** Data content setting */
    memset(&st_speed, 0x00, sizeof(st_speed));
    st_speed.getMethod = SENSOR_GET_METHOD_POS;
    st_speed.SyncCnt = 0x00;
    st_speed.isEnable  = SENSORMOTION_STATUS_DISABLE;
    memcpy(&gstMotionSpeed_i.uc_data[0], &st_speed, sizeof(st_speed));
}

/**
 * @brief
 *   Rate information data master SET process(Internal calculation information)
 *
 * @param[in]  *pst_speed  - SENSORMOTION_SPEEDINFO_DAT Pointer to vehicle speed information
 *
 * @return VEHICLESENS_EQ  : No data change<br>
 *       VEHICLESENS_NEQ : Data change
 */
u_int8 VehicleSensSetMotionSpeedI(const SENSORMOTION_SPEEDINFO_DAT *pst_speed) {
    u_int8 uc_ret;
    VEHICLESENS_DATA_MASTER *pst_master;

    pst_master = &gstMotionSpeed_i;

    /** With the contents of the current data master,Compare received data */
    uc_ret = VehicleSensmemcmp(pst_master->uc_data, pst_speed, sizeof(SENSORMOTION_SPEEDINFO_DAT));

    /** Received data is set in the data master. */
    pst_master->ul_did    = VEHICLE_DID_MOTION_SPEED_INTERNAL;
    pst_master->us_size    = sizeof(SENSORMOTION_SPEEDINFO_DAT);
    pst_master->uc_rcvflag  = VEHICLE_RCVFLAG_ON;
    memcpy(pst_master->uc_data, pst_speed, sizeof(SENSORMOTION_SPEEDINFO_DAT));

    return(uc_ret);
}

/**
 * @brief
 *   Speed information data master GET processing(Internal calculation information)
 *
 * @param[out]  *pst_speed  - VEHICLESENS_DATA_MASTER Pointer to the data master acquisition destination
 */
void VehicleSensGetMotionSpeedI(VEHICLESENS_DATA_MASTER *pst_data) {
    const VEHICLESENS_DATA_MASTER *pst_master;

    pst_master = &gstMotionSpeed_i;

    /* Store the data master in the specified destination. */
    pst_data->ul_did    = pst_master->ul_did;
    pst_data->us_size    = pst_master->us_size;
    pst_data->uc_rcvflag  = pst_master->uc_rcvflag;
    memcpy(pst_data->uc_data, pst_master->uc_data, pst_master->us_size);
}
