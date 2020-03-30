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
#ifndef POSITIONING_CLIENT_INCLUDE_VEHICLE_SERVICE_POS_COMMON_API_H_
#define POSITIONING_CLIENT_INCLUDE_VEHICLE_SERVICE_POS_COMMON_API_H_

/**
 * @file POS_common_API.h
 * @brief API definition file for common function
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
//#include <gps_hal.h>

/*---------------------------------------------------------------------------------*
 * Definition                                                                      *
 *---------------------------------------------------------------------------------*/
/* State definitions can be used */
#define  SENSORLOCATION_STATUS_DISABLE  (0)  //!< \~english Not available
#define  SENSORLOCATION_STATUS_ENABLE   (1)  //!< \~english Available

/* State definitions can be used */
#define  SENSORMOTION_STATUS_DISABLE    (0)  //!< \~english Not available
#define  SENSORMOTION_STATUS_ENABLE     (1)  //!< \~english Available
/* Acquisition method */
#define  SENSOR_GET_METHOD_AUTO         (0)  //!< \~english Not specified
#define  SENSOR_GET_METHOD_GPS          (1)  //!< \~english GPS
#define  SENSOR_GET_METHOD_NAVI         (2)  //!< \~english Navigation
#define  SENSOR_GET_METHOD_DR           (3)  //!< \~english Dead Reckoning
#define  SENSOR_GET_METHOD_POS          (4)  //!< \~english positioning (Internal)


#define  LOCATIONINFO_NMEA_MAX         (1020)
//!< \~english Max length of 'Location Information'

/* Command ID */
/* Vehicle sensor information notification CID (Rate information) */
#define CID_POSIF_REGISTER_LISTENER_SPEED     (0x0203)  //!< \~english Delivery speed
#define CID_POSIF_REGISTER_LISTENER_LONLAT    0x0781
//!< \~english Delivery longitude and latitude
#define CID_POSIF_REGISTER_LISTENER_ALTITUDE  0x0782  //!< \~english Delivery altitude
#define CID_POSIF_REGISTER_LISTENER_HEADING   0x0783  //!< \~english Delivery heading

/*---------------------------------------------------------------------------------*
 * Typedef declaration                                                             *
 *---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*
 * Struct declaration                                                              *
 *---------------------------------------------------------------------------------*/
/**
 * @struct POS_POSDATA
 * \~english position information
 */
typedef struct {
  int8_t    status;       //!< \~english data status
  uint8_t   posSts;       //!< \~english position status
  uint16_t  posAcc;       //!< \~english position accuracy
  int32_t   longitude;    //!< \~english current longitude
  int32_t   latitude;     //!< \~english current latitude
  int32_t   altitude;     //!< \~english current altitude
  int16_t   heading;      //!< \~english current heading
  uint8_t   reserved[2];  //!< \~english reserve
} POS_POSDATA;

/**
 * @struct SENSORLOCATION_MSG_LONLATINFO
 * \~english longitude and latitude information data delivery message
 */
typedef struct {
//  SENSORLOCATION_LONLATINFO_DAT data;
  //!< \~english longitude and latitude information data
} SENSORLOCATION_MSG_LONLATINFO;

/**
 * @struct SENSORLOCATION_MSG_ALTITUDEINFO
 * \~english altitude information data delivery message
 */
typedef struct {
//  SENSORLOCATION_ALTITUDEINFO_DAT data;  //!< \~english altitude information data
} SENSORLOCATION_MSG_ALTITUDEINFO;

/**
 * @struct SENSORMOTION_MSG_HEADINGINFO
 * \~english heading information data delivery message
 */
typedef struct {
//  SENSORMOTION_HEADINGINFO_DAT data;  //!< \~english heading information data
} SENSORMOTION_MSG_HEADINGINFO;

/**
 * @struct SENSORMOTION_MSG_SPEEDINFO
 * \~english speed information data delivery message
 */
typedef struct {
//  SENSORMOTION_SPEEDINFO_DAT data;  //!< \~english speed information data
} SENSORMOTION_MSG_SPEEDINFO;


/**
 * @struct POS_LOCATIONINFO_NMEA
 * \~english Structure of Location Information (NMEA)
 */
typedef struct {
    uint32_t length;  //!< \~english length
    uint8_t  data[LOCATIONINFO_NMEA_MAX];  //!< \~english data
} POS_LOCATIONINFO_NMEA;


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
///     - Register for longitude and lattitude delivery
///
/// \~english @param [in] hApp
///     - HANDLE - App Handle
/// \~english @param [in] notifyName
///     - PCSTR - Destination thread name
/// \~english @param [in] ucCtrlFlg
///     - uint8_t - Delivery control flag(register)
/// \~english @param [in] ucDeliveryTiming
///     - uint8_t - Delivery timing(change/update)
/// \~english @param [in] ucGetMethod
///     - uint8_t - Get method(GPS/Navigation/Not specified)
///
/// \~english @par
///     - Delivery control flag(ucCtrlFlg)
///     - SENSOR_DELIVERY_REGIST - register
///     - Register for specified logtitude and latitude delivery
///     - Please note if call this function for multiple times, the data will be deliveried for multiple times.
///     - The specified longitude and latitude will be deliveried during the registration(first delivery)
///     - Delivery timing(ucDeliveryTiming)
///     - SENSOR_DELIVERY_TIMING_CHANGE - change. Specified longitude and latitude will be deliveried \n
///       only when it has been changed.
///     - SENSOR_DELIVERY_TIMING_UPDATE - update. Specified longitude and latitude will be deliveried \n
///       every time that has been updated by the vehicle sensors.
///     - Get method(ucGetMethod)
///     - SENSOR_GET_METHOD_GPS - GPS. The longitude and latitude from GPS will be deliveried.
///     - SENSOR_GET_METHOD_NAVI - Navi. The longitude and latitude from Navigation will be deliveried.
///     - SENSOR_GET_METHOD_AUTO - Not specified. The specified longitude and latitude will be deliveried \n
///       according to the current environment.
///     - Avaliable method is descriped as following:\n
///       In the following environment, if the SENSOR_GET_METHOD_AUTO is set,treated as default \n
///       In the following environment, if the unsupported method has been specified, \n
///       POS_RET_ERROR_PARAM will be returned.\n
///
/// \~english @retval POS_RET_NORMAL normal finish
/// \~english @retval POS_RET_ERROR_PARAM parameter error
/// \~english @retval POS_RET_ERROR_BUFFULL the register count is full
/// \~english @retval POS_RET_ERROR_INNER internal error
/// \~english @retval POS_RET_ERROR_NOSUPPORT unsupported
/// \~english @retval POS_RET_ERROR_RESOURCE lack of resource
///
/// \~english @par Precondition
///     - The creation/initialization(FrameworkunifiedCreateDispatcherWithoutLoop and etc.) of the dispatcher for App are completed.
///     - Availability of positioning service is TRUE.
///
/// \~english @par changes of the internal status
///     - There is no changes of the internal status by this API
///
/// \~english @par Failure condition
///     - The parameter ucDeliveryTiming is neither update(SENSOR_DELIVERY_TIMING_UPDATE) nor change \n
///       (SENSOR_DELIVERY_TIMING_CHANGE) [POS_RET_ERROR_PARAM]
///     - The parameter ucCtrlFlg is not register(SENSOR_DELIVERY_REGIST) [POS_RET_ERROR_PARAM]
///     - The parameter hApp is NULL [POS_RET_ERROR_PARAM]
///     - The parameter notifyName is NULL [POS_RET_ERROR_PARAM]
///     - The count of message in message queue is reach to the max [POS_RET_ERROR_RESOURCE]
///     - The count of mutex is reach to the max [POS_RET_ERROR_RESOURCE]
///     - The count of item in ProcessName-ProcessNo convert table is reach to the max [POS_RET_ERROR_RESOURCE]
///     - The event is created in same process, but the count of reference to the event \n
///       is reach to the max [POS_RET_ERROR_INNER]
///     - The event is created in system, but the count of reference to the event \n
///       is reach to the max  [POS_RET_ERROR_INNER]
///     - Memory allocate failed during the event table creation for the event registering. [POS_RET_ERROR_INNER]
///     - The thread can not register in the event table. [POS_RET_ERROR_INNER]
///     - Memory allocate failed in event table during the thread table creation. [POS_RET_ERROR_INNER]
///     - After register the thread table in event table, the event flag has already been registered. \n
///       [POS_RET_ERROR_INNER]
///     - After register the thread table in event table, the event flag register failed. [POS_RET_ERROR_INNER]
///     - The event table is full. [POS_RET_ERROR_INNER]
///     - The memory for event HANDLE allocate failed. [POS_RET_ERROR_INNER]
///     - The thread can not be registered in event table. [POS_RET_ERROR_INNER]
///     - In event table, try to allocate the memory of the thread table creation, but failed. [POS_RET_ERROR_INNER]
///     - After register the thread in event table, the event flag has already been registered. [POS_RET_ERROR_INNER]
///     - After register the thread in event table, the event flag register failed. [POS_RET_ERROR_INNER]
///     - Specified event ID has not been registered in table. [POS_RET_ERROR_INNER]
///     - Initialize event object failed. [POS_RET_ERROR_INNER]
///     - ProcessNo has not been registered in message control table when message transfered between processes.\n
///       [POS_RET_ERROR_INNER]
///     - Message transfer HANDLE get failed when message transfered between processes. [POS_RET_ERROR_INNER]
///     - Message transfer failed between processes. [POS_RET_ERROR_INNER]
///     - The destination process name size is larger than 20 characters when message transfer between processes. \n
///       [POS_RET_ERROR_INNER]
///     - The message queue name has not been registered in control table when message transfer between processes.\n
///       [POS_RET_ERROR_INNER]
///     - Message transfer HANDLE create failed when message transfered between process. [POS_RET_ERROR_INNER]
///     - Message transfer HANDLE get failed from internal table when message transfered between processes. \n
///       [POS_RET_ERROR_INNER]
///     - Message transfer failed between process. [POS_RET_ERROR_INNER]
///     - Specified event HANDLE has not been registered in event HANDLE table during waiting event. \n
///       [POS_RET_ERROR_INNER]
///     - The count of thread beyond max in event management table during event waiting. [POS_RET_ERROR_INNER]
///     - The EV_FLAG_BIT is not set in flagID during getting event. [POS_RET_ERROR_INNER]
///     - The ID of message event queue has not been created during event getting. [POS_RET_ERROR_INNER]
///     - The flagID has not been registered during event getting. [POS_RET_ERROR_INNER]
///     - The interruption happened during event getting [POS_RET_ERROR_INNER]
///     - Whatever error happened during event getting. [POS_RET_ERROR_INNER]
///     - Getting event timeout. [POS_RET_ERROR_INNER]
///     - Error happened during event getting. [POS_RET_ERROR_INNER]
///     - The result of positioning service returned is SENSOR_RET_ERROR_PARAM. [POS_RET_ERROR_PARAM]
///     - The result of positioning service returned is SENSOR_RET_ERROR_BUFFULL. [POS_RET_ERROR_BUFFULL]
///     - The result of positioning service returned is SENSOR_RET_ERROR_RESOURCE. [POS_RET_ERROR_RESOURCE]
///     - The result of positioning service returned is not SENSOR_RET_ERROR_PARAM and SENSOR_RET_ERROR_BUFFULL \n
///       and SENSOR_RET_ERROR_RESOURCE. [POS_RET_ERROR_INNER]
///
/// \~english @par Detail
///     - Call this API to register longitude and latitude information delivery.\n
///       This API return the result of registered.
///        \n
///
/// \~english @par
///        Please note the following points when use this API.
///        - Duplication registering
///        - The same destination thread name has already been registered
///        - The registered delivery data updated and normal return.(first delivery)
///        - To one delivery destination, the same data will not be duplication deliveried at same time.
///        - After call this API, if the delivery destination thread name has been changed, please call this API again.
///
/// \~english @par
///          Notification of message
///        - After registered successfully, vehicle sensor information will been sent as system API \n
///          message with following format.\n
///          Command ID : CID_POSIF_REGISTER_LISTENER_LONLAT \n
///          message structure
/// \~english @code
///   typedef struct {
///     SENSORLOCATION_LONLATINFO_DAT  data;  /* message data */
///   } SENSORLOCATION_MSG_LONLATINFO;
/// @endcode
///           message data structure \n
///
/// \~english @code
///   typedef struct {
///   uint8_t   getMethod;  /* get method */
///   uint8_t   SyncCnt;    /* Synchrony count */
///   uint8_t   isEnable;   /* enable or not */
///   uint8_t   isExistDR;  /* DR exist or not[not used] */
///   uint8_t   DRStatus    /* DR status[not used] */
///   uint8_t   posSts;     /* position status */
///   uint16_t  posAcc;     /* Position accuracy */
///   int32_t   Longitude;  /* Longitude */
///   int32_t   Latitude;   /* Latitude */
///   } SENSORLOCATION_LONLATINFO_DAT;
/// @endcode
///        - Get method(getMethod)
///        - SENSOR_GET_METHOD_GPS - longitude and latitude from GPS
///        - SENSOR_GET_METHOD_NAVI - longitude and latitude from Navigation
///        - Synchrony count(SyncCnt)
///        - Count for position data synchronous \n
///          When delivery altitude and heading data, position data can be synchronized by SyncCnt.\n
///          But the data of different method can not be synchronized by SyncCnt.\n
///          Example 1: [LonLat from GPS] and [Heading from GPS] can be synchronized by SyncCnt.\n
///          Example 2: [LonLat from GPS] and [LonLat from Navi] can not be synchronized by SyncCnt.\n
///          Caution: The sensor count in sensor data delivery is another data.
///        - Enable or not(isEnable) \n
///          To describe this delivery message is whether can be used or not
///        - 0 - not avaliable
///        - not 0 - avaliable
///        - when GPS data is specified,longitude and latitude is invalid at following condition:\n
///          so [not avaliable] provieded
///        - After system start, GPS unit has not received current location data and GPS unit \n
///          status is not positioning fixed.
///        - If it is not initialized status, certainly provide [avaliable] when Navigation data specified
///        - If the status is [not avaliable], data following can not be guaranteed.
///        - Position status(posSts)
///        - Bit0 : GPS data used result(1:used, 0:not used, definition of maskbit:POS_LOC_INFO_USE_GSP)
///        - Bit1 : DGPS data used result(1:used, 0:not used, definition of maskbit:POS_LOC_INFO_USE_DGPS)
///        - Bit2 : DR data used result(1:used, 0:not used, definition of maskbit:POS_LOC_INFO_USE_DR)
///        - Bit3 : MapMatching data used result(1:used, 0:not used, definition of \n
///          maskbit:POS_LOC_INFO_USE_MAPMATCHING)
///        - Position accuracy(posAcc)
///        - Detected accruray of current position:1LSB=1m \n
///        - Longitude : (WGS-84)(10^-7degree as 1) \n
///          East longitude is positive value and west longitude is negative value.
///        - Latitude : (WGS-84)(10^-7degree as 1) \n
///          North latitude positive value and south latitude is negative value.
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Method
///
/// \~english @see
///        - POS_GetLonLat
///
////////////////////////////////////////////////////////////////////////////////////////////
POS_RET_API POS_RegisterListenerLonLat(HANDLE hApp, PCSTR notifyName, uint8_t ucCtrlFlg, uint8_t ucDeliveryTiming,
                                        uint8_t ucGetMethod);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup tag_Positioning
/// \~english @par Brief
///        - Register altitude delivery
///
/// \~english @param [in] hApp
///        - HANDLE - App Handle
/// \~english @param [in] notifyName
///        - PCSTR - Destination thread name
/// \~english @param [in] ucCtrlFlg
///        - uint8_t - Delivery control flag(register)
/// \~english @param [in] ucDeliveryTiming
///        - uint8_t - Delivery timing(change/update)
/// \~english @param [in] ucGetMethod
///        - uint8_t - Get method(GPS/Navi/Not specified)
///
/// \~english @par
///        - Delivery control flag(ucCtrlFlg)
///        - SENSOR_DELIVERY_REGIST - register
///        - Register specified altitude delivery
///        - Please note that if the same data delivery has been registered for multiple times, the data will \n
///          also be deliveried for registered multiple times.
///        - The specified altitude will be deliveried at registered time (first delivery).
///        - Delivery timing(ucDeliveryTiming)
///        - SENSOR_DELIVERY_TIMING_CHANGE - change. Specified altitude will be deliveried only when it is changed.
///        - SENSOR_DELIVERY_TIMING_UPDATE - update. Specified altitude will be deliveried when it is updated by \n
///          vehicle sensor.
///        - Get method(ucGetMethod)
///        - SENSOR_GET_METHOD_GPS - GPS. The altitude from GPS will be deliveried.
///        - SENSOR_GET_METHOD_NAVI - Navi. The altitude from Navigation will be deliveried.
///        - SENSOR_GET_METHOD_AUTO - Not specified. The altitude will be deliveried according to \n
///          the current environment
///        - Avaliable method is descriped as following in each environment. \n
///          In the following environment, if the SENSOR_GET_METHOD_AUTO is set,treated as default \n
///          In the following environment, if the unsupported method has been specified, \n
///          POS_RET_ERROR_PARAM will be returned.
///
/// \~english @retval POS_RET_NORMAL normal finish
/// \~english @retval POS_RET_ERROR_PARAM parameter error
/// \~english @retval POS_RET_ERROR_BUFFULL the register count is full
/// \~english @retval POS_RET_ERROR_INNER internal error
/// \~english @retval POS_RET_ERROR_NOSUPPORT unsupported
/// \~english @retval POS_RET_ERROR_RESOURCE lack of resource
///
/// \~english @par Precondition
///        - The creation/initialization(FrameworkunifiedCreateDispatcherWithoutLoop and etc.) of the dispatcher \n
///          for App are completed.
///        - Availability of positioning service is TRUE.
///
/// \~english @par changes of internal status
///        - There is no changes of internal status
///
/// \~english @par Failure condition
///        - The parameter ucDeliveryTiming is neither update(SENSOR_DELIVERY_TIMING_UPDATE) nor change \n
///          (SENSOR_DELIVERY_TIMING_CHANGE) [POS_RET_ERROR_PARAM]
///        - The parameter ucCtrlFlg is not registered(SENSOR_DELIVERY_REGIST) [POS_RET_ERROR_PARAM]
///        - The parameter hApp is NULL [POS_RET_ERROR_PARAM]
///        - The parameter notifyName is NULL [POS_RET_ERROR_PARAM]
///        - The count of message in message queue is reached to the max [POS_RET_ERROR_RESOURCE]
///        - The count of mutex is reached to the max [POS_RET_ERROR_RESOURCE]
///        - The count of item in ProcessName-ProcessNo convert table is reached to the max [POS_RET_ERROR_RESOURCE]
///        - The event is created in same process, but the count of reference to the event \n
///          is reach to max [POS_RET_ERROR_INNER]
///        - The event is created in system, but the count of reference to the event \n
///          is reach to the max  [POS_RET_ERROR_INNER]
///        - Memory allocate failed during the event table creation for that event registering. [POS_RET_ERROR_INNER]
///        - The thread can not register in the event table. [POS_RET_ERROR_INNER]
///        - Memory allocate failed in event table during the thread table creation. [POS_RET_ERROR_INNER]
///        - After register the thread table in event table, the event flag has already been registered. \n
///          [POS_RET_ERROR_INNER]
///        - After register the thread table in event table, the event flag register failed. [POS_RET_ERROR_INNER]
///        - The event table is full during event creation. [POS_RET_ERROR_INNER]
///        - The memory for event HANDLE allocate failed during event table creation. [POS_RET_ERROR_INNER]
///        - The thread can not be registered in event table. [POS_RET_ERROR_INNER]
///        - In event table, try to allocate the memory of the thread table creation, but failed. [POS_RET_ERROR_INNER]
///        - After register the thread in event table, the event flag has already been registered. [POS_RET_ERROR_INNER]
///        - After register the thread in event table, the event flag register failed. [POS_RET_ERROR_INNER]
///        - Specified event ID has not been registered in table. [POS_RET_ERROR_INNER]
///        - Initialize event object failed. [POS_RET_ERROR_INNER]
///        - ProcessNo has not been registered in message control table when message transfered between processes.\n
///          [POS_RET_ERROR_INNER]
///        - Message transfer HANDLE get failed when message transfered between processes. [POS_RET_ERROR_INNER]
///        - Message transfer failed between processes. [POS_RET_ERROR_INNER]
///        - The destination process name size is larger than 20 characters when message transfer between processes.\n
///          [POS_RET_ERROR_INNER]
///        - The message queue name has not been registered in control table when message transfer between processes.\n
///          [POS_RET_ERROR_INNER]
///        - Message transfer HANDLE create failed when message transfered between processes. [POS_RET_ERROR_INNER]
///        - Message transfer HANDLE get failed from internal table when message transfered between processes.\n
///          [POS_RET_ERROR_INNER]
///        - Message transfer failed between processes. [POS_RET_ERROR_INNER]
///        - Specified event HANDLE has not been registered in event HANDLE table during event waiting. \n
///          [POS_RET_ERROR_INNER]
///        - The count of thread is reach to max in event management table during event waiting.[POS_RET_ERROR_INNER]
///        - The EV_FLAG_BIT is not set in flagID during event getting. [POS_RET_ERROR_INNER]
///        - The ID of message event queue has not been created during event getting. [POS_RET_ERROR_INNER]
///        - The flagID has not been registered during event getting. [POS_RET_ERROR_INNER]
///        - The interruption happened during event getting [POS_RET_ERROR_INNER]
///        - Whatever error happened during event getting. [POS_RET_ERROR_INNER]
///        - Getting event timeout. [POS_RET_ERROR_INNER]
///        - Error happened during event getting. [POS_RET_ERROR_INNER]
///        - The result of positioning service is SENSOR_RET_ERROR_PARAM. [POS_RET_ERROR_PARAM]
///        - The result of positioning service is SENSOR_RET_ERROR_BUFFULL. [POS_RET_ERROR_BUFFULL]
///        - The result of positioning service is SENSOR_RET_ERROR_RESOURCE. [POS_RET_ERROR_RESOURCE]
///        - The result of positioning service is not SENSOR_RET_ERROR_PARAM and SENSOR_RET_ERROR_BUFFULL and \n
///          SENSOR_RET_ERROR_RESOURCE. [POS_RET_ERROR_INNER]
///
/// \~english @par Detail
///        - Call this API to register altitude delivery. \n
///          This API will finish when get the return value.
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
///          Notification of message
///        - After registered successfully, vehicle sensor will send system API message with following format. \n
///          Command ID : CID_POSIF_REGISTER_LISTENER_ALTITUDE \n
///          message structure
/// \~english @code
///   typedef struct {
///   SENSORLOCATION_ALTITUDEINFO_DAT  data;  /* message data */
/// } SENSORLOCATION_MSG_ALTITUDEINFO;
/// @endcode
///          message data structure
/// \~english @code
/// typedef struct {
///   uint8_t   getMethod;  /* get method */
///   uint8_t   SyncCnt;    /* synchrony count */
///   uint8_t   isEnable;   /* enable or not */
///   uint8_t   isExistDR;  /* DR exist or not[not used] */
///   uint8_t   DRStatus    /* DR status[not used] */
///   uint8_t   Reserve[3]; /* Reserve */
///   int32_t   Altitude;   /* Altitude(unit:0.01m) */
/// } SENSORLOCATION_ALTITUDEINFO_DAT;
/// @endcode
///        - Get method(getMethod)
///        - SENSOR_GET_METHOD_GPS - altitude from GPS
///        - SENSOR_GET_METHOD_NAVI - altitude from Navigation
///        - Synchrony count(SyncCnt)
///        - Count for position data synchronous \n
///          When delivery altitude and heading data, position data can be synchronized by SyncCnt. \n
///          But the data of different method can not be synchronized by SyncCnt.\n
///          example 1: [longitude and latitude from GPS] and [heading from GPS] can be synchronized by SyncCnt.\n
///          example 2: [longitude and latitude from GPS] and [longitude and latitude from Navi] can not be
///          synchronized by SyncCnt. \n
///          Caution: The sensor count in sensor data delivery is another data.
///        - Enable or not(isEnable) \n
///          To describe this delivery message is whether can be used or not.
///        - 0 - not avaliable
///        - not 0 - avaliable
///        - when GPS data specified, Altitude is invalid at following condition(so [not avaliable] provieded):
///        - Immediately after system start, GPS unit has not received current location data and GPS unit status \n
///          is not positioning fix
///        - If it is not initialization status, certainly provide [avaliable] when Navi data specified
///        - If the status is [not avaliable], data following can not be guaranteed.
///        - Altitude
///        - altitude data(unit 0.01m)
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Method
///
/// \~english @see delivery
///        - POS_GetAltitude
///
////////////////////////////////////////////////////////////////////////////////////////////
POS_RET_API POS_RegisterListenerAltitude(HANDLE hApp, PCSTR notifyName, uint8_t ucCtrlFlg, uint8_t ucDeliveryTiming,
                                         uint8_t ucGetMethod);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup tag_Positioning
/// \~english @par Brief
///        - Register speed delivery
///
/// \~english @param [in] hApp
///        - HANDLE - App Handle
/// \~english @param [in] notifyName
///        - PCSTR - Destination thread name
/// \~english @param [in] ucCtrlFlg
///        - uint8_t - Delivery control flag(register)
/// \~english @param [in] ucDeliveryTiming
///        - uint8_t - Delivery timing(change/update)
/// \~english @param [in] ucGetMethod
///        - uint8_t - Get method(POS/Navi/Not specified)
///
/// \~english @par
///        - Delivery control flag(ucCtrlFlg)
///        - SENSOR_DELIVERY_REGIST - register
///        - Register specified speed delivery
///        - Please note that if the same data delivery has been registered for multiple times, the data will \n
///          also be deliveried for registered mutiple times.
///        - The specified speed will be deliveried at registered time (first delivery).
///        - Delivery timing(ucDeliveryTiming)
///        - SENSOR_DELIVERY_TIMING_CHANGE - change. Specified speed will be deliveried only when it is changed.
///        - SENSOR_DELIVERY_TIMING_UPDATE - update. Specified speed will be deliveried when it is updated by \n
///          vehicle sensor.
///        - Get method(ucGetMethod)
///        - SENSOR_GET_METHOD_POS - The speed calculated in positioning based on speed pulse will be deliveried.
///        - SENSOR_GET_METHOD_NAVI - Navi  The speed from Navi will be deliveried.
///        - SENSOR_GET_METHOD_AUTO - Not specified  The speed will be deliveried according to the current environment
///          deliveried.
///        - Avaliable method is descriped as following in each environment. \n
///          In the following environment, if the SENSOR_GET_METHOD_AUTO is set,treated as default \n
///          In the following environment, if the unsupported method has been specified, \n
///          POS_RET_ERROR_PARAM will be returned.
///
/// \~english @retval POS_RET_NORMAL normal end
/// \~english @retval POS_RET_ERROR_PARAM parameter error
/// \~english @retval POS_RET_ERROR_BUFFULL the register count is full
/// \~english @retval POS_RET_ERROR_INNER internal error
/// \~english @retval POS_RET_ERROR_NOSUPPORT unsupported
/// \~english @retval POS_RET_ERROR_RESOURCE lack of resource
///
/// \~english @par Precondition
///        - The creation/initialization(FrameworkunifiedCreateDispatcherWithoutLoop and etc.) of the Dispatcher for App \n
///          are completed.
///        - Availability of service positioning is TRUE.
///
/// \~english @par change of internal status
///        - There is no change of internal status
///
/// \~english @par Failure condition
///        - The parameter ucDeliveryTiming is neither update(SENSOR_DELIVERY_TIMING_UPDATE) \n
///          nor change(SENSOR_DELIVERY_TIMING_CHANGE) [POS_RET_ERROR_PARAM]
///        - The parameter ucCtrlFlg is not register(SENSOR_DELIVERY_REGIST) [POS_RET_ERROR_PARAM]
///        - The parameter hApp is NULL [POS_RET_ERROR_PARAM]
///        - The parameter notifyName is NULL [POS_RET_ERROR_PARAM]
///        - The count of message in message queue is reach to max [POS_RET_ERROR_RESOURCE]
///        - The count of mutex is reach to max [POS_RET_ERROR_RESOURCE]
///        - The count of item in ProcessName-ProcessNo convert table is reach to max [POS_RET_ERROR_RESOURCE]
///        - The event is created in same process, but the count of reference is reach to max [POS_RET_ERROR_INNER]
///        - The event is created in system, but the count of reference is reach to max  [POS_RET_ERROR_INNER]
///        - Memory allocate failed during the event table creation for event registering. [POS_RET_ERROR_INNER]
///        - The thread can not register in the event table. [POS_RET_ERROR_INNER]
///        - Memory allocate failed in event table during the thread table creation. [POS_RET_ERROR_INNER]
///        - After register the thread table in event table, the event flag has already been registered. \n
///          [POS_RET_ERROR_INNER]
///        - After register the thread table in event table, the event flag register failed. [POS_RET_ERROR_INNER]
///        - The event table is full during event creation. [POS_RET_ERROR_INNER]
///        - The memory for event HANDLE allocate failed during event table creation. [POS_RET_ERROR_INNER]
///        - The thread can not be registered in event table. [POS_RET_ERROR_INNER]
///        - In event table, try to allocate the memory of the thread table creation, but failed. [POS_RET_ERROR_INNER]
///        - After register the thread in event table, the event flag has already been registered. [POS_RET_ERROR_INNER]
///        - After register the thread in event table, the event flag register failed. [POS_RET_ERROR_INNER]
///        - Specified event ID has not been registered in table. [POS_RET_ERROR_INNER]
///        - Initialize event object failed. [POS_RET_ERROR_INNER]
///        - ProcessNo has not been registered in message control table when message transfered between processes. \n
///          [POS_RET_ERROR_INNER]
///        - Message transfer HANDLE get failed when message transfered between processes. [POS_RET_ERROR_INNER]
///        - Message transfer failed between processes. [POS_RET_ERROR_INNER]
///        - The destination process name size is larger than 20 characters when message transfer between processes.\n
///          [POS_RET_ERROR_INNER]
///        - The message queue name has not been registered in control table when message transfer between processes.\n
///          [POS_RET_ERROR_INNER]
///        - Message transfer HANDLE create failed when message transfered between processes. [POS_RET_ERROR_INNER]
///        - Message transfer HANDLE get failed from internal table when message transfered between processes. \n
///          [POS_RET_ERROR_INNER]
///        - Message transfer failed between processes. [POS_RET_ERROR_INNER]
///        - Specified event HANDLE has not been registered in event HANDLE table during event waiting. \n
///          [POS_RET_ERROR_INNER]
///        - The count of thread is reach to max in event management table during event waiting. [POS_RET_ERROR_INNER]
///        - The EV_FLAG_BIT is not set in flagID during event getting. [POS_RET_ERROR_INNER]
///        - The ID of message event queue has not been created during event getting. [POS_RET_ERROR_INNER]
///        - The flagID has not been registered during event getting. [POS_RET_ERROR_INNER]
///        - The interruption happened during event getting [POS_RET_ERROR_INNER]
///        - Whatever error happened during event getting. [POS_RET_ERROR_INNER]
///        - Getting event timeout. [POS_RET_ERROR_INNER]
///        - Error happened during event getting. [POS_RET_ERROR_INNER]
///        - The result of positioning service is SENSOR_RET_ERROR_PARAM. [POS_RET_ERROR_PARAM]
///        - The result of positioning service is SENSOR_RET_ERROR_BUFFULL. [POS_RET_ERROR_BUFFULL]
///        - The result of positioning service is SENSOR_RET_ERROR_RESOURCE. [POS_RET_ERROR_RESOURCE]
///        - The result of positioning service is not SENSOR_RET_ERROR_PARAM and SENSOR_RET_ERROR_BUFFULL \n
///          and SENSOR_RET_ERROR_RESOURCE. [POS_RET_ERROR_INNER]
///
/// \~english @par Detail
///        - Call this API to register speed delivery. \n
///          This API will finish when get the return value.
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
///          Notification of message
///        - After registered successfully, vehicle sensor will send system API message with following format. \n
///          Command ID : CID_POSIF_REGISTER_LISTENER_SPEED \n
///           message structure
/// \~english @code
///   typedef struct {
///     SENSORMOTION_SPEEDINFO_DAT  data;  /* message data */
///   } SENSORMOTION_MSG_SPEEDINFO;
/// @endcode
///           message data structure
/// \~english @code
///   typedef struct {
///   uint8_t   getMethod;    /* get method */
///   uint8_t   SyncCnt;      /* synchrony count */
///   uint8_t   isEnable;     /* enable or not */
///   uint8_t   isExistDR;    /* DR exist or not[not used] */
///   uint8_t   Reserve1[3];  /* Reserve */
///   uint8_t   DRStatus      /* DR status[not used] */
///   uint16_t  Speed;        /* speed(unit:0.01m/sec) */
///   uint8_t   Reserve2[2];  /* Reserve */
///   } SENSORMOTION_SPEEDINFO_DAT;
/// @endcode
///        - Get method(getMethod)
///        - SENSOR_GET_METHOD_POS - The speed calculated in positioning based on speed pulse will be deliveried.
///        - SENSOR_GET_METHOD_NAVI - speed from Navi
///        - Synchrony count(SyncCnt)
///        - 0 (not change).
///        - Enable or not(isEnable) \n
///          To describe this data is whether enable or not.
///        - 0 - not avaliable
///        - not 0 - avaliable
///        - Speed is invalid at following condition when speed pulse specified, so [not avaliable] provieded
///        - Immediately after system start, the sensor data have not been received from SYS micon
///        - If it is not initialization status, certainly provide [avaliable] when Navi data specified
///        - If the status is [not avaliable], data following can not be guaranteed.
///        - Speed
///        - speed data(unit 0.01m/sec)
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Method
///
/// \~english @see
///        - POS_GetSpeed
///
////////////////////////////////////////////////////////////////////////////////////////////
POS_RET_API POS_RegisterListenerSpeed(HANDLE hApp, PCSTR notifyName, uint8_t ucCtrlFlg, uint8_t ucDeliveryTiming,
                                      uint8_t ucGetMethod);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup tag_Positioning
/// \~english @par Brief
///        - Register heading delivery
///
/// \~english @param [in] hApp
///        - HANDLE - App Handle
/// \~english @param [in] notifyName
///        - PCSTR - Destination thread name
/// \~english @param [in] ucCtrlFlg
///        - uint8_t - Delivery control flag(register)
/// \~english @param [in] ucDeliveryTiming
///        - uint8_t - Delivery timing(change/update)
/// \~english @param [in] ucGetMethod
///        - uint8_t - Get method(GPS/Navi/Not specified)
///
/// \~english @par
///        - Delivery control flag(ucCtrlFlg)
///        - SENSOR_DELIVERY_REGIST - register
///        - Register specified heading delivery
///        - Please note that if the same data delivery has been registered for mutiple times, the data will \n
///          also be deliveried for registered mutiple times.
///        - The specified heading will be deliveried at registered time (first delivery).
///        - Delivery timing(ucDeliveryTiming)
///        - SENSOR_DELIVERY_TIMING_CHANGE - change. Specified heading will be deliveried only when it is changed.
///        - SENSOR_DELIVERY_TIMING_UPDATE - update. Specified heading will be deliveried when it is updated by \n
///          vehicle sensor.
///        - Get method(ucGetMethod)
///        - SENSOR_GET_METHOD_GPS - GPS. The heading from GPS will be deliveried.
///        - SENSOR_GET_METHOD_NAVI - Navi. The heading from Navi will be deliveried.
///        - SENSOR_GET_METHOD_AUTO - Not specified. The heading will be deliveried \n
///          according to the current environment
///        - Avaliable method is descriped as following in each environment. \n
///          In the following environment, if the SENSOR_GET_METHOD_AUTO is set,treated as default \n
///          In the following environment, if the unsupported method has been specified, \n
///          POS_RET_ERROR_PARAM will be returned.
///
/// \~english @retval POS_RET_NORMAL normal finish
/// \~english @retval POS_RET_ERROR_PARAM parameter error
/// \~english @retval POS_RET_ERROR_BUFFULL the register count is full
/// \~english @retval POS_RET_ERROR_INNER internal error
/// \~english @retval POS_RET_ERROR_NOSUPPORT unsupported
/// \~english @retval POS_RET_ERROR_RESOURCE lack of resource
///
/// \~english @par Precondition
///        - The creation/initialization(FrameworkunifiedCreateDispatcherWithoutLoop and etc.) of the Dispatcher \n
///          for App are completed.
///        - Availability of positioning service is TRUE.
///
/// \~english @par changes of internal status
///        - There is no changes of internal status
///
/// \~english @par Failure condition
///        - The parameter ucDeliveryTiming is neither update(SENSOR_DELIVERY_TIMING_UPDATE) nor change \n
///          (SENSOR_DELIVERY_TIMING_CHANGE) [POS_RET_ERROR_PARAM]
///        - The parameter ucCtrlFlg is not register(SENSOR_DELIVERY_REGIST) [POS_RET_ERROR_PARAM]
///        - The parameter hApp is NULL [POS_RET_ERROR_PARAM]
///        - The parameter notifyName is NULL [POS_RET_ERROR_PARAM]
///        - The count of message in message queue is reach to max [POS_RET_ERROR_RESOURCE]
///        - The count of mutex is reach to max [POS_RET_ERROR_RESOURCE]
///        - The count of item in ProcessName-ProcessNo convert table is reach to max [POS_RET_ERROR_RESOURCE]
///        - The event is created in same process, but the count of reference is reach max [POS_RET_ERROR_INNER]
///        - The event is created in system, but the count of reference is reach to max  [POS_RET_ERROR_INNER]
///        - Memory allocate failed during the event table creation for event registering. [POS_RET_ERROR_INNER]
///        - The thread can not register in the event table. [POS_RET_ERROR_INNER]
///        - Memory allocate failed in event table during the thread table creation. [POS_RET_ERROR_INNER]
///        - After register the thread table in event table, the event flag has already \n
///          been registered. [POS_RET_ERROR_INNER]
///        - After register the thread table in event table, the event flag register failed. [POS_RET_ERROR_INNER]
///        - The event table is full during event creation. [POS_RET_ERROR_INNER]
///        - The memory for event HANDLE allocate failed during event table creation. [POS_RET_ERROR_INNER]
///        - The thread can not be registered in event table. [POS_RET_ERROR_INNER]
///        - In event table, try to allocate the memory of the thread table creation, but failed. [POS_RET_ERROR_INNER]
///        - After register the thread in event table, the event flag has already been registered. [POS_RET_ERROR_INNER]
///        - After register the thread in event table, the event flag register failed. [POS_RET_ERROR_INNER]
///        - Specified event ID has not been registered in table. [POS_RET_ERROR_INNER]
///        - Initialize event object failed. [POS_RET_ERROR_INNER]
///        - ProcessNo has not been registered in message control table when message transfered between processes. \n
///          [POS_RET_ERROR_INNER]
///        - Message transfer HANDLE get failed when message transfered between processes. [POS_RET_ERROR_INNER]
///        - Message transfer failed between processes. [POS_RET_ERROR_INNER]
///        - The destination process name size is larger than 20 characters when message transfer \n
///          between processes. [POS_RET_ERROR_INNER]
///        - The message queue name has not been registered in control table when message transfer between \n
///          processes. [POS_RET_ERROR_INNER]
///        - Message transfer HANDLE create failed when message transfered between processes. [POS_RET_ERROR_INNER]
///        - Message transfer HANDLE get failed from internal table when message transfered between processes. \n
///          [POS_RET_ERROR_INNER]
///        - Message transfer failed between processes. [POS_RET_ERROR_INNER]
///        - Specified event HANDLE has not been registered in event HANDLE table during event waiting. \n
///          [POS_RET_ERROR_INNER]
///        - The count of thread is reach to  max in event management table during event waiting. [POS_RET_ERROR_INNER]
///        - The EV_FLAG_BIT is not set in flagID during event getting. [POS_RET_ERROR_INNER]
///        - The ID of message event queue has not been created during event getting. [POS_RET_ERROR_INNER]
///        - The flagID has not been registered during event getting. [POS_RET_ERROR_INNER]
///        - The interruption happened during event getting [POS_RET_ERROR_INNER]
///        - Whatever error happened during event getting. [POS_RET_ERROR_INNER]
///        - Getting event timeout. [POS_RET_ERROR_INNER]
///        - Error happened during event getting. [POS_RET_ERROR_INNER]
///        - The result of positioning service is SENSOR_RET_ERROR_PARAM. [POS_RET_ERROR_PARAM]
///        - The result of positioning service is SENSOR_RET_ERROR_BUFFULL. [POS_RET_ERROR_BUFFULL]
///        - The result of positioning service is SENSOR_RET_ERROR_RESOURCE. [POS_RET_ERROR_RESOURCE]
///        - The result of positioning service is not SENSOR_RET_ERROR_PARAM and SENSOR_RET_ERROR_BUFFULL \n
///          and SENSOR_RET_ERROR_RESOURCE. [POS_RET_ERROR_INNER]
///
/// \~english @par Detail
///        - Call this API to register heading delivery.
///          This API will finish when get the return value.
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
///           Notification of message
///        - After registered successfully, vehicle sensor will send system API message with following format. \n
///          Command ID : CID_POSIF_REGISTER_LISTENER_HEADING \n
///           message structure
/// \~english @code
///   typedef struct {
///     SENSORMOTION_HEADINGINFO_DAT  data;  /* message data */
///   } SENSORMOTION_MSG_HEADINGINFO;
/// @endcode
///           message data structure
/// \~english @code
///   typedef struct {
///   uint8_t   getMethod;    /* get method */
///   uint8_t   SyncCnt;      /* sync count */
///   uint8_t   isEnable;     /* enable or not */
///   uint8_t   isExistDR;    /* DR exist or not[not used] */
///   uint8_t   DRStatus      /* DR status[not used] */
///   uint8_t   posSts;       /* position status */
///   uint8_t   Reserve1[2];  /* Reserve */
///   uint16_t  Heading;      /* heading(unit:0.01degree) */
///   uint8_t   Reserve2[2];  /* Reserve */
///   } SENSORMOTION_HEADINGINFO_DAT;
/// @endcode
///        - Get method(getMethod)
///        - SENSOR_GET_METHOD_GPS - Heading from GPS
///        - SENSOR_GET_METHOD_NAVI - Heading from Navi
///        - Synchrony count(SyncCnt)
///        - Count for position data synchronous \n
///          When delivery altitude and heading data, position data can be synchronized by this count. \n
///          But the data of different method can not be synchronized by this count.\n
///          example 1: [longitude and latitude from GPS] and [heading from GPS] can be synchronized by the count. \n
///          example 2: [longitude and latitude from GPS] and [longitude and latitude from Navi] can not be \n
///          synchronized by the count. \n
///          Caution: The sensor count in sensor data delivery is another data.
///        - Enable or not(isEnable) \n
///          To describe this data is whether enable or not.
///        - 0 - not avaliable
///        - not 0 - avaliable
///        - Heading is invalid at following condition when GPS data specified, so [not avaliable] provieded \n
///        - Immediately after system start, GPS unit has not received current location data and GPS unit status \n
///          is not positioning fix
///        - If it is not initialization status, certainly provide [avaliable] when Navi data specified
///        - If the status is [not avaliable], data following can not be guaranteed.
///        - Position status(posSts)
///        - Bit0 : GPS data used result(1:used, 0:not used, definition of maskbit:POS_LOC_INFO_USE_GSP)
///        - Bit1 : DGPS data used result(1:used, 0:not used, definition of maskbit:POS_LOC_INFO_USE_DGPS)
///        - Bit2 : DR data used result(1:used, 0:not used, definition of maskbit:POS_LOC_INFO_USE_DR)
///        - Bit3 : MapMatching data used result(1:used, 0:not used, definition of \n
///          maskbit:POS_LOC_INFO_USE_MAPMATCHING)
///        - Heading
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Method
///
/// \~english @see
///        - POS_GetHeading
///
////////////////////////////////////////////////////////////////////////////////////////////
POS_RET_API POS_RegisterListenerHeading(HANDLE hApp, PCSTR notifyName, uint8_t ucCtrlFlg, uint8_t ucDeliveryTiming,
                                        uint8_t ucGetMethod);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup tag_Positioning
/// \~english @par Brief
///        - Get longitude and latitude information
///
/// \~english @param [in] hApp
///        - HANDLE - App Handle
/// \~english @param [out] dat
///        - SENSORLOCATION_LONLATINFO_DAT* - output pointer to longitude and latitude informaiton
/// \~english @param [in] ucGetMethod
///        - uint8_t - Get method(GPS/Navi/Not specified)
///
/// \~english @par
///        - Get method(ucGetMethod)
///        - SENSOR_GET_METHOD_GPS - GPS  The longitude and latitude from GPS will be deliveried.
///        - SENSOR_GET_METHOD_NAVI - Navi  The longitude and latitude from Navi will be deliveried.
///        - SENSOR_GET_METHOD_AUTO - Not specified  The longitude and latitude will be deliveried \n
///          according to the current environment.
///        - Avaliable method is descriped as following in each environment. \n
///          In the following environment, if the SENSOR_GET_METHOD_AUTO is set,treated as default \n
///          In the following environment, if the unsupported method has been specified, \n
///          POS_RET_ERROR_PARAM will be returned.
/// \~english @par
///        - SENSORLOCATION_LONLATINFO_DAT structure
/// \~english @code
///   typedef struct {
///   uint8_t   getMethod;  /* get method */
///   uint8_t   SyncCnt;    /* sync count */
///   uint8_t   isEnable;   /* enable or not */
///   uint8_t   isExistDR;  /* DR exist or not[not used] */
///   uint8_t   DRStatus    /* DR status[not used] */
///   uint8_t   posSts;     /* position status */
///   uint16_t  posAcc;     /* Position accuracy */
///   int32_t   Longitude;  /* Longitude */
///   int32_t   Latitude;   /* Latitude */
///   } SENSORLOCATION_LONLATINFO_DAT;
/// @endcode
///        - Get method(getMethod)
///        - SENSOR_GET_METHOD_GPS - longitude and latitude from GPS
///        - SENSOR_GET_METHOD_NAVI - longitude and latitude from Navi
///        - Synchrony count(SyncCnt)
///        - Count for position data synchronous\n
///          When delivery altitude and heading data, position data can be synchronized by this count. \n
///          But the data of different method can not be synchronized by this count. \n
///          example 1: [longitude and latitude from GPS] and [heading from GPS] can be synchronized by this count. \n
///          example 2: [longitude and latitude from GPS] and [longitude and latitude from Navi] can not be  \n
///          synchronized by this count. \n
///          Caution: The sensor count in sensor data delivery is another data.
///        - Enable or not(isEnable) \n
///          To describe this data is whether enable or not.
///        - 0 - not avaliable
///        - not 0 - avaliable
///        - longitude and latitude is invalid at following condition when GPS data specified, \n
///          so [not avaliable] provieded
///        - Immediately after system start, GPS unit has not received current location data and \n
///          GPS unit status is not positioning fix
///        - If it is not initialization status, certainly provide [avaliable] when Navi data specified\n
///        - If the status is [not avaliable], data following can not be guaranteed.
///        - Position status(posSts)
///        - Bit0 : GPS data used result(1:used, 0:not used, definition of maskbit:POS_LOC_INFO_USE_GSP)
///        - Bit1 : DGPS data used result(1:used, 0:not used, definition of maskbit:POS_LOC_INFO_USE_DGPS)
///        - Bit2 : DR data used result(1:used, 0:not used, definition of maskbit:POS_LOC_INFO_USE_DR)
///        - Bit3 : MapMatching data used result(1:used, 0:not used, definition of \n
///          maskbit:POS_LOC_INFO_USE_MAPMATCHING)
///        - Position accuracy(posAcc)
///        - Detected accruray of current position:1LSB=1m
///        - Longitude : (WGS-84)(10^-7degree as 1) \n
///          East longitude is positive value and west longitude is minus value.
///        - Latitude : (WGS-84)(10^-7degree as 1)\n
///          North latitudeis positive value and south latitude is minus value.
///
/// \~english @retval POS_RET_NORMAL normal finish
/// \~english @retval POS_RET_ERROR_PARAM parameter error
/// \~english @retval POS_RET_ERROR_INNER internal error
/// \~english @retval POS_RET_ERROR_NOSUPPORT unsupported
/// \~english @retval POS_RET_ERROR_RESOURCE lack of resource
///
/// \~english @par Precondition
///        - The creation/initialization(FrameworkunifiedCreateDispatcherWithoutLoop and etc.) of the Dispatcher \n
///          for App are completed.
///        - Availability of positioning service is TRUE.
///
/// \~english @par change of internal status
///        - There is no change of internal status
///
/// \~english @par Failure condition
///        - The parameter hApp is NULL [POS_RET_ERROR_PARAM]
///        - The parameter dat is NULL [POS_RET_ERROR_PARAM]
///        - The count of message in message queue is reach to max [POS_RET_ERROR_RESOURCE]
///        - The count of mutex is reach to max [POS_RET_ERROR_RESOURCE]
///        - The count of item in ProcessName-ProcessNo convert table is reach to max [POS_RET_ERROR_RESOURCE]
///        - The event is created in same process, but the count of reference is reach to max [POS_RET_ERROR_INNER]
///        - The event is created in system, but the count of reference is reach to max  [POS_RET_ERROR_INNER]
///        - Memory allocate failed during the event table creation for event registering. [POS_RET_ERROR_INNER]
///        - The thread can not register in the event table. [POS_RET_ERROR_INNER]
///        - Memory allocate failed in event table during the thread table creation. [POS_RET_ERROR_INNER]
///        - After register the thread table in event table, the event flag has already been registered. \n
///          [POS_RET_ERROR_INNER]
///        - After register the thread table in event table, the event flag register failed. [POS_RET_ERROR_INNER]
///        - The event table is full during event creation. [POS_RET_ERROR_INNER]
///        - The memory for event HANDLE allocate failed during event table creation. [POS_RET_ERROR_INNER]
///        - The thread can not be registered in event table. [POS_RET_ERROR_INNER]
///        - In event table, try to allocate the memory of the thread table creation, but failed. [POS_RET_ERROR_INNER]
///        - After register the thread in event table, the event flag has already been registered. [POS_RET_ERROR_INNER]
///        - After register the thread in event table, the event flag register failed. [POS_RET_ERROR_INNER]
///        - Specified event ID has not been registered in table. [POS_RET_ERROR_INNER]
///        - Initialize event object failed. [POS_RET_ERROR_INNER]
///        - The memory for storing semaphore control data allocate failed. [POS_RET_ERROR_INNER]
///        - Specified semaphore ID has not been registered when lock semaphore. [POS_RET_ERROR_INNER]
///        - Internal mutex HANDLE is NULL when lock mutex. [POS_RET_ERROR_INNER]
///        - Internal mutex HANDLE has not been registered in mutex table when lock mutex. [POS_RET_ERROR_INNER]
///        - The owner of specified mutex is not itself when lock mutex. [POS_RET_ERROR_INNER]
///        - Mutex has been multiple locked [POS_RET_ERROR_INNER]
///        - The HANDLE is NULL when getting usable share memory address. [POS_RET_ERROR_INNER]
///        - Can not get usable share memory address. [POS_RET_ERROR_INNER]
///        - Memory allocate failed for share memory map. [POS_RET_ERROR_INNER]
///        - Memory allocate failed for share memory management. [POS_RET_ERROR_INNER]
///        - Can not open share memory. [POS_RET_ERROR_INNER]
///        - Failed to mapping share memory. [POS_RET_ERROR_INNER]
///        - No empty field in share memory. [POS_RET_ERROR_INNER]
///        - Failed to guarantee share memory. [POS_RET_ERROR_INNER]
///        - ProcessNo has not been registered in message control table when message transfered between processes. \n
///          [POS_RET_ERROR_INNER]
///        - Message transfer HANDLE get failed when message transfered between processes. [POS_RET_ERROR_INNER]
///        - Message transfer failed between processes. [POS_RET_ERROR_INNER]
///        - The destination process name size is larger than 20 characters when message transfer \n
///          between processes. [POS_RET_ERROR_INNER]
///        - The message queue name has not been registered in control table when message transfer \n
///          between processes. [POS_RET_ERROR_INNER]
///        - Message transfer HANDLE create failed when message transfered between processes. [POS_RET_ERROR_INNER]
///        - Message transfer HANDLE get failed from internal table when message transfered between processes. \n
///          [POS_RET_ERROR_INNER]
///        - Message transfer failed between process. [POS_RET_ERROR_INNER]
///        - Specified event HANDLE has not been registered in event HANDLE table during event waiting. \n
///          [POS_RET_ERROR_INNER]
///        - The count of thread is reach to max in event management table during event waiting. [POS_RET_ERROR_INNER]
///        - The EV_FLAG_BIT is not set in flagID during event getting. [POS_RET_ERROR_INNER]
///        - The ID of message event queue has not been created during event getting. [POS_RET_ERROR_INNER]
///        - The flagID has not been registered during event getting. [POS_RET_ERROR_INNER]
///        - The interruption happened during event getting [POS_RET_ERROR_INNER]
///        - Whatever error happened during event getting. [POS_RET_ERROR_INNER]
///        - The HANDLE is NULL when getting usable share memory address for accessing received data. \n
///          [POS_RET_ERROR_INNER]
///        - Can not get usable share memory address for accessing received data. [POS_RET_ERROR_INNER]
///        - Memory for share memory map allocate failed for accessing received data. [POS_RET_ERROR_INNER]
///        - Memory for share memory management allocate failed for accessing received data. [POS_RET_ERROR_INNER]
///        - Can not open share memory for accessing received data. [POS_RET_ERROR_INNER]
///        - Failed to mapping share memory for accessing received data. [POS_RET_ERROR_INNER]
///        - The size of data stored in share memory is larger than the size of received data. [POS_RET_ERROR_INNER]
///        - The result of getting sensor data get is POS_RET_ERROR_RESOURCE. [POS_RET_ERROR_RESOURCE]
///        - The result of getting sensor data get is not POS_RET_ERROR_RESOURCE. [POS_RET_ERROR_INNER]
///
/// \~english @par Detail
///        - Call this API to get longitude and latitude from vehicle sensor
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Sync
///
/// \~english @see
///        - POS_RegisterListenerLonLat
///
////////////////////////////////////////////////////////////////////////////////////////////
//POS_RET_API POS_GetLonLat(HANDLE hApp, SENSORLOCATION_LONLATINFO_DAT *dat, uint9_t ucGetMethod);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup tag_Positioning
/// \~english @par Brief
///        - Get altitude data
///
/// \~english @param [in] hApp
///        - HANDLE - App Handle
/// \~english @param [out] dat
///        - SENSORLOCATION_ALTITUDEINFO_DAT* - output pointer to altitude data
/// \~english @param [in] ucGetMethod
///        - uint8_t - Get method(GPS/Navi/Not specified)
///
/// \~english @par
///        - Get method(ucGetMethod)
///        - SENSOR_GET_METHOD_GPS - GPS  The altitude from GPS will be deliveried.
///        - SENSOR_GET_METHOD_NAVI - Navi  The altitude from Navi will be deliveried.
///        - SENSOR_GET_METHOD_AUTO - Not specified  The altitude will be deliveried \n
///          according to the current environment
///        - Avaliable method is descriped as following in each environment. \n
///          In the following environment, if the SENSOR_GET_METHOD_AUTO is set,treated as default \n
///          In the following environment, if the unsupported method has been specified, \n
///          POS_RET_ERROR_PARAM will be returned.
/// \~english @par
///        - SENSORLOCATION_ALTITUDEINFO_DAT structure
/// \~english @code
/// typedef struct {
///   uint8_t   getMethod;  /* get method */
///   uint8_t   SyncCnt;    /* sync count */
///   uint8_t   isEnable;   /* enable or not */
///   uint8_t   isExistDR;  /* DR exist or not[not used] */
///   uint8_t   DRStatus    /* DR status[not used] */
///   uint8_t   Reserve[3]; /* Reserve */
///   int32_t   Altitude;   /* Altitude(unit:0.01m) */
/// } SENSORLOCATION_ALTITUDEINFO_DAT;
/// @endcode
///        - Get method(getMethod)
///        - SENSOR_GET_METHOD_GPS - altitude from GPS
///        - SENSOR_GET_METHOD_NAVI - altitude from Navi
///        - Synchrony count(SyncCnt)
///        - Count for position data synchronous \n
///          When delivery altitude and heading data, position data can be synchronized by this count. \n
///          But the data of different method can not be synchronized by this count. \n
///          example 1: [longitude and latitude from GPS] and [heading from GPS] can be synchronized by the count. \n
///          example 2: [longitude and latitude from GPS] and [longitude and latitude from Navi] can not be \n
///          synchronized by the count. \n
///          Caution: The sensor count in sensor data delivery is another data.
///        - Enable or not(isEnable)\n
///          To describe this data is whether enable or not.
///        - 0 - not avaliable
///        - not 0 - avaliable
///        - Altitude is invalid at following condition when GPS data specified, so [not avaliable] provieded
///        - Immediately after system start, GPS unit has not received current location data and GPS unit \n
///          status is not positioning fix
///        - If it is not initialization status, certainly provide [avaliable] when Navi data specified
///        - If the status is [not avaliable], data following can not be guaranteed.
///        - Altitude
///        - altitude data(unit 0.01m)
///
/// \~english @retval POS_RET_NORMAL normal finish
/// \~english @retval POS_RET_ERROR_PARAM parameter error
/// \~english @retval POS_RET_ERROR_INNER internal error
/// \~english @retval POS_RET_ERROR_NOSUPPORT unsupported
/// \~english @retval POS_RET_ERROR_RESOURCE lack of resource
///
/// \~english @par Precondition
///        - The creation/initialization(FrameworkunifiedCreateDispatcherWithoutLoop and etc.) of \n
///          the Dispatcher for App are completed.
///        - Availability of positioning service is TRUE.
///
/// \~english @par changes of internal status
///        - There is no changes of internal status
///
/// \~english @par Failure condition
///        - The parameter hApp is NULL [POS_RET_ERROR_PARAM]
///        - The parameter dat is NULL [POS_RET_ERROR_PARAM]
///        - The count of message in message queue is reach to max [POS_RET_ERROR_RESOURCE]
///        - The count of mutex is reach to max [POS_RET_ERROR_RESOURCE]
///        - The count of item in ProcessName-ProcessNo convert table is reach to max [POS_RET_ERROR_RESOURCE]
///        - The event is created in same process, but the count of reference is reach to max [POS_RET_ERROR_INNER]
///        - The event is created in system, but the count of reference is reach to max  [POS_RET_ERROR_INNER]
///        - Memory allocate falied during the event table creation for event registering. [POS_RET_ERROR_INNER]
///        - The thread can not register in the event table. [POS_RET_ERROR_INNER]
///        - Memory allocate failed in event table during the thread table creation. [POS_RET_ERROR_INNER]
///        - After register the thread table in event table, the event flag has already been \n
///          registered. [POS_RET_ERROR_INNER]
///        - After register the thread table in event table, the event flag register failed. [POS_RET_ERROR_INNER]
///        - The event table is full during event creation. [POS_RET_ERROR_INNER]
///        - The memory for event HANDLE allocate failed during event table creation. [POS_RET_ERROR_INNER]
///        - The thread can not be registered in event table. [POS_RET_ERROR_INNER]
///        - In event table, try to allocate the memory of the thread table creation, but failed. [POS_RET_ERROR_INNER]
///        - After register the thread in event table, the event flag has already been registered. [POS_RET_ERROR_INNER]
///        - After register the thread in event table, the event flag register failed. [POS_RET_ERROR_INNER]
///        - Specified event ID has not been registered in table. [POS_RET_ERROR_INNER]
///        - Initialize event object failed. [POS_RET_ERROR_INNER]
///        - The memory for storing semaphore control data allocate failed. [POS_RET_ERROR_INNER]
///        - Specified semaphore ID has not been registered when lock semaphore. [POS_RET_ERROR_INNER]
///        - Internal mutex HANDLE is NULL when lock mutex. [POS_RET_ERROR_INNER]
///        - Internal mutex HANDLE has not been registered in mutex table when lock mutex. [POS_RET_ERROR_INNER]
///        - The owner of specified mutex is not itself when lock mutex. [POS_RET_ERROR_INNER]
///        - Mutex has been multiple locked [POS_RET_ERROR_INNER]
///        - The HANDLE is NULL when getting usable share memory address. [POS_RET_ERROR_INNER]
///        - Can not get usable share memory address. [POS_RET_ERROR_INNER]
///        - Memory allocate failed for share memory map. [POS_RET_ERROR_INNER]
///        - Memory allocate failed for share memory management. [POS_RET_ERROR_INNER]
///        - Can not open share memory. [POS_RET_ERROR_INNER]
///        - Failed to mapping share memory. [POS_RET_ERROR_INNER]
///        - No empty field in share memory. [POS_RET_ERROR_INNER]
///        - Failed to guarantee share memory. [POS_RET_ERROR_INNER]
///        - ProcessNo has not been registered in message control table when message \n
///          transfered between processes. [POS_RET_ERROR_INNER]
///        - Message transfer HANDLE get failed when message transfered between processes. [POS_RET_ERROR_INNER]
///        - Message transfer failed between processes. [POS_RET_ERROR_INNER]
///        - The destination process name size is larger than 20 characters when message transfer \n
///          between processes. [POS_RET_ERROR_INNER]
///        - The message queue name has not been registered in control table when message \n
///          transfer between processes. [POS_RET_ERROR_INNER]
///        - Message transfer HANDLE create failed when message transfered between processes. [POS_RET_ERROR_INNER]
///        - Message transfer HANDLE get failed from internal table when message transfered \n
///          between processes. [POS_RET_ERROR_INNER]
///        - Message transfer failed between processes. [POS_RET_ERROR_INNER]
///        - Specified event HANDLE has not been registered in event HANDLE table during \n
///          event waiting. [POS_RET_ERROR_INNER]
///        - The count of thread is reach to max in event management table during event waiting. [POS_RET_ERROR_INNER]
///        - The EV_FLAG_BIT is not set in flagID during event getting. [POS_RET_ERROR_INNER]
///        - The ID of message event queue has not been created during event getting. [POS_RET_ERROR_INNER]
///        - The flagID has not been registered during event getting. [POS_RET_ERROR_INNER]
///        - The interruption happened during event getting [POS_RET_ERROR_INNER]
///        - Whatever error happened during event getting. [POS_RET_ERROR_INNER]
///        - The HANDLE is NULL when getting usable share memory address for accessing
///          received data. [POS_RET_ERROR_INNER]
///        - Can not get usable share memory address for accessing received data. [POS_RET_ERROR_INNER]
///        - Memory for share memory map allocate failed for accessing received data. [POS_RET_ERROR_INNER]
///        - Memory for share memory management allocate failed for accessing received data. [POS_RET_ERROR_INNER]
///        - Can not open share memory for accessing received data. [POS_RET_ERROR_INNER]
///        - Failed to mapping share memory for accessing received data. [POS_RET_ERROR_INNER]
///        - The size of data stored in share memory is larger than the size of received data. [POS_RET_ERROR_INNER]
///        - The result of getting sensor data is POS_RET_ERROR_RESOURCE. [POS_RET_ERROR_RESOURCE]
///        - The result of getting sensor data is not POS_RET_ERROR_RESOURCE. [POS_RET_ERROR_INNER]
///
/// \~english @par Detail
///        - Call this API to get altitude from vehicle sensor
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Sync
///
/// \~english @see
///        - POS_RegisterListenerAltitude
////////////////////////////////////////////////////////////////////////////////////////////
//POS_RET_API POS_GetAltitude(HANDLE hApp, SENSORLOCATION_ALTITUDEINFO_DAT *dat, uint8_t ucGetMethod);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup tag_Positioning
/// \~english @par Brief
///        - Get speed data
///
/// \~english @param [in] hApp
///        - HANDLE - App Handle
/// \~english @param [out] dat
///        - SENSORMOTION_SPEEDINFO_DAT* - output buffer pointer to store speed data
/// \~english @param [in] ucGetMethod
///        - uint8_t - Get method(POS/Navi/Not specified)
///
/// \~english @par
///        - Get method(ucGetMethod)
///        - SENSOR_GET_METHOD_POS - The speed calculated in positioning based on speed pulse will be deliveried.
///        - SENSOR_GET_METHOD_NAVI - Navi  The speed from Navi will be deliveried.
///        - SENSOR_GET_METHOD_AUTO - Not specified  The speed will be deliveried according to current \n
///          environment.
///        - Avaliable method is descriped as following in each environment. \n
///          In the following environment, if the SENSOR_GET_METHOD_AUTO is set,treated as default \n
///          In the following environment, if the unsupported method has been specified, \n
///          POS_RET_ERROR_PARAM will be returned.
///
/// \~english @par
///        - SENSORMOTION_SPEEDINFO_DAT structure
/// \~english @code
///   typedef struct {
///   uint8_t   getMethod;    /* get method */
///   uint8_t   SyncCnt;      /* sync count */
///   uint8_t   isEnable;     /* enable or not */
///   uint8_t   isExistDR;    /* DR exist or not[not used] */
///   uint8_t   Reserve1[3];  /* Reserve */
///   uint8_t   DRStatus      /* DR status[not used] */
///   uint16_t  Speed;        /* speed(unit:0.01m/sec) */
///   uint8_t   Reserve2[2];  /* Reserve */
///   } SENSORMOTION_SPEEDINFO_DAT;
/// @endcode
///        - Get method(getMethod)
///        - SENSOR_GET_METHOD_POS - The speed calculated in positioning based on speed pulse will be deliveried.
///        - SENSOR_GET_METHOD_NAVI - speed from Navi
///        - Sync count(SyncCnt)
///        - 0.
///        - Enable or not(isEnable) \n
///          To describe this data is whether enable or not.
///        - 0 - not avaliable
///        - not 0 - avaliable
///        - Speed is invalid at following condition when speed pulse specified, so [not avaliable] provieded
///        - Immediately after system start, the sensor data have not been received from SYS micon
///        - If it is not initialization status, certainly provide [avaliable] when Navi data specified
///        - If the status is [not avaliable], data following can not be guaranteed.
///        - Speed
///        - speed data(unit 0.01m/sec)
///
/// \~english @retval POS_RET_NORMAL normal finish
/// \~english @retval POS_RET_ERROR_PARAM parameter error
/// \~english @retval POS_RET_ERROR_INNER internal error
/// \~english @retval POS_RET_ERROR_NOSUPPORT unsupported
/// \~english @retval POS_RET_ERROR_RESOURCE lack of resource
///
/// \~english @par Precondition
///        - The creation/initialization(FrameworkunifiedCreateDispatcherWithoutLoop and etc.) \n
///          of the dispatcher for App are completed.
///        - Availability of positioning service is TRUE.
///
/// \~english @par changes of internal status
///        - There is no changes of internal status
///
/// \~english @par Failure condition
///        - The parameter hApp is NULL [POS_RET_ERROR_PARAM]
///        - The parameter dat is NULL [POS_RET_ERROR_PARAM]
///        - The count of message in message queue is reach to  max [POS_RET_ERROR_RESOURCE]
///        - The count of mutex is reach to  max [POS_RET_ERROR_RESOURCE]
///        - The count of item in ProcessName-ProcessNo convert table is reach to  max [POS_RET_ERROR_RESOURCE]
///        - The event is created in same process, but the count of reference is reach to max [POS_RET_ERROR_INNER]
///        - The event is created in system, but the count of reference is reach to max  [POS_RET_ERROR_INNER]
///        - Memory allocate falied when the event table creation that for event registering. [POS_RET_ERROR_INNER]
///        - The thread can not register in the event table. [POS_RET_ERROR_INNER]
///        - Memory allocate failed in event table when the thread table creation. [POS_RET_ERROR_INNER]
///        - After register the thread table in event table, the event flag has already been \n
///          registered. [POS_RET_ERROR_INNER]
///        - After register the thread table in event table, the event flag register failed. [POS_RET_ERROR_INNER]
///        - The event table is full when event creation. [POS_RET_ERROR_INNER]
///        - The memory for event HANDLE allocate failed when event table creation. [POS_RET_ERROR_INNER]
///        - The thread can not be registered in event table. [POS_RET_ERROR_INNER]
///        - In event table, to allocate the memory of the thread table creation, but failed. [POS_RET_ERROR_INNER]
///        - After register the thread in event table, the event flag has already been registered. [POS_RET_ERROR_INNER]
///        - After register the thread in event table, the event flag register failed. [POS_RET_ERROR_INNER]
///        - Specified event ID has not been registered in table. [POS_RET_ERROR_INNER]
///        - Initialize event object failed. [POS_RET_ERROR_INNER]
///        - The memory for storing semaphore control data allocate failed. [POS_RET_ERROR_INNER]
///        - Specified semaphore ID has not been registered when semaphore lock. [POS_RET_ERROR_INNER]
///        - Internal mutex HANDLE is NULL when mutex lock. [POS_RET_ERROR_INNER]
///        - Internal mutex HANDLE has not been registered in mutex table when mutex lock. [POS_RET_ERROR_INNER]
///        - The owner of specified mutex is not itself when mutex lock. [POS_RET_ERROR_INNER]
///        - Mutex has been multiple locked [POS_RET_ERROR_INNER]
///        - The HANDLE is NULL when getting usable share memory address. [POS_RET_ERROR_INNER]
///        - Can not get usable share memory address. [POS_RET_ERROR_INNER]
///        - Memory allocate failed for share memory map. [POS_RET_ERROR_INNER]
///        - Memory allocate failed for share memory management. [POS_RET_ERROR_INNER]
///        - Can not open share memory. [POS_RET_ERROR_INNER]
///        - Failed to mapping share memory. [POS_RET_ERROR_INNER]
///        - No empty field in share memory. [POS_RET_ERROR_INNER]
///        - Failed to guarantee share memory. [POS_RET_ERROR_INNER]
///        - ProcessNo has not been registered in message control table when message transfered \n
///          between processes. [POS_RET_ERROR_INNER]
///        - Message transfer HANDLE get failed when message transfered between processes. [POS_RET_ERROR_INNER]
///        - Message transfer failed between processes. [POS_RET_ERROR_INNER]
///        - The destination process name size is larger than 20 characters when message transfer \n
///          between processes. [POS_RET_ERROR_INNER]
///        - The message queue name has not been registered in control table when message transfer \n
///          between processes [POS_RET_ERROR_INNER]
///        - Message transfer HANDLE create failed when message transfered between processes. [POS_RET_ERROR_INNER]
///        - Message transfer HANDLE get failed from internal table when message transfered between \n
///          processes. [POS_RET_ERROR_INNER]
///        - Message transfer failed between processes. [POS_RET_ERROR_INNER]
///        - Specified event HANDLE has not been registered in event HANDLE table during event \n
///          waiting. [POS_RET_ERROR_INNER]
///        - The count of thread is reach to max in event management table during event waiting. [POS_RET_ERROR_INNER]
///        - The EV_FLAG_BIT is not set in flagID during event getting. [POS_RET_ERROR_INNER]
///        - The ID of message event queue has not been created during event getting. [POS_RET_ERROR_INNER]
///        - The flagID has not been registered during event getting. [POS_RET_ERROR_INNER]
///        - The interruption happened during event getting [POS_RET_ERROR_INNER]
///        - Whatever error happened during event getting. [POS_RET_ERROR_INNER]
///        - The HANDLE is NULL when getting usable share memory address for accessing \n
///          received data. [POS_RET_ERROR_INNER]
///        - Can not get usable share memory address for accessing received data. [POS_RET_ERROR_INNER]
///        - Memory for share memory map allocate failed for accessing received data. [POS_RET_ERROR_INNER]
///        - Memory for share memory management allocate failed for accessing received data. [POS_RET_ERROR_INNER]
///        - Can not open share memory for accessing received data. [POS_RET_ERROR_INNER]
///        - Failed to mapping share memory for accessing received data. [POS_RET_ERROR_INNER]
///        - The size of data stored in share memory is larger than the size of received data. [POS_RET_ERROR_INNER]
///        - The result of getting sensor data is POS_RET_ERROR_RESOURCE. [POS_RET_ERROR_RESOURCE]
///        - The result of getting sensor data is not POS_RET_ERROR_RESOURCE. [POS_RET_ERROR_INNER]
///
/// \~english @par Detail
///        - Call this API to get speed from vehicle sensor.
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Sync
///
/// \~english @see
///        - POS_RegisterListenerSpeed
///
////////////////////////////////////////////////////////////////////////////////////////////
//POS_RET_API POS_GetSpeed(HANDLE hApp, SENSORMOTION_SPEEDINFO_DAT *dat, uint8_t ucGetMethod);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup tag_Positioning
/// \~english @par Brief
///        - Get heading data
///
/// \~english @param [in] hApp
///        - HANDLE - App Handle
/// \~english @param [out] dat
///        - SENSORMOTION_HEADINGINFO_DAT* - output buffer pointer to store heading data
/// \~english @param [in] ucGetMethod
///        - uint8_t - Get method(GPS/Navi/Not specified)
///
/// \~english @par
///        - Get method(ucGetMethod)
///        - SENSOR_GET_METHOD_GPS - GPS  The altitude from GPS will be deliveried.
///        - SENSOR_GET_METHOD_NAVI - Navi  The altitude from Navi will be deliveried.
///        - Avaliable method is descriped as following in each environment. \n
///          In corresponding environment, the SENSOR_GET_METHOD_AUTO is set as default. \n
///          In corresponding environment, if the unsupported method has been specified, \n
///          POS_RET_ERROR_PARAM will be returned.
/// \~english @par
///        - SENSORMOTION_HEADINGINFO_DAT structure
/// \~english @code
///   typedef struct {
///   uint8_t   getMethod;    /* get method */
///   uint8_t   SyncCnt;      /* sync count */
///   uint8_t   isEnable;     /* enable or not */
///   uint8_t   isExistDR;    /* DR exist or not[not used] */
///   uint8_t   DRStatus      /* DR status[not used] */
///   uint8_t   posSts;       /* position status */
///   uint8_t   Reserve1[2];  /* Reserve */
///   uint16_t  Heading;      /* heading(unit:0.01degree) */
///   uint8_t   Reserve2[2];  /* Reserve */
///   } SENSORMOTION_HEADINGINFO_DAT;
/// @endcode
///        - Get method(getMethod)
///        - SENSOR_GET_METHOD_GPS - Heading from GPS
///        - SENSOR_GET_METHOD_NAVI - Heading from Navi
///        - Synchrony count(SyncCnt)
///        - Count for position data synchronous \n
///          When delivery altitude and heading data, position data can be synchronized by this count. \n
///          But the data of different method can not be synchronized by this count. \n
///          example 1: [longitude and latitude from GPS] and [heading from GPS] can be synchronized by the count. \n
///          example 2: [longitude and latitude from GPS] and [longitude and latitude from Navi] can not be \n
///          synchronized by the count. \n
///          Caution: The sensor count in sensor data delivery is another data.
///        - Enable or not(isEnable) \n
///          To describe this data is whether enable or not.
///        - 0 - not avaliable
///        - not 0 - avaliable
///        - Heading is invalid at following condition when GPS data specified, so [not avaliable] provieded
///        - Immediately after system start, GPS unit has not received current location data and GPS unit \n
///          status is not positioning fix
///        - If it is not initialization status, certainly provide [avaliable] when Navi data specified
///        - If the status is [not avaliable], data following can not be guaranteed.
///        - Position status(posSts)
///        - Bit0 : GPS data used result(1:used, 0:not used, definition of maskbit:POS_LOC_INFO_USE_GSP)
///        - Bit1 : DGPS data used result(1:used, 0:not used, definition of maskbit:POS_LOC_INFO_USE_DGPS)
///        - Bit2 : DR data used result(1:used, 0:not used, definition of maskbit:POS_LOC_INFO_USE_DR)
///        - Bit3 : MapMatching data use result(1:used, 0:not used, definition of maskbit:POS_LOC_INFO_USE_MAPMATCHING)
///        - Heading
///
/// \~english @retval POS_RET_NORMAL normal finish
/// \~english @retval POS_RET_ERROR_PARAM parameter error
/// \~english @retval POS_RET_ERROR_INNER internal error
/// \~english @retval POS_RET_ERROR_NOSUPPORT unsupported
/// \~english @retval POS_RET_ERROR_RESOURCE lack of resource
///
/// \~english @par Precondition
///        - The creation/initialization(FrameworkunifiedCreateDispatcherWithoutLoop and etc.) of the Dispatcher
///        - for App are completed.
///        - Availability of positioning service is TRUE.
///
/// \~english @par changes of internal status
///        - There is no changes of internal status
///
/// \~english @par Failure condition
///        - The parameter hApp is NULL [POS_RET_ERROR_PARAM]
///        - The parameter dat is NULL [POS_RET_ERROR_PARAM]
///        - The count of message in message queue is reach to  max [POS_RET_ERROR_RESOURCE]
///        - The count of mutex is reach to  max [POS_RET_ERROR_RESOURCE]
///        - The count of item in ProcessName-ProcessNo convert table is reach to max [POS_RET_ERROR_RESOURCE]
///        - The event is created in same process, but the count of reference is reach to max [POS_RET_ERROR_INNER]
///        - The event is created in system, but the count of reference is reach to max  [POS_RET_ERROR_INNER]
///        - Memory allocate falied during the event table creation for event registering. [POS_RET_ERROR_INNER]
///        - The thread can not register in the event table. [POS_RET_ERROR_INNER]
///        - Memory allocate failed in event table during the thread table creation. [POS_RET_ERROR_INNER]
///        - After register the thread table in event table, the event flag has already been \n
///          registered. [POS_RET_ERROR_INNER]
///        - After register the thread table in event table, the event flag register failed. [POS_RET_ERROR_INNER]
///        - The event table is full during event creation. [POS_RET_ERROR_INNER]
///        - The memory for event HANDLE allocate failed during event table creation. [POS_RET_ERROR_INNER]
///        - The thread can not be registered in event table. [POS_RET_ERROR_INNER]
///        - In event table, try to allocate the memory of the thread table creation, but failed. [POS_RET_ERROR_INNER]
///        - After register the thread in event table, the event flag has already been registered. [POS_RET_ERROR_INNER]
///        - After register the thread in event table, the event flag register failed. [POS_RET_ERROR_INNER]
///        - Specified event ID has not been registered in table. [POS_RET_ERROR_INNER]
///        - Initialize event object failed. [POS_RET_ERROR_INNER]
///        - The memory for storing semaphore control data allocate failed. [POS_RET_ERROR_INNER]
///        - Specified semaphore ID has not been registered when semaphore lock. [POS_RET_ERROR_INNER]
///        - Internal mutex HANDLE is NULL when mutex lock. [POS_RET_ERROR_INNER]
///        - Internal mutex HANDLE has not been registered in mutex table when mutex lock. [POS_RET_ERROR_INNER]
///        - The owner of specified mutex is not itself when mutex lock. [POS_RET_ERROR_INNER]
///        - Mutex has been multiple locked [POS_RET_ERROR_INNER]
///        - The HANDLE is NULL when getting usable share memory address. [POS_RET_ERROR_INNER]
///        - Can not get usable share memory address. [POS_RET_ERROR_INNER]
///        - Memory allocate failed for share memory map. [POS_RET_ERROR_INNER]
///        - Memory allocate failed for share memory management. [POS_RET_ERROR_INNER]
///        - Can not open share memory. [POS_RET_ERROR_INNER]
///        - Failed to mapping share memory. [POS_RET_ERROR_INNER]
///        - No empty field in share memory. [POS_RET_ERROR_INNER]
///        - Failed to guarantee share memory. [POS_RET_ERROR_INNER]
///        - ProcessNo has not been registered in message control table when message transfered \n
///          between processes. [POS_RET_ERROR_INNER]
///        - Message transfer HANDLE get failed when message transfered between processes. [POS_RET_ERROR_INNER]
///        - Message transfer failed between processes. [POS_RET_ERROR_INNER]
///        - The destination process name size is larger than 20 characters when message transfer \n
///          between processes. [POS_RET_ERROR_INNER]
///        - The message queue name has not been registered in control table when message transfer \n
///          between processes. [POS_RET_ERROR_INNER]
///        - Message transfer HANDLE create failed when message transfered between processes. [POS_RET_ERROR_INNER]
///        - Message transfer HANDLE get failed from internal table when message transfered between \n
///          processes. [POS_RET_ERROR_INNER]
///        - Message transfer failed between processes. [POS_RET_ERROR_INNER]
///        - Specified event HANDLE has not been registered in event HANDLE table \n
///          during event waiting[POS_RET_ERROR_INNER]
///        - The count of thread is reach to  max in event management table during event waiting. [POS_RET_ERROR_INNER]
///        - The EV_FLAG_BIT is not set in flagID during event getting. [POS_RET_ERROR_INNER]
///        - The ID of message event queue has not been created during event getting. [POS_RET_ERROR_INNER]
///        - The flagID has not been registered during event getting. [POS_RET_ERROR_INNER]
///        - The interruption happened during event getting [POS_RET_ERROR_INNER]
///        - Whatever error happened during event getting. [POS_RET_ERROR_INNER]
///        - The HANDLE is NULL when getting usable share memory address for accessing received \n
///          data. [POS_RET_ERROR_INNER]
///        - Can not get usable share memory address for accessing received data. [POS_RET_ERROR_INNER]
///        - Memory for share memory map allocate failed for accessing received data. [POS_RET_ERROR_INNER]
///        - Memory for share memory management allocate failed for accessing received data. [POS_RET_ERROR_INNER]
///        - Can not open share memory for accessing received data. [POS_RET_ERROR_INNER]
///        - Failed to mapping share memory for accessing received data. [POS_RET_ERROR_INNER]
///        - The size of data stored in share memory is larger than the size of received data. [POS_RET_ERROR_INNER]
///        - The result of getting sensor data is POS_RET_ERROR_RESOURCE. [POS_RET_ERROR_RESOURCE]
///        - The result of getting sensor data is not POS_RET_ERROR_RESOURCE. [POS_RET_ERROR_INNER]
///
/// \~english @par Detail
///        - Call this API to get heading from vehicle sensor.
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Sync
///
/// \~english @see
///        - POS_RegisterListenerHeading
///
////////////////////////////////////////////////////////////////////////////////////////////
//POS_RET_API POS_GetHeading(HANDLE hApp, SENSORMOTION_HEADINGINFO_DAT *dat, uint8_t ucGetMethod);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup tag_Positioning
/// \~english @par Brief
///        - Set speed information
///
/// \~english @param [in] hApp
///        - HANDLE - App Handle
/// \~english @param [in] navispeed
///        - uint16_t - speed data[unit: 1.0km/h]
///
/// \~english @retval POS_RET_NORMAL normal finish
/// \~english @retval POS_RET_ERROR_PARAM parameter error
/// \~english @retval POS_RET_ERROR_INNER internal error
/// \~english @retval POS_RET_ERROR_NOSUPPORT unsupported
/// \~english @retval POS_RET_ERROR_RESOURCE lack of resource
///
/// \~english @par Precondition
///        - The creation/initialization(FrameworkunifiedCreateDispatcherWithoutLoop and etc.) of \n
///          the Dispatcher for App are completed.
///        - Availability of positioning service is TRUE.
///
/// \~english @par changes of internal status
///        - There is no changes of internal status
///
/// \~english @par Failure condition
///        - The parameter hApp is NULL [POS_RET_ERROR_PARAM]
///        - The data size is larger than specified value(POS_MSG_INFO_DSIZE) [POS_RET_ERROR_RESOURCE]
///        - The count of message in message queue is reach to  max [POS_RET_ERROR_RESOURCE]
///        - The count of mutex is reach to max [POS_RET_ERROR_RESOURCE]
///        - The count of item in ProcessName-ProcessNo convert table is reach to max [POS_RET_ERROR_RESOURCE]
///        - The message queue name has not been registered in control table. [POS_RET_ERROR_INNER]
///        - Message transfer HANDLE create failed. [POS_RET_ERROR_INNER]
///        - Message transfer HANDLE get failed from internal table. [POS_RET_ERROR_INNER]
///        - Speed data setting message transfer failed. [POS_RET_ERROR_INNER]
///
/// \~english @par Detail
///        - Call this API to set speed to vehicle sensor. \n
///          This API will finish when get the return value. \n
///          The speed set by this API is saved in positioning as the speed calculated by Navi.
///
/// \~english @par
///        - Please note the following points when use this API.
///        - This API is only called by Navi proxy.
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Fire and Forget
///
/// \~english @see
///        - None
///
////////////////////////////////////////////////////////////////////////////////////////////
POS_RET_API POS_SetSpeedInfo(HANDLE hApp, uint16_t navispeed);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup tag_Positioning
/// \~english @par Brief
///        - Set location information
///
/// \~english @param [in] hApp
///        - HANDLE - App Handle
/// \~english @param [in] pstPosData
///        - POS_POSDATA * - pointer to location information
///
/// \~english @par
///        - POS_POSDATA structure
/// \~english @code
/// typedef struct
/// {
///   int8_t    status;       /* data status */
///   uint8_t   posSts;       /* position status */
///   uint16_t  posAcc;       /* Position accuracy 1LSB=1m */
///   int32_t   Longitude;    /* Longitude(unit:1/128sec) */
///   int32_t   Latitude;     /* Latitude(unit:1/128sec) */
///   int32_t   altitude;     /* altitude (unit:0.01m) */
///   int16_t   heading;      /* heading (unit:1degree) */
///   uint8_t   reserved[2];  /* reserve */
///   } POS_POSDATA;
/// @endcode
///
/// \~english @par
///        - data status(status) \n
///          Do not set the invalid data(do not update data).
///        - data valid/invalid status
///        - Bit0 : latitude(1:valid, 0:invalid, definition of maskbit:POS_LOC_INFO_LAT)
///        - Bit1 : longitude(1:valid, 0:invalid, definition of maskbit:POS_LOC_INFO_LON)
///        - Bit2 : altitude (1:valid, 0:invalid, definition of maskbit:POS_LOC_INFO_ALT)
///        - Bit3 : heading (1:valid, 0:invalid, definition of maskbit:POS_LOC_INFO_HEAD)
///        - Bit4~7 : reserve
///        - Position status(posSts)
///        - Bit0 : GPS data used result(1:used, 0:not used, definition of maskbit:POS_LOC_INFO_USE_GSP)
///        - Bit1 : DGPS data used result(1:used, 0:not used, definition of maskbit:POS_LOC_INFO_USE_DGPS)
///        - Bit2 : DR data used result(1:used, 0:not used, definition of maskbit:POS_LOC_INFO_USE_DR)
///        - Bit3 : MapMatching data use result(1:used, 0:not used, definition of maskbit:POS_LOC_INFO_USE_MAPMATCHING)
///        - Bit4~7 : reserve
///        - Position accuracy(posAcc)
///        - Detected accruray of current position:1LSB=1m
///        - longitude
///        - data range: -180deg ~ +180deg
///        - +: east longitude -: west longitude
///        - latitude
///        - data range:-90deg ~ +90deg
///        - +: north latitude -: south latitude
///        - heading
///        - data range:-179deg ~ +180deg
///        - Based on north, west(counter-clockwise) is +, east(clockwise) is -
///
/// \~english @retval POS_RET_NORMAL normal finish
/// \~english @retval POS_RET_ERROR_PARAM parameter error
/// \~english @retval POS_RET_ERROR_INNER internal error
/// \~english @retval POS_RET_ERROR_NOSUPPORT unsupported
/// \~english @retval POS_RET_ERROR_RESOURCE lack of resource
///
/// \~english @par Precondition
///        - The creation/initialization(FrameworkunifiedCreateDispatcherWithoutLoop and etc.) of the Dispatcher \n
///          for App are completed.
///        - Availability of positioning service is TRUE.
///
/// \~english @par change of internal status
///        - There is no change of internal status
///
/// \~english @par Failure condition
///        - The parameter pstPosData is NULL [POS_RET_ERROR_PARAM]
///        - The parameter hApp is NULL [POS_RET_ERROR_PARAM]
///        - The data status in parameter pstPosData is invalid value(0). [POS_RET_ERROR_PARAM]
///        - The data status in parameter pstPosData is abnormal value(<15). [POS_RET_ERROR_PARAM]
///        - The data size is larger than specified value(POS_MSG_INFO_DSIZE) [POS_RET_ERROR_RESOURCE]
///        - The count of message in message queue is reach to max [POS_RET_ERROR_RESOURCE]
///        - The count of mutex is reach to max [POS_RET_ERROR_RESOURCE]
///        - The count of item in ProcessName-ProcessNo convert table is reach to max [POS_RET_ERROR_RESOURCE]
///        - The message queue name has not been registered in control table. [POS_RET_ERROR_INNER]
///        - Message transfer HANDLE create failed. [POS_RET_ERROR_INNER]
///        - Message transfer HANDLE get failed from internal table. [POS_RET_ERROR_INNER]
///        - Location data setting message transfer failed. [POS_RET_ERROR_INNER]
///
/// \~english @par Detail
///        - Call this API to set location to vehicle sensor. \n
///          This API will finish when get the return value. \n
///          The location set by this API is saved in positioning as the location calculated by Navi. \n
///          If one of longitude and latitude is valid, the another data will be used as valid data no \n
///          matter what status it is. \n
///          If one of longitude and latitude is valid, the position accuracy data will be used as valid data.
///
/// \~english @par
///        - Please note the following points when use this API.
///        - This API is only called by Navi proxy.
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Fire and Forget
///
/// \~english @see
///        - None
///
////////////////////////////////////////////////////////////////////////////////////////////
POS_RET_API POS_SetLocationInfo(HANDLE hApp, POS_POSDATA* pstPosData);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup tag_Positioning
/// \~english @par Brief
///        - Set location NMEA information
///
/// \~english @param [in] hApp
///        - HANDLE - App Handle
/// \~english @param [in] locationInfo
///        - POS_POSDATA * - pointer to location information NMEA
///
/// \~english @retval POS_RET_NORMAL normal finish
/// \~english @retval POS_RET_ERROR_PARAM parameter error
/// \~english @retval POS_RET_ERROR_INNER internal error
/// \~english @retval POS_RET_ERROR_NOSUPPORT unsupported
/// \~english @retval SENSOR_RET_ERROR_NOSUPPORT unsupported
/// \~english @retval POS_RET_ERROR_RESOURCE lack of resource
///
/// \~english @par Precondition
///        - The creation/initialization(FrameworkunifiedCreateDispatcherWithoutLoop and etc.) of \n
///          the Dispatcher for App are completed.
///        - Availability of positioning service is TRUE.
///
/// \~english @par changes of internal status
///        - There is no changes of internal status
///
/// \~english @par Failure condition
///        - The parameter hApp is NULL [POS_RET_ERROR_PARAM]
///        - The length in parameter locationInfo is invalid value(0). [POS_RET_ERROR_PARAM]
///        - The length in parameter locationInfo is abnormal value(> LOCATIONINFO_NMEA_MAX). [POS_RET_ERROR_PARAM]
///        - The count of message in message queue is reach to  max [POS_RET_ERROR_RESOURCE]
///        - The count of mutex is reach to max [POS_RET_ERROR_RESOURCE]
///        - The count of item in ProcessName-ProcessNo convert table is reach to max [POS_RET_ERROR_RESOURCE]
///        - The message queue name has not been registered in control table. [POS_RET_ERROR_INNER]
///        - Message transfer HANDLE create failed. [POS_RET_ERROR_INNER]
///        - Message transfer HANDLE get failed from internal table. [POS_RET_ERROR_INNER]
///        - Location NMEA data setting message transfer failed. [POS_RET_ERROR_INNER]
///
/// \~english @par Detail
///        - Call this API to set location nmea to vehicle sensor. \n
///          This API will finish when get the return value. \n
///          The location set by this API is saved in positioning as the location calculated by Navi. \n
///
/// \~english @par
///        - Please note the following points when use this API.
///        - This API is only called by Navi proxy.
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Fire and Forget
///
/// \~english @see
///        - None
///
////////////////////////////////////////////////////////////////////////////////////////////yy
POS_RET_API POS_SetLocationInfoNmea(HANDLE hApp, POS_LOCATIONINFO_NMEA* locationInfo);

#ifdef __cplusplus
}
#endif
/** @}*/  // end of positioning
/** @}*/  // end of vehicle_service
/** @}*/  // end of BaseSystem
#endif  // POSITIONING_CLIENT_INCLUDE_VEHICLE_SERVICE_POS_COMMON_API_H_
