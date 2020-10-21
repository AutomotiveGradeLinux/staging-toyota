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
* @file MDev_GpsRollOver.h
*/
#ifndef INC_GPSCOMMON_MDEV_GPSROLLOVER_H_
#define INC_GPSCOMMON_MDEV_GPSROLLOVER_H_

#include <vehicle_service/WPF_STD.h>

typedef struct TimRolovrYMD {
  u_int16 year;
  u_int16 month;
  u_int16 day;
  u_int8  dummy[2];
} TG_TIM_ROLOVR_YMD;


void GPSRollOverConvTime(TG_TIM_ROLOVR_YMD* base_ymd, TG_TIM_ROLOVR_YMD* conv_ymd, u_int8 gpsweekcorcnt);

#endif  // INC_GPSCOMMON_MDEV_GPSROLLOVER_H_
