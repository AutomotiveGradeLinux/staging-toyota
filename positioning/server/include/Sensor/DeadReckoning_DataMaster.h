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

#ifndef POSITIONING_SERVER_INCLUDE_SENSOR_DEADRECKONING_DATAMASTER_H_
#define POSITIONING_SERVER_INCLUDE_SENSOR_DEADRECKONING_DATAMASTER_H_
/****************************************************************************
 * File name    :DeadReckoning_DataMaster.h
 * System name    :PastModel002
 * Subsystem name  :DeadReckoning processes
 ****************************************************************************/

#include "DeadReckoning_Common.h"
#include "Dead_Reckoning_Local_Api.h"

/************************************************************************
*      Macro definitions                          *
************************************************************************/
/* Initial value */
#define  VEHICLE_DINIT_LONGITUDE                        0x00
#define  VEHICLE_DINIT_LATITUDE                         0x00
#define  VEHICLE_DINIT_ALTITUDE                         0x00
#define  VEHICLE_DINIT_SPEED                            0x00
#define  VEHICLE_DINIT_HEADING                          0x00
#define  VEHICLE_DINIT_DR_SNS_COUNTER                   0x00
#define  VEHICLE_DINIT_GYRO_OFFSET                      0x0000
#define  VEHICLE_DINIT_GYRO_SCALE_FACTOR                0x00
#define  VEHICLE_DINIT_GYRO_SCALE_FACTOR_LEVEL          0x00
#define  VEHICLE_DINIT_SPEED_PULSE_SCALE_FACTOR         0x0000
#define  VEHICLE_DINIT_SPEED_PULSE_SCALE_FACTOR_LEVEL   0x00

#define  VEHICLE_DSIZE_LONGITUDE                        0x04
#define  VEHICLE_DSIZE_LATITUDE                         0x04
#define  VEHICLE_DSIZE_ALTITUDE                         0x04
#define  VEHICLE_DSIZE_SPEED                            0x02
#define  VEHICLE_DSIZE_HEADING                          0x02
#define  VEHICLE_DSIZE_DR_SNS_COUNTER                   0x01
#define  VEHICLE_DSIZE_GYRO_OFFSET                      0x02
#define  VEHICLE_DSIZE_GYRO_SCALE_FACTOR                0x01
#define  VEHICLE_DSIZE_GYRO_SCALE_FACTOR_LEVEL          0x01
#define  VEHICLE_DSIZE_SPEED_PULSE_SCALE_FACTOR         0x02
#define  VEHICLE_DSIZE_SPEED_PULSE_SCALE_FACTOR_LEVEL   0x01

/* Data received*/
#define  DEADRECKONING_RCVFLAG_ON                       0x01
#define  DEADRECKONING_RCVFLAG_OFF                      0x00

/************************************************************************
*      Struct definitions                          *
************************************************************************/

/*********************************************************************
* TAG    : DEADRECKONING_DATA_MASTER
* ABSTRACT  : Vehicle SW Data Master Structure
***********************************************************************/
typedef struct {
    DID      ul_did;      /* Data ID      */
    u_int16  us_size;      /* Size of the data  */
    u_int8   uc_rcv_flag;    /* Receive flag    */
    u_int8   dr_status;    /* DR status    */
    u_int8   uc_data[4];    /* Vehicle DR data    */
} DEADRECKONING_DATA_MASTER;

/************************************************************************
*      Function prototype                        *
************************************************************************/
void  DeadReckoningInitDataMaster(void);
void  DeadReckoningSetDataMaster(const DEADRECKONING_DATA_MASTER *, PFUNC_DR_DMASTER_SET_N);
void  DeadReckoningGetDataMaster(DID ul_did, DEADRECKONING_DATA_MASTER *);

void  DeadReckoningInitLongitudeDr(void);
u_int8  DeadReckoningSetLongitudeDr(const DEADRECKONING_DATA_MASTER *);
void  DeadReckoningGetLongitudeDr(DEADRECKONING_DATA_MASTER *);

void  DeadReckoningInitLatitudeDr(void);
u_int8  DeadReckoningSetLatitudeDr(const DEADRECKONING_DATA_MASTER *);
void  DeadReckoningGetLatitudeDr(DEADRECKONING_DATA_MASTER *);

void  DeadReckoningInitAltitudeDr(void);
u_int8  DeadReckoningSetAltitudeDr(const DEADRECKONING_DATA_MASTER *);
void  DeadReckoningGetAltitudeDr(DEADRECKONING_DATA_MASTER *);

void  DeadReckoningInitSpeedDr(void);
u_int8  DeadReckoningSetSpeedDr(const DEADRECKONING_DATA_MASTER *);
void  DeadReckoningGetSpeedDr(DEADRECKONING_DATA_MASTER *);

void  DeadReckoningInitHeadingDr(void);
u_int8  DeadReckoningSetHeadingDr(const DEADRECKONING_DATA_MASTER *);
void  DeadReckoningGetHeadingDr(DEADRECKONING_DATA_MASTER *);

void  DeadReckoningInitSnsCounterDr(void);
u_int8  DeadReckoningSetSnsCounterDr(const DEADRECKONING_DATA_MASTER *);
void  DeadReckoningGetSnsCounterDr(DEADRECKONING_DATA_MASTER *);

void  DeadReckoningInitGyroOffsetDr(void);
u_int8  DeadReckoningSetGyroOffsetDr(const DEADRECKONING_DATA_MASTER *);
void  DeadReckoningGetGyroOffsetDr(DEADRECKONING_DATA_MASTER *);

void  DeadReckoningInitGyroScaleFactorDr(void);
u_int8  DeadReckoningSetGyroScaleFactorDr(const DEADRECKONING_DATA_MASTER *);
void  DeadReckoningGetGyroScaleFactorDr(DEADRECKONING_DATA_MASTER *);

void  DeadReckoningInitGyroScaleFactorLevelDr(void);
u_int8  DeadReckoningSetGyroScaleFactorLevelDr(const DEADRECKONING_DATA_MASTER *);
void  DeadReckoningGetGyroScaleFactorLevelDr(DEADRECKONING_DATA_MASTER *);

void  DeadReckoningInitSpeedPulseScaleFactorDr(void);
u_int8  DeadReckoningSetSpeedPulseScaleFactorDr(const DEADRECKONING_DATA_MASTER *);
void  DeadReckoningGetSpeedPulseScaleFactorDr(DEADRECKONING_DATA_MASTER *);

void  DeadReckoningInitSpeedPulseScaleFactorLevelDr(void);
u_int8  DeadReckoningSetSpeedPulseScaleFactorLevelDr(const DEADRECKONING_DATA_MASTER *);
void  DeadReckoningGetSpeedPulseScaleFactorLevelDr(DEADRECKONING_DATA_MASTER *);

#endif  // POSITIONING_SERVER_INCLUDE_SENSOR_DEADRECKONING_DATAMASTER_H_
