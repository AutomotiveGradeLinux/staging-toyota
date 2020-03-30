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
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <unistd.h>
#include <native_service/frameworkunified_multithreading.h>
#include <string>
#include "ns_npp_persistence.h"
#include "ns_npp_persist_folder.h"
#include "ns_npp_registry_entry.h"

// Constructor of class CFolderPersistence
CFolderPersistence::CFolderPersistence() {
}

// Destructor of class CFolderPersistence
CFolderPersistence::~CFolderPersistence() {   // LCOV_EXCL_START 14: Resident process, global instance not released
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
}
// LCOV_EXCL_STOP

// Method to release folder for persistence.
EFrameworkunifiedStatus CFolderPersistence::Release(std::string f_crequesterappname,
                                       std::string f_ctag,
                                       std::string f_cmempath,
                                       EFrameworkunifiedReleaseType enotificationpersistentservicereleasetype,
                                       std::string f_cusername) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (f_cmempath.empty() || f_crequesterappname.empty() || f_ctag.empty()) {  // LCOV_EXCL_BR_LINE 6: double check, f_cmempath, f_crequesterappname and f_ctag can't be empty  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 6: f_ctag, f_crequesterappname and f_cmempath can't be empty
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Invalid folder path, requester or tag.");
    l_estatus = eFrameworkunifiedStatusInvldParam;
    // LCOV_EXCL_STOP
  } else {
    // Call to method requesting to release the folder for persistence.
    l_estatus = ProcessReleaseRequest(f_crequesterappname, f_ctag, ENOTIFICATIONPERSISTENTSERVICEPERSISTFOLDER, f_cmempath, enotificationpersistentservicereleasetype,
                                      f_cusername);
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "- l_estatus:0x%x", l_estatus);
  return l_estatus;
}

// Method to load folder from persistent memory.
EFrameworkunifiedStatus CFolderPersistence::Load(std::string f_crequesterappname,
                                    std::string f_ctag,
                                    std::string f_cretrievepath,
                                    std::string f_cusername) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");


  if (!f_ctag.empty() && !f_crequesterappname.empty() && !f_cretrievepath.empty()) {  // LCOV_EXCL_BR_LINE 6: double check, f_cretrievepath, f_crequesterappname and f_ctag can't be empty  // NOLINT[whitespace/line_length]
    // Call to method requesting to load the folder from persistent memory.
    l_estatus = ProcessLoadRequest(f_crequesterappname, f_ctag, ENOTIFICATIONPERSISTENTSERVICEPERSISTFOLDER, f_cretrievepath, f_cusername);
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
