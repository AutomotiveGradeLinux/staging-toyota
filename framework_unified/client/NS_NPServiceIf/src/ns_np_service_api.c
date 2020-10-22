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
/// \ingroup  tag_NPService
/// \brief    APIs to be used by clients while invoking Notification Service.
///
/// Implementation of APIs for Publishers and Subscribers of notifications.
///
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Include Files
///////////////////////////////////////////////////////////////////////////////
#include <native_service/ns_np_service.h>
#include <native_service/ns_np_service_protocol.h>
#include <native_service/ns_np_service_if.h>
#include <native_service/ns_message_center_if.h>
#include <native_service/ns_mc_system_info.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <limits.h>
#include <other_service/strlcpy.h>

// define user name as blank for global persistence file
#define GLOBAL_PERSISTENCE    ""

// this is the maximum number of notifications that can be registered/ unregistered at once
const UI_32 maxNotification = UINT_MAX / sizeof(NC_register_notif_msg);



////////////////////////////////////////////////////////////////////////////////////////////
/// NPRegisterNotifications
/// API to send message to Notification Service to register a set of notifications
///
/// \param [in] hNPMsgQ
///         HANDLE - Handle to message queue of Notification service.
/// \param [in] pPublisherName
///         PCSTR - Name of Publisher message queue
/// \param [in] numNotifications
///         PCSTR - Name of Notification
/// \param [in] pNotificationArray
///         NotificationInfo - Array of notifications
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPRegisterNotifications(HANDLE hNPMsgQ, PCSTR pPublisherName, UI_32 numNotifications,
                                   NotificationInfo *pNotificationArray) {
  EFrameworkunifiedStatus eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;

  if (maxNotification < numNotifications) {
    return eFrameworkunifiedStatusFail;
  }

  size_t sizeOfMsg = (sizeof(NC_register_notif_msg) * numNotifications) + sizeof(UI_32);
  if (NULL == pNotificationArray) {
    return eFrameworkunifiedStatusInvldParam;
  }

  PCHAR *pMsgBuffer = (PCHAR *)malloc(sizeOfMsg);

  if (NULL == pMsgBuffer) {
    return eFrameworkunifiedStatusFail;
  }

  NC_register_multiple_notif_msg *pNotifMsg = (NC_register_multiple_notif_msg *)pMsgBuffer;

  pNotifMsg->numNotifications = numNotifications;
  memcpy(&pNotifMsg->notifierList[0], pNotificationArray, sizeof(NC_register_notif_msg) * numNotifications);

  for(UI_32 i = 0; i < numNotifications; i++) {
    // Insert \0 at the end of character (up to 64bytes) by copying with strlcpy.
    strlcpy(pNotifMsg->notifierList[i].notificationName,
            pNotificationArray[i].notificationName,
            sizeof(pNotificationArray[i].notificationName));
  }

  eFrameworkunifiedStatus = McSend(hNPMsgQ, pPublisherName, NPS_REGISTER_EV_REQ, (UI_32)sizeOfMsg, (PVOID)pNotifMsg) ;

  free(pMsgBuffer);
  pMsgBuffer = NULL;

  return  eFrameworkunifiedStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  NPUnRegisterNotifications
/// API to send message to Notification Service to remove a set of notifications
///
/// \param [in] hNPMsgQ
///         HANDLE - Handle to message queue of Notification service.
/// \param [in] pPublisherName
///         PCSTR - Name of Publisher message queue
/// \param [in] numNotifications
///         PCSTR - Name of Notification
/// \param [in] pNotificationArray
///         NotificationInfo - Array of notifications
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPUnRegisterNotifications(HANDLE hNPMsgQ, PCSTR pPublisherName, UI_32 numNotifications,
                                     NotificationInfo *pNotificationArray) {
  EFrameworkunifiedStatus eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;

  if (maxNotification < numNotifications) {
    return eFrameworkunifiedStatusFail;
  }

  size_t sizeOfMsg = (sizeof(NC_unregister_notif_msg) * numNotifications) + sizeof(UI_32);
  UI_32 i = 0;
  if (NULL == pNotificationArray) {
    return eFrameworkunifiedStatusInvldParam;
  }

  PCHAR *pMsgBuffer = (PCHAR *)malloc(sizeOfMsg);

  if (NULL == pMsgBuffer) {
    return eFrameworkunifiedStatusFail;
  }

  NC_unregister_multiple_notif_msg *pNotifMsg = (NC_unregister_multiple_notif_msg *)pMsgBuffer;

  pNotifMsg->numNotifications = numNotifications;

  for (i = 0; i < numNotifications; ++i) {
    strlcpy(pNotifMsg->notificationList[i].notificationName, pNotificationArray[i].notificationName,
            sizeof(pNotifMsg->notificationList[i].notificationName));
  }

  // mb20110110 Fixed  issue 135
  eFrameworkunifiedStatus = McSend(hNPMsgQ, pPublisherName, NPS_UNREGISTER_EV_REQ, (UI_32)sizeOfMsg, (VOID *)pMsgBuffer) ;

  free(pMsgBuffer);
  pMsgBuffer = NULL;  // mb20110110  issue 134

  return  eFrameworkunifiedStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// NPRegisterNotification
/// API to send message to Notification Service to register a notification
///
/// \param [in] hNPMsgQ
///         HANDLE - Handle to message queue of Notification service.
/// \param [in] pPublisherName
///         PCSTR - Name of Publisher message queue
/// \param [in] notif_name
///         PCSTR - Name of Notification
/// \param [in] max_length
///         const UI_32 - Max size of the notification message
/// \param [in] bIsPersistent
///         const UI_8 - Flag to indicate if it has to be persistent
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPRegisterNotification(HANDLE hNPMsgQ, PCSTR pPublisherName, PCSTR notificationName,
                                  const UI_32 maxLength, const EFrameworkunifiedNotificationType perstype)

{
  EFrameworkunifiedStatus eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;

  if (NULL != hNPMsgQ && NULL != pPublisherName && NULL != notificationName) {
    if (strlen(pPublisherName) && strlen(notificationName)) {
      NotificationInfo data = {};
      strlcpy(data.notificationName, notificationName, sizeof(data.notificationName));
      data.persType = perstype;
      data.maxLength = maxLength;

      eFrameworkunifiedStatus = NPRegisterNotifications(hNPMsgQ, pPublisherName, 1, &data);
    } else {
      eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
    }
  } else {
    eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eFrameworkunifiedStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// NPUnRegisterNotification
/// API to send message to Notification Service to remove a notification
///
/// \param [in] hNPMsgQ
///         HANDLE - Handle to message queue of Notification service.
/// \param [in] pPublisherName
///         PCSTR - Name of Publisher message queue
/// \param [in] pNotification
///         PCSTR - Name of Notification
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPUnRegisterNotification(HANDLE hNPMsgQ, PCSTR pPublisherName, PCSTR pNotification) {

  NotificationInfo data = {};
  strlcpy(data.notificationName, pNotification, sizeof(data.notificationName));
  return (NPUnRegisterNotifications(hNPMsgQ, pPublisherName, 1, &data));
}

////////////////////////////////////////////////////////////////////////////////////////////
/// NPPublishNotification
/// API to send message to Notification Service to notify subscribers
///
/// \param [in] hNPMsgQ
///         HANDLE - Handle to message queue of Notification service.
/// \param [in] pPublisherName
///         PCSTR - Name of Publisher message queue
/// \param [in] pNotification
///         PCSTR - Name of Notification
/// \param [in] pData
///         VOID * - Data buffer
/// \param [in] iLength
///         const UI_32 - Size of data buffer
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPPublishNotification(HANDLE hNPMsgQ, PCSTR pPublisherName, PCSTR pNotification,
                                 PCVOID pData, const UI_32 iLength) {
  EFrameworkunifiedStatus eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;

  if (NULL != hNPMsgQ && NULL != pPublisherName && NULL != pNotification) {
    if (strlen(pPublisherName) && strlen(pNotification)) {
      if (strlen(pNotification) < MAX_STRING_SIZE_NOTIFICATION) {
        // Insert \0 at the end of characters (up to 64bytes) by copying pNotification to local variables with strlcpy.
        char tmp_notification[MAX_STRING_SIZE_NOTIFICATION] = {0};
        strlcpy(tmp_notification, pNotification, MAX_STRING_SIZE_NOTIFICATION);
        eFrameworkunifiedStatus =  McSendWithSysInfo(hNPMsgQ, pPublisherName, NPS_PUBLISH_EV_REQ,
                                        tmp_notification, iLength, pData, 0);
      } else {
        eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
      }
    } else {
      eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
    }
  } else {
    eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eFrameworkunifiedStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// NPSubscribeToNotification
/// API to send message to Notification Service to add to subscription list for
/// that notification
///
/// \param [in] hNPMsgQ
///         HANDLE - Handle to message queue of Notification service.
/// \param [in] pSubscriberName
///         PCSTR - Name of subscriber message queue
/// \param [in] pNotification
///         PCSTR - Name of Notification
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPSubscribeToNotification(HANDLE hNPMsgQ, PCSTR pSubscriberName, PCSTR pNotification) {
  EFrameworkunifiedStatus eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;
  if (NULL != hNPMsgQ && NULL != pSubscriberName && NULL != pNotification) {
    if (strlen(pSubscriberName) && strlen(pNotification)) {
      NC_subscribe_msg data;
      snprintf(data.notificationName, sizeof(data.notificationName), "%s", pNotification);

      eFrameworkunifiedStatus = McSend(hNPMsgQ, pSubscriberName, NPS_SUBSCRIBE_TO_EV_REQ, sizeof(NC_subscribe_msg), (PVOID)&data);
    } else {
      eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
    }
  } else {
    eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eFrameworkunifiedStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
// NPSubscribeToNotifications
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPSubscribeToNotifications(HANDLE hNPMsgQ, PCSTR pSubscriberName, UI_32 numNotifications,
                                      SubscribeInfo *pSubscribeInfoArray) {
  EFrameworkunifiedStatus eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;

  if (maxNotification < numNotifications) {
    return eFrameworkunifiedStatusFail;
  }

  size_t sizeOfMsg = (sizeof(NC_subscribe_msg) * numNotifications) + sizeof(UI_32);
  if (NULL == pSubscribeInfoArray) {
    return eFrameworkunifiedStatusInvldParam;
  }

  PCHAR *pMsgBuffer = (PCHAR *)malloc(sizeOfMsg);

  if (NULL == pMsgBuffer) {
    return eFrameworkunifiedStatusFail;
  }

  NC_subscribe_multiple_notif_msg *pSubscribeMsg = (NC_subscribe_multiple_notif_msg *)pMsgBuffer;

  pSubscribeMsg->numNotifications = numNotifications;
  memcpy(&pSubscribeMsg->notificationList[0], pSubscribeInfoArray, sizeof(NC_subscribe_msg) * numNotifications);

  for(UI_32 i = 0; i < numNotifications; i++) {
    // Insert \0 at the end of character (up to 64bytes) by copying with strlcpy.
    strlcpy(pSubscribeMsg->notificationList[i].notificationName,
            pSubscribeInfoArray[i].notificationName,
            sizeof(pSubscribeInfoArray[i].notificationName));
  }

  eFrameworkunifiedStatus = McSend(hNPMsgQ, pSubscriberName, NPS_BATCH_SUBSCRIBE_TO_EV_REQ, (UI_32)sizeOfMsg, (PVOID)pSubscribeMsg) ;

  free(pMsgBuffer);
  pMsgBuffer = NULL;

  return  eFrameworkunifiedStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// NPUnsubscribeFromNotification
/// API to send message to Notification Service to remove from subscription list for
/// that notification
///
/// \param [in] hNPMsgQ
///         HANDLE - Handle to message queue of Notification service.
/// \param [in] pSubscriberName
///         PCSTR - Name of subscriber message queue
/// \param [in] pNotification
///         PCSTR - Name of Notification
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPUnsubscribeFromNotification(HANDLE hNPMsgQ, PCSTR pSubscriberName, PCSTR pNotification) {
  EFrameworkunifiedStatus eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;

  if (NULL != hNPMsgQ && NULL != pSubscriberName && NULL != pNotification) {
    if (strlen(pSubscriberName) && strlen(pNotification)) {
      NC_unsubscribe_frm_notif_msg data;
      snprintf(data.notificationName, sizeof(data.notificationName), "%s", pNotification);

      eFrameworkunifiedStatus = McSend(hNPMsgQ, pSubscriberName, NPS_UNSUBSCRIBE_FROM_EV_REQ, sizeof(NC_unsubscribe_frm_notif_msg),
                          (PVOID)&data);
    } else {
      eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
    }
  } else {
    eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eFrameworkunifiedStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
// NPUnsubscribeToNotifications
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPUnsubscribeFromNotifications(HANDLE hNPMsgQ, PCSTR pUnSubscriberName, UI_32 numNotifications,
                                          SubscribeInfo *pUnSubscribeInfoArray) {
  EFrameworkunifiedStatus eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;

  if (NULL != hNPMsgQ && NULL != pUnSubscriberName) {
    if (strlen(pUnSubscriberName)) {
      if (maxNotification < numNotifications) {
        return eFrameworkunifiedStatusFail;
      }

      size_t sizeOfMsg = (sizeof(NC_subscribe_msg) * numNotifications) + sizeof(UI_32);
      if (NULL == pUnSubscribeInfoArray) {
        return eFrameworkunifiedStatusInvldParam;
      }

      PCHAR *pMsgBuffer = (PCHAR *)malloc(sizeOfMsg);

      if (NULL == pMsgBuffer) {
        return eFrameworkunifiedStatusFail;
      }

      NC_unsubscribe_multiple_notif_msg *pUnSubscribeMsg = (NC_unsubscribe_multiple_notif_msg *)pMsgBuffer;

      pUnSubscribeMsg->numNotifications = numNotifications;
      memcpy(&pUnSubscribeMsg->notificationList[0], pUnSubscribeInfoArray, sizeof(NC_subscribe_msg) * numNotifications);

      for(UI_32 i = 0; i < numNotifications; i++) {
        // Insert \0 at the end of character (up to 64bytes) by copying with strlcpy.
        strlcpy(pUnSubscribeMsg->notificationList[i].notificationName,
                pUnSubscribeInfoArray[i].notificationName,
                sizeof(pUnSubscribeInfoArray[i].notificationName));
      }

      eFrameworkunifiedStatus = McSend(hNPMsgQ, pUnSubscriberName, NPS_BATCH_UNSUBSCRIBE_FROM_EV_REQ, (UI_32)sizeOfMsg, (PVOID)pUnSubscribeMsg) ;

      free(pMsgBuffer);
      pMsgBuffer = NULL;
    } else {
      eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
    }
  } else {
    eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
  }

  return  eFrameworkunifiedStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// NPReadPersistedData
/// API to requested the persistent data corresponding to specified notification if available
/// The caller recieves an acknowledgement once NPS completes data retrieval and sends it.
///
/// \param [in] hNPMsgQ
///         HANDLE - Handle to message queue of Notification service.
/// \param [in] pPublisherName
///         PCSTR - Name of publisher message queue
/// \param [in] notification
///         PCSTR - Name of Notification
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPReadPersistedData(HANDLE hNPMsgQ, PCSTR pPublisherName, PCSTR notification) {
  EFrameworkunifiedStatus eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;

  if (NULL != hNPMsgQ && NULL != pPublisherName && NULL != notification) {
    if (strlen(pPublisherName) && strlen(notification)) {
      NC_get_pers_data_msg msg;

      snprintf(msg.notificationName, sizeof(msg.notificationName), "%s", notification);

      eFrameworkunifiedStatus = McSend(hNPMsgQ, pPublisherName, NPS_GET_PERS_DATA_REQ, sizeof(msg), (PVOID)&msg) ;
    } else {
      eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
    }
  } else {
    eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
  }

  return  eFrameworkunifiedStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  NPSavePersistentData
/// API to send message to Notification Service to save all persisted data that in Ram
/// to the filesystem via a file write.
///
/// \param [in] hNPMsgQ
///         HANDLE - Handle to message queue of Notification service.
/// \param [in] pPublisherName
///         PCSTR - Name of publisher message queue
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPSavePersistentData(HANDLE hNPMsgQ, PCSTR pPublisherName) {
  EFrameworkunifiedStatus eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;

  eFrameworkunifiedStatus = McSend(hNPMsgQ, pPublisherName, NPS_SAVE_PERS_DATA_REQ, 0, NULL) ;

  return eFrameworkunifiedStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// NPRegisterPersistentFile
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPRegisterPersistentFile(HANDLE hNPMsgQ, PCSTR pPublisherName, PCSTR pTag,
                                    BOOL bIsUserFile) {  // EFrameworkunifiedRegisterType eRegisterType)
  EFrameworkunifiedStatus eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;

  if (NULL != hNPMsgQ && NULL != pPublisherName && NULL != pTag) {
    if (strlen(pPublisherName) && strlen(pTag)) {
      NC_RegisterPersistentFileMsg tMsg;

      snprintf(tMsg.cFileTag, sizeof(tMsg.cFileTag), "%s", pTag);
      tMsg.bIsUserFile = bIsUserFile;

      eFrameworkunifiedStatus = McSend(hNPMsgQ, pPublisherName, NPS_SET_PERSIST_FILE_PATH_REQ, sizeof(tMsg),
                          (PVOID)&tMsg);
    } else {
      eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
    }
  } else {
    eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eFrameworkunifiedStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// NPLoadPersistentFile
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPLoadPersistentFile(HANDLE hNPMsgQ, PCSTR pPublisherName, PCSTR pDstFilePath,
                                PCSTR pTag, HANDLE hUser) {
  EFrameworkunifiedStatus eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;

  if (NULL != hNPMsgQ && NULL != pPublisherName &&
      NULL != pDstFilePath && NULL != pTag) {
    if (strlen(pPublisherName) && strlen(pDstFilePath) && strlen(pTag)) {
      NC_LoadPersistedFileMsg tMsg = {};

      snprintf(tMsg.cFilePath, sizeof(tMsg.cFilePath), "%s", pDstFilePath);
      snprintf(tMsg.cFileTag,  sizeof(tMsg.cFileTag),  "%s", pTag);

      if (hUser) {
        NC_User *pUser = (NC_User *) hUser;
        snprintf(tMsg.cUsername,  sizeof(tMsg.cUsername),  "%s", pUser->cUsername != 0 ? pUser->cUsername : NULL);
      } else {
        strlcpy(tMsg.cUsername, GLOBAL_PERSISTENCE, sizeof(tMsg.cUsername));
      }

      eFrameworkunifiedStatus = McSend(hNPMsgQ, pPublisherName, NPS_GET_PERS_FILE_REQ, sizeof(tMsg), (PVOID)&tMsg) ;
    } else {
      eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
    }
  } else {
    eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eFrameworkunifiedStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// NPReleasePersistentFile
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPReleasePersistentFile(HANDLE hNPMsgQ, PCSTR pPublisherName, EFrameworkunifiedReleaseType eFrameworkunifiedReleaseType, PCSTR pTag,
                                   PCSTR pFullFilePath,  HANDLE hUser) {
  EFrameworkunifiedStatus eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;

  if (NULL != hNPMsgQ && NULL != pPublisherName &&
      NULL != pTag && NULL != pFullFilePath) {
    if (strlen(pPublisherName) && strlen(pTag) && strlen(pFullFilePath)) {
      NC_ReleasePersistentFileMsg tMsg = {eFrameworkunifiedReleaseType, {0}, {0}};

      snprintf(tMsg.cFilePath, sizeof(tMsg.cFilePath), "%s", pFullFilePath);
      snprintf(tMsg.cFileTag, sizeof(tMsg.cFileTag), "%s", pTag);

      if (hUser) {
        NC_User *pUser = (NC_User *) hUser;
        snprintf(tMsg.cUsername,  sizeof(tMsg.cUsername),  "%s", pUser->cUsername != 0 ? pUser->cUsername : NULL);
      } else {
        strlcpy(tMsg.cUsername, GLOBAL_PERSISTENCE, sizeof(tMsg.cUsername));
      }

      eFrameworkunifiedStatus = McSend(hNPMsgQ, pPublisherName, NPS_RELEASE_PERS_FILE_REQ, sizeof(tMsg), (PVOID)&tMsg) ;
    } else {
      eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
    }
  } else {
    eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eFrameworkunifiedStatus;
}
////////////////////////////////////////////////////////////////////////////////////////////
/// NPPersistentSync
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPPersistentSync(PCSTR SrcName, HANDLE hNPMsgQ, UI_32 sessionid, PCSTR pPublisherName) {
  EFrameworkunifiedStatus eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;
  CHAR data[4];
  UI_32 length;
  HANDLE hResponse;
  char mc_invoker_name[MAX_QUEUE_NAME_SIZE];

  McCreateInvokerName(SrcName, sessionid, mc_invoker_name, sizeof(mc_invoker_name));
  hResponse = McOpenSyncReceiver(&mc_invoker_name[0]);
  if (hResponse != NULL) {
    eFrameworkunifiedStatus = McInvokeSync(hNPMsgQ, pPublisherName, NPS_NPP_SYNC_REQ   , sizeof(data), data, sessionid,
                              hResponse, 0, NULL, &length);
    McClose(hResponse);
  } else {
    eFrameworkunifiedStatus = eFrameworkunifiedStatusFail;
  }
  return eFrameworkunifiedStatus;
}
////////////////////////////////////////////////////////////////////////////////////////////
/// NPSetPersonality
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPSetPersonality(HANDLE hNPMsgQ, PCSTR pPublisherName, PCSTR pUserName) {
  EFrameworkunifiedStatus eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;

  if (NULL != hNPMsgQ && NULL != pPublisherName && NULL != pUserName) {
    if (strlen(pPublisherName) && strlen(pUserName)) {
      // NC_SetPersonalityMsg tMsg;
      NC_User tMsg;

      snprintf(tMsg.cUsername, sizeof(tMsg.cUsername), "%s", pUserName);

      // eFrameworkunifiedStatus = McSend(hNPMsgQ, pPublisherName, NPS_SET_PERSONALITY_REQ, sizeof(tMsg), (PVOID)&tMsg ) ;
      eFrameworkunifiedStatus = McSend(hNPMsgQ, pPublisherName, NPS_CHANGE_PERSONALITY_REQ, sizeof(tMsg), (PVOID)&tMsg) ;
    } else {
      eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
    }
  } else {
    eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eFrameworkunifiedStatus;
}
////////////////////////////////////////////////////////////////////////////////////////////
/// NPRegisterPersistentFolder
/// API to register persistence tag to folder that need to persist
///
/// \param [in] hNPMsgQ
///         HANDLE - Handle to message queue of Notification service.
/// \param [in] pPublisherName
///         PCSTR - Name of publisher message queue
/// \param [in] pTag
///         PCSTR - Tag associated to folder that need to persist
/// \param [in] bIsUserFolder
///         BOOL - TRUE - user specific folder
///          FALSE - global file
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPRegisterPersistentFolder(HANDLE hNPMsgQ, PCSTR pPublisherName, PCSTR pTag, BOOL bIsUserFolder) {
  EFrameworkunifiedStatus eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;

  if (NULL != hNPMsgQ && NULL != pPublisherName && NULL != pTag) {
    if (strlen(pPublisherName) && strlen(pTag)) {
      NC_RegisterPersistentFolderMsg tMsg;

      snprintf(tMsg.cFolderTag, sizeof(tMsg.cFolderTag), "%s", pTag);
      tMsg.bIsUserFolder = bIsUserFolder;

      eFrameworkunifiedStatus = McSend(hNPMsgQ, pPublisherName, NPS_SET_PERSIST_FOLDER_PATH_REQ, sizeof(tMsg),
                          (PVOID)&tMsg);
    } else {
      eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
    }
  } else {
    eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eFrameworkunifiedStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// NPLoadPersistentFolder
/// API to load folder to given path and associated with given tag from persistent memory
///
/// \param [in] hNPMsgQ
///         HANDLE - Handle to message queue of Notification service.
/// \param [in] pPublisherName
///         PCSTR - Name of publisher message queue
/// \param [in] pDstFolderPath
///         PCSTR - Destination where folder needs to be loaded
/// \param [in] pTag
///         PCSTR - Tag associated to folder that need to loaded from persistent memory
/// \param [in] hUser
///         HANDLE - Handle of the user
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPLoadPersistentFolder(HANDLE hNPMsgQ, PCSTR pPublisherName, PCSTR pDstFolderPath,
                                  PCSTR pTag, HANDLE hUser) {
  EFrameworkunifiedStatus eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;

  if (NULL != hNPMsgQ && NULL != pPublisherName &&
      NULL != pDstFolderPath && NULL != pTag) {
    if (strlen(pPublisherName) && strlen(pDstFolderPath) && strlen(pTag)) {
      NC_LoadPersistedFolderMsg tMsg;

      snprintf(tMsg.cFolderPath, sizeof(tMsg.cFolderPath), "%s", pDstFolderPath);
      snprintf(tMsg.cFolderTag,  sizeof(tMsg.cFolderTag),  "%s", pTag);

      if (hUser) {
        NC_User *pUser = (NC_User *) hUser;
        snprintf(tMsg.cUsername,  sizeof(tMsg.cUsername),  "%s", pUser->cUsername != 0 ? pUser->cUsername : NULL);
      } else {
        strlcpy(tMsg.cUsername, GLOBAL_PERSISTENCE, sizeof(tMsg.cUsername));
      }

      eFrameworkunifiedStatus = McSend(hNPMsgQ, pPublisherName, NPS_GET_PERS_FOLDER_REQ, sizeof(tMsg), (PVOID)&tMsg) ;
    } else {
      eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
    }
  } else {
    eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eFrameworkunifiedStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// NPReleasePersistentFolder
/// API to release folder for saving with tag name to persistent memory
///
/// \param [in] hNPMsgQ
///         HANDLE - Handle to message queue of Notification service.
/// \param [in] pPublisherName
///         PCSTR - Name of publisher message queue
/// \param [in] eFrameworkunifiedReleaseType
///         EFrameworkunifiedReleaseType - eFrameworkunifiedNotOnRelease     :not on release
///                           eFrameworkunifiedPersistOnShutdown:persist on shutdown
///                           eFrameworkunifiedPersistInstantly :persist instantly
/// \param [in] pTag
///         PCSTR - Tag associated to folder that need to persist
/// \param [in] pFullFolderPath
///         PCSTR - full folder path of folder that need to be stored in persistent memory
/// \param [in] hUser
///         HANDLE - Handle of the user
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPReleasePersistentFolder(HANDLE hNPMsgQ, PCSTR pPublisherName, EFrameworkunifiedReleaseType eFrameworkunifiedReleaseType,
                                     PCSTR pTag, PCSTR pFullFolderPath, HANDLE hUser) {
  EFrameworkunifiedStatus eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;

  if (NULL != hNPMsgQ && NULL != pPublisherName &&
      NULL != pTag && NULL != pFullFolderPath) {
    if (strlen(pPublisherName) && strlen(pFullFolderPath) && strlen(pTag)) {
      NC_ReleasePersistentFolderMsg tMsg = {eFrameworkunifiedReleaseType, {0}, {0}};

      snprintf(tMsg.cFolderPath, sizeof(tMsg.cFolderPath), "%s", pFullFolderPath);
      snprintf(tMsg.cFolderTag, sizeof(tMsg.cFolderTag), "%s", pTag);

      if (hUser) {
        NC_User *pUser = (NC_User *) hUser;
        snprintf(tMsg.cUsername,  sizeof(tMsg.cUsername),  "%s", pUser->cUsername != 0 ? pUser->cUsername : NULL);
      } else {
        strlcpy(tMsg.cUsername, GLOBAL_PERSISTENCE, sizeof(tMsg.cUsername));
      }

      eFrameworkunifiedStatus = McSend(hNPMsgQ, pPublisherName, NPS_RELEASE_PERS_FOLDER_REQ, sizeof(tMsg), (PVOID)&tMsg) ;
    } else {
      eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
    }
  } else {
    eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eFrameworkunifiedStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// NPChangePersonality
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPChangePersonality(HANDLE hNPMsgQ, PCSTR pPublisherName, PCSTR pUserName) {
  EFrameworkunifiedStatus eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;

  if (NULL != pUserName && 0 != strlen(pUserName)) {
    eFrameworkunifiedStatus = McSend(hNPMsgQ, pPublisherName, NPS_CHANGE_PERSONALITY_REQ, sizeof(pUserName), (PVOID)&pUserName) ;
  } else {
    eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
  }
  return eFrameworkunifiedStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// SendStopToNSNPP
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus SendStopToNSNPP(HANDLE hNPMsgQ, PCSTR pPublisherName, EFrameworkunifiedShutdownType eShutdownType, UI_32 uiStopMsgData) {
  EFrameworkunifiedStatus eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;

  if (NULL != pPublisherName && 0 != strlen(pPublisherName)) {
    NC_StopMsgData tMsg;
    tMsg.eShutdownType = eShutdownType;
    tMsg.uiStopMsgData = uiStopMsgData;

    eFrameworkunifiedStatus = McSend(hNPMsgQ, pPublisherName, NPS_NPP_STOP_REQ, sizeof(tMsg), (PVOID)&tMsg);
  } else {
    eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
  }
  return eFrameworkunifiedStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// NPGetReadyStatusOfNPP
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPGetReadyStatusOfNPP(HANDLE hNPMsgQ, PCSTR pRequesterName) {
  EFrameworkunifiedStatus eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;

  if (NULL != pRequesterName && 0 != strlen(pRequesterName)) {
    eFrameworkunifiedStatus = McSend(hNPMsgQ, pRequesterName, NPS_GET_READYSTATUS_REQ, 0, NULL);
  } else {
    eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
  }
  return eFrameworkunifiedStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// NPRegisterImmediateNotifications
/// API to send message to Notification Service to register a set of notifications
///
/// \param [in] hNPMsgQ
///         HANDLE - Handle to message queue of Notification service.
/// \param [in] pPublisherName
///         PCSTR - Name of Publisher message queue
/// \param [in] numNotifications
///         PCSTR - Name of Notification
/// \param [in] pNotificationArray
///         ImmediateNotificationInfo - Array of notifications
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPRegisterImmediateNotifications(HANDLE hNPMsgQ, PCSTR pPublisherName, UI_32 numNotifications,
                                            ImmediateNotificationInfo *pNotificationArray) {
  EFrameworkunifiedStatus eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;

  if (maxNotification < numNotifications) {
    return eFrameworkunifiedStatusFail;
  }

  size_t sizeOfMsg = (sizeof(NC_register_immediate_notif_msg) * numNotifications) + sizeof(UI_32);
  if (NULL == pNotificationArray) {
    return eFrameworkunifiedStatusInvldParam;
  }

  PCHAR *pMsgBuffer = (PCHAR *)malloc(sizeOfMsg);

  if (NULL == pMsgBuffer) {
    return eFrameworkunifiedStatusFail;
  }

  NC_register_multiple_immediate_notif_msg *pNotifMsg = (NC_register_multiple_immediate_notif_msg *)pMsgBuffer;

  if (NULL != pNotifMsg) {
      pNotifMsg->numNotifications = numNotifications;
  }

  memcpy(&pNotifMsg->notifierList[0], pNotificationArray, sizeof(NC_register_immediate_notif_msg) * numNotifications);

  for(UI_32 i = 0; i < numNotifications; i++ ) {
    // Insert \0 at the end of character (up to 64bytes) by copying with strlcpy.
    strlcpy(pNotifMsg->notifierList[i].notificationName,
            pNotificationArray[i].notificationName,
            sizeof(pNotificationArray[i].notificationName));
  }

  eFrameworkunifiedStatus = McSend(hNPMsgQ, pPublisherName, NPS_REGISTER_NOR_EV_REQ, (UI_32)sizeOfMsg, (PVOID)pNotifMsg) ;

  if (NULL != pMsgBuffer) {
    free(pMsgBuffer);
    pMsgBuffer = NULL;
  }
  return  eFrameworkunifiedStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// NPClearPersistedData
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPClearPersistedData(HANDLE hNPMsgQ, PCSTR pRequesterName, EFrameworkunifiedClearPersistence eFrameworkunifiedClearPersistenceScope) {
  EFrameworkunifiedStatus eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;

  if ((NULL != hNPMsgQ) && (NULL != pRequesterName) && (0 != strlen(pRequesterName))) {
    NC_ClearPersistedDataReq tMsg = {};
    tMsg.ePersistenceData = eFrameworkunifiedClearPersistenceScope;

    eFrameworkunifiedStatus = McSend(hNPMsgQ,
                        pRequesterName,
                        NPS_DELETE_PERSISTED_DATA_REQ,
                        sizeof(tMsg),
                        &tMsg);
  } else {
    eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
  }
  return eFrameworkunifiedStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// NPSetPersistNotfnDefaultValue
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPSetPersistNotfnDefaultValue(HANDLE hNPMsgQ, PCSTR pPublisherName, PCSTR pNotification, PCVOID pData,
                                         const UI_32 iLength) {
  EFrameworkunifiedStatus eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;

  if (NULL != hNPMsgQ && NULL != pPublisherName && NULL != pNotification) {
    if ((0 != strlen(pPublisherName)) && (0 != strlen(pNotification))) {

      // Insert \0 at the end of characters (up to 64bytes) by copying pNotification to local variables with strlcpy.
      char tmp_notification[MAX_STRING_SIZE_NOTIFICATION] = {0};
      strlcpy(tmp_notification, pNotification, MAX_STRING_SIZE_NOTIFICATION);

      eFrameworkunifiedStatus =  McSendWithSysInfo(hNPMsgQ,
                                      pPublisherName,
                                      NPS_SET_DEFAULT_PERS_DATA,
                                      (PCHAR)tmp_notification,
                                      iLength,
                                      pData,
                                      0);
    } else {
      eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
    }
  } else {
    eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eFrameworkunifiedStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// NPSetPersistentNotfnType
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPSetPersistentNotfnType(HANDLE hNPMsgQ, PCSTR pPublisherName, PCSTR pNotification,
                                    EFrameworkunifiedPersistCategory ePersistCategory) {
  EFrameworkunifiedStatus eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;

  if (NULL != hNPMsgQ && NULL != pPublisherName && NULL != pNotification) {
    if ((0 != strlen(pPublisherName)) && (0 != strlen(pNotification))) {

      // Insert \0 at the end of characters (up to 64bytes) by copying pNotification to local variables with strlcpy.
      char tmp_notification[MAX_STRING_SIZE_NOTIFICATION] = {0};
      strlcpy(tmp_notification, pNotification, MAX_STRING_SIZE_NOTIFICATION);

      eFrameworkunifiedStatus =  McSendWithSysInfo(hNPMsgQ,
                                      pPublisherName,
                                      NPS_SET_NOTFN_PERSISTENT_TYPE,
                                      (PCHAR)tmp_notification,
                                      sizeof(ePersistCategory),
                                      &ePersistCategory,
                                      0);
    } else {
      eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
    }
  } else {
    eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eFrameworkunifiedStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// NPSetFilePersistentType
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPSetFilePersistentType(HANDLE hNPMsgQ, PCSTR pPublisherName,
                                   PCSTR pTag, EFrameworkunifiedPersistCategory ePersistCategory) {
  EFrameworkunifiedStatus eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;

  if (NULL != hNPMsgQ && NULL != pPublisherName && NULL != pTag) {
    if ((0 != strlen(pPublisherName)) && (0 != strlen(pTag))) {
      NC_SetFilePersistType tMsg = {};

      tMsg.ePersistType = ePersistCategory;

      snprintf(tMsg.cTag, sizeof(tMsg.cTag), "%s", pTag);

      eFrameworkunifiedStatus = McSend(hNPMsgQ,
                          pPublisherName,
                          NPS_SET_FILE_PERSISTENT_TYPE,
                          sizeof(tMsg),
                          (PVOID)&tMsg) ;
    } else {
      eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
    }
  } else {
    eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eFrameworkunifiedStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// NPSetFolderPersistentType
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPSetFolderPersistentType(HANDLE hNPMsgQ, PCSTR pPublisherName,
                                     PCSTR pTag, EFrameworkunifiedPersistCategory ePersistCategory) {
  EFrameworkunifiedStatus eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;

  if (NULL != hNPMsgQ && NULL != pPublisherName && NULL != pTag) {
    if ((0 != strlen(pPublisherName)) && (0 != strlen(pTag))) {
      NC_SetFolderPersistType tMsg = {};

      tMsg.ePersistType = ePersistCategory;
      snprintf(tMsg.cTag, sizeof(tMsg.cTag), "%s", pTag);

      eFrameworkunifiedStatus = McSend(hNPMsgQ,
                          pPublisherName,
                          NPS_SET_FOLDER_PERSISTENT_TYPE,
                          sizeof(tMsg),
                          (PVOID)&tMsg) ;
    } else {
      eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
    }
  } else {
    eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eFrameworkunifiedStatus;
}

// EOF
