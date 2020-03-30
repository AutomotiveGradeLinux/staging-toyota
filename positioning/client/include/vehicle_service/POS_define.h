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
#ifndef POSITIONING_CLIENT_INCLUDE_VEHICLE_SERVICE_POS_DEFINE_H_
#define POSITIONING_CLIENT_INCLUDE_VEHICLE_SERVICE_POS_DEFINE_H_

/**
 * @file POS_define.h
 * @brief Header file to define the constants and structure
 */

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup vehicle_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup positioning
 *  @ingroup vehicle_service
 *  @{
 */

#include <native_service/frameworkunified_types.h>
//#include <gps_hal.h>

/*---------------------------------------------------------------------------------*
 * Definition                                                                      *
 *---------------------------------------------------------------------------------*/
#define POS_AVAILABILITY            "Positioning/Availability"  //!< \~english Availability

/* message sender thread */
#define POS_NTFY_SEND_THREAD        "POS_Main"  //!< \~english POS_Main thread
#define POS_NTFY_SEND_THREAD_GPS    "POS_Gps"   //!< \~english POS_Gps thread

/* SENSOR_RET_API */
#define SENSOR_RET_NORMAL             0     //!< \~english normal finish
#define SENSOR_RET_ERROR_PID          (-1)  //!< \~english thread ID error
#define SENSOR_RET_ERROR_DID          (-2)  //!< \~english data ID error
#define SENSOR_RET_ERROR_DID_DIS      (-3)  //!< \~english data ID not CAN ID
#define SENSOR_RET_ERROR_PARAM        (-4)  //!< \~english parameter error
#define SENSOR_RET_ERROR_BUFFULL      (-5)  //!< \~english buffer full error
#define SENSOR_RET_ERROR_CREATE_EVENT (-6)  //!< \~english create event error
#define SENSOR_RET_ERROR_TIMER        (-7)  //!< \~english create timer error
#define SENSOR_RET_ERROR_OUTOF_MEMORY (-8)
//!< \~english share memory allocation size error

#define SENSOR_RET_ERROR_SIZE         (-9)  //!< \~english memory size error
#define SENSOR_RET_ERROR              (-10)  //!< \~english error occured
#define SENSOR_RET_ERROR_NOSUPPORT    (-11)  //!< \~english no support
#define SENSOR_RET_ERROR_INNER        (-12)  //!< \~english Internal error
#define SENSOR_RET_ERROR_RESOURCE     (-13)  //!< \~english lack of resources
#define SENSOR_RET_ERROR_MIN  POS_RET_ERROR_MIN  //!< \~english min value of error

/* Data Status Definition */
#define POS_LOC_INFO_LAT              0x01
//!< \~english current position latitude(bit0) 1:valid 0:invalid

#define POS_LOC_INFO_LON              0x02
//!< \~english current position longitude(bit1) 1:valid 0:invalid

#define POS_LOC_INFO_ALT              0x04
//!< \~english current position altitude(bit2) 1:valid 0:invalid

#define POS_LOC_INFO_HEAD             0x08
//!< \~english current position heading(bit3) 1:valid 0:invalid

/* Definition of positioning system */
#define POS_LOC_INFO_USE_GSP          0x01
//!< \~english GPS data used result(bit0) 1:valid 0:invalid

#define POS_LOC_INFO_USE_DGPS         0x02
//!< \~english DGPS data used result(bit1) 1:valid 0:invalid

#define POS_LOC_INFO_USE_DR           0x04
//!< \~english Dead Reckoning used result(bit2) 1:valid 0:invalid

#define POS_LOC_INFO_USE_MAPMATCHING  0x08
//!< \~english MapMatching result(bit3) 1:valid 0:invalid

/*--- for message ---*/
/* message body size (byte) */
/* move to gps_hal.h */

/*--- for register listener API ---*/
/* control delivery */
#define  SENSOR_DELIVERY_REGIST       0x01  //!< \~english register delivery

/* delivery timing control */
#define  SENSOR_DELIVERY_TIMING_UPDATE  0x01  //!< \~english delivery update timing
#define  SENSOR_DELIVERY_TIMING_CHANGE  0x02  //!< \~english delivery change timing

/* POS_RET_API */
#define POS_RET_NORMAL              0       //!< \~english normal finish
#define POS_RET_ERROR               (-1)    //!< \~english error occured
#define POS_RET_ERROR_DID           (-2)    //!< \~english data ID error
#define POS_RET_ERROR_INNER         (-3)    //!< \~english internal error
#define POS_RET_ERROR_PARAM         (-4)    //!< \~english parameter error
#define POS_RET_ERROR_BUFFULL       (-5)    //!< \~english buffer full error
#define POS_RET_ERROR_CREATE_EVENT  (-6)    //!< \~english create event error
#define POS_RET_ERROR_OUTOF_MEMORY  (-8)
//!< \~english share memory allocation size error

#define POS_RET_ERROR_SIZE          (-9)    //!< \~english memory size error
#define POS_RET_ERROR_TIMEOUT       (-10)   //!< \~english timeout error
#define POS_RET_ERROR_NOSUPPORT     (-11)   //!< \~english no support
#define POS_RET_ERROR_BUSY          (-12)   //!< \~english busy
#define POS_RET_ERROR_RESOURCE      (-13)   //!< \~english lack of resources
#define POS_RET_ERROR_MIN           (-13)   //!< \~english min value of error

/*---------------------------------------------------------------------------------*
 * Typedef declaration                                                             *
 *---------------------------------------------------------------------------------*/
typedef int32_t   SENSOR_RET_API;    //!< \~english API return value
typedef int32_t   POS_RET_API;       //!< \~english API return value
typedef uint32_t  DID;               //!< \~english data ID
typedef int32_t   RET_API;           //!< \~english _CWORD64_ API return value

/** @}*/  // end of positioning
/** @}*/  // end of vehicle_service
/** @}*/  // end of BaseSystem
#endif  // POSITIONING_CLIENT_INCLUDE_VEHICLE_SERVICE_POS_DEFINE_H_
