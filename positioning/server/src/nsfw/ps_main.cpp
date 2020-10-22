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
#include <native_service/frameworkunified_dispatcher.h>
#include <native_service/frameworkunified_application.h>
#include <peripheral_service/ps_services.h>
#include <native_service/ns_version_if.h>
#include <system_service/ss_system_if.h>
#include <vehicle_service/positioning_base_library.h>
#include <cstdlib>
#include <iostream>
#include "vehicle_version.h"

const CHAR kAppName[PS_APPNAME_MAX] = "Positioning";

CFrameworkunifiedVersion g_FrameworkunifiedVersion(MAJORNO,    // NOLINT(readability/nolint)
                         MINORNO,    // NOLINT(readability/nolint)
                         REVISION);

/* FRAMEWORKUNIFIEDLOGPARAM : g_FrameworkunifiedLogParams :  definition for LOG */
FRAMEWORKUNIFIEDLOGPARAM g_FrameworkunifiedLogParams = {    // NOLINT(readability/nolint)
    FRAMEWORKUNIFIEDLOGOPTIONS,
    {
        ZONE_TEXT_10, ZONE_TEXT_11, ZONE_TEXT_12,
        ZONE_TEXT_13, ZONE_TEXT_14, ZONE_TEXT_15,
        ZONE_TEXT_16, ZONE_TEXT_17, ZONE_TEXT_18,
        ZONE_TEXT_19, ZONE_TEXT_20, ZONE_TEXT_21,
        ZONE_TEXT_22, ZONE_TEXT_23, ZONE_TEXT_24,
        ZONE_TEXT_25, ZONE_TEXT_26, ZONE_TEXT_27,
        ZONE_TEXT_28, ZONE_TEXT_29, ZONE_TEXT_30,
        ZONE_TEXT_31,
        ZONE_POS_SYS_IN, ZONE_POS_GPS_IN, ZONE_POS_CMD_IN, ZONE_POS_NAV_IN
    },
    FRAMEWORKUNIFIEDLOGZONES
};

/* Function : main */
int main(int argc, char *argv[]) {
    EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
    FrameworkunifiedDefaultCallbackHandler cb_funcs;
    FRAMEWORKUNIFIED_MAKE_DEFAULT_CALLBACK(cb_funcs);

    FRAMEWORKUNIFIED_SET_ZONES();
    e_status = FrameworkunifiedDispatcher(kAppName, &cb_funcs);

    return e_status;
}  // LCOV_EXCL_BR_LINE 10:The final line
