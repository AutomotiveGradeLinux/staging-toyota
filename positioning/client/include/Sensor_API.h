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

/************************************************************************
*                            prototype declalation                      *
************************************************************************/
/*[SENSOR_API public API]*/
#ifdef __cplusplus
extern "C" {
#endif
SENSOR_RET_API SensorPkgDeliveryEntry(PNO pno, u_int8 pkg_num, DID *did, u_int8 ctrl_flg, u_int8 delivery_timing);
SENSOR_RET_API SensorPkgDeliveryEntryExt(PNO pno, u_int8 pkg_num, DID *did, u_int8 ctrl_flg, u_int8 delivery_timing);
int32 SensorGetSensorPkgData(PNO pno, u_int8 pkg_num, DID *did, void *dest_data, u_int16 dest_size);
RET_API SensorReqGpsReset(PNO proc_no, u_int8 mode, RID req_id);
SENSOR_RET_API GpsGetGpsInterruptSignal(GPS_INTERRUPT *gps_interrupt);
SENSOR_RET_API SensorGetGyroConnectStatus(u_int8 *gyro_connect_status);
SENSOR_RET_API GpsGetValidEphemerisNumberOnShutdown(u_int8 *valid_ephemeris_num);
SENSOR_RET_API ClockGetLocalTimeOnShutdown(LOCALTIME *local_time);
int32 VehicleGetVehicleData(PNO pno, DID did, void *dest_data, u_int16 dest_size);
#ifdef __cplusplus
}
#endif

#endif  // POSITIONING_CLIENT_INCLUDE_SENSOR_API_H_
