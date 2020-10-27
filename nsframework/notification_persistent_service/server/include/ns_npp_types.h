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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup <<Group Tag>> <<Group Name>>
/// \ingroup  tag_NS_NPPService
/// .
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NS_NPPService
/// \brief
///
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_TYPES_H_
#define NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_TYPES_H_

#include <native_service/frameworkunified_sm_eventdata.h>
#include <aglpath.h>
#include <native_service/ns_np_types.h>

// name of notification file to be saved in persistent memory depending on the persistent type category

#define NOTIFICATIONFILE "PERSISTENT_NOTIFICATION"     // stores user notification common to all user
#define USERNOTIFICATIONFILE "USER_PERSISTENT_NOTIFICATION"    // stores user notification specific to particular user
#define FACTORYNOTIFICATIONFILE "FACTORY_NOTIFICATION"        // stores factory notification
#define FACTORYCUSTOMERNOTIFICATIONFILE   "FACTORYCUSTOMER_NOTIFICATION"      // stores factorycustomer notification
#define DEALERNOTIFICATIONFILE        "DEALER_NOTIFICATION"       // stores dealer notification

// name of persistence folder for persistent type categorization
#define USERDATA          "UserData"          // name of folder contatining userdata
#define FACTORYDATA         "FactoryData"       // name of folder contatining factory data
#define FACTORYCUSTOMERDATA     "FactoryCustomerData"   // name of folder contatining factory customer data
#define DEALERDATA          "DealerData"        // name of folder contatining dealer data

// default user name for personalization
#define DEFAULTUSERNAME           "DefaultUser"

// name of NSNPS version file
#define VERSION_TXT             "version.txt"

#define STORAGE_PATH             "/nv/BS/ns/npp/rwdata/"


typedef enum ENPSLoadType {
  LOADTYPE_NONE    = -1,
  LOADTYPE_RELEASE = 0,  // load file to persistent memory
  LOADTYPE_LOAD    = 1,  // load file to temporary memory
  LOADTYPE_RELOAD  = 2,  // reload the file.
} ENPS_Loadtype;

/// \typedef ENotificationpersistentservicePersistType
/// \brief Persist file types
typedef enum ENotificationpersistentservicePersistType {
  ENOTIFICATIONPERSISTENTSERVICEPERSISTFIRST = 0,   ///< Add values after this value only.
  ENOTIFICATIONPERSISTENTSERVICEPERSISTFILE,        ///< Persist a file to a persistent memory
  ENOTIFICATIONPERSISTENTSERVICEPERSISTFOLDER,      ///< Persist a folder to a persistent memory
  ENOTIFICATIONPERSISTENTSERVICEPERSISTNONE,        ///< no type
  ENOTIFICATIONPERSISTENTSERVICEPERSISTLAST,        ///< Add values before this value only.
} ENotificationpersistentservicePersistType;

typedef enum ENPSPersistenceJobState {
  ENOTIFICATIONPERSISTENTSERVICEPERSISTJObSTATEIDLE = 0,            ///< Job is idle.  // NOLINT (readability/naming)
  ENOTIFICATIONPERSISTENTSERVICEPERSISTJObSTATEINPROCESS = 1,       ///< Job is in process. Either release or load.  // NOLINT (readability/naming)
  ENOTIFICATIONPERSISTENTSERVICEPERSISTJObSTATERELEASEABORTED = 2,  ///< Job release was aborted due to abort shutdown.  // NOLINT (readability/naming)
} ENPSPersistenceJobState;

typedef CHAR CopyInfoStr[MAX_PATH_LENGTH];  ///< Information passed via the framework


typedef enum ENPSCopyWorkerProtocol {
  // > Command(s)
  CP_WRK_CMD_START                = 1500,
  CP_WRK_CMD_STOP                 = 1501,
  CP_WRK_CMD_COPY                 = 1502,
  CP_WRK_CMD_RESUME               = 1503,  // This will release the thread from the abort command.

  // < Command Ack(s)
  CP_WRK_ACK_CMD_COMPLETE           = 1504,

  // < Notification(s)
  CP_WRK_NTFY                     = 1505,

  // < Shutdown request command
  CMD_WRK_SHUTDOWN_REQ          = 1506,
  CMD_WRK_SHUTDOWN_ACK          = 1507,

  CMD_DELETE_OLD_DATA           = 1508,

  AR_CMD_START                  = 1400,
  AR_CMD_STOP                   = 1401,
  AR_CMD_ARCHIVE                = 1402,
  AR_CMD_RESUME                 = 1403,  // This will release the thread from the abort command.

  // < Command Ack(s)
  AR_ACK_CMD_COMPLETE           = 1404,

  // < Notification(s)
  AR_CMD_NTFY                     = 1405,

  NOR_PERSISTENCE_TIMER_START       = 1406,
  NOR_PERSISTENCE_REGISTER        = 1407,
  NOR_PERSISTENCE_UNREGISTER        = 1408,
  // this cmd is used to tell the NOR thread to stop the timer
  // for all the the notifications in case of userdata reset during shutdown
  NOR_PERSISTENCE_ONSHUTDOWN        = 1409,
  // while to persist the data immediately irrespective of delay in case of shutdown without userdata reset.
  // ack sent to NPPService main thread by Nor worker thread after persisting immediate data
  NOR_PERSISTENCE_ONSHUTDOWN_ACK          = 1410,
  // message sent by main thread to immediate persistence thread
  // to change the persistent category of immediate notification
  NOR_PERSISTENCE_CHANGE_CATEGORY         = 1411
} ENSP_CopyWorkerProtocol, *PENSP_CopyWorkerProtocol, ENPS_ArchiveProtocol;


typedef enum ENPSCopyWorkerFailures {
  // > Failure codes(s)
  CP_WRK_FAILURE_SRC_NOT_FND            = 3500,
  CP_WRK_FAILURE_DST_CREATE           = 3501,
  CP_WRK_FAILURE_ABORT_DURING_CP      = 3502,
  CP_WRK_FAILURE_ABORT_BEFORE_CP      = 3503,
  CP_WRK_FAILURE_WRITE_ERROR_CP       = 3504,

  AR_THREAD_FAILURE_SRC_NOT_FND         = 3400,
  AR_THREAD_FAILURE_DST_CREATE          = 3401,
  AR_THREAD_FAILURE_ABORT_DURING_ARCHIVE  = 3402,
  AR_THREAD_FAILURE_ABORT_BEFORE_ARCHIVE  = 3403,
} ENPS_CopyWorkerFailures, *PENPS_CopyWorkerFailures, ENPS_ArchiveFailures;

typedef struct NPSCopyAckMsg {
  ENSP_CopyWorkerProtocol m_eworkerprotocol;  // Worker Load or Release Protocol Id
} NSP_CopyAckMsg;

typedef enum NotificationpersistentserviceCompressionType {
  ENOTIFICATIONPERSISTENTSERVICENONE = 0,       /// No Compression before file persistence
  ENOTIFICATIONPERSISTENTSERVICEDEFAULTCOMPRESSION,   /// Compress file before persistence using default compression method
  ENOTIFICATIONPERSISTENTSERVICECOMPRESSUSINGLIBZ   /// Compress file before persistence using libz
} NOTIFICATIONPERSISTENTSERVICE_COMPRESSION_TYPE;

typedef struct NPSCopyInfoMsg {
  CopyInfoStr     m_csourcepath;      // Source path
  CopyInfoStr     m_cdestinationpath;  // Destination path
  CopyInfoStr     m_cpersistenttag;   // Tag for Persistence
  CopyInfoStr     m_crequesterappname;  // Requester Name
  ENPS_Loadtype     m_eloadtype;    // Load to Persistent or Temporary Memory
  ENotificationpersistentservicePersistType   m_epersisttype;     // Folder or File Persistence
  NOTIFICATIONPERSISTENTSERVICE_COMPRESSION_TYPE m_ecompressiontype;  // Check for Compression of the file
} NSP_CopyInfoCmd;

typedef struct NPSCopyStatus {
  BOOL              m_bpersistencechk;  // Persistence Success or Failure
  ENPS_CopyWorkerFailures m_ecopyfailures;    // Failure Protocol ID
  CopyInfoStr       m_cpersistenttag;   // Tag for Persistence
  CopyInfoStr       m_crequesterappname;  // Requester Name
  ENPS_Loadtype     m_eloadtype;    // Load to Persistent or Temporary Memory
  ENotificationpersistentservicePersistType     m_epersisttype;   // Folder or File Persistence
  NOTIFICATIONPERSISTENTSERVICE_COMPRESSION_TYPE m_ecompressiontype;    // Check for Compression of the file
} NSP_CopyStatusResponse, NSP_CopyCancel;


// Structure for Nor Persistent Notification
typedef struct TNorPersistenceNotifInfoHeader {
  CHAR m_cnotificationname[MAX_STRING_SIZE_NOTIFICATION];  ///< Name of Notification
  UI_32 m_uimaxmsglength;             ///< Maximum data length of notification
  EFrameworkunifiedNotificationType m_epersistenttype;     ///< type of notification
  CHAR m_cpublishername[MAX_NAME_SIZE_APP];   ///< Service Name
  UI_32 m_uimsgsize;                ///< length of message
  UI_32 m_uidelay;                ///< Time delay between persistence
  EFrameworkunifiedPersistCategory m_epersistcategory;     ///< Persistent Category
} TNorPersistenceNotifInfoHeader;

typedef struct _TImmediatePersistenceRegisterNotifInfo { // NOLINT (readability/naming)
  CHAR  m_cnotificationname[MAX_STRING_SIZE_NOTIFICATION];    ///< Name of Notification
  UI_32 m_uidelay;                ///< Time delay between persistence
  EFrameworkunifiedPersistCategory m_epersistcategory;     ///< Persistent Category
} TImmediatePersistenceRegisterNotifInfo;

typedef struct _TImmediatePersistenceUnregisterNotifInfo { // NOLINT (readability/naming)
  CHAR  m_cnotificationname[MAX_STRING_SIZE_NOTIFICATION];  ///< Name of Notification
} TImmediatePersistenceUnregisterNotifInfo;

typedef struct _NPS_CopyShutdown { // NOLINT (readability/naming)
  CopyInfoStr m_crequesterappname;
} NSP_CopyShutdown, NSP_CopyShutdownAck;

typedef struct _TImmediatePersistenceChangeCategory { // NOLINT (readability/naming)
  TNorPersistenceNotifInfoHeader  m_tnornotifInfoheader;    ///< Notification info with new persist category type
  EFrameworkunifiedPersistCategory m_eoldpersistcategory;     ///< Old Persistent Category of notification
} TImmediatePersistenceChangeCategory;

typedef struct _TImmediateShutdown { // NOLINT (readability/naming)
  EFrameworkunifiedShutdownType f_eshutdowntype;
  UI_32 f_uinotificationpersistentservicepersistcategoryflag;
} TImmediateShutdown;

#endif  // NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_TYPES_H_
