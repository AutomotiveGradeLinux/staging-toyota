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
/// \brief    This file contain declaration of CNotificationpersistentservicePersonality class.
///       The class object define personality.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_PERSONALITY_H_
#define NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_PERSONALITY_H_

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <native_service/frameworkunified_types.h>
#include <string>

/**
 *  This class holds all the informations related to a personality.
 */
class CNotificationpersistentservicePersonality {
 private:
  // no members in private

 public:
  std::string m_cUserName;  ///< user name

  UI_32 m_uiUserId;     ///< unique identifier of each user

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CNotificationpersistentservicePersonality
  /// Constructor of CNotificationpersistentservicePersonality class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CNotificationpersistentservicePersonality(): m_cUserName(""), m_uiUserId(0) {}  // LCOV_EXCL_BR_LINE 11: unexpected branch

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// ~CNotificationpersistentservicePersonality
  /// Destructor of CNotificationpersistentservicePersonality class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  ~CNotificationpersistentservicePersonality() {}
};

#endif  // NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_PERSONALITY_H_
