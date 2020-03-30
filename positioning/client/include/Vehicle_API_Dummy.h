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

#ifndef POSITIONING_CLIENT_INCLUDE_VEHICLE_API_DUMMY_H_
#define POSITIONING_CLIENT_INCLUDE_VEHICLE_API_DUMMY_H_
/******************************************************************************
 * File name        : Vehicle_API_Dummy.h
 * System name        : GPF
 * Sub System name    : Vehicle I/F Liblary
 ******************************************************************************/

#include "Sensor_Common_API.h"

/****************************************************************************
*            Definition                                                     *
*****************************************************************************/
/*--------------------------------------------------------------------------*
 *    VECHILE_RET_API Definition                                            *
 *--------------------------------------------------------------------------*/
/* Normal system */
#define VEHICLE_RET_NORMAL                0       /* Successful completion */

/* Abnormal system */
#define VEHICLE_RET_ERROR_PID             (-1)    /* Abnormal thread ID        */
#define VEHICLE_RET_ERROR_DID             (-2)    /* NoneDID                    */
#define VEHICLE_RET_ERROR_DID_DIS         (-3)    /* Non-ID data CANID        */
#define VEHICLE_RET_ERROR_PARAM           (-4)    /* Parameter error            */
#define VEHICLE_RET_ERROR_BUFFULL         (-5)    /* registration number FULL    */
#define VEHICLE_RET_ERROR_CREATE_EVENT    (-6)    /* Failure event generation    */
#define VEHICLE_RET_ERROR_TIMER           (-7)    /* Generation failure timer    */
#define VEHICLE_RET_ERROR_OUTOF_MEMORY    (-8)    /* Shared memory allocation failure    */
#define VEHICLE_RET_ERROR_SIZE            (-9)    /* Size error destination    */
#define VEHICLE_RET_ERROR_INVALID         (-10)   /* CANID undetermined        */
#define VEHICLE_RET_ERROR_MIN             POS_RET_ERROR_MIN

/*----------------------------------------------------------------------*
 *    API-related registration information delivery vehicle sensor      *
 *----------------------------------------------------------------------*/
/* Delivery control */
#define VEHICLE_DELIVERY_REGIST           0x01    /* Shipping register    */

/* Delivery opportunity */
#define VEHICLE_DELIVERY_TIMING_UPDATE    0x01    /* Update                */
#define VEHICLE_DELIVERY_TIMING_CHANGE    0x02    /* Change                */

/*----------------------------------------------------------------------*
 *    Message Definition                                                *
 *----------------------------------------------------------------------*/
/* Command ID */

/* Registration information delivery vehicle sensor CID    */
#define CID_VEHICLEIF_DELIVERY_ENTRY      0x0101

/* Vehicle sensor information notification CID            */
#define CID_VEHICLESENS_VEHICLE_INFO      0x0200

#define CID_VEHICLEIF_GET_DR_DATA         0x0205

/* The message body size data(byte) */
#define VEHICLE_MSGBUF_DSIZE              (SENSOR_MSG_VSINFO_DSIZE + 12)
/* Header size of SENSOR_MSG_VSINFO_DSIZE + SENSOR_MSG_GPSDATA_DAT (12) */

/************************************************************************
*            typedef Definition                                         *
************************************************************************/

/************************************************************************
*            struct Definition                                          *
************************************************************************/

/************************************************************************
* TAG       : VEHICLE_MSG_BUF
* ABSTRACT  : Message buffer
************************************************************************/
typedef struct {
    T_APIMSG_MSGBUF_HEADER                hdr;                           /* Message header    */
    u_int8                                data[VEHICLE_MSGBUF_DSIZE];    /* Message data        */
} VEHICLE_MSG_BUF;

/************************************************************************
* TAG       : VEHICLE_MSG_GET_VEHICLE_DATA
* ABSTRACT  : Vehicle sensor information acquisition message(-> Vehicle sensor)
************************************************************************/
typedef struct {
    DID                                   did;                /* Data ID corresponding to vehicle sensor information */
    PNO                                   pno;                /* Destination PNO                        */
    u_int16                               offset;             /* Offset to shared memory storage area    */
    u_int16                               size;               /* Size of shared memory storage area            */
    u_int8                                reserve[2];
    EventID                               event_id;           /* Event ID                        */
} VEHICLE_MSG_GET_VEHICLE_DATA_DAT;

typedef struct {
    T_APIMSG_MSGBUF_HEADER                hdr;                /* Message header    */
    VEHICLE_MSG_GET_VEHICLE_DATA_DAT      data;               /* Message data    */
} VEHICLE_MSG_GET_VEHICLE_DATA;

/************************************************************************
* TAG       : VEHICLE_MSG_DELIVERY_ENTRY
* ABSTRACT  : Registration message delivery vehicle sensor information(->Vehicle sensor)
************************************************************************/
typedef struct {
    DID                                   did;    /* ID data corresponding to the vehicle sensor information */
    PNO                                   pno;                /* Shipping addressPNO                */
    u_int8                                delivery_timing;    /* Delivery opportunity                */
    u_int8                                ctrl_flg;           /* Delivery control                    */
    EventID                               event_id;           /* Event ID                            */
} VEHICLE_MSG_DELIVERY_ENTRY_DAT;

typedef struct {
    T_APIMSG_MSGBUF_HEADER                hdr;                /* Message header    */
    VEHICLE_MSG_DELIVERY_ENTRY_DAT        data;               /* Message data        */
} VEHICLE_MSG_DELIVERY_ENTRY;

/************************************************************************
*            Function prototypes                                        *
************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
/*[VEHICLE_API Public API]*/

int32 VehicleGetDrData(PNO pno, DID did, void *dest_data, u_int16 dest_size);

#ifdef __cplusplus
}
#endif

#endif  // POSITIONING_CLIENT_INCLUDE_VEHICLE_API_DUMMY_H_
