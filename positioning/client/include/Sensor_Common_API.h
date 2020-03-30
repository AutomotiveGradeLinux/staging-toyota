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

#ifndef POSITIONING_CLIENT_INCLUDE_SENSOR_COMMON_API_H_
#define POSITIONING_CLIENT_INCLUDE_SENSOR_COMMON_API_H_
/******************************************************************************
 * File name        :Sensor_Common_API.h
 * System name        :PastModel002
 * Sub System name    :Vehicle I/F Liblary,Sensor I/F Liblary
 ******************************************************************************/

#include <vehicle_service/positioning_base_library.h>
#include <vehicle_service/POS_sensor_API.h>
#include "CommonDefine.h"

/************************************************************************
 *            Definition                                                *
 ************************************************************************/
/*----------------------------------------------------------------------*
 *    ID data corresponding to the vehicle sensor information           *
 *----------------------------------------------------------------------*/

/*
 *  Note.
 *  This feature needs to be defined by the vendor.
 */

/* VEHICLE__DID */

/* ++ PastModel002 Support_UBX_Protocol_DID */
#define VEHICLE_DID_GPS_UBLOX_NAV_POSLLH                 0x00000000
#define VEHICLE_DID_GPS_UBLOX_NAV_STATUS                 0x00000001
#define VEHICLE_DID_GPS_UBLOX_NAV_TIMEUTC                0x00000002
#define VEHICLE_DID_GPS_UBLOX_NAV_VELNED                 0x00000003
#define VEHICLE_DID_GPS_UBLOX_NAV_DOP                    0x00000004
#define VEHICLE_DID_GPS_UBLOX_NAV_TIMEGPS                0x00000005
#define VEHICLE_DID_GPS_UBLOX_NAV_SVINFO                 0x00000006
#define VEHICLE_DID_GPS_UBLOX_NAV_CLOCK                  0x00000007
#define VEHICLE_DID_GPS_UBLOX_MON_HW                     0x00000008
#define VEHICLE_DID_GPS_UBLOX_ACK_ACK                    0x00000009
#define VEHICLE_DID_GPS_UBLOX_ACK_NAK                    0x0000000A
#define VEHICLE_DID_GPS_UBLOX_CFG_RST                    0x0000000B
#define VEHICLE_DID_GPS_UBLOX_AID_INI                    0x0000000C
#define VEHICLE_DID_GPS_UBLOX_AID_EPH                    0x0000000D
#define VEHICLE_DID_GPS_UBLOX_CFG_MSG                    0x0000000E
#define VEHICLE_DID_GPS_UBLOX_CFG_NAVX5                  0x0000000F
/* -- PastModel002 Support_UBX_Protocol_DID */

/* ++ used internally by the DR */
#define VEHICLE_DID_GPS_COUNTER                          0x00000010
#define VEHICLE_DID_GYRO_EXT                             0x00000011    /* 3 to 14bit A/D value,0bit:REV    */

#define VEHICLE_DID_DR_LONGITUDE                         0x00000012
#define VEHICLE_DID_DR_LATITUDE                          0x00000013
#define VEHICLE_DID_DR_ALTITUDE                          0x00000014
#define VEHICLE_DID_DR_SPEED                             0x00000015
#define VEHICLE_DID_DR_HEADING                           0x00000016
#define VEHICLE_DID_DR_SNS_COUNTER                       0x00000017
#define VEHICLE_DID_DR_GYRO_OFFSET                       0x00000018
#define VEHICLE_DID_DR_GYRO_SCALE_FACTOR                 0x00000019
#define VEHICLE_DID_DR_GYRO_SCALE_FACTOR_LEVEL           0x0000001A
#define VEHICLE_DID_DR_SPEED_PULSE_SCALE_FACTOR          0x0000001B
#define VEHICLE_DID_DR_SPEED_PULSE_SCALE_FACTOR_LEVEL    0x0000001C

/* -- used internally by the DR */

/* ++ PastModel002 Support_DID */
#define VEHICLE_DID_GYRO_TROUBLE                         0x0000001D
#define VEHICLE_DID_MAIN_GPS_INTERRUPT_SIGNAL            0x0000001E
#define VEHICLE_DID_SYS_GPS_INTERRUPT_SIGNAL             0x0000001F
#define VEHICLE_DID_GYRO_CONNECT_STATUS                  0x00000020
#define VEHICLE_DID_VALID_EPH_NUM                        0x00000021
/* -- PastModel002 Support_DID */

/* ++ Not supported by UBX_Protocol */
#define VEHICLE_DID_2WD4WD                               0x00000022
#define VEHICLE_DID_TEST                                 0x00000023
#define VEHICLE_DID_VTRADAPTER                           0x00000024
#define VEHICLE_DID_AUXADAPTER                           0x00000025
#define VEHICLE_DID_PANELTEMP                            0x00000026
#define VEHICLE_DID_MINIJACK                             0x00000027
#define VEHICLE_DID_SIRF_BINARY                          0x00000028
#define VEHICLE_DID_RTC                                  0x00000029
#define VEHICLE_DID_SATELLITE_STATUS                     0x0000002A
#define VEHICLE_DID_LOCATION                             0x0000002B
#define VEHICLE_DID_BACKDOOR_LINE                        0x0000002C
#define VEHICLE_DID_ADIM_LINE                            0x0000002D
#define VEHICLE_DID_BACKDOOR_CAN                         0x0000002E
#define VEHICLE_DID_ADIM_CAN                             0x0000002F
#define VEHICLE_DID_GGA                                  0x00000030
#define VEHICLE_DID_GLL                                  0x00000031
#define VEHICLE_DID_GSA                                  0x00000032
#define VEHICLE_DID_GSV                                  0x00000033
#define VEHICLE_DID_RMC                                  0x00000034
#define VEHICLE_DID_VTG                                  0x00000035
#define VEHICLE_DID_TOUCH                                0x00000036
#define VEHICLE_DID_KEY                                  0x00000037
#define VEHICLE_DID_REMO                                 0x00000038
#define VEHICLE_DID_VSC1S03                              0x00000039
#define VEHICLE_DID_ECO1S01                              0x0000003A
#define VEHICLE_DID_ENG1F07                              0x0000003B
#define VEHICLE_DID_ENG1S03                              0x0000003C
#define VEHICLE_DID_ACN1S04                              0x0000003D
#define VEHICLE_DID_ACN1S05                              0x0000003E
#define VEHICLE_DID_ACN1S06                              0x0000003F
#define VEHICLE_DID_ACN1S08                              0x00000040
#define VEHICLE_DID_ACN1S07                              0x00000041
#define VEHICLE_DID_EHV1S90                              0x00000042
#define VEHICLE_DID_ECT1S92                              0x00000043
#define VEHICLE_DID_ENG1S28                              0x00000044
#define VEHICLE_DID_BGM1S01                              0x00000045
#define VEHICLE_DID_ENG1F03                              0x00000046
#define VEHICLE_DID_CAA1N01                              0x00000047
#define VEHICLE_DID_MET1S01                              0x00000048
#define VEHICLE_DID_MET1S03                              0x00000049
#define VEHICLE_DID_MET1S04                              0x0000004A
#define VEHICLE_DID_MET1S05                              0x0000004B
#define VEHICLE_DID_MET1S07                              0x0000004C
#define VEHICLE_DID_BDB1S01                              0x0000004D
#define VEHICLE_DID_BDB1S03                              0x0000004E
#define VEHICLE_DID_BDB1S08                              0x0000004F
#define VEHICLE_DID_BDB1F03                              0x00000050
#define VEHICLE_DID_TPM1S02                              0x00000051
#define VEHICLE_DID_TPM1S03                              0x00000052
#define VEHICLE_DID_ENG1S92                              0x00000053
#define VEHICLE_DID_MMT1S52                              0x00000054
#define VEHICLE_DID_EPB1S01                              0x00000055
#define VEHICLE_DID_GPS__CWORD82__NMEA                         0x00000056
#define VEHICLE_DID_GPS__CWORD82__SETINITIAL                   0x00000057
#define VEHICLE_DID_GPS__CWORD82__SETRMODE                     0x00000058
#define VEHICLE_DID_GPS__CWORD82__SETRMODEEX                   0x00000059
#define VEHICLE_DID_GPS__CWORD82__SELSENT                      0x0000005A
#define VEHICLE_DID_GPS__CWORD82__SETSBAS                      0x0000005B
#define VEHICLE_DID_GPS__CWORD82__SETCONF1                     0x0000005C
#define VEHICLE_DID_GPS__CWORD82__SETCONF2                     0x0000005D
#define VEHICLE_DID_GPS__CWORD82__NMEA_GGA_INTERNAL            0x0000005E
#define VEHICLE_DID_GPS__CWORD82__NMEA_DGGA_INTERNAL           0x0000005F
#define VEHICLE_DID_GPS__CWORD82__NMEA_VTG_INTERNAL            0x00000060
#define VEHICLE_DID_GPS__CWORD82__NMEA_RMC_INTERNAL            0x00000061
#define VEHICLE_DID_GPS__CWORD82__NMEA_DRMC_INTERNAL           0x00000062
#define VEHICLE_DID_GPS__CWORD82__NMEA_GLL_INTERNAL            0x00000063
#define VEHICLE_DID_GPS__CWORD82__NMEA_DGLL_INTERNAL           0x00000064
#define VEHICLE_DID_GPS__CWORD82__NMEA_GSA_INTERNAL            0x00000065
#define VEHICLE_DID_GPS__CWORD82__NMEA_GSV1_INTERNAL           0x00000066
#define VEHICLE_DID_GPS__CWORD82__NMEA_GSV2_INTERNAL           0x00000067
#define VEHICLE_DID_GPS__CWORD82__NMEA_GSV3_INTERNAL           0x00000068
#define VEHICLE_DID_GPS__CWORD82__NMEA_PJRDC_GP_3_INTERNAL     0x00000069
#define VEHICLE_DID_LOCATION_LONLAT                      0x0000006A
#define VEHICLE_DID_LOCATION_ALTITUDE                    0x0000006B
#define VEHICLE_DID_MOTION_SPEED                         0x0000006C
#define VEHICLE_DID_MOTION_HEADING                       0x0000006D
#define VEHICLE_DID_GPS_TIME                             0x0000006E
#define VEHICLE_DID_NAVIINFO_DIAG_GPS                    0x0000006F
#define VEHICLE_DID_GPSWEEKCOR_CNT                       0x00000070

#define VEHICLE_DID_GPS_CUSTOMDATA_NAVI                  0x00000071
#define VEHICLE_DID_LOCATION_LONLAT_NAVI                 0x00000072
#define VEHICLE_DID_LOCATION_ALTITUDE_NAVI               0x00000073
#define VEHICLE_DID_MOTION_SPEED_NAVI                    0x00000074
#define VEHICLE_DID_MOTION_HEADING_NAVI                  0x00000075
#define VEHICLE_DID_SETTINGTIME                          0x00000076

#define VEHICLE_DID_LOCATIONINFO_NMEA_NAVI               0x00000077

#define VEHICLE_DID_MOTION_SPEED_INTERNAL                0x00000078

#define VEHICLE_DID_GPS_TIME_RAW                         0x00000079U   /* QAC 1281 */
#define VEHICLE_DID_GPS_WKNROLLOVER                      0x0000007AU   /* QAC 1281 */

#define VEHICLE_DID_SPEED_PULSE_VEHICLE                  (0x00000080UL)

/* -- Not supported by UBX_Protocol */

/*----------------------------------------------------------------------*
 *    Vehicle sensor data information                                   *
 *----------------------------------------------------------------------*/
#define VEHICLE_SNS_BATTERY_SHORT                        0x02          /* BATTERY SHORT        */
#define VEHICLE_SNS_THERMAL_SHUT_DOWN                    0x04          /* THERMAL SHUT DOWN    */
#define VEHICLE_SNS_UNKNOWN                              0x05          /* UNKNOWN                */

/* Missing information data */
#define VEHICLE_SNS_BREAK                                0x01          /* Data without continuity    */
#define VEHICLE_SNS_NORMAL                               0x00          /* Continuity of data    */

/*-------------------------------------------------------------------------------
 * Time Status Return Values
 -------------------------------------------------------------------------------*/
#define GPS_TIMESTS_OK                                   (0x00U)       /* Exact values from the satellites     */
/* Ignore->MISRA-C++ Rule 16-2-2 */
#define GPS_TIMESTS_NOTUTC                               (0x01U)       /* Time from satellite but UTC uncorrected  */
/* Ignore->MISRA-C++ Rule 16-2-2 */
#define GPS_TIMESTS_RTC                                  (0x02U)       /* RTC time   */
/* Ignore->MISRA-C++ Rule 16-2-2 */
#define GPS_TIMESTS_NG                                   (0x03U)       /* Anomaly Time  */
/* Ignore->MISRA-C++ Rule 16-2-2 */

/*-------------------------------------------------------------------------------
 * NMEA Receive Flag
 -------------------------------------------------------------------------------*/
#define VEHICLE_SNS__CWORD82__DRMC                             POS_SNS__CWORD82__DRMC
#define VEHICLE_SNS__CWORD82__GSA                              POS_SNS__CWORD82__GSA
#define VEHICLE_SNS__CWORD82__GSV_1                            POS_SNS__CWORD82__GSV_1
#define VEHICLE_SNS__CWORD82__GSV_2                            POS_SNS__CWORD82__GSV_2
#define VEHICLE_SNS__CWORD82__GSV_3                            POS_SNS__CWORD82__GSV_3
#define VEHICLE_SNS__CWORD82__PJRDC_GP_3                       POS_SNS__CWORD82__PJRDC_GP_3
#define VEHICLE_SNS__CWORD82__PJRDC_GP_4                       (0x40U)       /* Ignore->MISRA-C++ Rule 16-2-2 */

/************************************************************************
*            enum Definition                                                 *
*************************************************************************/

/*-------------------------------------------------------------------------------
 * _CWORD71_ UNIT type
 -------------------------------------------------------------------------------*/
typedef enum {
    POS_TYPE_NONE = (0U),       /* Undefined               */
    POS_TYPE_GRADE1,            /* GRADE1 (Integrated)      */
    POS_TYPE_GRADE2             /* GRADE2 (Binary type)   */
} POS_TYPE;

/************************************************************************
*            typedef Definition                                            *
*************************************************************************/

typedef u_int32 DID;            /* DataID            */

/************************************************************************
*            struct Definition                                            *
*************************************************************************/

#endif  // POSITIONING_CLIENT_INCLUDE_SENSOR_COMMON_API_H_
