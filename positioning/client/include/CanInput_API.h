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
 * File name          : CanInput_API.h
 * System name        : PastModel002
 * Sub System name    : CanInput_API library
 ******************************************************************************/
#ifndef POSITIONING_CLIENT_INCLUDE_CANINPUT_API_H_
#define POSITIONING_CLIENT_INCLUDE_CANINPUT_API_H_

/************************************************************************
 *            Include                                                   *
 ***********************************************************************/
#include <vehicle_service/positioning_base_library.h>        /* for type definitions            */

/************************************************************************
*            Definition                                                 *
************************************************************************/
/*----------------------------------------------------------------------*
 *    CAN_INPUT_RET_API Definition                                      *
 *----------------------------------------------------------------------*/
/* Normal */
#define CAN_INPUT_RET_NORMAL                    0       /* Successful completion    */

/* Abnormal */
#define CAN_INPUT_RET_ERROR                     (-1)    /* An error has occurred    */
#define CAN_INPUT_RET_ERROR_PARAM               (-2)    /* Parameter error          */

/*----------------------------------------------------------------------*
 *    Message Definition                                                *
 *----------------------------------------------------------------------*/
/* Command ID */
#define CANINPUT_CID_LOCALTIME_NOTIFICATION     0x0220

/*----------------------------------------------------------------------*
 *    Localtime status Definition                                       *
 *----------------------------------------------------------------------*/
#define CANINPUT_LOCALTIME_STATUS_INVALID       0U
#define CANINPUT_LOCALTIME_STATUS_INIT          1U
#define CANINPUT_LOCALTIME_STATUS_PLAUSIBLE     2U
#define CANINPUT_LOCALTIME_STATUS_NEW           3U
#define CANINPUT_LOCALTIME_STATUS_DIAG          4U
#define CANINPUT_LOCALTIME_STATUS_INCOMPLETE    5U

/************************************************************************
*            type Definition                                            *
************************************************************************/
typedef RET_API CAN_INPUT_RET_API;    /* API return value    */

/************************************************************************
*            struct Definition                                          *
************************************************************************/
/************************************************************************
 * TAG         : CANINPUT_MSG_LOCALTIMEINFO_DAT
 * ABSTRACT     : Localtime data structure
 * NOTE         :
 ************************************************************************/
typedef struct {
    u_int8    status;     /* status    : Refer to Localtime status Definition   */
    u_int8    year;       /* year      : 0..99 (FFh:invalid)                    */
    u_int8    month;      /* month     : 1..12 (FFh:invalid)                    */
    u_int8    date;       /* date      : 1..31 (FFh:invalid)                    */
    u_int8    hour;       /* hour      : 0..23 (FFh:invalid)                    */
    u_int8    minute;     /* minute    : 0..59 (FFh:invalid)                    */
    u_int8    second;     /* second    : 0..59 (FFh:invalid)                    */
    u_int8    reserve;    /* reserve                                            */
} CANINPUT_MSG_LOCALTIMEINFO_DAT;

/************************************************************************
 *            Function prototypes                                       *
 ************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/*[CanInput_API Public API]*/
CAN_INPUT_RET_API CanInputInitialize(void);
CAN_INPUT_RET_API CanInputSndMsg(PNO pno, CID cid, u_int16 msg_len, const void *msg_data);

#ifdef __cplusplus
}
#endif

#endif  // POSITIONING_CLIENT_INCLUDE_CANINPUT_API_H_
