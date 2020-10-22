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
/// \brief    This file contains declaration of class CStatePersistenceUserNotification.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_STATE_PERSISTENCE_USER_NOTIFICATION_H_
#define NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_STATE_PERSISTENCE_USER_NOTIFICATION_H_

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <string>
#include "ns_npp_state_notification.h"


/**
 *  This class inherits CStateNotification class and implements specific operations related to
 *  persistent notifications specific to user.
 */
class CStatePersistenceUserNotification : public CStateNotification {
 public:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CStatePersistenceUserNotification
  /// Constructor of CStatePersistenceUserNotification class
  ///
  /// \param  [IN] f_cnotificationname
  ///     std::string - Notification name
  ///
  /// \param  [IN] f_uimaxmsgsize
  ///     UI_32 - Maximum size of notification data
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CStatePersistenceUserNotification(const std::string &f_cnotificationname,
                                    const UI_32 f_uimaxmsgsize);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// ~CStatePersistenceUserNotification
  /// Destructor of CStatePersistenceUserNotification class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  ~CStatePersistenceUserNotification();
};

#endif  // NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_STATE_PERSISTENCE_USER_NOTIFICATION_H_
