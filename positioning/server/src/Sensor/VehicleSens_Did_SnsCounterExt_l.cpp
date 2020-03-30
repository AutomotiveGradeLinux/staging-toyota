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
 *  File name    :VehicleSens_Did_SnsCounterExt_l.cpp
 *  System name    :_CWORD107_
 *  Subsystem name  :Vehicle sensor process
 *  Program name  :Vehicle sensor data master(POSHAL_DID_SNS_COUNTER)
 *  Module configuration  :VehicleSensInitSnsCounterl()    Vehicle sensor SNS_COUNTER initialization function
 *                  :VehicleSensSetSnsCounterl()    Vehicle sensor SNS_COUNTER SET function
 *                  :VehicleSensGetSnsCounterl()    Vehicle sensor SNS_COUNTER GET function
 ******************************************************************************/

#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"

/* Ignore->MISRA-C++:2008 Rule 2-7-2 */
#if CONFIG_SENSOR_EXT_VALID        /* Initial Sensor Response */
/*************************************************/
/*           Global variable                      */
/*************************************************/
static  VEHICLESENS_DATA_MASTER_EXT  gstSnsCounterExt_l;    // NOLINT(readability/nolint)

/*******************************************************************************
* MODULE    : VehicleSensInitSnsCounterExtl
* ABSTRACT  : Vehicle sensor SNS_COUNTER initialization function
* FUNCTION  : SNS_COUNTER data master initialization processing
* ARGUMENT  : void
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensInitSnsCounterExtl(void) {
    u_int16  *pus;

    memset(&gstSnsCounterExt_l, 0x00, sizeof(VEHICLESENS_DATA_MASTER_EXT));
//    gstSnsCounterExt_l.ul_did    = POSHAL_DID_SNS_COUNTER;
    gstSnsCounterExt_l.us_size    = VEHICLE_DSIZE_SNS_COUNTER_EXT_INIT;
    pus    = reinterpret_cast<u_int16 *>(gstSnsCounterExt_l.uc_data);
    memset(reinterpret_cast<void*>(pus), VEHICLE_DINIT_SNS_COUNTER, VEHICLE_DSIZE_SNS_COUNTER_EXT);

    /* Initializing Initial Delivery Internal Information */
    memset(&gstPkgTempExt, 0x00, sizeof(VEHICLESENS_PKG_DELIVERY_TEMP_EXT));
}

/*******************************************************************************
* MODULE    : VehicleSensSetSnsCounterExtl
* ABSTRACT  : Vehicle sensor SNS_COUNTER SET function
* FUNCTION  : Update the SNS_COUNTER data master
* ARGUMENT  : *pst_data : Pointer to the message data received by the direct line
* NOTE      :
* RETURN    : void
******************************************************************************/
//void VehicleSensSetSnsCounterExtl(const LSDRV_LSDATA *pst_data) {  // LCOV_EXCL_START 8: dead code
//    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//    return;
//}
// LCOV_EXCL_STOP

/*******************************************************************************
* MODULE    : VehicleSensSetSnsCounterExtlG
* ABSTRACT  : Vehicle sensor SNS_COUNTER SET function
* FUNCTION  : Update the SNS_COUNTER data master
* ARGUMENT  : *pst_data : Pointer to the message data received by the direct line
* NOTE      :
* RETURN    : void
******************************************************************************/
//void VehicleSensSetSnsCounterExtlG(const LSDRV_LSDATA_G *pst_data) {
//    VEHICLESENS_DATA_MASTER_EXT *pst_master;
//    u_int16 us_start = 0;
//
//    pst_master = &gstSnsCounterExt_l;
//
//    /* Retrieve the location where the received one is stored */
//    us_start = gstPkgTempExt.start_point[0];
//
//    /* Stored in data master(Order of reception)*/
//    if (us_start >= VEHICLE_DKEEP_MAX) {
//        /* Store the latest one at position 0 */
//        us_start = VEHICLE_DATA_POS_00;
//        /* If you are discarding old data,,Set a flag */
//        gstPkgTempExt.data_break = VEHICLE_SNS_BREAK;
//    }
//    pst_master->ul_did      = pst_data->ul_did;
//    pst_master->uc_rcvflag    = VEHICLE_RCVFLAG_ON;
//    pst_master->uc_data[us_start]  = pst_data->uc_data[0];
//
//    /* Update next storage start position and latest data storage position */
//    us_start++;
//    gstPkgTempExt.start_point[0] = us_start;
//
//    /* Update data master size */
//    if (gstPkgTempExt.data_break == VEHICLE_SNS_BREAK) {
//        /* Make the size of all extended data masters */
//        pst_master->us_size = VEHICLE_DSIZE_SNS_COUNTER_EXT;
//    } else {
//        /* Add the size of one received data item */
//        pst_master->us_size = static_cast<u_int16>(pst_master->us_size + sizeof(u_int8));
//    }
//}

/*******************************************************************************
* MODULE    : VehicleSensGetSnsCounterExtl
* ABSTRACT  : Vehicle sensor SNS_COUNTER GET function
* FUNCTION  : Provide the SNS_COUNTER data master
* ARGUMENT  : *pst_data : Pointer to the data master acquisition destination
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensGetSnsCounterExtl(VEHICLESENS_DATA_MASTER_EXT *pst_data) {
    const VEHICLESENS_DATA_MASTER_EXT *pst_master;
    u_int16 us_data_cnt = 0;
    u_int16 us_cnt = 0;
    u_int16 us_loop_cnt = 0;

    /* Store the data master in the specified destination. */
    pst_master      = &gstSnsCounterExt_l;
    pst_data->ul_did    = pst_master->ul_did;
    pst_data->us_size    = pst_master->us_size;
    pst_data->uc_rcvflag  = pst_master->uc_rcvflag;

    /* Checking whether the number of stored entries is looped */
    if (gstPkgTempExt.data_break == VEHICLE_SNS_BREAK) {  /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
        us_data_cnt = VEHICLE_DKEEP_MAX;
    } else {
        us_data_cnt = gstPkgTempExt.start_point[0];
    }

    /* Acquire data from the newest data master */
    for (us_cnt = 0; us_cnt < us_data_cnt; us_cnt++) {
        if (gstPkgTempExt.data_break == VEHICLE_SNS_BREAK) {
            /* Get information after loop */
            if (gstPkgTempExt.start_point[0] > us_cnt) {
                pst_data->uc_data[us_cnt] = pst_master->uc_data[(gstPkgTempExt.start_point[0] - us_cnt - 1)];
                us_loop_cnt++;
            } else {
                pst_data->uc_data[us_cnt] = pst_master->uc_data[(us_data_cnt - us_cnt - 1 + us_loop_cnt)];
            }
        } else {
            if ((u_int32)(us_data_cnt - us_cnt) <= (u_int32)VEHICLE_DKEEP_MAX) {  // LCOV_EXCL_BR_LINE 6: always true
                pst_data->uc_data[us_cnt] = pst_master->uc_data[(us_data_cnt - us_cnt - 1)];
            }
        }
    }
}
#endif
