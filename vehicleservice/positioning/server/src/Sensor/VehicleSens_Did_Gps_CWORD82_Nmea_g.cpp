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
 *  File name    :VehicleSens_Did_Gps_CWORD82_Nmea_g.cpp
 *  System name    :_CWORD72_
 *  Subsystem name  :Vehicle sensor process
 *  Program name  :Vehicle sensor data master(VEHICLE_DID_GPS__CWORD82__FULLBINARY)
 *  Module configuration  :VehicleSensInitGps_CWORD82_FullBinaryG()  Vehicle sensor GPS__CWORD82__FULLBINARY initialization function
 *              :VehicleSensSetGps_CWORD82_FullBinaryG()  Vehicle sensor GPS__CWORD82__FULLBINARY SET function
 *              :VehicleSensGetGps_CWORD82_FullBinaryG()  Vehicle sensor GPS__CWORD82__FULLBINARY GET function
 ******************************************************************************/

#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"
#include "gps_hal.h"

/*************************************************/
/*           Global variable                      */
/*************************************************/
static  VEHICLESENS_DATA_MASTER_GPS_FORMAT  gstGps_CWORD82_Nmea_g;    // NOLINT(readability/nolint)

/*******************************************************************************
* MODULE    : VehicleSensInitGps_CWORD82_NmeaG
* ABSTRACT  : Vehicle sensor GPS__CWORD82__NMEA initialization function
* FUNCTION  : GPS__CWORD82__NMEA data master initialization processing
* ARGUMENT  : void
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensInitGps_CWORD82_NmeaG(void) {
    memset(&gstGps_CWORD82_Nmea_g, 0x00, sizeof(VEHICLESENS_DATA_MASTER_GPS_FORMAT));
    gstGps_CWORD82_Nmea_g.ul_did  = VEHICLE_DID_GPS__CWORD82__NMEA;
    /* _CWORD82_-only format with a fixed magic number */
    gstGps_CWORD82_Nmea_g.us_size      = 3                        /* NMEA reception flag + GPS antenna connection information + sensor counter */
                                  + VEHICLE_DSIZE_GPS_NMEA_DRMC
                                  + VEHICLE_DSIZE_GPS_NMEA_GSA
                                  + VEHICLE_DSIZE_GPS_NMEA_GSV_1
                                  + VEHICLE_DSIZE_GPS_NMEA_GSV_2
                                  + VEHICLE_DSIZE_GPS_NMEA_GSV_3
                                  + VEHICLE_DSIZE_GPS_NMEA_PJRDC_GP_3;
}

/*******************************************************************************
* MODULE    : VehicleSensSetGps_CWORD82_NmeaG
* ABSTRACT  : Vehicle sensor GPS_NMEA SET function
* FUNCTION  : Update the GPS__CWORD82___CWORD44__GP4 data master
* ARGUMENT  : *pst_data : Pointer to CAN received message data
* NOTE      :
* RETURN    : VEHICLESENS_EQ   : No data change
*             VEHICLESENS_NEQ  : Data change
*******************************************************************************/
u_int8 VehicleSensSetGps_CWORD82_NmeaG(const SENSOR_MSG_GPSDATA_DAT *pst_data) {
    u_int8 uc_ret;
    VEHICLESENS_DATA_MASTER_GPS_FORMAT *pst_master;

    pst_master = &gstGps_CWORD82_Nmea_g;

    /* Compare data master and received data */
    uc_ret = VehicleSensmemcmp(pst_master->uc_data, pst_data->uc_data, pst_data->us_size);

    /* Received data is set in the data master. */
    pst_master->ul_did        = pst_data->ul_did;
    pst_master->us_size       = pst_data->us_size;
    pst_master->uc_rcvflag    = VEHICLE_RCVFLAG_ON;
    memcpy(pst_master->uc_data, pst_data->uc_data, pst_data->us_size);

    return(uc_ret);
}

/*******************************************************************************
* MODULE    : VehicleSensGetGps_CWORD82_NmeaG
* ABSTRACT  : Vehicle sensor GPS__CWORD82__NMEA GET function
* FUNCTION  : GPS__CWORD82__NMEA Provides a data master
* ARGUMENT  : *pst_data : Pointer to the data master acquisition destination
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensGetGps_CWORD82_NmeaG(SENSOR_MSG_GPSDATA_DAT *pst_data) {
    const VEHICLESENS_DATA_MASTER_GPS_FORMAT *pst_master;

    pst_master = &gstGps_CWORD82_Nmea_g;

    /* Store the data master in the specified destination. */
    pst_data->ul_did    = pst_master->ul_did;
    pst_data->us_size    = pst_master->us_size;
    pst_data->uc_rcv_flag  = pst_master->uc_rcvflag;
    memcpy(pst_data->uc_data, pst_master->uc_data, pst_master->us_size);  /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
}
