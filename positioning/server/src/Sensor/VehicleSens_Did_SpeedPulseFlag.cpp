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

/***************************************************************************
@file      VehicleSens_Did_Nav_Clock_g.cpp
@detail      Master vehicle sensor data(POSHAL_DID_SPEED_PULSE_FLAG)
*****************************************************************************/

#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"

/*************************************************/
/*           Global variable                      */
/*************************************************/
static  VEHICLESENS_DATA_MASTER  gstSpeedPulseFlag;    // NOLINT(readability/nolint)

/***************************************************************************
@brief      VehicleSensInitSpeedPulseFlag
@outline    SPEED_PULSE_FLAG initialization process data master
@type      Completion return type
@param[in]    none
@threshold    none
@return      void
@retval      none
@trace
*****************************************************************************/
void VehicleSensInitSpeedPulseFlag(void) {
    (void)memset(reinterpret_cast<void *>(&gstSpeedPulseFlag), 0, sizeof(VEHICLESENS_DATA_MASTER));
    gstSpeedPulseFlag.ul_did    = POSHAL_DID_SPEED_PULSE_FLAG;
    gstSpeedPulseFlag.us_size  = VEHICLE_DSIZE_SPEED_PULSE_FLAG;
    gstSpeedPulseFlag.uc_rcvflag  = VEHICLE_RCVFLAG_OFF;
}

/***************************************************************************
@brief      NAV-CLOCK SET vehicle sensor function
@outline    To update the master data NAV-CLOCK.
@type      Completion return type
@param[in]    SENSOR_MSG_GPSDATA_DAT *pst_data  :  The pointer to GPS incoming message data
@threshold    none
@return      u_int8
@retval      VEHICLESENS_EQ  :   No data changes
@retval      VEHICLESENS_NEQ  :   With data changes
@trace
*****************************************************************************/
u_int8 VehicleSensSetSpeedPulseFlag(const LSDRV_LSDATA_G *pst_data) {  // LCOV_EXCL_START 8: dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    u_int8 uc_ret;
    VEHICLESENS_DATA_MASTER *pst_master;

    pst_master = &gstSpeedPulseFlag;

    /** Compare data master and received data */
    uc_ret = VehicleSensmemcmp(pst_master->uc_data, pst_data->uc_data, pst_data->uc_size);

    /** Received data is set in the data master. */
    pst_master->ul_did    = pst_data->ul_did;
    pst_master->us_size    = pst_data->uc_size;
    pst_master->uc_rcvflag  = VEHICLE_RCVFLAG_ON;
    pst_master->uc_snscnt    = pst_data->uc_sns_cnt;
    (void)memcpy(reinterpret_cast<void *>(pst_master->uc_data),
        (const void *)(pst_data->uc_data), (size_t)(pst_data->uc_size));

    return(uc_ret);
}

/***************************************************************************
@brief      Vehicle sensor function NAV-CLOCK GET
@outline    Master Data provides the NAV-CLOCK
@type      Completion return type
@param[in]    SENSOR_MSG_GPSDATA_DAT *pst_data  :  Where to get a pointer to the data master
@threshold    none
@return      void
@retval      none
@trace
*****************************************************************************/
void VehicleSensGetSpeedPulseFlag(VEHICLESENS_DATA_MASTER *pst_data) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    const VEHICLESENS_DATA_MASTER *pst_master;

    pst_master = &gstSpeedPulseFlag;

    /** Store the data master in the specified destination. */
    pst_data->ul_did    = pst_master->ul_did;
    pst_data->us_size    = pst_master->us_size;
    pst_data->uc_rcvflag  = pst_master->uc_rcvflag;
    pst_data->uc_snscnt  = pst_master->uc_snscnt;
    (void)memcpy(reinterpret_cast<void *>(pst_data->uc_data),
        (const void *)(pst_master->uc_data), (size_t)(pst_master->us_size));
}

#if CONFIG_SENSOR_EXT_VALID        /* Initial Sensor Support */
/*******************************************************************************
* MODULE    : VehicleSensGetSpeedPulseFlagFst
* ABSTRACT  : Vehicle sensor GET function
* FUNCTION  : Provide a data master
* ARGUMENT  : *pst_data    : Pointer to the data master acquisition destination
*             uc_get_method  : Acquisition method(Direct Line or CAN)
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensGetSpeedPulseFlagFst(VEHICLESENS_DATA_MASTER_FST *pst_data, u_int8 uc_get_method) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    switch (uc_get_method) {
        case VEHICLESENS_GETMETHOD_CAN:
        {
            /* When acquiring from CAN data */
            break;
        }
        case VEHICLESENS_GETMETHOD_LINE:
        {
            /* To acquire from LineSensor */
            VehicleSensGetSpeedPulseFlagFstl(pst_data);
            break;
        }
        default:
            break;
    }
}
#endif
// LCOV_EXCL_STOP
