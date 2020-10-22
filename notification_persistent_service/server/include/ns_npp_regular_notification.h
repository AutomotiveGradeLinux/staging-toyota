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
/// \brief    This file contains declaration of class CRegularNotification.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_REGULAR_NOTIFICATION_H_
#define NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_REGULAR_NOTIFICATION_H_

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <string>
#include "ns_npp_notification.h"

/**
 *  This class inherits CNotification class and implements regular notification related operations.
 */
class CRegularNotification : public CNotification {
 private:
  // no members in private

 public:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CRegularNotification
  /// Constructor of CRegularNotification class
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
  CRegularNotification(const std::string &f_cnotificationname,
                       const UI_32 f_uimaxmsgsize);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// ~CRegularNotification
  /// Destructor of CRegularNotification class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  ~CRegularNotification();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// AddEventReciever
  /// This function adds the name of the application to receiver list of particular notification.
  ///
  /// \param  [IN] f_csubscribername
  ///     std::string - name of application subscribing for notification
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus AddEventReciever(const std::string &f_csubscribername);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// Publish
  /// This function publishes the notification to subscribed clients.
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
  EFrameworkunifiedStatus Publish(const std::string &f_cservicename,
                     PVOID f_pmessage,
                     const UI_32 f_uimsgsize);
};


#endif  // NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_REGULAR_NOTIFICATION_H_
