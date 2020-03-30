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

#ifndef POSITIONING_SERVER_INCLUDE_SENSOR_DEADRECKONING_COMMON_H_
#define POSITIONING_SERVER_INCLUDE_SENSOR_DEADRECKONING_COMMON_H_
/****************************************************************************
 * File name    :DeadReckoning_Common.h
 * System name    :PastModel002
 * Subsystem name  :
 ****************************************************************************/

#include "Dead_Reckoning_Local_Api.h"

/************************************************************************
*      Macro definitions                          *
************************************************************************/
#define DEADRECKONING_ZERO          0
#define DEADRECKONING_BIT0          0x00000001
#define DEADRECKONING_BIT1          0x00000002
#define DEADRECKONING_BIT2          0x00000004
#define DEADRECKONING_BIT3          0x00000008
#define DEADRECKONING_BIT4          0x00000010
#define DEADRECKONING_BIT5          0x00000020
#define DEADRECKONING_BIT6          0x00000040
#define DEADRECKONING_BIT7          0x00000080
#define DEADRECKONING_BIT29         0x20000000
#define DEADRECKONING_BIT30         0x40000000
#define DEADRECKONING_BIT31         0x80000000
#define DEADRECKONING_NEQ           1
#define DEADRECKONING_EQ            0
#define DEADRECKONING_INVALID       0
#define DEADRECKONING_EFFECTIVE     1
#define DEADRECKONING_CHGTYPE_NCHG  DEADRECKONING_EQ
#define DEADRECKONING_CHGTYPE_CHG   DEADRECKONING_NEQ
#define DEADRECKONING_GETMETHOD_DR  4  /* Data collection way: DR */

/* Offset value of normal data of vehicle sensor */
#define  DEADRECKONING_OFFSET_NORMAL      4

/************************************************************************
*      Typedef definitions                          *
************************************************************************/

/* Data Master Set Notification Function */
typedef  void (*PFUNC_DR_DMASTER_SET_N) (DID, u_int8, u_int8);

/************************************************************************
*      Struct definitions                          *
************************************************************************/
/*********************************************************************
* TAG    : DEADRECKONING_DID_OFFSET_TBL
* ABSTRACT  : Vehicle sensor data ID,Structure of each data in the offset table
***********************************************************************/
typedef struct {
    DID      ul_did;                /* Data ID      */
    u_int16  us_offset;              /* Offset value    */
    u_int8   uc_reserve[2];            /* Reserved      */
} DEADRECKONING_DID_OFFSET_TBL;

/*********************************************************************
* TAG    :
* ABSTRACT  :
***********************************************************************/

/************************************************************************
*      Function prototype                        *
************************************************************************/
u_int8 DeadReckoningMemcmp(const void *, const void *, size_t);
int32 DeadReckoningCheckDid(DID);
u_int16  DeadReckoningGetDataMasterOffset(DID);
#endif  // POSITIONING_SERVER_INCLUDE_SENSOR_DEADRECKONING_COMMON_H_
