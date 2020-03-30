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
 *  File name    :VehicleSens_Did_GsnsYExt_l.cpp
 *  System name    :_CWORD107_
 *  Subsystem name  :Vehicle sensor process
 *  Program name  :Vehicle sensor data master(POSHAL_DID_GSNS_Y)
 *  Module configuration  :VehicleSensInitGsnsYExtl()    Vehicle sensor GSNS_Y initialization function
 *                  :VehicleSensSetGsnsYExtlG()    Vehicle sensor GSNS_Y SET function
 *                  :VehicleSensGetGsnsYExtl()    Vehicle sensor GSNS_Y GET function
 ******************************************************************************/

#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"

/* Ignore->MISRA-C++:2008 Rule 2-7-2 */
#if CONFIG_SENSOR_EXT_VALID        /* Initial Sensor Support */
/*************************************************/
/*           Global variable                      */
/*************************************************/
static  VEHICLESENS_DATA_MASTER_EXT  gstGsnsYExt_l;  // NOLINT(readability/nolint)

/*******************************************************************************
* MODULE    : VehicleSensInitGsnsYExtl
* ABSTRACT  : Vehicle sensor GSNS_Y initialization function
* FUNCTION  : GSNS_Y data master initialization processing
* ARGUMENT  : void
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensInitGsnsYExtl(void) {
    memset(&gstGsnsYExt_l, 0x00, sizeof(VEHICLESENS_DATA_MASTER_EXT));
//    gstGsnsYExt_l.ul_did    = POSHAL_DID_GSNS_Y;
    gstGsnsYExt_l.us_size  = VEHICLE_DSIZE_GSNS_Y_EXT_INIT;
    gstGsnsYExt_l.uc_rcvflag  = VEHICLE_RCVFLAG_OFF;
}

/**
 * @brief
 *   Vehicle sensor GSNS_Y SET function
 *
 *   Update the GSNS_Y data master
 *
 * @param[in]  *pst_data : Pointer to the message data received by the direct line
 */
//void VehicleSensSetGsnsYExtlG(const LSDRV_LSDATA_G *pst_data) {
//    VEHICLESENS_DATA_MASTER_EXT *pst_master;
//    u_int16 us_start = 0;
//    u_int16 us_size = 0;
//    u_int16 us_cnt = 0;
//
//    pst_master = &gstGsnsYExt_l;
//    us_size = static_cast<u_int16>(sizeof(u_int16) * 10);  /* Size of one data item: 2byte * 10 data items */
//
//    /* Store the latest one in the internal data structure */
//    us_start = gstPkgTempExt.start_point[4];  /* Location to store one received message    */
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
//    gstPkgTempExt.start_point[4] = us_start;
//
//    /* Update data master size */
//    if (gstPkgTempExt.data_break == VEHICLE_SNS_BREAK) {
//        /* Make the size of all extended data masters */
//        pst_master->us_size = VEHICLE_DSIZE_GSNS_Y_EXT;
//    } else {
//        /* Add the size of one received data item */
//        pst_master->us_size = static_cast<u_int16>(pst_master->us_size + us_size);
//    }
//}

/*******************************************************************************
* MODULE    : VehicleSensGetGsnsYExtl
* ABSTRACT  : Vehicle sensor GSNS_Y GET function
* FUNCTION  : Provide the GSNS_Y data master
* ARGUMENT  : *pst_data : Pointer to the data master acquisition destination
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensGetGsnsYExtl(VEHICLESENS_DATA_MASTER_EXT *pst_data) {
    const VEHICLESENS_DATA_MASTER_EXT *pst_master;
    u_int16 us_size = 0;
    u_int16 us_data_cnt = 0;
    u_int16 us_cnt = 0;
    u_int16 us_loop_cnt = 0;

    /* Store the data master in the specified destination. */
    pst_master      = &gstGsnsYExt_l;
    pst_data->ul_did    = pst_master->ul_did;
    pst_data->us_size    = pst_master->us_size;
    pst_data->uc_rcvflag  = pst_master->uc_rcvflag;

    us_size = static_cast<u_int16>(sizeof(u_int16) * 10); /* Size of one data item: 2byte * 10 data items */

    /* Checking whether the number of stored entries is looped */
    if (gstPkgTempExt.data_break == VEHICLE_SNS_BREAK) {
        us_data_cnt = VEHICLE_DKEEP_MAX;
    } else {
        us_data_cnt = gstPkgTempExt.start_point[4];
    }

    /* Acquire data from the newest data master */
    us_loop_cnt = 0;
    for (us_cnt = 0; us_cnt < us_data_cnt; us_cnt++) {
        if (gstPkgTempExt.data_break == VEHICLE_SNS_BREAK) {
            /* Get information after loop */
            if (gstPkgTempExt.start_point[4] > us_cnt) {
                memcpy(&pst_data->uc_data[us_cnt * us_size],
                    &pst_master->uc_data[(gstPkgTempExt.start_point[4] - us_cnt - 1) * us_size], us_size);
                us_loop_cnt++;
            } else {
                memcpy(&pst_data->uc_data[us_cnt * us_size],
                    &pst_master->uc_data[(us_data_cnt - us_cnt - 1 + us_loop_cnt) * us_size], us_size);
            }
        } else {
            if ((u_int32)(us_data_cnt - us_cnt) <= (u_int32)VEHICLE_DKEEP_MAX) {  // LCOV_EXCL_BR_LINE 6: always true
                memcpy(&pst_data->uc_data[us_cnt * us_size],
                    &pst_master->uc_data[(us_data_cnt - us_cnt - 1) * us_size], us_size);
            }
        }
    }
}
#endif
