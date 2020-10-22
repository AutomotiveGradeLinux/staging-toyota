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
/// \brief    The file conains declaration of CPersistentData, CNotificationsToPersist and CPersistDataHeader class.
///       This class is used to hold the persistent data related to state and persistent
///       notification.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_PERSISTENT_DATA_H_
#define NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_PERSISTENT_DATA_H_

#include <native_service/frameworkunified_types.h>
#include <string>

#ifdef AGL_STUB
#include <cstring>
#endif
/**
 *  This class holds the message and message length of notification data.
 */
class CPersistentData {
 private:
  // no members in private

 public:
  PVOID m_pMessage;         ///< pointer to message structure

  UI_32 m_uiMsgSize;          ///< length of message

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CPersistentData
  /// Constructor of CPersistentData class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CPersistentData();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// ~CPersistentData
  /// Destructor of CPersistentData class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  ~CPersistentData();
};

class CNotificationsToPersist {
 private:
  // no members in private

 public:
  std::string m_cNotificationName;      ///< Name of Notification

  UI_32 m_uiMaxMsgLength;           ///< Maximum data length of notification

  EFrameworkunifiedNotificationType m_ePersistentType;   ///< type of notification

  EFrameworkunifiedPersistCategory m_ePersistCategory;   ///< Persistent category

  std::string m_cPublisherName;       ///< Service Name

  CPersistentData *m_pPersistentData;     ///< Persistent data related to notification

  UI_32 m_uiDelay;              ///< Maximum delay for persistence

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CNotificationsToPersist
  /// Constructor of CNotificationsToPersist class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CNotificationsToPersist(): m_cNotificationName(""),
    m_uiMaxMsgLength(0),
    m_ePersistentType(eFrameworkunifiedUnknown),
    m_ePersistCategory(eFrameworkunifiedUserData),
    m_cPublisherName(""),
    m_pPersistentData(NULL),
    m_uiDelay(0) {
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// ~CNotificationsToPersist
  /// Destructor of CNotificationsToPersist class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  ~CNotificationsToPersist() {
    if (NULL != m_pPersistentData) {  // LCOV_EXCL_BR_LINE 6: m_pPersistentData can't be NULL
      delete m_pPersistentData;
      m_pPersistentData = NULL;
    }
  }
};

class CPersistDataHeader {
 private:
  // no members in private

 public:
  UI_32 m_uiSize;           ///< size of the associated data blob
  UI_32 m_uiOffset;         ///< Offset at which the data blob starts
  EFrameworkunifiedNotificationType m_ePersistentType;
  CHAR  m_cPublisherName[MAX_QUEUE_NAME_SIZE];
  CHAR  m_cNotificationName[MAX_STRING_SIZE_NOTIFICATION];
  UI_32 m_uiMaxMsgLength;   ///< max size of the associated data

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CPersistDataHeader
  /// Constructor of CPersistDataHeader class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CPersistDataHeader(): m_uiSize(0),
    m_uiOffset(0),
    m_ePersistentType(eFrameworkunifiedPersistedStateVar),
    m_uiMaxMsgLength(0) {
    std::memset(m_cPublisherName, 0, MAX_QUEUE_NAME_SIZE);
    std::memset(m_cNotificationName, 0, MAX_STRING_SIZE_NOTIFICATION);
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// ~CPersistDataHeader
  /// Destructor of CPersistDataHeader class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  ~CPersistDataHeader() {
  }
};

#endif  // NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_PERSISTENT_DATA_H_
