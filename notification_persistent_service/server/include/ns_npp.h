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
/// \brief
///
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_H_
#define NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_H_

#include <native_service/frameworkunified_sm_framework_types.h>
#include <native_service/frameworkunified_sm_hsm.h>
#include <native_service/frameworkunified_types.h>
#include <string>


class CPersistenceManager;
class CNotificationManager;
class CnotificationpersistentservicePersonalizationManager;

/**
 *  This class act as a controller class which holds the instance of notification, personalization
 *  and persistence manager respectively.
 */
class CNSNPP: public CFrameworkunifiedHSM {
 public:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CNSNPP
  /// Constructor of CNSNPP class
  ///
  /// \param f_happ
  ///      PVOID - application handle
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  explicit CNSNPP(PVOID f_happ);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// ~CNSNPP
  /// Destructor of CNSNPP class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  ~CNSNPP();

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// Init
  /// This function is used to allocate dynamic memory for member variables of this class after
  /// creation of object.
  ///
  /// \param f_happ
  ///        HANDLE - Application handle
  ///
  /// \return EFrameworkunifiedStatus
  ///         EFrameworkunifiedStatus - Returns status of operation
  ///
  ///////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus Init(HANDLE f_happ);

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// DeInit
  /// This function is used to deinitialize the NPP object.
  ///
  /// \param f_happ
  ///        HANDLE - Application handle
  ///
  /// \return VOID
  ///         VOID - Returns status of operation
  ///
  ///////////////////////////////////////////////////////////////////////////////////////////
  VOID DeInit(HANDLE f_happ);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedCreate
  /// This function is pure virtual function of parent class.
  /// In this function, states and events of StateMachine are created and connected.
  ///
  /// \param [in] f_peventdata
  ///     PVOID - Event data
  ///
  /// \return EFrameworkunifiedStatus
  ///         EFrameworkunifiedStatus - Returns status of operation
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus FrameworkunifiedCreate(PVOID f_peventdata = NULL);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// NotificationpersistentservicePersistAll
  /// Persist all persistent state, user persistent state notification data;
  /// and all global and user related files to permenant storage
  ///
  /// \param [in] f_eshutdowntype
  ///         EFrameworkunifiedShutdownType - shutdown type i.e. normal shutdown, quick, data reset
  ///
  /// \param [in] f_uinotificationpersistentservicepersistcategoryflag
  ///     UI_32 - Hex value from enum EFrameworkunifiedPersistCategory, representing data to persist
  ///     0 - persist orignal data and
  ///     1 - persist default data
  ///
  /// \return EFrameworkunifiedStatus
  ///         EFrameworkunifiedStatus - Returns status of operation
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus NotificationpersistentservicePersistAll(EFrameworkunifiedShutdownType f_eshutdowntype, UI_32 f_uinotificationpersistentservicepersistcategoryflag);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// NotificationpersistentserviceSaveNotificationPersistentData
  /// In this function, all the data related to persistent notification is retrieved from
  /// notification manager and stored in persistent file by persistent manager.
  ///
  /// \param [in] f_epersistenttype
  ///     EFrameworkunifiedPersistentVarType - Type of persistent notification
  /// \param [in] f_uinotificationpersistentservicepersistcategoryflag
  ///     UI_32 - Hex value from enum EFrameworkunifiedPersistCategory, representing data to persist
  ///     0 - persist orignal data and
  ///     1 - persist default data
  ///
  /// \return EFrameworkunifiedStatus
  ///         EFrameworkunifiedStatus - Returns status of operation
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus NotificationpersistentserviceSaveNotificationPersistentData(EFrameworkunifiedNotificationType f_epersistenttype,
                                               UI_32 f_uinotificationpersistentservicepersistcategoryflag = 0x0);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// NotificationpersistentserviceLoadPersistentNotificationData
  /// In this transition, persistent file is loaded by persistent manager and
  /// persistent notifications and its data is loaded to notification manager.
  ///
  /// \param [in] f_epersistenttype
  ///     EFrameworkunifiedPersistentVarType - Type of persistent notification
  ///
  /// \return EFrameworkunifiedStatus
  ///         EFrameworkunifiedStatus - Returns status of operation
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus NotificationpersistentserviceLoadPersistentNotificationData(EFrameworkunifiedNotificationType f_epersistenttype);

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// DeletePersistedDataFolder
  /// This function does not deletes the data instead just rename the folder which needs to
  /// be deleted. And this data will gets deleted during next startup.
  ///
  /// \param f_uipersistcategoryflag
  ///        UI_32 - Rename the persisted data folder depending on enum EFrameworkunifiedPersistCategory value
  ///
  /// \return EFrameworkunifiedStatus
  ///         EFrameworkunifiedStatus - Returns status of operation
  ///
  ///////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus DeletePersistedDataFolder(UI_32 f_uipersistcategoryflag);

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// Syncfs
  /// Syncfs is carried out.
  ///
  /// \param
  ///
  /// \return EFrameworkunifiedStatus
  ///         EFrameworkunifiedStatus - Returns status of operation
  ///
  ///////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus  Syncfs();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// Singleton instance of NotificationManager
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CNotificationManager *m_pNotificationManager;           ///< Instance of Notification Manager

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// Singleton instance of PersonalizationManager
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CnotificationpersistentservicePersonalizationManager *m_pPersonalizationManager;  ///< Instance of Personalization Manager

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// Singleton instance of PersistenceManager
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CPersistenceManager *m_pPersistenceManager;
  // scheduling priority of read thread
  static SI_32 m_siReadThreadPrio;  // NOLINT (readability/naming)
  // scheduling priority of write thread
  static SI_32 m_siWriteThreadPrio;  // NOLINT (readability/naming)
  // scheduling priority of immediate persistence thread
  static SI_32 m_siImmediatePersistenceThreadPrio;  // NOLINT (readability/naming)
  // number of times CRC check is required. (CRC check will be done more than once only on failure)
  static UI_16
  m_siCRCCheckCount;  // NOLINT (readability/naming)

 private:
  ///< name of reader thread
  const std::string m_cReadThreadName;  // NOLINT (readability/naming)
  ///< name of writer thread
  const std::string m_cWriteThreadName;  // NOLINT (readability/naming)
  ///< name of immediate persistence thread
  const std::string m_cImmediatePersistenceThreadName;  // NOLINT (readability/naming)

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// CreateAndStartChildThreads
  /// This function creates and start all child threads.
  ///
  /// \param f_happ
  ///      HANDLE - Application handle
  ///
  /// \return EFrameworkunifiedStatus
  ///         EFrameworkunifiedStatus - Returns status of operation
  ///
  ///////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus CreateAndStartChildThreads(HANDLE f_happ);

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// SwitchToFileStructureVersion1
  /// Copy all the data from old persistent path to new persistent path
  ///
  /// \param [IN] f_uinppversion
  ///        UI_32 - New version of NPPService
  ///
  /// \return EFrameworkunifiedStatus
  ///         EFrameworkunifiedStatus - Returns status of operation
  ///
  ///////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus SwitchToFileStructureVersion1(const UI_32 f_uinppversion);

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// SwitchToFileStructureVersion2
  /// Move immediate notification data files from IMMEDIATE_PERSISTENCE_STORAGE_V1 to
  /// IMMEDIATE_PERSISTENCE_STORAGE_V2/AllUserAppData/UserData
  ///
  /// \param [IN] f_uinppversion
  ///        UI_32 - New version of NPPService
  ///
  /// \return EFrameworkunifiedStatus
  ///         EFrameworkunifiedStatus - Returns status of operation
  ///
  ///////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus SwitchToFileStructureVersion2(const UI_32 f_uinppversion);

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// GetNSNPPVersion
  /// Get the version of NSNPP from version.txt
  ///
  /// \param
  ///
  /// \return UI_8
  ///         UI_8 - version number
  ///
  ///////////////////////////////////////////////////////////////////////////////////////////
  UI_32 GetNSNPPVersion();

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// SetNSNPPVersion
  /// Set the version of NSNPP in version.txt
  ///
  /// \param [IN] f_uiversion
  ///      UI_32 - version number
  ///
  /// \return EFrameworkunifiedStatus
  ///         EFrameworkunifiedStatus - Returns status of operation
  ///
  ///////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus SetNSNPPVersion(UI_8 f_uiversion);

  HANDLE m_hNSReadThread;  ///< Reader thread handle

  HANDLE m_hNSWriteThread;  ///< Writer thread handle

  HANDLE m_hNSImmediatePersistenceThread;  ///< immediate persistence thread handle
};

#endif  // NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_H_
