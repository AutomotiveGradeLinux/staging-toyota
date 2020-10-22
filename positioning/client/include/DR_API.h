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
@file       DR_API.h
@detail     DR_API external header file<BR>
            DR_API External public header
******************************************************************************/
#ifndef POSITIONING_CLIENT_INCLUDE_DR_API_H_
#define POSITIONING_CLIENT_INCLUDE_DR_API_H_

/*****************************************************************************
 *      Include                                                              *
 *****************************************************************************/

/*****************************************************************************
 *      Define                                                               *
 *****************************************************************************/
/* Command ID */
#define CID_DR_MAP_MATCHING_DATA        (0x0209)
#define CID_DR_CLEAR_BACKUP_DATA        (0x020F)

/* DR_EXT_RET_API definition */
#define DR_EXT_RET_NORMAL               (0)        /* Successful completion    */
#define DR_EXT_RET_ERROR                (-1)       /* Setup failure        */

/*****************************************************************************
 *      Typedef                                                              *
 *****************************************************************************/
typedef int32 DR_EXT_RET_API;

/*****************************************************************************
 *      Enumeration                                                          *
 *****************************************************************************/
typedef enum {
    NAVI_DATA_STATUS_2D_FIX = 0x00,
    NAVI_DATA_STATUS_3D_FIX,
    NAVI_DATA_STATUS_DR_COMBINED,
    NAVI_DATA_STATUS_DR_ONLY,
    NAVI_DATA_STATUS_MAP_2D_GPS_FIX,
    NAVI_DATA_STATUS_MAP_3D_GPS_FIX,
    NAVI_DATA_STATUS_MAP_DR_COMBINED,
    NAVI_DATA_STATUS_MAP_DR_ONLY,
    NAVI_DATA_STATUS_MAP_NO_FIX,
    NAVI_DATA_STATUS_AWAITING_FIRST_FIX,
    NAVI_DATA_STATUS_DATA_INVALID,
    NAVI_DATA_STATUS_NO_FIX,
    NAVI_DATA_STATUS_TIME_ONLY_FIX
} NAVI_DATA_STATUS;

typedef enum {
    NAVI_DATA_INVALID = 0x00,
    NAVI_DATA_VALID
} NAVI_DATA_AVAILABLE;

/*****************************************************************************
 *      Struct                                                               *
 *****************************************************************************/
/*****************************************************************************
 * TAG     : POSITION_INFO
 * ABSTRACT   : Position information structure
 * NOTE     :
 *****************************************************************************/
typedef struct {
    int32                  latitude;               /* 10^-7deg    North : plus, South : minus */
    int32                  longitude;              /* 10^-7deg    East  : plus, West  : minus */
    NAVI_DATA_AVAILABLE    status;                 /* Status of data */
} POSITION_INFO;

/*****************************************************************************
 * TAG     : RATE_INFO
 * ABSTRACT   : Rate information  structure
 * NOTE     :
 *****************************************************************************/
typedef struct {
    u_int16                rate;                   /* 10^-2m/sec */
    u_int8                 reserve[2];             /* reserve */
    NAVI_DATA_AVAILABLE    status;                 /* Status of data */
} RATE_INFO;

/*****************************************************************************
 * TAG     : ORIENT_INFO
 * ABSTRACT   : Orient information structure
 * NOTE     :
 *****************************************************************************/
typedef struct {
    u_int16                orient;                 /* 10^-2deg    N : 0, E : 90, S : 180, W : 270 */
    u_int8                 reserve[2];             /* reserve */
    NAVI_DATA_AVAILABLE    status;                 /* Status of data */
} ORIENT_INFO;

/*****************************************************************************
 * TAG     : MAP_MATCHING_DATA
 * ABSTRACT   : Map-Matching information data structure
 * NOTE     :
 *****************************************************************************/
typedef struct {
    POSITION_INFO          position_info;          /* Position information */
    RATE_INFO              rate_info;              /* Rate information */
    ORIENT_INFO            orient_info;            /* Orient information */
    NAVI_DATA_STATUS       status;                 /* Status */
} MAP_MATCHING_DATA;

/*****************************************************************************
 * TAG     : DR_MSG_MAP_MATCHING_DATA
 * ABSTRACT   : Map-Matching information data message buffer structure
 * NOTE     : Message structure(User -> VehicleSens)
 *****************************************************************************/
typedef struct {
    T_APIMSG_MSGBUF_HEADER    hdr;                 /* Message header */
    MAP_MATCHING_DATA         data;                /* Message data (Map-Matching information) */
} DR_MSG_MAP_MATCHING_DATA;

/*****************************************************************************
 * TAG     : DR_MSG_CLEAR_BACKUP_DATA
 * ABSTRACT   : Clear backup data message buffer structure
 * NOTE     : Message structure(User -> VehicleSens)
 *****************************************************************************/
typedef struct {
    T_APIMSG_MSGBUF_HEADER    hdr;                 /* Message header */
} DR_MSG_CLEAR_BACKUP_DATA;

/*****************************************************************************
*      Function prototypes                                                   *
*****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
extern DR_EXT_RET_API DrSetMapMatchingData(PNO pno, MAP_MATCHING_DATA* map_matching_data);
extern DR_EXT_RET_API DrClearBackupData(PNO pno);
#ifdef __cplusplus
}
#endif

#endif  // POSITIONING_CLIENT_INCLUDE_DR_API_H_
