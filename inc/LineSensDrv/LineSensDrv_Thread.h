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
* @file LineSensDrv_Thread.h
*/

#ifndef INC_LINESENSDRV_LINESENSDRV_THREAD_H_
#define INC_LINESENSDRV_LINESENSDRV_THREAD_H_

/*---------------------------------------------------------------------------*/
// Include files

#include "LineSensDrv_Api.h"
#include "LineSensDrv_Sensor.h"

/*---------------------------------------------------------------------------*/
// Define

// Return value
#define RET_LSDRV_SUCCESS       RET_NORMAL  // API processing success
#define RET_LSDRV_ERROR         RET_ERROR   // API processing failed

/*---------------------------------------------------------------------------*/
// Rcv Message Related

// Size of data section
#define LSDRV_RCVMSG_MSGBUF     512         // Byte count of RcvMSG data section

/*---------------------------------------------------------------------------*/
// Struct

// TAG       : LSDRV_RCVDATA
// ABSTRACT  : Message receive buffer structure
typedef struct LsDrvRcvData {
  T_APIMSG_MSGBUF_HEADER  st_head;                        // Message header
  u_int8                  uc_data[LSDRV_RCVMSG_MSGBUF];   // Data portion
} LSDRV_RCVDATA;

/*---------------------------------------------------------------------------*/
// Prototype

int32       LineSensDrvMainThreadInit(HANDLE);
void        LineSensDrvParamInit(void);

EFrameworkunifiedStatus  LineSensDrvThread(HANDLE);
void        LineSensDrvThreadStopProcess(void);
u_int8      LineSensDrvGetSysRecvFlag(void);

/*---------------------------------------------------------------------------*/
#endif  // INC_LINESENSDRV_LINESENSDRV_THREAD_H_

/*---------------------------------------------------------------------------*/
/*EOF*/
