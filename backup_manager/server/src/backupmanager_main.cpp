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

#include <native_service/ns_version_if.h>
#include <native_service/frameworkunified_dispatcher.h>
#include <agl_thread.h>
#include <system_service/ss_system_if.h>
#include <cerrno>
#include <cstdio>
#include "bkup_backupmanagerlog.h"

CFrameworkunifiedVersion g_FrameworkunifiedVersion(0, 0, 0);  // NOLINT(readability/nolint).

FRAMEWORKUNIFIEDLOGPARAM g_FrameworkunifiedLogParams = {  // NOLINT(readability/nolint) The varable is used by FW
  FRAMEWORKUNIFIEDLOGOPTIONS,
  {
    ZONE_TEXT_10, ZONE_TEXT_11, ZONE_TEXT_12,
    ZONE_TEXT_13, ZONE_TEXT_14, ZONE_TEXT_15,
    ZONE_TEXT_16, ZONE_TEXT_17, ZONE_TEXT_18,
    ZONE_TEXT_19, ZONE_TEXT_20, ZONE_TEXT_21,
    ZONE_TEXT_22, ZONE_TEXT_23, ZONE_TEXT_24,
    ZONE_TEXT_25, ZONE_TEXT_26, ZONE_TEXT_27,
    ZONE_TEXT_28, ZONE_TEXT_29, ZONE_TEXT_30,
    ZONE_TEXT_31
  },
  FRAMEWORKUNIFIEDLOGZONES
};

int main(int argc, char *argv[]) {
  // LCOV_EXCL_BR_START 11:unexpected branch
  FrameworkunifiedDefaultCallbackHandler cb_funcs;
  FRAMEWORKUNIFIED_MAKE_DEFAULT_CALLBACK(cb_funcs);
  FRAMEWORKUNIFIED_SET_ZONES();
  // LCOV_EXCL_BR_STOP 11:unexpected branch

  // LCOV_EXCL_BR_START 11:unexpected branch
  EFrameworkunifiedStatus e_status = FrameworkunifiedDispatcherWithArguments(MN_NS_BACKUPMGR, argc, argv, &cb_funcs);
  // LCOV_EXCL_BR_STOP 11:unexpected branch
  if (e_status != eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
    // LCOV_EXCL_START 4: NSFW error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "FrameworkunifiedDispatcherWithArguments: %d", e_status);
    return EXIT_FAILURE;
    // LCOV_EXCL_STOP 4: NSFW error case
  }

  return EXIT_SUCCESS;
}  // LCOV_EXCL_BR_LINE 10:The final line
