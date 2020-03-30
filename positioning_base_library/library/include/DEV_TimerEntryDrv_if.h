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

/****************************************************************************
 * File name      : DEV_TimerEntryDrv_if.h
 * System name    : Integrated PF
 * Subsystem name : Timer registry drivers
 * Title          : APIs for Timer registry drivers
 ****************************************************************************/
#ifndef POSITIONING_BASE_LIBRARY_LIBRARY_INCLUDE_DEV_TIMERENTRYDRV_IF_H_
#define POSITIONING_BASE_LIBRARY_LIBRARY_INCLUDE_DEV_TIMERENTRYDRV_IF_H_

#include <vehicle_service/positioning_base_library.h>

/*-----------------------------------------------------------------------------
 * Constant definition
 *----------------------------------------------------------------------------*/
/* Return value */
#define DEV_TED_INVALID            (u_int32)0        /* Timer start failure    */
#define DEV_TED_STOP_NG            (u_int32)0        /* Timer stop failure        */
#define DEV_TED_STOP_OK            (u_int32)1        /* Timer stop success        */

/* Definitions for event Handle names */
#define DEV_TED_EVTNAME_MAX        (u_int32)32        /* _CWORD64_ Events HANDLE Name */

/*-----------------------------------------------------------------------------
 * Structure Definition
 *----------------------------------------------------------------------------*/
/* Structures for timer setting instructions (_CWORD64_ messages) */
typedef struct {
    int32    set_time;                            /* Count time                */
    PNO        pno;                                /* _CWORD64_ Messages Destination PNOs  */
    u_int32    ext_data;                            /* Extended Info for _CWORD64_ Messages */
} stTED_SetTime__CWORD64_msg;

/* Structures for timer setting directives (Windows events) */
typedef struct {
    int32    set_time;                            /* Count time                */
    char    evob_name[DEV_TED_EVTNAME_MAX];        /* _CWORD64_ Events HANDLE Name */
    int32    ext_data;                            /* _CWORD64_ Events Extended Info        */
} stTED_SetTime__CWORD64_evt;

/* Structures for sending _CWORD64_ messages */
typedef struct {
    T_APIMSG_MSGBUF_HEADER    header;                /* _CWORD64_ Message header */
    u_int32                    timer_id;            /* Timer IDs for _CWORD64_ messages */
    u_int32                    us_data;                /* Extended Info for _CWORD64_ Messages */
} stTimer_CWORD64_msg;

/* Structures for timer setting directives (Windows events) */
typedef struct {
    int32    set_time;                            /* Count time */
    wchar_t    evob_name[DEV_TED_EVTNAME_MAX];        /* _CWORD64_ Events HANDLE Name */
} stTED_SetTime_winevt;
/*
 API Function Prototype
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
u_int32 DEVSetTimer_CWORD64_msg(stTED_SetTime__CWORD64_msg  *p_set_data);
u_int32 DEVSetTimer_CWORD64_event(stTED_SetTime__CWORD64_evt *p_set_data);
u_int32 DEVStopTimer(u_int32 time_id);

#endif    // POSITIONING_BASE_LIBRARY_LIBRARY_INCLUDE_DEV_TIMERENTRYDRV_IF_H_

