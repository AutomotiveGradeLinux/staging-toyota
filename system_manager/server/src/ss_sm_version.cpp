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

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_SystemManager
/// \brief    This file provides support for module version management.
///
///////////////////////////////////////////////////////////////////////////////
#include <system_service/ss_package.h>
#include <system_service/ss_ver.h>
#include <system_service/ss_templates.h>
#include <system_service/ss_string_maps.h>
#include <system_service/ss_system_manager_protocol.h>
#include <system_service/ss_system_manager_protocol_local.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include "ss_sm_systemmanagerlog.h"
#include "ss_sm_version.h"

using namespace std;  // NOLINT

CSMVersion::CSMVersion() {
}

CSMVersion::~CSMVersion() {
}

EFrameworkunifiedStatus CSMVersion::get_version(SS_VersionNumberType &f_VersionNumber) noexcept {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  CSSVer ver;

  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  f_VersionNumber = 0;

  SSVER_PkgInfo info;
  l_eStatus = ver.getPkgInfo(SS_PKG_MAIN_EMMC, &info);
  if (l_eStatus == eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 6: To initialize conditions for launching SystemManager
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    f_VersionNumber = strtoull(info.version, NULL, 10);  // LCOV_EXCL_LINE 6: To initialize conditions for launching SystemManager
  } else {
    SS_ASERT(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus CSMVersion::get_version(SS_VersionNumberType &f_VersionNumber)

EFrameworkunifiedStatus CSMVersion::get_build_info(std::string &f_buildInfoStr) noexcept {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  std::string build_info_str;
  CSSVer ver;
  SSVER_PkgInfo info;
  l_eStatus = ver.getPkgInfo(SS_PKG_MAIN_PRODUCT_SI, &info);
  if (l_eStatus == eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 6: To initialize conditions for launching SystemManager
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    build_info_str += info.version;  // LCOV_EXCL_LINE 6: To initialize conditions for launching SystemManager
  } else {
    SS_ASERT(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  }
  f_buildInfoStr = build_info_str;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // LCOV_EXCL_BR_LINE 10: Final line End of EFrameworkunifiedStatus CSMVersion::get_build_info(SS_String &f_buildInfoStr)
//  EOF /SS_SystemManager/src/ss_sm_version.cpp
