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
#ifndef POSITIONING_CLIENT_INCLUDE_VEHICLE_SERVICE_POS_SENSOR_API_H_
#define POSITIONING_CLIENT_INCLUDE_VEHICLE_SERVICE_POS_SENSOR_API_H_
/**
 * @file POS_sensor_API.h
 * @brief API definition file for Sensor function
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
/*---------------------------------------------------------------------------------*
 * Incluce                                                                         *
 *---------------------------------------------------------------------------------*/
#include <vehicle_service/POS_define.h>

/*---------------------------------------------------------------------------------*
 * Definition                                                                      *
 *---------------------------------------------------------------------------------*/
/* POSITIONING_DID */
#define POS_DID_SPEED_PULSE     0x80000012  //!< \~english Data ID of speed pulse
#define POS_DID_SPEED_KMPH      0x80000013  //!< \~english Data ID of KMPH speed
#define POS_DID_SNS_COUNTER     0x8000001A
//!< \~english Data ID of sensor counter
#define POS_DID_GYRO_X          0x80000014  //!< \~english Data ID of X axis gyro
#define POS_DID_GYRO_Y          0x80000085  //!< \~english Data ID of Y axis gyro
#define POS_DID_GYRO_Z          0x80000086  //!< \~english Data ID of Z axis gyro
#define POS_DID_GYRO            POS_DID_GYRO_X
//!< \~english Data ID of POS_DID_GYRO is same as POS_DID_GYRO_X
#define POS_DID_GSNS_X          0x80000015  //!< \~english Data ID of x axis gsensor
#define POS_DID_GSNS_Y          0x80000016  //!< \~english Data ID of Y axis gsensor
#define POS_DID_GSNS_Z          0x80000026  //!< \~english Data ID of Z axis gsensor
#define POS_DID_REV             0x80000017  //!< \~english Data ID of reverse signal
#define POS_DID_GPS_ANTENNA     0x80000019
//!< \~english Data ID of GPS antenna status
#define POS_DID_SPEED_PULSE_FST 0x80000028
//!< \~english Data ID of first time speed pulse
#define POS_DID_GYRO_X_FST      0x80000029  //!< \~english Data ID of first time X axis gyro
#define POS_DID_GYRO_Y_FST      0x80000043  //!< \~english Data ID of first time Y axis gyro
#define POS_DID_GYRO_Z_FST      0x80000023  //!< \~english Data ID of first time Z axis gyro
#define POS_DID_GYRO_FST        POS_DID_GYRO_X_FST
//!< \~~english Data ID of POS_DID_GYRO_FST is same as POS_DID_GYRO_X_FST
#define POS_DID_REV_FST         0x8000007E
//!< \~english Data ID of first time reverse signal
#define POS_DID_GYRO_TEMP       0x80000090  //!< \~english Data ID of gyro temperature
#define POS_DID_GYRO_TEMP_FST   0x80000091
//!< \~english Data ID of first time gyro temperature
#define POS_DID_GSNS_X_FST      0x80000087
//!< \~english Data ID of first time x axis gsensor
#define POS_DID_GSNS_Y_FST      0x80000088
//!< \~english Data ID of first time Y axis gsensor
#define POS_DID_GSNS_Z_FST      0x80000089
//!< \~english Data ID of first time Z axis gsensor
#define POS_DID_PULSE_TIME      0x8000003A  //!< \~english Data ID of pulse time

#define POS_DID_GPS__CWORD82__NMEA    0x80000030U
//!< \~english Data ID of _CWORD82_ GPS NMEA sentence
#define POS_DID_GPS__CWORD82___CWORD44_GP4  0x80000031U
//!< \~english Data ID of _CWORD82_ GPS _CWORD44_GP4 data
#define POS_DID_GPS__CWORD82__FULLBINARY 0x80000032U
//!< \~english Data ID of _CWORD82_ GPS full binary data
#define POS_DID_GPS_NMEA        0x8000009AU
//!< \~english Data ID of GPS NMEA sentence
#define POS_DID_GPS_CLOCK_DRIFT 0x800000B3U
//!< \~english Data ID of GPS time drift data
#define POS_DID_GPS_CLOCK_FREQ  0x800000B4U
//!< \~english Data ID of GPS time frequency data

/**
 * \~english @brief  Delivery sensor extra package command ID
 * \~english @brief  If you want to catch above envents, use NSFW like below.
 * \~english @code
 * l_eStatus = FrameworkUnifiedAttachCallbackToDispatcher(h_app, POS_NTFY_SEND_THREAD, CID_POSIF_REGISTER_LISTENER_PKG_SENSOR_DATA, CBCallbackA);
 * @endcode
 */
#define CID_POSIF_REGISTER_LISTENER_PKG_SENSOR_DATA  0x0700

/**
 * \~english @brief  Delivery sensor information command ID
 * \~english @brief  If you want to catch above envents, use NSFW like below.
 * \~english @code
 * l_eStatus = FrameworkUnifiedAttachCallbackToDispatcher(h_app, POS_NTFY_SEND_THREAD, CID_POSIF_REGISTER_LISTENER_SENSOR_DATA, CBCallbackA);
 * @endcode
 */
#define CID_POSIF_REGISTER_LISTENER_SENSOR_DATA      0x0200

#define  SENSOR_MSGBUF_DSIZE    4096
//!< \~english message body maximum size

#define  SENSOR_VSHEAD_DSIZE    36
//!< \~english vehicle sensor header size(1+3+16*2)

#define  SENSOR_VSINFO_DSIZE    (SENSOR_MSGBUF_DSIZE - SENSOR_VSHEAD_DSIZE)
//!< \~english vehicle sensor data size

// Same name/value is defined, but client doesn't include HAL header.
// It defines SENSOR_MSG_VSINFO_DSIZE if not included HAL.
#ifndef HAL_API_POSITIONING_HAL_H_
#define  SENSOR_MSG_VSINFO_DSIZE  1904u
//!< \~english vehicle sensor message body maximum size
#endif

#define  SENSOR_PKG_DELIVERY_MAX  16
//!< \~english number of data ID per a package

/*---------------------------------------------------------------------------------*
 * Typedef declaration                                                             *
 *---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*
 * Struct declaration                                                              *
 *---------------------------------------------------------------------------------*/
/**
 * @struct SENSOR_PKG_MSG_VSINFO
 * \~english positioning sensor notification message (to User)
 */
typedef struct {
  uint8_t  ucDNum;         //!< \~english number of data
  uint8_t  ucDataBreak;    //!< \~english data lack infomation
  uint8_t  ucDivideCnt;    //!< \~english total partition
  uint8_t  ucDivideSendCnt;    //!< \~english partition transmit count
  uint16_t usOffset[SENSOR_PKG_DELIVERY_MAX];    //!< \~english offset
  uint8_t  ucData[SENSOR_VSINFO_DSIZE];    //!< \~english data body
} SENSOR_PKG_MSG_VSINFO;

/**
 * @struct SENSOR_MSG_VSINFO
 * \~english message delivery positioning sensor information
 */
typedef struct {
  DID       did;    //!< \~english data ID
  uint16_t  size;    //!< \~english data size
  uint8_t   rcvFlag;    //!< \~english reception flag
  uint8_t   reserve;    //!< \~english reserve
  uint8_t   data[SENSOR_MSG_VSINFO_DSIZE];    //!< \~english data body
} SENSOR_MSG_VSINFO;

/*---------------------------------------------------------------------------------*
 * Prototype Declaration                                                           *
 *---------------------------------------------------------------------------------*/
/* SENSOR_API public API */
#ifdef __cplusplus
extern "C" {
#endif
////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup tag_Positioning
/// \~english @par Brief
///        - Send the extra package when first delivery.
///
/// \~english @param [in] hApp
///        - HANDLE - App Handle
/// \~english @param [in] notifyName
///        - PCSTR - Destination thread name
/// \~english @param [in] ucPkgNum
///        - uint8_t - data number in package(1 to 16)
/// \~english @param [in] pulDid
///        - DID * - buffer pointer of the data ID array in package
/// \~english @param [in] ucCtrlFlg
///        - uint8_t - Delivery control flag(register)
/// \~english @param [in] ucDeliveryTiming
///        - uint8_t - Delivery timing(change/update)
///
/// \~english @par
///        - data number in package(ucPkgNum) \n
///          The following 8 data ID can be registered. And the register data number range is 1~16.
///        - buffer pointer of the data ID array in package(pulDid) \n
///          The data ID set to the first in pulDid is the delivery key. \n
///          If the data ID not one of the following be set, return SENSOR_RET_ERROR_PARAM.
///        - POS_DID_SNS_COUNTER - sensor counter
///        - POS_DID_GYRO_X - gyro output (X axis)
///        - POS_DID_GYRO_Y - gyro output (Y axis)
///        - POS_DID_GYRO_Z - gyro output (Z axis)
///        - POS_DID_SPEED_PULSE - speed pulse
///        - POS_DID_REV - REV signal(0:forward 1:backward)
///        - POS_DID_GSNS_X - Gsensor output X axis
///        - POS_DID_GSNS_Y - Gsensor output Y axis
///        - POS_DID_GSNS_Z - Gsensor output Z axis
///        - POS_DID_GYRO_TEMP - gyro temperature
///        - POS_DID_PULSE_TIME - pulse time\n
/// \~english @par
///        - Because positioning is G/W between Navi and HAL, value depends on the design of HAL.
/// \~english @par
///        - Note:The Gsensor output is 0 in the environment without Gsensor hardware.\n
/// \~english @par
///        - Delivery control flag(ucCtrlFlg)
///        - SENSOR_DELIVERY_REGIST - register
///        - Register specified LonLat delivery
///        - Please note that if the same data delivery has been registered for multiple times, \n
///          the data will also be deliveried for registered multiple times.
///        - The specified LonLat will be deliveried at register time no matter what delivery  \n
///          timing has been registered (first delivery).
///        - Delivery timing(ucDeliveryTiming)
///        - SENSOR_DELIVERY_TIMING_CHANGE - change  Specified LonLat be deliveried only when it is changed.
///        - SENSOR_DELIVERY_TIMING_UPDATE - update  Specified Lonlat be deliveried as long as \n
///          it is updated by vehicle sensor.
///
///
/// \~english @retval SENSOR_RET_NORMAL normal end
/// \~english @retval SENSOR_RET_ERROR_CREATE_EVENT event create failed
/// \~english @retval SENSOR_RET_ERROR_PARAM parameter error
/// \~english @retval SENSOR_RET_ERROR_INNER internal error
/// \~english @retval SENSOR_RET_ERROR_NOSUPPORT unsupported
/// \~english @retval SENSOR_RET_ERROR_RESOURCE lack of resource
///
/// \~english @par Precondition
///        - The creation/initialization \n
///          (FrameworkunifiedCreateDispatcherWithoutLoop and etc.) of the Dispatcher for App are completed.
///        - Availability of service positioning is TRUE.
///
/// \~english @par change of internal status
///        - There is no change of internal status
///
/// \~english @par Failure condition
///        - The parameter hApp is NULL [SENSOR_RET_ERROR_PARAM]
///        - The parameter ucCtrlFlg is not register(SENSOR_DELIVERY_REGIST) [SENSOR_RET_ERROR_PARAM]
///        - The parameter ucDeliveryTiming is neither update(SENSOR_DELIVERY_TIMING_UPDATE) \n
///          nor change(SENSOR_DELIVERY_TIMING_CHANGE) [SENSOR_RET_ERROR_PARAM]
///        - The parameter notifyName is NULL [SENSOR_RET_ERROR_PARAM]
///        - The parameter ucPkgNum is 0 or it is larger than 16 [SENSOR_RET_ERROR_PARAM]
///        - The parameter pulDid is NULL [SENSOR_RET_ERROR_PARAM]
///        - The data ID in paramter buffer pulDid is not avaliable value [SENSOR_RET_ERROR_PARAM]
///        - The count of message in message queue is reach to max [SENSOR_RET_ERROR_RESOURCE]
///        - The count of mutex is reach to max [SENSOR_RET_ERROR_RESOURCE]
///        - The count of item in ProcessName-ProcessNo convert table is reach to max [SENSOR_RET_ERROR_RESOURCE]
///        - The event has been registered in event table and created in same process, \n
///          but the count of reference is reach to max [SENSOR_RET_ERROR_CREATE_EVENT]
///        - The event has been registered in event table and created in system, but the \n
///          count of reference is reach to max  [SENSOR_RET_ERROR_CREATE_EVENT]
///        - The event has been registered in event table, but memory for the thread event table \n
///          creation allocate falied. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - The event has been registered in event table, but the thread can not be registered in \n
///          event table. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - The event has been registered in event table, but memory for thread table creation \n
///          allocate falied. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - The event has been registered in event table, and the event flag has already been \n
///          registered. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - The event has been registered in event table, and the event flag register failed. \n
///          [SENSOR_RET_ERROR_CREATE_EVENT]
///        - No empty field for registering the event HANDLE into event table. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - Memory for event HANDLE get failed. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - The event has not been registered in event table and not finished registering \n
///          [SENSOR_RET_ERROR_CREATE_EVENT]
///        - The event has not been registered in event table, and memory for thread table \n
///          creation allocate falied. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - The event has not been registered in event table, and the event flag has already been \n
///          registered. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - The event has not been registered in event table, and the event flag register failed. \n
///          [SENSOR_RET_ERROR_CREATE_EVENT]
///        - ProcessNo has not been registered in message control table when message transfered \n
///          between processes. [SENSOR_RET_ERROR_INNER]
///        - Message transfer HANDLE get failed when message transfered in process. [SENSOR_RET_ERROR_INNER]
///        - Message transfer failed in process. [SENSOR_RET_ERROR_INNER]
///        - The destination process name is NULL. [SENSOR_RET_ERROR_INNER]
///        - The destination process name size is larger than 20 characters when message transfer \n
///          between processes. [SENSOR_RET_ERROR_INNER]
///        - The message queue name has not been registered in control table when message \n
///          transfer between processes. [SENSOR_RET_ERROR_INNER]
///        - Message transfer HANDLE create failed when message transfered between processes. [SENSOR_RET_ERROR_INNER]
///        - Message transfer HANDLE get failed from internal table when message transfered \n
///          between processes. [SENSOR_RET_ERROR_INNER]
///        - Message transfer failed between processes. [SENSOR_RET_ERROR_INNER]
///        - Specified event HANDLE has not been registered in event HANDLE table during event \n
///          waiting. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - The count of thread is reach to max in event management table during event \n
///          waiting. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - The EV_FLAG_BIT is not set in flagID during event getting. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - The ID of message event queue has not been created during event getting. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - The flagID has not been registered during event getting. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - The interruption happened during event getting [SENSOR_RET_ERROR_CREATE_EVENT]
///        - Whatever error happened during event getting. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - Got event is SENSOR_RET_ERROR_INNER. [SENSOR_RET_ERROR_INNER]
///
/// \~english @par Detial
///        - Call this API to register vehicle sensor data delivery. \n
///          This API return the result of registering. \n
///          The data from sensor data received to registering will be deliveried. \n
///          The first delivery data is the sensor data(max 64 number of sensor counter, \n
///          reverse signal,gyro temperature, max 640 number of gyro output(X axis), gyro output(X axis), gyro output(Z axis), \n
///          speed pulse,Gsensor output(X axis), Gsensor output(Y axis), Gsensor output(Z axis), max 2048 number of pulse time) in 6.4 second. \n
///          If the data number is more than max number, delivery the data in newest 6.4 second. \n
///          If the sensor data accumulated more than max number, set VEHICLE_SNS_BREAK to data missing information. \n
///          If sensor data number is more than the data number send in one time(10 number of \n
///          sensor counter,reverse signal,gyro temperature, 100 number of gyro output(X axis), gyro output(X axis), gyro output(Z axis), \n
///          speed pulse, Gsensor output(X axis),Gsensor output(Y axis), Gsensor output(Z axis), 320 number of pulse time), \n
///          the old data is divided into partitions(every partition with 10 number of sensor counter, \n
///          reverse signal,gyro temperature, 100 number of gyro output(X axis), gyro output(X axis), gyro output(Z axis), \n
///          speed pulse,Gsensor output(X axis), Gsensor output(Y axis), Gsensor output(Z axis), 320 number of pulse time) to delivery. \n
///          The last message for first delivery is the message that the partition count equal to partition No. \n
///          After first delivery, the message data(1 number of sensor counter,reverse signal, \n
///          gyro temperature, 10 number of gyro output(X axis), gyro output(X axis), gyro output(Z axis),speed pulse, \n
///          Gsensor output(X axis), Gsensor output(Y axis), Gsensor output(Z axis), 32 number of pulse time) deliveried. \n
///          And because the data missing information, divided partition count, \n
///          diveided partition No is not used, they will be set to 0 in message.\n
///         (sample)The sensor data in 6.4 second divided to delivery
///        - 1st message(sensor counter, reverse signal, gyro temperature=10 number, gyro output(X axis), gyro output(Y axis), gyro output(Z axis), speed pulse, \n
///          Gsensor output(X axis), Gsensor output(Y axis), Gsensor output(Z axis)=100 number, pulse time=320 number), divided  \n
///          partition count=7, divided partition No=1)
///        - 2nd message(sensor counter, reverse signal, gyro temperature=10 number, gyro output(X axis), gyro output(Y axis), gyro output(Z axis), speed pulse, \n
///          Gsensor output(X axis), Gsensor output(Y axis), Gsensor output(Z axis)=100 number, pulse time=320 number), divided  \n
///          partition count=7, divided partition No=2)
///        - 3rd message(sensor counter, reverse signal, gyro temperature=10 number, gyro output(X axis), gyro output(Y axis), gyro output(Z axis), speed pulse, \n
///          Gsensor output(X axis), Gsensor output(Y axis), Gsensor output(Z axis)=100 number, pulse time=320 number), divided  \n
///          partition count=7,divided partition No=3)
///        - 4th message(sensor counter, reverse signal, gyro temperature=10 number, gyro output(X axis), gyro output(Y axis), gyro output(Z axis), speed pulse, \n
///          Gsensor output(X axis), Gsensor output(Y axis), Gsensor output(Z axis)=100 number, pulse time=320 number), divided  \n
///          partition count=7,divided partition No=4)
///        - 5th message(sensor counter, reverse signal, gyro temperature=10 number, gyro output(X axis), gyro output(Y axis), gyro output(Z axis), speed pulse, \n
///          Gsensor output(X axis), Gsensor output(Y axis), Gsensor output(Z axis)=100 number, pulse time=320 number), divided  \n
///          partition count=7,divided partition No=5)
///        - 6th message(sensor counter, reverse signal, gyro temperature=10 number, gyro output(X axis), gyro output(Y axis), gyro output(Z axis), speed pulse, \n
///          Gsensor output(X axis), Gsensor output(Y axis), Gsensor output(Z axis)=100 number, pulse time=320 number), divided  \n
///          partition count=7,divided partition No=6)
///        - 7th message(sensor counter, reverse signal, gyro temperature=4 number, gyro output(X axis), gyro output(Y axis), gyro output(Z axis), speed pulse, \n
///          Gsensor output(X axis), Gsensor output(Y axis), Gsensor output(Z axis)=40 number, pulse time=128 number), divided  \n
///          partition count=7,divided partition No=7)
///
/// \~english @par
///        - Please note the following points when use this API.
///        - Duplication registering
///        - The same destination thread name has already been registered
///        - The registered delivery data updated and normal return.(first delivery)
///        - To one delivery destination, the same data will not be duplication deliveried at same timing.
///        - After delivery the sensor data accumulated in 6.4 second(first delivery), the sensor data \n
///          will not be accumulated any more. So the same data will be deliveried as first \n
///          delivery when registered again.
///        - This API is only called by Navi proxy.
///        - After call this API, if the delivery destination thread name is changed, please call this API again.
///
/// \~english @par
///          message structure
///        - After success to register, vehicle sensor will send message as system API message with following format.
///          Command ID : @ref CID_POSIF_REGISTER_LISTENER_PKG_SENSOR_DATA \n
///          Definition of structure
/// \~english @code
///  #define  SENSOR_MSGBUF_DSIZE        4096  /* max size of message body */
///  #define  SENSOR_VSHEAD_DSIZE        36  /* vehicle sensor header size(1+3+16*2) */
///  #define  SENSOR_PKG_DELIVERY_MAX        16  /* max number of Data ID in package */
///  #define  SENSOR_VSINFO_DSIZE    (SENSOR_MSGBUF_DSIZE - SENSOR_VSHEAD_DSIZE)
///  typedef struct {
///    uint8_t    ucDNum;          /* number of data */
///    uint8_t    ucDataBreak;        /* data missing information */
///    uint8_t    ucDivideCnt;        /* divided partition count */
///    uint8_t    ucDivideSendCnt;      /* divided partition No. */
///    uint16_t  usOffset[SENSOR_PKG_DELIVERY_MAX];  /* offset */
///    uint8_t    ucData[SENSOR_VSINFO_DSIZE];    /* vehicle sensor data */
///  } SENSOR_PKG_MSG_VSINFO;
/// @endcode
///        - number of data \n
///          Data number in package
///        - data missing information \n
///          VEHICLE_SNS_BREAK:not continuous data \n
///          VEHICLE_SNS_NORMAL:continuous data
///        - divided partition count \n
///          All divided partition count \n
///          If it is more than 1, data divided to delivery
///        - divided partition No. \n
///          The No. of the divided partition. If it equal to the divided partition count, \n
///          that meanings this message is the last divided partition of the package.
///        - offset \n
///          The array of the offset from the head of vehicle sensor data
///        - vehicle sensor data
///        - Data ID(4Byte)
///        - Data size(2Byte)
///        - reception flag(1Byte)
///        - If the data get from CAN, direct line, GPS, set to 0x01
///        - If data has not been received, set to 0x00
///        - reserve(1Byte)\n
///        - Data body is cycle fit with the packaged data.
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Method
///
/// \~english @see
///        - POS_RegisterListenerSensData, POS_GetSensData
///
////////////////////////////////////////////////////////////////////////////////////////////
SENSOR_RET_API  POS_RegisterListenerPkgSensData(HANDLE hApp, PCSTR  notifyName, uint8_t  ucPkgNum, DID    *pulDid,
                                            uint8_t  ucCtrlFlg, uint8_t  ucDeliveryTiming);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup tag_Positioning
/// \~english @par Brief
///        - Register sensor data delivery.
///
/// \~english @param [in] hApp
///        - HANDLE - App Handle
/// \~english @param [in] notifyName
///        - PCSTR - Destination thread name
/// \~english @param [in] ulDid
///        - DID - Data ID of vehicle info
/// \~english @param [in] ucCtrlFlg
///        - uint8_t - Delivery control flag(register)
/// \~english @param [in] ucDeliveryTiming
///        - uint8_t - Delivery timing(change/update)
///
/// \~english @par
///        - Data ID of vehicle info(ulDid) \n
///
/// \~english @par
///        - Note:The Gsensor output is 0 in the environment without Gsensor hardware.
/// \~english @par
///        - Delivery control flag(ucCtrlFlg)
///        - SENSOR_DELIVERY_REGIST - register
///        - Register specified LonLat delivery
///        - Please note that if the same data delivery has been registered for multiple times, \n
///          the data will also be deliveried for registered multiple times.
///        - The specified LonLat will be deliveried at register time no matter what delivery  \n
///          timing has been registered (first delivery).
///        - Delivery timing(ucDeliveryTiming)
///        - SENSOR_DELIVERY_TIMING_CHANGE - change  Specified LonLat be deliveried only when it is changed.
///        - SENSOR_DELIVERY_TIMING_UPDATE - update  Specified Lonlat be deliveried  \n
///          as long as it is updated by vehicle sensor.
///
/// \~english @retval SENSOR_RET_NORMAL normal end
/// \~english @retval SENSOR_RET_ERROR_CREATE_EVENT event create failed
/// \~english @retval SENSOR_RET_ERROR_PARAM parameter error
/// \~english @retval SENSOR_RET_ERROR_INNER internal error
/// \~english @retval SENSOR_RET_ERROR_NOSUPPORT unsupported
/// \~english @retval SENSOR_RET_ERROR_RESOURCE lack of resource
///
/// \~english @par Precondition
///        - The creation/initialization(FrameworkunifiedCreateDispatcherWithoutLoop and etc.) \n
///          of the Dispatcher for App are completed.
///        - Availability of service positioning is TRUE.
///
/// \~english @par change of internal status
///        - There is no change of internal status
///
/// \~english @par Failure condition
///        - The parameter ucDeliveryTiming is neither update \n
///          (SENSOR_DELIVERY_TIMING_UPDATE) nor change(SENSOR_DELIVERY_TIMING_CHANGE) [SENSOR_RET_ERROR_PARAM]
///        - The parameter ucCtrlFlg is not register(SENSOR_DELIVERY_REGIST) [SENSOR_RET_ERROR_PARAM]
///        - The parameter hApp is NULL [SENSOR_RET_ERROR_PARAM]
///        - The parameter notifyName is NULL [SENSOR_RET_ERROR_PARAM]
///        - The parameter ulDID is not avaliable value [SENSOR_RET_ERROR_PARAM]
///        - The parameter ulDID is a value can not specified [SENSOR_RET_ERROR_PARAM]
///        - The count of message in message queue is reach to max [SENSOR_RET_ERROR_RESOURCE]
///        - The count of mutex is reach to max [SENSOR_RET_ERROR_RESOURCE]
///        - The count of item in ProcessName-ProcessNo convert table is reach to \n
///          max [SENSOR_RET_ERROR_RESOURCE]
///        - The event has been registered in event table and created in same process, \n
///          but the count of reference is reach to max [SENSOR_RET_ERROR_CREATE_EVENT]
///        - The event has been registered in event table and created in system, but the count \n
///          of reference is reach to max  [SENSOR_RET_ERROR_CREATE_EVENT]
///        - The event has been registered in event table, but memory for the thread event table \n
///          creation allocate falied. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - The event has been registered in event table, but the thread can not be registered \n
///          in event table. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - The event has been registered in event table, but memory for thread table creation \n
///          allocate falied. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - The event has been registered in event table, and the event flag has already been \n
///          registered. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - The event has been registered in event table, and the event flag register failed. \n
///          [SENSOR_RET_ERROR_CREATE_EVENT]
///        - No empty field for registering the event HANDLE into event table. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - Memory for event HANDLE get failed. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - The event has not been registered in event table and not finished registering \n
///          [SENSOR_RET_ERROR_CREATE_EVENT]
///        - The event has not been registered in event table, and memory for thread table \n
///          creation allocate falied. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - The event has not been registered in event table, and the event flag has already \n
///          been registered. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - The event has not been registered in event table, and the event flag register \n
///          failed. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - ProcessNo has not been registered in message control table when message transfered \n
///          in process. [SENSOR_RET_ERROR_INNER]
///        - Message transfer HANDLE get failed when message transfered in process. [SENSOR_RET_ERROR_INNER]
///        - Message transfer failed in process. [SENSOR_RET_ERROR_INNER]
///        - The destination process name is NULL. [SENSOR_RET_ERROR_INNER]
///        - The destination process name size is larger than 20 characters when message \n
///          transfer between process. [SENSOR_RET_ERROR_INNER]
///        - The message queue name has not been registered in control table when message \n
///          transfer between process. [SENSOR_RET_ERROR_INNER]
///        - Message transfer HANDLE create failed when message transfered between process. [SENSOR_RET_ERROR_INNER]
///        - Message transfer HANDLE get failed from internal table when message transfered \n
///          between process. [SENSOR_RET_ERROR_INNER]
///        - Message transfer failed between process. [SENSOR_RET_ERROR_INNER]
///        - Specified event HANDLE has not been registered in event HANDLE table during waiting \n
///          event. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - The count of thread is reach to max in event management table during waiting \n
///          event. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - The EV_FLAG_BIT is not set in flagID during getting event. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - The ID of message event queue has not been created during getting event. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - The flagID has not been registered during getting event. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - The interruption happened during getting event [SENSOR_RET_ERROR_CREATE_EVENT]
///        - Whatever error happened during getting event. [SENSOR_RET_ERROR_CREATE_EVENT]
///
/// \~english @par Detial
///        - Call this API to register vehicle sensor data delivery. \n
///          This API return the result of registering.
///
/// \~english @par
///          Please note the following points when use this API.
///        - Duplication registering
///        - The same destination thread name has already been registered
///        - The registered delivery data updated and normal return.(first delivery)
///        - To one delivery destination, the same data will not be duplication deliveried at same timing.
///        - After call this API, if the delivery destination thread name is changed, please call this API again.
///
/// \~english @par
///        message structure
///        - After success to register, vehicle sensor will send message as system API message with following format.
///        - If the register successed, certainly delivery first data. And then delivery data according to
///          the delivery timing. \n
///          Command ID : @ref CID_POSIF_REGISTER_LISTENER_SENSOR_DATA \n
///
/// \~english @code
///     #define  SENSOR_MSG_VSINFO_DSIZE  1904      /* max size of message body */
///     typedef struct
///     {
///       DID       did;         /* data ID */
///       uint16_t  size;        /* data size of vehicle sensor data */
///       uint8_t   rcvFlag;     /* reception flag */
///       uint8_t   reserve;     /* reserve */
///       uint8_t   data[SENSOR_MSG_VSINFO_DSIZE];  /* vehicle sensor data */
///     } SENSOR_MSG_VSINFO;
/// @endcode
///        - reception flag(1Byte)
///        - If the data get from CAN or direct line, set to 0x01
///        - If data has not been received, set to 0x00
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Method
///
/// \~english @see
///        - POS_RegisterListenerPkgSensData, POS_GetSensData
///
////////////////////////////////////////////////////////////////////////////////////////////
SENSOR_RET_API  POS_RegisterListenerSensData(HANDLE  hApp, PCSTR  notifyName, DID ulDid, uint8_t ucCtrlFlg,
                                            uint8_t  ucDeliveryTiming);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup tag_Positioning
/// \~english @par Brief
///        - Get vehicle sensor data.
///
/// \~english @param [in] hApp
///        - HANDLE - App Handle
/// \~english @param [in] ulDid
///        - DID - Data ID of vehicle info
/// \~english @param [out] pDestData
///        - void* - pointer of buffer for storing vehicle sensor data
/// \~english @param [in] usDestSize
///        - uint16_t - vehicle sensor data buffer size
///
/// \~english @par
///        - Data ID of vehicle info(ulDid)
///        - POS_DID_SPEED_PULSE - speed pulse(count of pulse)
///        - POS_DID_GYRO_X - gyro output (X axis)
///        - POS_DID_GYRO_Y - gyro output (Y axis)
///        - POS_DID_GYRO_Z - gyro output (Z axis)
///        - POS_DID_GSNS_X - Gsensor output (X axis)
///        - POS_DID_GSNS_Y - Gsensor output (Y axis)
///        - POS_DID_GSNS_Z - Gsensor output (Z axis)
///        - POS_DID_GPS_ANTENNA - GPS antenna connection status
///        - POS_DID_GPS__CWORD82__NMEA - GPS NMEA(_CWORD82_)
///        - POS_DID_GPS__CWORD82__FULLBINARY - GPS _CWORD82_ full binary(_CWORD82_)
///        - POS_DID_GPS_NMEA - GPS NMEA
///        - POS_DID_GYRO_TEMP - gyro temperature
///        - POS_DID_GPS_CLOCK_DRIFT - GPS clock drift
///        - POS_DID_GPS_CLOCK_FREQ - GPS clock frequency
///        - The avaliable data ID of each hardware type is as following.
/// \~english @par
///        - Because positioning is G/W between Navi and HAL, value depends on the design of HAL.
/// \~english @par
///        - Note:The Gsensor output is 0 in the environment without Gsensor hardware.
///        - vehicle sensor data buffer size(usDestSize) \n
///          Please note it is the size of output buffer, not the size of data.
///
/// \~english @retval more than 0  data size
/// \~english @retval POS_RET_ERROR_CREATE_EVENT event create failed
/// \~english @retval POS_RET_ERROR_OUTOF_MEMORY share memory guarantee failed
/// \~english @retval POS_RET_ERROR_PARAM parameter error
/// \~english @retval POS_RET_ERROR_SIZE buffer size error
/// \~english @retval POS_RET_ERROR_INNER internal error
/// \~english @retval POS_RET_ERROR_NOSUPPORT unsupported
/// \~english @retval POS_RET_ERROR_RESOURCE lack of resource
///
/// \~english @par Precondition
///        - The creation/initialization(FrameworkunifiedCreateDispatcherWithoutLoop and etc.) \n
///          of the Dispatcher for App are completed.
///        - Availability of service positioning is TRUE.
///
/// \~english @par change of internal status
///        - There is no change of internal status
///
/// \~english @par Failure condition
///        - The parameter hApp is NULL [POS_RET_ERROR_PARAM]
///        - The parameter pDestData is NULL [POS_RET_ERROR_PARAM]
///        - The parameter ulDid is not avaliable value in current hardware environment [POS_RET_ERROR_PARAM]
///        - The count of message in message queue is reach to max [POS_RET_ERROR_RESOURCE]
///        - The count of mutex is reach to max [POS_RET_ERROR_RESOURCE]
///        - The count of item in ProcessName-ProcessNo convert table is reach to max [POS_RET_ERROR_RESOURCE]
///        - The event is created in same process, but the count of reference is reach to max \n
///          [POS_RET_ERROR_CREATE_EVENT]
///        - The event is created in system, but the count of reference is reach to max  [POS_RET_ERROR_CREATE_EVENT]
///        - Memory allocate falied during the event table creation for \n
///          event registering. [POS_RET_ERROR_CREATE_EVENT]
///        - The thread can not register in the event table. [POS_RET_ERROR_CREATE_EVENT]
///        - Memory allocate failed in event table during the thread table creation. [POS_RET_ERROR_CREATE_EVENT]
///        - After register the thread table in event table, the event flag has already \n
///          been registered. [POS_RET_ERROR_CREATE_EVENT]
///        - After register the thread table in event table, the event flag register \n
///          failed. [POS_RET_ERROR_CREATE_EVENT]
///        - The event table is full during event creation. [POS_RET_ERROR_CREATE_EVENT]
///        - The memory for event HANDLE allocate failed during event table creation. [POS_RET_ERROR_CREATE_EVENT]
///        - The thread can not be registered in event table. [POS_RET_ERROR_CREATE_EVENT]
///        - In event table, to allocate the memory of the thread table creation, \n
///          but failed. [POS_RET_ERROR_CREATE_EVENT]
///        - After register the thread in event table, the event flag has already been \n
///          registered. [POS_RET_ERROR_CREATE_EVENT]
///        - After register the thread in event table, the event flag register failed. [POS_RET_ERROR_CREATE_EVENT]
///        - Specified event ID has not been registered in table. [POS_RET_ERROR_CREATE_EVENT]
///        - Initialize event object failed. [POS_RET_ERROR_CREATE_EVENT]
///        - There is no empty field in semaphore table for semaphore creation [POS_RET_ERROR_OUTOF_MEMORY]
///        - The memory for storing semaphore control data allocate failed. [POS_RET_ERROR_OUTOF_MEMORY]
///        - Specified semaphore ID has not been registered when semaphore lock. [POS_RET_ERROR_OUTOF_MEMORY]
///        - Internal mutex HANDLE is NULL when mutex lock. [POS_RET_ERROR_OUTOF_MEMORY]
///        - Internal mutex HANDLE has not been registered in mutex table when mutex lock. [POS_RET_ERROR_OUTOF_MEMORY]
///        - The owner of specified mutex is not itself when mutex lock. [POS_RET_ERROR_OUTOF_MEMORY]
///        - Mutex has been multiple locked [POS_RET_ERROR_OUTOF_MEMORY]
///        - The HANDLE is NULL when getting usable share memory address. [POS_RET_ERROR_OUTOF_MEMORY]
///        - Can not get usable share memory address. [POS_RET_ERROR_OUTOF_MEMORY]
///        - Memory allocate failed for share memory map. [POS_RET_ERROR_OUTOF_MEMORY]
///        - Memory allocate failed for share memory management. [POS_RET_ERROR_OUTOF_MEMORY]
///        - Can not open share memory. [POS_RET_ERROR_OUTOF_MEMORY]
///        - Failed to mapping share memory. [POS_RET_ERROR_OUTOF_MEMORY]
///        - No empty field in share memory. [POS_RET_ERROR_OUTOF_MEMORY]
///        - ProcessNo has not been registered in message control table when message \n
///          transfered between processes. [POS_RET_ERROR_CREATE_EVENT]
///        - Message transfer HANDLE get failed when message transfered between processes. [POS_RET_ERROR_CREATE_EVENT]
///        - Message transfer failed between processes. [POS_RET_ERROR_CREATE_EVENT]
///        - The destination process name size is larger than 20 characters when message \n
///          transfer between processes. [POS_RET_ERROR_CREATE_EVENT]
///        - The message queue name has not been registered in control table when message \n
///          transfer between processes. [POS_RET_ERROR_CREATE_EVENT]
///        - Message transfer HANDLE create failed when message transfered between processes. \n
///          [POS_RET_ERROR_CREATE_EVENT]
///        - Message transfer HANDLE get failed from internal table when message transfered \n
///          between processes. [POS_RET_ERROR_CREATE_EVENT]
///        - Message transfer failed between processes. [POS_RET_ERROR_CREATE_EVENT]
///        - Specified event HANDLE has not been registered in event HANDLE table during \n
///          event waiting. [POS_RET_ERROR_INNER]
///        - The count of thread is reach to max in event management table during event waiting. [POS_RET_ERROR_INNER]
///        - The EV_FLAG_BIT is not set in flagID during event waiting. [POS_RET_ERROR_INNER]
///        - The ID of message event queue has not been created during event waiting. [POS_RET_ERROR_INNER]
///        - The flagID has not been registered during event waiting. [POS_RET_ERROR_INNER]
///        - The interruption happened during event waiting [POS_RET_ERROR_INNER]
///        - Whatever error happened during event waiting. [POS_RET_ERROR_INNER]
///        - The EV_FLAG_BIT is not set in flagID during getting event. [POS_RET_ERROR_INNER]
///        - The ID of message event queue has not been created during getting event. [POS_RET_ERROR_INNER]
///        - The flagID has not been registered during getting event. [POS_RET_ERROR_INNER]
///        - The interruption happened during getting event [POS_RET_ERROR_INNER]
///        - Whatever error happened during getting event. [POS_RET_ERROR_INNER]
///        - The HANDLE is NULL when getting usable share memory address for accessing \n
///          received data. [POS_RET_ERROR_OUTOF_MEMORY]
///        - Can not get usable share memory address for accessing received data. [POS_RET_ERROR_OUTOF_MEMORY]
///        - Memory for share memory map allocate failed for accessing received data. [POS_RET_ERROR_OUTOF_MEMORY]
///        - Memory for share memory management allocate failed for accessing received \n
///          data. [POS_RET_ERROR_OUTOF_MEMORY]
///        - Can not open share memory for accessing received data. [POS_RET_ERROR_OUTOF_MEMORY]
///        - Failed to mapping share memory for accessing received data. [POS_RET_ERROR_OUTOF_MEMORY]
///        - Can not get share memory normally [POS_RET_ERROR_OUTOF_MEMORY]
///        - The size of data stored in share memory is larger than the size of received data. [POS_RET_ERROR_SIZE]
///
/// \~english @par Detial
///        - Call this API to get vehicle sensor data. \n
///          This vehicle sensor data stored in the output buffer of the parameter, this API return.
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Sync
///
/// \~english @see
///        - POS_RegisterListenerPkgSensData, POS_RegisterListenerSensData
///

POS_RET_API POS_GetSensData(HANDLE hApp, DID ulDid, void *pDestData, uint16_t usDestSize);

#ifdef __cplusplus
}
#endif
/** @}*/  // end of positioning
/** @}*/  // end of vehicle_service
/** @}*/  // end of BaseSystem
#endif  // POSITIONING_CLIENT_INCLUDE_VEHICLE_SERVICE_POS_SENSOR_API_H_
