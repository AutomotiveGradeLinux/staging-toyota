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
#define VEHICLE_DID_GPS_UBLOX_NAV_POSLLH                 0x80000060
#define VEHICLE_DID_GPS_UBLOX_NAV_STATUS                 0x80000061
#define VEHICLE_DID_GPS_UBLOX_NAV_TIMEUTC                0x80000062
#define VEHICLE_DID_GPS_UBLOX_NAV_VELNED                 0x80000063
#define VEHICLE_DID_GPS_UBLOX_NAV_DOP                    0x80000064
#define VEHICLE_DID_GPS_UBLOX_NAV_TIMEGPS                0x80000065
#define VEHICLE_DID_GPS_UBLOX_NAV_SVINFO                 0x80000066
#define VEHICLE_DID_GPS_UBLOX_NAV_CLOCK                  0x80000067
#define VEHICLE_DID_GPS_UBLOX_MON_HW                     0x80000068
#define VEHICLE_DID_GPS_UBLOX_ACK_ACK                    0x80000069
#define VEHICLE_DID_GPS_UBLOX_ACK_NAK                    0x8000006A
#define VEHICLE_DID_GPS_UBLOX_CFG_RST                    0x8000006B
#define VEHICLE_DID_GPS_UBLOX_AID_INI                    0x8000006C
#define VEHICLE_DID_GPS_UBLOX_AID_EPH                    0x8000006D
#define VEHICLE_DID_GPS_UBLOX_CFG_MSG                    0x8000006E
#define VEHICLE_DID_GPS_UBLOX_CFG_NAVX5                  0x8000006F
/* -- PastModel002 Support_UBX_Protocol_DID */

/* ++ used internally by the DR */
#define VEHICLE_DID_GPS_COUNTER                          0x8000001B
#define VEHICLE_DID_GYRO_EXT                             0x80000027

#define VEHICLE_DID_DR_LONGITUDE                         0x80000070
#define VEHICLE_DID_DR_LATITUDE                          0x80000071
#define VEHICLE_DID_DR_ALTITUDE                          0x80000072
#define VEHICLE_DID_DR_SPEED                             0x80000073
#define VEHICLE_DID_DR_HEADING                           0x80000074
#define VEHICLE_DID_DR_SNS_COUNTER                       0x80000075
#define VEHICLE_DID_DR_GYRO_OFFSET                       0x80000078
#define VEHICLE_DID_DR_GYRO_SCALE_FACTOR                 0x80000079
#define VEHICLE_DID_DR_GYRO_SCALE_FACTOR_LEVEL           0x8000007A
#define VEHICLE_DID_DR_SPEED_PULSE_SCALE_FACTOR          0x8000007B
#define VEHICLE_DID_DR_SPEED_PULSE_SCALE_FACTOR_LEVEL    0x8000007C

/* -- used internally by the DR */

/* ++ PastModel002 Support_DID */
#define VEHICLE_DID_GYRO_TROUBLE                         0x80000080
#define VEHICLE_DID_MAIN_GPS_INTERRUPT_SIGNAL            0x80000081
#define VEHICLE_DID_SYS_GPS_INTERRUPT_SIGNAL             0x80000082
#define VEHICLE_DID_GYRO_CONNECT_STATUS                  0x80000083
#define VEHICLE_DID_VALID_EPH_NUM                        0x80000084
/* -- PastModel002 Support_DID */

/* ++ Not supported by UBX_Protocol */
#define VEHICLE_DID_LOCATION                             0x80000020
#define VEHICLE_DID_GPS__CWORD82__NMEA                   POS_DID_GPS__CWORD82__NMEA
#define VEHICLE_DID_GPS__CWORD82__SETINITIAL             POS_DID_GPS__CWORD82__SETINITIAL
#define VEHICLE_DID_GPS__CWORD82__SETRMODE               0x80000034
#define VEHICLE_DID_GPS__CWORD82__SETRMODEEX             POS_DID_GPS__CWORD82__SETRMODEEX
#define VEHICLE_DID_GPS__CWORD82__SELSENT                POS_DID_GPS__CWORD82__SELSENT
#define VEHICLE_DID_GPS__CWORD82__SETSBAS                0x80000037
#define VEHICLE_DID_GPS__CWORD82__SETCONF1               0x80000038
#define VEHICLE_DID_GPS__CWORD82__SETCONF2               0x80000039
#define VEHICLE_DID_GPS__CWORD82__NMEA_GGA_INTERNAL      0xA050
#define VEHICLE_DID_GPS__CWORD82__NMEA_DGGA_INTERNAL     0xA051
#define VEHICLE_DID_GPS__CWORD82__NMEA_VTG_INTERNAL      0xA052
#define VEHICLE_DID_GPS__CWORD82__NMEA_RMC_INTERNAL      0xA053
#define VEHICLE_DID_GPS__CWORD82__NMEA_DRMC_INTERNAL     0xA054
#define VEHICLE_DID_GPS__CWORD82__NMEA_GLL_INTERNAL      0xA055
#define VEHICLE_DID_GPS__CWORD82__NMEA_DGLL_INTERNAL     0xA056
#define VEHICLE_DID_GPS__CWORD82__NMEA_GSA_INTERNAL      0xA057
#define VEHICLE_DID_GPS__CWORD82__NMEA_GSV1_INTERNAL     0xA058
#define VEHICLE_DID_GPS__CWORD82__NMEA_GSV2_INTERNAL     0xA059
#define VEHICLE_DID_GPS__CWORD82__NMEA_GSV3_INTERNAL     0xA060
#define VEHICLE_DID_GPS__CWORD82__NMEA_PJRDC_GP_3_INTERNAL     0xA061
#define VEHICLE_DID_LOCATION_LONLAT                      0x80000095
#define VEHICLE_DID_LOCATION_ALTITUDE                    0x80000096
#define VEHICLE_DID_MOTION_SPEED                         0x80000062
#define VEHICLE_DID_MOTION_HEADING                       0x80000097
#define VEHICLE_DID_GPS_TIME                             0x80000098
#define VEHICLE_DID_NAVIINFO_DIAG_GPS                    0x80000099
#define VEHICLE_DID_GPSWEEKCOR_CNT                       0x8000009A

#define VEHICLE_DID_GPS_CUSTOMDATA_NAVI                  0x800000A0
#define VEHICLE_DID_LOCATION_LONLAT_NAVI                 0x800000A1
#define VEHICLE_DID_LOCATION_ALTITUDE_NAVI               0x800000A2
#define VEHICLE_DID_MOTION_SPEED_NAVI                    0x800000A3
#define VEHICLE_DID_MOTION_HEADING_NAVI                  0x800000A4
#define VEHICLE_DID_SETTINGTIME                          0x800000A5

#define VEHICLE_DID_LOCATIONINFO_NMEA_NAVI               0x800000A6

#define VEHICLE_DID_MOTION_SPEED_INTERNAL                0x800000B0

#define VEHICLE_DID_GPS_TIME_RAW                         0x800000B1U   /* QAC 1281 */
#define VEHICLE_DID_GPS_WKNROLLOVER                      0x800000B2U   /* QAC 1281 */

#define VEHICLE_DID_SPEED_PULSE_VEHICLE                  (0x80000044UL)

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
