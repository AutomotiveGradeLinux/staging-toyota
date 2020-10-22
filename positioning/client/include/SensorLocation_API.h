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

/******************************************************************************
@file       SensorLocation_API.h
@detail     SensorLocation_API external header file<BR>
            SensorLocation_API External public header
******************************************************************************/
#ifndef POSITIONING_CLIENT_INCLUDE_SENSORLOCATION_API_H_
#define POSITIONING_CLIENT_INCLUDE_SENSORLOCATION_API_H_

/*****************************************************************************
 *                                 Include                                   *
 *****************************************************************************/

/*****************************************************************************
 *                              Define                                       *
 *****************************************************************************/
/* Command ID */
#define CID_VEHICLE_SENSORLOCATION_LONLAT        (0x0201)
/* Vehicle sensor information notification CID (Latitude and longitude information)*/
#define CID_VEHICLE_SENSORLOCATION_ALTITUDE      (0x0202)
/* Vehicle sensor information notification CID (Altitude Information) */

/* SENSORLOCATION_RET_API definition */
#define SENSORLOCATION_RET_NORMAL                (0)    /* Successful completion  */
#define SENSORLOCATION_RET_ERROR_PARAM           (-1)   /* Parameter error    */
#define SENSORLOCATION_RET_ERROR_BUFFULL         (-2)   /* Number of registered FULL */
#define SENSORLOCATION_RET_ERROR_INNER           (-3)   /* Internal error    */

/* Delivery opportunity definition */
#define SENSORLOCATION_DELIVERY_TIMING_CHANGE    (0)    /* Change    */
#define SENSORLOCATION_DELIVERY_TIMING_UPDATE    (1)    /* UpDate    */

/* Presence information definitions DR */
#define SENSORLOCATION_EXISTDR_NODR              (0)    /* Without DR   */
#define SENSORLOCATION_EXISTDR_DR                (1)    /* There DR    */

/* DR state definition */
#define SENSORLOCATION_DRSTATUS_INVALID          (0)    /* Invalid            */
#define SENSORLOCATION_DRSTATUS_GPS_NODR         (1)    /* Information use GPS, not yet implemented DR   */
#define SENSORLOCATION_DRSTATUS_NOGPS_DR         (2)    /* No information available GPS, DR implementation */
#define SENSORLOCATION_DRSTATUS_GPS_DR           (3)    /* Information use GPS, DR implementation    */

/* LONLAT initial value definition */
#define SENSORLOCATION_LATITUDE_INIT_VALUE       0x7FFFFFFFL    /* initial value       */
#define SENSORLOCATION_LONGITUDE_INIT_VALUE      0x7FFFFFFFL    /* initial value       */

/*****************************************************************************
 *                            Typedef                                        *
 *****************************************************************************/
typedef int32 SENSORLOCATION_RET_API;

/*****************************************************************************
 *                            Struct                                         *
 *****************************************************************************/
/*****************************************************************************
 * TAG   : SENSORLOCATION_LONLATINFO_DAT
 * ABSTRACT  : Latitude and longitude information data structure
 * NOTE   :
 *****************************************************************************/
typedef struct {
    u_int16                                size;           /* Size          */
    u_int8                                 sensor_cnt;     /* Sensor counter value      */
    u_int8                                 is_exist_dr;    /* Presence or absence information DR  */
    u_int8                                 dr_status;      /* DR state         */
    u_int8                                 reserve[3];     /* Reserve         */
    int32                                  longitude;      /* Longitude        */
    int32                                  latitude;       /* Latitude         */
} SENSORLOCATION_MSG_LONLATINFO_DAT;

/*****************************************************************************
 * TAG   : SENSORLOCATION_ALTITUDEINFO_DAT
 * ABSTRACT  : Altitude information data structure
 * NOTE   :
 *****************************************************************************/
typedef struct {
    u_int16                                size;           /* Size          */
    u_int8                                 sensor_cnt;     /* Sensor counter value      */
    u_int8                                 is_exist_dr;    /* Presence or absence information DR  */
    u_int8                                 dr_status;      /* DR state         */
    u_int8                                 reserve[3];     /* Reserve         */
    int32                                  altitude;       /* Altitude(0.01m)       */
} SENSORLOCATION_MSG_ALTITUDEINFO_DAT;

/*****************************************************************************
 * TAG   : LONLAT
 * ABSTRACT  : Latitude and Longitude data structure
 * NOTE   :
 *****************************************************************************/
typedef struct {
    int32                                  latitude;       /* Latitude         */
    int32                                  longitude;      /* Longitude        */
} LONLAT;

/*****************************************************************************
*   Function prototypes               *
*****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
extern SENSORLOCATION_RET_API SensorLocationRegisterListenerLonLat(PNO pno, u_int8 delivery_timing);
extern SENSORLOCATION_RET_API SensorLocationRegisterListenerAltitude(PNO pno, u_int8 delivery_timing);
extern SENSORLOCATION_RET_API SensorLocationGetLonLat(SENSORLOCATION_MSG_LONLATINFO_DAT *dat);
extern SENSORLOCATION_RET_API SensorLocationGetAltitude(SENSORLOCATION_MSG_ALTITUDEINFO_DAT *dat);
extern SENSORLOCATION_RET_API SensorLocationGetLonLatOnShutdown(LONLAT *lonlat);
#ifdef __cplusplus
}
#endif

#endif  // POSITIONING_CLIENT_INCLUDE_SENSORLOCATION_API_H_
