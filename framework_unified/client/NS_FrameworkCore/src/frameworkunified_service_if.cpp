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

////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_HMI_CWORD77_Controller
/// \brief
///
////////////////////////////////////////////////////////////////////////////////

#include <native_service/frameworkunified_service_if.h>
#include <string>
//////////////////////////////////////////
/// Function : CFrameworkunifiedServiceInterface
//////////////////////////////////////////
CFrameworkunifiedServiceInterface::CFrameworkunifiedServiceInterface() {
}
//////////////////////////////////////////
/// Function : ~CFrameworkunifiedServiceInterface
//////////////////////////////////////////
CFrameworkunifiedServiceInterface::~CFrameworkunifiedServiceInterface() {
}
//////////////////////////////////////////
/// Function : OpenService
//////////////////////////////////////////
HANDLE CFrameworkunifiedServiceInterface::OpenService(const HANDLE f_hApp, std::string f_sServiceName) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");
  HANDLE l_hService = NULL;
  if (NULL == (l_hService = FrameworkunifiedOpenService(f_hApp, f_sServiceName.c_str()))) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, " Failed to open Service [%s]", f_sServiceName.c_str());
  }
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");
  return l_hService;
}
//////////////////////////////////////////
/// Function : CloseService
//////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedServiceInterface::CloseService(const HANDLE f_hApp, const HANDLE f_hService) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");
  if ((NULL == f_hApp) || (NULL == f_hService)) {
    return eFrameworkunifiedStatusInvldParam;
  }

  if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedCloseService(f_hApp, f_hService))) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "CloseService Failed");
  }

  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");
  return l_eStatus;
}
//////////////////////////////////////////
/// Function : OpenSession
//////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedServiceInterface::OpenSession(const HANDLE f_hApp, UI_32 f_uisessionType) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");
  if (NULL == f_hApp) {
    return eFrameworkunifiedStatusInvldParam;
  }

  l_eStatus = FrameworkunifiedOpenSessionWithData(f_hApp, (PVOID)&f_uisessionType, sizeof(UI_32));
  if (eFrameworkunifiedStatusOK != l_eStatus) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "FrameworkunifiedOpenSession Failed, eFrameworkunifiedStatusOK = 0x%x, l_eStatus = 0x%x", eFrameworkunifiedStatusOK,
           l_eStatus);
  }
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");
  return l_eStatus;
}
//////////////////////////////////////////
/// Function : OpenSessionAcknowledge
//////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedServiceInterface::OpenSessionAcknowledge(const HANDLE f_hApp, HANDLE &hSession) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");
  return l_eStatus;
}
//////////////////////////////////////////
/// Function : CloseSession
//////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedServiceInterface::CloseSession(const HANDLE f_hService, const HANDLE f_hSession) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");
  if ((NULL == f_hService) || (NULL == f_hSession)) {
    return eFrameworkunifiedStatusInvldParam;
  }

  if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedCloseSession(f_hService, f_hSession))) {
    FRAMEWORKUNIFIEDLOG0(ZONE_NS_ERR, __FUNCTION__, " Failed to send session close request");
  } else {
    FRAMEWORKUNIFIEDLOG0(ZONE_NS__CWORD77_, __FUNCTION__, " Session close request sent successfully");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");
  return l_eStatus;
}
//////////////////////////////////////////
/// Function : CloseSessionAcknowledge
//////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedServiceInterface::CloseSessionAcknowledge(const HANDLE hClient) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  CloseSessionAck tCloseAck;
  if (NULL == hClient) {
    return eFrameworkunifiedStatusNullPointer;
  }
  if (eFrameworkunifiedStatusOK == (l_eStatus = FrameworkunifiedGetMsgDataOfSize(hClient, &tCloseAck, sizeof(tCloseAck)))) {
    l_eStatus = tCloseAck.eStatus;
  } else {
    FRAMEWORKUNIFIEDLOG0(ZONE_NS_ERR,
            __FUNCTION__,
            "Failed to get message data:%x", l_eStatus);
  }
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");
  return l_eStatus;
}
