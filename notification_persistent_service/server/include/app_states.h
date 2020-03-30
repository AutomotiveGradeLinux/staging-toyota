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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup <<Group Tag>> <<Group Name>>
/// \ingroup  tag_NS_NPPService
/// .
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NS_NPPService
/// \brief    This file all the states, events, internal transitions of state machine.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_APP_STATES_H_
#define NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_APP_STATES_H_

#include <native_service/frameworkunified_sm_framework_if.h>
#include <native_service/ns_np_service.h>
#include <string>

const UI_32 kCuiEventIdStartLimit = 20;

DEFINE_COMPOSITESTATE(sNotificationpersistentserviceNPPRoot)  // NOLINT (readability/naming)
DEFINE_ORTHOGONALSTATE_OVERRIDE(sNotificationpersistentserviceNPPStart)
DEFINE_LEAFSTATE(sNotificationpersistentserviceNPPError)

// =====Define Persistence state
DEFINE_LEAFSTATE_OVERRIDE(sNotificationpersistentservicePersistenceReady)
DEFINE_LEAFSTATE(sNotificationpersistentservicePersistenceError)

// DEFINE_INTERNALTRANSITION(NotificationpersistentserviceOnLoadPersistentData)
DEFINE_INTERNALTRANSITION(NotificationpersistentserviceOnNPReadPersistedData)
DEFINE_INTERNALTRANSITION(NotificationpersistentserviceOnNPReleaseFileAck)
DEFINE_INTERNALTRANSITION(NotificationpersistentserviceOnSaveDataAck)
DEFINE_INTERNALTRANSITION(NotificationpersistentserviceOnNPNorDataSaveAck)
DEFINE_INTERNALTRANSITION(NotificationpersistentserviceOnRegisterPersistentFile)
DEFINE_INTERNALTRANSITION(NotificationpersistentserviceOnReleasePersistentFile)
DEFINE_INTERNALTRANSITION(NotificationpersistentserviceOnLoadPersistentFile)
DEFINE_INTERNALTRANSITION(NotificationpersistentserviceOnRegisterPersistentFolder)
DEFINE_INTERNALTRANSITION(NotificationpersistentserviceOnLoadPersistentFolder)
DEFINE_INTERNALTRANSITION(NotificationpersistentserviceOnReleasePersistentFolder)
DEFINE_INTERNALTRANSITION(NotificationpersistentserviceOnNPSetFilePersistentType)
DEFINE_INTERNALTRANSITION(NotificationpersistentserviceOnNPSetFolderPersistentType)

DEFINE_INTERNALTRANSITION(NotificationpersistentserviceOnUserChange)
DEFINE_INTERNALTRANSITION(NotificationpersistentserviceCheckAllReleaseRequestsProcessed)
DEFINE_INTERNALTRANSITION(NotificationpersistentserviceOnNPPersistentSync)

// =====Define Shutdown state
DEFINE_LEAFSTATE(sNotificationpersistentserviceShutdownIdle)
// DEFINE_LEAFSTATE_OVERRIDE(sNotificationpersistentserviceShutdownDataSave)
class CsNotificationpersistentserviceShutdownDataSave : public CFrameworkunifiedLeafState {
 public:
  explicit CsNotificationpersistentserviceShutdownDataSave(std::string f_pName);
  virtual ~CsNotificationpersistentserviceShutdownDataSave();
  EFrameworkunifiedStatus FrameworkunifiedOnEntry(CEventDataPtr f_peventdata);
  EFrameworkunifiedStatus FrameworkunifiedOnExit(CEventDataPtr f_peventdata);
 private:
  std::string m_cShutdownRequestor;
};

DEFINE_INTERNALTRANSITION(NotificationpersistentserviceOnNPShutdown)  // NOLINT (readability/naming)
DEFINE_INTERNALTRANSITION(NotificationpersistentserviceOnShutdownTimeout)
DEFINE_INTERNALTRANSITION(NotificationpersistentserviceReInitShutdownTimer)

// =====Define Notification state
DEFINE_LEAFSTATE(sNotificationpersistentserviceNotificationReady)
DEFINE_LEAFSTATE(sNotificationpersistentserviceNotificationError)

DEFINE_INTERNALTRANSITION(NotificationpersistentserviceOnNPPublishNotification)
DEFINE_INTERNALTRANSITION(NotificationpersistentserviceOnNPRegisterNotifications)
DEFINE_INTERNALTRANSITION(NotificationpersistentserviceOnNPSubscribeToNotification)
DEFINE_INTERNALTRANSITION(NotificationpersistentserviceOnNPSubscribeToNotifications)
DEFINE_INTERNALTRANSITION(NotificationpersistentserviceOnNPUnRegisterNotifications)
DEFINE_INTERNALTRANSITION(NotificationpersistentserviceOnNPUnSubscribeFromNotification)
DEFINE_INTERNALTRANSITION(NotificationpersistentserviceOnNPUnSubscribeFromNotifications)
DEFINE_INTERNALTRANSITION(NotificationpersistentserviceOnNPRegisterImmediateNotifications)
DEFINE_INTERNALTRANSITION(NotificationpersistentserviceOnNPGetReadyStatus)
DEFINE_INTERNALTRANSITION(NotificationpersistentserviceOnNPClearPersistedData)
DEFINE_INTERNALTRANSITION(NotificationpersistentserviceOnNPSetDefaultPersistentData)
DEFINE_INTERNALTRANSITION(NotificationpersistentserviceOnNPSetNotfnPersistentType)
DEFINE_INTERNALTRANSITION(NotificationpersistentserviceOnNPPublishImmediateNotification)

#ifdef NPP_PROFILEINFO_ENABLE

DEFINE_INTERNALTRANSITION(NotificationpersistentserviceOnNPProfileNotifications)
DEFINE_INTERNALTRANSITION(NotificationpersistentserviceOnNPProfilePersistence)

#endif

// ==========Define event
DEFINE_EVENT(evIdle,              kCuiEventIdStartLimit + 1)
DEFINE_EVENT(evReady,               kCuiEventIdStartLimit + 2)
DEFINE_EVENT(evShutdown,            kCuiEventIdStartLimit + 3)
DEFINE_EVENT(evError,               kCuiEventIdStartLimit + 4)

DEFINE_EVENT(evNPSetNotfnPersistentType,    kCuiEventIdStartLimit + 5)
DEFINE_EVENT(evNPSetDefaultPersistentData,    kCuiEventIdStartLimit + 6)
DEFINE_EVENT(evNPReadPersistedData,       kCuiEventIdStartLimit + 7)
DEFINE_EVENT(evNPReleaseLoadAck,        kCuiEventIdStartLimit + 8)
DEFINE_EVENT(evNPNorShutdownAck,                kCuiEventIdStartLimit + 9)
DEFINE_EVENT(evNPRegisterPersistentFile,    kCuiEventIdStartLimit + 10)
DEFINE_EVENT(evNPReleasePersistentFile,     kCuiEventIdStartLimit + 11)
DEFINE_EVENT(evNPLoadPersistentFile,        kCuiEventIdStartLimit + 12)
DEFINE_EVENT(evNPSetFilePersistentType,     kCuiEventIdStartLimit + 13)
DEFINE_EVENT(evNPSetFolderPersistentType,     kCuiEventIdStartLimit + 14)
DEFINE_EVENT(evUserChange,            kCuiEventIdStartLimit + 15)
DEFINE_EVENT(evShutdownTimeout,           kCuiEventIdStartLimit + 16)

DEFINE_EVENT(evNPPublishNotification,         kCuiEventIdStartLimit + 17)
DEFINE_EVENT(evNPRegisterNotifications,         kCuiEventIdStartLimit + 18)
DEFINE_EVENT(evNPSubscribeToNotification,       kCuiEventIdStartLimit + 19)
DEFINE_EVENT(evNPSubscribeToNotifications,      kCuiEventIdStartLimit + 20)
DEFINE_EVENT(evNPUnRegisterNotifications,       kCuiEventIdStartLimit + 21)
DEFINE_EVENT(evNPUnSubscribeFromNotification,   kCuiEventIdStartLimit + 22)
DEFINE_EVENT(evNPRegisterPersistentFolder,    kCuiEventIdStartLimit + 23)
DEFINE_EVENT(evNPLoadPersistentFolder,      kCuiEventIdStartLimit + 24)
DEFINE_EVENT(evNPReleasePersistentFolder,     kCuiEventIdStartLimit + 25)
DEFINE_EVENT(evNPUnSubscribeFromNotifications,  kCuiEventIdStartLimit + 26)
DEFINE_EVENT(evCheckAllFilesPersisted,      kCuiEventIdStartLimit + 27)
DEFINE_EVENT(evNPGetReadyStatus,        kCuiEventIdStartLimit + 28)
DEFINE_EVENT(evNPRegisterImmediateNotifications, kCuiEventIdStartLimit + 29)
DEFINE_EVENT(evNPClearPersistedData,      kCuiEventIdStartLimit + 30)

DEFINE_EVENT(evNPShutdownDataSave,        kCuiEventIdStartLimit + 31)

#ifdef NPP_PROFILEINFO_ENABLE

DEFINE_EVENT(evNPProfileNotification,       kCuiEventIdStartLimit + 32)
DEFINE_EVENT(evNPProfilePersistence,        kCuiEventIdStartLimit + 33)

#endif

DEFINE_EVENT(evNPPublishImmediateNotification,  kCuiEventIdStartLimit + 34)
DEFINE_EVENT(evNPPersistentSync,                kCuiEventIdStartLimit + 35)

////////////////////////////////////////////////////////////////////////////////////////////////
/// SetPersonality
/// Set the personality in the framework on userchange.
///
/// \param [IN] f_happ
///      HANDLE - Application Handle
///
/// \param [IN] f_tMsg
///      NC_User - Structure containing user specific details
///
/// \return EFrameworkunifiedStatus
//      EFrameworkunifiedStatus - success or failure status
///
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus SetPersonality(HANDLE f_happ, NC_User &f_tMsg);  // NOLINT (runtime/references)


/*
 * CShutDownMsgData
 * This class is used to pass the data between states sShutdownIdle and sShutdownDataSave
 */
class CShutDownMsgData : public CEventData {
 public:
  // constructor
  CShutDownMsgData(UI_32 f_uieventid, EFrameworkunifiedShutdownType f_etype, UI_32 f_uidata): CEventData(f_uieventid),
    m_eShutdownType(f_etype), m_uiMsgData(f_uidata) {}  // LCOV_EXCL_BR_LINE 11: unexpected branch

  // destructor
  virtual ~CShutDownMsgData() {}

  EFrameworkunifiedShutdownType m_eShutdownType;

  UI_32 m_uiMsgData;
};

#endif  // NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_APP_STATES_H_
