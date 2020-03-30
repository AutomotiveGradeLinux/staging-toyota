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

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * File name        : TimerDrv_If.h
 * System name      : Integrated PF
 * Process name     : Timer registry driversI/F
 * Overview         : Timer registry drivers I/F Header file
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef POSITIONING_BASE_LIBRARY_LIBRARY_INCLUDE_TIMERENTRYDRV_IF_H_
#define POSITIONING_BASE_LIBRARY_LIBRARY_INCLUDE_TIMERENTRYDRV_IF_H_

#include <vehicle_service/positioning_base_library.h>
#include "DEV_TimerEntryDrv_if.h"

/*
 Constant definition
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define TED_DRV_NAME           "TED"                             /* Channel names of drivers registered in the Timer */
/* The channel name cannot be a leading slash. Location is under /dev/name/local */

/*-----------------------------------------------------------------------------
 * CTRL CODE definitions
 *----------------------------------------------------------------------------*/
/* IOCTRL internal-processing sort codes */
#define TED_IOCTRL_TIMREQ        __DIOTF(_DCMD_MISC, 1, stTedApidt)    /* Timer start    */
#define TED_IOCTRL_STPREQ        __DIOT(_DCMD_MISC, 2, stTedApidt)    /* Timer stop    */

#define    TED_IF_SETTIME_MIN            (int32)1                        /* Minimum count time(1ms)             */

#define TED_IF_SETPNO_NG            (PNO)0                            /* PNO invalid value for _CWORD64_ messages        */
#define TED_IF_MMEVENT_INVALID_ID    0                                /* Invalid _CWORD64_ EventID                */

#define TED_IF_RET__CWORD64_MSG          (u_int16)1           /* How Users Are Notified on Timeouts: _CWORD64_ Messages */
#define TED_IF_RET_WINEVT           (u_int16)2           /* How Users Are Notified on Timeouts: Windows Events */

/* Timer type */
#define    TED_IF_TIMERTYPE_CYCLE        1                                /* Fixed period Timer  Specified in [10ms] */
#define    TED_IF_TIMERTYPE_SINGLE        2                                /* Single occurrence Timer  Specified in [10ms]  */

/* API type(Distinguishing between API Functions Used at Timer Startup) */
#define TED_IF_APITYPE_SYS            1                                /* Timer start by _pb_ReqTimerStart() */
#define TED_IF_APITYPE_DEV            2                                /* Timer start by DEV_SetTimer_XXX() */

/*
 Data type definition
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* Timer registration data by user(Timer register driver IF -> timer register driver) */
typedef struct {
    int32        time_rem;        /* User's remaining timer count time(count value in 10 [msec] units)        */
    u_int32        timer_id;        /* User's timer ID                                            */
    u_int32        ret_data;        /* User's extended information                                            */
    PNO         ret_pno;            /* User's _CWORD64_ message-destination process number                        */
    u_int16        fin_mode;        /* User Timeout Notification Method                             */
    char        wev_ob_name[DEV_TED_EVTNAME_MAX];    /* Event name(32 characters)                        */
    u_int32        timer_seq;        /* Timer Sequence Number                                            */
    u_int8        time_type;        /* Timer type(Fixed period/single occurrence)                                        */
    u_int8        api_type;        /* API type(_pb_ReqTimerStart/DEV_SetTimer)                        */
    u_int8        reserve[2];
} stTedApidt;
typedef stTedApidt *PTED_IFDT;

/* Timer Start/Stop Messages Structures, */
typedef struct {
    int32        time_rem;        /* User's remaining timer count time(count value in 10 [msec] units)        */
    u_int32        timer_id;        /* User's timer ID                                            */
    u_int32        ret_data;        /* User's extended information                                            */
    PNO         ret_pno;            /* User's _CWORD64_ message-destination process number                        */
    u_int16        fin_mode;        /* User Timeout Notification Method                             */
    char        wev_ob_name[DEV_TED_EVTNAME_MAX];    /* Event name(32 characters)                        */
    u_int32        timer_seq;        /* Timer Sequence Number                                            */
    u_int8        time_type;        /* Timer type(Fixed period/single occurrence)                                        */
    u_int8        api_type;        /* API type(_pb_ReqTimerStart/DEV_SetTimer)                        */
    u_int8        reserve[2];
} TIMERUSERDATA;

/* Structures for sending _CWORD64_ message timer START */
typedef struct {
    T_APIMSG_MSGBUF_HEADER    header;                /* _CWORD64_ Message header */
    /* Extended Info for _CWORD64_ Messages */
    TIMERUSERDATA      st_us_data;
} stTimerStart_CWORD64_msg;

/* Structures for sending _CWORD64_ message timer STOP */
typedef struct {
    T_APIMSG_MSGBUF_HEADER    header;                /* _CWORD64_ Message header */
    /* Extended Info for _CWORD64_ Messages */
    TIMERUSERDATA      st_us_data;
} stTimerStop_CWORD64_msg;

#endif   // POSITIONING_BASE_LIBRARY_LIBRARY_INCLUDE_TIMERENTRYDRV_IF_H_
/*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 End of File : TimerEntryDrv_If.h
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*/

