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
#ifndef POSITIONING_CLIENT_INCLUDE_VEHICLE_SERVICE_POS_GPS_API_H_
#define POSITIONING_CLIENT_INCLUDE_VEHICLE_SERVICE_POS_GPS_API_H_
/**
 * @file POS_gps_API.h
 * @brief API definition file for GPS function
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
 * Typedef declaration                                                             *
 *---------------------------------------------------------------------------------*/
typedef int32_t NAVIINFO_RET_API;
//!< \~english define return value of Get/Set GPS information API

/*---------------------------------------------------------------------------------*
 * Event declaration                                                             *
 *---------------------------------------------------------------------------------*/


/**
 * \~english @brief  GPS time setting result delivery command ID
 * \~english @brief  If you want to catch above envents, use NSFW like below.
 * \~english @code  
 * l_eStatus = FrameworkunifiedAttachCallbackToDispatcher(test_params_->h_app,"POS_Main", CID_POSIF_REGISTER_LISTENER_GPS_TIME_SET_REQ, CBCallbackA);
 *            @endcode  
 */
#define CID_POSIF_REGISTER_LISTENER_GPS_TIME_SET_REQ  0x0780

/*---------------------------------------------------------------------------------*
 * Struct declaration                                                              *
 *---------------------------------------------------------------------------------*/
/**
 * @struct SENSOR_MSG_SEND_DAT
 * \~english message for setting vehicle sensor data(to vehicle sensor)
 */
typedef struct {
  DID         did;        //!< \~english data ID
  uint16_t    usSize;     //!< \~english data size
  uint8_t     data[502];  //!< \~english data body
} SENSOR_MSG_SEND_DAT;

/*---------------------------------------------------------------------------------*
 * Prototype Declaration                                                           *
 *---------------------------------------------------------------------------------*/
/* GPS_API public API */
#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup tag_Positioning
/// \~english @par Brief
///        - GPS setting request
///
/// \~english @param [in] hApp
///        - HANDLE - App Handle
/// \~english @param [in] p_data
///        - SENSOR_MSG_SEND_DAT * - pointer of GPS setting data
///
/// \~english @par
///        - SENSOR_MSG_SEND_DAT structure
/// \~english @code
///   typedef struct
///   {
///     DID       did;        /* Data ID */
///     uint16_t  usSize;     /* Data size */
///     uint8_t   data[502];  /* Data body */
///   } SENSOR_MSG_SEND_DAT;
/// @endcode
///
/// \~english @par
///        - Data ID(did)
///        - POS_DID_GPS__CWORD82__SETINITIAL - setting initial location and time data to GPS  \n
///          (reference to initial location, time setting(_CWORD82_))
///        - POS_DID_GPS__CWORD82__SETRMODEEX - setting GPS receiver mode(pull extension sentence) \n
///          (reference to GPS receiver mode setting(_CWORD82_))
///        - POS_DID_GPS__CWORD82__SELSENT    - setting command to GPS that output any sentence  \n
///          (reference to output any sentence setting(_CWORD82_))
///
///
/// \~english @retval SENSOR_RET_NORMAL normal finish
/// \~english @retval SENSOR_RET_ERROR_CREATE_EVENT event create error
/// \~english @retval SENSOR_RET_ERROR_PARAM parameter error
/// \~english @retval SENSOR_RET_ERROR_DID data ID not registered
/// \~english @retval SENSOR_RET_ERROR_NOSUPPORT unsupported
/// \~english @retval SENSOR_RET_ERROR_RESOURCE lack of resource
///
/// \~english @par Precondition
///        - The creation/initialization(FrameworkunifiedCreateDispatcherWithoutLoop and etc.) \n
///          of the Dispatcher for App are completed.
///        - Availability of positioning service is TRUE.
///
/// \~english @par changes of internal status
///        - There is no changes of internal status by this API
///
/// \~english @par Failure condition
///        - The parameter hApp is NULL [SENSOR_RET_ERROR_PARAM]
///        - The parameter p_data is NULL [SENSOR_RET_ERROR_PARAM]
///        - Data ID(p_data->did) is not supported [SENSOR_RET_ERROR_DID]
///        - GPS setting data size(p_data->usSize) is not same with the data  \n
///          that related to data ID [SENSOR_RET_ERROR_PARAM]
///        - The count of message in message queue is reach to max [SENSOR_RET_ERROR_RESOURCE]
///        - The count of mutex is reach to max [SENSOR_RET_ERROR_RESOURCE]
///        - The count of item in ProcessName-ProcessNo convert table is reach to max [SENSOR_RET_ERROR_RESOURCE]
///        - The message queue name has not been registered in control table when GPS \n
///          setting message transfer between processes. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - Message transfer HANDLE create failed. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - Message transfer HANDLE get failed from internal table. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - GPS setting message transfer failed. [SENSOR_RET_ERROR_CREATE_EVENT]
///
/// \~english @par Detail
///        - Call this API to set GPS data. \n
///          The GPS data specified by the parameter p_data will be set in positioning.
///
/// \~english @par
///        - Please note the following points when use this API.
///        - This API is only called by Navigation proxy.
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
int32_t POS_ReqGPSSetting(HANDLE hApp, SENSOR_MSG_SEND_DAT *p_data);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup tag_Positioning
/// \~english @par Brief
///        - Set GPS information
///
/// \~english @param [in] hApp
///        - HANDLE - App Handle
/// \~english @param [in] navilocinfo
///        - NAVIINFO_ALL * - pointer of GPS information
///
/// \~english @par
///        - NAVIINFO_ALL structure
/// \~english @code
///   typedef struct
///   {
///     uint8_t           ucSensorCnt;  /* sensor count */
///     uint8_t           reserve[3];   /* reserve */
///     NAVIINFO_DIAG_GPS stDiagGps;    /* position fix related information */
///     NAVIINFO_NAVI_GPS stNaviGps;    /* other GPS related information */
///   } NAVIINFO_ALL;
/// @endcode
///        - sensor count \n
///          sensor count value when GPS data received
///
/// \~english @par
///        - NAVIINFO_DIAG_GPS structure
/// \~english @code
///   typedef struct
///   {
///     NAVIINFO_DIAG_GPS_FIX  stFix;  /* position fix information */
///     NAVIINFO_DIAG_GPS_SAT  stSat;  /* all satellite information */
///   } NAVIINFO_DIAG_GPS;
/// @endcode
///        \n
/// \~english @par
///        - NAVIINFO_DIAG_GPS_FIX structure
/// \~english @code
///   typedef struct
///   {
///     uint8_t                   ucFixSts;     /* position fix status */
///     uint8_t                   ucReserve[3]; /* reserve */
///     NAVIINFO_DIAG_GPS_FIX_CNT stCnt;        /* position fix count data */
///     NAVIINFO_DIAG_GPS_FIX_XYZ stWgs84;      /* lonlat data(WGS84 geodetic)  */
///   } NAVIINFO_DIAG_GPS_FIX;
/// @endcode
///        - position fix status(ucFixSts)
///        - NAVIINFO_DIAG_GPS_FIX_STS_NON:not fixed
///        - NAVIINFO_DIAG_GPS_FIX_STS_2D :2D fix
///        - NAVIINFO_DIAG_GPS_FIX_STS_3D :3D fix
/// \~english @par
///        - NAVIINFO_DIAG_GPS_FIX_CNT structure
/// \~english @code
///   typedef struct
///   {
///     uint32_t  ulCnt3d;    /* position fix ratio:3D(unit:sec) */
///     uint32_t  ulCnt2d;    /* position fix ratio:2D(unit:sec) */
///     uint32_t  ulCntElse;  /* position fix ratio:not fix(unit:sec) */
///   } NAVIINFO_DIAG_GPS_FIX_CNT;
/// @endcode
///
/// \~english @par
///        - NAVIINFO_DIAG_GPS_FIX_XYZ structure
/// \~english @code
///   typedef struct
///   {
///     int32_t   lLat;    /* GPS latitude(unit:1/256sec) (+: north latitude,-: south latitude) */
///     int32_t   lLon;    /* GPS longitude(unit:1/256sec) (+: east longitude, -: west longitude) */
///   } NAVIINFO_DIAG_GPS_FIX_XYZ;
/// @endcode
///
/// \~english @par
///        - NAVIINFO_DIAG_GPS_SAT structure
/// \~english @code
///   typedef struct
///   {
///     NAVIINFO_DIAG_GPS_PRN   stPrn[12];  /* all satellite information */
///   } NAVIINFO_DIAG_GPS_SAT;
/// @endcode
///
/// \~english @par
///        - NAVIINFO_DIAG_GPS_PRN structure
/// \~english @code
///   typedef struct
///   {
///     uint8_t   ucRcvSts;     /* reception status */
///     uint8_t   ucPrn;        /* satellite No. */
///     uint8_t   ucelv;        /* satellite angle(unit:1.0deg) */
///     uint8_t   ucLv;         /* satellite level */
///     uint16_t  usAzm;        /* satellite azimuth(unit:1.0deg) (clockwise from north) */
///     uint8_t   ucReserve[2]; /* reserve */
///   } NAVIINFO_DIAG_GPS_PRN;
/// @endcode
///        - reception status(ucRcvSts)
///        - NAVIINFO_DIAG_GPS_RCV_STS_NOTUSE :not used
///        - NAVIINFO_DIAG_GPS_RCV_STS_SEARCHING :searching
///        - NAVIINFO_DIAG_GPS_RCV_STS_TRACHING :tracking
///        - NAVIINFO_DIAG_GPS_RCV_STS_NOTUSEFIX :not used for position fix
///        - NAVIINFO_DIAG_GPS_RCV_STS_USEFIX :used for position fix
/// \~english @par
///        - NAVIINFO_NAVI_GPS structure
/// \~english @code
///   typedef struct
///   {
///     int32_t           altitude;   /* altitude(unit:1m) */
///     uint16_t          speed;      /* speed(unit:1.00km/h) */
///     uint16_t          heading;    /* heading(unit:0.1deg) (clockwise from north) */
///     NAVIINFO_UTCTIME  utc;        /* UTC time */
///     uint8_t           tdsts;      /* date and time status */
///     uint8_t           reserve[3]; /* reserve */
///   } NAVIINFO_NAVI_GPS;
/// @endcode
///        - altitude(altitude) \n
///          As the altitude is used as unit [0.01m] in internal, \n
///          the altitude data range is (-21,474,839~21,474,839).
///        - UTC(utc)
///        - The time set after rollover.
///        - date and time status(tdsts)
///        - 0= time has not been adjusted after GPS receiver reset(time input or master reset or CSF start)
///        - 1= time output from RTC Backup(have time adjustment result)
///        - 2= time adjustment completed
/// \~english @par
///        - NAVIINFO_UTCTIME structure
/// \~english @code
///   typedef struct {
///     uint16_t    year;     /* A.D. (1~) */
///     uint8_t     month;    /* month(1~12) */
///     uint8_t     date;     /* date(1~31) */
///     uint8_t     hour;     /* hour(0~23) */
///     uint8_t     minute;   /* minute(0~59) */
///     uint8_t     second;   /* second(0~59) */
///     uint8_t     reserved; /* not used */
///   } NAVIINFO_UTCTIME;
/// @endcode
///
/// \~english @retval NAVIINFO_RET_NORMAL normal finish
/// \~english @retval NAVIINFO_RET_ERROR_PARAM parameter error
/// \~english @retval NAVIINFO_RET_ERROR_INNER internal error
/// \~english @retval NAVIINFO_RET_ERROR_NOSUPPORT unsupported
/// \~english @retval NAVIINFO_RET_ERROR_RESOURCE lack of resource
///
/// \~english @par Precondition
///        - The creation/initialization(FrameworkunifiedCreateDispatcherWithoutLoop and etc.) \n
///          of the Dispatcher for App are completed.
///        - Availability of positioning service is TRUE.
///
/// \~english @par changes of internal status
///        - There is no changes of internal status
///
/// \~english @par Failure condition
///        - The parameter navilocinfo is NULL [NAVIINFO_RET_ERROR_PARAM]
///        - The parameter hApp is NULL [NAVIINFO_RET_ERROR_PARAM]
///        - GPS setting data size(p_data->usSize) is not same as the data that \n
///          related to data ID [NAVIINFO_RET_ERROR_RESOURCE]
///        - The count of message in message queue is reach to max [NAVIINFO_RET_ERROR_RESOURCE]
///        - The count of mutex is reach to max [NAVIINFO_RET_ERROR_RESOURCE]
///        - The count of item in ProcessName-ProcessNo convert table is reach to max [SENSOR_RET_ERROR_RESOURCE]
///        - The message queue name has not been registered in control table. [NAVIINFO_RET_ERROR_INNER]
///        - Message transfer HANDLE create failed. [NAVIINFO_RET_ERROR_INNER]
///        - Message transfer HANDLE get failed from internal table. [NAVIINFO_RET_ERROR_INNER]
///        - GPS setting message transfer failed between processes. [NAVIINFO_RET_ERROR_INNER]
///
/// \~english @par Detail
///        - Call this API to set GPS information to vehicle sensor.\n
///        - This API will finish when get the return value.
///
/// \~english @par
///        - Please note the following points when use this API.
///        - This API is only called by Navigation proxy.
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Fire and Forget and Method and Fire and Forget and Fire and Forget
///
/// \~english @see
///        - POS_GetGPSInfo
////////////////////////////////////////////////////////////////////////////////////////////
//NAVIINFO_RET_API POS_SetGPSInfo(HANDLE hApp, NAVIINFO_ALL *navilocinfo);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup tag_Positioning
/// \~english @par Brief
///        - Get GPS data
///
/// \~english @param [in] hApp
///        - HANDLE - App Handle
/// \~english @param [out] navidiaginfo
///        - NAVIINFO_DIAG_GPS* - pointer to get GPS data
///
/// \~english @par
///        - NAVIINFO_DIAG_GPS structure
/// \~english @code
///   typedef struct
///   {
///     NAVIINFO_DIAG_GPS_FIX stFix;  /* position fix information */
///     NAVIINFO_DIAG_GPS_SAT stSat;  /* all satellite information */
///   } NAVIINFO_DIAG_GPS;
/// @endcode
/// \~english @par
///        - NAVIINFO_DIAG_GPS_FIX structure
/// \~english @code
///   typedef struct
///   {
///     uint8_t                   ucFixSts;     /* position fix status */
///     uint8_t                   ucReserve[3]; /* reserve */
///     NAVIINFO_DIAG_GPS_FIX_CNT stCnt;        /* position fix count data */
///     NAVIINFO_DIAG_GPS_FIX_XYZ stWgs84;      /* lonlat data(WGS84 geodetic)  */
///   } NAVIINFO_DIAG_GPS_FIX;
/// @endcode
///        - position fix status(ucFixSts)
///        - NAVIINFO_DIAG_GPS_FIX_STS_NON:not fixed
///        - NAVIINFO_DIAG_GPS_FIX_STS_2D :2D fix
///        - NAVIINFO_DIAG_GPS_FIX_STS_3D :3D fix
/// \~english @par
///        - NAVIINFO_DIAG_GPS_FIX_CNT structure
/// \~english @code
///   typedef struct
///   {
///     uint32_t    ulCnt3d;    /* position fix count:3D(unit:sec) */
///     uint32_t    ulCnt2d;    /* position fix count:2D(unit:sec) */
///     uint32_t    ulCntElse;  /* position fix count:not fix(unit:sec) */
///   } NAVIINFO_DIAG_GPS_FIX_CNT;
/// @endcode
/// \~english @par
///        - NAVIINFO_DIAG_GPS_FIX_XYZ structure
/// \~english @code
///   typedef struct
///   {
///     int32_t      lLat;    /* GPS latitude(unit:1/256sec) (+: north latitude, -: south latitude) */
///     int32_t      lLon;    /* GPS longitude(unit:1/256sec) (+: east longitude, -: west longitude) */
///   } NAVIINFO_DIAG_GPS_FIX_XYZ;
/// @endcode
/// \~english @par
///        - NAVIINFO_DIAG_GPS_SAT structure
/// \~english @code
///   typedef struct
///   {
///     NAVIINFO_DIAG_GPS_PRN  stPrn[12];  /* all satellite information */
///   } NAVIINFO_DIAG_GPS_SAT;
/// @endcode
/// \~english @par
///        - NAVIINFO_DIAG_GPS_PRN structure
/// \~english @code
///   typedef struct
///   {
///     uint8_t   ucRcvSts;     /* reception status */
///     uint8_t   ucPrn;        /* satellite No. */
///     uint8_t   ucelv;        /* satellite angle(unit:1.0deg) */
///     uint8_t   ucLv;         /* satellite level */
///     uint16_t  usAzm;        /* satellite azimuth(unit:1.0deg) (clockwise from north) */
///     uint8_t   ucReserve[2]; /* reserve */
///   } NAVIINFO_DIAG_GPS_PRN;
/// @endcode
///        - reception status(ucRcvSts)
///        - NAVIINFO_DIAG_GPS_RCV_STS_NOTUSE : not used
///        - NAVIINFO_DIAG_GPS_RCV_STS_SEARCHING : searching
///        - NAVIINFO_DIAG_GPS_RCV_STS_TRACHING : tracking
///        - NAVIINFO_DIAG_GPS_RCV_STS_NOTUSEFIX : not used for position fix
///        - NAVIINFO_DIAG_GPS_RCV_STS_USEFIX : used for position fix
///
/// \~english @retval NAVIINFO_RET_NORMAL normal finish
/// \~english @retval NAVIINFO_RET_ERROR_PARAM parameter error
/// \~english @retval NAVIINFO_RET_ERROR_INNER internal error
/// \~english @retval NAVIINFO_RET_ERROR_NOSUPPORT unsupported
/// \~english @retval NAVIINFO_RET_ERROR_RESOURCE lack of resource
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
///        - The parameter navidiaginfo is NULL [NAVIINFO_RET_ERROR_PARAM]
///        - The parameter hApp is NULL [NAVIINFO_RET_ERROR_PARAM]
///        - The count of message in message queue is reach to max [NAVIINFO_RET_ERROR_RESOURCE]
///        - The count of mutex is reach to max [NAVIINFO_RET_ERROR_RESOURCE]
///        - The count of item in ProcessName-ProcessNo convert table is reach to max [NAVIINFO_RET_ERROR_RESOURCE]
///        - The event is created in same process, but the count of reference is reach to max [NAVIINFO_RET_ERROR_INNER]
///        - The event is created in system, but the count of reference is reach to max  [NAVIINFO_RET_ERROR_INNER]
///        - Memory allocate falied during the event table creation for event  \n
///          registering. [NAVIINFO_RET_ERROR_INNER]
///        - The thread can not register in the event table. [NAVIINFO_RET_ERROR_INNER]
///        - Memory allocate failed in event table during the thread table creation. [NAVIINFO_RET_ERROR_INNER]
///        - After register the thread table in event table, the event flag has already \n
///          been registered. [NAVIINFO_RET_ERROR_INNER]
///        - After register the thread table in event table, the event flag register failed. [NAVIINFO_RET_ERROR_INNER]
///        - The event table is full during event creation. [NAVIINFO_RET_ERROR_INNER]
///        - The memory for event HANDLE allocate failed during event table creation. [NAVIINFO_RET_ERROR_INNER]
///        - The thread can not be registered in event table. [NAVIINFO_RET_ERROR_INNER]
///        - In event table, try to allocate the memory of the thread table creation, but \n
///          failed. [NAVIINFO_RET_ERROR_INNER]
///        - After register the thread in event table, the event flag has already been \n
///          registered. [NAVIINFO_RET_ERROR_INNER]
///        - After register the thread in event table, the event flag register failed. [NAVIINFO_RET_ERROR_INNER]
///        - Specified event ID has not been registered in table. [NAVIINFO_RET_ERROR_INNER]
///        - Initialize event object failed. [NAVIINFO_RET_ERROR_INNER]
///        - The memory for storing semaphore control data allocate failed. [NAVIINFO_RET_ERROR_INNER]
///        - Specified semaphore ID has not been registered when semaphore lock. [NAVIINFO_RET_ERROR_INNER]
///        - Internal mutex HANDLE is NULL when mutex lock. [NAVIINFO_RET_ERROR_INNER]
///        - Internal mutex HANDLE has not been registered in mutex table when mutex lock. [NAVIINFO_RET_ERROR_INNER]
///        - The owner of specified mutex is not itself when mutex lock. [NAVIINFO_RET_ERROR_INNER]
///        - Mutex has been multiple locked [NAVIINFO_RET_ERROR_INNER]
///        - The HANDLE is NULL when getting usable share memory address. [NAVIINFO_RET_ERROR_INNER]
///        - Can not get usable share memory address. [NAVIINFO_RET_ERROR_INNER]
///        - Memory allocate failed for share memory map. [NAVIINFO_RET_ERROR_INNER]
///        - Memory allocate failed for share memory management. [NAVIINFO_RET_ERROR_INNER]
///        - Can not open share memory. [NAVIINFO_RET_ERROR_INNER]
///        - Failed to mapping share memory. [NAVIINFO_RET_ERROR_INNER]
///        - No empty field in share memory. [NAVIINFO_RET_ERROR_INNER]
///        - Failed to guarantee share memory. [NAVIINFO_RET_ERROR_INNER]
///        - ProcessNo has not been registered in message control table when message \n
///          transfered between processes. [NAVIINFO_RET_ERROR_INNER]
///        - Message transfer HANDLE get failed when message transfered between processes. [NAVIINFO_RET_ERROR_INNER]
///        - Message transfer failed between processes. [NAVIINFO_RET_ERROR_INNER]
///        - The destination process name size is larger than 20 characters when message \n
///          transfer between processes. [NAVIINFO_RET_ERROR_INNER]
///        - The message queue name has not been registered in control table when message \n
///          transfer between processes. [NAVIINFO_RET_ERROR_INNER]
///        - Message transfer HANDLE create failed when message transfered between processes. [NAVIINFO_RET_ERROR_INNER]
///        - Message transfer HANDLE get failed from internal table when message transfered \n
///          between processes. [NAVIINFO_RET_ERROR_INNER]
///        - Message transfer failed between processes. [NAVIINFO_RET_ERROR_INNER]
///        - Specified event HANDLE has not been registered in event HANDLE table during \n
///          event waiting. [NAVIINFO_RET_ERROR_INNER]
///        - The count of thread arrived max in event management table during event waiting. [NAVIINFO_RET_ERROR_INNER]
///        - The EV_FLAG_BIT is not set in flagID during event getting. [NAVIINFO_RET_ERROR_INNER]
///        - The ID of message event queue has not been created during event getting. [NAVIINFO_RET_ERROR_INNER]
///        - The flagID has not been registered during event getting. [NAVIINFO_RET_ERROR_INNER]
///        - The interruption happened during event getting [NAVIINFO_RET_ERROR_INNER]
///        - Whatever error happened during event getting. [NAVIINFO_RET_ERROR_INNER]
///        - The HANDLE is NULL when getting usable share memory address for accessing \n
///          received data. [NAVIINFO_RET_ERROR_INNER]
///        - Can not get usable share memory address for accessing received data. [NAVIINFO_RET_ERROR_INNER]
///        - Memory for share memory map allocate failed for accessing received data. [NAVIINFO_RET_ERROR_INNER]
///        - Memory for share memory management allocate failed for accessing received data. [NAVIINFO_RET_ERROR_INNER]
///        - Can not open share memory for accessing received data. [NAVIINFO_RET_ERROR_INNER]
///        - Failed to mapping share memory for accessing received data. [NAVIINFO_RET_ERROR_INNER]
///        - The size of data stored in share memory is larger than the size of \n
///          received data. [NAVIINFO_RET_ERROR_INNER]
///        - The result of sensor data get processing is POS_RET_ERROR_RESOURCE. [NAVIINFO_RET_ERROR_RESOURCE]
///        - The result of sensor data get processing is not POS_RET_ERROR_RESOURCE. [NAVIINFO_RET_ERROR_INNER]
///
/// \~english @par Detail
///        - Call this API to get GPS data for diag. \n
///          This API will finish when get the return value.
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Sync
///
/// \~english @see
///        - POS_SetGPSInfo
////////////////////////////////////////////////////////////////////////////////////////////
//NAVIINFO_RET_API POS_GetGPSInfo(HANDLE hApp, NAVIINFO_DIAG_GPS *navidiaginfo);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup tag_Positioning
/// \~english @par Brief
///        - GPS reset request
///
/// \~english @param [in] hApp
///        - HANDLE - App Handle
/// \~english @param [in] ResName
///        - PCSTR - response thread name
/// \~english @param [in] mode
///        - uint8_t - reset mode
///
/// \~english @par
///        - reset mode(mode) \n
///          If the mode is not one of the following, return POS_RET_ERROR_PARAM
///        - GPS_RST_COLDSTART - GPS reset request(cold start) \n
///          Automatic search, track satellite and position fix after RAM initialized(all config \n
///          items be initialized to default value).\n
///          Initialize data objects:almanac/ ephemeris data, current position, GPS receiver mode and etc.\n
///
/// \~english @retval POS_RET_NORMAL normal finish
/// \~english @retval POS_RET_ERROR_PARAM parameter error
/// \~english @retval POS_RET_ERROR_BUSY GPS device is still in setting
/// \~english @retval POS_RET_ERROR_INNER internal error
/// \~english @retval POS_RET_ERROR_NOSUPPORT unsupported
/// \~english @retval POS_RET_ERROR_RESOURCE lack of resource
///
/// \~english @par Precondition
///        - The creation/initialization(FrameworkunifiedCreateDispatcherWithoutLoop and etc.) \n
///          of the Dispatcher for App are completed.
///        - Availability of positioning service is TRUE.
///
/// \~english @par changes of internal status
///        - There is no changes of internal status
///
/// \~english @par Failure condition
///        - The parameter hApp is NULL [POS_RET_ERROR_PARAM]
///        - The parameter ResName is NULL [POS_RET_ERROR_PARAM]
///        - The parameter mode is not GPS_RST_COLDSTART [POS_RET_ERROR_PARAM]
///        - The count of message in message queue is reach to max [POS_RET_ERROR_RESOURCE]
///        - The count of mutex is reach to max [POS_RET_ERROR_RESOURCE]
///        - The count of item in ProcessName-ProcessNo convert table is reach to max [POS_RET_ERROR_RESOURCE]
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
///        - The message queue name has not been registered in control table when GPS reset \n
///          message transfer between processes. [POS_RET_ERROR_INNER]
///        - Message transfer HANDLE create failed when GPS reset message transfered \n
///          between processes. [POS_RET_ERROR_INNER]
///        - Message transfer HANDLE get failed from internal table when GPS reset message \n
///          transfered between processes. [POS_RET_ERROR_INNER]
///        - GPS reset message transfer failed between processes. [POS_RET_ERROR_INNER]
///        - Specified event HANDLE has not been registered in event HANDLE table during  \n
///          event waiting. [POS_RET_ERROR_INNER]
///        - The count of thread is reach to max in event management table during event waiting. [POS_RET_ERROR_INNER]
///        - The EV_FLAG_BIT is not set in flagID during event waiting. [POS_RET_ERROR_INNER]
///        - The ID of message event queue has not been created during event waiting. [POS_RET_ERROR_INNER]
///        - The flagID has not been registered during event waiting. [POS_RET_ERROR_INNER]
///        - The interruption happened during event waiting [POS_RET_ERROR_INNER]
///        - Whatever error happened during event waiting. [POS_RET_ERROR_INNER]
///        - The result of sensor data get processing is POS_RET_ERROR_RESOURCE. [POS_RET_ERROR_RESOURCE]
///        - The result of sensor data get processing is not POS_RET_ERROR_RESOURCE. [POS_RET_ERROR_INNER]
///
/// \~english @par Detail
///        - Call this API to reset GPS device.
///
/// \~english @par
///        - Please note the following points when use this API.
///        - This API is only called by Navigation proxy.
///
/// \~english @par
///          Notification of message
///        - The result of communication with GPS device will be sent by system API message with following format. \n
///          Command ID : CID_POSIF_REQ_GPS_RESET \n
///          message structure
/// \~english @code
///   typedef struct
///   {
///     TG_GPS_RET_RESET  data;      /* GPS reset status data */
///   } TG_GPS_RET_RESET_MSG;
/// @endcode
///        - message data body structure
/// \~english @code
///   typedef struct {
///     unsigned long    ret_rst_status;    /* response GPS reset status */
///   } TG_GPS_RET_RESET;
/// @endcode
///        - response GPS reset status(ret_rst_status)
///        - GPS_SENDOK:acknowledge
///        - GPS_SENDNG:connection error
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Fire and Forget
///
/// \~english @see
///        - None
////////////////////////////////////////////////////////////////////////////////////////////
POS_RET_API POS_ReqGPSReset(HANDLE hApp, PCSTR ResName, uint8_t mode);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup tag_Positioning
/// \~english @par Brief
///        - Get GPS device version
///
/// \~english @param [in] hApp
///        - HANDLE - App Handle
/// \~english @param [in] buf_size
///        - uint8_t - size of the output buffer
/// \~english @param [out] buf
///        - int8_t* - output buffer of storing GPS version
/// \~english @param [out] size
///        - uint8_t* - the size of getting GPS version data
///
/// \~english @retval POS_RET_ERROR_NOSUPPORT unsupported
///
/// \~english @par Precondition
///        - The creation/initialization(FrameworkunifiedCreateDispatcherWithoutLoop and etc.) \n
///          of the Dispatcher for App are completed.
///        - Availability of positioning service is TRUE.
///
/// \~english @par changes of internal status
///        - There is no changes of internal status
///
/// \~english @par Detail
///        - Call this API to get GPS device version from vehicle sensor.
///
/// \~english @par
///        - Please note the following points when use this API.
///        - Current enviroment does not support to get GPS version, and certainly return POS_RET_ERROR_NOSUPPORT.
///
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Fire and Forget
///
/// \~english @see
///        - None
////////////////////////////////////////////////////////////////////////////////////////////
POS_RET_API POS_GetGPSVersion(HANDLE hApp, uint8_t buf_size, int8_t* buf, uint8_t* size);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup tag_Positioning
/// \~english @par Brief
///        - Register GPS time setting request delivery
///
/// \~english @param [in] hApp
///        - HANDLE - App Handle
/// \~english @param [in] notifyName
///        - PCSTR - Destination thread name
/// \~english @param [in] ucCtrlFlg
///        - uint8_t - Delivery control flag(register)
///
/// \~english @par
///        - Delivery control flag(ucCtrlFlg)
///        - SENSOR_DELIVERY_REGIST - register
///
/// \~english @retval POS_RET_NORMAL normal finish
/// \~english @retval POS_RET_ERROR_PARAM parameter error
/// \~english @retval POS_RET_ERROR_BUFFULL the register count is full
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
///        - The parameter notifyName is NULL [POS_RET_ERROR_PARAM]
///        - The parameter ucCtrlFlg is not register(SENSOR_DELIVERY_REGIST) [POS_RET_ERROR_PARAM]
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
///        - ProcessNo has not been registered in message control table when message \n
///          transfered between processes. [POS_RET_ERROR_INNER]
///        - Message transfer HANDLE get failed when message transfered between processes. [POS_RET_ERROR_INNER]
///        - Message transfer failed in process. [POS_RET_ERROR_INNER]
///        - The destination process name size is larger than 20 characters when message transfer \n
///          between processes. [POS_RET_ERROR_INNER]
///        - The message queue name has not been registered in control table when message \n
///          transfer between processes. [POS_RET_ERROR_INNER]
///        - Message transfer HANDLE create failed when message transfered between processes. [POS_RET_ERROR_INNER]
///        - Message transfer HANDLE get failed from internal table when message transfered \n
///          between processes. [POS_RET_ERROR_INNER]
///        - Message transfer failed between processes. [POS_RET_ERROR_INNER]
///        - Specified event HANDLE has not been registered in event HANDLE table during  \n
///          event waiting. [POS_RET_ERROR_INNER]
///        - The count of thread arrived max in event management table during event waiting. [POS_RET_ERROR_INNER]
///        - The EV_FLAG_BIT is not set in flagID during event getting. [POS_RET_ERROR_INNER]
///        - The ID of message event queue has not been created during event getting. [POS_RET_ERROR_INNER]
///        - The flagID has not been registered during event getting. [POS_RET_ERROR_INNER]
///        - The interruption happened during event getting [POS_RET_ERROR_INNER]
///        - Whatever error happened during event getting. [POS_RET_ERROR_INNER]
///        - The result from positioning service is SENSOR_RET_ERROR_PARAM. [POS_RET_ERROR_PARAM]
///        - The result from positioning service is SENSOR_RET_ERROR_BUFFULL. [POS_RET_ERROR_BUFFULL]
///        - The result from positioning service is SENSOR_RET_ERROR_RESOURCE. [POS_RET_ERROR_RESOURCE]
///        - The result from positioning service is not SENSOR_RET_ERROR_PARAM or  \n
///          SENSOR_RET_ERROR_BUFFULL or SENSOR_RET_ERROR_RESOURCE. [POS_RET_ERROR_INNER]
///
/// \~english @par Detail
///        - Call this API to register GPS time setting request delivery. \n
///          This API will finish when get the return value.
///
///
/// \~english @par
///          Please note the following points when use this API.
///        - Duplication registering
///        - The same destination thread name has already been registered
///        - The registered delivery data updated and normal return.(first delivery)
///        - To one delivery destination, the same data will not be duplication deliveried at same timing.
///        - After call this API, if the delivery destination thread name has been changed, please call this API again.
///
/// \~english @par
///          Notification of message
///        - After registered successfully, vehicle sensor information will be sent by system \n
///          API message with following format. \n
///          Command ID : CID_POSIF_REGISTER_LISTENER_GPS_TIME_SET_REQ \n
///          time info structure \n
///          The year, month, date, hour, minute and second should be the format set to GPS
/// \~english @code
///   typedef struct {
///     uint16_t  year;     /* A.D.(1~) */
///     uint8_t   month;    /* month(1~12) */
///     uint8_t   date;     /* date(1~31) */
///     uint8_t   hour;     /* hour(0~23) */
///     uint8_t   minute;   /* minute(0~59) */
///     uint8_t   second;   /* second(0~59) */
///     uint8_t   reserved; /* not used */
///   } POS_DATETIME;
/// @endcode
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Method
///
/// \~english @see
///        - None
////////////////////////////////////////////////////////////////////////////////////////////
POS_RET_API POS_RegisterListenerGPSTimeSetReq(HANDLE hApp, PCSTR notifyName, uint8_t ucCtrlFlg);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup tag_Positioning
/// \~english @par Brief
///        - Temporarily set GPS time from Diag function
///
/// \~english @param [in] hApp
///        - HANDLE - App Handle
/// \~english @param [in] pstDateTime
///        - POS_DATETIME * - pointer of GPS time
///
/// \~english @par
///        - POS_DATETIME structure
/// \~english @code
///   typedef struct {
///     uint16_t  year;     /* A.D.(1~) */
///     uint8_t   month;    /* month(1~12) */
///     uint8_t   date;     /* date(1~31) */
///     uint8_t   hour;     /* hour(0~23) */
///     uint8_t   minute;   /* minute(0~59) */
///     uint8_t   second;   /* second(0~59) */
///     uint8_t   reserved; /* not used */
///   } POS_DATETIME;
/// @endcode
///
/// \~english @retval NAVIINFO_RET_NORMAL normal finish
/// \~english @retval NAVIINFO_RET_ERROR_PARAM parameter error
/// \~english @retval NAVIINFO_RET_ERROR_INNER internal error
/// \~english @retval NAVIINFO_RET_ERROR_NOSUPPORT unsupported
/// \~english @retval NAVIINFO_RET_ERROR_RESOURCE lack of resource
///
/// \~english @par Precondition
///        - The creation/initialization(FrameworkunifiedCreateDispatcherWithoutLoop and etc.) \n
///          of the dispatcher for App are completed.
///        - Availability of positioning service is TRUE.
///
/// \~english @par change of internal status
///        - There is no change of internal status
///
/// \~english @par Failure condition
///        - The parameter pstDateTime is NULL [POS_RET_ERROR_PARAM]
///        - The parameter hApp is NULL [POS_RET_ERROR_PARAM]
///        - GPS time Data size is larger than 144 [POS_RET_ERROR_PARAM]
///        - The count of message in message queue is reach to max [POS_RET_ERROR_RESOURCE]
///        - The count of mutex is reach to max [POS_RET_ERROR_RESOURCE]
///        - The count of item in ProcessName-ProcessNo convert table is reach to max [POS_RET_ERROR_RESOURCE]
///        - The message queue name has not been registered in control table. [POS_RET_ERROR_INNER]
///        - Message transfer HANDLE create failed. [POS_RET_ERROR_INNER]
///        - Message transfer HANDLE get failed from internal table. [POS_RET_ERROR_INNER]
///        - GPS time setting message transfer failed. [POS_RET_ERROR_INNER]
///
/// \~english @par Detail
///        - Call this API to set GPS time data to vehicle sensor. \n
///          This API will finish when get the return value.
///
///
/// \~english @par
///          Please note the following points when use this API.
///        - This API will not notify the result of setting time to GPS device. \n
///          Return normal only there is no error such as parameter error.
///        - If want to get the result of setting time to GPS device, use  \n
///          POS_RegisterListnerGpsTime to get GPS time and judge it.
///        - This API is only called by Diag service.
///        - While GPS data is receiving from GPS device, the GPS time set by this API is ignored \n
///          and it is set actual GPS time notified by positioning_hal.
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Fire and Forget
///
/// \~english @see
///        - POS_GetGPStime
////////////////////////////////////////////////////////////////////////////////////////////
//POS_RET_API POS_SetGPStime(HANDLE hApp, POS_DATETIME* pstDateTime);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup tag_Positioning
/// \~english @par Brief
///        - Register GPS time delivery
///
/// \~english @param [in] hApp
///        - HANDLE - App Handle
/// \~english @param [in] notifyName
///        - PCSTR - Destination thread name
/// \~english @param [in] ucCtrlFlg
///        - uint8_t - Delivery control flag(register)
/// \~english @param [in] ucDeliveryTiming
///        - uint8_t - Delivery timing(change/update)
///
/// \~english @par
///        - Delivery control flag(ucCtrlFlg)
///        - SENSOR_DELIVERY_REGIST - register
///        - Register specified data delivery
///        - Please note that if the same data delivery has been registered for multiple \n
///          times, the data will also be deliveried for registered multiple times.
///        - The specified GPS time information will be deliveried at registered time (first delivery).
///        - Delivery timing(ucDeliveryTiming)
///        - SENSOR_DELIVERY_TIMING_CHANGE - change  Specified data will be deliveried only when it changed.
///        - SENSOR_DELIVERY_TIMING_UPDATE - update  Specified data will be deliveried as \n
///          long as it updated by vehicle sensor.
///
/// \~english @retval SENSOR_RET_NORMAL normal finish
/// \~english @retval SENSOR_RET_ERROR_CREATE_EVENT event creation error
/// \~english @retval SENSOR_RET_ERROR_PARAM parameter error
/// \~english @retval SENSOR_RET_ERROR_INNER internal error
/// \~english @retval SENSOR_RET_ERROR_NOSUPPORT unsupported
/// \~english @retval SENSOR_RET_ERROR_RESOURCE lack of resource
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
///        - The parameter hApp is NULL [SENSOR_RET_ERROR_PARAM]
///        - The parameter notifyName is NULL [SENSOR_RET_ERROR_PARAM]
///        - The parameter ucCtrlFlg is not register(SENSOR_DELIVERY_REGIST) [SENSOR_RET_ERROR_PARAM]
///        - The parameter ucDeliveryTiming is neither update(SENSOR_DELIVERY_TIMING_UPDATE) \n
///          nor change(SENSOR_DELIVERY_TIMING_CHANGE) [SENSOR_RET_ERROR_PARAM]
///        - The count of message in message queue is reach to max [SENSOR_RET_ERROR_RESOURCE]
///        - The count of mutex is reach to max [SENSOR_RET_ERROR_RESOURCE]
///        - The count of item in ProcessName-ProcessNo convert table is reach to max [SENSOR_RET_ERROR_RESOURCE]
///        - The event is created in same process, but the count of reference is reach to \n
///          max [SENSOR_RET_ERROR_CREATE_EVENT]
///        - The event is created in system, but the count of reference is reach to max  [SENSOR_RET_ERROR_CREATE_EVENT]
///        - Memory allocate falied during the event table creation for event  \n
///          registering. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - The thread can not register in the event table. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - Memory allocate failed in event table during the thread table creation. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - After register the thread table in event table, the event flag has already been \n
///          registered. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - After register the thread table in event table, the event flag register \n
///          failed. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - The event table is full during event creation. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - The memory for event HANDLE allocate failed during event table creation. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - The thread can not be registered in event table. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - In event table, try to allocate the memory of the thread table creation, but \n
///          failed. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - After register the thread in event table, the event flag has already been  \n
///          registered. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - After register the thread in event table, the event flag register failed. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - Specified event ID has not been registered in table. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - Initialize event object failed. [SENSOR_RET_ERROR_CREATE_EVENT]
///        - ProcessNo has not been registered in message control table when message \n
///          transfered between processes. [SENSOR_RET_ERROR_INNER]
///        - Message transfer HANDLE get failed when message transfered between processes. [SENSOR_RET_ERROR_INNER]
///        - Message transfer failed between processes. [SENSOR_RET_ERROR_INNER]
///        - The destination process name size is larger than 20 characters when message \n
///          transfer between processes. [SENSOR_RET_ERROR_INNER]
///        - The message queue name has not been registered in control table when message \n
///          transfer between processes. [SENSOR_RET_ERROR_INNER]
///        - Message transfer HANDLE create failed when message transfered between processes. [SENSOR_RET_ERROR_INNER]
///        - Message transfer HANDLE get failed from internal table when message transfered \n
///          between processes. [SENSOR_RET_ERROR_INNER]
///        - Message transfer failed between processes. [SENSOR_RET_ERROR_INNER]
///        - Specified event HANDLE has not been registered in event HANDLE table during \n
///          event waiting. [SENSOR_RET_ERROR_INNER]
///        - The count of thread arrived max in event management table during event waiting. [SENSOR_RET_ERROR_INNER]
///        - The EV_FLAG_BIT is not set in flagID during event get. [SENSOR_RET_ERROR_INNER]
///        - The ID of message event queue has not been created during event get. [SENSOR_RET_ERROR_INNER]
///        - The flagID has not been registered during event get. [SENSOR_RET_ERROR_INNER]
///        - The interruption happened during event get [SENSOR_RET_ERROR_INNER]
///        - Whatever error happened during event get. [SENSOR_RET_ERROR_INNER]
///        - Get event timeout. [SENSOR_RET_ERROR_INNER]
///        - Error happened during event get. [SENSOR_RET_ERROR_INNER]
///
/// \~english @par Detail
///        - Call this API to register GPS time delivery. \n
///          This API will finish when get the return value.
///
///
/// \~english @par
///          Please note the following points when use this API.
///        - Duplication registering
///        - The same destination thread name has already been registered
///        - The registered delivery data updated and normal return.(first delivery)
///        - To one delivery destination, the same data will not be duplication deliveried at same timing.
///        - Although registering can be finished, but the data will not be deliveried.
///        - After call this API, if the delivery destination thread name has changed, please call this API again.
///
/// \~english @par
///           Notification of message
///         - After registered successfully, vehicle sensor will send GPS time data \n
///           as system API message with following format.
///         - If the register successed, certainly delivery first data. And then \n
///           delivery data according to the delivery timing.
///         - Command ID : CID_VEHICLESENS_VEHICLE_INFO_GPS_TIME\n
///         - SENSOR_MSG_GPSTIME structure
/// \~english @code
///   typedef struct {
///     NAVIINFO_UTCTIME  utc;        /* UTC time */
///     uint8_t           tdsts;      /* time status */
///     uint8_t           reserve[3]; /* reserve */
///   } SENSOR_MSG_GPSTIME;
/// @endcode
///        - time status(tdsts)
///        - 0= time has not been adjusted after GPS receiver reset(time input or master reset or CSF start)
///        - 1= time output from RTC Backup(have time adjustment result)
///        - 2= time adjustment completed
///
/// \~english @par
///        - NAVIINFO_UTCTIME structure
/// \~english @code
///   typedef struct {
///     uint16_t  year;     /* A.D.(1~) */
///     uint8_t   month;    /* month(1~12) */
///     uint8_t   date;     /* date(1~31) */
///     uint8_t   hour;     /* hour(0~23) */
///     uint8_t   minute;   /* minute(0~59) */
///     uint8_t   second;   /* second(0~59) */
///     uint8_t   reserved; /* not used */
///   } POS_DATETIME;
/// @endcode
///
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Method
///
/// \~english @see
///        - None
////////////////////////////////////////////////////////////////////////////////////////////
SENSOR_RET_API POS_RegisterListenerGPStime(HANDLE hApp, PCSTR notifyName, uint8_t ucCtrlFlg, uint8_t ucDeliveryTiming);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup tag_Positioning
/// \~english @par Brief
///        - Get GPS time
///
/// \~english @param [in] hApp
///        - HANDLE - App Handle
/// \~english @param [out] dat
///        - SENSOR_GPSTIME* - output buffer pointer to store GPS time
///
/// \~english @par
///        - SENSOR_GPSTIME structure
/// \~english @code
///   typedef struct {
///     NAVIINFO_UTCTIME  utc;        /* UTC time */
///     uint8_t           tdsts;      /* date amd time status */
///     uint8_t           reserve[3]; /* reserve */
///   } SENSOR_GPSTIME;
/// @endcode
///        - date amd time status(tdsts)
///        - 0= time has not been adjusted after GPS receiver reset(time input or master reset or CSF start)
///        - 1= time output from RTC Backup(have time adjustment result)
///        - 2= time adjustment completed
///
/// \~english @par
///        - NAVIINFO_UTCTIME structure
/// \~english @code
///   typedef struct {
///     uint16_t  year;     /* A.D.(1~) */
///     uint8_t   month;    /* month(1~12) */
///     uint8_t   date;     /* date(1~31) */
///     uint8_t   hour;     /* hour(0~23) */
///     uint8_t   minute;   /* minute(0~59) */
///     uint8_t   second;   /* second(0~59) */
///     uint8_t   reserved; /* not used */
///   } NAVIINFO_UTCTIME;
/// @endcode
///
/// \~english @retval POS_RET_NORMAL normal finish
/// \~english @retval POS_RET_ERROR_PARAM parameter error
/// \~english @retval POS_RET_ERROR_INNER internal error
/// \~english @retval POS_RET_ERROR_NOSUPPORT unsupported
/// \~english @retval POS_RET_ERROR_RESOURCE lack of resource
///
/// \~english @par Precondition
///        - The creation/initialization(FrameworkunifiedCreateDispatcherWithoutLoop and etc.) \n
///          of the Dispatcher for App are completed.
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
///        - After register the thread table in event table, the event flag has already  \n
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
///        - ProcessNo has not been registered in message control table when message \n
///          transfered between processes. [POS_RET_ERROR_INNER]
///        - Message transfer HANDLE get failed when message transfered between processes. [POS_RET_ERROR_INNER]
///        - Message transfer failed between processes. [POS_RET_ERROR_INNER]
///        - The destination process name size is larger than 20 characters when message \n
///          transfer between processes. [POS_RET_ERROR_INNER]
///        - The message queue name has not been registered in control table when message \n
///          transfer between processes. [POS_RET_ERROR_INNER]
///        - Message transfer HANDLE create failed when message transfered between processes. [POS_RET_ERROR_INNER]
///        - Message transfer HANDLE get failed from internal table when message transfered \n
///          between processes. [POS_RET_ERROR_INNER]
///        - Message transfer failed between processes. [POS_RET_ERROR_INNER]
///        - Specified event HANDLE has not been registered in event HANDLE table during \n
///          event waiting. [POS_RET_ERROR_INNER]
///        - The count of thread arrived max in event management table during event waiting. [POS_RET_ERROR_INNER]
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
///        - The result of sensor data getting process is POS_RET_ERROR_RESOURCE. [POS_RET_ERROR_RESOURCE]
///        - The result of sensor data getting process is not POS_RET_ERROR_RESOURCE. [POS_RET_ERROR_INNER]
///
/// \~english @par Detail
///        - Call this API to get GPS time from vehicle sensor.
///
/// \~english @par
///        - Please note the following points when use this API.
///
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Sync
///
/// \~english @see
///        - POS_SetGPStime
////////////////////////////////////////////////////////////////////////////////////////////
//POS_RET_API POS_GetGPStime(HANDLE hApp, SENSOR_GPSTIME* dat);

#ifdef __cplusplus
}
#endif
/** @}*/  // end of positioning
/** @}*/  // end of vehicle_service
/** @}*/  // end of BaseSystem
#endif  // POSITIONING_CLIENT_INCLUDE_VEHICLE_SERVICE_POS_GPS_API_H_
