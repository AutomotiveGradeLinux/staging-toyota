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
* @file positioning_common.h
*/

#ifndef INC_COMMON_POSITIONING_COMMON_H_
#define INC_COMMON_POSITIONING_COMMON_H_

/*---------------------------------------------------------------------------*/
// Include files

#include "positioning_def.h"

/*---------------------------------------------------------------------------*/
// Definition

/*---------------------------------------------------------------------------*/
// ENUMERATION

/*---------------------------------------------------------------------------*/
// STRUCTURE

typedef struct PosResetinfo {
    uint8_t       mode;               // Reset mode
    uint8_t       reserve[3];         // reserve
    PNO           sndpno;             // Caller PNO
    PNO           respno;             // Destination PNO
} POS_RESETINFO;

/*!
   @brief   Thread activation information
*/
typedef struct StThreadSetupInfo {
  EnumSetupMode_POS mode;            // Thread activation mode
} ST_THREAD_SETUP_INFO;


/*---------------------------------------------------------------------------*/
// Prototype
EFrameworkunifiedStatus PosInitialize(HANDLE h_app);
EnumSetupMode_POS PosSetupThread(HANDLE h_app, EnumTID_POS e_tid);
void PosTeardownThread(EnumTID_POS e_tid);
EFrameworkunifiedStatus PosCreateThread(HANDLE h_app, int8_t index);

/*---------------------------------------------------------------------------*/
#endif  // INC_COMMON_POSITIONING_COMMON_H_

/*---------------------------------------------------------------------------*/
/*EOF*/
