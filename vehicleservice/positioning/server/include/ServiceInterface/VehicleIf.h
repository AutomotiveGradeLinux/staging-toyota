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

#ifndef POSITIONING_SERVER_INCLUDE_SERVICEINTERFACE_VEHICLEIF_H_
#define POSITIONING_SERVER_INCLUDE_SERVICEINTERFACE_VEHICLEIF_H_

/*---------------------------------------------------------------------------------*
 * Incluce                                                                         *
 *---------------------------------------------------------------------------------*/
#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_framework_if.h>

#include <vehicle_service/positioning_base_library.h>
#include <stub/vehicle_notifications.h>
#include <stub/Vehicle_Sensor_Common_API.h>

/*---------------------------------------------------------------------------------*
 * Definition                                                                      *
 *---------------------------------------------------------------------------------*/
/* Transmission Type */
#define VEHICLEIF_TRANSMISSION_TYPE_MT         0
#define VEHICLEIF_TRANSMISSION_TYPE_AT         1
#define VEHICLEIF_TRANSMISSION_TYPE_UNKNOWN    2
#define VEHICLEIF_TRANSMISSION_TYPE_NUM        3 /* Number of Transmission Types */

/* Shift Position (CAUTION: These values depend on Vehicle I/F) */
#define VEHICLEIF_SHIFT_POSITION_U             0
#define VEHICLEIF_SHIFT_POSITION_R             1
#define VEHICLEIF_SHIFT_POSITION_P             2
#define VEHICLEIF_SHIFT_POSITION_N             4
#define VEHICLEIF_SHIFT_POSITION_D             8
#define VEHICLEIF_SHIFT_POSITION_NUM           5 /* Number of Shift Position Types */

/* Parking Brake (CAUTION: These values depend on Vehicle I/F) */
#define VEHICLEIF_PKB_OFF             0
#define VEHICLEIF_PKB_ON              1
#define VEHICLEIF_PKB_UNKNOWN         2
#define VEHICLEIF_PKB_NUM             3

/*---------------------------------------------------------------------------------*
 * Typedef declaration                                                             *
 *---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*
 * Struct declaration                                                              *
 *---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*
 * Prototype Declaration                                                           *
 *---------------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
void VehicleIf_SetAvailability(BOOL bIsAvailable);

//EFrameworkunifiedStatus VehicleIf_GetTypeOfTransmission(uint8_t* pType, BOOL* pbIsAvailable);
EFrameworkunifiedStatus VehicleIf_GetTypeOfTransmission(uint8_t* pType, uint8_t* pPkb, BOOL* pbIsAvailable);
EFrameworkunifiedStatus VehicleIf_GetShiftPosition(uint8_t* pShift, BOOL* pbIsAvailable);
EFrameworkunifiedStatus VehicleIfAttachCallbacksToDispatcher(    // NOLINT(readability/nolint)
                                                _FrameworkunifiedProtocolCallbackHandler const* p_msg_handler,
                                                unsigned int ui_handlercount);
EFrameworkunifiedStatus VehicleIfDetachCallbacksFromDispatcher(const PUI_32 pui_cmd_array, UI_32 ui_command_count);
EFrameworkunifiedStatus VehicleIfNotifyOnVehicleAvailability(CbFuncPtr fp_on_cmd);
EFrameworkunifiedStatus VehicleIfDeliveryEntry(uint32_t ul_did);
#ifdef __cplusplus
}
#endif

#endif  // POSITIONING_SERVER_INCLUDE_SERVICEINTERFACE_VEHICLEIF_H_
