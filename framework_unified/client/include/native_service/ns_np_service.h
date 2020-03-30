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

//////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NPPService
/// \brief    Header for message structures for notification_persistent_service.
///
/// Declares the external APIs to Notification and Persistence Service.
///
//////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @file ns_np_service.h
 */

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup native_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup notification_persistent_service
 *  @ingroup native_service
 *  @{
 */

#ifndef __NATIVESERVICES_NP_SERVICE_H__  // NOLINT  (build/header_guard)
#define __NATIVESERVICES_NP_SERVICE_H__

#include <native_service/frameworkunified_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \~english  A structure to register notification msg
 */
typedef struct _NC_register_notif_msg {
  CHAR  notificationName[MAX_STRING_SIZE_NOTIFICATION];  //!< \~english notification event name string
  UI_32 maxLength;                       //!< \~english Maximum possible length (bytes) of
                                          // the message data
  // UI_8  bIsPersistent;                     ///< Flag to indicate if data has to be persisted.
  EFrameworkunifiedNotificationType persType;           //!< \~english Specifies persitence for this notification
  UI_8  dummy[4];                          //!< \~english Packing to 32 bit boundary
} NC_register_notif_msg;

/**
 * \~english  A structure to register multiple notification msg
 */
typedef struct _NC_register_multiple_notif_msg {
  UI_32 numNotifications;                 //!< \~english This has to be UI_32 to avoid holes.
  NC_register_notif_msg notifierList[1];  //!< \~english variable number of structures
} NC_register_multiple_notif_msg;

/**
 * \~english  A structure to register immediate notification msg
 */
typedef struct _NC_register_immediate_notif_msg {
  CHAR  notificationName[MAX_STRING_SIZE_NOTIFICATION];  //!< \~english Notification Event Name string
  UI_32 maxLength;                       //!< \~english Maximum possible length (bytes) of
                                          // the message data
  EFrameworkunifiedNotificationType persType;           //!< \~english Specifies persitence for this notification
  UI_32 delay;              //!< \~english Delay time for persistence
  UI_8  dummy[4];                          //!< \~english Packing to 32 bit boundary
} NC_register_immediate_notif_msg;

/**
 * \~english  A structure to register multiple immediate notification msg
 */
typedef struct _NC_register_multiple_immediate_notif_msg {
  UI_32 numNotifications;                 //!< \~english This has to be UI_32 to avoid holes.
  NC_register_immediate_notif_msg notifierList[1];  //!< \~english variable number of structures
} NC_register_multiple_immediate_notif_msg;

/**
 * \~english  A structure to unregister notification msg
 */
typedef struct _NC_unregister_notif_msg {
  CHAR notificationName[MAX_STRING_SIZE_NOTIFICATION];  //!< \~english Notification Event Name string
} NC_unregister_notif_msg, NC_subscribe_msg, NC_unsubscribe_frm_notif_msg, NC_get_pers_data_msg;

/**
 * \~english  A structure to unregister multiple notification msg
 */
typedef struct _NC_unregister_multiple_notif_msg {
  UI_32 numNotifications;                       //!< \~english notification event name num
  NC_unregister_notif_msg notificationList[1];  //!< \~english notification event name List
} NC_unregister_multiple_notif_msg;

/**
 * \~english  A structure to save pers data
 */
typedef struct _NC_savepersdata_ack {
  EFrameworkunifiedStatus eStatus;  //!< \~english eFrameworkunifiedStatusOK is (valid - data stored, anything else is
} NC_savepersdata_ack;

/**
 * \~english  A structure to subscribe multiple notification msg
 */
typedef struct _NC_subscribe_multiple_notif_msg {
  UI_32 numNotifications;                //!< \~english notification event name num
  NC_subscribe_msg notificationList[1];  //!< \~english notification event name List
} NC_subscribe_multiple_notif_msg;

/**
 * \~english  A structure to subscribe multiple notification msg
 */
typedef struct _NC_unsubscribe_multiple_notif_msg {
  UI_32 numNotifications;                            //!< \~english notification event name num
  NC_unsubscribe_frm_notif_msg notificationList[1];  //!< \~english notification event name List
} NC_unsubscribe_multiple_notif_msg;

/**
 * \~english  A structure to get pers data failed
 */
typedef struct __NC_get_persdata_failed_ack {
  CHAR notificationName[MAX_STRING_SIZE_NOTIFICATION];      //!< \~english notification name
} NC_get_persdata_failed_ack;

/**
 * \~english  A structure to set personality
 */
typedef struct _NC_User {
  CHAR cUsername[MAX_PATH_LENGTH];       //!< \~english name of personality
} NC_User;

/**
 * \~english  A structure to register persistent file with the notification_persistent_service
 */
typedef struct _NC_RegisterPersistentFileMsg {
  CHAR cFileTag[MAX_STRING_SIZE_TAG];  //!< \~english Tag associated with the
                                       // file that would be persisted
  BOOL bIsUserFile;                    //!< \~english is user file
} NC_RegisterPersistentFileMsg;

/**
 * \~english  A structure to specify where to load the file associated with the Tag
 */
typedef struct _NC_LoadPersistedFileMsg {
  CHAR cFileTag[MAX_STRING_SIZE_TAG];  //!< \~english Tag associated with the file
  CHAR cFilePath[MAX_PATH_LENGTH];     //!< \~english path to which the file needs to retrieved
  CHAR cUsername[MAX_PATH_LENGTH];     //!< \~english Name of the user for user specific file.
} NC_LoadPersistedFileMsg;

/**
 * \~english  A structure to release persistent file
 */
typedef struct _NC_ReleasePersistentFileMsg {
  EFrameworkunifiedReleaseType eFrameworkunifiedReleaseType;             //!< \~english should the file be persisted
  CHAR cFilePath[MAX_PATH_LENGTH];             //!< \~english path from which the
                                               // file needs to be picked up to persist
  CHAR cFileTag[MAX_STRING_SIZE_NOTIFICATION];  //!< \~english Tag associated with the
                                                // file to be persisted
  CHAR cUsername[MAX_PATH_LENGTH];             //!< \~english Name of the user for user specific file.
} NC_ReleasePersistentFileMsg;

/**
 * \~english  A structure to register persistent folder with the notification_persistent_service
 */
typedef struct _NC_RegisterPersistentFolderMsg {
  CHAR cFolderTag[MAX_STRING_SIZE_TAG];  //!< \~english Tag associated with the
                                         // folder that would be persisted
  BOOL bIsUserFolder;                    //!< \~english Check if the folder is of user type
} NC_RegisterPersistentFolderMsg;

/**
 * \~english  A structure to specify where to load the folder associated with the Tag
 */
typedef struct _NC_LoadPersistedFolderMsg {
  CHAR cFolderTag[MAX_STRING_SIZE_TAG];  //!< \~english Tag associated with the folder
  CHAR cFolderPath[MAX_PATH_LENGTH];     //!< \~english path to which the folder needs to retrieved
  CHAR cUsername[MAX_PATH_LENGTH];       //!< \~english Name of the user for user specific folder.
} NC_LoadPersistedFolderMsg;

/**
 * \~english  A structure to Release Persistent Folder Msg
 */
typedef struct _NC_ReleasePersistentFolderMsg {
  EFrameworkunifiedReleaseType eFrameworkunifiedReleaseType;               //!< \~english should the file be persisted
  CHAR cFolderPath[MAX_PATH_LENGTH];             //!< \~english path from which the folder
                                                  // needs to be picked up to persist
  CHAR cFolderTag[MAX_STRING_SIZE_NOTIFICATION];  //!< \~english Tag associated with
                                                  // the folder to be persisted
  CHAR cUsername[MAX_PATH_LENGTH];               //!< \~english Name of the user for user
                                                 // specific folder.
} NC_ReleasePersistentFolderMsg;

/**
 * \~english  A structure to Load Persisted Ack
 */
typedef struct _NC_LoadPersistedAck {
  EFrameworkunifiedStatus eStatus;         //!< \~english ack the success/failure in
                              // retrieving the file/folder from persistence
  CHAR cTag[MAX_PATH_LENGTH];  //!< \~english tag of the file/folder for which acknowledge is made
} NC_LoadPersistedAck, NC_ReleasePersistedAck;

/**
 * \~english  A structure to Nor Persistent Data
 */
typedef struct _NC_NorPersistentData {
  CHAR notificationName[MAX_STRING_SIZE_NOTIFICATION];  //!< \~english Notification Event Name string
  CHAR pPublisherName[MAX_NAME_SIZE_APP];               //!< \~english Publisher Event Name string
  UI_32 dataSize;                                       //!< \~english data size
  UI_32 uiDelay;                                        //!< \~english ui Delay
  UI_32 uiMaxSize;                                      //!< \~english ui Max Size
} NC_NorPersistentData;

/**
 * \~english  A structure to request for deleting data from persistent memory
 */
typedef struct _NC_ClearPersistedDataReq {
  EFrameworkunifiedClearPersistence ePersistenceData;  //!< \~english defines what
                                          // data to be deleted from persistent memory
} NC_ClearPersistedDataReq;

/**
 * \~english  A structure for receiving ack of data deleted from persistent memory
 */
typedef struct _NC_ClearPersisteDatadAck {
  EFrameworkunifiedStatus eStatus;  //!< \~english ack status of ClearPersistedData request
} NC_ClearPersisteDatadAck;

/**
 * \~english  A structure to request stop message to notification_persistent_service.
 */
typedef struct _NC_StopMsgData {
  EFrameworkunifiedShutdownType eShutdownType;  //!< \~english shutdown type
  UI_32 uiStopMsgData;  //!< \~english Reason for sending stop message to NPPService
} NC_StopMsgData;

/**
 * \~english  A structure to set the persist type of file and folder
 */
typedef struct _NC_SetPersistType {
  EFrameworkunifiedPersistCategory ePersistType;  //!< \~english persist type
  CHAR cTag[MAX_PATH_LENGTH];   //!< \~english tag name of the file or folder
} NC_SetFilePersistType, NC_SetFolderPersistType;

/**
 * \~english  A data received with ack sent by notification_persistent_service when immediate notification data is
 * written in persistent memory
 */
typedef struct _NC_ImmediateWriteAck {
  CHAR notificationName[MAX_STRING_SIZE_NOTIFICATION];  //!< \~english notification Name
  EFrameworkunifiedStatus eStatus;                                  //!< \~english status of request
} NC_ImmediateWriteAck;

#ifdef __cplusplus
}
#endif

#endif /* __NATIVESERVICES_NP_SERVICE_H__ */  // NOLINT  (build/header_guard)
/** @}*/  // end of notification_persistent_service
/** @}*/  // end of native_service
/** @}*/  // end of BaseSystem
// EOF
