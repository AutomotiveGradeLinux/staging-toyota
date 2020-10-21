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
* @file LineSensDrv_Snesor.cpp
*/

/*---------------------------------------------------------------------------*/
// Include files

#include "LineSensDrv_Sensor.h"
#include "LineSensDrv_Api.h"

/*---------------------------------------------------------------------------*/
// Value Define

#define LSDRV_MASK_WORD_L                   0x00FF
#define LSDRV_MASK_WORD_U                   0xFF00

#define LINE_SENS_DRV_SENSOR_DEBUG_FACTORY  0
#define LINE_SENS_DRV_SENSOR_DEBUG_DIAG     0

#define VEHICLE_SNS_INFO_PULSE_NUM          32

/*---------------------------------------------------------------------------*/
// Global variable

static LSDRV_SPEEDKMPH_DAT    g_speed_kmph_data;
static uint8_t                g_rcv_data_len;     /* Receive SYS data length */
extern uint8_t g_uc_vehicle_reverse;
/*---------------------------------------------------------------------------*/
// Functions

/*******************************************************************************
 * MODULE    : LineSensDrv_Sensor
 * ABSTRACT  : Sensor data reception processing
 * FUNCTION  : Convert sensor data to delivery format
 * ARGUMENT  : *ucRcvdata : Data pointer
 * NOTE      :
 * RETURN    : None
 ******************************************************************************/
void LineSensDrvSensor(u_int8* uc_rcvdata) {
  u_int8              uc_sens_cnt = 0;
  u_int16             us_sp_kmph = 0;         /* Vehicle speed(km/h)      */
  u_int16             us_sp_pls1 = 0;         /* Total vehicle speed pulse(Latest)  */
  u_int16             us_sp_pls2 = 0;         /* Total vehicle speed pulse(Last time)  */
  u_int8              us_sp_ret = 0;          /* Last vehicle speed information acquisition result  */
  u_int8              uc_size = 0;            /* Size of the data      */
  u_int16             us_cnt = 0;             /* Data counter    */
  u_int16             us_cnt2 = 0;            /* Data counter    */
  u_int8*             uc_data_pos = NULL;     /* Data storage location    */
  LSDRV_MSG_LSDATA_G* p_snd_buf = NULL;
  LSDRV_LSDATA_G*     p_snd_data_base = NULL;
  LSDRV_LSDATA_G*     p_snd_data = NULL;
  RET_API             ret = RET_NORMAL;

  /* Receive sensor data top address acquisition */
  uc_data_pos = (uc_rcvdata);

  /* Create send buffer/delivery all received data */
  ret = _pb_GetZcSndBuf(PNO_VEHICLE_SENSOR, reinterpret_cast<void**>(&p_snd_buf));
  if ((ret == RET_NORMAL) && (p_snd_buf != NULL)) {
    p_snd_data_base = p_snd_buf->st_para.st_data;
    (void)memset(p_snd_data_base, 0, LSDRV_KINDS_MAX * sizeof( LSDRV_LSDATA_G ));

    /* Sensor counter */
    p_snd_data              = p_snd_data_base + LSDRV_SNS_COUNTER;
    uc_sens_cnt             = (u_int8)*(uc_data_pos);
    p_snd_data->ul_did      = VEHICLE_DID_SNS_COUNTER;    /* Data ID       */
    p_snd_data->uc_size     = LSDRV_SNDMSG_DTSIZE_1;      /* Size of the data   */
    p_snd_data->uc_sns_cnt  = uc_sens_cnt;                /* Sensor counter */
    p_snd_data->uc_data[0]  = uc_sens_cnt;                /* Data content     */
    uc_data_pos             = (uc_data_pos + LSDRV_SNDMSG_DTSIZE_1);

    /* Gyro output */
    p_snd_data              = p_snd_data_base + LSDRV_GYRO_EXT;
    p_snd_data->ul_did      = VEHICLE_DID_GYRO_EXT;        /* Data ID       */
    p_snd_data->uc_size     = LSDRV_SNDMSG_DTSIZE_20;      /* Size of the data   */
    p_snd_data->uc_sns_cnt  = uc_sens_cnt;                 /* Sensor counter */
    for (us_cnt = 0; us_cnt < LSDRV_SNDMSG_DTSIZE_20; us_cnt++) {
      /* Since [0] is older and [9] is newer, the order of received data is switched. */
      /* Be in reverse order for endian conversion */
      p_snd_data->uc_data[LSDRV_SNDMSG_DTSIZE_20 - (us_cnt + 1)] = (u_int8)*(uc_data_pos + us_cnt);
    }
    uc_data_pos = (uc_data_pos + LSDRV_SNDMSG_DTSIZE_20);

    p_snd_data              = p_snd_data_base + LSDRV_GYRO_X;
    p_snd_data->ul_did      = VEHICLE_DID_GYRO;             /* Data ID       */
    p_snd_data->uc_size     = LSDRV_SNDMSG_DTSIZE_20;       /* Size of the data   */
    p_snd_data->uc_sns_cnt  = uc_sens_cnt;                  /* Sensor counter */
    for (us_cnt = 0; us_cnt < LSDRV_SNDMSG_DTSIZE_20; us_cnt++) {
      /* Since [0] is older and [9] is newer, the order of received data is switched. */
      /* Be in reverse order for endian conversion */
      p_snd_data->uc_data[LSDRV_SNDMSG_DTSIZE_20 - (us_cnt + 1)] = (u_int8)*(uc_data_pos + us_cnt);
    }
    uc_data_pos = (uc_data_pos + LSDRV_SNDMSG_DTSIZE_20);

    /* Reverse flag */
    p_snd_data              = p_snd_data_base + LSDRV_REV;
    p_snd_data->ul_did      = VEHICLE_DID_REV;              /* Data ID       */
    p_snd_data->uc_size     = LSDRV_SNDMSG_DTSIZE_1;        /* Size of the data   */
    p_snd_data->uc_sns_cnt  = uc_sens_cnt;                  /* Sensor Counter */
    p_snd_data->uc_data[0] = g_uc_vehicle_reverse;
    /* Gyro Temperature */
    p_snd_data              = p_snd_data_base + LSDRV_GYRO_TEMP;
    p_snd_data->ul_did      = VEHICLE_DID_GYRO_TEMP;        /* Data ID       */
    p_snd_data->uc_size     = LSDRV_SNDMSG_DTSIZE_2;        /* Size of the data   */
    p_snd_data->uc_sns_cnt  = uc_sens_cnt;                  /* Sensor counter */
    p_snd_data->uc_data[1]  = (u_int8)(*(uc_data_pos    ) & (u_int8)(LSDRV_TEMP_MASK >> 8));
    p_snd_data->uc_data[0]  = (u_int8)(*(uc_data_pos + 1) & (u_int8)(LSDRV_TEMP_MASK));
    uc_data_pos = (uc_data_pos + LSDRV_SNDMSG_DTSIZE_2);

    /* Vehicle speed pulse */
    p_snd_data              = p_snd_data_base + LSDRV_SPEED_PULSE;
    p_snd_data->ul_did      = VEHICLE_DID_SPEED_PULSE;      /* Data ID       */
    p_snd_data->uc_size     = LSDRV_SNDMSG_DTSIZE_20;       /* Size of the data   */
    p_snd_data->uc_sns_cnt  = uc_sens_cnt;                  /* Sensor counter */
    for (us_cnt = 0; us_cnt < LSDRV_SNDMSG_DTSIZE_20; us_cnt++) {
      /* Since [0] is older and [9] is newer, the order of received data is switched. */
      /* Be in reverse order for endian conversion */
      p_snd_data->uc_data[LSDRV_SNDMSG_DTSIZE_20 - (us_cnt + 1)] = (u_int8)*(uc_data_pos + us_cnt);
    }
    uc_data_pos = (uc_data_pos + LSDRV_SNDMSG_DTSIZE_20);

    /* Vehicle speed(km/h) */
    p_snd_data              = p_snd_data_base + LSDRV_SPEED_PULSE;
    us_sp_kmph              = 0;
    us_sp_pls1              = (u_int16)p_snd_data->uc_data[1];
    us_sp_pls1              = (u_int16)((us_sp_pls1 << 8) | p_snd_data->uc_data[0]);
    us_sp_ret               = LineSensDrvGetLastSpeedPulse(&us_sp_pls2, us_sp_pls1, uc_sens_cnt);

    LineSensDrvSetLastSpeedPulse(us_sp_pls1, uc_sens_cnt);

    p_snd_data            = p_snd_data_base + LSDRV_SPEED_KMPH;
    if (us_sp_ret != LSDRV_SPKMPH_INVALID) {
      /* Vehicle speed pulse before 100 ms is valid */
      LineSensDrvSpeedPulseSave(us_sp_pls1, us_sp_pls2, uc_sens_cnt);
      us_sp_kmph = LineSensDrvSpeedCalc(uc_sens_cnt);
      /* The size can be set only when the vehicle speed [km/h] is calculated. "0" is notified to the vehicle sensor when the size cannot be set. */
      p_snd_data->uc_size  = LSDRV_SNDMSG_DTSIZE_2;      /* Size of the data   */
    }
    p_snd_data->ul_did      = VEHICLE_DID_SPEED_KMPH;       /* Data ID       */
    p_snd_data->uc_sns_cnt  = uc_sens_cnt;                  /* Sensor counter */
    p_snd_data->uc_data[0]  = (u_int8)(us_sp_kmph & 0x00FF);
    p_snd_data->uc_data[1]  = (u_int8)(us_sp_kmph >> 8);

    POSITIONING_LOG("[LOG, %d(cnt), %d(km/h), %d(pls1), %d(pls2)]  \r\n",
      uc_sens_cnt,
      us_sp_kmph,
      us_sp_pls1,
      us_sp_pls2);

    /* G-Sensor X-axes */
    p_snd_data              = p_snd_data_base + LSDRV_GSENSOR_X;
    p_snd_data->ul_did      = VEHICLE_DID_GSNS_X;           /* Data ID       */
    p_snd_data->uc_size     = LSDRV_SNDMSG_DTSIZE_20;       /* Size of the data   */
    p_snd_data->uc_sns_cnt  = uc_sens_cnt;                  /* Sensor counter */
    us_cnt2                 = p_snd_data->uc_size - 1;
    for (us_cnt = 0; us_cnt < 10; us_cnt++) {
      /* Since [0] is older and [9] is newer, the order of received data is switched. */
      p_snd_data->uc_data[us_cnt2    ] = (u_int8)*( uc_data_pos + (sizeof(SENSORINPUT_INFO_DAT_GSENS) * us_cnt)    );
      p_snd_data->uc_data[us_cnt2 - 1] = (u_int8)*( uc_data_pos + (sizeof(SENSORINPUT_INFO_DAT_GSENS) * us_cnt) + 1);
      us_cnt2 = us_cnt2 - 2;
    }

    /* G-Sensor Y-axes */
    p_snd_data              = p_snd_data_base + LSDRV_GSENSOR_Y;
    p_snd_data->ul_did      = VEHICLE_DID_GSNS_Y;           /* Data ID       */
    p_snd_data->uc_size     = LSDRV_SNDMSG_DTSIZE_20;       /* Size of the data   */
    p_snd_data->uc_sns_cnt  = uc_sens_cnt;                  /* Sensor counter */
    us_cnt2                 = p_snd_data->uc_size - 1;
    for (us_cnt = 0; us_cnt < 10; us_cnt++) {
      /* Since [0] is older and [9] is newer, the order of received data is switched. */
      p_snd_data->uc_data[us_cnt2] = (u_int8)*( (uc_data_pos + sizeof(u_int16)) \
                                           + (sizeof(SENSORINPUT_INFO_DAT_GSENS) * us_cnt));

      p_snd_data->uc_data[us_cnt2-1] = (u_int8)*( (uc_data_pos + sizeof(u_int16)) \
                                           + (sizeof(SENSORINPUT_INFO_DAT_GSENS) * us_cnt) + 1);
      us_cnt2 = us_cnt2 - 2;
    }
    uc_data_pos = ( uc_data_pos + (sizeof(SENSORINPUT_INFO_DAT_GSENS) * 10) );

    /* Inter-Pulse time */
    p_snd_data              = p_snd_data_base + LSDRV_PULSE_TIME;
    p_snd_data->ul_did      = VEHICLE_DID_PULSE_TIME;       /* Data ID       */
    p_snd_data->uc_size     = LSDRV_SNDMSG_DTSIZE_132;      /* Size of the data   */
    p_snd_data->uc_sns_cnt  = uc_sens_cnt;                  /* Sensor counter */

    /* Clear the buffer for variable length */
    (void)memset(&p_snd_data->uc_data[0], 0x00, sizeof(p_snd_data->uc_data));

    if (g_rcv_data_len == LSDRV_PLSTIME_LEN) {
      /* Inter-Pulse time (number of items + time). The number of items is stored at the beginning. */
      uc_size = (u_int8)*uc_data_pos;
      if (uc_size > VEHICLE_SNS_INFO_PULSE_NUM) {
        uc_size = VEHICLE_SNS_INFO_PULSE_NUM;
      }
      p_snd_data->uc_data[0] = uc_size;
      p_snd_data->uc_data[1] = 0x00;
      p_snd_data->uc_data[2] = 0x00;
      p_snd_data->uc_data[3] = 0x00;
      uc_data_pos = ( uc_data_pos + sizeof(u_int8) );

      /* Since [0] is old and [31] is new in the received time, the order is changed. */
      /* Be in reverse order for endian conversion */
      for (us_cnt = 0; us_cnt < (uc_size * sizeof(u_int32)); us_cnt++) {
        p_snd_data->uc_data[(uc_size * sizeof(u_int32)) - (us_cnt + 1) + 4] = (u_int8)*(uc_data_pos + us_cnt);
      }
    }

    /* Messaging */
    DeliveryLineSensorDataPositioning(p_snd_buf, LSDRV_KINDS_MAX);
  }

  return;
}

/*******************************************************************************
 * MODULE    : LineSensDrv_SpeedCalc
 * ABSTRACT  : Vehicle speed calculation processing for sensor data
 * FUNCTION  : Calculate vehicle speed for sensor data
 * ARGUMENT  : uc_sens_cnt : Sensor counter
 * NOTE      :
 * RETURN    : Vehicle speed(0.01km/h)
 ******************************************************************************/
u_int16 LineSensDrvSpeedCalc(u_int8 uc_sens_cnt) {
  u_int32 ul_sp_caluc = 0;          /* Vehicle speed(2^-8km/h)  */
  u_int16 us_speed = 0;             /* Vehicle speed(km/h)    */
  u_int8  uc_ptr = 0;               /* Data storage pointer for sensor data  */  /* #010 */
  u_int8  uc_ptr2 = 0;              /* Data storage pointer for sensor data  */  /* #010 */
  u_int32 ul_work = 0;              /* Variables for unsigned 32-bit type calculations      */  /* #010 */
  double  d_work = 0.0;             /* Variables for calculating double types          */  /* #010 */  /* Ignore -> MISRA-C++:2008 Rule 3-9-2 */
  u_int16 us_sens_cnt_search = 0;   /* Sensor counter to be searched        */  /* #010 */
  u_int16 us_sens_cnt_ref = 0;      /* Sensor counters to be compared      */  /* #010 */
  int32   i = 0;                    /* Generic counters              */  /* #010 */
  int32   j = 0;                    /* Generic counters              */  /* #010 */
  u_int16 us_offset = 0;            /* Offset value              */  /* #010 */

  /* #016 start */
  /* Is the number of data that can be calculated at the vehicle speed already received? */
  if (LSDRV_PLSDATA_NRCV == g_speed_kmph_data.uc_calc_start) {
    /* Do not compute if there is not enough data. */
  } else {
    if (0 == g_speed_kmph_data.uc_ptr) {
      uc_ptr  = LSDRV_SPKMPH_TBL_NUM - 1;
      uc_ptr2 = LSDRV_SPKMPH_TBL_NUM - 1;
    } else {
      uc_ptr  = g_speed_kmph_data.uc_ptr - 1;  /* Ignore -> MISRA-C++:2008 Rule 5-0-5 */
      uc_ptr2 = g_speed_kmph_data.uc_ptr - 1;  /* Ignore -> MISRA-C++:2008 Rule 5-0-5 */
    }

    ul_work = 0;
    if (LSDRV_SPKMPH_AVE_TIME > uc_sens_cnt) {
      us_offset = LSDRV_SENSCNT_BRW_ADD;
    } else {
      us_offset = 0;
    }

    us_sens_cnt_search = (u_int16)uc_sens_cnt + us_offset - LSDRV_SPKMPH_AVE_TIME;

    for (i = 0; i < LSDRV_SPKMPH_TBL_NUM; i++) {
      /* Invalid data is detected, and the search is completed. */
      if (LSDRV_SPKMPH_DATA_DIS == g_speed_kmph_data.st_data[uc_ptr].uc_flag) {
        break;
        }

      /* When the sensor counter is 29 or less, the borrow is considered. */
      if (LSDRV_SPKMPH_AVE_TIME > g_speed_kmph_data.st_data[uc_ptr].uc_sens_cnt) {
        us_sens_cnt_ref = g_speed_kmph_data.st_data[uc_ptr].uc_sens_cnt + us_offset;
      } else {
        us_sens_cnt_ref = g_speed_kmph_data.st_data[uc_ptr].uc_sens_cnt;
      }

      /* Checking the sensor counter to finish search */
      if (us_sens_cnt_search >= us_sens_cnt_ref) {
        break;
      }

      /* Add to calculate average value */
      ul_work += (u_int32)g_speed_kmph_data.st_data[uc_ptr].us_speed_pulse;

      if (0 == uc_ptr) {
        uc_ptr = LSDRV_SPKMPH_TBL_NUM - 1;                /* To the end of the data table */
      } else {
        uc_ptr--;                            /* To the previous data */
      }
    }

    /* Averaging computation */
    if (0 == i) {
      d_work = 0;
    } else {
      if (ul_work == 1) {
        for (j = 0; j < LSDRV_SPKMPH_TBL_NUM; j++) {
          if ((g_speed_kmph_data.st_data[uc_ptr2].us_speed_pulse == 1)
            && (g_speed_kmph_data.st_data[uc_ptr2].uc_noise_flag == 1)) {
            ul_work = 0;
            break;
          } else if (g_speed_kmph_data.st_data[uc_ptr2].us_speed_pulse == 1 &&
            g_speed_kmph_data.st_data[uc_ptr2].uc_noise_flag == 0) {
            ul_work = 1;
            break;
          } else {
            /* nop */
          }

          /* Borrow actions for pointer values */
          if (0 == uc_ptr2) {
            uc_ptr2 = LSDRV_SPKMPH_TBL_NUM - 1;                /* To the end of the data table */
          } else {
            uc_ptr2--;                            /* To the previous data */
          }
        }
      }

      d_work = static_cast<double>(ul_work);
      d_work = d_work / static_cast<double>(i);
      d_work = d_work * static_cast<double>(LSDRV_SENS_COEFFICIENT);
      d_work = d_work * 100;               /* [1km/h] -> [0.01km/h] */
      d_work = d_work + 0.5;              /* Preparation for rounding */
    }

    ul_sp_caluc = static_cast<int32>(d_work);

    /* When the vehicle speed calculation result overflows, the upper limit value is used for clipping. */
    if (LSDRV_PLSMAX - 1 >= ul_sp_caluc) {
      us_speed = (u_int16)ul_sp_caluc;
    } else {
      us_speed = (u_int16)(LSDRV_PLSMAX - 1);
    }
  }

  return us_speed;
}

/*******************************************************************************
 * MODULE    : LineSensDrv_SpeedKmphDataInit
 * ABSTRACT  : Data table initialization process for vehicle speed calculation
 * FUNCTION  : Initialize the data table for calculating the vehicle speed
 * ARGUMENT  : None
 * NOTE      :
 * RETURN    : None
 ******************************************************************************/
void LineSensDrvSpeedKmphDataInit(void) {
  int32 i = 0;

  memset(reinterpret_cast<void*>(&g_speed_kmph_data), static_cast<int32>(0), (size_t)sizeof(g_speed_kmph_data));

  /* Disable all data storage flags */
  for (i = 0; i < LSDRV_SPKMPH_TBL_NUM; i++) {
    g_speed_kmph_data.st_data[i].uc_flag = LSDRV_SPKMPH_DATA_DIS;
  }

  return;
}

/*******************************************************************************
 * MODULE    : LineSensDrv_SpeedPulseSave
 * ABSTRACT  : Sensor data vehicle speed pulse save processing
 * FUNCTION  : Saving the vehicle speed pulse of the sensor data to the data table for calculating the vehicle speed 
 * ARGUMENT  : us_sp1     : Vehicle speed pulse of the latest information
 *           : us_sp2     : Vehicle speed pulse of the previous information
 *           : uc_sens_cnt : Sensor counter
 * NOTE      :
 * RETURN    : None
 ******************************************************************************/
void LineSensDrvSpeedPulseSave(u_int16 us_sp1, u_int16 us_sp2, u_int8 uc_sens_cnt) {
  u_int16 us_sp_diff = 0;     /* Vehicle speed pulse difference  */
  u_int8  uc_ptr = 0;         /* Data storage pointer for sensor data  */  /* #010 */
  u_int8  us_last_ptr = 0;    /* Last pointer              */  /* #010 */
  int32   i = 0;              /* Generic counters              */  /* #010 */
  u_int8  uc_fstsns_cnt = 0;  /* Initial sensor data sensor counter value  */  /* #016 */
  int32   l_data_num = 0;     /* Number of registered data              */  /* #016 */
  u_int8  uc_noise_flag = 0;  /* Noise flag              */  /* #017 */

  /* Calculate the vehicle speed pulse difference between the latest and last 100ms information */
  if (us_sp1 >= us_sp2) {
    /* The cumulative pulse number of the latest information is larger. */
    us_sp_diff = us_sp1 - us_sp2;  /* Ignore -> MISRA-C++:2008 Rule 5-0-5 */
  } else {
    /* The cumulative pulse number of the latest information is smaller (the accumulated pulse overflows) */
    us_sp_diff = (LSDRV_PLSMAX - us_sp2) + us_sp1;  /* Ignore -> MISRA-C++:2008 Rule 5-0-5 */
  }

  /* Call noise check only if us_sp_diff is 1 */
  if (us_sp_diff == 1) {
    uc_noise_flag = LineSensDrvCheckNoise(uc_sens_cnt);
  }

  /* Saving sensor data vehicle speed pulse in data table for vehicle speed calculation */
  if (LSDRV_PLSDATA_NRCV == g_speed_kmph_data.uc_sns_rcv) {
    if (LSDRV_PLSDATA_RCV == g_speed_kmph_data.uc_fstsns_rcv) {
      /* If the sensor data has already been received for the first time, set the temporary sensor counter value when the sensor data has been saved for the first time. */
      uc_fstsns_cnt = uc_sens_cnt;
      for (i = (LSDRV_SPKMPH_TBL_NUM - 1); i >= 0 ; i--) {
        if (LSDRV_SPKMPH_DATA_EN == g_speed_kmph_data.st_data[i].uc_flag) {
          /* Data storage flag is valid */
          if (0 != uc_fstsns_cnt) {
            uc_fstsns_cnt--;
          } else {
            uc_fstsns_cnt = LSDRV_SENSCNT_MAX;
          }

          g_speed_kmph_data.st_data[i].uc_sens_cnt = uc_fstsns_cnt;
        }
      }
    }

    /* Sensor data reception status  <-  "Received" */
    g_speed_kmph_data.uc_sns_rcv = LSDRV_PLSDATA_RCV;
  }

  uc_ptr = g_speed_kmph_data.uc_ptr;

  /* If the sensor counter is the same as the previous one, overwrite update */
  if (0 == uc_ptr) {
    us_last_ptr = LSDRV_SPKMPH_TBL_NUM - 1;
  } else {
    us_last_ptr = uc_ptr - 1;   /* Ignore -> MISRA-C++:2008 Rule 5-0-5 */
  }

  if (g_speed_kmph_data.st_data[us_last_ptr].uc_sens_cnt == uc_sens_cnt) {
    /* Next update of the data storage location */
    if (0 == g_speed_kmph_data.uc_ptr) {
      g_speed_kmph_data.uc_ptr = LSDRV_SPKMPH_TBL_NUM - 1;
    } else {
      g_speed_kmph_data.uc_ptr--;
    }

    uc_ptr = g_speed_kmph_data.uc_ptr;
  }

  /* Input into data table for calculation of vehicle speed */
  g_speed_kmph_data.st_data[uc_ptr].uc_flag         = LSDRV_SPKMPH_DATA_EN; /* Data validity      */
  g_speed_kmph_data.st_data[uc_ptr].uc_sens_cnt     = uc_sens_cnt;          /* Sensor counter input  */
  g_speed_kmph_data.st_data[uc_ptr].us_speed_pulse  = us_sp_diff;           /* Vehicle speed pulse difference    */
  g_speed_kmph_data.st_data[uc_ptr].uc_noise_flag   = uc_noise_flag;        /* Noise flag      */ /* #017 */

  /* Next update of the data storage location */
  if ((LSDRV_SPKMPH_TBL_NUM - 1) <= g_speed_kmph_data.uc_ptr) {
    g_speed_kmph_data.uc_ptr = 0;
  } else {
    g_speed_kmph_data.uc_ptr++;
  }

  /* Determine whether the vehicle speed can be calculated. */
  if (g_speed_kmph_data.uc_calc_start == LSDRV_PLSDATA_NRCV) {
    /* Data count detection */
    l_data_num = 0;
    for (i = 0; i < LSDRV_SPKMPH_TBL_NUM; i++) {
      if (LSDRV_SPKMPH_DATA_EN == g_speed_kmph_data.st_data[i].uc_flag) {
          l_data_num++;
      }
    }

    if (LSDRV_SPKMPH_MIN_DATA_N <= l_data_num) {
      /* Vehicle Speed Calculation Required Data Number Received */
      g_speed_kmph_data.uc_calc_start = LSDRV_PLSDATA_RCV;
    }
  }

  return;
}

/*******************************************************************************
 * MODULE    : LineSensDrv_CheckNoise
 * ABSTRACT  : Sensor data noise check processing
 * FUNCTION  : The vehicle speed pulse is saved in the data table for the vehicle speed calculation. 
 * ARGUMENT  : uc_sens_cnt     : Sensor counter of the latest data
 * NOTE      :
 * RETURN    : Noise flag
 ******************************************************************************/
u_int8 LineSensDrvCheckNoise(u_int8 uc_sens_cnt) {
  int32   i = 0;                  /* Generic counters */
  u_int16 us_sens_cnt_search = 0; /* Sensor counter to be searched*/
  u_int8  uc_ptr = 0;             /* Data storage pointer */
  u_int16 us_offset = 0;          /* Offset value  */
  u_int8  noise_flag = 0;         /* Noise flag  */
  u_int16 us_sens_cnt_ref = 0;    /* Sensor counters to be compared */

  /* If there is no point where the difference in vehicle speed pulse is 1 or more 
     between -1 and -14 of sensor counter of the latest data, 
     set the noise flag of the latest data to 1. */
  /* Set the noise flag to 1 */
  noise_flag = 1;

  /* The Target is the one before the storage location of the latest data. */
  if (0 == g_speed_kmph_data.uc_ptr) {
    uc_ptr = LSDRV_SPKMPH_TBL_NUM - 1;
  } else {
    uc_ptr = g_speed_kmph_data.uc_ptr - 1;  /* Ignore -> MISRA-C++:2008 Rule 5-0-5 */
  }

  if (LSDRV_SPKMPH_NOISE_TIME > uc_sens_cnt) {
    us_offset = LSDRV_SENSCNT_BRW_ADD;
  } else {
    us_offset = 0;
  }

  us_sens_cnt_search = (u_int16)uc_sens_cnt + us_offset - LSDRV_SPKMPH_NOISE_TIME;

  for (i = 0; i < LSDRV_SPKMPH_NOISE_TIME; i++) {
    /* When the sensor counter is 15 or less, the borrow is considered. */
    if (LSDRV_SPKMPH_NOISE_TIME > g_speed_kmph_data.st_data[uc_ptr].uc_sens_cnt) {
      us_sens_cnt_ref = g_speed_kmph_data.st_data[uc_ptr].uc_sens_cnt + us_offset;
    } else {
      us_sens_cnt_ref = g_speed_kmph_data.st_data[uc_ptr].uc_sens_cnt;  /* Ignore -> MISRA-C++:2008 Rule 5-0-5 */
    }

    /* Checking the sensor Counter to Finish Search */
    if (us_sens_cnt_ref <= us_sens_cnt_search) {
      noise_flag = 1;
      break;
    } else {
      if (g_speed_kmph_data.st_data[uc_ptr].us_speed_pulse >= 1) {
        noise_flag = 0;
        break;
      }
    }

    /* Borrow actions for pointer values */
    if (0 == uc_ptr) {
      uc_ptr = LSDRV_SPKMPH_TBL_NUM - 1;                /* To the end of the data table */
    } else {
      uc_ptr--;                            /* To the previous data */
    }
  }

  return noise_flag;
}

/**
 * @brief
 *  Return the last (100ms ago) vehicle speed pulse 
 *
 * @param[out]  us_sp2    Last vehicle speed pulse
 * @param[in]  us_sp1    The latest vehicle speed pulse
 * @param[in]  uc_sens_cnt  Latest sensor counter
 *
 * @return  LSDRV_SPKMPH_INVALID  Vehicle speed pulse information valid<br>
 *      LSDRV_SPKMPH_VALID    Vehicle speed pulse information invalid
 */
u_int8 LineSensDrvGetLastSpeedPulse(u_int16* us_sp2, u_int16 us_sp1, u_int8 uc_sens_cnt) {
  u_int8  ret = LSDRV_SPKMPH_INVALID;   /* Return value        */
  u_int16 sp_pls_diff = 0;              /* Vehicle speed pulse difference    */
  u_int16 sp_pls = 0;                   /* Vehicle speed pulse every 100 ms  */
  u_int8  cnt_diff = 0;                 /* Sensor counter difference  */

  /* Check if the last vehicle speed pulse has been set  */
  if (g_speed_kmph_data.st_last_data.uc_flag == LSDRV_SPKMPH_DATA_EN) {
    /* Differential calculation of sensor counter */
    if (uc_sens_cnt >= g_speed_kmph_data.st_last_data.uc_sens_cnt) {
      /* Latest sensor counter is larger */
      cnt_diff = uc_sens_cnt - g_speed_kmph_data.st_last_data.uc_sens_cnt;
    } else {
      /* Last sensor counter is larger(sensor counter overflows) */
      cnt_diff = (LSDRV_SENSCNT_MAX - g_speed_kmph_data.st_last_data.uc_sens_cnt) + uc_sens_cnt + 1;
  }

  /* Check if sensor counter is continuous */
  if (cnt_diff <= 1) {
    /* Continuous or same as the previous one, so the previous (100ms previous) vehicle speed pulse is set as it is */
    *us_sp2 = g_speed_kmph_data.st_last_data.us_speed_pulse;
  } else {
    /* Determine the vehicle speed pulse 100ms ago from the average considering the skipped portion because it is not consecutive. */
    if (us_sp1 >= g_speed_kmph_data.st_last_data.us_speed_pulse) {
      /* Larger latest cumulative vehicle speed pulse */
      sp_pls_diff = us_sp1 - g_speed_kmph_data.st_last_data.us_speed_pulse;
      } else {
        /* Last cumulative vehicle speed pulse is larger(Cumulative vehicle speed pulse overflows) */
        sp_pls_diff = (LSDRV_PLSMAX - g_speed_kmph_data.st_last_data.us_speed_pulse) + us_sp1;
      }

      /* Calculate average vehicle speed pulse including skip period */
      sp_pls = (u_int16)(sp_pls_diff / cnt_diff);

      /* Calculate the vehicle speed pulse 100ms ahead from the vehicle speed average */
      if (us_sp1 >= sp_pls) {
        /* Does not overflow even if the 100ms vehicle speed pulse is pulled from the latest one. */
        *us_sp2 = us_sp1 - sp_pls;
      } else {
        /* Subtracting a 100ms vehicle speed pulse from the latest one overflows */
        *us_sp2 = LSDRV_PLSMAX - (sp_pls - us_sp1);
      }
    }

    ret = LSDRV_SPKMPH_VALID;
  }

  return ret;
}

/**
 * @brief
 *  Return the last (100ms ago) vehicle speed pulse 
 *
 * @param[in]  us_sp      Vehicle speed pulse
 * @param[in]  uc_sens_cnt  Sensor counter
 */
void LineSensDrvSetLastSpeedPulse(u_int16 us_sp, u_int8 uc_sens_cnt) {
  /* Vehicle speed pulse information valid setting */
  g_speed_kmph_data.st_last_data.uc_flag = LSDRV_SPKMPH_DATA_EN;
  /* Sensor counter setting */
  g_speed_kmph_data.st_last_data.uc_sens_cnt = uc_sens_cnt;
  /* Vehicle speed pulse setting */
  g_speed_kmph_data.st_last_data.us_speed_pulse = us_sp;
}

/*---------------------------------------------------------------------------*/
/*EOF*/
