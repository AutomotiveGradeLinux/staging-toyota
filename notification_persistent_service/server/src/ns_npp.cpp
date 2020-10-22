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
/// \ingroup  tag_NS_NPPService
/// \brief
///
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/timeb.h>
#include <sys/types.h>

#include <native_service/frameworkunified_sm_leafstate.h>
#include <native_service/frameworkunified_multithreading.h>
#include <native_service/ns_np_service_protocol.h>

#include <string>
#include <vector>

#include <native_service/ns_np_service_nor_persistence_internal.h>
#include "app_states.h"
#include "ns_npp.h"
#include "ns_npp_types.h"
#include "ns_npp_notificationpersistentservicelog.h"
#include "ns_npp_threads.h"
#include "ns_npp_copy_worker.h"
#include "ns_npp_fs_directory.h"
#include "ns_npp_persist_folder.h"
#include "ns_npp_persistent_data.h"
#include "ns_npp_persistence_manager.h"
#include "ns_npp_notification_manager.h"
#include "ns_npp_personalization_manager.h"
#include "ns_npp_nor_persistence_worker_thread.h"

#define NPP_VERSION_INVALID     255
#define NPP_VERSION_0           0
#define NPP_VERSION_1           1

#define NPP_CONNECT_DEFAULTSTATE(parent, child) \
    FrameworkunifiedConnect(l_p## parent, l_p## child, TRUE);

#define NPP_CONNECT_STATE(parent, child) \
    FrameworkunifiedConnect(l_p## parent, l_p## child);

#define NPP_CONNECT_DEFERREDEVENT(state, eventid, reaction) \
    FrameworkunifiedConnect(l_p## state, _## eventid, l_pTrn## reaction, #eventid, TRUE); \

#define NPP_CONNECT_EVENT(state, eventid, reaction) \
    FrameworkunifiedConnect(l_p## state, _## eventid, l_pTrn## reaction, #eventid);

#define NPP_CONNECTROOT(state) \
    FrameworkunifiedConnect(l_p## state);

#define NPP_CONNECT_ORTHOGONAL_REGION(orthogonalstate, orthogonalregion) \
    FrameworkunifiedConnectOrthogonal(l_p## orthogonalstate, l_p## orthogonalregion);

#define NPP_CREATE_STATE(class_name) \
    C## class_name *l_p## class_name = new (std::nothrow) C## class_name(#class_name);

#define NPP_PRINTSTATEMACHINE() \
    FrameworkunifiedPrintAllStates();

// typedef of vector of CNotificationsToPersist
typedef std::vector<CNotificationsToPersist *> Persistent_Notification_List_Type;

// iterator for CNotificationsToPersist vector
typedef Persistent_Notification_List_Type::iterator Persistent_Notification_List_Iterator;

// initialize static variables
SI_32 CNSNPP::m_siWriteThreadPrio = NS_NPP_WRITE_THREAD_PRIO;
SI_32 CNSNPP::m_siReadThreadPrio = NS_NPP_READ_THREAD_PRIO;
SI_32 CNSNPP::m_siImmediatePersistenceThreadPrio = NS_NPP_IMMEDIATE_PERSIST_THREAD_PRIO;
UI_16 CNSNPP::m_siCRCCheckCount = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// CNSNPP
/// Class Constructor
////////////////////////////////////////////////////////////////////////////////////////////////////
CNSNPP::CNSNPP(PVOID f_happ) : CFrameworkunifiedHSM(f_happ),
  m_cReadThreadName(NS_NPP_READ_THREAD_NAME),
  m_cWriteThreadName(NS_NPP_WRITE_THREAD_NAME),
  m_cImmediatePersistenceThreadName(NS_NPP_IMMEDIATE_PERSIST_THREAD_NAME) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  m_hNSReadThread = NULL;
  m_hNSWriteThread = NULL;
  m_hNSImmediatePersistenceThread = NULL;

  m_pNotificationManager = NULL;
  m_pPersonalizationManager = NULL;
  m_pPersistenceManager = NULL;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// CNSNPP
/// Class Destructor
////////////////////////////////////////////////////////////////////////////////////////////////////
CNSNPP::~CNSNPP() {  // LCOV_EXCL_START 200: cannot test code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (NULL != m_pNotificationManager) {
    delete m_pNotificationManager;
    m_pNotificationManager = NULL;
  }

  if (NULL != m_pPersonalizationManager) {
    delete m_pPersonalizationManager;
    m_pPersonalizationManager = NULL;
  }
  if (NULL != m_pPersistenceManager) {
    delete m_pPersistenceManager;
    m_pPersistenceManager = NULL;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}
// LCOV_EXCL_STOP

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Init
/// This function is used to allocate dynamic memory for member variables of this class after
/// creation of object.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNSNPP::Init(HANDLE f_happ) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  FrameworkunifiedProtocolEvent ns_npp_internal_protocol_handlers[] = {
    {CP_WRK_NTFY, EVENT(evNPReleaseLoadAck)}
  };

  FrameworkunifiedProtocolEvent immediate_thread_protocol_handlers[] = {
    {NOR_PERSISTENCE_ONSHUTDOWN_ACK, EVENT(evNPNorShutdownAck)},
  };

  UI_32 l_uiNPPVersion = NPP_VERSION_INVALID;

  // initialize Notification Manager
  m_pNotificationManager = new(std::nothrow) CNotificationManager();
  // initialize Personalization Manager
  m_pPersonalizationManager =  new(std::nothrow) CnotificationpersistentservicePersonalizationManager();
  // initialize Persistence Manager
  m_pPersistenceManager = new(std::nothrow) CPersistenceManager();

  if ((NULL == f_happ) || (NULL == m_pNotificationManager) || (NULL == m_pPersonalizationManager) || (NULL == m_pPersistenceManager)) {  // LCOV_EXCL_BR_LINE 5: f_happ, m_pNotificationManager, m_pPersonalizationManager, m_pPersistenceManager can't be NULL  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 5: f_happ, m_pNotificationManager, m_pPersonalizationManager, m_pPersistenceManager can't be NULL
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "f_happ or m_pNotificationManager or m_pPersonalizationManager or  m_pPersistenceManager is NULL");

    l_estatus = eFrameworkunifiedStatusNullPointer;
    // LCOV_EXCL_STOP
  } else {
    if ((eFrameworkunifiedStatusOK != FrameworkunifiedAttachCallbackToDispatcher(  // LCOV_EXCL_BR_LINE 4: NSFW error case
             f_happ, m_cReadThreadName.c_str(),
             CP_WRK_ACK_CMD_COMPLETE,
             NPServiceOnCpWorkerAckCmd))
        || (eFrameworkunifiedStatusOK != FrameworkunifiedAttachCallbackToDispatcher(
                f_happ, m_cWriteThreadName.c_str(),
                CP_WRK_ACK_CMD_COMPLETE,
                NPServiceOnCpWorkerAckCmd))
        || (eFrameworkunifiedStatusOK != FrameworkunifiedAttachHSMEventsToDispatcher(
                f_happ, m_cWriteThreadName.c_str(),
                ns_npp_internal_protocol_handlers,
                static_cast<UI_32>(_countof(ns_npp_internal_protocol_handlers))))
        || (eFrameworkunifiedStatusOK != FrameworkunifiedAttachHSMEventsToDispatcher(
                f_happ, m_cReadThreadName.c_str(),
                ns_npp_internal_protocol_handlers,
                static_cast<UI_32>(_countof(ns_npp_internal_protocol_handlers))))
        || (eFrameworkunifiedStatusOK != FrameworkunifiedAttachHSMEventsToDispatcher(
                f_happ, m_cImmediatePersistenceThreadName.c_str(),
                immediate_thread_protocol_handlers,
                static_cast<UI_32>(_countof(immediate_thread_protocol_handlers))))) {
      // LCOV_EXCL_START 4: NSFW error case.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Attaching callbacks and events failed for protocols CP_WRK_ACK_CMD_COMPLETE, CP_WRK_NTFY");
      l_estatus = eFrameworkunifiedStatusFail;
      // LCOV_EXCL_STOP
    } else {
      l_estatus = CreateAndStartChildThreads(f_happ);

      if (eFrameworkunifiedStatusOK == l_estatus) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
        if (NPP_VERSION_INVALID != (l_uiNPPVersion = GetNSNPPVersion())) {  // LCOV_EXCL_BR_LINE 6: l_uiNPPVersion can't be NPP_VERSION_INVALID  // NOLINT[whitespace/line_length]
          if (NPP_VERSION_0 == l_uiNPPVersion) {
            l_estatus = SwitchToFileStructureVersion1(l_uiNPPVersion + 1);

            l_estatus = SwitchToFileStructureVersion2(l_uiNPPVersion + 2);
          } else if (NPP_VERSION_1 == l_uiNPPVersion) {  // LCOV_EXCL_BR_LINE 200: cannot test code in init sequence
            // LCOV_EXCL_START 200: cannot test code in init sequence
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            l_estatus = SwitchToFileStructureVersion2(l_uiNPPVersion + 1);

            // send the message to write worker thread to delete the persistent
            /// which was requested for deletion during previous shutdown
            l_estatus = FrameworkunifiedSendChild(f_happ, m_hNSWriteThread, CMD_DELETE_OLD_DATA, 0, NULL);
            // LCOV_EXCL_STOP
          } else {
            // send the message to write worker thread to delete the persistent
            /// which was requested for deletion during previous shutdown
            l_estatus = FrameworkunifiedSendChild(f_happ, m_hNSWriteThread, CMD_DELETE_OLD_DATA, 0, NULL);
          }

          // Try to load persisted notification data
          NotificationpersistentserviceLoadPersistentNotificationData(eFrameworkunifiedPersistedStateVar);

          // load immediate persistence data as the respective thread is now started.
          NotificationpersistentserviceLoadPersistentNotificationData(eFrameworkunifiedImmediatePersistedStateVar);
        } else {
          // LCOV_EXCL_START 6: l_uiNPPVersion can't be NPP_VERSION_INVALID
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          l_estatus = eFrameworkunifiedStatusFail;
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Invalid version of NPPService");
          // LCOV_EXCL_STOP
        }
      } else {
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error in creating child threads of NPPService");  // LCOV_EXCL_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
      }
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// DeInit
/// This function is used to deinitialize the NPP object
////////////////////////////////////////////////////////////////////////////////////////////////////
VOID CNSNPP::DeInit(HANDLE f_happ) {  // LCOV_EXCL_START 200: cannot test code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (NULL != m_pNotificationManager) {
    delete m_pNotificationManager;
    m_pNotificationManager = NULL;
  }

  if (NULL != m_pPersonalizationManager) {
    delete m_pPersonalizationManager;
    m_pPersonalizationManager = NULL;
  }

  if (NULL != m_pPersistenceManager) {
    delete m_pPersistenceManager;
    m_pPersistenceManager = NULL;
  }

  if (NULL != f_happ) {
    EFrameworkunifiedStatus eStatus;

    if (NULL != m_hNSReadThread) {
      if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedStopChildThread(f_happ, m_hNSReadThread, 0, NULL))) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedStopChildThread Read Thread Failed. status=%d", eStatus);
      }
      if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedDestroyChildThread(f_happ, m_hNSReadThread))) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedDestroyChildThread Read Thread Failed. status=%d", eStatus);
      }
      m_hNSReadThread = NULL;
    }

    if (NULL != m_hNSWriteThread) {
      if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedStopChildThread(f_happ, m_hNSWriteThread, 0, NULL))) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedStopChildThread Write Thread Failed. status=%d", eStatus);
      }
      if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedDestroyChildThread(f_happ, m_hNSWriteThread))) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedDestroyChildThread Write Thread Failed. status=%d", eStatus);
      }
      m_hNSWriteThread = NULL;
    }

    if (NULL != m_hNSImmediatePersistenceThread) {
      if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedStopChildThread(f_happ, m_hNSImmediatePersistenceThread, 0, NULL))) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedStopChildThread ImmidatePersistence Thread Failed. status=%d", eStatus);
      }
      if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedDestroyChildThread(f_happ, m_hNSImmediatePersistenceThread))) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedDestroyChildThread ImmidatePersistence Thread Failed. status=%d", eStatus);
      }
      m_hNSImmediatePersistenceThread = NULL;
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}
// LCOV_EXCL_STOP

////////////////////////////////////////////////////////////////////////////////////////////////
/// SwitchToFileStructureVersion1
/// Copy all the user data from old persistent path to new persistent path i.e. UserData folder
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNSNPP::SwitchToFileStructureVersion1(const UI_32 f_uinppversion) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  std::string l_cSrcPath = "";
  std::string l_cDestPath = "";

  std::string l_cNSNPSPath = CPersistence::GetStoragePath();
  std::string l_cUserDataPath = "";

  if (CFSDirectory::DoesDirecotryExist(l_cNSNPSPath)) {  // LCOV_EXCL_BR_LINE 6: l_cNSNPSPath must exist
    DIR *l_pdir = NULL;

    struct dirent *l_pdirent = NULL;

    l_pdir = opendir(l_cNSNPSPath.c_str());

    if (NULL != l_pdir) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
      if (l_cNSNPSPath[l_cNSNPSPath.length() - 1] != '/') {  // LCOV_EXCL_BR_LINE 6: l_cNSNPSPath must has '/'
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        l_cNSNPSPath.append("/");  // LCOV_EXCL_LINE 6: l_cNSNPSPath must has '/'
      }

      // set the userdata folder path
      l_cUserDataPath.assign(l_cNSNPSPath);
      l_cUserDataPath.append(USERDATADIR);
      l_cUserDataPath.append(ALLUSERAPPDATADIR);

      // create the userdata directory
      if (!CFSDirectory::DoesDirecotryExist(l_cUserDataPath)) {
        l_estatus = CFSDirectory::CreateDirectory(l_cUserDataPath);
      }

      // if directory successfully created or is already present on target
      if (eFrameworkunifiedStatusOK == l_estatus) {  // LCOV_EXCL_BR_LINE 6: l_estatus must be eFrameworkunifiedStatusOK
        while (NULL != (l_pdirent = readdir(l_pdir))) {
          if (0 != std::strcmp(l_pdirent->d_name, ".") &&
              0 != std::strcmp(l_pdirent->d_name, "..") &&
              0 != std::strcmp(l_pdirent->d_name, AppName) &&
              0 != std::strcmp(l_pdirent->d_name, USERDATA)) {
            l_cSrcPath.assign(l_cNSNPSPath.c_str());
            l_cSrcPath.append(l_pdirent->d_name);

            if (CFSDirectory::IsDirectory(l_cSrcPath)) {  // LCOV_EXCL_BR_LINE 6: cannot test in init sequence
              l_cDestPath.assign(l_cUserDataPath);
              l_cDestPath.append(l_pdirent->d_name);

              // move the old app user data to the UserData fodler
              if (0 != rename(l_cSrcPath.c_str(), l_cDestPath.c_str())) {  // LCOV_EXCL_BR_LINE 6: rename must return ok  // NOLINT[whitespace/line_length]
                // LCOV_EXCL_START 6: rename must return ok
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error renaming folder %s to %s",
                       l_cSrcPath.c_str(), l_cDestPath.c_str());
                // LCOV_EXCL_STOP
              }
            }
          }
        }
      } else {
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Unable to create directory:: %s", l_cDestPath.c_str());  // LCOV_EXCL_LINE 6: l_estatus must be eFrameworkunifiedStatusOK  // NOLINT[whitespace/line_length]
      }

      closedir(l_pdir);  // close the directory
    } else {
      // LCOV_EXCL_START 4: NSFW error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_estatus = eFrameworkunifiedStatusNullPointer;
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "l_pdir is NULL");
      // LCOV_EXCL_STOP
    }
  }

  l_estatus = SetNSNPPVersion(static_cast<UI_8>(f_uinppversion));

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// SwitchToFileStructureVersion2
/// Move immediate notification data files from IMMEDIATE_PERSISTENCE_STORAGE_V1 to
/// IMMEDIATE_PERSISTENCE_STORAGE_V2/AllUserAppData/UserData
//// ////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNSNPP::SwitchToFileStructureVersion2(const UI_32 f_uinppversion) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // immediate notification persistent path for NPPService version 1
  std::string l_cSrcDir = IMMEDIATE_PERSISTENCE_STORAGE_V1;

  // immediate notification persistent path for NPPService version 2
  std::string l_cDestDir = IMMEDIATE_PERSISTENCE_STORAGE_V2;
  l_cDestDir.append(USERDATADIR);
  l_cDestDir.append(ALLUSERAPPDATADIR);
  l_cDestDir.append(IMMEDIATEDATADIR);

  if (eFrameworkunifiedStatusOK == (l_estatus = SynchronousMovePersistentData(l_cSrcDir.c_str(), l_cDestDir.c_str()))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
    if (!CFSDirectory::RemoveDirectory(l_cSrcDir)) {  // LCOV_EXCL_BR_LINE 6: cannot test in init sequence
      FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Error deleting old immediate pers. dir %s", l_cSrcDir.c_str());
    }

    l_estatus = SetNSNPPVersion(static_cast<UI_8>(f_uinppversion));
  } else {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "SynchronousMovePersistentData faiure, status=%d", l_estatus);  // LCOV_EXCL_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// GetNSNPPVersion
/// Get the version of NSNPP from version.txt
///////////////////////////////////////////////////////////////////////////////////////////
UI_32 CNSNPP::GetNSNPPVersion() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  UI_32 l_uiVersion = NPP_VERSION_INVALID;
  CHAR l_cVerStr[3] = {};
  FILE *l_pFile = NULL;

  std::string l_cVersion = CPersistence::GetStoragePath();
  if (l_cVersion[l_cVersion.length() - 1] != '/') {  // LCOV_EXCL_BR_LINE 6: must has '/'
    l_cVersion += "/";
  }
  l_cVersion.append(VERSION_TXT);

  if (NULL != (l_pFile = fopen(l_cVersion.c_str(), "r"))) {
    // read the version from file
    if (NULL == fgets(l_cVerStr, 2, l_pFile)) {  // LCOV_EXCL_BR_LINE 6: can't return NULL
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "File empty: %s", l_cVersion.c_str());  // LCOV_EXCL_LINE 6: can't return NULL
    } else {
      errno = EOK;
      l_uiVersion = static_cast<UI_32>(strtol(l_cVerStr, NULL, 10));

      if (EOK != errno) {  // LCOV_EXCL_BR_LINE 6: errno always is EOK
        // LCOV_EXCL_START 6: errno always is EOK
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error converting string %s to hex, errno=%d", l_cVerStr, errno);
        l_uiVersion = NPP_VERSION_0;
        // LCOV_EXCL_STOP
      }
    }

    fclose(l_pFile);
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Unable to open file: %s, errno=%d", l_cVersion.c_str(), errno);
    l_uiVersion = NPP_VERSION_0;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_uiVersion;
}
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
EFrameworkunifiedStatus  CNSNPP::Syncfs() {
  int dfd;

  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  std::string l_cPath = CPersistence::GetStoragePath();

  // create the file
  dfd = open(l_cPath.c_str(), O_RDONLY | O_DIRECTORY);
  if (-1 != dfd) {
    // syncfs()
    if (syncfs(dfd) < 0) {  // LCOV_EXCL_BR_LINE 5: syncfs's error case.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Unable to syncfs: %s errno=%d", l_cPath.c_str(), errno);  // LCOV_EXCL_LINE 5: syncfs's error case.  // NOLINT[whitespace/line_length]
    }
    close(dfd);
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Unable to open file: %s serrno=%d", l_cPath.c_str(), errno);
    l_estatus = eFrameworkunifiedStatusNullPointer;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}
///////////////////////////////////////////////////////////////////////////////////////////
/// SetNSNPPVersion
/// Set the version of NSNPP in version.txt
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNSNPP::SetNSNPPVersion(UI_8 f_uiversion) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  FILE *l_pFile = NULL;

  std::string l_cVersion = CPersistence::GetStoragePath();
  if (l_cVersion[l_cVersion.length() - 1] != '/') {  // LCOV_EXCL_BR_LINE 6: must has '/'
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_cVersion += "/";  // LCOV_EXCL_LINE 6: must has '/'
  }
  if (!CFSDirectory::DoesDirecotryExist(l_cVersion)) {
    CFSDirectory::CreateDirectory(l_cVersion);
  }

  l_cVersion.append(VERSION_TXT);

  // create the file and write
  l_pFile = fopen(l_cVersion.c_str(), "w");
  if (NULL != l_pFile) {  // LCOV_EXCL_BR_LINE 6: l_pFile can't be NULL
    // write the version to file
    fprintf(l_pFile, "%d", f_uiversion);
    fclose(l_pFile);
  } else {
    // LCOV_EXCL_START 6: l_pFile can't be NULL
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Unable to open file: %s", l_cVersion.c_str());
    l_estatus = eFrameworkunifiedStatusNullPointer;
    // LCOV_EXCL_STOP
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedCreate
/// In this function, states and events of StateMachine are created and connected.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNSNPP::FrameworkunifiedCreate(PVOID f_peventdata) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // ========Create states
  NPP_CREATE_STATE(sNotificationpersistentserviceNPPRoot)
  NPP_CREATE_STATE(sNotificationpersistentserviceNPPStart)
  NPP_CREATE_STATE(sNotificationpersistentserviceNPPError)

  CREATE_ORTHOGONALREGION(sNotificationpersistentservicePersistence)
  CREATE_ORTHOGONALREGION(sNotificationpersistentserviceShutdown)
  CREATE_ORTHOGONALREGION(sNotificationpersistentserviceNotification)

  NPP_CREATE_STATE(sNotificationpersistentservicePersistenceReady)
  NPP_CREATE_STATE(sNotificationpersistentservicePersistenceError)

  NPP_CREATE_STATE(sNotificationpersistentserviceNotificationReady)
  NPP_CREATE_STATE(sNotificationpersistentserviceNotificationError)

  NPP_CREATE_STATE(sNotificationpersistentserviceShutdownIdle)
  NPP_CREATE_STATE(sNotificationpersistentserviceShutdownDataSave)

  // ========Connect states
  NPP_CONNECT_DEFAULTSTATE(sNotificationpersistentserviceNPPRoot, sNotificationpersistentserviceNPPStart)
  NPP_CONNECT_STATE(sNotificationpersistentserviceNPPRoot, sNotificationpersistentserviceNPPStart)
  NPP_CONNECT_STATE(sNotificationpersistentserviceNPPRoot, sNotificationpersistentserviceNPPError)

  NPP_CONNECT_ORTHOGONAL_REGION(sNotificationpersistentserviceNPPStart, sNotificationpersistentservicePersistence)
  NPP_CONNECT_ORTHOGONAL_REGION(sNotificationpersistentserviceNPPStart, sNotificationpersistentserviceShutdown)
  NPP_CONNECT_ORTHOGONAL_REGION(sNotificationpersistentserviceNPPStart, sNotificationpersistentserviceNotification)

  NPP_CONNECT_DEFAULTSTATE(sNotificationpersistentservicePersistence, sNotificationpersistentservicePersistenceReady)
  NPP_CONNECT_STATE(sNotificationpersistentservicePersistence, sNotificationpersistentservicePersistenceError)

  NPP_CONNECT_DEFAULTSTATE(sNotificationpersistentserviceShutdown, sNotificationpersistentserviceShutdownIdle)
  NPP_CONNECT_STATE(sNotificationpersistentserviceShutdown, sNotificationpersistentserviceShutdownDataSave)

  NPP_CONNECT_DEFAULTSTATE(sNotificationpersistentserviceNotification, sNotificationpersistentserviceNotificationReady)
  NPP_CONNECT_STATE(sNotificationpersistentserviceNotification, sNotificationpersistentserviceNotificationError)

  // ========Create external transitions
  CREATE_EXTERNALTRANSITION(sNotificationpersistentserviceNPPStart)
  CREATE_EXTERNALTRANSITION(sNotificationpersistentserviceNPPError)
  CREATE_EXTERNALTRANSITION(sNotificationpersistentservicePersistenceReady)
  CREATE_EXTERNALTRANSITION(sNotificationpersistentservicePersistenceError)
  CREATE_EXTERNALTRANSITION(sNotificationpersistentserviceShutdownIdle)
  CREATE_EXTERNALTRANSITION(sNotificationpersistentserviceShutdownDataSave)
  CREATE_EXTERNALTRANSITION(sNotificationpersistentserviceNotificationReady)
  CREATE_EXTERNALTRANSITION(sNotificationpersistentserviceNotificationError)

  // ========Create internal transitions
  CREATE_INTERNALTRANSITION(NotificationpersistentserviceOnNPReadPersistedData)
  CREATE_INTERNALTRANSITION(NotificationpersistentserviceOnNPReleaseFileAck)
  CREATE_INTERNALTRANSITION(NotificationpersistentserviceOnUserChange)
  CREATE_INTERNALTRANSITION(NotificationpersistentserviceOnNPNorDataSaveAck)
  CREATE_INTERNALTRANSITION(NotificationpersistentserviceOnRegisterPersistentFile)
  CREATE_INTERNALTRANSITION(NotificationpersistentserviceOnReleasePersistentFile)
  CREATE_INTERNALTRANSITION(NotificationpersistentserviceOnLoadPersistentFile)
  CREATE_INTERNALTRANSITION(NotificationpersistentserviceOnRegisterPersistentFolder)
  CREATE_INTERNALTRANSITION(NotificationpersistentserviceOnLoadPersistentFolder)
  CREATE_INTERNALTRANSITION(NotificationpersistentserviceOnReleasePersistentFolder)
  CREATE_INTERNALTRANSITION(NotificationpersistentserviceOnNPSetFilePersistentType)
  CREATE_INTERNALTRANSITION(NotificationpersistentserviceOnNPSetFolderPersistentType)
  CREATE_INTERNALTRANSITION(NotificationpersistentserviceOnNPPersistentSync)

  CREATE_INTERNALTRANSITION(NotificationpersistentserviceOnNPShutdown)
  CREATE_INTERNALTRANSITION(NotificationpersistentserviceOnShutdownTimeout)
  CREATE_INTERNALTRANSITION(NotificationpersistentserviceCheckAllReleaseRequestsProcessed)

  CREATE_INTERNALTRANSITION(NotificationpersistentserviceOnNPPublishNotification)
  CREATE_INTERNALTRANSITION(NotificationpersistentserviceOnNPRegisterNotifications)
  CREATE_INTERNALTRANSITION(NotificationpersistentserviceOnNPSubscribeToNotification)
  CREATE_INTERNALTRANSITION(NotificationpersistentserviceOnNPSubscribeToNotifications)
  CREATE_INTERNALTRANSITION(NotificationpersistentserviceOnNPUnRegisterNotifications)
  CREATE_INTERNALTRANSITION(NotificationpersistentserviceOnNPUnSubscribeFromNotification)
  CREATE_INTERNALTRANSITION(NotificationpersistentserviceOnNPUnSubscribeFromNotifications)
  CREATE_INTERNALTRANSITION(NotificationpersistentserviceOnNPGetReadyStatus)
  CREATE_INTERNALTRANSITION(NotificationpersistentserviceOnNPRegisterImmediateNotifications)
  CREATE_INTERNALTRANSITION(NotificationpersistentserviceOnNPClearPersistedData)
  CREATE_INTERNALTRANSITION(NotificationpersistentserviceOnNPSetDefaultPersistentData)
  CREATE_INTERNALTRANSITION(NotificationpersistentserviceOnNPSetNotfnPersistentType)
  CREATE_INTERNALTRANSITION(NotificationpersistentserviceOnNPPublishImmediateNotification)

#ifdef NPP_PROFILEINFO_ENABLE

  CREATE_INTERNALTRANSITION(NotificationpersistentserviceOnNPProfileNotifications)
  CREATE_INTERNALTRANSITION(NotificationpersistentserviceOnNPProfilePersistence)

  NPP_CONNECT_EVENT(sNotificationpersistentserviceNotificationReady, evNPProfileNotification, NotificationpersistentserviceOnNPProfileNotifications)
  NPP_CONNECT_EVENT(sNotificationpersistentservicePersistenceReady, evNPProfilePersistence, NotificationpersistentserviceOnNPProfilePersistence)

#endif

  // ========Connect events
  NPP_CONNECT_EVENT(sNotificationpersistentserviceNPPRoot, evNPClearPersistedData, NotificationpersistentserviceOnNPClearPersistedData)

  NPP_CONNECT_EVENT(sNotificationpersistentserviceNPPStart, evError, sNotificationpersistentserviceNPPError)
  NPP_CONNECT_EVENT(sNotificationpersistentserviceNPPError, evReady, sNotificationpersistentserviceNPPStart)

  NPP_CONNECT_EVENT(sNotificationpersistentservicePersistenceReady, evError, sNotificationpersistentservicePersistenceError)
  NPP_CONNECT_EVENT(sNotificationpersistentservicePersistenceError, evReady, sNotificationpersistentservicePersistenceReady)
  NPP_CONNECT_EVENT(sNotificationpersistentservicePersistenceReady, evNPReadPersistedData, NotificationpersistentserviceOnNPReadPersistedData)
  NPP_CONNECT_EVENT(sNotificationpersistentservicePersistenceReady, evNPReleaseLoadAck, NotificationpersistentserviceOnNPReleaseFileAck)
  NPP_CONNECT_EVENT(sNotificationpersistentservicePersistenceReady, evNPRegisterPersistentFile, NotificationpersistentserviceOnRegisterPersistentFile)
  NPP_CONNECT_EVENT(sNotificationpersistentservicePersistenceReady, evNPReleasePersistentFile, NotificationpersistentserviceOnReleasePersistentFile)
  NPP_CONNECT_EVENT(sNotificationpersistentservicePersistenceReady, evNPLoadPersistentFile, NotificationpersistentserviceOnLoadPersistentFile)
  NPP_CONNECT_EVENT(sNotificationpersistentservicePersistenceReady, evNPRegisterPersistentFolder, NotificationpersistentserviceOnRegisterPersistentFolder)
  NPP_CONNECT_EVENT(sNotificationpersistentservicePersistenceReady, evNPLoadPersistentFolder, NotificationpersistentserviceOnLoadPersistentFolder)
  NPP_CONNECT_EVENT(sNotificationpersistentservicePersistenceReady, evNPReleasePersistentFolder, NotificationpersistentserviceOnReleasePersistentFolder)
  NPP_CONNECT_EVENT(sNotificationpersistentservicePersistenceReady, evNPGetReadyStatus, NotificationpersistentserviceOnNPGetReadyStatus)
  NPP_CONNECT_EVENT(sNotificationpersistentservicePersistenceReady, evNPSetFilePersistentType, NotificationpersistentserviceOnNPSetFilePersistentType)
  NPP_CONNECT_EVENT(sNotificationpersistentservicePersistenceReady, evNPSetFolderPersistentType, NotificationpersistentserviceOnNPSetFolderPersistentType)
  NPP_CONNECT_EVENT(sNotificationpersistentservicePersistenceReady, evNPPersistentSync, NotificationpersistentserviceOnNPPersistentSync)

  NPP_CONNECT_EVENT(sNotificationpersistentservicePersistenceReady, evUserChange, NotificationpersistentserviceOnUserChange)

  NPP_CONNECT_EVENT(sNotificationpersistentserviceShutdownIdle, evShutdown, NotificationpersistentserviceOnNPShutdown)
  NPP_CONNECT_EVENT(sNotificationpersistentserviceShutdownIdle, evNPShutdownDataSave, sNotificationpersistentserviceShutdownDataSave)
  NPP_CONNECT_EVENT(sNotificationpersistentserviceShutdownDataSave, evIdle, sNotificationpersistentserviceShutdownIdle)
  NPP_CONNECT_EVENT(sNotificationpersistentserviceShutdownDataSave, evShutdownTimeout, NotificationpersistentserviceOnShutdownTimeout)
  NPP_CONNECT_EVENT(sNotificationpersistentserviceShutdownDataSave, evCheckAllFilesPersisted, NotificationpersistentserviceCheckAllReleaseRequestsProcessed)
  NPP_CONNECT_EVENT(sNotificationpersistentserviceShutdownDataSave, evNPNorShutdownAck, NotificationpersistentserviceOnNPNorDataSaveAck)

  NPP_CONNECT_EVENT(sNotificationpersistentserviceNotificationReady, evError, sNotificationpersistentserviceNotificationError)
  NPP_CONNECT_EVENT(sNotificationpersistentserviceNotificationError, evReady, sNotificationpersistentserviceNotificationReady)
  NPP_CONNECT_EVENT(sNotificationpersistentserviceNotificationReady, evNPPublishNotification, NotificationpersistentserviceOnNPPublishNotification)
  NPP_CONNECT_EVENT(sNotificationpersistentserviceNotificationReady, evNPRegisterNotifications, NotificationpersistentserviceOnNPRegisterNotifications)
  NPP_CONNECT_EVENT(sNotificationpersistentserviceNotificationReady, evNPSubscribeToNotification, NotificationpersistentserviceOnNPSubscribeToNotification)
  NPP_CONNECT_EVENT(sNotificationpersistentserviceNotificationReady, evNPSubscribeToNotifications, NotificationpersistentserviceOnNPSubscribeToNotifications)
  NPP_CONNECT_EVENT(sNotificationpersistentserviceNotificationReady, evNPUnRegisterNotifications, NotificationpersistentserviceOnNPUnRegisterNotifications)
  NPP_CONNECT_EVENT(sNotificationpersistentserviceNotificationReady, evNPUnSubscribeFromNotification, NotificationpersistentserviceOnNPUnSubscribeFromNotification)
  NPP_CONNECT_EVENT(sNotificationpersistentserviceNotificationReady, evNPUnSubscribeFromNotifications, NotificationpersistentserviceOnNPUnSubscribeFromNotifications)
  NPP_CONNECT_EVENT(sNotificationpersistentserviceNotificationReady, evNPRegisterImmediateNotifications, NotificationpersistentserviceOnNPRegisterImmediateNotifications)
  NPP_CONNECT_EVENT(sNotificationpersistentserviceNotificationReady, evNPSetDefaultPersistentData, NotificationpersistentserviceOnNPSetDefaultPersistentData)
  NPP_CONNECT_EVENT(sNotificationpersistentserviceNotificationReady, evNPSetNotfnPersistentType, NotificationpersistentserviceOnNPSetNotfnPersistentType)
  NPP_CONNECT_EVENT(sNotificationpersistentserviceNotificationReady, evNPPublishImmediateNotification, NotificationpersistentserviceOnNPPublishImmediateNotification)

  NPP_CONNECTROOT(sNotificationpersistentserviceNPPRoot)

  NPP_PRINTSTATEMACHINE() // NOLINT (whitespace/empty_if_body)

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentservicePersistAll
///
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNSNPP::NotificationpersistentservicePersistAll(EFrameworkunifiedShutdownType f_eshutdowntype, UI_32 f_uinotificationpersistentservicepersistcategoryflag) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

  if (eFrameworkunifiedQuickShutdown == f_eshutdowntype) {
    if (NULL != m_pPersistenceManager) {   // LCOV_EXCL_BR_LINE 6: m_pPersistenceManager can't be NULL
      // set all file persisted status as TRUE
      m_pPersistenceManager->SetFilePersistedStatus(TRUE);

      // send request to immediate persistence thread to stop all the immediate notification timer
      m_pPersistenceManager->PersistNORData(f_eshutdowntype, f_uinotificationpersistentservicepersistcategoryflag);
    }
  } else {
    if (eFrameworkunifiedNormalShutdown == f_eshutdowntype) {
      f_uinotificationpersistentservicepersistcategoryflag = 0x0;
    }

    // save global notification data of persisted state notification
    if (eFrameworkunifiedStatusOK != NotificationpersistentserviceSaveNotificationPersistentData(eFrameworkunifiedPersistedStateVar, f_uinotificationpersistentservicepersistcategoryflag)) {  // LCOV_EXCL_BR_LINE 6: NotificationpersistentserviceSaveNotificationPersistentData will always return ok  // NOLINT[whitespace/line_length]
      // LCOV_EXCL_START 6: NotificationpersistentserviceSaveNotificationPersistentData will always return ok
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_estatus = eFrameworkunifiedStatusFail;
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error while saving notification persistent data.");
      // LCOV_EXCL_STOP
    }

    // save user notification data for the current user
    if (eFrameworkunifiedStatusOK != NotificationpersistentserviceSaveNotificationPersistentData(eFrameworkunifiedPersistedStateUserVar, f_uinotificationpersistentservicepersistcategoryflag)) {  // LCOV_EXCL_BR_LINE 6: NotificationpersistentserviceSaveNotificationPersistentData will always return ok  // NOLINT[whitespace/line_length]
      // LCOV_EXCL_START 6: NotificationpersistentserviceSaveNotificationPersistentData will always return ok
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_estatus = eFrameworkunifiedStatusFail;
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error while saving user notification persistent data.");
      // LCOV_EXCL_STOP
    }

    // NOR data is saved immediately,
    // but during shutdown it can be possible that because of delay in consecutive save to NOR
    // the latest data may not get persisted. So if we get shutdown we will need to persist the data immediately.
    if (NULL != m_pPersistenceManager) {  // LCOV_EXCL_BR_LINE 6: m_pPersistenceManager can't be NULL
      m_pPersistenceManager->PersistNORData(f_eshutdowntype, f_uinotificationpersistentservicepersistcategoryflag);
    }

    // delete all persistent files and folders for all applications
    if ((eFrameworkunifiedUserData | eFrameworkunifiedFactoryData | eFrameworkunifiedFactoryCustomerData | eFrameworkunifiedDealerData) == f_uinotificationpersistentservicepersistcategoryflag) {
      if (NULL != m_pPersistenceManager) {  // LCOV_EXCL_BR_LINE 6: m_pPersistenceManager can't be NULL
        m_pPersistenceManager->SetFilePersistedStatus(TRUE);
      } else {
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        l_estatus = eFrameworkunifiedStatusNullPointer;  // LCOV_EXCL_LINE 6: m_pPersistenceManager can't be NULL
      }
    } else {
      // writes the temporary global and user files to permanent memory
      if (m_pPersistenceManager) {  // LCOV_EXCL_BR_LINE 6: m_pPersistenceManager can't be NULL
        if (eFrameworkunifiedStatusOK == m_pPersistenceManager->PersistAllReleaseRequests(f_uinotificationpersistentservicepersistcategoryflag)) {  // LCOV_EXCL_BR_LINE 6: PersistAllReleaseRequests will always return ok  // NOLINT[whitespace/line_length]
          // this is needed to check in case when there are no file to persist
          if (TRUE == m_pPersistenceManager->HaveAllReleaseRequestsPersisted()) {  // LCOV_EXCL_BR_LINE 6: HaveAllReleaseRequestsPersisted will always return ok  // NOLINT[whitespace/line_length]
            // LCOV_EXCL_START 6: HaveAllReleaseRequestsPersisted will always return ok
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "All file release requests processed");
            m_pPersistenceManager->SetFilePersistedStatus(TRUE);
            // LCOV_EXCL_STOP
          }
        } else {
          // LCOV_EXCL_START 6: PersistAllReleaseRequests will always return ok
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          l_estatus = eFrameworkunifiedStatusFail;
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error while persisting temporary files.");
          // LCOV_EXCL_STOP
        }
      }
    }
  }

  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// DeletePersistedDataFolder
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNSNPP::DeletePersistedDataFolder(UI_32 f_uinotificationpersistentservicepersistcategoryflag) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

  struct timeb timebuf;
  struct tm   *theTime;

  ftime(&timebuf);
  theTime = gmtime(&timebuf.time); // NOLINT (runtime/threadsafe_fn)

  // get the current date and time to append to the folder name
  PCHAR l_cDateTime = new CHAR[MAX_STRING_SIZE_TAG];
  snprintf(l_cDateTime, MAX_STRING_SIZE_TAG, "_%02d%02d%02d_%02d%02d%02d", theTime->tm_mon + 1, theTime->tm_mday,
           theTime->tm_year + 1900, theTime->tm_hour, theTime->tm_min, theTime->tm_sec);

  std::string l_cBasePath = CPersistence::GetStoragePath();
  std::string l_cSrcPath = "";
  std::string l_cDestPath = "";

  if (eFrameworkunifiedUserData == (eFrameworkunifiedUserData & f_uinotificationpersistentservicepersistcategoryflag)) {
    l_cSrcPath.assign(l_cBasePath);
    l_cSrcPath.append(USERDATA);

    if (CFSDirectory::DoesDirecotryExist(l_cSrcPath)) {
      l_cDestPath.assign(l_cSrcPath);
      l_cDestPath.append(l_cDateTime);

      // instead of deleting the data, rename the persistence folder before shutdown
      if (0 != rename(l_cSrcPath.c_str(), l_cDestPath.c_str())) {  // LCOV_EXCL_BR_LINE 6: rename must return ok
        // LCOV_EXCL_START 6: rename must return ok
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Error renaming folder %s to %s, errno = %d",
               l_cSrcPath.c_str(),
               l_cDestPath.c_str(),
               errno);
        // LCOV_EXCL_STOP
      }
    }
  }
  if (eFrameworkunifiedFactoryData == (eFrameworkunifiedFactoryData & f_uinotificationpersistentservicepersistcategoryflag)) {
    l_cSrcPath.assign(l_cBasePath);
    l_cSrcPath.append(FACTORYDATA);

    if (CFSDirectory::DoesDirecotryExist(l_cSrcPath)) {
      l_cDestPath.assign(l_cSrcPath);
      l_cDestPath.append(l_cDateTime);

      // instead of deleting the data, rename the persistence folder before shutdown
      if (0 != rename(l_cSrcPath.c_str(), l_cDestPath.c_str())) {  // LCOV_EXCL_BR_LINE 6: rename must return ok
        // LCOV_EXCL_START 6: rename must return ok
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Error renaming folder %s to %s, errno = %d",
               l_cSrcPath.c_str(),
               l_cDestPath.c_str(),
               errno);
        // LCOV_EXCL_STOP
      }
    }
  }
  if (eFrameworkunifiedFactoryCustomerData == (eFrameworkunifiedFactoryCustomerData & f_uinotificationpersistentservicepersistcategoryflag)) {
    l_cSrcPath.assign(l_cBasePath);
    l_cSrcPath.append(FACTORYCUSTOMERDATA);

    if (CFSDirectory::DoesDirecotryExist(l_cSrcPath)) {
      l_cDestPath.assign(l_cSrcPath);
      l_cDestPath.append(l_cDateTime);

      // instead of deleting the data, rename the persistence folder before shutdown
      if (0 != rename(l_cSrcPath.c_str(), l_cDestPath.c_str())) {  // LCOV_EXCL_BR_LINE 6: rename must return ok
        // LCOV_EXCL_START 6: rename must return ok
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Error renaming folder %s to %s, errno = %d",
               l_cSrcPath.c_str(),
               l_cDestPath.c_str(),
               errno);
        // LCOV_EXCL_STOP
      }
    }
  }
  if (eFrameworkunifiedDealerData == (eFrameworkunifiedDealerData & f_uinotificationpersistentservicepersistcategoryflag)) {
    l_cSrcPath.assign(l_cBasePath);
    l_cSrcPath.append(DEALERDATA);

    if (CFSDirectory::DoesDirecotryExist(l_cSrcPath)) {
      l_cDestPath.assign(l_cSrcPath);
      l_cDestPath.append(l_cDateTime);

      // instead of deleting the data, rename the persistence folder before shutdown
      if (0 != rename(l_cSrcPath.c_str(), l_cDestPath.c_str())) {  // LCOV_EXCL_BR_LINE 6: rename must return ok
        // LCOV_EXCL_START 6: rename must return ok
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Error renaming folder %s to %s, errno = %d",
               l_cSrcPath.c_str(),
               l_cDestPath.c_str(),
               errno);
        // LCOV_EXCL_STOP
      }
    }
  }

  if (NULL != l_cDateTime) {  // LCOV_EXCL_BR_LINE 6: l_cDateTime can't be NULL
    delete[] l_cDateTime;
    l_cDateTime = NULL;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceSaveNotificationPersistentData
/// In this function, all the data related to persistent notification is retrieved from
/// notification manager and stored in persistent file by persistent manager.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNSNPP::NotificationpersistentserviceSaveNotificationPersistentData(EFrameworkunifiedNotificationType f_epersistenttype,
                                                     UI_32 f_uinotificationpersistentservicepersistcategoryflag) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

  // pointer of Notification Manager
  CNotificationManager *l_pNotificationManager = NULL;

  // pointer of Persistence Manager
  CPersistenceManager *l_pCNotificationpersistentservicePersistenceManager = NULL;

  Persistent_Notification_List_Type *l_vPersistentNotificationList = NULL;

  // get the instance of notification manager
  l_pNotificationManager = m_pNotificationManager;

  // get the instance of persistence manager
  l_pCNotificationpersistentservicePersistenceManager = m_pPersistenceManager;

  if (NULL != l_pNotificationManager && NULL != l_pCNotificationpersistentservicePersistenceManager) {  // LCOV_EXCL_BR_LINE 6: l_pNotificationManager and l_pCNotificationpersistentservicePersistenceManager can't be NULL  // NOLINT[whitespace/line_length]
    // create new vector to retrieve persistent notification data
    l_vPersistentNotificationList = new(std::nothrow) Persistent_Notification_List_Type();

    if (NULL != l_vPersistentNotificationList) {  // LCOV_EXCL_BR_LINE 5: l_vPersistentNotificationList can't be NULL
      // get all the persistent notification data in vector from notification manager
      l_pNotificationManager->NotificationpersistentserviceGetPersistentNotificationData(l_vPersistentNotificationList,
                                                               f_epersistenttype,
                                                               f_uinotificationpersistentservicepersistcategoryflag);
    } else {
      // LCOV_EXCL_START 5: l_vPersistentNotificationList can't be NULL
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "l_vPersistentNotificationList is NULL");
      l_estatus = eFrameworkunifiedStatusNullPointer;
      // LCOV_EXCL_STOP
    }

    if (NULL != l_vPersistentNotificationList) {  // LCOV_EXCL_BR_LINE 5: l_vPersistentNotificationList can't be NULL
      if (eFrameworkunifiedPersistedStateVar == f_epersistenttype) {
        // save persistent notification data from vector to file
        l_estatus = l_pCNotificationpersistentservicePersistenceManager->NotificationpersistentserviceSaveNotificationData(l_vPersistentNotificationList);
      } else if (eFrameworkunifiedPersistedStateUserVar == f_epersistenttype) {  // LCOV_EXCL_BR_LINE 200: f_epersistenttype must be eFrameworkunifiedPersistedStateVar or eFrameworkunifiedPersistedStateUserVar  // NOLINT[whitespace/line_length]
        l_estatus = l_pCNotificationpersistentservicePersistenceManager->NotificationpersistentserviceSaveUserNotificationData(l_vPersistentNotificationList);
      } else {
        // LCOV_EXCL_START 200: f_epersistenttype must be eFrameworkunifiedPersistedStateVar or eFrameworkunifiedPersistedStateUserVar
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        l_estatus = eFrameworkunifiedStatusFail;
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Save data for %d PersistentVarType not supported", f_epersistenttype);
        // LCOV_EXCL_STOP
      }


      // delete the locally created vector
      if (!l_vPersistentNotificationList->empty()) {
        for (UI_32 l_uiCount = 0;
             l_uiCount < l_vPersistentNotificationList->size();
             l_uiCount++) {
          if (NULL != l_vPersistentNotificationList->at(l_uiCount)) {  // LCOV_EXCL_BR_LINE 6: can't be NULL
            delete l_vPersistentNotificationList->at(l_uiCount);
          }
        }

        // clear the vector
        l_vPersistentNotificationList->clear();
      }

      delete l_vPersistentNotificationList;
      l_vPersistentNotificationList = NULL;
    } else {
      // LCOV_EXCL_START 5: l_vPersistentNotificationList can't be NULL
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "l_vPersistentNotificationList is NULL");
      l_estatus = eFrameworkunifiedStatusNullPointer;
      // LCOV_EXCL_STOP
    }
  } else {
    // LCOV_EXCL_START 6: l_pNotificationManager and l_pCNotificationpersistentservicePersistenceManager can't be NULL
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "l_pNotificationManager is NULL");
    l_estatus = eFrameworkunifiedStatusNullPointer;
    // LCOV_EXCL_STOP
  }


  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceOnLoadPersistentData
/// In this transition, persistent file is loaded by persistent manager and
/// persistent notifications and its data is stored by notification manager.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNSNPP::NotificationpersistentserviceLoadPersistentNotificationData(EFrameworkunifiedNotificationType f_epersistenttype) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

  // pointer of Notification Manager
  CNotificationManager *l_pNotificationManager = NULL;

  // pointer of Persistence Manager
  CPersistenceManager *l_pNotificationpersistentservicePersistenceManager = NULL;

  // iterator of CNotificationToPersist vector
  Persistent_Notification_List_Type *l_vPersistentNotificationList = NULL;

  // get the instance of notification manager
  l_pNotificationManager = m_pNotificationManager;

  // get the instance of persistent manager
  l_pNotificationpersistentservicePersistenceManager = m_pPersistenceManager;

  // create new vector for persistent notifications
  l_vPersistentNotificationList = new(std::nothrow) Persistent_Notification_List_Type();

  if (NULL != l_vPersistentNotificationList) {  // LCOV_EXCL_BR_LINE 5: l_vPersistentNotificationList can't be NULL
    if (NULL != l_pNotificationManager && NULL != l_pNotificationpersistentservicePersistenceManager) {  // LCOV_EXCL_BR_LINE 6: l_pNotificationManager and l_pNotificationpersistentservicePersistenceManager can't be NULL  // NOLINT[whitespace/line_length]
      if (eFrameworkunifiedPersistedStateVar == f_epersistenttype) {
        // read persistent notification data from file and store in vector
        l_estatus = l_pNotificationpersistentservicePersistenceManager->NotificationpersistentserviceReadNotificationData(l_vPersistentNotificationList);
      } else if (eFrameworkunifiedPersistedStateUserVar == f_epersistenttype) {
        l_estatus = l_pNotificationpersistentservicePersistenceManager->NotificationpersistentserviceReadUserNotificationData(l_vPersistentNotificationList);
      } else if (eFrameworkunifiedImmediatePersistedStateVar == f_epersistenttype) {   // LCOV_EXCL_BR_LINE 200: f_epersistenttype must be eFrameworkunifiedPersistedStateVar or eFrameworkunifiedPersistedStateUserVar or eFrameworkunifiedImmediatePersistedStateVar  // NOLINT[whitespace/line_length]
        l_estatus = l_pNotificationpersistentservicePersistenceManager->NotificationpersistentserviceReadNorNotificationData(l_vPersistentNotificationList);
      } else {
        // LCOV_EXCL_START 200: f_epersistenttype must be eFrameworkunifiedPersistedStateVar or eFrameworkunifiedPersistedStateUserVar or eFrameworkunifiedImmediatePersistedStateVar  // NOLINT[whitespace/line_length]
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        l_estatus = eFrameworkunifiedStatusFail;
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Read data for %d PersistentVarType not supported", f_epersistenttype);
        // LCOV_EXCL_STOP
      }

      if (0 < l_vPersistentNotificationList->size()) {
        // send the notification list to notification manager
        l_estatus = l_pNotificationManager->NotificationpersistentserviceSetPersistentNotificationData(l_vPersistentNotificationList);
      }
    } else {
      // LCOV_EXCL_START 6: l_pNotificationManager and l_pNotificationpersistentservicePersistenceManager can't be NULL
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "l_pNotificationManager or l_pNotificationpersistentservicePersistenceManager is NULL");
      l_estatus = eFrameworkunifiedStatusNullPointer;
      // LCOV_EXCL_STOP
    }

    if (!l_vPersistentNotificationList->empty()) {
      for (UI_32 l_uiCount = 0;
           l_uiCount < l_vPersistentNotificationList->size();
           l_uiCount++) {
        if (NULL != l_vPersistentNotificationList->at(l_uiCount)) {  // LCOV_EXCL_BR_LINE 6: can't be NULL
          delete l_vPersistentNotificationList->at(l_uiCount);
        }
      }

      // clear the vector
      l_vPersistentNotificationList->clear();
    }

    delete l_vPersistentNotificationList;
    l_vPersistentNotificationList = NULL;
  } else {
    // LCOV_EXCL_START 5: l_vPersistentNotificationList can't be NULL
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Memory allocation error for l_vPersistentNotificationList");
    l_estatus = eFrameworkunifiedStatusNullPointer;
    // LCOV_EXCL_STOP
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// CreateAndStartChildThreads
///
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNSNPP::CreateAndStartChildThreads(HANDLE f_happ) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

  if (NULL == f_happ) {  // LCOV_EXCL_BR_LINE 6: f_happ can't be NULL
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_estatus = eFrameworkunifiedStatusNullPointer;  // LCOV_EXCL_LINE 6: f_happ can't be NULL
  }

  if (eFrameworkunifiedStatusOK == l_estatus) {  // LCOV_EXCL_BR_LINE 6: l_estatus is eFrameworkunifiedStatusOK
    // create reader thread
    if (NULL != (m_hNSReadThread = FrameworkunifiedCreateChildThreadWithPriority(f_happ, m_cReadThreadName.c_str(), NSPCopyWorkerOnStart, NSPCopyWorkerOnStop, CNSNPP::m_siReadThreadPrio))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
             "Thread %s created, Prio=%d",
             m_cReadThreadName.c_str(),
             CNSNPP::m_siReadThreadPrio);

      // start the reader thread
      if (eFrameworkunifiedStatusOK == (l_estatus = FrameworkunifiedStartChildThread(f_happ, m_hNSReadThread, 0, NULL))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
        m_pPersistenceManager->SetReadThreadHandle(m_hNSReadThread);
      } else {
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Fail to Start Read Worker thread. Status:0x%x", l_estatus);  // LCOV_EXCL_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
      }
    } else {
      // LCOV_EXCL_START 4: NSFW error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_estatus = eFrameworkunifiedStatusNullPointer;
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error creating NPP Read Worker thread");
      // LCOV_EXCL_STOP
    }
  }

  if (eFrameworkunifiedStatusOK == l_estatus) {  // LCOV_EXCL_BR_LINE 6: l_estatus is eFrameworkunifiedStatusOK
    // create writer thread
    if (NULL != (m_hNSWriteThread = FrameworkunifiedCreateChildThreadWithPriority(f_happ, m_cWriteThreadName.c_str(), NSPCopyWorkerOnStart, NSPCopyWorkerOnStop, CNSNPP::m_siWriteThreadPrio))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
             "Thread %s created, Prio=%d",
             m_cWriteThreadName.c_str(),
             CNSNPP::m_siWriteThreadPrio);

      // start the writer thread
      if (eFrameworkunifiedStatusOK == (l_estatus = FrameworkunifiedStartChildThread(f_happ, m_hNSWriteThread, 0, NULL))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
        m_pPersistenceManager->SetWriteThreadHandle(m_hNSWriteThread);
      } else {
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Fail to Start Write Worker thread. Status:0x%x", l_estatus);  // LCOV_EXCL_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
      }
    } else {
      // LCOV_EXCL_START 4: NSFW error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_estatus = eFrameworkunifiedStatusNullPointer;
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error creating NPP Write Worker thread");
      // LCOV_EXCL_STOP
    }
  }

  if (eFrameworkunifiedStatusOK == l_estatus) {  // LCOV_EXCL_BR_LINE 6: l_estatus is eFrameworkunifiedStatusOK
    // create immediate Persistence thread
      if (NULL != (m_hNSImmediatePersistenceThread = FrameworkunifiedCreateChildThreadWithPriority(f_happ, m_cImmediatePersistenceThreadName.c_str(), NSPNorPersistenceWorkerOnStart, NSPNorPersistenceWorkerOnStop, CNSNPP::m_siImmediatePersistenceThreadPrio))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Thread %s created, Prio=%d", m_cImmediatePersistenceThreadName.c_str(),
             CNSNPP::m_siImmediatePersistenceThreadPrio);

      // start the immediate Persistence thread
      if (eFrameworkunifiedStatusOK == (l_estatus = FrameworkunifiedStartChildThread(f_happ, m_hNSImmediatePersistenceThread, 0, NULL))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
        m_pPersistenceManager->SetNorPersistenceThreadHandle(m_hNSImmediatePersistenceThread);
      } else {
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Fail to Start immediate persistence thread. Status:0x%x", l_estatus);  // LCOV_EXCL_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
      }
    } else {
      // LCOV_EXCL_START 4: NSFW error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_estatus = eFrameworkunifiedStatusNullPointer;
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error creating NPP immediate persistence Worker thread");
      // LCOV_EXCL_STOP
    }
  }

  return l_estatus;
}
