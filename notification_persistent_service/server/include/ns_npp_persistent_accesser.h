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
/// \ingroup  tag_NPPService
/// \brief    The file contains declaration of CPersistentAccesser class.
///       This is a abstract class. Derived classes of this class will store the data in different
///       file formats.
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_PERSISTENT_ACCESSER_H_
#define NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_PERSISTENT_ACCESSER_H_

#include <native_service/frameworkunified_types.h>
#include <string>
#include <vector>

class CPersistentData;
class CNotificationsToPersist;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// vector of all available persistent notification and its data.
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef std::vector<CNotificationsToPersist *> Persistent_Notification_List_Type;

/**
 *  This is a abstract class. Derived classes of this class will store the data in different
 *  file formats.
 */
class CPersistentAccesser {
 public:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CPersistentAccesser
  /// Constructor of CPersistentAccesser class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CPersistentAccesser() {}

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// ~CPersistentAccesser
  /// Destructor of CPersistentAccesser class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  virtual ~CPersistentAccesser() {  // LCOV_EXCL_START 14: Resident process, global instance not released
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  }
  // LCOV_EXCL_STOP

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// PersistData
  /// Persist data in persistent memory depending on file format.
  ///
  /// \param [IN] f_vpersistentnotificationlist
  ///      Persistent_Notification_List_Type - Data which has to be persisted.
  ///
  /// \param [IN] f_cmemfilepath
  ///      std::string - Read data from this file.
  ///
  /// \param [IN] f_epersistenttype
  ///      EFrameworkunifiedNotificationType - Type of persistent notification
  ///
  /// \param [IN] f_epersistcategory
  ///      EFrameworkunifiedPersistCategory - persist category
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus PersistData(std::string f_cmemfilepath,
                                 Persistent_Notification_List_Type *f_vpersistentnotificationlist,
                                 EFrameworkunifiedNotificationType f_epersistenttype,
                                 EFrameworkunifiedPersistCategory f_epersistcategory = eFrameworkunifiedUserData) = 0;

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// RetrieveData
  /// Retrieve data in persistent memory depending on file format.
  ///
  /// \param [IN] f_cmemfilepath
  ///      std::string - Write data from this file.
  ///
  /// \param [IN] f_vpersistentnotificationlist
  ///      Persistent_Notification_List_Type* - Retrieved data.
  ///
  /// \param [IN] f_epersistcategory
  ///      EFrameworkunifiedPersistCategory - persist category
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus RetrieveData(std::string f_cmemfilepath,
                                  Persistent_Notification_List_Type *&f_vpersistentnotificationlist, // NOLINT (runtime/references)
                                  EFrameworkunifiedPersistCategory f_epersistcategory = eFrameworkunifiedUserData) = 0;
};

#endif  // NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_PERSISTENT_ACCESSER_H_
