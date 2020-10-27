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

#ifndef POSITIONING_CLIENT_INCLUDE_SENSOR_API_H_
#define POSITIONING_CLIENT_INCLUDE_SENSOR_API_H_
/******************************************************************************
 * file name    :Sensor_API.h
 * system name    :GPF
 * sub system name  :sensor interface library
 ******************************************************************************/
#include <vehicle_service/positioning_base_library.h>
#include "Sensor_Common_API.h"
#include <vehicle_service/POS_sensor_API.h>
#include "POS_sensor_private.h"

/*!
    @brief   GPS Rollover Based Week Number Structure
*/
typedef struct {
    uint16_t                          wkn;              /**< GPS rollover base week number  */
} SENSOR_WKNROLLOVER;

#endif  // POSITIONING_CLIENT_INCLUDE_SENSOR_API_H_
