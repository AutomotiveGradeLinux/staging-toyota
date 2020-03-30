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
/// \brief
///
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <native_service/frameworkunified_application.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/ns_np_service_protocol.h>
#include <native_service/ns_np_service_notification.h>

#include "app_states.h"
#include "ns_npp.h"
#include "ns_npp_notificationpersistentservicelog.h"
#include "ns_npp_persistence_manager.h"

using std::malloc;

#ifdef NPP_PROFILEINFO_ENABLE
#include "ns_npp_profiling_protocols_internal.h"
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Function : FrameworkunifiedOnInitialization
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOnInitialization(HANDLE happ) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (NULL != happ) {  // LCOV_EXCL_BR_LINE 4: happ can't be null
    FrameworkunifiedProtocolEvent ns_npp_protocol_handlers[] = {
      {NPS_REGISTER_EV_REQ,         EVENT(evNPRegisterNotifications)},
      {NPS_PUBLISH_EV_REQ,        EVENT(evNPPublishNotification)},
      {NPS_SUBSCRIBE_TO_EV_REQ,         EVENT(evNPSubscribeToNotification)},
      {NPS_BATCH_SUBSCRIBE_TO_EV_REQ,   EVENT(evNPSubscribeToNotifications)},
      {NPS_UNSUBSCRIBE_FROM_EV_REQ,     EVENT(evNPUnSubscribeFromNotification)},
      {NPS_UNREGISTER_EV_REQ,           EVENT(evNPUnRegisterNotifications)},
      {NPS_GET_PERS_DATA_REQ,           EVENT(evNPReadPersistedData)},
      {NPS_SET_PERSIST_FILE_PATH_REQ,   EVENT(evNPRegisterPersistentFile)},
      {NPS_RELEASE_PERS_FILE_REQ,       EVENT(evNPReleasePersistentFile)},
      {NPS_GET_PERS_FILE_REQ,           EVENT(evNPLoadPersistentFile)},
      {NPS_SET_PERSIST_FOLDER_PATH_REQ,   EVENT(evNPRegisterPersistentFolder)},
      {NPS_GET_PERS_FOLDER_REQ,         EVENT(evNPLoadPersistentFolder)},
      {NPS_RELEASE_PERS_FOLDER_REQ,     EVENT(evNPReleasePersistentFolder)},
      {NPS_CHANGE_PERSONALITY_REQ,      EVENT(evUserChange)},
      {NPS_NPP_STOP_REQ,            EVENT(evShutdown)},
      {NPS_BATCH_UNSUBSCRIBE_FROM_EV_REQ, EVENT(evNPUnSubscribeFromNotifications)},
      {NPS_GET_READYSTATUS_REQ,     EVENT(evNPGetReadyStatus)},
      {NPS_REGISTER_NOR_EV_REQ,       EVENT(evNPRegisterImmediateNotifications)},
      {NPS_DELETE_PERSISTED_DATA_REQ,   EVENT(evNPClearPersistedData)},
      {NPS_SET_DEFAULT_PERS_DATA,     EVENT(evNPSetDefaultPersistentData)},
      {NPS_SET_NOTFN_PERSISTENT_TYPE,   EVENT(evNPSetNotfnPersistentType)},
      {NPS_SET_FILE_PERSISTENT_TYPE,    EVENT(evNPSetFilePersistentType)},
      {NPS_SET_FOLDER_PERSISTENT_TYPE,  EVENT(evNPSetFolderPersistentType)},
      {NPS_SYNCHRONOUS_WRITE_NOTIFY_REQ,  EVENT(evNPPublishImmediateNotification)},
      {NPS_NPP_SYNC_REQ,                  EVENT(evNPPersistentSync)}
    };

#ifdef NPP_PROFILEINFO_ENABLE

    FrameworkunifiedProtocolEvent ns_npp_profile_protocol_handlers[] = {
      {NPS_PROFILE_NOTIFICATION_REQ, EVENT(evNPProfileNotification)},
      {NPS_PROFILE_PERSISTENCE_REQ,  EVENT(evNPProfilePersistence)},
    };

    if (eFrameworkunifiedStatusOK == FrameworkunifiedAttachHSMEventsToDispatcher(happ,
                                                       "NSProfileUtil",
                                                       ns_npp_profile_protocol_handlers,
                                                       _countof(ns_npp_profile_protocol_handlers))) {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Profile info messages are attached to events");
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to attach profile info messages to events");
    }

#endif

    if (eFrameworkunifiedStatusOK == (l_estatus = FrameworkunifiedAttachHSMEventsToDispatcher(happ, FRAMEWORKUNIFIED_ANY_SOURCE, ns_npp_protocol_handlers, static_cast<UI_32>(_countof(ns_npp_protocol_handlers))))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
      CNSNPP *l_pnsnpp =  new(std::nothrow) CNSNPP(happ);
      if (NULL != l_pnsnpp) {   // LCOV_EXCL_BR_LINE 5: new's error case.
        FRAMEWORKUNIFIEDLOG(ZONE_NPP_INFO, __FUNCTION__, "l_pnsnpp object created");

        if (eFrameworkunifiedStatusOK == (l_estatus = l_pnsnpp->Init(happ))) {  // LCOV_EXCL_BR_LINE 200: can't test
          FRAMEWORKUNIFIEDLOG(ZONE_NPP_INFO, __FUNCTION__, "l_pnsnpp object initialized");

          if (eFrameworkunifiedStatusOK == (l_estatus = l_pnsnpp->FrameworkunifiedCreate())) {  // LCOV_EXCL_BR_LINE 200: the result of l_pnsnpp->FrameworkunifiedCreate() is always eFrameworkunifiedStatusOK  // NOLINT[whitespace/line_length]
            FrameworkunifiedSetStateMachine(happ, l_pnsnpp);

            l_pnsnpp->FrameworkunifiedStart();
          } else {
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "NSNPP statemachine not created, status: 0x%x", l_estatus);  // LCOV_EXCL_LINE 200: the result of l_pnsnpp->FrameworkunifiedCreate() is always eFrameworkunifiedStatusOK  // NOLINT[whitespace/line_length]
          }
        } else {
          // LCOV_EXCL_START 200: can't test
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "l_pnsnpp object initialization error, status: %d", l_estatus);

          // de-initialize the partially initialize object
          l_pnsnpp->DeInit(happ);

          delete l_pnsnpp;
          l_pnsnpp = NULL;
          // LCOV_EXCL_STOP
        }
      } else {
        // LCOV_EXCL_START 5: malloc's error case.
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "l_pnsnpp object not created");
        l_estatus = eFrameworkunifiedStatusNullPointer;
        // LCOV_EXCL_STOP
      }
    } else {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedAttachHSMEventsToDispatcher Failed Status:0x%x ", l_estatus);  // LCOV_EXCL_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
    }
  } else {
    // LCOV_EXCL_START 4: happ can't be null
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_estatus = eFrameworkunifiedStatusInvldParam;
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Application handle happ is NULL");
    // LCOV_EXCL_STOP
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOnDestroy(HANDLE happ) {    // LCOV_EXCL_START 14: Resident process, not called by NSFW
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}
// LCOV_EXCL_STOP

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Function : FrameworkunifiedOnDebugDump
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOnDebugDump(HANDLE happ) {    // LCOV_EXCL_START 7: debug code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, "+", __FUNCTION__);
  /// << Dump internal tables!
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, "-", __FUNCTION__);
  return eFrameworkunifiedStatusOK;
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus FrameworkunifiedCreateStateMachine(HANDLE happ) {
  return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus FrameworkunifiedSSFrameworkInterface(HANDLE happ) {
  return eFrameworkunifiedStatusOK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Function : FrameworkunifiedOnStart
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOnStart(HANDLE happ) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, "+", __FUNCTION__);

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, "-", __FUNCTION__);
  return eFrameworkunifiedStatusOK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Function : FrameworkunifiedOnStop
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOnStop(HANDLE happ) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, "+", __FUNCTION__);

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, "-", __FUNCTION__);
  return eFrameworkunifiedStatusOK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Function : FrameworkunifiedOnPreStart
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOnPreStart(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, "+", __FUNCTION__);

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, "-", __FUNCTION__);
  return eFrameworkunifiedStatusOK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Function : FrameworkunifiedOnPreStop
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOnPreStop(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, "+", __FUNCTION__);

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, "-", __FUNCTION__);
  return eFrameworkunifiedStatusOK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Function : FrameworkunifiedOnBackgroundStart
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOnBackgroundStart(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, "+", __FUNCTION__);

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, "-", __FUNCTION__);
  return eFrameworkunifiedStatusOK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Function : FrameworkunifiedOnBackgroundStop
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOnBackgroundStop(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, "+", __FUNCTION__);

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, "-", __FUNCTION__);
  return eFrameworkunifiedStatusOK;
}

extern const FrameworkunifiedDefaultCallbackHandler kDefaultCbHandler = {  // NOLINT (readability/naming)
  FrameworkunifiedOnInitialization,
  FrameworkunifiedOnDestroy,
  FrameworkunifiedOnStart,
  FrameworkunifiedOnStop,
  FrameworkunifiedOnPreStart,
  FrameworkunifiedOnPreStop,
  FrameworkunifiedOnBackgroundStart,
  FrameworkunifiedOnBackgroundStop,
  FrameworkunifiedOnDebugDump,
  FrameworkunifiedCreateStateMachine,
  FrameworkunifiedSSFrameworkInterface
};  // LCOV_EXCL_BR_LINE 10: The final line
