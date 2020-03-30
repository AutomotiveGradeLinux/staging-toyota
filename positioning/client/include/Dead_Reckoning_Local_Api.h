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

#ifndef POSITIONING_CLIENT_INCLUDE_DEAD_RECKONING_LOCAL_API_H_
#define POSITIONING_CLIENT_INCLUDE_DEAD_RECKONING_LOCAL_API_H_
/****************************************************************************
 * File name        :Dead_Reckoning_Local_Api.h
 * System name        :PastModel002
 * Subsystem name    :
 ******************************************************************************/

#include <vehicle_service/positioning_base_library.h>
#include "Sensor_Common_API.h"
#include "SensorLocation_API.h"
#include "SensorMotion_API.h"
#include "DR_API.h"

/************************************************************************
*            Macro definitions                                                 *
************************************************************************/

/*----------------------------------------------------------------------*
 *    DEADRECKONING_RET_API define                                         *
 *----------------------------------------------------------------------*/
/* Normal system */
#define DEAD_RECKONING_RET_NORMAL               0       /* Normal completion */
/* Abnormal system */
#define DEADRECKONING_RET_ERROR_PID             (-1)    /* Thread ID error */
#define DEADRECKONING_RET_ERROR_DID             (-2)    /* Unregistered DID */
#define DEADRECKONING_RET_ERROR_PARAM           (-4)    /* Parameter error */
#define DEADRECKONING_RET_ERROR_BUFFULL         (-5)    /* FULL of registered numbers */
#define DEADRECKONING_RET_ERROR_CREATE_EVENT    (-6)    /* Event generation failure */
#define DEADRECKONING_RET_ERROR_OUTOF_MEMORY    (-8)    /* Shared memory allocation failed */
#define DEADRECKONING_RET_ERROR_SIZE            (-9)    /* Storage destination size error */
#define DEADRECKONING_RET_ERROR_MIN             (-10)

/*----------------------------------------------------------------------*
 *    DEADRECKONING delivery registry APIs                                       *
 *----------------------------------------------------------------------*/
/* Delivery control */
#define DEADRECKONING_DELIVERY_REGIST           0x01    /* Delivery registration */

/* Delivery timing */
#define DEADRECKONING_DELIVERY_TIMING_UPDATE    0x01    /* Updating */

/*----------------------------------------------------------------------*
 *    Command ID                                                         *
 *----------------------------------------------------------------------*/
/* Vehicle Sensor -> Vehicle Sensor(DR) */
#define CID_DEAD_RECKONING_DELIVERY_ENTRY       0x0310  /* DR Transmission Registration Signal Notification */

/* Vehicle Sensor -> Vehicle Sensor(DR) */
#define CID_DEAD_RECKONING_SENS_DATA            0x0311  /* DR data signal notification */
#define CID_DEAD_RECKONING_GPS_DATA             0x0312  /* DR data signal notification */
#define CID_DEAD_RECKONING_SENS_FST_DATA        0x0313  /* DR data signal notification */

/* Message Data Size(byte) */
#define DEADRECKONING_MSGBUF_DSIZE              512     /* Message body MAX */

/*----------------------------------------------------------------------*
 *    Shared Memory Related Extensions                                                     *
 *----------------------------------------------------------------------*/
/* Semaphore name(MAX 32Byte) Synchronize with VehicleDebug_API_private.h */
#define SENSOR_LOG_SETTING_SEMAPHO_NAME         ("SEBSIR_GET_LOG_SETTING_SEMAPHO")

/* Shared memory name(MAX 32Byte) Synchronize with VehicleDebug_API_private */
#define LOG_SETTING_SHARE_MEMORY_NAME           ("LOG_SETTING_SHARE_MEMORY")

/************************************************************************
*            typedef Definition                                         *
************************************************************************/
typedef RET_API DEAD_RECKONING_RET_API;                 /* API return value */

/************************************************************************
*            Struct definitions                                                 *
************************************************************************/

/************************************************************************
* TAG       : DEADRECKONING_MSG_BUF
* ABSTRACT  : message buffer
************************************************************************/

typedef struct {
    T_APIMSG_MSGBUF_HEADER    hdr;                                 /* Message header */
    u_int8                    data[DEADRECKONING_MSGBUF_DSIZE];    /* Message data */
} DEADRECKONING_MSG_BUF;

/************************************************************************
* TAG       : DEADRECKONING_MSG_DELIVERY_ENTRY
* ABSTRACT  : Registration message delivery vehicle sensor information(->Vehicle sensor)
************************************************************************/
typedef struct {
    DID                       did;                /* ID data corresponding to the vehicle sensor information */
    PNO                       pno;                /* Shipping addressPNO */
    u_int8                    delivery_timing;    /* Delivery opportunity */
    u_int8                    ctrl_flg;           /* Delivery control */
    EventID                   event_id;           /* Event ID */
} DEADRECKONING_MSG_DELIVERY_ENTRY_DAT;

typedef struct {
    T_APIMSG_MSGBUF_HEADER                  hdr;  /* Message header */
    DEADRECKONING_MSG_DELIVERY_ENTRY_DAT    data; /* Message data */
} DEADRECKONING_MSG_DELIVERY_ENTRY;

/************************************************************************
* TAG       : DEADRECKONING_MSG_GET_DR_DATA
* ABSTRACT  : Vehicle sensor information acquisition message(->Vehicle sensor)
************************************************************************/
typedef struct {
    DID                       did;                /* Data ID corresponding to vehicle sensor information    */
    PNO                       pno;                /* Destination PNO                        */
    u_int16                   offset;             /* Offset to shared memory storage area    */
    u_int16                   size;               /* Size of shared memory storage area            */
    u_int8                    reserve[2];
    EventID                   event_id;           /* Event ID                        */
} DEADRECKONING_MSG_GET_DR_DATA_DAT;

typedef struct {
    T_APIMSG_MSGBUF_HEADER                  hdr;  /* Message header    */
    DEADRECKONING_MSG_GET_DR_DATA_DAT       data; /* Message data    */
} DEADRECKONING_MSG_GET_DR_DATA;

/************************************************************************
*            Function prototype                                              *
************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif  // POSITIONING_CLIENT_INCLUDE_DEAD_RECKONING_LOCAL_API_H_
