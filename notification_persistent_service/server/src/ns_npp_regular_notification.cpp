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
/// \brief    This file contains implementation of class CRegularNotification.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////


#include <native_service/frameworkunified_framework_if.h>
#include <native_service/ns_message_center_if.h>
#include <native_service/ns_np_service_protocol.h>
#include <mqueue.h>
#include <iostream>
#include <string>
#include <native_service/ns_mc_system_info.h>
#include "ns_npp_notificationpersistentservicelog.h"
#include "ns_npp_regular_notification.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// CRegularNotification
/// Constructor of CRegularNotification class
////////////////////////////////////////////////////////////////////////////////////////////////////
CRegularNotification::CRegularNotification(const std::string &f_cnotificationname,
                                           const UI_32 f_uimaxmsgsize):
  CNotification(f_cnotificationname, f_uimaxmsgsize) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  m_ePersistentType = eFrameworkunifiedNotificationVar;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/// ~CRegularNotification
/// Destructor of CRegularNotification class
////////////////////////////////////////////////////////////////////////////////////////////////////
CRegularNotification::~CRegularNotification() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");


  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/// AddEventReciever
/// This function adds the name of the application to receiver list of particular notification.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CRegularNotification::AddEventReciever(const std::string &f_csubscribername) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (!f_csubscribername.empty()) {  // LCOV_EXCL_BR_LINE 6: double check, f_csubscribername can't be empty
    l_estatus = AddReceiverInMap(f_csubscribername);
  } else {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_estatus = eFrameworkunifiedStatusInvldParam;  // LCOV_EXCL_LINE 6: f_csubscribername can't be empty
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/// Publish
/// This function publishes the notification to subscribed clients.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CRegularNotification::Publish(const std::string &f_cservicename,
                                         PVOID f_pmessage,
                                         const UI_32 f_uimsgsize) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // Message Queue Handle
  HANDLE l_hMsgQHandle = NULL;

  // Pointer to class CNotificationReceiver
  CNotificationReceiver *l_pNotificationReceiver = NULL;

  // Iterator for Notification Receiver map
  NotifReceiver_Iterator_Type l_itrNotifReceiver;

  if (m_uiMaxMsgSize >= f_uimsgsize) {
    if (0 == m_cServiceName.compare(f_cservicename)) {
      for (l_itrNotifReceiver = m_pmSubscribersList->begin();
           l_itrNotifReceiver != m_pmSubscribersList->end();
           l_itrNotifReceiver++) {
        l_pNotificationReceiver = l_itrNotifReceiver->second;

        if (NULL != l_pNotificationReceiver) {  // LCOV_EXCL_BR_LINE 6: l_pNotificationReceiver can't be NULL
          l_hMsgQHandle = l_pNotificationReceiver->m_MsgQHandle;

          if (NULL != l_hMsgQHandle) {  // LCOV_EXCL_BR_LINE 6: l_hMsgQHandle can't be NULL
            if (eFrameworkunifiedStatusOK != (l_estatus = PublishData(l_hMsgQHandle, f_pmessage, f_uimsgsize))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
              // LCOV_EXCL_START 4: NSFW error case
              AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
              FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                     "Error Publishing notification %s to %s published by %s, error status: 0x%x",
                     m_cNotificationName.c_str(), l_itrNotifReceiver->first.c_str(), f_cservicename.c_str(), l_estatus);
              // LCOV_EXCL_STOP
            }
          } else {
            // LCOV_EXCL_START 6: l_hMsgQHandle can't be NULL
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "MsgQ Handle NULL");
            l_estatus = eFrameworkunifiedStatusNullPointer;
            // LCOV_EXCL_STOP
          }
        } else {
          // LCOV_EXCL_START 6: l_pNotificationReceiver can't be NULL
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "l_pNotificationReceiver is NULL");
          l_estatus = eFrameworkunifiedStatusNullPointer;
          // LCOV_EXCL_STOP
        }
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Can't publish notification %s. Registered by %s and published by %s.",
             m_cNotificationName.c_str(), m_cServiceName.c_str(), f_cservicename.c_str());  // LCOV_EXCL_BR_LINE 15: marco defined in "native_service/ns_logger_if.h"
      l_estatus = eFrameworkunifiedStatusInvldParam;
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "Can't publish notification %s. Message data size (%d) is greater than maximum registered data size (%d)",
           m_cNotificationName.c_str(), f_uimsgsize, m_uiMaxMsgSize);
    l_estatus = eFrameworkunifiedStatusFail;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}
