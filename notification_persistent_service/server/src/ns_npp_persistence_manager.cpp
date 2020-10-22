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
/// \brief    This file contains implementation of class CPersistenceManager.
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <native_service/ns_np_service.h>
#include <native_service/ns_np_service_if.h>
#include <native_service/ns_util_directory.h>
#include <native_service/ns_np_service_protocol.h>
#include <native_service/frameworkunified_sm_framework_dispatch.h>
#include <string>
#include <native_service/ns_np_service_nor_persistence_internal.h>
#include "ns_npp.h"
#include "ns_npp_types.h"
#include "ns_npp_notificationpersistentservicelog.h"
#include "ns_npp_copy_worker.h"
#include "ns_npp_persistence.h"
#include "ns_npp_persist_file.h"
#include "ns_npp_fs_directory.h"
#include "ns_npp_persist_folder.h"
#include "ns_npp_persistent_data.h"
#include "ns_npp_binary_accesser.h"
#include "ns_npp_persistence_manager.h"
#include "ns_npp_nor_persistence_worker_thread.h"
#include "ns_npp_state_nor_persistence_notification.h"

extern const CHAR AppName[];  // NOLINT (readability/naming)

// Initialisation of static class member for disabling persistence
BOOL CPersistenceManager::m_bPersistenceDisabled = FALSE;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// CPersistenceManager
/// Constructor of CPersistenceManager class
////////////////////////////////////////////////////////////////////////////////////////////////////
CPersistenceManager::CPersistenceManager() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  m_cUserNotificationTag = USERNOTIFICATIONFILE;

  m_poDataAccesser = NULL;

  m_cNotificationPersistFilepath = CPersistence::GetStoragePath();    // LCOV_EXCL_BR_LINE 11: unexpected branch

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Storage path for persistence=%s", m_cNotificationPersistFilepath.c_str());   // LCOV_EXCL_BR_LINE 11: unexpected branch  // NOLINT[whitespace/line_length]

  m_cNotificationPersistFilepath += AppName;
  m_cNotificationPersistFilepath += "/";

  m_hNSImmediatePersistenceThread = NULL;
  m_bAllFilePersisted = FALSE;
  m_bImmediatedDataPersisted = FALSE;

  if (eFrameworkunifiedStatusOK != Init()) {  // LCOV_EXCL_BR_LINE 6: Init() must be return eFrameworkunifiedStatusOK
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Unable to initialize Persistent Manager.");  // LCOV_EXCL_LINE 6: Init() must be return eFrameworkunifiedStatusOK  // NOLINT[whitespace/line_length]
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}
////////////////////////////////////////////////////////////////////////////////////////////////////
/// CPersistenceManager
/// Destructor of CPersistenceManager class
////////////////////////////////////////////////////////////////////////////////////////////////////
CPersistenceManager::~CPersistenceManager() {  // LCOV_EXCL_START 14: Resident process, global instance not released
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (NULL != m_poDataAccesser) {
    delete m_poDataAccesser;
  }

  Persist_Type_Iter l_itPersist_Type = m_mPersist_Type.begin();
  // Remove memory allocated for persistence objects
  for (; l_itPersist_Type != m_mPersist_Type.end(); ++l_itPersist_Type) {
    if (NULL != (*l_itPersist_Type).second) {
      delete(*l_itPersist_Type).second;
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus CPersistenceManager::Init() {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  m_mPersist_Type[ENOTIFICATIONPERSISTENTSERVICEPERSISTFILE] = new(std::nothrow) CFilePersistence();    // LCOV_EXCL_BR_LINE 11:unexpected branch

  m_mPersist_Type[ENOTIFICATIONPERSISTENTSERVICEPERSISTFOLDER] = new(std::nothrow) CFolderPersistence();    // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT[whitespace/line_length]

  m_poDataAccesser = new(std::nothrow) CBinaryAccesser();  // Save data in binary format.   // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT[whitespace/line_length]

  // check if all object creation is successful
  if (NULL != m_poDataAccesser) {   // LCOV_EXCL_BR_LINE 5:m_poDataAccesser can`t be NULL
    Persist_Type_Iter l_itPersist_Type = m_mPersist_Type.begin();

    // check if all object creation is successful
    for (; l_itPersist_Type != m_mPersist_Type.end();
         ++l_itPersist_Type) {
      if (NULL == (*l_itPersist_Type).second) {  // LCOV_EXCL_START 6:(*l_itPersist_Type).second can`t be NULL
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        l_estatus = eFrameworkunifiedStatusFail;
        break;
      }
      // LCOV_EXCL_STOP
    }
  } else {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_estatus = eFrameworkunifiedStatusFail;  // LCOV_EXCL_LINE 5: m_poDataAccesser can`t be NULL
  }

  if (eFrameworkunifiedStatusOK != l_estatus) {   // LCOV_EXCL_START 6: l_estatus must be eFrameworkunifiedStatusOK
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    if (NULL != m_poDataAccesser) {
      delete m_poDataAccesser;
    }

    Persist_Type_Iter l_itPersist_Type = m_mPersist_Type.begin();
    // check if all object creation is successful
    for (; l_itPersist_Type != m_mPersist_Type.end(); ++l_itPersist_Type) {
      if (NULL != (*l_itPersist_Type).second) {
        delete(*l_itPersist_Type).second;
      }
    }
    // LCOV_EXCL_STOP
  }
  std::string f_csourcepath = CPersistence::GetStoragePath();
  if (f_csourcepath.length() > 0) {   // LCOV_EXCL_BR_LINE 6: f_csourcepath.length() is always bigger then 0
    f_csourcepath.append(RELEASETEMP_DIR);    // LCOV_EXCL_BR_LINE 11:unexpected branch
    if (CFSDirectory::DoesDirecotryExist(f_csourcepath)) {
      f_csourcepath.append(RELEASETEMP_FILENAME);
      if (0 != remove(f_csourcepath.c_str())) {
        FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Temporary file not deleted::%s, errno:%d", f_csourcepath.c_str(), errno);
      }
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceRegister
/// Registers a tag for the file/folder. This tag will be used for releasing or loading
/// a file/folder.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CPersistenceManager::NotificationpersistentserviceRegister(std::string f_cappname,
                                            std::string f_ctag,
                                            ENotificationpersistentservicePersistType f_epersisttype,
                                            BOOL bisuserpersistence) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusFail;

  if (f_cappname.empty() || f_ctag.empty()) {  // LCOV_EXCL_BR_LINE 6: double check, f_cappname and f_ctag can't be empty  // NOLINT[whitespace/line_length]
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Appname & tag should not be empty.");  // LCOV_EXCL_LINE 6: f_cappname and f_ctag can't be empty  // NOLINT[whitespace/line_length]
  } else {
    // check boundary conditions
    if ((ENOTIFICATIONPERSISTENTSERVICEPERSISTFIRST < f_epersisttype) && (f_epersisttype < ENOTIFICATIONPERSISTENTSERVICEPERSISTLAST)) {  // LCOV_EXCL_BR_LINE 6: f_epersisttype is always bigger then ENOTIFICATIONPERSISTENTSERVICEPERSISTFIRST and less then ENOTIFICATIONPERSISTENTSERVICEPERSISTLAST  // NOLINT[whitespace/line_length]
      if (NULL != m_mPersist_Type[f_epersisttype]) {  // LCOV_EXCL_BR_LINE 6: m_mPersist_Type[f_epersisttype] would not be null  // NOLINT[whitespace/line_length]
        // register tag
        l_estatus = m_mPersist_Type[f_epersisttype]->Register(f_cappname, f_ctag,
                                                              bisuserpersistence);  // f_eRegisterType, f_cUser); // LCOV_EXCL_BR_LINE 11: unexpected branch  // NOLINT[whitespace/line_length]
        // m_mPersist_Type[f_ePersistFileType]->ListAllInMap();
      } else {
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Persistence object for %d is null.", f_epersisttype);   // LCOV_EXCL_LINE 6: m_mPersist_Type[f_epersisttype] would not be null  // NOLINT[whitespace/line_length]
      }
    } else {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Enum value %d is out of bound.", f_epersisttype);  // LCOV_EXCL_LINE 6: f_epersisttype is always biger then ENOTIFICATIONPERSISTENTSERVICEPERSISTFIRST and less then ENOTIFICATIONPERSISTENTSERVICEPERSISTLAST  // NOLINT[whitespace/line_length]
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceRelease
/// Entry for the file/folder is stored in map for persistence.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CPersistenceManager::NotificationpersistentserviceRelease(std::string f_cappname,
                                           std::string f_ctag,
                                           std::string f_cmempath,
                                           EFrameworkunifiedReleaseType enotificationpersistentservicereleasetype,
                                           ENotificationpersistentservicePersistType f_epersisttype,
                                           std::string f_cusername) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusFail;

  if (f_cappname.empty() || f_ctag.empty() || f_cmempath.empty()) {  // LCOV_EXCL_BR_LINE 6: double check, f_cappname, f_cmempath and f_ctag can't be empty  // NOLINT[whitespace/line_length]
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Appname & tag should not be empty.");  // LCOV_EXCL_LINE 6: double check, f_cappname, f_cmempath and f_ctag can't be empty  // NOLINT[whitespace/line_length]
  } else {
    // check boundary conditions
    if ((ENOTIFICATIONPERSISTENTSERVICEPERSISTFIRST < f_epersisttype) && (f_epersisttype < ENOTIFICATIONPERSISTENTSERVICEPERSISTLAST)) {  // LCOV_EXCL_BR_LINE 6: f_epersisttype is always bigger then ENOTIFICATIONPERSISTENTSERVICEPERSISTFIRST and less then ENOTIFICATIONPERSISTENTSERVICEPERSISTLAST  // NOLINT[whitespace/line_length]
      // release file
      if (NULL != m_mPersist_Type[f_epersisttype]) {  // LCOV_EXCL_BR_LINE 6: m_mPersist_Type[f_epersisttype] would not be null  // NOLINT[whitespace/line_length]
        l_estatus = m_mPersist_Type[f_epersisttype]->Release(f_cappname,
                                                             f_ctag,
                                                             f_cmempath,
                                                             enotificationpersistentservicereleasetype,
                                                             f_cusername);    // LCOV_EXCL_BR_LINE 11: unexpected branch
      } else {
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Persistence object for %d is null.", f_epersisttype);  // LCOV_EXCL_LINE 6: m_mPersist_Type[f_epersisttype] would not be null  // NOLINT[whitespace/line_length]
      }
    } else {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Enum value %d is out of bound.", f_epersisttype);  // LCOV_EXCL_LINE 6: f_epersisttype is always bigger then ENOTIFICATIONPERSISTENTSERVICEPERSISTFIRST and less then ENOTIFICATIONPERSISTENTSERVICEPERSISTLAST  // NOLINT[whitespace/line_length]
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceLoad
/// Load file/folder from persistent memory to the specified location.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CPersistenceManager::NotificationpersistentserviceLoad(std::string f_cappname,
                                        std::string f_ctag,
                                        std::string f_cretrievepath,
                                        ENotificationpersistentservicePersistType f_epersisttype,
                                        std::string f_cusername) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusFail;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (f_cappname.empty() || f_ctag.empty() || f_cretrievepath.empty()) {  // LCOV_EXCL_BR_LINE 6: double check, f_cretrievepath, f_cappname and f_ctag can't be empty  // NOLINT[whitespace/line_length]
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Appname, tag & retrieve path should not be empty.");  // LCOV_EXCL_LINE 6: double check, f_cretrievepath, f_cappname and f_ctag can't be empty  // NOLINT[whitespace/line_length]
  } else {
    // check boundary conditions
    if ((ENOTIFICATIONPERSISTENTSERVICEPERSISTFIRST < f_epersisttype) && (f_epersisttype < ENOTIFICATIONPERSISTENTSERVICEPERSISTLAST)) {  // LCOV_EXCL_BR_LINE 6: f_epersisttype is always bigger then ENOTIFICATIONPERSISTENTSERVICEPERSISTFIRST and less then ENOTIFICATIONPERSISTENTSERVICEPERSISTLAST  // NOLINT[whitespace/line_length]
      if (NULL != m_mPersist_Type[f_epersisttype]) {  // LCOV_EXCL_BR_LINE 6: m_mPersist_Type[f_epersisttype] would not be null  // NOLINT[whitespace/line_length]
        // load file
        l_estatus = m_mPersist_Type[f_epersisttype]->Load(f_cappname, f_ctag, f_cretrievepath, f_cusername);  // LCOV_EXCL_BR_LINE 11: unexpected branch  // NOLINT[whitespace/line_length]
      } else {
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Persistence object for %d is null.", f_epersisttype);   // LCOV_EXCL_LINE 6: m_mPersist_Type[f_epersisttype] would not be null  // NOLINT[whitespace/line_length]
      }
    } else {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Enum value %d is out of bound.", f_epersisttype);   // LCOV_EXCL_LINE 6: f_epersisttype is always bigger then ENOTIFICATIONPERSISTENTSERVICEPERSISTFIRST and less then ENOTIFICATIONPERSISTENTSERVICEPERSISTLAST  // NOLINT[whitespace/line_length]
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// AckReceivedFromWorker
/// Send release ack to file/folder persistence object.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CPersistenceManager::AckReceivedFromWorker(PCSTR f_csource, PCSTR f_ctag, ENotificationpersistentservicePersistType f_epersisttype,
                                                      BOOL f_bcopystatus, ENPS_Loadtype f_eloadtype) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusFail;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if ((NULL == f_csource) || (NULL == f_ctag)) {  // LCOV_EXCL_BR_LINE 6: f_csource and f_ctag can't be NULL
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "f_csource or f_ctag is NULL.");  // LCOV_EXCL_LINE 6: f_csource and f_ctag can't be NULL  // NOLINT[whitespace/line_length]
  } else {
    // check boundary conditions
    if ((ENOTIFICATIONPERSISTENTSERVICEPERSISTFIRST < f_epersisttype) && (f_epersisttype < ENOTIFICATIONPERSISTENTSERVICEPERSISTLAST)) {   // LCOV_EXCL_BR_LINE 6: f_epersisttype is always bigger then ENOTIFICATIONPERSISTENTSERVICEPERSISTFIRST and less then ENOTIFICATIONPERSISTENTSERVICEPERSISTLAST  // NOLINT[whitespace/line_length]
      if (NULL != m_mPersist_Type[f_epersisttype]) {
        // Send release ack to file/folder persistence object.
        l_estatus = m_mPersist_Type[f_epersisttype]->AckReceivedFromWorker(f_csource, f_ctag,
                                                                           f_bcopystatus, f_eloadtype);
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Persistence object for %d is null.", f_epersisttype);
      }
    } else {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Enum value %d is out of bound.", f_epersisttype);   // LCOV_EXCL_LINE 6: f_epersisttype is always bigger then ENOTIFICATIONPERSISTENTSERVICEPERSISTFIRST and less then ENOTIFICATIONPERSISTENTSERVICEPERSISTLAST  // NOLINT[whitespace/line_length]
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceSaveNotificationData
/// Save notification data in a persistent file.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CPersistenceManager::NotificationpersistentserviceSaveNotificationData(Persistent_Notification_List_Type
                                                        *f_vpersistentnotificationlist) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if ((NULL == m_poDataAccesser) || (NULL == f_vpersistentnotificationlist)) {  // LCOV_EXCL_BR_LINE 6: m_poDataAccesser and f_vpersistentnotificationlist would not be null  // NOLINT[whitespace/line_length]
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "m_poDataAccesser or f_vpersistentnotificationlist is NULL.");   // LCOV_EXCL_LINE 6: m_poDataAccesser and f_vpersistentnotificationlist would not be null  // NOLINT[whitespace/line_length]
  } else {
    EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
    std::string l_cNotificationFilePath = "";

    l_cNotificationFilePath = m_cNotificationPersistFilepath;
    l_cNotificationFilePath +=  NOTIFICATIONFILE;
    if (eFrameworkunifiedStatusOK != (l_estatus = m_poDataAccesser->PersistData(l_cNotificationFilePath,
                                                                   f_vpersistentnotificationlist,
                                                                   eFrameworkunifiedPersistedStateVar,
                                                                   eFrameworkunifiedUserData))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error retrieving notification data from file : %s, status: %d",
             l_cNotificationFilePath.c_str(), l_estatus);
    }

    l_cNotificationFilePath = m_cNotificationPersistFilepath;
    l_cNotificationFilePath +=  FACTORYNOTIFICATIONFILE;
    if (eFrameworkunifiedStatusOK != (l_estatus = m_poDataAccesser->PersistData(l_cNotificationFilePath,
                                                                   f_vpersistentnotificationlist,
                                                                   eFrameworkunifiedPersistedStateVar,
                                                                   eFrameworkunifiedFactoryData))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error retrieving notification data from file : %s, status: %d",
             l_cNotificationFilePath.c_str(), l_estatus);
    }

    l_cNotificationFilePath = m_cNotificationPersistFilepath;
    l_cNotificationFilePath +=  FACTORYCUSTOMERNOTIFICATIONFILE;
    if (eFrameworkunifiedStatusOK != (l_estatus = m_poDataAccesser->PersistData(l_cNotificationFilePath,
                                                                   f_vpersistentnotificationlist,
                                                                   eFrameworkunifiedPersistedStateVar,
                                                                   eFrameworkunifiedFactoryCustomerData))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error retrieving notification data from file : %s, status: %d",
             l_cNotificationFilePath.c_str(), l_estatus);
    }

    l_cNotificationFilePath = m_cNotificationPersistFilepath;
    l_cNotificationFilePath +=  DEALERNOTIFICATIONFILE;
    if (eFrameworkunifiedStatusOK != (l_estatus = m_poDataAccesser->PersistData(l_cNotificationFilePath,
                                                                   f_vpersistentnotificationlist,
                                                                   eFrameworkunifiedPersistedStateVar,
                                                                   eFrameworkunifiedDealerData))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error retrieving notification data from file : %s, status: %d",
             l_cNotificationFilePath.c_str(), l_estatus);
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eFrameworkunifiedStatusOK;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceSaveUserNotificationData
/// Save user notification data in a persistent file.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CPersistenceManager::NotificationpersistentserviceSaveUserNotificationData(Persistent_Notification_List_Type
                                                            *f_vpersistentnotificationlist) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusFail;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if ((NULL == m_poDataAccesser) || (NULL == f_vpersistentnotificationlist)) {  // LCOV_EXCL_BR_LINE 6: m_poDataAccesser and f_vpersistentnotificationlist would not be null  // NOLINT[whitespace/line_length]
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "m_poDataAccesser or f_vpersistentnotificationlist is NULL.");   // LCOV_EXCL_LINE 6: m_poDataAccesser and f_vpersistentnotificationlist would not be null  // NOLINT[whitespace/line_length]
  } else {
    l_estatus = m_poDataAccesser->PersistData(m_cNotificationUserMemFilepath,
                                              f_vpersistentnotificationlist,
                                              eFrameworkunifiedPersistedStateUserVar);   // LCOV_EXCL_BR_LINE 11: unexpected branch
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceReadNotificationData
/// Get the list of all persistent notifications from a persistent memory and store it in a map.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CPersistenceManager::NotificationpersistentserviceReadNotificationData(Persistent_Notification_List_Type
                                                        * &f_vpersistentnotificationlist) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (NULL != m_poDataAccesser) {   // LCOV_EXCL_BR_LINE 6: m_poDataAccesser would not be null
    EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
    std::string l_cNotificationFilePath = "";

    l_cNotificationFilePath = m_cNotificationPersistFilepath;
    l_cNotificationFilePath += NOTIFICATIONFILE;
    if (eFrameworkunifiedStatusOK != (l_estatus = m_poDataAccesser->RetrieveData(l_cNotificationFilePath,
                                                                    f_vpersistentnotificationlist,
                                                                    eFrameworkunifiedUserData))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error retrieving notification data from file : %s, status: %d",
             l_cNotificationFilePath.c_str(), l_estatus);
    }

    l_cNotificationFilePath = m_cNotificationPersistFilepath;
    l_cNotificationFilePath += FACTORYNOTIFICATIONFILE;
    if (eFrameworkunifiedStatusOK != (l_estatus = m_poDataAccesser->RetrieveData(l_cNotificationFilePath,
                                                                    f_vpersistentnotificationlist,
                                                                    eFrameworkunifiedFactoryData))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error retrieving notification data from file : %s, status: %d",
             l_cNotificationFilePath.c_str(), l_estatus);
    }

    l_cNotificationFilePath = m_cNotificationPersistFilepath;
    l_cNotificationFilePath += FACTORYCUSTOMERNOTIFICATIONFILE;
    if (eFrameworkunifiedStatusOK != (l_estatus = m_poDataAccesser->RetrieveData(l_cNotificationFilePath,
                                                                    f_vpersistentnotificationlist,
                                                                    eFrameworkunifiedFactoryCustomerData))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error retrieving notification data from file : %s, status: %d",
             l_cNotificationFilePath.c_str(), l_estatus);
    }

    l_cNotificationFilePath = m_cNotificationPersistFilepath;
    l_cNotificationFilePath += DEALERNOTIFICATIONFILE;
    if (eFrameworkunifiedStatusOK != (l_estatus = m_poDataAccesser->RetrieveData(l_cNotificationFilePath,
                                                                    f_vpersistentnotificationlist,
                                                                    eFrameworkunifiedDealerData))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error retrieving notification data from file : %s, status: %d",
             l_cNotificationFilePath.c_str(), l_estatus);
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eFrameworkunifiedStatusOK;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceReadUserNotificationData
/// Get the list of all user persistent notifications from a persistent memory and store it in a map.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CPersistenceManager::NotificationpersistentserviceReadUserNotificationData(Persistent_Notification_List_Type
                                                            * &f_vpersistentnotificationlist) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusFail;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (NULL != m_poDataAccesser) {   // LCOV_EXCL_BR_LINE 6: m_poDataAccesser would not be null
    l_estatus = m_poDataAccesser->RetrieveData(m_cNotificationUserMemFilepath,
                                               f_vpersistentnotificationlist,
                                               eFrameworkunifiedUserData);   // LCOV_EXCL_BR_LINE 11:unexpected branch
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// PersistAllReleaseRequests
/// Persist all released files and folder contained in the map in persistent memory.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CPersistenceManager::PersistAllReleaseRequests(UI_32 f_uinotificationpersistentservicepersistcategoryflag) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  ENotificationpersistentservicePersistType l_ePersistType = ENOTIFICATIONPERSISTENTSERVICEPERSISTFIRST;
  l_ePersistType = (ENotificationpersistentservicePersistType)(l_ePersistType + 1);

  // Persist all i.e files and folders
  while (l_ePersistType < ENOTIFICATIONPERSISTENTSERVICEPERSISTLAST) {
    if (NULL != m_mPersist_Type[l_ePersistType]) {
      if (eFrameworkunifiedStatusFail == m_mPersist_Type[l_ePersistType]->PersistAllReleaseRequests(f_uinotificationpersistentservicepersistcategoryflag)) {   // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        l_estatus = eFrameworkunifiedStatusFail;   // LCOV_EXCL_LINE 4: NSFW error case.
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Persistence object for %d is null.", l_ePersistType);   // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
    }

    l_ePersistType = (ENotificationpersistentservicePersistType)(l_ePersistType + 1);
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}


////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentservicePersistAllUserRequests
/// Persist all user files and folder contained in the map in persistent memory.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CPersistenceManager::NotificationpersistentservicePersistAllUserRequests() {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusFail;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  ENotificationpersistentservicePersistType l_ePersistType = ENOTIFICATIONPERSISTENTSERVICEPERSISTFIRST;
  l_ePersistType = (ENotificationpersistentservicePersistType)(l_ePersistType + 1);

  // Persist all i.e files and folders
  while (l_ePersistType < ENOTIFICATIONPERSISTENTSERVICEPERSISTLAST) {
    if (NULL != m_mPersist_Type[l_ePersistType]) {
      l_estatus = m_mPersist_Type[l_ePersistType]->PersistAllUserRequests();
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Persistence object for %d is null.", l_ePersistType);
    }

    l_ePersistType = (ENotificationpersistentservicePersistType)(l_ePersistType + 1);
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}



////////////////////////////////////////////////////////////////////////////////////////////////
/// SetReadThreadHandle
/// Pass the handle of the read thread to the object of file/folder persistence
////////////////////////////////////////////////////////////////////////////////////////////////
VOID CPersistenceManager::SetReadThreadHandle(HANDLE f_hreadthread) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (f_hreadthread != NULL) {    // LCOV_EXCL_BR_LINE 6: f_hreadthread would not be null
    // Set thread handle for object persistfile objects. i.e. for file/folder
    for (Persist_Type_Iter l_itPersist_Type = m_mPersist_Type.begin();
         l_itPersist_Type != m_mPersist_Type.end();
         ++l_itPersist_Type) {
      if (NULL != (*l_itPersist_Type).second) {  // LCOV_EXCL_BR_LINE 6: (*l_itPersist_Type).second can't be NULL
        (*l_itPersist_Type).second->SetReadThreadHandle(f_hreadthread);
      }
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// SetWriteThreadHandle
/// Pass the handle of the write thread to the object of file/folder persistence
////////////////////////////////////////////////////////////////////////////////////////////////
VOID CPersistenceManager::SetWriteThreadHandle(HANDLE f_hwritethread) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (NULL != f_hwritethread) {   // LCOV_EXCL_BR_LINE 6: f_hwritethread would not be null
    // Set thread handle for object persist objects. i.e. for file/folder
    for (Persist_Type_Iter l_itPersist_Type = m_mPersist_Type.begin();
         l_itPersist_Type != m_mPersist_Type.end();
         ++l_itPersist_Type) {
      if (NULL != (*l_itPersist_Type).second) {  // LCOV_EXCL_BR_LINE 6: (*l_itPersist_Type).second can't be NULL
        (*l_itPersist_Type).second->SetWriteThreadHandle(f_hwritethread);
      }
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// SetWriteThreadHandle
/// Pass the handle of the write thread to the object of file/folder persistence
////////////////////////////////////////////////////////////////////////////////////////////////
VOID CPersistenceManager::SetNorPersistenceThreadHandle(HANDLE f_hwritethread) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (NULL != f_hwritethread) {   // LCOV_EXCL_BR_LINE 6: f_hwritethread would not be null
    m_hNSImmediatePersistenceThread = f_hwritethread;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

////////////////////////////////////////////////////////////////////////////////////////////
/// NPServiceOnCpWorkerAckCmd
/// Handles when the CopyWorker sends an ack back for a message received .
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPServiceOnCpWorkerAckCmd(HANDLE f_happ) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  NSP_CopyAckMsg l_tack = {};
  if (f_happ) {   // LCOV_EXCL_BR_LINE 4: NSFW error case.
    if (eFrameworkunifiedStatusOK == (l_estatus = FrameworkunifiedGetMsgDataOfSize(f_happ, (PVOID)&l_tack, sizeof(NSP_CopyAckMsg), eSMRRelease))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]  // NOLINT[whitespace/line_length]
      FRAMEWORKUNIFIEDLOG(ZONE_NPP_INFO, __FUNCTION__, "CP_WRK_ACK_CMD_COMPLETE for cmd 0x%X", l_tack.m_eworkerprotocol);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
    } else {
      // LCOV_EXCL_START 4: NSFW error case.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Unable to get message data in NPServiceOnCpWorkerAckCmd, status : 0x%x", l_estatus);
      // LCOV_EXCL_STOP
    }
  } else {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_estatus = eFrameworkunifiedStatusNullPointer;  // LCOV_EXCL_LINE 4: NSFW error case.
  }

  return l_estatus;
}


////////////////////////////////////////////////////////////////////////////////////////////
/// SetUserPersistentPath
///
////////////////////////////////////////////////////////////////////////////////////////////
VOID CPersistenceManager::SetUserPersistentPath(std::string f_cusername) {
  m_cNotificationUserMemFilepath = CPersistence::GetStoragePath();
  m_cNotificationUserMemFilepath += AppName;
  m_cNotificationUserMemFilepath += "/";
  m_cNotificationUserMemFilepath += f_cusername;
  m_cNotificationUserMemFilepath += "/";
  m_cNotificationUserMemFilepath += m_cUserNotificationTag;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// IsUserPersistence
/// Check if tag is user persistence
////////////////////////////////////////////////////////////////////////////////////////////
BOOL CPersistenceManager::IsUserPersistence(std::string f_ctag, ENotificationpersistentservicePersistType f_epersisttype) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  BOOL l_bUserPersistence = FALSE;

  // check boundary conditions
  if ((ENOTIFICATIONPERSISTENTSERVICEPERSISTFIRST < f_epersisttype) && (f_epersisttype < ENOTIFICATIONPERSISTENTSERVICEPERSISTLAST)) {  // LCOV_EXCL_BR_LINE 6: f_epersisttype is always bigger then ENOTIFICATIONPERSISTENTSERVICEPERSISTFIRST and less then ENOTIFICATIONPERSISTENTSERVICEPERSISTLAST  // NOLINT[whitespace/line_length]
    if (NULL != m_mPersist_Type[f_epersisttype]) {  // LCOV_EXCL_BR_LINE 6: m_mPersist_Type[f_epersisttype] can't be null  // NOLINT[whitespace/line_length]
      // load file
      l_bUserPersistence = m_mPersist_Type[f_epersisttype]->IsUserPersistence(f_ctag);    // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT[whitespace/line_length]
    } else {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Persistence object for %d is null.", f_epersisttype);  // LCOV_EXCL_LINE 6: m_mPersist_Type[f_epersisttype] can't be null  // NOLINT[whitespace/line_length]
    }
  } else {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Enum value %d is out of bound.", f_epersisttype);   // LCOV_EXCL_LINE 6: f_epersisttype is always bigger then ENOTIFICATIONPERSISTENTSERVICEPERSISTFIRST and less then ENOTIFICATIONPERSISTENTSERVICEPERSISTLAST  // NOLINT[whitespace/line_length]
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_bUserPersistence;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// HaveAllReleaseRequestsPersisted
/// Check if all release requests processed or not
////////////////////////////////////////////////////////////////////////////////////////////
BOOL CPersistenceManager::HaveAllReleaseRequestsPersisted() {
  BOOL l_bRetVal = TRUE;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // check if all files are persisted
  if (FALSE == m_bAllFilePersisted) {
    ENotificationpersistentservicePersistType l_ePersistType = ENOTIFICATIONPERSISTENTSERVICEPERSISTFIRST;
    l_ePersistType = (ENotificationpersistentservicePersistType)(l_ePersistType + 1);

    std::string l_cTagNotReleased = "";
    std::string l_cTagList = "";    // LCOV_EXCL_BR_LINE 11:except,C++ STL

    // Persist all i.e files and folders
    while (l_ePersistType < ENOTIFICATIONPERSISTENTSERVICEPERSISTLAST) {
      if (NULL != m_mPersist_Type[l_ePersistType]) {
        l_cTagNotReleased.assign("");   // LCOV_EXCL_BR_LINE 11:except,C++ STL
        // if(eFrameworkunifiedStatusFail == m_mPersist_Type[l_ePersistType]->PersistAllReleaseRequests())
        if (FALSE == m_mPersist_Type[l_ePersistType]->HaveAllReleaseRequestsPersisted(l_cTagNotReleased)) {
          l_bRetVal = FALSE;
          l_cTagList.append(l_cTagNotReleased);
        }
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Persistence object for %d is null.", l_ePersistType);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
      }

      l_ePersistType = (ENotificationpersistentservicePersistType)(l_ePersistType + 1);
    }

    if (0 != l_cTagList.size()) {
      FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Tags not released:: %s", l_cTagList.c_str());
    }
  }

  if (TRUE == l_bRetVal) {
    m_bAllFilePersisted = TRUE;

    // if all files are persisted set the return value to the status of immediate data persistency
    l_bRetVal = m_bImmediatedDataPersisted;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_bRetVal;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// ResetPersistFlag
///
////////////////////////////////////////////////////////////////////////////////////////////
VOID CPersistenceManager::ResetPersistFlag() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  ENotificationpersistentservicePersistType l_ePersistType = ENOTIFICATIONPERSISTENTSERVICEPERSISTFIRST;
  l_ePersistType = (ENotificationpersistentservicePersistType)(l_ePersistType + 1);

  // Persist all i.e files and folders
  while (l_ePersistType < ENOTIFICATIONPERSISTENTSERVICEPERSISTLAST) {
    if (NULL != m_mPersist_Type[l_ePersistType]) {
      m_mPersist_Type[l_ePersistType]->ResetPersistFlag();
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Persistence object for %d is null.", l_ePersistType);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
    }

    l_ePersistType = (ENotificationpersistentservicePersistType)(l_ePersistType + 1);
  }

  m_bAllFilePersisted = FALSE;

  m_bImmediatedDataPersisted = FALSE;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceReadNorNotificationData
/// Get the list of all user persistent notifications from a persistent memory and store it in a map.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CPersistenceManager::NotificationpersistentserviceReadNorNotificationData(Persistent_Notification_List_Type
                                                           * &f_vpersistentnotificationlist) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (eFrameworkunifiedStatusOK != ReadImmediateNotificationData(f_vpersistentnotificationlist, eFrameworkunifiedUserData)) {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "No Immediate User data in persistent memory");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
  }

  if (eFrameworkunifiedStatusOK != ReadImmediateNotificationData(f_vpersistentnotificationlist, eFrameworkunifiedFactoryData)) {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "No Immediate Factory data in persistent memory");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
  }

  if (eFrameworkunifiedStatusOK != ReadImmediateNotificationData(f_vpersistentnotificationlist, eFrameworkunifiedFactoryCustomerData)) {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "No Immediate Factory Customer data in persistent memory");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
  }

  if (eFrameworkunifiedStatusOK != ReadImmediateNotificationData(f_vpersistentnotificationlist, eFrameworkunifiedDealerData)) {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "No Immediate Dealer data in persistent memory");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// ClearPersistenceData
/// Deletes the data from the persistent memory.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CPersistenceManager::ClearPersistenceData(const EFrameworkunifiedClearPersistence &f_enotificationpersistentserviceclearpersistencescope) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

  // persistence memory path to be cleared
  std::string l_cPath = "";

  switch (f_enotificationpersistentserviceclearpersistencescope) {  // LCOV_EXCL_BR_LINE 6: f_enotificationpersistentserviceclearpersistencescope must be eFrameworkunifiedClearAllData, eFrameworkunifiedClearAllApplicationData, eFrameworkunifiedClearAllNotificationData, eFrameworkunifiedClearCurrentUserData, eFrameworkunifiedClearCurrentUserApplicationData, eFrameworkunifiedClearCurrentUserNotificationData.  // NOLINT[whitespace/line_length]
    case eFrameworkunifiedClearAllData: {
        // clears all the data from the persistent memory
        l_cPath.append(CPersistence::GetStoragePath());
      }
      break;

    case eFrameworkunifiedClearAllApplicationData: {
        // TODO(my_username): clears all the application folder in persistence memory
        // don't delete the NS_NPS folder
      }
      break;

    case eFrameworkunifiedClearAllNotificationData: {
        // TODO(my_username): deletes the NS_NPS folder
      }
      break;

    case eFrameworkunifiedClearCurrentUserData: {
        // TODO(my_username): deletes the user folder from all the application folder in persisted memory
        // as well as from the NS_NPS folder
      }
      break;

    case eFrameworkunifiedClearCurrentUserApplicationData: {
        // TODO(my_username): deletes the user folder from all the application folder
      }
      break;

    case eFrameworkunifiedClearCurrentUserNotificationData: {
        // TODO(my_username): deletes the user folder from the NS_NPS folder
      }
      break;

    default: {
        // LCOV_EXCL_START 6: f_enotificationpersistentserviceclearpersistencescope must be eFrameworkunifiedClearAllData, eFrameworkunifiedClearAllApplicationData, eFrameworkunifiedClearAllNotificationData, eFrameworkunifiedClearCurrentUserData, eFrameworkunifiedClearCurrentUserApplicationData, eFrameworkunifiedClearCurrentUserNotificationData.  // NOLINT[whitespace/line_length]
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Invalid _EFrameworkunifiedClearPersistence Parameter:: %d", f_enotificationpersistentserviceclearpersistencescope);
        l_estatus = eFrameworkunifiedStatusInvldParam;
        // LCOV_EXCL_STOP
      }
      break;
  }

  if (0 != l_cPath.size()) {
    if (CFSDirectory::DoesDirecotryExist(l_cPath)) {
      if (TRUE == CFSDirectory::RemoveDirectory(l_cPath)) {  // LCOV_EXCL_BR_LINE 6: RemoveDirectory always return ok
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Delete persistent directory successful : %s", l_cPath.c_str());
      } else {
        // LCOV_EXCL_START 6: RemoveDirectory always return ok
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Delete persistent directory unsuccessful : %s", l_cPath.c_str());
        l_estatus = eFrameworkunifiedStatusErrOther;
        // LCOV_EXCL_STOP
      }
    }
  } else {
    l_estatus = eFrameworkunifiedStatusErrOther;
  }

  // delete NOR data
  l_cPath.clear();
  l_cPath.append(IMMEDIATE_PERSISTENCE_STORAGE_V2);
  if (0 != l_cPath.size()) {  // LCOV_EXCL_BR_LINE 6: l_cPath.size can't be 0
    if (CFSDirectory::DoesDirecotryExist(l_cPath)) {
      if (TRUE == CFSDirectory::RemoveDirectory(l_cPath)) {  // LCOV_EXCL_BR_LINE 6: RemoveDirectory always return ok
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Delete nor persistent directory successful : %s", l_cPath.c_str());
      } else {
        // LCOV_EXCL_START 6: RemoveDirectory always return ok
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Delete nor persistent directory unsuccessful : %s", l_cPath.c_str());
        l_estatus = eFrameworkunifiedStatusErrOther;
        // LCOV_EXCL_STOP
      }
    }
  } else {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_estatus = eFrameworkunifiedStatusErrOther;  // LCOV_EXCL_LINE 6: l_cPath.size can't be 0
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceSetPersistentCategory
/// Sets the persist type of file or folder.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CPersistenceManager::NotificationpersistentserviceSetPersistentCategory(std::string f_crequestorname,
                                                         std::string f_ctag,
                                                         EFrameworkunifiedPersistCategory f_epersistcategory,
                                                         ENotificationpersistentservicePersistType f_epersisttype) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

  if (f_ctag.empty()) {  // LCOV_EXCL_BR_LINE 6: double check, f_ctag.size can't be empty
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Tag name is empty.");  // LCOV_EXCL_LINE 6: double check, f_ctag.size can't be empty
  } else {
    // check boundary conditions
    if ((ENOTIFICATIONPERSISTENTSERVICEPERSISTFIRST < f_epersisttype) && (f_epersisttype < ENOTIFICATIONPERSISTENTSERVICEPERSISTLAST)) {  // LCOV_EXCL_BR_LINE 6: f_epersisttype is always bigger then ENOTIFICATIONPERSISTENTSERVICEPERSISTFIRST and less then ENOTIFICATIONPERSISTENTSERVICEPERSISTLAST  // NOLINT[whitespace/line_length]
      if (NULL != m_mPersist_Type[f_epersisttype]) {  // LCOV_EXCL_BR_LINE 6: m_mPersist_Type[f_epersisttype] can't be null  // NOLINT[whitespace/line_length]
        // set the persist type
        l_estatus = m_mPersist_Type[f_epersisttype]->SetPersistentCategory(f_crequestorname,
                                                                           f_ctag,
                                                                           f_epersistcategory);
      } else {
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Persistence object for %d is null.", f_epersisttype);  // LCOV_EXCL_LINE 6: m_mPersist_Type[f_epersisttype] can't be null  // NOLINT[whitespace/line_length]
      }
    } else {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Enum value %d is out of bound.", f_epersisttype);  // LCOV_EXCL_LINE 6: f_epersisttype is always bigger then ENOTIFICATIONPERSISTENTSERVICEPERSISTFIRST and less then ENOTIFICATIONPERSISTENTSERVICEPERSISTLAST  // NOLINT[whitespace/line_length]
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// PersistNORData
/// Sends the message to Immediate Persistence Thread to Persists the data immediately or reset
/// the NOR data depending on persist category flag during shutdown irrespective of delay.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CPersistenceManager::PersistNORData(EFrameworkunifiedShutdownType f_eshutdowntype, UI_32 f_uinotificationpersistentservicepersistcategoryflag) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

  TImmediateShutdown l_tImmShutdown = {};
  l_tImmShutdown.f_eshutdowntype = f_eshutdowntype;
  l_tImmShutdown.f_uinotificationpersistentservicepersistcategoryflag = f_uinotificationpersistentservicepersistcategoryflag;

  // send the message to NOR to persist or reset the data during shutdown
  if (NULL != CStateNorPersistenceNotification::m_hNSImmediatePersistenceThread) {    // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
    if (eFrameworkunifiedStatusOK != (l_estatus = McSend(CStateNorPersistenceNotification::m_hNSImmediatePersistenceThread, AppName, NOR_PERSISTENCE_ONSHUTDOWN, sizeof(l_tImmShutdown), &l_tImmShutdown))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "McSend for cmd 0x%X failed", NOR_PERSISTENCE_ONSHUTDOWN);   // LCOV_EXCL_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// SetImmediateDataPersistedStatus
/// Set/Reset the persistence status of immediate persistence data
///////////////////////////////////////////////////////////////////////////////////////////
VOID CPersistenceManager::SetImmediateDataPersistedStatus(BOOL f_bstatus) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  m_bImmediatedDataPersisted = f_bstatus;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

////////////////////////////////////////////////////////////////////////////////////////////
/// SetFilePersistedStatus
/// Set/Reset the persistence status of files and folders
///////////////////////////////////////////////////////////////////////////////////////////
VOID CPersistenceManager::SetFilePersistedStatus(BOOL f_bstatus) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  m_bAllFilePersisted = f_bstatus;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// ReadImmediateNotificationData
/// Get the list of all immediate persistent notifications of f_epersistcategory from a
/// persistent memory and store it in a map.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CPersistenceManager::ReadImmediateNotificationData(Persistent_Notification_List_Type
                                                              * &f_vpersistentnotificationlist,
                                                              const EFrameworkunifiedPersistCategory f_epersistcategory) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  std::string l_pcPath = IMMEDIATE_PERSISTENCE_STORAGE_V2;
  std::string l_cNotificationName = "";   // LCOV_EXCL_BR_LINE 11:except,C++ STL
  // stores the list of invalid immediate notification files at persistent location
  std::string l_cInvalidFileList = "";    // LCOV_EXCL_BR_LINE 11:except,C++ STL

  PCSTR l_pPublisherName = NULL;
  PSTR l_pData = NULL;
  UI_32 l_uiDataSize = 0;

  NC_NorPersistentData l_ptNorPersistentData = {};

  TFileList l_pTFileList;

  if ('/' != l_pcPath[l_pcPath.length() - 1]) {   // LCOV_EXCL_BR_LINE 6: the last char is always '/'
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_pcPath.append("/");   // LCOV_EXCL_LINE 6: the last char is always '/'
  }

  switch (f_epersistcategory) {
    case eFrameworkunifiedFactoryData: {
        l_pcPath.append(FACTORYDATADIR);  // LCOV_EXCL_BR_LINE 11:except,C++ STL
      }
      break;

    case eFrameworkunifiedFactoryCustomerData: {
        l_pcPath.append(FACTORYCUSTOMERDATADIR);  // LCOV_EXCL_BR_LINE 11:except,C++ STL
      }
      break;

    case eFrameworkunifiedDealerData: {
        l_pcPath.append(DEALERDATADIR);   // LCOV_EXCL_BR_LINE 11:except,C++ STL
      }
      break;

    case eFrameworkunifiedUserData:
    default: {
        l_pcPath.append(USERDATADIR);   // LCOV_EXCL_BR_LINE 11:except,C++ STL
        l_pcPath.append(ALLUSERAPPDATADIR);   // LCOV_EXCL_BR_LINE 11:except,C++ STL
      }
      break;
  }

  l_pcPath.append(IMMEDIATEDATADIR);    // LCOV_EXCL_BR_LINE 11:except,C++ STL

  if (eFrameworkunifiedStatusOK == GetFileList(&l_pTFileList, l_pcPath.c_str())) {   // LCOV_EXCL_BR_LINE 4: NSFW error case.
    if (!l_pTFileList.empty()) {  // LCOV_EXCL_BR_LINE 200: if GetFileList() returns eFrameworkunifiedStatusOK, l_pTFileList can't be empty  // NOLINT[whitespace/line_length]
      for (UI_32 l_uiCount = 0; l_uiCount < l_pTFileList.size(); l_uiCount++) {
        l_uiDataSize = 0;

        // get the persistent notification name from vector
        l_cNotificationName = l_pTFileList.at(l_uiCount);

        if (eFrameworkunifiedStatusOK == NPSynchronousGetPersistentDataSize(l_cNotificationName.c_str(), &l_uiDataSize,  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
                                                               f_epersistcategory) && (0 != l_uiDataSize)) {
          l_pData = new(std::nothrow) CHAR[l_uiDataSize];
          std::memset(l_pData, 0, l_uiDataSize);

          if (NULL != l_pData) {  // LCOV_EXCL_BR_LINE 6: l_pData can't be null
            l_pPublisherName = NULL;

            // clear the memory
            std::memset(&l_ptNorPersistentData, 0, sizeof(NC_NorPersistentData));

            // As NS_NPP does a recursive filename read from directory it has the complete filename.
            // So it does not require to pass the Publisher name i.e. l_pPublisherName.So it is being passed as NULL.
            if (eFrameworkunifiedStatusOK == NPSynchronousGetPersistentData(l_pPublisherName,  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
                                                               l_cNotificationName.c_str(),
                                                               (PVOID)l_pData,
                                                               l_uiDataSize,
                                                               l_ptNorPersistentData,
                                                               f_epersistcategory)) {
              CNotificationsToPersist *l_objCNotificationsToPersist = new(std::nothrow) CNotificationsToPersist();
              if (NULL != l_objCNotificationsToPersist) {  // LCOV_EXCL_BR_LINE 6: l_objCNotificationsToPersist can't be null  // NOLINT[whitespace/line_length]
                CPersistentData *l_objCPersistentData = new(std::nothrow) CPersistentData();

                // fill the appropriate values in  l_objCNotificationsToPersist
                if (NULL != l_objCPersistentData) {  // LCOV_EXCL_BR_LINE 6: l_objCPersistentData can't be null
                  l_objCPersistentData->m_pMessage = new(std::nothrow) CHAR[l_ptNorPersistentData.dataSize];
                  std::memset(l_objCPersistentData->m_pMessage, 0, l_ptNorPersistentData.dataSize);

                  if (NULL != l_objCPersistentData->m_pMessage) {  // LCOV_EXCL_BR_LINE 6: l_objCPersistentData->m_pMessage can't be null  // NOLINT[whitespace/line_length]
                    std::memcpy(l_objCPersistentData->m_pMessage, l_pData, l_ptNorPersistentData.dataSize);

                    l_objCNotificationsToPersist->m_uiMaxMsgLength = l_ptNorPersistentData.uiMaxSize;
                    l_objCPersistentData->m_uiMsgSize = l_ptNorPersistentData.dataSize;
                    l_objCNotificationsToPersist->m_ePersistentType = eFrameworkunifiedImmediatePersistedStateVar;

                    l_objCNotificationsToPersist->m_cNotificationName = l_ptNorPersistentData.notificationName;
                    l_objCNotificationsToPersist->m_cPublisherName = l_ptNorPersistentData.pPublisherName;
                    l_objCNotificationsToPersist->m_uiDelay = l_ptNorPersistentData.uiDelay;

                    l_objCNotificationsToPersist->m_pPersistentData = l_objCPersistentData;

                    l_objCNotificationsToPersist->m_ePersistCategory = f_epersistcategory;

                    f_vpersistentnotificationlist->push_back(l_objCNotificationsToPersist);
                  } else {
                    // LCOV_EXCL_START 6: l_objCPersistentData->m_pMessage can't be null
                    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Memory allocation error for notification data of size %d",
                           l_ptNorPersistentData.dataSize);

                    if (NULL != l_objCNotificationsToPersist) {
                      delete l_objCNotificationsToPersist;
                      l_objCNotificationsToPersist = NULL;
                    }

                    if (NULL != l_objCPersistentData) {
                      delete l_objCPersistentData;
                      l_objCPersistentData = NULL;
                    }
                    // LCOV_EXCL_STOP
                  }
                } else {
                  // LCOV_EXCL_START 6: l_objCPersistentData can't be null
                  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                  FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Memory allocation error for CPersistentData object");

                  if (NULL != l_objCNotificationsToPersist) {
                    delete l_objCNotificationsToPersist;
                    l_objCNotificationsToPersist = NULL;
                  }
                  // LCOV_EXCL_STOP
                }
              } else {
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Memory allocation error for CNotificationsToPersist object");  // LCOV_EXCL_LINE 6: l_objCNotificationsToPersist can't be null  // NOLINT[whitespace/line_length]
              }
            } else {
              // LCOV_EXCL_START 4: NSFW error case.
              AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
              l_cInvalidFileList.append(l_cNotificationName);
              l_cInvalidFileList.append(",");
              // LCOV_EXCL_STOP
            }

            delete[] l_pData;
            l_pData = NULL;
          } else {
            // LCOV_EXCL_START 6: l_pData can't be null
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Persistent data received as NULL from immediate notification file :: %s",
                   l_cNotificationName.c_str());
            // LCOV_EXCL_STOP
          }
        } else {
          // LCOV_EXCL_START 4: NSFW error case.
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          l_cInvalidFileList.append(l_cNotificationName);
          l_cInvalidFileList.append(",");
          // LCOV_EXCL_STOP
        }
      }

      if (0 != l_cInvalidFileList.size()) {  // LCOV_EXCL_START 200: l_cInvalidFileList's size must be 0
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__,
               "Unable to get persistent data for following immediate notification files :: %s",
               l_cInvalidFileList.c_str());
      }
      // LCOV_EXCL_STOP
    } else {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "No Immediate persistence data available at :: %s", l_pcPath.c_str());  // LCOV_EXCL_LINE 200: if GetFileList() returns eFrameworkunifiedStatusOK, l_pTFileList can't be empty  // NOLINT[whitespace/line_length]
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Unable to get the list of files from Immediate persistent location :: %s",
           l_pcPath.c_str());
    l_estatus = eFrameworkunifiedStatusFail;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

#ifdef NPP_PROFILEINFO_ENABLE

EFrameworkunifiedStatus CPersistenceManager::GetPersistenceProfilingData(std::string &f_cpersistenceprofileinfo) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

  f_cpersistenceprofileinfo.append("Application Name,");
  f_cpersistenceprofileinfo.append("Tag Name,");
  f_cpersistenceprofileinfo.append("Type,");
  f_cpersistenceprofileinfo.append("UserSpecific,");
  f_cpersistenceprofileinfo.append("Released,");
  f_cpersistenceprofileinfo.append("Persisted");

  CPersistence *l_pPersistence = m_mPersist_Type[ENOTIFICATIONPERSISTENTSERVICEPERSISTFILE];
  l_pPersistence->GetPersistenceProfilingData(f_cpersistenceprofileinfo);

  l_pPersistence = m_mPersist_Type[ENOTIFICATIONPERSISTENTSERVICEPERSISTFOLDER];
  l_pPersistence->GetPersistenceProfilingData(f_cpersistenceprofileinfo);

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

#endif
