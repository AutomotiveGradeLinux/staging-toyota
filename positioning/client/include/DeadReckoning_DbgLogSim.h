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

#ifndef POSITIONING_CLIENT_INCLUDE_DEADRECKONING_DBGLOGSIM_H_
#define POSITIONING_CLIENT_INCLUDE_DEADRECKONING_DBGLOGSIM_H_

/*******************************************************************************
 FILE        : DeadReckoning_DbgLogSim.h
 SYSTEM        : _CWORD64_ platform development
 SUBSYSTEM    : Log Simulator for Debugging
 PROGRAM    : PastModel002*1 log read
 FUNC LIST    :
--------------------------------------------------------------------------------

*******************************************************************************/

#include "Dead_Reckoning_API.h"

#define DR_DBG_LOG_PACKET_HEAD_SIZE    (4)        /* DR_LOG_PACKET_HEAD_SIZE */
#define DR_DBG_LOG_SIM_BUFF_SIZE       (0x200)

#define DR_DBG_LOG_SIM_DATA_NONE       (-1)
#define DR_DBG_LOG_SIM_DATA_GPS        (1)
#define DR_DBG_LOG_SIM_DATA_SENSOR     (2)

/*
 * prototype
 * public functions
 */

void DeadReckoningDbgLogSimInit(void);
void DeadReckoningDbgLogSimDispose(void);

BOOL DeadReckoningDbgLogSimIsDbgLogSimMode(void);
int DeadReckoningDbgLogSimReadPacket(u_int8 *, size_t, int16 *);

#endif  // POSITIONING_CLIENT_INCLUDE_DEADRECKONING_DBGLOGSIM_H_
