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
/// \ingroup  tag_NSFramework
/// \brief
///
///
///
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Interfaces
///////////////////////////////////////////////////////////////////////////////
#include <native_service/ns_buildversioncheck.hpp>
#include <cstdlib>
#include <cstring>

namespace {

namespace frameworkunified_ns_bvc {

const UI_32 kMaxBuildVersionLength = 32;
PCSTR g_build_version_env_var_name = "BUILD_VERSION";
PCSTR g_build_version = _BUILD_VERSION;  // Should be defined when library is built

bool isEmptyString(PCSTR str) {  // NOLINT (readability/nolint)
  return 0 == std::strncmp("", str, 1);
}

}  // namespace frameworkunified_ns_bvc

}  // namespace

namespace frameworkunified {

namespace ns {

namespace utility {

BOOL buildVersionsMatch(PCSTR build_version) {  // NOLINT (readability/nolint)
  PCSTR env_build_version = getEnvironmentBuildVersion();
  PCSTR lib_build_version = getLibraryBuildVersion();
  return 0 == std::strncmp(env_build_version, build_version, frameworkunified_ns_bvc::kMaxBuildVersionLength) &&
         0 == std::strncmp(lib_build_version, build_version, frameworkunified_ns_bvc::kMaxBuildVersionLength) &&
         !frameworkunified_ns_bvc::isEmptyString(build_version) &&
         !frameworkunified_ns_bvc::isEmptyString(env_build_version) &&
         !frameworkunified_ns_bvc::isEmptyString(lib_build_version);
}

PCSTR getEnvironmentBuildVersion() {  // NOLINT (readability/nolint)
  static PCSTR env_version = std::getenv(frameworkunified_ns_bvc::g_build_version_env_var_name);
  return env_version != NULL ? env_version : "";
}

PCSTR getLibraryBuildVersion() {  // NOLINT (readability/nolint)
  return frameworkunified_ns_bvc::g_build_version;
}

namespace utility_private {

PCSTR niceBuildVersion(PCSTR build_version) {  // NOLINT (readability/nolint)
  return frameworkunified_ns_bvc::isEmptyString(build_version) ?  "<Not defined>" : build_version;
}

}  // namespace utility_private

}  // namespace utility

}  // namespace ns

}  // namespace frameworkunified
