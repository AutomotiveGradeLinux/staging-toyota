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
 * @file ns_np_service_if.h
 * @brief \~english Declares the external APIs to Notification and Persistence Service.
 */

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup native_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup notification_persistent_service
 *  @ingroup native_service
 *  @{
 */

#ifndef __NATIVESERVICES_NP_SERVICE_IF_H__  // NOLINT  (build/header_guard)
#define __NATIVESERVICES_NP_SERVICE_IF_H__

///////////////////////////////////////////////////////////////////////////////
// Include Files
///////////////////////////////////////////////////////////////////////////////
#include <native_service/frameworkunified_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \~english  A structure to Notification Info
 */
typedef struct _NotificationInfo {
  CHAR  notificationName[MAX_STRING_SIZE_NOTIFICATION];  ///< Notification Event Name string
  UI_32 maxLength;                       ///< Maximum possible length (bytes) of the message data
  EFrameworkunifiedNotificationType persType;        ///< Specifies persitence for this notification
  UI_8  dummy[4];                        ///< Packing to 32 bit boundary
} NotificationInfo;

/**
 * \~english  A structure to Immediate Notification Info
 */
typedef struct _ImmediateNotificationInfo {
  CHAR  notificationName[MAX_STRING_SIZE_NOTIFICATION];  ///< Notification Event Name string
  UI_32 maxLength;                       ///< Maximum possible length (bytes) of the message data
  EFrameworkunifiedNotificationType persType;        ///< Specifies persitence for this notification
  UI_32 delay;
  UI_8  dummy[4];                        ///< Packing to 32 bit boundary
} ImmediateNotificationInfo;

/**
 * \~english  A structure to Subscribe Info
 */
typedef struct _SubscribeInfo {
  CHAR  notificationName[MAX_STRING_SIZE_NOTIFICATION];  ///< Notification Event Name string
} SubscribeInfo;



//////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NPRegisterNotifications
/// \~english @par Brief
///        API to send message to Notification Service to register a set of notification
/// \~english @param[in] hNPMsgQ
///        HANDLE - Handle to message queue of Notification service.
/// \~english @param[in] pPublisherName
///        PCSTR - Name of Publisher message queue
/// \~english @param[in] numNotifications
///        PCSTR - Name of Notification
/// \~english @param[in] pNotificationArray
///        NotificationInfo - Array of notifications
/// \~english @retval EFrameworkunifiedStatus indicates success or error
/// \~english @retval Success(eFrameworkunifiedStatusOK)
/// \~english @retval Error(anything else)
/// \~english @par
///        EFrameworkunifiedStatus Enum
/// \~english @code
///           typedef enum eFrameworkunifiedStatus {
///             eFrameworkunifiedStatusEmptyMediaList    = -10, ///< Empty media list
///             eFrameworkunifiedStatusSessionLimitMaxedOut  = -9,  ///< Maximum session limit reached
///             eFrameworkunifiedStatusDbRecNotFound       = -8, ///< Database record not found
///             eFrameworkunifiedStatusDbResultError       = -7, ///< Database result error
///             eFrameworkunifiedStatusDbExecuteFail       = -6, ///< Database execute fail
///             eFrameworkunifiedStatusSemCloseFail        = -5, ///< Semaphore close failed
///             eFrameworkunifiedStatusSemUnlockFail       = -4, ///< Semaphore unlock failed
///             eFrameworkunifiedStatusSemLockFail         = -3, ///< Semaphore lock failed
///             eFrameworkunifiedStatusFail                = -2, ///< Failed
///             eFrameworkunifiedStatusErrOther            = -1, ///< Unknown error
///             eFrameworkunifiedStatusOK                  = 0,  ///< Success / Pass / OK
///             eFrameworkunifiedStatusInvldBuf            = 1,  ///< Invalid buffer
///             eFrameworkunifiedStatusInvldHandle         = 2,  ///< Invalid handle
///             eFrameworkunifiedStatusInvldHndlType       = 3,  ///< Invalid handle type
///             eFrameworkunifiedStatusInvldQName        = 4,  ///< Invalid messasge queue name
///             eFrameworkunifiedStatusMsgQFull            = 5,  ///< Message queue full
///             eFrameworkunifiedStatusInvldNotification   = 6,///< The Notification event not present
///             eFrameworkunifiedStatusInvldParam        = 7,  ///< Invalid parameter
///             eFrameworkunifiedStatusInvldBufSize        = 8,  ///< Buf size too small
///             eFrameworkunifiedStatusInvldID             = 9,  ///< Unrecognized ID
///             eFrameworkunifiedStatusCannotRelease       = 10, ///< Cannot release resource
///             eFrameworkunifiedStatusBadConnection       = 11, ///< Could not locate resource
///             eFrameworkunifiedStatusExit            = 12,  ///< Normal application termination
///             eFrameworkunifiedStatusNotImplemented      = 13,  ///< incomplete feature
///             eFrameworkunifiedStatusThreadBusy          = 14,  ///< Joined thread is already being joined
///             eFrameworkunifiedStatusThreadSelfJoin      = 15,  ///< Thread is joining itself
///             eFrameworkunifiedStatusThreadInvalidVal    = 16, ///< Invalid value passed
///             eFrameworkunifiedStatusThreadNotExist      = 17,  ///< The thread does not exist
///             eFrameworkunifiedStatusFault               = 18,  ///< A fault occurred while attempting to make call
///             eFrameworkunifiedStatusServNotFound        = 19,  ///< Service not present in serv dir
///             eFrameworkunifiedStatusServerInUse         = 20,  ///< Service already processing 1 client request
///             eFrameworkunifiedStatusDbIndexing        = 21,  ///< Database Indexing in progress
///             eFrameworkunifiedStatusNullPointer       = 22,
///             eFrameworkunifiedStatusMsgNotProcessed     = 23,
///             eFrameworkunifiedStatusFileLoadSuccess     = 24, ///< File Load Success
///             eFrameworkunifiedStatusFileLoadError       = 25, ///< File Load Error
///             eFrameworkunifiedStatusAccessError         = 26, ///< Error when accessing resource
///             eFrameworkunifiedStatusDuplicate           = 27, ///< Duplicate entry
///             eFrameworkunifiedStatusMsgQEmpty             = 28, ///< Message queue empty
///             eFrameworkunifiedStatusThreadAlreadyRunning  = 29,
///             eFrameworkunifiedStatusErrNoEBADF            = 30,  ///< Bad file descriptor
///             eFrameworkunifiedStatusErrNoEAGAIN           = 31,  ///< Resource unavailable, try again
///             eFrameworkunifiedStatusErrNoEINTR            = 32,  ///< Interrupted system call
///             eFrameworkunifiedStatusSessionErr      = 33,  ///< Error in session handling
///             eFrameworkunifiedStatusDBCorrupt       = 34,  ///< Database corrupt
///             eFrameworkunifiedStatusDBFileNotFound    = 35  ///< Database file not found
///           } EFrameworkunifiedStatus, *PEFrameworkunifiedStatus;
///            @endcode
/// \~english @par Preconditons
///       none
/// \~english @par Change of internal status
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - Function pointer specified by the argument is NULL.
///       - It is an incorrect value(hNPMsgQ) or hNPMsgQ is NULL.
///       - It is an incorrect value(pPublisherName) or pPublisherName is NULL.
///       - It is an incorrect value(numNotifications).
///       - It is an incorrect value(pNotificationArray) or pNotificationArray is NULL.
/// \~english @par Detail
///       Invoke callback function with arguments to register a set of notification.\n
///       If callback function is null and expectedCalls is '0', using default function 'NPRegisterNotifications' to
///       register a set of notification.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Sync only
/// \~english @see
///        FrameworkunifiedNPRegisterNotification, NPRegisterNotification
//////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPRegisterNotifications(HANDLE hNPMsgQ, PCSTR pPublisherName, UI_32 numNotifications,
                                   NotificationInfo *pNotificationArray);


//////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NPRegisterImmediateNotifications
/// \~english @par Brief
///        API to send message to Notification Service to register a set of immediate notifications
/// \~english @param[in] hNPMsgQ
///        HANDLE - Handle to message queue of Notification service.
/// \~english @param[in] pPublisherName
///        PCSTR - Name of Publisher message queue.
/// \~english @param[in] numNotifications
///        PCSTR - Name of Notification.
/// \~english @param[in] pNotificationArray
///        ImmediateNotificationInfo - Array of immediate notifications
/// \~english @retval EFrameworkunifiedStatus indicates success or error
/// \~english @retval Success(eFrameworkunifiedStatusOK)
/// \~english @retval Error(anything else)
/// \~english @par
///        EFrameworkunifiedStatus Enum
/// \~english @code
///           typedef enum eFrameworkunifiedStatus {
///             eFrameworkunifiedStatusEmptyMediaList    = -10, ///< Empty media list
///             eFrameworkunifiedStatusSessionLimitMaxedOut  = -9,  ///< Maximum session limit reached
///             eFrameworkunifiedStatusDbRecNotFound       = -8, ///< Database record not found
///             eFrameworkunifiedStatusDbResultError       = -7, ///< Database result error
///             eFrameworkunifiedStatusDbExecuteFail       = -6, ///< Database execute fail
///             eFrameworkunifiedStatusSemCloseFail        = -5, ///< Semaphore close failed
///             eFrameworkunifiedStatusSemUnlockFail       = -4, ///< Semaphore unlock failed
///             eFrameworkunifiedStatusSemLockFail         = -3, ///< Semaphore lock failed
///             eFrameworkunifiedStatusFail                = -2, ///< Failed
///             eFrameworkunifiedStatusErrOther            = -1, ///< Unknown error
///             eFrameworkunifiedStatusOK                  = 0,  ///< Success / Pass / OK
///             eFrameworkunifiedStatusInvldBuf            = 1,  ///< Invalid buffer
///             eFrameworkunifiedStatusInvldHandle         = 2,  ///< Invalid handle
///             eFrameworkunifiedStatusInvldHndlType       = 3,  ///< Invalid handle type
///             eFrameworkunifiedStatusInvldQName        = 4,  ///< Invalid messasge queue name
///             eFrameworkunifiedStatusMsgQFull            = 5,  ///< Message queue full
///             eFrameworkunifiedStatusInvldNotification   = 6,///< The Notification event not present
///             eFrameworkunifiedStatusInvldParam        = 7,  ///< Invalid parameter
///             eFrameworkunifiedStatusInvldBufSize        = 8,  ///< Buf size too small
///             eFrameworkunifiedStatusInvldID             = 9,  ///< Unrecognized ID
///             eFrameworkunifiedStatusCannotRelease       = 10, ///< Cannot release resource
///             eFrameworkunifiedStatusBadConnection       = 11, ///< Could not locate resource
///             eFrameworkunifiedStatusExit            = 12,  ///< Normal application termination
///             eFrameworkunifiedStatusNotImplemented      = 13,  ///< incomplete feature
///             eFrameworkunifiedStatusThreadBusy          = 14,  ///< Joined thread is already being joined
///             eFrameworkunifiedStatusThreadSelfJoin      = 15,  ///< Thread is joining itself
///             eFrameworkunifiedStatusThreadInvalidVal    = 16, ///< Invalid value passed
///             eFrameworkunifiedStatusThreadNotExist      = 17,  ///< The thread does not exist
///             eFrameworkunifiedStatusFault               = 18,  ///< A fault occurred while attempting to make call
///             eFrameworkunifiedStatusServNotFound        = 19,  ///< Service not present in serv dir
///             eFrameworkunifiedStatusServerInUse         = 20,  ///< Service already processing 1 client request
///             eFrameworkunifiedStatusDbIndexing        = 21,  ///< Database Indexing in progress
///             eFrameworkunifiedStatusNullPointer       = 22,
///             eFrameworkunifiedStatusMsgNotProcessed     = 23,
///             eFrameworkunifiedStatusFileLoadSuccess     = 24, ///< File Load Success
///             eFrameworkunifiedStatusFileLoadError       = 25, ///< File Load Error
///             eFrameworkunifiedStatusAccessError         = 26, ///< Error when accessing resource
///             eFrameworkunifiedStatusDuplicate           = 27, ///< Duplicate entry
///             eFrameworkunifiedStatusMsgQEmpty             = 28, ///< Message queue empty
///             eFrameworkunifiedStatusThreadAlreadyRunning  = 29,
///             eFrameworkunifiedStatusErrNoEBADF            = 30,  ///< Bad file descriptor
///             eFrameworkunifiedStatusErrNoEAGAIN           = 31,  ///< Resource unavailable, try again
///             eFrameworkunifiedStatusErrNoEINTR            = 32,  ///< Interrupted system call
///             eFrameworkunifiedStatusSessionErr      = 33,  ///< Error in session handling
///             eFrameworkunifiedStatusDBCorrupt       = 34,  ///< Database corrupt
///             eFrameworkunifiedStatusDBFileNotFound    = 35  ///< Database file not found
///           } EFrameworkunifiedStatus, *PEFrameworkunifiedStatus;
///            @endcode
/// \~english @par Preconditons
///       none
/// \~english @par Change of internal status
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - Function pointer specified by the argument is NULL.
///       - It is an incorrect value(hNPMsgQ) or hNPMsgQ is NULL.
///       - It is an incorrect value(pPublisherName) or pPublisherName is NULL.
///       - It is an incorrect value(numNotifications).
///       - It is an incorrect value(pNotificationArray) or pNotificationArray is NULL.
/// \~english @par Detail
///       Invoke callback function with arguments to register a set of immediate notification.\n
///       If callback function is null and expectedCalls is '0', using default function
///       'NPRegisterImmediateNotifications' to register a set\n
///       of immediate notification.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Sync only
/// \~english @see
///        FrameworkunifiedNPRegisterImmediatePersistNotification
//////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPRegisterImmediateNotifications(HANDLE hNPMsgQ, PCSTR pPublisherName, UI_32 numNotifications,
                                            ImmediateNotificationInfo *pNotificationArray);

//////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NPRegisterNotification
/// \~english @par Brief
///        API to send message to Notification Service to register a notification
/// \~english @param[in] hNPMsgQ
///        HANDLE - Handle to message queue of Notification service.
/// \~english @param[in] pPublisherName
///        PCSTR - Name of Publisher message queue
/// \~english @param[in] notif_name
///        PCSTR - Name of Notification
/// \~english @param[in] max_length
///        const UI_32 - Max size of the notification message
/// \~english @param[in] perstype
///        const EFrameworkunifiedPersistentVarType - Flag to indicate if it has to be persistent
/// \~english @retval EFrameworkunifiedStatus indicates success or error
/// \~english @retval Success(eFrameworkunifiedStatusOK)
/// \~english @retval Error(anything else)
/// \~english @par
///        EFrameworkunifiedStatus Enum
/// \~english @code
///           typedef enum eFrameworkunifiedStatus {
///             eFrameworkunifiedStatusEmptyMediaList    = -10, ///< Empty media list
///             eFrameworkunifiedStatusSessionLimitMaxedOut  = -9,  ///< Maximum session limit reached
///             eFrameworkunifiedStatusDbRecNotFound       = -8, ///< Database record not found
///             eFrameworkunifiedStatusDbResultError       = -7, ///< Database result error
///             eFrameworkunifiedStatusDbExecuteFail       = -6, ///< Database execute fail
///             eFrameworkunifiedStatusSemCloseFail        = -5, ///< Semaphore close failed
///             eFrameworkunifiedStatusSemUnlockFail       = -4, ///< Semaphore unlock failed
///             eFrameworkunifiedStatusSemLockFail         = -3, ///< Semaphore lock failed
///             eFrameworkunifiedStatusFail                = -2, ///< Failed
///             eFrameworkunifiedStatusErrOther            = -1, ///< Unknown error
///             eFrameworkunifiedStatusOK                  = 0,  ///< Success / Pass / OK
///             eFrameworkunifiedStatusInvldBuf            = 1,  ///< Invalid buffer
///             eFrameworkunifiedStatusInvldHandle         = 2,  ///< Invalid handle
///             eFrameworkunifiedStatusInvldHndlType       = 3,  ///< Invalid handle type
///             eFrameworkunifiedStatusInvldQName        = 4,  ///< Invalid messasge queue name
///             eFrameworkunifiedStatusMsgQFull            = 5,  ///< Message queue full
///             eFrameworkunifiedStatusInvldNotification   = 6,///< The Notification event not present
///             eFrameworkunifiedStatusInvldParam        = 7,  ///< Invalid parameter
///             eFrameworkunifiedStatusInvldBufSize        = 8,  ///< Buf size too small
///             eFrameworkunifiedStatusInvldID             = 9,  ///< Unrecognized ID
///             eFrameworkunifiedStatusCannotRelease       = 10, ///< Cannot release resource
///             eFrameworkunifiedStatusBadConnection       = 11, ///< Could not locate resource
///             eFrameworkunifiedStatusExit            = 12,  ///< Normal application termination
///             eFrameworkunifiedStatusNotImplemented      = 13,  ///< incomplete feature
///             eFrameworkunifiedStatusThreadBusy          = 14,  ///< Joined thread is already being joined
///             eFrameworkunifiedStatusThreadSelfJoin      = 15,  ///< Thread is joining itself
///             eFrameworkunifiedStatusThreadInvalidVal    = 16, ///< Invalid value passed
///             eFrameworkunifiedStatusThreadNotExist      = 17,  ///< The thread does not exist
///             eFrameworkunifiedStatusFault               = 18,  ///< A fault occurred while attempting to make call
///             eFrameworkunifiedStatusServNotFound        = 19,  ///< Service not present in serv dir
///             eFrameworkunifiedStatusServerInUse         = 20,  ///< Service already processing 1 client request
///             eFrameworkunifiedStatusDbIndexing        = 21,  ///< Database Indexing in progress
///             eFrameworkunifiedStatusNullPointer       = 22,
///             eFrameworkunifiedStatusMsgNotProcessed     = 23,
///             eFrameworkunifiedStatusFileLoadSuccess     = 24, ///< File Load Success
///             eFrameworkunifiedStatusFileLoadError       = 25, ///< File Load Error
///             eFrameworkunifiedStatusAccessError         = 26, ///< Error when accessing resource
///             eFrameworkunifiedStatusDuplicate           = 27, ///< Duplicate entry
///             eFrameworkunifiedStatusMsgQEmpty             = 28, ///< Message queue empty
///             eFrameworkunifiedStatusThreadAlreadyRunning  = 29,
///             eFrameworkunifiedStatusErrNoEBADF            = 30,  ///< Bad file descriptor
///             eFrameworkunifiedStatusErrNoEAGAIN           = 31,  ///< Resource unavailable, try again
///             eFrameworkunifiedStatusErrNoEINTR            = 32,  ///< Interrupted system call
///             eFrameworkunifiedStatusSessionErr      = 33,  ///< Error in session handling
///             eFrameworkunifiedStatusDBCorrupt       = 34,  ///< Database corrupt
///             eFrameworkunifiedStatusDBFileNotFound    = 35  ///< Database file not found
///           } EFrameworkunifiedStatus, *PEFrameworkunifiedStatus;
///            @endcode
/// \~english @par Preconditons
///       none
/// \~english @par Change of internal status
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - Function pointer specified by the argument is NULL.
///       - It is an incorrect value(hNPMsgQ) or hNPMsgQ is NULL.
///       - It is an incorrect value(pPublisherName) or pPublisherName is NULL.
///       - It is an incorrect value(notif_name) or notif_name is NULL.
///       - It is an incorrect value(max_length).
/// \~english @par Detail
///       Invoke callback function with arguments to register a notification.\n
///       If callback function is null and expectedCalls is '0', using default function 'NPRegisterNotification' to
///       register a notification.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Sync only
/// \~english @see
///        NPRegisterNotifications, FrameworkunifiedNPRegisterNotification
//////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPRegisterNotification(HANDLE hNPMsgQ, PCSTR pPublisherName, PCSTR notif_name,
                                  const UI_32 max_length, const EFrameworkunifiedNotificationType perstype);


//////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NPUnRegisterNotification
/// \~english @par Brief
///        API to send message to Notification Service to remove a notification
/// \~english @param[in] hNPMsgQ
///        HANDLE - Handle to message queue of Notification service.
/// \~english @param[in] pPublisherName
///        PCSTR - Name of Publisher message queue
/// \~english @param[in] pNotification
///        PCSTR - Name of Notification
/// \~english @retval EFrameworkunifiedStatus indicates success or error
/// \~english @retval Success(eFrameworkunifiedStatusOK)
/// \~english @retval Error(anything else)
/// \~english @par
///        EFrameworkunifiedStatus Enum
/// \~english @code
///           typedef enum eFrameworkunifiedStatus {
///             eFrameworkunifiedStatusEmptyMediaList    = -10, ///< Empty media list
///             eFrameworkunifiedStatusSessionLimitMaxedOut  = -9,  ///< Maximum session limit reached
///             eFrameworkunifiedStatusDbRecNotFound       = -8, ///< Database record not found
///             eFrameworkunifiedStatusDbResultError       = -7, ///< Database result error
///             eFrameworkunifiedStatusDbExecuteFail       = -6, ///< Database execute fail
///             eFrameworkunifiedStatusSemCloseFail        = -5, ///< Semaphore close failed
///             eFrameworkunifiedStatusSemUnlockFail       = -4, ///< Semaphore unlock failed
///             eFrameworkunifiedStatusSemLockFail         = -3, ///< Semaphore lock failed
///             eFrameworkunifiedStatusFail                = -2, ///< Failed
///             eFrameworkunifiedStatusErrOther            = -1, ///< Unknown error
///             eFrameworkunifiedStatusOK                  = 0,  ///< Success / Pass / OK
///             eFrameworkunifiedStatusInvldBuf            = 1,  ///< Invalid buffer
///             eFrameworkunifiedStatusInvldHandle         = 2,  ///< Invalid handle
///             eFrameworkunifiedStatusInvldHndlType       = 3,  ///< Invalid handle type
///             eFrameworkunifiedStatusInvldQName        = 4,  ///< Invalid messasge queue name
///             eFrameworkunifiedStatusMsgQFull            = 5,  ///< Message queue full
///             eFrameworkunifiedStatusInvldNotification   = 6,///< The Notification event not present
///             eFrameworkunifiedStatusInvldParam        = 7,  ///< Invalid parameter
///             eFrameworkunifiedStatusInvldBufSize        = 8,  ///< Buf size too small
///             eFrameworkunifiedStatusInvldID             = 9,  ///< Unrecognized ID
///             eFrameworkunifiedStatusCannotRelease       = 10, ///< Cannot release resource
///             eFrameworkunifiedStatusBadConnection       = 11, ///< Could not locate resource
///             eFrameworkunifiedStatusExit            = 12,  ///< Normal application termination
///             eFrameworkunifiedStatusNotImplemented      = 13,  ///< incomplete feature
///             eFrameworkunifiedStatusThreadBusy          = 14,  ///< Joined thread is already being joined
///             eFrameworkunifiedStatusThreadSelfJoin      = 15,  ///< Thread is joining itself
///             eFrameworkunifiedStatusThreadInvalidVal    = 16, ///< Invalid value passed
///             eFrameworkunifiedStatusThreadNotExist      = 17,  ///< The thread does not exist
///             eFrameworkunifiedStatusFault               = 18,  ///< A fault occurred while attempting to make call
///             eFrameworkunifiedStatusServNotFound        = 19,  ///< Service not present in serv dir
///             eFrameworkunifiedStatusServerInUse         = 20,  ///< Service already processing 1 client request
///             eFrameworkunifiedStatusDbIndexing        = 21,  ///< Database Indexing in progress
///             eFrameworkunifiedStatusNullPointer       = 22,
///             eFrameworkunifiedStatusMsgNotProcessed     = 23,
///             eFrameworkunifiedStatusFileLoadSuccess     = 24, ///< File Load Success
///             eFrameworkunifiedStatusFileLoadError       = 25, ///< File Load Error
///             eFrameworkunifiedStatusAccessError         = 26, ///< Error when accessing resource
///             eFrameworkunifiedStatusDuplicate           = 27, ///< Duplicate entry
///             eFrameworkunifiedStatusMsgQEmpty             = 28, ///< Message queue empty
///             eFrameworkunifiedStatusThreadAlreadyRunning  = 29,
///             eFrameworkunifiedStatusErrNoEBADF            = 30,  ///< Bad file descriptor
///             eFrameworkunifiedStatusErrNoEAGAIN           = 31,  ///< Resource unavailable, try again
///             eFrameworkunifiedStatusErrNoEINTR            = 32,  ///< Interrupted system call
///             eFrameworkunifiedStatusSessionErr      = 33,  ///< Error in session handling
///             eFrameworkunifiedStatusDBCorrupt       = 34,  ///< Database corrupt
///             eFrameworkunifiedStatusDBFileNotFound    = 35  ///< Database file not found
///           } EFrameworkunifiedStatus, *PEFrameworkunifiedStatus;
///            @endcode
/// \~english @par Preconditons
///       none
/// \~english @par Change of internal status
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - Function pointer specified by the argument is NULL.
///       - It is an incorrect value(hNPMsgQ) or hNPMsgQ is NULL.
///       - It is an incorrect value(pPublisherName) or pPublisherName is NULL.
///       - It is an incorrect value(pNotification) or pNotification is NULL.
/// \~english @par Detail
///       Invoke NPUnRegisterNotifications to remove one notification named 'pPublisherName'.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Sync only
/// \~english @see
///        FrameworkunifiedNPUnRegisterNotification, NPUnRegisterNotifications
//////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPUnRegisterNotification(HANDLE hNPMsgQ, PCSTR pPublisherName, PCSTR pNotification);

//////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NPSetPersistentNotfnType
/// \~english @par Brief
///        API to send message to Notification Service to set the persist category of notification.
/// \~english @param[in] hNPMsgQ
///        HANDLE - Handle to message queue of Notification service.
/// \~english @param[in] pPublisherName
///        PCSTR - Service Name
/// \~english @param[in] pNotification
///        PCSTR - Name of Notification
/// \~english @param[in] ePersistCategory
///        EFrameworkunifiedPersistCategory - Persistent category
/// \~english @retval EFrameworkunifiedStatus indicates success or error
/// \~english @retval Success(eFrameworkunifiedStatusOK)
/// \~english @retval Error(anything else)
/// \~english @par
///        EFrameworkunifiedStatus Enum
/// \~english @code
///           typedef enum eFrameworkunifiedStatus {
///             eFrameworkunifiedStatusEmptyMediaList    = -10, ///< Empty media list
///             eFrameworkunifiedStatusSessionLimitMaxedOut  = -9,  ///< Maximum session limit reached
///             eFrameworkunifiedStatusDbRecNotFound       = -8, ///< Database record not found
///             eFrameworkunifiedStatusDbResultError       = -7, ///< Database result error
///             eFrameworkunifiedStatusDbExecuteFail       = -6, ///< Database execute fail
///             eFrameworkunifiedStatusSemCloseFail        = -5, ///< Semaphore close failed
///             eFrameworkunifiedStatusSemUnlockFail       = -4, ///< Semaphore unlock failed
///             eFrameworkunifiedStatusSemLockFail         = -3, ///< Semaphore lock failed
///             eFrameworkunifiedStatusFail                = -2, ///< Failed
///             eFrameworkunifiedStatusErrOther            = -1, ///< Unknown error
///             eFrameworkunifiedStatusOK                  = 0,  ///< Success / Pass / OK
///             eFrameworkunifiedStatusInvldBuf            = 1,  ///< Invalid buffer
///             eFrameworkunifiedStatusInvldHandle         = 2,  ///< Invalid handle
///             eFrameworkunifiedStatusInvldHndlType       = 3,  ///< Invalid handle type
///             eFrameworkunifiedStatusInvldQName        = 4,  ///< Invalid messasge queue name
///             eFrameworkunifiedStatusMsgQFull            = 5,  ///< Message queue full
///             eFrameworkunifiedStatusInvldNotification   = 6,///< The Notification event not present
///             eFrameworkunifiedStatusInvldParam        = 7,  ///< Invalid parameter
///             eFrameworkunifiedStatusInvldBufSize        = 8,  ///< Buf size too small
///             eFrameworkunifiedStatusInvldID             = 9,  ///< Unrecognized ID
///             eFrameworkunifiedStatusCannotRelease       = 10, ///< Cannot release resource
///             eFrameworkunifiedStatusBadConnection       = 11, ///< Could not locate resource
///             eFrameworkunifiedStatusExit            = 12,  ///< Normal application termination
///             eFrameworkunifiedStatusNotImplemented      = 13,  ///< incomplete feature
///             eFrameworkunifiedStatusThreadBusy          = 14,  ///< Joined thread is already being joined
///             eFrameworkunifiedStatusThreadSelfJoin      = 15,  ///< Thread is joining itself
///             eFrameworkunifiedStatusThreadInvalidVal    = 16, ///< Invalid value passed
///             eFrameworkunifiedStatusThreadNotExist      = 17,  ///< The thread does not exist
///             eFrameworkunifiedStatusFault               = 18,  ///< A fault occurred while attempting to make call
///             eFrameworkunifiedStatusServNotFound        = 19,  ///< Service not present in serv dir
///             eFrameworkunifiedStatusServerInUse         = 20,  ///< Service already processing 1 client request
///             eFrameworkunifiedStatusDbIndexing        = 21,  ///< Database Indexing in progress
///             eFrameworkunifiedStatusNullPointer       = 22,
///             eFrameworkunifiedStatusMsgNotProcessed     = 23,
///             eFrameworkunifiedStatusFileLoadSuccess     = 24, ///< File Load Success
///             eFrameworkunifiedStatusFileLoadError       = 25, ///< File Load Error
///             eFrameworkunifiedStatusAccessError         = 26, ///< Error when accessing resource
///             eFrameworkunifiedStatusDuplicate           = 27, ///< Duplicate entry
///             eFrameworkunifiedStatusMsgQEmpty             = 28, ///< Message queue empty
///             eFrameworkunifiedStatusThreadAlreadyRunning  = 29,
///             eFrameworkunifiedStatusErrNoEBADF            = 30,  ///< Bad file descriptor
///             eFrameworkunifiedStatusErrNoEAGAIN           = 31,  ///< Resource unavailable, try again
///             eFrameworkunifiedStatusErrNoEINTR            = 32,  ///< Interrupted system call
///             eFrameworkunifiedStatusSessionErr      = 33,  ///< Error in session handling
///             eFrameworkunifiedStatusDBCorrupt       = 34,  ///< Database corrupt
///             eFrameworkunifiedStatusDBFileNotFound    = 35  ///< Database file not found
///           } EFrameworkunifiedStatus, *PEFrameworkunifiedStatus;
///            @endcode
/// \~english @par Preconditons
///       none
/// \~english @par Change of internal status
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - Function pointer specified by the argument is NULL.
///       - It is an incorrect value(hNPMsgQ) or hNPMsgQ is NULL.
///       - It is an incorrect value(pPublisherName) or pPublisherName is NULL.
///       - It is an incorrect value(pNotification) or pNotification is NULL.
/// \~english @par Detail
///       Invoke callback function to send message to Notification Service to set the persist category of
///       notification.\n
///       If callback function is null and expectedCalls is '0', using default function 'NPSetPersistentNotfnType' to
///       send\n
///       message to Notification Service to set the persist category of notification.
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync only
/// \~english @see
///       FrameworkunifiedNPSetPersistentNotfnType
//////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPSetPersistentNotfnType(HANDLE hNPMsgQ, PCSTR pPublisherName, PCSTR pNotification,
                                    EFrameworkunifiedPersistCategory ePersistCategory);

//////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NPSetPersistNotfnDefaultValue
/// \~english @par Brief
///        API to send message to Notification Service to set default value of persistent category in notification.
/// \~english @param[in] hNPMsgQ
///        HANDLE - Handle to message queue of Notification service.
/// \~english @param[in] pPublisherName
///        PCSTR - Service Name
/// \~english @param[in] pNotification
///        PCSTR - Name of Notification
/// \~english @param[in] pData
///        PVOID - Data buffer
/// \~english @param[in] iLength
///        const UI_32 - Size of data buffer
/// \~english @retval EFrameworkunifiedStatus indicates success or error
/// \~english @retval Success(eFrameworkunifiedStatusOK)
/// \~english @retval Error(anything else)
/// \~english @par
///        EFrameworkunifiedStatus Enum
/// \~english @code
///           typedef enum eFrameworkunifiedStatus {
///             eFrameworkunifiedStatusEmptyMediaList    = -10, ///< Empty media list
///             eFrameworkunifiedStatusSessionLimitMaxedOut  = -9,  ///< Maximum session limit reached
///             eFrameworkunifiedStatusDbRecNotFound       = -8, ///< Database record not found
///             eFrameworkunifiedStatusDbResultError       = -7, ///< Database result error
///             eFrameworkunifiedStatusDbExecuteFail       = -6, ///< Database execute fail
///             eFrameworkunifiedStatusSemCloseFail        = -5, ///< Semaphore close failed
///             eFrameworkunifiedStatusSemUnlockFail       = -4, ///< Semaphore unlock failed
///             eFrameworkunifiedStatusSemLockFail         = -3, ///< Semaphore lock failed
///             eFrameworkunifiedStatusFail                = -2, ///< Failed
///             eFrameworkunifiedStatusErrOther            = -1, ///< Unknown error
///             eFrameworkunifiedStatusOK                  = 0,  ///< Success / Pass / OK
///             eFrameworkunifiedStatusInvldBuf            = 1,  ///< Invalid buffer
///             eFrameworkunifiedStatusInvldHandle         = 2,  ///< Invalid handle
///             eFrameworkunifiedStatusInvldHndlType       = 3,  ///< Invalid handle type
///             eFrameworkunifiedStatusInvldQName        = 4,  ///< Invalid messasge queue name
///             eFrameworkunifiedStatusMsgQFull            = 5,  ///< Message queue full
///             eFrameworkunifiedStatusInvldNotification   = 6,///< The Notification event not present
///             eFrameworkunifiedStatusInvldParam        = 7,  ///< Invalid parameter
///             eFrameworkunifiedStatusInvldBufSize        = 8,  ///< Buf size too small
///             eFrameworkunifiedStatusInvldID             = 9,  ///< Unrecognized ID
///             eFrameworkunifiedStatusCannotRelease       = 10, ///< Cannot release resource
///             eFrameworkunifiedStatusBadConnection       = 11, ///< Could not locate resource
///             eFrameworkunifiedStatusExit            = 12,  ///< Normal application termination
///             eFrameworkunifiedStatusNotImplemented      = 13,  ///< incomplete feature
///             eFrameworkunifiedStatusThreadBusy          = 14,  ///< Joined thread is already being joined
///             eFrameworkunifiedStatusThreadSelfJoin      = 15,  ///< Thread is joining itself
///             eFrameworkunifiedStatusThreadInvalidVal    = 16, ///< Invalid value passed
///             eFrameworkunifiedStatusThreadNotExist      = 17,  ///< The thread does not exist
///             eFrameworkunifiedStatusFault               = 18,  ///< A fault occurred while attempting to make call
///             eFrameworkunifiedStatusServNotFound        = 19,  ///< Service not present in serv dir
///             eFrameworkunifiedStatusServerInUse         = 20,  ///< Service already processing 1 client request
///             eFrameworkunifiedStatusDbIndexing        = 21,  ///< Database Indexing in progress
///             eFrameworkunifiedStatusNullPointer       = 22,
///             eFrameworkunifiedStatusMsgNotProcessed     = 23,
///             eFrameworkunifiedStatusFileLoadSuccess     = 24, ///< File Load Success
///             eFrameworkunifiedStatusFileLoadError       = 25, ///< File Load Error
///             eFrameworkunifiedStatusAccessError         = 26, ///< Error when accessing resource
///             eFrameworkunifiedStatusDuplicate           = 27, ///< Duplicate entry
///             eFrameworkunifiedStatusMsgQEmpty             = 28, ///< Message queue empty
///             eFrameworkunifiedStatusThreadAlreadyRunning  = 29,
///             eFrameworkunifiedStatusErrNoEBADF            = 30,  ///< Bad file descriptor
///             eFrameworkunifiedStatusErrNoEAGAIN           = 31,  ///< Resource unavailable, try again
///             eFrameworkunifiedStatusErrNoEINTR            = 32,  ///< Interrupted system call
///             eFrameworkunifiedStatusSessionErr      = 33,  ///< Error in session handling
///             eFrameworkunifiedStatusDBCorrupt       = 34,  ///< Database corrupt
///             eFrameworkunifiedStatusDBFileNotFound    = 35  ///< Database file not found
///           } EFrameworkunifiedStatus, *PEFrameworkunifiedStatus;
///            @endcode
/// \~english @par Preconditons
///       none
/// \~english @par Change of internal status
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - Function pointer specified by the argument is NULL.
///       - It is an incorrect value(hNPMsgQ) or hNPMsgQ is NULL.
///       - It is an incorrect value(pPublisherName) or pPublisherName is NULL.
///       - It is an incorrect value(pNotification) or pNotification is NULL.
///       - It is an incorrect value(pData) or pData is NULL.
/// \~english @par Detail
///       Invoke callback function to send message to Notification Service to set default value of persistent category
///       in notification.\n
///       If callback function is null and expectedCalls is '0', using default function 'NPSetPersistNotfnDefaultValue'
///       to send message\n
///       to Notification Service to set default value of persistent category in notification.
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync only
/// \~english @see
///       FrameworkunifiedNPSetPersistNotfnDefaultValue
//////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPSetPersistNotfnDefaultValue(HANDLE hNPMsgQ, PCSTR pPublisherName, PCSTR pNotification,
                                         PCVOID pData, const UI_32 iLength);

//////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NPUnRegisterNotifications
/// \~english @par Brief
///        API to send message to Notification Service to remove a set of notification
/// \~english @param[in] hNPMsgQ
///        HANDLE - Handle to message queue of Notification service.
/// \~english @param[in] pPublisherName
///        PCSTR - Name of Publisher message queue
/// \~english @param[in] numNotifications
///        UI_32 - Number of Notification
/// \~english @param[in] pNotificationArray
///        NotificationInfo - Array of notifications
/// \~english @retval EFrameworkunifiedStatus indicates success or error
/// \~english @retval Success(eFrameworkunifiedStatusOK)
/// \~english @retval Error(anything else)
/// \~english @par
///        EFrameworkunifiedStatus Enum
/// \~english @code
///           typedef enum eFrameworkunifiedStatus {
///             eFrameworkunifiedStatusEmptyMediaList    = -10, ///< Empty media list
///             eFrameworkunifiedStatusSessionLimitMaxedOut  = -9,  ///< Maximum session limit reached
///             eFrameworkunifiedStatusDbRecNotFound       = -8, ///< Database record not found
///             eFrameworkunifiedStatusDbResultError       = -7, ///< Database result error
///             eFrameworkunifiedStatusDbExecuteFail       = -6, ///< Database execute fail
///             eFrameworkunifiedStatusSemCloseFail        = -5, ///< Semaphore close failed
///             eFrameworkunifiedStatusSemUnlockFail       = -4, ///< Semaphore unlock failed
///             eFrameworkunifiedStatusSemLockFail         = -3, ///< Semaphore lock failed
///             eFrameworkunifiedStatusFail                = -2, ///< Failed
///             eFrameworkunifiedStatusErrOther            = -1, ///< Unknown error
///             eFrameworkunifiedStatusOK                  = 0,  ///< Success / Pass / OK
///             eFrameworkunifiedStatusInvldBuf            = 1,  ///< Invalid buffer
///             eFrameworkunifiedStatusInvldHandle         = 2,  ///< Invalid handle
///             eFrameworkunifiedStatusInvldHndlType       = 3,  ///< Invalid handle type
///             eFrameworkunifiedStatusInvldQName        = 4,  ///< Invalid messasge queue name
///             eFrameworkunifiedStatusMsgQFull            = 5,  ///< Message queue full
///             eFrameworkunifiedStatusInvldNotification   = 6,///< The Notification event not present
///             eFrameworkunifiedStatusInvldParam        = 7,  ///< Invalid parameter
///             eFrameworkunifiedStatusInvldBufSize        = 8,  ///< Buf size too small
///             eFrameworkunifiedStatusInvldID             = 9,  ///< Unrecognized ID
///             eFrameworkunifiedStatusCannotRelease       = 10, ///< Cannot release resource
///             eFrameworkunifiedStatusBadConnection       = 11, ///< Could not locate resource
///             eFrameworkunifiedStatusExit            = 12,  ///< Normal application termination
///             eFrameworkunifiedStatusNotImplemented      = 13,  ///< incomplete feature
///             eFrameworkunifiedStatusThreadBusy          = 14,  ///< Joined thread is already being joined
///             eFrameworkunifiedStatusThreadSelfJoin      = 15,  ///< Thread is joining itself
///             eFrameworkunifiedStatusThreadInvalidVal    = 16, ///< Invalid value passed
///             eFrameworkunifiedStatusThreadNotExist      = 17,  ///< The thread does not exist
///             eFrameworkunifiedStatusFault               = 18,  ///< A fault occurred while attempting to make call
///             eFrameworkunifiedStatusServNotFound        = 19,  ///< Service not present in serv dir
///             eFrameworkunifiedStatusServerInUse         = 20,  ///< Service already processing 1 client request
///             eFrameworkunifiedStatusDbIndexing        = 21,  ///< Database Indexing in progress
///             eFrameworkunifiedStatusNullPointer       = 22,
///             eFrameworkunifiedStatusMsgNotProcessed     = 23,
///             eFrameworkunifiedStatusFileLoadSuccess     = 24, ///< File Load Success
///             eFrameworkunifiedStatusFileLoadError       = 25, ///< File Load Error
///             eFrameworkunifiedStatusAccessError         = 26, ///< Error when accessing resource
///             eFrameworkunifiedStatusDuplicate           = 27, ///< Duplicate entry
///             eFrameworkunifiedStatusMsgQEmpty             = 28, ///< Message queue empty
///             eFrameworkunifiedStatusThreadAlreadyRunning  = 29,
///             eFrameworkunifiedStatusErrNoEBADF            = 30,  ///< Bad file descriptor
///             eFrameworkunifiedStatusErrNoEAGAIN           = 31,  ///< Resource unavailable, try again
///             eFrameworkunifiedStatusErrNoEINTR            = 32,  ///< Interrupted system call
///             eFrameworkunifiedStatusSessionErr      = 33,  ///< Error in session handling
///             eFrameworkunifiedStatusDBCorrupt       = 34,  ///< Database corrupt
///             eFrameworkunifiedStatusDBFileNotFound    = 35  ///< Database file not found
///           } EFrameworkunifiedStatus, *PEFrameworkunifiedStatus;
///            @endcode
/// \~english @par Preconditons
///       none
/// \~english @par Change of internal status
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - Function pointer specified by the argument is NULL.
///       - It is an incorrect value(hNPMsgQ) or hNPMsgQ is NULL.
///       - It is an incorrect value(pPublisherName) or pPublisherName is NULL.
///       - It is an incorrect value(numNotifications).
///       - It is an incorrect value(pNotificationArray) or pNotificationArray is NULL.
///       - Allocate memory for deal with original notification array failed.
/// \~english @par Detail
///       Copy original notification array to temp allocated memory and then remove the given notifications from the
///       memory.
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync only
/// \~english @see
///       FrameworkunifiedNPUnRegisterNotifications
//////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPUnRegisterNotifications(HANDLE hNPMsgQ, PCSTR pPublisherName, UI_32 numNotifications,
                                     NotificationInfo *pNotificationArray);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NPPublishNotification
/// \~english @par Brief
///        API to send message to Notification Service to notify subscribers
/// \~english @param[in] hNPMsgQ
///        HANDLE - Handle to message queue of Notification service.
/// \~english @param[in] pPublisherName
///        PCSTR - Name of Publisher message queue
/// \~english @param[in] pNotification
///        PCSTR - Name of Notification
/// \~english @param[in] pData
///        PVOID - Data buffer
/// \~english @param[in] iLength
///        const UI_32 - Size of data buffer
/// \~english @retval eFrameworkunifiedStatusOK
/// \~english @retval eFrameworkunifiedStatusInvldParam
/// \~english @retval eFrameworkunifiedStatusMsgQFull
/// \~english @retval eFrameworkunifiedStatusErrNoEBAD
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR
/// \~english @retval eFrameworkunifiedStatusInvldBufSize
/// \~english @retval eFrameworkunifiedStatusFail
/// \~english @retval eFrameworkunifiedStatusErrOther
/// \~english @par Preconditons
///               - none
/// \~english @par Change of internal status
///               - none
/// \~english @par Conditions of processing failure
///       - queue handle(hNPMsgQ,designated by parameter)is NULL[eFrameworkunifiedStatusInvldParam]
///       - pubulisher name(pPublisherName,designated by parameter)is NULL[eFrameworkunifiedStatusInvldParam]
///       - Notification name(pNotification)is NULL[eFrameworkunifiedStatusInvldParam]
///       - pubulisher name(pPublisherName,designated by parameter)is nullbuffer[eFrameworkunifiedStatusInvldParam]
///       - Notification name(pNotification,designated by parameter)is nullbuffer[eFrameworkunifiedStatusInvldParam]
///       - message queue is full [eFrameworkunifiedStatusMsgQFull]
///       - wrong file discriptor for message sending [eFrameworkunifiedStatusErrNoEBAD]
///       - system call disruput during message sending process[eFrameworkunifiedStatusErrNoEINTR]
///       - wrong buffer size for sending message[eFrameworkunifiedStatusInvldBufSize]
///       - message sending failed[eFrameworkunifiedStatusFail]
///       - failed access to shared memmory for message sending[eFrameworkunifiedStatusErrOther]
/// \~english @par Detail
///        send system info as Notification name and command[NPS_PUBLISH_EV_REQ] through McSendWithSysInfo()
///        NPP service wii process according to command[NPS_PUBLISH_EV_REQ]when get the message above
/// \~english @par Classification
///       - pubulic
/// \~english @par Type
///          Pub-Sub
/// \~english @see McSendWithSysInfo
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPPublishNotification(HANDLE hNPMsgQ, PCSTR pPublisherName, PCSTR pNotification,
                                 PCVOID pData, const UI_32 iLength);


//////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NPSubscribeToNotification
/// \~english @par Brief
///        API to send message to Notification Service to add to subscription list for.
/// \~english @param[in] hNPMsgQ
///        HANDLE - Handle to message queue of Notification service.
/// \~english @param[in] pSubscriberName
///        PCSTR - Name of subscriber message queue
/// \~english @param[in] notif_name
///        PCSTR - Name of Notification
/// \~english @retval EFrameworkunifiedStatus indicates success or error
/// \~english @retval Success(eFrameworkunifiedStatusOK)
/// \~english @retval Error(anything else)
/// \~english @par
///        EFrameworkunifiedStatus Enum
/// \~english @code
///           typedef enum eFrameworkunifiedStatus {
///             eFrameworkunifiedStatusEmptyMediaList    = -10, ///< Empty media list
///             eFrameworkunifiedStatusSessionLimitMaxedOut  = -9,  ///< Maximum session limit reached
///             eFrameworkunifiedStatusDbRecNotFound       = -8, ///< Database record not found
///             eFrameworkunifiedStatusDbResultError       = -7, ///< Database result error
///             eFrameworkunifiedStatusDbExecuteFail       = -6, ///< Database execute fail
///             eFrameworkunifiedStatusSemCloseFail        = -5, ///< Semaphore close failed
///             eFrameworkunifiedStatusSemUnlockFail       = -4, ///< Semaphore unlock failed
///             eFrameworkunifiedStatusSemLockFail         = -3, ///< Semaphore lock failed
///             eFrameworkunifiedStatusFail                = -2, ///< Failed
///             eFrameworkunifiedStatusErrOther            = -1, ///< Unknown error
///             eFrameworkunifiedStatusOK                  = 0,  ///< Success / Pass / OK
///             eFrameworkunifiedStatusInvldBuf            = 1,  ///< Invalid buffer
///             eFrameworkunifiedStatusInvldHandle         = 2,  ///< Invalid handle
///             eFrameworkunifiedStatusInvldHndlType       = 3,  ///< Invalid handle type
///             eFrameworkunifiedStatusInvldQName        = 4,  ///< Invalid messasge queue name
///             eFrameworkunifiedStatusMsgQFull            = 5,  ///< Message queue full
///             eFrameworkunifiedStatusInvldNotification   = 6,///< The Notification event not present
///             eFrameworkunifiedStatusInvldParam        = 7,  ///< Invalid parameter
///             eFrameworkunifiedStatusInvldBufSize        = 8,  ///< Buf size too small
///             eFrameworkunifiedStatusInvldID             = 9,  ///< Unrecognized ID
///             eFrameworkunifiedStatusCannotRelease       = 10, ///< Cannot release resource
///             eFrameworkunifiedStatusBadConnection       = 11, ///< Could not locate resource
///             eFrameworkunifiedStatusExit            = 12,  ///< Normal application termination
///             eFrameworkunifiedStatusNotImplemented      = 13,  ///< incomplete feature
///             eFrameworkunifiedStatusThreadBusy          = 14,  ///< Joined thread is already being joined
///             eFrameworkunifiedStatusThreadSelfJoin      = 15,  ///< Thread is joining itself
///             eFrameworkunifiedStatusThreadInvalidVal    = 16, ///< Invalid value passed
///             eFrameworkunifiedStatusThreadNotExist      = 17,  ///< The thread does not exist
///             eFrameworkunifiedStatusFault               = 18,  ///< A fault occurred while attempting to make call
///             eFrameworkunifiedStatusServNotFound        = 19,  ///< Service not present in serv dir
///             eFrameworkunifiedStatusServerInUse         = 20,  ///< Service already processing 1 client request
///             eFrameworkunifiedStatusDbIndexing        = 21,  ///< Database Indexing in progress
///             eFrameworkunifiedStatusNullPointer       = 22,
///             eFrameworkunifiedStatusMsgNotProcessed     = 23,
///             eFrameworkunifiedStatusFileLoadSuccess     = 24, ///< File Load Success
///             eFrameworkunifiedStatusFileLoadError       = 25, ///< File Load Error
///             eFrameworkunifiedStatusAccessError         = 26, ///< Error when accessing resource
///             eFrameworkunifiedStatusDuplicate           = 27, ///< Duplicate entry
///             eFrameworkunifiedStatusMsgQEmpty             = 28, ///< Message queue empty
///             eFrameworkunifiedStatusThreadAlreadyRunning  = 29,
///             eFrameworkunifiedStatusErrNoEBADF            = 30,  ///< Bad file descriptor
///             eFrameworkunifiedStatusErrNoEAGAIN           = 31,  ///< Resource unavailable, try again
///             eFrameworkunifiedStatusErrNoEINTR            = 32,  ///< Interrupted system call
///             eFrameworkunifiedStatusSessionErr      = 33,  ///< Error in session handling
///             eFrameworkunifiedStatusDBCorrupt       = 34,  ///< Database corrupt
///             eFrameworkunifiedStatusDBFileNotFound    = 35  ///< Database file not found
///           } EFrameworkunifiedStatus, *PEFrameworkunifiedStatus;
///            @endcode
/// \~english @par Preconditons
///       none
/// \~english @par Change of internal status
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - Function pointer specified by the argument is NULL.
///       - It is an incorrect value(hNPMsgQ) or hNPMsgQ is NULL.
///       - It is an incorrect value(pSubscriberName) or pSubscriberName is NULL.
///       - It is an incorrect value(notif_name) or notif_name is NULL.
/// \~english @par Detail
///       Invoke callback function to send message to Notification Service to add to subscription list for given
///       notification.\n
///       If callback function is null and expectedCalls is '0', using default function 'NPSubscribeToNotification' to
///       Notification Service to add to subscription list for given notification.
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync only
/// \~english @see
///       FrameworkunifiedNPSubscribeToNotification
//////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPSubscribeToNotification(HANDLE hNPMsgQ, PCSTR pSubscriberName, PCSTR notif_name);


//////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NPSubscribeToNotifications
/// \~english @par Brief
///        API to send message to Notification Service to add multiple subscriptions for a set of notification.
/// \~english @param[in] hNPMsgQ
///        HANDLE - Handle to message queue of Notification service.
/// \~english @param[in] pSubscriberName
///        PCSTR - Name of subscriber message queue
/// \~english @param[in] numNotifications
///        UI_32 - Number of Notifications
/// \~english @param[in] pSubscribeInfoArray
///        SubscribeInfo* - Array of SubscribeInfo
/// \~english @retval EFrameworkunifiedStatus indicates success or error
/// \~english @retval Success(eFrameworkunifiedStatusOK)
/// \~english @retval Error(anything else)
/// \~english @par
///        EFrameworkunifiedStatus Enum
/// \~english @code
///           typedef enum eFrameworkunifiedStatus {
///             eFrameworkunifiedStatusEmptyMediaList    = -10, ///< Empty media list
///             eFrameworkunifiedStatusSessionLimitMaxedOut  = -9,  ///< Maximum session limit reached
///             eFrameworkunifiedStatusDbRecNotFound       = -8, ///< Database record not found
///             eFrameworkunifiedStatusDbResultError       = -7, ///< Database result error
///             eFrameworkunifiedStatusDbExecuteFail       = -6, ///< Database execute fail
///             eFrameworkunifiedStatusSemCloseFail        = -5, ///< Semaphore close failed
///             eFrameworkunifiedStatusSemUnlockFail       = -4, ///< Semaphore unlock failed
///             eFrameworkunifiedStatusSemLockFail         = -3, ///< Semaphore lock failed
///             eFrameworkunifiedStatusFail                = -2, ///< Failed
///             eFrameworkunifiedStatusErrOther            = -1, ///< Unknown error
///             eFrameworkunifiedStatusOK                  = 0,  ///< Success / Pass / OK
///             eFrameworkunifiedStatusInvldBuf            = 1,  ///< Invalid buffer
///             eFrameworkunifiedStatusInvldHandle         = 2,  ///< Invalid handle
///             eFrameworkunifiedStatusInvldHndlType       = 3,  ///< Invalid handle type
///             eFrameworkunifiedStatusInvldQName        = 4,  ///< Invalid messasge queue name
///             eFrameworkunifiedStatusMsgQFull            = 5,  ///< Message queue full
///             eFrameworkunifiedStatusInvldNotification   = 6,///< The Notification event not present
///             eFrameworkunifiedStatusInvldParam        = 7,  ///< Invalid parameter
///             eFrameworkunifiedStatusInvldBufSize        = 8,  ///< Buf size too small
///             eFrameworkunifiedStatusInvldID             = 9,  ///< Unrecognized ID
///             eFrameworkunifiedStatusCannotRelease       = 10, ///< Cannot release resource
///             eFrameworkunifiedStatusBadConnection       = 11, ///< Could not locate resource
///             eFrameworkunifiedStatusExit            = 12,  ///< Normal application termination
///             eFrameworkunifiedStatusNotImplemented      = 13,  ///< incomplete feature
///             eFrameworkunifiedStatusThreadBusy          = 14,  ///< Joined thread is already being joined
///             eFrameworkunifiedStatusThreadSelfJoin      = 15,  ///< Thread is joining itself
///             eFrameworkunifiedStatusThreadInvalidVal    = 16, ///< Invalid value passed
///             eFrameworkunifiedStatusThreadNotExist      = 17,  ///< The thread does not exist
///             eFrameworkunifiedStatusFault               = 18,  ///< A fault occurred while attempting to make call
///             eFrameworkunifiedStatusServNotFound        = 19,  ///< Service not present in serv dir
///             eFrameworkunifiedStatusServerInUse         = 20,  ///< Service already processing 1 client request
///             eFrameworkunifiedStatusDbIndexing        = 21,  ///< Database Indexing in progress
///             eFrameworkunifiedStatusNullPointer       = 22,
///             eFrameworkunifiedStatusMsgNotProcessed     = 23,
///             eFrameworkunifiedStatusFileLoadSuccess     = 24, ///< File Load Success
///             eFrameworkunifiedStatusFileLoadError       = 25, ///< File Load Error
///             eFrameworkunifiedStatusAccessError         = 26, ///< Error when accessing resource
///             eFrameworkunifiedStatusDuplicate           = 27, ///< Duplicate entry
///             eFrameworkunifiedStatusMsgQEmpty             = 28, ///< Message queue empty
///             eFrameworkunifiedStatusThreadAlreadyRunning  = 29,
///             eFrameworkunifiedStatusErrNoEBADF            = 30,  ///< Bad file descriptor
///             eFrameworkunifiedStatusErrNoEAGAIN           = 31,  ///< Resource unavailable, try again
///             eFrameworkunifiedStatusErrNoEINTR            = 32,  ///< Interrupted system call
///             eFrameworkunifiedStatusSessionErr      = 33,  ///< Error in session handling
///             eFrameworkunifiedStatusDBCorrupt       = 34,  ///< Database corrupt
///             eFrameworkunifiedStatusDBFileNotFound    = 35  ///< Database file not found
///           } EFrameworkunifiedStatus, *PEFrameworkunifiedStatus;
///            @endcode
/// \~english @par Preconditons
///       none
/// \~english @par Change of internal status
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - Function pointer specified by the argument is NULL.
///       - It is an incorrect value(hNPMsgQ) or hNPMsgQ is NULL.
///       - It is an incorrect value(pSubscriberName) or pSubscriberName is NULL.
///       - It is an incorrect value(numNotifications).
///       - It is an incorrect value(pSubscribeInfoArray) or pSubscribeInfoArray is NULL.
/// \~english @par Detail
///       Invoke callback function to send message to Notification Service to add multiple subscriptions for a set of
///       notification.\n
///       If callback function is null and expectedCalls is '0', using default function 'NPSubscribeToNotifications' to
///       Notification Service to add multiple subscriptions for a set of notification.
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync only
/// \~english @see
///       FrameworkunifiedNPSubscribeToNotifications
EFrameworkunifiedStatus NPSubscribeToNotifications(HANDLE hNPMsgQ, PCSTR pSubscriberName, UI_32 numNotifications,
                                      SubscribeInfo *pSubscribeInfoArray);

//////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NPUnsubscribeFromNotification
/// \~english @par Brief
///        API to send message to Notification Service to remove from subscription list for given notification.
/// \~english @param[in] hNPMsgQ
///        HANDLE - Handle to message queue of Notification service.
/// \~english @param[in] pSubscriberName
///        PCSTR - Name of subscriber message queue
/// \~english @param[in] pNotification
///        PCSTR - Name of Notification
/// \~english @retval EFrameworkunifiedStatus indicates success or error
/// \~english @retval Success(eFrameworkunifiedStatusOK)
/// \~english @retval Error(anything else)
/// \~english @par
///        EFrameworkunifiedStatus Enum
/// \~english @code
///           typedef enum eFrameworkunifiedStatus {
///             eFrameworkunifiedStatusEmptyMediaList    = -10, ///< Empty media list
///             eFrameworkunifiedStatusSessionLimitMaxedOut  = -9,  ///< Maximum session limit reached
///             eFrameworkunifiedStatusDbRecNotFound       = -8, ///< Database record not found
///             eFrameworkunifiedStatusDbResultError       = -7, ///< Database result error
///             eFrameworkunifiedStatusDbExecuteFail       = -6, ///< Database execute fail
///             eFrameworkunifiedStatusSemCloseFail        = -5, ///< Semaphore close failed
///             eFrameworkunifiedStatusSemUnlockFail       = -4, ///< Semaphore unlock failed
///             eFrameworkunifiedStatusSemLockFail         = -3, ///< Semaphore lock failed
///             eFrameworkunifiedStatusFail                = -2, ///< Failed
///             eFrameworkunifiedStatusErrOther            = -1, ///< Unknown error
///             eFrameworkunifiedStatusOK                  = 0,  ///< Success / Pass / OK
///             eFrameworkunifiedStatusInvldBuf            = 1,  ///< Invalid buffer
///             eFrameworkunifiedStatusInvldHandle         = 2,  ///< Invalid handle
///             eFrameworkunifiedStatusInvldHndlType       = 3,  ///< Invalid handle type
///             eFrameworkunifiedStatusInvldQName        = 4,  ///< Invalid messasge queue name
///             eFrameworkunifiedStatusMsgQFull            = 5,  ///< Message queue full
///             eFrameworkunifiedStatusInvldNotification   = 6,///< The Notification event not present
///             eFrameworkunifiedStatusInvldParam        = 7,  ///< Invalid parameter
///             eFrameworkunifiedStatusInvldBufSize        = 8,  ///< Buf size too small
///             eFrameworkunifiedStatusInvldID             = 9,  ///< Unrecognized ID
///             eFrameworkunifiedStatusCannotRelease       = 10, ///< Cannot release resource
///             eFrameworkunifiedStatusBadConnection       = 11, ///< Could not locate resource
///             eFrameworkunifiedStatusExit            = 12,  ///< Normal application termination
///             eFrameworkunifiedStatusNotImplemented      = 13,  ///< incomplete feature
///             eFrameworkunifiedStatusThreadBusy          = 14,  ///< Joined thread is already being joined
///             eFrameworkunifiedStatusThreadSelfJoin      = 15,  ///< Thread is joining itself
///             eFrameworkunifiedStatusThreadInvalidVal    = 16, ///< Invalid value passed
///             eFrameworkunifiedStatusThreadNotExist      = 17,  ///< The thread does not exist
///             eFrameworkunifiedStatusFault               = 18,  ///< A fault occurred while attempting to make call
///             eFrameworkunifiedStatusServNotFound        = 19,  ///< Service not present in serv dir
///             eFrameworkunifiedStatusServerInUse         = 20,  ///< Service already processing 1 client request
///             eFrameworkunifiedStatusDbIndexing        = 21,  ///< Database Indexing in progress
///             eFrameworkunifiedStatusNullPointer       = 22,
///             eFrameworkunifiedStatusMsgNotProcessed     = 23,
///             eFrameworkunifiedStatusFileLoadSuccess     = 24, ///< File Load Success
///             eFrameworkunifiedStatusFileLoadError       = 25, ///< File Load Error
///             eFrameworkunifiedStatusAccessError         = 26, ///< Error when accessing resource
///             eFrameworkunifiedStatusDuplicate           = 27, ///< Duplicate entry
///             eFrameworkunifiedStatusMsgQEmpty             = 28, ///< Message queue empty
///             eFrameworkunifiedStatusThreadAlreadyRunning  = 29,
///             eFrameworkunifiedStatusErrNoEBADF            = 30,  ///< Bad file descriptor
///             eFrameworkunifiedStatusErrNoEAGAIN           = 31,  ///< Resource unavailable, try again
///             eFrameworkunifiedStatusErrNoEINTR            = 32,  ///< Interrupted system call
///             eFrameworkunifiedStatusSessionErr      = 33,  ///< Error in session handling
///             eFrameworkunifiedStatusDBCorrupt       = 34,  ///< Database corrupt
///             eFrameworkunifiedStatusDBFileNotFound    = 35  ///< Database file not found
///           } EFrameworkunifiedStatus, *PEFrameworkunifiedStatus;
///            @endcode
/// \~english @par Preconditons
///       none
/// \~english @par Change of internal status
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - Function pointer specified by the argument is NULL.
///       - It is an incorrect value(hNPMsgQ) or hNPMsgQ is NULL.
///       - It is an incorrect value(pSubscriberName) or pSubscriberName is NULL.
///       - It is an incorrect value(pNotification) or pNotification is NULL.
/// \~english @par Detail
///       Invoke callback function to send message to Notification Service to remove from subscription list for given
///       notification.\n
///       If callback function is null and expectedCalls is '0', using default function 'NPUnsubscribeFromNotification'
///       to send message to Notification Service to remove from subscription list for given notification.
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync only
/// \~english @see
///       FrameworkunifiedNPUnsubscribeFromNotification
EFrameworkunifiedStatus NPUnsubscribeFromNotification(HANDLE hNPMsgQ, PCSTR pSubscriberName, PCSTR pNotification);

//////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NPUnsubscribeFromNotifications
/// \~english @par Brief
///        API to send message to Notification Service to unsubscribe multiple subscriptions for a set of notification.
/// \~english @param[in] hNPMsgQ
///        HANDLE - Handle to message queue of Notification service.
/// \~english @param[in] pSubscriberName
///        PCSTR - Name of subscriber message queue
/// \~english @param[in] numNotifications
///        UI_32 - Number of Notifications
/// \~english @param[in] pUnSubscribeInfoArray
///        SubscribeInfo - Array of notifications
/// \~english @retval EFrameworkunifiedStatus indicates success or error
/// \~english @retval Success(eFrameworkunifiedStatusOK)
/// \~english @retval Error(anything else)
/// \~english @par
///        EFrameworkunifiedStatus Enum
/// \~english @code
///           typedef enum eFrameworkunifiedStatus {
///             eFrameworkunifiedStatusEmptyMediaList    = -10, ///< Empty media list
///             eFrameworkunifiedStatusSessionLimitMaxedOut  = -9,  ///< Maximum session limit reached
///             eFrameworkunifiedStatusDbRecNotFound       = -8, ///< Database record not found
///             eFrameworkunifiedStatusDbResultError       = -7, ///< Database result error
///             eFrameworkunifiedStatusDbExecuteFail       = -6, ///< Database execute fail
///             eFrameworkunifiedStatusSemCloseFail        = -5, ///< Semaphore close failed
///             eFrameworkunifiedStatusSemUnlockFail       = -4, ///< Semaphore unlock failed
///             eFrameworkunifiedStatusSemLockFail         = -3, ///< Semaphore lock failed
///             eFrameworkunifiedStatusFail                = -2, ///< Failed
///             eFrameworkunifiedStatusErrOther            = -1, ///< Unknown error
///             eFrameworkunifiedStatusOK                  = 0,  ///< Success / Pass / OK
///             eFrameworkunifiedStatusInvldBuf            = 1,  ///< Invalid buffer
///             eFrameworkunifiedStatusInvldHandle         = 2,  ///< Invalid handle
///             eFrameworkunifiedStatusInvldHndlType       = 3,  ///< Invalid handle type
///             eFrameworkunifiedStatusInvldQName        = 4,  ///< Invalid messasge queue name
///             eFrameworkunifiedStatusMsgQFull            = 5,  ///< Message queue full
///             eFrameworkunifiedStatusInvldNotification   = 6,///< The Notification event not present
///             eFrameworkunifiedStatusInvldParam        = 7,  ///< Invalid parameter
///             eFrameworkunifiedStatusInvldBufSize        = 8,  ///< Buf size too small
///             eFrameworkunifiedStatusInvldID             = 9,  ///< Unrecognized ID
///             eFrameworkunifiedStatusCannotRelease       = 10, ///< Cannot release resource
///             eFrameworkunifiedStatusBadConnection       = 11, ///< Could not locate resource
///             eFrameworkunifiedStatusExit            = 12,  ///< Normal application termination
///             eFrameworkunifiedStatusNotImplemented      = 13,  ///< incomplete feature
///             eFrameworkunifiedStatusThreadBusy          = 14,  ///< Joined thread is already being joined
///             eFrameworkunifiedStatusThreadSelfJoin      = 15,  ///< Thread is joining itself
///             eFrameworkunifiedStatusThreadInvalidVal    = 16, ///< Invalid value passed
///             eFrameworkunifiedStatusThreadNotExist      = 17,  ///< The thread does not exist
///             eFrameworkunifiedStatusFault               = 18,  ///< A fault occurred while attempting to make call
///             eFrameworkunifiedStatusServNotFound        = 19,  ///< Service not present in serv dir
///             eFrameworkunifiedStatusServerInUse         = 20,  ///< Service already processing 1 client request
///             eFrameworkunifiedStatusDbIndexing        = 21,  ///< Database Indexing in progress
///             eFrameworkunifiedStatusNullPointer       = 22,
///             eFrameworkunifiedStatusMsgNotProcessed     = 23,
///             eFrameworkunifiedStatusFileLoadSuccess     = 24, ///< File Load Success
///             eFrameworkunifiedStatusFileLoadError       = 25, ///< File Load Error
///             eFrameworkunifiedStatusAccessError         = 26, ///< Error when accessing resource
///             eFrameworkunifiedStatusDuplicate           = 27, ///< Duplicate entry
///             eFrameworkunifiedStatusMsgQEmpty             = 28, ///< Message queue empty
///             eFrameworkunifiedStatusThreadAlreadyRunning  = 29,
///             eFrameworkunifiedStatusErrNoEBADF            = 30,  ///< Bad file descriptor
///             eFrameworkunifiedStatusErrNoEAGAIN           = 31,  ///< Resource unavailable, try again
///             eFrameworkunifiedStatusErrNoEINTR            = 32,  ///< Interrupted system call
///             eFrameworkunifiedStatusSessionErr      = 33,  ///< Error in session handling
///             eFrameworkunifiedStatusDBCorrupt       = 34,  ///< Database corrupt
///             eFrameworkunifiedStatusDBFileNotFound    = 35  ///< Database file not found
///           } EFrameworkunifiedStatus, *PEFrameworkunifiedStatus;
///            @endcode
/// \~english @par Preconditons
///       none
/// \~english @par Change of internal status
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - Function pointer specified by the argument is NULL.
///       - It is an incorrect value(hNPMsgQ) or hNPMsgQ is NULL.
///       - It is an incorrect value(pSubscriberName) or pSubscriberName is NULL.
///       - It is an incorrect value(numNotifications).
///       - It is an incorrect value(pUnSubscribeInfoArray) or pUnSubscribeInfoArray is NULL.
/// \~english @par Detail
///       Invoke callback function to send message to Notification Service to unsubscribe multiple subscriptions for a
///       set of notification.\n
///       If callback function is null and expectedCalls is '0', using default function 'NPUnsubscribeFromNotifications'
///       to send message to Notification Service to unsubscribe multiple subscriptions for a set of notification.
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync only
/// \~english @see
///       FrameworkunifiedNPUnsubscribeFromNotifications
EFrameworkunifiedStatus NPUnsubscribeFromNotifications(HANDLE hNPMsgQ, PCSTR pUnSubscriberName, UI_32 numNotifications,
                                          SubscribeInfo *pUnSubscribeInfoArray);

//////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NPReadPersistedData
/// \~english @par Brief
///        API to requested the persistent data corresponding to the notification if available.
/// \~english @param[in] hNPMsgQ
///        HANDLE - Handle to message queue of Notification service.
/// \~english @param[in] pPublisherName
///        PCSTR - Name of publisher message queue
/// \~english @param[in] notification
///        PCSTR - Name of Notification
/// \~english @retval EFrameworkunifiedStatus indicates success or error
/// \~english @retval Success(eFrameworkunifiedStatusOK)
/// \~english @retval Error(anything else)
/// \~english @par
///        EFrameworkunifiedStatus Enum
/// \~english @code
///           typedef enum eFrameworkunifiedStatus {
///             eFrameworkunifiedStatusEmptyMediaList    = -10, ///< Empty media list
///             eFrameworkunifiedStatusSessionLimitMaxedOut  = -9,  ///< Maximum session limit reached
///             eFrameworkunifiedStatusDbRecNotFound       = -8, ///< Database record not found
///             eFrameworkunifiedStatusDbResultError       = -7, ///< Database result error
///             eFrameworkunifiedStatusDbExecuteFail       = -6, ///< Database execute fail
///             eFrameworkunifiedStatusSemCloseFail        = -5, ///< Semaphore close failed
///             eFrameworkunifiedStatusSemUnlockFail       = -4, ///< Semaphore unlock failed
///             eFrameworkunifiedStatusSemLockFail         = -3, ///< Semaphore lock failed
///             eFrameworkunifiedStatusFail                = -2, ///< Failed
///             eFrameworkunifiedStatusErrOther            = -1, ///< Unknown error
///             eFrameworkunifiedStatusOK                  = 0,  ///< Success / Pass / OK
///             eFrameworkunifiedStatusInvldBuf            = 1,  ///< Invalid buffer
///             eFrameworkunifiedStatusInvldHandle         = 2,  ///< Invalid handle
///             eFrameworkunifiedStatusInvldHndlType       = 3,  ///< Invalid handle type
///             eFrameworkunifiedStatusInvldQName        = 4,  ///< Invalid messasge queue name
///             eFrameworkunifiedStatusMsgQFull            = 5,  ///< Message queue full
///             eFrameworkunifiedStatusInvldNotification   = 6,///< The Notification event not present
///             eFrameworkunifiedStatusInvldParam        = 7,  ///< Invalid parameter
///             eFrameworkunifiedStatusInvldBufSize        = 8,  ///< Buf size too small
///             eFrameworkunifiedStatusInvldID             = 9,  ///< Unrecognized ID
///             eFrameworkunifiedStatusCannotRelease       = 10, ///< Cannot release resource
///             eFrameworkunifiedStatusBadConnection       = 11, ///< Could not locate resource
///             eFrameworkunifiedStatusExit            = 12,  ///< Normal application termination
///             eFrameworkunifiedStatusNotImplemented      = 13,  ///< incomplete feature
///             eFrameworkunifiedStatusThreadBusy          = 14,  ///< Joined thread is already being joined
///             eFrameworkunifiedStatusThreadSelfJoin      = 15,  ///< Thread is joining itself
///             eFrameworkunifiedStatusThreadInvalidVal    = 16, ///< Invalid value passed
///             eFrameworkunifiedStatusThreadNotExist      = 17,  ///< The thread does not exist
///             eFrameworkunifiedStatusFault               = 18,  ///< A fault occurred while attempting to make call
///             eFrameworkunifiedStatusServNotFound        = 19,  ///< Service not present in serv dir
///             eFrameworkunifiedStatusServerInUse         = 20,  ///< Service already processing 1 client request
///             eFrameworkunifiedStatusDbIndexing        = 21,  ///< Database Indexing in progress
///             eFrameworkunifiedStatusNullPointer       = 22,
///             eFrameworkunifiedStatusMsgNotProcessed     = 23,
///             eFrameworkunifiedStatusFileLoadSuccess     = 24, ///< File Load Success
///             eFrameworkunifiedStatusFileLoadError       = 25, ///< File Load Error
///             eFrameworkunifiedStatusAccessError         = 26, ///< Error when accessing resource
///             eFrameworkunifiedStatusDuplicate           = 27, ///< Duplicate entry
///             eFrameworkunifiedStatusMsgQEmpty             = 28, ///< Message queue empty
///             eFrameworkunifiedStatusThreadAlreadyRunning  = 29,
///             eFrameworkunifiedStatusErrNoEBADF            = 30,  ///< Bad file descriptor
///             eFrameworkunifiedStatusErrNoEAGAIN           = 31,  ///< Resource unavailable, try again
///             eFrameworkunifiedStatusErrNoEINTR            = 32,  ///< Interrupted system call
///             eFrameworkunifiedStatusSessionErr      = 33,  ///< Error in session handling
///             eFrameworkunifiedStatusDBCorrupt       = 34,  ///< Database corrupt
///             eFrameworkunifiedStatusDBFileNotFound    = 35  ///< Database file not found
///           } EFrameworkunifiedStatus, *PEFrameworkunifiedStatus;
///            @endcode
/// \~english @par Preconditons
///       none
/// \~english @par Change of internal status
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - Function pointer specified by the argument is NULL.
///       - It is an incorrect value(hNPMsgQ) or hNPMsgQ is NULL.
///       - It is an incorrect value(pSubscriberName) or pSubscriberName is NULL.
///       - It is an incorrect value(notification) or notification is NULL.
/// \~english @par Detail
///       Invoke callback function to send message to requested the persistent data corresponding to the notification if
///       available.\n
///       If callback function is null and expectedCalls is '0', using default function 'NPReadPersistedData' to
///       requested the persistent data corresponding to the notification if available.
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync only
/// \~english @see
///       FrameworkunifiedNPReadPersistedData
EFrameworkunifiedStatus NPReadPersistedData(HANDLE hNPMsgQ, PCSTR pPublisherName, PCSTR notification);

//////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NPSavePersistentData
/// \~english @par Brief
///        API to send message to Notification Service to save all persisted data that in Ram to the file system via a
///        file write.
/// \~english @param[in] hNPMsgQ
///        HANDLE - Handle to message queue of Notification service.
/// \~english @param[in] pPublisherName
///        PCSTR - Name of publisher message queue
/// \~english @retval EFrameworkunifiedStatus indicates success or error
/// \~english @retval Success(eFrameworkunifiedStatusOK)
/// \~english @retval Error(anything else)
/// \~english @par
///        EFrameworkunifiedStatus Enum
/// \~english @code
///           typedef enum eFrameworkunifiedStatus {
///             eFrameworkunifiedStatusEmptyMediaList    = -10, ///< Empty media list
///             eFrameworkunifiedStatusSessionLimitMaxedOut  = -9,  ///< Maximum session limit reached
///             eFrameworkunifiedStatusDbRecNotFound       = -8, ///< Database record not found
///             eFrameworkunifiedStatusDbResultError       = -7, ///< Database result error
///             eFrameworkunifiedStatusDbExecuteFail       = -6, ///< Database execute fail
///             eFrameworkunifiedStatusSemCloseFail        = -5, ///< Semaphore close failed
///             eFrameworkunifiedStatusSemUnlockFail       = -4, ///< Semaphore unlock failed
///             eFrameworkunifiedStatusSemLockFail         = -3, ///< Semaphore lock failed
///             eFrameworkunifiedStatusFail                = -2, ///< Failed
///             eFrameworkunifiedStatusErrOther            = -1, ///< Unknown error
///             eFrameworkunifiedStatusOK                  = 0,  ///< Success / Pass / OK
///             eFrameworkunifiedStatusInvldBuf            = 1,  ///< Invalid buffer
///             eFrameworkunifiedStatusInvldHandle         = 2,  ///< Invalid handle
///             eFrameworkunifiedStatusInvldHndlType       = 3,  ///< Invalid handle type
///             eFrameworkunifiedStatusInvldQName        = 4,  ///< Invalid messasge queue name
///             eFrameworkunifiedStatusMsgQFull            = 5,  ///< Message queue full
///             eFrameworkunifiedStatusInvldNotification   = 6,///< The Notification event not present
///             eFrameworkunifiedStatusInvldParam        = 7,  ///< Invalid parameter
///             eFrameworkunifiedStatusInvldBufSize        = 8,  ///< Buf size too small
///             eFrameworkunifiedStatusInvldID             = 9,  ///< Unrecognized ID
///             eFrameworkunifiedStatusCannotRelease       = 10, ///< Cannot release resource
///             eFrameworkunifiedStatusBadConnection       = 11, ///< Could not locate resource
///             eFrameworkunifiedStatusExit            = 12,  ///< Normal application termination
///             eFrameworkunifiedStatusNotImplemented      = 13,  ///< incomplete feature
///             eFrameworkunifiedStatusThreadBusy          = 14,  ///< Joined thread is already being joined
///             eFrameworkunifiedStatusThreadSelfJoin      = 15,  ///< Thread is joining itself
///             eFrameworkunifiedStatusThreadInvalidVal    = 16, ///< Invalid value passed
///             eFrameworkunifiedStatusThreadNotExist      = 17,  ///< The thread does not exist
///             eFrameworkunifiedStatusFault               = 18,  ///< A fault occurred while attempting to make call
///             eFrameworkunifiedStatusServNotFound        = 19,  ///< Service not present in serv dir
///             eFrameworkunifiedStatusServerInUse         = 20,  ///< Service already processing 1 client request
///             eFrameworkunifiedStatusDbIndexing        = 21,  ///< Database Indexing in progress
///             eFrameworkunifiedStatusNullPointer       = 22,
///             eFrameworkunifiedStatusMsgNotProcessed     = 23,
///             eFrameworkunifiedStatusFileLoadSuccess     = 24, ///< File Load Success
///             eFrameworkunifiedStatusFileLoadError       = 25, ///< File Load Error
///             eFrameworkunifiedStatusAccessError         = 26, ///< Error when accessing resource
///             eFrameworkunifiedStatusDuplicate           = 27, ///< Duplicate entry
///             eFrameworkunifiedStatusMsgQEmpty             = 28, ///< Message queue empty
///             eFrameworkunifiedStatusThreadAlreadyRunning  = 29,
///             eFrameworkunifiedStatusErrNoEBADF            = 30,  ///< Bad file descriptor
///             eFrameworkunifiedStatusErrNoEAGAIN           = 31,  ///< Resource unavailable, try again
///             eFrameworkunifiedStatusErrNoEINTR            = 32,  ///< Interrupted system call
///             eFrameworkunifiedStatusSessionErr      = 33,  ///< Error in session handling
///             eFrameworkunifiedStatusDBCorrupt       = 34,  ///< Database corrupt
///             eFrameworkunifiedStatusDBFileNotFound    = 35  ///< Database file not found
///           } EFrameworkunifiedStatus, *PEFrameworkunifiedStatus;
///            @endcode
/// \~english @par Preconditons
///       none
/// \~english @par Change of internal status
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - Function pointer specified by the argument is NULL.
///       - It is an incorrect value(hNPMsgQ) or hNPMsgQ is NULL.
///       - It is an incorrect value(pSubscriberName) or pSubscriberName is NULL.
/// \~english @par Detail
///       Invoke callback function to send message to Notification Service to save all persisted data.\n
///       If callback function is null and expectedCalls is '0', using default function 'NPSavePersistentData' to send
///       message to Notification Service to save all persisted data.
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync only
/// \~english @see
///       FrameworkunifiedNPSavePersistentData
EFrameworkunifiedStatus NPSavePersistentData(HANDLE hNPMsgQ, PCSTR pPublisherName);

//////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NPRegisterPersistentFile
/// \~english @par Brief
///        API to send message to Notification Service to add a file path as to be persisted.
/// \~english @param[in] hNPMsgQ
///        HANDLE - Handle to message queue of Notification service.
/// \~english @param[in] pPublisherName
///        PCSTR - Name of publisher message queue.
/// \~english @param[in] pTag
///        PCSTR - A unique identifier for the file path specified, this is used in retrieve/Load the file later.
/// \~english @param[in] bIsUserFile
///        BOOL - Set TRUE, if registering user file.
/// \~english @retval EFrameworkunifiedStatus indicates success or error
/// \~english @retval Success(eFrameworkunifiedStatusOK)
/// \~english @retval Error(anything else)
/// \~english @par
///        EFrameworkunifiedStatus Enum
/// \~english @code
///           typedef enum eFrameworkunifiedStatus {
///             eFrameworkunifiedStatusEmptyMediaList    = -10, ///< Empty media list
///             eFrameworkunifiedStatusSessionLimitMaxedOut  = -9,  ///< Maximum session limit reached
///             eFrameworkunifiedStatusDbRecNotFound       = -8, ///< Database record not found
///             eFrameworkunifiedStatusDbResultError       = -7, ///< Database result error
///             eFrameworkunifiedStatusDbExecuteFail       = -6, ///< Database execute fail
///             eFrameworkunifiedStatusSemCloseFail        = -5, ///< Semaphore close failed
///             eFrameworkunifiedStatusSemUnlockFail       = -4, ///< Semaphore unlock failed
///             eFrameworkunifiedStatusSemLockFail         = -3, ///< Semaphore lock failed
///             eFrameworkunifiedStatusFail                = -2, ///< Failed
///             eFrameworkunifiedStatusErrOther            = -1, ///< Unknown error
///             eFrameworkunifiedStatusOK                  = 0,  ///< Success / Pass / OK
///             eFrameworkunifiedStatusInvldBuf            = 1,  ///< Invalid buffer
///             eFrameworkunifiedStatusInvldHandle         = 2,  ///< Invalid handle
///             eFrameworkunifiedStatusInvldHndlType       = 3,  ///< Invalid handle type
///             eFrameworkunifiedStatusInvldQName        = 4,  ///< Invalid messasge queue name
///             eFrameworkunifiedStatusMsgQFull            = 5,  ///< Message queue full
///             eFrameworkunifiedStatusInvldNotification   = 6,///< The Notification event not present
///             eFrameworkunifiedStatusInvldParam        = 7,  ///< Invalid parameter
///             eFrameworkunifiedStatusInvldBufSize        = 8,  ///< Buf size too small
///             eFrameworkunifiedStatusInvldID             = 9,  ///< Unrecognized ID
///             eFrameworkunifiedStatusCannotRelease       = 10, ///< Cannot release resource
///             eFrameworkunifiedStatusBadConnection       = 11, ///< Could not locate resource
///             eFrameworkunifiedStatusExit            = 12,  ///< Normal application termination
///             eFrameworkunifiedStatusNotImplemented      = 13,  ///< incomplete feature
///             eFrameworkunifiedStatusThreadBusy          = 14,  ///< Joined thread is already being joined
///             eFrameworkunifiedStatusThreadSelfJoin      = 15,  ///< Thread is joining itself
///             eFrameworkunifiedStatusThreadInvalidVal    = 16, ///< Invalid value passed
///             eFrameworkunifiedStatusThreadNotExist      = 17,  ///< The thread does not exist
///             eFrameworkunifiedStatusFault               = 18,  ///< A fault occurred while attempting to make call
///             eFrameworkunifiedStatusServNotFound        = 19,  ///< Service not present in serv dir
///             eFrameworkunifiedStatusServerInUse         = 20,  ///< Service already processing 1 client request
///             eFrameworkunifiedStatusDbIndexing        = 21,  ///< Database Indexing in progress
///             eFrameworkunifiedStatusNullPointer       = 22,
///             eFrameworkunifiedStatusMsgNotProcessed     = 23,
///             eFrameworkunifiedStatusFileLoadSuccess     = 24, ///< File Load Success
///             eFrameworkunifiedStatusFileLoadError       = 25, ///< File Load Error
///             eFrameworkunifiedStatusAccessError         = 26, ///< Error when accessing resource
///             eFrameworkunifiedStatusDuplicate           = 27, ///< Duplicate entry
///             eFrameworkunifiedStatusMsgQEmpty             = 28, ///< Message queue empty
///             eFrameworkunifiedStatusThreadAlreadyRunning  = 29,
///             eFrameworkunifiedStatusErrNoEBADF            = 30,  ///< Bad file descriptor
///             eFrameworkunifiedStatusErrNoEAGAIN           = 31,  ///< Resource unavailable, try again
///             eFrameworkunifiedStatusErrNoEINTR            = 32,  ///< Interrupted system call
///             eFrameworkunifiedStatusSessionErr      = 33,  ///< Error in session handling
///             eFrameworkunifiedStatusDBCorrupt       = 34,  ///< Database corrupt
///             eFrameworkunifiedStatusDBFileNotFound    = 35  ///< Database file not found
///           } EFrameworkunifiedStatus, *PEFrameworkunifiedStatus;
///            @endcode
/// \~english @par Preconditons
///       none
/// \~english @par Change of internal status
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - Function pointer specified by the argument is NULL.
///       - It is an incorrect value(hNPMsgQ) or hNPMsgQ is NULL.
///       - It is an incorrect value(pSubscriberName) or pSubscriberName is NULL.
///       - It is an incorrect value(pTag) or pTag is NULL.
/// \~english @par Detail
///       Invoke callback function to send message to Notification Service to add a file path as to be persisted.\n
///       If callback function is null and expectedCalls is '0', using default function 'NPRegisterPersistentFile' to
///       send message to Notification Service to add a file path as to be persisted.
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync only
/// \~english @see
///       FrameworkunifiedNPRegisterPersistentFile
EFrameworkunifiedStatus NPRegisterPersistentFile(HANDLE hNPMsgQ, PCSTR pPublisherName, PCSTR pTag, BOOL bIsUserFile);

//////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NPSetFilePersistentType
/// \~english @par Brief
///        API to send message to Notification Service to add a file path as to be persisted.
/// \~english @param[in] hNPMsgQ
///        HANDLE - Handle to message queue of Notification service.
/// \~english @param[in] pPublisherName
///        PCSTR - Name of publisher message queue.
/// \~english @param[in] pTag
///        PCSTR - A unique identifier for the file path specified, this is used in retrieve/Load the file later.
/// \~english @param[in] ePersistCategory
///        EFrameworkunifiedPersistCategory - persist category
/// \~english @retval EFrameworkunifiedStatus indicates success or error
/// \~english @retval Success(eFrameworkunifiedStatusOK)
/// \~english @retval Error(anything else)
/// \~english @par
///        EFrameworkunifiedStatus Enum
/// \~english @code
///           typedef enum eFrameworkunifiedStatus {
///             eFrameworkunifiedStatusEmptyMediaList    = -10, ///< Empty media list
///             eFrameworkunifiedStatusSessionLimitMaxedOut  = -9,  ///< Maximum session limit reached
///             eFrameworkunifiedStatusDbRecNotFound       = -8, ///< Database record not found
///             eFrameworkunifiedStatusDbResultError       = -7, ///< Database result error
///             eFrameworkunifiedStatusDbExecuteFail       = -6, ///< Database execute fail
///             eFrameworkunifiedStatusSemCloseFail        = -5, ///< Semaphore close failed
///             eFrameworkunifiedStatusSemUnlockFail       = -4, ///< Semaphore unlock failed
///             eFrameworkunifiedStatusSemLockFail         = -3, ///< Semaphore lock failed
///             eFrameworkunifiedStatusFail                = -2, ///< Failed
///             eFrameworkunifiedStatusErrOther            = -1, ///< Unknown error
///             eFrameworkunifiedStatusOK                  = 0,  ///< Success / Pass / OK
///             eFrameworkunifiedStatusInvldBuf            = 1,  ///< Invalid buffer
///             eFrameworkunifiedStatusInvldHandle         = 2,  ///< Invalid handle
///             eFrameworkunifiedStatusInvldHndlType       = 3,  ///< Invalid handle type
///             eFrameworkunifiedStatusInvldQName        = 4,  ///< Invalid messasge queue name
///             eFrameworkunifiedStatusMsgQFull            = 5,  ///< Message queue full
///             eFrameworkunifiedStatusInvldNotification   = 6,///< The Notification event not present
///             eFrameworkunifiedStatusInvldParam        = 7,  ///< Invalid parameter
///             eFrameworkunifiedStatusInvldBufSize        = 8,  ///< Buf size too small
///             eFrameworkunifiedStatusInvldID             = 9,  ///< Unrecognized ID
///             eFrameworkunifiedStatusCannotRelease       = 10, ///< Cannot release resource
///             eFrameworkunifiedStatusBadConnection       = 11, ///< Could not locate resource
///             eFrameworkunifiedStatusExit            = 12,  ///< Normal application termination
///             eFrameworkunifiedStatusNotImplemented      = 13,  ///< incomplete feature
///             eFrameworkunifiedStatusThreadBusy          = 14,  ///< Joined thread is already being joined
///             eFrameworkunifiedStatusThreadSelfJoin      = 15,  ///< Thread is joining itself
///             eFrameworkunifiedStatusThreadInvalidVal    = 16, ///< Invalid value passed
///             eFrameworkunifiedStatusThreadNotExist      = 17,  ///< The thread does not exist
///             eFrameworkunifiedStatusFault               = 18,  ///< A fault occurred while attempting to make call
///             eFrameworkunifiedStatusServNotFound        = 19,  ///< Service not present in serv dir
///             eFrameworkunifiedStatusServerInUse         = 20,  ///< Service already processing 1 client request
///             eFrameworkunifiedStatusDbIndexing        = 21,  ///< Database Indexing in progress
///             eFrameworkunifiedStatusNullPointer       = 22,
///             eFrameworkunifiedStatusMsgNotProcessed     = 23,
///             eFrameworkunifiedStatusFileLoadSuccess     = 24, ///< File Load Success
///             eFrameworkunifiedStatusFileLoadError       = 25, ///< File Load Error
///             eFrameworkunifiedStatusAccessError         = 26, ///< Error when accessing resource
///             eFrameworkunifiedStatusDuplicate           = 27, ///< Duplicate entry
///             eFrameworkunifiedStatusMsgQEmpty             = 28, ///< Message queue empty
///             eFrameworkunifiedStatusThreadAlreadyRunning  = 29,
///             eFrameworkunifiedStatusErrNoEBADF            = 30,  ///< Bad file descriptor
///             eFrameworkunifiedStatusErrNoEAGAIN           = 31,  ///< Resource unavailable, try again
///             eFrameworkunifiedStatusErrNoEINTR            = 32,  ///< Interrupted system call
///             eFrameworkunifiedStatusSessionErr      = 33,  ///< Error in session handling
///             eFrameworkunifiedStatusDBCorrupt       = 34,  ///< Database corrupt
///             eFrameworkunifiedStatusDBFileNotFound    = 35  ///< Database file not found
///           } EFrameworkunifiedStatus, *PEFrameworkunifiedStatus;
///            @endcode
/// \~english @par Preconditons
///       none
/// \~english @par Change of internal status
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - Function pointer specified by the argument is NULL.
///       - It is an incorrect value(hNPMsgQ) or hNPMsgQ is NULL.
///       - It is an incorrect value(pSubscriberName) or pSubscriberName is NULL.
///       - It is an incorrect value(pTag) or pTag is NULL.
/// \~english @par Detail
///       Invoke callback function to send message to Notification Service to add a file path as to be persisted. \n
///       If callback function is null and expectedCalls is '0', using default function 'NPSetFilePersistentType' to
///       send message to Notification Service to add a file path as to be persisted.
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync only
/// \~english @see
///       FrameworkunifiedNPSetFilePersistentType
EFrameworkunifiedStatus NPSetFilePersistentType(HANDLE hNPMsgQ, PCSTR pPublisherName,
                                   PCSTR pTag, EFrameworkunifiedPersistCategory ePersistCategory);

//////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NPLoadPersistentFile
/// \~english @par Brief
///        API to send message to Notification Service to copy file from persistent memory to
///        specified path. The caller recieves an acknowledgement once NPS completes file copy
/// \~english @param[in] hNPMsgQ
///        HANDLE - Handle to message queue of Notification service.
/// \~english @param[in] pPublisherName
///        PCSTR - Name of publisher message queue.
/// \~english @param[in] pDstFilePath
///        PCSTR - Complete file path to which the file should be copied from persistent area.
/// \~english @param[in] pTag
///        PCSTR - Tag associated with the file that needs to be copied to pDstFilePath.
/// \~english @param[in] hUser
///        HANDLE - Load the file for the specified user.
/// \~english @retval EFrameworkunifiedStatus indicates success or error
/// \~english @retval Success(eFrameworkunifiedStatusOK)
/// \~english @retval Error(anything else)
/// \~english @par
///        EFrameworkunifiedStatus Enum
/// \~english @code
///           typedef enum eFrameworkunifiedStatus {
///             eFrameworkunifiedStatusEmptyMediaList    = -10, ///< Empty media list
///             eFrameworkunifiedStatusSessionLimitMaxedOut  = -9,  ///< Maximum session limit reached
///             eFrameworkunifiedStatusDbRecNotFound       = -8, ///< Database record not found
///             eFrameworkunifiedStatusDbResultError       = -7, ///< Database result error
///             eFrameworkunifiedStatusDbExecuteFail       = -6, ///< Database execute fail
///             eFrameworkunifiedStatusSemCloseFail        = -5, ///< Semaphore close failed
///             eFrameworkunifiedStatusSemUnlockFail       = -4, ///< Semaphore unlock failed
///             eFrameworkunifiedStatusSemLockFail         = -3, ///< Semaphore lock failed
///             eFrameworkunifiedStatusFail                = -2, ///< Failed
///             eFrameworkunifiedStatusErrOther            = -1, ///< Unknown error
///             eFrameworkunifiedStatusOK                  = 0,  ///< Success / Pass / OK
///             eFrameworkunifiedStatusInvldBuf            = 1,  ///< Invalid buffer
///             eFrameworkunifiedStatusInvldHandle         = 2,  ///< Invalid handle
///             eFrameworkunifiedStatusInvldHndlType       = 3,  ///< Invalid handle type
///             eFrameworkunifiedStatusInvldQName        = 4,  ///< Invalid messasge queue name
///             eFrameworkunifiedStatusMsgQFull            = 5,  ///< Message queue full
///             eFrameworkunifiedStatusInvldNotification   = 6,///< The Notification event not present
///             eFrameworkunifiedStatusInvldParam        = 7,  ///< Invalid parameter
///             eFrameworkunifiedStatusInvldBufSize        = 8,  ///< Buf size too small
///             eFrameworkunifiedStatusInvldID             = 9,  ///< Unrecognized ID
///             eFrameworkunifiedStatusCannotRelease       = 10, ///< Cannot release resource
///             eFrameworkunifiedStatusBadConnection       = 11, ///< Could not locate resource
///             eFrameworkunifiedStatusExit            = 12,  ///< Normal application termination
///             eFrameworkunifiedStatusNotImplemented      = 13,  ///< incomplete feature
///             eFrameworkunifiedStatusThreadBusy          = 14,  ///< Joined thread is already being joined
///             eFrameworkunifiedStatusThreadSelfJoin      = 15,  ///< Thread is joining itself
///             eFrameworkunifiedStatusThreadInvalidVal    = 16, ///< Invalid value passed
///             eFrameworkunifiedStatusThreadNotExist      = 17,  ///< The thread does not exist
///             eFrameworkunifiedStatusFault               = 18,  ///< A fault occurred while attempting to make call
///             eFrameworkunifiedStatusServNotFound        = 19,  ///< Service not present in serv dir
///             eFrameworkunifiedStatusServerInUse         = 20,  ///< Service already processing 1 client request
///             eFrameworkunifiedStatusDbIndexing        = 21,  ///< Database Indexing in progress
///             eFrameworkunifiedStatusNullPointer       = 22,
///             eFrameworkunifiedStatusMsgNotProcessed     = 23,
///             eFrameworkunifiedStatusFileLoadSuccess     = 24, ///< File Load Success
///             eFrameworkunifiedStatusFileLoadError       = 25, ///< File Load Error
///             eFrameworkunifiedStatusAccessError         = 26, ///< Error when accessing resource
///             eFrameworkunifiedStatusDuplicate           = 27, ///< Duplicate entry
///             eFrameworkunifiedStatusMsgQEmpty             = 28, ///< Message queue empty
///             eFrameworkunifiedStatusThreadAlreadyRunning  = 29,
///             eFrameworkunifiedStatusErrNoEBADF            = 30,  ///< Bad file descriptor
///             eFrameworkunifiedStatusErrNoEAGAIN           = 31,  ///< Resource unavailable, try again
///             eFrameworkunifiedStatusErrNoEINTR            = 32,  ///< Interrupted system call
///             eFrameworkunifiedStatusSessionErr      = 33,  ///< Error in session handling
///             eFrameworkunifiedStatusDBCorrupt       = 34,  ///< Database corrupt
///             eFrameworkunifiedStatusDBFileNotFound    = 35  ///< Database file not found
///           } EFrameworkunifiedStatus, *PEFrameworkunifiedStatus;
///            @endcode
/// \~english @par Preconditons
///       none
/// \~english @par Change of internal status
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - Function pointer specified by the argument is NULL.
///       - It is an incorrect value(hNPMsgQ) or hNPMsgQ is NULL.
///       - It is an incorrect value(pPublisherName) or pPublisherName is NULL.
///       - It is an incorrect value(pDstFilePath) or pDstFilePath is NULL.
///       - It is an incorrect value(pTag) or pTag is NULL.
///       - It is an incorrect value(hUser) or hUser is NULL.
/// \~english @par Detail
///       Invoke callback function to send message to Notification Service to copy file from persistent memory to
///       specified path. \n
///       If callback function is null and expectedCalls is '0', using default function 'NPLoadPersistentFile' to send
///       message to Notification Service to copy file from persistent memory to specified path.
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync only
/// \~english @see
///       FrameworkunifiedNPLoadPersistentFile
EFrameworkunifiedStatus NPLoadPersistentFile(HANDLE hNPMsgQ, PCSTR pPublisherName, PCSTR pDstFilePath,
                                PCSTR pTag, HANDLE hUser);

//////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NPReleasePersistentFile
/// \~english @par Brief
///        API to send message to Notification Service to notify that the file can be persisted.
/// \~english @param[in] hNPMsgQ
///        HANDLE - Handle to message queue of Notification service.
/// \~english @param[in] pPublisherName
///        PCSTR - Name of publisher message queue.
/// \~english @param[in]  eReleaseType
///        EFrameworkunifiedReleaseType - eFrameworkunifiedNotOnRelease:not on release
///                          eFrameworkunifiedPersistOnShutdown:persist on shutdown
///                          eFrameworkunifiedPersistInstantly:persist instantly
/// \~english @param[in] pTag
///        PCSTR - Tag associated with the file released
/// \~english @param[in] pFullFilePath
///        PCSTR - Full path name of the file to be persisted
/// \~english @param[in] hUser
///        HANDLE - Release the file for the specified user.
/// \~english @retval EFrameworkunifiedStatus indicates success or error
/// \~english @retval Success(eFrameworkunifiedStatusOK)
/// \~english @retval Error(anything else)
/// \~english @par
///        EFrameworkunifiedStatus Enum
/// \~english @code
///           typedef enum eFrameworkunifiedStatus {
///             eFrameworkunifiedStatusEmptyMediaList    = -10, ///< Empty media list
///             eFrameworkunifiedStatusSessionLimitMaxedOut  = -9,  ///< Maximum session limit reached
///             eFrameworkunifiedStatusDbRecNotFound       = -8, ///< Database record not found
///             eFrameworkunifiedStatusDbResultError       = -7, ///< Database result error
///             eFrameworkunifiedStatusDbExecuteFail       = -6, ///< Database execute fail
///             eFrameworkunifiedStatusSemCloseFail        = -5, ///< Semaphore close failed
///             eFrameworkunifiedStatusSemUnlockFail       = -4, ///< Semaphore unlock failed
///             eFrameworkunifiedStatusSemLockFail         = -3, ///< Semaphore lock failed
///             eFrameworkunifiedStatusFail                = -2, ///< Failed
///             eFrameworkunifiedStatusErrOther            = -1, ///< Unknown error
///             eFrameworkunifiedStatusOK                  = 0,  ///< Success / Pass / OK
///             eFrameworkunifiedStatusInvldBuf            = 1,  ///< Invalid buffer
///             eFrameworkunifiedStatusInvldHandle         = 2,  ///< Invalid handle
///             eFrameworkunifiedStatusInvldHndlType       = 3,  ///< Invalid handle type
///             eFrameworkunifiedStatusInvldQName        = 4,  ///< Invalid messasge queue name
///             eFrameworkunifiedStatusMsgQFull            = 5,  ///< Message queue full
///             eFrameworkunifiedStatusInvldNotification   = 6,///< The Notification event not present
///             eFrameworkunifiedStatusInvldParam        = 7,  ///< Invalid parameter
///             eFrameworkunifiedStatusInvldBufSize        = 8,  ///< Buf size too small
///             eFrameworkunifiedStatusInvldID             = 9,  ///< Unrecognized ID
///             eFrameworkunifiedStatusCannotRelease       = 10, ///< Cannot release resource
///             eFrameworkunifiedStatusBadConnection       = 11, ///< Could not locate resource
///             eFrameworkunifiedStatusExit            = 12,  ///< Normal application termination
///             eFrameworkunifiedStatusNotImplemented      = 13,  ///< incomplete feature
///             eFrameworkunifiedStatusThreadBusy          = 14,  ///< Joined thread is already being joined
///             eFrameworkunifiedStatusThreadSelfJoin      = 15,  ///< Thread is joining itself
///             eFrameworkunifiedStatusThreadInvalidVal    = 16, ///< Invalid value passed
///             eFrameworkunifiedStatusThreadNotExist      = 17,  ///< The thread does not exist
///             eFrameworkunifiedStatusFault               = 18,  ///< A fault occurred while attempting to make call
///             eFrameworkunifiedStatusServNotFound        = 19,  ///< Service not present in serv dir
///             eFrameworkunifiedStatusServerInUse         = 20,  ///< Service already processing 1 client request
///             eFrameworkunifiedStatusDbIndexing        = 21,  ///< Database Indexing in progress
///             eFrameworkunifiedStatusNullPointer       = 22,
///             eFrameworkunifiedStatusMsgNotProcessed     = 23,
///             eFrameworkunifiedStatusFileLoadSuccess     = 24, ///< File Load Success
///             eFrameworkunifiedStatusFileLoadError       = 25, ///< File Load Error
///             eFrameworkunifiedStatusAccessError         = 26, ///< Error when accessing resource
///             eFrameworkunifiedStatusDuplicate           = 27, ///< Duplicate entry
///             eFrameworkunifiedStatusMsgQEmpty             = 28, ///< Message queue empty
///             eFrameworkunifiedStatusThreadAlreadyRunning  = 29,
///             eFrameworkunifiedStatusErrNoEBADF            = 30,  ///< Bad file descriptor
///             eFrameworkunifiedStatusErrNoEAGAIN           = 31,  ///< Resource unavailable, try again
///             eFrameworkunifiedStatusErrNoEINTR            = 32,  ///< Interrupted system call
///             eFrameworkunifiedStatusSessionErr      = 33,  ///< Error in session handling
///             eFrameworkunifiedStatusDBCorrupt       = 34,  ///< Database corrupt
///             eFrameworkunifiedStatusDBFileNotFound    = 35  ///< Database file not found
///           } EFrameworkunifiedStatus, *PEFrameworkunifiedStatus;
///            @endcode
/// \~english @par Preconditons
///       none
/// \~english @par Change of internal status
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - Function pointer specified by the argument is NULL.
///       - It is an incorrect value(hNPMsgQ) or hNPMsgQ is NULL.
///       - It is an incorrect value(pPublisherName) or pPublisherName is NULL.
///       - It is an incorrect value(pDstFilePath) or pDstFilePath is NULL.
///       - It is an incorrect value(pTag) or pTag is NULL.
///       - It is an incorrect value(pFullFilePath) or pFullFilePath is NULL.
///       - It is an incorrect value(hUser) or hUser is NULL.
/// \~english @par Detail
///       Invoke callback function to send message to Notification Service to notify that the file can be persisted. \n
///       If callback function is null and expectedCalls is '0', using default function 'NPReleasePersistentFile' to
///       send message to Notification Service to notify that the file can be persisted.
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync only
/// \~english @see
///       FrameworkunifiedNPReleasePersistentFile
EFrameworkunifiedStatus NPReleasePersistentFile(HANDLE hNPMsgQ, PCSTR pPublisherName, EFrameworkunifiedReleaseType eReleaseType, PCSTR pTag,
                                   PCSTR pFullFilePath, HANDLE hUser);

//////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NPPersistentSync
/// \~english @par Brief
///        API to synchronizes by NPPService.
/// \~english @param[in] hNPMsgQ
///        HANDLE - Handle to message queue of Notification service.
/// \~english @param[in] pPublisherName
///        PCSTR - Name of publisher message queue.
/// \~english @param[in]  eReleaseType
///        EFrameworkunifiedReleaseType - eFrameworkunifiedNotOnRelease:not on release
///                          eFrameworkunifiedPersistOnShutdown:persist on shutdown
///                          eFrameworkunifiedPersistInstantly:persist instantly
/// \~english @param[in] SrcName
///        PCSTR - source service name.
/// \~english @param[in] sessionid
///        UI_32 - session ID
/// \~english @retval EFrameworkunifiedStatus indicates success or error
/// \~english @retval Success(eFrameworkunifiedStatusOK)
/// \~english @retval Error(anything else)
/// \~english @par
///        EFrameworkunifiedStatus Enum
/// \~english @code
///           typedef enum eFrameworkunifiedStatus {
///             eFrameworkunifiedStatusEmptyMediaList    = -10, ///< Empty media list
///             eFrameworkunifiedStatusSessionLimitMaxedOut  = -9,  ///< Maximum session limit reached
///             eFrameworkunifiedStatusDbRecNotFound       = -8, ///< Database record not found
///             eFrameworkunifiedStatusDbResultError       = -7, ///< Database result error
///             eFrameworkunifiedStatusDbExecuteFail       = -6, ///< Database execute fail
///             eFrameworkunifiedStatusSemCloseFail        = -5, ///< Semaphore close failed
///             eFrameworkunifiedStatusSemUnlockFail       = -4, ///< Semaphore unlock failed
///             eFrameworkunifiedStatusSemLockFail         = -3, ///< Semaphore lock failed
///             eFrameworkunifiedStatusFail                = -2, ///< Failed
///             eFrameworkunifiedStatusErrOther            = -1, ///< Unknown error
///             eFrameworkunifiedStatusOK                  = 0,  ///< Success / Pass / OK
///             eFrameworkunifiedStatusInvldBuf            = 1,  ///< Invalid buffer
///             eFrameworkunifiedStatusInvldHandle         = 2,  ///< Invalid handle
///             eFrameworkunifiedStatusInvldHndlType       = 3,  ///< Invalid handle type
///             eFrameworkunifiedStatusInvldQName        = 4,  ///< Invalid messasge queue name
///             eFrameworkunifiedStatusMsgQFull            = 5,  ///< Message queue full
///             eFrameworkunifiedStatusInvldNotification   = 6,///< The Notification event not present
///             eFrameworkunifiedStatusInvldParam        = 7,  ///< Invalid parameter
///             eFrameworkunifiedStatusInvldBufSize        = 8,  ///< Buf size too small
///             eFrameworkunifiedStatusInvldID             = 9,  ///< Unrecognized ID
///             eFrameworkunifiedStatusCannotRelease       = 10, ///< Cannot release resource
///             eFrameworkunifiedStatusBadConnection       = 11, ///< Could not locate resource
///             eFrameworkunifiedStatusExit            = 12,  ///< Normal application termination
///             eFrameworkunifiedStatusNotImplemented      = 13,  ///< incomplete feature
///             eFrameworkunifiedStatusThreadBusy          = 14,  ///< Joined thread is already being joined
///             eFrameworkunifiedStatusThreadSelfJoin      = 15,  ///< Thread is joining itself
///             eFrameworkunifiedStatusThreadInvalidVal    = 16, ///< Invalid value passed
///             eFrameworkunifiedStatusThreadNotExist      = 17,  ///< The thread does not exist
///             eFrameworkunifiedStatusFault               = 18,  ///< A fault occurred while attempting to make call
///             eFrameworkunifiedStatusServNotFound        = 19,  ///< Service not present in serv dir
///             eFrameworkunifiedStatusServerInUse         = 20,  ///< Service already processing 1 client request
///             eFrameworkunifiedStatusDbIndexing        = 21,  ///< Database Indexing in progress
///             eFrameworkunifiedStatusNullPointer       = 22,
///             eFrameworkunifiedStatusMsgNotProcessed     = 23,
///             eFrameworkunifiedStatusFileLoadSuccess     = 24, ///< File Load Success
///             eFrameworkunifiedStatusFileLoadError       = 25, ///< File Load Error
///             eFrameworkunifiedStatusAccessError         = 26, ///< Error when accessing resource
///             eFrameworkunifiedStatusDuplicate           = 27, ///< Duplicate entry
///             eFrameworkunifiedStatusMsgQEmpty             = 28, ///< Message queue empty
///             eFrameworkunifiedStatusThreadAlreadyRunning  = 29,
///             eFrameworkunifiedStatusErrNoEBADF            = 30,  ///< Bad file descriptor
///             eFrameworkunifiedStatusErrNoEAGAIN           = 31,  ///< Resource unavailable, try again
///             eFrameworkunifiedStatusErrNoEINTR            = 32,  ///< Interrupted system call
///             eFrameworkunifiedStatusSessionErr      = 33,  ///< Error in session handling
///             eFrameworkunifiedStatusDBCorrupt       = 34,  ///< Database corrupt
///             eFrameworkunifiedStatusDBFileNotFound    = 35  ///< Database file not found
///           } EFrameworkunifiedStatus, *PEFrameworkunifiedStatus;
///            @endcode
/// \~english @par Preconditons
///       none
/// \~english @par Change of internal status
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - Function pointer specified by the argument is NULL.
///       - It is an incorrect value(SrcName) or SrcName is NULL.
///       - It is an incorrect value(hNPMsgQ) or hNPMsgQ is NULL.
///       - It is an incorrect value(sessionid).
///       - It is an incorrect value(pPublisherName) or pPublisherName is NULL.
/// \~english @par Detail
///       Invoke callback function to synchronizes by NPPService. \n
///       If callback function is null and expectedCalls is '0', using default function 'NPPersistentSync' to
///       synchronizes by NPPService.
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync only
/// \~english @see
///       FrameworkunifiedNPPersistentSync
EFrameworkunifiedStatus NPPersistentSync(PCSTR SrcName, HANDLE hNPMsgQ, UI_32 sessionid, PCSTR pPublisherName);

//////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NPSetPersonality
/// \~english @par Brief
///        API to send message to Notification Service to set the Personality.
/// \~english @param[in] hNPMsgQ
///        HANDLE - Handle to message queue of Notification service.
/// \~english @param[in] pPublisherName
///        PCSTR - Name of publisher message queue.
/// \~english @param[in] pUserName
///        PCSTR - Name of new Personality
/// \~english @retval EFrameworkunifiedStatus indicates success or error
/// \~english @retval Success(eFrameworkunifiedStatusOK)
/// \~english @retval Error(anything else)
/// \~english @par
///        EFrameworkunifiedStatus Enum
/// \~english @code
///           typedef enum eFrameworkunifiedStatus {
///             eFrameworkunifiedStatusEmptyMediaList    = -10, ///< Empty media list
///             eFrameworkunifiedStatusSessionLimitMaxedOut  = -9,  ///< Maximum session limit reached
///             eFrameworkunifiedStatusDbRecNotFound       = -8, ///< Database record not found
///             eFrameworkunifiedStatusDbResultError       = -7, ///< Database result error
///             eFrameworkunifiedStatusDbExecuteFail       = -6, ///< Database execute fail
///             eFrameworkunifiedStatusSemCloseFail        = -5, ///< Semaphore close failed
///             eFrameworkunifiedStatusSemUnlockFail       = -4, ///< Semaphore unlock failed
///             eFrameworkunifiedStatusSemLockFail         = -3, ///< Semaphore lock failed
///             eFrameworkunifiedStatusFail                = -2, ///< Failed
///             eFrameworkunifiedStatusErrOther            = -1, ///< Unknown error
///             eFrameworkunifiedStatusOK                  = 0,  ///< Success / Pass / OK
///             eFrameworkunifiedStatusInvldBuf            = 1,  ///< Invalid buffer
///             eFrameworkunifiedStatusInvldHandle         = 2,  ///< Invalid handle
///             eFrameworkunifiedStatusInvldHndlType       = 3,  ///< Invalid handle type
///             eFrameworkunifiedStatusInvldQName        = 4,  ///< Invalid messasge queue name
///             eFrameworkunifiedStatusMsgQFull            = 5,  ///< Message queue full
///             eFrameworkunifiedStatusInvldNotification   = 6,///< The Notification event not present
///             eFrameworkunifiedStatusInvldParam        = 7,  ///< Invalid parameter
///             eFrameworkunifiedStatusInvldBufSize        = 8,  ///< Buf size too small
///             eFrameworkunifiedStatusInvldID             = 9,  ///< Unrecognized ID
///             eFrameworkunifiedStatusCannotRelease       = 10, ///< Cannot release resource
///             eFrameworkunifiedStatusBadConnection       = 11, ///< Could not locate resource
///             eFrameworkunifiedStatusExit            = 12,  ///< Normal application termination
///             eFrameworkunifiedStatusNotImplemented      = 13,  ///< incomplete feature
///             eFrameworkunifiedStatusThreadBusy          = 14,  ///< Joined thread is already being joined
///             eFrameworkunifiedStatusThreadSelfJoin      = 15,  ///< Thread is joining itself
///             eFrameworkunifiedStatusThreadInvalidVal    = 16, ///< Invalid value passed
///             eFrameworkunifiedStatusThreadNotExist      = 17,  ///< The thread does not exist
///             eFrameworkunifiedStatusFault               = 18,  ///< A fault occurred while attempting to make call
///             eFrameworkunifiedStatusServNotFound        = 19,  ///< Service not present in serv dir
///             eFrameworkunifiedStatusServerInUse         = 20,  ///< Service already processing 1 client request
///             eFrameworkunifiedStatusDbIndexing        = 21,  ///< Database Indexing in progress
///             eFrameworkunifiedStatusNullPointer       = 22,
///             eFrameworkunifiedStatusMsgNotProcessed     = 23,
///             eFrameworkunifiedStatusFileLoadSuccess     = 24, ///< File Load Success
///             eFrameworkunifiedStatusFileLoadError       = 25, ///< File Load Error
///             eFrameworkunifiedStatusAccessError         = 26, ///< Error when accessing resource
///             eFrameworkunifiedStatusDuplicate           = 27, ///< Duplicate entry
///             eFrameworkunifiedStatusMsgQEmpty             = 28, ///< Message queue empty
///             eFrameworkunifiedStatusThreadAlreadyRunning  = 29,
///             eFrameworkunifiedStatusErrNoEBADF            = 30,  ///< Bad file descriptor
///             eFrameworkunifiedStatusErrNoEAGAIN           = 31,  ///< Resource unavailable, try again
///             eFrameworkunifiedStatusErrNoEINTR            = 32,  ///< Interrupted system call
///             eFrameworkunifiedStatusSessionErr      = 33,  ///< Error in session handling
///             eFrameworkunifiedStatusDBCorrupt       = 34,  ///< Database corrupt
///             eFrameworkunifiedStatusDBFileNotFound    = 35  ///< Database file not found
///           } EFrameworkunifiedStatus, *PEFrameworkunifiedStatus;
///            @endcode
/// \~english @par Preconditons
///       none
/// \~english @par Change of internal status
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - Function pointer specified by the argument is NULL.
///       - It is an incorrect value(hNPMsgQ) or hNPMsgQ is NULL.
///       - It is an incorrect value(pPublisherName) or pPublisherName is NULL.
///       - It is an incorrect value(pUserName) or pUserName is NULL.
/// \~english @par Detail
///       Invoke callback function to send message to Notification Service to set the Personality. \n
///       If callback function is null and expectedCalls is '0', using default function 'NPSetPersonality' to send
///       message to Notification Service to set the Personality.
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync only
/// \~english @see
///       FrameworkunifiedNPSetPersonality
EFrameworkunifiedStatus NPSetPersonality(HANDLE hNPMsgQ, PCSTR pPublisherName, PCSTR pUserName);

//////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NPChangePersonality
/// \~english @par Brief
///        API to send message to Notification Service to change the current Personality.
/// \~english @param[in] hNPMsgQ
///        HANDLE - Handle to message queue of Notification service.
/// \~english @param[in] pPublisherName
///        PCSTR - Name of publisher message queue.
/// \~english @param[in] pUserName
///        PCSTR - Name of new Personality
/// \~english @retval EFrameworkunifiedStatus indicates success or error
/// \~english @retval Success(eFrameworkunifiedStatusOK)
/// \~english @retval Error(anything else)
/// \~english @par
///        EFrameworkunifiedStatus Enum
/// \~english @code
///           typedef enum eFrameworkunifiedStatus {
///             eFrameworkunifiedStatusEmptyMediaList    = -10, ///< Empty media list
///             eFrameworkunifiedStatusSessionLimitMaxedOut  = -9,  ///< Maximum session limit reached
///             eFrameworkunifiedStatusDbRecNotFound       = -8, ///< Database record not found
///             eFrameworkunifiedStatusDbResultError       = -7, ///< Database result error
///             eFrameworkunifiedStatusDbExecuteFail       = -6, ///< Database execute fail
///             eFrameworkunifiedStatusSemCloseFail        = -5, ///< Semaphore close failed
///             eFrameworkunifiedStatusSemUnlockFail       = -4, ///< Semaphore unlock failed
///             eFrameworkunifiedStatusSemLockFail         = -3, ///< Semaphore lock failed
///             eFrameworkunifiedStatusFail                = -2, ///< Failed
///             eFrameworkunifiedStatusErrOther            = -1, ///< Unknown error
///             eFrameworkunifiedStatusOK                  = 0,  ///< Success / Pass / OK
///             eFrameworkunifiedStatusInvldBuf            = 1,  ///< Invalid buffer
///             eFrameworkunifiedStatusInvldHandle         = 2,  ///< Invalid handle
///             eFrameworkunifiedStatusInvldHndlType       = 3,  ///< Invalid handle type
///             eFrameworkunifiedStatusInvldQName        = 4,  ///< Invalid messasge queue name
///             eFrameworkunifiedStatusMsgQFull            = 5,  ///< Message queue full
///             eFrameworkunifiedStatusInvldNotification   = 6,///< The Notification event not present
///             eFrameworkunifiedStatusInvldParam        = 7,  ///< Invalid parameter
///             eFrameworkunifiedStatusInvldBufSize        = 8,  ///< Buf size too small
///             eFrameworkunifiedStatusInvldID             = 9,  ///< Unrecognized ID
///             eFrameworkunifiedStatusCannotRelease       = 10, ///< Cannot release resource
///             eFrameworkunifiedStatusBadConnection       = 11, ///< Could not locate resource
///             eFrameworkunifiedStatusExit            = 12,  ///< Normal application termination
///             eFrameworkunifiedStatusNotImplemented      = 13,  ///< incomplete feature
///             eFrameworkunifiedStatusThreadBusy          = 14,  ///< Joined thread is already being joined
///             eFrameworkunifiedStatusThreadSelfJoin      = 15,  ///< Thread is joining itself
///             eFrameworkunifiedStatusThreadInvalidVal    = 16, ///< Invalid value passed
///             eFrameworkunifiedStatusThreadNotExist      = 17,  ///< The thread does not exist
///             eFrameworkunifiedStatusFault               = 18,  ///< A fault occurred while attempting to make call
///             eFrameworkunifiedStatusServNotFound        = 19,  ///< Service not present in serv dir
///             eFrameworkunifiedStatusServerInUse         = 20,  ///< Service already processing 1 client request
///             eFrameworkunifiedStatusDbIndexing        = 21,  ///< Database Indexing in progress
///             eFrameworkunifiedStatusNullPointer       = 22,
///             eFrameworkunifiedStatusMsgNotProcessed     = 23,
///             eFrameworkunifiedStatusFileLoadSuccess     = 24, ///< File Load Success
///             eFrameworkunifiedStatusFileLoadError       = 25, ///< File Load Error
///             eFrameworkunifiedStatusAccessError         = 26, ///< Error when accessing resource
///             eFrameworkunifiedStatusDuplicate           = 27, ///< Duplicate entry
///             eFrameworkunifiedStatusMsgQEmpty             = 28, ///< Message queue empty
///             eFrameworkunifiedStatusThreadAlreadyRunning  = 29,
///             eFrameworkunifiedStatusErrNoEBADF            = 30,  ///< Bad file descriptor
///             eFrameworkunifiedStatusErrNoEAGAIN           = 31,  ///< Resource unavailable, try again
///             eFrameworkunifiedStatusErrNoEINTR            = 32,  ///< Interrupted system call
///             eFrameworkunifiedStatusSessionErr      = 33,  ///< Error in session handling
///             eFrameworkunifiedStatusDBCorrupt       = 34,  ///< Database corrupt
///             eFrameworkunifiedStatusDBFileNotFound    = 35  ///< Database file not found
///           } EFrameworkunifiedStatus, *PEFrameworkunifiedStatus;
///            @endcode
/// \~english @par Preconditons
///       none
/// \~english @par Change of internal status
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - Function pointer specified by the argument is NULL.
///       - It is an incorrect value(hNPMsgQ) or hNPMsgQ is NULL.
///       - It is an incorrect value(pPublisherName) or pPublisherName is NULL.
///       - It is an incorrect value(pUserName) or pUserName is NULL.
/// \~english @par Detail
///       Invoke callback function to send message to Notification Service to change the current Personality. \n
///       If callback function is null and expectedCalls is '0', using default function 'NPChangePersonality' to send
///       message to Notification Service to change the current Personality.
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync only
/// \~english @see
///       FrameworkunifiedNPChangePersonality
EFrameworkunifiedStatus NPChangePersonality(HANDLE hNPMsgQ, PCSTR pPublisherName, PCSTR pUserName);

//////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NPGetReadyStatusOfNPP
/// \~english @par Brief
///        To be used by System Manager Service only. SystemManager will use this API to get the ready status of
///        NPPService.
/// \~english @param[in] hNPMsgQ
///        HANDLE - Handle to message queue of Notification service.
/// \~english @param[in] pRequesterName
///        PCSTR - Name of the requesters message queue.
/// \~english @param[in] pUserName
///        PCSTR - Name of new Personality
/// \~english @retval EFrameworkunifiedStatus indicates success or error
/// \~english @retval Success(eFrameworkunifiedStatusOK)
/// \~english @retval Error(anything else)
/// \~english @par
///        EFrameworkunifiedStatus Enum
/// \~english @code
///           typedef enum eFrameworkunifiedStatus {
///             eFrameworkunifiedStatusEmptyMediaList    = -10, ///< Empty media list
///             eFrameworkunifiedStatusSessionLimitMaxedOut  = -9,  ///< Maximum session limit reached
///             eFrameworkunifiedStatusDbRecNotFound       = -8, ///< Database record not found
///             eFrameworkunifiedStatusDbResultError       = -7, ///< Database result error
///             eFrameworkunifiedStatusDbExecuteFail       = -6, ///< Database execute fail
///             eFrameworkunifiedStatusSemCloseFail        = -5, ///< Semaphore close failed
///             eFrameworkunifiedStatusSemUnlockFail       = -4, ///< Semaphore unlock failed
///             eFrameworkunifiedStatusSemLockFail         = -3, ///< Semaphore lock failed
///             eFrameworkunifiedStatusFail                = -2, ///< Failed
///             eFrameworkunifiedStatusErrOther            = -1, ///< Unknown error
///             eFrameworkunifiedStatusOK                  = 0,  ///< Success / Pass / OK
///             eFrameworkunifiedStatusInvldBuf            = 1,  ///< Invalid buffer
///             eFrameworkunifiedStatusInvldHandle         = 2,  ///< Invalid handle
///             eFrameworkunifiedStatusInvldHndlType       = 3,  ///< Invalid handle type
///             eFrameworkunifiedStatusInvldQName        = 4,  ///< Invalid messasge queue name
///             eFrameworkunifiedStatusMsgQFull            = 5,  ///< Message queue full
///             eFrameworkunifiedStatusInvldNotification   = 6,///< The Notification event not present
///             eFrameworkunifiedStatusInvldParam        = 7,  ///< Invalid parameter
///             eFrameworkunifiedStatusInvldBufSize        = 8,  ///< Buf size too small
///             eFrameworkunifiedStatusInvldID             = 9,  ///< Unrecognized ID
///             eFrameworkunifiedStatusCannotRelease       = 10, ///< Cannot release resource
///             eFrameworkunifiedStatusBadConnection       = 11, ///< Could not locate resource
///             eFrameworkunifiedStatusExit            = 12,  ///< Normal application termination
///             eFrameworkunifiedStatusNotImplemented      = 13,  ///< incomplete feature
///             eFrameworkunifiedStatusThreadBusy          = 14,  ///< Joined thread is already being joined
///             eFrameworkunifiedStatusThreadSelfJoin      = 15,  ///< Thread is joining itself
///             eFrameworkunifiedStatusThreadInvalidVal    = 16, ///< Invalid value passed
///             eFrameworkunifiedStatusThreadNotExist      = 17,  ///< The thread does not exist
///             eFrameworkunifiedStatusFault               = 18,  ///< A fault occurred while attempting to make call
///             eFrameworkunifiedStatusServNotFound        = 19,  ///< Service not present in serv dir
///             eFrameworkunifiedStatusServerInUse         = 20,  ///< Service already processing 1 client request
///             eFrameworkunifiedStatusDbIndexing        = 21,  ///< Database Indexing in progress
///             eFrameworkunifiedStatusNullPointer       = 22,
///             eFrameworkunifiedStatusMsgNotProcessed     = 23,
///             eFrameworkunifiedStatusFileLoadSuccess     = 24, ///< File Load Success
///             eFrameworkunifiedStatusFileLoadError       = 25, ///< File Load Error
///             eFrameworkunifiedStatusAccessError         = 26, ///< Error when accessing resource
///             eFrameworkunifiedStatusDuplicate           = 27, ///< Duplicate entry
///             eFrameworkunifiedStatusMsgQEmpty             = 28, ///< Message queue empty
///             eFrameworkunifiedStatusThreadAlreadyRunning  = 29,
///             eFrameworkunifiedStatusErrNoEBADF            = 30,  ///< Bad file descriptor
///             eFrameworkunifiedStatusErrNoEAGAIN           = 31,  ///< Resource unavailable, try again
///             eFrameworkunifiedStatusErrNoEINTR            = 32,  ///< Interrupted system call
///             eFrameworkunifiedStatusSessionErr      = 33,  ///< Error in session handling
///             eFrameworkunifiedStatusDBCorrupt       = 34,  ///< Database corrupt
///             eFrameworkunifiedStatusDBFileNotFound    = 35  ///< Database file not found
///           } EFrameworkunifiedStatus, *PEFrameworkunifiedStatus;
///            @endcode
/// \~english @par Preconditons
///       none
/// \~english @par Change of internal status
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - Function pointer specified by the argument is NULL.
///       - It is an incorrect value(hNPMsgQ) or hNPMsgQ is NULL.
///       - It is an incorrect value(pRequesterName) or pRequesterName is NULL.
///       - It is an incorrect value(pUserName) or pUserName is NULL.
/// \~english @par Detail
///       Invoke callback function to get the ready status of NPPService. \n
///       If callback function is null and expectedCalls is '0', using default function 'NPGetReadyStatusOfNPP' to get
///       the ready status of NPPService.
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync only
/// \~english @see
///       FrameworkunifiedNPGetReadyStatusOfNPP
EFrameworkunifiedStatus NPGetReadyStatusOfNPP(HANDLE hNPMsgQ, PCSTR pRequesterName);

//////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NPClearPersistedData
/// \~english @par Brief
///        API to delete the persisted data of NS_NPS.
///        Note: Currently to be used by HMI service only
/// \~english @param[in] hNPMsgQ
///        HANDLE - Handle to message queue of Notification service.
/// \~english @param[in] pRequesterName
///        PCSTR - Name of the requesters message queue.
/// \~english @param[in] eFrameworkunifiedClearPersistenceScope
///        EFrameworkunifiedClearPersistence - specifies what data to delete from persistent memory.
/// \~english @retval EFrameworkunifiedStatus indicates success or error
/// \~english @retval Success(eFrameworkunifiedStatusOK)
/// \~english @retval Error(anything else)
/// \~english @par
///        EFrameworkunifiedStatus Enum
/// \~english @code
///           typedef enum eFrameworkunifiedStatus {
///             eFrameworkunifiedStatusEmptyMediaList    = -10, ///< Empty media list
///             eFrameworkunifiedStatusSessionLimitMaxedOut  = -9,  ///< Maximum session limit reached
///             eFrameworkunifiedStatusDbRecNotFound       = -8, ///< Database record not found
///             eFrameworkunifiedStatusDbResultError       = -7, ///< Database result error
///             eFrameworkunifiedStatusDbExecuteFail       = -6, ///< Database execute fail
///             eFrameworkunifiedStatusSemCloseFail        = -5, ///< Semaphore close failed
///             eFrameworkunifiedStatusSemUnlockFail       = -4, ///< Semaphore unlock failed
///             eFrameworkunifiedStatusSemLockFail         = -3, ///< Semaphore lock failed
///             eFrameworkunifiedStatusFail                = -2, ///< Failed
///             eFrameworkunifiedStatusErrOther            = -1, ///< Unknown error
///             eFrameworkunifiedStatusOK                  = 0,  ///< Success / Pass / OK
///             eFrameworkunifiedStatusInvldBuf            = 1,  ///< Invalid buffer
///             eFrameworkunifiedStatusInvldHandle         = 2,  ///< Invalid handle
///             eFrameworkunifiedStatusInvldHndlType       = 3,  ///< Invalid handle type
///             eFrameworkunifiedStatusInvldQName        = 4,  ///< Invalid messasge queue name
///             eFrameworkunifiedStatusMsgQFull            = 5,  ///< Message queue full
///             eFrameworkunifiedStatusInvldNotification   = 6,///< The Notification event not present
///             eFrameworkunifiedStatusInvldParam        = 7,  ///< Invalid parameter
///             eFrameworkunifiedStatusInvldBufSize        = 8,  ///< Buf size too small
///             eFrameworkunifiedStatusInvldID             = 9,  ///< Unrecognized ID
///             eFrameworkunifiedStatusCannotRelease       = 10, ///< Cannot release resource
///             eFrameworkunifiedStatusBadConnection       = 11, ///< Could not locate resource
///             eFrameworkunifiedStatusExit            = 12,  ///< Normal application termination
///             eFrameworkunifiedStatusNotImplemented      = 13,  ///< incomplete feature
///             eFrameworkunifiedStatusThreadBusy          = 14,  ///< Joined thread is already being joined
///             eFrameworkunifiedStatusThreadSelfJoin      = 15,  ///< Thread is joining itself
///             eFrameworkunifiedStatusThreadInvalidVal    = 16, ///< Invalid value passed
///             eFrameworkunifiedStatusThreadNotExist      = 17,  ///< The thread does not exist
///             eFrameworkunifiedStatusFault               = 18,  ///< A fault occurred while attempting to make call
///             eFrameworkunifiedStatusServNotFound        = 19,  ///< Service not present in serv dir
///             eFrameworkunifiedStatusServerInUse         = 20,  ///< Service already processing 1 client request
///             eFrameworkunifiedStatusDbIndexing        = 21,  ///< Database Indexing in progress
///             eFrameworkunifiedStatusNullPointer       = 22,
///             eFrameworkunifiedStatusMsgNotProcessed     = 23,
///             eFrameworkunifiedStatusFileLoadSuccess     = 24, ///< File Load Success
///             eFrameworkunifiedStatusFileLoadError       = 25, ///< File Load Error
///             eFrameworkunifiedStatusAccessError         = 26, ///< Error when accessing resource
///             eFrameworkunifiedStatusDuplicate           = 27, ///< Duplicate entry
///             eFrameworkunifiedStatusMsgQEmpty             = 28, ///< Message queue empty
///             eFrameworkunifiedStatusThreadAlreadyRunning  = 29,
///             eFrameworkunifiedStatusErrNoEBADF            = 30,  ///< Bad file descriptor
///             eFrameworkunifiedStatusErrNoEAGAIN           = 31,  ///< Resource unavailable, try again
///             eFrameworkunifiedStatusErrNoEINTR            = 32,  ///< Interrupted system call
///             eFrameworkunifiedStatusSessionErr      = 33,  ///< Error in session handling
///             eFrameworkunifiedStatusDBCorrupt       = 34,  ///< Database corrupt
///             eFrameworkunifiedStatusDBFileNotFound    = 35  ///< Database file not found
///           } EFrameworkunifiedStatus, *PEFrameworkunifiedStatus;
///            @endcode
/// \~english @par Preconditons
///       none
/// \~english @par Change of internal status
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - Function pointer specified by the argument is NULL.
///       - It is an incorrect value(hNPMsgQ) or hNPMsgQ is NULL.
///       - It is an incorrect value(pRequesterName) or pRequesterName is NULL.
/// \~english @par Detail
///       Invoke callback function to delete the persisted data of NS_NPS.\n
///       If callback function is null and expectedCalls is '0', using default function 'NPClearPersistedData' to delete
///       the persisted data of NS_NPS.
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync only
/// \~english @see
///       FrameworkunifiedNPClearPersistedData
EFrameworkunifiedStatus NPClearPersistedData(HANDLE hNPMsgQ, PCSTR pRequesterName, EFrameworkunifiedClearPersistence eFrameworkunifiedClearPersistenceScope);

//////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NPRegisterPersistentFolder
/// \~english @par Brief
///        API to send message to Notification Service to add a folder path as to be persisted.
/// \~english @param[in] hNPMsgQ
///        HANDLE - Handle to message queue of Notification service.
/// \~english @param[in] pPublisherName
///        PCSTR - Name of publisher message queue.
/// \~english @param[in] pTag
///        PCSTR - A unique identifier for the folder path specified, this is used in retrieve/Load the folder later.
/// \~english @param[in] bIsUserFolder
///        BOOL - Set TRUE, if registering user folder.
/// \~english @retval EFrameworkunifiedStatus indicates success or error
/// \~english @retval Success(eFrameworkunifiedStatusOK)
/// \~english @retval Error(anything else)
/// \~english @par
///        EFrameworkunifiedStatus Enum
/// \~english @code
///           typedef enum eFrameworkunifiedStatus {
///             eFrameworkunifiedStatusEmptyMediaList    = -10, ///< Empty media list
///             eFrameworkunifiedStatusSessionLimitMaxedOut  = -9,  ///< Maximum session limit reached
///             eFrameworkunifiedStatusDbRecNotFound       = -8, ///< Database record not found
///             eFrameworkunifiedStatusDbResultError       = -7, ///< Database result error
///             eFrameworkunifiedStatusDbExecuteFail       = -6, ///< Database execute fail
///             eFrameworkunifiedStatusSemCloseFail        = -5, ///< Semaphore close failed
///             eFrameworkunifiedStatusSemUnlockFail       = -4, ///< Semaphore unlock failed
///             eFrameworkunifiedStatusSemLockFail         = -3, ///< Semaphore lock failed
///             eFrameworkunifiedStatusFail                = -2, ///< Failed
///             eFrameworkunifiedStatusErrOther            = -1, ///< Unknown error
///             eFrameworkunifiedStatusOK                  = 0,  ///< Success / Pass / OK
///             eFrameworkunifiedStatusInvldBuf            = 1,  ///< Invalid buffer
///             eFrameworkunifiedStatusInvldHandle         = 2,  ///< Invalid handle
///             eFrameworkunifiedStatusInvldHndlType       = 3,  ///< Invalid handle type
///             eFrameworkunifiedStatusInvldQName        = 4,  ///< Invalid messasge queue name
///             eFrameworkunifiedStatusMsgQFull            = 5,  ///< Message queue full
///             eFrameworkunifiedStatusInvldNotification   = 6,///< The Notification event not present
///             eFrameworkunifiedStatusInvldParam        = 7,  ///< Invalid parameter
///             eFrameworkunifiedStatusInvldBufSize        = 8,  ///< Buf size too small
///             eFrameworkunifiedStatusInvldID             = 9,  ///< Unrecognized ID
///             eFrameworkunifiedStatusCannotRelease       = 10, ///< Cannot release resource
///             eFrameworkunifiedStatusBadConnection       = 11, ///< Could not locate resource
///             eFrameworkunifiedStatusExit            = 12,  ///< Normal application termination
///             eFrameworkunifiedStatusNotImplemented      = 13,  ///< incomplete feature
///             eFrameworkunifiedStatusThreadBusy          = 14,  ///< Joined thread is already being joined
///             eFrameworkunifiedStatusThreadSelfJoin      = 15,  ///< Thread is joining itself
///             eFrameworkunifiedStatusThreadInvalidVal    = 16, ///< Invalid value passed
///             eFrameworkunifiedStatusThreadNotExist      = 17,  ///< The thread does not exist
///             eFrameworkunifiedStatusFault               = 18,  ///< A fault occurred while attempting to make call
///             eFrameworkunifiedStatusServNotFound        = 19,  ///< Service not present in serv dir
///             eFrameworkunifiedStatusServerInUse         = 20,  ///< Service already processing 1 client request
///             eFrameworkunifiedStatusDbIndexing        = 21,  ///< Database Indexing in progress
///             eFrameworkunifiedStatusNullPointer       = 22,
///             eFrameworkunifiedStatusMsgNotProcessed     = 23,
///             eFrameworkunifiedStatusFileLoadSuccess     = 24, ///< File Load Success
///             eFrameworkunifiedStatusFileLoadError       = 25, ///< File Load Error
///             eFrameworkunifiedStatusAccessError         = 26, ///< Error when accessing resource
///             eFrameworkunifiedStatusDuplicate           = 27, ///< Duplicate entry
///             eFrameworkunifiedStatusMsgQEmpty             = 28, ///< Message queue empty
///             eFrameworkunifiedStatusThreadAlreadyRunning  = 29,
///             eFrameworkunifiedStatusErrNoEBADF            = 30,  ///< Bad file descriptor
///             eFrameworkunifiedStatusErrNoEAGAIN           = 31,  ///< Resource unavailable, try again
///             eFrameworkunifiedStatusErrNoEINTR            = 32,  ///< Interrupted system call
///             eFrameworkunifiedStatusSessionErr      = 33,  ///< Error in session handling
///             eFrameworkunifiedStatusDBCorrupt       = 34,  ///< Database corrupt
///             eFrameworkunifiedStatusDBFileNotFound    = 35  ///< Database file not found
///           } EFrameworkunifiedStatus, *PEFrameworkunifiedStatus;
///            @endcode
/// \~english @par Preconditons
///       none
/// \~english @par Change of internal status
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - Function pointer specified by the argument is NULL.
///       - It is an incorrect value(hNPMsgQ) or hNPMsgQ is NULL.
///       - It is an incorrect value(pPublisherName) or pPublisherName is NULL.
///       - It is an incorrect value(pTag) or pTag is NULL.
/// \~english @par Detail
///       Invoke callback function to send message to Notification Service to add a folder path as to be persisted.\n
///       If callback function is null and expectedCalls is '0', using default function 'NPRegisterPersistentFolder' to
///       send message to Notification Service to add a folder path as to be persisted.
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync only
/// \~english @see
///       FrameworkunifiedNPRegisterPersistentFolder
EFrameworkunifiedStatus NPRegisterPersistentFolder(HANDLE hNPMsgQ, PCSTR pPublisherName, PCSTR pTag, BOOL bIsUserFolder);

//////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NPSetFolderPersistentType
/// \~english @par Brief
///        API to send message to Notification Service to set the persist category of file.
/// \~english @param[in] hNPMsgQ
///        HANDLE - Handle to message queue of Notification service.
/// \~english @param[in] pPublisherName
///        PCSTR - Name of publisher message queue.
/// \~english @param[in] pTag
///        PCSTR - A unique identifier for the file path specified, this is used in retrieve/Load the file later
/// \~english @param[in] ePersistCategory
///        EFrameworkunifiedPersistCategory - persist category
/// \~english @retval EFrameworkunifiedStatus indicates success or error
/// \~english @retval Success(eFrameworkunifiedStatusOK)
/// \~english @retval Error(anything else)
/// \~english @par
///        EFrameworkunifiedStatus Enum
/// \~english @code
///           typedef enum eFrameworkunifiedStatus {
///             eFrameworkunifiedStatusEmptyMediaList    = -10, ///< Empty media list
///             eFrameworkunifiedStatusSessionLimitMaxedOut  = -9,  ///< Maximum session limit reached
///             eFrameworkunifiedStatusDbRecNotFound       = -8, ///< Database record not found
///             eFrameworkunifiedStatusDbResultError       = -7, ///< Database result error
///             eFrameworkunifiedStatusDbExecuteFail       = -6, ///< Database execute fail
///             eFrameworkunifiedStatusSemCloseFail        = -5, ///< Semaphore close failed
///             eFrameworkunifiedStatusSemUnlockFail       = -4, ///< Semaphore unlock failed
///             eFrameworkunifiedStatusSemLockFail         = -3, ///< Semaphore lock failed
///             eFrameworkunifiedStatusFail                = -2, ///< Failed
///             eFrameworkunifiedStatusErrOther            = -1, ///< Unknown error
///             eFrameworkunifiedStatusOK                  = 0,  ///< Success / Pass / OK
///             eFrameworkunifiedStatusInvldBuf            = 1,  ///< Invalid buffer
///             eFrameworkunifiedStatusInvldHandle         = 2,  ///< Invalid handle
///             eFrameworkunifiedStatusInvldHndlType       = 3,  ///< Invalid handle type
///             eFrameworkunifiedStatusInvldQName        = 4,  ///< Invalid messasge queue name
///             eFrameworkunifiedStatusMsgQFull            = 5,  ///< Message queue full
///             eFrameworkunifiedStatusInvldNotification   = 6,///< The Notification event not present
///             eFrameworkunifiedStatusInvldParam        = 7,  ///< Invalid parameter
///             eFrameworkunifiedStatusInvldBufSize        = 8,  ///< Buf size too small
///             eFrameworkunifiedStatusInvldID             = 9,  ///< Unrecognized ID
///             eFrameworkunifiedStatusCannotRelease       = 10, ///< Cannot release resource
///             eFrameworkunifiedStatusBadConnection       = 11, ///< Could not locate resource
///             eFrameworkunifiedStatusExit            = 12,  ///< Normal application termination
///             eFrameworkunifiedStatusNotImplemented      = 13,  ///< incomplete feature
///             eFrameworkunifiedStatusThreadBusy          = 14,  ///< Joined thread is already being joined
///             eFrameworkunifiedStatusThreadSelfJoin      = 15,  ///< Thread is joining itself
///             eFrameworkunifiedStatusThreadInvalidVal    = 16, ///< Invalid value passed
///             eFrameworkunifiedStatusThreadNotExist      = 17,  ///< The thread does not exist
///             eFrameworkunifiedStatusFault               = 18,  ///< A fault occurred while attempting to make call
///             eFrameworkunifiedStatusServNotFound        = 19,  ///< Service not present in serv dir
///             eFrameworkunifiedStatusServerInUse         = 20,  ///< Service already processing 1 client request
///             eFrameworkunifiedStatusDbIndexing        = 21,  ///< Database Indexing in progress
///             eFrameworkunifiedStatusNullPointer       = 22,
///             eFrameworkunifiedStatusMsgNotProcessed     = 23,
///             eFrameworkunifiedStatusFileLoadSuccess     = 24, ///< File Load Success
///             eFrameworkunifiedStatusFileLoadError       = 25, ///< File Load Error
///             eFrameworkunifiedStatusAccessError         = 26, ///< Error when accessing resource
///             eFrameworkunifiedStatusDuplicate           = 27, ///< Duplicate entry
///             eFrameworkunifiedStatusMsgQEmpty             = 28, ///< Message queue empty
///             eFrameworkunifiedStatusThreadAlreadyRunning  = 29,
///             eFrameworkunifiedStatusErrNoEBADF            = 30,  ///< Bad file descriptor
///             eFrameworkunifiedStatusErrNoEAGAIN           = 31,  ///< Resource unavailable, try again
///             eFrameworkunifiedStatusErrNoEINTR            = 32,  ///< Interrupted system call
///             eFrameworkunifiedStatusSessionErr      = 33,  ///< Error in session handling
///             eFrameworkunifiedStatusDBCorrupt       = 34,  ///< Database corrupt
///             eFrameworkunifiedStatusDBFileNotFound    = 35  ///< Database file not found
///           } EFrameworkunifiedStatus, *PEFrameworkunifiedStatus;
///            @endcode
/// \~english @par Preconditons
///       none
/// \~english @par Change of internal status
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - Function pointer specified by the argument is NULL.
///       - It is an incorrect value(hNPMsgQ) or hNPMsgQ is NULL.
///       - It is an incorrect value(pPublisherName) or pPublisherName is NULL.
///       - It is an incorrect value(pTag) or pTag is NULL.
/// \~english @par Detail
///       Invoke callback function to send message to Notification Service to set the persist category of file.\n
///       If callback function is null and expectedCalls is '0', using default function 'NPSetFolderPersistentType' to
///       send message to Notification Service to set the persist category of file.
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync only
/// \~english @see
///       FrameworkunifiedNPSetFolderPersistentType
EFrameworkunifiedStatus NPSetFolderPersistentType(HANDLE hNPMsgQ, PCSTR pPublisherName,
                                     PCSTR pTag, EFrameworkunifiedPersistCategory ePersistCategory);

//////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NPLoadPersistentFolder
/// \~english @par Brief
///        API to send message to Notification Service to copy folder from persistent memory to
///        specified path. The caller recieves an acknowledgement once NPS completes folder copy.
/// \~english @param[in] hNPMsgQ
///        HANDLE - Handle to message queue of Notification service.
/// \~english @param[in] pPublisherName
///        PCSTR - Name of publisher message queue.
/// \~english @param[in] pDstFolderPath
///        PCSTR - Complete folder path to which the folder should be copied from persistent area
/// \~english @param[in] pTag
///        PCSTR - Tag associated with the folder that needs to be copied to pDstFolderPath
/// \~english @param[in] hUser
///        HANDLE - Name of new Personality
/// \~english @retval EFrameworkunifiedStatus indicates success or error
/// \~english @retval Success(eFrameworkunifiedStatusOK)
/// \~english @retval Error(anything else)
/// \~english @par
///        EFrameworkunifiedStatus Enum
/// \~english @code
///           typedef enum eFrameworkunifiedStatus {
///             eFrameworkunifiedStatusEmptyMediaList    = -10, ///< Empty media list
///             eFrameworkunifiedStatusSessionLimitMaxedOut  = -9,  ///< Maximum session limit reached
///             eFrameworkunifiedStatusDbRecNotFound       = -8, ///< Database record not found
///             eFrameworkunifiedStatusDbResultError       = -7, ///< Database result error
///             eFrameworkunifiedStatusDbExecuteFail       = -6, ///< Database execute fail
///             eFrameworkunifiedStatusSemCloseFail        = -5, ///< Semaphore close failed
///             eFrameworkunifiedStatusSemUnlockFail       = -4, ///< Semaphore unlock failed
///             eFrameworkunifiedStatusSemLockFail         = -3, ///< Semaphore lock failed
///             eFrameworkunifiedStatusFail                = -2, ///< Failed
///             eFrameworkunifiedStatusErrOther            = -1, ///< Unknown error
///             eFrameworkunifiedStatusOK                  = 0,  ///< Success / Pass / OK
///             eFrameworkunifiedStatusInvldBuf            = 1,  ///< Invalid buffer
///             eFrameworkunifiedStatusInvldHandle         = 2,  ///< Invalid handle
///             eFrameworkunifiedStatusInvldHndlType       = 3,  ///< Invalid handle type
///             eFrameworkunifiedStatusInvldQName        = 4,  ///< Invalid messasge queue name
///             eFrameworkunifiedStatusMsgQFull            = 5,  ///< Message queue full
///             eFrameworkunifiedStatusInvldNotification   = 6,///< The Notification event not present
///             eFrameworkunifiedStatusInvldParam        = 7,  ///< Invalid parameter
///             eFrameworkunifiedStatusInvldBufSize        = 8,  ///< Buf size too small
///             eFrameworkunifiedStatusInvldID             = 9,  ///< Unrecognized ID
///             eFrameworkunifiedStatusCannotRelease       = 10, ///< Cannot release resource
///             eFrameworkunifiedStatusBadConnection       = 11, ///< Could not locate resource
///             eFrameworkunifiedStatusExit            = 12,  ///< Normal application termination
///             eFrameworkunifiedStatusNotImplemented      = 13,  ///< incomplete feature
///             eFrameworkunifiedStatusThreadBusy          = 14,  ///< Joined thread is already being joined
///             eFrameworkunifiedStatusThreadSelfJoin      = 15,  ///< Thread is joining itself
///             eFrameworkunifiedStatusThreadInvalidVal    = 16, ///< Invalid value passed
///             eFrameworkunifiedStatusThreadNotExist      = 17,  ///< The thread does not exist
///             eFrameworkunifiedStatusFault               = 18,  ///< A fault occurred while attempting to make call
///             eFrameworkunifiedStatusServNotFound        = 19,  ///< Service not present in serv dir
///             eFrameworkunifiedStatusServerInUse         = 20,  ///< Service already processing 1 client request
///             eFrameworkunifiedStatusDbIndexing        = 21,  ///< Database Indexing in progress
///             eFrameworkunifiedStatusNullPointer       = 22,
///             eFrameworkunifiedStatusMsgNotProcessed     = 23,
///             eFrameworkunifiedStatusFileLoadSuccess     = 24, ///< File Load Success
///             eFrameworkunifiedStatusFileLoadError       = 25, ///< File Load Error
///             eFrameworkunifiedStatusAccessError         = 26, ///< Error when accessing resource
///             eFrameworkunifiedStatusDuplicate           = 27, ///< Duplicate entry
///             eFrameworkunifiedStatusMsgQEmpty             = 28, ///< Message queue empty
///             eFrameworkunifiedStatusThreadAlreadyRunning  = 29,
///             eFrameworkunifiedStatusErrNoEBADF            = 30,  ///< Bad file descriptor
///             eFrameworkunifiedStatusErrNoEAGAIN           = 31,  ///< Resource unavailable, try again
///             eFrameworkunifiedStatusErrNoEINTR            = 32,  ///< Interrupted system call
///             eFrameworkunifiedStatusSessionErr      = 33,  ///< Error in session handling
///             eFrameworkunifiedStatusDBCorrupt       = 34,  ///< Database corrupt
///             eFrameworkunifiedStatusDBFileNotFound    = 35  ///< Database file not found
///           } EFrameworkunifiedStatus, *PEFrameworkunifiedStatus;
///            @endcode
/// \~english @par Preconditons
///       none
/// \~english @par Change of internal status
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - Function pointer specified by the argument is NULL.
///       - It is an incorrect value(hNPMsgQ) or hNPMsgQ is NULL.
///       - It is an incorrect value(pPublisherName) or pPublisherName is NULL.
///       - It is an incorrect value(pDstFolderPath) or pDstFolderPath is NULL.
///       - It is an incorrect value(pTag) or pTag is NULL.
///       - It is an incorrect value(hUser) or hUser is NULL.
/// \~english @par Detail
///       Invoke callback function to send message to Notification Service to copy folder from persistent memory to
///       specified path.\n
///       If callback function is null and expectedCalls is '0', using default function 'NPLoadPersistentFolder' to send
///       message to Notification Service to copy folder from persistent memory to specified path.
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync only
/// \~english @see
///       FrameworkunifiedNPLoadPersistentFolder
EFrameworkunifiedStatus NPLoadPersistentFolder(HANDLE hNPMsgQ, PCSTR pPublisherName, PCSTR pDstFolderPath,
                                  PCSTR pTag, HANDLE hUser);

//////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NPReleasePersistentFolder
/// \~english @par Brief
///        API to send message to Notification Service to notify that the folder can be persisted.
/// \~english @param[in] hNPMsgQ
///        HANDLE - Handle to the Framework
/// \~english @param[in] pPublisherName
///        PCSTR - Name of publisher message queue.
/// \~english @param[in] eFrameworkunifiedReleaseType
///        EFrameworkunifiedReleaseType - eFrameworkunifiedNotOnRelease     :not on release
///                          eFrameworkunifiedPersistOnShutdown:persist on shutdown
///                          eFrameworkunifiedPersistInstantly :persist instantly
/// \~english @param[in] pTag
///        PCSTR - Tag associated with the folder released
/// \~english @param[in] pFullFolderPath
///        PCSTR - Full path name of the folder to be persisted
/// \~english @param[in] hUser
///        HANDLE - Name of new Personality
/// \~english @retval EFrameworkunifiedStatus indicates success or error
/// \~english @retval Success(eFrameworkunifiedStatusOK)
/// \~english @retval Error(anything else)
/// \~english @par
///        EFrameworkunifiedStatus Enum
/// \~english @code
///           typedef enum eFrameworkunifiedStatus {
///             eFrameworkunifiedStatusEmptyMediaList    = -10, ///< Empty media list
///             eFrameworkunifiedStatusSessionLimitMaxedOut  = -9,  ///< Maximum session limit reached
///             eFrameworkunifiedStatusDbRecNotFound       = -8, ///< Database record not found
///             eFrameworkunifiedStatusDbResultError       = -7, ///< Database result error
///             eFrameworkunifiedStatusDbExecuteFail       = -6, ///< Database execute fail
///             eFrameworkunifiedStatusSemCloseFail        = -5, ///< Semaphore close failed
///             eFrameworkunifiedStatusSemUnlockFail       = -4, ///< Semaphore unlock failed
///             eFrameworkunifiedStatusSemLockFail         = -3, ///< Semaphore lock failed
///             eFrameworkunifiedStatusFail                = -2, ///< Failed
///             eFrameworkunifiedStatusErrOther            = -1, ///< Unknown error
///             eFrameworkunifiedStatusOK                  = 0,  ///< Success / Pass / OK
///             eFrameworkunifiedStatusInvldBuf            = 1,  ///< Invalid buffer
///             eFrameworkunifiedStatusInvldHandle         = 2,  ///< Invalid handle
///             eFrameworkunifiedStatusInvldHndlType       = 3,  ///< Invalid handle type
///             eFrameworkunifiedStatusInvldQName        = 4,  ///< Invalid messasge queue name
///             eFrameworkunifiedStatusMsgQFull            = 5,  ///< Message queue full
///             eFrameworkunifiedStatusInvldNotification   = 6,///< The Notification event not present
///             eFrameworkunifiedStatusInvldParam        = 7,  ///< Invalid parameter
///             eFrameworkunifiedStatusInvldBufSize        = 8,  ///< Buf size too small
///             eFrameworkunifiedStatusInvldID             = 9,  ///< Unrecognized ID
///             eFrameworkunifiedStatusCannotRelease       = 10, ///< Cannot release resource
///             eFrameworkunifiedStatusBadConnection       = 11, ///< Could not locate resource
///             eFrameworkunifiedStatusExit            = 12,  ///< Normal application termination
///             eFrameworkunifiedStatusNotImplemented      = 13,  ///< incomplete feature
///             eFrameworkunifiedStatusThreadBusy          = 14,  ///< Joined thread is already being joined
///             eFrameworkunifiedStatusThreadSelfJoin      = 15,  ///< Thread is joining itself
///             eFrameworkunifiedStatusThreadInvalidVal    = 16, ///< Invalid value passed
///             eFrameworkunifiedStatusThreadNotExist      = 17,  ///< The thread does not exist
///             eFrameworkunifiedStatusFault               = 18,  ///< A fault occurred while attempting to make call
///             eFrameworkunifiedStatusServNotFound        = 19,  ///< Service not present in serv dir
///             eFrameworkunifiedStatusServerInUse         = 20,  ///< Service already processing 1 client request
///             eFrameworkunifiedStatusDbIndexing        = 21,  ///< Database Indexing in progress
///             eFrameworkunifiedStatusNullPointer       = 22,
///             eFrameworkunifiedStatusMsgNotProcessed     = 23,
///             eFrameworkunifiedStatusFileLoadSuccess     = 24, ///< File Load Success
///             eFrameworkunifiedStatusFileLoadError       = 25, ///< File Load Error
///             eFrameworkunifiedStatusAccessError         = 26, ///< Error when accessing resource
///             eFrameworkunifiedStatusDuplicate           = 27, ///< Duplicate entry
///             eFrameworkunifiedStatusMsgQEmpty             = 28, ///< Message queue empty
///             eFrameworkunifiedStatusThreadAlreadyRunning  = 29,
///             eFrameworkunifiedStatusErrNoEBADF            = 30,  ///< Bad file descriptor
///             eFrameworkunifiedStatusErrNoEAGAIN           = 31,  ///< Resource unavailable, try again
///             eFrameworkunifiedStatusErrNoEINTR            = 32,  ///< Interrupted system call
///             eFrameworkunifiedStatusSessionErr      = 33,  ///< Error in session handling
///             eFrameworkunifiedStatusDBCorrupt       = 34,  ///< Database corrupt
///             eFrameworkunifiedStatusDBFileNotFound    = 35  ///< Database file not found
///           } EFrameworkunifiedStatus, *PEFrameworkunifiedStatus;
///            @endcode
/// \~english @par Preconditons
///       none
/// \~english @par Change of internal status
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - Function pointer specified by the argument is NULL.
///       - It is an incorrect value(hNPMsgQ) or hNPMsgQ is NULL.
///       - It is an incorrect value(pPublisherName) or pPublisherName is NULL.
///       - It is an incorrect value(pDstFolderPath) or pDstFolderPath is NULL.
///       - It is an incorrect value(pTag) or pTag is NULL.
///       - It is an incorrect value(hUser) or hUser is NULL.
/// \~english @par Detail
///       Invoke callback function to send message to Notification Service to notify that the folder can be persisted.\n
///       If callback function is null and expectedCalls is '0', using default function 'NPReleasePersistentFolder' to
///       send message to Notification Service to notify that the folder can be persisted.
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync only
/// \~english @see
///       FrameworkunifiedNPReleasePersistentFolder
EFrameworkunifiedStatus NPReleasePersistentFolder(HANDLE hNPMsgQ, PCSTR pPublisherName, EFrameworkunifiedReleaseType eFrameworkunifiedReleaseType, PCSTR pTag,
                                     PCSTR pFullFolderPath, HANDLE hUser);

#ifdef __cplusplus
}
#endif

#endif /* __NATIVESERVICES_NP_SERVICE_IF_H__ */  // NOLINT  (build/header_guard)
/** @}*/  // end of notification_persistent_service
/** @}*/  // end of native_service
/** @}*/  // end of BaseSystem
// EOF
