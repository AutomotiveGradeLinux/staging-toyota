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
#ifndef POSITIONING_CLIENT_INCLUDE_COMMONDEFINE_H_
#define POSITIONING_CLIENT_INCLUDE_COMMONDEFINE_H_

/**
 * @file CommonDefine.h
 */

/** @addtogroup vehicle_service
 *  @{
 */
/** @addtogroup positioning
 *  @ingroup vehicle_service
 *  @{
 */

/*---------------------------------------------------------------------------------*
 * Incluce                                                                         *
 *---------------------------------------------------------------------------------*/
#include <vehicle_service/POS_sensor_API.h>
#include <vehicle_service/POS_define.h>
#include <vehicle_service/pos_message_header.h>

/*---------------------------------------------------------------------------------*
 * Definition                                                                      *
 *---------------------------------------------------------------------------------*/
/* Date and Time Status Definition */
#define  NAVIINFO_NAVI_GPS_TD_STS_NOTFIX      (0)
//!< \~english time has not been adjusted after GPS receiver reset
#define  NAVIINFO_NAVI_GPS_TD_STS_RTCBACKUP   (1)   // !<\~english time output from RTC Backup(have time adjustment)
#define  NAVIINFO_NAVI_GPS_TD_STS_FIX         (2)   //!< \~english time adjustment completed

/* Vehicle sensor data information */
#define POS_SNS_CONNECTION      (0x01U)    //!< \~english connected
#define POS_SNS_UNCONNECTION    (0x00U)    //!< \~english not connected
#define POS_SNS_SHORT           (0x02U)    //!< \~english short
#define POS_SNS_UNSPECIFIED     (0x03U)    //!< \~english not specified

/* NMEA Receive Flag(_CWORD82_) */
#define POS_SNS__CWORD82__DRMC        (0x01U)    //!< \~english DRMC sentence
#define POS_SNS__CWORD82__GSA         (0x02U)    //!< \~english GSA sentence
#define POS_SNS__CWORD82__GSV_1       (0x04U)    //!< \~english GSV_1 sentence
#define POS_SNS__CWORD82__GSV_2       (0x08U)    //!< \~english GSV_2 sentence
#define POS_SNS__CWORD82__GSV_3       (0x10U)    //!< \~english GSV_3 sentence
#define POS_SNS__CWORD82__PJRDC_GP_3  (0x20U)    //!< \~english PJRDC_GP_3 sentence

/* NMEA Receive Flag */
#define POS_SNS_GPS_NMEA_GGA    (0x00000001U)    //!< \~english GGA sentence
#define POS_SNS_GPS_NMEA_GLL    (0x00000002U)    //!< \~english GLL sentence
#define POS_SNS_GPS_NMEA_GSA    (0x00000004U)    //!< \~english GSA sentence
#define POS_SNS_GPS_NMEA_GST    (0x00000008U)    //!< \~english GST sentence
#define POS_SNS_GPS_NMEA_RMC    (0x00000010U)    //!< \~english RMC sentence
#define POS_SNS_GPS_NMEA_VTG    (0x00000020U)    //!< \~english VTG sentence
#define POS_SNS_GPS_NMEA_GSV1   (0x00000040U)    //!< \~english GSV1 sentence
#define POS_SNS_GPS_NMEA_GSV2   (0x00000080U)    //!< \~english GSV2 sentence
#define POS_SNS_GPS_NMEA_GSV3   (0x00000100U)    //!< \~english GSV3 sentence
#define POS_SNS_GPS_NMEA_GSV4   (0x00000200U)    //!< \~english GSV4 sentence
#define POS_SNS_GPS_NMEA_GSV5   (0x00000400U)    //!< \~english GSV5 sentence

#define POS_SNS_GPS_NMEA_PASCD  (0x00000800U)    //!< \~english pascd sentence

/**
 * @enum POS_SNS_GPS_NMEA_SNO
 * \~english type of NMEA
 */
typedef enum {
  POS_SNS_GPS_NMEA_SNO_GGA = 0,    //!< \~english GGA sentence
  POS_SNS_GPS_NMEA_SNO_GLL,    //!< \~english GLL sentence
  POS_SNS_GPS_NMEA_SNO_GSA,    //!< \~english GSA sentence
  POS_SNS_GPS_NMEA_SNO_GST,    //!< \~english GST sentence
  POS_SNS_GPS_NMEA_SNO_RMC,    //!< \~english RMC sentence
  POS_SNS_GPS_NMEA_SNO_VTG,    //!< \~english VTG sentence
  POS_SNS_GPS_NMEA_SNO_GSV1,    //!< \~english GSV1 sentence
  POS_SNS_GPS_NMEA_SNO_GSV2,    //!< \~english GSV2 sentence
  POS_SNS_GPS_NMEA_SNO_GSV3,    //!< \~english GSV3 sentence
  POS_SNS_GPS_NMEA_SNO_GSV4,    //!< \~english GSV4 sentence
  POS_SNS_GPS_NMEA_SNO_GSV5,    //!< \~english GSV5 sentence

  POS_SNS_GPS_NMEA_SNO_PASCD,   //!< \~english pascd sentence
  POS_SNS_GPS_NMEA_SNO_MAX      //!< \~english MAX sentence(invalid)
} POS_SNS_GPS_NMEA_SNO;

typedef    u_int16    PNO;      //!< \~english PNo.

/*---------------------------------------------------------------------------------*
 * Struct declaration                                                              *
 *---------------------------------------------------------------------------------*/

/** @}*/  // end of positioning
/** @}*/  // end of vehicle_service
#endif  // POSITIONING_CLIENT_INCLUDE_COMMONDEFINE_H_
