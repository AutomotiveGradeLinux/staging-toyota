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

/***************************************************************************
@file            VehicleUtility.cpp
@detail            Common processing function of Vehicle
*****************************************************************************/

#include "VehicleUtility.h"
#include <vehicle_service/positioning_base_library.h>

//#include "gps_hal.h"
#include "positioning_common.h"


/*---------------------------------------------------------------------------------*
 * Grobal Value                                                                    *
 *---------------------------------------------------------------------------------*/
/** Timer management table */
static VEHICLEUTILITY_TIM_MNG   g_st_tim_mng;

/** Timer setting information table */
static const VEHICLEUTILITY_TIM_INFO g_tim_info[TIM_NUM] = {
    /* GSP-related */
    {TIMVAL_GPS_STARTUP,        PNO_NAVI_GPS_MAIN},   /* Start confirmation monitoring timer                */
    {TIMVAL_GPS_RCVCYCLDAT,     PNO_NAVI_GPS_MAIN},   /* Periodic reception data monitoring timer           */
    {TIMVAL_GPS_RCVACK,         PNO_NAVI_GPS_MAIN},   /* ACK reception monitoring timer                */
    {TIMVAL_GPS_NAVIFST,        PNO_NAVI_GPS_MAIN},   /* Initial Navigation Monitoring Timer                */
    {TIMVAL_GPS_NAVICYCLE,      PNO_NAVI_GPS_MAIN},   /* Navi monitoring timer                   */
    {TIMVAL_GPS_NAVIDISRPT,     PNO_NAVI_GPS_MAIN},   /* Navigation Monitoring Disruption Log Output Timer         */
    {TIMVAL_GPS_DIAGCLKGUARD,   PNO_NAVI_GPS_MAIN},   /* Diag provision time guard monitoring timer       */
    {TIMVAL_GPS_NMEADATAGUARD,  PNO_NAVI_GPS_MAIN},   /* NMEA data-providing guard monitoring timer     */
    {TIMVAL_GPS_RECOVERY,       PNO_NAVI_GPS_MAIN},   /* GPS recovery timer                    */
    {TIMVAL_GPS_RECEIVERERR,    PNO_NAVI_GPS_MAIN},   /* GPS receiver anomaly detection timer          */
    /* Sensor Related Extensions */
    {TIMVAL_SNS_RCVFSTDAT,      PNO_VEHICLE_SENSOR},  /* Initial cyclic sensor data reception monitoring timer  */
    {TIMVAL_SNS_RCVCYCLDAT,     PNO_VEHICLE_SENSOR},  /* Cyclic sensor data reception monitoring timer      */
    {TIMVAL_SNS_RCVDISRPT,      PNO_VEHICLE_SENSOR},  /* Cyclic sensor data interruption log output timer   */
};

/*---------------------------------------------------------------------------------*
 * Prototype                                                                       *
 *---------------------------------------------------------------------------------*/
static uint16_t VehicleUtilityTimeMakSeqNo(VEHICLEUTILITY_TIM_KIND tim_kind);

/***************************************************************************
@brief            send message function for Vehicle domain.
@outline        send message function with put error diag function if error occurred.
@type            Completion return type
@param[in]        PNO pno               :    PNO
@param[in]        u_int16 size          :    size of message data
@param[in]        void* msgbuf          :    message data
@param[in]        u_int16 mode          :    mode
@threshold
@return            RET_API
@retval            RET_NORMAL           :    Normal end
@retval            RET_ERROR            :    Abnormal end
*****************************************************************************/
RET_API VehicleUtilitySndMsg(PNO pno, u_int16 size, void *msgbuf, u_int16 mode) {
    RET_API        ret_api;
    if (msgbuf != NULL) {  // LCOV_EXCL_BR_LINE 6: msgbuf cannot be null
        ret_api = _pb_SndMsg(pno, size, msgbuf, mode);
        /* RET_ERROR: Execute _pb_Exit() after dialog registration */
        if (ret_api == RET_ERROR) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_SndMsg ERROR [ret_api:%d]", ret_api);
        }
    } else {
        ret_api = RET_ERRPARAM;
    }

    return ret_api;
}

/***************************************************************************
@brief            send message function for Vehicle domain.
@outline        receive message function with put error diag function if error occurred.
@type            Completion return type
@param[in]        PNO pno               :    PNO
@param[in]        u_int16 size          :    size of message data
@param[in]        void* msgbuf          :    message data
@param[in]        u_int16 mode          :    mode
@threshold
@return            RET_API
@retval            RET_NORMAL           :    Normal end
@retval            RET_ERROR            :    Abnormal end
*****************************************************************************/
RET_API VehicleUtilityRcvMsg(PNO pno, u_int16 size, void **msgbuf, u_int16 mode)
{
    RET_API        ret_api;

    if (msgbuf != NULL) {  // LCOV_EXCL_BR_LINE 6: msgbuf cannot be null
        ret_api = _pb_RcvMsg(pno, size, msgbuf, mode);
        /* For RET_ERROR Sys_Exit() after dialog registration */
        if (ret_api == RET_ERROR) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_RcvMsg ERROR [ret_api:%d]", ret_api);
        }
    } else {
        ret_api = RET_ERRPARAM;
    }

    return ret_api;
}

/***************************************************************************
@brief            Diagcode submit for Vehicle domain.
@outline        submit the DiagCode for Vehicle domain.
@type            Completion return type
@param[in]        u_int32 err_id        :    Diag Code
@param[in]        u_int16 positioning_code     :    Positioning Code
@param[in]        void* msgbuf          :    message data
@return            void
*****************************************************************************/
void VehicleUtilityDiagCodePut(u_int32 err_id, u_int16 positioning_code) {  // LCOV_EXCL_START 8 : dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
}
// LCOV_EXCL_STOP

/**
 * @brief
 *   Timer function initialization processing
 *
 */
void VehicleUtilityInitTimer(void) {
    VEHICLEUTILITY_TIM_MNG *pst_tim_mng;
    u_int32    i;

    pst_tim_mng = &g_st_tim_mng;

    /* Initialize timer management table */
    memset(pst_tim_mng, 0x00, sizeof(VEHICLEUTILITY_TIM_MNG));

    for (i = 0; i < TIM_NUM; i++) {
        pst_tim_mng->sts[i].flag = TIMER_OFF;
        pst_tim_mng->sts[i].cnt = 0;
    }
}

/**
 * @brief
 *   Timer start processing<br>
 *
 *   Starts a timer of the specified type<br>
 *   1.Increment total number of timer start<br>
 *   2.Timer Sequence Number Creation<br>
 *   3.Get timeout value<br>
 *   4.Timer start<br>
 *
 * @param[in]  Tim_kind Timer type
 *
 * @return TRUE  Normal completion<br>
 *         FALSE abend<br>
 */
BOOL VehicleUtilitySetTimer(VEHICLEUTILITY_TIM_KIND tim_kind) {
    VEHICLEUTILITY_TIM_MNG *pst_tim_mng;
    const uint32_t *p_time_val;
    const PNO   *p_pno;
    RET_API    api_ret;                /* Timer API return value */
    u_int16    seq_no;
    BOOL    ret = TRUE;

    pst_tim_mng = &g_st_tim_mng;

    p_time_val = &(g_tim_info[tim_kind].timer_val); /* Timer set value */
    p_pno = &(g_tim_info[tim_kind].pno); /* Notify party PNO */

    if (pst_tim_mng->sts[tim_kind].flag == TIMER_ON) {
        /*-----------------------------------------------------------------------*/
        /* When the same timer has already started                                    */
        /* Terminate without starting the timer because the timer is set multiple times.            */
        /*-----------------------------------------------------------------------*/
        ret = FALSE;
    } else {
        /*-----------------------------------------------------------------------*/
        /* Count up the timer counter of the corresponding timer by 1.                     */
        /*-----------------------------------------------------------------------*/
        if (pst_tim_mng->sts[tim_kind].cnt >= TIM_CNTMAX) {
            /*-----------------------------------------------------------------------*/
            /* When the count reaches the maximum number,Count again from 1                 */
            /*-----------------------------------------------------------------------*/
            pst_tim_mng->sts[tim_kind].cnt = TIM_CNTMIN;
        } else {
            /*-----------------------------------------------------------------------*/
            /* If the count has not reached the maximum,Count up              */
            /*-----------------------------------------------------------------------*/
            pst_tim_mng->sts[tim_kind].cnt++;
        }

        /*-----------------------------------------------------------------------*/
        /* Creating Timer Sequence Numbers                                            */
        /*-----------------------------------------------------------------------*/
        seq_no = VehicleUtilityTimeMakSeqNo(tim_kind);

        /*-----------------------------------------------------------------------*/
        /* Start the timer                                                      */
        /*-----------------------------------------------------------------------*/
        api_ret = _pb_ReqTimerStart(*p_pno, seq_no, TIMER_TYPE_USN, (u_int32)*p_time_val);
        if (api_ret != RET_NORMAL) {
            ret = FALSE;
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, \
                "_pb_ReqTimerStart ERROR!! [api_ret=%d]", api_ret);
        } else {
            /*-----------------------------------------------------------------------*/
            /* Successful timer start                                            */
            /* Set the start/stop flag of the corresponding timer to start (MCSUB_ON) */
            /*-----------------------------------------------------------------------*/
            pst_tim_mng->sts[tim_kind].flag = TIMER_ON;
        }
    }

    return ret;
}

/**
 * @brief
 *   Timer stop processing<br>
 *
 *   Stops a timer of the specified type<br>
 *   1.Get the sequence number of the specified type<br>
 *   2.Timer stop<br>
 *
 * @param[in]  Tim_kind Timer type
 * @param[in]  pno       Process number
 *
 * @return TRUE  Normal completion<br>
 *         FALSE abend<br>
 */
BOOL VehicleUtilityStopTimer(VEHICLEUTILITY_TIM_KIND tim_kind) {
    VEHICLEUTILITY_TIM_MNG *pst_tim_mng;
    const PNO   *p_pno;
    BOOL    ret = TRUE;
    RET_API    api_ret;
    u_int16    seq_no;

    pst_tim_mng = &g_st_tim_mng;

    p_pno = &(g_tim_info[tim_kind].pno); /* Notify party PNO */

    /* Check timer start/stop flag    */
    if (pst_tim_mng->sts[tim_kind].flag == TIMER_OFF) {
        /*  If it is already stopped, do nothing.    */
        ret = FALSE;
    } else {
        /*-----------------------------------------------------------------------*/
        /* Creating Timer Sequence Numbers                                            */
        /*-----------------------------------------------------------------------*/
        seq_no = VehicleUtilityTimeMakSeqNo(tim_kind);

        /*-----------------------------------------------------------------------*/
        /* Set the corresponding timer to stop                                          */
        /*-----------------------------------------------------------------------*/
        api_ret = _pb_TimerStop(*p_pno, seq_no, TIMER_TYPE_USN);
        if (api_ret != RET_NORMAL) {
            ret = FALSE;
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_TimerStop ERROR!! [api_ret=%d]", api_ret);
        }

        /*-----------------------------------------------------------------------*/
        /* Set the start/stop flag of the corresponding timer to stop (MCSUB_OFF) */
        /* Set the ID of the corresponding timer to invalid (DEV_TED_INVALID)    */
        /*-----------------------------------------------------------------------*/
        pst_tim_mng->sts[tim_kind].flag = TIMER_OFF;
    }
    return ret;
}

/**
 * @brief
 *   Timer Sequence Number Determination<br>
 *
 *   Determine whether the timer sequence number corresponds to the one being managed
 *
 * @param[in]  seqno  Timer Sequence Number
 *
 * @return TRUE  Normal completion(No problem)<br>
 *         FALSE abend(Unusual number)<br>
 */
BOOL VehicleUtilityTimeJdgKnd(uint16_t seqno) {  // LCOV_EXCL_START 8 : dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    VEHICLEUTILITY_TIM_MNG *pst_tim_mng;
    BOOL ret;
    u_int8 timekind;
    u_int8 count;

    pst_tim_mng = &g_st_tim_mng;

    timekind = (u_int8)((seqno & 0xff00) >> 8);
    count = (u_int8)(seqno & 0x00ff);

    /* Timer type is unexpected        */
    if (timekind >= TIM_NUM) {
        ret = FALSE;
    } else {
        if ((pst_tim_mng->sts[timekind].cnt == count) &&
                (pst_tim_mng->sts[timekind].flag == TIMER_ON)) {
            /* The counter matches and the counter start/stop flag is "Start".        */
            ret = TRUE;
        } else {
            /* Not applicable due to differences                                    */
            ret = FALSE;
        }
    }

    return ret;
}
// LCOV_EXCL_STOP

/**
 * @brief
 *   Timer Sequence Number Creation<br>
 *
 *   Creating a Sequence Number for a Timer<br>
 *   1. The upper 1 byte is the timer type.,So that the lower 1 times is the total number of timer starts
 *     Create a sequence number.
 *
 * @param[in]  Tim_kind Timer type
 *
 * @return Timer Sequence Number
 */
static uint16_t VehicleUtilityTimeMakSeqNo(VEHICLEUTILITY_TIM_KIND tim_kind) {
    VEHICLEUTILITY_TIM_MNG *pst_tim_mng;
    u_int16    seq_no;                    /* Timer Sequence Number */

    pst_tim_mng = &g_st_tim_mng;

    /*------------------------------------------------------------------------*/
    /* Definition of Sequence Number                                                   */
    /* |-------------------  Sequence number(2Byte)  -----------------------|  */
    /* 15                             8 7                                  0  */
    /* +-------------------------------+-----------------------------------+  */
    /* |      Timer type(1Byte)        |   Counter(1Byte)(0x01 ? 0xFF)   |  */
    /* +-------------------------------+-----------------------------------+  */
    /* For the timer type,0x00 ? (Number of timers-1)                                     */
    /* For counters,0x01 ? 0xFF(Do not use 0x00.)                             */
    /* (Counters are counted up each time a timer is started.                  */
    /*  Count up when counter is 0xFF,                          */
    /*  Be counted up from the 0x01)                                         */
    /*------------------------------------------------------------------------*/
    seq_no = static_cast<u_int16>(((u_int16)tim_kind << 8) | (pst_tim_mng->sts[tim_kind].cnt));

    return seq_no;
}

/**
 * @brief
 *   External pin status request
 */
void LineSensDrvExtTermStsReq(void) {
    T_APIMSG_MSGBUF_HEADER  st_snd_msg;
    RET_API                 lret;

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "+");

    memset(&st_snd_msg, 0x00, sizeof(st_snd_msg));

    st_snd_msg.hdr.sndpno      = PNO_LINE_SENS_DRV;
    st_snd_msg.hdr.respno      = 0x0000;
    st_snd_msg.hdr.cid         = CID_EXTTERM_REQ;
    st_snd_msg.hdr.msgbodysize = 0x00; /* No data */
    st_snd_msg.hdr.rid         = 0x00;

    /* Messaging */
    lret = _pb_SndMsg(PNO_LINE_SENS_DRV, sizeof(T_APIMSG_MSGBUF_HEADER), &st_snd_msg, 0);
    if (lret != RET_NORMAL) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_SndMsg ERROR!! lret=%d", lret);
        lret = RET_ERROR;
    }

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "-");

    return;
}


/**
 * @brief
 *    Backup data read request send processing<BR>
 *
 * @return  RET_NORMAL    Normal completion
 * @return  RET_ERROR     ABENDs
 */
RET_API DEVGpsSndBackupDataLoadReq(void) {
    RET_API lret;
    T_APIMSG_MSGBUF_HEADER st_snd_msg;

    /** Create GPS Data Notification Message */
    (void)memset(&st_snd_msg, 0x00, sizeof(st_snd_msg)); /* QAC 3200 */
    /** Message header */
    st_snd_msg.hdr.sndpno = 0x0000;
    st_snd_msg.hdr.respno = 0x0000;
//    st_snd_msg.hdr.cid    = CID_GPS_BACKUPDATA_LOAD;
    st_snd_msg.hdr.msgbodysize = 0x00;
    st_snd_msg.hdr.rid    = 0x00;

    /* Messaging */
    lret = _pb_SndMsg(PNO_NAVI_GPS_MAIN, sizeof(st_snd_msg), &st_snd_msg, 0);
    if (lret != RET_NORMAL) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_SndMsg ERROR!! [lret=%d]", lret);
        lret = RET_ERROR;
    }

    return(lret);
}

/**
 * @brief
 *  Acquisition of GPS-format error count information (dump)
 *
 * @param[out]     p_buf    Dump information
 */
void DEVGpsGetDebugGpsFormatFailCnt(void* p_buf) {  // LCOV_EXCL_START 8 : dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    if (p_buf != NULL) {
        snprintf(reinterpret_cast<char *>(p_buf), \
            512, "GPS Format Fail Count is not supported.\n");  // NOLINT(readability/nolint)
    }
    return;
}
// LCOV_EXCL_STOP

/**
 * @brief
 *   Initial sensor data reception flag acquisition
 */
u_int8 LineSensDrvGetSysRecvFlag(void) {  // LCOV_EXCL_START 7: uesd only in FrameworkunifiedOnDebugDump
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return 0;
}
// LCOV_EXCL_STOP

/**
 * @brief
 *   Get GPS Rollover Standard Week Number
 *
 * @return  GPS rollover base week number
 */
uint16_t DEVGpsGetWknRollover(void) {  // LCOV_EXCL_START 7: uesd only in FrameworkunifiedOnDebugDump
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return 0;
}
// LCOV_EXCL_STOP

/* end of file */
