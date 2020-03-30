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
/// \brief    This file provides support for process launching and termination.
///
///////////////////////////////////////////////////////////////////////////////
#include <native_service/frameworkunified_framework_if.h>

#include "ProcessLauncher.h"
#include "ss_sm_process_launcher_protocol.h"
#include "ss_sm_systemmanagerlog.h"

template<typename C, eFrameworkunifiedStatus (C::*M)(HANDLE)> EFrameworkunifiedStatus ProcessLauncherCallback(HANDLE hThread) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  C * pObj = static_cast<C *>(FrameworkunifiedGetThreadSpecificData(hThread));

  if (pObj) {  // LCOV_EXCL_BR_LINE 5:pObj must not be NULL
    l_eStatus = (pObj->*M)(hThread);
  }

  return l_eStatus;
}

EFrameworkunifiedStatus ProcessLauncherOnStart(HANDLE hThread) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  CProcessLauncher * pObj = new (std::nothrow) CProcessLauncher(NULL);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  if (pObj) {  // LCOV_EXCL_BR_LINE 5:new error
    // LCOV_EXCL_BR_START 200:l_eStatus always return eFrameworkunifiedStatusOK
    if (eFrameworkunifiedStatusOK != (l_eStatus = pObj->PLOnCmdStart(hThread))) {
    // LCOV_EXCL_BR_STOP
      // LCOV_EXCL_START 200:l_eStatus always return eFrameworkunifiedStatusOK
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
              " Error: pObj->PLOnCmdStart(hThread) errored: %d/'%s'",
              l_eStatus, GetStr(l_eStatus).c_str());
      // LCOV_EXCL_STOP
    } else {
      FrameworkunifiedSetThreadSpecificData(hThread, pObj);

      FrameworkunifiedProtocolCallbackHandler aParentHandlers[] = { {
              ePLThrdCmd_LAUNCH_MODULE_REQST, ProcessLauncherCallback<
                      CProcessLauncher,
                      &CProcessLauncher::PLOnCmdLaunchModule> }, {
              ePLThrdCmd_TERMINATE_MODULE_REQST, ProcessLauncherCallback<
                      CProcessLauncher,
                      &CProcessLauncher::PLOnCmdTerminateModule> }, {
              ePLThrdCmd_RELAUNCH_MODULE_REQST, ProcessLauncherCallback<
                      CProcessLauncher,
                      &CProcessLauncher::PLOnCmdRelaunchModule> }, {
              ePLThrdCmd_MODULE_STATUS_REQST, ProcessLauncherCallback<
                      CProcessLauncher,
                      &CProcessLauncher::PLOnCmdModuleStatus> }, {
              ePLThrdCmd_THREAD_STATUS_REQST, ProcessLauncherCallback<
                      CProcessLauncher,
                      &CProcessLauncher::PLOnCmdHeartbeatStatusReq> } };  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

      // LCOV_EXCL_BR_START 4:NSFW error case
      if (eFrameworkunifiedStatusOK
          != (l_eStatus = FrameworkunifiedAttachParentCallbacksToDispatcher(hThread, aParentHandlers,
                                                               static_cast<UI_32>(_countof(aParentHandlers))))) {
      // LCOV_EXCL_BR_STOP
        // LCOV_EXCL_START 4:NSFW error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                " Error: FrameworkunifiedAttachParentCallbacksToDispatcher() errored: %d/'%s'",
                l_eStatus, GetStr(l_eStatus).c_str());
        // LCOV_EXCL_STOP
      } else {
        // Add other Attaches here!!!
      }
    }
  } else {  // LCOV_EXCL_BR_LINE 5::new operation failed
    // LCOV_EXCL_START 5:new error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: new CProcessLauncher( NULL ) returned NULL");
    l_eStatus = eFrameworkunifiedStatusNullPointer;
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

// LCOV_EXCL_START 200:Thread which registered in init_process_launcher() is never stoppped.
EFrameworkunifiedStatus ProcessLauncherOnStop(HANDLE hThread) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  CProcessLauncher * pObj = static_cast<CProcessLauncher *>(FrameworkunifiedGetThreadSpecificData(hThread));

  if (pObj) {
    if (eFrameworkunifiedStatusOK != (l_eStatus = pObj->PLOnCmdStop(hThread))) {
      LOG_ERROR("PLOnCmdStop()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    }
    delete pObj;
    pObj = NULL;
  }

  FrameworkunifiedSetThreadSpecificData(hThread, NULL);

  if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedDetachServiceFromDispatcher(hThread, FrameworkunifiedGetAppName(hThread)))) {
    LOG_ERROR("FrameworkunifiedDetachServiceFromDispatcher()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
// LCOV_EXCL_STOP

