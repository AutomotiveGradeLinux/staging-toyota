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
 *  File name    :VehicleSens_Did_GsnsX_l.cpp
 *  System name    :_CWORD107_
 *  Subsystem name  :Vehicle sensor process
 *  Program name  :Vehicle sensor data master(POSHAL_DID_GSNS_X)
 *  Module configuration  :VehicleSensInitGsnsXl()    Vehicle sensor GSNS_X initialization function
 *                  :VehicleSensSetGsnsXlG()  Vehicle sensor GSNS_X SET function
 *                  :VehicleSensGetGsnsXl()    Vehicle sensor GSNS_X GET function
 ******************************************************************************/

#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"

/*************************************************/
/*           Global variable                      */
/*************************************************/
static  VEHICLESENS_DATA_MASTER  gstGsnsX_l;    // NOLINT(readability/nolint)

/*******************************************************************************
* MODULE    : VehicleSensInitGsnsXl
* ABSTRACT  : Vehicle sensor GSNS_X initialization function
* FUNCTION  : GSNS_X data master initialization processing
* ARGUMENT  : void
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensInitGsnsXl(void) {
    memset(&gstGsnsX_l, 0x00, sizeof(VEHICLESENS_DATA_MASTER));
//    gstGsnsX_l.ul_did    = POSHAL_DID_GSNS_X;
    gstGsnsX_l.us_size    = VEHICLE_DSIZE_GSNS_X;
    gstGsnsX_l.uc_rcvflag  = VEHICLE_RCVFLAG_OFF;
}

/**
 * @brief
 *   Vehicle sensor GSNS_X SET function
 *
 *   Update the GSNS_X data master
 *
 * @param[in]  *pst_data : Pointer to the message data received by the direct line
 *
 * @return  VEHICLESENS_EQ  No data change<br>
 *      VEHICLESENS_NEQ Data change
 */
//u_int8 VehicleSensSetGsnsXlG(const LSDRV_LSDATA_G *pst_data) {
//    u_int8 uc_ret;
//    VEHICLESENS_DATA_MASTER *pst_master;
//
//    pst_master = &gstGsnsX_l;
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

/*******************************************************************************
* MODULE    : VehicleSensGetGsnsXl
* ABSTRACT  : Vehicle sensor GSNS_X GET function
* FUNCTION  : Provide the GSNS_X data master
* ARGUMENT  : *pst_data : Pointer to the data master acquisition destination
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensGetGsnsXl(VEHICLESENS_DATA_MASTER *pst_data) {
    const VEHICLESENS_DATA_MASTER *pst_master;

    pst_master = &gstGsnsX_l;

    /* Store the data master in the specified destination. */
    pst_data->ul_did    = pst_master->ul_did;
    pst_data->us_size    = pst_master->us_size;
    pst_data->uc_rcvflag  = pst_master->uc_rcvflag;
    memcpy(pst_data->uc_data, pst_master->uc_data, pst_master->us_size);  /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
}
