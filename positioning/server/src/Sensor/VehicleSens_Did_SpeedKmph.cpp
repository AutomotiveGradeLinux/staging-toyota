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
 *  File name    :VehicleSens_Did_SpeedKmph.cpp
 *  System name    :_CWORD107_
 *  Subsystem name  :Vehicle sensor process
 *  Program name  :Vehicle sensor data master(POSHAL_DID_SPEED_KMPH)
 *  Module configuration  :VehicleSensGetSpeedKmph()    Vehicle Sensor SPEED_KMPH GET Function
 ******************************************************************************/

#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"

/*************************************************/
/*           Global variable                      */
/*************************************************/

/*******************************************************************************
* MODULE    : VehicleSensGetSpeedKmph
* ABSTRACT  : Vehicle Sensor SPEED_KMPH GET Function
* FUNCTION  : Provide the SPEED_KMPH data master
* ARGUMENT  : *pst_data    : Pointer to the data master acquisition destination
*             uc_get_method  : Acquisition method(DIRECT or CAN or NAVI)
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensGetSpeedKmph(VEHICLESENS_DATA_MASTER *pst_data, u_int8 uc_get_method) {
    switch (uc_get_method) {
        case VEHICLESENS_GETMETHOD_CAN:
        {
            /* When acquiring from CAN data */
            break;
        }
        case VEHICLESENS_GETMETHOD_LINE:
        {
            /* To acquire from LineSensor */
            VehicleSensGetSpeedKmphl(pst_data);
            break;
        }
        default:  /* Ignore->MISRA-C++:2008 Rule 6-3-1, 6-4-1 */
            break;
    }
}
