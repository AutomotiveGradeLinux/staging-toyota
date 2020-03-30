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
/// \brief    This file contains declaration of class CStateNotification.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_STATE_NOTIFICATION_H_
#define NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_STATE_NOTIFICATION_H_

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <string>
#include "ns_npp_notification.h"

class CPersistentData;

/**
 *  This class inherits CNotification class and implements state and persistent notification
 *  related operations.
 */
class CStateNotification : public CNotification {
 public:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CStateNotification
  /// Constructor of CStateNotification class
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
  CStateNotification(const std::string &f_cnotificationname,
                     const UI_32 f_uimaxmsgsize);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// ~CStateNotification
  /// Destructor of CStateNotification class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  virtual ~CStateNotification();

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
  EFrameworkunifiedStatus AddEventReciever(const std::string &f_csubscribername);

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
  /// GetPersistentData
  /// This function get the data related to notification
  ///
  /// \param
  ///
  /// \return CPersistentData
  //      CPersistentData - notification data
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  const CPersistentData *GetPersistentData();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// SetPersistentData
  /// This function is used to set the data related to notification
  ///
  /// \param  [IN] f_pmessage
  ///     PVOID - Message data
  ///
  /// \param  [IN] f_msgsize
  ///     UI_32 - Size of Message data
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus SetPersistentData(PVOID f_pmessage,
                                       const UI_32 f_msgsize);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// GetDefaultPersistentData
  /// This function get the default data(if any) related to notification
  ///
  /// \param
  ///
  /// \return CPersistentData
  //      CPersistentData - notification data
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  const CPersistentData *GetDefaultPersistentData();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// SetDefaultPersistentData
  /// This function is used to set the default data related to notification
  ///
  /// \param  [IN] f_pmessage
  ///     PVOID - Message data
  ///
  /// \param  [IN] f_msgsize
  ///     UI_32 - Size of Message data
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus SetDefaultPersistentData(PVOID f_pmessage,
                                              const UI_32 f_msgsize);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// IsPublished
  /// This functions returns the published status of notification.
  ///
  /// \param
  ///
  /// \return BOOL
  //      BOOL - returns true if published before
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  BOOL IsPublished();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// ResetMaxMessageSize
  /// This function reset the max size of data that can be published with notification.
  /// Also deletes the old persistent data and default data.
  ///
  /// \param  [IN] f_uilength
  ///         std::string - Max size for notification data
  ///
  /// \return EFrameworkunifiedStatus
  //          EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus ResetMaxMessageSize(const UI_32 f_uilength);

 protected:
  CPersistentData *m_pData;  ///< persistent data of notification

  CPersistentData *m_pDefaultData;  ///< persistent data of notification

  BOOL m_bWasPublished;       ///< Flag to check whether publisher had published notification

 private:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// SetData
  /// This function is used to set the persistent data
  ///
  /// \param  [IN] f_pdata
  ///     CPersistentData - persistent data ptr
  ///
  /// \param  [IN] f_pmessage
  ///     PVOID - Message data
  ///
  /// \param  [IN] f_msgsize
  ///     UI_32 - Size of Message data
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus SetData(CPersistentData *f_pdata,
                     PVOID f_pmessage,
                     const UI_32 f_msgsize);
};

#endif  // NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_STATE_NOTIFICATION_H_
