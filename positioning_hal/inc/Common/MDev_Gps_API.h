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
* @file MDev_Gps_API.h
*/

#ifndef INC_COMMON_MDEV_GPS_API_H_
#define INC_COMMON_MDEV_GPS_API_H_

/*---------------------------------------------------------------------------*/
// Include files

#include "gps_hal.h"
#include "positioning_def.h"

/*---------------------------------------------------------------------------*/
// Prototype

RET_API SendNmeaGps(const MDEV_GPS_NMEA* p_nmea_data);
RET_API SendCustomGps(const SENSOR_MSG_GPSTIME* p_gps_time,
                      const SENSORLOCATION_LONLATINFO_DAT* p_lonlat,
                      const SENSORLOCATION_ALTITUDEINFO_DAT* p_altitude,
                      const SENSORMOTION_HEADINGINFO_DAT* p_heading,
                      const NAVIINFO_DIAG_GPS* p_diag_data);
RET_API SendSpeedGps(const SENSORMOTION_SPEEDINFO_DAT* p_seed, u_int16 us_peed);
RET_API SendTimeGps(const MDEV_GPS_RTC* p_rtc);
RET_API SendClockDriftGps(int32_t drift);
RET_API SendClockFrequencyGps(uint32_t freq);
RET_API DevGpsSndWknRollover(const SensorWknRollOverHal* p_week_rollover);
int32   DevGpsRstAnsSend(PNO u_pno, RID uc_rid, u_int32 ul_rst_sts);
int32   DevGpsTimesetAnsSend(PNO u_pno, RID uc_rid, u_int32 ul_rst_sts);
RET_API DevSendGpsConnectError(BOOL);
RET_API SndGpsTimeRaw(const SENSOR_GPSTIME_RAW* pst_gpstime_raw);
/*---------------------------------------------------------------------------*/
#endif  // INC_COMMON_MDEV_GPS_API_H_

/*---------------------------------------------------------------------------*/
/*EOF*/
