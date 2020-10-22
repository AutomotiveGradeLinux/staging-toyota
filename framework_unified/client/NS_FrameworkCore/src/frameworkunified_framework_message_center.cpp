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
/// \ingroup  tag_NSFramework
/// \brief
///
///
///
///////////////////////////////////////////////////////////////////////////////
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/ns_message_center_if.h>
#include <other_service/strlcpy.h>
#include "frameworkunified_framework_core.h"
#include "ns_mc_internal.h"

using std::malloc;
using std::free;
using std::strcpy;
using std::strlen;
using std::memset;

extern __thread HANDLE responseWaitQ;

//////////////////////////////////////////
// Function : FrameworkunifiedMcOpenSender
//////////////////////////////////////////
HANDLE FrameworkunifiedMcOpenSender(HANDLE hApp, PCSTR pName) {
  MsgQInfo *pMsgQ = NULL;

  if ((frameworkunifiedCheckValidAppHandle(hApp)) && (NULL != pName)) {
    UI_32 l_ui32SrvNameLen = static_cast<UI_32>(strlen(pName));
    /**
     * @todo
     *  CHAR cSessionName[MAX_QUEUE_NAME_SIZE] cannot be determined to be 20 bytes including the termination NULL.
     */
    if ((l_ui32SrvNameLen < MAX_NAME_SIZE_APP) && (l_ui32SrvNameLen > 0)) {
      CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
      pMsgQ = reinterpret_cast<MsgQInfo *>(malloc(sizeof(MsgQInfo)));

      if (NULL != pMsgQ) {  // LCOV_EXCL_BR_LINE 5:malloc's error case.
        errno = EOK;  // flush previous errors, if any.
        memset(pMsgQ, 0, sizeof(MsgQInfo));
        if (NULL == (pMsgQ->hMsgQ = McOpenSender(pName))) {
          FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__,
                 "Error::errno:%d, hApp:0x%p, %s failed to McOpenSender",
                 errno, hApp, pName);
          free(pMsgQ);
          pMsgQ = NULL;
        } else {
          pMsgQ->checkCode = MSGQ_CHECK_CODE;
          pMsgQ->self = pthread_self();
          strlcpy(pMsgQ->cSrcName, pApp->cAppName, sizeof(pMsgQ->cSrcName));
          strlcpy(pMsgQ->cMsgQName, pName, sizeof(pMsgQ->cMsgQName));
          pMsgQ->sessionId = 0;
        }
      }
    } else {
      // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__,
             "Invalid param. Name:%s, Following must be true: %d > len of passed name(%u) > 0.",
             pName, MAX_NAME_SIZE_APP, l_ui32SrvNameLen);
      // LCOV_EXCL_BR_STOP
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__,
           "Invalid param. hApp:0x%p, name:0x%p",
           hApp, pName);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  }

  return static_cast<HANDLE>(pMsgQ);
}


//////////////////////////////////////////
// Function : FrameworkunifiedMcClose
//////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedMcClose(HANDLE hService) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  if (frameworkunifiedCheckValidMsgQ(hService)) {
    MsgQInfo *pMsgQ    = reinterpret_cast<MsgQInfo *>(hService);
    pMsgQ->checkCode = 0;
    if (NULL != pMsgQ->hMsgQ) {
      eStatus = McClose(pMsgQ->hMsgQ);
      pMsgQ->hMsgQ = NULL;
    }
    free(pMsgQ);
    pMsgQ = NULL;
    hService = NULL;  // This has no effect because hService is not passed as reference.
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }
  return eStatus;
}


//////////////////////////////////////////
// Function : FrameworkunifiedSendMsg
//////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSendMsg(HANDLE hService, UI_32 iCmd, UI_32 length, PCVOID data) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  if (frameworkunifiedCheckValidMsgQ(hService)) {
    MsgQInfo *pMsgQ = reinterpret_cast<MsgQInfo *>(hService);

    if ((NULL != pMsgQ->hMsgQ) && (0 != std::strlen(pMsgQ->cSrcName))) {
      eStatus = McSendWithSession(pMsgQ->hMsgQ, pMsgQ->cSrcName, iCmd, length, data, pMsgQ->sessionId);
    } else {
      eStatus = eFrameworkunifiedStatusInvldHandle;
    }
  } else {
    eStatus = eFrameworkunifiedStatusNullPointer;
  }
  // check if McSendWithSession failed with error destination msg queue full
  // log the information
  if (eFrameworkunifiedStatusMsgQFull == eStatus) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error!! Message queue full of receiver '%s' while sending CmdId = 0x%X",
           FrameworkunifiedGetSessionName(hService), iCmd);
  }

  return eStatus;
}

//////////////////////////////////////////
// Function : FrameworkunifiedSendPriorityMsg
//////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSendPriorityMsg(HANDLE hService, UI_32 iCmd, UI_32 length, PCVOID data) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  if (frameworkunifiedCheckValidMsgQ(hService)) {
    MsgQInfo *pMsgQ    = reinterpret_cast<MsgQInfo *>(hService);
    eStatus = McSendWithPriority(pMsgQ->hMsgQ, pMsgQ->cSrcName, iCmd,
      length, data, eFrameworkunifiedMsgPrioEmergency, pMsgQ->sessionId);
  } else {
    eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return eStatus;
}

//////////////////////////////////////////
// Function: FrameworkunifiedSendSelf
//////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSendSelf(HANDLE hApp, UI_32 iCmd, UI_32 length, PCVOID data) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = reinterpret_cast< CFrameworkunifiedFrameworkApp * >(hApp);
    eStatus = McSend(pApp->hAppSndMsgQ, &pApp->cAppName[ 0 ], iCmd, length, data);
  } else {
    eStatus = eFrameworkunifiedStatusNullPointer;
  }
  return eStatus;
}

//////////////////////////////////////////
// Function : FrameworkunifiedInvokeSync
//////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedInvokeSync(HANDLE hService, UI_32 iCmd, UI_32 msgLenght, PCVOID msgData, UI_32 responseLength,
                         PVOID responseData, UI_32 *receivedLength) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidMsgQ(hService)) {
    if (NULL != receivedLength) {
      MsgQInfo *pMsgQ = static_cast<MsgQInfo *>(hService);

      if (responseWaitQ == NULL) {
        char mc_invoker_name[MAX_QUEUE_NAME_SIZE];
        InvokerName invokerName;
        // LCOV_EXCL_BR_START 11:except branch
        McCreateInvokerName(pMsgQ->cSrcName, pMsgQ->sessionId, mc_invoker_name, sizeof(mc_invoker_name));
        // LCOV_EXCL_BR_STOP
        invokerName = mc_invoker_name;

        responseWaitQ = McOpenSyncReceiver(invokerName.c_str());
        if (NULL == responseWaitQ) {
          eStatus = eFrameworkunifiedStatusFail;
        }
      }

      if (eStatus == eFrameworkunifiedStatusOK) {
        eStatus = McInvokeSync(pMsgQ->hMsgQ, pMsgQ->cSrcName, iCmd, msgLenght, msgData, pMsgQ->sessionId,
                               responseWaitQ, responseLength, responseData, receivedLength);
      }
    } else {
      eStatus = eFrameworkunifiedStatusInvldParam;
    }
  } else {
    eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return eStatus;
}

//////////////////////////////////////////
// Function : frameworkunifiedSendSyncResponse
//////////////////////////////////////////
EFrameworkunifiedStatus frameworkunifiedSendSyncResponse(HANDLE hApp, UI_32 iCmd, UI_32 seq_id, EFrameworkunifiedStatus retValue, UI_32 length, PCVOID data) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    HANDLE responseSendQ;

    char mc_invoker_name[MAX_QUEUE_NAME_SIZE];
    InvokerName invokerName;
    PVOID rcv_buf = static_cast<PVOID>(pApp->uiMsgRcvBuffer);
    McCreateInvokerName(McGetMsgSrc(rcv_buf), mcGetMsgSsessionId(rcv_buf), mc_invoker_name, sizeof(mc_invoker_name));
    invokerName = mc_invoker_name;

    InvokeResponseQTable::iterator s_iterator = pApp->invokeResQTable.find(invokerName);
    if (s_iterator != pApp->invokeResQTable.end()) {
      responseSendQ = s_iterator->second;
    } else {
      responseSendQ = McOpenSyncSender(invokerName.c_str());  // LCOV_EXCL_BR_LINE 11:except branch
      /*
       * @todo
       * FrameworkunifiedSendSyncResponse() uses McOpenSyncSender() to create a synchronous communication response queue handle,
       * but there is no process to release the queue handle after the response.
       */
      if (NULL == responseSendQ) {
        eStatus = eFrameworkunifiedStatusFail;
      } else {
        pApp->invokeResQTable[invokerName] = responseSendQ;  // LCOV_EXCL_BR_LINE 11:except branch
      }
    }

    if (eStatus == eFrameworkunifiedStatusOK) {
      eStatus = McSendSyncResponse(responseSendQ, pApp->cAppName, iCmd, seq_id, retValue, length, data);
    }
  } else {
    eStatus = eFrameworkunifiedStatusNullPointer;
  }
  return eStatus;
}

//////////////////////////////////////////
// Function : FrameworkunifiedDispatchProcess
//////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSetSyncResponseData(HANDLE hApp, PVOID data, UI_32 size) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  /*
   * @todo
   * When the response data is 0 bytes, it can be transmitted as SYNC data.
   */
  if (frameworkunifiedCheckValidAppHandle(hApp) && data != NULL) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    try {
      pApp->responsedata.set(data, size);  // LCOV_EXCL_BR_LINE 11:except branch
    }
    catch(std::bad_alloc) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __PRETTY_FUNCTION__, "bad_alloc");
      eStatus = eFrameworkunifiedStatusFail;
    }
  } else {
    eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return eStatus;  // LCOV_EXCL_BR_LINE 11:except branch
}

//////////////////////////////////////////
// Function : FrameworkunifiedGetMsgLength
//////////////////////////////////////////
UI_32 FrameworkunifiedGetMsgLength(HANDLE hApp) {
  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    return McGetLength(static_cast<PVOID>(pApp->uiMsgRcvBuffer));
  }
  return 0;
}


//////////////////////////////////////////
// Function : FrameworkunifiedGetMsgProtocol
//////////////////////////////////////////
UI_32 FrameworkunifiedGetMsgProtocol(HANDLE hApp) {
  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    return static_cast<UI_32>(pApp->uiProtocolCmd);
  }
  return 0;
}


//////////////////////////////////////////
// Function : FrameworkunifiedGetMsgDataOfSize
//////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedGetMsgDataOfSize(HANDLE hApp, PVOID pData, UI_32 uiSize, ESMRetrieveTypes eRetrieveMethod) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    /*
     * @todo
     * The behavior is the same as when "eSMRRetain" is set without getting an error when the ESMRetrieveTypes is not appropriate.
     */
    if (eSMRRelease == eRetrieveMethod) {
      eStatus = McGetDataOfSize(static_cast<PVOID>(pApp->uiMsgRcvBuffer), pData, uiSize);
    } else {
      eStatus = McGetDataOfSizeWithSMRetain(static_cast<PVOID>(pApp->uiMsgRcvBuffer), pData, uiSize);
    }
  } else {
    eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return eStatus;
}


//////////////////////////////////////////
// Function : FrameworkunifiedGetDataPointer
//////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedGetDataPointer(HANDLE hApp, void **datap) {
  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    if (NULL == datap) {
      return eFrameworkunifiedStatusInvldParam;
    }

    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    void *dt;
    if ((dt = McGetDataPointer(reinterpret_cast<PVOID>(pApp->uiMsgRcvBuffer))) == NULL) {
      return eFrameworkunifiedStatusInvldBufSize;
    }

    *datap = dt;
    return eFrameworkunifiedStatusOK;
  }
  return eFrameworkunifiedStatusInvldHandle;
}


//////////////////////////////////////////
// Function : FrameworkunifiedGetMsgSrc
//////////////////////////////////////////
PCSTR FrameworkunifiedGetMsgSrc(HANDLE hApp) {
  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    return (reinterpret_cast<PCSTR>(pApp->cMsgSrcName));
  }
  return NULL;
}


//////////////////////////////////////////
// Function : FrameworkunifiedGetMsgNotification
//////////////////////////////////////////
PCSTR FrameworkunifiedGetMsgNotification(HANDLE hApp) {
  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    return (reinterpret_cast<PCSTR>(pApp->cSystemInfo));
  }
  return NULL;
}


//////////////////////////////////////////
// Function : FrameworkunifiedClearMsgData
//////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedClearMsgData(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    eStatus = McClearData(static_cast<PVOID>(pApp->uiMsgRcvBuffer));
  } else {
    eStatus = eFrameworkunifiedStatusNullPointer;
  }
  return eStatus;
}


//////////////////////////////////////////
// Function : FrameworkunifiedGetDataUSID
//////////////////////////////////////////
TMemID FrameworkunifiedGetDataUSID(HANDLE hApp) {
  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    return McGetDataUSID(static_cast<PVOID>(pApp->uiMsgRcvBuffer));
  }
  return BAD_MEM_ID;
}


//////////////////////////////////////////
// Function : FrameworkunifiedForwardMessage
//////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedForwardMessage(HANDLE hApp, HANDLE hChildQ, UI_32 iCmd) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusInvldBuf;

  if (frameworkunifiedCheckValidAppHandle(hApp) && hChildQ) {
    CFrameworkunifiedFrameworkApp *pApp = reinterpret_cast< CFrameworkunifiedFrameworkApp * >(hApp);

    eStatus = McForward(hChildQ, &pApp->cAppName[ 0 ], iCmd, FrameworkunifiedGetDataUSID(hApp));
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  return eStatus;
}


//////////////////////////////////////////
// Function : FrameworkunifiedGetSystemInfo
//////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedGetSystemInfo(HANDLE hApp, PVOID pSystemInfo) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusInvldBuf;

  if (frameworkunifiedCheckValidAppHandle(hApp) && pSystemInfo != NULL) {  // LCOV_EXCL_BR_LINE 11:except branch
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

    eStatus = McGetSysInfoData(pApp->uiMsgRcvBuffer, pSystemInfo);
//    memcpy(pSystemInfo, pApp->cSystemInfo, _countof(pApp->cSystemInfo));
//    eStatus = eFrameworkunifiedStatusOK;
  }
  return eStatus;
}


//////////////////////////////////////////
// Function : frameworkunifiedGetIsTypeOfSync
//////////////////////////////////////////
BOOL frameworkunifiedGetIsTypeOfSync(HANDLE hApp) {
  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    return mcGetIsTypeOfSync(pApp->uiMsgRcvBuffer);
  }
  return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedGetLastNotification
/// Returns the last notification string that was received.
///
/// \param [in] hApp
///         HANDLE - Application framework handle
///
/// \return pSourceName
///         PCSTR - the last notification name that has been received.
///
/// \see FrameworkunifiedMcOpenSender, FrameworkunifiedMcClose, FrameworkunifiedSendMsg, FrameworkunifiedGetMsgLength, FrameworkunifiedGetMsgDataOfSize,
///      FrameworkunifiedClearMsgData, FrameworkunifiedGetDataUSID, FrameworkunifiedForwardMessage, FrameworkunifiedGetSystemInfo
///
////////////////////////////////////////////////////////////////////////////////////////////
PCSTR FrameworkunifiedGetLastNotification(HANDLE hApp) {
  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    return static_cast<PCSTR>(pApp->cSystemInfo);
  }

  return NULL;
}


EFrameworkunifiedStatus FrameworkunifiedSendResponse(HANDLE hApp, UI_32 iCmd, UI_32 length, PCVOID data) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusFail;
  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    HANDLE hSession = FrameworkunifiedGetCurrentSessionHandle(hApp);

    if (hSession) {
      // send response
      if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedSendMsg(hSession, iCmd, length, data))) {
        // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __PRETTY_FUNCTION__,
               "Response send Failed Status:0x%x ", eStatus);
        // LCOV_EXCL_BR_STOP
      } else {
        FRAMEWORKUNIFIEDLOG0(ZONE_NS_INFO, __PRETTY_FUNCTION__, "Response sent ");
      }
    }
  }
  return eStatus;
}

EFrameworkunifiedStatus FrameworkunifiedSendRequest(HANDLE hApp, PCSTR pServerName, UI_32 uiSessionId, UI_32 iCmd,
                          UI_32 length, PCVOID data) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusFail;
  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    HANDLE hSession = FrameworkunifiedGetSessionHandle(hApp, pServerName, uiSessionId);
    if (hSession) {
      // send response
      if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedSendMsg(hSession, iCmd, length, data))) {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __PRETTY_FUNCTION__,
               "Response send Failed Status:0x%x ", eStatus);
      } else {
        FRAMEWORKUNIFIEDLOG0(ZONE_NS_INFO, __PRETTY_FUNCTION__, "Response sent ");
      }
    }
  }
  return eStatus;
}
