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
/// \brief    This file contains declaration of singleton class CNotificationManager which is used
///       to manage notification information.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_NOTIFICATION_MANAGER_H_
#define NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_NOTIFICATION_MANAGER_H_

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <native_service/frameworkunified_types.h>
#include <native_service/ns_np_service.h>

#include <map>
#include <vector>
#include <string>
#include <iostream>

/// forward declaration of class
class CNotification;
class CPersistentData;
class CNotificationsToPersist;
class CStateNotification;

/// map of all the available notification.
typedef std::map<std::string, CNotification *> Notification_Type;

/// iterator for map of notifications.
typedef  Notification_Type::iterator Notification_Iterator_Type;

/// map of all available persistent notification and its data.
typedef std::map<std::string, CPersistentData *> Persistent_Type;

/// Iterator for map of persistent notifications.
typedef  Persistent_Type::iterator Persistent_Iterator_Type;

/**
 *  This class is used to manage the operations related to notification
 *  on registration, subscription, publication, etc.
 */
class CNotificationManager {
 public:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CNotificationManager
  /// Constructor of CNotificationManager class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CNotificationManager();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// ~CNotificationManager
  /// Destructor of CNotificationManager class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  ~CNotificationManager();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// NotificationpersistentserviceServiceOnRegisterEvents
  /// This function creates notification object depending on its type and if already created it
  /// adds the service name to the list in the notification object.
  ///
  /// \param  [IN] f_cservicename
  ///     std::string - name of service registering for notification
  ///
  /// \param  [IN] f_cnotificationname
  ///     std::string - notification name
  ///
  /// \param  [IN] f_uimsglength
  ///     UI_32 - Maximum length of Message
  ///
  /// \param  [IN] f_enotificationtype
  ///     EFrameworkunifiedPersistentVarType - type of notification
  ///
  /// \param  [IN] f_uidelay
  ///     UI_32 - Delay time for persistence
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus NotificationpersistentserviceServiceOnRegisterEvents(const std::string &f_cservicename,
                                        const std::string &f_cnotificationname,
                                        const UI_32 f_uimsglength,
                                        const EFrameworkunifiedNotificationType f_enotificationtype,
                                        const UI_32 f_uidelay = 0);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// NotificationpersistentserviceServiceOnUnRegisterEvents
  /// This function removes the name of the service from the notification object.
  ///
  /// \param  [IN] f_cservicename
  ///     std::string - name of service unregistering from notification
  ///
  /// \param  [IN] f_cnotificationname
  ///     std::string - notification name
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus NotificationpersistentserviceServiceOnUnRegisterEvents(const std::string &f_cservicename,
                                          const std::string &f_cnotificationname);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// NotificationpersistentserviceServiceOnPublishEvent
  /// This function stores the published data in the notification object in case of state and
  /// persistent notification.
  ///
  /// \param  [IN] f_cservicename
  ///     std::string - name of service registering for notification
  ///
  /// \param  [IN] f_cnotificationname
  ///     std::string - notification name
  ///
  /// \param  [IN] f_pmessage
  ///     PVOID - pointer to message
  ///
  /// \param  f_uimsgsize
  ///     UI_32 - size of Message
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus NotificationpersistentserviceServiceOnPublishEvent(const std::string &f_cservicename,
                                      const std::string &f_cnotificationname,
                                      PVOID f_pmessage,
                                      const UI_32 f_uimsgsize);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// NotificationpersistentserviceServiceOnSubscribeToEvent
  /// This function adds the name of the application to subscribers list in notification object.
  ///
  /// \param  [IN] f_csubscribername
  ///     std::string - name of application subscribing to notification
  ///
  /// \param  [IN] f_cnotificationname
  ///     std::string - notification name
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus NotificationpersistentserviceServiceOnSubscribeToEvent(const std::string &f_csubscribername,
                                          const std::string &f_cnotificationname);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// NotificationpersistentserviceServiceOnUnSubscribeFromEvent
  /// This function adds the name of the application to subscribers list in notification object.
  ///
  /// \param  [IN] f_csubscribername
  ///     std::string - name of application subscribing to notification
  ///
  /// \param  [IN] f_cnotificationname
  ///     std::string - notification name
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus NotificationpersistentserviceServiceOnUnSubscribeFromEvent(const std::string &f_csubscribername,
                                              const std::string &f_cnotificationname);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// NPGetPersistentNotificationData
  /// Get data for notification name
  ///
  /// \param  [IN] f_cnotificationname
  ///     std::string - name of notification
  /// \param  [OUT] f_pnotificationdata
  ///     PVOID - data corresponding to notification name
  /// \param  [IN] f_uidatasize
  ///     UI_32 - size of data
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus NPGetPersistentNotificationData(const std::string &f_cnotificationname,
                                             PVOID f_pnotificationdata, const UI_32 f_uidatasize);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// NotificationpersistentserviceServiceOnGetPersistentData
  /// This function is used to get the persistent data stored related to notification.
  ///
  /// \param  [IN] f_cnotificationname
  ///     std::string - Notification Name
  ///
  /// \param  [IN] f_creceivername
  ///     std::string - Source Name
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus NotificationpersistentserviceServiceOnGetPersistentData(const std::string &f_cnotificationname,
                                           const std::string &f_creceivername);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// NotificationpersistentserviceGetPersistentNotificationData
  /// This function is used to get the list of persistent notifications and the data associated
  /// with it.
  ///
  /// \param  [OUT] f_pvpersistnotification
  ///     CNotificationsToPersist - vector containing list of persistent notifications and
  ///     data associated with it.
  ///
  /// \param  [IN] f_enotificationtype
  ///     EFrameworkunifiedNotificationType - Type of notification
  ///
  /// \param  [IN] f_uinotificationpersistentservicepersistcategoryflag
  ///     UI_32 - Hex value from enum EFrameworkunifiedPersistCategory, representing data to persist
  ///     0 - persist orignal data and
  ///     1 - persist default data
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus NotificationpersistentserviceGetPersistentNotificationData(std::vector<CNotificationsToPersist *> *f_pvpersistnotification,
                                              const EFrameworkunifiedNotificationType f_enotificationtype,
                                              UI_32 f_uinotificationpersistentservicepersistcategoryflag);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// NotificationpersistentserviceSetPersistentNotificationData
  /// This function is used to create the persistent notifications object and fill the data
  /// related with it on system load.
  ///
  /// \param  [IN] f_pvpersistnotification
  ///     vector<CNotificationsToPersist> - vector containing list of notifications and data associated with it
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus NotificationpersistentserviceSetPersistentNotificationData(std::vector<CNotificationsToPersist *> *f_pvpersistnotification);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// GetNotificationInfo
  /// This function is used to get the notification information.
  ///
  /// \param  [IN] f_cnotificationname
  ///     std::string - Notification Name
  ///
  /// \param  [IN] l_pnotificationstopersist
  ///     CNotificationsToPersist - Notification info structure
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus GetNotificationInfo(const std::string &f_cnotificationname,
                                 CNotificationsToPersist *&l_pnotificationstopersist); // NOLINT (runtime/references)

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// NotificationpersistentserviceSetDefaultPersistentNotificationData
  /// This function is used to set the default data of persistent notification
  ///
  /// \param  [IN] f_cnotificationname
  ///     std::string - notification name
  /// \param  [IN] f_pmessage
  ///     PVOID - message data
  /// \param  [IN] f_uimsgsize
  ///     UI_32 - message size
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus NotificationpersistentserviceSetDefaultPersistentNotificationData(const std::string &f_cnotificationname,
                                                     PVOID f_pmessage,
                                                     const UI_32 f_uimsgsize);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// NotificationpersistentserviceSetPersistentCategory
  /// This function is used to set the persistent type of persistent notification
  ///
  /// \param  [IN] f_cnotificationname
  ///     std::string - notification name
  /// \param  [IN] f_epersistenttype
  ///     EFrameworkunifiedPersistCategory - persistent category
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus NotificationpersistentserviceSetPersistentCategory(const std::string &f_cnotificationname,
                                      const EFrameworkunifiedPersistCategory f_epersistcategory);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// NotificationpersistentservicePublishImmediateNotification
  /// This function publish the immediate notification f_cnotificationname to all its subscribers.
  /// This API is called when service updates the immediate notification data in persistent memory
  /// using synchronous API.
  ///
  /// \param  [IN] f_cservicename
  ///         const std::string& - name of service registering for notification
  ///
  /// \param  [IN] f_cnotificationname
  ///         const std::string& - notification name
  ///
  /// \param  [IN] f_pmessage
  ///         PVOID - pointer to message
  ///
  /// \param  f_uimsgsize
  ///         UI_32 - size of Message
  ///
  /// \return EFrameworkunifiedStatus
  //          EFrameworkunifiedStatus - eFrameworkunifiedStatusOK on success
  ///                      eFrameworkunifiedStatusInvldParam - invalid parameter
  ///                      eFrameworkunifiedStatusNullPointer - if notification not found
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus NotificationpersistentservicePublishImmediateNotification(const std::string &f_cservicename,
                                             const std::string &f_cnotificationname,
                                             PVOID f_pmessage,
                                             const UI_32 f_uimsgsize);

#ifdef NPP_PROFILEINFO_ENABLE

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// GetNotificationProfilingData
  /// This function is used to get the notification profiling information.
  ///
  /// \param  [OUT] f_tNotificationProfileInfo
  ///     std::string - all the notification info of all applications concated in a string
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus GetNotificationProfilingData(std::string &f_cnotificationprofileInfo); // NOLINT (runtime/references)

#endif

 private:
  Notification_Type *m_pmNotificationList;      /// < map containing list of all notifications.

  std::vector<std::string> *m_pvPersistentList;   /// < vector containing list of all persistent notifications.

  std::vector<std::string> *m_pvUserPersistentList;   /// < vector containing list of all user persistent notifications.

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CreateNotificationObject
  /// This function is used to get notification object from map if it exists else create new
  /// object as per type of notification.
  ///
  /// \param  [IN] f_cnotificationname
  ///     std::string - notification name
  ///
  /// \param  [IN] f_uimsglength
  ///     UI_32 - Maximum size of notification message
  ///
  /// \param  [IN] f_enotificationtype
  ///     EFrameworkunifiedPersistentVarType - Delay time for persistence
  ///
  /// \param  [IN] f_uidelay
  ///     UI_32 - Persistence Delay
  ///
  /// \return CNotification
  //      CNotification - pointer of notification object
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CNotification *CreateNotificationObject(const std::string &f_cnotificationname,
                                          const UI_32 f_uimsglength,
                                          const EFrameworkunifiedNotificationType f_enotificationtype,
                                          const UI_32 f_uidelay = 0);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// SearchNotification
  /// This function is used to search whether notification object is present in map or not.
  /// If present it sends the reference.    CNotification - reference to pointer of notification object
  ///
  /// \param  [IN] f_cnotificationname
  ///     std::string - notification name
  ///
  /// \return CNotification
  //      CNotification - pointer of notification object
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CNotification *SearchNotification(const std::string &f_cnotificationname);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// SearchPersistenceNotification
  /// This function is used to search whether the given persistent notification object
  /// is present in map or not.
  ///
  /// \param  [IN] f_cnotificationname
  ///     std::string - notification name
  ///
  /// \return CStateNotification
  //      CStateNotification - pointer of notification object
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CStateNotification *SearchPersistenceNotification(const std::string &f_cnotificationname);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CreateNotificationObjectToPersist
  /// Creates the CNotificationsToPersist object from notification object and the persistent data.
  ///
  /// \param  [IN] f_pnotification
  ///     CStateNotification - notification object ptr
  /// \param  [IN] f_pdata
  ///     CPersistentData - persistent data
  ///
  /// \return CNotification
  //      CNotification - pointer of notification object
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CNotificationsToPersist *CreateNotificationObjectToPersist(CStateNotification *f_pnotification,
                                                             const CPersistentData *f_pdata);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// RemoveUserSpecificNotificationEntry
  /// This function is used to get the singleton instance of class.
  ///
  /// \param
  ///
  /// \return EFrameworkunifiedStatus
  ///     EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus RemoveUserSpecificNotificationEntry();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// ChangeNotificationType
  /// This function creates new notification object depending on the type and replaces the old one.
  /// The subscribers list is copied from the old notification object is copied to the new one.
  ///
  /// \param  [IN] f_pnotification
  ///     CNotification* - old notification object
  ///
  /// \param  [IN] f_cservicename
  ///     std::string - name of service registering for notification
  ///
  /// \param  [IN] f_cnotificationname
  ///     std::string - notification name
  ///
  /// \param  [IN] f_uimsglength
  ///     UI_32 - Maximum length of Message
  ///
  /// \param  [IN] f_enotificationtype
  ///     EFrameworkunifiedPersistentVarType - new type of notification
  ///
  /// \param  [IN] f_uidelay
  ///     UI_32 - Delay time for persistence
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus ChangeNotificationType(CNotification *f_pnotification,
                                    const std::string &f_cservicename,
                                    const std::string &f_cnotificationname,
                                    const UI_32 f_uimsglength,
                                    const EFrameworkunifiedNotificationType f_enotificationtype,
                                    const UI_32 f_uidelay);
};

#endif  // NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_NOTIFICATION_MANAGER_H_
