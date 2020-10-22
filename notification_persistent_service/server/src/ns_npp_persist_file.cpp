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
/// \brief    The file contains definition of CFilePersistence class.
///           This class is responsible for persisting and retrieving of files.
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <unistd.h>
#include <string>
#include "ns_npp_notificationpersistentservicelog.h"
#include "ns_npp_copy_worker.h"
#include "ns_npp_persist_file.h"
#include "ns_npp_registry_entry.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// CFilePersistence
/// Constructor of CFilePersistence class
////////////////////////////////////////////////////////////////////////////////////////////////////
CFilePersistence::CFilePersistence() {
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// CFilePersistence
/// Destructor of CFilePersistence class
////////////////////////////////////////////////////////////////////////////////////////////////////
CFilePersistence::~CFilePersistence() {   // LCOV_EXCL_START 14: Resident process, global instance not released
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
}
// LCOV_EXCL_STOP

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Release
/// Entry for the file is stored in map for persistence.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFilePersistence::Release(std::string f_crequesterappname,
                                     std::string f_ctag,
                                     std::string f_cmempath,
                                     EFrameworkunifiedReleaseType enotificationpersistentservicereleasetype,
                                     std::string f_cusername) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (f_cmempath.empty() || f_crequesterappname.empty() || f_ctag.empty()) {  // LCOV_EXCL_BR_LINE 6: double check, f_cmempath, f_crequesterappname and f_ctag can't be empty
    // LCOV_EXCL_START 6: f_cmempath, f_crequesterappname and f_ctag can't be empty
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Invalid file path, requester or tag.");
    l_estatus = eFrameworkunifiedStatusInvldParam;
    // LCOV_EXCL_STOP
  } else {
    l_estatus = ProcessReleaseRequest(f_crequesterappname, f_ctag, ENOTIFICATIONPERSISTENTSERVICEPERSISTFILE, f_cmempath, enotificationpersistentservicereleasetype,
                                      f_cusername);  // LCOV_EXCL_BR_LINE 11: unexpected branch
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "- l_estatus:0x%x", l_estatus);
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Load
/// Load file from persistent memory to the specified location.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFilePersistence::Load(std::string f_crequesterappname,
                                  std::string f_ctag,
                                  std::string f_cretrievepath,
                                  std::string f_cusername) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");


  if (!f_ctag.empty() && !f_crequesterappname.empty() && !f_cretrievepath.empty()) {  // LCOV_EXCL_BR_LINE 6: double check, f_ctag, f_crequesterappname and f_cretrievepath can't be empty
    l_estatus = ProcessLoadRequest(f_crequesterappname, f_ctag, ENOTIFICATIONPERSISTENTSERVICEPERSISTFILE, f_cretrievepath, f_cusername);  // LCOV_EXCL_BR_LINE 11: unexpected branch
  } else {
    // LCOV_EXCL_START 6: f_ctag, f_crequesterappname and f_cretrievepath can't be empty
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Invalid argument passed (RetrievePath:%s ,tag:%s, requester:%s)",
           f_cretrievepath.c_str(), f_ctag.c_str(), f_crequesterappname.c_str());

    l_estatus = eFrameworkunifiedStatusInvldParam;
    // LCOV_EXCL_STOP
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}
