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
/// \brief    This file contains implementation of singleton class CNotificationManager.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <native_service/ns_np_service.h>
#include <native_service/ns_message_center_if.h>
#include <native_service/ns_np_service_protocol.h>
#include <algorithm>
#include <utility>
#include <string>
#include <vector>
#include "ns_npp_types.h"
#include "ns_npp_notificationpersistentservicelog.h"
#include "ns_npp_threads.h"
#include "ns_npp_notification.h"
#include "ns_npp_persistent_data.h"
#include "ns_npp_state_notification.h"
#include "ns_npp_regular_notification.h"
#include "ns_npp_notification_manager.h"
#include "ns_npp_state_persistence_notification.h"
#include "ns_npp_state_nor_persistence_notification.h"
#include "ns_npp_state_persistence_user_notification.h"

#ifdef NPP_PROFILEINFO_ENABLE
#include <sstream>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
/// CNotificationManager
/// Constructor of CNotificationManager class
////////////////////////////////////////////////////////////////////////////////////////////////////
CNotificationManager::CNotificationManager() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  m_pmNotificationList = new(std::nothrow) Notification_Type();  // LCOV_EXCL_BR_LINE 11: unexpected branch

  m_pvPersistentList = new(std::nothrow) std::vector<std::string>();  // LCOV_EXCL_BR_LINE 11: unexpected branch

  m_pvUserPersistentList = new(std::nothrow) std::vector<std::string>();  // LCOV_EXCL_BR_LINE 11: unexpected branch

  if (NULL == CStateNorPersistenceNotification::m_hNSImmediatePersistenceThread) {  // LCOV_EXCL_BR_LINE 11: unexpected branch  // NOLINT[whitespace/line_length]
    CStateNorPersistenceNotification::m_hNSImmediatePersistenceThread =
            McOpenSender(NS_NPP_IMMEDIATE_PERSIST_THREAD_NAME);
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// ~CNotificationManager
/// Destructor of CNotificationManager class
////////////////////////////////////////////////////////////////////////////////////////////////////
CNotificationManager::~CNotificationManager() {   // LCOV_EXCL_START 14: Resident process, global instance not released
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // iterator to find notification from map of notifications
  Notification_Iterator_Type l_itNotification;

  CNotification *l_pNotification = NULL;

  // delete the members in map of Notification
  if (NULL != m_pmNotificationList) {
    if (!m_pmNotificationList->empty()) {
      for (l_itNotification = m_pmNotificationList->begin();
           l_itNotification != m_pmNotificationList->end();
           l_itNotification++) {
        l_pNotification = (*l_itNotification).second;

        if (NULL != l_pNotification) {
          delete l_pNotification;
          l_pNotification = NULL;
        }
      }

      // clear the map
      m_pmNotificationList->clear();
    }

    delete m_pmNotificationList;
    m_pmNotificationList = NULL;
  }

  // delete the members in vector of persistent notification
  if (NULL != m_pvPersistentList) {
    if (!m_pvPersistentList->empty()) {
      // clear the vector
      m_pvPersistentList->clear();
    }

    delete m_pvPersistentList;
    m_pvPersistentList = NULL;
  }

  // delete the members in vector of user persistent notification
  if (NULL != m_pvUserPersistentList) {
    if (!m_pvUserPersistentList->empty()) {
      // clear the vector
      m_pvUserPersistentList->clear();
    }

    delete m_pvUserPersistentList;
    m_pvUserPersistentList = NULL;
  }

  if (NULL != CStateNorPersistenceNotification::m_hNSImmediatePersistenceThread) {
    McClose(CStateNorPersistenceNotification::m_hNSImmediatePersistenceThread);
    CStateNorPersistenceNotification::m_hNSImmediatePersistenceThread = NULL;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}
// LCOV_EXCL_STOP

////////////////////////////////////////////////////////////////////////////////////////////////////
/// CreateNotificationObject
/// This function is used to get notification object from map. If doesn't exists, it create new
/// object as per type of notification.
////////////////////////////////////////////////////////////////////////////////////////////////////
CNotification *CNotificationManager::CreateNotificationObject(const std::string &f_cnotificationname,
                                                              const UI_32 f_uimsglength,
                                                              const EFrameworkunifiedNotificationType f_enotificationtype,
                                                              const UI_32 f_uidelay) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // pointer of notification object
  CNotification *l_pNotification = NULL;

  // iterator to find notification from map of CNotification
  Notification_Iterator_Type l_itNotification;

  if (NULL == m_pmNotificationList || NULL == m_pvPersistentList || NULL == m_pvUserPersistentList) {  // LCOV_EXCL_START 6: double check, m_pmNotificationList, m_pvPersistentList, m_pvUserPersistentList can't be NULL  // NOLINT[whitespace/line_length]
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "map/vector object is NULL");  // LCOV_EXCL_LINE 6: m_pmNotificationList, m_pvPersistentList, m_pvUserPersistentList can't be NULL  // NOLINT[whitespace/line_length]
    return NULL;
  }
  // LCOV_EXCL_STOP

  // check if notification already exists
  l_itNotification = m_pmNotificationList->find(f_cnotificationname);

  // if exists remove previous entry
  if (m_pmNotificationList->end() != l_itNotification) {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "Removing previous entry of notification %s from map", f_cnotificationname.c_str());
    m_pmNotificationList->erase(l_itNotification);
  }

  switch (f_enotificationtype) {
    // if notification is regular notification
    case eFrameworkunifiedNotificationVar: {
        FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "Regular Notification :: %s", f_cnotificationname.c_str());
        l_pNotification = new(std::nothrow) CRegularNotification(f_cnotificationname,
                                                                 f_uimsglength);  // LCOV_EXCL_BR_LINE 11: unexpected branch  // NOLINT[whitespace/line_length]
        break;
      }

    // if notification is state notification
    case eFrameworkunifiedStateVar: {
        FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "State Notification :: %s", f_cnotificationname.c_str());
        l_pNotification = new(std::nothrow) CStateNotification(f_cnotificationname,
                                                               f_uimsglength);  // LCOV_EXCL_BR_LINE 11: unexpected branch  // NOLINT[whitespace/line_length]
        break;
      }

    // if notification is persistence notification
    case eFrameworkunifiedPersistedStateVar: {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "PersistedState Notification :: %s", f_cnotificationname.c_str());  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
        l_pNotification = new(std::nothrow) CStatePersistenceNotification(f_cnotificationname,
                                                                          f_uimsglength);  // LCOV_EXCL_BR_LINE 11: unexpected branch  // NOLINT[whitespace/line_length]

        if (NULL != l_pNotification) {  // LCOV_EXCL_BR_LINE 5: It's impossible to mock new() function
          // insert notification in persistent notification vector
          m_pvPersistentList->push_back(f_cnotificationname);
        } else {
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Memory not allocated");  // LCOV_EXCL_LINE 5: It's impossible to mock new() function  // NOLINT[whitespace/line_length]
        }

        break;
      }

    // if notification is user persistence notification
    case eFrameworkunifiedPersistedStateUserVar: {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "PersistedStateUser Notification :: %s", f_cnotificationname.c_str());  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
        l_pNotification = new(std::nothrow) CStatePersistenceUserNotification(f_cnotificationname,
                                                                              f_uimsglength);  // LCOV_EXCL_BR_LINE 11: unexpected branch  // NOLINT[whitespace/line_length]

        if (NULL != l_pNotification) {  // LCOV_EXCL_BR_LINE 5: It's impossible to mock new() function
          // insert notification in user persistent notification vector
          m_pvUserPersistentList->push_back(f_cnotificationname);
        } else {
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "NULL pointer error");  // LCOV_EXCL_LINE 5: It's impossible to mock new() function  // NOLINT[whitespace/line_length]
        }

        break;
      }

    // if notification is immediate persistence notification
    case eFrameworkunifiedImmediatePersistedStateVar: {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Immediate PersistedState Notification :: %s", f_cnotificationname.c_str());  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
        l_pNotification = new(std::nothrow) CStateNorPersistenceNotification(f_cnotificationname,
                                                                             f_uimsglength,
                                                                             f_uidelay);  // LCOV_EXCL_BR_LINE 11: unexpected branch  // NOLINT[whitespace/line_length]
        break;
      }

    default:
      break;
  }

  if (NULL != l_pNotification) {
    // insert notification in notification map
    m_pmNotificationList->insert(make_pair(f_cnotificationname, l_pNotification));
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Notification object creation failed for %s.", f_cnotificationname.c_str());  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_pNotification;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// SearchNotification
/// This function is used to search whether notification object is present in map or not.
/// If present it sends the reference.
////////////////////////////////////////////////////////////////////////////////////////////////////
CNotification *CNotificationManager::SearchNotification(const std::string &f_cnotificationname) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  CNotification *l_pNotification = NULL;

  // iterator to find notification from map of notifications
  Notification_Iterator_Type l_itNotification;

  if (NULL == m_pmNotificationList || f_cnotificationname.empty()) {  // LCOV_EXCL_BR_LINE 6: double check, m_pmNotificationList can't be NULL and f_cnotificationname can't be empty  // NOLINT[whitespace/line_length]
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "map pointer is NULL");  // LCOV_EXCL_LINE 6: m_pmNotificationList can't be NULL and f_cnotificationname can't be empty  // NOLINT[whitespace/line_length]
  } else {
    // check if this notification's object present in map
    l_itNotification = m_pmNotificationList->find(f_cnotificationname);

    // if notification found in map
    if (m_pmNotificationList->end() != l_itNotification) {
      FRAMEWORKUNIFIEDLOG(ZONE_NPP_INFO, __FUNCTION__, "Notification found in map :: %s", f_cnotificationname.c_str());  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]

      // get the notification object pointer from map
      l_pNotification = (*l_itNotification).second;
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "Notification not found :: %s", f_cnotificationname.c_str());
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_pNotification;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// SearchPersistenceNotification
/// This function is used to search for the given persistent notification in map.
////////////////////////////////////////////////////////////////////////////////////////////////////
CStateNotification *CNotificationManager::SearchPersistenceNotification(const std::string &f_cnotificationname) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // pointer of notification object
  CNotification *l_pNotification = NULL;
  CStateNotification *l_pStateNotification = NULL;

  // search the notification in the map
  l_pNotification = SearchNotification(f_cnotificationname);

  if (NULL != l_pNotification) {
    if (eFrameworkunifiedPersistedStateVar      == l_pNotification->GetNotificationType() ||
        eFrameworkunifiedPersistedStateUserVar    == l_pNotification->GetNotificationType() ||
        eFrameworkunifiedImmediatePersistedStateVar == l_pNotification->GetNotificationType()) {
      l_pStateNotification = static_cast<CStateNotification *>(l_pNotification);
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "There is no persistence notification registered with name :: %s",
           f_cnotificationname.c_str());
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_pStateNotification;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceServiceOnRegisterEvents
/// This function creates notification object depending on its type and if already created it adds
/// the service name to the list in the notification object.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNotificationManager::NotificationpersistentserviceServiceOnRegisterEvents(const std::string &f_cservicename,
                                                            const std::string &f_cnotificationname,
                                                            const UI_32 f_uimsglength,
                                                            const EFrameworkunifiedNotificationType f_enotificationtype,
                                                            const UI_32 f_uidelay) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  CNotification *l_pNotification = NULL;

  if (f_cservicename.empty() || f_cnotificationname.empty()) {
    l_estatus = eFrameworkunifiedStatusInvldParam;
  } else {
    // get the notification object from map or create new object
    l_pNotification = SearchNotification(f_cnotificationname);

    // if new notification is registered
    if (NULL == l_pNotification) {
      FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "Create new notification object :: %s", f_cnotificationname.c_str());

      // create the new notification object depending on type and insert in map and vector
      l_pNotification = CreateNotificationObject(f_cnotificationname,
                                                 f_uimsglength,
                                                 f_enotificationtype,
                                                 f_uidelay);

      if (NULL != l_pNotification) {
        FRAMEWORKUNIFIEDLOG(ZONE_NPP_INFO, __FUNCTION__, "Set Notification publisher %s for notification %s", f_cservicename.c_str(),
               f_cnotificationname.c_str());  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"

        // set publisher name
        l_estatus = l_pNotification->SetEventPublisher(f_cservicename);
      }
    } else {  // if notification object already exists
      EFrameworkunifiedNotificationType l_eNotificationType = l_pNotification->GetNotificationType();

      // <<notification type is eFrameworkunifiedUnknown
      // if some service had already subscribed to the notification before registering it.>>
      if (eFrameworkunifiedUnknown == l_eNotificationType) {
        // creates new notification object depending on the type and insert it in notification map
        l_estatus = ChangeNotificationType(l_pNotification, f_cservicename, f_cnotificationname, f_uimsglength,
                                           f_enotificationtype, f_uidelay);

        // delete the old notification object
        delete l_pNotification;  // LCOV_EXCL_BR_LINE 11: unexpected branch
        l_pNotification = NULL;
      } else {  // notification is already registered by some service
        // update the notification property, if re-registered by the same publisher
        if (f_cservicename == l_pNotification->GetPublisherName() ||    // if re-register by service
            "" == l_pNotification->GetPublisherName()) {           // if previously unregistered by service
          // if type is same just update the length of notification data
          if (l_eNotificationType == f_enotificationtype) {
            if (l_pNotification->GetMaxMessageSize() != f_uimsglength) {
              FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
                     "Updating max size of notification %s to %d from %d", f_cnotificationname.c_str(),
                     f_uimsglength, l_pNotification->GetMaxMessageSize());
              l_estatus = l_pNotification->ResetMaxMessageSize(f_uimsglength);
            }

            if ("" == l_pNotification->GetPublisherName()) {
              FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
                     "Updating publisher name of notification %s to %s from %s", f_cservicename.c_str(),
                     f_cnotificationname.c_str(), l_pNotification->GetPublisherName().c_str());
              l_pNotification->SetEventPublisher(f_cservicename);
            }
          } else if (eFrameworkunifiedImmediatePersistedStateVar != l_eNotificationType &&
                     eFrameworkunifiedImmediatePersistedStateVar != f_enotificationtype) {
            // else create new notification object depending on the type and delete the old one
            // note: do not change immediate notification type
            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Changing type of notfn %s from %d to %d", f_cnotificationname.c_str(),
                   l_eNotificationType, f_enotificationtype);

            std::vector<std::string>::iterator l_itNotificationName;

            // remove the notification name from the persistent list of notification(if any)
            if (eFrameworkunifiedPersistedStateVar == l_eNotificationType) {
              l_itNotificationName = find(m_pvPersistentList->begin(), m_pvPersistentList->end(), f_cnotificationname);

              if (m_pvPersistentList->end() != l_itNotificationName) {
                m_pvPersistentList->erase(l_itNotificationName);
              }
            } else if (eFrameworkunifiedPersistedStateUserVar == l_eNotificationType) {
              l_itNotificationName = find(m_pvUserPersistentList->begin(),
                                          m_pvUserPersistentList->end(),
                                          f_cnotificationname);

              if (m_pvUserPersistentList->end() != l_itNotificationName) {
                m_pvUserPersistentList->erase(l_itNotificationName);
              }
            } else {
              // do nothing
            }

            // creates new notification object depending on the new type and insert it in map and delete the old one
            l_estatus = ChangeNotificationType(l_pNotification, f_cservicename, f_cnotificationname, f_uimsglength,
                                               f_enotificationtype, f_uidelay);

            // delete the old notification object
            delete l_pNotification;
            l_pNotification = NULL;
          } else {
            // do nothing
          }
        } else {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Notfn: %s already registered by %s, skipping register request by %s",
                 f_cnotificationname.c_str(), l_pNotification->GetPublisherName().c_str(), f_cservicename.c_str());

          l_estatus = eFrameworkunifiedStatusFail;
        }
      }
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// ChangeNotificationType
/// This function creates new notification object depending on the type and replaces the old one.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNotificationManager::ChangeNotificationType(CNotification *f_pnotification,
                                                        const std::string &f_cservicename,
                                                        const std::string &f_cnotificationname,
                                                        const UI_32 f_uimsglength,
                                                        const EFrameworkunifiedNotificationType f_enotificationtype,
                                                        const UI_32 f_uidelay) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  CNotification *l_pNewNotification = NULL;

  if (NULL != f_pnotification) {  // LCOV_EXCL_BR_LINE 6: f_pnotification can't be NULL
    FRAMEWORKUNIFIEDLOG(ZONE_NPP_INFO, __FUNCTION__, "Create New Notification :: %s", f_cnotificationname.c_str());  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]

    // create new notification object as per notification type
    l_pNewNotification = CreateNotificationObject(f_cnotificationname, f_uimsglength, f_enotificationtype, f_uidelay);

    if (NULL != l_pNewNotification) {
      l_estatus = l_pNewNotification->SetEventPublisher(f_cservicename);

      // assign subscribers list to newly created notification
      l_pNewNotification->SetNewSubscribersList(f_pnotification);
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Error creating notification object for notification:%s",
             f_cnotificationname.c_str());  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
      l_estatus = eFrameworkunifiedStatusNullPointer;
    }
  } else {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_estatus = eFrameworkunifiedStatusNullPointer;  // LCOV_EXCL_LINE 6: f_pnotification can't be NULL
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceServiceOnUnRegisterEvents
/// This function removes the name of the service from the notification object.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNotificationManager::NotificationpersistentserviceServiceOnUnRegisterEvents(const std::string &f_cservicename,
                                                              const std::string &f_cnotificationname) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  CNotification *l_pNotification = NULL;
  std::vector<std::string> *l_pvPersistentList = NULL;

  // iterator to find notification from map of notifications
  Notification_Iterator_Type l_itNotification;

  if (f_cservicename.empty() || f_cnotificationname.empty()) {
    l_estatus = eFrameworkunifiedStatusInvldParam;
  } else {
    // check if this notification's object present in map
    l_itNotification = m_pmNotificationList->find(f_cnotificationname);

    // if notification found in map
    if (m_pmNotificationList->end() != l_itNotification) {
      FRAMEWORKUNIFIEDLOG(ZONE_NPP_INFO, __FUNCTION__, "Notification found in map");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]

      // get the notification object pointer from map
      l_pNotification = (*l_itNotification).second;

      if (NULL != l_pNotification) {  // LCOV_EXCL_BR_LINE 6: f_pnotification can't be NULL
        if (eFrameworkunifiedPersistedStateUserVar == l_pNotification->GetNotificationType()) {
          l_pvPersistentList = m_pvUserPersistentList;
        } else if (eFrameworkunifiedPersistedStateVar == l_pNotification->GetNotificationType()) {
          l_pvPersistentList = m_pvPersistentList;
        }

        // reset the publisher name
        l_estatus = l_pNotification->ResetEventPublisher(f_cservicename);

        if (eFrameworkunifiedStatusOK == l_estatus) {
          // if no other service is subscribed to this notification and no service is registered
          // to this notification remove the notification object from map
          if (!l_pNotification->IsServiceRegistered() &&
              l_pNotification->IsSubscribersListEmpty()) {
            FRAMEWORKUNIFIEDLOG(ZONE_NPP_INFO, __FUNCTION__, "Deleting Notification Object");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]

            // delete the notification
            delete l_pNotification;  // LCOV_EXCL_BR_LINE 11: unexpected branch
            l_pNotification = NULL;

            m_pmNotificationList->erase(l_itNotification);

            if (l_pvPersistentList) {
              // find the notification in vector
              std::vector<std::string>::iterator l_itNotificationName;

              l_itNotificationName = find(l_pvPersistentList->begin(),
                                          l_pvPersistentList->end(),
                                          f_cnotificationname);

              if (l_pvPersistentList->end() != l_itNotificationName) {
                FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Deleting Notification name from vector");
                l_pvPersistentList->erase(l_itNotificationName);
              }
            } else {
              FRAMEWORKUNIFIEDLOG(ZONE_NPP_INFO, __FUNCTION__, "Persistent list empty");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
            }
          } else {
            FRAMEWORKUNIFIEDLOG(ZONE_NPP_INFO, __FUNCTION__, "Services still registered or subscriber list empty");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
          }
        } else {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Notification %s is not registered by service %s",
                 f_cnotificationname.c_str(), f_cservicename.c_str());  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
        }
      } else {
        // LCOV_EXCL_START 6: f_pnotification can't be NULL
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Notification %s not registered", f_cnotificationname.c_str());

        l_estatus = eFrameworkunifiedStatusNullPointer;
        // LCOV_EXCL_STOP
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Notification %s not found.", f_cnotificationname.c_str());  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]

      l_estatus = eFrameworkunifiedStatusFail;
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceServiceOnPublishEvent
/// This function stores the published data in the notification object in case of state and
/// persistent notification.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNotificationManager::NotificationpersistentserviceServiceOnPublishEvent(const std::string &f_cservicename,
                                                          const std::string &f_cnotificationname,
                                                          PVOID f_pmessage,
                                                          const UI_32 f_uimsgsize) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  CNotification *l_pNotification = NULL;

  if (f_cservicename.empty() || f_cnotificationname.empty()) {  // LCOV_EXCL_BR_LINE 6: f_cservicename and f_cnotificationname can't be empty  // NOLINT[whitespace/line_length]
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_estatus = eFrameworkunifiedStatusInvldParam;  // LCOV_EXCL_LINE 6: f_cservicename and f_cnotificationname can't be empty
  } else {
    // get the notification object from map
    l_pNotification = SearchNotification(f_cnotificationname);

    if (NULL != l_pNotification) {
      // publish the notification
      if (eFrameworkunifiedStatusOK != (l_estatus = l_pNotification->Publish(f_cservicename,
                                                                f_pmessage,
                                                                f_uimsgsize))) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error publishing notification %s published by %s, status: %d",
               f_cnotificationname.c_str(), f_cservicename.c_str(), l_estatus);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Notification not registered");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]

      l_estatus = eFrameworkunifiedStatusNullPointer;
    }
  }


  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceServiceOnSubscribeToEvent
/// This function adds the name of the application to subscribers list in notification object.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNotificationManager::NotificationpersistentserviceServiceOnSubscribeToEvent(const std::string &f_csubscribername,
                                                              const std::string &f_cnotificationname) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // iterator to find notification from map of notifications
  Notification_Iterator_Type l_itNotification;

  CNotification *l_pNotification = NULL;

  if (f_cnotificationname.empty() || f_csubscribername.empty()) {
    l_estatus = eFrameworkunifiedStatusInvldParam;
  } else if (NULL == m_pmNotificationList) {  // LCOV_EXCL_BR_LINE 6 m_pmNotificationList can't be NULL
    // LCOV_EXCL_START 6: m_pmNotificationList can't be NULL
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "map pointer is NULL");
    l_estatus = eFrameworkunifiedStatusNullPointer;
    // LCOV_EXCL_STOP
  } else {
    // get the notification object from map or create new object
    l_pNotification = SearchNotification(f_cnotificationname);

    if (NULL == l_pNotification) {
      FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "CNotification object created for %s", f_cnotificationname.c_str());

      l_pNotification = new(std::nothrow) CNotification(f_cnotificationname, 0);  // LCOV_EXCL_BR_LINE 11: unexpected branch  // NOLINT[whitespace/line_length]

      m_pmNotificationList->insert(make_pair(f_cnotificationname, l_pNotification));
    }

    if (NULL != l_pNotification) {  // LCOV_EXCL_BR_LINE 6: double check, l_pNotification can't be NULL
      // add subscribers name in subscribers list of notification
      l_estatus = l_pNotification->AddEventReciever(f_csubscribername);
    } else {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Memory not allocated for l_pNotification");  // LCOV_EXCL_LINE 6: double check, l_pNotification can't be NULL  // NOLINT[whitespace/line_length]
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceServiceOnUnSubscribeFromEvent
/// This function removes the name of the application from the subscribers list in notification object.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNotificationManager::NotificationpersistentserviceServiceOnUnSubscribeFromEvent(const std::string &f_csubscribername,
                                                                  const std::string &f_cnotificationname) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  CNotification *l_pNotification = NULL;

  // iterator to find notification from map of notifications
  Notification_Iterator_Type l_itNotificationType;

  std::vector<std::string> *l_pvPersistentList = NULL;

  if (f_cnotificationname.empty() || f_csubscribername.empty()) {
    l_estatus = eFrameworkunifiedStatusInvldParam;
  } else if (NULL == m_pmNotificationList) {  // LCOV_EXCL_BR_LINE 6: m_pmNotificationList can't be NULL
    // LCOV_EXCL_START 6: m_pmNotificationList can't be NULL
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Notification list pointer is NULL");
    l_estatus = eFrameworkunifiedStatusNullPointer;
    // LCOV_EXCL_STOP
  } else {
    // check if this notification's object present in map
    l_itNotificationType = m_pmNotificationList->find(f_cnotificationname);

    // if notification found in map
    if (m_pmNotificationList->end() != l_itNotificationType) {
      FRAMEWORKUNIFIEDLOG(ZONE_NPP_INFO, __FUNCTION__, "Notification %s found in map", f_cnotificationname.c_str());  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]

      l_pNotification = (*l_itNotificationType).second;

      if (NULL != l_pNotification) {  // LCOV_EXCL_BR_LINE 6: l_pNotification can't be NULL
        if (eFrameworkunifiedPersistedStateUserVar == l_pNotification->GetNotificationType()) {
          l_pvPersistentList = m_pvUserPersistentList;
        } else if (eFrameworkunifiedPersistedStateVar == l_pNotification->GetNotificationType()) {
          l_pvPersistentList = m_pvPersistentList;
        }

        // removes the subscribers name from subscribers list of notification
        l_estatus = l_pNotification->DeleteEventReciever(f_csubscribername);

        // if no other service is subscribed to this notification and no service is registered
        // to this notification remove the notification object from map
        if (!l_pNotification->IsServiceRegistered() &&
            l_pNotification->IsSubscribersListEmpty()) {
          FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Deleting Notification Object from map");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]

          // delete notification object from map
          delete l_pNotification;  // LCOV_EXCL_BR_LINE 11: unexpected branch
          l_pNotification = NULL;

          m_pmNotificationList->erase(l_itNotificationType);

          if (l_pvPersistentList) {
            //  find notification in vector
            std::vector<std::string>::iterator l_itNotificationName;
            l_itNotificationName = find(l_pvPersistentList->begin(),
                                        l_pvPersistentList->end(),
                                        f_cnotificationname);

            if (l_pvPersistentList->end() != l_itNotificationName) {
              FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Deleting Notification from vector");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
              l_pvPersistentList->erase(l_itNotificationName);
            }
          } else {
            FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Persistent list Empty.");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
          }
        }
      } else {
        // LCOV_EXCL_START 6: l_pNotification can't be NULL
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Notification %s not registered", f_cnotificationname.c_str());
        l_estatus = eFrameworkunifiedStatusNullPointer;
        // LCOV_EXCL_STOP
      }

    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Notification %s not found.", f_cnotificationname.c_str());  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
      l_estatus = eFrameworkunifiedStatusFail;
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NPGetPersistentNotificationData
///
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNotificationManager::NPGetPersistentNotificationData(const std::string &f_cnotificationname,
                                                                 PVOID f_pnotificationdata,
                                                                 const UI_32 f_uidatasize) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  CNotification *l_pNotification = NULL;
  CStateNotification *l_pStateNotification = NULL;
  const CPersistentData *l_pPersistentData = NULL;

  if (f_cnotificationname.empty() || NULL == f_pnotificationdata) {  // LCOV_EXCL_BR_LINE 6: f_pnotificationdata can't be NULL and f_cnotificationname can't be empty  // NOLINT[whitespace/line_length]
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_estatus = eFrameworkunifiedStatusInvldParam;  // LCOV_EXCL_LINE 6: f_pnotificationdata can't be NULL and f_cnotificationname can't be empty  // NOLINT[whitespace/line_length]
  } else {
    l_pNotification = SearchNotification(f_cnotificationname);

    if (NULL != l_pNotification) {  // LCOV_EXCL_BR_LINE 200: f_cnotificationname must be NTFY_NPPService_UserChange, NTFY_NPPService_UserChange must be registered in FrameworkunifiedOnEntry() by itself.  // NOLINT[whitespace/line_length]
      const EFrameworkunifiedNotificationType l_eNotificationType = l_pNotification->GetNotificationType();

      if (eFrameworkunifiedStateVar                   == l_eNotificationType ||
          eFrameworkunifiedPersistedStateVar          == l_eNotificationType ||
          eFrameworkunifiedPersistedStateUserVar      == l_eNotificationType ||
          eFrameworkunifiedImmediatePersistedStateVar == l_eNotificationType) {  // LCOV_EXCL_BR_LINE 200: NTFY_NPPService_UserChange's type is eFrameworkunifiedPersistedStateVar.   // NOLINT[whitespace/line_length]
        l_pStateNotification = static_cast<CStateNotification *>(l_pNotification);

        l_pPersistentData = l_pStateNotification->GetPersistentData();

        if (NULL != l_pPersistentData) {
          if (f_uidatasize >= l_pPersistentData->m_uiMsgSize) {
            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Sending Data Size %d", l_pPersistentData->m_uiMsgSize);

            if (NULL != l_pPersistentData->m_pMessage) {
              std::memcpy(f_pnotificationdata, l_pPersistentData->m_pMessage, l_pPersistentData->m_uiMsgSize);
            } else {
              FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Notification data is NULL");
            }
          } else {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                   "Smaller buffer size %d is received for notification data of size %d", f_uidatasize,
                   l_pPersistentData->m_uiMsgSize);
          }
        } else {
          l_estatus = eFrameworkunifiedStatusFail;
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                 "Persistent data object is NULL for notification %s", f_cnotificationname.c_str());  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
        }
      } else {
        // LCOV_EXCL_START 200: NTFY_NPPService_UserChange's type is eFrameworkunifiedPersistedStateVar.
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        l_estatus = eFrameworkunifiedStatusFail;
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Get notification data failed, Notification %s is registered as type %d",
               f_cnotificationname.c_str(), l_eNotificationType);
        // LCOV_EXCL_STOP
      }
    } else {
      // LCOV_EXCL_START 200: f_cnotificationname must be NTFY_NPPService_UserChange, NTFY_NPPService_UserChange must be registered in FrameworkunifiedOnEntry() by itself.  // NOLINT[whitespace/line_length]
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_estatus = eFrameworkunifiedStatusFail;
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Notification %s not found.", f_cnotificationname.c_str());
      // LCOV_EXCL_STOP
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceServiceOnGetPersistentData
/// This function is used to get the persistent data stored related to notification.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNotificationManager::NotificationpersistentserviceServiceOnGetPersistentData(const std::string &f_cnotificationname,
                                                               const std::string &f_creceivername) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  CNotification *l_pNotification = NULL;
  CStateNotification *l_pStateNotification = NULL;
  const CPersistentData *l_pPersistentData = NULL;

  if (f_cnotificationname.empty() || f_creceivername.empty()) {  // LCOV_EXCL_BR_LINE 6: f_creceivername can't be NULL and f_cnotificationname can't be empty  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 6: f_creceivername can't be NULL and f_cnotificationname can't be empty
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_estatus = eFrameworkunifiedStatusInvldParam;
    // LCOV_EXCL_STOP
  } else {
    // open the receiver message queue
    HANDLE l_hReceiverMq = McOpenSender(f_creceivername.c_str());

    if (NULL == l_hReceiverMq) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
      // LCOV_EXCL_START 4: NSFW error case.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      // catastrophic failure!
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to open MessageQ %s ", f_creceivername.c_str());
      l_estatus = eFrameworkunifiedStatusFail;
      // LCOV_EXCL_STOP
    } else {
      // get the notification object from map or create new object
      l_pNotification = SearchNotification(f_cnotificationname);

      if (NULL != l_pNotification) {
        l_pStateNotification = static_cast<CStateNotification *>(l_pNotification);

        if (NULL != l_pStateNotification) {  // LCOV_EXCL_BR_LINE 5: fail safe for static_cast<CStateNotification *>
          l_pPersistentData = l_pStateNotification->GetPersistentData();

          if (NULL != l_pPersistentData) {
            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Sending Data Size %d", l_pPersistentData->m_uiMsgSize);

            if (NULL == l_pPersistentData->m_pMessage) {
              FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Sending Data is NULL");
            }

            // sent the notification data to requester
            l_estatus = McSend(l_hReceiverMq,
                               AppName,
                               NPS_GET_PERS_DATA_ACK,
                               l_pPersistentData->m_uiMsgSize,
                               l_pPersistentData->m_pMessage);
            FRAMEWORKUNIFIEDLOG(ZONE_PRD_INFO2, __FUNCTION__, "Persistent data found for notification %s. "
                   "Sent NPS_GET_PERS_DATA_ACK to %s. Status: %d."
                   , f_cnotificationname.c_str(), f_creceivername.c_str(), l_estatus);
          } else {  // In case of failure, we send back a failure ACK
            l_estatus = eFrameworkunifiedStatusFail;
            FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "No persistent data found for notification %s. "
                   , f_cnotificationname.c_str());
          }
        }
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Notification %s not found", f_cnotificationname.c_str());   // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
        l_estatus = eFrameworkunifiedStatusInvldParam;
      }
      // Close the mq handle
      McClose(l_hReceiverMq);
      l_hReceiverMq = NULL;
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceGetPersistentNotificationData
/// This function is used to get the list of persistent notifications and data associated with it.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNotificationManager::NotificationpersistentserviceGetPersistentNotificationData(std::vector<CNotificationsToPersist *>
                                                                  *f_pvpersistnotification,
                                                                  const EFrameworkunifiedNotificationType f_enotificationtype,
                                                                  UI_32 f_uinotificationpersistentservicepersistcategoryflag) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  CNotification *l_pNotification = NULL;
  CStateNotification *l_pStateNotification = NULL;
  CStatePersistenceNotification *l_pStatePersistenceNotification = NULL;

  std::string l_cNotificationName = "";
  CNotificationsToPersist *l_pNotificationsToPersist = NULL;
  const CPersistentData *l_pPData = NULL;

  std::vector<std::string> *l_pvPersistentList = NULL;

  // iterator for retrieving notifications from map of notifications
  Notification_Iterator_Type l_itNotification;

  if (eFrameworkunifiedPersistedStateUserVar == f_enotificationtype) {
    l_pvPersistentList = m_pvUserPersistentList;
  } else if (eFrameworkunifiedPersistedStateVar == f_enotificationtype) {  // LCOV_EXCL_BR_LINE 6: f_enotificationtype must be eFrameworkunifiedPersistedStateUserVar or eFrameworkunifiedPersistedStateVar  // NOLINT[whitespace/line_length]
    l_pvPersistentList = m_pvPersistentList;
  }

  // copy all the notification data in map to received vector
  if (NULL != f_pvpersistnotification && NULL != l_pvPersistentList) {  // LCOV_EXCL_BR_LINE 6: f_pvpersistnotification and l_pvPersistentList can't be null  // NOLINT[whitespace/line_length]
    for (UI_32 l_uiCount = 0;
         l_uiCount < l_pvPersistentList->size();
         l_uiCount++) {
      // get the persistent notification name from vector
      l_cNotificationName = l_pvPersistentList->at(l_uiCount);

      // search for notification in map
      l_itNotification = m_pmNotificationList->find(l_cNotificationName);

      // notification found in map
      if (m_pmNotificationList->end() != l_itNotification) {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Persistent Notification: %s", l_cNotificationName.c_str());   // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]

        // get notification object from map
        l_pNotification = (*l_itNotification).second;

        if (NULL != l_pNotification) {   // LCOV_EXCL_BR_LINE 6: l_pNotification can't be NULL
          if (eFrameworkunifiedPersistedStateVar == f_enotificationtype) {
            l_pStatePersistenceNotification = static_cast<CStatePersistenceNotification *>(l_pNotification);

            // get the notification's default data
            if ((eFrameworkunifiedUserData        == l_pStatePersistenceNotification->GetPersistentCategory() &&
                 (f_uinotificationpersistentservicepersistcategoryflag & eFrameworkunifiedUserData)) ||
                (eFrameworkunifiedFactoryData       == l_pStatePersistenceNotification->GetPersistentCategory() &&
                 (f_uinotificationpersistentservicepersistcategoryflag & eFrameworkunifiedFactoryData)) ||
                (eFrameworkunifiedFactoryCustomerData == l_pStatePersistenceNotification->GetPersistentCategory() &&
                 (f_uinotificationpersistentservicepersistcategoryflag & eFrameworkunifiedFactoryCustomerData)) ||
                (eFrameworkunifiedDealerData        == l_pStatePersistenceNotification->GetPersistentCategory() &&
                 (f_uinotificationpersistentservicepersistcategoryflag & eFrameworkunifiedDealerData))) {
              l_pPData = l_pStatePersistenceNotification->GetDefaultPersistentData();
            } else {
              l_pStateNotification = l_pStatePersistenceNotification;

              // get the published notification data
              if (l_pStateNotification->IsPublished()) {
                l_pPData = l_pStateNotification->GetPersistentData();
              } else {
                FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Notification not published");
              }
            }
          } else {
            l_pStateNotification = static_cast<CStateNotification *>(l_pNotification);

            // if not to reset userdata
            if (0 == (f_uinotificationpersistentservicepersistcategoryflag & eFrameworkunifiedUserData)) {
              if (l_pStateNotification->IsPublished()) {
                l_pPData = l_pStateNotification->GetPersistentData();
              } else {
                FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Notification not published");
              }
            }
          }

          if ((NULL != l_pStateNotification) && (NULL != l_pPData)) {
            l_pNotificationsToPersist = CreateNotificationObjectToPersist(l_pStateNotification, l_pPData);  // LCOV_EXCL_BR_LINE 11: unexpected branch  // NOLINT[whitespace/line_length]

            // insert the persistent data in map
            f_pvpersistnotification->push_back(l_pNotificationsToPersist);  // LCOV_EXCL_BR_LINE 11: unexpected branch

            l_pPData = NULL;
          } else {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "no data is stored in notification %s", l_cNotificationName.c_str());  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
          }
        } else {
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Notification object is NULL for %s", l_cNotificationName.c_str());  // LCOV_EXCL_LINE 6: l_pNotification can't be NULL  // NOLINT[whitespace/line_length]
        }
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Non Persistent Notification: %s", l_cNotificationName.c_str());
      }
    }
  } else {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_estatus = eFrameworkunifiedStatusNullPointer;  // LCOV_EXCL_LINE 6: f_pvpersistnotification and l_pvPersistentList can't be null  // NOLINT[whitespace/line_length]
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// CreateNotificationObjectToPersist
/// Creates the CNotificationsToPersist object from notification object and the persistent data.
////////////////////////////////////////////////////////////////////////////////////////////////////
CNotificationsToPersist *CNotificationManager::CreateNotificationObjectToPersist(CStateNotification
                                                                                 *f_pStateNotification,
                                                                                 const CPersistentData *f_pdata) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  CNotificationsToPersist *l_pNotificationsToPersist = NULL;
  CPersistentData *l_pData = NULL;

  if (NULL != f_pStateNotification && NULL != f_pdata) {  // LCOV_EXCL_BR_LINE 6: f_pStateNotification and f_pdata can't be NULL  // NOLINT[whitespace/line_length]
    if (f_pStateNotification->GetMaxMessageSize() >= f_pdata->m_uiMsgSize) {
      l_pNotificationsToPersist = new(std::nothrow) CNotificationsToPersist();  // LCOV_EXCL_BR_LINE 11: unexpected branch  // NOLINT[whitespace/line_length]

      if (NULL != l_pNotificationsToPersist) {  // LCOV_EXCL_BR_LINE 5: It's impossible to mock new() function
        l_pNotificationsToPersist->m_pPersistentData = new(std::nothrow) CPersistentData();  // LCOV_EXCL_BR_LINE 11: unexpected branch  // NOLINT[whitespace/line_length]
        l_pData = l_pNotificationsToPersist->m_pPersistentData;

        if (NULL != l_pData) {  // LCOV_EXCL_BR_LINE 5: It's impossible to mock new() function
          l_pNotificationsToPersist->m_cNotificationName = f_pStateNotification->GetNotificationName();
          l_pNotificationsToPersist->m_uiMaxMsgLength = f_pStateNotification->GetMaxMessageSize();
          l_pNotificationsToPersist->m_ePersistentType = f_pStateNotification->GetNotificationType();

          if (eFrameworkunifiedPersistedStateVar == l_pNotificationsToPersist->m_ePersistentType) {
            l_pNotificationsToPersist->m_ePersistCategory = (static_cast<CStatePersistenceNotification *>
                                                             (f_pStateNotification))->GetPersistentCategory();
          }

          l_pNotificationsToPersist->m_cPublisherName = f_pStateNotification->GetPublisherName();

          l_pData->m_uiMsgSize = f_pdata->m_uiMsgSize;
          l_pData->m_pMessage = new(std::nothrow) CHAR[l_pNotificationsToPersist->m_uiMaxMsgLength];

          if (NULL != l_pData->m_pMessage) {  // LCOV_EXCL_BR_LINE 5: It's impossible to mock new() function
            std::memset(l_pData->m_pMessage, 0, l_pNotificationsToPersist->m_uiMaxMsgLength);
            std::memcpy(l_pData->m_pMessage, f_pdata->m_pMessage, f_pdata->m_uiMsgSize);
          }
        } else {
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "memory not allocated for l_pNotificationsToPersist");  // LCOV_EXCL_LINE 5: It's impossible to mock new() function  // NOLINT[whitespace/line_length]
        }
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Error persisting notfn, Data size of notification %s is %d greater than max registered size %d",
             f_pStateNotification->GetNotificationName().c_str(),
             f_pdata->m_uiMsgSize, f_pStateNotification->GetMaxMessageSize());
    }
  } else {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Invalid parameter received");  // LCOV_EXCL_LINE 6: f_pStateNotification and f_pdata can't be NULL  // NOLINT[whitespace/line_length]
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_pNotificationsToPersist;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceSetPersistentNotificationData
/// This function is used to create the persistent notifications object and fill the data related
/// with it on system load.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNotificationManager::NotificationpersistentserviceSetPersistentNotificationData(std::vector<CNotificationsToPersist *>
                                                                  *f_pvpersistnotification) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  CStateNotification *l_pNotification = NULL;
  CNotificationsToPersist *l_pNotificationsToPersist = NULL;

  std::vector<std::string> *l_pvPersistentList = NULL;

  Persistent_Iterator_Type l_itPersistentData;

  // store all the notification data received in vector to map of CNotification and to vector of
  // persistent notification in case of persistent and user persistent notification
  if (NULL != f_pvpersistnotification) {  // LCOV_EXCL_BR_LINE 6: double check, f_pvpersistnotification can't be NULL
    for (UI_32 l_uiCount = 0;
         l_uiCount < f_pvpersistnotification->size();
         l_uiCount++) {
      // get the persistent data object
      l_pNotificationsToPersist = f_pvpersistnotification->at(l_uiCount);

      if (NULL != l_pNotificationsToPersist) {  // LCOV_EXCL_BR_LINE 6: double check, l_pNotificationsToPersist can't be NULL  // NOLINT[whitespace/line_length]
        FRAMEWORKUNIFIEDLOG(ZONE_NPP_INFO, __FUNCTION__, "Persistent Type %d", l_pNotificationsToPersist->m_ePersistentType);

        if (eFrameworkunifiedPersistedStateVar == l_pNotificationsToPersist->m_ePersistentType) {
          FRAMEWORKUNIFIEDLOG(ZONE_NPP_INFO, __FUNCTION__, "PersistentState Notification");

          l_pNotification = new(std::nothrow) CStatePersistenceNotification(
                      l_pNotificationsToPersist->m_cNotificationName,
                      l_pNotificationsToPersist->m_uiMaxMsgLength,
                      l_pNotificationsToPersist->m_ePersistCategory);

          l_pvPersistentList = m_pvPersistentList;
        } else if (eFrameworkunifiedPersistedStateUserVar == l_pNotificationsToPersist->m_ePersistentType) {
          FRAMEWORKUNIFIEDLOG(ZONE_NPP_INFO, __FUNCTION__, "PersistentStateUser Notification");

          l_pNotification = new(std::nothrow) CStatePersistenceUserNotification(
                      l_pNotificationsToPersist->m_cNotificationName,
                      l_pNotificationsToPersist->m_uiMaxMsgLength);

          l_pvPersistentList = m_pvUserPersistentList;
        } else if (eFrameworkunifiedImmediatePersistedStateVar == l_pNotificationsToPersist->m_ePersistentType) {  // LCOV_EXCL_BR_LINE 6: l_pNotificationsToPersist->m_ePersistentType must be eFrameworkunifiedPersistedStateVar, eFrameworkunifiedPersistedStateUserVar, eFrameworkunifiedImmediatePersistedStateVar // NOLINT[whitespace/line_length]
          FRAMEWORKUNIFIEDLOG(ZONE_NPP_INFO, __FUNCTION__, "PersistentStateUser Notification");

          l_pNotification = new(std::nothrow) CStateNorPersistenceNotification(
                      l_pNotificationsToPersist->m_cNotificationName,
                      l_pNotificationsToPersist->m_uiMaxMsgLength,
                      l_pNotificationsToPersist->m_uiDelay,
                      l_pNotificationsToPersist->m_ePersistCategory);
        }

        if (NULL != l_pNotification) {  // LCOV_EXCL_BR_LINE 6: l_pNotification can't be NULL
          FRAMEWORKUNIFIEDLOG(ZONE_NPP_INFO, __FUNCTION__, "Insert persistent notification object in map");

          l_pNotification->SetPersistentData(l_pNotificationsToPersist->m_pPersistentData->m_pMessage,
                                             l_pNotificationsToPersist->m_pPersistentData->m_uiMsgSize);

          l_pNotification->SetEventPublisher(l_pNotificationsToPersist->m_cPublisherName);

          if (NULL != m_pmNotificationList) {  // LCOV_EXCL_BR_LINE 6: double check, m_pmNotificationList can't be NULL
            m_pmNotificationList->insert(make_pair(l_pNotificationsToPersist->m_cNotificationName, l_pNotification));

            if (NULL != l_pvPersistentList) {
              l_pvPersistentList->push_back(l_pNotificationsToPersist->m_cNotificationName);
              FRAMEWORKUNIFIEDLOG(ZONE_NPP_INFO, __FUNCTION__, "Inserted persistent notification object in map");
            }
            FRAMEWORKUNIFIEDLOG(ZONE_NPP_INFO, __FUNCTION__, "Inserted persistent notification object in vector");
          } else {
            // LCOV_EXCL_START 6: double check, m_pmNotificationList can't be NULL
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            delete l_pNotification;
            l_pNotification = NULL;
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Notification object not inserted in map/vector");
            // LCOV_EXCL_STOP
          }
        } else {
          // LCOV_EXCL_START 6: l_pNotification can't be NULL
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "notification object is NULL");

          l_estatus = eFrameworkunifiedStatusNullPointer;
          // LCOV_EXCL_STOP
        }
      } else {
        // LCOV_EXCL_START 6: double check, l_pNotificationsToPersist can't be NULL
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "l_pNotificationsToPersist is NULL");
        l_estatus = eFrameworkunifiedStatusNullPointer;
        // LCOV_EXCL_STOP
      }
    }
  } else {
    // LCOV_EXCL_START 6: double check, f_pvpersistnotification can't be NULL
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Persistent data pointer is NULL");

    l_estatus = eFrameworkunifiedStatusNullPointer;
    // LCOV_EXCL_STOP
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// RemoveUserSpecificNotificationEntry
/// This function is used to remove  the user persistent notifications entry
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNotificationManager::RemoveUserSpecificNotificationEntry() {    // LCOV_EXCL_START 100: never be used
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  std::string l_cNotificationName;

  std::vector<std::string>::iterator l_itNotificationName;

  // iterator to find notification from map of notifications
  Notification_Iterator_Type l_itNotification;

  CNotification *l_pNotification = NULL;

  for (l_itNotificationName = m_pvUserPersistentList->begin() ; l_itNotificationName < m_pvUserPersistentList->end() ;
       l_itNotificationName++) {
    l_cNotificationName = *(l_itNotificationName);

    // check if this notification's object present in map
    l_itNotification = m_pmNotificationList->find(l_cNotificationName);

    if (m_pmNotificationList->end() != l_itNotification) {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Notification found in map");

      // get the notification object pointer from map
      l_pNotification = (*l_itNotification).second;

      if (NULL != l_pNotification) {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Deleting Notification Object");

        // delete the notification
        delete l_pNotification;
        l_pNotification = NULL;

        m_pmNotificationList->erase(l_itNotification);

        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Deleting Notification name from vector");
        m_pvUserPersistentList->erase(l_itNotificationName);

        l_itNotificationName = m_pvUserPersistentList->begin();
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Notification not found.");

      l_estatus = eFrameworkunifiedStatusFail;
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus CNotificationManager::GetNotificationInfo(const std::string &f_cnotificationname,  // LCOV_EXCL_START 100: never be used  // NOLINT[whitespace/line_length]
                                                     CNotificationsToPersist *&f_pnotificationstopersist) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  CNotification *l_pNotification = NULL;
  CStateNotification *l_pStateNotification = NULL;
  const CPersistentData *l_pPData = NULL;

  l_pNotification = SearchNotification(f_cnotificationname);
  l_pStateNotification = static_cast<CStateNotification *>(l_pNotification);

  if (NULL != l_pStateNotification) {
    if (l_pStateNotification->IsPublished()) {
      l_pPData = l_pStateNotification->GetPersistentData();

      if (NULL != l_pPData) {
        f_pnotificationstopersist = new(std::nothrow) CNotificationsToPersist();

        if (NULL == f_pnotificationstopersist) {
          l_estatus = eFrameworkunifiedStatusNullPointer;
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Memory allocation error for f_pnotificationstopersist");
        }

        if (eFrameworkunifiedStatusOK == l_estatus) {
          f_pnotificationstopersist->m_pPersistentData = new(std::nothrow) CPersistentData();

          if (NULL == f_pnotificationstopersist->m_pPersistentData) {
            l_estatus = eFrameworkunifiedStatusNullPointer;
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Memory allocation error for f_pnotificationstopersist->m_pPersistentData");
          }
        }

        if (eFrameworkunifiedStatusOK == l_estatus) {
          f_pnotificationstopersist->m_cNotificationName = l_pStateNotification->GetNotificationName();
          f_pnotificationstopersist->m_uiMaxMsgLength    = l_pStateNotification->GetMaxMessageSize();
          f_pnotificationstopersist->m_cPublisherName    = l_pStateNotification->GetPublisherName();
          f_pnotificationstopersist->m_ePersistentType   = l_pStateNotification->GetNotificationType();

          if (eFrameworkunifiedPersistedStateVar == f_pnotificationstopersist->m_ePersistentType) {
            f_pnotificationstopersist->m_ePersistCategory = (static_cast<CStatePersistenceNotification *>
                                                             (l_pStateNotification))->GetPersistentCategory();
          }

          if (eFrameworkunifiedImmediatePersistedStateVar == f_pnotificationstopersist->m_ePersistentType) {
            CStateNorPersistenceNotification *l_pStateNorPersistenceNotification =
                    static_cast<CStateNorPersistenceNotification *>(l_pStateNotification);
            f_pnotificationstopersist->m_uiDelay = l_pStateNorPersistenceNotification->GetPersistenceDelay();
            f_pnotificationstopersist->m_ePersistCategory = (static_cast<CStateNorPersistenceNotification *>
                                                             (l_pStateNotification))->GetPersistentCategory();
          }

          f_pnotificationstopersist->m_pPersistentData->m_pMessage = new(std::nothrow)
          CHAR[f_pnotificationstopersist->m_uiMaxMsgLength];
          std::memset(f_pnotificationstopersist->m_pPersistentData->m_pMessage, 0,
                      f_pnotificationstopersist->m_uiMaxMsgLength);
          std::memcpy(f_pnotificationstopersist->m_pPersistentData->m_pMessage,
                      l_pPData->m_pMessage, l_pPData->m_uiMsgSize);

          f_pnotificationstopersist->m_pPersistentData->m_uiMsgSize = l_pPData->m_uiMsgSize;
        }
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "No data is stored in notification %s", f_cnotificationname.c_str());
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Notification %s not published", f_cnotificationname.c_str());
    }
  } else {
    l_estatus = eFrameworkunifiedStatusNullPointer;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}
// LCOV_EXCL_STOP

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceSetDefaultPersistentNotificationData
/// This function is used to set the default data of persistent notification
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNotificationManager::NotificationpersistentserviceSetDefaultPersistentNotificationData(const std::string &f_cnotificationname,
                                                                         PVOID f_pmessage,
                                                                         const UI_32 f_uimsgsize) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // pointer of notification object
  CStateNotification *l_pStateNotification = SearchPersistenceNotification(f_cnotificationname);

  if (NULL != l_pStateNotification) {
    l_estatus = l_pStateNotification->SetDefaultPersistentData(f_pmessage, f_uimsgsize);
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Unable to set default notification data for %s, notification does not exists",
           f_cnotificationname.c_str());
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceSetPersistentCategory
/// This function is used to set the persistent type of persistent notification
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNotificationManager::NotificationpersistentserviceSetPersistentCategory(const std::string &f_cnotificationname,
                                                          const EFrameworkunifiedPersistCategory f_epersistcategory) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // pointer of notification object
  CStateNotification *l_pStateNotification = SearchPersistenceNotification(f_cnotificationname);

  if (NULL != l_pStateNotification) {
    EFrameworkunifiedNotificationType l_eNotificationType = l_pStateNotification->GetNotificationType();

    if (eFrameworkunifiedPersistedStateVar == l_eNotificationType) {
      l_estatus = (static_cast<CStatePersistenceNotification *>(l_pStateNotification))->SetPersistentCategory(
                    f_epersistcategory);
    } else if (eFrameworkunifiedImmediatePersistedStateVar == l_eNotificationType) {
      l_estatus = (static_cast<CStateNorPersistenceNotification *>(l_pStateNotification))->SetPersistentCategory(
                    f_epersistcategory);
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Cannot set category %d for notification %s of type %d", f_epersistcategory,
             f_cnotificationname.c_str(), l_eNotificationType);
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "Cannot set category %d for %s, Either not registered or not a persistent notification",
           f_epersistcategory, f_cnotificationname.c_str());  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentservicePublishImmediateNotification
/// This function publish the immediate notification f_cnotificationname to all its subscribers.
/// This API is called when service updates the immediate notification data in persistent memory
/// using synchronous API.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNotificationManager::NotificationpersistentservicePublishImmediateNotification(const std::string &f_cservicename,
                                                                 const std::string &f_cnotificationname,
                                                                 PVOID f_pmessage,
                                                                 const UI_32 f_uimsgsize) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (f_cservicename.empty() || f_cnotificationname.empty()) {
    l_estatus = eFrameworkunifiedStatusInvldParam;
  } else {
    // get the notification object from map
    CStateNotification *l_pStateNotf = SearchPersistenceNotification(f_cnotificationname);

    if (NULL != l_pStateNotf) {
      if (eFrameworkunifiedImmediatePersistedStateVar == l_pStateNotf->GetNotificationType()) {
        CStateNorPersistenceNotification *l_pImmediateNotf =
                static_cast<CStateNorPersistenceNotification *>(l_pStateNotf);

        // publish the notification
        if (eFrameworkunifiedStatusOK != (l_estatus = l_pImmediateNotf->PublishNotification(f_cservicename,
                                                                               f_pmessage,
                                                                               f_uimsgsize))) {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error publishing notification %s published by %s, status: %d",
                 f_cnotificationname.c_str(), f_cservicename.c_str(), l_estatus);
        }
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Notification %s is not registered as immediate persistence by service %s",
               f_cnotificationname.c_str(), f_cservicename.c_str());
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Notification %s not registered by service %s", f_cnotificationname.c_str(),
             f_cservicename.c_str());

      l_estatus = eFrameworkunifiedStatusNullPointer;
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

#ifdef NPP_PROFILEINFO_ENABLE

EFrameworkunifiedStatus CNotificationManager::GetNotificationProfilingData(std::string &f_cnotificationprofileInfo) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  Notification_Iterator_Type l_itNotification;
  NotifReceiver_Iterator_Type l_itNotifReceiver_Iterator;

  CNotification *l_pNotification = NULL;
  NotifReceiver_Type *l_pSubscriberList = NULL;

  std::stringstream ss;
  UI_32 l_uiSubscriberCnt = 0;

  std::string l_cPublisher = "";
  std::string l_cNotification = "";

  try {
    if (NULL == m_pmNotificationList) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Notification List m_pmNotificationList is NULL  in notification manager");
      l_estatus = eFrameworkunifiedStatusFail;
    }

    if (eFrameworkunifiedStatusOK == l_estatus && m_pmNotificationList->empty()) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Notification List m_pmNotificationList Empty in notification manager");
      l_estatus = eFrameworkunifiedStatusFail;
    }

    if (eFrameworkunifiedStatusOK == l_estatus) {
      f_cnotificationprofileInfo.append("Application Name,");
      f_cnotificationprofileInfo.append("Notification Name,");
      f_cnotificationprofileInfo.append("Type,");
      f_cnotificationprofileInfo.append("Length,");
      f_cnotificationprofileInfo.append("Subscribed By,");
      f_cnotificationprofileInfo.append("Subscribers Count");

      for (l_itNotification = m_pmNotificationList->begin();
           l_itNotification != m_pmNotificationList->end();
           l_itNotification++) {
        l_pNotification = (*l_itNotification).second;
        if (NULL == l_pNotification) {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Notification object ptr is NULL");
          l_estatus = eFrameworkunifiedStatusFail;
        } else {
          l_cPublisher = l_pNotification->GetPublisherName();
          if (l_cPublisher.empty()) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Publisher name empty");
            l_estatus = eFrameworkunifiedStatusFail;
          }

          l_cNotification = l_pNotification->GetNotificationName();
          if (l_cNotification.empty()) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Notification name empty");
            l_estatus = eFrameworkunifiedStatusFail;
          }

          l_pSubscriberList = l_pNotification->GetSubscriberList();
          if (NULL == l_pSubscriberList) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Subscribers List ptr is NULL");
            l_estatus = eFrameworkunifiedStatusFail;
          }
        }

        if (eFrameworkunifiedStatusOK == l_estatus) {
          f_cnotificationprofileInfo.append("\n");

          f_cnotificationprofileInfo.append(l_cPublisher);
          f_cnotificationprofileInfo.append(",");

          f_cnotificationprofileInfo.append(l_cNotification);
          f_cnotificationprofileInfo.append(",");

          switch (l_pNotification->GetNotificationType()) {
            case eFrameworkunifiedNotificationVar : {
                f_cnotificationprofileInfo.append("Regular");
              }
              break;

            case eFrameworkunifiedStateVar : {
                f_cnotificationprofileInfo.append("State");
              }
              break;

            case eFrameworkunifiedPersistedStateVar : {
                f_cnotificationprofileInfo.append("PersistenceState");
              }
              break;

            case eFrameworkunifiedPersistedStateUserVar : {
                f_cnotificationprofileInfo.append("UserPersistenceState");
              }
              break;

            case eFrameworkunifiedImmediatePersistedStateVar : {
                f_cnotificationprofileInfo.append("ImmediatePersistenceState");
              }
              break;

            case eFrameworkunifiedUnknown: {
                f_cnotificationprofileInfo.append("Unknown");
              }
              break;

            default:
              break;
          }
          f_cnotificationprofileInfo.append(",");

          ss << l_pNotification->GetMaxMessageSize();
          f_cnotificationprofileInfo.append(ss.str());
          ss.str("");
          f_cnotificationprofileInfo.append(",\"");

          l_uiSubscriberCnt = 0;
          for (l_itNotifReceiver_Iterator =  l_pSubscriberList->begin();
               l_pSubscriberList->end() != l_itNotifReceiver_Iterator;) {
            f_cnotificationprofileInfo.append(l_itNotifReceiver_Iterator->first);
            ++l_uiSubscriberCnt;

            ++l_itNotifReceiver_Iterator;

            if (l_pSubscriberList->end() != l_itNotifReceiver_Iterator) {
              f_cnotificationprofileInfo.append(",");
            }
          }

          f_cnotificationprofileInfo.append("\",");

          ss << l_uiSubscriberCnt;
          f_cnotificationprofileInfo.append(ss.str());
          ss.str("");
        }

        l_estatus = eFrameworkunifiedStatusOK;
      }
    }
  } catch (std::exception &exp) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Exception:: %s", exp.what());
    l_estatus = eFrameworkunifiedStatusFail;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

#endif
