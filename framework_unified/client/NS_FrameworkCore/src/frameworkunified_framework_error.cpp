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

#include <native_service/frameworkunified_framework_error.hpp>
#include <cstdio>
#include <string>
#include "frameworkunified_framework_error_internal.hpp"

// using std::snprintf;
typedef std::string TErrorDesc;
typedef const TErrorDesc TCErrorDesc;

namespace {

PCSTR FrameworkunifiedStatusDesc(EFrameworkunifiedStatus status) {
  switch (status) {
    case eFrameworkunifiedStatusSessionLimitMaxedOut:
      return "Session limit maxed out";
    case eFrameworkunifiedStatusDbRecNotFound:
      return "Db record not Found";
    case eFrameworkunifiedStatusDbResultError:
      return "Db result error";
    case eFrameworkunifiedStatusDbExecuteFail:
      return "Db execute failure";
    case eFrameworkunifiedStatusSemCloseFail:
      return "Semaphore close failure";
    case eFrameworkunifiedStatusSemUnlockFail:
      return "Semaphore unlock gailure";
    case eFrameworkunifiedStatusSemLockFail:
      return "Semaphore lock failure";
    case eFrameworkunifiedStatusFail:
      return "failure";
    case eFrameworkunifiedStatusOK:
      return "Status OK";
    case eFrameworkunifiedStatusInvldBuf:
      return "Invalid buffer";
    case eFrameworkunifiedStatusInvldHandle:
      return "Invalid handle";
    case eFrameworkunifiedStatusInvldHndlType:
      return "Invalid handle yype";
    case eFrameworkunifiedStatusInvldQName:
      return "Invalid queue name";
    case eFrameworkunifiedStatusMsgQFull:
      return "Message queue full";
    case eFrameworkunifiedStatusInvldNotification:
      return "Invliad notification";
    case eFrameworkunifiedStatusInvldParam:
      return "Invalid parameter";
    case eFrameworkunifiedStatusInvldBufSize:
      return "Invalid buffer size";
    case eFrameworkunifiedStatusInvldID:
      return "Invalid MemID";
    case eFrameworkunifiedStatusCannotRelease:
      return "Cannot release resource";
    case eFrameworkunifiedStatusBadConnection:
      return "Bad connection";
    case eFrameworkunifiedStatusExit:
      return "Exit";
    case eFrameworkunifiedStatusNotImplemented:
      return "Not implemented";
    case eFrameworkunifiedStatusThreadBusy:
      return "Thread busy";
    case eFrameworkunifiedStatusThreadSelfJoin:
      return "Attempted to self-join";
    case eFrameworkunifiedStatusThreadInvalidVal:
      return "Invalid value passed";
    case eFrameworkunifiedStatusThreadNotExist:
      return "Thread does not exist";
    case eFrameworkunifiedStatusFault:
      return "Fault";
    case eFrameworkunifiedStatusServNotFound:
      return "Service not found";
    case eFrameworkunifiedStatusErrOther:
    default:
      return "Other error";
  }
}

TCErrorDesc BuildFrameworkunifiedErrorDesc(EFrameworkunifiedStatus error, PCSTR errorMsg) {
  CHAR buf[ MAX_QUEUE_MSG_SIZE ] = {};
  snprintf(&buf[ 0 ],
           MAX_QUEUE_MSG_SIZE,
           "Framework Error: %s - %s",
           FrameworkunifiedStatusDesc(error),
           errorMsg != 0 ? errorMsg : NULL);
  return TErrorDesc(&buf[ 0 ]);
}

TCErrorDesc BuildFrameworkunifiedSystemErrorDesc(EFrameworkunifiedSystemError error, PCSTR errorMsg) {
  TErrorDesc l_cErrorMsg("Framework defined error desc: ");

  switch (error) {
    // Commented, because this is product specific error. Must handle by SystemServices only.
    // case eFrameworkunifiedDSPHardwareReset : l_cErrorMsg.append("DSP HW RESET ERROR"); break;
    default:
      l_cErrorMsg.append("Other error");
      break;
  }

  l_cErrorMsg.append(" :: Application provided error desc:");
  l_cErrorMsg.append(errorMsg);
  return l_cErrorMsg;
}

}  // end namespace



frameworkunified::framework::error::error::error(EFrameworkunifiedStatus error, PCSTR errorMsg)
  : std::runtime_error(BuildFrameworkunifiedErrorDesc(error, errorMsg)),
    m_error(error) {
}

frameworkunified::framework::error::CSystemError::CSystemError(EFrameworkunifiedSystemError error, PCSTR errorMsg)
  : std::runtime_error(BuildFrameworkunifiedSystemErrorDesc(error, errorMsg)),
    m_eSystemError(error) {
}
