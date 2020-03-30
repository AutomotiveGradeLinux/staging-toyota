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

//////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup
/// \brief <INSERT INFO HERE>.
///
//////////////////////////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <string.h>

#include <native_service/frameworkunified_thread_priority.h>
#include <native_service/ns_logger_if.h>

#include <string>

char *priopts[] = {
#define THREAD    0
  const_cast<PSTR>("thrd"),
  NULL
};

namespace frameworkunified {
namespace framework {

CFrameworkunifiedThreadPriorities::TThreadPriorityList CFrameworkunifiedThreadPriorities::ms_mapThreadPritories;

//////////////////////////////////////////
// Constructor
//////////////////////////////////////////
CFrameworkunifiedThreadPriorities::CFrameworkunifiedThreadPriorities() {  // LCOV_EXCL_START 200:only use static function of this class
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
}
// LCOV_EXCL_STOP

//////////////////////////////////////////
// Destructor
//////////////////////////////////////////
CFrameworkunifiedThreadPriorities::~CFrameworkunifiedThreadPriorities() {  // LCOV_EXCL_START 200:only use static function of this class
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
}
// LCOV_EXCL_STOP

//////////////////////////////////////////
// GetPriority
//////////////////////////////////////////
SI_32 CFrameworkunifiedThreadPriorities::GetPriority(const std::string &f_cThreadName) {
  SI_32 l_si32Prio = FRAMEWORKUNIFIED_PRIORITY_NOT_FOUND;
  ThreadPrioMapIter iter = ms_mapThreadPritories.find(f_cThreadName);
  if (iter != ms_mapThreadPritories.end() && (FRAMEWORKUNIFIED_PRIORITY_NOT_FOUND == l_si32Prio)) {
    l_si32Prio = iter->second;
  }

  return l_si32Prio;
}

//////////////////////////////////////////
// AddPriority
//////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedThreadPriorities::AddPriority(const std::string &f_cThreadName, SI_32 f_si32Priority) {
  ms_mapThreadPritories[f_cThreadName] = f_si32Priority;
  return eFrameworkunifiedStatusOK;
}

//////////////////////////////////////////
// PrintPriorites
//////////////////////////////////////////
VOID CFrameworkunifiedThreadPriorities::PrintPriorites() {  // LCOV_EXCL_START 7: debug code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  ThreadPrioMapIter iter = ms_mapThreadPritories.begin();
  for (; iter != ms_mapThreadPritories.end(); iter++) {
    std::string name = iter->first;
    FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, "Thread name: %s Priority: %d", name.data(), iter->second);
  }
}
// LCOV_EXCL_STOP

//////////////////////////////////////////
// ParseThreadArguments
//////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedThreadPriorities::ParseThreadArguments(PCHAR f_cArgumentValue) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  PCHAR l_cData = NULL;
  PSTR l_cOptions;
  PSTR l_cValue;
  PCHAR saveptr;

  if (NULL != f_cArgumentValue) {
    l_cOptions = f_cArgumentValue;
    FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, "args of -p: %s", l_cOptions);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"

    while (*l_cOptions != '\0' && (eFrameworkunifiedStatusOK == l_eStatus)) {
      switch (getsubopt(&l_cOptions, priopts, &l_cValue)) {
        case THREAD: {
            if (l_cValue == NULL) {
              l_eStatus = eFrameworkunifiedStatusFail;
            } else {
              std::string l_cName("");
              std::string l_cPriority("");  // LCOV_EXCL_BR_LINE 11: except branch
              int at = static_cast<int>(frameworkunified::framework::args::name);

              l_cData = strtok_r(l_cValue, ":", &saveptr);  // NOLINT  (readability/nolint)
              while (NULL != l_cData) {
                switch (at) {
                  case frameworkunified::framework::args::name:
                    l_cName = l_cData;
                    break;
                  case frameworkunified::framework::args::priority:
                    l_cPriority = l_cData;
                    break;
                  default:
                    break;
                }

                if (!l_cName.empty() && !l_cPriority.empty()) {
                  frameworkunified::framework::CFrameworkunifiedThreadPriorities::AddPriority(l_cName, atoi(l_cPriority.data()));
                  at = static_cast<int>(frameworkunified::framework::args::name);
                  l_cName.clear();
                  l_cPriority.clear();
                } else {
                  at++;
                }

                l_cData = strtok_r(NULL, ":", &saveptr);  // NOLINT  (readability/nolint)
              }
            }
            break;
          }
        default: {
            l_eStatus = eFrameworkunifiedStatusFail;
            break;
          }
      }
    }
  }
  return l_eStatus;
}

}  //  namespace framework
};  //  namespace frameworkunified
///  EOF


