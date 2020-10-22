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
/// \brief    This file contains implementation of class CStatePersistenceNotification.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <string>
#include "ns_npp_notificationpersistentservicelog.h"
#include "ns_npp_state_persistence_notification.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// CStatePersistenceNotification
/// Constructor of CStatePersistenceNotification class
////////////////////////////////////////////////////////////////////////////////////////////////////
CStatePersistenceNotification::CStatePersistenceNotification(const std::string &f_cnotificationname,
                                                             const UI_32 f_uimaxmsgsize,
                                                             const EFrameworkunifiedPersistCategory f_epersistcategory):
  CStateNotification(f_cnotificationname, f_uimaxmsgsize),
  m_ePersistCategory(f_epersistcategory) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  m_ePersistentType = eFrameworkunifiedPersistedStateVar;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// ~CStatePersistenceNotification
/// Constructor of CStatePersistenceNotification class
////////////////////////////////////////////////////////////////////////////////////////////////////
CStatePersistenceNotification::~CStatePersistenceNotification() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");


  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// GetPersistentCategory
/// Gets the persist type of notification.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedPersistCategory CStatePersistenceNotification::GetPersistentCategory() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return m_ePersistCategory;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// SetPersistentCategory
/// Sets the persist type of notification.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CStatePersistenceNotification::SetPersistentCategory(const EFrameworkunifiedPersistCategory f_epersistcategory) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  m_ePersistCategory = f_epersistcategory;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}
