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
/// \brief    This file contain declaration of class CNotificationReceiver.
///       It holds the handle of message queue for sending notification.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_NOTIFICATION_RECEIVER_H_
#define NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_NOTIFICATION_RECEIVER_H_

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <native_service/frameworkunified_types.h>

class CNotificationReceiver {
 private:
  // no members in private

 public:
  HANDLE m_MsgQHandle;    ///< handle to message queue

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CNotificationReceiver
  /// Constructor of CNotificationReceiver class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CNotificationReceiver();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// ~CNotificationReceiver
  /// Destructor of CNotificationReceiver class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  ~CNotificationReceiver();
};

#endif  // NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_NOTIFICATION_RECEIVER_H_
