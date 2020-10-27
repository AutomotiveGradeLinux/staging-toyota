/*
 * @copyright Copyright (c) 2017-2020 TOYOTA MOTOR CORPORATION.
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
* @file positioning_hal.cpp
*/

/*---------------------------------------------------------------------------*/
// Include files

#include <stdio.h>

#include "positioning_hal.h"
#include "positioning_def.h"
#include "positioning_common.h"

/*---------------------------------------------------------------------------*/
// Functions

EFrameworkunifiedStatus StartGpsMainThreadPositioning(HANDLE h_app) {
  return PosCreateThread(h_app, ETID_POS_GPS);
}

EFrameworkunifiedStatus StartGpsRecvThreadPositioning(HANDLE h_app) {
  return PosCreateThread(h_app, ETID_POS_GPS_RECV);
}

EFrameworkunifiedStatus StartLineSensorThreadPositioning(HANDLE h_app) {
  return PosCreateThread(h_app, ETID_POS_SENS);
}

EFrameworkunifiedStatus StartGpsRolloverThreadPositioning(HANDLE h_app) {
  return eFrameworkunifiedStatusErrOther;
}

/*---------------------------------------------------------------------------*/
/*EOF*/
