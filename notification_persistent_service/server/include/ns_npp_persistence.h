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
/// \brief    The file contains declaration of CPersistence class.
///           This is a abstract class for file and folder persistency.
///
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_PERSISTENCE_H_
#define NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_PERSISTENCE_H_

#include <native_service/frameworkunified_types.h>
#include <native_service/ns_np_service.h>
#include <string>
#include <map>
#include <list>

#include "ns_npp_notificationpersistentservicelog.h"
#include "ns_npp_copy_worker.h"

class CRegistryEntry;

typedef std::string TSourceName;
typedef std::string TTag;
typedef std::map< TTag, CRegistryEntry > TTagRegistryList;
typedef TTagRegistryList::iterator TTagRegistryListItr;

typedef std::map<TSourceName, TTagRegistryList> TSourceRegistryList;
typedef TSourceRegistryList::iterator TSourceRegistryListItr;

typedef std::list<NSP_CopyInfoCmd> TPendingJobs;
typedef TPendingJobs::iterator TPendingJobsItr;

/**
 *  This is a abstract class for file and folder persistence.
 */
class CPersistence {
 public:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CPersistence
  /// Constructor of CPersistence class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CPersistence();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// ~CPersistence
  /// Destructor of CPersistence class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  virtual ~CPersistence();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// Register
  ///  Register file/folder tag for persistence.
  ///
  /// \param [IN] f_cappname
  ///      string - Name of the application requesting for persistence
  ///
  /// \param [IN] f_ctag
  ///      string - File/Folder corresponding to this tag will be retrieved.
  ///
  /// \param [IN] bisuserpersistence
  ///      BOOL - TRUE if user persistence else FALSE
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus Register(std::string f_crequestorappname, std::string f_ctag, BOOL bisuserpersistence);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// Release
  /// Entry for the file/Folder is stored in map for persistence.
  /// If f_bPersist is TRUE file/folder will be persisted immediately else will be persist on shutdown
  ///
  /// \param [IN] f_cappname
  ///      string - Name of the application requesting for persistence
  ///
  /// \param [IN] f_ctag
  ///      string - File/Folder will be persist against this tag.
  ///
  /// \param [IN] f_cmempath
  ///      string - File/Folder path that is to be persisted.
  ///
  /// \param [IN] enotificationpersistentservicereleasetype
  ///      EFrameworkunifiedReleaseType - eFrameworkunifiedNotOnRelease = 0  :not on release
  ///                        eFrameworkunifiedPersistOnShutdown :persist on shutdown
  ///                        eFrameworkunifiedPersistInstantly  :persist instantly
  ///
  /// \param [IN] f_cusername
  ///      std::string - If tag is registered as user, then f_cusername holds the name of user,
  ///            else an empty string
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus Release(std::string f_cappname,
                             std::string f_ctag,
                             std::string f_cmempath,
                             EFrameworkunifiedReleaseType enotificationpersistentservicereleasetype,
                             std::string f_cusername) = 0;


  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// Load
  ///  Load file/folder from persistent memory to the specified location.
  ///
  /// \param [IN] f_cappname
  ///      string - Name of the application requesting for persistence
  ///
  /// \param [IN] f_ctag
  ///      string - File/Folder corresponding to this tag will be retrieved.
  ///
  /// \param [IN] f_cretrievepath
  ///      string - Path to retrieve.
  ///
  /// \param [IN] f_cusername
  ///      std::string - If tag is registered as user, then f_cusername holds the name of user,
  ///            else an empty string
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus Load(std::string f_cappname,
                          std::string f_ctag,
                          std::string f_cretrievepath,
                          std::string f_cusername) = 0;

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// Persist
  /// Persist file/folder immediately
  ///
  /// \param [IN] f_objregistryentry
  ///      CRegistryEntry& - Registry entry job object to persist.
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus Persist(CRegistryEntry &f_objregistryentry); // NOLINT (runtime/references)

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// AckReceivedFromWorker
  ///
  ///
  /// \param [in] pSource
  ///         PCSTR - Name of the source.
  /// \param [in] f_ctag
  ///          PCSTR - file/folder tag.
  /// \param [in] f_bcopystatus
  ///         BOOL - Status of file/folder copy by copy worker.
  /// \param [in] f_eloadtype
  ///         ENPS_Loadtype - Type of load release or load.
  ///
  /// \return status
  ///         EFrameworkunifiedStatus - success or error
  ////////////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus AckReceivedFromWorker(PCSTR f_csource, PCSTR f_ctag, BOOL f_bcopystatus,
                                           ENPS_Loadtype f_eloadtype);

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// PersistAllReleaseRequests
  /// Persist all files/folders which are not persisted yet.
  ///
  /// \param [in] f_uinotificationpersistentservicepersistcategoryflag
  ///     UI_32 - Hex value from enum EFrameworkunifiedPersistCategory, representing data to persist
  ///     0 - persist orignal data and
  ///     1 - persist default data
  ///
  /// \return status
  ///         EFrameworkunifiedStatus - success or error
  ////////////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus PersistAllReleaseRequests(UI_32 f_uinotificationpersistentservicepersistcategoryflag);

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// HaveAllReleaseRequestsPersisted
  /// Checks if all release requests are processed or not
  ///
  /// \param [out] f_ctagnotreleased
  ///     std::string - list of tags not released by application
  ///
  /// \return BOOL
  ///         TRUE if all release requests are processed else false
  ////////////////////////////////////////////////////////////////////////////////////////////
  BOOL HaveAllReleaseRequestsPersisted(std::string &f_ctagnotreleased); // NOLINT (runtime/references)

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// ResetPersistFlag
  /// Resets persist flag.
  ///
  /// \param none
  ///
  /// \return none
  ////////////////////////////////////////////////////////////////////////////////////////////
  VOID ResetPersistFlag();

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// PersistAllUserRequests
  /// Persist all files/folders which are not persisted yet.
  ///
  ///
  /// \return status
  ///         EFrameworkunifiedStatus - success or error
  ////////////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus PersistAllUserRequests();

  // virtual VOID ListAllInMap() = 0;

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// SetReadThreadHandle
  /// Set read thread handle.
  ///
  /// \param [in] f_hreadthread
  ///        HANDLE - Handle of the read thread.
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  VOID SetReadThreadHandle(HANDLE f_hreadthread);

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// SetWriteThreadHandle
  /// Set write thread handle.
  ///
  /// \param [in] f_hwritethread
  ///        HANDLE - Handle of the write thread.
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  VOID SetWriteThreadHandle(HANDLE f_hwritethread);

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// GetStoragePath
  /// Get the permanent storage path. Get it from NS_NPPService's config file.
  ///
  /// \return std::string
  ///         std::string - Permanant storage path
  ////////////////////////////////////////////////////////////////////////////////////////////
  static std::string GetStoragePath();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// SetPersistentCategory
  /// Sets the persist type of file/folder
  ///
  /// \param [IN] f_crequestorname
  ///      string - Application Name
  ///
  /// \param [IN] f_ctag
  ///      string - File/Folder will be persist against this tag.
  ///
  /// \param [IN] f_epersistcategory
  ///      EFrameworkunifiedPersistCategory - persistent category
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus SetPersistentCategory(const std::string &f_crequestorname,
                                   const std::string &f_ctag,
                                   EFrameworkunifiedPersistCategory f_epersistcategory);

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// IsUserPersistence
  /// Check if the file/folder being persisted is user specific
  ///
  /// \param [in] - f_ctag
  ///         std::string - File/Folder Tag
  ///
  /// \return status
  ///         BOOL - TRUE for user specific persistence
  ///          FALSE  for global persistence
  ///////////////////////////////////////////////////////////////////////////////////////////
  virtual BOOL IsUserPersistence(std::string f_ctag);

#ifdef NPP_PROFILEINFO_ENABLE

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// GetPersistenceProfilingData
  /// This function is used to get the persistence info
  ///
  /// \param [out] f_cpersistenceprofileinfo
  ///         std::string - Persistence info concated in a string
  ///
  /// \return EFrameworkunifiedStatus
  ///     EFrameworkunifiedStatus - success or failure
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus GetPersistenceProfilingData(std::string &f_cpersistenceprofileinfo); // NOLINT (runtime/references)

#endif

 protected:
  EFrameworkunifiedStatus ProcessReleaseRequest(std::string f_crequesterappname,
                                   std::string f_ctag,
                                   ENotificationpersistentservicePersistType f_epersisttype,
                                   std::string f_cmempath,
                                   EFrameworkunifiedReleaseType enotificationpersistentservicereleasetype,
                                   std::string f_cusername);

  EFrameworkunifiedStatus ProcessLoadRequest(std::string f_crequesterappname,
                                std::string f_ctag,
                                ENotificationpersistentservicePersistType f_epersisttype,
                                std::string f_cretrievepath,
                                std::string f_cusername);

  VOID AddRequestData(NSP_CopyInfoCmd &f_tcpinfo, // NOLINT (runtime/references)
                      std::string f_sourcepath,
                      std::string f_destpath,
                      std::string f_crequesterappname,
                      std::string f_ctag,
                      ENPS_Loadtype f_eloadtype,
                      ENotificationpersistentservicePersistType f_epersisttype);

  EFrameworkunifiedStatus SendRequestMessage(NSP_CopyInfoCmd &f_tcpinfo); // NOLINT (runtime/references)

  std::string m_cStoragePath;  ///< Persistent memory storage path

  NOTIFICATIONPERSISTENTSERVICE_COMPRESSION_TYPE m_eCompressionType;  ///< Compression type for file/folder persistency

  HANDLE m_hNSWriteToPersistentMem;  ///< Child thread handle, copies file to persistent memory.

  HANDLE m_hNSReadFromPersistentMem;  ///< Child thread handle, copies file from persistent memory.

  // TODO(my_username): Remove this member var. Once the API to get application handle is available, use that.
  HANDLE m_hAppHandle;

  TSourceRegistryList m_mPersistRegistry;

  TPendingJobs m_lPendingJobs;  ///< stores the list of pending jobs sequentially

  BOOL m_bPersist;  ///< persist or not. It will be set to TRUE either on shutdown or userchange

  UI_32 m_uiNotificationpersistentservicePersistCategoryFlag;  ///< Stores the persist category flag value on shutdown request.
  ///< It is needed to decide which data to persist, if
  ///< release request is received by NSNPP after shutdown request
};
#endif  // NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_PERSISTENCE_H_
