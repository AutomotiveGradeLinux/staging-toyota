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
 *    _pbTimer.cpp
 */
/*---------------------------------------------------------------------------------*
 * Include Files                                                                   *
 *---------------------------------------------------------------------------------*/
#include <native_service/frameworkunified_framework_if.h>

#include <native_service/ns_message_center_if.h>

#include <vehicle_service/positioning_base_library.h>
#include "TimerEntryDrv_If.h"
#include "DEV_TimerEntryDrv_if.h"

#include <native_service/ns_timer_if.h>
#include "WPF_STD_private.h"


/*---------------------------------------------------------------------------------*
 * Define                                                                          *
 *---------------------------------------------------------------------------------*/
#define MAX_CTRL_TIMER_NUM (8)

#define CID_TIMER_1    (0x1000)
#define CID_TIMER_2    (0x1001)
#define CID_TIMER_3    (0x1002)
#define CID_TIMER_4    (0x1003)
#define CID_TIMER_5    (0x1004)
#define CID_TIMER_6    (0x1005)
#define CID_TIMER_7    (0x1006)
#define CID_TIMER_8    (0x1007)

#define TIMER_MAKE_DEFAULT_MESSAGE(x) \
    (x)->Header.signo = 0; \
    (x)->Header.hdr.sndpno = 0; \
    (x)->Header.hdr.respno = 0; \
    (x)->Header.hdr.cid = CID_TIMER_TOUT; \
    (x)->Header.hdr.msgbodysize = \
        static_cast<uint16_t>(sizeof(TimerToutMsg) - sizeof(T_APIMSG_MSGBUF_HEADER)); \
    (x)->Header.hdr.rid = 0; \
    (x)->Header.hdr.reserve = 0; \
    (x)->TimerSeq = 0;

/*---------------------------------------------------------------------------------*
 * Structure                                                                       *
 *---------------------------------------------------------------------------------*/
/*!
   @brief    Timer control information
*/
typedef struct {
    CID            cid;        /**< Command ID              */
    HANDLE         h_timer;    /**< Timer handle            */
    TimerToutMsg   msg_buf;    /**< Message buffer        */
    uint16_t       size;        /**< Message size        */
    PNO            pno;        /**< Process number            */
    uint16_t       seq_no;        /**< Timer Sequence Number    */
    uint8_t        type;         /**< Timer type            */
    uint32_t       time_out;    /**< Timeout       */
} TIMER_CTRL_INFO;

/*---------------------------------------------------------------------------------*
 * Local Function Prototype                                                        *
 *---------------------------------------------------------------------------------*/
/* Timer control table manipulation functions */
static void        TimerSetCidOfCtrlTbl(u_int32 idx, CID cid);               /* Set timer CID    */
static CID        TimerGetCidOfCtrlTbl(u_int32 idx);                           /* Get timer CID    */
static void        TimerSetPnoOfCtrlTbl(u_int32 idx, PNO pno);               /* Set PNO              */
static PNO        TimerGetPnoOfCtrlTbl(u_int32 idx);                           /* Get PNO             */
static void        TimerSetSizeOfCtrlTbl(u_int32 idx, u_int16 size);           /* Set message size         */
static TimerToutMsg* TimerGetMsgBufOfCtrlTbl(u_int32 idx);                        /* Get message buffer     */
static void        TimerSetTimerHandleOfCtrlTbl(u_int32 idx, HANDLE handle);      /* Set timer handle         */
static HANDLE    TimerGetTimerHandleOfCtrlTbl(u_int32 idx);                        /* Get timer handle         */
static void        TimerSetTypeOfCtrlTbl(u_int32 idx, u_int8 type);                /* Set timer type             */
static u_int8    TimerGetTypeOfCtrlTbl(u_int32 idx);                            /* Get timer type             */
static void        TimerSetSeqNoOfCtrlTbl(u_int32 idx, u_int16 seq_no);            /* Set timer Sequence Number     */
static u_int16    TimerGetSeqNoOfCtrlTbl(u_int32 idx);                            /* Get timer sequence number     */
static void        TimerSetTimeOutOfCtrlTbl(u_int32 idx, u_int32 time_out);        /* Set timeout         */
static u_int32  TimerGetTimeOutOfCtrlTbl(u_int32 idx);                         /* Get timeout         */
static u_int32    TimerSearchEmptyOfCtrlTbl(void);                                /* Search unused area             */
static u_int32    TimerSearchTimerOfCtrlTbl(PNO snd_pno, u_int16 timer_seq, u_int8 time_type); /* Search specified timer */
static void        TimerClearSettingOfCtrlTbl(u_int32 idx);                        /* Clear timer information             */

/* Mutex handling Functions for accessing Timer Control Table */
static void TimerCreateMutex(void);    /* Create Mutex */
static void TimerDeleteMutex(void);    /* Delete Mutex */
static void TimerLockMutex(void);        /* Get Mutex */
static void TimerUnlockMutex(void);    /* Release Mutex */

/* Callback function resources */
static EFrameworkunifiedStatus TimerCallback1(HANDLE h_app);
static EFrameworkunifiedStatus TimerCallback2(HANDLE h_app);
static EFrameworkunifiedStatus TimerCallback3(HANDLE h_app);
static EFrameworkunifiedStatus TimerCallback4(HANDLE h_app);
static EFrameworkunifiedStatus TimerCallback5(HANDLE h_app);
static EFrameworkunifiedStatus TimerCallback6(HANDLE h_app);
static EFrameworkunifiedStatus TimerCallback7(HANDLE h_app);
static EFrameworkunifiedStatus TimerCallback8(HANDLE h_app);

static void TimerCallbackComProc(const uint8_t id);

/*---------------------------------------------------------------------------------*
 * Grobal Values                                                                   *
 *---------------------------------------------------------------------------------*/
/**
   Timer control table
   Note : Access to this instance shall be made through the operation module.
*/
static TIMER_CTRL_INFO g_timer_ctrl_tbl[MAX_CTRL_TIMER_NUM];    // NOLINT(readability/nolint) global class instance

/** Timer control table lock Mutex handle */
static HANDLE        g_h_mtx = NULL;

/** Timer event destination handle */
static HANDLE        g_h_service;


/** Dispatcher Registration Callback Table */
static const FrameworkunifiedProtocolCallbackHandler kTimerPcbhs[] = {
    {CID_TIMER_1,    &TimerCallback1    },
    {CID_TIMER_2,    &TimerCallback2    },
    {CID_TIMER_3,    &TimerCallback3    },
    {CID_TIMER_4,    &TimerCallback4    },
    {CID_TIMER_5,    &TimerCallback5    },
    {CID_TIMER_6,    &TimerCallback6    },
    {CID_TIMER_7,    &TimerCallback7    },
    {CID_TIMER_8,    &TimerCallback8    },
};  // LCOV_EXCL_BR_LINE 11:unexpected branch

/*---------------------------------------------------------------------------------*
 * Function                                                                        *
 *---------------------------------------------------------------------------------*/
/**
 * @brief
 *   Timer function initialization
 *
 * @return    RET_NORMAL    Normal completion
 * @return    RET_ERROR    ABEND
 */
RET_API TimerInit(HANDLE h_app) {
    RET_API ret_api = RET_NORMAL;
    u_int32    idx;
    HANDLE        h_timer;
    EFrameworkunifiedStatus    estatus;
    NSTimerInfo    timer_info;
    HANDLE* p_h_service = &g_h_service;

    if (h_app == NULL) {  // LCOV_EXCL_BR_LINE 6: h_app cannot be Null
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argument ERROR!! [h_app=%p]", h_app);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        ret_api = RET_ERROR;  // LCOV_EXCL_LINE 6: h_app cannot be Null
    } else {
        memset(&timer_info, 0x00, sizeof(timer_info));

        /* Create Mutex */
        TimerCreateMutex();  // LCOV_EXCL_BR_LINE 200: no branch

        /* Register callback function for timer control */
        // LCOV_EXCL_BR_LINE 4: nsfw error
        estatus = FrameworkunifiedAttachCallbacksToDispatcher(h_app, "NS_ANY_SRC", kTimerPcbhs, _countof(kTimerPcbhs));  // LCOV_EXCL_BR_LINE 4: nsfw error  // NOLINT(whitespace/line_length)
        if (estatus != eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4: nsfw error
            /* When registration fails */
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, \
                "FrameworkunifiedAttachCallbacksToDispatcher ERROR [status:%d]", estatus);
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            ret_api = RET_ERROR;  // LCOV_EXCL_LINE 4: nsfw error
        }

        /* Initialization of timer control table */
        for (idx = 0; idx < MAX_CTRL_TIMER_NUM; idx++) {
            TimerSetCidOfCtrlTbl(idx, static_cast<CID>(kTimerPcbhs[idx].iCmd)); /* Set timer control CID */  // LCOV_EXCL_BR_LINE 200: no branch  // NOLINT(whitespace/line_length)

            /* Initialize timeout */
            TimerSetTimeOutOfCtrlTbl(idx, 0);  // LCOV_EXCL_BR_LINE 200: no branch

            *p_h_service = McOpenSender("Positioning"); /* Be intended for use only in Positioning */  // LCOV_EXCL_BR_LINE 4: nsfw error  // NOLINT(whitespace/line_length)

            /* Create Timer Resource */
            timer_info.iCmd = TimerGetCidOfCtrlTbl(idx); /* Only CID needs to be set. */  // LCOV_EXCL_BR_LINE 200: no branch
            h_timer = NS_TimerCreate(timer_info, CALLBACK_MESSAGE, (HANDLE)*p_h_service);  // LCOV_EXCL_BR_LINE 4: nsfw error  // NOLINT(whitespace/line_length)
            if (h_timer == NULL) {  // LCOV_EXCL_BR_LINE 4: nsfw error
                // LCOV_EXCL_START 4: nsfw error
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                /* When an error occurs */
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, \
                    "NS_TimerCreate ERROR [h_timer:%p, hService:%p]", h_timer, *p_h_service);
                ret_api = RET_ERROR;
                // LCOV_EXCL_STOP
            } else {
                /* If successful */
                /* Set Handle information */
                TimerSetTimerHandleOfCtrlTbl(idx, h_timer);  // LCOV_EXCL_BR_LINE 200: no branch
            }
        }
    }

    return ret_api;
}

/**
 * @brief
 *   Terminate timer function
 *
 * @return    Normal completion
 */
RET_API TimerTerm(void) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API ret_api = RET_NORMAL;
    u_int32    idx;
    HANDLE h_timer;
    EFrameworkunifiedStatus estatus;
    HANDLE* p_h_service = &g_h_service;

    /* If a control Mutex has not been created, it is determined that the Timer function has not been initialized (non Positioning processes) and the process terminates with an error. */
    if (g_h_mtx == NULL) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "g_h_mtx is NULL!!");
        ret_api = RET_ERROR;
    } else {
        /* Delete timer control information */
        for (idx = 0; idx < MAX_CTRL_TIMER_NUM; idx++) {
            h_timer = TimerGetTimerHandleOfCtrlTbl(idx);
            if (h_timer != NULL) {
                /* Delete timer */
                estatus = NS_TimerDelete(h_timer);
                if (estatus != eFrameworkunifiedStatusOK) {
                    /* When an error occurs */
                    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, \
                        "NS_TimerDelete ERROR [estatus:%d, h_timer:%p]", estatus, h_timer);
                }
                TimerSetTimerHandleOfCtrlTbl(idx, NULL);
            }

            TimerClearSettingOfCtrlTbl(idx);
        }

        /* Mutex deletion */
        TimerDeleteMutex();
    }

    /* Delete transmission handle */
    estatus = McClose(*p_h_service);
    if (estatus != eFrameworkunifiedStatusOK) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "McClose ERROR [estatus:%d, hService:%p]", \
            estatus, *p_h_service);
    }

    return ret_api;
}
// LCOV_EXCL_STOP

/**
 * @brief
 *   Fixed period/asynchronous timer start instruction
 *
 * @param[in]  snd_pno        Requesting process number
 * @param[in]  timer_seq        Timer sequence number
 * @param[in]  TimerType    Timer type
 * @param[in]  time_out         Timeout value [10ms]
 *
 * @return    RET_NORMAL        Normal completion<br>
 *            RET_ERROR        Message transmission error<br>
 *            RET_ERRPARAM    Parameter error
 */
RET_API _pb_ReqTimerStart(PNO snd_pno, u_int16 timer_seq,      // NOLINT(readability/nolint) WPF_SYSAPI.h API
        u_int8 time_type, u_int32 time_out) {
    RET_API        ret_api = RET_NORMAL; /* Return value of this module */
    u_int32        idx;
    TimerToutMsg *p_msg;
    EFrameworkunifiedStatus  estatus;
    HANDLE        h_timer;
    NSTimerInfo    timer_info;

    FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, "+");

    /* If a control Mutex has not been created, it is determined that the Timer function has not been initialized (non Positioning processes) and the process terminates with an error. */
    if (g_h_mtx == NULL) {  // LCOV_EXCL_BR_LINE 6: g_h_mtx cannot be null
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "g_h_mtx is NULL!!");
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        ret_api = RET_ERROR;  // LCOV_EXCL_LINE 6: g_h_mtx cannot be null
    } else {
        /* Parameter study */
        if ((time_type != TIMER_TYPE_SYN) && (time_type != TIMER_TYPE_USN)) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argument ERROR [time_type:%d]", time_type);
            ret_api = RET_ERRPARAM;    /* Timer type error */
        }

        if (time_out == 0) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argument ERROR [time_out:%d]", time_out);
            ret_api =  RET_ERRPARAM; /* Timer setting value error */
        }

        /* When a timer of the same process number, sequence number, or type is already registered, creation of the timer is not allowed. */
        idx = TimerSearchTimerOfCtrlTbl(snd_pno, timer_seq, time_type);
        if (idx != MAX_CTRL_TIMER_NUM) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "TimerSearchTimerOfCtrlTbl ERROR!! " \
                "[snd_pno:%d, timer_seq:%d, TimeType:%d]", snd_pno, timer_seq, time_type);
            ret_api =  RET_ERRPARAM; /* Invalid timer value */
        }

        /* Parameter normal */
        if (ret_api == RET_NORMAL) {
            TimerLockMutex(); /* Get Mutex */

            /* Get free space in timer control table */
            idx = TimerSearchEmptyOfCtrlTbl();  // LCOV_EXCL_BR_LINE 200: no branch
            if (idx == MAX_CTRL_TIMER_NUM) {  // LCOV_EXCL_BR_LINE 200: idx can not be MAX_CTRL_TIMER_NUM
                // LCOV_EXCL_START 200: idx can not be MAX_CTRL_TIMER_NUM
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                /* When there is no free space */
                /* Be impossible by design */
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "TimerSearchEmptyCtrlTbl ERROR!! " \
                    "[idx = %d]", idx);
                _pb_Exit();
                /* don't arrive here. */
                // LCOV_EXCL_STOP
            }

            /* Get message buffer address */
            p_msg = TimerGetMsgBufOfCtrlTbl(idx);

            TIMER_MAKE_DEFAULT_MESSAGE(p_msg); /* Set message data to be send */
            p_msg->TimerSeq = timer_seq; /* Timer sequence number */

            /* Set callback function information in timer control table */
            TimerSetPnoOfCtrlTbl(idx, snd_pno);  // LCOV_EXCL_BR_LINE 200: no branch
            TimerSetSizeOfCtrlTbl(idx, sizeof(TimerToutMsg));  // LCOV_EXCL_BR_LINE 200: no branch
            TimerSetTypeOfCtrlTbl(idx, time_type);  // LCOV_EXCL_BR_LINE 200: no branch
            TimerSetSeqNoOfCtrlTbl(idx, timer_seq);  // LCOV_EXCL_BR_LINE 200: no branch
            TimerSetTimeOutOfCtrlTbl(idx, time_out);  // LCOV_EXCL_BR_LINE 200: no branch

            /* Set timer value */
            timer_info.t_sec = (uint32_t)((10 * time_out) / 1000);
            /* Coverity CID: 21979 compliant */
            timer_info.t_nsec = ((10 * (uint64_t)time_out) - ((uint64_t)(timer_info.t_sec) * 1000)) * 1000 * 1000;
            timer_info.iCmd = TimerGetCidOfCtrlTbl(idx);
            timer_info.rpt_sec = 0;
            timer_info.rpt_nsec = 0;
            if (time_type == TIMER_TYPE_SYN) {
                timer_info.rpt_sec  = timer_info.t_sec;
                timer_info.rpt_nsec = timer_info.t_nsec;
            }

            h_timer = TimerGetTimerHandleOfCtrlTbl(idx);

            /* Start timer */
            estatus = NS_TimerSetTime(h_timer, timer_info);  // LCOV_EXCL_BR_LINE 4: nsfw error
            if ((h_timer == NULL) || (estatus != eFrameworkunifiedStatusOK)) {  // LCOV_EXCL_BR_LINE 4: nsfw error
                // LCOV_EXCL_START 4: nsfw error
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                /* When an error occurs */
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "NS_TimerSetTime ERROR " \
                    "[estatus:%d, h_timer:%p]", estatus, h_timer);

                TimerClearSettingOfCtrlTbl(idx); /* Clear timer information */
                ret_api = RET_ERROR;
                // LCOV_EXCL_STOP
            } else {
                /* If successful */
                FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, "### TIMER TABLE INFORMATION # " \
                    "(++) idx:%d cid:%d h_timer:%p pno:%d seq_no:%d type:%d time_out:%d", \
                    idx, g_timer_ctrl_tbl[idx].cid, g_timer_ctrl_tbl[idx].h_timer, g_timer_ctrl_tbl[idx].pno, \
                    g_timer_ctrl_tbl[idx].seq_no, g_timer_ctrl_tbl[idx].type, g_timer_ctrl_tbl[idx].time_out);
            }

            TimerUnlockMutex(); /* Release Mutex */  // LCOV_EXCL_BR_LINE 200: no branch
        }
    }

    FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, "-");

    return ret_api;
}

/**
 * @brief
 *   Timer stop instruction
 *
 * @param[in]    snd_pno    Requesting process number
 * @param[in]    timer_seq    Timer sequence number
 * @param[in]    time_r_type    Timer type<br>
 *                TIMER_TYPE_SYN Fixed-period timer<br>
 *                TIMER_TYPE_USN Asynchronous timer<br>
 *                TIMER_TYPE_ALM Alerm with specified time <br>
 *
 * @return    RET_NORMAL        Normal completion<br>
 *            RET_ERROR        Message transmission error<br>
 *            RET_ERRPARAM    Parameter error
 */
RET_API _pb_TimerStop(PNO snd_pno, u_int16 timer_seq,     // NOLINT(readability/nolint) WPF_SYSAPI.h API
            u_int8 time_type) {
    RET_API    ret_api = RET_NORMAL; /* Return value of this module */
    u_int32    idx;
    EFrameworkunifiedStatus estatus;
    HANDLE     h_timer;
    const NSTimerInfo timer_info = {0};

    FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, "+");

    /* If a control Mutex has not been created, it is determined that the Timer function has not been initialized (non Positioning processes) and the process terminates with an error. */
    if (g_h_mtx == NULL) {  // LCOV_EXCL_BR_LINE 6: g_h_mtx cannot be NULL
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "g_h_mtx is NULL!!");
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        ret_api = RET_ERROR;  // LCOV_EXCL_LINE 6: g_h_mtx cannot be NULL
    } else {
        TimerLockMutex(); /* Get Mutex */  // LCOV_EXCL_BR_LINE 200: no branch

        idx = TimerSearchTimerOfCtrlTbl(snd_pno, timer_seq, time_type);
        if (idx == MAX_CTRL_TIMER_NUM) {  // LCOV_EXCL_BR_LINE 200: idx can not be MAX_CTRL_TIMER_NUM
            /* When the specified timer is not set */
            /* nop */
        } else {
            h_timer = TimerGetTimerHandleOfCtrlTbl(idx);

            /* Stop timer */
            estatus = NS_TimerSetTime(h_timer, timer_info);  // LCOV_EXCL_BR_LINE 4: nsfw error
            if (estatus != eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4: nsfw error
                // LCOV_EXCL_START 4: nsfw error
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                /* When deletion fails */
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "NS_TimerSetTime ERROR " \
                    "[estatus:%d, h_timer:%p]", estatus, h_timer);
                ret_api = RET_ERROR;
                // LCOV_EXCL_STOP
            } else {
                /* If successful */
                /* Clear timer information */
                TimerClearSettingOfCtrlTbl(idx);  // LCOV_EXCL_BR_LINE 200: no branch

                FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, "### TIMER TABLE INFORMATION # " \
                    "(--) idx:%d cid:%d h_timer:%p pno:%d seq_no:%d type:%d time_out:%d", \
                    idx, g_timer_ctrl_tbl[idx].cid, g_timer_ctrl_tbl[idx].h_timer, g_timer_ctrl_tbl[idx].pno, \
                    g_timer_ctrl_tbl[idx].seq_no, g_timer_ctrl_tbl[idx].type, g_timer_ctrl_tbl[idx].time_out);
            }
        }

        TimerUnlockMutex(); /* Release Mutex */  // LCOV_EXCL_BR_LINE 200: no branch
    }

    FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, "-");

    return ret_api;
}

/*---------------------------------------------------------------------------------*
 * Local Function                                                                  *
 *---------------------------------------------------------------------------------*/
/**
 * @brief
 *   Timer CID setting (Timer control table)
 *
 *   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 *
 * @param[in]  idx    Control table accessor
 * @param[in]  cid    Command ID
 */
static void TimerSetCidOfCtrlTbl(u_int32 idx, CID cid) {
    /* check index */
    if (idx >= MAX_CTRL_TIMER_NUM) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argment ERROR [idx:%d]", idx);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater
        /* don't arrive here. */
    } else {
        g_timer_ctrl_tbl[idx].cid = cid;
    }

    return;
}

/**
 * @brief
 *   Timer CID acquisition (Timer control table)
 *
 *   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 *
 * @param[in]    idx    Timer control table accessor
 *
 * @return Command ID
 */
static CID TimerGetCidOfCtrlTbl(u_int32 idx) {
    /* check index */
    if (idx >= MAX_CTRL_TIMER_NUM) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argment ERROR [idx:%d]", idx);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater
        /* don't arrive here. */
    }

    return g_timer_ctrl_tbl[idx].cid;
}

/**
 * @brief
 *   PNO setting (Timer control table)
 *
 *   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 *
 * @param[in]  idx    Timer control table accessor
 * @param[in]  pno    Process number
 */
static void TimerSetPnoOfCtrlTbl(u_int32 idx, PNO pno) {
    /* check index */
    if (idx >= MAX_CTRL_TIMER_NUM) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argment ERROR " \
            "[idx:%d, pno:%d]", idx, pno);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater
        /* don't arrive here. */
    } else {
        g_timer_ctrl_tbl[idx].pno = pno;
    }

    return;
}

/**
 * @brief
 *   PNO acquisition (Timer control table)
 *
 *   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 *
 * @param[in]  idx    Timer control table accessor
 *
 * @return Process number
 */
static PNO TimerGetPnoOfCtrlTbl(u_int32 idx) {
    /* check index */
    if (idx >= MAX_CTRL_TIMER_NUM) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argment ERROR [idx:%d]", idx);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater
        /* don't arrive here. */
    }

    return g_timer_ctrl_tbl[idx].pno;
}

/**
 * @brief
 *   Message size setting (Timer control table)
 *
 *   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 *
 * @param[in]  idx        Timer control table accessor
 * @param[in]  size    Message size
 */
static void TimerSetSizeOfCtrlTbl(u_int32 idx, u_int16 size) {
    /* check index */
    if (idx >= MAX_CTRL_TIMER_NUM) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argment ERROR " \
            "[idx:%d, size:%d]", idx, size);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater
        /* don't arrive here. */
    } else {
        g_timer_ctrl_tbl[idx].size = size;
    }

    return;
}

/**
 * @brief
 *   Message buffer acquisition (Timer control table)
 *
 *   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 *
 * @param[in]  idx    Timer control table accessor
 *
 * @return Pointer to message storage area
 */
static TimerToutMsg* TimerGetMsgBufOfCtrlTbl(u_int32 idx) {
    /* check index */
    if (idx >= MAX_CTRL_TIMER_NUM) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argment ERROR [idx:%d]", idx);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater
        /* don't arrive here. */
    }

    return &(g_timer_ctrl_tbl[idx].msg_buf);
}

/**
 * @brief
 *   Timer handle setting (Timer control table)
 *
 *   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 *
 * @param[in]  idx        Timer control table accessor
 * @param[in]   handle    Timer handle
 */
static void TimerSetTimerHandleOfCtrlTbl(u_int32 idx, HANDLE handle) {
    /* check index */
    if (idx >= MAX_CTRL_TIMER_NUM) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argment ERROR " \
            "[idx:%d, handle:%p]", idx, handle);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater
        /* don't arrive here. */
    } else {
        g_timer_ctrl_tbl[idx].h_timer = handle;
    }

    return;
}

/**
 * @brief
 *   Timer handle acquisition (Timer control table)
 *
 *   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 *
 * @param[in]  idx    Timer control table accessor
 *
 * @return    Timer handle
 */
static HANDLE TimerGetTimerHandleOfCtrlTbl(u_int32 idx) {
    /* check index */
    if (idx >= MAX_CTRL_TIMER_NUM) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argment ERROR [idx:%d]", idx);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater
        /* don't arrive here. */
    }

    return g_timer_ctrl_tbl[idx].h_timer;
}

/**
 * @brief
 *   Timer type setting (Timer control table)
 *
 *   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 *
 * @param[in]  idx        Timer control table accessor
 * @param[in]  type    Timer type
 */
static void TimerSetTypeOfCtrlTbl(u_int32 idx, u_int8 type) {
    /* check index */
    if (idx >= MAX_CTRL_TIMER_NUM) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argment ERROR " \
            "[idx:%d, type:%d]", idx, type);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater
        /* don't arrive here. */
    } else {
        g_timer_ctrl_tbl[idx].type = type;
    }

    return;
}

/**
 * @brief
 *   Timer type acquisition (Timer control table)
 *
 *   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 *
 * @param[in]  idx    Timer control table accessor
 *
 * @return Timer handle
 */
static u_int8 TimerGetTypeOfCtrlTbl(u_int32 idx) {
    /* check index */
    if (idx >= MAX_CTRL_TIMER_NUM) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argment ERROR [idx:%d]", idx);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater
        /* don't arrive here. */
    }

    return g_timer_ctrl_tbl[idx].type;
}

/**
 * @brief
 *   Timer sequence number setting (Timer control table)
 *
 *   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 *
 * @param[in]  idx        Timer control table accessor
 * @param[in]  seq_no    Timer Sequence Number
 */
static void TimerSetSeqNoOfCtrlTbl(u_int32 idx, u_int16 seq_no) {
    /* check index */
    if (idx >= MAX_CTRL_TIMER_NUM) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argment ERROR " \
            "[idx:%d, seq_no:%d]", idx, seq_no);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater
        /* don't arrive here. */
    } else {
        g_timer_ctrl_tbl[idx].seq_no = seq_no;
    }

    return;
}

/**
 * @brief
 *   Timer sequence number acquisition (Timer control table)
 *
 *   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 *
 * @param[in]  idx        Timer control table accessor
 */
static u_int16 TimerGetSeqNoOfCtrlTbl(u_int32 idx) {
    /* check index */
    if (idx >= MAX_CTRL_TIMER_NUM) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argment ERROR [idx:%d]", idx);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater
        /* don't arrive here. */
    }

    return g_timer_ctrl_tbl[idx].seq_no;
}

/**
 * @brief
 *   Timeout setting (Timer control table)
 *
 *   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 *
 * @param[in]  idx            Timer control table access Index
 * @param[in]  time_out    Timeout
 */
static void TimerSetTimeOutOfCtrlTbl(u_int32 idx, u_int32 time_out) {
    /* check index */
    if (idx >= MAX_CTRL_TIMER_NUM) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argment ERROR [idx:%d, " \
            "time_out:%d]", idx, time_out);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater
        /* don't arrive here. */
    } else {
        g_timer_ctrl_tbl[idx].time_out = time_out;
    }

    return;
}

/**
 * @brief
 *   Timeout acquisition (Timer control table)
 *
 *   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 *
 * @param[in]  idx            Timer control table access Index
 *
 * @return Timeout value
 */
static u_int32 TimerGetTimeOutOfCtrlTbl(u_int32 idx) {
    /* check index */
    if (idx >= MAX_CTRL_TIMER_NUM) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argment ERROR [idx:%d]", idx);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6:idx cannot greater
        /* don't arrive here. */
    }

    return g_timer_ctrl_tbl[idx].time_out;
}

/**
 * @brief
 *   Retreaval of unused area in the timer control table (Timer control table)
 *
 *   Return the lowest-numbered index for accessing unused space in the Timer control table.
 *   If the mutex is not registered, the maximum timer management value (MAX_CTRL_MUTEX_NUM)
 *   is returned.
 *
 * @return Table accessor
 */
static u_int32 TimerSearchEmptyOfCtrlTbl(void) {
    u_int32    idx;
    u_int32 time_out;

    for (idx = 0; idx < MAX_CTRL_TIMER_NUM; idx++) {
        time_out = TimerGetTimeOutOfCtrlTbl(idx);

        /* For unused space */
        if (time_out == 0) {
            break;
        }
    }

    return idx;
}

/**
 * @brief
 *   Retrieval of specified timer for in the timer control table (Timer control table)
 *
 *   Retrieve whether the specified timer is already registered in the timer control table.
 *   If it is registered, the access index is returned. If it is not registered,
 *   Return the maximum value of timer management (MAX_CTRL_TIMER_NUM).
 *
 * @param[in]  snd_pno        Process number
 * @param[in]  TimerSeq    Timer sequence number
 * @param[in]  TimeType    Timer type
 *
 * @return Index for access(If it is registered)<br>
 *              Maximum mutex management value (Not registered)
 */
static u_int32 TimerSearchTimerOfCtrlTbl(PNO snd_pno, u_int16 TimerSeq, u_int8 TimeType) {
    u_int32    idx;
    PNO        pno;
    u_int16    seq_no;
    u_int8        type;

    for (idx = 0; idx < MAX_CTRL_TIMER_NUM; idx++) {
        pno   = TimerGetPnoOfCtrlTbl(idx);
        seq_no = TimerGetSeqNoOfCtrlTbl(idx);
        type  = TimerGetTypeOfCtrlTbl(idx);

        /* If there is a match */
        if ((pno == snd_pno) && (seq_no == TimerSeq) && (type == TimeType)) {
            break;
        }
    }

    return idx;
}

/**
 * @brief
 *   Clear timer setting information
 *
 * @param[in]  idx    Timer control table accessor
 */
static void TimerClearSettingOfCtrlTbl(u_int32 idx) {
    void *p_msg;

    /* Delete timer information */
    TimerSetPnoOfCtrlTbl(idx, 0);
    TimerSetSizeOfCtrlTbl(idx, 0);
    TimerSetTypeOfCtrlTbl(idx, 0);
    TimerSetSeqNoOfCtrlTbl(idx, 0);
    TimerSetTimeOutOfCtrlTbl(idx, 0);
    p_msg = TimerGetMsgBufOfCtrlTbl(idx);
    memset(p_msg, 0x00, sizeof(TimerToutMsg));

    return;
}

/**
 * @brief
 *   Create Mutex for accessing the timer control table
 */
static void TimerCreateMutex(void) {
    g_h_mtx = _pb_CreateMutex(NULL, 0, "Timer_Mutex");
    if (g_h_mtx == NULL) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_CreateMutex ERROR " \
            "[g_h_mtx:%p]", g_h_mtx);
        _pb_Exit();
        /* don't arrive here. */
    }

    return;
}

/**
 * @brief
 *   Delete Mutex for accessing timer control table
 */
static void TimerDeleteMutex(void) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    DWORD    ret;

    ret = PbDeleteMutex(g_h_mtx);
    if (ret != WAIT_OBJECT_0) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "PbDeleteMutex ERROR " \
            "[ret:%lu, g_h_mtx:%p]", ret, g_h_mtx);
        _pb_Exit();
        /* don't arrive here. */
    }

    return;
}
// LCOV_EXCL_STOP

/**
 * @brief
 *   Get Mutex for accessing timer control table
 */
static void TimerLockMutex(void) {
    DWORD    ret;

    ret = PbMutexLock(g_h_mtx, INFINITE);  // LCOV_EXCL_BR_LINE 200: lock will not failed
    if (ret != WAIT_OBJECT_0) {  // LCOV_EXCL_BR_LINE 200: lock will not failed
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "PbMutexLock ERROR " \
            "[ret:%lu, g_h_mtx:%p]", ret, g_h_mtx);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 200: lock will not failed
        /* don't arrive here. */
    }

    return;
}

/**
 * @brief
 *   Release Mutex for accessing timer control table
 */
static void TimerUnlockMutex(void) {
    BOOL    ret;

    ret = PbMutexUnlock(g_h_mtx);  // LCOV_EXCL_BR_LINE 200: unlock will not failed
    if (ret != TRUE) {  // LCOV_EXCL_BR_LINE 200: unlock will not failed
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "PbMutexUnlock ERROR " \
            "[ret:%d, g_h_mtx:%p]", ret, g_h_mtx);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 200: unlock will not failed
        /* don't arrive here. */
    }

    return;
}

/**
 * @brief
 *   Timer Expiration Callback Functions
 *
 *   For setting the timer creation function (NS_TimerCreate)<br>
 *   TimerCallback1 ... TimerCallback8
 *
 * @param[in]  h_app Application handle
 *
 * @return eFrameworkunifiedStatusOK    Normal completion
 */
static EFrameworkunifiedStatus TimerCallback1(HANDLE h_app) {
    static const u_int8 ID = 0;

    FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, "+");

    TimerCallbackComProc(ID);

    FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, "-");

    return eFrameworkunifiedStatusOK;
}

static EFrameworkunifiedStatus TimerCallback2(HANDLE h_app) {
    static const u_int8 ID = 1;

    FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, "+");

    TimerCallbackComProc(ID);

    FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, "-");

    return eFrameworkunifiedStatusOK;
}

static EFrameworkunifiedStatus TimerCallback3(HANDLE h_app) {
    static const u_int8 ID = 2;

    FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, "+");

    TimerCallbackComProc(ID);

    FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, "-");

    return eFrameworkunifiedStatusOK;
}

static EFrameworkunifiedStatus TimerCallback4(HANDLE h_app) {
    static const u_int8 ID = 3;

    FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, "+");

    TimerCallbackComProc(ID);

    FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, "-");

    return eFrameworkunifiedStatusOK;
}

static EFrameworkunifiedStatus TimerCallback5(HANDLE h_app) {
    static const u_int8 ID = 4;

    FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, "+");

    TimerCallbackComProc(ID);

    FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, "-");

    return eFrameworkunifiedStatusOK;
}

static EFrameworkunifiedStatus TimerCallback6(HANDLE h_app) {
    static const u_int8 ID = 5;

    FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, "+");

    TimerCallbackComProc(ID);

    FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, "-");

    return eFrameworkunifiedStatusOK;
}

static EFrameworkunifiedStatus TimerCallback7(HANDLE h_app) {
    static const u_int8 ID = 6;

    FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, "+");

    TimerCallbackComProc(ID);

    FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, "-");

    return eFrameworkunifiedStatusOK;
}

static EFrameworkunifiedStatus TimerCallback8(HANDLE h_app) {
    static const u_int8 ID = 7;

    FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, "+");

    TimerCallbackComProc(ID);

    FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, "-");

    return eFrameworkunifiedStatusOK;
}

static void TimerCallbackComProc(const uint8_t id) {
    uint8_t type;

    TimerLockMutex(); /* Get Mutex */

    /* Message is sent to the thread specified when the timer is created. */
    (void)_pb_SndMsg(g_timer_ctrl_tbl[id].pno, g_timer_ctrl_tbl[id].size, &(g_timer_ctrl_tbl[id].msg_buf), 0);

    type = TimerGetTypeOfCtrlTbl(id);
    if (type == TIMER_TYPE_USN) {
        /* One-shot timer */
        /* Clear timer information */
        TimerClearSettingOfCtrlTbl(id);

        FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, "### TIMER TABLE INFORMATION # " \
            "(--) idx:%d cid:%d h_timer:%p pno:%d seq_no:%d type:%d time_out:%d", \
            id, g_timer_ctrl_tbl[id].cid, g_timer_ctrl_tbl[id].h_timer, g_timer_ctrl_tbl[id].pno, \
            g_timer_ctrl_tbl[id].seq_no, g_timer_ctrl_tbl[id].type, g_timer_ctrl_tbl[id].time_out);
    }

    TimerUnlockMutex(); /* Release Mutex */

    return;
}

/**
 * @brief
 *   Get dump information
 *
 * @param[out] p_buf      Dump info
 */
void _pb_GetDebugTimerMngTbl(void* p_buf) {
    static uint8_t  buf[DEBUG_DUMP_MAX_SIZE];
    static uint8_t  buf_tmp[256];
    uint32_t         i;

    if (p_buf != NULL) {
        memset(&buf, 0x00, sizeof(buf));
        snprintf(reinterpret_cast<char *>(&(buf)), sizeof(buf), "Timer");
        for (i = 0; i < MAX_CTRL_TIMER_NUM; i++) {
            memset(&buf_tmp[0], 0x00, sizeof(buf_tmp));
            snprintf(reinterpret_cast<char *>(&buf_tmp[0]), sizeof(buf_tmp),
                    "\n [%d] cid:%05d, hTim:%10p, sz:%05d, pno:0x%04x, seq:0x%04x, typ:%03d, tOut:%10d",
                    i,
                    g_timer_ctrl_tbl[i].cid,
                    g_timer_ctrl_tbl[i].h_timer,
                    g_timer_ctrl_tbl[i].size,
                    g_timer_ctrl_tbl[i].pno,
                    g_timer_ctrl_tbl[i].seq_no,
                    g_timer_ctrl_tbl[i].type,
                    g_timer_ctrl_tbl[i].time_out);
            strncat(reinterpret_cast<char *>(&buf[0]), reinterpret_cast<char *>(&buf_tmp[0]), \
                strlen(reinterpret_cast<char *>(&buf_tmp[0])));
        }
        memcpy(p_buf, &buf[0], sizeof(buf));
    }
}  // LCOV_EXCL_BR_LINE 10:The final line
