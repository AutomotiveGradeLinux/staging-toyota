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
* @file MDev_GpsUblox.h
*/

#ifndef INC_GPSCOMMON_MDEV_GPS_NMEA_H_
#define INC_GPSCOMMON_MDEV_GPS_NMEA_H_

/*---------------------------------------------------------------------------*/

#include "gps_hal.h"
#include "positioning_def.h"

/*---------------------------------------------------------------------------*/
// Value define

#define TEST_NMEA                       (0)                 /* 0:Normal mode 1:Testing mode */

#define GPS_MAX_NUM_VISIBLE_SATELLITES  (20)                /* Maximum Number of Visible Satellites  *Assume the GSV1~GSV5 */

/* UBX command *****************************/

/* CFG */
#define UBX_CMD_SIZE_CFG_RST            (12)  /* size of CFG-RST of UBX sentences       */
#define UBX_CMD_SIZE_CFG_MSG            (11)  /* size of CFG-MSG of UBX sentences       */
#define UBX_CMD_SIZE_CFG_NAV5           (44)  /* size of CFG-NAV5 of UBX sentences      */
#define UBX_CMD_SIZE_CFG_NAV5_POLL      (8)
#define UBX_CMD_SIZE_CFG_NAV5_PLUS_POLL (UBX_CMD_SIZE_CFG_NAV5 + UBX_CMD_SIZE_CFG_NAV5_POLL)
#define UBX_CMD_SIZE_CFG_NAVX5_POLL     (8)
#define UBX_CMD_OFFSET_WKNROLLOVER      (18)  /* offset from the top of 'payload'       */

/* MON */
#define UBX_CMD_SIZE_MON_VER            (108) /* size of MON-VER of UBX sentences       */
#define UBX_CMD_SIZE_MON_VER_POLL       (8)
#define UBX_CMD_SIZE_MON_VER_SW_VER     (30)

/* AID */
#define UBX_CMD_SIZE_AID_INI            (56)  /* size of AID-INI of UBX sentences       */
#define UBX_CMD_SIZE_AID_INI_POLL       (8)
#define UBX_CMD_SIZE_AID_INI_PLUS_POLL  (UBX_CMD_SIZE_AID_INI + UBX_CMD_SIZE_AID_INI_POLL)

/* ACK */
#define UBX_CMD_OFFSET_ACKNAK           (3)   /* offset to Ack/Nack Infomation          */

/* NAV */
#define UBX_CMD_MSK_NAV_TIMEUTC_VALID_TOW   (1)     /* mask bit for 'validTow' of NAV-TIMEUTC */
#define UBX_CMD_MSK_NAV_TIMEUTC_VALID_WKN   (2)     /* mask bit for 'validKwn' of NAV-TIMEUTC */
#define UBX_CMD_MSK_NAV_TIMEUTC_VALID_UTC   (4)     /* mask bit for 'validUtc' of NAV-TIMEUTC */

/* Common */
#define UBX_CMD_SIZE_HDR                    (2)     /* size of 'header' of UBX sentence       */
#define UBX_CMD_SIZE_ID                     (2)     /* size of 'id' of UBX sentence           */
#define UBX_CMD_SIZE_LEN                    (2)     /* size of 'length' of UBX sentence       */
#define UBX_CMD_SIZE_MAX                    (400)   /* size of NAV-SVINFO (numCh=32)          */
#define UBX_CMD_OFFSET_PAYLOAD              (UBX_CMD_SIZE_HDR + UBX_CMD_SIZE_ID + UBX_CMD_SIZE_LEN)
                                                    /* offset to 'payload' of UBX sentence    */

/* Types of UBX Poll Request Commands ************/
#define UBX_POLL_CMD_KIND_AID_INI           (1)
#define UBX_POLL_CMD_KIND_CFG_NAV5          (2)

// GPS-command (_CWORD82_ NMEA) related definitions      #GPF_60_024
/* Command identification character string */
#define  GPS_CMD_NMEA_GGA               ("$GPGGA")        /* GGA/double precision GGAs  */
#define  GPS_CMD_NMEA_VTG               ("$GPVTG")        /* VTG        */
#define  GPS_CMD_NMEA_RMC               ("$GPRMC")        /* RMC/double precision RMCs  */
#define  GPS_CMD_NMEA_GLL               ("$GPGLL")        /* GLL/double precision GLL  */
#define  GPS_CMD_NMEA_GSA               ("$GPGSA")        /* GSA        */
#define  GPS_CMD_NMEA_GSV_1             ("$GPGSV,*,1")    /* GSV(1)      */
#define  GPS_CMD_NMEA_GSV_2             ("$GPGSV,*,2")    /* GSV(2)      */
#define  GPS_CMD_NMEA_GSV_3             ("$GPGSV,*,3")    /* GSV(3)      */
#define  GPS_CMD_NMEA__CWORD44__GP_3        ("$_CWORD44_,GP,3")   /* _CWORD44_,GP,3    */
#define  GPS_CMD_NMEA__CWORD44__GP_4        ("$_CWORD44_,GP,4")   /* _CWORD44_,GP,4    */
#define  GPS_CMD_NMEA_P_CWORD82_F_GP_0        ("$P_CWORD82_F,GP,0")   /* P_CWORD82_F,GP,0    */
#define  GPS_CMD_NMEA_P_CWORD82_J_GP_1        ("$P_CWORD82_J,GP,1")   /* P_CWORD82_J,GP,1    */
#define  GPS_CMD_NMEA_P_CWORD82_I_GP          ("$P_CWORD82_I,GP")     /* P_CWORD82_I,GP      */
#define  GPS_CMD_NMEA_P_CWORD82_E_GP_0        ("$P_CWORD82_E,GP,0")   /* P_CWORD82_E,GP,0    */
#define  GPS_CMD_NMEA_P_CWORD82_J_GP_0        ("$P_CWORD82_J,GP,0")   /* P_CWORD82_J,GP,0    */
#define  GPS_CMD_NMEA_P_CWORD82_E_GP_2        ("$P_CWORD82_E,GP,2")   /* P_CWORD82_E,GP,2    */
#define  GPS_CMD_NMEA_P_CWORD82_G_GP_0        ("$P_CWORD82_G,GP,0")   /* P_CWORD82_G,GP,0    */
#define  GPS_CMD_NMEA_P_CWORD82_J_GP_7        ("$P_CWORD82_J,GP,7")   /* P_CWORD82_J,GP,7    */
#define  GPS_CMD_NMEA_P_CWORD82_J_GP_8        ("$P_CWORD82_J,GP,8")   /* P_CWORD82_J,GP,8    */
#define  GPS_CMD_NMEA_GST               ("$GPGST")        /* GST        */
#define  GPS_CMD_NMEA_GSV_4             ("$GPGSV,*,4")    /* GSV(4)      */
#define  GPS_CMD_NMEA_GSV_5             ("$GPGSV,*,5")    /* GSV(5)      */

/*-------------------------------------------------------------------------------
 * GPS-command (_CWORD82_ Binary) related definitions  #GPF_60_024
 -------------------------------------------------------------------------------*/
#define  GPS_CMD_BINARY                 (0xC6)        /* Standard binary    */
#define  GPS_CMD_FULLBIN                (0xB0)        /* FULL binaries    */

#define ENDMARK                         "ENDENDEND"   /* Table termination        */
#define GPSCMDANATBL_MAX                25            /* Maximum number of elements in table    */
#define RCV_CYCLE                       0x01          /* Reception type cycle reception command*/
#define RCV_RESP                        0x02          /* Reception type response command  */

// Length of FULLBINARY commands
#define GPS_CMD_FULLBIN1_SZ             30
#define GPS_CMD_FULLBIN2_SZ             160
#define GPS_CMD_FULLBIN3_SZ             25
#define GPS_CMD_FULLBIN4_SZ             26
#define GPS_CMD_FULLBIN5_SZ             14
#define GPS_CMD_FULLBIN6_SZ             61

/* Start offset of FULLBINARY command */
#define GPS_CMD_FULLBIN1_OFFSET         0
#define GPS_CMD_FULLBIN2_OFFSET         (GPS_CMD_FULLBIN1_OFFSET + GPS_CMD_FULLBIN1_SZ)
#define GPS_CMD_FULLBIN3_OFFSET         (GPS_CMD_FULLBIN2_OFFSET + GPS_CMD_FULLBIN2_SZ)
#define GPS_CMD_FULLBIN4_OFFSET         (GPS_CMD_FULLBIN3_OFFSET + GPS_CMD_FULLBIN3_SZ)
#define GPS_CMD_FULLBIN5_OFFSET         (GPS_CMD_FULLBIN4_OFFSET + GPS_CMD_FULLBIN4_SZ)
#define GPS_CMD_FULLBIN6_OFFSET         (GPS_CMD_FULLBIN5_OFFSET + GPS_CMD_FULLBIN6_SZ)


/* SUM value offset of FULLBINARY commands */
#define GPS_CMD_FULLBIN1_SUMOFFSET      (GPS_CMD_FULLBIN1_OFFSET + GPS_CMD_FULLBIN1_SZ - 2)
#define GPS_CMD_FULLBIN2_SUMOFFSET      (GPS_CMD_FULLBIN2_OFFSET + GPS_CMD_FULLBIN2_SZ - 2)
#define GPS_CMD_FULLBIN3_SUMOFFSET      (GPS_CMD_FULLBIN3_OFFSET + GPS_CMD_FULLBIN3_SZ - 2)
#define GPS_CMD_FULLBIN4_SUMOFFSET      (GPS_CMD_FULLBIN4_OFFSET + GPS_CMD_FULLBIN4_SZ - 2)
#define GPS_CMD_FULLBIN5_SUMOFFSET      (GPS_CMD_FULLBIN5_OFFSET + GPS_CMD_FULLBIN5_SZ - 2)
#define GPS_CMD_FULLBIN6_SUMOFFSET      (GPS_CMD_FULLBIN6_OFFSET + GPS_CMD_FULLBIN6_SZ - 2)
#define GPS_CMD_FULLBIN_MASK            0x8F

#define GPS_CMD_RESET                   1             /* Reset request command */
#define GPS_CMD_TIMESET                 2             /* Time setting request command                  */
#define GPS_CMD_VERSION                 3             /* GPS version request command             */
#define GPS_CMD_SENTENCEADD_NMEAGST     4             /* Add sentence command NMEA GST       */
#define GPS_CMD_SENTENCEADD_NAVTIMEUTC  5             /* Add sentence command NAV-TIMEUTC    */
#define GPS_CMD_SENTENCEADD_NAVCLOCK    6             /* Add sentence command NAV-CLOCK      */
#define GPS_CMD_SENTENCEADD_RXMRTC5     7             /* Add sentence command RXM-RTC5       */
#define GPS_CMD_SENTENCEADD_NAVSVINFO   8             /* Add sentence command NAV-SVINFO     */
#define GPS_CMD_AUTOMOTIVEMODE          9             /* Automatic mode request command    */
#define GPS_CMD_WKNROLLOVER             10            /* GPS rollover standard week number request command */
#define GPS_CMD_NOTRST                  0             /* Other request command */

/*---------------------------------------------------------------------------*/
// Struct

/*!
    @brief UBX command headers
*/
typedef struct GpsUbxPacketHeader {
  u_int8                  uc_sync_char1;        /* u-blox starts with 2 Bytes 0xB5 0x62 */
  u_int8                  uc_sync_char2;        /* u-blox starts with 2 Bytes 0xB5 0x62 */
  u_int8                  uc_class;             /* Class                                */
  u_int8                  uc_id;                /* ID                                   */
  u_int16                 us_length;            /* Length                               */
} TG_GPS_UBX_PACKET_HEADER;

/*!
    @brief Command-structure without UBX data
*/
typedef struct GpsUbxCmdNoData {
  TG_GPS_UBX_PACKET_HEADER header;
  u_int8                  uc_cka;               /* Checksum_A                           */
  u_int8                  uc_ckb;               /* Checksum_B                           */
} TG_GPS_UBX_COMMAND_NO_DATA;

/*!
    @brief
*/
typedef struct GpsUbxAidIniPolled {
  u_int8                  uc_sync_char1;        /* u-blox starts with 2 Bytes 0xB5 0x62 */
  u_int8                  uc_sync_char2;        /* u-blox starts with 2 Bytes 0xB5 0x62 */
  u_int8                  uc_class;             /* Class                                */
  u_int8                  uc_id;                /* ID                                   */
  u_int8                  uc_length[2];         /* Length                               */
  u_int8                  uc_ecefx_or_lat[4];   /* WGS84 ECEF X coordinate or latitude  */
  u_int8                  uc_ecefy_or_lon[4];   /* WGS84 ECEF Y coordinate or longitude */
  u_int8                  uc_ecefz_or_alt[4];   /* WGS84 ECEF Z coordinate or altitude  */
  u_int8                  uc_pos_acc[4];        /* Position accuracy (stddev)           */
  u_int8                  uc_tm_cfg[2];         /* Time mark configuration              */
  u_int8                  wn[2];                /* Actual week number                   */
  u_int8                  tow[4];               /* Actual time of week                  */
  u_int8                  uc_tow_ns[4];         /* Sub-millisecond part of time of week */
  u_int8                  uc_tacc_ms[4];        /* Milliseconds part of time accuracy   */
  u_int8                  uc_tacc_ns[4];        /* Nanoseconds part of time accuracy    */
  u_int8                  uc_clk_d_or_freq[4];  /* Clock drift or frequency             */
  u_int8                  uc_clk_dacc_or_freqacc[4];  /* Accuracy of clock drift or frequency */
  u_int8                  uc_flags[4];          /* Bitmask with the following flags     */
  u_int8                  uc_cka;               /* Checksum_A                           */
  u_int8                  uc_ckb;               /* Checksum_B                           */
} TG_GPS_UBX_AID_INI_POLLED;

/*!
    @brief UBX ACK command data structures
*/
typedef struct GpsUbxAckData {
  uint8_t                 uc_msg_class;         /* Message Class             */
  uint8_t                 uc_msg_id;            /* Message Identifier        */
} TG_GPS_UBX_ACK_DATA;

/*!
    @brief UBX NAV-TIMEUTC command data structures
*/
typedef struct GpsUbxNavUTC {
  uint32_t    ul_itow;    /* GPS time of week                     */
  uint32_t    ul_tacc;    /* Time accuracy estimate (UTC)         */
  int32_t     nano;       /* Fraction of second                   */
  uint16_t    year;       /* Year, range 1999..2099 (UTC)         */
  uint8_t     month;      /* Month, range 1..12 (UTC)             */
  uint8_t     day;        /* Day of month, range 1..31 (UTC)      */
  uint8_t     hour;       /* Hour of day, range 0..23 (UTC)       */
  uint8_t     min;        /* Minute of hour, range 0..59 (UTC)    */
  uint8_t     sec;        /* Seconds of minute, range 0..60 (UTC) */
  uint8_t     valid;      /* Validity Flags                       */
} TG_GPS_UBX_NAV_TIMEUTC_DATA;

/*!
    @brief   USB NAV-CLOCK command data structures
*/
typedef struct GpsUbxNavClock {
  uint32_t    ul_itow;    /**< GPS Time of week               */
  int32_t     l_clkb;     /**< Clock bias                     */
  int32_t     l_clkd;     /**< Clock drift                    */
  uint32_t    ul_tacc;    /**< Time accuaracy estimate        */
  uint32_t    ul_facc;    /**< Frequency accuracy estimate    */
} TG_GPS_UBX_NAV_CLOCK;

/*!
    @brief   GPS RXM-RTC5 command data structures
*/
typedef struct GpsUbxRxmRtc5 {
  uint32_t    ul_rtag_hw;     /**< RTAG high word                  */
  uint32_t    ul_rtag_lw;     /**< RTAG low word                   */
  uint32_t    freq;           /**< Clock frequency                 */
  uint32_t    ul_freq_frac;   /**< Clock frequency fractional part */
  uint32_t    ul_tow_frac;    /**< Time of week fractional part    */
  uint32_t    tow;            /**< Time of week                    */
  uint16_t    wno;            /**< GPS week number                 */
  uint8_t     uc_tow_valid;   /**< TOW is valid flag               */
  uint8_t     uc_freq_valid;  /**< Frequency is valid flag         */
} TG_GPS_UBX_RXM_RTC5;

/********************************************************************************
 * TAG      :TG_GPS_RCVDATA_NMEA
 * ABSTRACT :Cyclic data (NMEA) area
 * NOTE     :I/F information between communication management thread and vehicle sensor
 ********************************************************************************/
typedef struct GpsRcvDataNMEA {
  u_int8            uc_rcv_flag[GPS_NMEA_INDEX_MAX];  /* Receive flag        */
  u_int8            u_reserve[3];                     /* Reserved          */
  TG_GPS_NMEA_DAT   st_nmea[GPS_NMEA_INDEX_MAX];      /* NMEA data area      */
} TG_GPS_RCVDATA_NMEA;

/********************************************************************************
 * TAG      :TG_GPS_RCVDATA_BINARY
 * ABSTRACT :Cyclic data (standard binary) area
 * NOTE     :I/F information between communication management thread and vehicle sensor
 ********************************************************************************/
typedef struct GpsRcvDataBinary {
  u_int8      uc_rcv_flag;                    /* Receive flag        */
  u_int8      u_reserve[3];                   /* Reserved          */
  u_int8      uc_data[GPS_CMD_BINARY_SZ];     /* Standard binary data area  */
  u_int8      u_reserve2[3];                  /* Reserved          */
} TG_GPS_RCVDATA_BINARY;

/********************************************************************************
 * TAG      :TG_GPS_RCVDATA_FULLBIN
 * ABSTRACT :Cyclic data (full binary) area
 * NOTE     :I/F information between communication management thread and vehicle sensor
 ********************************************************************************/
typedef struct GpsRcvDataFullbin {
  u_int8      uc_rcv_flag;                    /* Receive flag        */
  u_int8      u_reserve[3];                   /* Reserved          */
  u_int8      uc_data[GPS_CMD_FULLBIN_SZ];    /* Full binary data area  */
  u_int8      u_reserve2;                     /* Reserved          */
} TG_GPS_RCVDATA_FULLBIN;

/********************************************************************************
 * TAG      :TG_GPS_RCVDATA
 * ABSTRACT :Cyclic data storage area
 * NOTE     :I/F information between communication management thread and vehicle sensor
 ********************************************************************************/
typedef struct GpsRcvData {
  u_int8                    uc_sns_cnt;       /* Sensor counter value      */
  u_int8                    u_reserve[3];     /* Reserved          */
  TG_GPS_RCVDATA_NMEA       st_nmea_data;     /* NMEA data area      */
  TG_GPS_RCVDATA_BINARY     st_binary_data;   /* Standard binary region      */
  TG_GPS_RCVDATA_FULLBIN    st_fullbin_data;  /* Full binary data area  */
} TG_GPS_RCVDATA;

/*---------------------------------------------------------------------------*/
// Prototype

void    DevGpsSndCycleDataNmea(void);
void    DevGpsRcvCyclCmd(void);
u_int8  DevGpsGetGpsRcvSts(u_int8 sv);
void    DevGpsAnalyzeNmea(NAVIINFO_ALL* navilocinfo);
void    DevGpsCycleDataClear(void);
void    DevGpsCycleDataSetNmea(const u_int8*, u_int32, ENUM_GPS_NMEA_INDEX);
BOOL    DevGpsCycleDataGetNmea(u_int8*, u_int32, ENUM_GPS_NMEA_INDEX);
void    DevGpsCmdEventCheckNmea(void);
void    DevGpsSetChkSum(u_int8* buffer, u_int32 length);
/*---------------------------------------------------------------------------*/
#endif  // INC_GPSCOMMON_MDEV_GPS_NMEA_H_

/*---------------------------------------------------------------------------*/
/*EOF*/
