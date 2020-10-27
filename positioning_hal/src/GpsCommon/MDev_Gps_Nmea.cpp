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
* @file MDev_Gps_Nmea.cpp
*/

/*---------------------------------------------------------------------------*/
// Include files

#include "MDev_Gps_Nmea.h"
#include "positioning_common.h"
#include "MDev_Gps_API.h"
#include "MDev_Gps_Main.h"
#include "MDev_GpsRecv.h"
#include "MDev_Gps_Common.h"
#include "MDev_GpsRollOver.h"

/*---------------------------------------------------------------------------*/
// Value define
#define ROLOVR_GPSWEEKCOR_NG            0xFF          /* WEEK COMP. CORRECT CORRECTOR INVALUE */
#define TMT_TGET_INI_INTERVAL           (100)         /* GPS time compensation interval immediately after startup (value with 10[msec] as 1) */
#define TMT_DISCONTINUITY_TIME_TOUT_SEQ (0x5051)      /* Sequence number of GPS time acquisition timeout based on date/time discontinuity */

/*---------------------------------------------------------------------------*/
// Global values

static TG_GPS_RCVDATA  g_st_gpscycle_data;
u_int8 g_gps_week_cor_cnt = ROLOVR_GPSWEEKCOR_NG;

extern uint8_t g_gpstime_raw_tdsts;

const TG_GPS_CMD_ANA_TBL kGpsCmdAnaTblUblox[MDEV_GPSCMDANATBLNMEA_MAX] = {
  /* Sentences                 Maximum length             Receiving type     Presence of notification  Receive Format    */
  /* NMEA */
  {{GPS_CMD_NMEA_RMC},        GPS_NMEA_MAX_SZ,  RCV_CYCLE,  TRUE,   GPS_FORMAT_RMC          },  /* RMC information        */
  {{GPS_CMD_NMEA_VTG},        GPS_NMEA_MAX_SZ,  RCV_CYCLE,  TRUE,   GPS_FORMAT_VTG          },  /* VTG information        */
  {{GPS_CMD_NMEA_GGA},        GPS_NMEA_MAX_SZ,  RCV_CYCLE,  TRUE,   GPS_FORMAT_GGA          },  /* GGA information        */
  {{GPS_CMD_NMEA_GSA},        GPS_NMEA_MAX_SZ,  RCV_CYCLE,  TRUE,   GPS_FORMAT_GSA          },  /* GSA information        */
  {{GPS_CMD_NMEA_GSV_1},      GPS_NMEA_MAX_SZ,  RCV_CYCLE,  TRUE,   GPS_FORMAT_GSV1         },  /* GSV information1       */
  {{GPS_CMD_NMEA_GSV_2},      GPS_NMEA_MAX_SZ,  RCV_CYCLE,  TRUE,   GPS_FORMAT_GSV2         },  /* GSV information2       */
  {{GPS_CMD_NMEA_GSV_3},      GPS_NMEA_MAX_SZ,  RCV_CYCLE,  TRUE,   GPS_FORMAT_GSV3         },  /* GSV information3       */
  {{GPS_CMD_NMEA_GSV_4},      GPS_NMEA_MAX_SZ,  RCV_CYCLE,  TRUE,   GPS_FORMAT_GSV4         },  /* GSV information4       */
  {{GPS_CMD_NMEA_GSV_5},      GPS_NMEA_MAX_SZ,  RCV_CYCLE,  TRUE,   GPS_FORMAT_GSV5         },  /* GSV information5       */
  {{GPS_CMD_NMEA_GLL},        GPS_NMEA_MAX_SZ,  RCV_CYCLE,  TRUE,   GPS_FORMAT_GLL          },  /* GLL information        */
  {{GPS_CMD_NMEA_GST},        GPS_NMEA_MAX_SZ,  RCV_CYCLE,  TRUE,   GPS_FORMAT_GST          },  /* GST information        */
  /* UBX */
  {{0xB5, 0x62, 0x0A, 0x04},  GPS_NMEA_MAX_SZ,  RCV_RESP,   TRUE,   GPS_FORMAT_MON_VER      },  /* MON-VER        */
  {{0xB5, 0x62, 0x0B, 0x01},  GPS_NMEA_MAX_SZ,  RCV_RESP,   TRUE,   GPS_FORMAT_AID_INI      },  /* AID-INI        */
  {{0xB5, 0x62, 0x05      },  GPS_NMEA_MAX_SZ,  RCV_RESP,   TRUE,   GPS_FORMAT_ACK_ACKNACK  },  /* ACK-ACKNACK    */
  {{0xB5, 0x62, 0x01, 0x21},  GPS_NMEA_MAX_SZ,  RCV_CYCLE,  TRUE,   GPS_FORMAT_NAV_TIMEUTC  },  /* NAV-TIMEUTC    */
  {{0xB5, 0x62, 0x01, 0x22},  GPS_NMEA_MAX_SZ,  RCV_CYCLE,  TRUE,   GPS_FORMAT_NAV_CLOCK    },  /* NAV-CLOCK      */
  {{0xB5, 0x62, 0x02, 0x23},  GPS_NMEA_MAX_SZ,  RCV_CYCLE,  TRUE,   GPS_FORMAT_RXM_RTC5     },  /* RXM-RTC5       */
  {{0xB5, 0x62, 0x01, 0x30},  GPS_NMEA_MAX_SZ,  RCV_CYCLE,  TRUE,   GPS_FORMAT_NAV_SVINFO   },  /* NAV-SVINFO     */
  {{0xB5, 0x62, 0x06, 0x23},  GPS_NMEA_MAX_SZ,  RCV_RESP,   TRUE,   GPS_FORMAT_CFG_NAVX5    },  /* CFG-NAVX5      */
  {{ENDMARK},                 0,                0,          FALSE,  GPS_FORMAT_MIN          }   /* Table termination */
};

/*---------------------------------------------------------------------------*/
// Functions
/**
 * @brief
 *  Time Calculation Process Considering Rollover
 *
 * @param[in/out]
 *
 * @return      none
 * @retval      none
 */
static BOOL DevCalcRollOverTime(TG_TIM_ROLOVR_YMD* base_ymd, TG_TIM_ROLOVR_YMD* conv_ymd) {
    BOOL ret = TRUE;
    static u_int16 year_old = 0;
    static u_int16 confirm_cnt = 0;
    u_int8 gps_week_corr = g_gps_week_cor_cnt;

    if (gps_week_corr == ROLOVR_GPSWEEKCOR_NG) {
        gps_week_corr = 0;
        ret = FALSE;
    } else {
        if (year_old > base_ymd->year) {
            confirm_cnt++;
            if (confirm_cnt >= 10) {
                confirm_cnt = 0;
                year_old = base_ymd->year;
                g_gps_week_cor_cnt = ROLOVR_GPSWEEKCOR_NG;
                /* Starting the GPS time acquisition timer (1 second timer) based on date/time discontinuity */
                _pb_ReqTimerStart(PNO_CLK_GPS, TMT_DISCONTINUITY_TIME_TOUT_SEQ, TIMER_TYPE_USN, TMT_TGET_INI_INTERVAL);
            }
            ret = FALSE;
        } else {
            confirm_cnt = 0;
            year_old = base_ymd->year;
        }
    }
    /* Calculate time for rollover */
    GPSRollOverConvTime(base_ymd, conv_ymd, gps_week_corr);

    return ret;
}
/******************************************************************************************************/


/**
 * @brief
 *  NMEA data notification
 */
void DevGpsSndCycleDataNmea(void) {
  /* Notifying vehicle sensor of NMEA */

  MDEV_GPS_NMEA    st_nmea;
  TG_GPS_NMEA_INFO st_gps_nmea_info;
  RET_API          l_ret = RET_NORMAL;
  BOOL             b_get = FALSE;
  u_int8           uc_nmea_data[GPS_NMEA_MAX_SZ];
  u_int32          ul_strlen = 0;
  u_int16          us_offset = sizeof(TG_GPS_NMEA_INFO);

  memset(&st_nmea, 0x00, sizeof(st_nmea) ); /* QAC 3200 */
  memset(&st_gps_nmea_info, 0x00, sizeof(st_gps_nmea_info) ); /* QAC 3200 */
  memset(uc_nmea_data, 0x00, sizeof(uc_nmea_data) ); /* QAC 3200 */

  /* Get received NMEA data from storage area(GGA) */
  b_get = DevGpsCycleDataGetNmea(uc_nmea_data, sizeof(uc_nmea_data) , GPS_NMEA_INDEX_GGA);
  if (b_get == TRUE) {
    /* Data present */
    st_gps_nmea_info.ul_rcvsts |= POS_SNS_GPS_NMEA_GGA;       /* Receive flag */
    ul_strlen = strlen(reinterpret_cast<const char *>(uc_nmea_data)); /* QAC 310 */

    if (ul_strlen > GPS_NMEA_MAX_SZ) {
      ul_strlen = GPS_NMEA_MAX_SZ;
    }

    if ((us_offset + ul_strlen) < SENSOR_MSG_VSINFO_DSIZE) {
      (void)memcpy(&(st_nmea.uc_nmea_data[us_offset]), uc_nmea_data, ul_strlen);  /* Data storage */
      st_gps_nmea_info.st_nmea_sentence_info[POS_SNS_GPS_NMEA_SNO_GGA].uc_size = (u_int8)ul_strlen;
      st_gps_nmea_info.st_nmea_sentence_info[POS_SNS_GPS_NMEA_SNO_GGA].us_offset = us_offset;
      us_offset += (u_int16)ul_strlen;
    }
  }

  /* Get received NMEA data from storage area(GLL) */
  b_get = DevGpsCycleDataGetNmea(uc_nmea_data, sizeof(uc_nmea_data) , GPS_NMEA_INDEX_GLL);
  if (b_get == TRUE) {
    /* Data present */
    st_gps_nmea_info.ul_rcvsts |= POS_SNS_GPS_NMEA_GLL;       /* Receive flag */
    ul_strlen = strlen(reinterpret_cast<const char *>(uc_nmea_data)); /* QAC 310 */

    if (ul_strlen > GPS_NMEA_MAX_SZ) {
        ul_strlen = GPS_NMEA_MAX_SZ;
    }

    if ((us_offset + ul_strlen) < SENSOR_MSG_VSINFO_DSIZE) {
      (void)memcpy(&(st_nmea.uc_nmea_data[us_offset]), uc_nmea_data, ul_strlen);  /* Data storage */
      st_gps_nmea_info.st_nmea_sentence_info[POS_SNS_GPS_NMEA_SNO_GLL].uc_size = (u_int8)ul_strlen;
      st_gps_nmea_info.st_nmea_sentence_info[POS_SNS_GPS_NMEA_SNO_GLL].us_offset = us_offset;
      us_offset += (u_int16)ul_strlen;
    }
  }

  /* Get received NMEA data from storage area(GSA) */
  b_get = DevGpsCycleDataGetNmea(uc_nmea_data, sizeof(uc_nmea_data) , GPS_NMEA_INDEX_GSA);
  if (b_get == TRUE) {
    /* Data present */
    st_gps_nmea_info.ul_rcvsts |= POS_SNS_GPS_NMEA_GSA1;       /* Receive flag */
    ul_strlen = strlen(reinterpret_cast<const char *>(uc_nmea_data)); /* QAC 310 */
    if (ul_strlen > GPS_NMEA_MAX_SZ) {
      ul_strlen = GPS_NMEA_MAX_SZ;
    }

    if ((us_offset + ul_strlen) < SENSOR_MSG_VSINFO_DSIZE) {
      (void)memcpy(&(st_nmea.uc_nmea_data[us_offset]), uc_nmea_data, ul_strlen);  /* Data storage */
      st_gps_nmea_info.st_nmea_sentence_info[POS_SNS_GPS_NMEA_SNO_GSA1].uc_size = (u_int8)ul_strlen;
      st_gps_nmea_info.st_nmea_sentence_info[POS_SNS_GPS_NMEA_SNO_GSA1].us_offset = us_offset;
      us_offset += (u_int16)ul_strlen;
    }
  }

  /* Get received NMEA data from storage area(GST) */
  b_get = DevGpsCycleDataGetNmea(uc_nmea_data, sizeof(uc_nmea_data) , GPS_NMEA_INDEX_GST);
  if (b_get == TRUE) {
    /* Data present */
    st_gps_nmea_info.ul_rcvsts |= POS_SNS_GPS_NMEA_GST;       /* Receive flag */
    ul_strlen = strlen(reinterpret_cast<const char *>(uc_nmea_data)); /* QAC 310 */
    if (ul_strlen > GPS_NMEA_MAX_SZ) {
      ul_strlen = GPS_NMEA_MAX_SZ;
    }

    if ((us_offset + ul_strlen) < SENSOR_MSG_VSINFO_DSIZE) {
      (void)memcpy(&(st_nmea.uc_nmea_data[us_offset]), uc_nmea_data, ul_strlen);  /* Data storage */
      st_gps_nmea_info.st_nmea_sentence_info[POS_SNS_GPS_NMEA_SNO_GST].uc_size = (u_int8)ul_strlen;
      st_gps_nmea_info.st_nmea_sentence_info[POS_SNS_GPS_NMEA_SNO_GST].us_offset = us_offset;
      us_offset += (u_int16)ul_strlen;
    }
  }

  /* Get received NMEA data from storage area(RMC) */
  b_get = DevGpsCycleDataGetNmea(uc_nmea_data, sizeof(uc_nmea_data) , GPS_NMEA_INDEX_RMC);
  if (b_get == TRUE) {
    /* Data present */
    st_gps_nmea_info.ul_rcvsts |= POS_SNS_GPS_NMEA_RMC;       /* Receive flag */
    ul_strlen = strlen(reinterpret_cast<const char *>(uc_nmea_data)); /* QAC 310 */
    if (ul_strlen > GPS_NMEA_MAX_SZ) {
      ul_strlen = GPS_NMEA_MAX_SZ;
    }

    if ((us_offset + ul_strlen) < SENSOR_MSG_VSINFO_DSIZE) {
      (void)memcpy(&(st_nmea.uc_nmea_data[us_offset]), uc_nmea_data, ul_strlen);  /* Data storage */
      st_gps_nmea_info.st_nmea_sentence_info[POS_SNS_GPS_NMEA_SNO_RMC].uc_size = (u_int8)ul_strlen;
      st_gps_nmea_info.st_nmea_sentence_info[POS_SNS_GPS_NMEA_SNO_RMC].us_offset = us_offset;
      us_offset += (u_int16)ul_strlen;
    }
  }

  /* Get received NMEA data from storage area(VTG) */
  b_get = DevGpsCycleDataGetNmea(uc_nmea_data, sizeof(uc_nmea_data) , GPS_NMEA_INDEX_VTG);
  if (b_get == TRUE) {
    /* Data present */
    st_gps_nmea_info.ul_rcvsts |= POS_SNS_GPS_NMEA_VTG;       /* Receive flag */
    ul_strlen = strlen(reinterpret_cast<const char *>(uc_nmea_data)); /* QAC 310 */
    if (ul_strlen > GPS_NMEA_MAX_SZ) {
      ul_strlen = GPS_NMEA_MAX_SZ;
    }

    if ((us_offset + ul_strlen) < SENSOR_MSG_VSINFO_DSIZE) {
      (void)memcpy(&(st_nmea.uc_nmea_data[us_offset]), uc_nmea_data, ul_strlen);  /* Data storage */
      st_gps_nmea_info.st_nmea_sentence_info[POS_SNS_GPS_NMEA_SNO_VTG].uc_size = (u_int8)ul_strlen;
      st_gps_nmea_info.st_nmea_sentence_info[POS_SNS_GPS_NMEA_SNO_VTG].us_offset = us_offset;
      us_offset += (u_int16)ul_strlen;
    }
  }

  /* Get received NMEA data from storage area(GSV1) */
  b_get = DevGpsCycleDataGetNmea(uc_nmea_data, sizeof(uc_nmea_data) , GPS_NMEA_INDEX_GSV1);
  if (b_get == TRUE) {
    /* Data present */
    st_gps_nmea_info.ul_rcvsts |= POS_SNS_GPS_NMEA_GSV1;       /* Receive flag */
    ul_strlen = strlen(reinterpret_cast<const char *>(uc_nmea_data)); /* QAC 310 */
    if (ul_strlen > GPS_NMEA_MAX_SZ) {
      ul_strlen = GPS_NMEA_MAX_SZ;
    }

    if ((us_offset + ul_strlen) < SENSOR_MSG_VSINFO_DSIZE) {
      (void)memcpy(&(st_nmea.uc_nmea_data[us_offset]), uc_nmea_data, ul_strlen);  /* Data storage */
      st_gps_nmea_info.st_nmea_sentence_info[POS_SNS_GPS_NMEA_SNO_GSV1].uc_size = (u_int8)ul_strlen;
      st_gps_nmea_info.st_nmea_sentence_info[POS_SNS_GPS_NMEA_SNO_GSV1].us_offset = us_offset;
      us_offset += (u_int16)ul_strlen;
    }
  }

  /* Get received NMEA data from storage area(GSV2) */
  b_get = DevGpsCycleDataGetNmea( uc_nmea_data, sizeof(uc_nmea_data), GPS_NMEA_INDEX_GSV2 );
  if (b_get == TRUE) {
    /* Data present */
    st_gps_nmea_info.ul_rcvsts |= POS_SNS_GPS_NMEA_GSV2;       /* Receive flag */
    ul_strlen = strlen(reinterpret_cast<const char *>(uc_nmea_data)); /* QAC 310 */
    if (ul_strlen > GPS_NMEA_MAX_SZ) {
      ul_strlen = GPS_NMEA_MAX_SZ;
    }

    if ((us_offset + ul_strlen) < SENSOR_MSG_VSINFO_DSIZE) {
      (void)memcpy(&(st_nmea.uc_nmea_data[us_offset]), uc_nmea_data, ul_strlen);  /* Data storage */
      st_gps_nmea_info.st_nmea_sentence_info[POS_SNS_GPS_NMEA_SNO_GSV2].uc_size = (u_int8)ul_strlen;
      st_gps_nmea_info.st_nmea_sentence_info[POS_SNS_GPS_NMEA_SNO_GSV2].us_offset = us_offset;
      us_offset += (u_int16)ul_strlen;
    }
  }

  /* Get received NMEA data from storage area(GSV3) */
  b_get = DevGpsCycleDataGetNmea( uc_nmea_data, sizeof(uc_nmea_data) , GPS_NMEA_INDEX_GSV3 );
  if (b_get == TRUE) {
    /* Data present */
    st_gps_nmea_info.ul_rcvsts |= POS_SNS_GPS_NMEA_GSV3;       /* Receive flag */
    ul_strlen = strlen(reinterpret_cast<const char *>(uc_nmea_data)); /* QAC 310 */
    if (ul_strlen > GPS_NMEA_MAX_SZ) {
      ul_strlen = GPS_NMEA_MAX_SZ;
    }

    if ((us_offset + ul_strlen) < SENSOR_MSG_VSINFO_DSIZE) {
      (void)memcpy(&(st_nmea.uc_nmea_data[us_offset]), uc_nmea_data, ul_strlen);
      st_gps_nmea_info.st_nmea_sentence_info[POS_SNS_GPS_NMEA_SNO_GSV3].uc_size = static_cast<u_int8>(ul_strlen);
      st_gps_nmea_info.st_nmea_sentence_info[POS_SNS_GPS_NMEA_SNO_GSV3].us_offset = us_offset;
      us_offset += (u_int16)ul_strlen;
    }
  }

  /* Get received NMEA data from storage area(GSV4) */
  b_get = DevGpsCycleDataGetNmea(uc_nmea_data, sizeof(uc_nmea_data), GPS_NMEA_INDEX_GSV4);
  if (b_get == TRUE) {
    /* Data present */
    st_gps_nmea_info.ul_rcvsts |= POS_SNS_GPS_NMEA_GSV4;       /* Receive flag */
    ul_strlen = strlen(reinterpret_cast<const char *>(uc_nmea_data)); /* QAC 310 */
    if (ul_strlen > GPS_NMEA_MAX_SZ) {
      ul_strlen = GPS_NMEA_MAX_SZ;
    }

    if ((us_offset + ul_strlen) < SENSOR_MSG_VSINFO_DSIZE) {
      (void)memcpy(&(st_nmea.uc_nmea_data[us_offset]), uc_nmea_data, ul_strlen);
      st_gps_nmea_info.st_nmea_sentence_info[POS_SNS_GPS_NMEA_SNO_GSV4].uc_size = static_cast<u_int8>(ul_strlen);
      st_gps_nmea_info.st_nmea_sentence_info[POS_SNS_GPS_NMEA_SNO_GSV4].us_offset = us_offset;
      us_offset += (u_int16)ul_strlen;
    }
  }

  /* Get received NMEA data from storage area(GSV5) */
  b_get = DevGpsCycleDataGetNmea(uc_nmea_data, sizeof(uc_nmea_data), GPS_NMEA_INDEX_GSV5);
  if (b_get == TRUE) {
    /* Data present */
    st_gps_nmea_info.ul_rcvsts |= POS_SNS_GPS_NMEA_GSV5;       /* Receive flag */
    ul_strlen = strlen(reinterpret_cast<const char *>(uc_nmea_data)); /* QAC 310 */
    if (ul_strlen > GPS_NMEA_MAX_SZ) {
      ul_strlen = GPS_NMEA_MAX_SZ;
    }

    if ((us_offset + ul_strlen) < SENSOR_MSG_VSINFO_DSIZE) {
      (void)memcpy(&(st_nmea.uc_nmea_data[us_offset]), uc_nmea_data, ul_strlen);
      st_gps_nmea_info.st_nmea_sentence_info[POS_SNS_GPS_NMEA_SNO_GSV5].uc_size = static_cast<u_int8>(ul_strlen);
      st_gps_nmea_info.st_nmea_sentence_info[POS_SNS_GPS_NMEA_SNO_GSV5].us_offset = us_offset;
      us_offset += (u_int16)ul_strlen;
    }
  }

  if (0 != st_gps_nmea_info.ul_rcvsts) {
    /* Receive flag */
    (void)memcpy(&(st_nmea.uc_nmea_data[0]), &st_gps_nmea_info, sizeof(st_gps_nmea_info));

    /* Provided to vehicle sensor */
    l_ret = SendNmeaGps(&st_nmea);

    if (RET_NORMAL != l_ret) {
      POSITIONING_LOG("SendNmeaGps SndMsg Error\n");
    }
  } else {
    /* Do not provide to vehicle sensor when data acquisition fails or no data */
  }

  return;
}

/**
 * @brief
 *  Analysis of the received command
 */
void DevGpsRcvCyclCmd(void) {
  int32                 i_ret = 0;
  TG_GPS_OUTPUT_FORMAT  e_format = GPS_FORMAT_MIN;

  TG_GPS_RCV_DATA      st_rcv_data; /* Pointer to the received message */
  (void)memcpy(&st_rcv_data, &(g_gps_msg_rcvr.msgdat[0]), sizeof(st_rcv_data) ); /* QAC 3200 */

  /* Analysis of received commands */
  i_ret = JudgeFormatGpsCommon(reinterpret_cast<u_char*>(&(st_rcv_data.bygps_data[0])),
                               static_cast<u_int32>(st_rcv_data.bydata_len),
                               &e_format);

  if (i_ret == GPSRET_SNDCMD) {
    /* For NMEA formats */
    if ((e_format == GPS_FORMAT_RMC)  ||
        (e_format == GPS_FORMAT_VTG)  ||
        (e_format == GPS_FORMAT_GGA)  ||
        (e_format == GPS_FORMAT_GSA)  ||
        (e_format == GPS_FORMAT_GSV1) ||
        (e_format == GPS_FORMAT_GSV2) ||
        (e_format == GPS_FORMAT_GSV3) ||
        (e_format == GPS_FORMAT_GSV4) ||
        (e_format == GPS_FORMAT_GSV5) ||
        (e_format == GPS_FORMAT_GLL)  ||
        (e_format == GPS_FORMAT_GST)) {
      /* NMEA reception process */
      RcvCyclCmdNmeaGpsCommon(&(st_rcv_data.bygps_data[0]), (u_int32)st_rcv_data.bydata_len, e_format);
    } else if ((e_format == GPS_FORMAT_MON_VER)     ||
               (e_format == GPS_FORMAT_AID_INI)     ||
               (e_format == GPS_FORMAT_ACK_ACKNACK) ||
               (e_format == GPS_FORMAT_NAV_TIMEUTC) ||
               (e_format == GPS_FORMAT_NAV_CLOCK)   ||
               (e_format == GPS_FORMAT_RXM_RTC5)    ||
               (e_format == GPS_FORMAT_NAV_SVINFO)) {
        /* UBX reception process */
      RcvCyclCmdExtGpsCommon(&(st_rcv_data.bygps_data[0]), (u_int32)st_rcv_data.bydata_len, e_format);
    } else {
      POSITIONING_LOG("Forbidden ERROR!![e_format=%d]", (int)e_format);
    }
  } else if (i_ret == GPSRET_CMDERR) {
    /* Receive command error */

    /* Discard previously received data */
    DevGpsCycleDataClear();
    /* Initialize receive format */
    g_rcv_format = GPS_FORMAT_MIN;
  } else {
  }

  return;
}

/**
 * @brief
 *  Check of the received command
 */
void DevGpsCmdEventCheckNmea(void) {
  u_int32         ul_cnt = 0;
  TG_GPS_RCV_DATA st_rcv_data;
  u_char*         pub_rcv_data = NULL;
  BOOL            brk_flg = FALSE;

  memset(&st_rcv_data, 0, sizeof(TG_GPS_RCV_DATA));
  memcpy( &st_rcv_data, &(g_gps_msg_rcvr.msgdat[0]), sizeof(TG_GPS_RCV_DATA) );
  pub_rcv_data = reinterpret_cast<u_char*>(&(st_rcv_data.bygps_data[0]));

  /* Analysis of received commands */
  for (ul_cnt = 0; ul_cnt < (u_int32)GPSCMDANATBL_MAX; ul_cnt++) {
    /* End-of-table decision */
    if (CheckFrontStringPartGpsCommon(reinterpret_cast<const u_char*>(ENDMARK),
        reinterpret_cast<const u_char*>(kGpsCmdAnaTbl[ul_cnt].c_sentence)) == RET_NORMAL ) {
      g_wrecv_err++;

      /* Data error is set to Event ID. */
      g_gps_mngr.event = (u_int32)NG;

      brk_flg = TRUE;
    } else if (CheckFrontStringPartGpsCommon(pub_rcv_data, kGpsCmdAnaTbl[ul_cnt].c_sentence) == RET_NORMAL) {
      /* Reception type determination */

      /* Using $GPRMC in responses to resets */
      if ((g_gps_mngr.sts == GPS_STS_SENT) &&
          (g_gps_mngr.resp_cmd == GPS_FORMAT_RMC) &&
          (kGpsCmdAnaTbl[ul_cnt].e_rcv_format == GPS_FORMAT_RMC)) {
        POSITIONING_LOG("Received response ($GPRMC) form GPS device.\n");

        /** Response command */
        g_gps_mngr.event = GPS_EVT_RECVRSPDAT;

        /** Receive format setting */
        g_gps_mngr.rcv_cmd = kGpsCmdAnaTbl[ul_cnt].e_rcv_format;
      } else if (kGpsCmdAnaTbl[ul_cnt].ul_rcv_kind == RCV_CYCLE) {
        /* Cyclic receive command */
        g_gps_mngr.event = GPS_EVT_RECVCYCLDAT;

        /* Receive format setting */
        g_gps_mngr.rcv_cmd = kGpsCmdAnaTbl[ul_cnt].e_rcv_format;
      } else if (kGpsCmdAnaTbl[ul_cnt].ul_rcv_kind == RCV_RESP) {
        /** Response command */
        g_gps_mngr.event = GPS_EVT_RECVRSPDAT;

        /** Receive format setting */
        g_gps_mngr.rcv_cmd = kGpsCmdAnaTbl[ul_cnt].e_rcv_format;
      } else {
        /* Undefined value */
        /* Data error is set to Event ID. */
        g_gps_mngr.event = (u_int32)NG;
      }

      brk_flg = TRUE;
    }

    if (brk_flg == TRUE) {
      break;
    }
  }

  return;
}

/**
 * @brief
 *   Get GPS reception status
 *
 *   By analyzing the last received GSA-sentence and using the satellite-number as the Satellite number
 *   Determines the reception status based on whether or not notification has been made, and returns it. 
 *
 * @param[in]  no_sv Satellite number
 *
 * @return  NAVIINFO_DIAG_GPS_RCV_STS_NOTUSEFIX  - Positioning not used
 *      NAVIIFNO_DIAG_GPS_RCV_STS_USEFIX  - Positioning use
 */
u_int8 DevGpsGetGpsRcvSts(u_int8 sv) {
  u_int8  rcv_sts = NAVIINFO_DIAG_GPS_RCV_STS_TRACHING; /* Tracking in progress */
  u_int8  uc_nmea_data[GPS_NMEA_MAX_SZ];
  u_int8  uc_no = 0;
  BOOL    b_get = FALSE;
  int32   i = 0;

  /* Get received NMEA data from storage area(GSA) */
  b_get = DevGpsCycleDataGetNmea(uc_nmea_data, sizeof(uc_nmea_data), GPS_NMEA_INDEX_GSA);

  if (b_get == TRUE) {
    for (i = 0; i < GPS_NMEA_NUM_GSA_SV; i++) {
      /* Get satellite number */
      uc_no = (uint8_t)GetIntegerFromNmeaGpsCommon(GPS_NMEA_FNO_GSA_SV + (u_int8)(1 * i), uc_nmea_data);

      if (uc_no == sv) {
        rcv_sts = NAVIINFO_DIAG_GPS_RCV_STS_USEFIX; /* Positioning use */
        break;
      }
    }
  }

  return rcv_sts;
}

/**
 * @brief
 *   GPS information analysis
 *
 *   Analyzing received NMEA sentences 
 * @param[out]  navilocinfo Navigation information
 */
void DevGpsAnalyzeNmea(NAVIINFO_ALL* navilocinfo) {
  u_int8  uc_nmea_data[GPS_NMEA_MAX_SZ];
  int32   no_sv = 0; /* number of Satellites in View */
  int32   __offset = 0;
  char    utc_time[12]; /* hhmmss.sss */
  char    _date[6]; /* ddmmyy */
  char    _status = 0; /* 'V' or 'A' */
  char    indicator; /* 'N' or 'S' or 'E' or 'W' */
  char    work[8]; /* Work buffer for converting String data */
  BOOL    b_get = FALSE;
  uint8_t fixsts = NAVIINFO_DIAG_GPS_FIX_STS_NON;
  uint8_t fixsts_gga;
  BOOL    bvalid_lon = FALSE;
  BOOL    bvalid_lat = FALSE;
  BOOL    bvalid = FALSE;
  TG_TIM_ROLOVR_YMD   base_ymd;
  TG_TIM_ROLOVR_YMD   conv_ymd;
  BOOL roll_over_sts;
  u_int8  _tdsts = g_gpstime_raw_tdsts;

  GpsSatelliteInfo st_visible_satellite_list[GPS_MAX_NUM_VISIBLE_SATELLITES]; /* Visible satellite list */
  GpsSatelliteInfo st_tmp_buf;

  int32 i = 0;
  int32 j = 0;
  int32 k = 0;

  SENSORLOCATION_LONLATINFO_DAT     st_lonlat;
  SENSORLOCATION_ALTITUDEINFO_DAT   st_altitude;
  SENSORMOTION_HEADINGINFO_DAT      st_heading;
  // MDEV_GPS_RTC                      st_rtc;
  SENSOR_MSG_GPSTIME                st_gps_time;
  SENSORMOTION_SPEEDINFO_DAT        st_speed;

  memset(&st_lonlat, 0x00, sizeof(SENSORLOCATION_LONLATINFO_DAT));
  memset(&st_altitude, 0x00, sizeof(SENSORLOCATION_ALTITUDEINFO_DAT));
  memset(&st_heading, 0x00, sizeof(SENSORMOTION_HEADINGINFO_DAT));
  // memset(&st_rtc, 0x00, sizeof(MDEV_GPS_RTC));
  memset(&st_gps_time, 0x00, sizeof(SENSOR_MSG_GPSTIME));
  memset(&st_speed, 0x00, sizeof(SENSORMOTION_SPEEDINFO_DAT));

  /* Satellite signal strength list initialization */
  (void)memset(st_visible_satellite_list, 0x00, sizeof(GpsSatelliteInfo) * GPS_MAX_NUM_VISIBLE_SATELLITES);

  /* Get received NMEA data from storage area(GSA) */
  b_get = DevGpsCycleDataGetNmea(uc_nmea_data, sizeof(uc_nmea_data), GPS_NMEA_INDEX_GSA);

  if (b_get == TRUE) {
    fixsts = static_cast<uint8_t>(GetIntegerFromNmeaGpsCommon(GPS_NMEA_FNO_GSA_FS, uc_nmea_data));
  }

  /* Get received NMEA data from storage area(RMC) */
  b_get = DevGpsCycleDataGetNmea(uc_nmea_data, sizeof(uc_nmea_data) , GPS_NMEA_INDEX_RMC);

  if (b_get == TRUE) {
    navilocinfo->stDiagGps.stFix.stWgs84.lLat =
          GetLonLatFromNmeaGpsCommon(GPS_NMEA_FNO_RMC_LATITUDE, uc_nmea_data, &bvalid_lat);  /* GPS location information and latitude  */

    GetStringFromNmeaGpsCommon(GPS_NMEA_FNO_RMC_NS, uc_nmea_data, &indicator, sizeof(indicator));

    if (indicator != GPS_NMEA_RMC_IND_NORTH) {
      navilocinfo->stDiagGps.stFix.stWgs84.lLat *= -1;
    }

    POSITIONING_LOG("lLat = %d", navilocinfo->stDiagGps.stFix.stWgs84.lLat);

    navilocinfo->stDiagGps.stFix.stWgs84.lLon =
          GetLonLatFromNmeaGpsCommon(GPS_NMEA_FNO_RMC_LONGITUDE, uc_nmea_data, &bvalid_lon);  /* GPS position information and longitude  */

    GetStringFromNmeaGpsCommon(GPS_NMEA_FNO_RMC_EW, uc_nmea_data, &indicator, sizeof(indicator));

    if (indicator != GPS_NMEA_RMC_IND_EAST) {
      navilocinfo->stDiagGps.stFix.stWgs84.lLon *= -1;
    }

    st_lonlat.Longitude = navilocinfo->stDiagGps.stFix.stWgs84.lLon;
    st_lonlat.Latitude  = navilocinfo->stDiagGps.stFix.stWgs84.lLat;

    POSITIONING_LOG("lLon = %d", navilocinfo->stDiagGps.stFix.stWgs84.lLon);

    /* Get Date information */
    (void)GetStringFromNmeaGpsCommon(GPS_NMEA_FNO_RMC_DATE, uc_nmea_data, _date, 6);
    (void)memset(&base_ymd,  0, sizeof(base_ymd)); /* QAC 3200 */
    (void)memset(&conv_ymd,  0, sizeof(conv_ymd)); /* QAC 3200 */
    (void)memset(work,  0, sizeof(work)); /* QAC 3200 */

    (void)strncpy(work, &_date[4], 2); /* QAC 3200 */
    base_ymd.year = (u_int16)(2000 + atoi(work));  /* YEAR          */

    st_gps_time.utc.year = (uint8_t)atoi(work);

    (void)strncpy(work, &_date[2], 2); /* QAC 3200 */
    base_ymd.month = (u_int16)(atoi(work));      /* MONTH          */

    st_gps_time.utc.month = (uint8_t)atoi(work);

    (void)strncpy(work, &_date[0], 2); /* QAC 3200 */
    base_ymd.day = (u_int16)(atoi(work));      /* DAY          */

    st_gps_time.utc.date = (uint8_t)atoi(work);

    POSITIONING_LOG("year = %d", base_ymd.year);
    POSITIONING_LOG("month = %d", base_ymd.month);
    POSITIONING_LOG("date = %d", base_ymd.day);

    /* UTC time information acquisition */
    (void)GetStringFromNmeaGpsCommon(GPS_NMEA_FNO_RMC_UTC, uc_nmea_data, utc_time, 12);

    (void)strncpy(work, &utc_time[0], 2); /* QAC 3200 */
    navilocinfo->stNaviGps.utc.hour = (uint8_t)atoi(work);      /* HOUR          */

    st_gps_time.utc.hour = navilocinfo->stNaviGps.utc.hour;
    POSITIONING_LOG("hour = %d", navilocinfo->stNaviGps.utc.hour);

    (void)strncpy(work, &utc_time[2], 2); /* QAC 3200 */
    navilocinfo->stNaviGps.utc.minute = (uint8_t)atoi(work);      /* MINUTE          */

    st_gps_time.utc.minute = navilocinfo->stNaviGps.utc.minute;
    POSITIONING_LOG("minute = %d", navilocinfo->stNaviGps.utc.minute);

    (void)strncpy(work, &utc_time[4], 2); /* QAC 3200 */
    navilocinfo->stNaviGps.utc.second = (uint8_t)atoi(work);      /* SECOND          */

    st_gps_time.utc.second = navilocinfo->stNaviGps.utc.second;
    POSITIONING_LOG("second = %d", navilocinfo->stNaviGps.utc.second);

    /* Compass information acquisition */
    navilocinfo->stNaviGps.heading =
        GetHeadingFromNmeaGpsCommon(GPS_NMEA_FNO_RMC_COG, uc_nmea_data, &bvalid);

    st_heading.Heading = navilocinfo->stNaviGps.heading;
    POSITIONING_LOG("heading = %u", navilocinfo->stNaviGps.heading);

    st_speed.Speed = GetSpeedFromNmeaGpsCommon(GPS_NMEA_FNO_RMC_SPEED, uc_nmea_data, &bvalid);

    /* Fix Status/Time Status Calculation */
    (void)GetStringFromNmeaGpsCommon(GPS_NMEA_FNO_RMC_STATUS, uc_nmea_data, &_status, sizeof(_status));

    if ((_status == GPS_NMEA_RMC_STS_VALID) && (bvalid_lat == TRUE) && (bvalid_lon == TRUE)) {
      /* Fix status information  */
      switch (fixsts) {
        case GPS_NMEA_GSA_FIX_STS_NON:
          navilocinfo->stDiagGps.stFix.ucFixSts = NAVIINFO_DIAG_GPS_FIX_STS_NON;
          break;
        case GPS_NMEA_GSA_FIX_STS_2D:
          navilocinfo->stDiagGps.stFix.ucFixSts = NAVIINFO_DIAG_GPS_FIX_STS_2D;
          break;
        case GPS_NMEA_GSA_FIX_STS_3D:
          navilocinfo->stDiagGps.stFix.ucFixSts = NAVIINFO_DIAG_GPS_FIX_STS_3D;
          break;
        default:
          POSITIONING_LOG("GSA Nav Mode Error [fixsts:%d]", fixsts);
          break;
      }

      if (_tdsts == POS_TIMESTS_OK) {
        roll_over_sts = DevCalcRollOverTime(&base_ymd, &conv_ymd);
        navilocinfo->stNaviGps.utc.year   = conv_ymd.year;        /* year (after conversion)          */
        navilocinfo->stNaviGps.utc.month  = (u_int8)(conv_ymd.month);  /* month (after conversion)          */
        navilocinfo->stNaviGps.utc.date   = (u_int8)(conv_ymd.day);  /* dat (after conversion)          */
        if (roll_over_sts == FALSE) {
          navilocinfo->stNaviGps.tdsts = POS_TIMESTS_NG;
          /* Reserve[0] is time setting information: anomaly time, but can be calculated by rolling over. */
          navilocinfo->stNaviGps.reserve[0] = GPS_TIME_ROLOVR;
        } else {
          /* When the location information is normal, the time information is also judged to be normal. */
          navilocinfo->stNaviGps.tdsts = POS_TIMESTS_OK; /* Time calibration completed */
          /* Reserve[0] is time setting information: Use time status received from GPS device. */
          navilocinfo->stNaviGps.reserve[0] = GPS_TIME_RX;
        }
      } else {
        navilocinfo->stNaviGps.tdsts      = POS_TIMESTS_NG;  /* Time uncalibrated */
        /* Reserve[0] is time setting information: Use time status received from GPS device. */
        navilocinfo->stNaviGps.reserve[0] = GPS_TIME_RX;
        navilocinfo->stNaviGps.utc.year   = base_ymd.year;        /* year(after conversion)          */
        navilocinfo->stNaviGps.utc.month  = (u_int8)(base_ymd.month);  /* month (after conversion)          */
        navilocinfo->stNaviGps.utc.date   = (u_int8)(base_ymd.day);  /* day (after conversion)          */
      }

      if (bvalid != TRUE) {
        /* Invalid value if measurement orientation is invalid. */
        navilocinfo->stNaviGps.heading    = GPS_HEADING_INVALID_VAL;
        // POSITIONING_LOG("RMC Heading[cog] Invalid");
      }
    } else {
      /* Fix status information: Non-position fix is set regardless of FS of GSA. */
      navilocinfo->stDiagGps.stFix.ucFixSts = NAVIINFO_DIAG_GPS_FIX_STS_NON;
      /* If the location information is invalid, the time information is also judged to be invalid. */
      /* Time not calibrated after receiver reset (time entry or master reset or CSF activation) */
      navilocinfo->stNaviGps.tdsts = POS_TIMESTS_NG;
      /* Reserve[0] is time setting information: Use time status received from GPS device. */
      navilocinfo->stNaviGps.reserve[0] = GPS_TIME_RX;
      navilocinfo->stNaviGps.utc.year = base_ymd.year;        /* year (after conversion)          */
      navilocinfo->stNaviGps.utc.month = (u_int8)(base_ymd.month);  /* month (after conversion)          */
      navilocinfo->stNaviGps.utc.date = (u_int8)(base_ymd.day);  /* day (after conversion)          */
      // POSITIONING_LOG("RMC Invalid[status:%d, bvalidLat:%d, bvalidLon:%d]", _status, bvalid_lat, bvalid_lon);
    }

    // POSITIONING_LOG("year(Fix) = %d", navilocinfo->stNaviGps.utc.year);
    // POSITIONING_LOG("month(Fix) = %d", navilocinfo->stNaviGps.utc.month);
    // POSITIONING_LOG("date(Fix) = %d", navilocinfo->stNaviGps.utc.date);
    // POSITIONING_LOG("tdsts = %d", navilocinfo->stNaviGps.tdsts);
  }

  /* Get received NMEA data from storage area(GGA) */
  b_get = DevGpsCycleDataGetNmea(uc_nmea_data, sizeof(uc_nmea_data), GPS_NMEA_INDEX_GGA);

  if (b_get == TRUE) {
    /* Data status acquisition */
    fixsts_gga = (uint8_t)GetIntegerFromNmeaGpsCommon(GPS_NMEA_FNO_GGA_FS, uc_nmea_data);

    /* Altitude information acquisition */
    if (((fixsts == GPS_NMEA_GSA_FIX_STS_2D) ||
            (fixsts == GPS_NMEA_GSA_FIX_STS_3D)) &&
            ((fixsts_gga == GPS_NMEA_GGA_FIX_STATUS_GPS) ||
             (fixsts_gga == GPS_NMEA_GGA_FIX_STATUS_DGPS) ||
             (fixsts_gga == GPS_NMEA_GGA_FIX_STATUS_DR))) {
      navilocinfo->stNaviGps.altitude =
          GetAltitudeFromNmeaGpsCommon(GPS_NMEA_FNO_GGA_MSL, uc_nmea_data, &bvalid);

      if (bvalid != TRUE) {
        /* If the location information is invalid, set an invalid value. */
        navilocinfo->stNaviGps.altitude = GPS_ALTITUDE_INVALID_VAL;
        // POSITIONING_LOG("GGA Altitude[msl] Invalid");
      }
    } else {
      /* If the location information is invalid, set an invalid value. */
      navilocinfo->stNaviGps.altitude = GPS_ALTITUDE_INVALID_VAL;
      // POSITIONING_LOG("GGA Invalid[fixsts:%d, fixstsGGA:%d]", fixsts, fixsts_gga);
    }

    st_altitude.Altitude = navilocinfo->stNaviGps.altitude;
    // POSITIONING_LOG("altitude = %d", navilocinfo->stNaviGps.altitude);
  }

  DevGpsCnvLonLatNavi(&st_lonlat, navilocinfo->stDiagGps.stFix.ucFixSts, navilocinfo->stDiagGps.stFix.stWgs84.lLon,
                                                                         navilocinfo->stDiagGps.stFix.stWgs84.lLat);

  DevGpsCnvAltitudeNavi(&st_altitude, navilocinfo->stDiagGps.stFix.ucFixSts, navilocinfo->stNaviGps.altitude);

  DevGpsCnvHeadingNavi(&st_heading, navilocinfo->stDiagGps.stFix.ucFixSts, navilocinfo->stNaviGps.heading);

  SendCustomGps(&st_gps_time, &st_lonlat, &st_altitude, &st_heading, &navilocinfo->stDiagGps);
  // For test todo don't needed
  // SendTimeGps(&st_rtc);
  // SendSpeedGps(&st_speed, 0);

  /* Create visual satellite information list from GSV1~GSV5 and GSV */
  for (i = 0; i < 5; i++) {
    /* Get received NMEA data from storage area */
    b_get = DevGpsCycleDataGetNmea( uc_nmea_data, sizeof(uc_nmea_data),
                                            (ENUM_GPS_NMEA_INDEX)(GPS_NMEA_INDEX_GSV1 + i));

    if (b_get == TRUE) {
      /* Get number of Satellites in View */
      no_sv = GetIntegerFromNmeaGpsCommon(GPS_NMEA_FNO_GSV_NOSV, uc_nmea_data);

      for (j = 0; j < GPS_NMEA_NUM_GSV_SINFO; j++) {
        if (__offset >= no_sv) {
          break;
        }

        st_visible_satellite_list[__offset].sv  = static_cast<uint8_t>(
          GetIntegerFromNmeaGpsCommon(GPS_NMEA_FNO_GSV_SV + (u_int8)(4 * j), uc_nmea_data));   /* Satellite information_No      */
        st_visible_satellite_list[__offset].elv = static_cast<uint8_t>(
          GetIntegerFromNmeaGpsCommon(GPS_NMEA_FNO_GSV_ELV + (u_int8)(4 * j), uc_nmea_data));  /* Satellite information_elevation    */
        st_visible_satellite_list[__offset].az  = static_cast<uint16_t>(
          GetIntegerFromNmeaGpsCommon(GPS_NMEA_FNO_GSV_AZ + (u_int8)(4 * j), uc_nmea_data));   /* Satellite Information_Azimuth    */
        st_visible_satellite_list[__offset].cno = static_cast<uint8_t>(
          GetIntegerFromNmeaGpsCommon(GPS_NMEA_FNO_GSV_CNO + (u_int8)(4 * j), uc_nmea_data));   /* Satellite information_level    */
        st_visible_satellite_list[__offset].sts = DevGpsGetGpsRcvSts(st_visible_satellite_list[__offset].sv);

        /* Sort in ascending order of status (priority to use fix) and received signal strength */
        for (k = __offset; k > 0; k--) {
          if (((st_visible_satellite_list[k].sts == NAVIINFO_DIAG_GPS_RCV_STS_USEFIX) &&
           (st_visible_satellite_list[k - 1].sts == NAVIINFO_DIAG_GPS_RCV_STS_TRACHING)) ||
          ((st_visible_satellite_list[k - 1].sts == st_visible_satellite_list[k].sts) &&
           (st_visible_satellite_list[k].cno > st_visible_satellite_list[k - 1].cno))) {
            (void)memcpy(&st_tmp_buf, &st_visible_satellite_list[k], sizeof(GpsSatelliteInfo));
            (void)memcpy(&st_visible_satellite_list[k], &st_visible_satellite_list[k - 1], sizeof(GpsSatelliteInfo));
            (void)memcpy(&st_visible_satellite_list[k - 1], &st_tmp_buf, sizeof(GpsSatelliteInfo));
          } else {
            break;
          }
        }

        __offset++;
      }
    }
  }

  return;
}

/****************************************************************************
@brief      DevGpsCycleDataClear<BR>
            Cyclic data storage area clear processing
@outline    Clear the cyclic data storage area
@param[in]  none
@param[out] none
@return     none
@retval     none
*******************************************************************************/
void DevGpsCycleDataClear(void) {
  int32 i = 0;

  /* Sensor counter, reception flag initialization */
  g_st_gpscycle_data.uc_sns_cnt = 0;

  for (i = 0; i < GPS_NMEA_INDEX_MAX; i++) {
    g_st_gpscycle_data.st_nmea_data.uc_rcv_flag[i] = GPS_CYCLECMD_NOTRCV;
  }

  g_st_gpscycle_data.st_binary_data.uc_rcv_flag  = GPS_CYCLECMD_NOTRCV;
  g_st_gpscycle_data.st_fullbin_data.uc_rcv_flag = GPS_CYCLECMD_NOTRCV;
}

/******************************************************************************
@brief      DEV_Gps_CycleData_SetNmea<BR>
            NMEA data setting process
@outline    Set NMEA data in cyclic data storage area
@param[in]  u_int8*            : p_data   ... NMEA data to be set
@param[in]  u_int32            : ul_length  ... Data length
@param[in]  ENUM_GPS_NMEA_INDEX: e_format   ... Sentence identification
@param[out] none
@return     none
@retval     none
*******************************************************************************/
void DevGpsCycleDataSetNmea(const u_int8* p_data, u_int32 ul_length, ENUM_GPS_NMEA_INDEX e_format) {
  u_int32 ul_copy_sz = 0;

  /** Anomaly detection */
  if (e_format >= GPS_NMEA_INDEX_MAX) {
    POSITIONING_LOG("# GpsCommCtl_API # Set NMEA Sentence ERROR ! \r\n");
  } else {
    /** Storage size determination */
    if (GPS_NMEA_MAX_SZ < ul_length) {
      ul_copy_sz = GPS_NMEA_MAX_SZ;
      POSITIONING_LOG("# GpsCommCtl_API # Set NMEA Cmd Size ERROR ! \r\n");
    } else {
      ul_copy_sz = ul_length;
    }

    /** Storing */
    g_st_gpscycle_data.st_nmea_data.uc_rcv_flag[e_format] = GPS_CYCLECMD_RCV;
    memset(&(g_st_gpscycle_data.st_nmea_data.st_nmea[e_format].uc_data[0]), 0x00, GPS_NMEA_MAX_SZ);
    memcpy(&(g_st_gpscycle_data.st_nmea_data.st_nmea[e_format].uc_data[0]), p_data, ul_copy_sz);
  }

  return;
}

/******************************************************************************
@brief      DevGpsCycleDataGetNmea<BR>
            NMEA data setting process
@outline    Set NMEA data in cyclic data storage area
@param[in]  u_int32            : ul_buf_size ... Storage destination buffer size
@param[in]  ENUM_GPS_NMEA_INDEX: e_format   ... Sentence identification
@param[out] u_int8*            : p_data   ... Storage destination buffer pointer
@return     BOOL
@retval     TRUE               : Data present
@retval     FALSE              : No data
*******************************************************************************/
BOOL DevGpsCycleDataGetNmea(u_int8 *p_data, u_int32 ul_buf_size, ENUM_GPS_NMEA_INDEX e_format) {
  BOOL ret = TRUE;

  /** Determining whether data exists in the cyclic data area */
  if (GPS_CYCLECMD_RCV == g_st_gpscycle_data.st_nmea_data.uc_rcv_flag[e_format]) {
    if (GPS_NMEA_MAX_SZ <= ul_buf_size) {
      /** Copy to storage destination buffer */
      memcpy(p_data, &(g_st_gpscycle_data.st_nmea_data.st_nmea[e_format].uc_data[0]), GPS_NMEA_MAX_SZ);
    } else {
      /** Storage destination buffer size is small */
      ret = FALSE;
    }
  } else {
    /** Not received */
    ret = FALSE;
  }

  return ret;
}

/**
 * @brief
 *  Setting of the check sum
 *
 * @param[in]  buffer   Pointer of data
 * @param[in]  length   length of data
 */
void DevGpsSetChkSum(u_int8* buffer, u_int32 length) {
  u_int16 i = 0;
  u_int8  ck_a = 0;
  u_int8  ck_b = 0;

  if (buffer != NULL) {
    for (i = 2; i < (length - 2); i++) {
      ck_a = ck_a + buffer[i];
      ck_b = ck_b + ck_a;
    }

    /* Checksum_Set */
    buffer[length - 2] = ck_a;
    buffer[length - 1] = ck_b;
  } else {
  }
}

/*---------------------------------------------------------------------------*/
/*EOF*/
