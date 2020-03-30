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

#ifndef POSITIONING_CLIENT_INCLUDE_POS_SENSOR_PRIVATE_H_
#define POSITIONING_CLIENT_INCLUDE_POS_SENSOR_PRIVATE_H_
/******************************************************************************
 * file name    :POS_sensor_private.h
 * system name    :Positioning
 * sub system name  :Positioning internal interface library
 ******************************************************************************/

#include <vehicle_service/POS_sensor_API.h>
#include <vehicle_service/pos_message_header.h>
#include <native_service/frameworkunified_types.h>
/************************************************************************
*                                  definition                           *
************************************************************************/
/*----------------------------------------------------------------------*
 *  for message                              *
 *----------------------------------------------------------------------*/
/* command ID */
#define CID_SENSOR_PKG_INFO           0x0700                        /* vehicle sensor package notification ID */

/*----------------------------------------------------------------------*
 *                     definition for GPS response                      *
 *----------------------------------------------------------------------*/
#define GPS_BUFFERFUL                 2                             /* transmit buffer full(rejection)    */
#define GPS_INITIAL                   3                             /* initialization state(rejection)    */
#define GPS_CONNECTNG                 4                             /* connection error(rejection)      */

/*----------------------------------------------------------------------*
 *                       definition clock status                        *
 *----------------------------------------------------------------------*/
#define CLOCK_VALID                   (1U)
#define CLOCK_INVALID                 (0U)

/*----------------------------------------------------------------------*
 *    Message Related Extensions                                                     *
 *----------------------------------------------------------------------*/
#define CID_SENSORIF_SET_GPSTIME      0x0701                        /* GPS time information setting CID */

/*----------------------------------------------------------------------*
 * GPS version-related definitions                                                 *
 *----------------------------------------------------------------------*/
#define GPS__CWORD82__VERSION_LEN           (7U)                          /* _CWORD82_ version data length */

/************************************************************************
*                               struct declaration                      *
************************************************************************/
/************************************************************************
* TAG       : SENSOR_MSG_DELIVERY_ENTRY
* ABSTRACT  : vehicle sensor delivery message (to vehicle sensor)
************************************************************************/
typedef struct {
    PNO                               pno;                          /* destination process number */
    u_int8                            pkg_num;                      /* number of package data (1 - 16) */
    u_int8                            delivery_timing;              /* delivery timing          */
    u_int8                            ctrl_flg;                     /* delivery control flag      */
    u_int8                            reserve[3];
    EventID                           event_id;                     /* event ID              */
    DID                               did[SENSOR_PKG_DELIVERY_MAX];    /* data ID array          */
} SENSOR_MSG_DELIVERY_ENTRY_DAT;

typedef struct {
    T_APIMSG_MSGBUF_HEADER            hdr;                          /* message header  */
    SENSOR_MSG_DELIVERY_ENTRY_DAT     data;                         /* message body    */
} SENSOR_MSG_DELIVERY_ENTRY;

/************************************************************************
* TAG       : SENSOR_MSG_GET_SENSOR_DATA
* ABSTRACT  : vehicle sensor getter message (to vehicle sensor)
************************************************************************/
typedef struct {
    PNO                               pno;                          /* destination process number    */
    u_int8                            pkg_num;                      /* number of package data (1 - 16)  */
    u_int8                            reserve;
    u_int16                           offset;                       /* share memory offset        */
    u_int16                           size;                         /* share memory allocation size    */
    EventID                           event_id;                     /* event ID              */
    DID                               did[SENSOR_PKG_DELIVERY_MAX];    /* data ID about vehicle sensor    */
} SENSOR_MSG_GET_SENSOR_DATA_DAT;

typedef struct {
    T_APIMSG_MSGBUF_HEADER            hdr;                          /* message header  */
    SENSOR_MSG_GET_SENSOR_DATA_DAT    data;                         /* message body    */
} SENSOR_MSG_GET_SENSOR_DATA;

/************************************************************************
* TAG       : SENSOR_INTERNAL_MSG_BUF
* ABSTRACT  : message buffer
************************************************************************/
typedef struct {
    T_APIMSG_MSGBUF_HEADER            hdr;                          /* message header  */
    u_int8                            data[SENSOR_MSGBUF_DSIZE];    /* message body    */
} SENSOR_INTERNAL_MSG_BUF;

/********************************************************************************
 * TAG      TG_GPS_RET_TIMESET_MSG
 * ABSTRACT :GPS Time Setting Response Message Structure
 ********************************************************************************/
typedef struct {
    T_APIMSG_MSGBUF_HEADER            header;                       /* Message header */
    uint64_t                          status;                       /* Response status */
} TG_GPS_RET_TIMESET_MSG;

/********************************************************************************
 * TAG      :TG_GPSTIME
 * ABSTRACT :GPS Absolute Time Structure
 * NOTE     :
 ********************************************************************************/
typedef struct {
    u_int16                           year;                         /* Year information(4-digit year)    */
    u_int8                            month;                        /* Month information          */
    u_int8                            day;                          /* Day information          */
    u_int8                            hour;                         /* Hour information          */
    u_int8                            minute;                       /* Minute information          */
    u_int8                            second;                       /* Second information          */
    u_int8                            reserve;                      /* reserve          */
} TG_GPSTIME;

/********************************************************************************
 * TAG      :TG_GPS_RTCBKUP_DATA
 * ABSTRACT :RTC backup-related data structures
 * NOTE     :RTC-backup-related data stored in the SRAM alternate DRAM
 ********************************************************************************/
typedef struct {
    u_int8                            time_status;                  /* Time Status */
    u_int8                            reserve1[3];                  /* Reserve 1 */
    TG_GPSTIME                        gps_time;                     /* GPS Absolute Time */
} TG_GPS_RTCBKUP_DATA;                      /*                                  */

/************************************************************************
 * TAG      : GPS_INTERRUPT
 * ABSTRACT : GPS interrupt status
 * NOTE     :
************************************************************************/
typedef struct {
    u_int8                            _CWORD102__interrupt;                 /* from GPS to _CWORD102_   interrupt status  */
    u_int8                            _CWORD56__interrupt;               /* from GPS to _CWORD56_ interrupt status  */
} GPS_INTERRUPT;

/********************************************************************************
 * TAG      :LOCALTIME
 * ABSTRACT :LocalTime data structure
 * NOTE     :
 ********************************************************************************/
typedef struct {
    u_int8                            status;                       /* status  : valid or invalid */
    u_int8                            reserve[3];    /* reserve                  */
    u_int16                           year;      /* year    : 2000..2099  (FFFFh:invalid)  */
    u_int8                            month;      /* month  : 1..12      (FFh:invalid)  */
    u_int8                            day;      /* date    : 1..31      (FFh:invalid)  */
    u_int8                            hour;      /* hour    : 0..23      (FFh:invalid)  */
    u_int8                            min;      /* minute  : 0..59      (FFh:invalid)  */
    u_int8                            sec;      /* second  : 0..59      (FFh:invalid)  */
    u_int8                            reserve2;    /* reserve                  */
} LOCALTIME;

#endif  // POSITIONING_CLIENT_INCLUDE_POS_SENSOR_PRIVATE_H_
