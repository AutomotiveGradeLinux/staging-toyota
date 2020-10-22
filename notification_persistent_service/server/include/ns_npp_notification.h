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
/// \brief    This file contains declaration of class CNotification.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_NOTIFICATION_H_
#define NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_NOTIFICATION_H_

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <native_service/frameworkunified_types.h>
#include <map>
#include <string>
#include <iostream>
#include "ns_npp_notification_receiver.h"

typedef std::map<std::string, CNotificationReceiver *> NotifReceiver_Type;

/// Iterator of map of CNotificationReceiver
typedef NotifReceiver_Type::iterator NotifReceiver_Iterator_Type;

/**
 *  This class is base class for all the notification types classes.
 */
class CNotification {
 private:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CNotification
  /// Parameterless Constructor of CNotification class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CNotification();

 protected:
  std::string m_cNotificationName;    ///< Name of Notification

  UI_32 m_uiMaxMsgSize;     ///< Maximum size of notification data

  EFrameworkunifiedNotificationType m_ePersistentType;  ///< type of notification

  std::string m_cServiceName;     ///< List of services registering notification

  NotifReceiver_Type *m_pmSubscribersList;  ///< list of subscribers of notification

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// AddReceiverInMap
  /// This function adds the name of the service to receiver list of notification.
  ///
  /// \param  [IN] f_csubscribername
  ///     std::string - name of application subscribing for notification
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus AddReceiverInMap(const std::string &f_csubscribername);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// PublishData
  /// This function publishes the notification to subscribed clients.
  ///
  /// \param  [IN] f_hmsgqhandle
  ///     HANDLE - MessageQ Handle of ReceiverQ
  ///
  /// \param  [IN] f_pmessage
  ///     std::string - data of notification
  ///
  /// \param  [IN] f_uimsgsize
  ///     std::string - length of data
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus PublishData(HANDLE f_hmsgqhandle,
                         const PVOID f_pmessage,
                         const UI_32 f_uimsgsize);

 public:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CNotification
  /// Constructor of CNotification class
  ///
  /// \param  [IN] f_cnotificationname
  ///     std::string - Notification name
  ///
  /// \param  [IN] f_uimaxmsgsize
  ///     UI_32 - Maximum size of notification data
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CNotification(const std::string &f_cnotificationname,
                const UI_32 f_uimaxmsgsize);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// ~CNotification
  /// Destructor of CNotification class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  virtual ~CNotification();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// AddEventReciever
  /// This function adds the name of the application to receiver list of particular notification.
  ///
  /// \param  [IN] f_csubscribername
  ///     std::string - name of application subscribing for notification
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus AddEventReciever(const std::string &f_csubscribername);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// Publish
  /// This function publishes the notification to subscribed clients.
  ///
  /// \param  [IN] f_cservicename
  ///     std::string - name of service publishing the notification
  ///
  /// \param  [IN] f_pmessage
  ///     std::string - data of notification
  ///
  /// \param  [IN] f_uimsgsize
  ///     std::string - length of data
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus Publish(const std::string &f_cservicename,
                             PVOID f_pmessage,
                             const UI_32 f_uimsgsize);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// DeleteEventReciever
  /// This function deletes the name of application from receivers list.
  ///
  /// \param  [IN] f_csubscribername
  ///     std::string - name of application
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus DeleteEventReciever(const std::string &f_csubscribername);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// SetNewSubscribersList
  /// This function sets the subscribers list of notification
  ///
  /// \param  [IN] f_pnotification
  ///     CNotification - notification object containing subscribers list
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus SetNewSubscribersList(CNotification *f_pnotification);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// SetEventPublisher
  /// This function set the publisher name to current received service name
  ///
  /// \param  [IN] f_cservicename
  ///     std::string - name of application
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus SetEventPublisher(const std::string &f_cservicename);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// ResetEventPublisher
  /// This function resets the publisher name to NULL
  ///
  /// \param  [IN] f_cservicename
  ///     std::string - name of application
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus ResetEventPublisher(const std::string &f_cservicename);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// IsServiceRegistered
  /// This function checks whether the notification is registered with any service or not.
  ///
  /// \param
  ///
  /// \return BOOL
  //      BOOL - true,if subscriber list is empty
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  BOOL IsServiceRegistered();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// IsSubscribersListEmpty
  /// This function is used to check whether any service is subscribed to notification
  ///
  /// \param
  ///
  /// \return BOOL
  //      BOOL - true,if subscriber list is empty
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  BOOL IsSubscribersListEmpty();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// GetNotificationName
  /// This function is used to get the notification name.
  ///
  /// \param
  ///
  /// \return std::string
  //      std::string - notification name
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  std::string GetNotificationName();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// GetPublisherName
  /// This function is used to get the publisher name of notification.
  ///
  /// \param
  ///
  /// \return std::string
  //      std::string - service name
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  std::string GetPublisherName();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// GetPersistenceType
  /// This function is used to get the type of notification.
  ///
  /// \param
  ///
  /// \return EFrameworkunifiedPersistentVarType
  //      EFrameworkunifiedPersistentVarType - persistent type
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedNotificationType GetNotificationType();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// GetMaxMessageSize
  /// This function is used to get the max size of data of notification message.
  ///
  /// \param
  ///
  /// \return UI_32
  //      UI_32 - Max Message Size
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  UI_32 GetMaxMessageSize();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// ResetMaxMessageSize
  /// This function reset the max size of data that can be published with notification
  ///
  /// \param  [IN] f_uilength
  ///     std::string - Max size for notification data
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus ResetMaxMessageSize(const UI_32 f_uilength);

#ifdef NPP_PROFILEINFO_ENABLE

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// GetSubscriberList
  /// Returns the list of subscribers subscribed to notification
  ///
  /// \param
  ///
  /// \return NotifReceiver_Type
  //      NotifReceiver_Type - subscriber's list of notification
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  NotifReceiver_Type *GetSubscriberList();

#endif
};

#endif  // NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_NOTIFICATION_H_
