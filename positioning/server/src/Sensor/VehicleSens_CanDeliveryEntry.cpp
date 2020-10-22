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
 *    File name        :VehicleSensCanDeliveryEntry.cpp
 *    System name        :_CWORD107_
 *    Subsystem name    :Vehicle sensor process
 *    Program name    :CAN data delivery registration
 *    Module configuration    :VehicleSensCanDeliveryEntry()    CAN data delivery registration
 ******************************************************************************/

#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DeliveryCtrl.h"

/*************************************************/
/*           Global variable                      */
/*************************************************/

/*******************************************************************************
* MODULE    : VehicleSensCanDeliveryEntry
* ABSTRACT  : CAN data delivery registration
* FUNCTION  : Register delivery of CAN data required as vehicle sensor middleware
* ARGUMENT  : void
* NOTE      :
* RETURN    : CANIF_RET_NORMAL                :Normal completion
*             CANIF_RET_ERROR_CREATE_EVENT    :Event generation failure
*             CANIF_RET_ERROR_PARAM        :Parameter error
*             CANIF_RET_ERROR_BUFFULL        :FULL of delivery registers
*             CANIF_RET_ERROR_CANIDFULL    :FULL of delivery CAN ID numbers
******************************************************************************/
RET_API    VehicleSensCanDeliveryEntry(void) {
    return RET_NORMAL;
}

