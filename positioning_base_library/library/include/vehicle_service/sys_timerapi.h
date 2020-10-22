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
#ifndef POSITIONING_BASE_LIBRARY_LIBRARY_INCLUDE_VEHICLE_SERVICE_SYS_TIMERAPI_H_
#define POSITIONING_BASE_LIBRARY_LIBRARY_INCLUDE_VEHICLE_SERVICE_SYS_TIMERAPI_H_

/**
 * @file sys_timerapi.h
 * @brief Header file for timer management
 */

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup vehicle_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup positioning_base_library
 *  @ingroup vehicle_service
 *  @{
 */

/*---------------------------------------------------------------------------------*
 * Definition                                                                      *
 *---------------------------------------------------------------------------------*/

#define WTM_DUPTIME_MAX   5  //!< \~english duplication timer max count

#define  TIMER_TYPE_SYN   1
//!< \~english fixing cycle timer(unit: 100ms)
#define  TIMER_TYPE_USN   2
//!< \~english asynchronous timer(unit: 100ms)
#define  TIMER_TYPE_ALM   5  //!< \~english specific time timer

#define  SUN              0    //!< \~english sunday
#define  MON              1    //!< \~english monday
#define  TUE              2    //!< \~english tuesday
#define  WED              3    //!< \~english wednesday
#define  THU              4    //!< \~english thursday
#define  FRI              5    //!< \~english friday
#define  SAT              6    //!< \~english saturday
#define DEFAULT_DIFFTIME  0x00  //!< \~english time difference value of japan

#define  GPS_SAT          0x00  //!< \~english corrected GPS time
#define  GPS_IST          0x01  //!< \~english not corrected GPS time
#define  GPS_RTC          0x02  //!< \~english RTC time
#define  GPS_IRG          0x03  //!< \~english initial time
#define GPS_NON           0x7E  //!< \~english GPS unused
#define  GPS_IGN          0x7F  //!< \~english time not received

#define END_TIMER_RPT_OFF 0     //!< \~english No continuation
#define END_TIMER_RPT_ON  1     //!< \~english With continuation

#define  CID_TIMER_SETGPS 0x4040
//!< \~english command ID of GPS setting timer
#define  CID_TIMER_DIFSET 0x4045
//!< \~english command ID of time difference setting timer
#define  CID_TIMER_CYCLE  (CID)(CID_TIMER_BASE | CID_COM_SERIAL5)
//!< \~english command ID of cycle data receive timer

#define  CNV_YEAR_MAX     136
//!< \~english max value of conversion year(number of years + 1)
#define  SEC_PER_MIN      60  //!< \~english 60[sec]
#define  SEC_PER_HOUR     3600          //!< \~english 60[min] * 60[sec]
#define  SEC_PER_DAY      86400  //!< \~english 24[h] * 60[min] * 60[sec]

#define TIM_ROLOVR_DISABLE 0  //!< \~english GPS week rollover correct prohibition
#define TIM_ROLOVR_ENABLE   1  //!< \~english GPS week rollover correct permission

/*
 * Definition
 * \~english timer manager error definition
 */
#define RET_ERR_NONEINIT  (-30)  //!< \~english initialization untreated error
#define RET_ERR_SEMLOCK   (-31)  //!< \~english get semaphore error
#define RET_ERR_SEMUNLOCK (-32)  //!< \~english open semaphore error
#define RET_ERR_SRAMREAD  (-33)  //!< \~english read SRAM error

/*
 * Definition
 * \~english command notify information definition(almanac abnormality notify, and time enable notify)
 */
#define TIM_NUM_SNDCMD    4    //!< \~english command send number

#define NON_TIMEUNIT_INVALID_TIME  0
//!< \~english No time manager unit - system time not set
#define NON_TIMEUNIT_VALID_TIME    1
//!< \~english No time manager unit - system time set complete
#define GPS_INVALID_TIME           2
//!< \~english With GPS unit - system time not set
#define GPS_VALID_TIME             3
//!< \~english With GPS unit - system time set complete

#define TMT_RETRY                  3    //!< \~english retry count
#define TMT_RETRY_INTERVAL         500  //!< \~english retry interval(500[msec])


/*---------------------------------------------------------------------------------*
 * Struct declaration                                                              *
 *---------------------------------------------------------------------------------*/

/**
 * \~english define data type
 */
typedef struct {
  u_int32    TimeOut[WTM_DUPTIME_MAX];  //!< \~english timeout value
  u_int8     ContFlg;  //!< \~english continue flag
  u_int8     Dummy1;  //!< \~english dummy data
  u_int16    Dummy2;  //!< \~english dummy data
} TimerDupData;

/**
 * @struct RealTimeData
 * \~english current time data
 */
typedef struct {
  u_int16    Yyear;    //!< \~english current time(year:A.D.)
  u_int8     Mmonth;   //!< \~english (month)
  u_int8     Dday;     //!< \~english (day)
  u_int8     Hhour;    //!< \~english (hour)
  u_int8     Mmin;     //!< \~english (min)
  u_int8     Ssec;     //!< \~english (sec)
  u_int8     Wweek;    //!< \~english (week)
  u_int8     CrtFlg;   //!< \~english GPS time not corrected/corrected flag
  u_int8     CrtHou;   //!< \~english the last correction(hour)
  u_int8     CrtMin;   //!< \~english min
  u_int8     CrtSec;   //!< \~english sec
} RealTimeData;

/**
 * @struct RealTimeDataBcd
 * \~english current time data(BCD format)
 */
typedef struct {
  u_int16    Yyear;  //!< \~english current time(year:A.D.BCD ex.0x1998)
  u_int8     Mmonth;  //!< \~english (month            BCD 0x01 to 0x12)
  u_int8     Dday;   //!< \~english (day              BCD 0x01 to 0x31)
  u_int8     Hhour;  //!< \~english (hour             BCD 0x00 to 0x23)
  u_int8     Mmin;   //!< \~english (min              BCD 0x00 to 0x59)
  u_int8     Ssec;   //!< \~english (sec              BCD 0x00 to 0x59)
  u_int8     Wweek;  //!< \~english (week             BIN 0x00 to 0x06)
  u_int8     CrtFlg;
  //!< \~english GPS time not corrected or corrected flag(BIN 0x00 to 0x03)
  u_int8     CrtHou;   //!< \~english the last correction(hour BCD 0x00 to 0x23)
  u_int8     CrtMin;   //!< \~english (min              BCD 0x00 to 0x59)
  u_int8     CrtSec;   //!< \~english (sec              BCD 0x00 to 0x59)
} RealTimeDataBcd;

/**
 * @struct GpsSetData
 * \~english GPS time setting data
 */
typedef struct {
  u_int16    Yyear;    //!< \~english current time(year:A.D.)
  u_int8     Mmonth;   //!< \~english (month)
  u_int8     Dday;     //!< \~english (day)
  u_int8     Hhour;    //!< \~english (hour)
  u_int8     Mmin;     //!< \~english (min)
  u_int8     Ssec;     //!< \~english (sec)
  u_int8     Dummy;    //!< \~english dummy data
} GpsSetData;

/**
 * @struct TimerToutMsg
 * \~english timeout message
 */
typedef struct {
  T_APIMSG_MSGBUF_HEADER  Header;    //!< \~english message header
  u_int16    TimerSeq;  //!< \~english timer sequence No.(claimant specific)
  u_int16    Dummy;     //!< \~english dummy
} TimerToutMsg;

/**
 * @struct TimerTimeReq
 * \~english timer start request
 */
typedef struct {
  T_APIMSG_MSGBUF_HEADER  Header;    //!< \~english message header
  PNO        SndPno;    //!< \~english process No.(claimant specific)
  u_int16    TimerSeq;  //!< \~english timer sequence No.(claimant specific)
  u_int8     TimeType;  //!< \~english timer type
  u_int8     Dummy[3];  //!< \~english dummy data
  TimerDupData  DupData;  //!< \~english timeout value
} TimerTimeReq;

/**
 * @struct TimerAlrmReq
 * \~english time specified timer start request
 */
typedef struct {
  T_APIMSG_MSGBUF_HEADER  Header;    //!< \~english message header
  PNO        SndPno;    //!< \~english process No.(claimant specific)
  u_int16    TimerSeq;  //!< \~english timer sequence No.(claimant specific)
  u_int8     TimeHou;   //!< \~english specific timeout hour
  u_int8     TimeMin;   //!< \~english                   min
  u_int8     TimeSec;   //!< \~english                   sec
  u_int8     Dummy;     //!< \~english dummy
} TimerAlrmReq;

/**
 * @struct TimerStopReq
 * \~english timer stop request
 */
typedef struct {
  T_APIMSG_MSGBUF_HEADER  Header;    //!< \~english message header
  PNO        SndPno;    //!< \~english process No.(claimant specific)
  u_int16    TimerSeq;  //!< \~english timer sequence No.(claimant specific)
  u_int8     TimeType;  //!< \~english timer type
  u_int8     Dummy[3];  //!< \~english dummy data
} TimerStopReq;

/**
 * @struct GpsSetTimeReq
 * \~english GPS RTC time set request
 */
typedef struct {
  T_APIMSG_MSGBUF_HEADER  Header;    //!< \~english message header
  PNO        SndPno;   //!< \~english process No.(claimant specific)
  u_int8     Dummy1;   //!< \~english dummy data
  u_int8     Dummy2;   //!< \~english dummy data
  GpsSetData SetData;  //!< \~english GPS setting data
} GpsSetTimeReq;

/**
 * @struct GpsSetTimeReq
 * \~english time difference set request
 */
typedef struct {
  T_APIMSG_MSGBUF_HEADER  Header;    //!< \~english message header
  int8       DiffTime;  //!< \~english time difference value
  int8       Dummy1;    //!< \~english dummy data
  int16      Dummy2;    //!< \~english dummy data
} SetDiffTimeReq;


/**
 * @struct T_TIM_RCV_DATA
 * \~english command notify information structure
 */
typedef struct {
  PNO        SndPno;   //!< \~english command delivery PNO
  u_int8     ucAlmanc_Flg;
  //!< \~english almanac abnormal judgement notify complete flag
  u_int8     ucInvalid_Flg;
  //!< \~english time acquisition notify complete flag
} T_TIM_RCV_DATA;

/**
 * @struct TimerSramData
 * \~english timer SRAM data structure
 */
typedef struct {
  u_int32    navitime;   //!< \~english Navi operation addition time
  int8       _difftime;  //!< \~english time difference value of current time
  int8       timeUnit;   //!< \~english specific time manager unit
  int8       dummy[2];   //!< \~english dummy data
  T_TIM_RCV_DATA  timCmdData[TIM_NUM_SNDCMD];
  //!< \~english command receive information
} TimerSramData;

/**
 * @struct SysTimeData
 * \~english system time data
 */
typedef struct {
  u_int16    Yyear;    //!< \~english current time(year:A.D.BCD ex. 0x1998)
  u_int8     Mmonth;   //!< \~english (month      BCD 0x01 to 0x12)
  u_int8     Dday;     //!< \~english (day        BCD 0x01 to 0x31)
  u_int8     Hhour;    //!< \~english (hour       BCD 0x00 to 0x23)
  u_int8     Mmin;     //!< \~english (min        BCD 0x00 to 0x59)
  u_int8     Ssec;     //!< \~english (sec        BCD 0x00 to 0x59)
  u_int8     Wweek;    //!< \~english (week       BIN 0x00 to 0x06)
} SysTimeData;

/**
 * @struct YearCntTbl
 * \~english year convert table
 */
typedef struct TimeCnvTbl_tag {
  uint32_t  ulYear;       //!< \~english year
  uint32_t  ulCount;       //!< \~english count
  uint32_t  ulMonth[12];       //!< \~english month
}  YearCntTbl;

/**
 * @struct DayCntTbl
 * \~english day convert table
 */
typedef struct DayCnvTbl_tag {
  YearCntTbl    stTbl[CNV_YEAR_MAX];       //!< \~english year convert table
} DayCntTbl;

/**
 * @struct TG_TIM_ROLOVR_GPSWEEKCORDATA
 * \~english GPS week correction data
 */
typedef struct {
  u_int8 gpsweekcorcnt;       //!< \~english GPS week rollover correct count
  u_int8 permission;       //!< \~english GPS week rollover correct permission
  u_int8 dummy[2];    //!< \~english dummy data
} TG_TIM_ROLOVR_GPSWEEKCORDATA;

/**
 * @struct TIMECHGIND_MSG
 * \~english time change information message structure
 */
typedef struct {
  T_APIMSG_MSGBUF_HEADER  Header;    //!< \~english message header
  RealTimeData    RealTime;  //!< \~english data body(current time data)
} TIMECHGIND_MSG;

/**
 * @struct T_TIM_CMDMSG
 * \~english time available notification/almanac abnormality notification structure
 */
typedef struct {
  T_APIMSG_MSGBUF_HEADER  Header;    //!< \~english message header
} T_TIM_CMDMSG;

/**
 * @struct T_TIM_RETTIMEMSG
 * \~english time setting response message
 */
typedef struct {
  T_APIMSG_MSGBUF_HEADER  Header;    //!< \~english message header
  int8            cResult;  //!< \~english process result
  int8            dummy[2];       //!< \~english dummy data
} T_TIM_RETTIMEMSG;

/**
 * @struct TimerModDat
 * \~english current time data module structure
 */
typedef struct {
  u_int32    InitFlg;  //!< \~english initialization complete flag
  u_int8     CrtFlg;   //!< \~english corrected/not corrected flag
  u_int8     CrtHou;   //!< \~english the last correction: hour
  u_int8     CrtMin;   //!< \~english                       min
  u_int8     CrtSec;   //!< \~english                       sec
} TimerModDat;

/** @}*/  // end of positioning_base_library
/** @}*/  // end of vehicle_service
/** @}*/  // end of BaseSystem
#endif  // POSITIONING_BASE_LIBRARY_LIBRARY_INCLUDE_VEHICLE_SERVICE_SYS_TIMERAPI_H_

