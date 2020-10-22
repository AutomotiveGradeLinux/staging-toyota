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
/// \brief    This file contains declaration of singleton class CnotificationpersistentservicePersonalizationManager which is
///       used to manage information of personality.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_PERSONALIZATION_MANAGER_H_
#define NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_PERSONALIZATION_MANAGER_H_

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <map>
#include "ns_npp_personality.h"

typedef std::map<std::string, CNotificationpersistentservicePersonality *> Personality_Type;

/**
 * This class maintains information on creation of new personality or
 * change of personality.
 */
class CnotificationpersistentservicePersonalizationManager {
 private:
  Personality_Type *m_pmPersonality;        ///< list of personality

  UI_32 m_uiUserId;               ///< unique user id

  std::string m_cCurrentPersonality;          ///< current personality name

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CreatePersonality
  /// This function is used to create new Personality.
  ///
  /// \param  [IN] f_cpersonalityname
  ///     std::string - Personality name
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus CreatePersonality(const std::string &f_cpersonalityname);

 public:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CnotificationpersistentservicePersonalizationManager
  /// Constructor of CnotificationpersistentservicePersonalizationManager class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CnotificationpersistentservicePersonalizationManager();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// ~CnotificationpersistentservicePersonalizationManager
  /// Destructor of CnotificationpersistentservicePersonalizationManager class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  ~CnotificationpersistentservicePersonalizationManager();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// NotificationpersistentserviceGetPersonality
  /// This function is used to get the current personality.
  ///
  /// \param  [OUT] f_cpersonalityname
  ///     std::string - Personality name
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus NotificationpersistentserviceGetPersonality(std::string &f_cpersonalityname); // NOLINT (runtime/references)

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// NotificationpersistentserviceSetPersonality
  /// This function is used to set the new personality.
  ///
  /// \param  [IN] f_cpersonalityname
  ///     std::string - Personality name
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus NotificationpersistentserviceSetPersonality(const std::string f_cpersonalityname);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// NotificationpersistentserviceIsValidPersonality
  /// Check if user is valid or not.
  ///
  /// \param  [IN] f_cpersonalityname
  ///     std::string - Personality name
  ///
  /// \return BOOL
  ///     TRUE - Valid user
  ///     FALSE - Not a valid user
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  BOOL NotificationpersistentserviceIsValidPersonality(const std::string f_cpersonalityname);
};

#endif  // NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_PERSONALIZATION_MANAGER_H_
