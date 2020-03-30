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

/**
 * @file
 *    ClockIf.cpp
 * @brief
 *    Clock service-to-service interface
 */

/*---------------------------------------------------------------------------------*
 * Include Files                                                                   *
 *---------------------------------------------------------------------------------*/
#include <vehicle_service/positioning_base_library.h>
#include "ClockIf.h"
//#include <vehicle_service/clock_notifications.h>

/*---------------------------------------------------------------------------------*
 * Definition                                                                      *
 *---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*
 * Structre                                                                        *
 *---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*
 * Local Function Prototype                                                        *
 *---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*
 * Grobal Value                                                                    *
 *---------------------------------------------------------------------------------*/
static BOOL g_clock_availability = FALSE;

/*---------------------------------------------------------------------------------*
 * Function                                                                        *
 *---------------------------------------------------------------------------------*/
/**
 * @brief
 *    Clock Services IF Availability Change Notification Registration
 *
 * @param[in]  fp_on_cmd    Callback function
 * @return     eFrameworkunifiedStatusOK
 * @return     eFrameworkunifiedStatusFail
 */
EFrameworkunifiedStatus ClockIfNotifyOnClockAvailability(CbFuncPtr fp_on_cmd) {
    EFrameworkunifiedStatus estatus = eFrameworkunifiedStatusFail;
    HANDLE happ;

    happ = _pb_GetAppHandle();
    if (happ != NULL) {
        /* Clock/Availability Changing notification registration */
//        estatus = FrameworkunifiedSubscribeNotificationWithCallback(happ, NTFY_Clock_Availability, fp_on_cmd);  // LCOV_EXCL_BR_LINE 6:unexpected branch  //NOLINT (whitespace/line_length)
//        if (eFrameworkunifiedStatusOK != estatus) {  // LCOV_EXCL_BR_LINE 4: nsfw error
//            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
//                          "PositioningSubscriveNotificationswithCallback ERROR!! [estatus=%d]", estatus);
//        }
    } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_GetAppHandle ERROR!! [happ=%p]", happ);
    }

    return estatus;
}

/**
 * @brief
 *    Clock Services IF-Availability Settings
 *
 * @param[in]  b_is_available    Available state
 * @return     none
 */
void ClockIfSetAvailability(BOOL b_is_available) {
    g_clock_availability = b_is_available;

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "g_clock_availability=%d", g_clock_availability);

    return;
}

/**
 * @brief
 *    Clock Services IF GPS Time
 *
 * @param[in]  *gps_time        GPS time
 * @param[out] *b_is_available    Available state
 * @return     eFrameworkunifiedStatusOK
 * @return     eFrameworkunifiedStatusFail
 */
//EFrameworkunifiedStatus ClockIfDtimeSetGpsTime(const SENSOR_MSG_GPSTIME *pst_gps_time, BOOL* pb_is_available) {
//    EFrameworkunifiedStatus estatus = eFrameworkunifiedStatusFail;
//    HANDLE happ;
//    T_DTIME_MSG_GPSTIME st_dtime_gpstime;
//
//    if (g_clock_availability == TRUE) {
//        happ = _pb_GetAppHandle();
//        if (happ != NULL) {  // LCOV_EXCL_BR_LINE 4: nsfw error
//            st_dtime_gpstime.utc.year   = pst_gps_time->utc.year;
//            st_dtime_gpstime.utc.month  = pst_gps_time->utc.month;
//            st_dtime_gpstime.utc.date   = pst_gps_time->utc.date;
//            st_dtime_gpstime.utc.hour   = pst_gps_time->utc.hour;
//            st_dtime_gpstime.utc.minute = pst_gps_time->utc.minute;
//            st_dtime_gpstime.utc.second = pst_gps_time->utc.second;
//            st_dtime_gpstime.tdsts      = pst_gps_time->tdsts;
//
//            /* Set GPS time for Clock services */
//            estatus = DTime_setGpsTime(happ, &st_dtime_gpstime);
//            if (estatus != eFrameworkunifiedStatusOK) {
//                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, \
//                    "DTime_setGpsTime ERROR!! [ret=%d, y=%d, m=%d, d=%d, h=%d, n=%d, s=%d, sts=%d]", \
//                    estatus, st_dtime_gpstime.utc.year, st_dtime_gpstime.utc.month, st_dtime_gpstime.utc.date, \
//                    st_dtime_gpstime.utc.hour, st_dtime_gpstime.utc.minute, st_dtime_gpstime.utc.second, \
//                    st_dtime_gpstime.tdsts);
//            }
//        } else {
//            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_GetAppHandle ERROR!! [happ=%p]", happ);
//        }
//    } else {
//        /* nop */
//    }
//
//    *pb_is_available = g_clock_availability;
//
//    return estatus;
//}

