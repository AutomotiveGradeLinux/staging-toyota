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
#include <string>
#include "ns_npp_notificationpersistentservicelog.h"
#include "ns_npp_registry_entry.h"

////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor
////////////////////////////////////////////////////////////////////////////////////////////
CRegistryEntry::CRegistryEntry(const std::string &f_ctag, const std::string &f_crequester,
                               const std::string &f_cstoragepath,
                               const BOOL f_bisuserpersistence):
  m_eJobState(ENOTIFICATIONPERSISTENTSERVICEPERSISTJObSTATEIDLE),
  m_cTag(f_ctag),
  m_cRequestor(f_crequester),
  m_bIsReleased(FALSE),
  m_bIsPersisted(FALSE),
  m_bIsUserPersistence(f_bisuserpersistence),
  m_eCurrentAction(LOADTYPE_NONE),
  m_ePersistType(ENOTIFICATIONPERSISTENTSERVICEPERSISTNONE),
  m_ePersistCategory(eFrameworkunifiedUserData) {
  m_cBasePath.clear();

  // set base path.
  m_cBasePath += f_cstoragepath;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// Copy Constructor
////////////////////////////////////////////////////////////////////////////////////////////
CRegistryEntry::CRegistryEntry(const CRegistryEntry &in):
  m_eJobState(ENOTIFICATIONPERSISTENTSERVICEPERSISTJObSTATEIDLE),
  m_cTag(in.m_cTag),
  m_cRequestor(in.m_cRequestor),
  m_bIsReleased(in.m_bIsReleased),
  m_bIsPersisted(in.m_bIsPersisted),
  m_cPersistPath(in.m_cPersistPath),
  m_cBasePath(in.m_cBasePath),
  m_bIsUserPersistence(in.m_bIsUserPersistence),
  m_eCurrentAction(in.m_eCurrentAction),
  m_ePersistType(in.m_ePersistType),
  m_ePersistCategory(in.m_ePersistCategory) {
}

////////////////////////////////////////////////////////////////////////////////////////////
/// Operator overload
///
////////////////////////////////////////////////////////////////////////////////////////////
CRegistryEntry &CRegistryEntry::operator=(const CRegistryEntry &in) {   // LCOV_EXCL_START 8: never be used
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  if (this != &in) {
    m_eJobState = in.m_eJobState;
    m_cTag = in.m_cTag;
    m_cRequestor = in.m_cRequestor;
    m_bIsReleased = in.m_bIsReleased;
    m_bIsPersisted = in.m_bIsPersisted;
    m_cPersistPath = in.m_cPersistPath;
    m_cBasePath = in.m_cBasePath;
    m_bIsUserPersistence = in.m_bIsUserPersistence;
    m_ePersistType = in.m_ePersistType;
  }
  return *this;
}
// LCOV_EXCL_STOP

////////////////////////////////////////////////////////////////////////////////////////////
/// SetReleasePath
/// Set release path.
////////////////////////////////////////////////////////////////////////////////////////////
VOID CRegistryEntry::SetReleasePath(const std::string &f_creleasepath) {
  m_bIsReleased = TRUE;
  m_cReleasePath = f_creleasepath;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// SetBeingPersisted
/// Set the file/folder is being persisted.
////////////////////////////////////////////////////////////////////////////////////////////
VOID CRegistryEntry::SetBeingPersisted() {
  if (LOADTYPE_RELEASE == m_eCurrentAction) {
    m_bIsPersisted = TRUE;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////
/// IsReleased
/// File/folder released or not.
////////////////////////////////////////////////////////////////////////////////////////////
BOOL CRegistryEntry::IsReleased() const {
  return m_bIsReleased;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// HasntBeenPersisted
/// File/Folder persisted or not
////////////////////////////////////////////////////////////////////////////////////////////
BOOL CRegistryEntry::HasntBeenPersisted() const {
  return !m_bIsPersisted;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// IsPersisted
/// File persisted or not
////////////////////////////////////////////////////////////////////////////////////////////
BOOL CRegistryEntry::IsPersisted() const {
  return m_bIsPersisted;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// GetPersistPath
/// Get the persist path.
////////////////////////////////////////////////////////////////////////////////////////////
std::string CRegistryEntry::GetPersistPath() const {
  return m_cPersistPath;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// GetReleasePath
/// Get the release path.
////////////////////////////////////////////////////////////////////////////////////////////
std::string CRegistryEntry::GetReleasePath() const {
  return m_cReleasePath;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// GetLoadPath
/// Get the load path.
////////////////////////////////////////////////////////////////////////////////////////////
std::string CRegistryEntry::GetLoadPath(ENotificationpersistentservicePersistType f_epersisttype, std::string f_cusername) const {
  std::string l_cLoadPath = "";

  if (f_epersisttype > ENOTIFICATIONPERSISTENTSERVICEPERSISTFIRST && f_epersisttype < ENOTIFICATIONPERSISTENTSERVICEPERSISTLAST) {  // LCOV_EXCL_BR_LINE 6: f_epersisttype must be in range.  // NOLINT[whitespace/line_length]
    l_cLoadPath = m_cBasePath;

    switch (m_ePersistCategory) {
      case eFrameworkunifiedUserData: {
          l_cLoadPath += USERDATADIR;

          // Check if to persist for user
          if (m_bIsUserPersistence) {
            //  if its a user file, user name can't be empty
            if (!f_cusername.empty()) {   // LCOV_EXCL_BR_LINE 6: f_cusername can't be empty
              FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Tag %s is registered for user %s", m_cTag.c_str(), f_cusername.c_str());
              l_cLoadPath += f_cusername;
              l_cLoadPath += "/";
            }
          } else {
            l_cLoadPath += ALLUSERAPPDATADIR;
          }

          break;
        }
      case eFrameworkunifiedFactoryData: {
          l_cLoadPath += FACTORYDATADIR;
          break;
        }
      case eFrameworkunifiedFactoryCustomerData: {
          l_cLoadPath += FACTORYCUSTOMERDATADIR;
          break;
        }
      case eFrameworkunifiedDealerData: {
          l_cLoadPath += DEALERDATADIR;
          break;
        }
    }

    l_cLoadPath += m_cRequestor;
    l_cLoadPath += "/";

    // file and folder persistency has different storage paths mapped
    if (ENOTIFICATIONPERSISTENTSERVICEPERSISTFILE == f_epersisttype) {
      l_cLoadPath += "NSFile/";
    } else if (ENOTIFICATIONPERSISTENTSERVICEPERSISTFOLDER == f_epersisttype) {  // LCOV_EXCL_BR_LINE 6: f_epersisttype must be ENOTIFICATIONPERSISTENTSERVICEPERSISTFILE or ENOTIFICATIONPERSISTENTSERVICEPERSISTFOLDER.  // NOLINT[whitespace/line_length]
      l_cLoadPath += "NSFolder/";
    }

    l_cLoadPath += m_cTag;
  }

  return l_cLoadPath;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// GetRequester
/// Get the name of requester.
////////////////////////////////////////////////////////////////////////////////////////////
std::string CRegistryEntry::GetRequester() const {
  return m_cRequestor;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// GetTag
/// Get the tag.
////////////////////////////////////////////////////////////////////////////////////////////
std::string CRegistryEntry::GetTag() const {
  return m_cTag;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// IsUserPersistence
///
////////////////////////////////////////////////////////////////////////////////////////////
BOOL CRegistryEntry::IsUserPersistence() {
  return m_bIsUserPersistence;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// SetCurrentAction
///
////////////////////////////////////////////////////////////////////////////////////////////
VOID CRegistryEntry::SetCurrentAction(ENPS_Loadtype f_ecurrentaction) {
  m_eCurrentAction = f_ecurrentaction;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// ResetPersistedFlag
///
////////////////////////////////////////////////////////////////////////////////////////////
VOID CRegistryEntry::ResetPersistedFlag() {
  m_bIsPersisted = FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// SetPersistProperties
///
////////////////////////////////////////////////////////////////////////////////////////////
VOID CRegistryEntry::SetPersistProperties(ENotificationpersistentservicePersistType f_epersisttype, std::string f_cusername) {
  if (f_epersisttype > ENOTIFICATIONPERSISTENTSERVICEPERSISTFIRST && f_epersisttype < ENOTIFICATIONPERSISTENTSERVICEPERSISTLAST) {  // LCOV_EXCL_BR_LINE 6: f_epersisttype must be in range.  // NOLINT[whitespace/line_length]
    // set the persist type
    m_ePersistType = f_epersisttype;

    // set the persist file based on the persist type and username (if a user file)
    m_cPersistPath = m_cBasePath;

    switch (m_ePersistCategory) {
      case eFrameworkunifiedUserData: {
          m_cPersistPath += USERDATADIR;

          // Check if to persist for user
          if (m_bIsUserPersistence) {
            //  if its a user file, user name can't be empty
            if (!f_cusername.empty()) {  // LCOV_EXCL_BR_LINE 6: f_cusername can't be empty
              FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Tag %s is registered for user %s", m_cTag.c_str(), f_cusername.c_str());
              m_cPersistPath += f_cusername;
              m_cPersistPath += "/";
            }
          } else {
            m_cPersistPath += ALLUSERAPPDATADIR;
          }

          break;
        }
      case eFrameworkunifiedFactoryData: {
          m_cPersistPath += FACTORYDATADIR;
          break;
        }
      case eFrameworkunifiedFactoryCustomerData: {
          m_cPersistPath += FACTORYCUSTOMERDATADIR;
          break;
        }
      case eFrameworkunifiedDealerData: {
          m_cPersistPath += DEALERDATADIR;
          break;
        }
    }

    m_cPersistPath += m_cRequestor;
    m_cPersistPath += "/";

    // file and folder persistency has different storage paths mapped
    if (ENOTIFICATIONPERSISTENTSERVICEPERSISTFILE == f_epersisttype) {
      m_cPersistPath += "NSFile/";
    } else if (ENOTIFICATIONPERSISTENTSERVICEPERSISTFOLDER == f_epersisttype) {  // LCOV_EXCL_BR_LINE 6: f_epersisttype must be ENOTIFICATIONPERSISTENTSERVICEPERSISTFILE or ENOTIFICATIONPERSISTENTSERVICEPERSISTFOLDER.  // NOLINT[whitespace/line_length]
      m_cPersistPath += "NSFolder/";
    }

    m_cPersistPath += m_cTag;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////
/// GetPersistType
///
////////////////////////////////////////////////////////////////////////////////////////////
ENotificationpersistentservicePersistType CRegistryEntry::GetPersistType() const {
  return m_ePersistType;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// SetPersistentCategory
/// Sets the persistent type related to tag
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CRegistryEntry::SetPersistentCategory(EFrameworkunifiedPersistCategory f_epersistcategory) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

  m_ePersistCategory = f_epersistcategory;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// GetPersistentCategory
/// Gets the persistent type of a tag
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedPersistCategory CRegistryEntry::GetPersistentCategory() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return m_ePersistCategory;
}
