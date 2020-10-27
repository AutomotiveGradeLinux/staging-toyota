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

#ifndef POSITIONING_CLIENT_INCLUDE_VEHICLE_API_H_
#define POSITIONING_CLIENT_INCLUDE_VEHICLE_API_H_
/******************************************************************************
 * File name        :Vehicle_API.h
 * System name        :GPF
 * Sub System name    :Vehicle I/F Liblary
 ******************************************************************************/
#include <vehicle_service/positioning_base_library.h>
#include <vehicle_service/POS_define.h>
#include "Sensor_Common_API.h"

/****************************************************************************
*            Definition                                                     *
*****************************************************************************/
/*--------------------------------------------------------------------------*
 *    VECHILE_RET_API Definition                                            *
 *--------------------------------------------------------------------------*/
/* Normal system */
#define VEHICLE_RET_NORMAL                       0       /* Successful completion */

/* Abnormal system */
#define VEHICLE_RET_ERROR_DID                    (-2)    /* NoneDID                    */
#define VEHICLE_RET_ERROR_PARAM                  (-4)    /* Parameter error            */
#define VEHICLE_RET_ERROR_BUFFULL                (-5)    /* registration number FULL    */
#define VEHICLE_RET_ERROR_CREATE_EVENT           (-6)    /* Failure event generation    */
#define VEHICLE_RET_ERROR_MIN                    POS_RET_ERROR_MIN

/*----------------------------------------------------------------------*
 *    API-related registration information delivery vehicle sensor        *
 *----------------------------------------------------------------------*/
/* Delivery control */
#define VEHICLE_DELIVERY_REGIST                  0x01    /* Shipping register    */

/* Delivery opportunity */
#define VEHICLE_DELIVERY_TIMING_UPDATE           0x01    /* Update                */
#define VEHICLE_DELIVERY_TIMING_CHANGE           0x02    /* Change                */

/*----------------------------------------------------------------------*
 *    Message Definition                                                    *
 *----------------------------------------------------------------------*/
/* Command ID */

/* Registration information delivery vehicle sensor CID    */
#define CID_VEHICLEIF_DELIVERY_ENTRY             0x0101

/* Vehicle sensor information notification CID            */
#define CID_VEHICLESENS_VEHICLE_INFO             0x0200

/* GPS time notification CID                             */
#define CID_VEHICLESENS_VEHICLE_INFO_GPS_TIME    0x0201

/************************************************************************
*            typedef Definition                                         *
************************************************************************/
typedef RET_API VEHICLE_RET_API;                         /* API return value    */

/************************************************************************
*            struct Definition                                          *
************************************************************************/

/************************************************************************
* TAG       : VEHICLE_MSG_VSINFO_DAT
* ABSTRACT  : message delivery vehicle sensor information
************************************************************************/
typedef struct {
    DID                       did;
    u_int16                   size;
    u_int8                    rcv_flag;
    u_int8                    sensor_cnt;
    u_int8                    data[256];
} VEHICLE_MSG_VSINFO_DAT;

/* ++ GPS _CWORD82_ support */

/************************************************************************
* TAG       : VEHICLE_MSG_SEND_DAT
* ABSTRACT  : Vehicle sensor information setting message(-> Vehicle sensor)
************************************************************************/
typedef struct {
    T_APIMSG_MSGBUF_HEADER    hdr;          /* Message header */
    VEHICLE_MSG_SEND_DAT      data;         /* Message data */
} VEHICLE_MSG_SEND;

/* -- GPS _CWORD82_ support */

/************************************************************************
*            Function prototypes                                        *
************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
/*[VEHICLE_API Public API]*/
VEHICLE_RET_API VehicleInitialize(u_int32(*sighand)(void)); /* QAC 3001 */

#ifdef __cplusplus
}
#endif

#endif  // POSITIONING_CLIENT_INCLUDE_VEHICLE_API_H_
