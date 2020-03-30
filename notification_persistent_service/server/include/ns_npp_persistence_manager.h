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
/// \ingroup  tag_NPPService
/// \brief    The file contains declaration of CPersistenceManager class.
///           This class acts as a manager for notification persistent data storage
///           and file and folder persistence.
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_PERSISTENCE_MANAGER_H_
#define NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_PERSISTENCE_MANAGER_H_


#include <native_service/frameworkunified_types.h>
#include <native_service/ns_np_service.h>
#include <map>
#include <string>
#include <vector>
#include "ns_npp_types.h"
#include "ns_npp_persistent_accesser.h"

class CPersistence;
class CPersistentData;
class CNotificationsToPersist;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// vector of all available persistent notification and its data.
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef std::vector<CNotificationsToPersist *> Persistent_Notification_List_Type;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Map of all persist file type. i.e. FilePersistence and FolderPersistence
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef std::map<ENotificationpersistentservicePersistType, CPersistence *> Persist_Type;

typedef Persist_Type::iterator Persist_Type_Iter;

/**
 *  This class acts as a manager for storing notification data,file and folder in
 *  persistent memory.
 */
class CPersistenceManager {
 public:
  /// Disable Persistence
  static BOOL m_bPersistenceDisabled;  // NOLINT (readability/naming)

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CPersistenceManager
  /// Constructor of CPersistenceManager class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CPersistenceManager();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// ~CPersistenceManager
  /// Destructor of CPersistenceManager class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  ~CPersistenceManager();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// NotificationpersistentserviceRegister
  /// Registers a tag for the file or folder. This tag will be used for releasing or loading a
  /// file or folder.
  ///
  /// \param [IN] f_cappname
  ///      string - Name of the application requesting for persistence
  ///
  /// \param [IN] f_ctag
  ///      string - File/Folder will be persist against this tag.
  ///
  /// \param [IN] f_epersisttype
  ///      ENotificationpersistentservicePersistType - ENOTIFICATIONPERSISTENTSERVICEPERSISTFILE - Tag to register is for a file
  ///                ENOTIFICATIONPERSISTENTSERVICEPERSISTFOLDER - Tag to register is for a folder
  ///
  /// \param [IN] bisuserpersistence

  ///      BOOL - TRUE if user persistence else FALSE
  ///
  /// \return EFrameworkunifiedStatus
  ///     EFrameworkunifiedStatus - success or failure status
  ///
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus NotificationpersistentserviceRegister(std::string f_cappname,
                         std::string f_ctag,
                         ENotificationpersistentservicePersistType f_epersisttype,
                         BOOL bisuserpersistence);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// NotificationpersistentserviceRelease
  /// Entry for the file or folder is stored in map for persistence.
  /// If f_bPersist is TRUE file or folder will be persist immediately else will be persist
  /// on shutdown
  ///
  /// \param [IN] f_cappname
  ///      string - Name of the application requesting for persistence
  ///
  /// \param [IN] f_ctag
  ///      string - File/Folder will be persist against this tag.
  ///
  /// \param [IN] f_cmempath
  ///      string - File/Folder which needs to persist.
  ///
  /// \param [IN] enotificationpersistentservicereleasetype
  ///      EFrameworkunifiedReleaseType - eFrameworkunifiedNotOnRelease = 0  :not on release
  ///                        eFrameworkunifiedPersistOnShutdown :persist on shutdown
  ///                        eFrameworkunifiedPersistInstantly  :persist instantly
  ///
  /// \param [IN] f_epersisttype
  ///      ENotificationpersistentservicePersistType - ENOTIFICATIONPERSISTENTSERVICEPERSISTFILE - Persist a file to a persistent memory
  ///                ENOTIFICATIONPERSISTENTSERVICEPERSISTFOLDER - Persist a folder to a persistent memory
  ///
  /// \param [IN] f_cusername
  ///      std::string - If tag is registered as user, then f_cusername holds the name of user,
  ///            else an empty string
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus NotificationpersistentserviceRelease(std::string f_cappname,
                        std::string f_ctag,
                        std::string f_cmempath,
                        EFrameworkunifiedReleaseType enotificationpersistentservicereleasetype,
                        ENotificationpersistentservicePersistType f_epersisttype,
                        std::string f_cusername);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// NotificationpersistentserviceLoad
  ///  Load file/folder from persistent memory to the specified location.
  ///
  /// \param [IN] f_cappname
  ///      string - Name of the application requesting for persistence
  ///
  /// \param [IN] f_ctag
  ///      string - File/Folder corresponding to this tag will be retrieved.
  ///
  /// \param [IN] f_cretrievepath
  ///      string - Filepath for retrieved file/folder.
  ///
  /// \param [IN] f_epersisttype
  ///      ENotificationpersistentservicePersistType - ENOTIFICATIONPERSISTENTSERVICEPERSISTFILE - Load a file from persistent memory
  ///                ENOTIFICATIONPERSISTENTSERVICEPERSISTFOLDER - Load a folder from persistent memory
  ///
  /// \param [IN] f_cusername
  ///      std::string - If tag is registered as user, then f_cusername holds the name of user,
  ///            else an empty string
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus NotificationpersistentserviceLoad(std::string f_cappname,
                     std::string f_ctag,
                     std::string f_cretrievepath,
                     ENotificationpersistentservicePersistType f_epersisttype,
                     std::string f_cusername);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// AckReceivedFromWorker
  ///  Send release ack to file/folder persistence object.
  ///
  /// \param [IN] f_csource
  ///      PCSTR - Source of released file/folder
  ///
  /// \param [IN] f_ctag
  ///      PCSTR - Tag of released file/folder
  ///
  /// \param [IN] f_epersisttype
  ///      ENotificationpersistentservicePersistType - ENOTIFICATIONPERSISTENTSERVICEPERSISTFILE - Ack for file persistence object
  ///                ENOTIFICATIONPERSISTENTSERVICEPERSISTFOLDER - Ack for folder persistence object
  ///
  /// \param [in] f_bcopystatus
  ///         BOOL - Status of file/folder copy by worker.
  ///
  /// \param [in] f_eloadtype
  ///         ENPS_Loadtype - Type of load release or load.
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus AckReceivedFromWorker(PCSTR f_csource, PCSTR f_ctag, ENotificationpersistentservicePersistType f_epersisttype,
                                   BOOL f_bcopystatus, ENPS_Loadtype f_eloadtype);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// NotificationpersistentserviceSaveNotificationData
  /// Save notification data in a persistent file.
  ///
  /// \param [IN] f_vpersistentnotificationlist
  ///      Persistent_Notification_List_Type - List of all notifications and corresponding data.
  ///
  /// \return EFrameworkunifiedStatus
  ///     EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus NotificationpersistentserviceSaveNotificationData(Persistent_Notification_List_Type *f_vpersistentnotificationlist);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// NotificationpersistentserviceSaveUserNotificationData
  /// Save notification data of user in a persistent file.
  ///
  /// \param [IN] f_vpersistentnotificationlist
  ///      Persistent_Notification_List_Type - List of all notifications and corresponding data.
  ///
  /// \return EFrameworkunifiedStatus
  ///     EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus NotificationpersistentserviceSaveUserNotificationData(Persistent_Notification_List_Type *f_vpersistentnotificationlist);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// NotificationpersistentserviceReadNotificationData
  /// Get the list of all persistent notifications from a persistent memory and store it in a map.
  ///
  /// \param [OUT] f_vpersistentnotificationlist
  ///      Persistent_Notification_List_Type - Retrieved list of all notifications and corresponding data
  ///
  /// \return EFrameworkunifiedStatus
  ///     EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus NotificationpersistentserviceReadNotificationData(Persistent_Notification_List_Type *&f_vnotificationlist); // NOLINT (runtime/references)

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// NotificationpersistentserviceReadUserNotificationData
  /// Get the list of all user persistent notifications from a persistent memory and store it in a map.
  ///
  /// \param [OUT] f_vpersistentnotificationlist
  ///      Persistent_Notification_List_Type - Retrieved list of all notifications and corresponding data
  ///
  /// \return EFrameworkunifiedStatus
  ///     EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus NotificationpersistentserviceReadUserNotificationData(Persistent_Notification_List_Type *&f_vnotificationlist); // NOLINT (runtime/references)

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// NotificationpersistentserviceReadNorNotificationData
  /// Get the list of all Nor persistent notifications from a persistent memory and store it in a map.
  ///
  /// \param [OUT] f_vpersistentnotificationlist
  ///      Persistent_Notification_List_Type - Retrieved list of all notifications and corresponding data
  ///
  /// \return EFrameworkunifiedStatus
  ///     EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus NotificationpersistentserviceReadNorNotificationData(Persistent_Notification_List_Type *&f_vnotificationlist); // NOLINT (runtime/references)

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// PersistAllReleaseRequests
  /// Persist all files and folder contained in the map in persistent memory.
  ///
  /// \param [in] f_uinotificationpersistentservicepersistcategoryflag
  ///     UI_32 - Hex value from enum EFrameworkunifiedPersistCategory, representing data to persist
  ///     0 - persist orignal data and
  ///     1 - persist default data
  ///
  /// \return EFrameworkunifiedStatus
  ///     EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus PersistAllReleaseRequests(UI_32 f_uinotificationpersistentservicepersistcategoryflag);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// NotificationpersistentservicePersistAllUserRequests
  /// Persist all user files and folder contained in the map in persistent memory.
  ///
  ///
  /// \return EFrameworkunifiedStatus
  ///     EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus NotificationpersistentservicePersistAllUserRequests();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// SetReadThreadHandle
  /// Pass the handle of the read thread to the object of file/folder persistence
  ///
  /// \param [IN] f_hreadthread
  ///      HANDLE - Handle of read thread.
  ///
  /// \return
  ///
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  VOID SetReadThreadHandle(HANDLE f_hreadthread);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// SetWriteThreadHandle
  /// Pass the handle of the write thread to the object of file/folder persistence
  ///
  /// \param [IN] f_hwritethread
  ///      HANDLE - Handle of write thread.
  ///
  /// \return
  ///
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  VOID SetWriteThreadHandle(HANDLE f_hwritethread);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// SetNorPersistenceThreadHandle
  /// Pass the handle of the Nor persistence write thread to the object of file/folder persistence
  ///
  /// \param [IN] f_hwritethread
  ///      HANDLE - Handle of write thread.
  ///
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  VOID SetNorPersistenceThreadHandle(HANDLE f_hwritethread);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// SetUserPersistentPath
  /// Set user persistent path.
  ///
  /// \param [IN] f_cusername
  ///      std::string - Name of user
  ///
  /// \return
  ///
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  VOID SetUserPersistentPath(std::string f_cusername);

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// IsUserPersistence
  /// Check if the persistence is user specific
  ///
  /// \param [in] - f_ctag
  ///         std::string - File/folder Tag
  ///
  /// \param [in] - f_epersisttype
  ///         ENotificationpersistentservicePersistType - persistent type
  ///
  /// \return status
  ///         BOOL - TRUE for user specific persistence
  ///          FALSE  for global persistence
  ///////////////////////////////////////////////////////////////////////////////////////////
  BOOL IsUserPersistence(std::string f_ctag, ENotificationpersistentservicePersistType f_epersisttype);

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// HaveAllReleaseRequestsPersisted
  /// Checks if all files and immediate persistent data are persisted.
  ///
  /// \param
  ///
  /// \return BOOL
  ///         TRUE if all release requests are processed else false
  ////////////////////////////////////////////////////////////////////////////////////////////
  BOOL HaveAllReleaseRequestsPersisted();

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// ResetPersistFlag
  /// Resets persist flag.
  ///
  /// \param none
  ///
  /// \return none
  ////////////////////////////////////////////////////////////////////////////////////////////
  VOID ResetPersistFlag();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// ClearPersistenceData
  ///  Deletes the data from the persistent memory
  ///
  /// \param [in] f_enotificationpersistentserviceclearpersistencescope
  ///         EFrameworkunifiedClearPersistence - data to be deleted from memory
  ///
  /// \return EFrameworkunifiedStatus
  ///     EFrameworkunifiedStatus - success or failure
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus ClearPersistenceData(const EFrameworkunifiedClearPersistence &f_enotificationpersistentserviceclearpersistencescope);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// NotificationpersistentserviceSetPersistentCategory
  /// Sets the persist type of file or folder
  ///
  /// \param [IN] f_crequestorname
  ///      string - Application name
  ///
  /// \param [IN] f_ctag
  ///      string - File/Folder will be persist against this tag.
  ///
  /// \param [IN] f_epersistcategory
  ///      EFrameworkunifiedPersistCategory - persistent category
  ///
  /// \param [IN] f_bPersist
  ///      ENotificationpersistentservicePersistType - file or folder
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus NotificationpersistentserviceSetPersistentCategory(std::string f_crequestorname,
                                      std::string f_ctag,
                                      EFrameworkunifiedPersistCategory f_epersistcategory,
                                      ENotificationpersistentservicePersistType f_epersisttype);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// PersistNORData
  /// Sends the message to Immediate Persistence Thread to Persists the data immediately or reset
  /// the NOR data depending on persist category flag during shutdown irrespective of delay.
  ///
  /// \param f_eshutdowntype
  ///        EFrameworkunifiedShutdownType - shutdown type - normal, quick, data reset
  ///
  /// \param f_uinotificationpersistentservicepersistcategoryflag
  ///      UI_32 - flag representing whether to persist or reset data.
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus PersistNORData(EFrameworkunifiedShutdownType f_eshutdowntype, UI_32 f_uinotificationpersistentservicepersistcategoryflag);

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// SetImmediateDataPersistedStatus
  /// Set/Reset the persistence status of immediate persistence data
  ///
  /// \param [in] - f_bstatus
  ///               BOOL
  ///                 TRUE  - immediate persistent data are persisted
  ///                 FALSE - immediate persistent data not persisted
  ///
  /// \return
  ///////////////////////////////////////////////////////////////////////////////////////////
  VOID SetImmediateDataPersistedStatus(BOOL f_bstatus);

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// SetFilePersistedStatus
  /// Set/Reset the persistence status of files and folders
  ///
  /// \param [in] - f_bstatus
  ///               BOOL
  ///                 TRUE  - all files are persisted
  ///                 FALSE - all files and folders are not persisted
  ///
  /// \return
  ///////////////////////////////////////////////////////////////////////////////////////////
  VOID SetFilePersistedStatus(BOOL f_bstatus);

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

 private:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// ReadImmediateNotificationData
  /// Get the list of all immediate persistent notifications of f_epersistcategory
  /// from a persistent memory in a vector.
  ///
  /// \param [OUT] f_vpersistentnotificationlist
  ///        Persistent_Notification_List_Type - Retrieved list of all notifications and corresponding data
  ///
  /// \param [IN] f_epersistcategory
  ///        const EFrameworkunifiedPersistCategory - persistent category
  ///
  /// \return EFrameworkunifiedStatus
  ///         EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus ReadImmediateNotificationData(Persistent_Notification_List_Type *&f_vpersistentnotificationlist, // NOLINT (runtime/references)
                                           const EFrameworkunifiedPersistCategory f_epersistcategory);

  std::string m_cUserNotificationTag;  ///< Tag associated with a file which is used
  ///< for storing user related notification persistent data.

  std::string m_cNotificationPersistFilepath;  ///< Persistent file path for storing persistent data

  std::string m_cNotificationUserMemFilepath;  ///< Memory location of a file for storing persistent data of a user

  Persist_Type m_mPersist_Type;  ///< Map holds the objects of file and folder persistency.

  CPersistentAccesser *m_poDataAccesser;  ///< Object for persistent data accesser

  HANDLE m_hNSImmediatePersistenceThread;  ///< Nor persistence thread handle

  BOOL m_bAllFilePersisted;           // all files have been persisted

  BOOL m_bImmediatedDataPersisted;    // all immediate peristence data have been persisted

  EFrameworkunifiedStatus Init();
};

////////////////////////////////////////////////////////////////////////////////////////////
/// NPServiceOnCpWorkerAckCmd
/// Handles when the CopyWorker sends an ack back for a message received .
///
/// \param [in] f_happ
///         HANDLE - Handle to notificationpersistentservice_application Framework.
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPServiceOnCpWorkerAckCmd(HANDLE f_happ);

#endif  // NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_PERSISTENCE_MANAGER_H_
