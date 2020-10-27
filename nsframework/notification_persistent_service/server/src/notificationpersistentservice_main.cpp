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
/// \brief
///
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <native_service/ns_version.h>
#include <native_service/ns_version_if.h>
#include <native_service/frameworkunified_dispatcher.h>
#include <native_service/frameworkunified_framework_if.h>

#include <stdlib.h>
#include <cstdlib>
#include <iostream>

#include "ns_npp.h"  // for static members. Set thread prio.
#include "ns_npp_notificationpersistentservicelog.h"
#include "ns_npp_persistence_manager.h"  // for static members. Set enable/disable persistence feature.

const CHAR AppName[] = FRAMEWORKUNIFIED_NS_NPSERVICE;  // NOLINT (readability/naming)
CHAR g_csendreadyackto[MAX_QUEUE_NAME_SIZE];

CFrameworkunifiedVersion g_FrameworkunifiedVersion(MAJORNO, MINORNO, REVISION);  // NOLINT (readability/naming)

// Argument Parser for NSNPP
EFrameworkunifiedStatus NPPArgumentParser(SI_32 f_iargument, PCHAR f_pargumentvalue);

FRAMEWORKUNIFIEDLOGPARAM g_FrameworkunifiedLogParams = { // NOLINT (readability/naming)
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
  },
  FRAMEWORKUNIFIEDLOGZONES
};

extern const FrameworkunifiedDefaultCallbackHandler kDefaultCbHandler;


////////////////////////////////////////////////////////////////////////////////////////////////////
//  Function : main
////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
  FRAMEWORKUNIFIED_SET_ZONES();
  CustomCommandLineOptions l_tcmdlineoptions = { "i:e:w:fa:v:", NULL, NPPArgumentParser};
  EFrameworkunifiedStatus l_estatus = FrameworkunifiedDispatcherWithArguments(AppName, argc, argv, &kDefaultCbHandler, &l_tcmdlineoptions);

  return static_cast<SI_32>(l_estatus);
}

EFrameworkunifiedStatus NPPArgumentParser(SI_32 f_iargument, PCHAR f_pargumentvalue) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  SI_32 l_iTmpArgValue = 0;
  switch (f_iargument) {  // LCOV_EXCL_BR_LINE 8: cannot test
    // Disable Persistence
    case 'f': {
        CPersistenceManager::m_bPersistenceDisabled =  TRUE;
        break;
      }
    case 'i': {
        l_iTmpArgValue = atoi(f_pargumentvalue);
        if (l_iTmpArgValue > 0) {  // LCOV_EXCL_BR_LINE 8: cannot test
          CNSNPP::m_siImmediatePersistenceThreadPrio = l_iTmpArgValue;
        }
        break;
      }
    case 'e': {
        l_iTmpArgValue = atoi(f_pargumentvalue);
        if (l_iTmpArgValue > 0) {  // LCOV_EXCL_BR_LINE 8: cannot test
          CNSNPP::m_siReadThreadPrio = l_iTmpArgValue;
        }
        break;
      }
    case 'w': {
        l_iTmpArgValue = atoi(f_pargumentvalue);
        if (l_iTmpArgValue > 0) {  // LCOV_EXCL_BR_LINE 8: cannot test
          CNSNPP::m_siWriteThreadPrio = l_iTmpArgValue;
        }
        break;
      }
    case 'a': {
        std::memset(g_csendreadyackto, 0, MAX_QUEUE_NAME_SIZE);
        std::strncpy(g_csendreadyackto, f_pargumentvalue, (MAX_QUEUE_NAME_SIZE - 1));
        break;
      }
    case 'v': {
        l_iTmpArgValue = atoi(f_pargumentvalue);
        CNSNPP::m_siCRCCheckCount = static_cast<UI_16>(l_iTmpArgValue);
        break;
      }

    default:
      l_estatus = eFrameworkunifiedStatusFail;
      break;
  }
  return l_estatus;
}
