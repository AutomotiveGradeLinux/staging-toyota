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

#include <stdio.h>
#include <native_service/frameworkunified_application.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_service_protocol.h>

#include "ss_resm_resourcemanagerlog.h"
#include "system_service/resm_type.h"

extern void FlushMemInfo(void);

// Called at process startup
/********************************************************
 * EFrameworkunifiedStatus FrameworkunifiedOnInitialization(HANDLE hApp)
 *
 *   System callbacks that applications should address.
 ********************************************************/
EFrameworkunifiedStatus FrameworkunifiedOnInitialization(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // API to Register Notification for Service Availability.
  eStatus = FrameworkunifiedRegisterServiceAvailabilityNotification(hApp, NTFY_ResourceMgr_Availability);
  if (eFrameworkunifiedStatusOK != eStatus) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to Register Service Availability Notification:%s",
        NTFY_ResourceMgr_Availability);
    return eStatus;
    // LCOV_EXCL_STOP 4: NSFW error case.
  }
  // API to Publish Service Availability Notification.
  eStatus = FrameworkunifiedPublishServiceAvailability(hApp, FALSE);
  if (eFrameworkunifiedStatusOK != eStatus) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "Failed to Publish Service Availability Notification:0x%x", eStatus);
    // LCOV_EXCL_STOP 4: NSFW error case.
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

// Called at process termination
/********************************************************
 * EFrameworkunifiedStatus FrameworkunifiedOnDestroy
 ********************************************************/
EFrameworkunifiedStatus FrameworkunifiedOnDestroy(HANDLE hApp) {  // LCOV_EXCL_START 14 Resident process, not called by NSFW
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}
// LCOV_EXCL_STOP 14 Resident process, not called by NSFW
/********************************************************
 * EFrameworkunifiedStatus FrameworkunifiedOnDebugDump
 ********************************************************/
EFrameworkunifiedStatus FrameworkunifiedOnDebugDump(HANDLE hApp) {  //LCOV_EXCL_START 7:debug code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
//LCOV_EXCL_STOP 7:debug code
/********************************************************
 * EFrameworkunifiedStatus FrameworkunifiedOnStart(HANDLE hApp)
 ********************************************************/
EFrameworkunifiedStatus FrameworkunifiedOnStart(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

/********************************************************
 * EFrameworkunifiedStatus FrameworkunifiedOnStop(HANDLE hApp)
 ********************************************************/
EFrameworkunifiedStatus FrameworkunifiedOnStop(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FlushMemInfo();
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

/********************************************************
 * EFrameworkunifiedStatus FrameworkunifiedOnPreStart(HANDLE hApp)
 ********************************************************/
EFrameworkunifiedStatus FrameworkunifiedOnPreStart(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

/********************************************************
 * EFrameworkunifiedStatus FrameworkunifiedOnPreStop(HANDLE hApp)
 ********************************************************/
EFrameworkunifiedStatus FrameworkunifiedOnPreStop(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

/********************************************************
 * EFrameworkunifiedStatus FrameworkunifiedOnBackgroundStart(HANDLE hApp)
 ********************************************************/
EFrameworkunifiedStatus FrameworkunifiedOnBackgroundStart(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

/********************************************************
 * EFrameworkunifiedStatus FrameworkunifiedOnBackgroundStop(HANDLE hApp)
 ********************************************************/
EFrameworkunifiedStatus FrameworkunifiedOnBackgroundStop(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

/********************************************************
 * EFrameworkunifiedStatus FrameworkunifiedCreateStateMachine(HANDLE hApp)
 ********************************************************/
EFrameworkunifiedStatus FrameworkunifiedCreateStateMachine(HANDLE hApp) {  // LCOV_EXCL_START 8: not use StateMachine
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
// LCOV_EXCL_STOP 8: not use StateMachine
// EOF
