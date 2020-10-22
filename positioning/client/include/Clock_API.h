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

/******************************************************************************
* File name        :Clock_API.h
* System name        :_CWORD107__PND-A
* Subsystem name    :System common functions header file
* Program name    :
* Publishing department
****************************************************************************/
#ifndef POSITIONING_CLIENT_INCLUDE_CLOCK_API_H_
#define POSITIONING_CLIENT_INCLUDE_CLOCK_API_H_

#include <vehicle_service/positioning_base_library.h>
#include "CommonDefine.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 Macro definitions
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* For time calculation */
#define CLOCK_TIMESTAMP_GETTIME(x)   ((x) & (0x00FFFFFFU))             /* Macros that Get Time Counter Values */
#define CLOCK_TIMESTAMP_GETFLAG(x)   (((x) & (0xE0000000U)) >> (29U))  /* Macros that Get Your Own Count Values */
#define CLOCK_TIMESTAMP_NORMAL       (0U)                              /* Normal count */
#define CLOCK_TIMESTAMP_INDIPENDENT  (1U)                              /* Own count */

#define CLOCK_TIMESTAMP_UNION(x, y)   (((x) << (29U)) | (y))  /* Macro that combines the unique count value (x) and the time counter value (y) */

    /* Timestamp Shared Memory Related */
#define TIMESTAMP_RETRY_COUNTE       (10U)               /* Number of time stamp retries */
#define TIMESTAMP_RETRY_INTERVAL     (10U)               /* Time stamp retry interval(100[msec]) */

    /* Time Management Shared Memory Related */
#define CLKMNG_RETRY_COUNTE          (10U)               /* Time management retries */
#define CLKMNG_RETRY_INTERVAL        (10U)               /* Time Management Retry Interval(100[msec]) */

    /* Day of the week definition */
#define CLKMNG_SUN                   (0U)                /* Sunday */
#define CLKMNG_MON                   (1U)                /* Mondays */
#define CLKMNG_TUE                   (2U)                /* Tuesdays */
#define CLKMNG_WED                   (3U)                /* Wednesdays */
#define CLKMNG_THU                   (4U)                /* Thursdays */
#define CLKMNG_FRI                   (5U)                /* Fridays */
#define CLKMNG_SAT                   (6U)

    /* Related to the accuracy of time */
#define CLOCK_CORRECTED_TIME         (0U)                /* GPS corrected time completed */
#define CLOCK_NOT_CORRECTED_TIME     (1U)                /* GPS uncorrected time */
#define CLOCK_RTC_TIME               (2U)                /* GPS RTC time    */
#define CLOCK_INVALID_TIME           (3U)                /* GPS error time   */

    /* Time Stamp Status Storage Data Module Related Definition */
#define TIMESTAMPDM_NAME        "TIMESTAMP_DATA"         /* Name of time stamp status storage data module */
#define TIMESTAMPDM_SEM_NAME    "TIMESTAMP_SEM"          /* Timestamp status storage data module semaphore name */

    /* Time Management Status Storage Data Module Related Definition */
#define CLKMNG_DM_NAME            "CLOCK_GPS_DATA"       /* Name of time management status storage data module */
#define CLKMNG_DM_SEM_NAME        "CLOCK_GPS_SEM"        /* Time management status storage data module semaphore name */

    /* Messaging communication */
#define CLKMNG_MSG_BUF_SIZE               (128U)         /* Message communication buffer size */
#define CLKMNG_SETTIME_MSGBODYSIZE        (4U)           /* Date/Time Setting Response Notification Data Size */
#define CLKMNG_NOTIOBSERVERS_MSGSZ        (4U)           /* Time change notification data body size */
#define CLKMNG_NOTIACCURACY_MSGSZ         (4U)           /* Change notification data body size from time accuracy */
#define CLKMNG_NOTIOBSERVERS_ENTRY_MSGSZ  (4U)           /* Time change notification registration data body size */
#define CLKMNG_NOTIACCURACY_ENTRY_MSGSZ   (0U)           /* Change Notification Stored Data Size from Time Accuracy */

    /* Semaphore Related Extensions */
#define CLOCK_SEM_RETRY_INTERVAL          (10)           /* Retry interval for semaphore acquisition/release processing 10ms             */

/*
 Data typing
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* Time-stamped data */
typedef struct TagTimestampData {
    u_int16    trip;                                     /* Trip counters */
    u_int16    reserve;                                  /* Reservation(Always 0) */
    u_int32    time;                                     /* Time counter */
    u_int8     day;                                      /* Number of days */
    u_int8     hour;                                     /* Time */
    u_int8     min;                                      /* Minutes */
    u_int8     sec;                                      /* Second */
} TimestampData;

/* TIMESTAMP INTERNAL COUNTER MODULE STRUCTURE DEFINITION */
typedef struct {
    u_int16    trip;                                     /* Trip counters */
    u_int32    time;                                     /* Time counter */
} TIMESTAMP_CNT;

/* Time management status storage area */
typedef struct TagClockStatus {
    u_int8     status;                                   /* GPS status        */
    u_int8     hour;                                     /* Time of final correction           */
    u_int8     min;                                      /* Minutes           */
    u_int8     sec;                                      /* Second           */
} ClockStatus;

/* Status storage area for writing time management shared memory */
typedef struct TagClkMngTimerSetTime {
    T_APIMSG_MSGBUF_HEADER    hdr;                       /* Message header             */
    u_int8                    reserve1[1];               /* (Not used)              */
    u_int8                    gps_timer_state;           /* GPS time status           */
    u_int16                   year;                      /* Year                    */
    u_int8                    month;                     /* Month                    */
    u_int8                    day;                       /* Day                    */
    u_int8                    hour;                      /* Hour                    */
    u_int8                    min;                       /* Minutes                    */
    u_int8                    sec;                       /* Second                    */
    u_int8                    reserve2[3];               /* (Not used)              */
} CLKMNG_TIMER_SET_TIME;

/* Existing sys_timerapi.h data */

/* User time status */
#define TIMEDATA_INVALID                0                /* Time invalid */
#define TIMEDATA_VALID                  1                /* Time valid */

/* Time Definition AM/PM */
#define CLOCK_AM                        0
#define CLOCK_PM                        1
/* Change to Enumeration */
/* Time notification unit */
typedef enum TagNotifyUnit {
    CLOCK_NOTIFY_SECOND = (1U),                          /* Second notifications */
    CLOCK_NOTIFY_MINUTE,                                 /* Minute Notification */
    CLOCK_NOTIFY_HOUR                                    /* Hourly notification */
} NOTIFYUNIT;

/* Return value definition */
typedef enum TagClockReturn {
    CLOCK_OK,                                            /* Normal completion */
    CLOCK_ERROR,                                         /* ABENDs */
    CLOCK_ERROR_ARGUMENTS                                /* Invalid argument */
} CLOCK_RETURN;

/* Time Type Definition */
typedef struct TagTimeData {
    u_int16    year;                                     /* Year */
    u_int8     month;                                    /* Month */
    u_int8     day;                                      /* Day */
    u_int8     hour;                                     /* Hour */
    u_int8     minute;                                   /* Minutes */
    u_int8     second;                                   /* Second */
} TimeData;

/* Offset time type definition */
typedef struct TagOffsetData {
    BOOL       sign;                                     /* Operator */
    TimeData   time;                                     /* Date and time */
} OffsetData;

/* Data Definition for System Time Delivery Registration */
typedef struct TagSystemTimeRegistData {
    NOTIFYUNIT unit;                                     /* Notification unit(Hour,Minutes,One of the seconds) */
} SYSTEMTIME_REGISTDATA;

/* Data Definition for Setting User Time */
typedef struct TagUserTimSetData {
    TimeData   time;                                     /* Time */
    u_int8     day_of_week;                              /* Day-of-week */
} USERTIME_SETDATA;

/* Data Definition for User Time Delivery Registration */
typedef struct TagUserTimeRegistData {
    NOTIFYUNIT unit;                                     /* Notification unit(Hour,Minutes,One of the seconds) */
} USERTIME_REGISTDATA;


/*
 Time Management Thread -> External Message Definition
- - - - - - - - - - - - - - - - - - - - - - */
/* Message data type */
typedef struct TagClkMngRecMsg {
    T_APIMSG_MSGBUF_HEADER    hdr;                       /* Message header             */
    u_int8                    data[CLKMNG_MSG_BUF_SIZE];
} CLKMNG_RCV_MSG;

/* For time management Time Set Response Message */
typedef struct TagClkMngRetTimeMsg {                     /* Time Set Response Message */
    T_APIMSG_MSGBUF_HEADER    header;                    /* Message header */
    int8                      result;                    /* Processing result */
    int8                      dummy[3];
} CLKMNG_RETTIME_MSG;

/* Change Notification Message from Time for Time Management */
typedef struct TagClkMngNotiAccuracyMsg {
    T_APIMSG_MSGBUF_HEADER    header;                    /* Message header */
    u_int8                    gps_status;                /* GPS time status(Likelihood) */
    int8                      dummy[3];
} CLKMNG_NOTIACCURACY_MSG;

/* Time change notification message for time management */
typedef struct TagClkMngNotiObserversMsg {
    T_APIMSG_MSGBUF_HEADER    header;                    /* Message header */
    u_int8                    hour;                      /* Hour */
    u_int8                    min;                       /* Minutes */
    u_int8                    sec;                       /* Second */
    int8                      dummy[1];
} CLKMNG_NOTIOBSERVERS_MSG;

/*
 Time Management API -> Time Management Thread Message Definition
- - - - - - - - - - - - - - - - - - - - - - */
/* Change Notification Registration Message from the Accuracy of Time for Time Management */
typedef struct TagClkMngNotAccuracyEntryMsg {
    T_APIMSG_MSGBUF_HEADER    header;                    /* Message header */
} CLKMNG_NOTIACCURACY_ENTRY_MSG;

/* Time change notification registration message for time management */
typedef struct TagClkMngNotiObserversEntryMsg {
    T_APIMSG_MSGBUF_HEADER    header;                    /* Message header */
    u_int8                    notify_time;               /* Notification unit (Hour,Minutes,One of the seconds) */
    int8                      dummy[3];
} CLKMNG_NOTIOBSERVERS_ENTRY_MSG;

/*
 API Function Prototype Declaration
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API ClockInitApi(void);                                                              /* API initialization processing */
RET_API ClockGetLocalTime(RealTimeData *real_time, TimestampData *timestamp);            /* Current time reference processing */
RET_API ClockGetLocalTimeBcd(RealTimeDataBcd *real_time_bcd, TimestampData *timestamp);  /* Current time reference processing(BCD) */
RET_API ClockGetUtcTime(RealTimeData* real_time, TimestampData *timestamp);              /* Current UTC time reference processing */
RET_API ClockCnvDateToSec(const LPSYSTEMTIME sys_time, u_int32* sec);                    /* Date and time,Total seconds conversion */
RET_API ClockCnvSecToDate(const u_int32* sec, LPSYSTEMTIME sys_time);                    /* Total seconds,Date and time conversion */
RET_API ClockGetAddOpeTime(u_int32 *ope_time);                                           /* Accumulated operating time reference processing */
RET_API ClockSetSystemTime(PNO snd_pno, const SysTimeData* sys_time, u_int8 gps_status); /* System time setting process */

RET_API ClockNotifyObserversClock(PNO snd_pno, u_int8 rsc_id, u_int8 notify_time);  /* Fixed period time change notification registration */
RET_API ClockNotifyObserversAccuracy(PNO snd_pno, u_int8 rsc_id);                   /* Register change notification based on the time probability */
RET_API ClockNotifyObserversAdjustClock(PNO snd_pno, u_int8 rsc_id);                /* System time setting change notification registration */

#ifdef __cplusplus
}
#endif

#endif  // POSITIONING_CLIENT_INCLUDE_CLOCK_API_H_
