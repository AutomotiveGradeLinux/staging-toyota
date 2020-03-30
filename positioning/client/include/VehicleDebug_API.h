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
 * File name        : VehicleDebug_API.h
 * System name        : PastModel002
 * Sub System name    : VehicleDebug_API library
 ******************************************************************************/
#ifndef POSITIONING_CLIENT_INCLUDE_VEHICLEDEBUG_API_H_
#define POSITIONING_CLIENT_INCLUDE_VEHICLEDEBUG_API_H_

/************************************************************************
 *            Include                                                   *
 ***********************************************************************/

/************************************************************************
*            Definition                                                 *
************************************************************************/
/*----------------------------------------------------------------------*
 *    VEHICLEDEBUG_RET_API Definition                                   *
 *----------------------------------------------------------------------*/
/* Normal */
#define VEHICLEDEBUG_RET_NORMAL         0       /* Successful completion    */

/* Abnormal */
#define VEHICLEDEBUG_RET_ERROR          (-1)    /* An error has occurred    */
#define VEHICLEDEBUG_RET_ERROR_PARAM    (-2)    /* Parameter error            */

/*----------------------------------------------------------------------*
 *    Message Definition                                                    *
 *----------------------------------------------------------------------*/
/* Command ID */
#define CID_VEHICLEDEBUG_LOG_SET        0x020D
#define CID_VEHICLEDEBUG_LOG_GET        0x020E

/* Message Length */
#define VEHICLEDEBUG_MSGBUF_DSIZE       36U

/*----------------------------------------------------------------------*
 *    Log Kind Definition                                               *
 *----------------------------------------------------------------------*/
#define LOG_KIND_NUM                    32U

/*----------------------------------------------------------------------*
 *    Log Mask Definition                                               *
 *----------------------------------------------------------------------*/
#define LOG_MASK_LOCATION_LOG           0x00000001UL

/*----------------------------------------------------------------------*
 *    Log Severity Definition                                           *
 *----------------------------------------------------------------------*/
/* Unused */

/************************************************************************
*            type Definition                                            *
************************************************************************/
typedef RET_API VEHICLEDEBUG_RET_API;           /* API return value    */

/************************************************************************
*            struct Definition                                            *
************************************************************************/
/************************************************************************
 * TAG         : VEHICLEDEBUG_MSG_LOGINFO_DAT
 * ABSTRACT     : Log data structure
 * NOTE         :
 ************************************************************************/
typedef struct {
    u_int32                   log_sw;                             /* Kind of log   */
    u_int8                    severity[LOG_KIND_NUM];             /* Output level(unused)    */
} VEHICLEDEBUG_MSG_LOGINFO_DAT;

/*****************************************************************************
 * TAG         : VEHICLEDEBUG_MSG_BUF
 * ABSTRACT     : VehicleDebug_API message buffer structure
 * NOTE         : Message structure
 *****************************************************************************/
typedef struct {
    T_APIMSG_MSGBUF_HEADER    hdr;                                /* Message header        */
    u_int8                    data[VEHICLEDEBUG_MSGBUF_DSIZE];    /* Message data            */
} VEHICLEDEBUG_MSG_BUF;

/************************************************************************
 *            Function prototypes                                              *
 ************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/*[VehicleDebug_API Public API]*/
VEHICLEDEBUG_RET_API SensorGetLogSetting(PNO pno, u_int32 *log_sw, u_int8 *severity);
VEHICLEDEBUG_RET_API SensorSetLogStatus(PNO pno, u_int32 log_sw, u_int8 *severity);

#ifdef __cplusplus
}
#endif

#endif  // POSITIONING_CLIENT_INCLUDE_VEHICLEDEBUG_API_H_
