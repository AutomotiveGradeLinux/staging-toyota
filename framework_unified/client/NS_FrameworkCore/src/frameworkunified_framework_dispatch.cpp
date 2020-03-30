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
/// \brief
///
///
///
///////////////////////////////////////////////////////////////////////////////
#include <sys/epoll.h>

#include <native_service/frameworkunified_framework_if.h>
#include <native_service/ns_message_center_if.h>
#include <native_service/ns_logger_if.h>
#include <native_service/ns_plogger_if.h>
#include <other_service/strlcpy.h>

#include <map>
#include <utility>

#include "frameworkunified_framework_core.h"
///////////////////////////////////////////////////
//        Utility
///////////////////////////////////////////////////

//////////////////////////////////////////
// Function : FrameworkunifiedGetAppName
//////////////////////////////////////////
PCSTR FrameworkunifiedGetAppName(HANDLE hApp) {
  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    const CFrameworkunifiedFrameworkApp *pApp = reinterpret_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    return ((PCSTR)pApp->cAppName);
  } else {
    return NULL;
  }
}

//////////////////////////////////////////
// Function : FrameworkunifiedSetThreadSpecificData
//////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSetThreadSpecificData(HANDLE hApp, PVOID data) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusInvldHandle;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = reinterpret_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    pApp->FrameworkData = data;

    eStatus = eFrameworkunifiedStatusOK;
  }

  return eStatus;
}

//////////////////////////////////////////
// Function : FrameworkunifiedGetThreadSpecificData
//////////////////////////////////////////
PVOID FrameworkunifiedGetThreadSpecificData(HANDLE hApp) {
  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    const CFrameworkunifiedFrameworkApp *pApp = reinterpret_cast< CFrameworkunifiedFrameworkApp * >(hApp);
    return pApp->FrameworkData;
  } else {
    return NULL;
  }
}



template <typename K, typename V, class C, class A>
std::ostream &operator<< (std::ostream &os, std::map<K, V, C, A> const &m) {
  os << "{ ";
  typename std::map<K, V, C, A>::const_iterator p;
  for (p = m.begin(); p != m.end(); ++p) {
    os << p->first << ":" << p->second << ", ";
  }
  return os << "}";
}







///////////////////////////////////////////////////
//   Service Protocol attach/detach to dispatcher
///////////////////////////////////////////////////







/////////////////////////////////////////////////////
// Function : FrameworkunifiedAttachCallbacksToDispatcher
/////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedAttachCallbacksToDispatcher(HANDLE hApp, PCSTR pServiceName, const FrameworkunifiedProtocolCallbackHandler *handlers,
                                          UI_32 handlerCount, HANDLE hSession) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && NULL != pServiceName && NULL != handlers && 0 != handlerCount) {
    // setup callbacks
    for (UI_32 i = 0; i < handlerCount; ++i) {
      if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedAttachCallbackToDispatcher(hApp, pServiceName, handlers[ i ].iCmd,
                                                                   handlers[ i ].callBack, hSession))) {
        break;
      }
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}


/////////////////////////////////////////////
// Function : FrameworkunifiedAttachCallbackToDispatcher
/////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedAttachCallbackToDispatcher(HANDLE hApp, PCSTR pServiceName, UI_32 iCmd, CbFuncPtr fpOnCmd,
                                         HANDLE hSession) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && NULL != pServiceName && NULL != fpOnCmd) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    Services::iterator s_iterator;
    SessionTable::iterator session_iterator;

    UI_32 uiSessionId = 0;
    if (hSession) {
      uiSessionId = FrameworkunifiedGetSessionId(hSession);
    }

    // finding the service
    s_iterator = pApp->services.find(pServiceName);
    if (s_iterator == pApp->services.end()) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, "%s : (New service entry): service name [%s]", pApp->cAppName, pServiceName);
      s_iterator = pApp->services.insert(std::make_pair(pServiceName, SessionTable())).first;
    }

    session_iterator = s_iterator->second.find(uiSessionId);
    if (session_iterator == s_iterator->second.end()) {
      session_iterator = s_iterator->second.insert(std::make_pair(uiSessionId, ServiceProtocolTable())).first;
    }

    session_iterator->second.insert(std::make_pair(iCmd, fpOnCmd));
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

///////////////////////////////////////////////////
// Function : FrameworkunifiedAttachCallbacksToDispatcherWithFd
///////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedAttachCallbacksToDispatcherWithFd(HANDLE hApp, const FrameworkunifiedFdProtocolCallbackHandler *handlers,
                                                UI_32 handlerCount) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && NULL != handlers && 0 != handlerCount) {
    // setup callbacksWithFd
    for (UI_32 i = 0; i < handlerCount; ++i) {
      if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedAttachCallbackToDispatcherWithFd(hApp, handlers[ i ].fd,
        handlers[ i ].callBack))) {
        break;
      }
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error : %d, Invalid param ", eStatus);
  }

  return eStatus;
}

///////////////////////////////////////////////////
// Function : FrameworkunifiedAttachCallbackToDispatcherWithFd
///////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedAttachCallbackToDispatcherWithFd(HANDLE hApp, int fd, CbFuncPtr fpOnCmd) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  int efd;        // FD for multi waiting
  struct epoll_event ev;  // Info struct to associate with multiwaiting FD

  if (frameworkunifiedCheckValidAppHandle(hApp) && 0 < fd && NULL != fpOnCmd) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

    // attach callbackFuncPtr and FD to FdTable.
    pApp->fds.insert(std::make_pair(fd, fpOnCmd));

    // Monitor FD by epoll.
    efd = pApp->efd;
    if (0 < efd) {
      ev.events = EPOLLIN;
      ev.data.fd = fd;
      if (-1 == epoll_ctl(efd, EPOLL_CTL_ADD, fd, &ev)) {
        /**
         *  Duplicate registering the same FD in the Dispatcher causes an internal epoll_ctl errno EEXIST(val17) to be returned, which results in an error.
         */
        eStatus = eFrameworkunifiedStatusFail;
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : epoll_ctl(ADD) Failed, status=%d, errno=%d", eStatus, errno);
      }
    } else {
      eStatus = eFrameworkunifiedStatusFail;
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Multi waiting FD is Invalid , status=%d, efd=%d", eStatus, efd);
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error : %d, Invalid param ", eStatus);
  }

  return eStatus;
}

/////////////////////////////////////////////////////
// Function : FrameworkunifiedAttachParentCallbacksToDispatcher
/////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedAttachParentCallbacksToDispatcher(HANDLE hChildApp, const FrameworkunifiedProtocolCallbackHandler *handlers,
                                                UI_32 handlerCount) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hChildApp) && NULL != handlers && 0 != handlerCount) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hChildApp);

    eStatus = FrameworkunifiedAttachCallbacksToDispatcher(hChildApp, pApp->cParentAppName, handlers, handlerCount);
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}


/////////////////////////////////////////////////////
// Function : FrameworkunifiedDetachParentCallbacksFromDispatcher
/////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedDetachParentCallbacksFromDispatcher(HANDLE hChildApp, const PUI_32 puiCmdArray, UI_32 uiCommandCount) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hChildApp) && NULL != puiCmdArray && 0 != uiCommandCount) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hChildApp);

    eStatus = FrameworkunifiedDetachCallbacksFromDispatcher(hChildApp, pApp->cParentAppName, puiCmdArray, uiCommandCount);
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

/////////////////////////////////////////////////////
// Function : FrameworkunifiedDetachCallbacksFromDispatcher
/////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedDetachCallbacksFromDispatcher(HANDLE hApp, PCSTR pServiceName, const PUI_32 puiCmdArray,
                                            UI_32 uiCommandCount, HANDLE hSession) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && NULL != pServiceName && NULL != puiCmdArray && 0 != uiCommandCount) {
    // setup callbacks
    for (UI_32 i = 0; i < uiCommandCount; ++i) {
      if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedDetachCallbackFromDispatcher(hApp, pServiceName,
        puiCmdArray[ i ], hSession))) {
        break;
      }
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}



////////////////////////////////////////////
// Function : FrameworkunifiedDetachCallbackFromDispatcher
/////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedDetachCallbackFromDispatcher(HANDLE hApp, PCSTR pServiceName, UI_32 iCmd, HANDLE hSession) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && NULL != pServiceName) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    Services::iterator s_iterator;

    UI_32 uiSessionId = 0;
    if (hSession) {
      uiSessionId = FrameworkunifiedGetSessionId(hSession);
    }

    // finding the service
    s_iterator = pApp->services.find(pServiceName);
    if (s_iterator != pApp->services.end()) {
      SessionTable::iterator session_iterator;
      session_iterator = (s_iterator->second).find(uiSessionId);
      if (session_iterator != (s_iterator->second).end()) {
        ServiceProtocolTable::iterator spt_iterator;
        FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, "%s : (found): service [%s]", pApp->cAppName, pServiceName);

        spt_iterator = (session_iterator->second).find(iCmd);
        if (spt_iterator != (session_iterator->second).end()) {
          FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, "%s : Detaching command [0x%X] service [%s]",
            pApp->cAppName, iCmd, pServiceName);
          (session_iterator->second).erase(spt_iterator);
        } else {
          FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, "%s : Warning : Cmd NOT found [%d] service [%s]",
            pApp->cAppName, iCmd, pServiceName);
        }
      }

    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, "%s : Warning : Cannot find service [%s]", pApp->cAppName, pServiceName);
      eStatus = eFrameworkunifiedStatusFail;
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }


  return eStatus;
}

/////////////////////////////////////////////////////
// Function : FrameworkunifiedDetachCallbacksFromDispatcherWithFd
/////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedDetachCallbacksFromDispatcherWithFd(HANDLE hApp, const int *fdArray, UI_32 uiCommandCount) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && NULL != fdArray && 0 != uiCommandCount) {
    // setup callbacks
    for (UI_32 i = 0; i < uiCommandCount; ++i) {
      if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedDetachCallbackFromDispatcherWithFd(hApp, fdArray[ i ]))) {
        break;
      }
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error : %d, Invalid param ", eStatus);
  }

  return eStatus;
}

////////////////////////////////////////////////////
// Function : FrameworkunifiedDetachCallbackFromDispatcherWithFd
////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedDetachCallbackFromDispatcherWithFd(HANDLE hApp, int fd) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && 0 < fd) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    FdTable::iterator f_iterator;
    int efd;        // FD for multi waiting
    struct epoll_event ev;  // Info struct to associate with multiwaiting FD

    // finding the FD from FD table
    f_iterator = pApp->fds.find(fd);
    if (f_iterator != pApp->fds.end()) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, "%s : Detaching fd [0x%x]", pApp->cAppName, fd);
      pApp->fds.erase(f_iterator);

      // Remove the monitoring of FD from multi waiting FD
      efd = pApp->efd;
      if (0 < efd) {
        ev.events = EPOLLIN;
        ev.data.fd = fd;
        if (-1 == epoll_ctl(efd, EPOLL_CTL_DEL, fd, &ev)) {
          eStatus = eFrameworkunifiedStatusFail;
          FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : epoll_ctl(DEL) Failed, status=%d, errno=%d", eStatus, errno);
        }
      } else {
        eStatus = eFrameworkunifiedStatusFail;
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Multi waiting FD is Invalid , status=%d, efd=%d", eStatus, efd);
      }
    } else {
      eStatus = eFrameworkunifiedStatusFail;
      FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, "%s : Warning : Cannot find fd [0x%x]", pApp->cAppName, fd);
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error :: %d, Invalid param ", eStatus);
  }


  return eStatus;
}

///////////////////////////////////////////////////
// Function : FrameworkunifiedDetachServiceFromDispatcher
///////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedDetachServiceFromDispatcher(HANDLE hApp, PCSTR pServiceName) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && NULL != pServiceName) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    Services::iterator s_iterator;
    EventServices::iterator es_iterator;

    // finding the service
    s_iterator = pApp->services.find(pServiceName);
    if (s_iterator != pApp->services.end()) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, "%s : (found): service [%s]", pApp->cAppName, pServiceName);
      pApp->services.erase(s_iterator);
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, "%s : Warning: Cannot find service [%s]", pApp->cAppName, pServiceName);
      eStatus = eFrameworkunifiedStatusFail;
    }

    // finding the service
    es_iterator = pApp->eventservices.find(pServiceName);
    if (es_iterator != pApp->eventservices.end()) {
      eStatus = eFrameworkunifiedStatusOK;
      FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, "%s : (found): service [%s] in event map", pApp->cAppName, pServiceName);
      pApp->eventservices.erase(es_iterator);
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, "%s : Warning : Cannot find service [%s] in event map",
        pApp->cAppName, pServiceName);
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

///////////////////////////////////////////////////
//        Notification functions
///////////////////////////////////////////////////


////////////////////////////////////////////////////////
// Function : FrameworkunifiedAttachNotificationCallbacksToDispatcher
////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSubscribeNotificationsWithCallback(HANDLE hApp,  const FrameworkunifiedNotificationCallbackHandler *pNtfyHandler,
                                                 UI_32 uiHandlerCount) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && pNtfyHandler && (uiHandlerCount > 0)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

    /// Deal with handling batch processing of subscriptions.
    if (eFrameworkunifiedStatusOK == (eStatus = FrameworkunifiedNPSubscribeToNotifications(hApp, pNtfyHandler, uiHandlerCount))) {
      for (UI_32 i = 0; i < uiHandlerCount; ++i) {
        // Notification names are character arrays of the MAX_STRING_SIZE_NOTIFICATION byte
        // If there are no NULL characters in the array of Notification names, they will overrun during make_pair,
        // so use a strlcpy that substitutes a trailing NULL character.
        char tmp_notification[MAX_STRING_SIZE_NOTIFICATION] = {0};
        strlcpy(tmp_notification, pNtfyHandler[ i ].cNotification, MAX_STRING_SIZE_NOTIFICATION);
        // service found
        pApp->notifications.insert(std::make_pair(tmp_notification, pNtfyHandler[ i ].callBack));

        FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, "%s : attaching call-back for notification [%s]", pApp->cAppName,
               pNtfyHandler[ i ].cNotification);
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "%s : Error : [%d] Unable to Subscribe to batch set of notifications",
        pApp->cAppName, eStatus);
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

////////////////////////////////////////////////////////
// Function : FrameworkunifiedAttachNotificationCallbackToDispatcher
////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSubscribeNotificationWithCallback(HANDLE hApp, PCSTR pNotification, CbFuncPtr fpOnCmd) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && NULL != pNotification && NULL != fpOnCmd) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    NotificationTableRetStatus mRet;

    if (eFrameworkunifiedStatusOK == (eStatus = FrameworkunifiedNPSubscribeToNotification(hApp, pNotification))) {
      // Notification names are character arrays of the MAX_STRING_SIZE_NOTIFICATION byte
      // If there are no NULL characters in the array of Notification names, they will overrun during make_pair,
      // so use a strlcpy that substitutes a trailing NULL character.
              char tmp_notification[MAX_STRING_SIZE_NOTIFICATION] = {0};
              strlcpy(tmp_notification, pNotification, MAX_STRING_SIZE_NOTIFICATION);

      // service found
      mRet = pApp->notifications.insert(std::make_pair(tmp_notification, fpOnCmd));
      if (false == mRet.second) {
        eStatus = eFrameworkunifiedStatusDuplicate;
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "%s:Error:[%d] Unable to Subscribe to notification [%s]",
          pApp->cAppName, eStatus, pNotification);
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, "%s:attaching call-back for notification [%s]",
          pApp->cAppName, pNotification);
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "%s:Error:[%d] Unable to Subscribe to notification [%s]",
        pApp->cAppName, eStatus, pNotification);
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}



////////////////////////////////////////////////////////
// Function : FrameworkunifiedDetachNotificationCallbacksFromDispatcher
////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedUnsubscribeNotificationsWithCallback(HANDLE hApp, const FrameworkunifiedNotificationCallbackHandler *pNtfyHandler,
                                                   UI_32 uiHandlerCount) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  NotificationTable::iterator n_iterator;
  PCSTR pNotification = NULL;

  if (frameworkunifiedCheckValidAppHandle(hApp) && NULL != pNtfyHandler && (uiHandlerCount > 0)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

    if (eFrameworkunifiedStatusOK == (eStatus = FrameworkunifiedNPUnsubscribeFromNotifications(hApp, pNtfyHandler, uiHandlerCount))) {
      for (UI_32 l_unCount = 0; l_unCount < uiHandlerCount; ++l_unCount) {
        // When registering using FrameworkunifiedSubscribeNotificationWithCallback, etc., the Nortification names NULL the 64th byte.
        // In response to this, the UnRegister uses a strlcpy that puts a NULL in the 64th byte.
        char tmp_notification[MAX_STRING_SIZE_NOTIFICATION] = {0};
        strlcpy(tmp_notification, pNtfyHandler[ l_unCount ].cNotification, MAX_STRING_SIZE_NOTIFICATION);

        n_iterator = pApp->notifications.find(tmp_notification);
        if (n_iterator != pApp->notifications.end()) {
          pApp->notifications.erase(n_iterator);
          FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, "%s : removed notification [%s]", pApp->cAppName, pNotification);
        } else {
          FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "%s : Error : Cannot find notification [%s]", pApp->cAppName,
                 pNotification != 0 ? pNotification : NULL);
          eStatus = eFrameworkunifiedStatusFail;
        }
      }

      FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, "%s : detaching call-back for notification [%s]", pApp->cAppName,
             pNotification != 0 ? pNotification : NULL);
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "%s : Error : [%d] Unable to UnSubscribe from notifications ", pApp->cAppName,
             eFrameworkunifiedStatusOK);
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}


////////////////////////////////////////////////////////
// Function : FrameworkunifiedDetachNotificationCallbackToDispatcher
////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedUnsubscribeNotificationWithCallback(HANDLE hApp, PCSTR pNotification) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && NULL != pNotification) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    NotificationTable::iterator n_iterator;

    if (eFrameworkunifiedStatusOK == (eStatus = FrameworkunifiedNPUnsubscribeFromNotification(hApp, pNotification))) {
      // \todo : error handling on all map function calls

      // When registering using FrameworkunifiedSubscribeNotificationWithCallback, etc., the Nortification names NULL the 64th byte.
      // In response to this, the UnRegister uses a strlcpy that puts a NULL in the 64th byte.
      char tmp_notification[MAX_STRING_SIZE_NOTIFICATION] = {0};
      strlcpy(tmp_notification, pNotification, MAX_STRING_SIZE_NOTIFICATION);

      n_iterator = pApp->notifications.find(tmp_notification);
      if (n_iterator != pApp->notifications.end()) {
        pApp->notifications.erase(n_iterator);
        FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, "%s:removed notification [%s]", pApp->cAppName, pNotification);
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "%s:Error:Cannot find notification [%s]", pApp->cAppName, pNotification);
        eStatus = eFrameworkunifiedStatusFail;
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "%s:Error:[%d] Unable to UnSubscribe from notification [%s]",
        pApp->cAppName, eStatus, pNotification);
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

///////////////////////////////////////////////////
//        Defer Message functions
///////////////////////////////////////////////////

////////////////////////////////////////////////////////
// Function : FrameworkunifiedGetDeferQueueCnt
////////////////////////////////////////////////////////
UI_32 FrameworkunifiedGetDeferQueueCnt(HANDLE hApp) {
  UI_32 l_uiCnt = 0;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    l_uiCnt = static_cast<UI_32>(pApp->deferedMsgQueue.size());
  }

  return l_uiCnt;
}

////////////////////////////////////////////////////////
// Function : FrameworkunifiedIsDeferQueueEmpty
////////////////////////////////////////////////////////
BOOL FrameworkunifiedIsDeferQueueEmpty(HANDLE hApp) {
  BOOL l_bIsQEmpty = TRUE;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    l_bIsQEmpty = static_cast<BOOL>(pApp->deferedMsgQueue.empty());
  }

  return l_bIsQEmpty;
}

////////////////////////////////////////////////////////
// Function : FrameworkunifiedDeferMessage
////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedDeferMessage(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    DeferedMsgInfo defer(pApp->uiProtocolCmd, pApp->cMsgSrcName, pApp->uiMsgRcvBuffer);
    pApp->deferedMsgQueue.push(defer);
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

////////////////////////////////////////////////////////
// Function : FrameworkunifiedClearDeferMessages
////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedClearDeferMessages(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

    // clear the pop flag!
    pApp->fPopDeferedMsg = FALSE;

    // remove all items from the queue!
    while (!pApp->deferedMsgQueue.empty()) {
      pApp->deferedMsgQueue.pop();
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

////////////////////////////////////////////////////////
// Function : FrameworkunifiedRetrieveDeferMessage
////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedRetrieveDeferMessage(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  uint64_t data = 1;
  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    pApp->fPopDeferedMsg = pApp->deferedMsgQueue.empty() ? FALSE : TRUE;
    if (pApp->fPopDeferedMsg) {
      if (-1 == (write(pApp->defer_evfd, &data, sizeof(uint64_t)))) {
        eStatus = eFrameworkunifiedStatusFail;
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR : write Failed, status=%d, errno=%d, defer_evfd=%d",
               eStatus, errno, pApp->defer_evfd);
      }
    }


  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}


////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedIsStateMachineApp
/// Returns TRUE if it's a state machine application else FALSE.
////////////////////////////////////////////////////////////////////////////////////////////
BOOL FrameworkunifiedIsStateMachineApp(HANDLE hApp) {
  BOOL l_bIsStateMachine = FALSE;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    if (NULL != pApp->m_pFrameworkunifiedStateMachine) {
      l_bIsStateMachine = TRUE;
    }
  }

  return l_bIsStateMachine;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedGetXMLConfigHandle
///   Returns the handle to config file handle
////////////////////////////////////////////////////////////////////////////////////////////
HANDLE FrameworkunifiedGetXMLConfigHandle(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __PRETTY_FUNCTION__, "This function is not implemented!!!!");
  return NULL;
}

EFrameworkunifiedStatus FrameworkunifiedSetMandatoryServiceInfo(HANDLE hApp, PCSTR pNotification, UI_32 uiEventId) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && NULL != pNotification) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    ServiceNotificationInfo objNotification = {};
    if (strlen(pNotification) < MAX_STRING_SIZE_NOTIFICATION) {
      strlcpy(objNotification.sNotificationName, pNotification, sizeof(objNotification.sNotificationName));
      objNotification.uiEventId = uiEventId;
      pApp->servicenotificationlist.push_back(objNotification);
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__,
             " Error : Aborting ... Exceeds Max Notification name size MAX_STRING_SIZE_NOTIFICATION : %d ",
             MAX_STRING_SIZE_NOTIFICATION);
      eStatus = eFrameworkunifiedStatusFail;
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

BOOL FrameworkunifiedIsServiceAvailable(HANDLE hApp) {
  BOOL l_bIsServiceAvailable = FALSE;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    // Publish notification
    ServiceAvailability tServiceAvailability = {};

    UI_32 l_uiLength = FrameworkunifiedGetMsgLength(hApp);

    if (sizeof(tServiceAvailability) == l_uiLength) {
      // Read the data from the message
      if (eFrameworkunifiedStatusOK != FrameworkunifiedGetMsgDataOfSize(hApp, &tServiceAvailability, sizeof(tServiceAvailability))) {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, " FrameworkunifiedGetMsgDataOfSize Failed");
      } else {
        // Check the Service availability
        if (eFrameworkunifiedServiceAvailable == tServiceAvailability.eServiceAvailability) {
          l_bIsServiceAvailable = TRUE;
        }
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, " Error: Received message data size not matched :: %d", l_uiLength);
    }
  }

  return l_bIsServiceAvailable;
}

EFrameworkunifiedStatus FrameworkunifiedPublishServiceAvailability(HANDLE hApp, BOOL bIsAvailable) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    if (strlen(pApp->sServiceAvailabilityNotification)) {
      // Publish Service available this can also be published from FrameworkunifiedOnStart callback
      pApp->bIsAvailable = bIsAvailable;
      ServiceAvailability tServiceAvailability = {};
      strlcpy(tServiceAvailability.cServiceName, FrameworkunifiedGetAppName(hApp), sizeof(tServiceAvailability.cServiceName));

      if (bIsAvailable) {
        tServiceAvailability.eServiceAvailability = eFrameworkunifiedServiceAvailable;
      } else {
        tServiceAvailability.eServiceAvailability = eFrameworkunifiedServiceNotAvailable;
      }

      if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedNPPublishNotification(hApp, pApp->sServiceAvailabilityNotification,
                                                              &tServiceAvailability, sizeof(tServiceAvailability)))) {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __PRETTY_FUNCTION__,
               "Failed to Publish notification: %s, status: %d", pApp->sServiceAvailabilityNotification, eStatus);
        FRAMEWORKUNIFIEDLOG_PERFORMANCE("Service Availability Status: %s, ERRORED %d", bIsAvailable ? "TRUE" : "FALSE", eStatus);
      } else {
        FRAMEWORKUNIFIEDLOG_PERFORMANCE("Service Availability Status: %s", bIsAvailable ? "TRUE" : "FALSE");
        FRAMEWORKUNIFIEDLOG(ZONE_NS_IMP_INFO, __FUNCTION__,
               "Publish availability notfn:%s, status:%s", pApp->sServiceAvailabilityNotification,
               bIsAvailable ? "TRUE" : "FALSE");
      }
    } else {
      eStatus = eFrameworkunifiedStatusFail;
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

BOOL FrameworkunifiedGetSelfAvailability(HANDLE hApp) {
  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    return pApp->bIsAvailable;
  }

  return FALSE;
}

EFrameworkunifiedStatus FrameworkunifiedRegisterServiceAvailabilityNotification(HANDLE hApp, PCSTR pNotification) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && NULL != pNotification) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    if (strlen(pNotification) < MAX_STRING_SIZE_NOTIFICATION) {
      strlcpy(pApp->sServiceAvailabilityNotification, pNotification, sizeof(pApp->sServiceAvailabilityNotification));

      //  Register Notifications
      if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedNPRegisterNotification(hApp, pNotification,
                                                               sizeof(ServiceAvailability), eFrameworkunifiedStateVar))) {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "FrameworkunifiedNPRegisterNotifications %s Failed Status:0x%x ", pNotification, eStatus);
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "FrameworkunifiedNPRegisterNotifications %s success Status:0x%x ",
          pNotification, eStatus);
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__,
             " Error : Aborting ... Exceeds Max Notification name size MAX_STRING_SIZE_NOTIFICATION : %d ",
             MAX_STRING_SIZE_NOTIFICATION);
      eStatus = eFrameworkunifiedStatusFail;
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

EFrameworkunifiedStatus FrameworkunifiedUnRegisterServiceAvailabilityNotification(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

    //  Register Notifications
    if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedNPUnRegisterNotification(hApp, pApp->sServiceAvailabilityNotification))) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "FrameworkunifiedNPUnRegisterNotifications %s Failed Status:0x%x ",
             pApp->sServiceAvailabilityNotification != 0 ? pApp->sServiceAvailabilityNotification : NULL, eStatus);
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "FrameworkunifiedNPUnRegisterNotifications %s success Status:0x%x ",
             pApp->sServiceAvailabilityNotification, eStatus);
    }
    memset(pApp->sServiceAvailabilityNotification, 0, MAX_SYS_INFO_SIZE);
    pApp->bIsAvailable = FALSE;
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedGetServiceNameOnServiceAvailabilityNotification
/// To be used when client receives service availability notification to get the available
/// service name.
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedGetServiceNameOnServiceAvailabilityNotification(HANDLE hApp, PSTR pServiceName) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  // Publish notification
  ServiceAvailability tServiceAvailability;

  if (frameworkunifiedCheckValidAppHandle(hApp) && NULL != pServiceName) {
    UI_32 l_uiLength = FrameworkunifiedGetMsgLength(hApp);

    if (sizeof(tServiceAvailability) == l_uiLength) {
      // Read the data from the message
      if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedGetMsgDataOfSize(hApp, &tServiceAvailability, sizeof(tServiceAvailability)))) {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, " FrameworkunifiedGetMsgDataOfSize Failed");
        eStatus = eFrameworkunifiedStatusFail;
      } else {
        if (NULL == std::strncpy(pServiceName, tServiceAvailability.cServiceName, MAX_NAME_SIZE_APP)) {
          FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, " strcpy failed Failed");
          eStatus = eFrameworkunifiedStatusFail;
        }
      }
    } else {
      eStatus = eFrameworkunifiedStatusFail;
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, " Error: Received message data size not matched :: %d", l_uiLength);
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

//////////////////////////////////////////
// Function : FrameworkunifiedGetCurrentUser
//////////////////////////////////////////
HANDLE FrameworkunifiedGetCurrentUser(HANDLE hApp) {
  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    const CFrameworkunifiedFrameworkApp *pApp = reinterpret_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    return pApp->hUser;
  } else {
    return NULL;
  }
}

//////////////////////////////////////////
// Function : FrameworkunifiedSetUser
//////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSetUser(HANDLE hApp, HANDLE hUser) {
  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = reinterpret_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    pApp->hUser = hUser;
    return eFrameworkunifiedStatusOK;
  } else {
    return eFrameworkunifiedStatusFail;
  }
}

//////////////////////////////////////////
// Function : FrameworkunifiedSetAppData
//////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSetAppData(HANDLE hApp, PCSTR pKey, PVOID pData) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusInvldHandle;
  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = reinterpret_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    // Insert the data element
    std::pair<AppData::iterator, bool> l_tRet = pApp->appdata.insert(std::make_pair(pKey, pData));
    if (false == l_tRet.second) {
      eStatus = eFrameworkunifiedStatusFail;
    } else {
      eStatus = eFrameworkunifiedStatusOK;
    }
  }
  return eStatus;
}

//////////////////////////////////////////
// Function : FrameworkunifiedGetAppData
//////////////////////////////////////////
PVOID FrameworkunifiedGetAppData(HANDLE hApp, PCSTR pKey) {
  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = reinterpret_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    AppData::iterator n_iterator = pApp->appdata.find(pKey);
    if (n_iterator != pApp->appdata.end()) {
      return n_iterator->second;
    }
  }
  return NULL;
}


//////////////////////////////////////////
// Function : FrameworkunifiedRemoveAppData
//////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedRemoveAppData(HANDLE hApp, PCSTR pKey) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusInvldHandle;
  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = reinterpret_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    AppData::iterator n_iterator = pApp->appdata.find(pKey);
    if (n_iterator != pApp->appdata.end()) {
      pApp->appdata.erase(n_iterator);

      eStatus = eFrameworkunifiedStatusOK;
    } else {
      eStatus = eFrameworkunifiedStatusFail;
    }
  }
  return eStatus;
}
