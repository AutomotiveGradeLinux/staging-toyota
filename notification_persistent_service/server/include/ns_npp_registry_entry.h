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
/// \brief    The file contains declaration of CRegistryEntry class.
///
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_REGISTRY_ENTRY_H_
#define NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_REGISTRY_ENTRY_H_

#include <native_service/frameworkunified_types.h>
#include <string>
#include "ns_npp_types.h"

/**
 *  This class contains the registry information  of the files/folders to be peresisted.
 */
class CRegistryEntry {
 public:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CRegistryEntry
  /// Constructor of CRegistryEntry class
  ///
  /// \param [OUT] f_ctag
  ///      std::string& - File or Folder tag
  ///
  /// \param [OUT] f_crequester
  ///      std::string& - Requester Name
  ///
  /// \param [OUT] f_cstoragepath
  ///      std::string& - File or Folder persistence storage path
  ///
  /// \param [IN] f_bisuserpersistence
  ///      BOOL - File or Folder is user specific
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  explicit CRegistryEntry(const std::string &f_ctag, const std::string &f_crequester,
                          const std::string &f_cstoragepath, BOOL f_bisuserpersistence);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CRegistryEntry
  /// Copy Constructor of CRegistryEntry class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CRegistryEntry(const CRegistryEntry &f_objin);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// operator=
  /// Operator overload =
  ///
  /// \param [IN] f_objin
  ///      CRegistryEntry& - In object.
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CRegistryEntry &operator=(const CRegistryEntry &f_objin);  // NOLINT (readability/naming)

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// SetReleasePath
  /// Set release path.
  ///
  /// \param [IN] f_creleasepath
  ///       std::string& - Path of the file/folder to be released.
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  VOID SetReleasePath(const std::string &f_creleasepath);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// SetBeingPersisted
  /// Set the file/folder persist member variable to true to notify that the file/folder is being
  /// persisted.
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  VOID SetBeingPersisted();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// AckReceived
  /// Ack from copy worker received. Process the ack.
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  VOID AckReceived();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// IsReleased
  /// Check if File/folder released or not.
  ///
  /// \param
  ///
  /// \return BOOL
  //      BOOL - True if file/folder is released else false
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  BOOL IsReleased() const;

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// HasntBeenPersisted
  /// File persisted or not
  ///
  /// \param
  ///
  /// \return BOOL
  //      BOOL - True if file is not persisted else false
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  BOOL HasntBeenPersisted() const;

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// IsPersisted
  /// File/Folder persisted or not
  ///
  /// \param
  ///
  /// \return BOOL
  //      BOOL - True if file/folder is persisted else false
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  BOOL IsPersisted() const;

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// IsAckPending
  /// Ack is pending or not.
  ///
  /// \param
  ///
  /// \return BOOL
  //      BOOL - True if ack is pending else false
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  BOOL IsAckPending() const;

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// GetPersistPath
  /// Get the persist path.
  ///
  /// \param
  ///
  /// \return std::string
  //      std::string - Returns the persist path.
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  std::string GetPersistPath() const;

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// GetReleasePath
  /// Get the release path.
  ///
  /// \param
  ///
  /// \return std::string
  //      std::string - Returns the release path.
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  std::string GetReleasePath() const;

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// GetLoadPath
  /// Get the load path.
  ///
  /// \param [IN] f_epersisttype
  ///       ENotificationpersistentservicePersistType - type of persisted data
  ///
  /// \param [IN] f_cusername
  ///       std::string - Current user name
  ///
  ///
  /// \return std::string
  //      std::string - Returns the load path.
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  std::string GetLoadPath(ENotificationpersistentservicePersistType f_epersisttype, std::string f_cusername) const;

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// GetRequester
  /// Get the name of requester.
  ///
  /// \param
  ///
  /// \return std::string
  //      std::string - Returns the name of requester.
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  std::string GetRequester() const;

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// GetTag
  /// Get the tag.
  ///
  /// \param
  ///
  /// \return std::string
  //      std::string - Returns the tag.
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  std::string GetTag() const;

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// IsUserPersistence
  /// Returns TRUE if it is a user persistence.
  ///
  /// \param
  ///
  /// \return BOOL
  //      BOOL - Returns TRUE if it is a user persistence.
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  BOOL IsUserPersistence();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// SetCurrentAction
  /// Set current copying action of a the corresponding tag.
  ///
  /// \param [IN] f_ecurrentaction
  ///       ENPS_Loadtype - Current action
  ///
  /// \return
  ////////////////////////////////////////////////////////////////////////////////////////////////
  VOID SetCurrentAction(ENPS_Loadtype f_ecurrentaction);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// ResetPersistedFlag
  /// Reset Persist flags
  ///
  /// \param
  ///
  /// \return
  ////////////////////////////////////////////////////////////////////////////////////////////////
  VOID ResetPersistedFlag();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// SetPersistProperties
  /// creates persistence path and stores the persist type
  ///
  /// \param [IN] f_epersisttype
  ///       ENotificationpersistentservicePersistType - Set persist type
  /// \param [IN] f_cusername
  ///       std::string - Username related to persist tag if any.
  ///
  ///
  /// \return
  ////////////////////////////////////////////////////////////////////////////////////////////////
  VOID SetPersistProperties(ENotificationpersistentservicePersistType f_epersisttype, std::string f_cusername = "");

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// GetPersistType
  /// Returns persist type of a job
  ///
  /// \param
  ///
  /// \return ENotificationpersistentservicePersistType
  //      ENotificationpersistentservicePersistType - Returns persist type
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  ENotificationpersistentservicePersistType GetPersistType() const;

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// SetPersistentCategory
  /// Sets the persistent type related to tag
  ///
  /// \param [IN] f_epersistcategory
  ///      EFrameworkunifiedPersistCategory - persist category
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus SetPersistentCategory(EFrameworkunifiedPersistCategory f_epersistcategory);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// GetPersistentCategory
  /// Returns persistent type of tag
  ///
  /// \param
  ///
  /// \return EFrameworkunifiedPersistCategory
  //      EFrameworkunifiedPersistCategory - Returns persistent type
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedPersistCategory GetPersistentCategory();

  ENPSPersistenceJobState m_eJobState;  // Current state of a job

 private:
  std::string    m_cTag;        // File/Folder tag
  std::string    m_cRequestor;      // Requester application name
  BOOL         m_bIsReleased;     // if file or folder released
  BOOL         m_bIsPersisted;    // if file or folder is persisted
  std::string    m_cPersistPath;    // Persist path
  std::string    m_cReleasePath;    // Release path
  std::string    m_cBasePath;     // Persist base path
  BOOL       m_bIsUserPersistence;  // If user persistence
  ENPS_Loadtype    m_eCurrentAction;    // Current action to load file to/from
  /// temporary/persistence memory
  ENotificationpersistentservicePersistType m_ePersistType;
  EFrameworkunifiedPersistCategory m_ePersistCategory;  // Persistent category
};

#endif  // NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_REGISTRY_ENTRY_H_
