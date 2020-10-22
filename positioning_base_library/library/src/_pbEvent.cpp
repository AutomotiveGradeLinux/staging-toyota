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

/**
 * @file
 *    _pbEvent.cpp
 */

/*---------------------------------------------------------------------------------*
 * Include Files                                                                   *
 *---------------------------------------------------------------------------------*/
#include <vehicle_service/positioning_base_library.h>
#include "_pbEvent_Internal.h"
#include "_pbInternalProc.h"
#include "WPF_STD_private.h"
#include "tchar.h"

/*---------------------------------------------------------------------------------*
 * Internal Function Prototype                                                     *
 *---------------------------------------------------------------------------------*/
static BOOL    FindEventTable(PB_EVENT*, TCHAR*, u_int32*);
static BOOL    AllocNewEventTable(PB_EVENT*, u_int32*);

static void       FreeEventTable(PB_EVENT* p_event_table, int index);
static RET_API    SetProc(PB_EVENT_OPEN_HANDLE* p_event_open, int32 i_mode, int32 l_val, int32* lp_val);
static RET_API    WaitProc(PB_EVENT_OPEN_HANDLE*, WAITING_CONDITION*, u_int32);
static BOOL       CheckCondition(PB_EVENT* p_sys_event, DWORD wcn, int32 l_event_data);

static EventID    EventCreateNewEventInSystem(u_int8, int32, TCHAR*);
static EventID    EventCreateNewEventInProcess(u_int32);

static RET_API    EventSendSignal(PB_EVENT_OPEN_HANDLE*, u_int32);
static RET_API    EventWaitForSignal(PB_EVENT_OPEN_HANDLE*, u_int32, u_int32);

static BOOL EventCreateMutex(PB_EVENT_OPEN_HANDLE*);
static void    EventLockMutex(PB_EVENT_OPEN_HANDLE*);
static void    EventUnlockMutex(PB_EVENT_OPEN_HANDLE*);
static void    EventDeleteMutex(PB_EVENT_OPEN_HANDLE*);

void GetDebugEventMngTblSysEvent(void* p_buf, PB_EVENT* p_evt, uint8_t* p_indent);

/*---------------------------------------------------------------------------------*
 * Grobal Value                                                                    *
 *---------------------------------------------------------------------------------*/
static PB_EVENT_INSTANCE g_instance;  // NOLINT(readability/nolint)

static uint8_t g_my_proc_cnt; /* Invoking process counter value */

/*---------------------------------------------------------------------------------*
 * Function                                                                        *
 *---------------------------------------------------------------------------------*/
/**
 * @brief
 *   Initializing Event-Related Processing
 *
 *   Instantiate and initialize system API event related processing.
 *   Creates a flags in the CLS event library.
 *
 * @return    RET_NORMAL    Normal completion<br>
 *            RET_ERRINIT    Initialization error
 */
RET_API EventInit(void) {
    PB_EVENT_INSTANCE    *p_inst      = &g_instance;
    PB_EVENT*     p_event_table        = NULL;
    u_int32       ul_share_mem_size     = 0;
    RET_API       ret_api            = RET_ERROR;
    char          c_share_mem_name[32]  = {0};
    char          c_sem_name[32]       = {0};
    void          *pv_share_mem_addr    = NULL;
    int32 n;
    RET_API        l_ret_api;

    /* Create Mutex */
    _tcscpy(c_sem_name, "POS_BASE_EVENT_MUTEX");
    p_inst->id_event_table_sem = _pb_CreateSemaphore(c_sem_name);  // LCOV_EXCL_BR_LINE 200: can not be 0
    if (p_inst->id_event_table_sem == 0) /* When mutex creation fails */ {  // LCOV_EXCL_BR_LINE 200: can not be 0
        // LCOV_EXCL_START 200: can not be 0
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, \
            "_pb_CreateSemaphore ERROR [name:%s]", c_sem_name);
        _pb_Exit();
        // LCOV_EXCL_STOP
    }

    l_ret_api = _pb_SemLock(p_inst->id_event_table_sem); /* Get event-control-table-locking Mutex */
    if (l_ret_api != RET_NORMAL) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_SemLock failed");
    }

    /* Initialize table of handles. */
    for (n = 0; n < MAX_PB_EVENTS; n++) {
        p_inst->p_handle_table[n] = NULL;
    }

    /* Generate shared memory name */
    _tcscpy(c_share_mem_name, "POS_BASE_EVENT_TABLE");

    /* Link to event information storage area    */
    ret_api = _pb_LinkShareData(c_share_mem_name, &pv_share_mem_addr, &ul_share_mem_size);
    if (ret_api != RET_NORMAL) /* When the link fails    */ {
        /* Generate shared memory */
        ret_api = _pb_CreateShareData(c_share_mem_name,
            static_cast<u_int32>((sizeof(PB_EVENT) * MAX_PB_EVENTS)), &pv_share_mem_addr);

        /* Terminate processing when generating fails */
        if (ret_api != RET_NORMAL) {
            ret_api = RET_ERRINIT;    /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
        } else {
            /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
            /* Event information storage area initialization processing */
            p_event_table = reinterpret_cast<PB_EVENT*>(pv_share_mem_addr);
            for (n = 0; n < MAX_PB_EVENTS; n++) {
                memset(reinterpret_cast<void *>(p_event_table[n].event_name), 0, \
                    sizeof(p_event_table[n].event_name));
                p_event_table[n].l_event_val = 0;
                for (int wcn = 0; wcn < MAX_PB_EVENT_WAIT_THREADS; wcn++) {
                    p_event_table[n].st_condition[wcn].uc_use_flag    = FALSE;    /* Initialize to unused */
                    p_event_table[n].st_condition[wcn].uc_waiting    = FALSE;
                    p_event_table[n].st_condition[wcn].us_mode    = 0;
                    p_event_table[n].st_condition[wcn].ul_mask    = 0;        /* Initialize Mask Value */
                    p_event_table[n].st_condition[wcn].l_min_val    = 0;
                    p_event_table[n].st_condition[wcn].l_max_val    = 0;
                    /* Initialize event values at WaitEvent Returns */
                    p_event_table[n].st_condition[wcn].l_last_val    = 0;
                    p_event_table[n].st_condition[wcn].flag_id[p_event_table->proc_cnt] = 0;
                }

                p_event_table[n].l_process_ref     = 0;
                p_event_table[n].l_reset_data     = 0;
                p_event_table[n].uc_manual_reset = _CWORD64_EVENT_MANUALRESET_OFF;
            }
        }
    } else {
        /* When the link is successful */
        p_event_table = reinterpret_cast<PB_EVENT*>(pv_share_mem_addr);
        p_event_table->proc_cnt++;
    }

    if (ret_api == RET_NORMAL)    /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */ {
        g_my_proc_cnt = p_event_table->proc_cnt;
        /* Get the address of the acquired event information storage area. */
        p_inst->h_shared_memory = (HANDLE)pv_share_mem_addr;
        p_inst->p_event_table = reinterpret_cast<PB_EVENT*>(pv_share_mem_addr);
    }

    _pb_SemUnlock(p_inst->id_event_table_sem);  // LCOV_EXCL_BR_LINE 200: no branch

    return ret_api;
}

/**
 * @brief
 *   Event-related instance destruction processing
 *
 *   Delete a Flag from the CLS Event Library (Not implemented)
 *
 * @return    RET_NORMAL    Normal completion<br>
 *            RET_ERROR    ABEND
 */
RET_API EventTerm(void) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    PB_EVENT_INSTANCE        *p_inst = &g_instance;
    char    c_share_mem_name[32] = {0};

    RET_API    ret_api = RET_NORMAL;

    /* TODO:Implement processing to delete event flags */

    /* Generate shared memory name */
    _tcscpy(c_share_mem_name, "POS_BASE_EVENT_TABLE");

    /* Discard the semaphore if it has already been created */
    if (p_inst->id_event_table_sem != 0) {
        PbDeleteSemaphore(p_inst->id_event_table_sem);
        p_inst->id_event_table_sem = 0;
    }

    /* Discard the shared memory if it has already been created */
    if (p_inst->h_shared_memory != NULL) {
        /* Release shared memory */
        PbDeleteShareData(c_share_mem_name);
        p_inst->h_shared_memory = NULL;
    }

    return ret_api;
}
// LCOV_EXCL_STOP

/**
 * @brief
 *   Create the event
 *
 *   Create an event with the specified name and returns the event ID.<br>
 *    If it has already been generated, the event ID is searched and returned.
 *
 * @param[in]    uc_manual_reset
 * @param[in]    l_init_data
 * @param[in]    *cp_event_name    Pointer to the names of the event to be generated (NULL termination string)
 *
 * @return    Non-zero    Generated event ID<br>
 *            0        Event generation error
 */
EventID _pb_CreateEvent(u_int8 uc_manual_reset, int32 l_init_data,  // NOLINT(readability/nolint)
        char *cp_event_name) {  // NOLINT(readability/nolint)
    PB_EVENT_INSTANCE    *p_inst = &g_instance;
    EventID ret_event_id = 0;
    TCHAR    *p_event_name    = NULL;
    u_int32    index        = 0;
    BOOL    bret         = FALSE;
    BOOL    check_status    = TRUE; /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
    RET_API    l_ret_api;

    /* Parameter check */
    if ((cp_event_name == NULL) ||
            (uc_manual_reset >= _CWORD64_EVENT_MANUALRESET_MAX)) {
        check_status    = FALSE; /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
    }

#ifdef UNICODE
    /* Event name character limit processing */
    if (strlen(cp_event_name) > MAX_EVENT_NAME_LEN) {
        _pb_Exit();  /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
    }

    TCHAR unicodeEventName[MAX_EVENT_NAME_LEN + 1];    /* Maxmum nunber of characters + NULL area */
    mbstowcs(unicodeEventName, cp_event_name, MAX_EVENT_NAME_LEN);
    p_event_name = unicodeEventName;
#else
    p_event_name = cp_event_name;
#endif  // UNICODE

    if ((check_status == TRUE) && /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
            (p_event_name[0] == __TEXT('\0'))) {
        check_status    = FALSE; /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
    }

    /* Event name character limit processing */
    if ((check_status == TRUE) && /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
            (_tcslen(p_event_name) < MAX_EVENT_NAME_LEN)) {
        l_ret_api = _pb_SemLock(p_inst->id_event_table_sem);                    /* Mutex from here */
        if (l_ret_api != RET_NORMAL) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_SemLock failed");
        }

        /* Search the event table by the specified event name */
        bret = FindEventTable(p_inst->p_event_table, p_event_name, &index);
        /* If the same event already exists on the system */
        if (bret != FALSE) {
            ret_event_id = EventCreateNewEventInProcess(index);  // LCOV_EXCL_BR_LINE 200: no branch
        } else {
            /* When creating a new file */
            ret_event_id = EventCreateNewEventInSystem(uc_manual_reset, l_init_data, p_event_name);  // LCOV_EXCL_BR_LINE 200: no branch  // NOLINT(whitespace/line_length)
        }

        _pb_SemUnlock(p_inst->id_event_table_sem);  // LCOV_EXCL_BR_LINE 200: no branch
    }

    return ret_event_id;
}

/**
 * @brief
 *   Set the event
 *
 *   Set the event value by specifying the event ID acquired when the event was created.<br>
 *    The event value setting modes are as follows.<br>
 *      SAPI_EVSET_ABSOLUTE    : Absolute value setting(Specify the value to be set.)<br>
 *      SAPI_EVSET_RELATE        : Relative value setting(Specifies the value relative to the current value.)
 *
 * @param[in]    event_id    Specify the event ID for which the event value is to be set.
 * @param[in]    l_set_mode    Specify the event value setting mode
 * @param[in]    l_Val        Specify the event value to be set
 *
 * @return    RET_NORMAL        Normal completion<br>
 *            RET_ERRPARAM    Configuration mode error<br>
 *            RET_EV_NONE        Specified event does not exist<br>
 *            RET_EV_MAX        The set event value exceeds the maximum value<br>
 *            RET_EV_MIN        The set event value is below the minimum value.
 */
RET_API _pb_SetEvent(EventID event_id, int32 l_set_mode, int32 l_val) {   // NOLINT(readability/nolint)
    PB_EVENT_OPEN_HANDLE    *p_event_open = NULL;
    PB_EVENT_INSTANCE        *p_inst        = &g_instance;
    RET_API        ret_sts     = RET_EV_NONE;
    int32        l_work_val = 0;
    u_int32        ul_index     = (u_int32)event_id - 1;

    /* Parameter check */
    if (ul_index < MAX_PB_EVENTS) {
        /* If the specified event ID value is within range */
        p_event_open = p_inst->p_handle_table[ul_index];
        /* If the specified event ID is registered in the table, */
        if (p_event_open != NULL) {
            /* Determine the event setting mode and call the event value setting function. */
            if (l_set_mode == SAPI_EVSET_ABSOLUTE) {
                ret_sts = SetProc(p_event_open, EVSET_ABSOLUTE, l_val, &l_work_val);  // LCOV_EXCL_BR_LINE 200: no branch  // NOLINT(whitespace/line_length)
            } else if (l_set_mode == SAPI_EVSET_RELATE) {
                ret_sts = SetProc(p_event_open, EVSET_RELATE, l_val, &l_work_val);  // LCOV_EXCL_BR_LINE 200: no branch
            } else {
                ret_sts = RET_ERRPARAM;
            }
        }
    }

    return ret_sts;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : SetandEvent()
 * ABSTRACT  : Event value AND setting process
 * NOTE      : Set the logical AND result of the specified mask value to the event value of the specified event ID.
 * ARGUMENT  : EventID        event_id        Specify the event ID to wait for an event
 *           : u_int32        ul_mask        Mask value to be logically ANDed with the event value
 *           : int32*        pl_val        Pointer to the area to store the pre-event value
 * RETURN    : RET_API    RET_NORMAL        Normal completion
 *           :             RET_EV_NONE        Specified event does not exist
 *           :             RET_ERROR
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
SetandEvent(EventID event_id, u_int32 ul_mask, int32* pl_val) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    PB_EVENT_OPEN_HANDLE    *p_event_open = NULL;
    PB_EVENT_INSTANCE        *p_inst         = &g_instance;
    RET_API        ret_sts     = RET_EV_NONE;
    u_int32        ul_index     = (u_int32)event_id - 1;

    /* Parameter check */
    if ((ul_index < MAX_PB_EVENTS) &&
            (pl_val != NULL)) {
        /* If the specified event ID value is within range */
        p_event_open = p_inst->p_handle_table[ul_index];
        /* If the specified event ID is registered in the table, */
        if (p_event_open != NULL) {
            ret_sts = SetProc(p_event_open, EVSET_AND, static_cast<int32>(ul_mask), pl_val);
        }
    }

    return ret_sts;
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : SetorEvent()
 * ABSTRACT  : Event value OR setting process
 * NOTE      : Set the logical OR result of the specified mask value and the event value of the specified event ID.
 * ARGUMENT  : EventID        event_id        Specify the event ID to wait for an event.
 *           : u_int32        ul_mask        Mask value to be logically ANDed with the event value
 *           : int32*        pl_val        Pointer to the area to store the pre-event value
 * RETURN    : RET_API    RET_NORMAL        Normal completion
 *           :             RET_EV_NONE        Specified event does not exist
 *           :             RET_ERROR
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
SetorEvent(EventID event_id, u_int32 ul_mask, int32* pl_val) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    PB_EVENT_OPEN_HANDLE    *p_event_open = NULL;
    PB_EVENT_INSTANCE        *p_inst        = &g_instance;
    RET_API        ret_sts     = RET_EV_NONE;
    u_int32        ul_index     = (u_int32)event_id - 1;

    /* Parameter check */
    if ((ul_index < MAX_PB_EVENTS) &&
            (pl_val != NULL)) {
        /* If the specified event ID value is within range */
        p_event_open = p_inst->p_handle_table[ul_index];

        /* If the specified event ID is registered in the table, */
        if (p_event_open != NULL) {
            ret_sts = SetProc(p_event_open, EVSET_OR, static_cast<int32>(ul_mask), pl_val);
        }
    }

    return ret_sts;
}
// LCOV_EXCL_STOP

/**
 * @brief
 *   Wait for the event
 *
 *   Wait until the event value of the specified event ID reaches the specified range.
 *
 * @param[in]    event_id        Specify the event ID for which the event value is to be set.
 * @param[in]    l_wait_mode        Monitoring mode of event * Current only SAPI_EVWAIT_VAL is allowed
 * @param[in]    l_min_val            Minimum Event Wait
 * @param[in]    l_max_val        Maximum value waiting for an event
 * @param[in]    *pl_event_val    Pointer to the event value storage area after waiting for an event
 * @param[in]    ul_mill_sec_time    Timeout period(ms)
 *
_ * @return    RET_NORMAL        Normal completion<br>
 *            RET_EV_NONE        Specified event does not exist<br>
 *            RET_ERROR        Other errors
 */
RET_API _pb_WaitEvent(EventID event_id, int32 l_wait_mode, int32 l_min_val,   // NOLINT(readability/nolint)
        int32 l_max_val, int32* pl_event_val, u_int32 ul_mill_sec_time) {     // NOLINT(readability/nolint)
    PB_EVENT_OPEN_HANDLE     *p_event_open = NULL;
    PB_EVENT_INSTANCE        *p_inst        = &g_instance;
    WAITING_CONDITION        st_condition = {0};
    RET_API        ret_sts     = RET_EV_NONE;
    u_int32        ul_index     = (u_int32)event_id - 1;

    /* Parameter check */
    if ((pl_event_val != NULL) &&
            (ul_index < MAX_PB_EVENTS)) {
        p_event_open = p_inst->p_handle_table[ul_index];

        /* If the specified event ID is registered in the table, */
        if (p_event_open != NULL) {
            /* Set Wait Mode and Mask Value to Parameter Blk */
            st_condition.us_mode  = EVWAIT_VAL;
            st_condition.l_min_val = l_min_val;
            st_condition.l_max_val = l_max_val;

            /* Call the event wait processing */
            ret_sts = WaitProc(p_event_open, &st_condition, ul_mill_sec_time);  // LCOV_EXCL_BR_LINE 200: no branch
            if (ret_sts == RET_NORMAL) {
                *pl_event_val = st_condition.l_last_val;
            }
        }
    }

    return ret_sts;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : WaitallclrEvent()
 * ABSTRACT  : Event Bit Clear Wait
 * NOTE      : Wait until all the bits specified by the mask value are cleared
 *           : for the event value of the specified event ID.
 * ARGUMENT  : EventID    event_id            Specifies the event ID to wait for an event.
 *           : u_int32    ul_mask            Mask value waiting for an event (Bit pattern)
 *           : int32*    pl_val            Pointer to the event value storage area after waiting for an event
 *           : u_itn32    ul_mill_sec_time    Timeout period(ms)
 * RETURN    : RET_API    RET_NORMAL        Normal completion
 *           :             RET_EV_NONE        Specified event does not exist
 *           :             RET_ERROR        Maximum number of waiting threads exceeded
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
WaitallclrEvent(EventID event_id, u_int32 ul_mask, int32* pl_val, u_int32 ul_mill_sec_time) {  // LCOV_EXCL_START 8:dead code  // NOLINT(whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    PB_EVENT_OPEN_HANDLE    *p_event_open = NULL;
    PB_EVENT_INSTANCE        *p_inst        = &g_instance;
    WAITING_CONDITION        st_condition = {0};
    RET_API        ret_sts     = RET_EV_NONE;
    u_int32        ul_index     = static_cast<u_int32>(event_id) - 1;

    /* Parameter check */
    if ((pl_val != NULL) &&
            (ul_index < MAX_PB_EVENTS)) {
        p_event_open = p_inst->p_handle_table[ul_index];

        /* If the specified event ID is registered in the table, */
        if (p_event_open != NULL) {
            /* Set Wait Mode and Mask Value to Parameter Blk */
            st_condition.us_mode = EVWAIT_ALLCLR;
            st_condition.ul_mask = ul_mask;

            /* Call the event wait processing */
            ret_sts = WaitProc(p_event_open, &st_condition, ul_mill_sec_time);
            if (ret_sts == RET_NORMAL) {
                *pl_val = st_condition.l_last_val;
            }
        }
    }

    return ret_sts;
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : WaitanysetEvent()
 * ABSTRACT  : Event Bit Set Waiting Process
 * NOTE      : Wait until one of the bits specified by the mask value is set
 *           : for the event value of the specified event ID.
 * ARGUMENT  : EventID        event_id            Specify the event ID to wait for an event.
 *           : u_int32        ul_mask            Mask value waiting for an event
 *           : int32*        ipVal            Pointer to the event value storage area after waiting for an event
 *           : u_itn32        ul_mill_sec_time    Timeout period(ms)
 * RETURN    : RET_API    RET_NORMAL        Normal completion
 *           : RET_EV_NONE        Specified event does not exist
 *           : RET_ERROR        When the maximum number of waiting events is exceeded
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
WaitanysetEvent(EventID event_id, u_int32 ul_mask, int32* pl_val, u_int32 ul_mill_sec_time) {  // LCOV_EXCL_START 8:dead code  // NOLINT(whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    PB_EVENT_OPEN_HANDLE    *p_event_open = NULL;
    PB_EVENT_INSTANCE        *p_inst        = &g_instance;
    WAITING_CONDITION        st_condition = {0};
    RET_API        ret_sts     = RET_EV_NONE;
    u_int32        ul_index     = (u_int32)event_id - 1;

    /* Parameter check */
    if ((pl_val != NULL) &&
            (ul_index < MAX_PB_EVENTS)) {
        p_event_open = p_inst->p_handle_table[ul_index];

        /* If the specified event ID is registered in the table, */
        if (p_event_open != NULL) {
            /* Set Wait Mode and Mask Value to Parameter Blk */
            st_condition.us_mode = EVWAIT_ANYSET;
            st_condition.ul_mask = ul_mask;

            /* Call the event wait processing */
            ret_sts = WaitProc(p_event_open, &st_condition, ul_mill_sec_time);
            if (ret_sts == RET_NORMAL) {
                *pl_val = st_condition.l_last_val;
            }
        }
    }

    return ret_sts;
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : LookupEvent()
 * ABSTRACT  : Event value reading process
 * NOTE      : Read the event value of the specified event ID.
 * ARGUMENT  : EventID    event_id            Specify the event ID to read the event value from.
 *           : int32    *iEventVal        Pointer to the read event value storage area
 * RETURN    : RET_API    RET_NORMAL        Normal completion
 *           :             RET_EV_NONE        Specified event does not exist
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
LookupEvent(EventID event_id, int32* pl_event_val) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    PB_EVENT_OPEN_HANDLE    *p_event_open = NULL;
    PB_EVENT_INSTANCE        *p_inst = &g_instance;
    u_int32        ul_index     = (u_int32)event_id - 1;
    RET_API        ret_sts     = RET_EV_NONE;

    /* Parameter check */
    if ((pl_event_val != NULL) &&
            (ul_index < MAX_PB_EVENTS)) {
        p_event_open = p_inst->p_handle_table[ul_index];

        /* When the specified event ID is already registered in the table */
        if (p_event_open != NULL) {
            EventLockMutex(p_event_open);
            /* Store the current event value. */
            *pl_event_val = static_cast<int32>(p_event_open->p_sys_event->l_event_val);
            EventUnlockMutex(p_event_open);
            ret_sts = RET_NORMAL;
        }
    }

    return ret_sts;
}
// LCOV_EXCL_STOP

/**
 * @brief
 *    Delete the event
 *
 *   Delete the event with the specified event ID.
 *
 * @param[in]    event_id        Specify the event ID for which the event value is to be set.
 *
 * @return    RET_NORMAL        Normal completion<br>
 *            RET_EV_NONE        Specified event does not exist
 */
RET_API _pb_DeleteEvent(EventID event_id) {   // NOLINT(readability/nolint)
    PB_EVENT_OPEN_HANDLE    *p_event_open = NULL;
    PB_EVENT_INSTANCE       *p_inst         = &g_instance;
    RET_API         ret_api = RET_EV_NONE;
    u_int32         index = static_cast<u_int32>(event_id) - 1;
    EV_ERR          ev_err;
    RET_API         l_ret_api;

    /* Parameter check */
    if (index < MAX_PB_EVENTS) {
        p_event_open = p_inst->p_handle_table[index];
        /* When the specified event ID is registered in the table */
        if (p_event_open != NULL) {
            ret_api = RET_NORMAL;
        }
    }

    /* Parameter normal */
    if (ret_api == RET_NORMAL) {
        l_ret_api = _pb_SemLock(p_inst->id_event_table_sem);
        if (l_ret_api != RET_NORMAL) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_SemLock failed");
        }

        /* When no one references in the same process */
        if ((p_event_open->l_thread_ref - 1) <= 0) {
            /* Delete event flag */
            ev_err = EV_destroy_flag(p_event_open->p_sys_event->st_condition[0].flag_id[g_my_proc_cnt]);
            /* When initialization fails */
            if (ev_err == EV_OK) {
                p_event_open->p_sys_event->st_condition[0].flag_id[g_my_proc_cnt] = 0;
            } else {
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, \
                    "EV_destroy_flag ERROR!! [ev_err=%d, flag_id=0x%x]", \
                    ev_err, p_event_open->p_sys_event->st_condition[0].flag_id[g_my_proc_cnt]);

                ret_api = RET_ERROR;
            }
        }

        /* When the event flag is deleted successfully */
        if (ret_api == RET_NORMAL) {
            /* Reduce the number of event references in the same process */
            p_event_open->l_thread_ref--;

            /* When no one references in the same process */
            if (p_event_open->l_thread_ref <= 0) {
                /* Reduce the number of event references in the system */
                p_event_open->p_sys_event->l_process_ref--;
            }

            /* When no one references in the system */
            if (p_event_open->p_sys_event->l_process_ref <= 0) {
                /* Initialization of the target area */
                FreeEventTable(p_inst->p_event_table, index);
            }

            /* If no one references in the same process, release the resource here */
            if (p_event_open->l_thread_ref <= 0) {
                /* Exclusive deletion for the target event */
                EventDeleteMutex(p_event_open);

                /* Open the heap area storing the target event. */
                /*  */
                PbProcessHeapFree(0, p_inst->p_handle_table[index]);
                p_inst->p_handle_table[index] = NULL;
            }
        }

        _pb_SemUnlock(p_inst->id_event_table_sem);
    }

    return ret_api;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : ResetEvent()
 * ABSTRACT  : Event Clear
 * NOTE      : Specified event clear processing
 * ARGUMENT  : EventID    event_id            Event ID to reset
 * RETURN    : RET_API    RET_NORMAL        Normal completion
 *           :             RET_EV_NONE        ABEND
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
ResetEvent(EventID event_id) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    PB_EVENT_OPEN_HANDLE    *p_event_open = NULL;
    PB_EVENT_INSTANCE       *p_inst       = &g_instance;
    RET_API        ret_sts   = RET_EV_NONE;
    u_int32        ul_index  = (u_int32)event_id - 1;

    /* Parameter check */
    if (ul_index < MAX_PB_EVENTS) {
        p_event_open = p_inst->p_handle_table[ul_index];

        /* When the specified event ID is already registered in the table */
        if (p_event_open != NULL) {
            EventLockMutex(p_event_open);

            /* Clear the event value */
            p_event_open->p_sys_event->l_event_val = p_event_open->p_sys_event->l_reset_data;

            EventUnlockMutex(p_event_open);
            ret_sts = RET_NORMAL;
        }
    }

    return ret_sts;
}
// LCOV_EXCL_STOP

/* Private functions. */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : SetProc()
 * ABSTRACT  : General Event Configuration Processing
 * NOTE      : Sets the event according to the specified event setting method.
 * ARGUMENT  : PB_EVENT_OPEN_HANDLE*    p_event_open    Pointer to manage event waiting for the event TBL
 *           : int32                    i_mode        Event setting method
 *           : int32                    iVal        Event setting value
 *           : int32*                    ipVal        Pointer to the area to store the pre-event value
 * RETURN    : RET_API    RET_NORMAL        Normal completion
 *           :             RET_ERROR
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static RET_API
SetProc(PB_EVENT_OPEN_HANDLE* p_event_open, int32 i_mode, int32 l_val, int32* lpVal) {
    RET_API    ret_sts         = RET_NORMAL;
    int32    lTempEventData    = 0;
    int32     lTestValue       = 0;
    BOOL    bCastCondFlag      = FALSE;

    EventLockMutex(p_event_open);

    /* Get current event value */
    lTempEventData = p_event_open->p_sys_event->l_event_val;
    *lpVal           = p_event_open->p_sys_event->l_event_val;                /* Set the value before the event operation */

    /* Switch Processing by event configuration mode */
    switch (i_mode) {  // LCOV_EXCL_BR_LINE 200:only the first two cases will be called
        case EVSET_ABSOLUTE:                     /* In absolute mode */ {
            /* Updating event values with specified values */
            lTempEventData = l_val;
            break;
        }
        case EVSET_RELATE:                         /* In relative setting mode */ {
            lTestValue = lTempEventData + l_val;
            /* Exceeding representable event value */
            if ((l_val > 0) && (lTempEventData > lTestValue)) {
                ret_sts = RET_EV_MAX;
            }
            /* Below representable event value */
            if ((l_val < 0) && (lTempEventData < lTestValue)) {
                ret_sts = RET_EV_MIN;
            }
            /* Normal range */
            if (ret_sts == RET_NORMAL) {
                /* Add specified value to event value */
                lTempEventData += l_val;
            }
            break;
        }
        case EVSET_AND: {  // LCOV_EXCL_BR_LINE 200: i_mode cannot be this value
            // LCOV_EXCL_START 200: i_mode cannot be this value
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            lTempEventData &= ((u_int32)l_val);    /* Logical AND of the event value and the specified value */
            break;
            // LCOV_EXCL_STOP
        }
        case EVSET_OR: {  // LCOV_EXCL_BR_LINE 200: i_mode cannot be this value
            // LCOV_EXCL_START 200: i_mode cannot be this value
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            lTempEventData |= ((u_int32)l_val);    /* Logical AND of the event value and the specified value */
            break;
            // LCOV_EXCL_STOP
        }
        default:  /* Event setting mode error */  // LCOV_EXCL_BR_LINE 200: i_mode cannot be this value
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            ret_sts = RET_ERRPARAM;    // LCOV_EXCL_LINE 200: i_mode cannot be this value
    }

    /* When the manual reset function is enabled */

    if (ret_sts == RET_NORMAL) {
        /* When the manual reset function is enabled */
        if (p_event_open->p_sys_event->uc_manual_reset == _CWORD64_EVENT_MANUALRESET_ON) {
            /* Set event value */
            p_event_open->p_sys_event->l_event_val = lTempEventData;
        }

        /* Loop for the maximum number of waiting threads per event and check the condition of event wait processing of the state TBL. */
        for (DWORD wcn = 0; wcn < MAX_PB_EVENT_WAIT_THREADS; wcn++) {
            /* If the event wait flag is waiting, */
            if (p_event_open->p_sys_event->st_condition[wcn].uc_waiting == TRUE) {
                /* Check if event wait conditions are met */
                BOOL bret = CheckCondition(p_event_open->p_sys_event, wcn, lTempEventData);
                /* If the event wait conditions are met, */
                if (bret == TRUE) {
                    bCastCondFlag = TRUE;
                    /* Save the event value at the time of SetEvent issuance (at the time of WaitEvent return). */
                    p_event_open->p_sys_event->st_condition[wcn].l_last_val = lTempEventData;

                    /* Processing to prevent concurrent SetEvent from more than one threads for a single event which is in WAIT state */
                    /* Set WAIT status to wait-canceled */
                    p_event_open->p_sys_event->st_condition[wcn].uc_waiting = FALSE;
                    /* Setting the default min value for event */
                    p_event_open->p_sys_event->st_condition[wcn].l_min_val = MIN_EVENT_VAL;
                    /* Setting the default max event */
                    p_event_open->p_sys_event->st_condition[wcn].l_max_val = MAX_EVENT_VAL;

                    /* Signal issuance */
                    (void)EventSendSignal(p_event_open, static_cast<int>(wcn));
                }
            }
        }

        /* When the manual reset function is disabled */
        if (p_event_open->p_sys_event->uc_manual_reset != _CWORD64_EVENT_MANUALRESET_ON) {
            /* If no one has issued the event */
            if (bCastCondFlag == FALSE) {
                /* Set event value */
                p_event_open->p_sys_event->l_event_val = lTempEventData;
            } else {
                /* If issued event */
                /* Reset event value */
                p_event_open->p_sys_event->l_event_val = p_event_open->p_sys_event->l_reset_data;
            }
        }
    }

    EventUnlockMutex(p_event_open);

    return ret_sts;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : WaitProc()
 * ABSTRACT  : Generic Event Wait Processing
 * NOTE      : Wait for an event according to the wait queue of the specified event.
 * ARGUMENT  : PB_EVENT_OPEN_HANDLE*    p_event_open    Pointer to TBL which is managed waiting for events
 *           : WAITING_CONDITION*        st_condition    Pointer to the event wait condition setting parameter
 * RETURN    : RET_API    RET_NORMAL        Normal completion
 *           :             RET_ERROR        The maximum number of waits has been exceeded, or a parameter error has occurred.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static RET_API
WaitProc(PB_EVENT_OPEN_HANDLE* p_event_open, WAITING_CONDITION* st_condition, u_int32 ul_mill_sec_time) {
    RET_API        ret_sts = RET_ERROR;
    u_int32        ul_wcn = 0;

    /* Get semaphore for event table */
    EventLockMutex(p_event_open);

    /* Loop for the maximum number of waiting threads per event and retrieve free area of state TBL */
    for (ul_wcn = 0; ul_wcn < MAX_PB_EVENT_WAIT_THREADS; ul_wcn++) {
        if ((p_event_open->p_sys_event->st_condition[ul_wcn].uc_use_flag == FALSE) && \
                (p_event_open->p_sys_event->st_condition[ul_wcn].uc_waiting == FALSE)) {
            /* For the unused state TBL */
            /* If the event wait flag is released, */
            /* finish searching when free area is found */
            ret_sts = RET_NORMAL;
            break;
        }
    }

    /* If there is free space in the state TBL */
    if (ret_sts == RET_NORMAL) {
        /* Set wait rule for free space of state TBL */
        /* Determine the wait rule */
        switch (st_condition->us_mode) {  // LCOV_EXCL_BR_LINE 200: can not be EVWAIT_ALLCLR and EVWAIT_ANYSET
            case EVWAIT_VAL:            /* For range waiting */
            {
                /* Set event monitoring mode */
                p_event_open->p_sys_event->st_condition[ul_wcn].us_mode  = st_condition->us_mode;
                /* Set the minimum value for establishing an event */
                p_event_open->p_sys_event->st_condition[ul_wcn].l_min_val = st_condition->l_min_val;
                /* Set the maximum value for establishing an event */
                p_event_open->p_sys_event->st_condition[ul_wcn].l_max_val = st_condition->l_max_val;
                break;
            }
            case EVWAIT_ALLCLR:  /* If waiting for the specified bit to be cleared */  // LCOV_EXCL_BR_LINE 200: can not be EVWAIT_ALLCLR
            case EVWAIT_ANYSET:  /* If waiting for the specified bit to be set */  // LCOV_EXCL_BR_LINE 200: can not be EVWAIT_ANYSET
             {
                // LCOV_EXCL_START 200: can not be EVWAIT_ANYSET and EVWAIT_ALLCLR
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                /* Set event monitoring mode */
                p_event_open->p_sys_event->st_condition[ul_wcn].us_mode = st_condition->us_mode;
                /* Set event wait mask value */
                p_event_open->p_sys_event->st_condition[ul_wcn].ul_mask = st_condition->ul_mask;
                break;
                // LCOV_EXCL_STOP
            }
            default:
                ret_sts = RET_ERROR;
        }
    }

    /* When the specified mode is normal */
    if (ret_sts == RET_NORMAL) {
        /* Check if event wait conditions are met */
        BOOL bret = CheckCondition(p_event_open->p_sys_event, ul_wcn, p_event_open->p_sys_event->l_event_val);

        /* Target event received */
        if (bret == TRUE) {
            /* Set the received event value */
            st_condition->l_last_val = p_event_open->p_sys_event->l_event_val;
            /* Since it does not wait for an event, set the initial value to the state TBL. */
            p_event_open->p_sys_event->st_condition[ul_wcn].uc_waiting = FALSE;
            p_event_open->p_sys_event->st_condition[ul_wcn].uc_use_flag = FALSE;
            /* Set the default minimum value for the event */
            p_event_open->p_sys_event->st_condition[ul_wcn].l_min_val = MIN_EVENT_VAL;
            /* Set the default maximum value for the event */
            p_event_open->p_sys_event->st_condition[ul_wcn].l_max_val = MAX_EVENT_VAL;

            /* When the manual reset function is disabled */
            if (p_event_open->p_sys_event->uc_manual_reset != _CWORD64_EVENT_MANUALRESET_ON) {
                /* Initialize event values */
                p_event_open->p_sys_event->l_event_val = p_event_open->p_sys_event->l_reset_data;
            }
        } else {
            /* When no event is received */
            /* Set event wait state in free area of state TBL */
            /* Set event wait flag to waiting */
            p_event_open->p_sys_event->st_condition[ul_wcn].uc_waiting = TRUE;
            /* Set table usage flag in use */
            p_event_open->p_sys_event->st_condition[ul_wcn].uc_use_flag = TRUE;

            /* Perform event wait */
            ret_sts = EventWaitForSignal(p_event_open, ul_wcn, ul_mill_sec_time);
            /* Set event wait flag to unused */
            p_event_open->p_sys_event->st_condition[ul_wcn].uc_waiting = FALSE;
            /* Set table usage flag to unused */
            p_event_open->p_sys_event->st_condition[ul_wcn].uc_use_flag = FALSE;
            /* Set the default minimum value for the event */
            p_event_open->p_sys_event->st_condition[ul_wcn].l_min_val = MIN_EVENT_VAL;
            /* Setting the default maximum value for the event */
            p_event_open->p_sys_event->st_condition[ul_wcn].l_max_val = MAX_EVENT_VAL;
            /* Set event return value */
            st_condition->l_last_val = p_event_open->p_sys_event->st_condition[ul_wcn].l_last_val;
        }
    }

    /* Release semaphore for event table */
    EventUnlockMutex(p_event_open);

    return ret_sts;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : FindEventTable()
 * ABSTRACT  : Event Table Search Processing
 * NOTE      : Search the event table for the specified event name and return the index number 
 *           : of the event if it has been already registerd.
 * ARGUMENT  : PB_EVENT    *p_event_table    Pointer to the start of event table array in the shared memory
 *           : TCHAR        *ptcEventName    Event name to search
 *           : u_int32*        puc_index        storage area for the index number of the specified event table
 * RETURN    : BOOL            FALSE            No specified event
 *           :                 TRUE            Specified Event Yes
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static BOOL
FindEventTable(PB_EVENT* p_event_table, TCHAR* ptcEventName, u_int32* puc_index) {
    u_int32    ul_index = 0;
    BOOL       bret = FALSE;

    for (ul_index = 0; ul_index < MAX_PB_EVENTS; ul_index++) {
        if (_tcscmp(p_event_table[ul_index].event_name, ptcEventName) == 0) {
            /* Save target index */
            *puc_index = ul_index;
            bret = TRUE;
            break;
        }
    }

    return bret;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : AllocNewEventTable()
 * ABSTRACT  : Event table allocation processing
 * NOTE      : Search the event table pointed to from the beginning for an area in
 *           : which the event name is not registered, and returns its index number.
 *           : The event table structure is allocated as an array in shared memory.
 *           : One element of the array is the event table structure, and its index
 *           : plus one is used as the event ID.
 *           : Whether the event table structure is in use or unused is determined 
 *           : by whether the event name is set or not.
 *           : Note: Since the Mutex part inside this function was deleted to
 *           : fix a bug caused by Mutex leak, use Mutex around this function from the
 *           : outside before using this function.
 * ARGUMENT  : PB_EVENT    *p_event_table    Start pointer of the event table array in shared memory
 *           : TCHAR        *name            Event name to reserve table            Note: Currently unused.
 *           : HANDLE        hMutex            Mutex handle for event table    Note: Currently unused.
 * RETURN    : DWORD        other than -1       Index number of the allocted event table
 *           :                 -1                There is no free space in the event table.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static BOOL
AllocNewEventTable(PB_EVENT* p_event_table, u_int32* puc_index) {
    u_int32    ul_index = 0;
    BOOL       bret = FALSE;

    for (ul_index = 0; ul_index < MAX_PB_EVENTS; ul_index++) {
        if (p_event_table[ul_index].event_name[0] == __TEXT('\0')) {
            *puc_index = ul_index;
            bret = TRUE;
            break;
        }
    }

    return bret;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : FreeEventTable()
 * ABSTRACT  : Event table release processing
 * NOTE      : Initialize the event name and event value of the index number
 *           : of the specified event table to make them free.
 * ARGUMENT  : PB_EVENT    *p_event_table    Start pointer of the event table array in shared memory
 *           : int            index            Index number of the event table to release
 *           : HANDLE        hMutex            Mutex handle for event table
 * RETURN    : None
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static void
FreeEventTable(PB_EVENT* p_event_table, int index) {
    p_event_table[index].event_name[0] = __TEXT('\0');
    p_event_table[index].l_event_val = 0;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : CheckCondition()
 * ABSTRACT  : Event condition determination processing
 * NOTE      : Check whether the event value of the specified event table is 
 *           : satisfied as an event wait condition.
 * ARGUMENT  : PB_EVENT    *p_event_table    Start pointer of the event table array in shared memory
 *           : DWORD        wcn                Index number of the event table to be checked
 * RETURN    : BOOL            TRUE            Condition satisfied
 *           :                FALSE            Condition not met
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static BOOL CheckCondition(PB_EVENT* p_sys_event, DWORD wcn, int32 l_event_data) {
    BOOL    bret = FALSE;

    if (p_sys_event != NULL) {  // LCOV_EXCL_BR_LINE 6: p_sys_event can not be NULL
        /* Determine the wait mode of the event state TBL. */
        switch (p_sys_event->st_condition[wcn].us_mode) {  // LCOV_EXCL_BR_LINE 200: EVWAIT_ALLCLR and EVWAIT_ANYSET will not be called  // NOLINT(whitespace/line_length)
            case EVWAIT_VAL:        /* For value range wait */
            {
                /* Check whether the event value is within the condition satisfied range */
                if ((l_event_data >= p_sys_event->st_condition[wcn].l_min_val) &&
                        (l_event_data <= p_sys_event->st_condition[wcn].l_max_val)) {
                    bret = TRUE;
                }
                break;
            }
            case EVWAIT_ALLCLR:  /* When waiting for all specified bits to be cleared */  // LCOV_EXCL_BR_LINE 200: can not be EVWAIT_ALLCLR  // NOLINT(whitespace/line_length)
            {
                // LCOV_EXCL_START 200: can not be EVWAIT_ALLCLR
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                if ((((u_int32)l_event_data) & p_sys_event->st_condition[wcn].ul_mask) == EVENT_BIT_ZERO) {
                    bret = TRUE;
                }
                break;
                // LCOV_EXCL_STOP
            }
            case EVWAIT_ANYSET:  /* If the specified bit is waiting to set any bits */  // LCOV_EXCL_BR_LINE 200: can not be EVWAIT_ANYSET  // NOLINT(whitespace/line_length)
            {
                // LCOV_EXCL_START 200: can not be EVWAIT_ALLCLR
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                if ((((u_int32)l_event_data) & p_sys_event->st_condition[wcn].ul_mask) != EVENT_BIT_ZERO) {
                    bret = TRUE;
                }
                break;
                // LCOV_EXCL_STOP
            }
            default:                /* If the wait mode is out of range, */
                break;                /* return with error */
        }
    }

    return bret;
}

/**
 * @brief
 *   Event generation processing
 *
 * @param[in]   u_int8  uc_manual_reset
 * @param[in]   int32   l_init_data
 * @param[in]    char    *cp_event_name    Pointer to the names of event to be generated (NULL termination string)
 * @return EventID    Non-zero    Event ID created<br>
 *                    0        Event generation error
 */
static EventID    EventCreateNewEventInSystem(u_int8 uc_manual_reset, int32 l_init_data, TCHAR* p_event_name) {
    PB_EVENT_OPEN_HANDLE    *p_event_open = NULL;
    PB_EVENT_INSTANCE       *p_inst         = &g_instance;
    PB_EVENT                *p_sys_event    = NULL;
    EventID     ret_event_id = 0;
    u_int32     ul_index     = 0;
    BOOL        bret         = FALSE;
    EV_ERR      ev_err;

    /* Parameter check */
    if (p_event_name != NULL) {  // LCOV_EXCL_BR_LINE 6: p_event_name can not be NULL
        /* Parameter normal */
        /* Get the index number of the newly created event table */
        bret = AllocNewEventTable(p_inst->p_event_table, &ul_index);

        /* When there is no free space */
        if (bret == FALSE) {
            /* Error log output */
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_CWORD64_api.dll:_pb_CreateEvent : AllocNewEventTable Full... \r\n");
        }
    }

    /* When there is free space */
    if (bret != FALSE) {
        /* allocate event table to generate from heap */
        /*  */
        p_event_open = reinterpret_cast<PB_EVENT_OPEN_HANDLE*>(PbProcessHeapAlloc(0, sizeof(PB_EVENT_OPEN_HANDLE)));  // LCOV_EXCL_BR_LINE 200: can not be NULL  // NOLINT(whitespace/line_length)

        /* Failure in allocating heap area */
        if (p_event_open == NULL) {  // LCOV_EXCL_BR_LINE 200: can not be NULL
            // LCOV_EXCL_START 200: can not be NULL
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            bret = FALSE;
            /* Error log output */
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                    "_CWORD64_api.dll:_pb_CreateEvent : CreateHeap ... GetAddr[0x%08x], size[%ld] \r\n",
                    p_event_open, sizeof(PB_EVENT_OPEN_HANDLE));
            // LCOV_EXCL_STOP
        }
    }

    /* When the heap area can be allocated */
    if (bret != FALSE) {
        /* Initialization of generated event management information */
        p_event_open->index        = ul_index;
        p_event_open->p_sys_event    = &p_inst->p_event_table[ul_index];
        p_event_open->l_thread_ref    = 1;

        /* Initialization processing of event information storage area */
        p_sys_event = p_event_open->p_sys_event;
        _tcscpy(p_sys_event->event_name, p_event_name);        /*    Event name registration                */
        p_sys_event->l_event_val     = l_init_data;            /*    Default setting                    */
        for (u_int32 ul_wcn = 0; ul_wcn < MAX_PB_EVENT_WAIT_THREADS; ul_wcn++) {
            p_sys_event->st_condition[ul_wcn].uc_use_flag = FALSE;
            p_sys_event->st_condition[ul_wcn].uc_waiting = FALSE;
            p_sys_event->st_condition[ul_wcn].us_mode    = 0;
            p_sys_event->st_condition[ul_wcn].l_min_val    = MIN_EVENT_VAL;
            p_sys_event->st_condition[ul_wcn].l_max_val    = MAX_EVENT_VAL;

            /* Create Event Flag */
            ev_err = EV_create_flag_auto_id(&(p_sys_event->st_condition[0].flag_id[g_my_proc_cnt]));  // LCOV_EXCL_BR_LINE 200: no branch  // NOLINT(whitespace/line_length)
            if (ev_err != EV_OK) /* When initialization fails */ {
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, \
                    "EV_create_flag_auto_id ERROR!! [ev_err=%d, flag_id=0x%x", \
                    ev_err, p_sys_event->st_condition[0].flag_id[g_my_proc_cnt]);

                /* Release heap space */
                PbProcessHeapFree(0, p_event_open);  // LCOV_EXCL_BR_LINE 200: no branch

                ret_event_id = 0;
                bret = FALSE;
            }
        }

        if (bret != FALSE) {
            p_sys_event->l_process_ref      = 1;                    /* Set the number of references to this event.    */
            p_sys_event->l_reset_data     = l_init_data;            /* Default setting                    */
            p_sys_event->uc_manual_reset = uc_manual_reset;        /* Setting for a manual reset            */

            /* Create an event table Mutex and set it in the event table. */
            bret = EventCreateMutex(p_event_open);
            /* If generating fails, reset is executed. */
            if (bret == FALSE) {
                FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "_CWORD64_api.dll:%s:LINE %d\r\n", LTEXT(__FILE__), __LINE__); \
                FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "CreateMutex Err ... Event Name[%s]\r\n", p_event_name);
                _pb_Exit();
            }

            /* Register event table with event instance */
            p_inst->p_handle_table[ul_index] = p_event_open;
            ret_event_id = ul_index + 1;
        }
    }

    return ret_event_id;
}

/**
 * @brief
 *   Event generation processing
 *
 * @param[in]    char    *cpEventName    Pointer to name of the event to be generated (NULL termination string)
 * @return EventID    Non-zero    Event ID created<br>
 *                    0        Event generation error
 */
static EventID    EventCreateNewEventInProcess(u_int32 index) {
    PB_EVENT_OPEN_HANDLE    *p_event_open = NULL;
    PB_EVENT_INSTANCE       *p_inst = &g_instance;
    EventID    ret_event_id = 0;
    EV_ERR     ev_err;

    /* Already created in the same process    */
    if (p_inst->p_handle_table[index] != NULL) {  // LCOV_EXCL_BR_LINE 200: can not be NULL
        /* When the number of event references in the same process is less than the upper limit */
        if (p_inst->p_handle_table[index]->l_thread_ref < _CWORD64_EVENT_MAXOPEN_IN_PROCESS) {
            /* Increase the number of thread references */
            (p_inst->p_handle_table[index]->l_thread_ref)++;
            ret_event_id = index + 1;
        } else {
            /* When the number of event references in the same process is the upper limit */
            /* Error log output */
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                                       "_pb_CreateEvent Err ... Event Max In Process : EventName[%s]\r\n",
                               p_inst->p_handle_table[index]->p_sys_event->event_name);
        }
    } else {
        /* Creating for the first time in the process */
        /* Checking the upper limit of the reference count of the same event in the system */
        if (p_inst->p_event_table[index].l_process_ref < _CWORD64_EVENT_MAXOPEN_IN_SYSTEM) {
            /* Allocate event table to generate from heap */
            /*  */
            p_event_open = reinterpret_cast<PB_EVENT_OPEN_HANDLE*>(PbProcessHeapAlloc(0, \
                sizeof(PB_EVENT_OPEN_HANDLE)));

            /* Failure in allocating heap area */
            if (p_event_open == NULL) {
                /* Error log output */
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, \
                        "_CWORD64_api.dll:_pb_CreateEvent : CreateHeap ... GetAddr[0x%08x], size[%ld] \r\n", \
                        p_event_open, sizeof(PB_EVENT_OPEN_HANDLE));
            }
        } else {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                                       "_pb_CreateEvent Err ... Event Max In sYSTEM : EventName[%s]\r\n",
                               p_inst->p_handle_table[index]->p_sys_event->event_name);
        }

        /* When heap allocation is successful */
        if (p_event_open != NULL) {
            /* When it is not created in the same process, set each data. */
            /* Set the index to which the event name is registered */
            p_event_open->index       = index;
            /* Initialize the reference count of the threads referencing this event in the same process. */
            p_event_open->l_thread_ref = 1;
            /* Set event instance start address */
            p_event_open->p_sys_event  = &p_inst->p_event_table[index];
            /* Add the reference count of the process referencing this event in the system. */
            p_event_open->p_sys_event->l_process_ref++;

            /* Create an event flag */
            ev_err = EV_create_flag_auto_id(&(p_event_open->p_sys_event->st_condition[0].flag_id[g_my_proc_cnt]));
            if (ev_err != EV_OK) /* When initialization fails */ {
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, \
                    "EV_create_flag_auto_id ERROR!! [ev_err=%d, flag_id=0x%x]",
                               ev_err, p_event_open->p_sys_event->st_condition[0].flag_id[g_my_proc_cnt]);

                /* Release heap space */
                PbProcessHeapFree(0, p_event_open);

                ret_event_id = 0;
            } else {
                /* Even if event information already exists in the system, the Mutex is created for each process. */
                (void)_pb_CreateMutex(NULL, FALSE, p_event_open->p_sys_event->name_of_mutex);

                p_inst->p_handle_table[index] = p_event_open;   /* Register event tables with event instance */
                ret_event_id = index + 1;
            }
        }
    }

    return ret_event_id;
}

/**
 * @brief
 *   Send the signal
 *
 *   Sends the specified event signal.
 *
 *   - Sending signals in the CLS event library
 *
 * @param[in]    PB_EVENT_OPEN_HANDLE *p_evet_open_handle
 * @param[in]    u_int32    ul_index
 *
 * @return RET_NORMAL        Normal completion<br>
 *           RET_ERROR        Other errors
 */
static RET_API EventSendSignal(PB_EVENT_OPEN_HANDLE *p_evet_open_handle, u_int32 ul_index) {
    RET_API             ret_api = RET_NORMAL;
    PB_EVENT_INSTANCE   *p_inst = &g_instance;
    EV_ERR    ev_err;
    int32     i;
    BOOL      errFlag = TRUE;

    if ((p_evet_open_handle != NULL) &&
            (ul_index < MAX_PB_EVENT_WAIT_THREADS)) {
        for (i = 0; i <= p_inst->p_event_table->proc_cnt; i++) {
            /* Signal issuance */
            if (p_evet_open_handle->p_sys_event->st_condition[ul_index].flag_id[i] != 0) {
                ev_err = EV_set_flag(p_evet_open_handle->p_sys_event->st_condition[ul_index].flag_id[i], 1);
                if (ev_err == EV_OK) {
                    errFlag = FALSE;
                }
            }
        }
    }

    if (errFlag == TRUE) /* Event issuance NG */ {
        ret_api = RET_ERROR;
    }

    return ret_api;
}

/**
 * @brief
 *   Wait for the signal
 *
 *   Wait until the specified signal is received. Timeout can be specified (ms).
 *   When this API is called, the semaphore for the event element must be acquired.
 *
 *   Receive a signal in the CLS event library.
 *
 * @param[in]    *p_evet_open_handle
 * @param[in]    ul_index
 * @param[in]    ul_mill_sec_time
 *
 * @return    RET_NORMAL        Normal completion<br>
 *            RET_ERRTIMEOUT    Timeout End<br>
 *            RET_ERROR        Other errors
 */
static RET_API EventWaitForSignal(PB_EVENT_OPEN_HANDLE *p_evet_open_handle, u_int32 ul_index, \
    u_int32 ul_mill_sec_time) {
    RET_API    ret_api = RET_ERRTIMEOUT;
    EV_ERR     ev_err;
    EV_Flag    ev_flag;
    u_int32    timeOutCnt = 0;

    /* Parameter check */
    if ((p_evet_open_handle != NULL) && (ul_index < MAX_PB_EVENT_WAIT_THREADS)) {  // LCOV_EXCL_BR_LINE 6: param can not be invalid  // NOLINT(whitespace/line_length)
        /* Release semaphore for event table */
        EventUnlockMutex(p_evet_open_handle);  // LCOV_EXCL_BR_LINE 200: no branch

        /* Distribute processing by timeout period */
        /* To check the event occurrence status */
        if (ul_mill_sec_time == 0) {
            /* Untreated */
        } else if (ul_mill_sec_time == INFINITE) {
            /* Without timeout */
            ret_api = RET_NORMAL;

            FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, "EV_wait_flag CALL [flag_id=0x%x]",
                           p_evet_open_handle->p_sys_event->st_condition[ul_index].flag_id[g_my_proc_cnt]);

            /* Wait for event flag */
            ev_err = EV_wait_flag(p_evet_open_handle->p_sys_event->st_condition[ul_index].flag_id[g_my_proc_cnt], \
                &ev_flag);  // LCOV_EXCL_BR_LINE 200: no branch
            if (ev_err != EV_OK) {
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, \
                    "EV_wait_flag ERROR!! [ev_err=%d]", ev_err);
                ret_api = RET_ERROR;
            } else {
                FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, \
                    "EV_wait_flag RETURN [ev_err=%d]", ev_err);
            }
        } else {
            /* When the timeout period is specified */
            FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, "EV_get_flag CALL [flag_id=0x%x]",
                           p_evet_open_handle->p_sys_event->st_condition[ul_index].flag_id[g_my_proc_cnt]);

            while (1) {
                /* Get elag event */
                ev_err = EV_get_flag(p_evet_open_handle->p_sys_event->st_condition[ul_index].flag_id[g_my_proc_cnt], \
                    &ev_flag);  // LCOV_EXCL_BR_LINE 200: no branch
                if (ev_err == EV_OK) {
                    if (ev_flag.flagID == EV_NO_ID) {
                        timeOutCnt++;
                        if (timeOutCnt <= ul_mill_sec_time) {
                            usleep(1000);  // LCOV_EXCL_BR_LINE 200: no branch
                        } else {
                            break; /* Timeout error */
                        }
                    } else {
                        ret_api = RET_NORMAL;
                        break;
                    }
                } else {
                    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, \
                        "EV_get_flag ERROR!! [ev_err=%d]", ev_err);
                    ret_api = RET_ERROR;
                    break;
                }
            }

            FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, "EV_get_flag BREAK [ret_api=%d]", ret_api);
        }

        /* Get event table semaphore */
        EventLockMutex(p_evet_open_handle);  // LCOV_EXCL_BR_LINE 200: no branch
    } else {
        /* Parameter error */
        ret_api = RET_ERROR;
    }

    return ret_api;
}

/**
 * @brief
 *   Create the mutex for event
 *
 * @param[in]    *p_evet_open_handle
 *
 * @return    TRUE        Normal completion<br>
 *            FALSE        ABENDs
 */
static BOOL EventCreateMutex(PB_EVENT_OPEN_HANDLE *p_evet_open_handle) {
    static u_int8    idx = 0;
    uint32_t    ulPid;    /* Process ID */
    BOOL        bret   = FALSE;
    TCHAR       name[NAME_MAX];
    HANDLE      handle;

    /* Parameter check */
    if (p_evet_open_handle != NULL) {  // LCOV_EXCL_BR_LINE 6: p_evet_open_handle can not be NULL
        ulPid = (uint32_t)getpid();

        wsprintf(name, __TEXT("POS_BASE_EVENT_MUTEX%05d_p%d"), idx, ulPid);

        /****************************************/
        /* Create Mutex                         */
        /****************************************/
        handle = _pb_CreateMutex(NULL, FALSE, name);
        if (handle != NULL) {
            _tcscpy(p_evet_open_handle->p_sys_event->name_of_mutex, name);
            idx++;
            bret = TRUE;
        }
    }

    /* When mutex processing fails */
    if (bret != TRUE) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "bret ERROR [bret:%d]", bret);
    }

    return bret;
}

/**
 * @brief
 *   Lock the mutex for event
 *
 * @param[in] *p_evet_open_handle
 */
static void EventLockMutex(PB_EVENT_OPEN_HANDLE *p_evet_open_handle) {
    DWORD     lret = WAIT_FAILED;
    HANDLE    handle;

    if (p_evet_open_handle != NULL) {  // LCOV_EXCL_BR_LINE 6: p_evet_open_handle can not be NULL
        /* Get handle from Mutex name */
        handle = _pb_CreateMutex(NULL, FALSE, p_evet_open_handle->p_sys_event->name_of_mutex);

        /****************************************/
        /* Get Mutex                            */
        /****************************************/
        lret = PbMutexLock(handle, INFINITE);

        /* Cancel by deleting the generated portion when a handle was acquired */
        (void)PbDeleteMutex(handle);
    }

    /* When mutex processing fails */
    if (lret != WAIT_OBJECT_0) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "lret ERROR [lret:%lu]", lret);
    }

    return;
}

/**
 * @brief
 *   Unlock the mutex for event
 *
 * @param[in] *p_evet_open_handle
 */
static void EventUnlockMutex(PB_EVENT_OPEN_HANDLE *p_evet_open_handle) {
    BOOL      bret = FALSE;
    HANDLE    handle;

    if (p_evet_open_handle != NULL) {  // LCOV_EXCL_BR_LINE 6: p_evet_open_handle can not be NULL
        /* Get handle from Mutex name */
        handle = _pb_CreateMutex(NULL, FALSE, p_evet_open_handle->p_sys_event->name_of_mutex);

        /****************************************/
        /* Release Mutex                        */
        /****************************************/
        bret = PbMutexUnlock(handle);

        /* Cancel by deleting the generated portion when a handle was acquired */
        (void)PbDeleteMutex(handle);
    }

    /* When mutex processing fails */
    if (bret != TRUE) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "bret ERROR [bret:%d]", bret);
    }

    return;
}

/**
 * @brief
 *   delete the mutex for event
 *
 * @param[in] *p_evet_open_handle
 */
static void EventDeleteMutex(PB_EVENT_OPEN_HANDLE *p_evet_open_handle) {
    RET_API   ret_api = RET_ERROR;
    HANDLE    handle;

    if (p_evet_open_handle != NULL) {  // LCOV_EXCL_BR_LINE 6: p_evet_open_handle can not be NULL
        /* Get handle from Mutex name */
        handle = _pb_CreateMutex(NULL, FALSE, p_evet_open_handle->p_sys_event->name_of_mutex);

        /****************************************/
        /* Delete Mutex                         */
        /****************************************/
        ret_api = static_cast<RET_API>(PbDeleteMutex(handle));    /* Coverity CID:18817 Comment Managed */

        /* Cancel by deleting the generated portion when a handle was acquired */
        (void)PbDeleteMutex(handle);
    }

    /* When mutex processing fails */
    if (ret_api != RET_NORMAL) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "ret_api ERROR [ret_api:%d]", ret_api);
    }

    return;
}

/**
 * @brief
 *   Get dump information
 *
 * @param[out]    p_buf      Dump info
 * @param[in/out] p_len      Buffer size
 */
void _pb_GetDebugEventMngTbl(void* p_buf, uint8_t* p_len) {
    PB_EVENT_INSTANCE    *p_inst = &g_instance;
    static uint8_t        buf[DEBUG_DUMP_MAX_SIZE];
    static uint8_t        bufHdlTbl[DEBUG_DUMP_MAX_SIZE];
    static uint8_t        bufSysEvt[DEBUG_DUMP_MAX_SIZE];
    static uint8_t        buf_tmp[DEBUG_DUMP_MAX_SIZE];
    uint8_t                buf_indent[16];
    uint32_t               i;
    PB_EVENT_OPEN_HANDLE* p_hdl_tbl;
    uint8_t                cnt = 0;

    if ((p_buf != NULL) && (p_len != NULL)) {
        memset(&buf[0], 0x00, sizeof(buf));
        memset(&bufSysEvt, 0x00, sizeof(bufSysEvt));
        snprintf(reinterpret_cast<char *>(&buf_indent[0]), sizeof(buf_indent), "  ");
        GetDebugEventMngTblSysEvent(&bufSysEvt[0], p_inst->p_event_table, &buf_indent[0]);  // LCOV_EXCL_BR_LINE 200: no branch  // NOLINT(whitespace/line_length)
        snprintf(reinterpret_cast<char *>(&buf[0]), sizeof(buf),
                "Event-1\n ShrMem:%p\n idEvt:%d\n Evt:\n%s",
                p_inst->h_shared_memory,
                p_inst->id_event_table_sem,
                &bufSysEvt[0]);
        memcpy(p_buf, &buf[0], sizeof(buf));
        p_buf = reinterpret_cast<void *>((reinterpret_cast<uint8_t *>(p_buf)) + sizeof(buf));
        cnt++;
        if (cnt < *p_len) {
            memset(&bufHdlTbl[0], 0x00, sizeof(bufHdlTbl));
            for (i = 0; i < MAX_PB_EVENTS; i++) {
                // p_handle_table
                memset(&buf_tmp[0], 0x00, sizeof(buf_tmp));
                p_hdl_tbl = p_inst->p_handle_table[i];
                if (p_hdl_tbl == NULL) {
                    snprintf(reinterpret_cast<char *>(&buf_tmp[0]), sizeof(buf_tmp),
                            "\n  [%d] NULL",
                            i);
                } else {
                    memset(&bufSysEvt[0], 0x00, sizeof(bufSysEvt));
                    snprintf(reinterpret_cast<char *>(&buf_indent[0]), sizeof(buf_indent), "    ");
                    GetDebugEventMngTblSysEvent(&bufSysEvt[0], p_hdl_tbl->p_sys_event, &buf_indent[0]);
                    snprintf(reinterpret_cast<char *>(&buf_tmp[0]), sizeof(buf_tmp),
                            "\n  [%d]\n   h_heap:%p, index:%lu, l_thread_ref:%d\n   p_sys_event:\n%s",
                            i,
                            p_hdl_tbl->h_heap,
                            p_hdl_tbl->index,
                            p_hdl_tbl->l_thread_ref,
                            &bufSysEvt[0]);
                }
                strncat(reinterpret_cast<char *>(&bufHdlTbl[0]), reinterpret_cast<char *>(&buf_tmp[0]), \
                    strlen(reinterpret_cast<char *>(&buf_tmp[0])));
                if (((i + 1) % 4) == 0) {
                    cnt++;
                    memset(&buf[0], 0x00, sizeof(buf));
                    snprintf(reinterpret_cast<char *>(&buf[0]), sizeof(buf),
                            "Event-%d\n Handle:%s",
                            cnt,
                            &bufHdlTbl[0]);
                    memcpy(p_buf, &buf[0], sizeof(buf));
                    p_buf = reinterpret_cast<void *>((reinterpret_cast<uint8_t *>(p_buf)) + sizeof(buf));
                    memset(&bufHdlTbl[0], 0x00, sizeof(bufHdlTbl));
                    if (cnt >= *p_len) {
                        break;
                    }
                }
            }
        }
        if (cnt < *p_len) {
            if (bufHdlTbl[0] != 0x00) {
                cnt++;
                memset(&buf[0], 0x00, sizeof(buf));
                snprintf(reinterpret_cast<char *>(&buf[0]), sizeof(buf),
                        "Event-%d\n Handle:%s",
                        cnt,
                        &bufHdlTbl[0]);
                memcpy(p_buf, &buf[0], sizeof(buf));
            }
            *p_len = cnt;
        }
    }
}

/**
 * @brief
 *   Get dump information(PB_EVENT)
 *
 * @param[out] p_buf      Dump info
 * @param[in]  pEvt      PB_EVENT
 * @param[in]  pIndent   Indenting
 */
void GetDebugEventMngTblSysEvent(void* p_buf, PB_EVENT* pEvt, uint8_t* pIndent) {
    static uint8_t buf[DEBUG_DUMP_MAX_SIZE];
    static uint8_t buf_condition[1024];
    static uint8_t buf_flag_id[512];
    static uint8_t buf_tmp[DEBUG_DUMP_MAX_SIZE];
    uint32_t i;
    uint32_t e;

    if ((p_buf != NULL) && (pEvt != NULL)) {  // LCOV_EXCL_BR_LINE 6: p_buf and pEvt can not be NULL
        memset(&buf, 0x00, sizeof(buf));
        memset(&buf_condition, 0x00, sizeof(buf_condition));
        snprintf(reinterpret_cast<char *>(&(buf_condition)), sizeof(buf_condition), "stCnd:");
        for (i = 0; i < MAX_PB_EVENT_WAIT_THREADS; i++) {
            memset(&buf_flag_id, 0x00, sizeof(buf_flag_id));
            for (e = 0; e < MAX_EVENT_PROC_NUM; e++) {
                memset(&buf_tmp[0], 0x00, sizeof(buf_tmp));
                snprintf(reinterpret_cast<char *>(&buf_tmp[0]), sizeof(buf_tmp),
                        "[%d]0x%08x ", e, pEvt->st_condition[i].flag_id[e]);
                strncat(reinterpret_cast<char *>(&buf_flag_id[0]), reinterpret_cast<char *>(&buf_tmp[0]), \
                    strlen(reinterpret_cast<char *>(&buf_tmp[0])));
            }
            memset(&buf_tmp[0], 0x00, sizeof(buf_tmp));
            snprintf(reinterpret_cast<char *>(&buf_tmp[0]), sizeof(buf_tmp),
                    "\n%s   [%d] UseFlg:%d, Wait:%d, Mode:%d, Mask:%d, Min:%d, Max:%d, Last:%d, flag:%s",
                    pIndent,
                    i,
                    pEvt->st_condition[i].uc_use_flag,
                    pEvt->st_condition[i].uc_waiting,
                    pEvt->st_condition[i].us_mode,
                    pEvt->st_condition[i].ul_mask,
                    pEvt->st_condition[i].l_min_val,
                    pEvt->st_condition[i].l_max_val,
                    pEvt->st_condition[i].l_last_val,
                    &buf_flag_id[0]);
            strncat(reinterpret_cast<char *>(&buf_condition[0]), reinterpret_cast<char *>(&buf_tmp[0]), \
                strlen(reinterpret_cast<char *>(&buf_tmp[0])));
        }
        memset(&buf_tmp[0], 0x00, sizeof(buf_tmp));
        snprintf(reinterpret_cast<char *>(&buf_tmp[0]), sizeof(buf_tmp),
                "%s EvtName:%s",
                pIndent,
                pEvt->event_name);
        strncat(reinterpret_cast<char *>(&buf[0]), reinterpret_cast<char *>(&buf_tmp[0]), \
            strlen(reinterpret_cast<char *>(&buf_tmp[0])));
        memset(&buf_tmp[0], 0x00, sizeof(buf_tmp));
        snprintf(reinterpret_cast<char *>(&buf_tmp[0]), sizeof(buf_tmp),
                "\n%s EvtVal:%d",
                pIndent,
                pEvt->l_event_val);
        strncat(reinterpret_cast<char *>(&buf[0]), reinterpret_cast<char *>(&buf_tmp[0]), \
            strlen(reinterpret_cast<char *>(&buf_tmp[0])));
        memset(&buf_tmp[0], 0x00, sizeof(buf_tmp));
        snprintf(reinterpret_cast<char *>(&buf_tmp[0]), sizeof(buf_tmp),
                "\n%s %s",
                pIndent,
                &buf_condition[0]);
        strncat(reinterpret_cast<char *>(&buf[0]), reinterpret_cast<char *>(&buf_tmp[0]), \
            strlen(reinterpret_cast<char *>(&buf_tmp[0])));
        memset(&buf_tmp[0], 0x00, sizeof(buf_tmp));
        snprintf(reinterpret_cast<char *>(&buf_tmp[0]), sizeof(buf_tmp),
                "\n%s ProcRef:%d",
                pIndent,
                pEvt->l_process_ref);
        strncat(reinterpret_cast<char *>(&buf[0]), reinterpret_cast<char *>(&buf_tmp[0]), \
            strlen(reinterpret_cast<char *>(&buf_tmp[0])));
        memset(&buf_tmp[0], 0x00, sizeof(buf_tmp));
        snprintf(reinterpret_cast<char *>(&buf_tmp[0]), sizeof(buf_tmp),
                "\n%s ResetData:%d",
                pIndent,
                pEvt->l_reset_data);
        strncat(reinterpret_cast<char *>(&buf[0]), reinterpret_cast<char *>(&buf_tmp[0]), \
            strlen(reinterpret_cast<char *>(&buf_tmp[0])));
        memset(&buf_tmp[0], 0x00, sizeof(buf_tmp));
        snprintf(reinterpret_cast<char *>(&buf_tmp[0]), sizeof(buf_tmp),
                "\n%s ManualReset:%d",
                pIndent,
                pEvt->uc_manual_reset);
        strncat(reinterpret_cast<char *>(&buf[0]), reinterpret_cast<char *>(&buf_tmp[0]), \
            strlen(reinterpret_cast<char *>(&buf_tmp[0])));
        memset(&buf_tmp[0], 0x00, sizeof(buf_tmp));
        snprintf(reinterpret_cast<char *>(&buf_tmp[0]), sizeof(buf_tmp),
                "\n%s name_of_mutex:%s",
                pIndent,
                pEvt->name_of_mutex);
        strncat(reinterpret_cast<char *>(&buf[0]), reinterpret_cast<char *>(&buf_tmp[0]), \
            strlen(reinterpret_cast<char *>(&buf_tmp[0])));
        memcpy(p_buf, &buf[0], sizeof(buf));
    }
}
