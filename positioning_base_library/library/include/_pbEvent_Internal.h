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

/*******************************************************************************
* $Header::                                                                   $
* $Revision::                                                                 $
*******************************************************************************/

/****************************************************************************
 * File name        : _pbEvent_Internal.h
 * System name      : GPF
 * Subsystem name   : _CWORD64_API Events Feature
 * Title            : Internal definition file for event function
 ****************************************************************************/
#ifndef POSITIONING_BASE_LIBRARY_LIBRARY_INCLUDE__PBEVENT_INTERNAL_H_
#define POSITIONING_BASE_LIBRARY_LIBRARY_INCLUDE__PBEVENT_INTERNAL_H_

#include <other_service/ev_lib.h>

/*
 Constant definition
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#define MAX_PB_EVENTS                16
#define MAX_EVENT_NAME_LEN           32
#define MIN_EVENT_VAL                0x80000000   /* Minimum event value(-2147483648) */
#define MAX_EVENT_VAL                0x7FFFFFFF   /* Maximum event value( 2147483647) */
#define EVENT_BIT_ZERO               0x00000000   /* Event value All Bit Zero Definition */
#define MAX_PB_EVENT_WAIT_THREADS    1            /* Values other than 1 are not designed while the EV library is being used. */
#define EVSET_ABSOLUTE               1            /* Absolute setting */
#define EVSET_RELATE                 2            /* Relative value setting */
#define EVSET_AND                    3            /* AND value setting */
#define EVSET_OR                     4            /* OR setting */
#define EVWAIT_VAL                   1            /* Waiting mode for an event by specifying a range */
#define EVWAIT_ALLCLR                2            /* Mode to wait for an event when the specified bit is cleared */
#define EVWAIT_ANYSET                3            /* Mode to wait for an event with a specified set of bits */

#define _CWORD64_EVENT_MAXOPEN_IN_PROCESS    0x7FFFFFFF    /* Maximum number of open events in the same process    */
#define _CWORD64_EVENT_MAXOPEN_IN_SYSTEM     0x7FFFFFFF    /* Maximum number of the same event open in the system        */

#define MAX_EVENT_PROC_NUM (32) /* EVENT Max Processes Used */

/*
 Structure Definition
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*****************************************************************************
 * TAG      : WAITING_CONDITION
 * ABSTRACT : Event Status Settings
******************************************************************************/
typedef struct {
    u_int8                   uc_use_flag;        /* Table Usage Flags(TRUE:Use FALSE:Not used) */
    u_int8                   uc_waiting;        /* Event Waiting Flag */
    /* TRUE        Waiting for event */
    /* FALSE    Release Event Wait */
    u_int16                  us_mode;            /* Waiting mode */
    /* EVWAIT_VAL        Wait for Value Range */
    /* EVWAIT_ALLCLR    Wait for specified bit to be cleared */
    /* EVWAIT_ANYSET    Waiting for setting the specified bit */
    u_int32                  ul_mask;            /* Mask value when waiting for specified bit */
    int32                    l_min_val;        /* Minimum value of condition when waiting for value range */
    int32                    l_max_val;        /* Maximum value of condition when waiting for value range */
    int32                    l_last_val;        /* Events at WaitEvent Return */
    EV_ID                    flag_id[MAX_EVENT_PROC_NUM];            /* Event flags(CLS event library) */
} WAITING_CONDITION;                        /* Total 28 Bytes. */

/*****************************************************************************
 * TAG      : WAITING_CONDITION
 * ABSTRACT : Event information storage area
******************************************************************************/
typedef struct {
    TCHAR                    event_name[MAX_EVENT_NAME_LEN];            /* Maximum number of characters + NULL area */
    int32                    l_event_val;
    WAITING_CONDITION        st_condition[MAX_PB_EVENT_WAIT_THREADS];
    int32                    l_process_ref;
    int32                    l_reset_data;
    u_int8                   uc_manual_reset;
    u_int8                   uc_reserve[3];
    char                     name_of_mutex[NAME_MAX];
    /* TODO: Members for the following EV library use:Structurely strange, but borrowed shared space.Correction is required. */
    u_int8                   proc_cnt;
} PB_EVENT;

/*****************************************************************************
 * TAG      : PB_EVENT_OPEN_HANDLE
 * ABSTRACT : Generated event management information
******************************************************************************/
typedef struct {
    HANDLE                   h_heap;
    PB_EVENT*                p_sys_event;
    DWORD                    index;
    int32                    l_thread_ref;
} PB_EVENT_OPEN_HANDLE;

/*****************************************************************************
 * TAG      : PB_EVENT_INSTANCE
 * ABSTRACT : Event management table
******************************************************************************/
typedef struct {
    PB_EVENT_OPEN_HANDLE*    p_handle_table[MAX_PB_EVENTS];
    HANDLE                   h_shared_memory;
    SemID                    id_event_table_sem;
    PB_EVENT*                p_event_table;
} PB_EVENT_INSTANCE;

/*
 Prototype declaration
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifdef    __cplusplus
extern "C" {
#endif

#ifdef    __cplusplus
}
#endif

#endif   // POSITIONING_BASE_LIBRARY_LIBRARY_INCLUDE__PBEVENT_INTERNAL_H_
