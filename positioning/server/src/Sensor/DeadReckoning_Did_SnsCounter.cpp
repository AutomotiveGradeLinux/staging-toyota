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
 *  File name    :DeadReckoning_Did_SnsCounter.cpp
 *  System name    :PastModel002
 *  Subsystem name  :Vehicle sensor process
 *  Program name  :Vehicle sensor data master(VEHICLE_DID_SNS_COUNTER)
 *  Module configuration  :DeadReckoningInitSnsCounterDr()    Vehicle sensor SNS_COUNTER initialization function
 *                  :DeadReckoningSetSnsCounterDr()    Vehicle sensor SNS_COUNTER SET function
 *                  :DeadReckoningGetSnsCounterDr()    Vehicle sensor SNS_COUNTER GET function
 ******************************************************************************/

#include <vehicle_service/positioning_base_library.h>
#include "DeadReckoning_DataMaster.h"

/*************************************************/
/*           Global variable                      */
/*************************************************/
static  DEADRECKONING_DATA_MASTER  gst_snscounter_dr;    // NOLINT(readability/nolint)

/*******************************************************************************
* MODULE    : DeadReckoningInitSnsCounterDr
* ABSTRACT  : Vehicle sensor SNS_COUNTER initialization function
* FUNCTION  : SNS_COUNTER data master initialization processing
* ARGUMENT  : void
* NOTE      :
* RETURN    : void
******************************************************************************/
void DeadReckoningInitSnsCounterDr(void) {  // LCOV_EXCL_START 8: dead code.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    memset(&(gst_snscounter_dr), 0x00, sizeof(DEADRECKONING_DATA_MASTER));
    gst_snscounter_dr.ul_did    = VEHICLE_DID_DR_SNS_COUNTER;
    gst_snscounter_dr.us_size    = VEHICLE_DSIZE_DR_SNS_COUNTER;
    gst_snscounter_dr.uc_rcv_flag  = DEADRECKONING_RCVFLAG_OFF;
    gst_snscounter_dr.dr_status  = SENSORLOCATION_DRSTATUS_INVALID;
    gst_snscounter_dr.uc_data[0]  = VEHICLE_DINIT_DR_SNS_COUNTER;
}

/*******************************************************************************
* MODULE    : DeadReckoningSetSnsCounterDr
* ABSTRACT  : Vehicle sensor SNS_COUNTER SET function
* FUNCTION  : Update the SNS_COUNTER data master
* ARGUMENT  : *p_st_data : Pointer to the message data received by the direct line
* NOTE      :
* RETURN    : VEHICLESENS_EQ  : No data change
*             VEHICLESENS_NEQ  : Data change
******************************************************************************/
u_int8 DeadReckoningSetSnsCounterDr(const DEADRECKONING_DATA_MASTER *p_st_data) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    u_int8 uc_ret;
    DEADRECKONING_DATA_MASTER *p_st_master;

    p_st_master = &gst_snscounter_dr;

    /* Compare data master and received data */
    uc_ret = DeadReckoningMemcmp(p_st_master->uc_data, p_st_data->uc_data, p_st_data->us_size);

    /* Received data is set in the data master. */
    p_st_master->ul_did    = p_st_data->ul_did;
    p_st_master->us_size    = p_st_data->us_size;
    p_st_master->uc_rcv_flag  = DEADRECKONING_RCVFLAG_ON;
    p_st_master->dr_status    = p_st_data->dr_status;
    memcpy(p_st_master->uc_data, p_st_data->uc_data, p_st_data->us_size);

    return (uc_ret);
}

/*******************************************************************************
* MODULE    : DeadReckoningGetSnsCounterDr
* ABSTRACT  : Vehicle sensor SNS_COUNTER GET function
* FUNCTION  : Provide the SNS_COUNTER data master
* ARGUMENT  : *p_st_data : Pointer to the data master acquisition destination
* NOTE      :
* RETURN    : void
******************************************************************************/
void DeadReckoningGetSnsCounterDr(DEADRECKONING_DATA_MASTER *p_st_data) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    const DEADRECKONING_DATA_MASTER *p_st_master;

    p_st_master = &gst_snscounter_dr;

    /* Store the data master in the specified destination. */
    p_st_data->ul_did    = p_st_master->ul_did;
    p_st_data->us_size    = p_st_master->us_size;
    p_st_data->uc_rcv_flag  = p_st_master->uc_rcv_flag;
    p_st_data->dr_status  = p_st_master->dr_status;
    memcpy(p_st_data->uc_data, p_st_master->uc_data, p_st_master->us_size);
}
// LCOV_EXCL_STOP
