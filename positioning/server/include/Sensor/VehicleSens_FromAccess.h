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

#ifndef POSITIONING_SERVER_INCLUDE_SENSOR_VEHICLESENS_FROMACCESS_H_
#define POSITIONING_SERVER_INCLUDE_SENSOR_VEHICLESENS_FROMACCESS_H_
/******************************************************************************
 * File name        :VehicleSens_FromAccess.h
 * System name        :PastModel002
 * Subsystem name    :
 ******************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include "Sensor_API.h"
#include "SensorLocation_API.h"

/************************************************************************
*            Macro definitions                                                    *
************************************************************************/
#define NV_FILE_VEHICLESENS                "/fs/tmpfs/VehicleSens_nv_data.bin"
#define NV_FILE_VEHICLESENS_TEMP        "/fs/tmpfs/VehicleSens_nv_data_temp.bin"
#define NV_FILE_VEHICLESENS2            "/fs/tmpfs/VehicleSens_nv_data_2nd.bin"
#define NV_FILE_VEHICLESENS2_TEMP        "/fs/tmpfs/VehicleSens_nv_data_2nd_temp.bin"
#define NV_LOAD_WAIT_TIME_VEHICLESENS    100U
#define NV_UPDATE_CYCLE_LONLAT            100U
#define NV_UPDATE_CYCLE_LOCALTIME        10U

/************************************************************************
*            Typedef definitions                                                 *
************************************************************************/

/************************************************************************
*            Struct definitions                                                  *
************************************************************************/
typedef struct {
    LOCALTIME    localtime;
    LONLAT        lonlat;
    int32        timediff;
    u_int32        update_counter;
    u_int8        reserve[2];
    u_int8        cka;
    u_int8        ckb;
} NV_DATA_VEHICLESENS;

/************************************************************************
*            Function prototype                                              *
************************************************************************/
void VehicleSensFromAccessInitialize(void);
RET_API VehicleSensRegistNvTag(void);
RET_API VehicleSensReadNVLocalTime(LOCALTIME * local_time);
RET_API VehicleSensReadNVLonLat(LONLAT * lonlat);
RET_API VehicleSensReadNVTimeDiff(int32 * time_diff);
void VehicleSensStoreLonlat(LONLAT * plonlat);
RET_API VehicleSensWriteNVLocaltime(LOCALTIME * local_time, int32 * time_diff);

#endif  // POSITIONING_SERVER_INCLUDE_SENSOR_VEHICLESENS_FROMACCESS_H_
