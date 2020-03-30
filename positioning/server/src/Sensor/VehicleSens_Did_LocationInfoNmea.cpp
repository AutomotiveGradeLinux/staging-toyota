/*
 * @copyright Copyright (c) 2018-2019 TOYOTA MOTOR CORPORATION.
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

/**************************************************************************//**
@file       VehicleSens_Did_LocationInfoNmea.cpp
@detail     Location Information (NMEA) Management of information data master
******************************************************************************/
#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"

/*************************************************
 *           Global variable                      *
 *************************************************/

/**************************************************************************//**
@brief      VehicleSens_GetLocationInfoNmea<BR>
            Location Information (NMEA) Information Data Master GET Processing
@outline    Location Information (NMEA) Provide an information data master
@param[in]  u_int8                   ucGetMethod : Acquisition method(GPS or Navi)
@param[out] VEHICLESENS_DATA_MASTER_GPS_FORMAT *pstData     : Pointer to the data master acquisition destination
@return     none
@retval     none
*******************************************************************************/
void VehicleSens_GetLocationInfoNmea(VEHICLESENS_DATA_MASTER_GPS_FORMAT *pstData, u_int8 ucGetMethod)
{
    switch(ucGetMethod)
    {
        case VEHICLESENS_GETMETHOD_NAVI:
        {
            VehicleSens_GetLocationInfoNmea_n(pstData);
            break;
        }

        default:
            break;
    }
}
