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

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_SystemManager
/// \brief    This file provides support for module version management.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef SS_SM_VERSION_H_  // NOLINT
#define SS_SM_VERSION_H_

#include <native_service/frameworkunified_types.h>
#include <system_service/ss_system_manager_protocol.h>
#include "ss_sm_process_launcher_protocol.h"

class CSMVersion {
 public:
  CSMVersion();
  ~CSMVersion();
  EFrameworkunifiedStatus get_version(SS_VersionNumberType& f_VersionNumber);   // NOLINT
  EFrameworkunifiedStatus get_build_info(std::string& f_buildInfoStr);  // NOLINT

 private:
};
#endif  // SS_SM_VERSION_H_  // NOLINT
