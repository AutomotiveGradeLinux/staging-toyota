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
 * @file ss_string_maps.h
 * @brief \~english This file supports the a generic std::map<std::string> wrapper.
 */
/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup system_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup interface_unified
 *  @ingroup system_service
 *  @{
 */
#ifndef SS_STRING_MAPS_H_  // NOLINT (build/header_guard)
#define SS_STRING_MAPS_H_

#include "system_service/ss_power_service.h"
#include "system_service/ss_system_manager_if.h"
#include "system_service/ss_system_manager_protocol.h"

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup GetStr
/// \~english @par Summary
///       change member from BOOL type to string
/// \~english @param [in] f_enum
///         BOOL - the member what will convert farm BOOL type to string type
/// \~english @code
///            typedef int BOOL;
///            #define TRUE  (1==1)
///            #define true  (1==1)
///            #define FALSE (0==1)
///            @endcode
/// \~english @retval SS_String
/// \~english @par Preconditions
///       - None.
/// \~english @par Change of the internal state
///       - The internal state is not changed.
/// \~english @par Causes of failures
///       None
/// \~english @par Classification
///       Public
/// \~english @par Type
///          sync only
/// \~english @par Detail
///       change member from BOOL type to string. \n
/// \~english @see
////////////////////////////////////////////////////////////////////////////////////
extern SS_String GetStr(BOOL f_enum);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup GetStr
/// \~english @par Summary
///       change member from EErrorEventType type to string
/// \~english @param [in] f_enum
///         EErrorEventType - the member what will convert farm EErrorEventType type to string type.error event type.
/// \~english @code
///            enum EErrorEventType {
///                eErrorEventTypeProcessCrash = 0,
///                eErrorEventTypeHeartBeatFailure,
///                eErrorEventTypeSystemLowMemory,
///                eErrorEventTypeReserved1,  // Formerly SS_SM_EVENT_ERROR_HIGH_CPU_LOAD, now HMI enum place holder.
///                eErrorEventTypeUserInvokedUserForceReset,
///                eErrorEventTypeReserved2,
///                eErrorEventTypeReserved3,
///                eErrorEventTypeUserInvokedCollectAllLogs,
///                eErrorEventTypeBootMicroReset,
///                eErrorEventTypeProcessExit,
///                eErrorEventTypeUserInvokedCollectScreenShot,
///                eErrorEventTypeUserInvokedCollectInterfaceunifiedLogs,
///                eErrorEventTypeEelExport,
///                eErrorEventTypeInterfaceunifiedEmmcLogs,
///                eErrorEventTypeDiagEvent,
///                eErrorEventTypeCanEvent,
///                eErrorEventTypeDtcEvent,
///                eErrorEventTypeModConnFailed,
///                eErrorEventTypeStartRespFailed,
///                eErrorEventTypeUserInvokedCollectDevLogs,
///                eErrorEventTypeModuleInvokedResetRequest,
///                eErrorEventTypeModuleInvokedCollectDebugLogs,
///                eErrorEventTypeUserInvokedClearLogs,
///                eErrorEventTypeUserInvokedCollectNaviLog,
///                eErrorEventTypeGroupRelaunch,
///                eErrorEventTypeMaxValue
///            };
///            @endcode
/// \~english @retval SS_String
/// \~english @par Preconditions
///       - None.
/// \~english @par Change of the internal state
///       - The internal state is not changed.
/// \~english @par Causes of failures
///       None
/// \~english @par Classification
///       Public
/// \~english @par Type
///          sync only
/// \~english @par Detail
///       change member from EErrorEventType type to string. \n
/// \~english @see
////////////////////////////////////////////////////////////////////////////////////
extern SS_String GetStr(EErrorEventType f_enum);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup GetStr
/// \~english @par Summary
///       change member from EFrameworkunifiedSystemError type to string
/// \~english @param [in] f_enum
///         EFrameworkunifiedSystemError - the member what will convert farm EFrameworkunifiedSystemError type to string type.The type of system
///         error.
/// \~english @codes
///            typedef enum _EFrameworkunifiedSystemError {
///              eFrameworkunifiedSystemErrorNone = 0,
///              eFrameworkunifiedDSPHardwareReset = 1
///            } EFrameworkunifiedSystemError;
///            @endcode
/// \~english @retval SS_String
/// \~english @par Preconditions
///       - None.
/// \~english @par Change of the internal state
///       - The internal state is not changed.
/// \~english @par Causes of failures
///       None
/// \~english @par Classification
///       Public
/// \~english @par Type
///          sync only
/// \~english @par Detail
///       change member from EFrameworkunifiedSystemError type to string. \n
/// \~english @see
////////////////////////////////////////////////////////////////////////////////////
extern SS_String GetStr(EFrameworkunifiedSystemError f_enum);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup GetStr
/// \~english @par Summary
///       change member from EPWR_LHC_TYPE type to string
/// \~english @param [in] f_enum
///         EPWR_LHC_TYPE - the member what will convert farm EPWR_LHC_TYPE type to string type.Limp home cutoff
///         protocol type.
/// \~english @codes
///            typedef enum {
///              epslhcINVALID   = 0xFF,
///              epslhcDISABLED  = 0x00,
///              epslhcENABLED   = 0x01
///            } ePwrServiceLHCType, EPWR_LHC_TYPE;
///            @endcode
/// \~english @retval SS_String
/// \~english @par Preconditions
///       - None.
/// \~english @par Change of the internal state
///       - The internal state is not changed.
/// \~english @par Causes of failures
///       None
/// \~english @par Classification
///       Public
/// \~english @par Type
///          sync only
/// \~english @par Detail
///       change member from EPWR_LHC_TYPE type to string. \n
/// \~english @see
////////////////////////////////////////////////////////////////////////////////////
extern SS_String GetStr(EPWR_LHC_TYPE f_enum);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup GetStr
/// \~english @par Summary
///       change member from EPWR_PROD_MODE_TYPE type to string
/// \~english @param [in] f_enum
///         EPWR_PROD_MODE_TYPE - the member what will convert farm EPWR_PROD_MODE_TYPE type to string type.The type of
///         production mode protocol.
/// \~english @codes
///            typedef enum {
///              epspmINVALID   = 0xFF,
///              epspmDISABLED  = 0x00,
///              epspmENABLED   = 0x01
///            } ePwrServiceProdModeType, EPWR_PROD_MODE_TYPE;
///            @endcode
/// \~english @retval SS_String
/// \~english @par Preconditions
///       - None.
/// \~english @par Change of the internal state
///       - The internal state is not changed.
/// \~english @par Causes of failures
///       None
/// \~english @par Classification
///       Public
/// \~english @par Type
///          sync only
/// \~english @par Detail
///       change member from EPWR_PROD_MODE_TYPE type to string. \n
/// \~english @see
////////////////////////////////////////////////////////////////////////////////////
extern SS_String GetStr(EPWR_PROD_MODE_TYPE f_enum);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup GetStr
/// \~english @par Summary
///       change member from EPWR_SC_COLD_START_REQ_TYPE type to string
/// \~english @param [in] f_enum
///         EPWR_SC_COLD_START_REQ_TYPE - the member what will convert farm EFrameworkunifiedSystemError type to string type.The type
///         of cold start.
/// \~english @codes
///            typedef enum {
///              epsscrtINVALID      = 0xFF,
///              epsscrtNOT_REQUIRED = 0x00,
///              epsscrtREQUIRED     = 0x01
///            } EPWR_SC_COLD_START_REQ_TYPE;
///            @endcode
/// \~english @retval SS_String
/// \~english @par Preconditions
///       - None.
/// \~english @par Change of the internal state
///       - The internal state is not changed.
/// \~english @par Causes of failures
///       None
/// \~english @par Classification
///       Public
/// \~english @par Type
///          sync only
/// \~english @par Detail
///       change member from EPWR_SC_COLD_START_REQ_TYPE type to string. \n
/// \~english @see
////////////////////////////////////////////////////////////////////////////////////
extern SS_String GetStr(EPWR_SC_COLD_START_REQ_TYPE f_enum);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup GetStr
/// \~english @par Summary
///       change member from EPWR_SC__CWORD56__BOOT_MODE_TYPE type to string
/// \~english @param [in] f_enum
///         EPWR_SC__CWORD56__BOOT_MODE_TYPE - the member what will convert farm EPWR_SC__CWORD56__BOOT_MODE_TYPE type to string
///         type.The type of startup confirmation.
/// \~english @codes
///            typedef enum {
///              eps_CWORD56_bmINVALID          = 0xFF,
///              eps_CWORD56_bmAPPLICATION_MODE = 0x00,
///              eps_CWORD56_bmPROGRAMMING_MODE = 0x08
///            } EPWR_SC__CWORD56__BOOT_MODE_TYPE;
///            @endcode
/// \~english @retval SS_String
/// \~english @par Preconditions
///       - None.
/// \~english @par Change of the internal state
///       - The internal state is not changed.
/// \~english @par Causes of failures
///       None
/// \~english @par Classification
///       Public
/// \~english @par Type
///          sync only
/// \~english @par Detail
///       change member from EPWR_SC__CWORD56__BOOT_MODE_TYPE type to string. \n
/// \~english @see
////////////////////////////////////////////////////////////////////////////////////
extern SS_String GetStr(EPWR_SC__CWORD56__BOOT_MODE_TYPE f_enum);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup GetStr
/// \~english @par Summary
///       change member from EPWR_SHUTDOWN_TRIGGER_TYPE type to string
/// \~english @param [in] f_enum
///         EPWR_SHUTDOWN_TRIGGER_TYPE - the member what will convert farm EPWR_SHUTDOWN_TRIGGER_TYPE type to string
///         type.The type of shutdown trigger.
/// \~english @codes
///            typedef enum {
///              epssdmsdtINVALID             = 0xFF,
///              epssdmsdtTESTACC_OFF         = 0x00,
///              epssdmsdtON_KEY              = 0x01,
///              epssdmsdtIGN_LOCK            = 0x02,
///              epssdmsdtPWR_SAVE            = 0x03,
///              epssdmsdtTMP_STARTUP         = 0x04,
///              epssdmsdtDIAG_DEACTIVATION   = 0x05,
///              epssdmsdtABNORMAL_VOLTAGE    = 0x06,
///              epssdmsdtABNORMAL_TEMP       = 0x07,
///              epssdmsdtBATTERYCUTOFF       = 0x08,
///              epssdmsdtLIMPHOME            = 0x09,
///              epssdmsdtHU_CAN_ERROR        = 0x0A,
///              epssdmsdtBODY_CAN_ERROR      = 0x0B,
///              epssdmsdtTRANSPORT_MODE      = 0x0C,
///              epssdmsdtPRODUCTION_MODE     = 0x0D,
///              epssdmsdtIGN_OFF             = 0x0E,
///              epssdmsdtGENERIC_ERROR_RESET = 0x0F,
///              epssdmsdtFATAL_ERROR_RESET   = 0x10,
///              epssdmsdtUSER_DATA_RESET     = 0x11,
///              epssdmsdtFACTORY_DATA_RESET  = 0x12,
///              epssdmsdtFAST_SLEEP_MODE     = 0x13,
///              epssdmsdtNORMAL_RESET        = 0x14,
///              epssdmsdtPROGUPDATE_RESET    = 0x15,
///            } ePowerSrvPwrShutdownTriggerType, EPWR_SHUTDOWN_TRIGGER_TYPE;
///            @endcode
/// \~english @retval SS_String
/// \~english @par Preconditions
///       - None.
/// \~english @par Change of the internal state
///       - The internal state is not changed.
/// \~english @par Causes of failures
///       None
/// \~english @par Classification
///       Public
/// \~english @par Type
///          sync only
/// \~english @par Detail
///       change member from EPWR_SHUTDOWN_TRIGGER_TYPE type to string. \n
/// \~english @see
////////////////////////////////////////////////////////////////////////////////////
extern SS_String GetStr(EPWR_SHUTDOWN_TRIGGER_TYPE f_enum);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup GetStr
/// \~english @par Summary
///       change member from EPWR_STARTUP_STAGE_TYPE type to string
/// \~english @param [in] f_enum
///         EPWR_STARTUP_STAGE_TYPE - the member what will convert farm EPWR_STARTUP_STAGE_TYPE type to string type.The
///         type of startup stage.
/// \~english @codes
///            typedef enum {
///              epssusfINVALID                = 0xFF,
///              epssusSYSTEM_SERVICES_STARTED = 0x00,
///              epssusALL_SERVICES_LAUNCHED   = 0x01
///            }ePowerSrvStartupStageType, EPWR_STARTUP_STAGE_TYPE;
///            @endcode
/// \~english @retval SS_String
/// \~english @par Preconditions
///       - None.
/// \~english @par Change of the internal state
///       - The internal state is not changed.
/// \~english @par Causes of failures
///       None
/// \~english @par Classification
///       Public
/// \~english @par Type
///          sync only
/// \~english @par Detail
///       change member from EPWR_STARTUP_STAGE_TYPE type to string. \n
/// \~english @see
////////////////////////////////////////////////////////////////////////////////////
extern SS_String GetStr(EPWR_STARTUP_STAGE_TYPE f_enum);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup GetStr
/// \~english @par Summary
///       change member from EPWR_SYSTEM_MODE_INFO type to string
/// \~english @param [in] f_enum
///         EPWR_SYSTEM_MODE_INFO - the member what will convert farm EPWR_SYSTEM_MODE_INFO type to string type.The type
///         of system mode info.
/// \~english @codes
///            typedef enum {
///              epssinfINVALID      = 0xFF,
///              epssinfNORMAL       = 0x00,
///              epssinfPROGRAMMING  = 0x08,
///            }ePowerSrvSystemModeInfo, EPWR_SYSTEM_MODE_INFO;
///            @endcode
/// \~english @retval SS_String
/// \~english @par Preconditions
///       - None.
/// \~english @par Change of the internal state
///       - The internal state is not changed.
/// \~english @par Causes of failures
///       None
/// \~english @par Classification
///       Public
/// \~english @par Type
///          sync only
/// \~english @par Detail
///       change member from EPWR_SYSTEM_MODE_INFO type to string. \n
/// \~english @see
////////////////////////////////////////////////////////////////////////////////////
extern SS_String GetStr(EPWR_SYSTEM_MODE_INFO f_enum);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup GetStr
/// \~english @par Summary
///       change member from EPWR_TRANSPORT_MODE_TYPE type to string
/// \~english @param [in] f_enum
///         EPWR_TRANSPORT_MODE_TYPE - the member what will convert farm EPWR_TRANSPORT_MODE_TYPE type to string
///         type.The type of transport mode protocol.
/// \~english @codes
///            typedef enum {
///              epstmINVALID   = 0xFF,
///              epstmDISABLED  = 0x00,
///              epstmENABLED   = 0x01
///            } ePwrServiceTransportModeType, EPWR_TRANSPORT_MODE_TYPE;
///            @endcode
/// \~english @retval SS_String
/// \~english @par Preconditions
///       - None.
/// \~english @par Change of the internal state
///       - The internal state is not changed.
/// \~english @par Causes of failures
///       None
/// \~english @par Classification
///       Public
/// \~english @par Type
///          sync only
/// \~english @par Detail
///       change member from EPWR_TRANSPORT_MODE_TYPE type to string. \n
/// \~english @see
////////////////////////////////////////////////////////////////////////////////////
extern SS_String GetStr(EPWR_TRANSPORT_MODE_TYPE f_enum);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup GetStr
/// \~english @par Summary
///       change member from \ref ePwrServiceUserModeChangeReasonType "EPWR_USER_MODE_CHANGE_REASON_TYPE" type to string
/// \~english @param [in] f_enum
///       \ref ePwrServiceUserModeChangeReasonType "EPWR_USER_MODE_CHANGE_REASON_TYPE" - the member what will convert farm \ref ePwrServiceUserModeChangeReasonType "EPWR_USER_MODE_CHANGE_REASON_TYPE" type
///         to string type.The type of user mode change reason.
/// \~english @retval SS_String
/// \~english @par Preconditions
///       - None.
/// \~english @par Change of the internal state
///       - The internal state is not changed.
/// \~english @par Causes of failures
///       None
/// \~english @par Classification
///       Public
/// \~english @par Type
///          sync only
/// \~english @par Detail
///       change member from type \ref ePwrServiceUserModeChangeReasonType "EPWR_USER_MODE_CHANGE_REASON_TYPE"  to string. \n
/// \~english @see
////////////////////////////////////////////////////////////////////////////////////
extern SS_String GetStr(EPWR_USER_MODE_CHANGE_REASON_TYPE f_enum);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup GetStr
/// \~english @par Summary
///       change member from EPWR_USER_MODE_TYPE type to string
/// \~english @param [in] f_enum
///         EPWR_USER_MODE_TYPE - the member what will convert farm EPWR_USER_MODE_TYPE type to string type.The type of
///         user mode.
/// \~english @codes
///            typedef enum {
///              epsumINVALID    = 0xFF,
///              epsumOFF        = 0x00,
///              epsumON         = 0x01,
///            } ePwrServiceUserModeType, EPWR_USER_MODE_TYPE;
///            @endcode
/// \~english @retval SS_String
/// \~english @par Preconditions
///       - None.
/// \~english @par Change of the internal state
///       - The internal state is not changed.
/// \~english @par Causes of failures
///       None
/// \~english @par Classification
///       Public
/// \~english @par Type
///          sync only
/// \~english @par Detail
///       change member from EPWR_USER_MODE_TYPE type to string. \n
/// \~english @see
////////////////////////////////////////////////////////////////////////////////////
extern SS_String GetStr(EPWR_USER_MODE_TYPE f_enum);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup GetStr
/// \~english @par Summary
///       change member from EPWR_WAKEUP_FACTOR_TYPE type to string
/// \~english @param [in] f_enum
///         EPWR_WAKEUP_FACTOR_TYPE - the member what will convert farm EPWR_WAKEUP_FACTOR_TYPE type to string type.The
///         type of startup reason.
/// \~english @codes
///            typedef enum {
///              epswfINVALID         = 0xFF,
///              epswfTESTACC         = 0x00,
///              epswfON_KEY          = 0x01,
///              epswfIGN_ACC         = 0x02,
///              epswfDR_OPEN_CLOSE   = 0x03,
///              epswfDX_ACTIVATION   = 0x04,
///              epswfPASS_ACTIVATION = 0x05,
///              epswfSPVACTIVATION   = 0x06,
///              epswfUSER_DATA_RESET = 0x07
///            } ePowerSrvWakeupFactors, EPWR_WAKEUP_FACTOR_TYPE;
///            @endcode
/// \~english @retval SS_String
/// \~english @par Preconditions
///       - None.
/// \~english @par Change of the internal state
///       - The internal state is not changed.
/// \~english @par Causes of failures
///       None
/// \~english @par Classification
///       Public
/// \~english @par Type
///          sync only
/// \~english @par Detail
///       change member from EPWR_WAKEUP_FACTOR_TYPE type to string. \n
/// \~english @see
////////////////////////////////////////////////////////////////////////////////////
extern SS_String GetStr(EPWR_WAKEUP_FACTOR_TYPE f_enum);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup GetStr JP_ZPT
/// \~english @par Summary
///       change member from EFrameworkunifiedStatus type to string
/// \~english @param [in] f_enum
///         EFrameworkunifiedStatus - the member what will convert farm EFrameworkunifiedStatus type to string type.Status and return types.
/// \~english @codes
///            typedef enum eFrameworkunifiedStatus {
///              eFrameworkunifiedStatusEmptyMediaList    = -10, ///< Empty media list
///              eFrameworkunifiedStatusSessionLimitMaxedOut  = -9,  ///< Maximum session limit reached
///              eFrameworkunifiedStatusDbRecNotFound       = -8, ///< Database record not found
///              eFrameworkunifiedStatusDbResultError       = -7, ///< Database result error
///              eFrameworkunifiedStatusDbExecuteFail       = -6, ///< Database execute fail
///              eFrameworkunifiedStatusSemCloseFail        = -5, ///< Semaphore close failed
///              eFrameworkunifiedStatusSemUnlockFail       = -4, ///< Semaphore unlock failed
///              eFrameworkunifiedStatusSemLockFail         = -3, ///< Semaphore lock failed
///              eFrameworkunifiedStatusFail                = -2, ///< Failed
///              eFrameworkunifiedStatusErrOther            = -1, ///< Unknown error
///              eFrameworkunifiedStatusOK                  = 0,  ///< Success / Pass / OK
///              eFrameworkunifiedStatusInvldBuf            = 1,  ///< Invalid buffer
///              eFrameworkunifiedStatusInvldHandle         = 2,  ///< Invalid handle
///              eFrameworkunifiedStatusInvldHndlType       = 3,  ///< Invalid handle type
///              eFrameworkunifiedStatusInvldQName        = 4,  ///< Invalid messasge queue name
///              eFrameworkunifiedStatusMsgQFull            = 5,  ///< Message queue full
///              eFrameworkunifiedStatusInvldNotification   = 6,///< The Notification event not present
///              eFrameworkunifiedStatusInvldParam        = 7,  ///< Invalid parameter
///              eFrameworkunifiedStatusInvldBufSize        = 8,  ///< Buf size too small
///              eFrameworkunifiedStatusInvldID             = 9,  ///< Unrecognized ID
///              eFrameworkunifiedStatusCannotRelease       = 10, ///< Cannot release resource
///              eFrameworkunifiedStatusBadConnection       = 11, ///< Could not locate resource
///              eFrameworkunifiedStatusExit            = 12,  ///< Normal application termination
///              eFrameworkunifiedStatusNotImplemented      = 13,  ///< incomplete feature
///              eFrameworkunifiedStatusThreadBusy          = 14,  ///< Joined thread is already being joined
///              eFrameworkunifiedStatusThreadSelfJoin      = 15,  ///< Thread is joining itself
///              eFrameworkunifiedStatusThreadInvalidVal    = 16, ///< Invalid value passed
///              eFrameworkunifiedStatusThreadNotExist      = 17,  ///< The thread does not exist
///              eFrameworkunifiedStatusFault               = 18,  ///< A fault occurred while attempting to make call
///              eFrameworkunifiedStatusServNotFound        = 19,  ///< Service not present in serv dir
///              eFrameworkunifiedStatusServerInUse         = 20,  ///< Service already processing 1 client request
///              eFrameworkunifiedStatusDbIndexing        = 21,  ///< Database Indexing in progress
///              eFrameworkunifiedStatusNullPointer       = 22,
///              eFrameworkunifiedStatusMsgNotProcessed     = 23,
///              eFrameworkunifiedStatusFileLoadSuccess     = 24, ///< File Load Success
///              eFrameworkunifiedStatusFileLoadError       = 25, ///< File Load Error
///              eFrameworkunifiedStatusAccessError         = 26, ///< Error when accessing resource
///              eFrameworkunifiedStatusDuplicate           = 27, ///< Duplicate entry
///              eFrameworkunifiedStatusMsgQEmpty             = 28, ///< Message queue empty
///              eFrameworkunifiedStatusThreadAlreadyRunning  = 29,
///              eFrameworkunifiedStatusErrNoEBADF            = 30,  ///< Bad file descriptor
///              eFrameworkunifiedStatusErrNoEAGAIN           = 31,  ///< Resource unavailable, try again
///              eFrameworkunifiedStatusErrNoEINTR            = 32,  ///< Interrupted system call
///              eFrameworkunifiedStatusSessionErr      = 33,  ///< Error in session handling
///              eFrameworkunifiedStatusDBCorrupt       = 34,  ///< Database corrupt
///              eFrameworkunifiedStatusDBFileNotFound    = 35  ///< Database file not found
///            } EFrameworkunifiedStatus, *PEFrameworkunifiedStatus;
///            @endcode
/// \~english @retval SS_String
/// \~english @par Preconditions
///       - None.
/// \~english @par Change of the internal state
///       - The internal state is not changed.
/// \~english @par Causes of failures
///       None
/// \~english @par Classification
///       Public
/// \~english @par Type
///          sync only
/// \~english @par Detail
///       change member from EFrameworkunifiedStatus type to string. \n
/// \~english @see
////////////////////////////////////////////////////////////////////////////////////
extern SS_String GetStr(EFrameworkunifiedStatus f_enum);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup GetStr
/// \~english @par Summary
///       change member from ESMBootModeInfo type to string
/// \~english @param [in] f_enum
///         ESMBootModeInfo - the member what will convert farm ESMBootModeInfo type to string type.The type of boot
///         mode information.
/// \~english @codes
///            typedef enum {
///                e_SS_SM_BOOT_MODE_INVALID     = -1
///              , e_SS_SM_BOOT_MODE_APPLICATION
///              , e_SS_SM_BOOT_MODE_PROGRAMMING
///            }ESMBootModeInfo;
///            @endcode
/// \~english @retval SS_String
/// \~english @par Preconditions
///       - None.
/// \~english @par Change of the internal state
///       - The internal state is not changed.
/// \~english @par Causes of failures
///       None
/// \~english @par Classification
///       Public
/// \~english @par Type
///          sync only
/// \~english @par Detail
///       change member from ESMBootModeInfo type to string. \n
/// \~english @see
////////////////////////////////////////////////////////////////////////////////////
extern SS_String GetStr(ESMBootModeInfo f_enum);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup GetStr
/// \~english @par Summary
///       change member from ESMCpuResetReason type to string
/// \~english @param [in] f_enum
///         ESMCpuResetReason - the member what will convert farm ESMCpuResetReason type to string type.The type of CPU
///         reset reason.
/// \~english @codes
///            typedef enum {
///                e_SS_SM_CPU_RESET_REASON_INVALID = -1
///              , e_SS_SM_CPU_RESET_REASON_NORMAL  =  0
///              , e_SS_SM_CPU_RESET_REASON_DATA_RESET
///              , e_SS_SM_CPU_RESET_REASON_USER_FORCE_RESET
///              , e_SS_SM_CPU_RESET_REASON_GENERIC
///              , e_SS_SM_CPU_RESET_REASON_GENERIC_ERR = e_SS_SM_CPU_RESET_REASON_GENERIC
///              , e_SS_SM_CPU_RESET_REASON_DSP_ERR
///              , e_SS_SM_CPU_RESET_REASON_IMMRESET_NORMAL
///              , e_SS_SM_CPU_RESET_REASON_CRITICAL_ERR
///            }ESMCpuResetReason;
///            @endcode
/// \~english @retval SS_String
/// \~english @par Preconditions
///       - None.
/// \~english @par Change of the internal state
///       - The internal state is not changed.
/// \~english @par Causes of failures
///       None
/// \~english @par Classification
///       Public
/// \~english @par Type
///          sync only
/// \~english @par Detail
///       change member from ESMCpuResetReason type to string. \n
/// \~english @see
////////////////////////////////////////////////////////////////////////////////////
extern SS_String GetStr(ESMCpuResetReason f_enum);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup GetStr
/// \~english @par Summary
///       change member from \ref _SS_SystemManagerProtocol "SS_SystemManagerProtocol" type to string
/// \~english @param [in] f_enum
///       \ref _SS_SystemManagerProtocol "SS_SystemManagerProtocol" - the member what will convert farm \ref SS_SystemManagerProtocol "SS_SystemManagerProtocol" type to string
///       type.The type of system manager protocol.
/// \~english @retval SS_String
/// \~english @par Preconditions
///       - None.
/// \~english @par Change of the internal state
///       - The internal state is not changed.
/// \~english @par Causes of failures
///       None
/// \~english @par Classification
///       Public
/// \~english @par Type
///       sync only
/// \~english @par Detail
///       change member from \ref _SS_SystemManagerProtocol) "SS_SystemManagerProtocol" type to string. \n
/// \~english @see
////////////////////////////////////////////////////////////////////////////////////
extern SS_String GetStr(SS_SystemManagerProtocol f_enum);

#endif /* SS_STRING_MAPS_H_ */  // NOLINT (build/header_guard)
// EOF ss_string_maps.h
/** @}*/
/** @}*/
/** @}*/
