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
* @file MDev_Gps_TimerCtrl.h
*/

#ifndef  INC_GPSCOMMON_MDEV_GPS_TIMERCTRL_H_
#define  INC_GPSCOMMON_MDEV_GPS_TIMERCTRL_H_

/*---------------------------------------------------------------------------*/
// Include files

#include "positioning_def.h"

/*---------------------------------------------------------------------------*/
// Value define

/* GPS-related timer value */
#define TIMVAL_GPS_STARTUP        500     /* 5Sec  Start confirmation monitoring timer              */
#define TIMVAL_GPS_RCVCYCLDAT     500     /* 5Sec  Periodic reception data monitoring timer       */
#define TIMVAL_GPS_RCVACK         500     /* 5Sec  ACK reception monitoring timer             */
#define TIMVAL_GPS_RCVDAT         500     /* 5Sec  Data reception monitoring timer(Unused)    */
#define TIMVAL_GPS_NAVIFST        3000    /* 30sec Initial navigation monitoring timer              */
#define TIMVAL_GPS_NAVICYCLE      300     /* 3sec  Navigation monitoring timer                    */
#define TIMVAL_GPS_NAVIDISRPT     1000    /* 10Sec  Navigation monitoring disruption log output timer */
#define TIMVAL_GPS_DIAGCLKGUARD   1000    /* 10sec Diagnosis provision time guard monitoring timer   */
#define TIMVAL_GPS_NMEADATAGUARD  1000    /* 10sec NMEA data-providing guard monitoring timer  */
#define TIMVAL_GPS_RECOVERY       60000   /* 600sec GPS recovery timer                  */
#define TIMVAL_GPS_RECEIVERERR    60000   /* 600sec GPS receiver anomaly detection timer     */

/* Sensor-related timer value */
#define TIMVAL_SNS_RCVFSTDAT      3000    /* 30Sec Initial sensor data reception monitoring timer */
#define TIMVAL_SNS_RCVCYCLDAT     300     /* 3Sec  Cyclic sensor data reception monitoring timer */
#define TIMVAL_SNS_RCVDISRPT      1000    /* 10Sec  Cyclic sensor data interruption log output timer */

/* Timer management table */
#define TIM_NON                   0x00    /* Timer counter initial value                   */
#define TIM_CNTMIN                0x01    /* Timer counter minimum value                   */
#define TIM_CNTMAX                0xff    /* Maximum value of timer counter                   */
#define TIMER_OFF                 0       /* Timer enable flag OFF                  */
#define TIMER_ON                  1       /* Timer enable flag ON                   */

/*!
   @brief   Timer type
 */
typedef enum GpsTimKind {
  GPS_STARTUP_TIMER = 0,      /*  0 Start confirmation monitoring timer                       */
  GPS_CYCL_TIMER,             /*  1 Cyclic GPS data reception monitoring timer                 */
  GPS_RECV_ACK_TIMER,         /*  2 ACK reception monitoring timer                              */
  GPS_NAVIFST_TIMER,          /*  3 Initial navigation monitoring timer                       */
  GPS_NAVICYCLE_TIMER,        /*  4 Navigation monitoring timer                             */
  GPS_NAVIDISRPT_TIMER,       /*  5 Navigation monitoring disruption log output timer               */
  GPS_DIAGCLK_GUARDTIMER,     /*  6 Diagnosis provision time guard monitoring timer                */
  GPS_NMEADATA_GUARDTIMER,    /*  7 NMEA data-providing guard monitoring timer           */
  GPS_RECOVERY_TIMER,         /*  8 GPS recovery timer                               */
  GPS_RECEIVERERR_TIMER,      /*  9 GPS receiver anomaly detection timer                   */
  SNS_FST_TIMER,              /* 10 Initial sensor data reception monitoring timer             */
  SNS_CYCLE_TIMER,            /* 11 Cyclic sensor data reception monitoring timer             */
  SNS_DISRPT_TIMER,           /* 12 Cyclic sensor data interruption log output timer       */
  TIM_NUM                     /* 13 Number of timer types                                  */
} GPS_TIM_KIND;

/*!
   @brief   Timer status
*/
typedef struct GpsTimSts {
  u_int8  flag;               /**< Timer flag OFF:Stop,ON:Start */
  u_int8  cnt;                /**< Start counter                     */
  int8    rsv[2];             /**< Reserved                           */
} GPS_TIM_STS;

/*!
   @brief   Timer status management table
 */
typedef struct GpsTimMng {
  GPS_TIM_STS  sts[TIM_NUM];  /**< Timer status         */
} GPS_TIM_MNG;

/*!
   @brief   Timer setting information table
*/
typedef struct GpsTimInfo {
  uint32_t  timer_val;        /**< Timer value                           */
  PNO       pno;              /**< Event notification destination process number         */
} GPS_TIM_INFO;

/*---------------------------------------------------------------------------*/
// Prototype

void    DevGpsTimInit(void);
BOOL    DevGpsTimeSet(GPS_TIM_KIND tim_kind);
BOOL    DevGpsTimeStop(GPS_TIM_KIND tim_kind);
u_int16 DevGpsTimeMakSeqNo(GPS_TIM_KIND tim_kind);
BOOL    DevGpsTimeJdgKind(u_int16 seqno);

/*---------------------------------------------------------------------------*/
#endif  // INC_GPSCOMMON_MDEV_GPS_TIMERCTRL_H_

/*---------------------------------------------------------------------------*/
/*EOF*/
