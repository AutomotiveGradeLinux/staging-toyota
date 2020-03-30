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

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NativeService
/// \brief    This file contains the class declaration for synchronization
///       API's.
///
///////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file frameworkunified_framework_sync.h
 * @brief \~english This file contains the class declaration for synchronization
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
 *  @ingroup native_service
 *  @{
 */
#ifndef __FRAMEWORKUNIFIED_NATIVESERVICES_NATIVESERVICES_INC_FRAMEWORK_FRAMEWORKUNIFIED_FRAMEWORK_SYNC_H__  // NOLINT  (build/header_guard)
#define __FRAMEWORKUNIFIED_NATIVESERVICES_NATIVESERVICES_INC_FRAMEWORK_FRAMEWORKUNIFIED_FRAMEWORK_SYNC_H__

#include <pthread.h>
#include <native_service/frameworkunified_framework_types.h>
#include <time.h>
#include <map>
#include <string>

struct SFrameworkunifiedSyncDataPacket;

// Map containing the synchronization data as per the notification
typedef std::map<std::string, SFrameworkunifiedSyncDataPacket *> TSyncDataPacketList;

// Iterator to the map containing the synchronization data as per the notification
typedef TSyncDataPacketList::iterator TSyncDataPacketItr;

/**
 * @class CFrameworkunifiedSyncData
 * \~english @brief this file has the CFrameworkunifiedSyncData class definitions
 * \~english @par   Brief Introduction
 *            - This class provids some functions to handle the synchronization data as per the notification.
 *
 */
class CFrameworkunifiedSyncData {
 public:
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedSyncData
  /// \~english @par Brief
  ///       -  This function is used to get the singleton instance of class.
  /// \~english @param None
  /// \~english @retval CFrameworkunifiedSyncData - singleton instance of class
  /// \~english @par Preconditons
  ///       - None
  /// \~english @par Change of internal status
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Detail
  ///       - If m_psSyncData is NULL,will create a new object,otherwise, will return m_psSyncData.
  /// \~english @par Classification
  ///          Public
  /// \~english @see CFrameworkunifiedSyncData::CFrameworkunifiedSyncData
  ///////////////////////////////////////////////////////////////////////////////////////////
  static CFrameworkunifiedSyncData *FrameworkunifiedGetSyncDataInstance();

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedSyncData
  /// \~english @par Brief
  ///       -  This function is used to release the instance of class.
  /// \~english @param None
  /// \~english @retval eFrameworkunifiedStatusOK - success
  /// \~english @retval except eFrameworkunifiedStatusOK - failure
  /// \~english @par Preconditons
  ///       - None
  /// \~english @par Change of internal status
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - system function error.[Not eFrameworkunifiedStatusOK]
  /// \~english @par Detail
  ///       - If m_psSyncData is NULL,return success,otherwise, will delete m_psSyncData.
  /// \~english @par Classification
  ///          Public
  /// \~english @par
  ///           - eFrameworkunifiedStatus:Result
  ///               - eFrameworkunifiedStatusOK:Success
  ///               - Except eFrameworkunifiedStatusOK:Failure
  /// \~english @see CFrameworkunifiedSyncData::CFrameworkunifiedSyncData
  ///////////////////////////////////////////////////////////////////////////////////////////
  static EFrameworkunifiedStatus FrameworkunifiedReleaseSyncDataInstance();

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedSyncData
  /// \~english @par Brief
  ///       -  This method is used by the application to start the notification synchronization thread.
  /// \~english @param [in] hApp
  ///       HANDLE - Application Handle.
  /// \~english @retval eFrameworkunifiedStatusOK           - On Success
  /// \~english @retval eFrameworkunifiedStatusThreadAlreadyRunning - If thread already running
  /// \~english @retval eFrameworkunifiedStatusNullPointer      - Null Handle
  /// \~english @retval eFrameworkunifiedStatusInvldHandle        - Invalid Handle
  /// \~english @par Preconditons
  ///       - The function FrameworkunifiedReleaseSyncDataInstance is successed, object is created successfully.
  /// \~english @par Change of internal status
  ///       -m_hSyncThreadMsgQHandle saves sync thread handle.
  /// \~english @par Conditions of processing failure
  ///       - m_hSyncThreadMsgQHandle is not NULL, thread already running.[eFrameworkunifiedStatusThreadAlreadyRunning]
  ///       - Null Handle.[eFrameworkunifiedStatusNullPointer]
  ///       - Invalid Handle.[eFrameworkunifiedStatusInvldHandle]
  /// \~english @par Detail
  ///       - This fucntion create a child thread of hApp, m_hSyncThreadMsgQHandle saves the child thread.
  ///       - The child thread will be used to receive and send notification data.
  /// \~english @par Classification
  ///          Public
  /// \~english @par
  ///           - eFrameworkunifiedStatus:Result
  ///               - eFrameworkunifiedStatusOK:Success
  ///               - Except eFrameworkunifiedStatusOK:Failure
  /// \~english @see None
  ///////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus FrameworkunifiedStartNotificationSync(HANDLE hApp);

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedSyncData
  /// \~english @par Brief
  ///       - This method is used to stop the synchronization notification thread.
  /// \~english @param [in] hApp
  ///       HANDLE - Application Handle.
  /// \~english @retval eFrameworkunifiedStatusOK           - On Success
  /// \~english @retval eFrameworkunifiedStatusThreadNotExist - If thread already running
  /// \~english @retval eFrameworkunifiedStatusNullPointer      - Null Handle
  /// \~english @retval eFrameworkunifiedStatusInvldHandle        - Invalid Handle
  /// \~english @par Preconditons
  ///       -The function FrameworkunifiedStartNotificationSync is successed.
  /// \~english @par Change of internal status
  ///       - m_hSyncThreadMsgQHandle will be delete.
  /// \~english @par Conditions of processing failure
  ///       - If thread doesn't exist.[eFrameworkunifiedStatusThreadNotExist]
  ///       - Null Handle.[eFrameworkunifiedStatusNullPointer]
  ///       - Invalid Handle.[eFrameworkunifiedStatusInvldHandle]
  /// \~english @par Detail
  ///       - This method is used by the application to stop the notification synchronization thread.
  ///         m_hSyncThreadMsgQHandle is Invalid and can't be used to get notification data for hApp.
  /// \~english @par Classification
  ///          Public
  /// \~english @par
  ///           - eFrameworkunifiedStatus:Result
  ///               - eFrameworkunifiedStatusOK:Success
  ///               - Except eFrameworkunifiedStatusOK:Failure
  /// \~english @see None
  ///////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus FrameworkunifiedStopNotificationSync(HANDLE hApp);

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedSyncData
  /// \~english @par Brief
  ///       - API to subscribe to a synchronization notification data.
  /// \~english @param [in] f_cNotification
  ///       std::string - Notification to be subscribed.
  /// \~english @retval eFrameworkunifiedStatusOK           - On Success
  /// \~english @retval eFrameworkunifiedStatusFail - On failure
  /// \~english @par Preconditons
  ///       - none
  /// \~english @par Change of internal status
  ///       - none
  /// \~english @par Conditions of processing failure
  ///       - If f_cNotification is empty or m_hSyncThreadAppHandle is NULL.[eFrameworkunifiedStatusFail]
  /// \~english @par Detail
  ///       - API to subscribe to a synchronization notification data.\n
  ///         application will get the synchronization notification data at run-time.
  /// \~english @par Classification
  ///          Public
  /// \~english @par
  ///           - eFrameworkunifiedStatus:Result
  ///               - eFrameworkunifiedStatusOK:Success
  ///               - eFrameworkunifiedStatusFail:Failure
  /// \~english @see None
  ///////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus FrameworkunifiedSubscribeNotificationWithDataSync(const std::string &f_cNotification);

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedSyncData
  /// \~english @par Brief
  ///       - API to unsubscribe to a synchronization notification data.
  /// \~english @param [in] f_cNotification
  ///       std::string - Notification to be subscribed.
  /// \~english @retval eFrameworkunifiedStatusOK           - On Success
  /// \~english @retval eFrameworkunifiedStatusFail - On failure
  /// \~english @par Preconditons
  ///       - the FrameworkunifiedSubscribeNotificationWithDataSync result is success.
  /// \~english @par Change of internal status
  ///       - The synchronization data's pointor of the special notification will be remove and delete.
  ///       - The synchronization data of the special notification will delete from m_mSyncDataMap.
  /// \~english @par Conditions of processing failure
  ///       - If f_cNotification is empty or m_hSyncThreadAppHandle is NULL.[eFrameworkunifiedStatusFail]
  /// \~english @par Detail
  ///       - API to unsubscribe to a synchronization notification data.\n
  ///         After unsubscribing,notification synchronization data of application will be freed.
  /// \~english @par Classification
  ///          Public
  /// \~english @par
  ///           - eFrameworkunifiedStatus:Result
  ///               - eFrameworkunifiedStatusOK:Success
  ///               - eFrameworkunifiedStatusFail:Failure
  /// \~english @see None
  ///////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus FrameworkunifiedUnSubscribeNotificationWithDataSync(const std::string &f_cNotification);

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedSyncData
  /// \~english @par Brief
  ///       - This function is used to get the synchronization notification data for a particular notification.
  /// \~english @param [in] f_cNotification
  ///       std::string - Notification for which data is required
  /// \~english @param [in] f_pBuffer
  ///       PVOID - Buffer for the synchronization data
  /// \~english @param [in] f_nBufferSize
  ///       UI_16 - size of notification data buffer
  /// \~english @retval eFrameworkunifiedStatusOK           - Success
  /// \~english @retval eFrameworkunifiedStatusNullPointer - Null pointor
  /// \~english @retval eFrameworkunifiedStatusFail - failure
  /// \~english @par Preconditons
  ///       - none
  /// \~english @par Change of internal status
  ///       - none
  /// \~english @par Conditions of processing failure
  ///       - If f_cNotification is empty or m_hSyncThreadAppHandle is NULL or f_nBufferSize is 0.[eFrameworkunifiedStatusFail]
  ///       - If l_itSyncDataPacketItr is not in m_mSyncDataMap.[eFrameworkunifiedStatusNullPointer]
  /// \~english @par Detail
  ///       - This function is used to get the synchronization notification data for a particular notification.
  ///         f_cNotification is the iterator to the map containing the synchronization data of notification,\n
  ///         using this iterator to get data from m_mSyncDataMap.
  /// \~english @par Classification
  ///          Public
  /// \~english @par
  ///           - eFrameworkunifiedStatus:Result
  ///               - eFrameworkunifiedStatusOK:Success
  ///               - Except eFrameworkunifiedStatusOK:Failure
  /// \~english @see None
  ///////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus FrameworkunifiedGetSyncNotificationData(const std::string &f_cNotification, PVOID f_pBuffer, UI_16 f_nBufferSize);

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedSyncData
  /// \~english @par Brief
  ///       - API to get the size of synchronization notification data.
  /// \~english @param [in] f_cNotification
  ///       const std::string - Notification to be subscribed.
  /// \~english @retval UI_32 - size of notification data
  /// \~english @par Preconditons
  ///       -Instance is created successfully.
  /// \~english @par Change of internal status
  ///       -None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Detail
  ///       - This function is used to get the size of the synchronization data.
  ///         if f_cNotification is null or l_itSyncDataPacketItr is not in m_mSyncDataMap\n
  ///         the function will return 0.
  /// \~english @par Classification
  ///       - Public
  /// \~english @see None
  ///////////////////////////////////////////////////////////////////////////////////////////
  UI_32 FrameworkunifiedGetSyncDataSize(const std::string &f_cNotification);

 private:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CFrameworkunifiedSyncData
  /// Constructor of CFrameworkunifiedSyncData class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CFrameworkunifiedSyncData();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// ~CFrameworkunifiedSyncData
  /// Destructor of CFrameworkunifiedSyncData class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  ~CFrameworkunifiedSyncData();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedSyncDataThreadStart
  /// Callback on the start of the sync thread.
  ///
  /// \param [in] hThread
  ///         HANDLE - Thread Application Handle.
  ///
  /// \return status
  ///         EFrameworkunifiedStatus - success or error
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus FrameworkunifiedSyncDataThreadStart(HANDLE hThread);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedSyncDataThreadStop
  /// Callback on the stop of the sync thread.
  ///
  /// \param [in] hThread
  ///         HANDLE - Thread Application Handle.
  ///
  /// \return status
  ///         EFrameworkunifiedStatus - success or error
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus FrameworkunifiedSyncDataThreadStop(HANDLE hThread);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedSyncDataNotifCallback
  /// Callback on the receiving the published notification.
  ///
  /// \param [in] hThread
  ///         HANDLE - Thread Application Handle.
  ///
  /// \return status
  ///         EFrameworkunifiedStatus - success or error
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus FrameworkunifiedSyncDataNotifCallback(HANDLE hThread);

  static CFrameworkunifiedSyncData *m_psSyncData;    ///< singleton instance of class

  BOOL m_bSyncThreadStarted;        /// Check to verify if thread started

  TSyncDataPacketList
  *m_mSyncDataMap;    /// Pointer to the map containing the synchronization data as per the notification

  HANDLE m_hSyncThreadAppHandle;      /// S ynchronization thread Application Handle

  HANDLE m_hAppHandle;          /// Application Handle

  HANDLE m_hSyncThreadMsgQHandle;     /// Synchronization thread message queue handle

  static pthread_spinlock_t m_pSyncLock;      /// Synchronization Spinlock
};

#endif /* __FRAMEWORKUNIFIED_NATIVESERVICES_NATIVESERVICES_INC_FRAMEWORK_FRAMEWORKUNIFIED_FRAMEWORK_SYNC_H__ */  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
