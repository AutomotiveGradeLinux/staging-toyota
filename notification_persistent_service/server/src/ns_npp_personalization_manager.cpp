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
/// \brief    This file contains implementation of class CnotificationpersistentservicePersonalizationManager.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <utility>
#include <string>
#ifdef AGL_STUB
#include "ns_npp_personalization_manager.h"
#else
#include "ns_npp_Personalization_manager.h"
#endif
#include "ns_npp_notificationpersistentservicelog.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// CnotificationpersistentservicePersonalizationManager
/// Constructor of CnotificationpersistentservicePersonalizationManager class
////////////////////////////////////////////////////////////////////////////////////////////////////
CnotificationpersistentservicePersonalizationManager::CnotificationpersistentservicePersonalizationManager() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  m_uiUserId = 0;

  m_cCurrentPersonality.clear();

  m_pmPersonality = new(std::nothrow) Personality_Type();  // LCOV_EXCL_BR_LINE 11: except,C++ STL

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/// ~CnotificationpersistentservicePersonalizationManager
/// Destructor of CnotificationpersistentservicePersonalizationManager class
////////////////////////////////////////////////////////////////////////////////////////////////////
CnotificationpersistentservicePersonalizationManager::~CnotificationpersistentservicePersonalizationManager() {   // LCOV_EXCL_START 14: Resident process, global instance not released  // NOLINT[whitespace/line_length]
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (NULL != m_pmPersonality) {
    m_pmPersonality->clear();

    delete m_pmPersonality;
    m_pmPersonality = NULL;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}
// LCOV_EXCL_STOP

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceGetPersonality
/// This function is used to get the current personality.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CnotificationpersistentservicePersonalizationManager::NotificationpersistentserviceGetPersonality(std::string &f_cpersonalityname) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  f_cpersonalityname = m_cCurrentPersonality;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceSetPersonality
/// This function is used to set the new personality.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CnotificationpersistentservicePersonalizationManager::NotificationpersistentserviceSetPersonality(const std::string f_cpersonalityname) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // iterator to find personality name from map of personality
  Personality_Type::iterator l_itPersonality;

  if (!f_cpersonalityname.empty()) {
    if (NULL != m_pmPersonality) {  // LCOV_EXCL_BR_LINE 6: double check, m_pmPersonality can't be NULL
      l_itPersonality = m_pmPersonality->find(f_cpersonalityname);

      // if personality found in map
      if (m_pmPersonality->end() != l_itPersonality) {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Personality found in map");
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Personality not found in map");

        l_estatus = CreatePersonality(f_cpersonalityname);
      }

      m_cCurrentPersonality = f_cpersonalityname;
    } else {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "map object is NULL");  // LCOV_EXCL_LINE 6: m_pmPersonality can't be NULL
    }
  } else {
    l_estatus = eFrameworkunifiedStatusInvldParam;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// CreatePersonality
/// This function is used to create new Personality.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CnotificationpersistentservicePersonalizationManager::CreatePersonality(const std::string &f_cpersonalityname) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (NULL != m_pmPersonality) {  // LCOV_EXCL_BR_LINE 6: m_pmPersonality can't be NULL
    CNotificationpersistentservicePersonality *l_pPersonality = new(std::nothrow) CNotificationpersistentservicePersonality();  // LCOV_EXCL_BR_LINE 11: unexpected branch

    if (NULL != l_pPersonality) {  // LCOV_EXCL_BR_LINE 5: It's impossible to mock new() function
      l_pPersonality->m_uiUserId = ++m_uiUserId;
      l_pPersonality->m_cUserName = f_cpersonalityname;

      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Inserting new personality in map");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
      m_pmPersonality->insert(make_pair(f_cpersonalityname, l_pPersonality));
    }
  } else {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "map object is NULL");  // LCOV_EXCL_LINE 6: m_pmPersonality can't be NULL
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceIsValidPersonality
///
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CnotificationpersistentservicePersonalizationManager::NotificationpersistentserviceIsValidPersonality(const std::string f_cpersonalityname) {
  BOOL l_bReturnValue = FALSE;
  // iterator to find personality name from map of personality
  Personality_Type::iterator l_itPersonality;
  if (NULL != m_pmPersonality) {  // LCOV_EXCL_BR_LINE 6: m_pmPersonality can't be NULL
    l_itPersonality = m_pmPersonality->find(f_cpersonalityname);

    // if personality found in map
    if (m_pmPersonality->end() != l_itPersonality) {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Personality found in map");
      l_bReturnValue = TRUE;
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Personality not found in map");
    }
  } else {
    // LCOV_EXCL_START 6: m_pmPersonality can't be NULL
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "map object is NULL");
    // LCOV_EXCL_STOP
  }

  return l_bReturnValue;
}
