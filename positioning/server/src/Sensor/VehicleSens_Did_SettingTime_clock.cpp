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

/**
 * @file
 *   VehicleSens_Did_SettingTime_clock.cpp
 * @brief
 */
/*---------------------------------------------------------------------------------*
 * Include Files                                                                   *
 *---------------------------------------------------------------------------------*/
#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"

/*---------------------------------------------------------------------------------*
 * Global Value                                                                    *
 *---------------------------------------------------------------------------------*/
static  VEHICLESENS_DATA_MASTER  gstSettingTime_clock;    // NOLINT(readability/nolint)

/**
 * @brief
 *  GPS setting time information data master initialization process(NAVI information)
 *
 *  Initialize the GPS setting time information data master
 *
 * @param[in]   none
 * @param[out]  none
 * @return      none
 * @retval      none
 */
void VehicleSensInitSettingTimeclock(void) {
//    POS_DATETIME  st_time;

    memset(&gstSettingTime_clock, 0x00, sizeof(VEHICLESENS_DATA_MASTER));

    /** Data ID setting */
    gstSettingTime_clock.ul_did    = VEHICLE_DID_SETTINGTIME;

    /** Data size setting */
//    gstSettingTime_clock.us_size    = sizeof(POS_DATETIME);

    /** Data content setting */
//    memset(&st_time, 0x00, sizeof(st_time));
//    memcpy(&gstSettingTime_clock.uc_data[0], &st_time, sizeof(st_time));

    return;
}

/**
 * @brief
 *  GPS setting time information data master SET process(NAVI information)
 *
 *  Update the GPS setting time information data master
 *
 * @param[in]  VEHICLESENS_DATA_MASTER *pst_data     : Pointer to the data master acquisition destination
 * @param[out] none
 * @return     u_int8
 * @retval     VEHICLESENS_EQ  : No data change
 * @retval     VEHICLESENS_NEQ : Data change
 */
//u_int8 VehicleSensSetSettingTimeclock(const POS_DATETIME *pst_rcv_time) {
//    u_int8            uc_ret;
//    VEHICLESENS_DATA_MASTER    *pst_master;
//
//    pst_master = &gstSettingTime_clock;
//
//    /** Compare the data master and generated data */
//    uc_ret = VehicleSensmemcmp(pst_master->uc_data, pst_rcv_time, sizeof(POS_DATETIME));
//
//    /** Received data is set in the data master. */
//    pst_master->ul_did    = VEHICLE_DID_SETTINGTIME;
//    pst_master->us_size    = sizeof(POS_DATETIME);
//    pst_master->uc_rcvflag  = VEHICLE_RCVFLAG_ON;
//    memcpy(pst_master->uc_data, pst_rcv_time, sizeof(POS_DATETIME));
//
//    return(uc_ret);
//}

/**
 * @brief
 *  GPS setting time information data master GET process(NAVI information)
 *
 *  Provide the GPS setting time information data master
 *
 * @param[in]  none
 * @param[out] VEHICLESENS_DATA_MASTER *pst_data : Pointer to the data master acquisition destination
 * @return     none
 * @retval     none
 */
void VehicleSensGetSettingTimeclock(VEHICLESENS_DATA_MASTER *pst_data) {
    const VEHICLESENS_DATA_MASTER *pst_master;

    pst_master = &gstSettingTime_clock;

    /** Store the data master in the specified destination. */
    pst_data->ul_did    = pst_master->ul_did;
    pst_data->us_size    = pst_master->us_size;
    pst_data->uc_rcvflag  = pst_master->uc_rcvflag;
    memcpy(pst_data->uc_data, pst_master->uc_data, pst_master->us_size);  /* Ignore->MISRA-C++:2008 Rule 5-0-5 */

    return;
}

