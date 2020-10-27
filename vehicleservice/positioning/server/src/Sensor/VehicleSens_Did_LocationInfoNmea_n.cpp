/*
 * @copyright Copyright (c) 2018-2020 TOYOTA MOTOR CORPORATION.
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
 *  VehicleSens_Did_LocationInfoNmea_n.cpp
 * @brief
 */
/*---------------------------------------------------------------------------------*
 * Include Files                                                                   *
 *---------------------------------------------------------------------------------*/
#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"
#include "SensorLocation_API.h"

/*---------------------------------------------------------------------------------*
 * Global Value                                                                    *
 *---------------------------------------------------------------------------------*/
static	VEHICLESENS_DATA_MASTER_GPS_FORMAT	gstLocationInfoNmea_n;

/**
 * @brief
 *  Location Information (NMEA) Initialization of information data master(NAVI information)
 *
 *  Location Information (NMEA) Initialize the information data master
 *
 * @param[in]   none
 * @param[out]  none
 * @return      none
 */
void VehicleSens_InitLocationInfoNmea_n(void)
{
    POS_LOCATIONINFO_NMEA	stLocInfoNmea;

    _pb_memset(&gstLocationInfoNmea_n, 0x00, sizeof(VEHICLESENS_DATA_MASTER_GPS_FORMAT));

    /* Set Data ID */
    gstLocationInfoNmea_n.ul_did	= VEHICLE_DID_LOCATIONINFO_NMEA_NAVI;

    /* Set Data Size */
    gstLocationInfoNmea_n.us_size	= sizeof(POS_LOCATIONINFO_NMEA);

    /** Set Data itself */
    _pb_memset(&stLocInfoNmea, 0x00, sizeof(stLocInfoNmea));
    _pb_memcpy(&gstLocationInfoNmea_n.uc_data[0], &stLocInfoNmea, sizeof(stLocInfoNmea));

    return;
}

/**
 * @brief
 *  Location Information (NMEA) Information data master SET process(NAVI information)
 *
 *  Location Information (NMEA) Update the information data master
 *
 * @param[in]  VEHICLESENS_DATA_MASTER_GPS_FORMAT *pstData     : Pointer to the data master acquisition destination
 * @param[out] none
 * @return     u_int8
 * @retval     VEHICLESENS_EQ  : No data change
 * @retval     VEHICLESENS_NEQ : Data change
 */
u_int8 VehicleSens_SetLocationInfoNmea_n( const POS_LOCATIONINFO_NMEA *pstLocInfoNmea )
{
    u_int8						ucRet;
    VEHICLESENS_DATA_MASTER_GPS_FORMAT		*pstMaster;

    pstMaster = &gstLocationInfoNmea_n;

    /* Compare Received Data with Master Data */
    ucRet = VehicleSensmemcmp( pstMaster->uc_data, pstLocInfoNmea, sizeof(POS_LOCATIONINFO_NMEA) );

    /* Set Received Data as Master Data */
    pstMaster->ul_did		= VEHICLE_DID_LOCATIONINFO_NMEA_NAVI;
    pstMaster->us_size		= sizeof(POS_LOCATIONINFO_NMEA);
    pstMaster->uc_rcvflag	= VEHICLE_RCVFLAG_ON;
    _pb_memcpy(pstMaster->uc_data, pstLocInfoNmea, sizeof(POS_LOCATIONINFO_NMEA));

    return ucRet;
}

/**
 * @brief
 *  Location Information (NMEA) Information Data Master GET Processing(NAVI information)
 *
 *  Location Information (NMEA) Provide an information data master
 *
 * @param[in]  none
 * @param[out] VEHICLESENS_DATA_MASTER_GPS_FORMAT *pstData : Pointer to the data master acquisition destination
 * @return     none
 * @retval     none
 */
void VehicleSens_GetLocationInfoNmea_n(VEHICLESENS_DATA_MASTER_GPS_FORMAT *pstData)
{
    const VEHICLESENS_DATA_MASTER_GPS_FORMAT *pstMaster;

    pstMaster = &gstLocationInfoNmea_n;

    /* Set Master Data to Indicated Region */
    pstData->ul_did		= pstMaster->ul_did;
    pstData->us_size	= pstMaster->us_size;
    pstData->uc_rcvflag	= pstMaster->uc_rcvflag;
    _pb_memcpy(pstData->uc_data, pstMaster->uc_data, pstMaster->us_size);

    return;
}
