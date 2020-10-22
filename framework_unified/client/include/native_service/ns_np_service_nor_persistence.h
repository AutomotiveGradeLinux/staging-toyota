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

//////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup tag_NPPService
/// \brief    This file contains the declaration for methods to retrieve and store data for NOR storage
///
///
///
//////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @file ns_np_service_nor_persistence.h
 * @brief    This file contains the declaration for methods to retrieve and store data for NOR storage
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

#ifndef __NATIVESERVICES_NP_NOR_SERVICE_H__  // NOLINT  (build/header_guard)
#define __NATIVESERVICES_NP_NOR_SERVICE_H__

///////////////////////////////////////////////////////////////////////////////
// Include Files
///////////////////////////////////////////////////////////////////////////////
#include <native_service/frameworkunified_types.h>

#ifdef __cplusplus
extern "C" {
#endif

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NPSynchronousReadPersistentData
  /// \~english @par Brief
  ///        API to synchronously read persistent data corresponding to a notification before NPPService is available
  /// \~english @param[in] pAppName
  ///        PCSTR - Application/ thread name
  /// \~english @param[in] notif_name
  //         PCSTR - Name of Notification
  /// \~english @param[in] pData
  ///        PVOID - Pointer to the buffer where read data will be written.
  /// \~english @param[in] uiDataSize
  ///        UI_32 - Length of the data buffer.
  /// \~english @param[in] ePersistCategory
  ///        const EFrameworkunifiedPersistCategory - Persist Category of Notification.
  /// \~english @retval eFrameworkunifiedStatusOK
  /// \~english @retval eFrameworkunifiedStatusInvldParam
  /// \~english @retval eFrameworkunifiedStatusFail
  /// \~english @par Preconditons
  ///        none
  /// \~english @par Change of internal status
  ///       - Change of internal state according to the API does not occur.
  /// \~english @par Conditions of processing failure
  ///       - NULL handle specified in the argument(pAppName).[eFrameworkunifiedStatusInvldParam]
  ///       - NULL handle specified in the argument(notif_name).[eFrameworkunifiedStatusInvldParam]
  ///       - NULL handle specified in the argument(pData).[eFrameworkunifiedStatusInvldParam]
  ///       - 0 specified in the argument(uiDataSize).[eFrameworkunifiedStatusFail]
  ///       - Handle of the application [eFrameworkunifiedStatusFail]
  ///       - If the file access process fails,.... [eFrameworkunifiedStatusFail]
  /// \~english @par Detail
  ///          API to synchronously read persistent data corresponding to a notification before NPPService is available
  /// \~english @par
  ///          File reading by this API is exclusive control by semaphore,
  ///          and writing to non-voatie area is to use NPSynchronousWritePersistentData().
  /// \~english @par Classification
  ///          Public
  /// \~english @par Type
  ///          Set Get
  /// \~english @see NPSynchronousWritePersistentData
  ////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPSynchronousReadPersistentData(PCSTR pAppName,
                                           PCSTR notif_name,
                                           PVOID pData,
                                           UI_32 uiDataSize,
                                           const EFrameworkunifiedPersistCategory ePersistCategory = eFrameworkunifiedUserData);

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NPSynchronousWritePersistentData
  /// \~english @par Brief
  ///        API to synchronously write persistent data corresponding to a notification before NPPService is available
  /// \~english @param[in] pAppName
  ///        PCSTR - Application/ thread name
  /// \~english @param[in] notif_name
  //         PCSTR - Name of Notification
  /// \~english @param[in] pData
  ///        PVOID - Pointer to the buffer where read data will be written.
  /// \~english @param[in] uiDataSize
  ///        UI_32 - Length of the data buffer.
  /// \~english @param[in] ePersistCategory
  ///        const EFrameworkunifiedPersistCategory - Persist Category of Notification.
    /// \~english @retval eFrameworkunifiedStatusOK
  /// \~english @retval eFrameworkunifiedStatusInvldParam
  /// \~english @retval eFrameworkunifiedStatusFail
  /// \~english @par Preconditons
  ///        none
  /// \~english @par Change of internal status
  ///       - Change of internal state according to the API does not occur.
  /// \~english @par Conditions of processing failure
  ///       - NULL handle specified in the argument(pAppName).[eFrameworkunifiedStatusInvldParam]
  ///       - NULL handle specified in the argument(notif_name).[eFrameworkunifiedStatusInvldParam]
  ///       - NULL handle specified in the argument(pData).[eFrameworkunifiedStatusInvldParam]
  ///       - 0 specified in the argument(uiDataSize).[eFrameworkunifiedStatusFail]
  ///       - Handle of the application [eFrameworkunifiedStatusFail]
  ///       - If the file access process fails,.... [eFrameworkunifiedStatusFail]
  /// \~english @par Detail
  ///          API to synchronously read persistent data corresponding to a notification before NPPService is available
  /// \~english @par
  ///        File reading by this API is exclusive control by semaphore,
  ///        and writing to non-voatie area is to use NPSynchronousWritePersistentData().
  /// \~english @par Classification
  ///          Public
  /// \~english @par Type
  ///          Set Get
  /// \~english @see NPSynchronousWritePersistentData
  ////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPSynchronousWritePersistentData(PCSTR pAppName,
                                            PCSTR notif_name,
                                            PVOID pData,
                                            const UI_32 uiDataSize,
                                            const EFrameworkunifiedPersistCategory ePersistCategory = eFrameworkunifiedUserData);
#ifdef __cplusplus
}
#endif

#endif /* __NATIVESERVICES_NP_NOR_SERVICE_H__ */  // NOLINT  (build/header_guard)
/** @}*/  // end of notification_persistent_service
/** @}*/  // end of native_service
/** @}*/  // end of BaseSystem
// EOF
