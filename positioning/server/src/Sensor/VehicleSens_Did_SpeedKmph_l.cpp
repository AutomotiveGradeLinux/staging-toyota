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
 *  File name    :VehicleSens_Did_SpeedKmph_l.cpp
 *  System name    :_CWORD107_
 *  Subsystem name  :Vehicle sensor process
 *  Program name  :Vehicle sensor data master(POSHAL_DID_SPEED_KMPH)
 *  Module configuration  :VehicleSensInitSpeedKmphl()    Vehicle sensor SPEED_KMPH initialization function
 *                  :VehicleSensSetSpeedKmphl()    Vehicle sensor SPEED_KMPH SET function
 *                  :VehicleSensGetSpeedKmphl()    Vehicle Sensor SPEED_KMPH GET Function
 ******************************************************************************/

#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"

/*************************************************/
/*           Global variable                      */
/*************************************************/
static  VEHICLESENS_DATA_MASTER  gstSpeedKmph_l;    // NOLINT(readability/nolint)

/*******************************************************************************
* MODULE    : VehicleSensInitSpeedKmphl
* ABSTRACT  : Vehicle sensor SPEED_KMPH initialization function
* FUNCTION  : SPEED_KMPH data master initialization processing
* ARGUMENT  : void
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensInitSpeedKmphl(void) {
    u_int16  *pus;

    memset(&gstSpeedKmph_l, 0x00, sizeof(VEHICLESENS_DATA_MASTER));
//    gstSpeedKmph_l.ul_did  = POSHAL_DID_SPEED_KMPH;
    gstSpeedKmph_l.us_size  = VEHICLE_DSIZE_SPEED_KMPH;

    pus    = reinterpret_cast<u_int16 *>(gstSpeedKmph_l.uc_data);
    *pus  = VEHICLE_DINIT_SPEED_KMPH;
}

/*******************************************************************************
* MODULE    : VehicleSensSetSpeedKmphl
* ABSTRACT  : Vehicle sensor SPEED_KMPH SET function
* FUNCTION  : Update the SPEED_KMPH data master
* ARGUMENT  : *pst_data : Pointer to the message data received by the direct line
* NOTE      :
* RETURN    : VEHICLESENS_EQ  : No data change
*             VEHICLESENS_NEQ  : Data change
******************************************************************************/
//u_int8 VehicleSensSetSpeedKmphl(const LSDRV_LSDATA *pst_data) {  // LCOV_EXCL_START 8: dead code.
//    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//    u_int8 uc_ret;
//    VEHICLESENS_DATA_MASTER *pst_master;
//
//    pst_master = &gstSpeedKmph_l;
//
//    /* Compare data master and received data */
//    uc_ret = VehicleSensmemcmp(pst_master->uc_data,
//                    pst_data->uc_data, pst_data->uc_size);  /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//
//    /* Received data is set in the data master. */
//    pst_master->ul_did    = pst_data->ul_did;
//    pst_master->us_size    = pst_data->uc_size;  /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//    pst_master->uc_rcvflag  = VEHICLE_RCVFLAG_ON;
//    memset(pst_master->uc_data, 0x00, sizeof(pst_master->uc_data));
//    memcpy(pst_master->uc_data, pst_data->uc_data, pst_data->uc_size);  /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//
//    return(uc_ret);
//}
// LCOV_EXCL_STOP

/*******************************************************************************
* MODULE    : VehicleSensSetSpeedKmphlG
* ABSTRACT  : Vehicle sensor SPEED_KMPH SET function
* FUNCTION  : Update the SPEED_KMPH data master
* ARGUMENT  : *pst_data : Pointer to the message data received by the direct line
* NOTE      :
* RETURN    : VEHICLESENS_EQ  : No data change
*             VEHICLESENS_NEQ  : Data change
******************************************************************************/
//u_int8 VehicleSensSetSpeedKmphlG(const LSDRV_LSDATA_G *pst_data) {
//    u_int8 uc_ret;
//    VEHICLESENS_DATA_MASTER *pst_master;
//
//    static u_int16 pre_speed[2] = {0, 0};
//    u_int16 cur_speed = 0;
//    memcpy(&cur_speed, &pst_data->uc_data[0], sizeof(u_int16));
//
//    BOOL under2 = TRUE;
//    BOOL eq_speed = TRUE;
//
//    pst_master = &gstSpeedKmph_l;
//
//    /*  Transition of 0->1km/h and 1->0km/h requires 3 consecutive matches. Compliance with driving regulations */
//    under2 = ((pre_speed[1] < 2) && (pre_speed[0] < 2) && (cur_speed < 2));
//    eq_speed = ((pre_speed[1] == pre_speed[0]) && (pre_speed[0] == cur_speed));
//
//    if ((under2 == TRUE) && (eq_speed != TRUE) && (pst_master->uc_rcvflag == VEHICLE_RCVFLAG_ON)) {
//        uc_ret = VEHICLESENS_EQ; /* Return without data change */
//
//        /* Received data is set in the data master. */
//        pst_master->ul_did    = pst_data->ul_did;
//        pst_master->us_size    = pst_data->uc_size;  /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//        pst_master->uc_rcvflag  = VEHICLE_RCVFLAG_ON;
//
//    } else {
//        /* Compare data master and received data */
//        uc_ret = VehicleSensmemcmp(pst_master->uc_data,
//                    pst_data->uc_data, pst_data->uc_size);  /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//
//        /* Received data is set in the data master. */
//        pst_master->ul_did    = pst_data->ul_did;
//        pst_master->us_size    = pst_data->uc_size;  /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//        pst_master->uc_rcvflag  = VEHICLE_RCVFLAG_ON;
//        memcpy(pst_master->uc_data, pst_data->uc_data, pst_data->uc_size);  /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//    }
//
//    /* For the next comparison,Update Speed */
//    pre_speed[1] = pre_speed[0];
//    pre_speed[0] = cur_speed;
//
//    return(uc_ret);
//}

/*******************************************************************************
* MODULE    : VehicleSensGetSpeedKmphl
* ABSTRACT  : Vehicle Sensor SPEED_KMPH GET Function
* FUNCTION  : Provide the SPEED_KMPH data master
* ARGUMENT  : *pst_data : Pointer to the data master acquisition destination
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensGetSpeedKmphl(VEHICLESENS_DATA_MASTER *pst_data) {
    const VEHICLESENS_DATA_MASTER *pst_master;

    pst_master = &gstSpeedKmph_l;

    /* Store the data master in the specified destination. */
    pst_data->ul_did    = pst_master->ul_did;
    pst_data->us_size    = pst_master->us_size;
    pst_data->uc_rcvflag  = pst_master->uc_rcvflag;
    memcpy(pst_data->uc_data, pst_master->uc_data, pst_master->us_size);  /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
}
