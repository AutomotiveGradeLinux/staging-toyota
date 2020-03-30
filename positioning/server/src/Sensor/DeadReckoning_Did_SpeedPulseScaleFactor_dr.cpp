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

/*************************************************************************
@file      DeadReckoning_Did_SpeedPulseScaleFactor_dr.cpp
@detail      DeadReckoning data Master(VEHICLE_DID_DR_SPEED_PULSE_SCALE_FACTOR)
*****************************************************************************/

#include <vehicle_service/positioning_base_library.h>
#include "DeadReckoning_DataMaster.h"

/*************************************************/
/*           Global variable                      */
/*************************************************/
static  DEADRECKONING_DATA_MASTER  gst_speedpulsescalefactor;    // NOLINT(readability/nolint)

/***********************************************************************
@brief      SpeedPulseScaleFactor initialization function
@outline    SpeedPulseScaleFactor initialization process data master
@type      Completion return type
@param[in]    none
@threshold    none
@return      void
@retval      none
@trace
**************************************************************************** */
void DeadReckoningInitSpeedPulseScaleFactorDr(void) {  // LCOV_EXCL_START 8: dead code.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    (void)memset(reinterpret_cast<void *>(&gst_speedpulsescalefactor), 0x00, sizeof(gst_speedpulsescalefactor));
    gst_speedpulsescalefactor.ul_did      = VEHICLE_DID_DR_SPEED_PULSE_SCALE_FACTOR;
    gst_speedpulsescalefactor.us_size      = VEHICLE_DSIZE_SPEED_PULSE_SCALE_FACTOR;
    gst_speedpulsescalefactor.uc_rcv_flag    = DEADRECKONING_RCVFLAG_OFF;
    gst_speedpulsescalefactor.dr_status    = 0U;              /* Not used */
}

/*************************************************************************
@brief      SpeedPulseScaleFactor SET function
@outline    To update the master data SpeedPulseScaleFactor
@type      Completion return type
@param[in]    DEADRECKONING_DATA_MASTER *p_st_data  :  The pointer to incoming message data
@threshold    none
@return      u_int8
@retval      DEADRECKONING_EQ  :   No data changes
@retval      DEADRECKONING_NEQ  :   With data changes
@trace
*****************************************************************************/
u_int8 DeadReckoningSetSpeedPulseScaleFactorDr(const DEADRECKONING_DATA_MASTER *p_st_data) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    u_int8 uc_ret = DEADRECKONING_EQ;
    DEADRECKONING_DATA_MASTER *p_st_master;

    if (p_st_data == NULL) {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "p_st_data == NULL\r\n");
    } else {
        p_st_master = &gst_speedpulsescalefactor;

        /** Compare data master and received data */
        uc_ret = DeadReckoningMemcmp(reinterpret_cast<void *>(&(p_st_master->uc_data)),
            (const void *)(&(p_st_data->uc_data) ), (size_t)(p_st_data->us_size));

        /** Received data is set in the data master. */
        p_st_master->ul_did    = p_st_data->ul_did;
        p_st_master->us_size    = p_st_data->us_size;
        p_st_master->uc_rcv_flag  = DEADRECKONING_RCVFLAG_ON;
        p_st_master->dr_status    = 0U;              /* Not used */

        (void)memcpy(reinterpret_cast<void *>(&(p_st_master->uc_data)),
            (const void *)(&(p_st_data->uc_data)), sizeof(p_st_master->uc_data));
    }

    return (uc_ret);
}

/************************************************************************
@brief      SpeedPulseScaleFactor GET function
@outline    Master Data provides the SpeedPulseScaleFactor
@type      Completion return type
@param[in]    DEADRECKONING_DATA_MASTER *p_st_data  :  Where to get a pointer to the data master
@threshold    none
@return      void
@retval      none
@trace
*****************************************************************************/
void DeadReckoningGetSpeedPulseScaleFactorDr(DEADRECKONING_DATA_MASTER *p_st_data) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    const DEADRECKONING_DATA_MASTER *p_st_master;

    if (p_st_data == NULL) {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "p_st_data == NULL\r\n");
    } else {
        p_st_master = &gst_speedpulsescalefactor;

        /** Store the data master in the specified destination. */
        p_st_data->ul_did    = p_st_master->ul_did;
        p_st_data->us_size    = p_st_master->us_size;
        p_st_data->uc_rcv_flag  = p_st_master->uc_rcv_flag;
        p_st_data->dr_status  = p_st_master->dr_status;        /* Not used(Set DataMaster defaults) */
        (void)memcpy(reinterpret_cast<void *>(&(p_st_data->uc_data)),
            (const void *)(&(p_st_master->uc_data)), (size_t)p_st_master->us_size);
    }
}
// LCOV_EXCL_STOP
