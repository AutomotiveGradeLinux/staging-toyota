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

/*************************************************************************
@file      DeadReckoning_Did_Altitude.cpp
@detail      DeadReckoning data Master(VEHICLE_DID_DR_ALTITUDE)<BR>
        DeadReckoning data master(VEHICLE_DID_DR_ALTITUDE)
*****************************************************************************/

#include <vehicle_service/positioning_base_library.h>
#include "DeadReckoning_DataMaster.h"

/*************************************************/
/*           Global variable                      */
/*************************************************/
static  DEADRECKONING_DATA_MASTER  gst_altitude;    // NOLINT(readability/nolint)

/************************************************************************
@brief      Altitude Dead Reckoning initialization function
@outline    Altitude initialization process data master
@type      Completion return type
@param[in]    none
@threshold    none
@return      void
@retval      none
@trace
**************************************************************************** */
void DeadReckoningInitAltitudeDr(void) {  // LCOV_EXCL_START 8: dead code.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    memset(&(gst_altitude), 0x00, sizeof(DEADRECKONING_DATA_MASTER));
    gst_altitude.ul_did    = VEHICLE_DID_DR_ALTITUDE;
    gst_altitude.us_size    = VEHICLE_DSIZE_ALTITUDE;
    gst_altitude.uc_rcv_flag  = DEADRECKONING_RCVFLAG_OFF;
    gst_altitude.dr_status  = SENSORLOCATION_DRSTATUS_INVALID;
    gst_altitude.uc_data[0]  = VEHICLE_DINIT_ALTITUDE;
}

/***********************************************************************
@brief      Altitude Dead Reckoning SET function
@outline    To update the master data Altitude.
@type      Completion return type
@param[in]    DEADRECKONING_DATA_MASTER *p_st_data  :  The pointer to GPS incoming message data
@threshold    none
@return      u_int8
@retval      DEADRECKONING_EQ  :   No data changes
@retval      DEADRECKONING_NEQ  :   With data changes
@trace
*****************************************************************************/
u_int8 DeadReckoningSetAltitudeDr(const DEADRECKONING_DATA_MASTER *p_st_data) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    u_int8 uc_ret;
    DEADRECKONING_DATA_MASTER *p_st_master;

    p_st_master = &gst_altitude;

    /** Compare data master and received data */
    uc_ret = DeadReckoningMemcmp(p_st_master->uc_data, p_st_data->uc_data, p_st_data->us_size);

    /** Received data is set in the data master. */
    p_st_master->ul_did    = p_st_data->ul_did;
    p_st_master->us_size    = p_st_data->us_size;
    p_st_master->uc_rcv_flag  = DEADRECKONING_RCVFLAG_ON;
    p_st_master->dr_status    = p_st_data->dr_status;

    memcpy(p_st_master->uc_data, p_st_data->uc_data, p_st_data->us_size);

    return (uc_ret);
}

/************************************************************************
@brief      Altitude Dead Reckoning GET function
@outline    Master Data provides the Altitude
@type      Completion return type
@param[in]    DEADRECKONING_DATA_MASTER *p_st_data  :  Where to get a pointer to the data master
@threshold    none
@return      void
@retval      none
@trace
*****************************************************************************/
void DeadReckoningGetAltitudeDr(DEADRECKONING_DATA_MASTER *p_st_data) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    const DEADRECKONING_DATA_MASTER *p_st_master;

    p_st_master = &gst_altitude;

    /** Store the data master in the specified destination. */
    p_st_data->ul_did    = p_st_master->ul_did;
    p_st_data->us_size    = p_st_master->us_size;
    p_st_data->uc_rcv_flag  = p_st_master->uc_rcv_flag;
    p_st_data->dr_status  = p_st_master->dr_status;
    memcpy(p_st_data->uc_data, p_st_master->uc_data, p_st_master->us_size);
}
// LCOV_EXCL_STOP
