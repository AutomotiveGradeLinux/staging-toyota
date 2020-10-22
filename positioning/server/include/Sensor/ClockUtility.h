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

/*************************************************************************
@file     ClockUtility.h
@detail   Common processing function header file concerning clock
*****************************************************************************/
#ifndef POSITIONING_SERVER_INCLUDE_SENSOR_CLOCKUTILITY_H_
#define POSITIONING_SERVER_INCLUDE_SENSOR_CLOCKUTILITY_H_

#include <vehicle_service/positioning_base_library.h>
#include "Clock_API.h"

#ifdef  __cplusplus
extern "C" {
#endif

    /*
     Function prototype declaration
    * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    RET_API ClockUtilityConvertDateToSecond(const LPSYSTEMTIME lp_st, u_int32* uli_sec);
    RET_API ClockUtilityConvertSecondToDate(const u_int32* uli_sec, LPSYSTEMTIME lp_st);

    /* Converting TimeData Types to SYSTEMTIME Types Functions */
    CLOCK_RETURN ClockApiConvertTimeDataToSYSTEMTIME(const TimeData* base_time,
                                                      SYSTEMTIME* result_time);

#ifdef  __cplusplus
}
#endif

#endif  // POSITIONING_SERVER_INCLUDE_SENSOR_CLOCKUTILITY_H_
