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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup <<Group Tag>> <<Group Name>>
/// \ingroup  tag_NS_NPPService
/// .
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NS_NPPService
/// \brief    This file contains declaration of class CStatePersistenceNotification.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_STATE_PERSISTENCE_NOTIFICATION_H_
#define NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_STATE_PERSISTENCE_NOTIFICATION_H_

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <string>
#include "ns_npp_state_notification.h"

/**
 *  This class inherits CStateNotification class and implements specific operations related to
 *  persistent notifications.
 */
class CStatePersistenceNotification : public CStateNotification {
 public:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CStatePersistenceNotification
  /// Constructor of CStatePersistenceNotification class
  ///
  /// \param  [IN] f_cnotificationname
  ///     std::string - Notification name
  ///
  /// \param  [IN] f_uimaxmsgsize
  ///     UI_32 - Maximum size of notification data
  ///
  /// \param  [IN] f_epersistcategory
  ///     EFrameworkunifiedPersistCategory - persist category
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CStatePersistenceNotification(const std::string &f_cnotificationname,
                                const UI_32 f_uimaxmsgsize,
                                const EFrameworkunifiedPersistCategory f_epersistcategory = eFrameworkunifiedUserData);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// ~CStatePersistenceNotification
  /// Destructor of CStatePersistenceNotification class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  ~CStatePersistenceNotification();

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

 private:
  EFrameworkunifiedPersistCategory m_ePersistCategory;  // Persistent category
};

#endif  // NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_STATE_PERSISTENCE_NOTIFICATION_H_
