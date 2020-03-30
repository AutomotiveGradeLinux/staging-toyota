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

//////////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup tag_NS_NPPService
/// \ingroup  tag_NS_NPPService
///
//////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NS_NPPService
/// \brief    This class holds the message queue handles for notification subscribers
///
/// This is a singleton class which holds the message queue handles for notification subscribers.
/// This will ensure that NS_NPPService will hold only one handle per subscriber.
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_HANDLELIST_H_
#define NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_HANDLELIST_H_

#include <native_service/frameworkunified_types.h>
#include <string>
#include <map>
#include <utility>

typedef std::map<std::string, HANDLE> HandleList_Type;
typedef std::pair<HandleList_Type::iterator, bool> HandleListRetStatus_Type;



/**
 *  This class holds the message queue handles for notification subscribers
 */
class CHandleList {
 public:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// GetHandleList
  /// This function is used to get the singleton instance of class.
  /// NOTE: Not thread safe. Others threads would not use this function.
  ///
  /// \return CHandleList
  ///     CHandleList - singleton instance of class
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  static CHandleList *GetHandleList();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// ReleaseHandleList
  /// This function is used to release the instance of class.
  /// NOTE: Not thread safe. Others threads would not use this function.
  ///
  /// \param
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  static EFrameworkunifiedStatus ReleaseHandleList();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// AddHandleInList
  /// Add pair of subscriber name and corresponding handle in map.
  /// NOTE: Not thread safe. Others threads would not use this function.
  ///
  /// \param  [IN] f_csubscribername
  ///     std::string - name of application subscribing for notification
  /// \param  [IN] f_hmqhandle
  ///     HANDLE - Message queue handle of the subscriber
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus -
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus AddHandleInList(std::string f_csubscribername, HANDLE f_hmqhandle);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// RemoveHandleFromList
  /// Remove handle for subscriber from the list.
  /// Handle will actually get removed when all the notifications are unsubscribed by the subscriber
  /// NOTE: Not thread safe. Others threads would not use this function.
  ///
  /// \param  [IN] f_csubscribername
  ///     std::string - name of application subscribing for notification
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus -
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus RemoveHandleFromList(std::string f_csubscribername);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// GetSubscriberMqHandle
  /// Get the message queue handle of a subscriber.
  /// NOTE: Not thread safe. Others threads would not use this function.
  ///
  /// \param  [IN] f_csubscribername
  ///     std::string - name of application subscribing for notification
  ///
  /// \return HANDLE
  //      HANDLE - Message queue handle of the subscriber
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  HANDLE  GetSubscriberMqHandle(std::string f_csubscribername);

 private:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CHandleList
  /// Constructor of CHandleList class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CHandleList();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// ~CHandleList
  /// Destructor of CHandleList class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  ~CHandleList();

  HandleList_Type    m_mHandleList;     ///< map which stores all the message queue handle

  static CHandleList *m_psHandleList;  // NOLINT (readability/naming)
};
#endif  // NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_HANDLELIST_H_
