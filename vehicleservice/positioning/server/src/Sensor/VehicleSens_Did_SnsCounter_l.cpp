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
 *  File name    :VehicleSens_Did_SnsCounter_l.cpp
 *  System name    :_CWORD107_
 *  Subsystem name  :Vehicle sensor process
 *  Program name  :Vehicle sensor data master(POSHAL_DID_SNS_COUNTER)
 *  Module configuration  :VehicleSensInitSnsCounterl()    Vehicle sensor SNS_COUNTER initialization function
 *                  :VehicleSensSetSnsCounterl()    Vehicle sensor SNS_COUNTER SET function
 *                  :VehicleSensGetSnsCounterl()    Vehicle sensor SNS_COUNTER GET function
 ******************************************************************************/

#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"

/*************************************************/
/*           Global variable                      */
/*************************************************/
static  VEHICLESENS_DATA_MASTER  gstSnsCounter_l;    // NOLINT(readability/nolint)

/*******************************************************************************
* MODULE    : VehicleSensInitSnsCounterl
* ABSTRACT  : Vehicle sensor SNS_COUNTER initialization function
* FUNCTION  : SNS_COUNTER data master initialization processing
* ARGUMENT  : void
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensInitSnsCounterl(void) {
    memset(&gstSnsCounter_l, 0x00, sizeof(VEHICLESENS_DATA_MASTER));
    gstSnsCounter_l.ul_did    = POSHAL_DID_SNS_COUNTER;
    gstSnsCounter_l.us_size    = VEHICLE_DSIZE_SNS_COUNTER;
    gstSnsCounter_l.uc_data[0]  = VEHICLE_DINIT_SNS_COUNTER;
}

/*******************************************************************************
* MODULE    : VehicleSensSetSnsCounterl
* ABSTRACT  : Vehicle sensor SNS_COUNTER SET function
* FUNCTION  : Update the SNS_COUNTER data master
* ARGUMENT  : *pst_data : Pointer to the message data received by the direct line
* NOTE      :
* RETURN    : VEHICLESENS_EQ  : No data change
*             VEHICLESENS_NEQ  : Data change
******************************************************************************/
u_int8 VehicleSensSetSnsCounterl(const LSDRV_LSDATA *pst_data) {  // LCOV_EXCL_START 8: dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    u_int8 uc_ret;
    VEHICLESENS_DATA_MASTER *pst_master;

    pst_master = &gstSnsCounter_l;

    /* Compare data master and received data */
    uc_ret = VehicleSensmemcmp(pst_master->uc_data, pst_data->uc_data, pst_data->uc_size);

    /* Received data is set in the data master. */
    pst_master->ul_did    = pst_data->ul_did;
    pst_master->us_size    = pst_data->uc_size;
    pst_master->uc_rcvflag  = VEHICLE_RCVFLAG_ON;
    memcpy(pst_master->uc_data, pst_data->uc_data, pst_data->uc_size);

    return(uc_ret);
}
// LCOV_EXCL_STOP

/*******************************************************************************
* MODULE    : VehicleSensSetSnsCounterlG
* ABSTRACT  : Vehicle sensor SNS_COUNTER SET function
* FUNCTION  : Update the SNS_COUNTER data master
* ARGUMENT  : *pst_data : Pointer to the message data received by the direct line
* NOTE      :
* RETURN    : VEHICLESENS_EQ  : No data change
*             VEHICLESENS_NEQ  : Data change
******************************************************************************/
u_int8 VehicleSensSetSnsCounterlG(const LSDRV_LSDATA_G *pst_data) {
    u_int8 uc_ret;
    VEHICLESENS_DATA_MASTER *pst_master;

    pst_master = &gstSnsCounter_l;

    /* Compare data master and received data */
    uc_ret = VehicleSensmemcmp(pst_master->uc_data, pst_data->uc_data, pst_data->uc_size);

    /* Received data is set in the data master. */
    pst_master->ul_did    = pst_data->ul_did;
    pst_master->us_size    = pst_data->uc_size;
    pst_master->uc_rcvflag  = VEHICLE_RCVFLAG_ON;
    memcpy(pst_master->uc_data, pst_data->uc_data, pst_data->uc_size);

    return(uc_ret);
}

/*******************************************************************************
* MODULE    : VehicleSensGetSnsCounterl
* ABSTRACT  : Vehicle sensor SNS_COUNTER GET function
* FUNCTION  : Provide the SNS_COUNTER data master
* ARGUMENT  : *pst_data : Pointer to the data master acquisition destination
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensGetSnsCounterl(VEHICLESENS_DATA_MASTER *pst_data) {
    const VEHICLESENS_DATA_MASTER *pst_master;

    pst_master = &gstSnsCounter_l;

    /* Store the data master in the specified destination. */
    pst_data->ul_did    = pst_master->ul_did;
    pst_data->us_size    = pst_master->us_size;
    pst_data->uc_rcvflag  = pst_master->uc_rcvflag;
    memcpy(pst_data->uc_data, pst_master->uc_data, pst_master->us_size);
}
