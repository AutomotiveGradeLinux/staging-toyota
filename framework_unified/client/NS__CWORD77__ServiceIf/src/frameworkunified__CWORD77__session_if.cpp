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

#include <native_service/frameworkunified__CWORD77__session_if.h>
#include <native_service/ns__CWORD77__data_pool_if.h>
#include <boost/bind.hpp>
#include <utility>
#include <vector>
#include "ns__CWORD77__data_pool_internal.h"
#include "ns__CWORD77__common_internal.h"

//////////////////////////////////////////
/// Function : CFrameworkunified_CWORD77_Session
//////////////////////////////////////////
CFrameworkunified_CWORD77_Session::CFrameworkunified_CWORD77_Session() {
  m_hSession = NULL;
  m_hApp = NULL;
  m_hService = NULL;
  m_uiSessionType = 0;
}
//////////////////////////////////////////
/// Function : ~CFrameworkunified_CWORD77_Session
//////////////////////////////////////////
CFrameworkunified_CWORD77_Session::~CFrameworkunified_CWORD77_Session() {  // LCOV_EXCL_BR_LINE 11:except branch
}

void CFrameworkunified_CWORD77_Session::SetSessionType(const UI_32 &sessioType) {
  m_uiSessionType = sessioType;
}

//////////////////////////////////////////
/// Function : GetSessionType
//////////////////////////////////////////
UI_32 CFrameworkunified_CWORD77_Session::GetSessionType() {
  return m_uiSessionType;
}
//////////////////////////////////////////
/// Function : SetResponseCallback
//////////////////////////////////////////
void CFrameworkunified_CWORD77_Session::SetResponseCallback(ResponseServiceTo_CWORD77_ f_cbResponse) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");   // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  m_ResponseTo_CWORD77_ = f_cbResponse;
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");   // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
}
//////////////////////////////////////////
/// Function : AddProtocolHandler
//////////////////////////////////////////
void CFrameworkunified_CWORD77_Session::AddProtocolHandler(UI_32 f_uiCmdId, CbFuncPtr f_cbFuncPtr) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");
  FrameworkunifiedProtocolCallbackHandler l_tProtocolCallback;
  l_tProtocolCallback.iCmd = f_uiCmdId;
  l_tProtocolCallback.callBack = f_cbFuncPtr;
  m_vSessionProtocolHanlder.push_back(l_tProtocolCallback);
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");
}
//////////////////////////////////////////
/// Function : AddEventHandler
//////////////////////////////////////////
void CFrameworkunified_CWORD77_Session::AddEventHandler(UI_32 f_uiCmdId, CbFuncPtr f_cbFuncPtr) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");
  FrameworkunifiedProtocolCallbackHandler l_tProtocolCallback;
  l_tProtocolCallback.iCmd = f_uiCmdId;
  l_tProtocolCallback.callBack = f_cbFuncPtr;
  m_vSessionEventHandler.push_back(l_tProtocolCallback);
  m_vEventVector.push_back(f_uiCmdId);  // Code added by Shyam
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");
}
//////////////////////////////////////////
/// Function : AttachProtocolwithDispatcher
//////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunified_CWORD77_Session::AttachProtocolwithDispatcher(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  size_t l_uiSize = m_vSessionProtocolHanlder.size();
  PCSTR l_pRequester   = FrameworkunifiedGetMsgSrc(f_hApp);
  for (size_t i = 0; i < l_uiSize; i++) {
    FrameworkunifiedAttachCallbackToDispatcher(f_hApp, l_pRequester, m_vSessionProtocolHanlder[i].iCmd,
                                  m_vSessionProtocolHanlder[i].callBack, m_hSession);
  }
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");
  return l_eStatus;
}
//////////////////////////////////////////
/// Function : DetachProtocolwithDispatcher
//////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunified_CWORD77_Session::DetachProtocolwithDispatcher(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  size_t l_uiSize = m_vSessionProtocolHanlder.size();
  PCSTR l_pRequester   = FrameworkunifiedGetMsgSrc(f_hApp);
  for (size_t i = 0; i < l_uiSize; i++) {
    FrameworkunifiedDetachCallbackFromDispatcher(f_hApp, l_pRequester, m_vSessionProtocolHanlder[i].iCmd,
                                    m_hSession);
  }
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");
  return l_eStatus;
}
//////////////////////////////////////////
/// Function : AttachEventswithDispatcher
//////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunified_CWORD77_Session::AttachEventswithDispatcher(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  size_t l_uiSize = m_vEventVector.size();

  for (size_t i = 0; i < l_uiSize; i++) {
    l_eStatus = FrameworkunifiedRegisterEvent(m_hSession, m_vEventVector[i]);
  }
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");
  return l_eStatus;
}
//////////////////////////////////////////
/// Function : AttachEventswithDispatcher
//////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunified_CWORD77_Session::RegisterEvents(HANDLE f_hApp) {
  // Function: added by Shyam to overcome the naming conflict in registering the event
  return AttachEventswithDispatcher(f_hApp);
}

//////////////////////////////////////////
/// Function : AttachEventHandlerswithDispatcher
//////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunified_CWORD77_Session::AttachEventHandlerswithDispatcher(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  size_t l_uiSize = m_vSessionEventHandler.size();
  PCSTR l_pRequester   = FrameworkunifiedGetMsgSrc(f_hApp);
  for (size_t i = 0; i < l_uiSize; i++) {
    FrameworkunifiedAttachCallbackToDispatcher(f_hApp, l_pRequester, m_vSessionEventHandler[i].iCmd,
                                  m_vSessionEventHandler[i].callBack, m_hSession);
  }
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");
  return l_eStatus;
}
//////////////////////////////////////////
/// Function : DetachEventHandlerswithDispatcher
//////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunified_CWORD77_Session::DetachEventHandlerswithDispatcher(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  size_t l_uiSize = m_vSessionEventHandler.size();
  PCSTR l_pRequester   = FrameworkunifiedGetMsgSrc(f_hApp);
  for (size_t i = 0; i < l_uiSize; i++) {
    FrameworkunifiedDetachCallbackFromDispatcher(f_hApp, l_pRequester, m_vSessionEventHandler[i].iCmd,
                                    m_hSession);
  }
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");
  return l_eStatus;
}

//////////////////////////////////////////
/// Function : HandleSessionMessage
//////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunified_CWORD77_Session::HandleSessionMessage(UI_32 cmdID) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  UI_32 size = 0;
  PVOID data = NULL;

  if (NULL != m_hSession && NULL != m_hService) {
    eStatus = GetReqDataFrom_CWORD77_DataPool(std::make_pair(cmdID, FrameworkunifiedGetSessionName(m_hService)), size, data);

    if (eFrameworkunifiedStatusOK == eStatus) {
      eStatus = FrameworkunifiedSendMsg(m_hSession, cmdID, size, data);
      if (eFrameworkunifiedStatusOK != eStatus) {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error: FrameworkunifiedSendMsg() returned %d", eStatus);
      }
    } else if (eFrameworkunifiedStatusDbRecNotFound == eStatus) {
      // FIXME: This is added to help, if there is any command with no data. So, that services
      //      don't need to provide _CWORD77_ interfaces for such command.
      //       Correct way to solve the problem is, let services provide one _CWORD77_ interface
      //     which will write 0 length in data pool.
      // Impact: This may have one impact that, service is expecting some data, but
      //      _CWORD77_ sends data with 0 length because it didn't find anything in the data pool.
      FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__,
             "GetReqDataFrom_CWORD77_DataPool() returned %d. But, still sending cmd 0x%X to service %s with length 0.",
             eStatus, cmdID, FrameworkunifiedGetSessionName(m_hService));
      eStatus = FrameworkunifiedSendMsg(m_hSession, cmdID, 0, NULL);
      if (eFrameworkunifiedStatusOK != eStatus) {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error: FrameworkunifiedSendMsg() returned %d", eStatus);
      }
      // end fixme
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__,
             "GetReqDataFrom_CWORD77_DataPool() returned %d. Can't send cmd 0x%X to service %s.",
             eStatus, cmdID, FrameworkunifiedGetSessionName(m_hService));
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__,
           "Error: HandleSessionMessage: Error sending cmd 0x%X request to service."
           " Session handle %p Service Handle %p", cmdID, m_hSession, m_hService);
    if (NULL == m_hSession) {
      eStatus = eFrameworkunifiedStatusSessionErr;
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "- %d", eStatus);
  return eStatus;
}

//////////////////////////////////////////
/// Function : AttachResponseCallbacks
//////////////////////////////////////////
void CFrameworkunified_CWORD77_Session::AttachResponseCallbacks(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");   // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  UI_32 l_uiCount = 0;
  PCSTR l_pRequester   = FrameworkunifiedGetMsgSrc(f_hApp);

  // attach response protocols
  UI_32 const *l_uiResponseProtocol = GetResponseProtocolArray();
  if (NULL != l_uiResponseProtocol) {
    for (; l_uiCount < GetResponseProtocolArrayLength(); l_uiResponseProtocol++, l_uiCount++) {
      FrameworkunifiedAttachCallbackToDispatcher(f_hApp, l_pRequester, *l_uiResponseProtocol,
                                    boost::bind(&CFrameworkunified_CWORD77_Session::OnResponse, this, f_hApp), m_hSession);
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "No Response protocols available for session with %s",
           FrameworkunifiedGetSessionName(m_hService));
  }

  // attach events
  l_uiCount = 0;
  UI_32 const *l_uiEvents = GetEventArray();
  if (NULL != l_uiEvents) {
    for (; l_uiCount < GetEventArrayLength(); l_uiEvents++, l_uiCount++) {
      // attach callback
      FrameworkunifiedAttachCallbackToDispatcher(f_hApp, l_pRequester, *l_uiEvents,
                                    boost::bind(&CFrameworkunified_CWORD77_Session::OnResponse, this, f_hApp), m_hSession);  // LCOV_EXCL_BR_LINE 11:except branch
      // register event
      FrameworkunifiedRegisterEvent(m_hSession, *l_uiEvents);
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "No event protocols available for session with %s",
      FrameworkunifiedGetSessionName(m_hService));
  }

  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");   // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
}

//////////////////////////////////////////
/// Function : DetachResponseCallbacks
//////////////////////////////////////////
void CFrameworkunified_CWORD77_Session::DetachResponseCallbacks(HANDLE f_hApp) {
  if (NULL != m_hSession) {
    UI_32 l_uiCount = 0;
    PCSTR l_cSessionName   = FrameworkunifiedGetSessionName(m_hSession);
    // detach response protocols
    UI_32 const *l_uiResponseProtocol = GetResponseProtocolArray();
    if (NULL != l_uiResponseProtocol) {
      for (; l_uiCount < GetResponseProtocolArrayLength(); l_uiResponseProtocol++, l_uiCount++) {
        FrameworkunifiedDetachCallbackFromDispatcher(f_hApp, l_cSessionName, *l_uiResponseProtocol, m_hSession);
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "No Response protocols available for session with %s",
             FrameworkunifiedGetSessionName(m_hService));
    }

    // detach and unregister events
    l_uiCount = 0;
    UI_32 const *l_uiEvents = GetEventArray();
    if (NULL != l_uiEvents) {
      for (; l_uiCount < GetEventArrayLength(); l_uiEvents++, l_uiCount++) {
        FrameworkunifiedDetachCallbackFromDispatcher(f_hApp, l_cSessionName, *l_uiEvents, m_hSession);
        FrameworkunifiedUnRegisterEvent(m_hSession, *l_uiEvents);
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "No event protocols available for session with %s",
        FrameworkunifiedGetSessionName(m_hService));
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "Detach not required. Session not yet available");
  }
}

//////////////////////////////////////////
/// Function : OnResponse
//////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunified_CWORD77_Session::OnResponse(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  UI_32 length = FrameworkunifiedGetMsgLength(f_hApp);
  UI_32 cmdId = FrameworkunifiedGetMsgProtocol(f_hApp);
  PCSTR l_cMsgSourceName = FrameworkunifiedGetMsgSrc(f_hApp);
  if (NULL != l_cMsgSourceName) {
    if (length != 0) {
      std::vector<char> data;
      data.resize(length);
      eStatus = FrameworkunifiedGetMsgDataOfSize(f_hApp, &data[0], length);
      FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "OnResponse from %s cmdId:%d", l_cMsgSourceName, cmdId);
      SetRespDataIn_CWORD77_DataPool(std::make_pair(cmdId, l_cMsgSourceName), static_cast<UI_32>(data.size()), &data[0]);
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "OnResponse from %s cmdId:[%d], Value: [NULL]", l_cMsgSourceName, cmdId);
      SetRespDataIn_CWORD77_DataPool(std::make_pair(cmdId, l_cMsgSourceName), 0, NULL);
    }

    m_ResponseTo_CWORD77_(f_hApp, RESPONSE, l_cMsgSourceName);
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error: Setting response value of cmd 0x%X in data pool.", cmdId);
  }

  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "- %d", eStatus);
  return eStatus;
}

