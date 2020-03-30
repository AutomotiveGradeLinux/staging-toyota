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

#ifndef POSITIONING_CLIENT_INCLUDE_DEAD_RECKONING_API_H_
#define POSITIONING_CLIENT_INCLUDE_DEAD_RECKONING_API_H_
/****************************************************************************
 * File name        :Dead_Reckoning_API.h
 * System name        :PastModel002
 * Subsystem name    :
 ******************************************************************************/

/*****************************************************************************
 *            Include                                                        *
 *****************************************************************************/
#include <vehicle_service/positioning_base_library.h>
#include "Sensor_Common_API.h"
#include <native_service/frameworkunified_types.h>

/*****************************************************************************
 *            Define                                                         *
 *****************************************************************************/
#define NUM_OF_100msData    (10)    /* Number of gyro data items delivered every 100 ms */

/*****************************************************************************
 *            Typedef                                                        *
 *****************************************************************************/
typedef RET_API DR_RET_API;

/*****************************************************************************
 *            Enumeration                                                    *
 *****************************************************************************/
/*******************************************************************************
 TAG        : NAVI_MATCHING_STATUS
 ABSTRACT    : Map matching data(Status)
 NOTE        :
*******************************************************************************/
typedef enum NaviMatchingStatus {
    NAVI_MATCHING_STS_2D_FIX = 0x00,
    NAVI_MATCHING_STS_3D_FIX,
    NAVI_MATCHING_STS_DR_COMBINED,
    NAVI_MATCHING_STS_DR_ONLY,
    NAVI_MATCHING_STS_MAP_2D_GPS_FIX,
    NAVI_MATCHING_STS_MAP_3D_GPS_FIX,
    NAVI_MATCHING_STS_MAP_DR_COMBINED,
    NAVI_MATCHING_STS_MAP_DR_ONLY,
    NAVI_MATCHING_STS_MAP_NO_FIX,
    NAVI_MATCHING_STS_AWAITING_FIRST_FIX,
    NAVI_MATCHING_STS_DATA_INVALID,
    NAVI_MATCHING_STS_NO_FIX,
    NAVI_MATCHING_STS_TIME_ONLY_FIX
} NAVI_MATCHING_STATUS;

/*******************************************************************************
 TAG        : NAVI_MATCHING_DATA_AVAILABLE
 ABSTRACT    : Map matching data(Availability)
 NOTE        :
*******************************************************************************/
typedef enum {
    NAVI_MATCHING_DATA_INVALID = 0x00,
    NAVI_MATCHING_DATA_VALID
} NAVI_MATCHING_DATA_AVAILABLE;

/*****************************************************************************
 *            Struct                                                         *
 *****************************************************************************/
/* Sensor data */
/*******************************************************************************
 TAG      : Struct_PulseRevTbl
 ABSTRACT : Vehicle speed/REV data table structure
 NOTE      : 4byte
*******************************************************************************/
typedef struct {
    u_int8     reverse_flag;                /* 0:Forward march 1:Backward 3:Disabled Reverse signal status */
    u_int8     pulse_flag;                  /* Vehicle speed pulse flag */
    /* 0:Enabled 1:Receive invalid value from CAN 2:Unable to receive at download timing */
    u_int16    pulse_sum_cyclic;            /* Vehicle speed pulse counter value */
} Struct_PulseRevTbl;
/*******************************************************************************
 TAG      : Struct_GyroTbl
 ABSTRACT : Gyro Data Table Structure
 NOTE      : 20byte
*******************************************************************************/
typedef struct {
    u_int16  gyro_data[NUM_OF_100msData];    /* A/D-converted gyro data */
} Struct_GyroTbl;
/*******************************************************************************
 TAG        : Struct_SensData
 ABSTRACT    : Sensor data structure
 NOTE        :
*******************************************************************************/
typedef struct {
    u_int8                sens_cnt_flag;    /* Sensor data transmission counter flag */
    /* 0:Disabled(->Initial Sensor) 1:Enabled(->100 ms periodic transmission) */
    /* Set "0" for initial sensor transmission and "1" for periodic transmission. */
    u_int8                sens_cnt;         /* Sensor data transmission counter value Set "0" at initial transmission of sensor */
    Struct_PulseRevTbl    pulse_rev_tbl;    /* Vehicle speed/REV Data table */
    Struct_GyroTbl        gyro_tbl;         /* Gyro data table */
} Struct_SensData;

/* GPS data */
/*******************************************************************************
 TAG        : Struct_GpsData
 ABSTRACT    : GPS data structure
 NOTE        :
*******************************************************************************/
typedef struct {
    u_int8                sens_cnt;         /* Sensor data transmission counter value             */
    u_int8                sens_cnt_flag;    /* Sensor data transmission counter flag         */
    /* 0:Disabled(Incorrect GPS sensor counter)    */
    /* 1:Enabled                                */
    u_int16               gps_data_size;    /* GPS data storage area size                */
    DID                   did;              /* Data ID                                */
    const void*           gps_data;         /* GPS data storage area                     */
} Struct_GpsData;

/* Estimated Navigation Calculation Result */
/*******************************************************************************
 TAG        : Struct_DR_DATA
 ABSTRACT    : Estimated Navigation Calculation Result
 NOTE        :
*******************************************************************************/
typedef struct {
    int64_t               latitude;         /* Latitude(WGS-84) 10^-7deg        */
    int64_t               longitude;        /* Longitude(WGS-84) 10^-7deg        */
    int64_t               altitude;         /* Altitude 10^-2m                    */
    u_int16               rate;             /* Speeds 10^-2m/sec                */
    u_int16               heading;          /* Orientation North-reference clockwise 10^-2deg    */
    u_int16               reserve;          /* RESERVE                        */
    u_int8                dr_status;        /* DR calculation status                */
    u_int8                positioning_time; /* Sensor Counter                */
} Struct_DR_DATA;

/*******************************************************************************
 TAG        : DR_CALC_INFO
 ABSTRACT    : Estimated navigation information
 NOTE        :
*******************************************************************************/
typedef struct {
    u_int16               gyro_offset;                     /* Correction coefficient:Gyro offset */
    u_int8                gyro_scale_factor;               /* Correction coefficient:Gyro Sensitivity */
    u_int8                gyro_scale_factor_level;         /* Learning Level(Gyro Sensitivity) */
    u_int16               speed_pulse_scale_factor;        /* Correction coefficient:Vehicle speed factor */
    u_int8                speed_pulse_scale_factor_level;  /* Learning Level(Vehicle speed factor) */
    u_int8                reserve;                         /* Reserve(1Byte) */
} DR_CALC_INFO;

/*******************************************************************************
 TAG        : DR_POSITION_INFO
 ABSTRACT    : Location information
 NOTE        :
*******************************************************************************/
typedef struct {
    int64_t                         latitude;
    int64_t                         longitude;
    NAVI_MATCHING_DATA_AVAILABLE    status;
} DR_POSITION_INFO;

/*******************************************************************************
 TAG        : DR_RATE_INFO
 ABSTRACT    : Location information
 NOTE        :
*******************************************************************************/
typedef struct {
    u_int16                         rate;
    u_int8                          reserve[2];
    NAVI_MATCHING_DATA_AVAILABLE    status;
} DR_RATE_INFO;

/*******************************************************************************
 TAG        : DR_ORIENT_INFO
 ABSTRACT    : Location information
 NOTE        :
*******************************************************************************/
typedef struct {
    u_int16                         orient;
    u_int8                          reserve[2];
    NAVI_MATCHING_DATA_AVAILABLE    status;
} DR_ORIENT_INFO;

/*******************************************************************************
 TAG        : NAVI_MATCHING_DATA
 ABSTRACT    : Map matching data
 NOTE        :
*******************************************************************************/
typedef struct {
    DR_POSITION_INFO        position_info;
    DR_RATE_INFO            rate_info;
    DR_ORIENT_INFO          orient_info;
    NAVI_MATCHING_STATUS    status;
} NAVI_MATCHING_DATA;

/************************************************************************
*            Function prototype                                            *
************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

DR_RET_API DeadReckoningLibraryInit(void);
DR_RET_API SetGpsData(Struct_GpsData *st_gps_data);
DR_RET_API CalculateDeadReckoningData(Struct_SensData *st_sensor_data,
                                       Struct_DR_DATA *st_dr_data, DR_CALC_INFO *dr_calc_data);
DR_RET_API SetMatchingData(NAVI_MATCHING_DATA *st_matching_data);

DR_RET_API ClearBackupData(void);                                          /* Clear DR backup data */
/* BOOL Problem Avoidance */
DR_RET_API GetLocationLogSetting(int *log_sw, unsigned char *severity);    /* Location log output ON/OFF acquisition */
DR_RET_API SetLocationLogStatus(int log_sw, unsigned char severity);       /* Location log output ON/OFF setting */

#ifdef __cplusplus
}
#endif

#endif  // POSITIONING_CLIENT_INCLUDE_DEAD_RECKONING_API_H_
