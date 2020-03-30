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
/// \brief    Framework wrapper over the NPService interface APIs
///
///
///
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Include Files
///////////////////////////////////////////////////////////////////////////////
#include <string.h>

#include <native_service/frameworkunified_framework_types.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/ns_np_service_if.h>
#include <native_service/ns_np_service_protocol.h>
#include <other_service/strlcpy.h>

#include "frameworkunified_framework_core.h"
#include "ns_np_service_internal.h"

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPRegisterNotification
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPRegisterNotification(HANDLE hApp, PCSTR pNotification,  const UI_32 max_length,
                                     const EFrameworkunifiedNotificationType persType) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && pNotification) {
    if (strlen(pNotification) && strlen(pNotification) < MAX_STRING_SIZE_NOTIFICATION) {
      CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
      MsgQInfo *pMqInfo = static_cast<MsgQInfo *>(pApp->hNPSndMsgQ);

      NotificationInfo notifInfo = {};
      strlcpy(notifInfo.notificationName, pNotification, sizeof(notifInfo.notificationName));
      notifInfo.persType = persType;
      notifInfo.maxLength = max_length;

      eStatus = NPRegisterNotifications(pMqInfo->hMsgQ, pApp->cAppName, 1, &notifInfo);
    } else {
      eStatus = eFrameworkunifiedStatusInvldParam;
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

EFrameworkunifiedStatus FrameworkunifiedNPRegisterImmediatePersistNotification(HANDLE hApp, PCSTR pNotification,  const UI_32 max_length,
                                                     const UI_32 delay) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && NULL != pNotification) {
    if (strlen(pNotification) && strlen(pNotification) <= MAX_STRING_SIZE_NOTIFICATION) {
      CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
      MsgQInfo *pMqInfo = static_cast<MsgQInfo *>(pApp->hNPSndMsgQ);

      ImmediateNotificationInfo notifInfo = {};
      strlcpy(notifInfo.notificationName, pNotification, sizeof(notifInfo.notificationName));
      notifInfo.persType = eFrameworkunifiedImmediatePersistedStateVar;
      notifInfo.maxLength = max_length;
      notifInfo.delay = delay;

      eStatus = NPRegisterImmediateNotifications(pMqInfo->hMsgQ, pApp->cAppName, 1, &notifInfo);
    } else {
      eStatus = eFrameworkunifiedStatusInvldParam;
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPPersistentSync
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPPersistentSync(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    MsgQInfo *pMqInfo = static_cast<MsgQInfo *>(pApp->hNPSndMsgQ);

    eStatus = NPPersistentSync(pMqInfo->cSrcName, pMqInfo->hMsgQ, pMqInfo->sessionId,  pApp->cAppName);
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }
  return eStatus;
}
////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPRegisterNotifications
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPRegisterNotifications(HANDLE hApp, const  FrameworkunifiedNotificationsList *pList, UI_32 uiListLength) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  if (frameworkunifiedCheckValidAppHandle(hApp) && pList) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    MsgQInfo *pMqInfo = static_cast<MsgQInfo *>(pApp->hNPSndMsgQ);

    if (NULL != pMqInfo) {
      NotificationInfo *pNotificationList =
      reinterpret_cast<NotificationInfo *>(malloc(sizeof(NotificationInfo) * uiListLength));

      for (UI_32 i = 0; i < uiListLength; ++i) {
        strlcpy(pNotificationList[i].notificationName, pList[i].cNotification,
          sizeof(pNotificationList[i].notificationName));
        pNotificationList[i].maxLength = pList[i].uiLengthData;
        pNotificationList[i].persType = pList[i].persType;
      }
      eStatus = NPRegisterNotifications(pMqInfo->hMsgQ, pApp->cAppName, uiListLength, pNotificationList);

      free(pNotificationList);
      pNotificationList = NULL;  // mb20110110 Fixed per  comment 295
    } else {
      eStatus = eFrameworkunifiedStatusNullPointer;
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPUnRegisterNotification
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPUnRegisterNotification(HANDLE hApp, PCSTR pNotification) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && pNotification && strlen(pNotification)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    MsgQInfo *pMqInfo = static_cast<MsgQInfo *>(pApp->hNPSndMsgQ);

    NotificationInfo notifInfo = {};
    strlcpy(notifInfo.notificationName, pNotification, sizeof(notifInfo.notificationName));

    eStatus = NPUnRegisterNotifications(pMqInfo->hMsgQ, pApp->cAppName, 1, &notifInfo);
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPUnRegisterNotifications
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPUnRegisterNotifications(HANDLE hApp, const  FrameworkunifiedNotificationsList *pList, UI_32 uiListLength) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  if (frameworkunifiedCheckValidAppHandle(hApp) && pList) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    MsgQInfo *pMqInfo = static_cast<MsgQInfo *>(pApp->hNPSndMsgQ);
    if (NULL != pMqInfo) {
      NotificationInfo *pNotificationList =
      reinterpret_cast<NotificationInfo *>(malloc(sizeof(NotificationInfo) * uiListLength));

      for (UI_32 i = 0; i < uiListLength; ++i) {
        strlcpy(pNotificationList[i].notificationName, pList[i].cNotification,
          sizeof(pNotificationList[i].notificationName));
        pNotificationList[i].maxLength = pList[i].uiLengthData;
        pNotificationList[i].persType = pList[i].persType;
      }
      eStatus = NPUnRegisterNotifications(pMqInfo->hMsgQ, pApp->cAppName, uiListLength, pNotificationList);

      free(pNotificationList);
    } else {
      eStatus = eFrameworkunifiedStatusNullPointer;
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPPublishNotification
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPPublishNotification(HANDLE hApp, PCSTR pNotification,
                                    PCVOID pData, UI_32 iLength) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && NULL != pNotification) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    MsgQInfo *pMqInfo = static_cast<MsgQInfo *>(pApp->hNPSndMsgQ);

    if ((NULL != pMqInfo->hMsgQ) && (0 != std::strlen(pApp->cAppName))) {
      eStatus = NPPublishNotification(pMqInfo->hMsgQ, pApp->cAppName, pNotification, pData, iLength);
    } else {
      eStatus = eFrameworkunifiedStatusInvldHandle;
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}


////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPSubscribeToNotification
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPSubscribeToNotification(HANDLE hApp, PCSTR pNotification) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && pNotification) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    MsgQInfo *pMqInfo = static_cast<MsgQInfo *>(pApp->hNPSndMsgQ);

    eStatus = NPSubscribeToNotification(pMqInfo->hMsgQ,  pApp->cAppName, pNotification);
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}


EFrameworkunifiedStatus FrameworkunifiedNPSubscribeToNotifications(HANDLE hApp, const  FrameworkunifiedNotificationCallbackHandler *pList,
                                         UI_32 uiListLength) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && pList) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    MsgQInfo *pMqInfo = static_cast<MsgQInfo *>(pApp->hNPSndMsgQ);

    SubscribeInfo *pSubscribeList =
    reinterpret_cast<SubscribeInfo *>(malloc(sizeof(SubscribeInfo) * uiListLength));

    for (UI_32 i = 0; i < uiListLength; ++i) {
      strlcpy(pSubscribeList[i].notificationName, pList[i].cNotification,
        sizeof(pSubscribeList[i].notificationName));
    }
    eStatus = NPSubscribeToNotifications(pMqInfo->hMsgQ, pApp->cAppName, uiListLength, pSubscribeList);

    free(pSubscribeList);
    pSubscribeList = NULL;  // mb20110110 Fixed per  comment 307
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}


////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPUnSubscribeNotifications
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPUnsubscribeFromNotifications(HANDLE hApp, const  FrameworkunifiedNotificationCallbackHandler *pList,
                                             UI_32 uiListLength) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && pList) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    MsgQInfo *pMqInfo = static_cast<MsgQInfo *>(pApp->hNPSndMsgQ);

    SubscribeInfo *pSubscribeList =
    reinterpret_cast<SubscribeInfo *>(malloc(sizeof(SubscribeInfo) * uiListLength));

    for (UI_32 i = 0; i < uiListLength; ++i) {
      strlcpy(pSubscribeList[i].notificationName, pList[i].cNotification, MAX_STRING_SIZE_NOTIFICATION);
    }
    eStatus = NPUnsubscribeFromNotifications(pMqInfo->hMsgQ, pApp->cAppName, uiListLength, pSubscribeList);

    free(pSubscribeList);
    pSubscribeList = NULL;  // mb20110110 Fixed per  comment 307
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}


////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPUnsubscribeFromNotification
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPUnsubscribeFromNotification(HANDLE hApp, PCSTR pNotification) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && pNotification) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    MsgQInfo *pMqInfo = static_cast<MsgQInfo *>(pApp->hNPSndMsgQ);

    eStatus = NPUnsubscribeFromNotification(pMqInfo->hMsgQ, pApp->cAppName, pNotification);
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPReadPersistedData
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPReadPersistedData(HANDLE hApp, PCSTR pNotification) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && pNotification) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    MsgQInfo *pMqInfo = static_cast<MsgQInfo *>(pApp->hNPSndMsgQ);

    eStatus = NPReadPersistedData(pMqInfo->hMsgQ,  pApp->cAppName, pNotification);
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPSavePersistentData
////////////////////////////////////////////////////////////////////////////////////////////
// LCOV_EXCL_START 8: dead code
EFrameworkunifiedStatus FrameworkunifiedNPSavePersistentData(HANDLE hApp) {
  AGL_ASSERT_NOT_TESTED();
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    MsgQInfo *pMqInfo = static_cast<MsgQInfo *>(pApp->hNPSndMsgQ);

    eStatus = NPSavePersistentData(pMqInfo->hMsgQ,  pApp->cAppName);
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}
// LCOV_EXCL_STOP


////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPRegisterPersistentFile
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPRegisterPersistentFile(HANDLE hApp, PCSTR pTag, BOOL bIsUserFile) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  if (frameworkunifiedCheckValidAppHandle(hApp) && pTag) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    MsgQInfo *pMqInfo = static_cast<MsgQInfo *>(pApp->hNPSndMsgQ);
    if (NULL != pMqInfo) {
      eStatus = NPRegisterPersistentFile(pMqInfo->hMsgQ,  pApp->cAppName, pTag, bIsUserFile);
    } else {
      eStatus = eFrameworkunifiedStatusNullPointer;
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}


////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPLoadPersistentFile
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPLoadPersistentFile(HANDLE hApp, PCSTR pDstFilePath, PCSTR pTag, HANDLE hUser) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && pDstFilePath && pTag) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    MsgQInfo *pMqInfo = static_cast<MsgQInfo *>(pApp->hNPSndMsgQ);

    if (NULL != pMqInfo) {
      eStatus = NPLoadPersistentFile(pMqInfo->hMsgQ,  pApp->cAppName, pDstFilePath, pTag, hUser);
    } else {
      eStatus = eFrameworkunifiedStatusNullPointer;
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}


////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPReleasePersistentFile
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPReleasePersistentFile(HANDLE hApp, BOOL bIsPersist, PCSTR pTag,
                                      PCSTR pFullFilePath, HANDLE hUser) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  if (frameworkunifiedCheckValidAppHandle(hApp) && pTag && pFullFilePath) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    MsgQInfo *pMqInfo = static_cast<MsgQInfo *>(pApp->hNPSndMsgQ);

    if (NULL != pMqInfo) {
      EFrameworkunifiedReleaseType eReleaseType = (bIsPersist == FALSE) ? eFrameworkunifiedNotOnRelease : eFrameworkunifiedPersistOnShutdown;
      eStatus = NPReleasePersistentFile(pMqInfo->hMsgQ,  pApp->cAppName, eReleaseType, pTag,
                                        pFullFilePath, hUser);
    } else {
      eStatus = eFrameworkunifiedStatusNullPointer;
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPReleasePersistentFile
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPReleasePersistentFile(HANDLE hApp, EFrameworkunifiedReleaseType eReleaseType, PCSTR pTag,
                                      PCSTR pFullFilePath, HANDLE hUser) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  if (frameworkunifiedCheckValidAppHandle(hApp) && pTag && pFullFilePath) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    MsgQInfo *pMqInfo = static_cast<MsgQInfo *>(pApp->hNPSndMsgQ);

    if (NULL != pMqInfo) {
      eStatus = NPReleasePersistentFile(pMqInfo->hMsgQ,  pApp->cAppName, eReleaseType, pTag,
                                        pFullFilePath, hUser);
    } else {
      eStatus = eFrameworkunifiedStatusNullPointer;
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPRegisterPersistentFolder
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPRegisterPersistentFolder(HANDLE hApp, PCSTR pTag, BOOL bIsUserFolder) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  if (frameworkunifiedCheckValidAppHandle(hApp) && pTag) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    MsgQInfo *pMqInfo = static_cast<MsgQInfo *>(pApp->hNPSndMsgQ);

    if (NULL != pMqInfo) {
      eStatus = NPRegisterPersistentFolder(pMqInfo->hMsgQ,  pApp->cAppName, pTag, bIsUserFolder);
    } else {
      eStatus = eFrameworkunifiedStatusNullPointer;
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}


////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPLoadPersistentFolder
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPLoadPersistentFolder(HANDLE hApp, PCSTR pDstFolderPath, PCSTR pTag, HANDLE hUser) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  if (frameworkunifiedCheckValidAppHandle(hApp) && pTag && pDstFolderPath) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    MsgQInfo *pMqInfo = static_cast<MsgQInfo *>(pApp->hNPSndMsgQ);
    if (NULL != pMqInfo) {
      eStatus = NPLoadPersistentFolder(pMqInfo->hMsgQ,  pApp->cAppName, pDstFolderPath, pTag, hUser);
    } else {
      eStatus = eFrameworkunifiedStatusNullPointer;
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}


////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPReleasePersistentFolder
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPReleasePersistentFolder(HANDLE hApp, BOOL bIsPersist, PCSTR pTag, PCSTR pFullFolderPath,
                                        HANDLE hUser) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  if (frameworkunifiedCheckValidAppHandle(hApp) && pTag && pFullFolderPath) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    MsgQInfo *pMqInfo = static_cast<MsgQInfo *>(pApp->hNPSndMsgQ);
    if (NULL != pMqInfo) {
      EFrameworkunifiedReleaseType eReleaseType = (bIsPersist == FALSE) ? eFrameworkunifiedNotOnRelease : eFrameworkunifiedPersistOnShutdown;
      eStatus = NPReleasePersistentFolder(pMqInfo->hMsgQ,  pApp->cAppName, eReleaseType, pTag,
                                          pFullFolderPath, hUser);
    } else {
      eStatus = eFrameworkunifiedStatusNullPointer;
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}
////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPReleasePersistentFolder
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPReleasePersistentFolder(HANDLE hApp, EFrameworkunifiedReleaseType eReleaseType, PCSTR pTag,
                                        PCSTR pFullFolderPath,
                                        HANDLE hUser) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  if (frameworkunifiedCheckValidAppHandle(hApp) && pTag && pFullFolderPath) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    MsgQInfo *pMqInfo = static_cast<MsgQInfo *>(pApp->hNPSndMsgQ);
    if (NULL != pMqInfo) {
      eStatus = NPReleasePersistentFolder(pMqInfo->hMsgQ,  pApp->cAppName, eReleaseType, pTag,
                                          pFullFolderPath, hUser);
    } else {
      eStatus = eFrameworkunifiedStatusNullPointer;
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

// 20110429_brp
// defined for backward compatibility; will be removed once the persistence feature is finalized
EFrameworkunifiedStatus FrameworkunifiedRegisterPersistentStorage(HANDLE hApp, PCSTR pFullFilePath) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && pFullFilePath) {
    eStatus = FrameworkunifiedNPRegisterPersistentFile(hApp, pFullFilePath);
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}
EFrameworkunifiedStatus FrameworkunifiedReleaseFileToPersistentStorage(HANDLE hApp, PCSTR pFullFilePath, BOOL persist) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && pFullFilePath) {
    eStatus = FrameworkunifiedNPReleasePersistentFile(hApp, persist, pFullFilePath, pFullFilePath);
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPHSMRegisterNotificationsEvents
/// API to send message to Notification Service to register a notification
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPHSMRegisterNotificationsEvents(HANDLE hApp, const  FrameworkunifiedNotificationsList *pList,
                                               UI_32 uiListLength) {
  return FrameworkunifiedNPRegisterNotifications(hApp, pList, uiListLength);
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPHSMRegisterNotificatsionEvent
/// API to send message to Notification Service to register a notification
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPHSMRegisterNotificatsionEvent(HANDLE hApp, PCSTR pNotification, const UI_32 max_length,
                                              const EFrameworkunifiedNotificationType persType) {
  return FrameworkunifiedNPRegisterNotification(hApp, pNotification, max_length, persType);
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPHSMUnRegisterNotificationEvent
/// API to send message to Notification Service to remove a notification
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPHSMUnRegisterNotificationEvent(HANDLE hApp, PCSTR pNotification) {
  return FrameworkunifiedNPUnRegisterNotification(hApp, pNotification);
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPHSMSubscribeToNotificationsEvents
/// API to send message to Notification Service to add multiple subscriptions list for
/// that notification
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPHSMSubscribeToNotificationsEvents(HANDLE hApp, const  FrameworkunifiedNotificationEvent *pList,
                                                  UI_32 uiListLength) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && pList) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    MsgQInfo *pMqInfo = static_cast<MsgQInfo *>(pApp->hNPSndMsgQ);

    SubscribeInfo *pSubscribeList =
      reinterpret_cast<SubscribeInfo *>(malloc(sizeof(SubscribeInfo) * uiListLength));

    for (UI_32 i = 0; i < uiListLength; ++i) {
      strlcpy(pSubscribeList[i].notificationName, pList[i].cNotification,
        sizeof(pSubscribeList[i].notificationName));
    }
    eStatus = NPSubscribeToNotifications(pMqInfo->hMsgQ, pApp->cAppName, uiListLength, pSubscribeList);

    free(pSubscribeList);
    pSubscribeList = NULL;  // mb20110110 Fixed per  comment 307
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
///  FrameworkunifiedNPHSMSubscribeToNotificationEvent
/// API to send message to Notification Service to add to subscription list for
/// that notification
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPHSMSubscribeToNotificationEvent(HANDLE hApp, PCSTR pNotification) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && pNotification) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    MsgQInfo *pMqInfo = static_cast<MsgQInfo *>(pApp->hNPSndMsgQ);

    eStatus = NPSubscribeToNotification(pMqInfo->hMsgQ,  pApp->cAppName, pNotification);
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPHSMUnsubscribeFromNotificationEvent
/// API to send message to Notification Service to remove from subscription list for
/// that notification
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPHSMUnsubscribeFromNotificationEvent(HANDLE hApp, PCSTR pNotification) {
  return FrameworkunifiedNPUnsubscribeFromNotification(hApp, pNotification);
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPHSMUnsubscribeFromNotificationEvents
/// API to send message to Notification Service to remove from subscription list for
/// that notification
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPHSMUnsubscribeFromNotificationEvents(HANDLE hApp, const  FrameworkunifiedNotificationEvent *pList,
                                                     UI_32 uiListLength) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && pList) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    MsgQInfo *pMqInfo = static_cast<MsgQInfo *>(pApp->hNPSndMsgQ);

    SubscribeInfo *pSubscribeList =
    reinterpret_cast<SubscribeInfo *>(malloc(sizeof(SubscribeInfo) * uiListLength));

    for (UI_32 i = 0; i < uiListLength; ++i) {
      strlcpy(pSubscribeList[i].notificationName, pList[i].cNotification,
        sizeof(pSubscribeList[i].notificationName));
    }
    eStatus = NPUnsubscribeFromNotifications(pMqInfo->hMsgQ, pApp->cAppName, uiListLength, pSubscribeList);

    free(pSubscribeList);
    pSubscribeList = NULL;  // mb20110110 Fixed per  comment 307
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPSetPersonality
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPSetPersonality(HANDLE hApp, PCSTR pUserName) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && pUserName) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    MsgQInfo *pMqInfo = static_cast<MsgQInfo *>(pApp->hNPSndMsgQ);

    eStatus = NPSetPersonality(pMqInfo->hMsgQ,  pApp->cAppName, pUserName);
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPChangePersonality
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPChangePersonality(HANDLE hApp, PCSTR pUserName) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp) && pUserName) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    MsgQInfo *pMqInfo = static_cast<MsgQInfo *>(pApp->hNPSndMsgQ);

    eStatus = NPChangePersonality(pMqInfo->hMsgQ,  pApp->cAppName, pUserName);
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedSendStopToNSNPP
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSendStopToNSNPP(HANDLE hApp, EFrameworkunifiedShutdownType eShutdownType, UI_32 uiStopMsgData) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    MsgQInfo *pMqInfo = static_cast<MsgQInfo *>(pApp->hNPSndMsgQ);

    eStatus = SendStopToNSNPP(pMqInfo->hMsgQ,  pApp->cAppName, eShutdownType, uiStopMsgData);
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPGetReadyStatusOfNPP
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPGetReadyStatusOfNPP(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
    MsgQInfo *pMqInfo = static_cast<MsgQInfo *>(pApp->hNPSndMsgQ);

    eStatus = NPGetReadyStatusOfNPP(pMqInfo->hMsgQ,  pApp->cAppName);
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPClearPersistedData
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPClearPersistedData(HANDLE hApp, EFrameworkunifiedClearPersistence eFrameworkunifiedClearPersistenceScope) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);
  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    MsgQInfo *pMqInfo = static_cast<MsgQInfo *>(pApp->hNPSndMsgQ);
    if (NULL != pMqInfo) {
      eStatus = NPClearPersistedData(pMqInfo->hMsgQ,  pApp->cAppName, eFrameworkunifiedClearPersistenceScope);
    } else {
      eStatus = eFrameworkunifiedStatusNullPointer;
    }
  } else {
    eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPSetPersistNotfnDefaultValue
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPSetPersistNotfnDefaultValue(HANDLE hApp, PCSTR pNotification, PVOID pData,
                                            const UI_32 uiLength) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if ((frameworkunifiedCheckValidAppHandle(hApp)) && (NULL != pNotification)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

    MsgQInfo *pMqInfo = static_cast<MsgQInfo *>(pApp->hNPSndMsgQ);

    if (NULL != pMqInfo) {
      eStatus = NPSetPersistNotfnDefaultValue(pMqInfo->hMsgQ,
                                              pApp->cAppName,
                                              pNotification,
                                              pData,
                                              uiLength);
    } else {
      eStatus = eFrameworkunifiedStatusNullPointer;
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPSetPersistentNotfnType
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPSetPersistentNotfnType(HANDLE hApp, PCSTR pNotification, EFrameworkunifiedPersistCategory ePersistCategory) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if ((frameworkunifiedCheckValidAppHandle(hApp)) && (NULL != pNotification)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

    MsgQInfo *pMqInfo = static_cast<MsgQInfo *>(pApp->hNPSndMsgQ);
    if (NULL != pMqInfo) {
      eStatus = NPSetPersistentNotfnType(pMqInfo->hMsgQ,
                                         pApp->cAppName,
                                         pNotification,
                                         ePersistCategory);
    } else {
      eStatus = eFrameworkunifiedStatusNullPointer;
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPSetFilePersistentType
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPSetFilePersistentType(HANDLE hApp, PCSTR pTag, EFrameworkunifiedPersistCategory ePersistCategory) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if ((frameworkunifiedCheckValidAppHandle(hApp)) && (NULL != pTag)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

    MsgQInfo *pMqInfo = static_cast<MsgQInfo *>(pApp->hNPSndMsgQ);

    if (NULL != pMqInfo) {
      eStatus = NPSetFilePersistentType(pMqInfo->hMsgQ,
                                        pApp->cAppName,
                                        pTag,
                                        ePersistCategory);
    } else {
      eStatus = eFrameworkunifiedStatusNullPointer;
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPSetFolderPersistentType
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPSetFolderPersistentType(HANDLE hApp, PCSTR pTag, EFrameworkunifiedPersistCategory ePersistCategory) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if ((frameworkunifiedCheckValidAppHandle(hApp)) && (NULL != pTag)) {
    CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

    MsgQInfo *pMqInfo = static_cast<MsgQInfo *>(pApp->hNPSndMsgQ);

    if (NULL != pMqInfo) {
      eStatus = NPSetFolderPersistentType(pMqInfo->hMsgQ,
                                          pApp->cAppName,
                                          pTag,
                                          ePersistCategory);
    } else {
      eStatus = eFrameworkunifiedStatusNullPointer;
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

