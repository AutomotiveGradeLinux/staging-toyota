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
* @file LineSensDrv_Api.h
*/

#ifndef INC_COMMON_LINESENSDRV_API_H_
#define INC_COMMON_LINESENSDRV_API_H_

/*---------------------------------------------------------------------------*/
// Include files

#include "positioning_def.h"

/*---------------------------------------------------------------------------*/
// Define

#define MUTEX_GPS_IRQ_FLG "MUTEX_GPS_IRQ_FLG"

/*---------------------------------------------------------------------------*/
// Prototype

void  DeliveryLineSensorDataPositioning(LSDRV_MSG_LSDATA_G*, u_int8);
BOOL  LineSensDrvApiInitialize(void);
void  LineSensDrvApiInitEstGpsCnt(void);

/*---------------------------------------------------------------------------*/
#endif  // INC_COMMON_LINESENSDRV_API_H_

/*---------------------------------------------------------------------------*/
/*EOF*/
