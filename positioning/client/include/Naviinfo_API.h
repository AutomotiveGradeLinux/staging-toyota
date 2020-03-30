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

/******************************************************************************
@file       Naviinfo_API.h
@detail     Naviinfo_API external header file<BR>
            Naviinfo_API External Public Header
******************************************************************************/
#ifndef POSITIONING_CLIENT_INCLUDE_NAVIINFO_API_H_
#define POSITIONING_CLIENT_INCLUDE_NAVIINFO_API_H_

/*****************************************************************************
 *                                 Include                                   *
 *****************************************************************************/
#include <vehicle_service/positioning_base_library.h>
#include "Sensor_Common_API.h"
#include "Vehicle_API.h"
#include "Sensor_API.h"
#include <vehicle_service/POS_gps_API.h>

/*****************************************************************************
 *                                   Define                                  *
 *****************************************************************************/
/* Command ID Definition */
#define CID_NAVIINFO_DELIVER                   (0x0205)    /* Navigation information setting CID  */
#define CID_NAVIINFO_SPEED_DELIVER             (0x0206)    /* Vehicle speed setting CID    */
#define CID_NAVIINFO_SETGPSTIME                (0x0207)    /* GPS time delivery CID  */

/* Positioning Level/GPS Information Positioning Status Information Definition */
#define NAVIINFO_DIAG_GPS_FIX_STS_NON          (0)         /* Not fix      */
#define NAVIINFO_DIAG_GPS_FIX_STS_2D           (1)         /* 2D positioning      */
#define NAVIINFO_DIAG_GPS_FIX_STS_3D           (2)         /* 3D positioning      */

/* Reception status definition */
#define NAVIINFO_DIAG_GPS_RCV_STS_NOTUSE       (0)         /* Not used      */
#define NAVIINFO_DIAG_GPS_RCV_STS_SEARCHING    (1)         /* Searching      */
#define NAVIINFO_DIAG_GPS_RCV_STS_TRACHING     (2)         /* Tracking in progress  */
#define NAVIINFO_DIAG_GPS_RCV_STS_NOTUSEFIX    (3)         /* Positioning not used    */
#define NAVIINFO_DIAG_GPS_RCV_STS_USEFIX       (4)         /* Positioning use      */

/* Date and Time Status Definition */
#define NAVIINFO_NAVI_GPS_TD_STS_NOTFIX        (0)
/* Time not calibrated after receiver reset  */
#define NAVIINFO_NAVI_GPS_TD_STS_RTCBACKUP     (1)
/* Time output by RTC backup(Time Calibration Result)*/
#define NAVIINFO_NAVI_GPS_TD_STS_FIX           (2)         /* Time calibration completed    */

/* ++ Porting from PastModel002 enabled APIs */
#define CID_VEHICLEIF_GET_VEHICLE_DATA         0x0102      /* Vehicle sensor information acquisition CID    */
#define CID_VEHICLEIF_COMM_WATCH               0x0103      /* Vehicle Sensor Information Disruption Monitoring CID  */
#define CID_SENSORIF__CWORD82__REQUEST               0x0800      /* Vehicle sensor information setting CID    */

#define VEHICLE_RET_ERROR_OUTOF_MEMORY         (-8)        /* Shared memory allocation failure  */
#define VEHICLE_RET_ERROR_INVALID              (-10)       /* CANID undetermined    */

/* -- Porting from PastModel002 enabled APIs */

/*****************************************************************************
 *                               Typedef                                     *
 *****************************************************************************/

/*****************************************************************************
 *                                 Struct                                    *
 *****************************************************************************/
/*****************************************************************************
 * TAG     : NAVIINFO_DELIVER_MSG
 * ABSTRACT   : Navigation information delivery message structure
 * NOTE     :
 *****************************************************************************/
typedef struct {
    T_APIMSG_MSGBUF_HEADER       hdr;                      /* Message header    */
//    NAVIINFO_ALL                 dat;                      /* Message data section    */
} NAVIINFO_DELIVER_MSG;

/*****************************************************************************
 * TAG     : NAVIINFO_SPEED_DELIVER_MSG
 * ABSTRACT   : Navigation Information (Vehicle Speed) Delivery Message Structure
 * NOTE     :
 *****************************************************************************/
typedef struct {
    T_APIMSG_MSGBUF_HEADER       hdr;                      /* Message header    */
    u_int16                      navi_speed;               /* Vehicle speed information          */
    u_int8                       reserve[2];               /* reserve          */
} NAVIINFO_SPEED_DELIVER_MSG;

/*****************************************************************************
*                           Function prototype                                  *
*****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
//extern int32 VehicleSetVehicleData(PNO pid, VEHICLE_MSG_SEND_DAT *p_data);
#ifdef __cplusplus
}
#endif

#endif  // POSITIONING_CLIENT_INCLUDE_NAVIINFO_API_H_
