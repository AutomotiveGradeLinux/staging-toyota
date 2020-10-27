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

/*******************************************************************************
 *  File name    :VehicleSens_Did_SpeedPulse_l.cpp
 *  System name    :_CWORD107_
 *  Subsystem name  :Vehicle sensor process
 *  Program name  :Vehicle sensor data master(POSHAL_DID_SPEED_PULSE)
 *  Module configuration  :VehicleSensInitSpeedPulsel()    Vehicle sensor SPEED_PULSE initialization function
 *                  :VehicleSensSetSpeedPulsel()    Vehicle Sensor SPEED_PULSE SET Function
 *                  :VehicleSensGetSpeedPulsel()    Vehicle Sensor SPEED_PULSE GET Function
 ******************************************************************************/

#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"

/*************************************************/
/*           Global variable                      */
/*************************************************/
static  VEHICLESENS_DATA_MASTER  gstSpeedPulse_l;    // NOLINT(readability/nolint)

/*******************************************************************************
* MODULE    : VehicleSensInitSpeedPulsel
* ABSTRACT  : Vehicle sensor SPEED_PULSE initialization function
* FUNCTION  : SPEED_PULSE data master initialization processing
* ARGUMENT  : void
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensInitSpeedPulsel(void) {
    (void)memset(reinterpret_cast<void *>(&gstSpeedPulse_l), 0, sizeof(VEHICLESENS_DATA_MASTER));
    gstSpeedPulse_l.ul_did    = POSHAL_DID_SPEED_PULSE;
    gstSpeedPulse_l.us_size    = VEHICLE_DSIZE_SPEED_PULSE;
    gstSpeedPulse_l.uc_rcvflag  = VEHICLE_RCVFLAG_OFF;
}

/*******************************************************************************
* MODULE    : VehicleSensSetSpeedPulsel
* ABSTRACT  : Vehicle Sensor SPEED_PULSE SET Function
* FUNCTION  : Update the SPEED_PULSE data master
* ARGUMENT  : *pst_data : Pointer to the message data received by the direct line
* NOTE      :
* RETURN    : VEHICLESENS_EQ  : No data change
*             VEHICLESENS_NEQ  : Data change
******************************************************************************/
u_int8 VehicleSensSetSpeedPulsel(const LSDRV_LSDATA *pst_data) {
    u_int8 uc_ret;
    VEHICLESENS_DATA_MASTER *pst_master;

    pst_master = &gstSpeedPulse_l;

    /* Compare data master and received data */
    uc_ret = VehicleSensmemcmp(pst_master->uc_data, pst_data->uc_data, pst_data->uc_size);

    /* Received data is set in the data master. */
    pst_master->ul_did    = pst_data->ul_did;
    pst_master->us_size    = pst_data->uc_size;
    pst_master->uc_rcvflag  = VEHICLE_RCVFLAG_ON;
    pst_master->uc_snscnt    = pst_data->uc_sns_cnt;
    (void)memcpy(reinterpret_cast<void *>(pst_master->uc_data),
        (const void *)(pst_data->uc_data), (size_t)(pst_data->uc_size));

    return(uc_ret);
}

/*******************************************************************************
* MODULE    : VehicleSensSetSpeedPulselG
* ABSTRACT  : Vehicle Sensor SPEED_PULSE SET Function
* FUNCTION  : Update the SPEED_PULSE data master
* ARGUMENT  : *pst_data : Pointer to the message data received by the direct line
* NOTE      :
* RETURN    : VEHICLESENS_EQ  : No data change
*             VEHICLESENS_NEQ  : Data change
******************************************************************************/
u_int8 VehicleSensSetSpeedPulselG(const LSDRV_LSDATA_G *pst_data) {
    u_int8 uc_ret;
    VEHICLESENS_DATA_MASTER *pst_master;

    pst_master = &gstSpeedPulse_l;

    /* Compare data master and received data */
    uc_ret = VehicleSensmemcmp(pst_master->uc_data, pst_data->uc_data, pst_data->uc_size);

    /* Received data is set in the data master. */
    pst_master->ul_did    = pst_data->ul_did;
    pst_master->us_size    = pst_data->uc_size;
    pst_master->uc_rcvflag  = VEHICLE_RCVFLAG_ON;
    pst_master->uc_snscnt    = pst_data->uc_sns_cnt;
    (void)memcpy(reinterpret_cast<void *>(pst_master->uc_data),
        (const void *)(pst_data->uc_data), (size_t)(pst_data->uc_size));

    return(uc_ret);
}

/*******************************************************************************
* MODULE    : VehicleSensGetSpeedPulsel
* ABSTRACT  : Vehicle Sensor SPEED_PULSE GET Function
* FUNCTION  : Provide the SPEED_PULSE data master
* ARGUMENT  : *pst_data : Pointer to the data master acquisition destination
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensGetSpeedPulsel(VEHICLESENS_DATA_MASTER *pst_data) {
    const VEHICLESENS_DATA_MASTER *pst_master;

    pst_master = &gstSpeedPulse_l;

    /* Store the data master in the specified destination. */
    pst_data->ul_did    = pst_master->ul_did;
    pst_data->us_size    = pst_master->us_size;
    pst_data->uc_rcvflag  = pst_master->uc_rcvflag;
    pst_data->uc_snscnt  = pst_master->uc_snscnt;
    (void)memcpy(reinterpret_cast<void *>(pst_data->uc_data),
        (const void *)(pst_master->uc_data), (size_t)(pst_master->us_size));
}
