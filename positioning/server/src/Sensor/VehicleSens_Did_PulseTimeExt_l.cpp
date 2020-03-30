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
 *  VehicleSens_Did_PulseTimeExt_l.cpp
 * @brief
 *  Vehicle sensor data master(POSHAL_DID_PULSE_TIME)
 */

#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"

/*---------------------------------------------------------------------------------*
 * Global Value                                                                    *
 *---------------------------------------------------------------------------------*/
//static VEHICLESENS_DATA_MASTER_EXT  g_st_pulsetime_ext_l;    // NOLINT(readability/nolint)

/**
 * @brief
 *   Vehicle sensor inter-pulse time initialization function
 *
 *   Inter-pulse time data master initialization processing
 */
void VehicleSensInitPulseTimeExtl(void) {
//    (void)memset(reinterpret_cast<void *>(&g_st_pulsetime_ext_l), 0, sizeof(VEHICLESENS_DATA_MASTER_EXT));
//    g_st_pulsetime_ext_l.ul_did    = POSHAL_DID_PULSE_TIME;
//    g_st_pulsetime_ext_l.us_size  = VEHICLE_DSIZE_PULSE_TIME_EXT_INIT;
//    g_st_pulsetime_ext_l.uc_rcvflag  = VEHICLE_RCVFLAG_OFF;
}

/**
 * @brief
 *   Vehicle sensor pulse time SET function
 *
 *   Update the interpulse time data master
 *
 * @param[in]  *pst_data : Pointer to the message data received by the direct line
 *
 * @return  VEHICLESENS_EQ  No data change<br>
 *      VEHICLESENS_NEQ Data change
 */
//void VehicleSensSetPulseTimeExtlG(const LSDRV_LSDATA_G *pst_data) {
//    VEHICLESENS_DATA_MASTER_EXT *pst_master;
//    u_int16 us_size = 0;
//    u_int16 us_start = 0;
//    u_int16 us_cnt = 0;
//
//    pst_master = &g_st_pulsetime_ext_l;
//    /* 4byte * (Number of data items + 32 data items) */
//    us_size = static_cast<u_int16>(sizeof(u_int32) * (1 + VEHICLE_SNS_INFO_PULSE_NUM));
//
//    /* Retrieve the location where the received one is stored */
//    us_start = gstPkgTempExt.start_point[7];
//
//    /* Stored in data master(Order of reception)*/
//    if (us_start >= VEHICLE_DKEEP_MAX) {
//        /* Store the latest one at position 0 */
//        us_start = VEHICLE_DATA_POS_00;
//        /* If you are discarding old data,,Set a flag */
//        gstPkgTempExt.data_break = VEHICLE_SNS_BREAK;
//    }
//    pst_master->ul_did    = pst_data->ul_did;
//    pst_master->uc_rcvflag  = VEHICLE_RCVFLAG_ON;
//    for (us_cnt = 0; us_cnt < us_size; us_cnt++) {
//        pst_master->uc_data[us_start * us_size + us_cnt] = (u_int8)(pst_data->uc_data[us_cnt]);
//    }
//
//    /* Update next storage start position and latest data storage position */
//    us_start++;
//    gstPkgTempExt.start_point[7] = us_start;
//
//    /* Update data master size */
//    if (gstPkgTempExt.data_break == VEHICLE_SNS_BREAK) {
//        /* Make the size of all extended data masters */
//        pst_master->us_size = VEHICLE_DSIZE_PULSE_TIME_EXT;
//    } else {
//        /* Add the size of one received data item */
//        pst_master->us_size = static_cast<u_int16>(pst_master->us_size + us_size);
//    }
//}

/**
 * @brief
 *   Vehicle sensor pulse time GET function
 *
 *   Provide interpulse time data master
 *
 * @param[in]  Pointer to the data master acquisition destination
 */
//void VehicleSensGetPulseTimeExtl(VEHICLESENS_DATA_MASTER_EXT *pst_data) {
//    const VEHICLESENS_DATA_MASTER_EXT *pst_master;
//    u_int16 us_size = 0;
//    u_int16 us_data_cnt = 0;
//    u_int16 us_cnt = 0;
//    u_int16 us_loop_cnt = 0;
//
//    /* Store the data master in the specified destination. */
//    pst_master      = &g_st_pulsetime_ext_l;
//    pst_data->ul_did    = pst_master->ul_did;
//    pst_data->us_size    = pst_master->us_size;
//    pst_data->uc_rcvflag  = pst_master->uc_rcvflag;
//
//    /* Size of one data item: 4byte * (Number of data items + 32 data items) */
//    us_size = static_cast<u_int16>(sizeof(u_int32) * (1 + VEHICLE_SNS_INFO_PULSE_NUM));
//
//    /* Checking whether the number of stored entries is looped */
//    if (gstPkgTempExt.data_break == VEHICLE_SNS_BREAK) {
//        us_data_cnt = VEHICLE_DKEEP_MAX;
//    } else {
//        us_data_cnt = gstPkgTempExt.start_point[7];
//    }
//
//    /* Acquire data from the newest data master */
//    us_loop_cnt = 0;
//    for (us_cnt = 0; us_cnt < us_data_cnt; us_cnt++) {
//        if (gstPkgTempExt.data_break == VEHICLE_SNS_BREAK) {
//            /* Get information after loop */
//            if (gstPkgTempExt.start_point[7] > us_cnt) {
//                memcpy(&pst_data->uc_data[us_cnt * us_size],
//                    &pst_master->uc_data[(gstPkgTempExt.start_point[7] - us_cnt - 1) * us_size], us_size);
//                us_loop_cnt++;
//            } else {
//                memcpy(&pst_data->uc_data[us_cnt * us_size],
//                    &pst_master->uc_data[(us_data_cnt - us_cnt - 1 + us_loop_cnt) * us_size], us_size);
//            }
//        } else {
//            if ((u_int32)(us_data_cnt - us_cnt) <= (u_int32)VEHICLE_DKEEP_MAX) {  // LCOV_EXCL_BR_LINE 6: always true
//                memcpy(&pst_data->uc_data[us_cnt * us_size],
//                    &pst_master->uc_data[(us_data_cnt - us_cnt - 1) * us_size], us_size);
//            }
//        }
//    }
//}
