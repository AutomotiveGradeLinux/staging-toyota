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

////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_HMI_CWORD77_Controller
/// \brief
///
////////////////////////////////////////////////////////////////////////////////

#include <native_service/frameworkunified_service_protocol.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified__CWORD77__service_if.h>
#include <native_service/ns__CWORD77__data_pool_if.h>
#include <other_service/strlcpy.h>

#include <map>
#include <string>
#include <vector>

#include "ns__CWORD77__data_pool_internal.h"
#include "ns__CWORD77__common_internal.h"

#define NS__CWORD77__MAX_SESSION_RETRY_COUNT 10

//////////////////////////////////////////
/// Function : CFrameworkunified_CWORD77_Service
//////////////////////////////////////////
CFrameworkunified_CWORD77_Service::CFrameworkunified_CWORD77_Service()
  : m_cbResponse(NULL),
    m_cbSessionACK(NULL),
    m_pSession(NULL),
    m_cServiceName(""),
    m_bServiceAvailable(FALSE),
    m_ui8SessionRetryCount(0),
    m_hApp(NULL) {  // LCOV_EXCL_BR_LINE 11:except branch
  m_SessTypeToSessObj.clear();
}
//////////////////////////////////////////
/// Function : ~CFrameworkunified_CWORD77_Service
//////////////////////////////////////////
CFrameworkunified_CWORD77_Service::~CFrameworkunified_CWORD77_Service() {  // LCOV_EXCL_BR_LINE 11:except branch
  std::map<UI_32, CFrameworkunified_CWORD77_Session *>::iterator l_iter;
  l_iter = m_SessTypeToSessObj.begin();
  while (l_iter != m_SessTypeToSessObj.end()) {
    CFrameworkunified_CWORD77_Session *l_pTemp = l_iter->second;
    DELETEPTR(l_pTemp);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns__CWORD77__common.h"
    l_iter++;
  }
}
//////////////////////////////////////////
/// Function : SetResponseCallback
//////////////////////////////////////////
void CFrameworkunified_CWORD77_Service::SetResponseCallback(ResponseServiceTo_CWORD77_ cbResponse) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  m_cbResponse = cbResponse;
  if (m_cbResponse == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __PRETTY_FUNCTION__, "Call back function ptr is NULL");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  }
  std::map<UI_32, CFrameworkunified_CWORD77_Session *>::iterator l_iter;
  l_iter = m_SessTypeToSessObj.begin();
  while (l_iter != m_SessTypeToSessObj.end()) {
    if (l_iter->second != NULL) {
      l_iter->second->SetResponseCallback(m_cbResponse);
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, " Object ptr for session type[%d] is NULL ", l_iter->first);
    }
    l_iter++;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
}
//////////////////////////////////////////
/// Function : SetSession
//////////////////////////////////////////
void CFrameworkunified_CWORD77_Service::SetSession(CFrameworkunified_CWORD77_Session *f_pSessionObj) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  m_SessTypeToSessObj[f_pSessionObj->GetSessionType()] = f_pSessionObj;
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
}
//////////////////////////////////////////
/// Function : SendMessageToSession
//////////////////////////////////////////
void CFrameworkunified_CWORD77_Service::SendMessageToSession(UI_32 f_uiSessionType, UI_32 f_uiSrvProtocol) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");
  std::map<UI_32, CFrameworkunified_CWORD77_Session *>::iterator l_iter;
  l_iter = m_SessTypeToSessObj.begin();
  l_iter = m_SessTypeToSessObj.find(f_uiSessionType);
  if (l_iter != m_SessTypeToSessObj.end()) {
    if (l_iter->second != NULL) {
      if (eFrameworkunifiedStatusSessionErr == l_iter->second->HandleSessionMessage(f_uiSrvProtocol)) {
        if (m_bServiceAvailable) {
          if (NS__CWORD77__MAX_SESSION_RETRY_COUNT < m_ui8SessionRetryCount) {
            FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, "Unable to send cmd 0x%X."
                   " But, service is available. Open session req max limit reached [%s].",
                   f_uiSrvProtocol, m_cServiceName.c_str());
            // TODO(framework_unified): what to do in case of max limit reached?
          } else {
            FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, "Unable to send cmd 0x%X."
                   " But, service is available. Re-sending open session req [%s].",
                   f_uiSrvProtocol, m_cServiceName.c_str());
            // send open session request
            OpenSessionRetry(f_uiSessionType);
            m_ui8SessionRetryCount++;
          }
        } else {
          FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, "Unable to send cmd 0x%X."
                 " Service not yet available.", f_uiSrvProtocol);
        }
      } else {
        // ignore error, already printed by HandleSessionMessage
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, " Object ptr for session type[%d] is NULL ", l_iter->first);
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, " Object is not there in Map for session type %d ", f_uiSessionType);
  }
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");
}
//////////////////////////////////////////
/// Function : OpenSession
//////////////////////////////////////////
void CFrameworkunified_CWORD77_Service::OpenSession(HANDLE f_hApp, const std::string &f_sServiceName) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __PRETTY_FUNCTION__, "+");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  m_cServiceName = f_sServiceName;
  std::map<UI_32, CFrameworkunified_CWORD77_Session *>::iterator l_iter;
  l_iter = m_SessTypeToSessObj.begin();
  while (l_iter != m_SessTypeToSessObj.end()) {
    if (l_iter->second != NULL) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, " Sent open session request for session %d ", l_iter->first);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
      l_iter->second->OpenSession(f_hApp, f_sServiceName);  // LCOV_EXCL_BR_LINE 11:except branch
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, " Object ptr for session type[%d] is NULL ", l_iter->first);
    }
    l_iter++;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __PRETTY_FUNCTION__, "-");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
}
//////////////////////////////////////////
/// Function : CloseSession
//////////////////////////////////////////
void CFrameworkunified_CWORD77_Service::CloseSession(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  std::map<UI_32, CFrameworkunified_CWORD77_Session *>::iterator l_iter;
  l_iter = m_SessTypeToSessObj.begin();
  while (l_iter != m_SessTypeToSessObj.end()) {
    if (l_iter->second != NULL) {
      l_iter->second->CloseSession(f_hApp);
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, " Object ptr for session type[%d] is NULL ", l_iter->first);
    }
    l_iter++;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
}
//////////////////////////////////////////
/// Function : OpenSessionAcks
//////////////////////////////////////////
void CFrameworkunified_CWORD77_Service::OpenSessionAcks(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");
  OpenSessionAck tOpenSessAck;

  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  m_ui8SessionRetryCount = 0;

  if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedGetMsgDataOfSize(f_hApp, &tOpenSessAck, sizeof(tOpenSessAck)))) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, " FrameworkunifiedGetMsgDataOfSize Failed");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
    l_eStatus = eFrameworkunifiedStatusFail;
  }
  // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __PRETTY_FUNCTION__, "/***********************************************************************/");
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_,
         __PRETTY_FUNCTION__,
         "Open session ACK obtained for service: [%s] with session type: [%d]",
         tOpenSessAck.cSessionName != 0 ? tOpenSessAck.cSessionName : NULL,
         tOpenSessAck.sessionType);
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __PRETTY_FUNCTION__, "/***********************************************************************/");
  // LCOV_EXCL_BR_STOP
  UI_32 l_uisessionType = tOpenSessAck.sessionType;
  /// Session Acks
  std::map<UI_32, CFrameworkunified_CWORD77_Session *>::iterator l_iter;
  l_iter = m_SessTypeToSessObj.begin();
  while (l_iter != m_SessTypeToSessObj.end()) {
    if (l_iter->second != NULL) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __PRETTY_FUNCTION__, "Get session type = %d", l_iter->second->GetSessionType());  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
      if (l_uisessionType == l_iter->second->GetSessionType()) {
        l_iter->second->OpenSessionAcknowledge(f_hApp);

        if (m_cbSessionACK != NULL) {
          (VOID)m_cbSessionACK(f_hApp, m_cServiceName, tOpenSessAck.cSessionName, tOpenSessAck.sessionType,
                               tOpenSessAck.eStatus);
        }
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR,
               __FUNCTION__,
               "Unexpected Session_Type from service [%s]. Obtained Session_Type: [%d] != Expected Session_Type: [%d]",
               tOpenSessAck.cSessionName != 0 ? tOpenSessAck.cSessionName : NULL,
               l_uisessionType,
               l_iter->second->GetSessionType());  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, " Object ptr for session type[%d] is NULL ", l_iter->first);
    }
    l_iter++;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
}
//////////////////////////////////////////
/// Function : OpenServiceOnAvailability
//////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunified_CWORD77_Service::OpenServiceOnAvailability(HANDLE f_hApp, const std::string &f_sServiceName) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  m_hApp = f_hApp;
  m_bServiceAvailable = TRUE;
  m_ui8SessionRetryCount = 0;
  AttachOpenSessionAckCallback(f_hApp);
  OpenSession(f_hApp, f_sServiceName);
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  return eStatus;
}
//////////////////////////////////////////
/// Function : CloseServiceOnUnavailability
//////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunified_CWORD77_Service::CloseServiceOnUnavailability(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  m_bServiceAvailable = FALSE;
  m_ui8SessionRetryCount = 0;
  CloseSession(f_hApp);
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  return eStatus;
}

//////////////////////////////////////////
/// Function : AddNotification
//////////////////////////////////////////
void CFrameworkunified_CWORD77_Service::AddNotification(PCSTR f_pNotification) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  FrameworkunifiedNotificationCallbackHandler tNotifyStruct = {};
  /**
   * @todo
   * Defining a NULL for argument f_pNotification results in a segmentation fault.
   */
  strlcpy(tNotifyStruct.cNotification, f_pNotification, sizeof(tNotifyStruct.cNotification));  // LCOV_EXCL_BR_LINE 11:except branch
  tNotifyStruct.callBack  = CCallback< CFrameworkunified_CWORD77_Service, &CFrameworkunified_CWORD77_Service::OnServiceNtf >::set(this);  // LCOV_EXCL_BR_LINE 11:except branch
  m_vServiceNotifications.push_back(tNotifyStruct);  // LCOV_EXCL_BR_LINE 11:except branch
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
}

//////////////////////////////////////////
/// Function : SubscribeNotifications
//////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunified_CWORD77_Service::SubscribeNotifications(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  size_t Size = m_vServiceNotifications.size();
  for (size_t i = 0; i < Size; i++) {
    if (m_vServiceNotifications[i].callBack != NULL) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "Before subscribing notifications");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
      if (eFrameworkunifiedStatusOK != FrameworkunifiedSubscribeNotificationWithCallback(f_hApp, m_vServiceNotifications[i].cNotification,
                                                               m_vServiceNotifications[i].callBack)) {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "FrameworkunifiedSubscribeNotificationWithCallback fail");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
      }
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  return l_eStatus;
}
//////////////////////////////////////////
/// Function : UnSubscribeNotifications
//////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunified_CWORD77_Service::UnSubscribeNotifications(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  size_t Size = m_vServiceNotifications.size();
  for (size_t i = 0; i < Size; i++) {
    if (eFrameworkunifiedStatusOK != FrameworkunifiedUnsubscribeNotificationWithCallback(f_hApp, m_vServiceNotifications[i].cNotification)) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "FrameworkunifiedUnsubscribeNotificationWithCallback fail");
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");
  return l_eStatus;
}

//////////////////////////////////////////
/// Function : OnServiceNtf
//////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunified_CWORD77_Service::OnServiceNtf(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  // CHAR l_cNotificationName[MAX_SYS_INFO_SIZE] = {};
  PCSTR l_cNotificationName = FrameworkunifiedGetLastNotification(hApp);
  // retrieve notification name
  // FrameworkunifiedGetSystemInfo(hApp, l_cNotificationName);

  UI_32 l_uiLength = FrameworkunifiedGetMsgLength(hApp);
  if (l_uiLength != 0) {
    std::vector<char> data;
    data.resize(l_uiLength);
    if (eFrameworkunifiedStatusOK == (l_eStatus = FrameworkunifiedGetMsgDataOfSize(hApp, &data[0], l_uiLength))) {
      SetRespNotfnDataIn_CWORD77_DataPool(l_cNotificationName, static_cast<UI_32>(data.size()), &data[0]);
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, " FrameworkunifiedGetMsgDataOfSize Error, "
        "Resp Notfn Data not set in _CWORD77_ data pool, status: 0x%x", l_eStatus);
    }
  } else {
    SetRespNotfnDataIn_CWORD77_DataPool(l_cNotificationName, 0, NULL);
  }

  m_cbResponse(hApp, NOTIFICATION, m_cServiceName);

  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");

  return l_eStatus;
}

//////////////////////////////////////////
/// Function : OpenSessionRetry
//////////////////////////////////////////
void CFrameworkunified_CWORD77_Service::OpenSessionRetry(UI_32 f_ui32SessionType) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_FUNC, __FUNCTION__, "+");
  if (!m_cServiceName.empty() && (NULL != m_hApp)) {
    std::map<UI_32, CFrameworkunified_CWORD77_Session *>::iterator l_iter;
    l_iter = m_SessTypeToSessObj.find(f_ui32SessionType);
    if (l_iter != m_SessTypeToSessObj.end()) {
      if (NULL != l_iter->second) {
        FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "Re-sending open session req for session type %d ", l_iter->first);
        l_iter->second->OpenSession(m_hApp, m_cServiceName);
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Object ptr for session type[%d] is NULL ", l_iter->first);
      }
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Can't retry for open session req. Service name: [%s], hApp:[%p].",
           m_cServiceName.c_str(),
           m_hApp);
  }
  FRAMEWORKUNIFIEDLOG(ZONE_NS_FUNC, __FUNCTION__, "-");
}

//////////////////////////////////////////
/// Function : Set_CWORD77_OpenSessionACK
//////////////////////////////////////////
void CFrameworkunified_CWORD77_Service::Set_CWORD77_OpenSessionACK(SessionAckTo_CWORD77_ cbResponse) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"

  m_cbSessionACK = cbResponse;
  if (NULL == m_cbSessionACK) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __PRETTY_FUNCTION__, "Call back function ptr is NULL");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  }

  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
}
