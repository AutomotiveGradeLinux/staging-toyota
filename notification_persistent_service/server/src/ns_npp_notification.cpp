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
/// \ingroup  tag_NS_NPPService
/// \brief    This file contains implementation of CNotification class.
///       It define functionalities common to all types of notification.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <native_service/frameworkunified_framework_if.h>
#include <native_service/ns_message_center_if.h>
#include <native_service/ns_np_service_protocol.h>
#include <utility>
#include <iostream>
#include <string>
#include <native_service/ns_mc_system_info.h>
#include "ns_npp_notificationpersistentservicelog.h"
#include "ns_npp_handlelist.h"
#include "ns_npp_notification.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
/// CNotification
/// Constructor of CNotification class
////////////////////////////////////////////////////////////////////////////////////////////////////
CNotification::CNotification() {  // LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  m_cNotificationName = "";
  m_uiMaxMsgSize = 0;
  m_ePersistentType = eFrameworkunifiedUnknown;

  m_cServiceName = "";

  m_pmSubscribersList = NULL;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}
// LCOV_EXCL_STOP

////////////////////////////////////////////////////////////////////////////////////////////////////
/// CNotification
/// Constructor of CNotification class
////////////////////////////////////////////////////////////////////////////////////////////////////
CNotification::CNotification(const std::string &f_cnotificationname,
                             const UI_32 f_uimaxmsgsize) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  m_cNotificationName = "";
  m_uiMaxMsgSize = 0;
  m_ePersistentType = eFrameworkunifiedUnknown;
  m_pmSubscribersList = NULL;

  if (!f_cnotificationname.empty()) {  // LCOV_EXCL_BR_LINE 200: f_cnotificationname can't be empty
    m_cNotificationName = f_cnotificationname;
    m_uiMaxMsgSize = f_uimaxmsgsize;
//    m_ePersistentType = eFrameworkunifiedUnknown;

    m_cServiceName = "";

    m_pmSubscribersList = new(std::nothrow) NotifReceiver_Type();  // LCOV_EXCL_BR_LINE 11:unexpected branch
  } else {
    // LCOV_EXCL_START 200: f_cnotificationname can't be empty
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NPP_INFO, __FUNCTION__, "Notification String is Empty");

    CNotification();
    // LCOV_EXCL_STOP
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/// ~CNotification
/// Destructor of CNotification class
////////////////////////////////////////////////////////////////////////////////////////////////////
CNotification::~CNotification() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // iterator of CNotificationReceiver map
  NotifReceiver_Iterator_Type l_itNotifReceiver;

  CNotificationReceiver *l_pNotificationReceiver = NULL;

  if (NULL != m_pmSubscribersList) {  // LCOV_EXCL_BR_LINE 6: m_pmSubscribersList can't be NULL
    if (!m_pmSubscribersList->empty()) {
      for (l_itNotifReceiver = m_pmSubscribersList->begin();
           l_itNotifReceiver != m_pmSubscribersList->end();
           l_itNotifReceiver++) {
        l_pNotificationReceiver = (*l_itNotifReceiver).second;

        if (NULL != l_pNotificationReceiver) {  // LCOV_EXCL_BR_LINE 6: l_pNotificationReceiver can't be NULL
          // NOTE: Uncomment following code when implementation of the function RemoveHandleFromList
          // is available. To find the reason why it is not implemented, see the function definition.
          // CHandleList *l_hHandleList = CHandleList::GetHandleList();
          // l_hHandleList->RemoveHandleFromList((*l_itNotifReceiver).first);

          delete l_pNotificationReceiver;    // LCOV_EXCL_BR_LINE 11: unexpected branch
          l_pNotificationReceiver = NULL;
        }
      }

      // clear the map
      m_pmSubscribersList->clear();
    }

    delete m_pmSubscribersList;    // LCOV_EXCL_BR_LINE 11: unexpected branch
    m_pmSubscribersList = NULL;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// AddEventReciever
/// This function adds the name of the application to receiver list of particular notification.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNotification::AddEventReciever(const std::string &f_csubscribername) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (!f_csubscribername.empty()) {  // LCOV_EXCL_BR_LINE 6: f_csubscribername can't be empty
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
EFrameworkunifiedStatus CNotification::Publish(const std::string &f_cservicename,
                                  PVOID f_pmessage,
                                  const UI_32 f_uimsgsize) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eFrameworkunifiedStatusFail;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// DeleteEventReciever
/// This function deletes the name of application from receivers list.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNotification::DeleteEventReciever(const std::string &f_csubscribername) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // iterator of CNotificationReceiver map
  NotifReceiver_Iterator_Type l_iterator;

  CNotificationReceiver *l_pNotificationReceiver = NULL;

  if (!f_csubscribername.empty()) {  // LCOV_EXCL_BR_LINE 6: double check, f_csubscribername can't be empty
    l_iterator = m_pmSubscribersList->find(f_csubscribername);


    if (l_iterator != m_pmSubscribersList->end()) {
      l_pNotificationReceiver = (*l_iterator).second;

      if (NULL != l_pNotificationReceiver) {  // LCOV_EXCL_BR_LINE 6: l_pNotificationReceiver can't be NULL
        // NOTE: Uncomment following code when implementation of the function RemoveHandleFromList
        // is available. To find the reason why it is not implemented, see the function definition.
        // CHandleList *l_hHandleList = CHandleList::GetHandleList();
        // l_hHandleList->RemoveHandleFromList((*l_iterator).first);

        delete l_pNotificationReceiver;  // LCOV_EXCL_BR_LINE 11: unexpected branch
        l_pNotificationReceiver = NULL;
      }

      m_pmSubscribersList->erase(l_iterator);
      FRAMEWORKUNIFIEDLOG(ZONE_NPP_INFO, __FUNCTION__, "Subscriber successfully deleted");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Invalid Iterator");
    }
  } else {
    // LCOV_EXCL_START 6: double check, f_csubscribername can't be empty
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Subscriber Name String is Empty");
    l_estatus = eFrameworkunifiedStatusFail;
    // LCOV_EXCL_STOP
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// SetNewSubscribersList
/// This function sets the subscribers list of notification
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNotification::SetNewSubscribersList(CNotification *f_pnotification) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // iterator of CNotificationReceiver map
  NotifReceiver_Iterator_Type l_itNotifReceiver;

  std::string l_cSubscriberName;

  if (NULL != f_pnotification && NULL != f_pnotification->m_pmSubscribersList) {  // LCOV_EXCL_BR_LINE 6: f_pnotification and f_pnotification->m_pmSubscribersList can't be empty  // NOLINT[whitespace/line_length]
    if (!f_pnotification->m_pmSubscribersList->empty()) {
      for (l_itNotifReceiver = f_pnotification->m_pmSubscribersList->begin();
           l_itNotifReceiver != f_pnotification->m_pmSubscribersList->end();
           l_itNotifReceiver++) {
        l_cSubscriberName = (*l_itNotifReceiver).first;
        AddReceiverInMap(l_cSubscriberName);  // LCOV_EXCL_BR_LINE 11: unexpected branch
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Subscriber List Empty");
    }
  } else {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_estatus = eFrameworkunifiedStatusNullPointer;  // LCOV_EXCL_LINE 6: f_pnotification and f_pnotification->m_pmSubscribersList can't be empty  // NOLINT[whitespace/line_length]
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// SetEventPublisher
/// This function set the publisher name to current received service name
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNotification::SetEventPublisher(const std::string &f_cservicename) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (!f_cservicename.empty()) {
    m_cServiceName = f_cservicename;
    FRAMEWORKUNIFIEDLOG(ZONE_NPP_INFO, __FUNCTION__, "Service successfully set");  // LCOV_EXCL_BR_LINE 15: marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Service Name String is Empty");
    l_estatus = eFrameworkunifiedStatusFail;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/// ResetEventPublisher
/// This function resets the publisher name
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNotification::ResetEventPublisher(const std::string &f_cservicename) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (f_cservicename.empty()) {  // LCOV_EXCL_BR_LINE 6: f_cservicename can't be empty
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_estatus = eFrameworkunifiedStatusInvldParam;  // LCOV_EXCL_LINE 6: f_cservicename can't be empty
  } else {
    if (0 == m_cServiceName.compare(f_cservicename)) {
      m_cServiceName = "";
      FRAMEWORKUNIFIEDLOG(ZONE_NPP_INFO, __FUNCTION__, "Service successfully reset");  // LCOV_EXCL_BR_LINE 15: marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Service  Name Not Registered");  // LCOV_EXCL_BR_LINE 15: marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
      l_estatus = eFrameworkunifiedStatusFail;
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

EFrameworkunifiedStatus CNotification::AddReceiverInMap(const std::string &f_csubscribername) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // Message Queue Handle for passsing messages
  HANDLE l_hMsgSenQueHandle = NULL;

  // Pointer to class CNotificationReceiver
  CNotificationReceiver *l_pCNotificationReceiver = NULL;

  // Iterator for Notification Receiver map
  NotifReceiver_Iterator_Type l_itrNotifReceiver;

  if (!f_csubscribername.empty()) {  // LCOV_EXCL_BR_LINE 6: f_csubscribername can't be empty
    FRAMEWORKUNIFIEDLOG(ZONE_NPP_INFO, __FUNCTION__, "Message Queue Handle Set");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]

    CHandleList *l_hHandleList = CHandleList::GetHandleList();

    l_hMsgSenQueHandle = l_hHandleList->GetSubscriberMqHandle(f_csubscribername);  // LCOV_EXCL_BR_LINE 11: unexpected branch  // NOLINT[whitespace/line_length]

    // If handle not found
    if (NULL == l_hMsgSenQueHandle) {
      // Open a handle for sending messages to another message queue
      l_hMsgSenQueHandle = McOpenSender(f_csubscribername.c_str());
      l_hHandleList->AddHandleInList(f_csubscribername, l_hMsgSenQueHandle);  // LCOV_EXCL_BR_LINE 11: unexpected branch  // NOLINT[whitespace/line_length]
    }

    if (NULL !=  m_pmSubscribersList) {  // LCOV_EXCL_BR_LINE 6: m_pmSubscribersList can't be NULL
      l_pCNotificationReceiver = new(std::nothrow) CNotificationReceiver();  // LCOV_EXCL_BR_LINE 11: unexpected branch

      if (NULL != l_pCNotificationReceiver) {  // LCOV_EXCL_BR_LINE 5: l_pCNotificationReceiver can't be NULL
        l_pCNotificationReceiver->m_MsgQHandle = l_hMsgSenQueHandle;

        // Insert Subscriber name and associated message queue handle in the map
        m_pmSubscribersList->insert(make_pair(f_csubscribername, l_pCNotificationReceiver));
      } else {
        // LCOV_EXCL_START 5: l_pCNotificationReceiver can't be NULL
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "l_pCNotificationReceiver is NULL");
        l_estatus = eFrameworkunifiedStatusNullPointer;
        // LCOV_EXCL_STOP
      }
    } else {
      // LCOV_EXCL_START 6: m_pmSubscribersList can't be NULL
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "m_pmSubscribersList is NULL");
      l_estatus = eFrameworkunifiedStatusNullPointer;
      // LCOV_EXCL_STOP
    }

    if (eFrameworkunifiedStatusNullPointer == l_estatus) {
      // close the queue handle
      // NOTE: Uncomment following code when implementation of the function RemoveHandleFromList
      // is available. To find the reason why it is not implemented, see the function definition.
      // CHandleList *l_hHandleList = CHandleList::GetHandleList();
      // l_hHandleList->RemoveHandleFromList(f_csubscribername);
    }
  } else {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_estatus = eFrameworkunifiedStatusInvldParam;  // LCOV_EXCL_LINE 6: f_csubscribername can't be empty
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// PublishData
/// This function publishes the notification to client.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNotification::PublishData(HANDLE f_hmsgqhandle,
                                      const PVOID f_pmessage,
                                      const UI_32 f_uimsgsize) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (NULL != f_hmsgqhandle) {  // LCOV_EXCL_BR_LINE 6: f_hmsgqhandle can't be NULL
    // Notification Name
    PSTR l_cNotificationName = const_cast<PSTR>(m_cNotificationName.c_str());

    if (eFrameworkunifiedStatusOK != (l_estatus = McSendWithSysInfo(f_hmsgqhandle, AppName, NPS_NOTIFY_EV_REQ, l_cNotificationName, f_uimsgsize, f_pmessage, 0))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
      // LCOV_EXCL_START 4: NSFW error case.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "PublishData failed while McSend for Notification %s, Error Status: 0x%x",
             l_cNotificationName, l_estatus);
      // LCOV_EXCL_STOP
    }
  } else {
    // LCOV_EXCL_START 6: f_hmsgqhandle can't be NULL
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "MsgQ Handle NULL");
    l_estatus = eFrameworkunifiedStatusNullPointer;
    // LCOV_EXCL_STOP
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// IsServiceRegistered
/// This function checks whether the notification is registered with any service or not.
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CNotification::IsServiceRegistered() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (!m_cServiceName.empty()) {
    return TRUE;
  } else {
    return FALSE;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// IsSubscribersListEmpty
/// This function is used to check whether any service is subscribed to notification
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CNotification::IsSubscribersListEmpty() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (NULL != m_pmSubscribersList) {  // LCOV_EXCL_BR_LINE 6: m_pmSubscribersList can't be NULL
    if (m_pmSubscribersList->empty()) {
      return TRUE;
    }
  }

  return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// GetNotificationName
/// This function is used to get the notification name.
////////////////////////////////////////////////////////////////////////////////////////////////////
std::string CNotification::GetNotificationName() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  return m_cNotificationName;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// GetPublisherName
/// This function is used to get the publisher name of notification.
////////////////////////////////////////////////////////////////////////////////////////////////////
std::string CNotification::GetPublisherName() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  return m_cServiceName;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// GetPersistenceType
/// This function is used to get the type of notification.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedNotificationType CNotification::GetNotificationType() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  return m_ePersistentType;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// GetMaxMessageSize
/// This function is used to get the max size of data of notification message.
////////////////////////////////////////////////////////////////////////////////////////////////////
UI_32 CNotification::GetMaxMessageSize() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  return m_uiMaxMsgSize;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// ResetMaxMessageSize
/// This function reset the max size of data that can be published with notification.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNotification::ResetMaxMessageSize(const UI_32 f_uilength) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  m_uiMaxMsgSize = f_uilength;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

#ifdef NPP_PROFILEINFO_ENABLE

////////////////////////////////////////////////////////////////////////////////////////////////
/// GetSubscriberList
/// Returns the list of subscribers subscribed to notification
////////////////////////////////////////////////////////////////////////////////////////////////
NotifReceiver_Type *CNotification::GetSubscriberList() {
  return m_pmSubscribersList;
}

#endif
