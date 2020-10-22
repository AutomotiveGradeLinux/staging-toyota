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
/// \brief    This file contains implementation of class CStateNotification.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <string>
#include "ns_npp_notificationpersistentservicelog.h"
#include "ns_npp_persistent_data.h"
#include "ns_npp_state_notification.h"



////////////////////////////////////////////////////////////////////////////////////////////////////
/// CStateNotification
/// Constructor of CStateNotification class
////////////////////////////////////////////////////////////////////////////////////////////////////
CStateNotification::CStateNotification(const std::string &f_cnotificationname,
                                       const UI_32 f_uimaxmsgsize):
  CNotification(f_cnotificationname, f_uimaxmsgsize),
  m_pData(NULL),
  m_pDefaultData(NULL),
  m_bWasPublished(FALSE) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  m_ePersistentType = eFrameworkunifiedStateVar;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/// ~CStateNotification
/// Destructor of CStateNotification class
////////////////////////////////////////////////////////////////////////////////////////////////////
CStateNotification::~CStateNotification() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (NULL != m_pData) {
    delete m_pData;   // LCOV_EXCL_BR_LINE 11: unexpected branch
    m_pData = NULL;
  }

  if (NULL != m_pDefaultData) {
    delete m_pDefaultData;
    m_pDefaultData = NULL;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}
////////////////////////////////////////////////////////////////////////////////////////////////////
/// GetPersistentData
/// This function is used to get the persistent data pointer
////////////////////////////////////////////////////////////////////////////////////////////////////
const CPersistentData *CStateNotification::GetPersistentData() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");

  return m_pData;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// SetPersistentData
/// This function is used to set the data related to notification
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CStateNotification::SetPersistentData(PVOID f_pmessage,
                                                 const UI_32 f_msgsize) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (NULL == m_pData) {
    m_pData = new(std::nothrow) CPersistentData();  // LCOV_EXCL_BR_LINE 11: unexpected branch
  }

  l_estatus = SetData(m_pData, f_pmessage, f_msgsize);

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// GetDefaultPersistentData
/// This function get the default data(if any) related to notification
////////////////////////////////////////////////////////////////////////////////////////////////////
const CPersistentData *CStateNotification::GetDefaultPersistentData() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");

  return m_pDefaultData;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// SetDefaultPersistentData
/// This function is used to set the default data related to notification
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CStateNotification::SetDefaultPersistentData(PVOID f_pmessage,
                                                        const UI_32 f_msgsize) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (NULL == m_pDefaultData) {
    m_pDefaultData = new(std::nothrow) CPersistentData();  // LCOV_EXCL_BR_LINE 11: except,C++ STL
  }

  // set the default persistent data
  if (eFrameworkunifiedStatusOK == (l_estatus = SetData(m_pDefaultData, f_pmessage, f_msgsize))) {
    if (NULL == m_pData) {
      // set the default data as persistent data if notification is not yet published
      l_estatus = SetPersistentData(f_pmessage, f_msgsize);
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Setting Persistent data");
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Persistent data already set");
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error setting default persistent data for notification:: %s",
           m_cNotificationName.c_str());
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// SetData
/// This function is used to set the persistent data
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CStateNotification::SetData(CPersistentData *f_pdata,
                                       PVOID f_pmessage,
                                       const UI_32 f_msgsize) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (NULL != f_pdata) {  // LCOV_EXCL_BR_LINE 6: double check, f_pdata can't be NULL
    if (m_uiMaxMsgSize >= f_msgsize) {
      f_pdata->m_uiMsgSize = f_msgsize;

      if (NULL != f_pmessage && 0 != f_msgsize) {
        if (NULL == f_pdata->m_pMessage) {
          f_pdata->m_pMessage = new(std::nothrow) CHAR[m_uiMaxMsgSize];
        }

        if (NULL != f_pdata->m_pMessage) {  // LCOV_EXCL_BR_LINE 5: new error case
          std::memset(f_pdata->m_pMessage, 0, m_uiMaxMsgSize);
          std::memcpy(f_pdata->m_pMessage, f_pmessage, f_msgsize);
        } else {
          // LCOV_EXCL_START 5: new error case
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Memory allocation error, unable to set persistent data for notification: %s",
                 m_cNotificationName.c_str());
          l_estatus = eFrameworkunifiedStatusNullPointer;
          // LCOV_EXCL_STOP
        }
      } else {
        if (NULL != f_pdata->m_pMessage) {
          delete[](static_cast<PCHAR>(f_pdata->m_pMessage));
          f_pdata->m_pMessage = NULL;
        }
      }

      m_bWasPublished = TRUE;
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Can't set notification data. Message data size (%d) is greater than maximum registered data size (%d)"
             " of notification (%s).", f_msgsize, m_uiMaxMsgSize, GetNotificationName().c_str());
      l_estatus = eFrameworkunifiedStatusFail;
    }
  } else {
    // LCOV_EXCL_START 6: double check, f_pdata can't be NULL
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Unable to set persistent data for notification: %s, persistent object is NULL",
           m_cNotificationName.c_str());
    l_estatus = eFrameworkunifiedStatusNullPointer;
    // LCOV_EXCL_STOP
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// AddEventReciever
/// This function adds the name of the application to receiver list of particular notification.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CStateNotification::AddEventReciever(const std::string &f_csubscribername) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // Message Queue Handle
  HANDLE l_hMsgQHandle = NULL;

  // iterator of CNotificationReceiver map
  NotifReceiver_Iterator_Type l_iterator;

  CNotificationReceiver *l_pNotificationReceiver = NULL;

  if (!f_csubscribername.empty()) {  // LCOV_EXCL_BR_LINE 6: double check, f_csubscribername can't be empty
    l_estatus = AddReceiverInMap(f_csubscribername);
  } else {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_estatus = eFrameworkunifiedStatusInvldParam;  // LCOV_EXCL_LINE 6: double check, f_csubscribername can't be empty
  }

  if (eFrameworkunifiedStatusOK == l_estatus) {  // LCOV_EXCL_BR_LINE 6: double check, l_estatus must be eFrameworkunifiedStatusOK
    if (m_bWasPublished) {
      FRAMEWORKUNIFIEDLOG(ZONE_NPP_INFO, __FUNCTION__, "Republish Notification to subscriber :: %s", f_csubscribername.c_str());  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]

      if (NULL != m_pData) {
        l_iterator = m_pmSubscribersList->find(f_csubscribername);

        if (m_pmSubscribersList->end() != l_iterator) {
          l_pNotificationReceiver = (*l_iterator).second;

          if (NULL != l_pNotificationReceiver) {  // LCOV_EXCL_BR_LINE 6: double check, l_pNotificationReceiver can't be NULL  // NOLINT[whitespace/line_length]
            l_hMsgQHandle = l_pNotificationReceiver->m_MsgQHandle;

            if (NULL != l_hMsgQHandle) {  // LCOV_EXCL_BR_LINE 6: double check, l_hMsgQHandle can't be NULL
              if (eFrameworkunifiedStatusOK != (l_estatus = PublishData(l_pNotificationReceiver->m_MsgQHandle, m_pData->m_pMessage, m_pData->m_uiMsgSize))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
                // LCOV_EXCL_START 4: NSFW error case
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                       "Error Publishing notification %s to %s published by %s, error status: 0x%x",
                       m_cNotificationName.c_str(), l_iterator->first.c_str(), m_cServiceName.c_str(), l_estatus);
                // LCOV_EXCL_STOP
              }
            } else {
              // LCOV_EXCL_START 6: double check, l_hMsgQHandle can't be NULL
              AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
              FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "MsgQ Handle NULL");
              l_estatus = eFrameworkunifiedStatusNullPointer;
              // LCOV_EXCL_STOP
            }
          } else {
            // LCOV_EXCL_START 6: double check, l_pNotificationReceiver can't be NULL
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "l_pNotificationReceiver is NULL");
            l_estatus = eFrameworkunifiedStatusNullPointer;
            // LCOV_EXCL_STOP
          }
        }
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "m_pData pointer is NULL");
        l_estatus = eFrameworkunifiedStatusNullPointer;
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "Subscribing Notfn, src=%s name=%s. Notification not published.",
             f_csubscribername.c_str(), m_cNotificationName.c_str());
    }
  } else {
    // LCOV_EXCL_START 6: double check, l_estatus must be eFrameworkunifiedStatusOK
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_estatus = eFrameworkunifiedStatusFail;
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Receiver Not Added in map");
    // LCOV_EXCL_STOP
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/// Publish
/// This function publishes the notification to subscribed clients.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CStateNotification::Publish(const std::string &f_cservicename,
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
      l_estatus = SetPersistentData(f_pmessage,
                                    f_uimsgsize);

      if (eFrameworkunifiedStatusOK == l_estatus) {  // LCOV_EXCL_BR_LINE 6: l_estatus is always eFrameworkunifiedStatusOK
        for (l_itrNotifReceiver = m_pmSubscribersList->begin();
             l_itrNotifReceiver != m_pmSubscribersList->end();
             l_itrNotifReceiver++) {
          l_pNotificationReceiver = l_itrNotifReceiver->second;

          if (NULL != l_pNotificationReceiver) {  // LCOV_EXCL_BR_LINE 6:double check, l_pNotificationReceiver can not be null  // NOLINT[whitespace/line_length]
            l_hMsgQHandle = l_pNotificationReceiver->m_MsgQHandle;

            if (NULL != l_hMsgQHandle && NULL != m_pData) {  // LCOV_EXCL_BR_LINE 6:double check, l_hMsgQHandle and m_pData can not be null  // NOLINT[whitespace/line_length]
              if (eFrameworkunifiedStatusOK != (l_estatus = PublishData(l_hMsgQHandle, m_pData->m_pMessage, f_uimsgsize))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
                // LCOV_EXCL_START 4: NSFW error case
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                       "Error Publishing notification %s to %s published by %s, error status: 0x%x",
                       m_cNotificationName.c_str(),
                       l_itrNotifReceiver->first.c_str(),
                       f_cservicename.c_str(), l_estatus);
                // LCOV_EXCL_STOP
              }
            } else {
              // LCOV_EXCL_START 6: double check, l_hMsgQHandle and m_pData can not be null
              AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
              FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "MsgQ Handle NULL");
              l_estatus = eFrameworkunifiedStatusNullPointer;
              // LCOV_EXCL_STOP
            }
          } else {
            // LCOV_EXCL_START 6: double check, l_pNotificationReceiver can not be null
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "l_pNotificationReceiver is NULL");
            l_estatus = eFrameworkunifiedStatusNullPointer;
            // LCOV_EXCL_STOP
          }
        }
      } else {
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Persistent Data Not Set");  // LCOV_EXCL_LINE 6: double check, l_estatus is always eFrameworkunifiedStatusOK  // NOLINT[whitespace/line_length]
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Can't publish notification %s. Registered by %s and published by %s.",
             GetNotificationName().c_str(), m_cServiceName.c_str(), f_cservicename.c_str());
      l_estatus = eFrameworkunifiedStatusInvldParam;
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "Can't publish notification %s. Message data size (%d) is greater than maximum registered data size (%d)",
           GetNotificationName().c_str(), f_uimsgsize, m_uiMaxMsgSize);
    l_estatus = eFrameworkunifiedStatusFail;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// IsPublished
/// This functions returns the published status of notification.
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CStateNotification::IsPublished() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  return m_bWasPublished;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// ResetMaxMessageSize
/// This function reset the max size of data that can be published with notification.
/// Also deletes the old persistent data and default data.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CStateNotification::ResetMaxMessageSize(const UI_32 f_uilength) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  CNotification::ResetMaxMessageSize(f_uilength);

  // delete the old data as the size is changed the data may become invalid
  if (NULL != m_pData) {
    delete m_pData;
    m_pData = NULL;
  }

  if (NULL != m_pDefaultData) {
    delete m_pDefaultData;
    m_pDefaultData = NULL;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}
