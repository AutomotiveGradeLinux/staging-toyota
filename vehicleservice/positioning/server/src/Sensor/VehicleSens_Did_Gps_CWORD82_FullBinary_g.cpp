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
 *  File name    :VehicleSens_Did_Gps_CWORD82_FullBinary_g.cpp
 *  System name    :_CWORD72_
 *  Subsystem name  :Vehicle sensor process
 *  Program name  :Vehicle sensor data master(VEHICLE_DID_GPS__CWORD82__FULLBINARY)
 *  Module configuration  :VehicleSensInitGps_CWORD82_FullBinaryG()  Vehicle sensor GPS__CWORD82__FULLBINARY initialization function
 *          :VehicleSensSetGps_CWORD82_FullBinaryG()  Vehicle sensor GPS__CWORD82__FULLBINARY SET function
 *          :VehicleSensGetGps_CWORD82_FullBinaryG()  Vehicle sensor GPS__CWORD82__FULLBINARY GET function
 ******************************************************************************/

#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"
#include "VehicleSens_Common.h"
#include "gps_hal.h"

/*************************************************/
/*           Global variable                      */
/*************************************************/
static  VEHICLESENS_DATA_MASTER_GPS_FORMAT  gstGps_CWORD82_FullBinary_g;    // NOLINT(readability/nolint)

/*******************************************************************************
* MODULE    : VehicleSensInitGps_CWORD82_FullBinaryG
* ABSTRACT  : Vehicle sensor GPS_VERSION initialization function
* FUNCTION  : GPS__CWORD82__FULLBINARY data master initialization processing
* ARGUMENT  : void
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensInitGps_CWORD82_FullBinaryG(void) {
    memset(&gstGps_CWORD82_FullBinary_g, 0x00, sizeof(VEHICLESENS_DATA_MASTER_GPS_FORMAT));
    gstGps_CWORD82_FullBinary_g.ul_did    = POSHAL_DID_GPS__CWORD82__FULLBINARY;
    /* _CWORD82_-only format with a fixed magic number */
    /* GPS antenna connection information(1byte) + Sensor Counter(1byte) + 191 */
    gstGps_CWORD82_FullBinary_g.us_size  = (VEHICLE_DSIZE_GPS_ANTENNA + VEHICLE_DSIZE_SNS_COUNTER + GPS_CMD_FULLBIN_SZ);
}

/*******************************************************************************
* MODULE    : VehicleSensSetGps_CWORD82_FullBinaryG
* ABSTRACT  : Vehicle sensor GPS_VERSION SET function
* FUNCTION  : Update the GPS__CWORD82__FULLBINARY data master
* ARGUMENT  : *pst_data : Pointer to CAN received message data
* NOTE      :
* RETURN    : VEHICLESENS_EQ  : No data change
*             VEHICLESENS_NEQ  : Data change
******************************************************************************/
u_int8 VehicleSensSetGps_CWORD82_FullBinaryG(const SENSOR_MSG_GPSDATA_DAT *pst_data) {
    u_int8 uc_ret;
    VEHICLESENS_DATA_MASTER_GPS_FORMAT *pst_master;
    VehicleSensSetGpsVersion(pst_data); /* Pass the _CWORD82_ binary */

    pst_master = &gstGps_CWORD82_FullBinary_g;

    /* Compare data master and received data */
    uc_ret = VehicleSensmemcmp(pst_master->uc_data, pst_data->uc_data, pst_data->us_size);

    /* Received data is set in the data master. */
    pst_master->ul_did    = pst_data->ul_did;
    pst_master->us_size    = pst_data->us_size;
    pst_master->uc_rcvflag  = VEHICLE_RCVFLAG_ON;
    memcpy(pst_master->uc_data, pst_data->uc_data, pst_data->us_size);

    return(uc_ret);
}

/*******************************************************************************
* MODULE    : VehicleSensGetGps_CWORD82_FullBinaryG
* ABSTRACT  : Vehicle sensor GPS_VERSION GET function
* FUNCTION  : Provide the GPS__CWORD82__FULLBINARY data master
* ARGUMENT  : *pst_data : Pointer to the data master acquisition destination
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensGetGps_CWORD82_FullBinaryG(SENSOR_MSG_GPSDATA_DAT *pst_data) {
    const VEHICLESENS_DATA_MASTER_GPS_FORMAT *pst_master;

    pst_master = &gstGps_CWORD82_FullBinary_g;

    /* Store the data master in the specified destination. */
    pst_data->ul_did    = pst_master->ul_did;
    pst_data->us_size    = pst_master->us_size;
    pst_data->uc_rcv_flag  = pst_master->uc_rcvflag;
    memcpy(pst_data->uc_data, pst_master->uc_data, pst_master->us_size);  /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
}
