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
@file       SensorMotion_API.h
@detail     SensorMotion_API external header file<BR>
            SensorMotion_API External public header
******************************************************************************/
#ifndef POSITIONING_CLIENT_INCLUDE_SENSORMOTION_API_H_
#define POSITIONING_CLIENT_INCLUDE_SENSORMOTION_API_H_

/*****************************************************************************
 *                              Include                                      *
 *****************************************************************************/

/*****************************************************************************
 *                               Define                                      *
 *****************************************************************************/
/* Command ID */
/* Vehicle sensor information notification CID (Rate information) */
#define CID_VEHICLE_SENSORMOTION_SPEED                  (0x0203)
/* Vehicle sensor information notification CID (Orientation information) */
#define CID_VEHICLE_SENSORMOTION_HEADING                (0x0204)

/* Vehicle sensor information notification CID (GyroTrouble information) */
#define CID_VEHICLE_SENSORMOTION_GYROTROUBLE            (0x020A)
/* Vehicle sensor information notification CID (GyroParameter information) */
#define CID_VEHICLE_SENSORMOTION_GYROPARAMETER          (0x020B)
/* Vehicle sensor information notification CID (SpeedParameter information) */
#define CID_VEHICLE_SENSORMOTION_SPEEDPULSEPARAMETER    (0x020C)

/* SENSORMOTION_RET_API definition */
#define SENSORMOTION_RET_NORMAL                         (0)     /* Successful completion  */
#define SENSORMOTION_RET_ERROR_PARAM                    (-1)    /* Parameter error    */
#define SENSORMOTION_RET_ERROR_BUFFULL                  (-2)    /* Number of registered FULL */
#define SENSORMOTION_RET_ERROR_INNER                    (-3)    /* Internal error    */

/* Delivery opportunity definition */
#define SENSORMOTION_DELIVERY_TIMING_CHANGE             (0)     /* Change    */
#define SENSORMOTION_DELIVERY_TIMING_UPDATE             (1)     /* UpDate    */

/* Presence information definitions DR */
#define SENSORMOTION_EXISTDR_NODR                       (0)     /* Without DR   */
#define SENSORMOTION_EXISTDR_DR                         (1)     /* There DR    */

/* DR state definition */
#define SENSORMOTION_DRSTATUS_INVALID                   (0)     /* Invalid            */
#define SENSORMOTION_DRSTATUS_GPS_NODR                  (1)     /* Information use GPS, not yet implemented DR */
#define SENSORMOTION_DRSTATUS_NOGPS_DR                  (2)     /* No information available GPS, DR implementation */
#define SENSORMOTION_DRSTATUS_GPS_DR                    (3)     /* Information use GPS, DR implementation */

/*----------------------------------------------------------------------*
 * General Purpose Definition
 *----------------------------------------------------------------------*/
#define SENSORMOTION_NORMAL                             (0U)
#define SENSORMOTION_ERROR                              (1U)
#define SENSORMOTION_UNFIXED                            (2U)

/*****************************************************************************
 *                                  Typedef                                  *
 *****************************************************************************/
typedef int32 SENSORMOTION_RET_API;

/*****************************************************************************
 *                                  Struct                                   *
 *****************************************************************************/
/*****************************************************************************
 * TAG   : SENSORMOTION_SPEEDINFO_DAT
 * ABSTRACT  : Vehicle speed information data structure
 * NOTE   :
 *****************************************************************************/
typedef struct {
    u_int16                                   size;           /* Size          */
    u_int8                                    sensor_cnt;     /* Sensor counter value      */
    u_int8                                    is_exist_dr;    /* Presence or absence information DR  */
    u_int8                                    dr_status;      /* DR state         */
    u_int8                                    reserve;        /* Reserve         */
    u_int16                                   speed;          /* Speed(0.01m/sec)       */
} SENSORMOTION_MSG_SPEEDINFO_DAT;

/*****************************************************************************
 * TAG   : SENSORMOTION_MSG_HEADINGINFO_DAT
 * ABSTRACT  : Orientation information data structure
 * NOTE   :
 *****************************************************************************/
typedef struct {
    u_int16                                   size;           /* Size          */
    u_int8                                    sensor_cnt;     /* Sensor counter value      */
    u_int8                                    is_exist_dr;    /* Presence or absence information DR  */
    u_int8                                    dr_status;      /* DR state         */
    u_int8                                    reserve;        /* Reserve         */
    u_int16                                   heading;        /* Heading         */
} SENSORMOTION_MSG_HEADINGINFO_DAT;

/*****************************************************************************
 * TAG   : SENSORMOTION_MSG_GYROTROUBLEINFO_DAT
 * ABSTRACT  : GyroTrouble information data structure
 * NOTE   :
 *****************************************************************************/
typedef struct {
    u_int16                                   size;           /* Size          */
    u_int8                                    gyro_trouble;   /* Gyro Trouble Status      */
    u_int8                                    reserve;        /* Reserve         */
} SENSORMOTION_MSG_GYROTROUBLEINFO_DAT;

/*****************************************************************************
 * TAG   : SENSORMOTION_MSG_GYROTROUBLEINFO
 * ABSTRACT  : Delivery of the message GyroTrouble information
 * NOTE   : Message structure(VehicleSens -> User)
 *****************************************************************************/
typedef struct {
    T_APIMSG_MSGBUF_HEADER                    hdr;            /* Message header    */
    SENSORMOTION_MSG_GYROTROUBLEINFO_DAT      data;           /* Delivery data (GyroTrouble) */
} SENSORMOTION_MSG_GYROTROUBLEINFO;

/*****************************************************************************
 * TAG   : SENSORMOTION_MSG_GYROPARAMETERINFO_DAT
 * ABSTRACT  : GyroParameter information data structure
 * NOTE   :
 *****************************************************************************/
typedef struct {
    u_int16                                   size;           /* Size          */
    u_int16                                   gyro_offset;    /* Gyro Offset        */
    u_int8                                    gyro_scale_factor;          /* Gyro Scale Factor */
    u_int8                                    gyro_scale_factor_level;    /* Gyro Scale Factor Level */
    u_int8                                    reserve[2];     /* Reserve         */
} SENSORMOTION_MSG_GYROPARAMETERINFO_DAT;

/*****************************************************************************
 * TAG   : SENSORMOTION_MSG_GYROPARAMETERINFO
 * ABSTRACT  : Delivery of the message GyroParameter information
 * NOTE   : Message structure(VehicleSens -> User)
 *****************************************************************************/
typedef struct {
    T_APIMSG_MSGBUF_HEADER                    hdr;            /* Message header     */
    SENSORMOTION_MSG_GYROPARAMETERINFO_DAT    data;           /* Delivery data (GyroParameter) */
} SENSORMOTION_MSG_GYROPARAMETERINFO;

/*****************************************************************************
 * TAG   : SENSORMOTION_MSG_SPEEDPULSEPARAMETERINFO_DAT
 * ABSTRACT  : SpeedPulseParameter information data structure
 * NOTE   :
 *****************************************************************************/
typedef struct {
    u_int16                                   size;           /* Size          */
    u_int16                                   speed_pulse_scale_factor;          /* Speed Pulse Scale Factor */
    u_int8                                    speed_pulse_scale_factor_level;    /* Speed Pulse Scale Factor Level */
    u_int8                                    reserve[3];     /* Reserve         */
} SENSORMOTION_MSG_SPEEDPULSEPARAMETERINFO_DAT;

/*****************************************************************************
 * TAG   : SENSORMOTION_MSG_SPEEDPULSEPARAMETERINFO
 * ABSTRACT  : Delivery of the message SpeedPulseParameter information
 * NOTE   : Message structure(VehicleSens -> User)
 *****************************************************************************/
typedef struct {
    T_APIMSG_MSGBUF_HEADER                          hdr;      /* Message header      */
    SENSORMOTION_MSG_SPEEDPULSEPARAMETERINFO_DAT    data;     /* Delivery data (SpeedPulseParameter) */
} SENSORMOTION_MSG_SPEEDPULSEPARAMETERINFO;

/*****************************************************************************
 *                              Function prototypes                          *
 *****************************************************************************/

#endif  // POSITIONING_CLIENT_INCLUDE_SENSORMOTION_API_H_
