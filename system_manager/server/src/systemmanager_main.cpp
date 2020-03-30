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
/// \brief    Application entry point.
///
///////////////////////////////////////////////////////////////////////////////
#include <system_service/ss_system_if.h>
#include <native_service/frameworkunified_dispatcher.h>
#include <native_service/ns_version_if.h>
#include <system_service/ss_services.h>
#include <system_service/ss_string_maps.h>
#include <system_service/ss_version.h>
#include <system_service/ss_system_types.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstdlib>
#include <fstream>

#include "ss_sm_systemmanagerlog.h"
#include "ss_system_manager.h"


CFrameworkunifiedVersion g_FrameworkunifiedVersion(MAJORNO, MINORNO, REVISION);

/// \brief: Name of the Queue that will be used to read data from.
const CHAR AppName[] = SERVICE_SYSMANAGER;

static EFrameworkunifiedStatus ArgumentParser(SI_32 cc, PCHAR str) {
  switch (cc) {
    case 'b':
        CSystemManager::m_bootOpt += str;
        break;
    default:
        break;
  }
  return eFrameworkunifiedStatusOK;
}

//////////////////////////////////////////
//  Function : main
//////////////////////////////////////////
int main(int argc, char *argv[]) {
  EFrameworkunifiedStatus l_eStatus;
  FrameworkunifiedDefaultCallbackHandler cbFuncs;
  FRAMEWORKUNIFIED_MAKE_DEFAULT_CALLBACK(cbFuncs);  // LCOV_EXCL_BR_LINE 15: marco defined in ss-system_if.h  // NOLINT(whitespace/line_length)
  int fd;
/*
  if (geteuid() == 0) {  // LCOV_EXCL_BR_LINE 200: process cannot be executed by root
    // LCOV_EXCL_START 200: process cannot be executed by root
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    printf("System cannot be started by root user!!\n"
           "You can start system: \n"
            " # sm > /dev/null &\n"
            "    or \n"
            " # sm &\n");
    // LCOV_EXCL_STOP 
    return -1;
  }
*/
  fd = open("/dev/null", O_RDONLY);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  if (fd > 0) {  // LCOV_EXCL_BR_LINE 5: open file error case.
    int ret;
    ret = dup2(fd, 0);
    if (ret == -1) perror("dup2");  // LCOV_EXCL_BR_LINE 5: dup2 error case.
    ret = close(fd);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    if (ret == -1) perror("close");  // LCOV_EXCL_BR_LINE 5: close error case.
  } else {
    // LCOV_EXCL_START 5: open file error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    perror("open");  // LCOV_EXCL_LINE 5: open file error case.
    // LCOV_EXCL_STOP
  }

  // This allows overriding the already-passed-in System Manager's command
  // line arguments by having a file exist named
  // "/agl/rwdata/<this_file's_base_name>.args".
  // For example, when this file is named 'SS_SystemManager', then
  // if '/agl/rwdata/SS_SystemManager.args' is present, then the
  // '/agl/rwdata/SS_SystemManager.args' strings are read as command line
  // values and are passed to the FrameworkunifiedDispatcher() function.
  // -m
  // 0xFFFFFFFF,0xFFFFFFFF, where each bit represents a logging Zone.
  // -c
  // [configuration file..]

  struct sched_param l_schedParam;

  l_schedParam.sched_priority = PR_SS_SYSMANAGER;

  if (0 != sched_setscheduler(getpid(), SCHED_FIFO, &l_schedParam)) {  // LCOV_EXCL_BR_LINE 5: sched_setscheduler error case.  // NOLINT(whitespace/line_length)
    printf("ASSERT %d:%s\n", errno, strerror(errno));  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  }

  CustomCommandLineOptions l_tCmdLineOptions = { "b:", NULL, ArgumentParser };  // cShortOptions,rsv,callback
  {
    l_eStatus = FrameworkunifiedDispatcherWithArguments(AppName, argc, argv, &cbFuncs, &l_tCmdLineOptions);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  }
  printf("SM: Line %d: FrameworkunifiedDispatcherWithArguments() returned l_eStatus: '%d'/'%s'\n",  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
        __LINE__,  l_eStatus,   GetStr(l_eStatus).c_str());  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  return l_eStatus;
}  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
