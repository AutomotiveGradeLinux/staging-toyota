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

#ifndef POSITIONING_SERVER_INCLUDE_SENSOR_VEHICLESENS_COMMON_H_
#define POSITIONING_SERVER_INCLUDE_SENSOR_VEHICLESENS_COMMON_H_
/****************************************************************************
 * File name    :VehicleSens_Common.h
 * System name    :_CWORD107_
 * Subsystem name  :Vehicle sensor process
 ****************************************************************************/

#include "Vehicle_API_Dummy.h"
#include "Vehicle_API_private.h"

//#include "positioning_hal.h"

/************************************************************************
*      Macro definitions                          *
************************************************************************/
#define VEHICLESENS_ZERO          0
#define VEHICLESENS_BIT0          0x00000001
#define VEHICLESENS_BIT1          0x00000002
#define VEHICLESENS_BIT2          0x00000004
#define VEHICLESENS_BIT3          0x00000008
#define VEHICLESENS_BIT4          0x00000010
#define VEHICLESENS_BIT5          0x00000020
#define VEHICLESENS_BIT6          0x00000040
#define VEHICLESENS_BIT7          0x00000080
#define VEHICLESENS_BIT29          0x20000000
#define VEHICLESENS_BIT30          0x40000000
#define VEHICLESENS_BIT31          0x80000000
#define VEHICLESENS_NEQ            1
#define VEHICLESENS_EQ            0
#define VEHICLESENS_INVALID          0
#define VEHICLESENS_EFFECTIVE        1
#define VEHICLESENS_CHGTYPE_NCHG      VEHICLESENS_EQ
#define VEHICLESENS_CHGTYPE_CHG        VEHICLESENS_NEQ
#define VEHICLESENS_GETMETHOD_CAN      0  /* Data collection way: CAN communication */
#define VEHICLESENS_GETMETHOD_LINE      1  /* Data collection way: LineSensor drivers */
#define VEHICLESENS_GETMETHOD_NO_DETECTION  2  /* Data collection way: Not downloaded */
#define VEHICLESENS_GETMETHOD_GPS      3  /* Data collection way: GPS */
#define VEHICLESENS_GETMETHOD_NAVI      4  /* Data collection way: NAVI */
#define VEHICLESENS_GETMETHOD_CLOCK      5  /* Data collection way: Clock Information */
#define VEHICLESENS_GETMETHOD_INTERNAL    6  /* Data collection way: Internal calculation */
#define VEHICLESENS_GETMETHOD_OTHER      7  /* Data collection way: Others */

/* Offset value of normal data of vehicle sensor */
#define  VEHICLESENS_OFFSET_NORMAL      32
/* Offset value of normal data of vehicle sensor */
#define  VEHICLESENS_OFFSET_GPS_FORMAT    268    /* 8 + 260 */
/* Offset value of normal data of vehicle sensor */
#define  VEHICLESENS_OFFSET_GPS_BINARY    368    /* 8 + 360 */
/* ++ GPS _CWORD82_ support */
#define  VEHICLESENS_OFFSET_GPS_NMEA      460    /* 8 + 452  Packaged delivery offset */
/* -- GPS _CWORD82_ support */

/* ++ PastModel002 support */
/* U-Blox GPS offsets */
#define  VEHICLESENS_OFFSET_GPS_UBLOX    220    /* 220 */
/* -- PastModel002 support */

#if CONFIG_SENSOR_EXT_VALID        /* Initial Sensor Support */
#define  VEHICLESENS_OFFSET_10WORD_PKG_EXT  208    /* Offset value of 10 initial deliveries(WORD) 8 + (20 * 10)  */
#define  VEHICLESENS_OFFSET_WORD_PKG_EXT    28    /* Offset value of 10 initial deliveries(WORD) 8 + 20  */
#define  VEHICLESENS_OFFSET_BYTE_PKG_EXT    18    /* Offset value of 10 initial deliveries(BYTE) 8 + 10  */
#define  VEHICLESENS_OFFSET_32LONG_PKG_EXT  1328  /* Offset value of 10 initial deliveries(LONG) 8 + (132 * 10)  */
#define  VEHICLESENS_OFFSET_20WORD      48    /* Offset value of 20 sensor data(WORD) 8 + 40  */
#define  VEHICLESENS_OFFSET_20WORD_FST    408    /* Offset value of 20 initial sensor data(WORD) 8 + 400  */
#define  VEHICLESENS_DNUM_4          4    /* Number of sensor data */
#define  VEHICLESENS_DNUM_5          5    /* Number of sensor data */
#define  VEHICLESENS_DNUM_7          7    /* Number of sensor data */
#endif
#define  VEHICLESENS_EXT_OFF          0    /* Normal delivery  */
#define  VEHICLESENS_EXT_ON          1    /* Extended delivery  */

#if CONFIG_HW_PORTSET_TYPE_C
#define  VEHICLESENS_OFFSET_GPS_NMEA      248    /* 8 + 240 */
#endif

/************************************************************************
*      Typedef definitions                          *
************************************************************************/
/* typedef  u_int32    DID;          */  /* Data ID      */

/* Data Master Set Notification Function */
typedef  void (*PFUNC_DMASTER_SET_N) (DID, u_int8, u_int8);
typedef  void (*PFUNC_DMASTER_SET_SHARED_MEMORY) (DID, u_int8);

/************************************************************************
*      Struct definitions                          *
************************************************************************/
/*********************************************************************
* TAG       : VEHICLESENS_DID_OFFSET_TBL
* ABSTRACT  : Vehicle sensor data ID,Structure of each data in the offset table
***********************************************************************/
typedef struct {
    DID    ul_did;                /* Data ID      */
    u_int16  us_offset;              /* Offset value    */
    u_int8  uc_reserve[2];            /* Reserved      */
} VEHICLESENS_DID_OFFSET_TBL;

/*********************************************************************
* TAG       :
* ABSTRACT  :
***********************************************************************/

/************************************************************************
*      Function prototype                        *
************************************************************************/
u_int8 VehicleSensmemcmp(const void *, const void *, size_t);
int32 VehicleSensCheckDid(DID);
u_int16  VehicleSensGetDataMasterOffset(DID);
#if CONFIG_SENSOR_EXT_VALID        /* Initial Sensor Support */
u_int16  VehicleSensGetDataMasterExtOffset(DID);
#endif
//void VehicleSensSetGpsVersion(const SENSOR_MSG_GPSDATA_DAT *pstData);
void VehicleSensGetDebugPosDate(void* pBuf, u_int8 uc_get_method);
#endif  // POSITIONING_SERVER_INCLUDE_SENSOR_VEHICLESENS_COMMON_H_
