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

#ifndef VEHICLE_CLIENT_INCLUDE_VEHICLE_SERVICE_VEHICLE_API_H_
#define VEHICLE_CLIENT_INCLUDE_VEHICLE_SERVICE_VEHICLE_API_H_
/**
 * @file Vehicle_API.h
 * @brief vehicle api header file
 */

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup vehicle_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup vehicle
 *  @ingroup vehicle_service
 *  @{
 */

/******************************************************************************
 * Fine name    :Vehicle_API.h
 * System name    :GPF
 * Subsystem name  :Vehicle I/F library
 ******************************************************************************/
#include <peripheral_service/Canif_API.h>
#include <stub/Vehicle_Sensor_Common_API.h>
#include <stub/vehicle_notifications.h>
#include <native_service/frameworkunified_service_protocol.h>

/************************************************************************
 *      Macro definition                          *
 ************************************************************************/
/*----------------------------------------------------------------------*
 *  VECHILE_RET_API definition                        *
 *----------------------------------------------------------------------*/
/* Normal */
/**
 * \~english  Sucess
 */
#define  VEHICLE_RET_NORMAL        0

/**
 * \~english  No register data ID
 */
#define  VEHICLE_RET_ERROR_DID      (-2)

/**
 * \~english  Input parameter error
 */
#define  VEHICLE_RET_ERROR_PARAM      (-4)

/**
 * \~english  Create event error
 */
#define  VEHICLE_RET_ERROR_CREATE_EVENT  (-6)

/*----------------------------------------------------------------------*
 *  Vehicle sensor information delivery registration API related        *
 *----------------------------------------------------------------------*/
/* Delivery control */
/**
 * \~english  Register delivery
 */
#define  VEHICLE_DELIVERY_REGIST      0x01

#if 0
/**
 * \~english  Stop delivery
 */
#define  VEHICLE_DELIVERY_STOP    0x02

/**
 * \~english  Restart delivery
 */
#define  VEHICLE_DELIVERY_RESTART  0x03
#endif

/* Delivery opportunities */
/**
 * \~english  Delivery timing(update)
 */
#define  VEHICLE_DELIVERY_TIMING_UPDATE  0x01

/**
 * \~english  Delivery timing(change)
 */
#define  VEHICLE_DELIVERY_TIMING_CHANGE  0x02

/* Drive mode switching */
/**
 * \~english  TAB mode
 */
#define  VEHICLE_DRIVE_MODE_TAB    0

/**
 * \~english  AIAPI mode(USB)
 */
#define  VEHICLE_DRIVE_MODE_ATAPI  1

/*----------------------------------------------------------------------*
 *  Message related                            *
 *----------------------------------------------------------------------*/
/* Command ID */
/* Since it is defined in #define the commodity code, the public CID is defined in #define */
/**
 * \~english  Vehicle sensor information notify commandID
 */
#define  CID_VEHICLESENS_VEHICLE_INFO    0x00000200

/* Message body size(byte) */
/**
 * \~english  Vehicle sensor information data max size
 */
#define  VEHICLE_VSINFO_DSIZE      80

/************************************************************************
 *      typedef definition                          *
 ************************************************************************/
/**
 * \~english  API return value
 */
typedef int32_t VEHICLE_RET_API;

/************************************************************************
 *      struct definition                          *
 ************************************************************************/

/************************************************************************
 * TAG       : VEHICLE_MSG_VSINFO_DAT
 * ABSTRACT  : Vehicle sensor information notification message(->User)
 ************************************************************************/
/** \~english Vehicle sensor information notification message */
/**
 * \~english  Vehicle sensor information notify message data
 */
typedef struct {
  /**
   * \~english  Vehicle sensor information data ID
   */
  uint32_t did;

  /**
   * \~english  Vehicle sensor information data size
   */
  uint16_t size;

  /**
   * \~english  Vehicle sensor information receive flag
   */
  uint8_t rcvFlag;

  /**
   * \~english  Reserve
   */
  uint8_t reserve;

  /**
   * \~english  Vehicle sensor information data
   */
  uint8_t data[VEHICLE_VSINFO_DSIZE];
} VEHICLE_MSG_VSINFO_DAT;

/**
 * \~english  Vehicle sensor information notify message
 */
typedef struct {
  /**
   * \~english  Vehicle sensor information data struct
   */
  VEHICLE_MSG_VSINFO_DAT data;
} VEHICLE_MSG_VSINFO;

/************************************************************************
 *      Function prototype                        *
 ************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
/*[VEHICLE_API Public API]*/
/* Display time middle API prototype *************/

/**
 * \ingroup Vehicle_DeliveryEntry
 * \~english @par Overview
 *      - Register delivery of vehicle information.
 * \~english @param [in]  h_app
 *      HANDLE          -    The application handle of the calling thread
 * \~english @param [in]  notify_name
 *      PCSTR           -    Vehicle information notification destination threa name
 * \~english @param [in]  ul_did
 *      uint32_t        -    Vehicle Data ID
 * \~english @param [in]  uc_ctrl_flg
 *      uint8_t         -    Register delivery
 * \~english @param [in]  uc_delivery_timing
 *      uint8_t         -    Delivery timing(change/update)
 * \~english @retval  VEHICLE_RET_NORMAL             : Success(register successful)
 * \~english @retval  VEHICLE_RET_ERROR_CREATE_EVENT : Failure(event generation failure)
 * \~english @retval  VEHICLE_RET_ERROR_PARAM        : Invalid parameter
 * \~english @par Precondition
 *      - The generation/initialization of dispatcher for application(PadCreateDispatcherWithoutLoop etc.) is done.
 *      - The availability of the vehicle process is TRUE.
 * \~english @par Changing internal state
 *      - No change of internal state by this API occurs.
 * \~english @par Processing failure condition
 *      - Delivery control flag(uc_ctrl_flg) is not VEHICLE_DELIVERRY_REGIST [VEHICLE_RET_ERROR_PARAM]
 *      - Delivery timing is invalid(not change/update) [VEHICLE_RET_ERROR_PARAM]
 *      - Failed to generate a message queue handle for transmission with
 *        the vehicle process [VEHICLE_RET_ERROR_CREATE_EVENT]
 *      - Synchronous communication received on the vehicle process side failed to
 *        get data [VEHICLE_RET_ERROR_CREATE_EVENT]
 *      - The data ID does not exit in the management list on the vehicle process [VEHICLE_RET_ERROR_CREATE_EVENT]
 *      - The number of delivery registrations has reached the max number [VEHICLE_RET_ERROR_CREATE_EVENT]
 * \~english @par Classification
 *      - Public
 * \~english @par Type
 *      - Sync
 * \~english @par Details
 *      - By calling this API, we request to register delivery specified vehicle information.
 *      - It returns from this API by returning the delivery registration result by the Return value.
 *      - The specified vehicle information is notified after delivery registration.
 *      - Data delivery/reception is performed by asynchronous communication,
 *        and the receiving side acquires with command"CID_VEHICLESENS_VEHICLE_INFO".
 *      - An example in which a process using NSFW registers"CID_VEHICLESENS_VEHICLE_INFO" is shown below.
 *      - Example) when above CID received, callback function(SampleCallBackFunc) is executed.
 *      -   FrameworkunifiedAttachCallbackToDispatcher(hApp, TN_VEHICLE_SENS, CID_VEHICLESENS_VEHICLE_INFO, SampleCallBackFunc).
 * \~english @see Vehicle_GetVehicleData, FrameworkunifiedMcOpenSender, FrameworkunifiedInvokeSync
 */
VEHICLE_RET_API Vehicle_DeliveryEntry(HANDLE h_app, PCSTR notify_name,
                                      uint32_t ul_did, uint8_t uc_ctrl_flg,
                                      uint8_t uc_delivery_timing);

/**
 * \ingroup Vehicle_GetVehicleData
 * \~english @par Overview
 *      - Get vehicle information data(arbitrary data).
 * \~english @param [in]  h_app
 *      HANDLE         -    The application handle of the calling thread
 * \~english @param [in]  ul_did
 *      uint32_t       -    Vehicle Data ID
 * \~english @param [out] p_dest_data
 *      void *         -    Data buffer
 * \~english @param [in]  us_dest_size
 *      uint16_t       -    Data buffer size(byte)
 * \~english @retval  Integer(>0)                      : The stored data size
 * \~english @retval  VEHICLE_RET_ERROR_CREATE_EVENT   : Failure(event generation failure)
 * \~english @retval  VEHICLE_RET_ERROR_PARAM          : Invalid parameter
 * \~english @par Precondition
 *      - The generation/initialization of dispatcher for application(PadCreateDispatcherWithoutLoop etc.) is done.
 *      - The availability of the vehicle process is TRUE.
 * \~english @par Changing internal state
 *      - No change of internal state by this API occurs.
 * \~english @par Processing failure condition
 *      - Failed to generate a message queue handle for transmission with
 *        the vehicle process [VEHICLE_RET_ERROR_CREATE_EVENT]
 *      - Input data buffer size(us_dest_size) is zero [VEHICLE_RET_ERROR_PARAM]
 *      - Input data buffer(p_dest_data) is NULL [VEHICLE_RET_ERROR_PARAM]
 *      - Synchronous communication received on the vehicle process side failed to
 *        get data [VEHICLE_RET_ERROR_CREATE_EVENT]
 *      - The data ID does not exit in the management list on the vehicle process [VEHICLE_RET_ERROR_CREATE_EVENT]
 * \~english @par Classification
 *      - Public
 * \~english @par Type
 *      - Sync
 * \~english @par Details
 *      - By calling this API, we obtain specified vehicle information.
 *      - The vehicle data will be written to parameter(pDestData).
 *      - Data delivery/reception is performed by asynchronous communication,
 *        and the receiving side acquires with command"CID_VEHICLESENS_VEHICLE_INFO".
 * \~english @see Vehicle_DeliveryEntry, FrameworkunifiedMcOpenSender, FrameworkunifiedInvokeSync
 */
int32_t Vehicle_GetVehicleData(HANDLE h_app, uint32_t ul_did, void *p_dest_data,
                               uint16_t us_dest_size);

#ifdef __cplusplus
}
#endif

/** @}*/  // end of vehicle
/** @}*/  // end of VehicleService
/** @}*/  // end of BaseSystem

#endif  // VEHICLE_CLIENT_INCLUDE_VEHICLE_SERVICE_VEHICLE_API_H_
