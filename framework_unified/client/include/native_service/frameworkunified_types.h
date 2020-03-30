/*
 *
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

/**
 * @file frameworkunified_types.h
 * @brief \~english This file contains basic define,enum and struct of framework_unified.
 *
 */
/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup native_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup framework_unified
 *  @ingroup native_service
 *  @{
 */
/** @addtogroup framework
 *  @ingroup framework_unified
 *  @{
 */
#ifndef FRAMEWORKUNIFIED_TYPES_H_   // NOLINT  (build/header_guard)
#define FRAMEWORKUNIFIED_TYPES_H_

#include <stdint.h>

#define DEFINE_HANDLE_TYPE(struct_type, handle_type) typedef struct struct_type* handle_type;

#define MIN_VAL(a, b) ((a) < (b)? (a) : (b))

#ifndef NULL
#define NULL    0
#endif
#ifndef VOID
typedef void VOID;
#endif
#ifndef CONST
#define CONST   const
#endif

/// Lengths and Max Sizes
#define INVALID_HANDLE    NULL
#define MAX_QUEUE_MSG_SIZE  4096
// #define MAX_QUEUE_NAME_SIZE 24
#define MAX_QUEUE_NAME_SIZE 20
#define LIMIT_QUEUE_NAME_SIZE 16
#define MAX_SYS_INFO_SIZE 64

#define MAX_NAME_SIZE_APP             MAX_QUEUE_NAME_SIZE  ///< Maximum name size of the APP (Client/Service)
#define LIMIT_NAME_SIZE_APP           LIMIT_QUEUE_NAME_SIZE  ///< Limit name size of the APP (Client/Service)
#define MAX_NAME_SIZE_NOTIFICATION    24  ///< Maximum name size of the Notification
#define MAX_STRING_SIZE_NOTIFICATION  64

#define MAX_STRING_SIZE_TAG  64
#define MAX_PATH_LENGTH      255


#define NS_SHM_ERROR       -1

// Types
typedef int8_t        SI_8;
typedef uint8_t       UI_8;
typedef int16_t       SI_16;
typedef uint16_t      UI_16;
typedef int32_t       SI_32;
typedef uint32_t      UI_32;
typedef int64_t       SI_64;
typedef uint64_t      UI_64;
typedef float       F_32;
typedef double        F_64;
// use it defined kernelmoduleevklibdd.h typedef UI_32       BOOL;
#define PFALSE ((BOOL)0)
#define PTRUE  ((BOOL)1)

#ifndef FALSE
#define FALSE PFALSE
#endif
#ifndef TRUE
#define TRUE PTRUE
#endif

// Pointer Types
typedef BOOL                *PBOOL;
typedef SI_8        *PSI_8;
typedef UI_8              *PUI_8;
typedef SI_16             *PSI_16;
typedef UI_16             *PUI_16;
typedef UI_32             *PUI_32;
typedef SI_32             *PSI_32;
typedef UI_64             *PUI_64;
typedef SI_64             *PSI_64;
typedef F_32                *PF_32;
typedef void              *PVOID;
typedef CONST void        *PCVOID;

typedef char        CHAR, *PCHAR;
typedef CONST char      *PCSTR;
typedef char        *PSTR;

typedef UI_8          _TUCHAR;
typedef SI_32         HFILE;
typedef PVOID         HANDLE;
typedef HANDLE        *PHANDLE;
typedef CHAR        TSysMsgSystemInfo[MAX_SYS_INFO_SIZE];  ///< Information passed via the framework


/// CR carry over types...
/// \brief Types that have been carried
/// over from Color Radio header file WPFAPI_types.h.
typedef UI_16     MID;
typedef UI_16     FRAMEWORKUNIFIED_CID;
typedef UI_8      RID;
typedef UI_16     PNO;
typedef UI_8      BYTE;

#define LOWORD(l)           ((WORD)(l))
#define HIWORD(l)           ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#define _countof(array) (sizeof(array)/sizeof(array[0]))

/// \typedef EFrameworkunifiedStatus
/// \brief Status and return types
typedef enum eFrameworkunifiedStatus {
  eFrameworkunifiedStatusEmptyMediaList    = -10,  ///< Empty media list
  eFrameworkunifiedStatusSessionLimitMaxedOut  = -9,   ///< Maximum session limit reached
  eFrameworkunifiedStatusDbRecNotFound       = -8,  ///< Database record not found
  eFrameworkunifiedStatusDbResultError       = -7,  ///< Database result error
  eFrameworkunifiedStatusDbExecuteFail       = -6,  ///< Database execute fail
  eFrameworkunifiedStatusSemCloseFail        = -5,  ///< Semaphore close failed
  eFrameworkunifiedStatusSemUnlockFail       = -4,  ///< Semaphore unlock failed
  eFrameworkunifiedStatusSemLockFail         = -3,  ///< Semaphore lock failed
  eFrameworkunifiedStatusFail                = -2,  ///< Failed
  eFrameworkunifiedStatusErrOther            = -1,  ///< Unknown error
  eFrameworkunifiedStatusOK                  = 0,   ///< Success / Pass / OK
  eFrameworkunifiedStatusInvldBuf            = 1,   ///< Invalid buffer
  eFrameworkunifiedStatusInvldHandle         = 2,   ///< Invalid handle
  eFrameworkunifiedStatusInvldHndlType       = 3,   ///< Invalid handle type
  eFrameworkunifiedStatusInvldQName        = 4,   ///< Invalid messasge queue name
  eFrameworkunifiedStatusMsgQFull            = 5,   ///< Message queue full
  eFrameworkunifiedStatusInvldNotification   = 6,  ///< The Notification event not present
  eFrameworkunifiedStatusInvldParam        = 7,   ///< Invalid parameter
  eFrameworkunifiedStatusInvldBufSize        = 8,   ///< Buf size too small
  eFrameworkunifiedStatusInvldID             = 9,   ///< Unrecognized ID
  eFrameworkunifiedStatusCannotRelease       = 10,  ///< Cannot release resource
  eFrameworkunifiedStatusBadConnection       = 11,  ///< Could not locate resource
  eFrameworkunifiedStatusExit            = 12,   ///< Normal application termination
  eFrameworkunifiedStatusNotImplemented      = 13,   ///< incomplete feature
  eFrameworkunifiedStatusThreadBusy          = 14,   ///< Joined thread is already being joined
  eFrameworkunifiedStatusThreadSelfJoin      = 15,   ///< Thread is joining itself
  eFrameworkunifiedStatusThreadInvalidVal    = 16,  ///< Invalid value passed
  eFrameworkunifiedStatusThreadNotExist      = 17,   ///< The thread does not exist
  eFrameworkunifiedStatusFault               = 18,   ///< A fault occurred while attempting to make call
  eFrameworkunifiedStatusServNotFound        = 19,   ///< Service not present in serv dir
  eFrameworkunifiedStatusServerInUse         = 20,   ///< Service already processing 1 client request
  eFrameworkunifiedStatusDbIndexing        = 21,   ///< Database Indexing in progress
  eFrameworkunifiedStatusNullPointer       = 22,
  eFrameworkunifiedStatusMsgNotProcessed     = 23,
  eFrameworkunifiedStatusFileLoadSuccess     = 24,  ///< File Load Success
  eFrameworkunifiedStatusFileLoadError       = 25,  ///< File Load Error
  eFrameworkunifiedStatusAccessError         = 26,  ///< Error when accessing resource
  eFrameworkunifiedStatusDuplicate           = 27,  ///< Duplicate entry
  eFrameworkunifiedStatusMsgQEmpty             = 28,  ///< Message queue empty
  eFrameworkunifiedStatusThreadAlreadyRunning  = 29,
  eFrameworkunifiedStatusErrNoEBADF            = 30,   ///< Bad file descriptor
  eFrameworkunifiedStatusErrNoEAGAIN           = 31,   ///< Resource unavailable, try again
  eFrameworkunifiedStatusErrNoEINTR            = 32,   ///< Interrupted system call
  eFrameworkunifiedStatusSessionErr      = 33,   ///< Error in session handling
  eFrameworkunifiedStatusDBCorrupt       = 34,   ///< Database corrupt
  eFrameworkunifiedStatusDBFileNotFound    = 35   ///< Database file not found
} EFrameworkunifiedStatus, *PEFrameworkunifiedStatus;

/// \typedef EFrameworkunifiedNotificationType
/// \brief Notification types
typedef enum _EFrameworkunifiedNotificationType {
  eFrameworkunifiedNotificationVar = 0,   ///< Non Persistent. Not stored locally by NPS
  eFrameworkunifiedStateVar,   ///< Non Persistent. stored locally by NPS
  eFrameworkunifiedPersistedStateVar,   ///< Persistent. Also stored locally by NPS
  eFrameworkunifiedPersistedStateUserVar,   ///< User specific Persistent. Also stored locally by NPS
  eFrameworkunifiedImmediatePersistedStateVar,     ///< Not to be used. See \ref FrameworkunifiedNPRegisterImmediatePersistNotification
  eFrameworkunifiedUnknown     ///< This is not a type of notification and should not be used while
  ///< defining any notification.
} EFrameworkunifiedNotificationType;

/// \typedef ENS_ApplicationState
/// \brief Application States that all module must handle
/// this should be handle by Publishing a "ModuleName/State"
/// with the parameter being one of these state values.
typedef enum _EApplicationStates {
  eAppInital = 0,    ///< Initial: state value
  eAppIdle,        ///< Idle: application shouldn't be processing anything
  eAppReady,       ///< Ready: application is ready to do work
  eAppConnecting,    ///< Connecting: Service is connecting
  eAppDisconnecting  ///< Disconnecting: Service is disconnecting
} EApplicationStates;


typedef enum _EFrameworkunifiedMessagePriorties {
  eFrameworkunifiedMsgPrioVeryLow = 1,
  eFrameworkunifiedMsgPrioLow = 8,
  eFrameworkunifiedMsgPrioNormal = 16,
  eFrameworkunifiedMsgPrioEmergency = 30,
} EFrameworkunifiedMessagePriorties;

// defines what data to delete from persistent memory
typedef enum _EFrameworkunifiedClearPersistence {
  eFrameworkunifiedClearAllData = 0,          ///< clears all the data in persistence memory for all users
  ///< (currently only this enum value is supported.)
  eFrameworkunifiedClearAllApplicationData,       ///< clears all the data(files, folders) related to all application for all users
  eFrameworkunifiedClearAllNotificationData,      ///< clears all the notification data related to all application for all users
  eFrameworkunifiedClearCurrentUserData,        ///< clears all the data in persistence memory for current users
  eFrameworkunifiedClearCurrentUserApplicationData,  ///< clears all the data(files, folders)
                                        ///< related to all application for current users
  eFrameworkunifiedClearCurrentUserNotificationData   ///< clears all the notification data
                                        ///< related to all application for current users
} EFrameworkunifiedClearPersistence;

/// Enum defining category of persistent data
typedef enum _EFrameworkunifiedPersistCategory {
  eFrameworkunifiedUserData        = 0x0001,
  eFrameworkunifiedFactoryData       = 0x0010,
  eFrameworkunifiedFactoryCustomerData   = 0x0100,
  eFrameworkunifiedDealerData        = 0x1000
} EFrameworkunifiedPersistCategory;

// defines the type of system error
typedef enum _EFrameworkunifiedSystemError {
  // TODO(framework_unified): add generic system errors
  eFrameworkunifiedSystemErrorNone = 0,
  eFrameworkunifiedDSPHardwareReset = 1  // system error is DSP hardware reset.
                         // It was commented, because this is product
                         // specific error. Must handle by SystemServices only.
                         // Uncommented per system services request.
} EFrameworkunifiedSystemError;

/// defines the shutdown types
typedef enum _EFrameworkunifiedShutdownType {
  eFrameworkunifiedNormalShutdown = 0,
  eFrameworkunifiedQuickShutdown,
  eFrameworkunifiedDataResetShutdown
} EFrameworkunifiedShutdownType;

/// defines the Release types
typedef enum _EFrameworkunifiedReleaseType {
  eFrameworkunifiedNotOnRelease = 0,    ///< 0:not on release
  eFrameworkunifiedPersistOnShutdown,     ///< 1:persist on shutdown
  eFrameworkunifiedPersistInstantly,    ///< 2:persist instantly
} EFrameworkunifiedReleaseType;

#endif  // FRAMEWORKUNIFIED_TYPES_H_   // NOLINT  (build/header_guard))
/** @}*/
/** @}*/
/** @}*/
/** @}*/
