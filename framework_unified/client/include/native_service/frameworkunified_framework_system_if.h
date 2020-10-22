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

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NSFramework
/// \brief    This file provides framework's API which are used by system services.
///
///
///
///////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file frameworkunified_framework_system_if.h
 * @brief \~english This file provides framework's API which are used by system services.
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
#ifndef __FRAMEWORKUNIFIED_NATIVESERVICES_INC_FRAMEWORK_FRAMEWORKUNIFIED_FRAMEWORK_SYSTEM_IF_H__  // NOLINT  (build/header_guard)
#define __FRAMEWORKUNIFIED_NATIVESERVICES_INC_FRAMEWORK_FRAMEWORKUNIFIED_FRAMEWORK_SYSTEM_IF_H__

#include <native_service/frameworkunified_types.h>

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup FrameworkunifiedNPClearPersistedData
  /// \~english @par Summary
  ///           API to delete all the persistent files and notification data from persistent memory
  ///           related to NS_NPS.
  ///           Note: This API is only to be used HMI service.
  ///           This API should not be used by the other application.
  /// \~english @param  [in] hApp
  ///            hApp   - Handle to the Framework
  /// \~english @par
  ///           hApp   HANDLE*
  /// \~english @param  [in] eFrameworkunifiedClearPersistenceScope
  ///            eFrameworkunifiedClearPersistenceScope   - specifies what data to delete from persistent memory
  /// \~english @par
  ///           eFrameworkunifiedClearPersistenceScope   EFrameworkunifiedClearPersistence enum
  /// \~english @code
  /// typedef enum _EFrameworkunifiedClearPersistence {
  ///   eFrameworkunifiedClearAllData = 0,         ///< clears all the data in persistence memory for all users
  ///   ///< (currently only this enum value is supported.)
  ///   eFrameworkunifiedClearAllApplicationData,      ///< clears all the data(files, folders) related to all application for all
  ///   users
  ///   eFrameworkunifiedClearAllNotificationData,     ///< clears all the notification data related to all application for all users
  ///   eFrameworkunifiedClearCurrentUserData,       ///< clears all the data in persistence memory for current users
  ///   eFrameworkunifiedClearCurrentUserApplicationData,  ///< clears all the data(files, folders) related to all application for
  ///   current users
  ///   eFrameworkunifiedClearCurrentUserNotificationData  ///< clears all the notification data related to all application for
  ///   current users
  /// } EFrameworkunifiedClearPersistence;
  ///           @endcode
  /// \~english @retval EFrameworkunifiedStatus
  /// \~english @par
  ///            eFrameworkunifiedStatusOK
  ///            eFrameworkunifiedStatusNullPointer
  ///            eFrameworkunifiedStatusInvldParam
  /// \~english @par Preconditions
  ///       -
  /// \~english @par Change of the internal state
  ///       -  The internal state is not changed.
  /// \~english @par Causes of failures
  ///         - If input parameter Framework handler is invalid. [eFrameworkunifiedStatusNullPointer]
  ///         - If communicate handler of NPPService is NULL. [eFrameworkunifiedStatusNullPointer]
  ///         - Function NPClearPersistedData return invalid parameter. [eFrameworkunifiedStatusInvldParam]
  /// \~english @par Classification
  ///       - public
  /// \~english @par Type
  ///       - sync only
  /// \~english @par Detail
  ///           API to delete all the persistent files and notification data from persistent memory
  ///           related to NS_NPS.
  ///           Note: This API is only to be used HMI service.
  ///           This API should not be used by the other application.
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPClearPersistedData(HANDLE hApp, EFrameworkunifiedClearPersistence eFrameworkunifiedClearPersistenceScope = eFrameworkunifiedClearAllData);

#endif /* __FRAMEWORKUNIFIED_NATIVESERVICES_INC_FRAMEWORK_FRAMEWORKUNIFIED_FRAMEWORK_SYSTEM_IF_H__ */  // NOLINT  (build/header_guard)
/**@}*/  // end of framework
/**@}*/  // end of framework_unified
/**@}*/  // end of native_service
/**@}*/  // end of BaseSystem
//@}
