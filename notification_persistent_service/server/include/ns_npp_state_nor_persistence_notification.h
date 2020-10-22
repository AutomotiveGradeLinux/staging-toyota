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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup <<Group Tag>> <<Group Name>>
/// \ingroup  tag_NS_NPPService
/// .
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NS_NPPService
/// \brief    This file contains declaration of class CStateNorPersistenceNotification.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_STATE_NOR_PERSISTENCE_NOTIFICATION_H_
#define NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_STATE_NOR_PERSISTENCE_NOTIFICATION_H_

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <string>
#include "ns_npp_state_notification.h"

/**
 *  This class inherits CStateNotification class and implements specific operations related to
 *  nor persistent notifications.
 */
class CStateNorPersistenceNotification : public CStateNotification {
 public:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CStateNorPersistenceNotification
  /// Constructor of CStateNorPersistenceNotification class
  ///
  /// \param  [IN] f_cnotificationname
  ///     std::string - Notification name
  ///
  /// \param  [IN] f_uimaxmsgsize
  ///     UI_32 - Maximum size of notification data
  ///
  /// \param  [IN] f_uidelay
  ///     UI_32 - delay
  ///
  /// \param  [IN] f_epersistcategory
  ///     EFrameworkunifiedPersistCategory - persist category
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CStateNorPersistenceNotification(const std::string &f_cnotificationname,
                                   const UI_32 f_uimaxmsgsize,
                                   const UI_32 f_uidelay,
                                   const EFrameworkunifiedPersistCategory f_epersistcategory = eFrameworkunifiedUserData);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// ~CStateNorPersistenceNotification
  /// Destructor of CStateNorPersistenceNotification class
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  ~CStateNorPersistenceNotification();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// GetPersistenceDelay
  /// Method to get the persistence delay for a specific notification.
  ///
  /// \param
  ///
  /// \return UI_32
  //      UI_32 - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  UI_32 GetPersistenceDelay();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// Publish
  /// This function publishes the notification to subscribed clients and saves the data
  /// immediately to persistent memory.
  ///
  /// \param  [IN] f_cservicename
  ///     std::string - name of service publishing the notification
  ///
  /// \param  [IN] f_pmessage
  ///     std::string - data of notification
  ///
  /// \param  [IN] f_uimsgsize
  ///     std::string - length of data
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus Publish(const std::string &f_cservicename,
                             PVOID f_pmessage,
                             const UI_32 f_uimsgsize);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// PublishNotification
  /// This function publishes the notification to subscribed clients.
  ///
  /// \param  [IN] f_cservicename
  ///         std::string - name of service publishing the notification
  ///
  /// \param  [IN] f_pmessage
  ///         std::string - data of notification
  ///
  /// \param  [IN] f_uimsgsize
  ///         std::string - length of data
  ///
  /// \return EFrameworkunifiedStatus
  //          EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus PublishNotification(const std::string &f_cservicename,
                                 PVOID f_pmessage,
                                 const UI_32 f_uimsgsize);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// GetPersistentCategory
  /// Gets the persist type of notification
  ///
  ///
  /// \return EFrameworkunifiedPersistCategory
  //      EFrameworkunifiedPersistCategory - persist type
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedPersistCategory GetPersistentCategory();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// SetPersistentCategory
  /// Sets the persist type of notification
  ///
  /// \param  [IN] f_epersistcategory
  ///     EFrameworkunifiedPersistCategory - persist category
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus SetPersistentCategory(const EFrameworkunifiedPersistCategory f_epersistcategory);
  // Handle of the immediate persistence worker thread.
  static HANDLE m_hNSImmediatePersistenceThread;  // NOLINT (readability/naming)

 private:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// SaveDataToNor
  /// Saves the persistent data to nor
  ///
  /// \param  [IN] f_pmessage
  ///     PVOID - Message data
  ///
  /// \param  [IN] f_msgsize
  ///     UI_32 - Size of Message data
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus SaveDataToNor(PVOID f_pmessage,
                           const UI_32 f_msgsize);

  UI_32 m_uiDelay;  // Time Delay between persistence on NOR

  EFrameworkunifiedPersistCategory m_ePersistCategory;  // Persistent category
};

#endif  // NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_STATE_NOR_PERSISTENCE_NOTIFICATION_H_
