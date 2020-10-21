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
* @file LineSensDrv_Sensor.h
*/

#ifndef INC_LINESENSDRV_LINESENSDRV_SENSOR_H_
#define INC_LINESENSDRV_LINESENSDRV_SENSOR_H_

/*---------------------------------------------------------------------------*/
// Include files

#include "positioning_def.h"

/*---------------------------------------------------------------------------*/
// Macro definitions

/* Message related */
/* Message identification number definition received from the ICR */
#define LSDRV_RCVMSG_INVALID      0x00    /* Not applicable          */
#define LSDRV_RCVMSG_SENSOR       0x12    /* Sensor data        */

/* LineSensor vehicle signal notification */

#define LSDRV_SENS_MASK           0xFFF   /* Sensor data MASK      */
#define LSDRV_PLSMAX              65536   /* Maximum vehicle speed pulse number    */
/* #011 start */
#define LSDRV_SENS_COEFFICIENT    14.1287284284144427    /* Vehicle speed calculation factor */
#define LSDRV_SPKMPH_METHOD_FST   0       /* Vehicle speed calculation method(Initial Sensor Data) */
#define LSDRV_SPKMPH_METHOD_NML   1       /* Vehicle speed calculation method(Sensor data)  */
#define LSDRV_SPKMPH_AVE_TIME     29      /* Effective time for average calculation data(in units of 100ms) */  /* #013 */
#define LSDRV_SPKMPH_NOISE_TIME   (LSDRV_SPKMPH_AVE_TIME + 2)  /* Effective Time for Noise Detection(in units of 100ms) */  /* #017 */
#define LSDRV_SPKMPH_INVALID      1       /* No pulse of previous vehicle speed    */
#define LSDRV_SPKMPH_VALID        0       /* Last vehicle speed pulse    */
#define LSDRV_SENSCNT_BRW_ADD     256     /* Measures for borrow occurrence during search of sensor counters */
/* #011 end */
#define LSDRV_FSENS_NRCV          0       /* Not received          */
#define LSDRV_FSENS_RCV           1       /* Receiving          */
#define LSDRV_FSENS_RCVCOMP       2       /* Reception completion          */
#define LSDRV_FSENS_NORMAL        0       /* Receive data normal      */  /* #001 */
#define LSDRV_FSENS_ERROR         1       /* Receive data anomaly      */  /* #001 */
/* #016 start */
#define LSDRV_PLSDATA_NRCV        0       /* Not received          */
#define LSDRV_PLSDATA_RCV         1       /* Received          */
#define LSDRV_SENSCNT_MAX         255     /* Maximum value of sensor counter    */
#define LSDRV_SPKMPH_MIN_DATA_N   6       /* Number of data required for vehicle speed calculation    */
#define LSDRV_FSPDPLS_NUM         3       /* Number of cumulative pulses registered for the first time    */
/* #016 end */
#define LSDRV_REV_MASK            0x0040  /* REV-data MASK(Before endian conversion)      */
#define LSDRV_BITSHIFT_REV        6       /* REV data bit shift  */
#define LSDRV_TEMP_MASK           0x07FF  /* Gyro temperature data MASK(after endian conversion)  */
#define LSDRV_PLSTIME_LEN         232     /* Data length when inter-pulse time exists */


#define LSDRV_DETAILED_DATA_MULTI_ENABLE      TRUE    /* Detailed data multiplexing of initial sensor data Enabled/Disabled    */

#if LSDRV_DETAILED_DATA_MULTI_ENABLE
#define LSDRV_DETAILED_DATA_MULTI_MAX_NUM     2       /* Detailed data multiplexing of initial sensor data Maximum number    */
#else
#define LSDRV_DETAILED_DATA_MULTI_MAX_NUM     1       /* Detailed data multiplexing of initial sensor data Maximum number    */
#endif

/* Total number of partitions of initial sensor data : Maximum value */
#define LSDRV_FSENS_TOTAL_NUM_MAX             LSDRV_FSTSNS_SENSOR_FIRST_SEND_NUM

/* Detailed data of initial sensor data : Maximum number of acquisitions      */
#define LSDRV_DETAILED_DATA_MAX_NUM           (LSDRV_FSENS_TOTAL_NUM_MAX * LSDRV_DETAILED_DATA_MULTI_MAX_NUM)

/* Initial sensor data : Maximum number of saved messages      */
#define LSDRV_FSENS_SAVE_MSG_NUM              (LSDRV_FSENS_TOTAL_NUM_MAX + 1)

/* Initial sensor data : Maximum number of data stored        */
#define LSDRV_FSENS_SAVE_DATA_NUM             (LSDRV_FSENS_SAVE_MSG_NUM * LSDRV_DETAILED_DATA_MULTI_MAX_NUM)

#define LSDRV_DETAILED_DATA_GYRO_NUM          LSDRV_FSTSNS_DETAILED_DATA_GYRO_NUM
#define LSDRV_DETAILED_DATA_GSENSOR_NUM       LSDRV_FSTSNS_DETAILED_DATA_GSENSOR_NUM
#define LSDRV_DETAILED_DATA_GSENSOR_X_OFFSET  0      /* G-Sensor data X-axis offset included in detail data  */
#define LSDRV_DETAILED_DATA_GSENSOR_Y_OFFSET  1      /* G-Sensor data Y-axis offsets included in the detail data  */
#define LSDRV_DETAILED_DATA_GSENSOR_OFFSET    3      /* G-Sensor data access offset included in the detail data  */

/* Gyro Output Specifications:11500 ~ 54036LSB */
#define GYRO_OUTPUPT_SPEC_LOWER_LIMIT         11500U
#define GYRO_OUTPUPT_SPEC_UPPER_LIMIT         54036U

/* Gyro Failure Status : How many normal status continues should be made to Gyro Failure Status normal when it is judged to be "error -> normal" */
#define GYRO_TROUBLE_NORMAL_COUNTER_THRESHOLD 10U     /* 10 ms (Gyro data acquisition interval) * 10 times = 100ms */

/* Gyro Failure Status : How many error status continues should be made to Gyro Failure Status error when it is judged to be "normal -> error" */
#define GYRO_TROUBLE_ERROR_COUNTER_THRESHOLD  3000U   /* 10 ms (Gyro data acquisition interval) * 3000 times = 30s */

/* SYS GPS Interrupt Signal : How many disable continues should be made to SYS GPS Interrupt Signal disable when it is judged to be "enable -> disable" */
#define SYS_GPS_INTERRUPT_SIGNAL_INVALID_COUNTER_THRESHOLD 10U

/* Gyro Failure Status : Match values with SENSORMOTION_NORMAL and ERROR,UNFIXED in SensorMotion_API.h */
#define GYRO_NORMAL             (0U)
#define GYRO_ERROR              (1U)
#define GYRO_UNFIXED            (2U)

/* Gyro connection status : Match values with SENSOR_CONNECT and SENSOR_DISCONNECT in Sensor_API.h */
#define GYRO_DISCONNECT         (0U)
#define GYRO_CONNECT            (1U)

/*** Data table for calculating vehicle speed *****************************************/
#define LSDRV_SPKMPH_TBL_NUM    32      /* Number of data tables      */ /* #50836 */
#define LSDRV_SPKMPH_DATA_EN    0x01    /* Data storage flag valid    */
#define LSDRV_SPKMPH_DATA_DIS   0x00    /* Data storage flag invalid    */

/*** Operation code *********************************************/
#define LSDRV_OPC_WILDCARD      0xFF    /* Wildcarded      #012 */

#define LSDRV_OPC_EXTTERM_R     0x89    /* External pin status request      */
#define LSDRV_OPC_EXTTERM_A     0x99    /* External pin status notification      */
#define LSDRV_OPC_SENSOR        0xC1    /* Sensor data        */
#define LSDRV_OPC_FSENS_R       0xE4    /* Initial sensor data request    */
#define LSDRV_OPC_FSENS_A       0xF4    /* Initial sensor data response    */

/*---------------------------------------------------------------------------*/
// Enum

/*---------------------------------------------------------------------------*/
// Struct

typedef struct {
  u_int16   uc_sensor_x;            /* G-Sensor X data */
  u_int16   uc_sensor_y;            /* G-Sensor Y data */
  u_int16   uc_sensor_z;            /* G-Sensor Z data */
} SENSORINPUT_INFO_DAT_GSENS;

// TAG      : LSDRV_SPEEDKMPH_DAT
// ABSTRACT : Data Table Structure for Vehicle Speed Calculation
typedef struct LsDrvSpeedKmph {
  u_int8    uc_flag;                /* Data storage flag        */
  u_int8    uc_sens_cnt;            /* Sensor counter        */
  u_int16   us_speed_pulse;         /* Vehicle speed pulse          */
  u_int8    uc_noise_flag;          /* Noise flag          */  /* #017 */
  u_int8    u_reserve[3];           /* Reserved            */  /* #017 */
} LSDRV_SPEEDKMPH_DAT_DAT;

typedef struct LsDrvSpeedKmphLast {
  u_int8    uc_flag;                /* Data storage flag        */
  u_int8    uc_sens_cnt;            /* Sensor counter        */
  u_int16   us_speed_pulse;         /* Vehicle speed pulse          */
} LSDRV_SPEEDKMPH_LAST_DAT;

typedef struct LsDrvSpeedKmphDat {
  u_int8                    uc_ptr;                         /* Data storage pointer      */
  u_int8                    uc_fstsns_rcv;                  /* Initial sensor data reception status    */  /* #016 */
  u_int8                    uc_sns_rcv;                     /* Sensor data reception status      */  /* #016 */
  u_int8                    uc_calc_start;                  /* Vehicle speed calculation start flag      */  /* #016 */
  LSDRV_SPEEDKMPH_LAST_DAT  st_last_data;                   /* Previous vehicle speed pulse information      */
  LSDRV_SPEEDKMPH_DAT_DAT   st_data[LSDRV_SPKMPH_TBL_NUM];  /* Data portion  */
} LSDRV_SPEEDKMPH_DAT;

/*---------------------------------------------------------------------------*/
// Prototype

void    LineSensDrvSensor(u_int8*);
u_int16 LineSensDrvSpeedCalc(u_int8);
void    LineSensDrvSpeedKmphDataInit(void);
void    LineSensDrvSpeedPulseSave(u_int16, u_int16, u_int8);
u_int8  LineSensDrvCheckNoise(u_int8);

u_int8  LineSensDrvGetLastSpeedPulse(u_int16*, u_int16, u_int8);
void    LineSensDrvSetLastSpeedPulse(u_int16, u_int8);

/*---------------------------------------------------------------------------*/
#endif  // INC_LINESENSDRV_LINESENSDRV_SENSOR_H_

/*---------------------------------------------------------------------------*/
/*EOF*/
