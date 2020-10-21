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
* @file MDev_Gps_TimerCtrl.cpp
*/

/*---------------------------------------------------------------------------*/
// Include files

#include  "MDev_Gps_TimerCtrl.h"

// #include  "MDev_Gps_Main.h"
// #include  "MDev_GpsRecv.h"

/*---------------------------------------------------------------------------*/
// Global values

static GPS_TIM_MNG          g_st_tim_mng;

/** Timer setting information table */
static const GPS_TIM_INFO   kTimInfo[TIM_NUM] = {
  /* GSP related */
  {TIMVAL_GPS_STARTUP,        PNO_NAVI_GPS_MAIN},  /* Start confirmation monitoring timer                           */
  {TIMVAL_GPS_RCVCYCLDAT,     PNO_NAVI_GPS_MAIN},  /* Periodic reception data monitoring timer                    */
  {TIMVAL_GPS_RCVACK,         PNO_NAVI_GPS_MAIN},  /* ACK reception monitoring timer                          */
  {TIMVAL_GPS_NAVIFST,        PNO_NAVI_GPS_MAIN},  /* Initial Navigation Monitoring Timer                           */
  {TIMVAL_GPS_NAVICYCLE,      PNO_NAVI_GPS_MAIN},  /* Navi monitoring timer                             */
  {TIMVAL_GPS_NAVIDISRPT,     PNO_NAVI_GPS_MAIN},  /* Navigation Monitoring Disruption Log Output Timer                   */
  {TIMVAL_GPS_DIAGCLKGUARD,   PNO_NAVI_GPS_MAIN},  /* Diagnosis provision time guard monitoring timer                */
  {TIMVAL_GPS_NMEADATAGUARD,  PNO_NAVI_GPS_MAIN},  /* NMEA data providing guard monitoring timer               */
  {TIMVAL_GPS_RECOVERY,       PNO_NAVI_GPS_MAIN},  /* GPS recovery timer                                */
  {TIMVAL_GPS_RECEIVERERR,    PNO_NAVI_GPS_MAIN},  /* GPS receiver anomaly detection timer                   */
  /* Sensor related */
  {TIMVAL_SNS_RCVFSTDAT,      PNO_VEHICLE_SENSOR}, /* Initial cyclic sensor data reception monitoring timer           */
  {TIMVAL_SNS_RCVCYCLDAT,     PNO_VEHICLE_SENSOR}, /* Cyclic sensor data reception monitoring timer             */
  {TIMVAL_SNS_RCVDISRPT,      PNO_VEHICLE_SENSOR}, /* Cyclic sensor data interruption log output timer           */
};

/*---------------------------------------------------------------------------*/
// Functions

static uint16_t TimeMakSeqNo(GPS_TIM_KIND tim_kind) {
  GPS_TIM_MNG*            pst_tim_mng = NULL;
  u_int16                 seq_no = 0;         /* Timer sequence number */

  pst_tim_mng = &g_st_tim_mng;

  /*------------------------------------------------------------------------*/
  /* Definition of Sequence Number                                                   */
  /* |-------------------  Sequence number(2Byte)  -----------------------|  */
  /* 15                             8 7                                  0  */
  /* +-------------------------------+-----------------------------------+  */
  /* |      Timer type(1Byte)        |   Counter(1Byte)(0x01 ? 0xFF)   |  */
  /* +-------------------------------+-----------------------------------+  */
  /* The timer type is 0x00. ? (Number of timers-1)                                     */
  /* counters is 0x01 ? 0xFF(Do not use 0x00)                             */
  /* (Counters are counted up each time a timer is started.                  */
  /*  If the counter counts up when it is 0xFF,                          */
  /*  be counted up from the 0x01.)                                         */
  /*------------------------------------------------------------------------*/
  seq_no = ((u_int16)tim_kind << 8) | (pst_tim_mng->sts[tim_kind].cnt);

  return seq_no;
}

static BOOL VehicleUtilitySetTimer(GPS_TIM_KIND tim_kind) {
  GPS_TIM_MNG*            pst_tim_mng = NULL;
  const uint32_t *        p_time_val;
  const PNO*              p_pno;
  RET_API                 api_ret = RET_NORMAL;
  u_int16                 seq_no = 0;
  BOOL                    ret = TRUE;

  // Initialize
  pst_tim_mng = &g_st_tim_mng;
  p_time_val  = &(kTimInfo[tim_kind].timer_val);  /* Timer set value */
  p_pno       = &(kTimInfo[tim_kind].pno);        /* Notify party PNO */

  if (pst_tim_mng->sts[tim_kind].flag == TIMER_ON) {
    /*-----------------------------------------------------------------------*/
    /* When the same timer has already started,                                    */
    /* terminate without starting the timer because the timer is set multiple times.            */
    /*-----------------------------------------------------------------------*/
    ret = FALSE;
  } else {
    /*-----------------------------------------------------------------------*/
    /* Count up the timer counter of the corresponding timer by 1.                     */
    /*-----------------------------------------------------------------------*/
    if (pst_tim_mng->sts[tim_kind].cnt >= TIM_CNTMAX) {
      /*-----------------------------------------------------------------------*/
      /* When the count reaches the maximum number, it counts again from 1.                 */
      /*-----------------------------------------------------------------------*/
      pst_tim_mng->sts[tim_kind].cnt = TIM_CNTMIN;
    } else {
      /*-----------------------------------------------------------------------*/
      /* If the count has not reached the maximum, it is counted up.              */
      /*-----------------------------------------------------------------------*/
      pst_tim_mng->sts[tim_kind].cnt++;
    }

    /*-----------------------------------------------------------------------*/
    /* Creating timer sequence numbers                                            */
    /*-----------------------------------------------------------------------*/
    seq_no = TimeMakSeqNo(tim_kind);

    /*-----------------------------------------------------------------------*/
    /* Start the timer                                                      */
    /*-----------------------------------------------------------------------*/
    api_ret = _pb_ReqTimerStart(*p_pno, seq_no, TIMER_TYPE_USN, static_cast<u_int32>(*p_time_val));
    if (api_ret != RET_NORMAL) {
      ret = FALSE;
    } else {
      /*-----------------------------------------------------------------------*/
      /* If successful timer start,                                            */
      /* set the start/stop flag of the corresponding timer to start (MCSUB_ON).               */
      /*-----------------------------------------------------------------------*/
      pst_tim_mng->sts[tim_kind].flag = TIMER_ON;
    }
  }

  return ret;
}

static BOOL VehicleUtilityStopTimer(GPS_TIM_KIND tim_kind) {
  GPS_TIM_MNG*            pst_tim_mng = NULL;
  const PNO*              p_pno;
  BOOL                    ret = TRUE;
  RET_API                 api_ret = RET_NORMAL;
  u_int16                 seq_no = 0;

  // Initialize
  pst_tim_mng = &g_st_tim_mng;
  p_pno = &(kTimInfo[tim_kind].pno); /* Notify party PNO */

  /* Check timer start/stop flag   */
  if (pst_tim_mng->sts[tim_kind].flag == TIMER_OFF) {
    /*  If it is already stopped, do nothing.  */
    ret = FALSE;
  } else {
    /*-----------------------------------------------------------------------*/
    /* Creating timer sequence numbers                                            */
    /*-----------------------------------------------------------------------*/
    seq_no = TimeMakSeqNo(tim_kind);

    /*-----------------------------------------------------------------------*/
    /* Set the corresponding timer to stop                                          */
    /*-----------------------------------------------------------------------*/
    api_ret = _pb_TimerStop(*p_pno, seq_no, TIMER_TYPE_USN);

    if (api_ret != RET_NORMAL) {
      ret = FALSE;
    }

    /*-----------------------------------------------------------------------*/
    /* Set the start/stop flag of the corresponding timer to stop (MCSUB_OFF)              */
    /* Set the ID of the corresponding timer to invalid (DEV_TED_INVALID)                   */
    /*-----------------------------------------------------------------------*/
    pst_tim_mng->sts[tim_kind].flag = TIMER_OFF;
  }

  return ret;
}

static BOOL VehicleUtilityTimeJdgKnd(uint16_t seqno) {
  GPS_TIM_MNG*            pst_tim_mng = NULL;
  BOOL                    ret = FALSE;
  u_int8                  timekind = 0;
  u_int8                  count = 0;

  // Initialize
  pst_tim_mng = &g_st_tim_mng;

  timekind = (u_int8)((seqno & 0xff00) >> 8);
  count = (u_int8)(seqno & 0x00ff);

  /* Timer type is unexpected        */
  if (timekind >= TIM_NUM) {
    ret = FALSE;
  } else {
    if ((pst_tim_mng->sts[timekind].cnt == count) && (pst_tim_mng->sts[timekind].flag == TIMER_ON)) {
      /* The counter matches and the counter start/stop flag is "Start".      */
      ret = TRUE;
    } else {
      /* Not applicable due to differences                                  */
      ret = FALSE;
    }
  }

  return ret;
}

/*******************************************************************************
 * MODULE  : DEV_Gps_Tim_Init
 * ABSTRACT  : Timer function initialization processing
 * FUNCTION  : Initialize the timer function 
 * ARGUMENT  : None
 * NOTE    :  1.Initialize timer management table
 * RETURN  : None
 ******************************************************************************/
void  DevGpsTimInit(void) {
  GPS_TIM_MNG*            pst_tim_mng = NULL;
  u_int32                 i = 0;

  // Initialie
  pst_tim_mng = &g_st_tim_mng;

  /* Initialize timer management table */
  memset(pst_tim_mng, 0x00, sizeof(GPS_TIM_MNG));

  for (i = 0; i < TIM_NUM; i++) {
    pst_tim_mng->sts[i].flag = TIMER_OFF;
    pst_tim_mng->sts[i].cnt = 0;
  }

  return;
}

/*******************************************************************************
 * MODULE   : DevGpsTimeSet
 * ABSTRACT : Timer start processing
 * FUNCTION : Starts a timer of the specified type 
 * ARGUMENT : GPS_TIM_KIND tim_kind  Timer type
 * NOTE     :  1.Increment total number of timer start
 *             2.Timer Sequence Number Creation
 *             3.Get timeout value
 *             4.Timer start
 * RETURN   : TRUE : Normal completion
 *          : FALSE : ABENDs
 ******************************************************************************/
BOOL  DevGpsTimeSet(GPS_TIM_KIND tim_kind) {
  BOOL ret = TRUE;

  /* Binding of unused timer */
  if ((tim_kind != GPS_RECV_ACK_TIMER)
    && (tim_kind != GPS_STARTUP_TIMER)
    && (tim_kind != GPS_CYCL_TIMER)
    && (tim_kind != GPS_NAVIFST_TIMER)
    && (tim_kind != GPS_NAVICYCLE_TIMER)
    && (tim_kind != GPS_NAVIDISRPT_TIMER)
    && (tim_kind != GPS_RECOVERY_TIMER)
    && (tim_kind != GPS_RECEIVERERR_TIMER)) {
    return ret;
  }
  ret = VehicleUtilitySetTimer(tim_kind);
  return ret;
}

/*******************************************************************************
 * MODULE  : DevGpsTimeStop
 * ABSTRACT  : Timer stop processing
 * FUNCTION  : Stops a timer of the specified type 
 * ARGUMENT  : GPS_TIM_KIND tim_kind  Timer type
 * NOTE    :  1.Get the sequence number of the specified type
 *         2.Timer stop
 * RETURN  : TRUE : Normal completion
 *      : FALSE : ABENDs
 ******************************************************************************/
BOOL  DevGpsTimeStop(GPS_TIM_KIND tim_kind) {
    BOOL ret = TRUE;
    ret = VehicleUtilityStopTimer(tim_kind);
    return ret;
}

/********************************************************************************
 * MODULE      : DevGpsTimeJdgKind
 * ABSTRACT    : Timer Sequence Number Determination
 * FUNCTION    : Determine whether the timer sequence number corresponds to the one being managed
 * ARGUMENT    : Timer Sequence Number
 * NOTE        :
 * RETURN      : TRUE : Normal completion(No problem)
 *             : FALSE : ABENDs(Unusual number)
 ********************************************************************************/
BOOL DevGpsTimeJdgKind(u_int16 seqno) {
    BOOL ret;
    ret = VehicleUtilityTimeJdgKnd(seqno);
    return ret;
}

/*---------------------------------------------------------------------------*/
/*EOF*/
