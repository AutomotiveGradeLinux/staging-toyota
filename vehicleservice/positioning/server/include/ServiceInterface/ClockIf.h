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

/**
 * @file
 *    ClockIf.h
 * @brief
 *    Clock service-to-service interface
 */
#ifndef POSITIONING_SERVER_INCLUDE_SERVICEINTERFACE_CLOCKIF_H_
#define POSITIONING_SERVER_INCLUDE_SERVICEINTERFACE_CLOCKIF_H_

/*---------------------------------------------------------------------------------*
 * Incluce                                                                         *
 *---------------------------------------------------------------------------------*/
#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_framework_if.h>

#include <stub/DTime_Api.h>
#include <vehicle_service/positioning_base_library.h>
#include <vehicle_service/POS_gps_API.h>

/*---------------------------------------------------------------------------------*
 * Definition                                                                      *
 *---------------------------------------------------------------------------------*/

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
EFrameworkunifiedStatus ClockIfNotifyOnClockAvailability(CbFuncPtr fp_on_cmd);
void ClockIfSetAvailability(BOOL b_is_available);
EFrameworkunifiedStatus ClockIfDtimeSetGpsTime(const SENSOR_MSG_GPSTIME *pst_gps_time, BOOL* pb_is_available);
#ifdef __cplusplus
}
#endif

#endif  // POSITIONING_SERVER_INCLUDE_SERVICEINTERFACE_CLOCKIF_H_
