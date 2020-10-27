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
#define POS_SNS_GPS_NMEA_PASCD  (0x01000000U)    //!< \~english pascd sentence

typedef    u_int16    PNO;      //!< \~english PNo.

/*---------------------------------------------------------------------------------*
 * Struct declaration                                                              *
 *---------------------------------------------------------------------------------*/

/** @}*/  // end of positioning
/** @}*/  // end of vehicle_service
#endif  // POSITIONING_CLIENT_INCLUDE_COMMONDEFINE_H_
