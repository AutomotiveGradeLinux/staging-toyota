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
/// \defgroup <<Group Tag>> <<Group Name>>
/// \ingroup  tag_NS_NPPService
/// .
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NS_NPPService
/// \brief    This file contains implementation of class CPersistence.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef AGL_STUB
#include <other_service/strlcpy.h>
// #include "frameworkunified_stub.h"
#endif
#include <unistd.h>
#include <stdlib.h>  // for getenv()
#ifdef AGL_STUB
#include <cstdio>
#endif
#include <utility>
#include <string>
#include "ns_npp_persistence.h"
#include "ns_npp_registry_entry.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// CPersistence
/// Constructor of CPersistence class
////////////////////////////////////////////////////////////////////////////////////////////////////
CPersistence::CPersistence():
  m_cStoragePath(GetStoragePath()),
  m_eCompressionType(ENOTIFICATIONPERSISTENTSERVICEDEFAULTCOMPRESSION),
  m_hNSWriteToPersistentMem(NULL),
  m_hNSReadFromPersistentMem(NULL),
  m_hAppHandle(NULL),
  m_bPersist(FALSE),
  m_uiNotificationpersistentservicePersistCategoryFlag(0) {
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// ~CPersistence
/// Destructor of CPersistence class
////////////////////////////////////////////////////////////////////////////////////////////////////
CPersistence::~CPersistence() {  // LCOV_EXCL_START 14: Resident process, global instance not released
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
}
// LCOV_EXCL_STOP

////////////////////////////////////////////////////////////////////////////////////////////////////
/// SetReadThreadHandle
/// Set read thread handle.
////////////////////////////////////////////////////////////////////////////////////////////////////
VOID CPersistence::SetReadThreadHandle(HANDLE f_hreadthread) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  m_hNSReadFromPersistentMem = f_hreadthread;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// SetWriteThreadHandle
/// Set write thread handle.
////////////////////////////////////////////////////////////////////////////////////////////////////
VOID CPersistence::SetWriteThreadHandle(HANDLE f_hwritethread) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  m_hNSWriteToPersistentMem = f_hwritethread;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// GetStoragePath
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::string CPersistence::GetStoragePath() {
  std::string l_cStoragePath(STORAGE_PATH);

  return l_cStoragePath;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Register
/// Register tag for persistence.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CPersistence::Register(std::string f_crequestorappname, std::string f_ctag, BOOL bisuserpersistence) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  if (f_ctag.empty() || f_crequestorappname.empty()) {  // LCOV_EXCL_BR_LINE 6: f_ctag and f_crequestorappname can't be empty  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 6: f_ctag and f_crequestorappname can't be empty
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Invalid tag or requester.");
    l_estatus = eFrameworkunifiedStatusFail;
    // LCOV_EXCL_STOP
  } else {
    TSourceRegistryListItr l_itRegistry = m_mPersistRegistry.find(f_crequestorappname);
    if (l_itRegistry != m_mPersistRegistry.end()) {
      // source available, check for file/folder availability
      TTagRegistryListItr l_itRegistryList = (l_itRegistry->second).find(f_ctag);
      if (l_itRegistryList != (l_itRegistry->second).end()) {
        // found already in the registry
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Already exists %s , %s", f_ctag.c_str(), f_crequestorappname.c_str());  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
      } else {
        // tag not found .. so add to the registry
        CRegistryEntry l_objRegistryEntry(f_ctag, f_crequestorappname, m_cStoragePath,
                                          bisuserpersistence);  // f_eRegisterType, f_cUser);

        (l_itRegistry->second).insert(make_pair(f_ctag, l_objRegistryEntry));
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " %s found adding tag %s", f_crequestorappname.c_str(), f_ctag.c_str());
      }
    } else {
      // source not found, so creating a new entry
      CRegistryEntry l_objRegistryEntry(f_ctag, f_crequestorappname, m_cStoragePath,
                                        bisuserpersistence);  // f_eRegisterType, f_cUser);

      TTagRegistryList l_mRegList;
      l_mRegList.insert(make_pair(f_ctag, l_objRegistryEntry));   // LCOV_EXCL_BR_LINE 11:except,C++ STL

      m_mPersistRegistry.insert(std::make_pair(f_crequestorappname, l_mRegList));   // LCOV_EXCL_BR_LINE 11:except,C++ STL  // NOLINT[whitespace/line_length]

      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "none exists %s , %s. So, added entry.",  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
             f_ctag.c_str(), f_crequestorappname.c_str());  // LCOV_EXCL_BR_LINE 11: unexpected branch
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "- l_estatus:0x%x", l_estatus);

  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// ProcessReleaseRequest
/// Persist file/folder that need to be persisted.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CPersistence::ProcessReleaseRequest(std::string f_crequesterappname,
                                               std::string f_ctag,
                                               ENotificationpersistentservicePersistType f_epersisttype,
                                               std::string f_cmempath,
                                               EFrameworkunifiedReleaseType enotificationpersistentservicereleasetype,
                                               std::string f_cusername) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

  if (f_cmempath.empty() || f_crequesterappname.empty() || f_ctag.empty()) {    // LCOV_EXCL_BR_LINE 6:f_cmempath, f_crequesterappname and f_ctag can`t be empty  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 6: f_cmempath, f_crequesterappname and f_ctag can`t be empty
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG0(ZONE_ERR, __FUNCTION__, "Invalid file path, requester or tag.");
    l_estatus = eFrameworkunifiedStatusInvldParam;
    // LCOV_EXCL_STOP
  } else {
    // Persistence Registry map iterator
    TSourceRegistryListItr l_itTSourceRegistryListItr = m_mPersistRegistry.find(f_crequesterappname);

    if (l_itTSourceRegistryListItr != m_mPersistRegistry.end()) {
      // source available, check for file if registered
      TTagRegistryListItr l_itTRegistryList = (l_itTSourceRegistryListItr->second).find(f_ctag);
      if (l_itTRegistryList != (l_itTSourceRegistryListItr->second).end()) {
        if (eFrameworkunifiedNotOnRelease != enotificationpersistentservicereleasetype) {
          // Set release path
          l_itTRegistryList->second.SetReleasePath(f_cmempath);
          // Set persist path
          l_itTRegistryList->second.SetPersistProperties(f_epersisttype, f_cusername);  // LCOV_EXCL_BR_LINE 11:except,C++ STL  // NOLINT[whitespace/line_length]

          // file/folder found in registry
          FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
                 "%s released %s. Found in registry. queued up to persist from %s",
                 f_crequesterappname.c_str(), f_ctag.c_str(), l_itTRegistryList->second.GetReleasePath().c_str());

          // check if persist flag is set due to shutdown or user change
          if ((m_bPersist     // It will be set to TRUE either on shutdown or userchange
               && l_itTRegistryList->second.HasntBeenPersisted())  // File/Folder persisted or not
              || (eFrameworkunifiedPersistInstantly == enotificationpersistentservicereleasetype)) {
            // reset the data
            if (((eFrameworkunifiedUserData             == l_itTRegistryList->second.GetPersistentCategory()) &&
                 (eFrameworkunifiedUserData & m_uiNotificationpersistentservicePersistCategoryFlag)) ||
                ((eFrameworkunifiedFactoryData          == l_itTRegistryList->second.GetPersistentCategory()) &&
                 (eFrameworkunifiedFactoryData & m_uiNotificationpersistentservicePersistCategoryFlag)) ||
                ((eFrameworkunifiedFactoryCustomerData  == l_itTRegistryList->second.GetPersistentCategory()) &&
                 (eFrameworkunifiedFactoryCustomerData & m_uiNotificationpersistentservicePersistCategoryFlag)) ||
                ((eFrameworkunifiedDealerData           == l_itTRegistryList->second.GetPersistentCategory()) &&
                 (eFrameworkunifiedDealerData & m_uiNotificationpersistentservicePersistCategoryFlag))) {
              // set the status of file/folder persisted as reset has been called on these data
              l_itTRegistryList->second.SetCurrentAction(LOADTYPE_RELEASE);
              l_itTRegistryList->second.SetBeingPersisted();
            } else {  // persist the data
              if (eFrameworkunifiedStatusOK != (l_estatus = Persist(static_cast<CRegistryEntry &>(l_itTRegistryList->second)))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                l_estatus = eFrameworkunifiedStatusFail;   // LCOV_EXCL_LINE 4: NSFW error case.
              }
            }
          } else {
            // Don't persist now. Persist on shutdown.
          }
        } else {  // not on release
          // set the status of file/folder as released and persisted as these data need not to be persisted on shutdown
          l_itTRegistryList->second.SetReleasePath("");
          l_itTRegistryList->second.SetCurrentAction(LOADTYPE_RELEASE);
          l_itTRegistryList->second.SetBeingPersisted();
        }
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Warning: %s didn't register %s, not persisting",
               f_crequesterappname.c_str(), f_ctag.c_str());  // LCOV_EXCL_BR_LINE 11: unexpected branch
        l_estatus = eFrameworkunifiedStatusInvldParam;
      }
    } else {
      // source not registered, cannot persist
      FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Warning: %s is not registered ignoring release of %s",
             f_ctag.c_str(), f_crequesterappname.c_str());
      l_estatus = eFrameworkunifiedStatusInvldParam;
    }
  }
  return l_estatus;
}

EFrameworkunifiedStatus CPersistence::Persist(CRegistryEntry &f_objregistryentry) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

  // Persistence Info Structure
  NSP_CopyInfoCmd l_tCpInfo = {};


  std::string l_cSourcePath = f_objregistryentry.GetReleasePath();
  std::string l_cPersistPath = f_objregistryentry.GetPersistPath();   // LCOV_EXCL_BR_LINE 11:except,C++ STL

  // Fill the persistence info structure
  AddRequestData(l_tCpInfo,
                 l_cSourcePath,
                 l_cPersistPath,
                 f_objregistryentry.GetRequester(),
                 f_objregistryentry.GetTag(),
                 LOADTYPE_RELEASE,
                 f_objregistryentry.GetPersistType());

  // Persist only if file was released.
  // Release means application has asked NPPService to persist file at shutdown.)
  // Persist means actual persisting a file at persistent storage
  if (f_objregistryentry.IsReleased()) {
    if (ENOTIFICATIONPERSISTENTSERVICEPERSISTJObSTATEINPROCESS == f_objregistryentry.m_eJobState) {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Another job with source: %s and tag %s is in process. "
             "Therefore adding release request to the pending queue.",
             f_objregistryentry.GetRequester().c_str(), f_objregistryentry.GetTag().c_str());
      // if job corresponding to the tag is already in process, add it in pending queue
      m_lPendingJobs.push_back(l_tCpInfo);
    } else if (ENOTIFICATIONPERSISTENTSERVICEPERSISTJObSTATEIDLE == f_objregistryentry.m_eJobState) {
      if (eFrameworkunifiedStatusOK == (SendRequestMessage(l_tCpInfo))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Release message passed to writer thread.");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
        f_objregistryentry.SetCurrentAction(LOADTYPE_RELEASE);
        f_objregistryentry.m_eJobState = ENOTIFICATIONPERSISTENTSERVICEPERSISTJObSTATEINPROCESS;
      } else {
        // LCOV_EXCL_START 4: NSFW error case.
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failure in passing release message to writer thread.");
        l_estatus = eFrameworkunifiedStatusFail;
        // LCOV_EXCL_STOP
      }
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "File/Folder not released yet.");
  }

  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// ProcessLoadRequest
/// Load persisted file/folder.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CPersistence::ProcessLoadRequest(std::string f_crequesterappname,
                                            std::string f_ctag,
                                            ENotificationpersistentservicePersistType f_epersisttype,
                                            std::string f_cretrievepath,
                                            std::string f_cusername) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

  // file/folder found in registry
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
         "Load request received: Requester: %s and Tag: %s Load at: %s.",
         f_crequesterappname.c_str(), f_ctag.c_str(), f_cretrievepath.c_str());  // LCOV_EXCL_BR_LINE 11:except,C++ STL

  if (f_ctag.empty() || f_crequesterappname.empty() || f_cretrievepath.empty()) {   // LCOV_EXCL_BR_LINE 6: both f_ctag and f_crequesterappname and f_cretrievepath can`t be empty  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 6: both f_ctag and f_crequesterappname and f_cretrievepath can`t be empty
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Invalid argument passed (RetrievePath:%s ,tag:%s, requester:%s)",
           f_cretrievepath.c_str(), f_ctag.c_str(), f_crequesterappname.c_str());
    l_estatus = eFrameworkunifiedStatusInvldParam;
    // LCOV_EXCL_STOP
  } else {
    // Persistence Registry Map Iterator
    TSourceRegistryListItr l_itTRegistry = m_mPersistRegistry.find(f_crequesterappname);

    // Persistence Info Struct
    NSP_CopyInfoCmd l_tCopyInfo = {};

    if (l_itTRegistry != m_mPersistRegistry.end()) {
      // source available, check for file/folder
      TTagRegistryListItr l_itTRegistryList = (l_itTRegistry->second).find(f_ctag);
      if (l_itTRegistryList != (l_itTRegistry->second).end()) {
        if ((ENOTIFICATIONPERSISTENTSERVICEPERSISTJObSTATERELEASEABORTED == l_itTRegistryList->second.m_eJobState)) {  // LCOV_EXCL_BR_LINE 6: m_eJobState can`t be ENOTIFICATIONPERSISTENTSERVICEPERSISTJObSTATERELEASEABORTED  // NOLINT[whitespace/line_length]
          // LCOV_EXCL_START 6: m_eJobState can`t be ENOTIFICATIONPERSISTENTSERVICEPERSISTJObSTATERELEASEABORTED  // NOLINT[whitespace/line_length]
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          std::string l_cReleasePath(l_itTRegistryList->second.GetReleasePath());
          // if job was aborted due to some reason like abort shutdown.
          // Then persistent storage doesn't have updated file/folder. Just restore
          // aborted released file.
          // just rename it
          if (0 == std::rename(l_cReleasePath.c_str(), f_cretrievepath.c_str())) {
            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "File loaded at requested location.");
          } else {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                   "Error while processing load request when tried to move. source: %s dest: %s",
                   l_cReleasePath.c_str(), f_cretrievepath.c_str());
            l_estatus = eFrameworkunifiedStatusFail;
          }
          l_itTRegistryList->second.m_eJobState = ENOTIFICATIONPERSISTENTSERVICEPERSISTJObSTATEIDLE;
          // LCOV_EXCL_STOP
        } else {
          std::string l_cLoadPath(l_itTRegistryList->second.GetLoadPath(f_epersisttype, f_cusername));  // LCOV_EXCL_BR_LINE 11:except,C++ STL  // NOLINT[whitespace/line_length]

          if (0 == access(l_cLoadPath.c_str(), R_OK)) {
            // file/folder found in registry
            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Found in registry. checking in persistence...(%s)", l_cLoadPath.c_str());

            AddRequestData(l_tCopyInfo,
                           l_cLoadPath,
                           f_cretrievepath,
                           f_crequesterappname,
                           f_ctag,
                           LOADTYPE_LOAD,
                           f_epersisttype);

            if (ENOTIFICATIONPERSISTENTSERVICEPERSISTJObSTATEINPROCESS == (l_itTRegistryList->second).m_eJobState) {
              FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Another job with source: %s and tag %s is in process. "
                     "Therefore adding load request to the pending queue.",
                     f_crequesterappname.c_str(), f_ctag.c_str());
              m_lPendingJobs.push_back(l_tCopyInfo);
            } else if (ENOTIFICATIONPERSISTENTSERVICEPERSISTJObSTATEIDLE == (l_itTRegistryList->second).m_eJobState) {
              FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Found in persistence.");
              if (eFrameworkunifiedStatusOK == (SendRequestMessage(l_tCopyInfo))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
                FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Load message passed to reader thread. Retrieving..");
                // file/folder is requested for loading so reset the persisted flag.
                l_itTRegistryList->second.ResetPersistedFlag();
                l_itTRegistryList->second.SetCurrentAction(LOADTYPE_LOAD);
                l_itTRegistryList->second.m_eJobState = ENOTIFICATIONPERSISTENTSERVICEPERSISTJObSTATEINPROCESS;
              } else {
                // LCOV_EXCL_START 4: NSFW error case.
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failure in passing load message to reader thread.");
                l_estatus = eFrameworkunifiedStatusFail;
                // LCOV_EXCL_STOP
              }
            }
          } else {
            FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "%s requested by %s. Not found in persistence memory.",  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
                   f_ctag.c_str(), f_crequesterappname.c_str());  // LCOV_EXCL_BR_LINE 11:except,C++ STL
            l_estatus = eFrameworkunifiedStatusAccessError;
          }
        }
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Warning: %s requesting %s .. Tag not found",
               f_crequesterappname.c_str(), f_ctag.c_str());  // LCOV_EXCL_BR_LINE 11:except,C++ STL
        l_estatus = eFrameworkunifiedStatusFail;
      }
    } else {
      // source not found
      FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Warning: %s requesting %s .. Requester not found",
             f_crequesterappname.c_str(), f_ctag.c_str());
      l_estatus = eFrameworkunifiedStatusFail;
    }
  }
  return l_estatus;
}

VOID CPersistence::AddRequestData(NSP_CopyInfoCmd &f_tcpinfo,
                                  std::string f_sourcepath,
                                  std::string f_destpath,
                                  std::string f_crequesterappname,
                                  std::string f_ctag,
                                  ENPS_Loadtype f_eloadtype,
                                  ENotificationpersistentservicePersistType f_epersisttype) {
#ifdef AGL_PosixBasedOS001LEGACY_USED
  strlcpy(f_tcpinfo.m_csourcepath, f_sourcepath.c_str(), sizeof(f_tcpinfo.m_csourcepath));
  strlcpy(f_tcpinfo.m_cdestinationpath, f_destpath.c_str(), sizeof(f_tcpinfo.m_cdestinationpath));
  strlcpy(f_tcpinfo.m_crequesterappname, f_crequesterappname.c_str(), sizeof(f_tcpinfo.m_crequesterappname));
  strlcpy(f_tcpinfo.m_cpersistenttag, f_ctag.c_str(), sizeof(f_tcpinfo.m_cpersistenttag));
#endif
  f_tcpinfo.m_eloadtype = f_eloadtype;
  f_tcpinfo.m_epersisttype = f_epersisttype;
  // f_tcpinfo.m_eCompressionType = ENOTIFICATIONPERSISTENTSERVICECOMPRESSUSINGLIBZ;
}

EFrameworkunifiedStatus CPersistence::SendRequestMessage(NSP_CopyInfoCmd &f_tcpinfo) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  HANDLE l_hThreadHandle = NULL;
  // Worker Command Protocol
  ENSP_CopyWorkerProtocol l_eWorkerProtocol = CP_WRK_CMD_COPY;

  if (ENOTIFICATIONPERSISTENTSERVICEPERSISTFILE == f_tcpinfo.m_epersisttype) {
    l_eWorkerProtocol = CP_WRK_CMD_COPY;
  } else if (ENOTIFICATIONPERSISTENTSERVICEPERSISTFOLDER == f_tcpinfo.m_epersisttype) {
    l_eWorkerProtocol = AR_CMD_ARCHIVE;
  }

  if (LOADTYPE_RELEASE == f_tcpinfo.m_eloadtype) {
    l_hThreadHandle = m_hNSWriteToPersistentMem;
  } else if (LOADTYPE_LOAD == f_tcpinfo.m_eloadtype) {
    l_hThreadHandle = m_hNSReadFromPersistentMem;
  }

  if (l_hThreadHandle) {
    // issue a copy to the worker thread
    if (eFrameworkunifiedStatusOK != (l_estatus = McSend(l_hThreadHandle, AppName, l_eWorkerProtocol, sizeof(NSP_CopyInfoCmd), &f_tcpinfo))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "McSend for thread failed %d", l_estatus);   // LCOV_EXCL_LINE 4: NSFW error case.
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// AckReceivedFromWorker
/// This is callback function for ack that file is released to persistenet memory.
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CPersistence::AckReceivedFromWorker(PCSTR f_csource, PCSTR f_ctag, BOOL f_bcopystatus,
                                               ENPS_Loadtype f_eloadtype) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus retVal = eFrameworkunifiedStatusOK;

  std::string l_cRequester(f_csource);
  std::string l_cTag(f_ctag);   // LCOV_EXCL_BR_LINE 11:except,C++ STL

  TSourceRegistryListItr l_itTReg = m_mPersistRegistry.find(l_cRequester);
  if (l_itTReg != m_mPersistRegistry.end()) {
    // source available, check for file
    TTagRegistryListItr l_itTRegList = (l_itTReg->second).find(l_cTag);
    if (l_itTRegList != (l_itTReg->second).end()) {
      if (LOADTYPE_RELEASE == f_eloadtype) {
        l_itTRegList->second.SetBeingPersisted();   // LCOV_EXCL_BR_LINE 11: unexpected branch

        if (f_bcopystatus) {
          FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "File persisted %s / %s", f_csource, f_ctag);
        } else {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
                 "File not persisted %s / %s, Negative ack received from copy worker thread",
                 f_csource,
                 f_ctag);  // LCOV_EXCL_BR_LINE 11:except,C++ STL
        }
      }

      // Job processed. Set job state as idle
      l_itTRegList->second.m_eJobState = ENOTIFICATIONPERSISTENTSERVICEPERSISTJObSTATEIDLE;

      // check if there are any pending jobs for corresponding source and tag
      if (!m_lPendingJobs.empty()) {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Retrieving next pending job for persistence...");
        TPendingJobsItr l_itrPendingJobs;
        for (l_itrPendingJobs = m_lPendingJobs.begin(); l_itrPendingJobs != m_lPendingJobs.end(); ++l_itrPendingJobs) {
          if (0 == std::strcmp((*l_itrPendingJobs).m_crequesterappname, f_csource)
              && (0 == std::strcmp((*l_itrPendingJobs).m_cpersistenttag, f_ctag))) {
            if (eFrameworkunifiedStatusOK == (retVal = (SendRequestMessage((*l_itrPendingJobs))))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
              // set job state as processing
              l_itTRegList->second.m_eJobState = ENOTIFICATIONPERSISTENTSERVICEPERSISTJObSTATEINPROCESS;
              // set current action
              l_itTRegList->second.SetCurrentAction((*l_itrPendingJobs).m_eloadtype);
              // Reset persisted flag. It requires to process release request for this tag.
              // Because, file will be persisted only once. Unless, anyone loads the file again.
              if (LOADTYPE_LOAD == (*l_itrPendingJobs).m_eloadtype) {
                l_itTRegList->second.ResetPersistedFlag();
              }

              // remove job from pending list
              m_lPendingJobs.erase(l_itrPendingJobs);
            } else {
              // LCOV_EXCL_START 4: NSFW error case.
              AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
              FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failure in passing load/release message to the worker threads.");
              retVal = eFrameworkunifiedStatusFail;
              // LCOV_EXCL_STOP
            }

            break;
          }
        }
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "No pending jobs");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
      }
    } else {
      // tag not found ..
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " ERROR %s, tag %s not found ", f_csource, f_ctag);
    }
  } else {
    // source not found
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "ERROR none exists %s , %s", f_csource, f_ctag);
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return retVal;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// PersistAllReleaseRequests
/// Persist all files which are not persisted yet.
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CPersistence::PersistAllReleaseRequests(UI_32 f_uinotificationpersistentservicepersistcategoryflag) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

  m_bPersist = TRUE;  // set to true because persist of all release requests have been triggered.
  // Trigger reason: shutdown

  m_uiNotificationpersistentservicePersistCategoryFlag = f_uinotificationpersistentservicepersistcategoryflag;

  TSourceRegistryListItr l_itTReg = m_mPersistRegistry.begin();

  for (; l_itTReg != m_mPersistRegistry.end(); ++l_itTReg) {
    TTagRegistryListItr l_itTRegList = (l_itTReg->second).begin();
    for (; l_itTRegList != (l_itTReg->second).end(); ++l_itTRegList) {
      if (l_itTRegList->second.HasntBeenPersisted()) {
        // reset the data
        if (((eFrameworkunifiedUserData       == l_itTRegList->second.GetPersistentCategory()) &&
             (eFrameworkunifiedUserData & f_uinotificationpersistentservicepersistcategoryflag)) ||
            ((eFrameworkunifiedFactoryData        == l_itTRegList->second.GetPersistentCategory()) &&
             (eFrameworkunifiedFactoryData & f_uinotificationpersistentservicepersistcategoryflag)) ||
            ((eFrameworkunifiedFactoryCustomerData  == l_itTRegList->second.GetPersistentCategory()) &&
             (eFrameworkunifiedFactoryCustomerData & f_uinotificationpersistentservicepersistcategoryflag)) ||
            ((eFrameworkunifiedDealerData         == l_itTRegList->second.GetPersistentCategory()) &&
             (eFrameworkunifiedDealerData & f_uinotificationpersistentservicepersistcategoryflag))) {
          // set the status of file/folder as released and persisted as these data need not to be persisted on shutdown
          l_itTRegList->second.SetCurrentAction(LOADTYPE_RELEASE);
          l_itTRegList->second.SetBeingPersisted();
        } else {  // persist the data
          if (eFrameworkunifiedStatusOK != Persist(static_cast<CRegistryEntry &>(l_itTRegList->second))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            l_estatus = eFrameworkunifiedStatusFail;  // LCOV_EXCL_LINE 4: NSFW error case.
          }
        }
      }
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

BOOL CPersistence::HaveAllReleaseRequestsPersisted(std::string &f_ctagnotreleased) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  BOOL l_bRetVal = TRUE;

  TSourceRegistryListItr l_itTReg = m_mPersistRegistry.begin();

  f_ctagnotreleased.assign("");

  for (; l_itTReg != m_mPersistRegistry.end(); ++l_itTReg) {
    TTagRegistryListItr l_itTRegList = (l_itTReg->second).begin();
    for (; l_itTRegList != (l_itTReg->second).end(); ++l_itTRegList) {
      if (l_itTRegList->second.HasntBeenPersisted()) {
        l_bRetVal = FALSE;

        if (!l_itTRegList->second.IsReleased()) {
          f_ctagnotreleased.append("\n");
          f_ctagnotreleased.append(l_itTRegList->second.GetTag());
          f_ctagnotreleased.append(" [");
          f_ctagnotreleased.append(l_itTRegList->second.GetRequester());
          f_ctagnotreleased.append("]");
        }
      }
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_bRetVal;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// PersistAllUserRequests
/// Persist all user files which are not persisted yet.
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CPersistence::PersistAllUserRequests() {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  // TODO(my_username): Persist data on userchange. Uncomment following.
  //  m_bPersist = TRUE;  // set to true because persist of all user release requests have been triggered.
  // Trigger reason: user change

  TSourceRegistryListItr l_itTReg = m_mPersistRegistry.begin();

  for (; l_itTReg != m_mPersistRegistry.end(); ++l_itTReg) {
    TTagRegistryListItr l_itTRegList = (l_itTReg->second).begin();
    for (; l_itTRegList != (l_itTReg->second).end(); ++l_itTRegList) {
      if (l_itTRegList->second.IsUserPersistence()) {
        if (eFrameworkunifiedStatusOK != Persist(static_cast<CRegistryEntry &>(l_itTRegList->second))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          l_estatus = eFrameworkunifiedStatusFail;  // LCOV_EXCL_LINE 4: NSFW error case.
        }
      }
    }
  }
  return l_estatus;
}

BOOL CPersistence::IsUserPersistence(std::string f_ctag) {
  BOOL l_bUserPersistence = FALSE;

  TSourceRegistryListItr l_itTReg = m_mPersistRegistry.begin();

  for (; l_itTReg != m_mPersistRegistry.end(); ++l_itTReg) {
    TTagRegistryListItr l_itTRegList = (l_itTReg->second).begin();
    for (; l_itTRegList != (l_itTReg->second).end(); ++l_itTRegList) {
      if (l_itTRegList->second.GetTag() == f_ctag) {
        if (l_itTRegList->second.IsUserPersistence()) {
          l_bUserPersistence =  TRUE;
        }
        break;
      }
    }
  }

  return l_bUserPersistence;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// SetPersistentCategory
/// Sets the persist type of file/folder
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CPersistence::SetPersistentCategory(const std::string &f_crequesterappname,
                                               const std::string &f_ctag,
                                               EFrameworkunifiedPersistCategory f_epersistcategory) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

  // Persistence Registry map iterator
  TSourceRegistryListItr l_itTSourceRegistryListItr = m_mPersistRegistry.find(f_crequesterappname);

  if (m_mPersistRegistry.end() != l_itTSourceRegistryListItr) {
    // source available, check for file if registered
    TTagRegistryListItr l_itTRegistryList = (l_itTSourceRegistryListItr->second).find(f_ctag);

    if ((l_itTSourceRegistryListItr->second).end() != l_itTRegistryList) {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "AppName:: %s, Tag:: %s found in registry", f_crequesterappname.c_str(),
             f_ctag.c_str());

      l_estatus = (l_itTRegistryList->second).SetPersistentCategory(f_epersistcategory);
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Warning: %s didn't register %s", f_crequesterappname.c_str(), f_ctag.c_str());
      l_estatus = eFrameworkunifiedStatusInvldParam;
    }
  } else {
    // source not registered, cannot persist
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Warning: %s is not registered", f_ctag.c_str());
    l_estatus = eFrameworkunifiedStatusInvldParam;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// ResetPersistFlag
/// Resets the persist flag.
////////////////////////////////////////////////////////////////////////////////////////////
VOID CPersistence::ResetPersistFlag() {
  m_bPersist = FALSE;

  m_uiNotificationpersistentservicePersistCategoryFlag = 0;
}

#ifdef NPP_PROFILEINFO_ENABLE

EFrameworkunifiedStatus CPersistence::GetPersistenceProfilingData(std::string &f_cpersistenceprofileinfo) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

  TSourceRegistryListItr l_itrTSourceRegistryListItr;
  TTagRegistryListItr l_itrTTagRegistryListItr;

  for (l_itrTSourceRegistryListItr =  m_mPersistRegistry.begin();
       l_itrTSourceRegistryListItr != m_mPersistRegistry.end();
       l_itrTSourceRegistryListItr++) {
    for (l_itrTTagRegistryListItr = (l_itrTSourceRegistryListItr->second).begin();
         l_itrTTagRegistryListItr != (l_itrTSourceRegistryListItr->second).end();
         l_itrTTagRegistryListItr++) {
      f_cpersistenceprofileinfo.append("\n");
      f_cpersistenceprofileinfo.append((*l_itrTSourceRegistryListItr).first);
      f_cpersistenceprofileinfo.append(",");

      f_cpersistenceprofileinfo.append(l_itrTTagRegistryListItr->first);
      f_cpersistenceprofileinfo.append(",");

      switch ((l_itrTTagRegistryListItr->second).GetPersistType()) {
        case ENOTIFICATIONPERSISTENTSERVICEPERSISTFILE: {
            f_cpersistenceprofileinfo.append("File,");
          }
          break;

        case ENOTIFICATIONPERSISTENTSERVICEPERSISTFOLDER: {
            f_cpersistenceprofileinfo.append("Folder,");
          }
          break;

        default: {
            f_cpersistenceprofileinfo.append(",");
          }
          break;
      }

      if ((l_itrTTagRegistryListItr->second).IsUserPersistence()) {
        f_cpersistenceprofileinfo.append("Yes,");
      } else {
        f_cpersistenceprofileinfo.append("No,");
      }

      if ((l_itrTTagRegistryListItr->second).IsReleased()) {
        f_cpersistenceprofileinfo.append("Yes,");
      } else {
        f_cpersistenceprofileinfo.append("No,");
      }

      if ((l_itrTTagRegistryListItr->second).IsPersisted()) {
        f_cpersistenceprofileinfo.append("Yes");
      } else {
        f_cpersistenceprofileinfo.append("No");
      }
    }
  }
  return l_estatus;
}

#endif
