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
* @file positioning_def.h
*/

#ifndef INC_COMMON_POSITIONING_DEF_H_
#define INC_COMMON_POSITIONING_DEF_H_

/*---------------------------------------------------------------------------*/
// Include files

#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_application.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_multithreading.h>
#include <native_service/frameworkunified_service_protocol.h>
#include <vehicle_service/std_types.h>
#include <vehicle_service/pos_message_header.h>
#include <vehicle_service/sys_timerapi.h>
#include <vehicle_service/WPF_STD.h>

#include "gps_hal.h"
#include "positioning_hal.h"

#include "positioning_log.h"

/*---------------------------------------------------------------------------*/
// Definition

#define POS_THREAD_NAME             "Positioning"

/**
 * @enum ENUM_GPS_NMEA_INDEX
 * \~english  NMEA index
 */
typedef enum  {
  GPS_NMEA_INDEX_GGA = 0,                     //!< \~english GGA
  GPS_NMEA_INDEX_DGGA,                        //!< \~english DGGA
  GPS_NMEA_INDEX_VTG,                         //!< \~english VTG
  GPS_NMEA_INDEX_RMC,                         //!< \~english RMC
  GPS_NMEA_INDEX_DRMC,                        //!< \~english DRMC
  GPS_NMEA_INDEX_GLL,                         //!< \~english GLL
  GPS_NMEA_INDEX_DGLL,                        //!< \~english DGLL
  GPS_NMEA_INDEX_GSA,                         //!< \~english GSA
  GPS_NMEA_INDEX_GSV1,                        //!< \~english GSV1
  GPS_NMEA_INDEX_GSV2,                        //!< \~english GSV2
  GPS_NMEA_INDEX_GSV3,                        //!< \~english GSV3
  GPS_NMEA_INDEX_GSV4,                        //!< \~english GSV4
  GPS_NMEA_INDEX_GSV5,                        //!< \~english GSV5
  GPS_NMEA_INDEX_GST,                         //!< \~english GST
  GPS_NMEA_INDEX__CWORD44__GP_3,                  //!< \~english _CWORD44_,GP,3
  GPS_NMEA_INDEX__CWORD44__GP_4,                  //!< \~english _CWORD44_,GP,4
  GPS_NMEA_INDEX_P_CWORD82_F_GP_0,                  //!< \~english _CWORD44_,GP,0
  GPS_NMEA_INDEX_P_CWORD82_J_GP_1,                  //!< \~english _CWORD44_,GP,1
  GPS_NMEA_INDEX_MAX                          //!< \~english GGA
} ENUM_GPS_NMEA_INDEX;

//!< \~english Gps cycle Data Event
#define GPS_EVT_CYCLEDATA_PROV       ("GPS_EVT_CYCLEDATA_PROV")
//!< \~english Event value: Waiting.
#define GPS_EVT_PROV_WAIT            (0x00000000)
//!< \~english Event value: Clear Waiting status.
#define GPS_EVT_PROV_RELEASE         (0x00000001)

//!< \~english Gps received
#define GPS_CYCLECMD_NOTRCV          (0x00)
//!< \~english Gps no received
#define GPS_CYCLECMD_RCV             (0x01)

//!< \~english store position of Hour data
#define GPSCMD_DRMC_HOUR_POS         (7)
//!< \~english store position of minute data
#define GPSCMD_DRMC_MIN_POS          (9)
//!< \~english store position of second data
#define GPSCMD_DRMC_SEC_POS          (11)
//!< \~english store position of status
#define GPSCMD_DRMC_STATUS_POS       (14)
//!< \~english store position of day data
#define GPSCMD_DRMC_DAY_POS          (54)
//!< \~english store position of month data
#define GPSCMD_DRMC_MONTH_POS        (56)
//!< \~english store position of year data
#define GPSCMD_DRMC_YEAR_POS         (58)

//!< \~english store position of control code
#define GPSCMD_RTC_CTRLCODE_POS      (0)
//!< \~english store position of year data (BCD)
#define GPSCMD_RTC_BCD_YEAR_POS      (1)
//!< \~english store position of month data(BCD)
#define GPSCMD_RTC_BCD_MONTH_POS     (2)
//!< \~english store position of day data(BCD)
#define GPSCMD_RTC_BCD_DAY_POS       (3)
//!< \~english store position of hour data(BCD)
#define GPSCMD_RTC_BCD_HOUR_POS      (4)
//!< \~english store position of minute data(BCD)
#define GPSCMD_RTC_BCD_MIN_POS       (5)
//!< \~english store position of second data(BCD)
#define GPSCMD_RTC_BCD_SEC_POS       (6)
//!< \~english store position of status
#define GPSCMD_RTC_STATUS_POS        (7)

#define GPSCMD_RTC_CTRLCODE          (0xA4)       //!< \~english control code (RTC)
#define GPSCMD_RTC_STATUS_UTCTIME    (0x00)       //!< \~english Normal
#define GPSCMD_RTC_STATUS_ERRTIME    (0x03)       //!< \~english AbNormal

// Internal thread activation status determination
#define THREAD_STS_MSK_POS_MAIN             (0x01)
#define THREAD_STS_MSK_POS_SENS             (0x02)
#define THREAD_STS_MSK_POS_GPS              (0x04)
#define THREAD_STS_MSK_POS_GPS_RECV         (0x08)
#define THREAD_STS_MSK_POS_GPS_ROLLOVER     (0x10)
#define POS_DID_GPS_NMEA                    0x8000009AU /* QAC 1281 */
#define POS_DID_GPS_CLOCK_DRIFT             0x800000B3U /* QAC 1281 */
#define POS_DID_GPS_CLOCK_FREQ              0x800000B4U /* QAC 1281 */
#define VEHICLE_DID_GPS_CUSTOMDATA          0x80000094U
#define VEHICLE_DID_GPS_CLOCK_DRIFT         POS_DID_GPS_CLOCK_DRIFT
#define VEHICLE_DID_GPS_CLOCK_FREQ          POS_DID_GPS_CLOCK_FREQ
#define CID_NAVIINFO_DELIVER                (0x0205)  /* Navigation information setting CID  */
#define CID_NAVIINFO_SPEED_DELIVER          (0x0206)  /* Vehicle speed setting CID    */
#define VEHICLE_DID_GPS_WKNROLLOVER         0x800000B2U
#define VEHICLE_DID_GPS_TIME_RAW            0x800000B1
#define POS_DID_GYRO_TEMP                   0x80000090
#define VEHICLE_DID_SPEED_PULSE             0x80000012
#define VEHICLE_DID_GYRO_X                  0x80000014
#define VEHICLE_DID_GYRO_Y                  0x80000020
#define VEHICLE_DID_GYRO_Z                  0x80000021
#define VEHICLE_DID_GYRO                    VEHICLE_DID_GYRO_X
#define VEHICLE_DID_REV                     0x80000017
#define VEHICLE_DID_SNS_COUNTER             0x8000001A
#define VEHICLE_DID_GPS_COUNTER             0x8000001B
#define VEHICLE_DID_GYRO_EXT                0x80000027    /* 3 ~ 14bit A/D value,0bit:REV */
#define VEHICLE_DID_SPEED_PULSE_FST         0x80000028    /* Pulse(Number)        */
#define VEHICLE_DID_GYRO_FST_X              0x80000029
#define VEHICLE_DID_GYRO_FST_Y              0x80000022
#define VEHICLE_DID_GYRO_FST_Z              0x80000023
#define VEHICLE_DID_GYRO_FST                VEHICLE_DID_GYRO_FST
#define VEHICLE_DID_GYRO_TEMP               POS_DID_GYRO_TEMP
#define VEHICLE_DID_SPEED_KMPH              0x80000013
#define VEHICLE_DID_GSNS_X                  0x80000015
#define VEHICLE_DID_GSNS_Y                  0x80000016
#define VEHICLE_DID_GSNS_Z                  0x80000024
#define VEHICLE_DID_PULSE_TIME              0x8000003A
#define CID__CWORD83__CMD_RCV                     0x0103
#define CID__CWORD83__CMD_SND_STS                 0x0104
#define CID_GPS_SERIAL0                     (CID)0x0100
#define CID_GPS_REQRESET                    (CID)(CID_GPS_BASE | CID_GPS_SERIAL0)

#define VEHICLE_CONFIG_GPS_UNITSET_NOGPS    (0x00U)  //!< \~english no Gps
#define VEHICLE_CONFIG_GPS_UNITSET__CWORD82_      (0x01U)  //!< \~english _CWORD82_ Gps
#define VEHICLE_CONFIG_GPS_UNITSET_EXTBOX   (0x02U)  //!< \~english extern u-blox

//!< \~english no Navi
#define VEHICLE_CONFIG_NAVI_FUNCTION_NONAVI (0x00U)
//!< \~english Navi has AW navi
#define VEHICLE_CONFIG_NAVI_FUNCTION_AWNAVI (0x01U)

#define VEHICLE_GPS_SYSTIME_YEAR_MAX        (2085U)  //!< \~english Gps Time allowed max year
#define VEHICLE_GPS_SYSTIME_MONTH_MAX       (12U)   //!< \~english Gps Time allowed max month
#define VEHICLE_GPS_SYSTIME_DAY_MAX         (31U)   //!< \~english Gps Time allowed max day
#define VEHICLE_GPS_SYSTIME_HOUR_MAX        (23U)   //!< \~english Gps Time allowed max hour
#define VEHICLE_GPS_SYSTIME_MINUTE_MAX      (59U)   //!< \~english Gps Time allowed max minute
#define VEHICLE_GPS_SYSTIME_SECOND_MAX      (59U)   //!< \~english Gps Time allowed max second

#define VEHICLE_GPS_SYSTIME_YEAR_MIN        (1986U)  //!< \~english Gps Time allowed min year
#define VEHICLE_GPS_SYSTIME_MONTH_MIN       (1U)    //!< \~english Gps Time allowed min month
#define VEHICLE_GPS_SYSTIME_DAY_MIN         (1U)    //!< \~english Gps Time allowed min day
#define VEHICLE_GPS_SYSTIME_HOUR_MIN        (0U)    //!< \~english Gps Time allowed min hour
#define VEHICLE_GPS_SYSTIME_MINUTE_MIN      (0U)    //!< \~english Gps Time allowed min minute
#define VEHICLE_GPS_SYSTIME_SECOND_MIN      (0U)    //!< \~english Gps Time allowed min second
#define VEHICLE_EVENT_VAL_INIT              (-14)       /* Event initial value        */
//!< \~english check OK (value not change)
#define MDEV_GPS_SRAM_CHK_OK                (0)
//!< \~english check OK(vaule change)
#define MDEV_GPS_SRAM_CHK_CHG               (1)
//!< \~english   check NG
#define MDEV_GPS_SRAM_CHK_NG                (-1)

/*---------------------------------------------------------------------------*/
// ENUMERATION

/*!
   @brief   Positioning operating status definitions
*/
typedef enum EnumExeStsPos {
  EPOS_EXE_STS_STOP = 0,             /* Stopped                               */
  EPOS_EXE_STS_RUNNING,              /* Running (from FrameworkunifiedOnStart to FrameworkunifiedOnStop) */
  EPOS_EXE_STS_RUNNING_COLDSTART     /* Running after cold start             */
} EnumExeSts_POS;

/*!
   @brief   Positioning Thread Startup Modes
*/
typedef enum EnumSetupModePos {
  EPOS_SETUP_MODE_NORMAL = 0,        /* Normal start           */
  EPOS_SETUP_MODE_DATA_RESET         /* Data reset start */ /* QAC 930 */
} EnumSetupMode_POS;

/**
 * @struct TG_GPS_RCV_DATA
 * \~english  Struct of Gps receive data for VehicleSens
 */
typedef struct TgGpsRcvData {
  uint32_t    dwret_status;                       //!< \~english return status
  uint16_t    bydata_len;                         //!< \~english data length
  uint8_t     bygps_data[GPS_READ_LEN];           //!< \~english receive data
  uint8_t     u_reserve2[2];                      //!< \~english reserve
} TG_GPS_RCV_DATA;

/**
 * @struct TG_GPS_NMEA_DAT
 * \~english  Struct of NMEA data
 */
typedef struct TgGpsNmeaDat {
  u_int8      uc_data[GPS_NMEA_MAX_SZ];           //!< \~english NMEA data
} TG_GPS_NMEA_DAT;

/**
 * @struct TG_GPS_CYCLEDATA_NMEA
 * \~english  Struct of NMEA cycle data
 */
typedef struct TgGpsCycledataNmea {
  //!< \~english receive flag
  u_int8      uc_rcvflag[GPS_NMEA_INDEX_MAX];
  //!< \~english reserve
  u_int8      u_reserve[3];
  //!< \~english structure of NMEA data
  TG_GPS_NMEA_DAT    st_nmea[GPS_NMEA_INDEX_MAX];
} TG_GPS_CYCLEDATA_NMEA;

/**
 * @struct TG_GPS_CYCLEDATA_BINARY
 * \~english  Struct of binary cycle data
 */
typedef struct TgGpsCycledataBinary {
  u_int8      uc_rcvflag;                        //!< \~english receive flag
  u_int8      u_reserve[3];                       //!< \~english reserve
  u_int8      uc_data[GPS_CMD_BINARY_SZ];        //!< \~english binary data
  u_int8      u_reserve2[3];                      //!< \~english reserve flag
} TG_GPS_CYCLEDATA_BINARY;

/**
 * @struct TG_GPS_CYCLEDATA_FULLBIN
 * \~english  Struct of full bin cycle data
 */
typedef struct TgGpsCycledataFullbin {
  u_int8      uc_rcvflag;                        //!< \~english receive flag
  u_int8      u_reserve[3];                       //!< \~english reserve
  u_int8      uc_data[GPS_CMD_FULLBIN_SZ];       //!< \~english fullbin data
  u_int8      u_reserve2;                         //!< \~english reserve
} TG_GPS_CYCLEDATA_FULLBIN;

/**
 * @struct TG_GPS_CYCLEDATA_RTC
 * \~english  Struct of RTC cycle data
 */
typedef struct TgGpsCycledataRtc {
  u_int8      uc_rcvflag;                        //!< \~english receive flag
  u_int8      u_reserve[3];                       //!< \~english reserve
  u_int8      uc_data[GPS_DATASIZE_RTC];         //!< \~english Rtc data
} TG_GPS_CYCLEDATA_RTC;

/**
 * @struct TG_GPS_CYCLEDATA
 * \~english  Struct of Gps cycle data
 */
typedef struct TgGpsCycledata {
  u_int8                        uc_sesncnt;      //!< \~english count value
  u_int8                        u_reserve[3];     //!< \~english reserve
  TG_GPS_CYCLEDATA_NMEA         st_nmea_data;     //!< \~english NMEA Data
  TG_GPS_CYCLEDATA_BINARY       st_binary_data;   //!< \~english binary Data
  TG_GPS_CYCLEDATA_FULLBIN      st_fullbin_data;  //!< \~english Full Bin Data
  TG_GPS_CYCLEDATA_RTC          st_rtc_data;      //!< \~english RTC Data
} TG_GPS_CYCLEDATA;

typedef struct {
    u_int16 year; /* YEAR */
    u_int8 month; /* MONTH */
    u_int8 date;  /* DAY */
    u_int8 hour;  /* HOUR */
    u_int8 minute;/* MINUTE */
    u_int8 second;/* SECOND */
    u_int8 flag;  /* Whether or not the time is set */
} ST_GPS_SET_TIME;

/*---------------------------------------------------------------------------*/
#endif  // INC_COMMON_POSITIONING_DEF_H_

/*---------------------------------------------------------------------------*/
/*EOF*/
