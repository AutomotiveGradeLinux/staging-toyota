/*
 * @copyright Copyright (c) 2019-2020 TOYOTA MOTOR CORPORATION.
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
 *  File name    :VehicleSens_Did_GsnsZ_l.cpp
 *  Subsystem name  :Vehicle sensor process
 *  Program name  :Vehicle sensor data master(POSHAL_DID_GSNS_Z)
 *  Module configuration  :VehicleSensInitGsnsZl()    Vehicle sensor GSNS_Z initialization function
 *                  :VehicleSensSetGsnsZlG()  Vehicle sensor GSNS_Z SET function
 *                  :VehicleSensGetGsnsZl()    Vehicle sensor GSNS_Z GET function
 ******************************************************************************/

#include "VehicleSens_DataMaster.h"

/*************************************************/
/*           Global variable                     */
/*************************************************/
static  VEHICLESENS_DATA_MASTER  gstGsnsZ_l;    // NOLINT(readability/nolint)

/*******************************************************************************
* MODULE    : VehicleSensInitGsnsZl
* ABSTRACT  : Vehicle sensor GSNS_Z initialization function
* FUNCTION  : GSNS_Z data master initialization processing
* ARGUMENT  : void
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensInitGsnsZl(void) {
    memset(&gstGsnsZ_l, 0x00, sizeof(VEHICLESENS_DATA_MASTER));
    gstGsnsZ_l.ul_did     = POSHAL_DID_GSNS_Z;
    gstGsnsZ_l.us_size    = VEHICLE_DSIZE_GSNS_Z;
    gstGsnsZ_l.uc_rcvflag = VEHICLE_RCVFLAG_OFF;
}

/**
 * @brief
 *   Vehicle sensor GSNS_Z SET function
 *
 *   Update the GSNS_Z data master
 *
 * @param[in]  *pst_data : Pointer to the message data received by the direct line
 *
 * @return  VEHICLESENS_EQ  No data change<br>
 *      VEHICLESENS_NEQ Data change
 *
 */
u_int8 VehicleSensSetGsnsZlG(const LSDRV_LSDATA_G *pst_data) {
    u_int8 uc_ret;
    VEHICLESENS_DATA_MASTER *pst_master;

    pst_master = &gstGsnsZ_l;

    /* Compare data master and received data */
    uc_ret = VehicleSensmemcmp(pst_master->uc_data, pst_data->uc_data, pst_data->uc_size);

    /* Received data is set in the data master. */
    pst_master->ul_did     = pst_data->ul_did;
    pst_master->us_size    = pst_data->uc_size;
    pst_master->uc_rcvflag = VEHICLE_RCVFLAG_ON;
    (void)memcpy(reinterpret_cast<void *>(pst_master->uc_data),
        (const void *)(pst_data->uc_data), (size_t)(pst_data->uc_size));

    return(uc_ret);
}

/*******************************************************************************
* MODULE    : VehicleSensGetGsnsZl
* ABSTRACT  : Vehicle sensor GSNS_Z GET function
* FUNCTION  : Provide the GSNS_Z data master
* ARGUMENT  : *pst_data : Pointer to the data master acquisition destination
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensGetGsnsZl(VEHICLESENS_DATA_MASTER *pst_data) {
    const VEHICLESENS_DATA_MASTER *pst_master;

    pst_master = &gstGsnsZ_l;

    /* Store the data master in the specified destination. */
    pst_data->ul_did     = pst_master->ul_did;
    pst_data->us_size    = pst_master->us_size;
    pst_data->uc_rcvflag = pst_master->uc_rcvflag;
    memcpy(pst_data->uc_data, pst_master->uc_data, pst_master->us_size);
}
