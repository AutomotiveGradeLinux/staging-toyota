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
/// \brief    Framework wrapper over the service directory interface APIs
///
///
///
///////////////////////////////////////////////////////////////////////////////

#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_framework_types.h>
#include <native_service/frameworkunified_service_protocol.h>
#include <native_service/ns_logger_if.h>
#include "frameworkunified_framework_core.h"
#include "frameworkunified_framework_internal.h"

/// This is a library method in System Services....
/// Note: Special care has been taken so that Native Services
/// binaries don't have any circular dependence on System services.
extern EFrameworkunifiedStatus SendSystemErrorToSystemManager(EFrameworkunifiedSystemError  eSystemError) __attribute__((weak));


EFrameworkunifiedStatus FrameworkunifiedAttachSystemCallbacksDispatcher(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG0(ZONE_NS_INFO, __FUNCTION__, "+");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"

  EFrameworkunifiedStatus eStatus = FrameworkunifiedSSFrameworkInterfaceInternal(hApp);

  FRAMEWORKUNIFIEDLOG0(ZONE_NS_INFO, __FUNCTION__, "-");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"

  return eStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedSendSystemErrMessage
/// Sends a request to system manager for hard reset using function SendHardResetRequestToSystemManager
/// which is defined in SystemServices library.
///
/// \param [in] hApp
///     HANDLE - Application handle
///
/// \param [in] eResetReason
///     EFrameworkunifiedSystemError - Error Type
///
/// \return EFrameworkunifiedStatus
///         EFrameworkunifiedStatus - Returns status of operation
///
///////////////////////////////////////////////////////////////////////////////////////////
// LCOV_EXCL_START 200: System Error can't be tested for UT.
EFrameworkunifiedStatus FrameworkunifiedSendSystemErrMessage(HANDLE hApp, EFrameworkunifiedSystemError eSystemError) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;
  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "+");

  if (SendSystemErrorToSystemManager) {
    l_eStatus = SendSystemErrorToSystemManager(eSystemError);
  }

  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "-");
  return l_eStatus;
}
// LCOV_EXCL_STOP

