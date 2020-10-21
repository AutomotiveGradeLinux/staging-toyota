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

/**
 * @file DiagCodeAPI.h
 */
/*******************************************************************************
 * FILE      : DiagCodeAPI.h
 * SYSTEM    : LinuxPF
 * SUBSYSTEM :
 * TITLE     : DiagCode management function include definition
 ******************************************************************************/

#ifndef DIAG_CODE_LIBRARY_INCLUDE_VEHICLE_SERVICE_DIAGCODEAPI_H_
#define DIAG_CODE_LIBRARY_INCLUDE_VEHICLE_SERVICE_DIAGCODEAPI_H_

/**
 * @file DiagCodeAPI.h
 * @~english
 * @brief Diag code API header
 */

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup vehicle_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup diag_code
 *  @ingroup vehicle_service
 *  @{
 */

#include <time.h>
#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_framework_types.h>

#ifdef  __cplusplus
extern  "C" {
#endif

/*! @~english API return value */
typedef int DGCODE_RET_API;

/**
 * \~english The definition of diag code processing result
 */
/*! @~english Success */
#define DGCODE_RET_NORMAL       (0)
/*! @~english Fail */
#define DGCODE_RET_ERROR        (-1)
/*! @~english Illegal parameter */
#define DGCODE_RET_PARA_ERR     (-2)

/**
 * \~english Category ID definition
 */

/*! @~english none device */
#define DGCODE_DIV_UNIT_NONE    (0x00)
/*! @~english device base */
#define DGCODE_DIV_UNIT_BASE    (0x10)
/*! @~english Your own device */
#define DGCODE_DIV_UNIT__CWORD92_     (DGCODE_DIV_UNIT_BASE + 0x00)
/*! @~english Other devices */
#define DGCODE_DIV_UNIT_OTHER   (DGCODE_DIV_UNIT_BASE + 0x01)
/*! @~english device maximum number definition */
#define DGCODE_DIV_UNIT_MAX     (DGCODE_DIV_UNIT_OTHER)

/*! @~english proto base */
#define DGCODE_DIV_PROT_BASE    (0x20)
/*! @~english _CWORD43_ */
#define DGCODE_DIV_PROT__CWORD43_   (DGCODE_DIV_PROT_BASE + 0x00)
/*! @~english USB */
#define DGCODE_DIV_PROT_USB     (DGCODE_DIV_PROT_BASE + 0x02)
/*! @~english _CWORD119_ */
#define DGCODE_DIV_PROT__CWORD119_    (DGCODE_DIV_PROT_BASE + 0x03)
/*! @~english proto device maximum number definition */
#define DGCODE_DIV_PROT_MAX     (DGCODE_DIV_PROT_USB)
/*! @~english Communication full selection */
#define DGCODE_DIV_PROT_ALL     (DGCODE_DIV_PROT_BASE + 0x0F)
/*! @~english All select */
#define DGCODE_DIV_ALL          (DGCODE_DIV_UNIT_BASE + DGCODE_DIV_PROT_BASE)
/*! \~english Public RoB selection */
#define DGCODE_DIV_ROB_PUBLIC      (1)
/*! \~english Private Rob selection */
#define DGCODE_DIV_ROB_PRIVATE     (2)
/*! \~english RoB All select selection*/
#define DGCODE_DIV_ROB_ALL      (DGCODE_DIV_ROB_PUBLIC + DGCODE_DIV_ROB_PRIVATE)

/**
 * \~english Past present registered flag definition
 */
/*! @~english Past and present */
#define DGCODE_TYPE_PAST_PRES       (0)
/*! @~english Present only */
#define DGCODE_TYPE_PAST            (1)
/*! @~english Past only */
#define DGCODE_TYPE_PRES            (2)
/*! @~english Unspecified */
#define DGCODE_TYPE_NOTSPECIFIED    (3)

/**
 * \~english FF data registration flag definition
 */
/*! @~english Do not register FF data */
#define DGCODE_FFDATA_REG_OFF           (0)
/*! @~english FF data registered */
#define DGCODE_FFDATA_REG_ON            (1)
/*! @~english Unspecified */
#define DGCODE_FFDATA_REG_NOTSPECIFIED  (2)

/**
 * \~english Validity invalid time definition
 */
/*! @~english Time information invalid */
#define DGCODE_TIME_INVALID         (0)
/*! @~english Time information valid */
#define DGCODE_TIME_VALID           (1)
/*! @~english Unspecified */
#define DGCODE_TIME_NOTSPECIFIED    (2)

/**
 * \~english Diag code existence check result definition
 */
/*! @~english Diag code not registered */
#define DGCODE_CHECK_NOT_EXIST      (0)
/*! @~english Diag code registered */
#define DGCODE_CHECK_EXIST          (1)

/**
 * \~english Diag code type definition
 */
/*! @~english Development diag code */
#define DGCODE_KIND_DEVELOPMENT     (0)
/*! @~english Service diag code */
#define DGCODE_KIND_SERVICE         (1)
/*! @~english Communication diag code */
#define DGCODE_KIND_COMMUNICATION   (2)
/*! @~english Manufacturer diag code */
#define DGCODE_KIND_MAKER           (3)

/**
 * \~english RoB code type definition
 */
/*! \~english Public(service use)  */
#define DGCODE_ROB_KIND_PUBLIC      (0)
/*! \~english Private(only design)  */
#define DGCODE_ROB_KIND_PRIVATE     (1)

/**
 * \~english type of rob_code specification
 */
/*! \~english Specify all RoB codes. */
#define DGCODE_GET_ALL_ROB_CODE     (0xFFFF)

/**
 * \~english Logical unit number definition
 */
/*! @~english Unit number 0X */
#define DGCODE_MAKER_UNIT_0         (0x00)
/*! @~english Unit number 1X */
#define DGCODE_MAKER_UNIT_1         (0x01)
/*! @~english Unit number 2X */
#define DGCODE_MAKER_UNIT_2         (0x02)
/*! @~english Unit number 3X */
#define DGCODE_MAKER_UNIT_3         (0x03)
/*! @~english Unit number 4X */
#define DGCODE_MAKER_UNIT_4         (0x04)
/*! @~english Unit number 5X */
#define DGCODE_MAKER_UNIT_5         (0x05)
/*! @~english Unit number 6X */
#define DGCODE_MAKER_UNIT_6         (0x06)
/*! @~english Unit number 7X */
#define DGCODE_MAKER_UNIT_7         (0x07)
/*! @~english Unit number 8X */
#define DGCODE_MAKER_UNIT_8         (0x08)
/*! @~english Unit number 9X */
#define DGCODE_MAKER_UNIT_9         (0x09)
/*! @~english Unit number AX */
#define DGCODE_MAKER_UNIT_A         (0x0A)
/*! @~english Unit number BX */
#define DGCODE_MAKER_UNIT_B         (0x0B)
/*! @~english Unit number CX */
#define DGCODE_MAKER_UNIT_C         (0x0C)
/*! @~english Unit number DX */
#define DGCODE_MAKER_UNIT_D         (0x0D)
/*! @~english Unit number EX */
#define DGCODE_MAKER_UNIT_E         (0x0E)
/*! @~english Unit number FX */
#define DGCODE_MAKER_UNIT_F         (0x0F)
/*! @~english All above */
#define DGCODE_MAKER_UNIT_ALL       (0x10)

/**
 * \~english All designation definitions of registered devices
 */
/*! @~english Error designation of all registered devices */
#define DGCODE_PHYSADR_ALL          (0xFFFF)

/**
 * \~english Diag code registration / deletion notification type definition
 */
/*! @~english Diag code registration */
#define DGCODE_NOTIFY_PUT           (0)
/*! @~english Diag code clear */
#define DGCODE_NOTIFY_DELETE        (1)
/*! \~english DTC registration */
#define DGCODE_NOTIFY_DTC_PUT       (2)
/*! \~english RoB registration */
#define DGCODE_NOTIFY_ROB_PUT       (3)
/*! \~english DTC clear */
#define DGCODE_NOTIFY_DTC_DELETE    (4)
/*! \~english RoB clear */
#define DGCODE_NOTIFY_ROB_DELETE    (5)

/**
 * \~english Type of activation definition
 */
/*! @~english Hot start */
#define DGCODE_START_KIND_HOT       (0)
/*! @~english Cold start */
#define DGCODE_START_KIND_COLD      (1)

/**
 * \~english Analysis support log saved DTC event presence / absence flag definition
 */

/*! 
    @~english The unnecessary diag code of saving analysis support log
*/
#define DGCODE_DTC_EVENT_LOG_OFF    (0)
/*! 
    @~english The necessary diag code of saving analysis support Log
*/
#define DGCODE_DTC_EVENT_LOG_ON     (1)

/**
 * \~english Diag code registration mask setting definition
 */
/*! @~english Diag code registration mask OFF */
#define DGCODE_PUT_MASK_OFF         (0)
/*! @~english Diag code registration mask ON */
#define DGCODE_PUT_MASK_ON          (1)

/**
 * \~english Diag code unique count value definition
 */
/*! @~english Normal count */
#define DGC_TIMESTAMP_NORMAL        0U
/*! @~english Original count */
#define DGC_TIMESTAMP_INDIPENDENT   1U

/**
 * \~english Availability register id define.
 */
/*! @~english Backup manager register id */
#define DGC_REG_AVAILABILITY_BACKUP         0x01
/*! @~english Clock register id */
#define DGC_REG_AVAILABILITY_CLOCK          0x02
/*! @~english diag_record register id */
#define DGC_REG_AVAILABILITY_PFDRECTHREAD   0x04
/*! @~english Vehicle register id */
#define DGC_REG_AVAILABILITY_VEHICLE        0x08
/*! @~english Postion register id */
#define DGC_REG_AVAILABILITY_POSITION       0x10

/**
 * \~english Diag code registration / erasing detailed setting structure
 */
typedef struct _DGCODE_MEMINFO_CUSTOM {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} DGCODE_MEMINFO_CUSTOM;

/**
 * \~english Time information for SSR information
 */
typedef struct _DGCODE_SSR_INFO_TIME_INFO {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} DGCODE_SSR_INFO_TIME_INFO;

/**
 * \~english DTC SSR information
 */
typedef struct _DGCODE_DTC_SSR_INFO {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} DGCODE_DTC_SSR_INFO;

/**
 * \~english RoB SSR information
 */
typedef struct _DGCODE_ROB_SSR_INFO {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} DGCODE_ROB_SSR_INFO;

/**
 * \~english Diag code list acquisition setting structure
 */
typedef struct _DGCODE_GET_LIST_INFO {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} DGCODE_GET_LIST_INFO;

/**
 * \~english Diag code list information structure
 */
typedef struct _DGCODE_LIST_INFO {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} DGCODE_LIST_INFO;

/**
 * \~english DTC list acquisition setting structure
 */
typedef struct _DGCODE_GET_DTC_LIST_INFO {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} DGCODE_GET_DTC_LIST_INFO;

/**
 * \~english DTC list information structure
 */
typedef struct _DGCODE_DTC_LIST_INFO {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} DGCODE_DTC_LIST_INFO;


/**
 * \~english RoB list acquisition setting structure
 */
typedef struct _DGCODE_GET_ROB_LIST_INFO {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} DGCODE_GET_ROB_LIST_INFO;

/**
 * \~english RoB list information structure
 */
typedef struct _DGCODE_ROB_LIST_INFO {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} DGCODE_ROB_LIST_INFO;

/**
 * \~english FF data list information structure
 */
typedef struct _DGCODE_FFDATA_LIST_INFO {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} DGCODE_FFDATA_LIST_INFO;

/**
 * \~english Diag code list delete setting structure
 */
typedef struct _DGCODE_DELETE_INFO {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} DGCODE_DELETE_INFO;

/**
 * \~english DTC list delete setting structure
 */
typedef struct _DGCODE_DELETE_DTC_INFO {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} DGCODE_DELETE_DTC_INFO;

/**
 * \~english RoB list delete setting structure
 */
typedef struct _DGCODE_DELETE_ROB_INFO {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} DGCODE_DELETE_ROB_INFO;

/**
 * \~english DID change structure
 */
typedef struct _DGCODE_DID_LIST {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} DGCODE_DID_LIST;

/****************************************/
/*             DiagCodeAPI              */
/****************************************/
/// \~english Subscribe the availability of the external module
/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup Diag_SubscribeAvailability
/// \~english @par Overview
///      - Notify the availability status of the external module
/// \~english @param [in]   hApp            Application handle
/// \~english @param [in]   availabilityId  The availability id(refer to DGC_REG_AVAILABILITY_XXX)
/// \~english @retval  DGCODE_RET_NORMAL    Success
/// \~english @retval  DGCODE_RET_ERROR     Failure
/// \~english @par Precondition
///      - None
/// \~english @par Changing internal state
///      - No change of internal state by this API occurs.
/// \~english @par Processing failure condition
///      - Subscribe the availability callback to NSFW failed [DGCODE_RET_ERROR]
/// \~english @par Classification
///      - Public
/// \~english @par Type
///      - Sync
/// \~english @par Details
///      - Subscribe the availability of the external module
///      - Use "|" to subscribe the availability of the multiply modules
///        example: Use the follow way to subscribe the availability of the backup manager and the clock
///        Diag_SubscribeAvailability(hApp, DGC_REG_AVAILABILITY_BACKUP | DGC_REG_AVAILABILITY_CLOCK)
/// \~english @see Diag_NotifyAvailability
///
/// \~english @ref DIAGCODE.h
///
/////////////////////////////////////////////////////////////////////////////////////
DGCODE_RET_API Diag_SubscribeAvailability(HANDLE hApp, uint8_t availabilityId);

/// \~english Notify the availability of external module
/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup Diag_NotifyAvailability
/// \~english @par Overview
///      - Notify the availability status of the external module
/// \~english @param [in]   availabilityId  The availability id(refer to DGC_REG_AVAILABILITY_XXX)
/// \~english @param [in]   bAvailability   Availability status(TRUE: Available, FALSE: Unavailable)
/// \~english @retval  DGCODE_RET_NORMAL    Success
/// \~english @par Precondition
///      - None
/// \~english @par Changing internal state
///      - No change of internal state by this API occurs.
/// \~english @par Processing failure condition
///      - None
/// \~english @par Classification
///      - Public
/// \~english @par Type
///      - Sync
/// \~english @par Details
///      - Notify the available status of external module
///      - Use "|" to notify the availability status of the multiply modules
///        example: Use the follow way to notify that the backup manager and the clock are available
///        Diag_NotifyAvailability(DGC_REG_AVAILABILITY_BACKUP | DGC_REG_AVAILABILITY_CLOCK, TRUE)
/// \~english @see Diag_SubscribeAvailability
///
/// \~english @ref DIAGCODE.h
///
/////////////////////////////////////////////////////////////////////////////////////
DGCODE_RET_API Diag_NotifyAvailability(uint8_t availabilityId, BOOL bAvailability);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup Diag_InitDiagCode
/// \~english @par Overview
///      - Perform initialization processing of diag code management.
/// \~english @param [in]   hApp            Application handle
/// \~english @param [in]   start_kind      activation type
/// \~english @param [in]   fpOnCmd         fpOnCmd callback definition
/// \~english @retval  DGCODE_RET_NORMAL    Success
/// \~english @retval  DGCODE_RET_ERROR     Failure
/// \~english @retval  DGCODE_RET_PARA_ERR  Invalid parameter
/// \~english @par Precondition
///      - BackupMgr is available
///      - PFDRECThread is available
///      - The generation/initialization of dispatcher for application(PadCreateDispatcherWithoutLoop etc.) is done.
/// \~english @par Changing internal state
///      - No change of internal state by this API occurs.
/// \~english @par Processing failure condition
///      - Initialization of diagcode management information is fail [DGCODE_RET_ERROR]
///      - When activation type is invalid [DGCODE_RET_PARA_ERR]
///      - When diag work memory allocation fails [DGCODE_RET_ERROR]
///      - Failure occurs when read data from the backup area [DGCODE_RET_ERROR]
///      - Failure occurs when write data to the backup area [DGCODE_RET_ERROR]
///      - The sum value is invalid when write data to the backup area. [DGCODE_RET_ERROR]
///      - In case acquisition of application name from argument hApp failed [DGCODE_RET_ERROR]
///      - Failed to register callback information specified by argument fpOnCmd [DGCODE_RET_ERROR]
/// \~english @par Classification
///      - Public
/// \~english @par Type
///      - Sync only(None communication)
/// \~english @par Details
///      - The API should be called only by DiagService
///      - Call this API, before registering, deleting, checking and acquiring the code.
///      - Call this API, before registering, deleting, checking and acquiring the DTC information.
///      - Call this API, before registering, deleting, checking and acquiring the RoB information.
///      - Call this API, before update all StatusOfDTC.
///      - This API is a process that is called only once at system startup and should be called with NS_BackupMgr with
///        Avalability set to TRUE.
///      - Calls the specified callback function when diag code registration / deletion is executed.
///      - Calls the specified callback function when DTC information registration / deletion is executed.
///      - Calls the specified callback function when RoB information registration / deletion is executed.
///      - You can get message data by calling FrameworkunifiedGetMsgDataOfSize in the callback function.
///      - message data, a diag code registration / erasure notification structure is set.
/// \~english @see none
///
/////////////////////////////////////////////////////////////////////////////////////
DGCODE_RET_API Diag_InitDiagCode(HANDLE hApp, uint8_t start_kind, CbFuncPtr fpOnCmd);

/// \~english Setting of diag code registration mask information
/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup Diag_SetDiagCodeMask
/// \~english @par Overview
///      - Perform diag code registration mask setting.
/// \~english @param [in]   onoff           Diag code registration mask setting
/// \~english @retval  DGCODE_RET_NORMAL    Success
/// \~english @retval  DGCODE_RET_ERROR     Failure
/// \~english @par Precondition
///      - BackupMgr is available
///      - The generation/initialization of dispatcher for application(PadCreateDispatcherWithoutLoop etc.) is done.
/// \~english @par Changing internal state
///      - No change of internal state by this API occurs.
/// \~english @par Processing failure condition
///      - Diag code registration mask setting is invalid [DGCODE_RET_ERROR]
///      - Initialization of diagcode management information is fail  [DGCODE_RET_ERROR]
///      - Failure occurs when write data to the backup area [DGCODE_RET_ERROR]
/// \~english @par Classification
///      - Public
/// \~english @par Type
///      - Sync
/// \~english @par Details
///      - The API should be called only by DiagService
///      - Set the registration mask of the diag code. If + B OFF, the setting information is held. (Initial state,
///        registration mask OFF)
///      - If diag code registration mask setting is ON, when Diag_PutDiagCode,
///        Diag_PutDiagCode_Custom of the diag code registration API is called,
///        do not register diag code and register FF data.
/// \~english @see Diag_PutDiagCode, Diag_PutDiagCode_Custom
///
/////////////////////////////////////////////////////////////////////////////////////
DGCODE_RET_API Diag_SetDiagCodeMask(uint8_t onoff);

/// \~english Synchronization of time stamp information
/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup Diag_SyncTimeStamp
/// \~english @par Overview
///      - Synchronize time stamp information.
/// \~english @param [in]   time_cnt        time counter
/// \~english @param [in]   trip_cnt        Trip counter
/// \~english @retval  DGCODE_RET_NORMAL    Success
/// \~english @retval  DGCODE_RET_ERROR     Failure
/// \~english @par Precondition
///      - BackupMgr is available
///      - The generation/initialization of dispatcher for application(PadCreateDispatcherWithoutLoop etc.) is done.
/// \~english @par Changing internal state
///      - No change of internal state by this API occurs.
/// \~english @par Processing failure condition
///      - Initialization of diagcode management information is fail [DGCODE_RET_ERROR]
///      - Failure occurs when export data from backup area [DGCODE_RET_ERROR]
/// \~english @par Classification
///      - Public
/// \~english @par Type
///      - Sync
/// \~english @par Details
///      - The API should be called only by DiagService
///      - Updates the time stamp information held by the library with the value of the specified time counter and
///        Trip counter.
///      - The diagnosis service should synchronize the time counter and Trip counter with this API when time
///        information from the time master is received.
///      - However, if any of the time counter and Trip counter specified in the argument is Fail value,
///        do not perform synchronization processing and and update the time stamp information with Fail value.
/// \~english @see DGC_TIMESTAMP_GETTIME
///
/////////////////////////////////////////////////////////////////////////////////////
DGCODE_RET_API Diag_SyncTimeStamp(uint32_t time_cnt, uint16_t trip_cnt);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup Diag_SyncDcmTime
/// \~english @par Brief
///      - Synchronize Hour information.
/// \~english @param [in] hour_info Hour information
///      - #DGCODE_SSR_INFO_TIME_INFO : SSR infomation (Time infomation for SSR, Time infomation for RoB)
/// \~english @retval DGCODE_RET_NORMAL   Success
/// \~english @retval DGCODE_RET_ERROR    Failure
/// \~english @par Precondition
///      - BackupMgr is available
/// \~english @par Changing internal state
///      - No change of internal state by this API occurs.
/// \~english @par Processing failure condition
///      - The pointer of Hour information is NULL [#DGCODE_RET_PARA_ERR]
///      - Initialization of diagcode management information is fail [#DGCODE_RET_ERROR]
///      - Get mutex failed [#DGCODE_RET_ERROR]
/// \~english @par Classification
///      - Public
/// \~english @par Type
///      - Sync only(None communication)
/// \~english @par Detail
///      - This API should be called only by DiagService.
///      - Updates the Hour information held by the library with the value of the specified Hour information.
///      - DiagService should synchronize Hour information with this API,
///        when DiagService received the Hour information from the DCM.
///      - If any of Hour information specified is Fail value,
///        do not perform synchronization processing, and update the Hour information with Fail value(0xFF).
/// \~english @see None
/////////////////////////////////////////////////////////////////////////////////////
DGCODE_RET_API Diag_SyncDcmTime(const DGCODE_SSR_INFO_TIME_INFO * const hour_info);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup Diag_SyncOdometer
/// \~english @par Overview
///      - Synchronize odo meter information.
/// \~english @param [in]   mileage         accumulative mileage
/// \~english @param [in]   unit            odo unit
/// \~english @retval  DGCODE_RET_NORMAL    Success
/// \~english @retval  DGCODE_RET_ERROR     Failure
/// \~english @retval  DGCODE_RET_PARA_ERR  Invalid parameter
/// \~english @par Precondition
///      - BackupMgr is available
///      - The generation/initialization of dispatcher for application(PadCreateDispatcherWithoutLoop etc.) is done.
/// \~english @par Changing internal state
///      - No change of internal state by this API occurs.
/// \~english @par Processing failure condition
///      - The accumulative mileage is invalid (mileage > 999,999) [\ref DGCODE_RET_PARA_ERR]
///      - Initialization of diagcode management information is fail [\ref DGCODE_RET_ERROR]
///      - Failure occurs when export data from backup area [\ref DGCODE_RET_ERROR]
/// \~english @par Classification
///      - Public
/// \~english @par Type
///      - Sync only(None communication)
/// \~english @par Details
///      - The API should be called only by DiagService
///      - Updates the odo meter information held by the library with the value of the specified accumulative mileage
///        and odo unit.
///      - The diagnosis service should synchronize the accumulative mileage and odo unit with this API when time
///        information from the odo meter is received.
/// \~english @see None
///
/////////////////////////////////////////////////////////////////////////////////////
DGCODE_RET_API Diag_SyncOdometer(const uint32_t mileage, const uint8_t unit);

/// \~english Inhibit diag code register
/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup Diag_InhibitPutDiagCode
/// \~english @par Overview
///      - Inhibit diag code register.
/// \~english @param   none
/// \~english @retval  DGCODE_RET_NORMAL     Success
/// \~english @retval  DGCODE_RET_ERROR      Failure
/// \~english @par Precondition
///      - The initialization of diag memory is finished.
/// \~english @par Changing internal state
///      - No change of internal state by this API occurs.
/// \~english @par Processing failure condition
///      - Initialization of diagcode management information is fail [DGCODE_RET_ERROR]
///      - Get mutex failed [DGCODE_RET_ERROR]
///      - The diag code's inhibit flag has not been initialized [DGCODE_RET_ERROR]
/// \~english @par Classification
///      - Public
/// \~english @par Type
///      - Sync
/// \~english @par Details
///      - The API should be called only by DiagService
///      - Diag code register inhibit.
/// \~english @see none
///
/////////////////////////////////////////////////////////////////////////////////////
DGCODE_RET_API Diag_InhibitPutDiagCode(void);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup Diag_PutDTCInfo
/// \~english @par Overview
///      - Register of DTC info.
/// \~english @param [in] hApp Application handle
/// \~english @param [in] dtc_id DTC ID\n
///        Use diag code definition defined in DIAGCODE.h.\n
/// \~english @param [in] test_result StatusOfDTC\n
///        testFailed(bit0 not Failed:0 Failed:1)\n
/// \~english @param [in] ssr_info SSR data info
/// \~english @param [in] spply_code Supplementary code
/// \~english @retval DGCODE_RET_NORMAL Success
/// \~english @retval DGCODE_RET_ERROR Failure
/// \~english @retval DGCODE_RET_PARA_ERR Invalid parameter
/// \~english @par Precondition
///      - Diagcode mask is unset.
///      - BackupMgr is available
///      - PFDRECThread is available
///      - The generation/initialization of dispatcher for application(PadCreateDispatcherWithoutLoop etc.) is done.
/// \~english @par Changing internal state
///      - No change of internal state by this API occurs.
/// \~english @par Processing failure condition
///      - Initialization of DTC management information is fail [DGCODE_RET_ERROR]
///      - DTC ID does not match the defination of DTC ROM table [DGCODE_RET_PARA_ERR]
///      - The communication unit ID(division) in the DTC ROM table is invalid [DGCODE_RET_ERROR]
///      - The communication unit ID(division) in the DTC ROM table is
///        none device(#DGCODE_DIV_UNIT_NONE) [DGCODE_RET_ERROR]
///      - Allocation of work memory fails when writing the DTC. [DGCODE_RET_ERROR]
///      - Failure occurs when reading data from backup area [DGCODE_RET_ERROR]
///      - Failure occurs when writing to the backup area [DGCODE_RET_ERROR]
///      - The sum value is invalid when writing to the backup area [DGCODE_RET_ERROR]
///      - Communication unit is not _CWORD92_ or other device when registering individual Diag Code. [DGCODE_RET_PARA_ERR]
///      - The pointer of DTC inhibit info is NULL [DGCODE_RET_ERROR]
/// \~english @par Classification
///      - Public
/// \~english @par Type
///      - Sync x Sync
/// \~english @par Details
///      - Register DTC info.
///      - When register development diag code for supplier, set the DTC ID assigned to supplier.
///      - Register MM DiagRecord, too.
///      - But if the register mask of DTC is set ON, this API returns DGCODE_RET_NORMAL
///        not registering the DTC info.
///      - diag_code sets the following bit of StatusOfDTC.\n
///        bit1:testFailedThisOperationCycle\n
///        bit2:pendingDTC\n
///        bit3:confirmedDTC\n
///        bit4:testNotCompletedSinceLastClear\n
///        bit5:testFailedSinceLastClear\n
///        bit6:testNotCompletedThisOperationCycle\n
///      - diag_code sets TimeStamp, Odometer and clock time of SSR.
///      - Treat current position longitude and current latitude of SSR as all 0 in this API.
///      - Set supplementary code if it is need.
///
/// \~english @ref DIAGCODE.h
///
/////////////////////////////////////////////////////////////////////////////////////
DGCODE_RET_API Diag_PutDTCInfo(const HANDLE hApp, const uint64_t dtc_id, const uint8_t test_result,
                               const DGCODE_DTC_SSR_INFO* ssr_info,
                               const uint16_t spply_code);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup Diag_PutRoBInfo
/// \~english @par Overview
///      - Register of RoB info.
/// \~english @param [in] hApp Application handle
/// \~english @param [in] rob_id RoB ID\n
///        Use diag code definition defined in DIAGCODE.h.\n
/// \~english @param [in] ssr_info SSR data info
/// \~english @retval DGCODE_RET_NORMAL Success
/// \~english @retval DGCODE_RET_ERROR Failure
/// \~english @retval DGCODE_RET_PARA_ERR Invalid parameter
/// \~english @par Precondition
///      - Diagcode mask is unset.
///      - BackupMgr is available
///      - PFDRECThread is available
///      - The generation/initialization of dispatcher for application(PadCreateDispatcherWithoutLoop etc.) is done.
/// \~english @par Changing internal state
///      - No change of internal state by this API occurs.
/// \~english @par Processing failure condition
///      - Initialization of RoB management information is fail [DGCODE_RET_ERROR]
///      - RoB ID does not match the defination of RoB ROM table [DGCODE_RET_PARA_ERR]
///      - The communication unit ID(division) in the RoB ROM table is invalid [DGCODE_RET_ERROR]
///      - The communication unit ID(division) in the RoB ROM table is
///        none device(#DGCODE_DIV_UNIT_NONE) [DGCODE_RET_ERROR]
///      - Allocation of work memory fails when writing the diag code. [DGCODE_RET_ERROR]
///      - Failure occurs when read data from backup area [DGCODE_RET_ERROR]
///      - Failure occur when write to the backup area [DGCODE_RET_ERROR]
///      - The sum value is invalid when write to the backup area [DGCODE_RET_ERROR]
///      - The pointer of RoB inhibit info is NULL [DGCODE_RET_ERROR]
/// \~english @par Classification
///      - Public
/// \~english @par Type
///      - Sync x Sync
/// \~english @par Details
///      - Register RoB info.
///      - When register development diag code for supplier, set the RoB ID assigned to supplier.
///      - Register MM DiagRecord, too.
///      - But if the register mask of Diag Code is set ON, this API returns DGCODE_RET_NORMAL
///        not registering the RoB info.
///      - Set SSR info related RoB ID to SSR data info(SSR info not related RoB ID is unnecessary).\n
///        diag_code sets TimeStamp, Odometer and clock time of SSR.
///      - Treat current position longitude and current latitude of SSR as all 0 in this API.
///
/// \~english @ref DIAGCODE.h
///
/////////////////////////////////////////////////////////////////////////////////////
DGCODE_RET_API Diag_PutRoBInfo(const HANDLE hApp, const uint64_t rob_id, const DGCODE_ROB_SSR_INFO* ssr_info);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup Diag_DeleteDTCInfo
/// \~english @par Overview
///      - Delete specified DTC.
/// \~english @param [in]   hApp       Application handle
/// \~english @param [in]   err_id       DTC detail info
/// \~english @retval  DGCODE_RET_NORMAL     Success
/// \~english @retval  DGCODE_RET_ERROR      Failure
/// \~english @retval  DGCODE_RET_PARA_ERR  Invalid parameter
/// \~english @par Precondition
///      - BackupMgr is available
///      - PFDRECThread is available
///      - The generation/initialization of dispatcher for application(PadCreateDispatcherWithoutLoop etc.) is done.
/// \~english @par Changing internal state
///      - No change of internal state by this API occurs.
/// \~english @par Processing failure condition
///      - Initialization of diagcode management information is fail [DGCODE_RET_ERROR]
///      - The param[err_id] do not match the defination in the diagcode ROM table [DGCODE_RET_PARA_ERR]
///      - The communication unit ID(division) in the diagcode ROM table is invalid [DGCODE_RET_ERROR]
///      - If the communication unit ID(division) is other in the diagcode ROM table,
///        and failure occur when getting unit number. [DGCODE_RET_ERROR]
/// \~english @par Classification
///      - Public
/// \~english @par Type
///      - Fire and Forget only
/// \~english @par Details
///      - For the latest diag which has been registered, do the erase procedure due to the fact that the DTC is initialized.
///      - If diagcode which specified is not registered, return DGCODE_RET_NORMAL.
///      - Register removal DTC to MM Diag Record.
///      - Delete SSR information,too.
///      - Delete status of DTC, too.
///
/// \~english @ref DIAGCODE.h
///
/////////////////////////////////////////////////////////////////////////////////////
DGCODE_RET_API Diag_DeleteDTCInfo(const HANDLE hApp, const uint64_t err_id);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup Diag_GetDTCInfoList_CWORD29_
/// \~english @par Overview
///      - Get the List of specified DTC
/// \~english @param [in]   get_info      Get info of DTC list
///      - #DGCODE_GET_DTC_LIST_INFO : DTC list acquisition setting structure
/// \~english @param [out]  list_info     List info of DTC
///      - #DGCODE_DTC_LIST_INFO : DTC list information structure
/// \~english @retval  DGCODE_RET_NORMAL    Success
/// \~english @retval  DGCODE_RET_ERROR     Failure
/// \~english @retval  DGCODE_RET_PARA_ERR  Invalid parameter
/// \~english @par Precondition
///      - The initialization of diag memory is finished.
///      - BackupMgr is available
/// \~english @par Changing internal state
///      - No change of internal state by this API occurs.
/// \~english @par Processing failure condition
///      - The pointer get_info of acquisition setting is NULL [DGCODE_RET_PARA_ERR]
///      - The pointer list_info of storage area is NULL [DGCODE_RET_PARA_ERR]
///      - Initialization of diagcode management information is fail [DGCODE_RET_ERROR]
///      - The flag of diag registration start is not OK yet(The function of diagcode is not prepared yet).
///        [DGCODE_RET_ERROR]
///      - The acquirement kind(diagcode) is invalid [DGCODE_RET_PARA_ERR]
///      - Common Diagnosis When the Diag code definition Name acquired from the memory
///        error table do not match the definition of the diagcode ROM Table [DGCODE_RET_PARA_ERR]
///      - The communication unit ID(division) in the diag code ROM talble is not valid [DGCODE_RET_ERROR]
///      - If the acquisition type is development or service and the division ID is other than the self device and
///        other devices [DGCODE_RET_PARA_ERR]
/// \~english @par Classification
///      - Public
/// \~english @par Type
///      - Sync only(None communication)
/// \~english @par Details
///      - Get the DTC list of conditions specified by get_info.
///      - Acquisition type must be set to diag code type definition value.
///      - StatusOfDTC information must be set to StatusOfDTC of getting DTC.\n
///        Targeting bit of StatusOfDTC information that is 1, this API gets DTC
///        that every target bit of StatusOfDTC is also 1.\n
///        Examples of getting DTC, when StatusOfDTC information is set 0b00100111 : 0b00101111、0b00100111
///      - Category ID must be set to category ID definition.
///      - If category ID is other than DGCODE_DIV_UNIT__CWORD92_, error registered device physical address must be set.
///      - If category ID is DGCODE_DIV_UNIT__CWORD92_, error registered device physical address is not used,
///        and thus an arbitary value may be set.
///      - Logic unit number for maker diag must be set DGCODE_MAKER_UNIT_ALL.
///      - All DTC that matches above condition store in list_info(list_info is variable length).
///      - The maximum number of DTC that can be acquired at a time is 64.
///      - By setting any value from 0 to 255 as the DTC acquisition start index position,
///        64 DTC can be acquired from that position.\n
///        To acquire from Nth DTC, N-1 should be set.\n
///        example : If the number of previous acquisitions is 64 and "DTC acquisition start index position" is
///        set to 64, DTC from 65th to 128th can be acquired.
///      - When the number of items remaining from DTC acquisition start index position to set
///        the last registered DTC is less than 64, only the remaining items are acquired.
///        example : If the case where 100 items of DTC are registered , when 64 is set
///        as the "DTC acquisition start index position", 36 items from 65th to 100th can be acquired.
///
/////////////////////////////////////////////////////////////////////////////////////
DGCODE_RET_API Diag_GetDTCInfoList_CWORD29_(const DGCODE_GET_DTC_LIST_INFO* const get_info,
                                   DGCODE_DTC_LIST_INFO* const list_info);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup Diag_DeleteDTCInfoList
/// \~english @par Overview
///      - Delete the list of specified DTC.
/// \~english @param [in]   del_info      Deletion info of DTC list
/// \~english @retval  DGCODE_RET_NORMAL    Success
/// \~english @retval  DGCODE_RET_ERROR     Failure
/// \~english @retval  DGCODE_RET_PARA_ERR  Invalid parameter
/// \~english @par Precondition
///      - BackupMgr is available.
///      - The initialization of diag memory is finished.
/// \~english @par Changing internal state
///      - No change of internal state by this API occurs.
/// \~english @par Processing failure condition
///      - The pointer del_info of acquisition setting is NULL [DGCODE_RET_PARA_ERR]
///      - Initialization of diagcode management information is fail [DGCODE_RET_ERROR]
///      - The flag of diag registration start is not OK yet(The function of diagcode is not prepared yet).
///        [DGCODE_RET_ERROR]
///      - Failure occurs when read data from the backup area [DGCODE_RET_ERROR]
///      - Failure occurs when write data to the backup area [DGCODE_RET_ERROR]
///      - The sum value is invalid when export data from backup area. [DGCODE_RET_ERROR]
///      - When the Category ID is invalid [DGCODE_RET_PARA_ERR]
///      - Register error of own device at all deletion Device physical address is invalid [DGCODE_RET_PARA_ERR]
/// \~english @par Classification
///      - Public
/// \~english @par Type
///      - Sync only(None communication)
/// \~english @par Details
///      - Delete the list of specified DTC by Category ID.
///      - Delete SSR information,too.
///
/////////////////////////////////////////////////////////////////////////////////////
DGCODE_RET_API Diag_DeleteDTCInfoList(const DGCODE_DELETE_DTC_INFO* const del_info);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup Diag_DeleteRoBInfo
/// \~english @par Overview
///      - Set the diagcode detail info and delete the diagcode.
/// \~english @param [in]   hApp       Application handle
/// \~english @param [in]   rob_id       RoB detail info
/// \~english @retval  DGCODE_RET_NORMAL     Success
/// \~english @retval  DGCODE_RET_ERROR      Failure
/// \~english @retval  DGCODE_RET_PARA_ERR  Invalid parameter
/// \~english @par Precondition
///      - BackupMgr is available
///      - PFDRECThread is available
///      - The generation/initialization of dispatcher for application(PadCreateDispatcherWithoutLoop etc.) is done.
/// \~english @par Changing internal state
///      - No change of internal state by this API occurs.
/// \~english @par Processing failure condition
///      - Initialization of diagcode management information is fail [DGCODE_RET_ERROR]
///      - The param[rob_id] do not match the defination in the diagcode ROM table [DGCODE_RET_PARA_ERR]
///      - The communication unit ID(division) in the diagcode ROM table is invalid [DGCODE_RET_ERROR]
///      - If the communication unit ID(division) is other in the diagcode ROM table,
///        and failure occur when getting unit number. [DGCODE_RET_ERROR]
/// \~english @par Classification
///      - Public
/// \~english @par Type
///      - Fire and Forget only
/// \~english @par Details
///      - For the RoB which has been registered, do the erase procedure due to the fact that the RoB is initialized.
///      - If diagcode which specified is not registered, return DGCODE_RET_NORMAL.
///      - Delete SSR information,too.
///
/// \~english @ref DIAGCODE.h
///
/////////////////////////////////////////////////////////////////////////////////////
DGCODE_RET_API Diag_DeleteRoBInfo(const HANDLE hApp, const uint64_t rob_id);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup Diag_GetRoBInfoList
/// \~english @par Overview
///      - Get the List of specified RoB
/// \~english @param [in]   get_info      Get info of RoB list
/// \~english @param [out]  list_info     List info of RoB
/// \~english @retval  DGCODE_RET_NORMAL    Success
/// \~english @retval  DGCODE_RET_ERROR     Failure
/// \~english @retval  DGCODE_RET_PARA_ERR  Invalid parameter
/// \~english @par Precondition
///      - The initialization of diag memory is finished.
///      - BackupMgr is available
/// \~english @par Changing internal state
///      - No change of internal state by this API occurs.
/// \~english @par Processing failure condition
///      - The pointer get_info of acquisition setting is NULL [DGCODE_RET_PARA_ERR]
///      - The pointer list_info of storage area is NULL [DGCODE_RET_PARA_ERR]
///      - Initialization of RoB code management information is fail [DGCODE_RET_ERROR]
///      - The flag of diag registration start is not OK yet(The function of RoB code is not prepared yet).
///        [DGCODE_RET_ERROR]
///      - When the acquisition type is other than "RoB code type definition" [DGCODE_RET_PARA_ERR]
///      - Common Diagnosis When the RoB code definition Name acquired from the memory
///        error table do not match the definition of the RoB code ROM Table [DGCODE_RET_PARA_ERR]
///      - The communication unit ID(division) in the RoB code ROM table is not valid [DGCODE_RET_ERROR]
/// \~english @par Classification
///      - Public
/// \~english @par Type
///      - Sync only(None communication)
/// \~english @par Details
///      - Get the RoB list of conditions specified by get_info.
///      - Acquisition type must be set to RoB code type definition value.
///      - Category ID must be set DGCODE_DIV_UNIT__CWORD92_.
///      - Error registered device physical address is not used, and thus an arbitary value may be set.
///      - Logic unit number for maker diag must be set DGCODE_MAKER_UNIT_ALL.
///      - For the RoB code, set the "RoB code" described in the diag code generation tool of the RoB
///        to be acquired.
///      - When the RoB code is #DGCODE_GET_ALL_ROB_CODE, the latest  RoB is acquired one of the registered RoB
///        for each RoB define.
///      - When th RoB code is other than #DGCODE_GET_ALL_ROB_CODE,
///        the list of RoB matching the RoB code is acquired.
///      - All DTC that matches above conditions store in list_info(list_info is variable length).
///      - The maximum number of RoB that can be acquired at a time is 64.
///      - By setting any value from 0 to 255 as the RoB acquisition start index position,
///        64 RoB can be acquired from that position.\n
///        To acquire from Nth RoB, N-1 should be set.\n
///        example : If the number of previous acquisitions is 64 and "RoB acquisition start index position" is
///        set to 64, RoB from 65th to 128th can be acquired.
///      - When the number of items remaining from RoB acquisition start index position to set
///        the last registered RoB is less than 64, only the remaining items are acquired.
///        example : If the case where 100 items of RoB are registered , when 64 is set
///        as the "RoB acquisition start index position", 36 items from 65th to 100th can be acquired.
///
/////////////////////////////////////////////////////////////////////////////////////
DGCODE_RET_API Diag_GetRoBInfoList(const DGCODE_GET_ROB_LIST_INFO* const get_info,
                                   DGCODE_ROB_LIST_INFO* const list_info);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup Diag_GetDIDList
/// \~english @par Overview
///      - Get DID list which relates DTC/RoB code.
/// \~english @param [in]   code            DTC or RoB
/// \~english @param [out]  list            structure of DID list
/// \~english @retval  DGCODE_RET_NORMAL    Success
/// \~english @retval  DGCODE_RET_ERROR     Failure
/// \~english @retval  DGCODE_RET_PARA_ERR  Invalid parameter
/// \~english @par Precondition
///      - None.
/// \~english @par Changing internal state
///      - No change of internal state by this API occurs.
/// \~english @par Processing failure condition
///      - Parameter of list is NULL. [\ref DGCODE_RET_PARA_ERR]
///      - DTC or RoB is associated parameter of code is not exist. [\ref DGCODE_RET_ERROR]
///      - Initialization of diagcode management information is fail [\ref DGCODE_RET_ERROR]
/// \~english @par Classification
///      - Public
/// \~english @par Type
///      - Sync only(None communication)
/// \~english @par Details
///      - Return DID list which is associated parameter of DTC or RoB.
/// \~english @see None
///
/////////////////////////////////////////////////////////////////////////////////////
DGCODE_RET_API Diag_GetDIDList(const uint64_t code, DGCODE_DID_LIST* const list);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup Diag_DeleteRoBInfoList
/// \~english @par Overview
///      - Delete the list of specified RoB.
/// \~english @param [in]   del_info      Deletion info of RoB code list
/// \~english @retval  DGCODE_RET_NORMAL    Success
/// \~english @retval  DGCODE_RET_ERROR     Failure
/// \~english @retval  DGCODE_RET_PARA_ERR  Invalid parameter
/// \~english @par Precondition
///      - BackupMgr is available.
///      - The initialization of diag memory is finished.
/// \~english @par Changing internal state
///      - No change of internal state by this API occurs.
/// \~english @par Processing failure condition
///      - The pointer del_info of acquisition setting is NULL [DGCODE_RET_PARA_ERR]
///      - Initialization of diagcode management information is fail [DGCODE_RET_ERROR]
///      - The flag of diag registration start is not OK yet(The function of diagcode is not prepared yet).
///        [DGCODE_RET_ERROR]
///      - Failure occurs when read data from the backup area [DGCODE_RET_ERROR]
///      - Failure occurs when write data to the backup area [DGCODE_RET_ERROR]
///      - The sum value is invalid when export data from backup area. [DGCODE_RET_ERROR]
///      - When the Category ID is invalid [DGCODE_RET_PARA_ERR]
///      - Register error of own device at all deletion Device physical address is invalid [DGCODE_RET_PARA_ERR]
/// \~english @par Classification
///      - Public
/// \~english @par Type
///      - Sync only(None communication)
/// \~english @par Details
///      - Delete the list of specified RoB by Category ID.
///      - Delete SSR information,too.
///
/////////////////////////////////////////////////////////////////////////////////////
DGCODE_RET_API Diag_DeleteRoBInfoList(const DGCODE_DELETE_ROB_INFO* const del_info);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup Diag_StartupAllStatusOfDTC
/// \~english @par Overview
///      - Update all StatusOfDTC for startup of OperationCycle.
/// \~english @param none
/// \~english @retval DGCODE_RET_NORMAL Success
/// \~english @retval DGCODE_RET_ERROR Failure
/// \~english @par Precondition
///      - BackupMgr is available
/// \~english @par Changing internal state
///      - No change of internal state by this API occurs.
/// \~english @par Processing failure condition
///      - DTC ID does not match the defination of DTC ROM table [DGCODE_RET_ERROR]
///      - Allocation of work memory fails when update the StatusOfDTC. [DGCODE_RET_ERROR]
///      - Failure occurs when reading data from backup area [DGCODE_RET_ERROR]
///      - Failure occurs when writing to the backup area [DGCODE_RET_ERROR]
///      - The sum value is invalid when writing to the backup area [DGCODE_RET_ERROR]
///      - The pointer of DTC inhibit info is NULL [DGCODE_RET_ERROR]
/// \~english @par Classification
///      - Public
/// \~english @par Type
///      - Sync only(None communication)
/// \~english @par Details
///      - This API should be called only by SystemManager.
///      - Update all StatusOfDTC for startup of OperationCycle.
///      - This API sets the following bit of StatusOfDTC.\n
///        bit0:testFailed is set to '0'.\n
///        bit1:testFailedThisOperationCycle is set to '0'.\n
///        bit2:pendingDTC is set to '0' only when bit1 and bit6 (before update) are both '0'.\n
///        bit6:testNotCompletedThisOperationCycle is set to '1'.
/// \~english @ref DIAGCODE.h
///
/////////////////////////////////////////////////////////////////////////////////////
DGCODE_RET_API Diag_StartupAllStatusOfDTC(void);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup Diag_ClearAllStatusOfDTC
/// \~english @par Overview
///      - Update all StatusOfDTC when receiving ClearDiagnosticInformation.
/// \~english @param none
/// \~english @retval DGCODE_RET_NORMAL Success
/// \~english @retval DGCODE_RET_ERROR Failure
/// \~english @par Precondition
///      - BackupMgr is available
/// \~english @par Changing internal state
///      - No change of internal state by this API occurs.
/// \~english @par Processing failure condition
///      - DTC ID does not match the defination of DTC ROM table [DGCODE_RET_ERROR]
///      - Allocation of work memory fails when update the StatusOfDTC. [DGCODE_RET_ERROR]
///      - Failure occurs when reading data from backup area [DGCODE_RET_ERROR]
///      - Failure occurs when writing to the backup area [DGCODE_RET_ERROR]
///      - The sum value is invalid when writing to the backup area [DGCODE_RET_ERROR]
///      - The pointer of DTC inhibit info is NULL [DGCODE_RET_ERROR]
/// \~english @par Classification
///      - Public
/// \~english @par Type
///      - Sync only(None communication)
/// \~english @par Details
///      - This API should be called only by DiagService.
///      - Update all StatusOfDTC for startup of OperationCycle.
///      - This API sets the following bit of StatusOfDTC.\n
///        bit0:testFailed is set to '0'.\n
///        bit1:testFailedThisOperationCycle is set to '0'.\n
///        bit2:pendingDTC is set to '0'.\n
///        bit3:confirmedDTC is set to '0'.\n
///        bit4:testNotCompletedSinceLastClear is set to '1'.\n
///        bit5:testFailedSinceLastClear is set to '0'.\n
///        bit6:testNotCompletedThisOperationCycle is set to '1'.
/// \~english @ref DIAGCODE.h
///
/////////////////////////////////////////////////////////////////////////////////////
DGCODE_RET_API Diag_ClearAllStatusOfDTC(void);

/// \~english Acquire time counter value
/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DGC_TIMESTAMP_GETTIME
/// \~english @par Overview
///      - This is a macro definition for extracting the time counter value included in the time counter information.
/// \~english @param [in]   x    time counter information
/// \~english @retval            time counter value
/// \~english @par Precondition
///      - None.
/// \~english @par Changing internal state
///      - No change of internal state by this API occurs.
/// \~english @par Processing failure condition
///      - None.
/// \~english @par Classification
///      - Public
/// \~english @par Type
///      - Sync
/// \~english @par Details
///      - Mask the unique count identification value and return only the time counter value.
/// \~english @see Diag_SyncTimeStamp
///
/////////////////////////////////////////////////////////////////////////////////////
#define DGC_TIMESTAMP_GETTIME(x)    ((x) & (0x00FFFFFFU))

/// \~english Get unique count value
/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DGC_TIMESTAMP_GETFLAG
/// \~english @par Overview
///      - This is a macro definition for extracting the unique counter identification value included in the time
///        counter information.
/// \~english @param [in]   x    time counter information
/// \~english @retval            unique counter identification value
/// \~english @par Precondition
///      - None.
/// \~english @par Changing internal state
///      - No change of internal state by this API occurs.
/// \~english @par Processing failure condition
///      - None.
/// \~english @par Classification
///      - Public
/// \~english @par Type
///      - Sync
/// \~english @par Details
///      - Mask the time counter value and return only the unique count identification value.
/// \~english @see Diag_SyncTimeStamp
///
/////////////////////////////////////////////////////////////////////////////////////
#define DGC_TIMESTAMP_GETFLAG(x)    (((x) & (0xE0000000U)) >> (29U))

#ifdef  __cplusplus
}
#endif

/** @}*/  // end of diag_code
/** @}*/  // end of vehicle_service
/** @}*/  // end of BaseSystem

#endif  // DIAG_CODE_LIBRARY_INCLUDE_VEHICLE_SERVICE_DIAGCODEAPI_H_
