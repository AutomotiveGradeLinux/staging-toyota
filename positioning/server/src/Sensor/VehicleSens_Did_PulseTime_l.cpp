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
 *  VehicleSens_Did_PulseTime_l.cpp
 * @brief
 *  Vehicle sensor data master(POSHAL_DID_PULSE_TIME)
 */

#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"

/*************************************************/
/*           Global variable                      */
/*************************************************/
static VEHICLESENS_DATA_MASTER  gstPulseTime_l;    // NOLINT(readability/nolint)

/**
 * @brief
 *   Vehicle sensor inter-pulse time initialization function
 *
 *   Inter-pulse time data master initialization processing
 */
void VehicleSensInitPulseTimel(void) {
    memset(&gstPulseTime_l, 0x00, sizeof(VEHICLESENS_DATA_MASTER));
//    gstPulseTime_l.ul_did    = POSHAL_DID_PULSE_TIME;
    gstPulseTime_l.us_size    = VEHICLE_DSIZE_PULSE_TIME;
    gstPulseTime_l.uc_rcvflag  = VEHICLE_RCVFLAG_OFF;
}

/**
 * @brief
 *   Vehicle sensor inter-pulse time initialization function
 *
 *   Update the interpulse time data master
 *
 * @param[in]  *pst_data : Pointer to the message data received by the direct line
 *
 * @return  VEHICLESENS_EQ  No data change<br>
 *      VEHICLESENS_NEQ Data change
 */
//u_int8 VehicleSensSetPulseTimelG(const LSDRV_LSDATA_G *pst_data) {
//    u_int8 uc_ret;
//    VEHICLESENS_DATA_MASTER *pst_master;
//
//    pst_master = &gstPulseTime_l;
//
//    /* Compare data master and received data */
//    uc_ret = VehicleSensmemcmp(pst_master->uc_data, pst_data->uc_data, pst_data->uc_size);
//
//    /* Received data is set in the data master. */
//    pst_master->ul_did    = pst_data->ul_did;
//    pst_master->us_size    = pst_data->uc_size;
//    pst_master->uc_rcvflag  = VEHICLE_RCVFLAG_ON;
//    (void)memcpy(reinterpret_cast<void *>(pst_master->uc_data),
//        (const void *)(pst_data->uc_data), (size_t)(pst_data->uc_size));
//
//    return(uc_ret);
//}

/**
 * @brief
 *   Vehicle sensor pulse time GET function
 *
 *   Provide interpulse time data master
 *
 * @param[in]  Pointer to the data master acquisition destination
 */
void VehicleSensGetPulseTimel(VEHICLESENS_DATA_MASTER *pst_data) {
    const VEHICLESENS_DATA_MASTER *pst_master;

    pst_master = &gstPulseTime_l;

    /* Store the data master in the specified destination. */
    pst_data->ul_did    = pst_master->ul_did;
    pst_data->us_size    = pst_master->us_size;
    pst_data->uc_rcvflag  = pst_master->uc_rcvflag;
    memcpy(pst_data->uc_data, pst_master->uc_data, pst_master->us_size);
}
