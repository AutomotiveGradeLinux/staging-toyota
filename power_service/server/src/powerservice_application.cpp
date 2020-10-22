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
/// \ingroup  tag_PowerService
/// \brief    This file contains the standard set functions called by the NS
//            dispatcher on application initialization, cleanup and wakeup.
///
///////////////////////////////////////////////////////////////////////////////

#include <system_service/ss_services.h>
#include <native_service/frameworkunified_application.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_service_protocol.h>

#include "ss_power_powerservicelog.h"
#include "ss_power.h"

/**
 * EFrameworkunifiedStatus FrameworkunifiedOnInitialization(HANDLE h_app)
 * @brief Handles setup for SS_Power subsystem, registers with
 * NP_Service and indicates that its ready.
 *
 * @param h_app Handle to the SS_Power Framework Obj.
 *
 * @return method status of completion or failure.
 */
EFrameworkunifiedStatus FrameworkunifiedOnInitialization(HANDLE h_app) {
  EFrameworkunifiedStatus eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  eStatus = Power::GetInstance().Initialize(h_app);

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

/**
 * EFrameworkunifiedStatus PowerserviceOnWakepup(HANDLE h_app)
 * @brief This is not handled by the Power Subsystem since send out the
 * notifications that indicated Wakeup.
 *
 * @param h_app Handle to the SS_Power Framework Obj.
 *
 * @return method status of completion or failure.
 */
EFrameworkunifiedStatus PowerserviceOnWakepup(HANDLE h_app) {  // LCOV_EXCL_START 200: not called by NSFW
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "N/A; returning eFrameworkunifiedStatusOK");
  return eFrameworkunifiedStatusOK;
}
// LCOV_EXCL_STOP 200: not called by NSFW

/**
 * EFrameworkunifiedStatus FrameworkunifiedOnShutdown(HANDLE h_app)
 * @brief This is not handled by the Power Subsystem since send out the
 * notifications that indicated Shutdown.
 *
 * @param h_app Handle to the SS_Power Framework Obj.
 *
 * @return method status of completion or failure.
 */
EFrameworkunifiedStatus FrameworkunifiedOnShutdown(HANDLE h_app) {  // LCOV_EXCL_START 200: not called by NSFW
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "N/A; returning eFrameworkunifiedStatusOK");
  return eFrameworkunifiedStatusOK;
}
// LCOV_EXCL_STOP 200: not called by NSFW

/**
 * EFrameworkunifiedStatus FrameworkunifiedOnEShutdown(HANDLE h_app)
 * @brief This is not handled by the Power Subsystem since send out the
 * notifications that indicated Emergency Shutdown.
 *
 * @param h_app Handle to the SS_Power Framework Obj.
 *
 * @return method status of completion or failure.
 */
EFrameworkunifiedStatus FrameworkunifiedOnEShutdown(HANDLE h_app) {  // LCOV_EXCL_START 200: not called by NSFW
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "N/A; returning eFrameworkunifiedStatusOK");
  return eFrameworkunifiedStatusOK;
}
// LCOV_EXCL_STOP 200: not called by NSFW

/**
 * EFrameworkunifiedStatus FrameworkunifiedOnDestroy(HANDLE h_app)
 * @brief SS_Power will exit when this method is invoked,
 * return value will always be invalid.
 *
 * @param h_app Handle to the SS_Power Framework Obj.
 *
 * @return method status of completion or failure.
 */
EFrameworkunifiedStatus FrameworkunifiedOnDestroy(HANDLE h_app) {  // LCOV_EXCL_START 200: not called by NSFW
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG0(ZONE_FUNC, __FUNCTION__,
          " This method is not valid for %s, returning "
          "eFrameworkunifiedStatusNotImplemented",
          SERVICE_POWER);
  return eFrameworkunifiedStatusNotImplemented;
}
// LCOV_EXCL_STOP 200: not called by NSFW

/**
 * EFrameworkunifiedStatus FrameworkunifiedOnDebugDump(HANDLE h_app)
 * @brief Used to ask SS_Power to dump its vital information about the subsystem.
 *
 * @param h_app Handle to the SS_Power Framework Obj.
 *
 * @return method status of completion or failure.
 */
EFrameworkunifiedStatus FrameworkunifiedOnDebugDump(HANDLE h_app) {  // LCOV_EXCL_START 7: debug code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  Power* l_instance = &Power::GetInstance();

  if (l_instance != NULL) {
    l_instance->SSPowerDebugDump(h_app);

  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }
  return eStatus;
}
// LCOV_EXCL_STOP 7: debug code

/**
 * EFrameworkunifiedStatus FrameworkunifiedOnStart(HANDLE h_app)
 * @brief Used to .
 *
 * @param h_app Handle to the SS_Power Framework Obj.
 *
 * @return method status of completion or failure.
 */
EFrameworkunifiedStatus FrameworkunifiedOnStart(HANDLE h_app) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "N/A; returning eFrameworkunifiedStatusOK");
  return eFrameworkunifiedStatusOK;
}

/**
 * EFrameworkunifiedStatus FrameworkunifiedOnStop(HANDLE h_app)
 * @brief Used to .
 *
 * @param h_app Handle to the SS_Power Framework Obj.
 *
 * @return method status of completion or failure.
 */

EFrameworkunifiedStatus FrameworkunifiedOnStop(HANDLE h_app) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "N/A; returning eFrameworkunifiedStatusOK");
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
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "N/A; returning eFrameworkunifiedStatusOK");
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
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "N/A; returning eFrameworkunifiedStatusOK");
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
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "N/A; returning eFrameworkunifiedStatusOK");
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
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "N/A; returning eFrameworkunifiedStatusOK");
  return eFrameworkunifiedStatusOK;
}

/**
 * EFrameworkunifiedStatus FrameworkunifiedCreateStateMachine(HANDLE h_app)
 * @brief Used to .
 *
 * @param h_app Handle to the SS_Power Framework Obj.
 *
 * @return method status of completion or failure.
 */
EFrameworkunifiedStatus FrameworkunifiedCreateStateMachine(HANDLE h_app) {  // LCOV_EXCL_START 200: not called by NSFW
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "N/A; returning eFrameworkunifiedStatusOK");
  return eFrameworkunifiedStatusOK;
}
// LCOV_EXCL_STOP 200: not called by NSFW
// EOF

