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
/// \brief    This file contains the standard set functions called by the NS
//            dispatcher on application initialization, cleanup and wakeup.
///
///////////////////////////////////////////////////////////////////////////////

#include <native_service/frameworkunified_application.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_service_protocol.h>
#include <system_service/ss_templates.h>

#include "ss_sm_systemmanagerlog.h"
#include "ss_system_manager.h"

#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>

//////////////////////////////////////////
//  Function : FrameworkunifiedOnInitialization
//////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOnInitialization(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  eStatus = CSystemManager::GetInstance().Initialize(hApp);
  struct rlimit setrl = {RLIM_INFINITY, RLIM_INFINITY};
  struct rlimit getrl = {};
  int ret = prlimit(getpid(), RLIMIT_MSGQUEUE, &setrl, &getrl);
  if (ret != 0) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "prlimit ret%d", ret);
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Previous limits: soft=%lld; hard=%lld", (long long) getrl.rlim_cur,
           (long long) getrl.rlim_max);
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

//////////////////////////////////////////
//  Function : FrameworkunifiedOnWakeup
//////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOnWakeup(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

//////////////////////////////////////////
//  Function : FrameworkunifiedOnShutdown
//////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOnShutdown(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnDestroy
/// \todo Add behavior to this function
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOnDestroy(HANDLE hApp) {  // LCOV_EXCL_START 14:Resident process, not called by NSFW
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}
// LCOV_EXCL_STOP

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedOnEShutdown
/// \todo Add behavior to this function
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOnEShutdown(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus FrameworkunifiedOnDebugDump(HANDLE hApp) {  // LCOV_EXCL_START 7:Debug code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  CSystemManager* l_instance = &CSystemManager::GetInstance();
  if (l_instance !=NULL) {
    l_instance->SystemManagerDebugDump(hApp);
  } else {
    l_eStatus = eFrameworkunifiedStatusInvldHandle;
    LOG_STATUS(l_eStatus, "Unable to acquire SM instance. Debug Dump handler not called.");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
// LCOV_EXCL_STOP

/**
 * EFrameworkunifiedStatus FrameworkunifiedOnStart(HANDLE hApp)
 * @brief Used to .
 *
 * @param hApp Handle to the SS_Power Framework Obj.
 *
 * @return method status of completion or failure.
*/
EFrameworkunifiedStatus FrameworkunifiedOnStart(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "This method has not been implemented, return: eFrameworkunifiedStatusOK!");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eFrameworkunifiedStatusOK;
}

/**
 * EFrameworkunifiedStatus FrameworkunifiedOnStop(HANDLE hApp)
 * @brief Used to .
 *
 * @param hApp Handle to the SS_Power Framework Obj.
 *
 * @return method status of completion or failure.
*/
EFrameworkunifiedStatus FrameworkunifiedOnStop(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "This method has not been implemented, return: eFrameworkunifiedStatusOK!");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eFrameworkunifiedStatusOK;
}

/**
 * EFrameworkunifiedStatus FrameworkunifiedOnPreStart(HANDLE h_app)
 *
 * @param h_app Handle to the SS_Power Framework Obj.
 *
 * @return method status of completion or failure.
 */

EFrameworkunifiedStatus FrameworkunifiedOnPreStart(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "This method has not been implemented, return: eFrameworkunifiedStatusOK!");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eFrameworkunifiedStatusOK;
}

/**
 * EFrameworkunifiedStatus FrameworkunifiedOnPreStop(HANDLE h_app)
 *
 * @param h_app Handle to the SS_Power Framework Obj.
 *
 * @return method status of completion or failure.
 */

EFrameworkunifiedStatus FrameworkunifiedOnPreStop(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "This method has not been implemented, return: eFrameworkunifiedStatusOK!");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eFrameworkunifiedStatusOK;
}

/**
 * EFrameworkunifiedStatus FrameworkunifiedOnBackgroundStart(HANDLE h_app)
 *
 * @param h_app Handle to the SS_Power Framework Obj.
 *
 * @return method status of completion or failure.
 */

EFrameworkunifiedStatus FrameworkunifiedOnBackgroundStart(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "This method has not been implemented, return: eFrameworkunifiedStatusOK!");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eFrameworkunifiedStatusOK;
}

/**
 * EFrameworkunifiedStatus FrameworkunifiedOnBackgroundStop(HANDLE h_app)
 *
 * @param h_app Handle to the SS_Power Framework Obj.
 *
 * @return method status of completion or failure.
 */

EFrameworkunifiedStatus FrameworkunifiedOnBackgroundStop(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "This method has not been implemented, return: eFrameworkunifiedStatusOK!");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eFrameworkunifiedStatusOK;
}

/**
 * EFrameworkunifiedStatus FrameworkunifiedCreateStateMachine(HANDLE hApp)
 * @brief Used to .
 *
 * @param hApp Handle to the SS_Power Framework Obj.
 *
 * @return method status of completion or failure.
*/
EFrameworkunifiedStatus FrameworkunifiedCreateStateMachine(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "This method has not been implemented, return: eFrameworkunifiedStatusOK!");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eFrameworkunifiedStatusOK;
}

////////////////////////////////////////////////////////////////////////////////////////////
///
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus SystemmanagerCloseApplication(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  l_eStatus = CSystemManager::GetInstance().CloseApplication(hApp);
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
// EOF

