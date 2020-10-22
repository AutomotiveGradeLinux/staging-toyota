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

/******************************************************************************
 *    File name        : ClockDataMng.h
 *    System name        : PastModel002
 *    Subsystem name    : Time-related processing
 ******************************************************************************/
#ifndef POSITIONING_SERVER_INCLUDE_SENSOR_CLOCKDATAMNG_H_
#define POSITIONING_SERVER_INCLUDE_SENSOR_CLOCKDATAMNG_H_

#include <vehicle_service/positioning_base_library.h>


#include "positioning_hal.h"
#include "CanInput_API.h"
#include "Sensor_API.h"

/********************************************************************************
*            Macro definitions                                                            *
*********************************************************************************/
#define CLOCKDATAMNG_SECONDS_PAR_WEEK            (7 * 24 * 60 * 60)
#define CLOCKDATAMNG_CNV_WEEK2SEC(WEEK)        ((WEEK)* CLOCKDATAMNG_SECONDS_PAR_WEEK)

/* [PastModel002 Specifications]Time difference tolerance(plus or minus 512 week or less,GPS TIME range from 0 to 1023 week) */
#define CLOCKDATAMNG_TIME_DIFF_LIMIT_UPPER        (512 * CLOCKDATAMNG_SECONDS_PAR_WEEK)
#define CLOCKDATAMNG_TIME_DIFF_LIMIT_LOWER        (-512 * CLOCKDATAMNG_SECONDS_PAR_WEEK)

/* [PastModel002 Specifications]Time difference addition value(+1024week,GPS TIME range from 0 to 1023 week) */
#define CLOCKDATAMNG_TIME_DIFF_ADD_VALUE          (CLOCKDATAMNG_TIME_DIFF_LIMIT_UPPER * 2)

/* [PastModel002 Specifications]GPS TIME weekNo scope */
#define CLOCKDATAMNG_GPS_TIME_WEEKNO_LIMIT        1024

/* [PastModel002 Specifications]Initial value of time difference information at the previous shutdown */
#define CLOCKDATAMNG_TIME_DIFF_INIT_VALUE        0x7FFFFFFFL

/* [PastModel002 Specifications]AID-INI lifetime(+4Time) */
#define CLOCKDATAMNG_AID_INI_EFFECTIVE_TIME        (4 * 60 * 60)

/* [PastModel002 Specifications]AID-EPH lifetime(plus or minus 2 hours) */
#define CLOCKDATAMNG_AID_EPH_EFFECTIVE_TIME        (2 * 60 * 60)

#define MUTEX_CLOCKDATAMNG                        __TEXT("MUTEX_VEHICLE_CLOCKDATAMNG")

/********************************************************************************
*            Struct definitions                                                            *
*********************************************************************************/
/********************************************************************************
 * TAG      :CLOCKDATAMNG_GPS_TIME
 * ABSTRACT :GPS time structure
 * NOTE     :
 ********************************************************************************/
typedef struct {
    u_int32                  tow;                    /* Time of week (msec)    */
    int16                    week_no;                /* GPS Week No.            */
    u_int8                   reserve[2];             /* reserve                */
    BOOL                     validity_flag;          /* Enabled/DisabledFlagging        */
} CLOCKDATAMNG_GPS_TIME;

/********************************************************************************
*            Function prototype                                                      *
*********************************************************************************/
#ifdef    __cplusplus
extern "C"    {
#endif

    BOOL ClockDataMngInitialize(void);
    void ClockDataMngSetLastTimeDiff(int32 time_diff);

    void ClockDataMngSetGpsTime(const SENSOR_MSG_GPSDATA_DAT *gps_time_data);
    void ClockDataMngGetGpsTime(CLOCKDATAMNG_GPS_TIME *gps_time_data);

    BOOL ClockDataMngGetLocalTime(const CANINPUT_MSG_LOCALTIMEINFO_DAT *can_local_time, LOCALTIME *vehicle_local_time);
    BOOL ClockDataMngGetTimeDiff(LOCALTIME *local_time, int32 *time_diff);

    void ClockDataMngGetStartupTime(CLOCKDATAMNG_GPS_TIME *start_up_time);
    int32 ClockDataMngCalculateTimeDiff(int32 munuend, int32 subtrahend);

    BOOL ClockDataMngCheckLocalTime(LOCALTIME *local_time);
    int32 ClockDataMngConversionLocalTime2GpsTime(LOCALTIME *local_time, BOOL calc_second_available);

#ifdef    __cplusplus
}
#endif

#endif  // POSITIONING_SERVER_INCLUDE_SENSOR_CLOCKDATAMNG_H_
