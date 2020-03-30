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
/// \brief    This file contains implementation of class CStatePersistenceUserNotification.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <string>
#include "ns_npp_notificationpersistentservicelog.h"
#include "ns_npp_state_persistence_user_notification.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// CStatePersistenceUserNotification
/// Constructor of CStatePersistenceUserNotification class
////////////////////////////////////////////////////////////////////////////////////////////////////
CStatePersistenceUserNotification::CStatePersistenceUserNotification(const std::string &f_cnotificationname,
                                                                     const UI_32 f_uimaxmsgsize):
  CStateNotification(f_cnotificationname, f_uimaxmsgsize) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  m_ePersistentType = eFrameworkunifiedPersistedStateUserVar;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/// ~CStatePersistenceUserNotification
/// Destructor of CStatePersistenceUserNotification class
////////////////////////////////////////////////////////////////////////////////////////////////////
CStatePersistenceUserNotification::~CStatePersistenceUserNotification() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");


  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}
