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

/*************************************************************************
@file            ClockUtility_private.h
@detail            Common processing function private header file concerning clock
*****************************************************************************/

#ifndef POSITIONING_SERVER_INCLUDE_SENSOR_CLOCKUTILITY_PRIVATE_H_
#define POSITIONING_SERVER_INCLUDE_SENSOR_CLOCKUTILITY_PRIVATE_H_

#include "CommonDefine.h"

#ifdef    __cplusplus
extern "C" {
#endif

    /*
     Macro definitions
    * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#define    MONTH_MAX                        (12U)    /* Maximum value of month */

    /*
     Data typing
    * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    /* Time parameter */
    typedef struct ClockDayCnvTblTag {
        YearCntTbl        st_tbl[CNV_YEAR_MAX + 1 ];    /* Have +1 data for end determination */
    } ClockDayCntTbl;

#ifdef    __cplusplus
}
#endif

#endif  // POSITIONING_SERVER_INCLUDE_SENSOR_CLOCKUTILITY_PRIVATE_H_

