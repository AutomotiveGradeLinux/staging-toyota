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
/// \brief    This file contains implementation of internal transitions of state machine
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <unistd.h>
#include <native_service/ns_np_service.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/ns_np_service_protocol.h>
#include <native_service/ns_np_service_notification.h>
#include <string>
#include <vector>
#include "app_states.h"
#include "ns_npp.h"
#include "ns_npp_notificationpersistentservicelog.h"
#include "ns_npp_notification_manager.h"
#include "ns_npp_persistence_manager.h"
#include "ns_npp_persistent_data.h"
#include "ns_npp_personalization_manager.h"


#ifdef NPP_PROFILEINFO_ENABLE
#include <ns_npp_profiling_protocols_internal.h>
#endif

#ifdef AGL_STUB
#include <other_service/strlcpy.h>
#endif

static size_t NppUIToA(unsigned int value, char *buf) {
  static const char c[] = "0123456789";
  char b[12];
  char *p = b + sizeof(b);
  int i = 0;

  *--p = '\0';
  do {
    *--p = c[value % 10];
    value /= 10;
    ++i;
  } while (value);
  strcpy(buf, p); // NOLINT (runtime/printf)

  return i;
}

static size_t NppStrlcpy(char *dst, const char *src, size_t siz) {
  size_t ret = strlen(src);

  if (siz) {  // LCOV_EXCL_BR_LINE 6: siz can't be 0
    size_t len = (ret >= siz) ? siz - 1 : ret;
    memcpy(dst, src, len);
    dst[len] = '\0';
  }
  return ret;
}

#define NPP_SET_FIXSTR(buf, str) \
  do { \
    strcpy(buf, str); /* NOLINT (runtime/printf) */ \
    buf += sizeof(str) - 1; \
  } while (0)

#define NPP_SET_VARSTR(buf, str, siz) \
    buf += NppStrlcpy(buf, str, siz)

extern CHAR g_csendreadyackto[];

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceOnNPRegisterNotifications
/// The state machine executes this transition when event for registering notification is triggered.
////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_INTERNALTRANSITION(NotificationpersistentserviceOnNPRegisterNotifications) {  // NOLINT (readability/naming)
  try {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");
    EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

    CHKNULL(f_pSourceState);
    NC_register_multiple_notif_msg *l_pMsg = NULL;

    HANDLE l_pHApp = f_pSourceState->FrameworkunifiedGetAppHandle();
    CHKNULL(l_pHApp);

    CFrameworkunifiedHSM *l_pNotificationpersistentserviceHSM = FrameworkunifiedGetStateMachine(l_pHApp);
    CNSNPP *l_pnsnpp = static_cast<CNSNPP *>(l_pNotificationpersistentserviceHSM);

    // get app name of message source
    PCSTR l_cMsgSource = FrameworkunifiedGetMsgSrc(l_pHApp);
    CHKNULL(l_cMsgSource);

    // get instance of notification manager
    CNotificationManager *l_pNotificationManager = l_pnsnpp->m_pNotificationManager;

    if (NULL != l_pNotificationManager) {  // LCOV_EXCL_BR_LINE 6: l_pNotificationManager can't be NULL
      UI_32 l_uiMsgLength = FrameworkunifiedGetMsgLength(l_pHApp);

      if (l_uiMsgLength <= 0) {  // LCOV_EXCL_BR_LINE 6: l_uiMsgLength must be greater than 0
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Can't register notifications. Invalid message length received.");  // LCOV_EXCL_LINE 6: l_uiMsgLength must be greater than 0  // NOLINT[whitespace/line_length]
      } else {
        std::vector<CHAR> l_vData = std::vector<CHAR>(l_uiMsgLength);

        // get the received data
        if (eFrameworkunifiedStatusOK == (l_estatus = FrameworkunifiedGetMsgDataOfSize(
                                           l_pHApp, &l_vData[0], static_cast<UI_32>(l_vData.size()), eSMRRelease))) {
          l_pMsg = reinterpret_cast<NC_register_multiple_notif_msg *>(&l_vData[0]);

          if (NULL != l_pMsg) {  // LCOV_EXCL_BR_LINE 6: l_pMsg can't be NULL
            NC_register_notif_msg *l_pEventInfo = NULL;

            // register all the notifications
            for (UI_32 l_uiCount = 0;
                 l_uiCount < l_pMsg->numNotifications;
                 ++l_uiCount) {
              l_pEventInfo = &l_pMsg->notifierList[l_uiCount];

              if (NULL != l_pEventInfo) {  // LCOV_EXCL_BR_LINE 6: l_pEventInfo can't be NULL
                FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "Register Notfn request, src=%s, name=%s, len=%d, type=%d",
                       l_cMsgSource, l_pEventInfo->notificationName, l_pEventInfo->maxLength, l_pEventInfo->persType);

                // register the notification
                if (eFrameworkunifiedStatusOK != l_pNotificationManager->NotificationpersistentserviceServiceOnRegisterEvents(l_cMsgSource,
                                                                                       l_pEventInfo->notificationName,
                                                                                       l_pEventInfo->maxLength,
                                                                                       l_pEventInfo->persType)) {
                  FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                         "Error in NotificationpersistentserviceServiceOnRegisterEvents :: %s", l_pEventInfo->notificationName);
                }
              } else {
                // LCOV_EXCL_START 6: l_pEventInfo can't be NULL
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                       "Can't register notification from source %s. l_pEventInfo is NULL", l_cMsgSource);
                // LCOV_EXCL_STOP
              }
            }
          } else {
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Invalid src mq");  // LCOV_EXCL_LINE 6: l_pMsg can't be NULL
          }
        } else {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                 "Can't register notifications from source %s, Unable to get message data. Error Status: 0x%x",
                 l_cMsgSource, l_estatus);
        }
      }
    } else {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "l_pNotificationManager is NULL");  // LCOV_EXCL_LINE 6: l_pNotificationManager can't be NULL  // NOLINT[whitespace/line_length]
    }
  } catch (std::exception &e) {  // LCOV_EXCL_START 5: exception error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
    return NULL;
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return f_pSourceState;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceOnNPSubscribeToNotification
/// The state machine executes this transition when event for subscribing single notification is
/// triggered.
////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_INTERNALTRANSITION(NotificationpersistentserviceOnNPSubscribeToNotification) {
  try {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");
    EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

    CHKNULL(f_pSourceState);
    NC_subscribe_msg l_objSubscribeMsg;

    HANDLE l_pHApp = f_pSourceState->FrameworkunifiedGetAppHandle();
    CHKNULL(l_pHApp);

    CFrameworkunifiedHSM *l_pNotificationpersistentserviceHSM = FrameworkunifiedGetStateMachine(l_pHApp);
    CNSNPP *l_pnsnpp = static_cast<CNSNPP *>(l_pNotificationpersistentserviceHSM);

    PCSTR l_cMsgSource = FrameworkunifiedGetMsgSrc(l_pHApp);
    CHKNULL(l_cMsgSource);

    // get instance of notification manager
    CNotificationManager *l_pNotificationManager = l_pnsnpp->m_pNotificationManager;

    if (NULL != l_pNotificationManager) {  // LCOV_EXCL_BR_LINE 6: l_pNotificationManager can't be NULL
      if (FrameworkunifiedGetMsgLength(l_pHApp) == sizeof(NC_subscribe_msg)) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
        // get the data received
        if (eFrameworkunifiedStatusOK == (l_estatus = FrameworkunifiedGetMsgDataOfSize(l_pHApp, static_cast<PVOID>(&l_objSubscribeMsg), sizeof(NC_subscribe_msg), eSMRRelease))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
#ifdef NPP_PERFORMANCE_ANALYZE_ENABLE
          char buf[128];
          char *p = buf;
          NPP_SET_FIXSTR(p, "Subscribing Notfn request, src=");
          NPP_SET_VARSTR(p, l_cMsgSource, sizeof(buf) - (p - buf));
          NPP_SET_FIXSTR(p, ", name=");
          strcpy(p, l_objSubscribeMsg.notificationName); // NOLINT (runtime/printf)
          FRAMEWORKUNIFIEDLOG(ZONE_PRD_INFO2, __FUNCTION__, buf);
        /* FRAMEWORKUNIFIEDLOG(ZONE_PRD_INFO2, __FUNCTION__,
         * "Subscribing Notfn request, src=%s, name=%s",
         *  l_cMsgSource, l_objSubscribeMsg.notificationName);*/
#endif  // ifdef NPP_PERFORMANCE_ANALYZE_ENABLE

          // subscribe for notification
          if (eFrameworkunifiedStatusOK != l_pNotificationManager->NotificationpersistentserviceServiceOnSubscribeToEvent(
                      l_cMsgSource,
                      l_objSubscribeMsg.notificationName)) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                   "Error in NotificationpersistentserviceServiceOnSubscribeToEvent :: %s", l_objSubscribeMsg.notificationName);
          }
        } else {
          // LCOV_EXCL_START 4: NSFW error case
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                 "Can't subscribe to notification from source %s. Unable to get msg data, status: 0x%x",
                 l_cMsgSource, l_estatus);
           // LCOV_EXCL_STOP
        }
      } else {
        // LCOV_EXCL_START 4: NSFW error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Can't subscribe to notification from source %s. Invalid message length received.",
               l_cMsgSource);
        // LCOV_EXCL_STOP
      }
    } else {
      // LCOV_EXCL_START 6: l_pNotificationManager can't be NULL
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Can't subscribe to notification from source %s. l_pNotificationManager is NULL",
             l_cMsgSource);
      // LCOV_EXCL_STOP
    }
  } catch (std::exception &e) {  // LCOV_EXCL_START 5: exception error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
    return NULL;
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return f_pSourceState;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceOnNPSubscribeToNotifications
/// The state machine executes this transition when event for subscribing multiple notification is
/// triggered.
////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_INTERNALTRANSITION(NotificationpersistentserviceOnNPSubscribeToNotifications) {
  try {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");
    EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

    CHKNULL(f_pSourceState);
    NC_subscribe_multiple_notif_msg *l_pMsg = NULL;

    HANDLE l_pHApp = f_pSourceState->FrameworkunifiedGetAppHandle();
    CHKNULL(l_pHApp);

    CFrameworkunifiedHSM *l_pNotificationpersistentserviceHSM = FrameworkunifiedGetStateMachine(l_pHApp);
    CNSNPP *l_pnsnpp = static_cast<CNSNPP *>(l_pNotificationpersistentserviceHSM);

    PCSTR l_cMsgSource = FrameworkunifiedGetMsgSrc(l_pHApp);
    CHKNULL(l_cMsgSource);

    // get instance of notification manager
    CNotificationManager *l_pNotificationManager = l_pnsnpp->m_pNotificationManager;

    if (NULL != l_pNotificationManager) {  // LCOV_EXCL_BR_LINE 6: l_pNotificationManager can't be NULL
      UI_32 l_uiMsgLength = FrameworkunifiedGetMsgLength(l_pHApp);

      if (l_uiMsgLength <= 0) {  // LCOV_EXCL_BR_LINE 6: l_uiMsgLength must be greater than 0
        // LCOV_EXCL_START 6: l_uiMsgLength must be greater than 0
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Invalid msg len");
        // LCOV_EXCL_STOP
      } else {
        std::vector<CHAR> l_vData = std::vector<CHAR>(l_uiMsgLength);

        // get the data received
        if (eFrameworkunifiedStatusOK == (l_estatus = FrameworkunifiedGetMsgDataOfSize(l_pHApp, &l_vData[0], static_cast<UI_32>(l_vData.size()), eSMRRelease))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
          l_pMsg = reinterpret_cast<NC_subscribe_multiple_notif_msg *>(&l_vData[0]);

          if (NULL != l_pMsg) {  // LCOV_EXCL_BR_LINE 6: l_pMsg can't be NULL
            NC_subscribe_msg *l_pEventInfo = NULL;

            // subscribe to multiple notifications
            for (UI_32 l_uiCount = 0;
                 l_uiCount < l_pMsg->numNotifications;
                 ++l_uiCount) {
              l_pEventInfo = &l_pMsg->notificationList[l_uiCount];

              if (NULL != l_pEventInfo) {  // LCOV_EXCL_BR_LINE 6: l_pEventInfo can't be NULL
#ifdef NPP_PERFORMANCE_ANALYZE_ENABLE
                char buf[128];
                char *p = buf;
                NPP_SET_FIXSTR(p, "Subscribe Notfn request, src=");
                NPP_SET_VARSTR(p, l_cMsgSource, sizeof(buf) - (p - buf));
                NPP_SET_FIXSTR(p, ", name=");
                strcpy(p, l_pEventInfo->notificationName); // NOLINT (runtime/printf)
                FRAMEWORKUNIFIEDLOG(ZONE_PRD_INFO2, __FUNCTION__, buf);
                /* FRAMEWORKUNIFIEDLOG(ZONE_PRD_INFO2, __FUNCTION__,
                        "Subscribe Notfn request, src=%s, name=%s",
                        l_cMsgSource,
                        l_pEventInfo->notificationName);*/
#endif  // ifdef NPP_PERFORMANCE_ANALYZE_ENABLE

                // subscribe to notification
                if (eFrameworkunifiedStatusOK != l_pNotificationManager->NotificationpersistentserviceServiceOnSubscribeToEvent(
                            l_cMsgSource,
                            l_pEventInfo->notificationName)) {
                  FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                         "Error in NotificationpersistentserviceServiceOnSubscribeToEvent :: %s", l_pEventInfo->notificationName);
                }
              } else {
                // LCOV_EXCL_START 6: l_pEventInfo can't be NULL
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                      "Unable to subscribe to notifications from source %s, l_pEventInfo is NULL", l_cMsgSource);
                // LCOV_EXCL_STOP
              }
            }
          } else {
            // LCOV_EXCL_START 6: l_pMsg can't be NULL
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                   "Unable to subscribe to notifications from source %s, Invalid src mq", l_cMsgSource);
            // LCOV_EXCL_STOP
          }

          l_vData.clear();
        } else {
          // LCOV_EXCL_START 4: NSFW error case
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                 "Unable to subscribe to notifications from source %s, error retrieving message data, status: 0x%x",
                 l_cMsgSource,
                 l_estatus);
          // LCOV_EXCL_STOP
        }
      }
    } else {
      // LCOV_EXCL_START 6: l_pNotificationManager can't be NULL
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Unable to subscribe to notifications from source %s, l_pNotificationManager is NULL",
             l_cMsgSource);
      // LCOV_EXCL_STOP
    }
  } catch (std::exception &e) {  // LCOV_EXCL_START 5: exception error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
    return NULL;
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return f_pSourceState;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceOnNPPublishNotification
/// The state machine executes this transition when event for publishing notification is triggered.
////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_INTERNALTRANSITION(NotificationpersistentserviceOnNPPublishNotification) {
  try {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");
    EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

    CHKNULL(f_pSourceState);

    CHAR l_cData[MAX_SYS_INFO_SIZE];

    HANDLE l_pHApp = f_pSourceState->FrameworkunifiedGetAppHandle();
    CHKNULL(l_pHApp);

    CFrameworkunifiedHSM *l_pNotificationpersistentserviceHSM = FrameworkunifiedGetStateMachine(l_pHApp);
    CNSNPP *l_pnsnpp = static_cast<CNSNPP *>(l_pNotificationpersistentserviceHSM);

    // get the app name of message source
    PCSTR l_cMsgSource = FrameworkunifiedGetMsgSrc(l_pHApp);
    CHKNULL(l_cMsgSource);

    // get instance of notification manager
    CNotificationManager *l_pNotificationManager = l_pnsnpp->m_pNotificationManager;

    if (NULL != l_pNotificationManager) {  // LCOV_EXCL_BR_LINE 6: l_pNotificationManager can't be NULL
      // retrieve notification name
      FrameworkunifiedGetSystemInfo(l_pHApp, l_cData);

      UI_32 l_uiMsgLength = FrameworkunifiedGetMsgLength(l_pHApp);

      char buf[144];
      char *p = buf;
      NPP_SET_FIXSTR(p, "Publish Notfn request, src=");
      NPP_SET_VARSTR(p, l_cMsgSource, sizeof(buf) - (p - buf));
      NPP_SET_FIXSTR(p, ", name=");
      NPP_SET_VARSTR(p, l_cData, sizeof(buf) - (p - buf));
      NPP_SET_FIXSTR(p, ", len=");
      NppUIToA(l_uiMsgLength, p);
      FRAMEWORKUNIFIEDLOG(ZONE_PRD_INFO2, __FUNCTION__, buf);
      /* FRAMEWORKUNIFIEDLOG(ZONE_PRD_INFO2, __FUNCTION__,
                "Publish Notfn request, src=%s, name=%s, len=%d",
                l_cMsgSource, l_cData, l_uiMsgLength);*/

      if (0 != l_uiMsgLength) {
        std::vector<CHAR> pMsgData = std::vector<CHAR>(l_uiMsgLength);

        // get the received data
        if (eFrameworkunifiedStatusOK == (l_estatus = FrameworkunifiedGetMsgDataOfSize(l_pHApp, &pMsgData[0], static_cast<UI_32>(pMsgData.size()), eSMRRelease))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
          // publish the notification
          (VOID)l_pNotificationManager->NotificationpersistentserviceServiceOnPublishEvent(l_cMsgSource, l_cData,
                                                                 (PVOID)&pMsgData[0], l_uiMsgLength);
        } else {
          // LCOV_EXCL_START 4: NSFW error case
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                 "Can't publish notification from source %s, Unable to get messsage data, Error Status: 0x%x",
                 l_cMsgSource, l_estatus);
          // LCOV_EXCL_STOP 4: NSFW error case
        }
      } else {
        // publish the notification
        (VOID)l_pNotificationManager->NotificationpersistentserviceServiceOnPublishEvent(l_cMsgSource, l_cData, NULL, l_uiMsgLength);
      }
    } else {
      // LCOV_EXCL_START 6: l_pNotificationManager can't be NULL
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Can't publish notification from source %s. l_pNotificationManager is NULL",
             l_cMsgSource);
      // LCOV_EXCL_STOP
    }
  } catch (std::exception &e) {  // LCOV_EXCL_START 5: exception error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
    return NULL;
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return f_pSourceState;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceOnNPUnSubscribeFromNotification
/// The state machine executes this transition when event for unsubscribing notification is triggered.
////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_INTERNALTRANSITION(NotificationpersistentserviceOnNPUnSubscribeFromNotification) {
  try {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");
    EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

    CHKNULL(f_pSourceState);
    NC_unsubscribe_frm_notif_msg unsubscribeMsg;

    HANDLE l_pHApp = f_pSourceState->FrameworkunifiedGetAppHandle();
    CHKNULL(l_pHApp);

    CFrameworkunifiedHSM *l_pNotificationpersistentserviceHSM = FrameworkunifiedGetStateMachine(l_pHApp);
    CNSNPP *l_pnsnpp = static_cast<CNSNPP *>(l_pNotificationpersistentserviceHSM);

    // get the msg source name
    PCSTR l_cMsgSource = FrameworkunifiedGetMsgSrc(l_pHApp);
    CHKNULL(l_cMsgSource);

    // get instance of notification manager
    CNotificationManager *l_pNotificationManager = l_pnsnpp->m_pNotificationManager;

    if (NULL != l_pNotificationManager) {  // LCOV_EXCL_BR_LINE 200: l_pNotificationManager can't be NULL
      if (FrameworkunifiedGetMsgLength(l_pHApp) == sizeof(NC_unsubscribe_frm_notif_msg)) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
        // get the received data
        if (eFrameworkunifiedStatusOK == (l_estatus = FrameworkunifiedGetMsgDataOfSize(l_pHApp, static_cast<PVOID>(&unsubscribeMsg), sizeof(NC_unsubscribe_frm_notif_msg), eSMRRelease))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
          FRAMEWORKUNIFIEDLOG(ZONE_NPP_INFO, __FUNCTION__, "Source: %s is unsubscribing from notification: %s", l_cMsgSource,
                 unsubscribeMsg.notificationName);

          // unsubscribe from notification
          if (eFrameworkunifiedStatusOK != l_pNotificationManager->NotificationpersistentserviceServiceOnUnSubscribeFromEvent(
                      l_cMsgSource,
                      unsubscribeMsg.notificationName)) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                   "Error in NotificationpersistentserviceServiceOnUnSubscribeFromEvent :: %s",
                   unsubscribeMsg.notificationName);
          }
        } else {
          // LCOV_EXCL_START 4: NSFW error case
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                 "%s is unable to subscribe from notification, error getting message data, status: 0x%x",
                 l_cMsgSource, l_estatus);
          // LCOV_EXCL_STOP
        }
      } else {
        // LCOV_EXCL_START 4: NSFW error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Can't unsubscribe to notification from source %s, Invalid Message size received.",
               l_cMsgSource);
        // LCOV_EXCL_STOP
      }
    } else {
      // LCOV_EXCL_START 6: l_pNotificationManager can't be NULL
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Can't unsubscribe to notification from source %s, l_pNotificationManager is NULL",
             l_cMsgSource);
      // LCOV_EXCL_STOP
    }
  } catch (std::exception &e) {  // LCOV_EXCL_START 5: exception error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
    return NULL;
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return f_pSourceState;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceOnNPUnSubscribeFromNotification
/// The state machine executes this transition when event for unsubscribing notification is triggered.
////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_INTERNALTRANSITION(NotificationpersistentserviceOnNPUnSubscribeFromNotifications) {
  try {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");
    EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

    CHKNULL(f_pSourceState);
    NC_unsubscribe_multiple_notif_msg *l_pMsg = NULL;

    HANDLE l_pHApp = f_pSourceState->FrameworkunifiedGetAppHandle();
    CHKNULL(l_pHApp);

    CFrameworkunifiedHSM *l_pNotificationpersistentserviceHSM = FrameworkunifiedGetStateMachine(l_pHApp);
    CNSNPP *l_pnsnpp = static_cast<CNSNPP *>(l_pNotificationpersistentserviceHSM);

    PCSTR l_cMsgSource = FrameworkunifiedGetMsgSrc(l_pHApp);
    CHKNULL(l_cMsgSource);

    // get instance of notification manager
    CNotificationManager *l_pNotificationManager = l_pnsnpp->m_pNotificationManager;

    if (NULL != l_pNotificationManager) {  // LCOV_EXCL_BR_LINE 6: l_pNotificationManager can't be NULL
      UI_32 l_uiMsgLength = FrameworkunifiedGetMsgLength(l_pHApp);

      if (l_uiMsgLength <= 0) {  // LCOV_EXCL_BR_LINE 6: l_uiMsgLength must be greater than 0
        // LCOV_EXCL_START 6: l_uiMsgLength must be greater than 0
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Can't unsubscribe from notifications from source %s. Invalid message length received",
               l_cMsgSource);
        // LCOV_EXCL_STOP
      } else {
        std::vector<CHAR> l_vData = std::vector<CHAR>(l_uiMsgLength);

        // get the data received
        if (eFrameworkunifiedStatusOK == (l_estatus = FrameworkunifiedGetMsgDataOfSize(l_pHApp, &l_vData[0], static_cast<UI_32>(l_vData.size()), eSMRRelease))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
          l_pMsg = reinterpret_cast<NC_unsubscribe_multiple_notif_msg *>(&l_vData[0]);

          if (NULL != l_pMsg) {  // LCOV_EXCL_BR_LINE 6: l_pMsg can't be NULL
            NC_unsubscribe_frm_notif_msg *l_pEventInfo = NULL;

            // subscribe to multiple notifications
            for (UI_32 l_uiCount = 0;
                 l_uiCount < l_pMsg->numNotifications;
                 ++l_uiCount) {
              l_pEventInfo = &l_pMsg->notificationList[l_uiCount];

              if (NULL != l_pEventInfo) {  // LCOV_EXCL_BR_LINE 6: l_pEventInfo can't be NULL
                FRAMEWORKUNIFIEDLOG(ZONE_NPP_INFO, __FUNCTION__, "Source: %s is unsubscribing from notification: %s", l_cMsgSource,
                       l_pEventInfo->notificationName);

                // unsubscribe from notification
                if (eFrameworkunifiedStatusOK != l_pNotificationManager->NotificationpersistentserviceServiceOnUnSubscribeFromEvent(
                            l_cMsgSource,
                            l_pEventInfo->notificationName)) {
                  FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                         "Error in NotificationpersistentserviceServiceOnUnSubscribeFromEvent :: %s", l_pEventInfo->notificationName);
                }

              } else {
                // LCOV_EXCL_START 6: l_pEventInfo can't be NULL
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                       "Can't unsubscribe from notifications from source %s. l_pEventInfo is NULL",
                       l_cMsgSource);
                // LCOV_EXCL_STOP
              }
            }
          } else {
            // LCOV_EXCL_START 6: l_pMsg can't be NULL
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                   "Can't unsubscribe from notifications from source %s. Invalid src mq", l_cMsgSource);
            // LCOV_EXCL_STOP
          }

          l_vData.clear();
        } else {
          // LCOV_EXCL_START 4: NSFW error case
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                 "Can't unsubscribe from notifications from source %s, error getting message data, status: 0x%x",
                 l_cMsgSource,
                 l_estatus);
          // LCOV_EXCL_STOP
        }
      }
    } else {
      // LCOV_EXCL_START 6: l_pNotificationManager can't be NULL
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Can't unsubscribe from notifications from source %s. l_pNotificationManager is NULL",
             l_cMsgSource);
      // LCOV_EXCL_STOP
    }
  } catch (std::exception &e) {  // LCOV_EXCL_START 5: exception error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
    return NULL;
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return f_pSourceState;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceOnNPUnRegisterNotifications
/// The state machine executes this transition when event for unregistering notifications is triggered.
////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_INTERNALTRANSITION(NotificationpersistentserviceOnNPUnRegisterNotifications) {
  try {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");
    EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

    CHKNULL(f_pSourceState);
    NC_unregister_multiple_notif_msg *l_pMsg = NULL;

    HANDLE l_pHApp = f_pSourceState->FrameworkunifiedGetAppHandle();
    CHKNULL(l_pHApp);

    CFrameworkunifiedHSM *l_pNotificationpersistentserviceHSM = FrameworkunifiedGetStateMachine(l_pHApp);
    CNSNPP *l_pnsnpp = static_cast<CNSNPP *>(l_pNotificationpersistentserviceHSM);

    // get the source name
    PCSTR l_cMsgSource = FrameworkunifiedGetMsgSrc(l_pHApp);
    CHKNULL(l_cMsgSource);

    // get instance of notification manager
    CNotificationManager *l_pNotificationManager = l_pnsnpp->m_pNotificationManager;

    if (NULL != l_pNotificationManager) {  // LCOV_EXCL_BR_LINE 6: l_pNotificationManager can't be NULL
      UI_32 l_uiMsgLength = FrameworkunifiedGetMsgLength(l_pHApp);

      if (l_uiMsgLength <= 0) {  // LCOV_EXCL_BR_LINE 6: l_uiMsgLength must be greater than 0
        // LCOV_EXCL_START 6: l_uiMsgLength must be greater than 0
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Can't unregister to notifications from source %s. Invalid message length received.",
               l_cMsgSource);
        // LCOV_EXCL_STOP
      } else {
        std::vector<CHAR>l_vData = std::vector<CHAR>(l_uiMsgLength);

        // get the received data
        if (eFrameworkunifiedStatusOK == (l_estatus = FrameworkunifiedGetMsgDataOfSize(l_pHApp, &l_vData[0], static_cast<UI_32>(l_vData.size()), eSMRRelease))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
          l_pMsg = reinterpret_cast<NC_unregister_multiple_notif_msg *>(&l_vData[0]);

          // unregister multiple notifications
          if (NULL != l_cMsgSource && NULL != l_pMsg) {  // LCOV_EXCL_BR_LINE 6: l_cMsgSource and l_pMsg can't be NULL
            NC_unregister_notif_msg *l_pEventInfo = NULL;

            for (UI_32 l_uiCount = 0;
                 l_uiCount < l_pMsg->numNotifications;
                 ++l_uiCount) {
              l_pEventInfo = &l_pMsg->notificationList[l_uiCount];

              if (NULL != l_pEventInfo) {  // LCOV_EXCL_BR_LINE 6: l_pEventInfo can't be NULL
                FRAMEWORKUNIFIEDLOG(ZONE_NPP_INFO, __FUNCTION__, "Source %s is unregistering to notification %s", l_cMsgSource,
                       l_pEventInfo->notificationName);

                // unregister notification
                if (eFrameworkunifiedStatusOK != l_pNotificationManager->NotificationpersistentserviceServiceOnUnRegisterEvents(
                            l_cMsgSource,
                            l_pEventInfo->notificationName)) {
                  FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                         "Error in NotificationpersistentserviceServiceOnUnRegisterEvents :: %s", l_pEventInfo->notificationName);
                }
              } else {
                // LCOV_EXCL_START 6: l_pEventInfo can't be NULL
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                       "Can't unregister to notifications from source %s, l_pEventInfo is NULL", l_cMsgSource);
                // LCOV_EXCL_STOP
              }
            }
          } else {
            // LCOV_EXCL_START 6: l_cMsgSource and l_pMsg can't be NULL
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                   "Can't unregister to notificationsfrom source %s, Invalid src mq ", l_cMsgSource);
            // LCOV_EXCL_STOP
          }

          l_vData.clear();
        } else {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                 "Can't unregister to notifications from source %s, Unable to get message data, status: 0x%x",
                 l_cMsgSource, l_estatus);
        }
      }
    } else {
      // LCOV_EXCL_START 6: l_pNotificationManager can't be NULL
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Can't unregister to notifications from source %s, l_pNotificationManager is NULL",
             l_cMsgSource);
      // LCOV_EXCL_STOP
    }
  } catch (std::exception &e) {  // LCOV_EXCL_START 5: exception error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
    return NULL;
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return f_pSourceState;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceOnNPReadPersistedData
/// The state machine executes this transition when event for getting notification's persistent data
/// is triggered.
////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_INTERNALTRANSITION(NotificationpersistentserviceOnNPReadPersistedData) {
  try {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");
    EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

    CHKNULL(f_pSourceState);
    NC_get_pers_data_msg l_tMsg;

    l_tMsg.notificationName[0] = '\0';

    HANDLE l_pHApp = f_pSourceState->FrameworkunifiedGetAppHandle();
    CHKNULL(l_pHApp);

    PCSTR l_cMsgSource = FrameworkunifiedGetMsgSrc(l_pHApp);
    CHKNULL(l_cMsgSource);

    CFrameworkunifiedHSM *l_pNotificationpersistentserviceHSM = FrameworkunifiedGetStateMachine(l_pHApp);
    CNSNPP *l_pnsnpp = static_cast<CNSNPP *>(l_pNotificationpersistentserviceHSM);

    // get instance of notification manager
    CNotificationManager *l_pNotificationManager = l_pnsnpp->m_pNotificationManager;

    if (NULL != l_pNotificationManager) {  // LCOV_EXCL_BR_LINE 6: l_pNotificationManager can't be NULL
      if (FrameworkunifiedGetMsgLength(l_pHApp) == sizeof(NC_get_pers_data_msg)) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
        // get the message data received
        if (eFrameworkunifiedStatusOK == (l_estatus = FrameworkunifiedGetMsgDataOfSize(l_pHApp, static_cast<PVOID>(&l_tMsg), sizeof(NC_get_pers_data_msg), eSMRRelease))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
          FRAMEWORKUNIFIEDLOG(ZONE_PRD_INFO2, __FUNCTION__, "Source %s is requesting to read data for persistent notification %s",
                 l_cMsgSource, l_tMsg.notificationName);
          // get persistent data related to notification
          if (eFrameworkunifiedStatusOK != (l_estatus = l_pNotificationManager->NotificationpersistentserviceServiceOnGetPersistentData(
                                   l_tMsg.notificationName,
                                   l_cMsgSource))) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error in NotificationpersistentserviceServiceOnGetPersistentData :: %s", l_tMsg.notificationName);
          }
        } else {
          // LCOV_EXCL_START 4: NSFW error case
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                 "Can't read persistent data for source %s, Error getting message data, status: 0x%x",
                 l_cMsgSource, l_estatus);
          // LCOV_EXCL_STOP
        }
      } else {
        // LCOV_EXCL_START 4: NSFW error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Can't read persistent data for source %s. Invalid Message size received.",
               l_cMsgSource);
        l_estatus = eFrameworkunifiedStatusFail;
        // LCOV_EXCL_STOP
      }
    } else {
      // LCOV_EXCL_START 6: l_pNotificationManager can't be NULL
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Can't read persistent data for source %s. l_pNotificationManager is NULL",
             l_cMsgSource);
      l_estatus = eFrameworkunifiedStatusFail;
      // LCOV_EXCL_STOP
    }

    // if any error occurs while reading persistent data, send failed ack to the requestor
    if (eFrameworkunifiedStatusOK != l_estatus) {
      HANDLE l_hReceiverMq = McOpenSender(l_cMsgSource);
      if (NULL != l_hReceiverMq) {   // LCOV_EXCL_BR_LINE 4: NSFW error case.
        NC_get_persdata_failed_ack gpdFailed = {};
        std::strncpy(&gpdFailed.notificationName[0], l_tMsg.notificationName, MAX_STRING_SIZE_NOTIFICATION);

        // send the failure ack to requester
        l_estatus = McSend(l_hReceiverMq, AppName, NPS_GET_PERS_DATA_FAILED_ACK, sizeof(gpdFailed), &gpdFailed);  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
        FRAMEWORKUNIFIEDLOG(ZONE_PRD_INFO2, __FUNCTION__,
               "Sent NPS_GET_PERS_DATA_FAILED_ACK to %s. Status: %d.",
               l_cMsgSource, l_estatus);

        McClose(l_hReceiverMq);
      } else {
        // LCOV_EXCL_START 4: NSFW error case.
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error in opening sender queue of %s. Can't send NPS_GET_PERS_DATA_FAILED_ACK",
               l_cMsgSource);
        // LCOV_EXCL_STOP
      }
    }
  } catch (std::exception &e) {  // LCOV_EXCL_START 5: exception error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
    return NULL;
  // LCOV_EXCL_STOP
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return f_pSourceState;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceOnNPPublishImmediateNotification
/// This transition is executed when service updates the immediate notification data
/// using synchronous API
////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_INTERNALTRANSITION(NotificationpersistentserviceOnNPPublishImmediateNotification) {
  try {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");
    EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

    CHKNULL(f_pSourceState);

    HANDLE l_pHApp = f_pSourceState->FrameworkunifiedGetAppHandle();
    CHKNULL(l_pHApp);

    // get the app name of message source
    PCSTR l_cMsgSource = FrameworkunifiedGetMsgSrc(l_pHApp);
    CHKNULL(l_cMsgSource);

    CFrameworkunifiedHSM *l_pNotificationpersistentserviceHSM = FrameworkunifiedGetStateMachine(l_pHApp);
    CHKNULL(l_pNotificationpersistentserviceHSM);
    CNSNPP *l_pnsnpp = static_cast<CNSNPP *>(l_pNotificationpersistentserviceHSM);

    // get instance of notification manager
    CNotificationManager *l_pNotificationManager = l_pnsnpp->m_pNotificationManager;
    CHKNULL(l_pNotificationManager);

    CHAR l_cNotfName[MAX_SYS_INFO_SIZE] = {};

    // retrieve notification name
    (VOID)FrameworkunifiedGetSystemInfo(l_pHApp, l_cNotfName);

    UI_32 l_uiMsgLength = FrameworkunifiedGetMsgLength(l_pHApp);

    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__,
           "Warning::Notification %s published using sync API by %s, Data length: %d",
           l_cNotfName,
           l_cMsgSource, l_uiMsgLength);

    if (0 != l_uiMsgLength) {
      std::vector<CHAR> pMsgData = std::vector<CHAR>(l_uiMsgLength);

      // get the received data
      if (eFrameworkunifiedStatusOK == (l_estatus = FrameworkunifiedGetMsgDataOfSize(l_pHApp, &pMsgData[0], l_uiMsgLength, eSMRRelease))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
        // publish the notification
        (VOID)l_pNotificationManager->NotificationpersistentservicePublishImmediateNotification(l_cMsgSource, l_cNotfName, (PVOID)&pMsgData[0],
                                                                      l_uiMsgLength);
      } else {
        // LCOV_EXCL_START 4: NSFW error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Can't publish notification %s from source %s, Unable to get messsage data, Error Status: 0x%x",
               l_cNotfName,
               l_cMsgSource, l_estatus);
        // LCOV_EXCL_STOP
      }
    } else {
      // publish the notification
      (VOID)l_pNotificationManager->NotificationpersistentservicePublishImmediateNotification(l_cMsgSource, l_cNotfName, NULL, l_uiMsgLength);
    }
  } catch (std::exception &e) {  // LCOV_EXCL_START 5: exception error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    f_pSourceState = NULL;
    // LCOV_EXCL_STOP
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return f_pSourceState;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceOnNPPersistentSync
/// Processing which synchronizes by NPPService (syncfs)
///
////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_INTERNALTRANSITION(NotificationpersistentserviceOnNPPersistentSync) {
  try {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");
    CHKNULL(f_pSourceState);

    HANDLE l_pHApp = f_pSourceState->FrameworkunifiedGetAppHandle();

    CFrameworkunifiedHSM *l_pNotificationpersistentserviceHSM = FrameworkunifiedGetStateMachine(l_pHApp);
    CHKNULL(l_pNotificationpersistentserviceHSM);
    CNSNPP *l_pnsnpp = static_cast<CNSNPP *>(l_pNotificationpersistentserviceHSM);
    CHKNULL(l_pnsnpp);

    if (TRUE == l_pnsnpp->Syncfs()) {  // LCOV_EXCL_BR_LINE 6: always return true
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "syncfs are processed.");  // LCOV_EXCL_LINE 6: always return true
    }
  } catch (std::exception &e) {  // LCOV_EXCL_START 5: exception error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
    return NULL;
    // LCOV_EXCL_STOP
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return f_pSourceState;
}

IMPLEMENT_INTERNALTRANSITION(NotificationpersistentserviceOnNPReleaseFileAck) {
  try {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");

    NSP_CopyStatusResponse l_tCpStatus = {};

    HANDLE l_pHApp = f_pSourceState->FrameworkunifiedGetAppHandle();
    CHKNULL(l_pHApp);

    CFrameworkunifiedHSM *l_pNotificationpersistentserviceHSM = FrameworkunifiedGetStateMachine(l_pHApp);
    CHKNULL(l_pNotificationpersistentserviceHSM);

    CNSNPP *l_pnsnpp = static_cast<CNSNPP *>(l_pNotificationpersistentserviceHSM);

    if (eFrameworkunifiedStatusOK == FrameworkunifiedGetMsgDataOfSize(l_pHApp, (PVOID)&l_tCpStatus, sizeof(l_tCpStatus), eSMRRelease)) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "%s for tag: %s sender: %s, persist type:%d",
             (l_tCpStatus.m_bpersistencechk ? "Successful copy" : "Non-Successful copy"),
             l_tCpStatus.m_cpersistenttag, l_tCpStatus.m_crequesterappname, l_tCpStatus.m_eloadtype);

      if (LOADTYPE_LOAD == l_tCpStatus.m_eloadtype) {  // ack for load file/folder
        HANDLE hMq = NULL;
        NC_LoadPersistedAck l_tMsgAck = {};

        // HANDLE hMq = McOpenSender(l_tCpStatus.sender);
        hMq = FrameworkunifiedMcOpenSender(l_pHApp, l_tCpStatus.m_crequesterappname);

        // send NC_LoadPersistedAck to the requester
        l_tMsgAck.eStatus = (TRUE == l_tCpStatus.m_bpersistencechk) ? eFrameworkunifiedStatusOK : eFrameworkunifiedStatusFail;

#ifdef AGL_PosixBasedOS001LEGACY_USED
        strlcpy(l_tMsgAck.cTag, l_tCpStatus.m_cpersistenttag, sizeof(l_tMsgAck.cTag));
#endif

        if (NULL == hMq) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          // catastrophic failure!
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "McOpenSender failed for %s ", l_tCpStatus.m_crequesterappname);  // LCOV_EXCL_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
        } else {
          if (ENOTIFICATIONPERSISTENTSERVICEPERSISTFILE == l_tCpStatus.m_epersisttype) {
            // send ack to requester for file successfully persisted
            if (eFrameworkunifiedStatusOK != FrameworkunifiedSendMsg(hMq, NPS_GET_PERS_FILE_ACK, sizeof(l_tMsgAck), &l_tMsgAck)) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
              // LCOV_EXCL_START 4: NSFW error case
              AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
              FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                     "ERROR :: Sending NPS_GET_PERS_FILE_ACK message to %s", l_tCpStatus.m_crequesterappname);
              // LCOV_EXCL_STOP
            } else {
              FRAMEWORKUNIFIEDLOG(ZONE_PRD_INFO2, __FUNCTION__,
                     "NPS_GET_PERS_FILE_ACK is sent to %s for file tag %s and load status: %d.",
                     l_tCpStatus.m_crequesterappname, l_tMsgAck.cTag, l_tMsgAck.eStatus);
            }
          } else if (ENOTIFICATIONPERSISTENTSERVICEPERSISTFOLDER == l_tCpStatus.m_epersisttype) {  // LCOV_EXCL_BR_LINE 6: m_epersisttype must be ENOTIFICATIONPERSISTENTSERVICEPERSISTFILE or ENOTIFICATIONPERSISTENTSERVICEPERSISTFOLDER  // NOLINT[whitespace/line_length]
            // send ack to requester for folder successfully persisted
            if (eFrameworkunifiedStatusOK != FrameworkunifiedSendMsg(hMq, NPS_GET_PERS_FOLDER_ACK, sizeof(l_tMsgAck), &l_tMsgAck)) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
              // LCOV_EXCL_START 4: NSFW error case
              AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
              FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                     "ERROR :: Sending NPS_GET_PERS_FOLDER_ACK message to %s", l_tCpStatus.m_crequesterappname);
              // LCOV_EXCL_STOP
            } else {
              FRAMEWORKUNIFIEDLOG(ZONE_PRD_INFO2, __FUNCTION__,
                     "NPS_GET_PERS_FOLDER_ACK is sent to %s for folder tag %s and load status: %d.",
                     l_tCpStatus.m_crequesterappname, l_tMsgAck.cTag, l_tMsgAck.eStatus);
            }
          } else {
            // do nothing
          }
          // close mq
          FrameworkunifiedMcClose(hMq);
          hMq = NULL;
        }
      }

      // Update registry
      CPersistenceManager *l_pPersistenceManager = l_pnsnpp->m_pPersistenceManager;
      if (l_pPersistenceManager) {  // LCOV_EXCL_BR_LINE 6: unexpected branch can't be NULL
        l_pPersistenceManager->AckReceivedFromWorker(l_tCpStatus.m_crequesterappname,
                                                     l_tCpStatus.m_cpersistenttag,
                                                     l_tCpStatus.m_epersisttype,
                                                     l_tCpStatus.m_bpersistencechk,
                                                     l_tCpStatus.m_eloadtype);
      }

      if (LOADTYPE_RELEASE == l_tCpStatus.m_eloadtype) {
        f_pSourceState->FrameworkunifiedPostEvent(EVENT(evCheckAllFilesPersisted));
      }
    } else {
      // LCOV_EXCL_START 4: NSFW error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "ERROR ::invalid length FrameworkunifiedGetMsgDataOfSize expected(%ld)", static_cast<long int>(sizeof(l_tCpStatus))); // NOLINT (runtime/int)
      // LCOV_EXCL_STOP
    }
  } catch (std::exception &e) {  // LCOV_EXCL_START 5: exception error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
    return NULL;
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return f_pSourceState;
}


IMPLEMENT_INTERNALTRANSITION(NotificationpersistentserviceOnSaveDataAck) {  // LCOV_EXCL_START 6: unused code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return f_pSourceState;
}
// LCOV_EXCL_STOP

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceOnUserChange
/// The state machine executes this transition when event to set or change personality is received
/// from the system.
////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_INTERNALTRANSITION(NotificationpersistentserviceOnUserChange) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

  try {
    CHKNULL(f_pSourceState);
    NC_User l_tMsg;
    std::string l_cCurrentUsername;

    HANDLE l_pHApp = f_pSourceState->FrameworkunifiedGetAppHandle();
    // TODO(my_username): Only accept user change from authorized app. Compare l_cMsgSource with authorized source.
    if (eFrameworkunifiedStatusOK == (l_estatus = FrameworkunifiedGetMsgDataOfSize(l_pHApp, static_cast<PVOID>(&l_tMsg), sizeof(NC_User), eSMRRelease))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
      CFrameworkunifiedHSM *l_pNotificationpersistentserviceHSM = FrameworkunifiedGetStateMachine(l_pHApp);
      CNSNPP *l_pnsnpp = static_cast<CNSNPP *>(l_pNotificationpersistentserviceHSM);

      if (NULL != l_pnsnpp) {  // LCOV_EXCL_BR_LINE 6: l_pnsnpp can't be NULL
        CPersistenceManager *l_pPersistenceManager = l_pnsnpp->m_pPersistenceManager;
        CnotificationpersistentservicePersonalizationManager *l_pPersonalizationManager = l_pnsnpp->m_pPersonalizationManager;

        if (NULL != l_pPersonalizationManager) {  // LCOV_EXCL_BR_LINE 6: l_pPersonalizationManager can't be NULL
          l_pPersonalizationManager->NotificationpersistentserviceGetPersonality(l_cCurrentUsername);
        }

        // writes the temporary user files and folders to permanent memory
        if (NULL != l_pPersistenceManager) {  // LCOV_EXCL_BR_LINE 6: l_pPersistenceManager can't be NULL
          if (!l_cCurrentUsername.empty()) {  // LCOV_EXCL_BR_LINE 6: double check, l_cCurrentUsername can't be empty
            if (eFrameworkunifiedStatusOK != l_pPersistenceManager->NotificationpersistentservicePersistAllUserRequests()) {  // LCOV_EXCL_BR_LINE 6: NotificationpersistentservicePersistAllUserRequests will always return ok  // NOLINT[whitespace/line_length]
              AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
              FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error while persisting user files and folders.");  // LCOV_EXCL_LINE 6: NotificationpersistentservicePersistAllUserRequests will always return ok  // NOLINT[whitespace/line_length]
            }
          }
        }

        // save the user persistent notification data
        if (eFrameworkunifiedStatusOK != l_pnsnpp->NotificationpersistentserviceSaveNotificationPersistentData(eFrameworkunifiedPersistedStateUserVar)) {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error while NotificationpersistentserviceSaveNotificationPersistentData");
        }
      }

      // set new personality
      SetPersonality(l_pHApp, l_tMsg);
    } else {
      // LCOV_EXCL_START 4: NSFW error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__,
             "Cannot set new personality, error getting message data, status: 0x%x", l_estatus);
      // LCOV_EXCL_STOP
    }
  } catch (std::exception &e) {  // LCOV_EXCL_START 5: exception error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
    return NULL;
    // LCOV_EXCL_STOP
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return f_pSourceState;
}

// ===========Shutdown
IMPLEMENT_INTERNALTRANSITION(NotificationpersistentserviceOnShutdownTimeout) {  // LCOV_EXCL_START 200: cannot test code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return f_pSourceState;
}
// LCOV_EXCL_STOP

IMPLEMENT_INTERNALTRANSITION(NotificationpersistentserviceCheckAllReleaseRequestsProcessed) {
  try {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");
    CHKNULL(f_pSourceState);

    HANDLE l_pHApp = f_pSourceState->FrameworkunifiedGetAppHandle();

    CFrameworkunifiedHSM *l_pNotificationpersistentserviceHSM = FrameworkunifiedGetStateMachine(l_pHApp);
    CHKNULL(l_pNotificationpersistentserviceHSM);
    CNSNPP *l_pnsnpp = static_cast<CNSNPP *>(l_pNotificationpersistentserviceHSM);

    CPersistenceManager *l_pPersistenceManager = l_pnsnpp->m_pPersistenceManager;
    CHKNULL(l_pPersistenceManager);
    if (TRUE == l_pPersistenceManager->HaveAllReleaseRequestsPersisted()) {  // LCOV_EXCL_BR_LINE 6: always return ok
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "All release requests are processed.");
      f_pSourceState->FrameworkunifiedPostEvent(EVENT(evIdle));
    }
  } catch (std::exception &e) {  // LCOV_EXCL_START 5: exception error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
    return NULL;
    // LCOV_EXCL_STOP
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return f_pSourceState;
}

IMPLEMENT_INTERNALTRANSITION(NotificationpersistentserviceReInitShutdownTimer) {   // LCOV_EXCL_START 8: not be used
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return f_pSourceState;
}
// LCOV_EXCL_STOP

CsNotificationpersistentserviceNPPStart::CsNotificationpersistentserviceNPPStart(std::string f_strName): CFrameworkunifiedOrthogonalState(f_strName) {
}

CsNotificationpersistentserviceNPPStart::~CsNotificationpersistentserviceNPPStart() {  // LCOV_EXCL_START 200: cannot test code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus CsNotificationpersistentserviceNPPStart::FrameworkunifiedOnEntry(CEventDataPtr f_peventdata) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusFail;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");

  try {
    HANDLE l_pHApp = FrameworkunifiedGetAppHandle();
    CHKNULL(l_pHApp);

    CFrameworkunifiedHSM *l_pNotificationpersistentserviceHSM = FrameworkunifiedGetStateMachine(l_pHApp);
    CNSNPP *l_pnsnpp = static_cast<CNSNPP *>(l_pNotificationpersistentserviceHSM);

    // Get instance of notification manager
    CNotificationManager *l_pNotificationManager = l_pnsnpp->m_pNotificationManager;

    if (l_pNotificationManager) {  // LCOV_EXCL_BR_LINE 6: l_pNotificationManager can't be NULL
      NC_User l_tMsg = {};

      // register the user change notification
      if (eFrameworkunifiedStatusOK != l_pNotificationManager->NotificationpersistentserviceServiceOnRegisterEvents(AppName,  // LCOV_EXCL_BR_LINE 6: always return ok  // NOLINT[whitespace/line_length]
                                                                             NTFY_NPPService_UserChange,
                                                                             sizeof(NC_User),
                                                                             eFrameworkunifiedPersistedStateVar)) {
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error in user change notification registration.");  // LCOV_EXCL_LINE 6: always return ok  // NOLINT[whitespace/line_length]
      }

      // Set initial user. By default, NS_NPPService will set the current user as last user. If no
      // last user found set the default user.
      UI_32 l_uiUserNotificationDataSize = 0;
      EFrameworkunifiedStatus l_eGetNotificationDataStatus = eFrameworkunifiedStatusFail;
      l_eGetNotificationDataStatus = l_pNotificationManager->NPGetPersistentNotificationData(
                  NTFY_NPPService_UserChange,
                  (PVOID)&l_tMsg,
                  sizeof(l_tMsg));

      if ((eFrameworkunifiedStatusOK != l_eGetNotificationDataStatus) || (l_uiUserNotificationDataSize <= 0)) {
        // set default user
#ifdef AGL_PosixBasedOS001LEGACY_USED
        strlcpy(l_tMsg.cUsername, DEFAULTUSERNAME, sizeof(l_tMsg.cUsername));
#endif
      }

      l_estatus = SetPersonality(l_pHApp, l_tMsg);
    }
  } catch (std::exception &e) {  // LCOV_EXCL_START 5: exception error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    l_estatus =  eFrameworkunifiedStatusInvldHandle;
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return l_estatus;
}

EFrameworkunifiedStatus SetPersonality(HANDLE l_pHApp, NC_User &f_tMsg) { // NOLINT (runtime/references)
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusFail;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");

  CFrameworkunifiedHSM *l_pNotificationpersistentserviceHSM = FrameworkunifiedGetStateMachine(l_pHApp);
  CNSNPP *l_pnsnpp = static_cast<CNSNPP *>(l_pNotificationpersistentserviceHSM);

  if (NULL != l_pnsnpp) {  // LCOV_EXCL_BR_LINE 6: l_pnsnpp can't be NULL
    // Get the instance of personalization manager
    CnotificationpersistentservicePersonalizationManager *l_pNotificationpersistentservicePersonalizationManager = l_pnsnpp->m_pPersonalizationManager;

    if (NULL != l_pNotificationpersistentservicePersonalizationManager) {  // LCOV_EXCL_BR_LINE 6: l_pNotificationpersistentservicePersonalizationManager can't be NULL
      // send msg to all notificationpersistentservice apps that user has change.
      // register user change notification
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Personality :: %s", f_tMsg.cUsername);

      if (eFrameworkunifiedStatusOK != (l_estatus = l_pNotificationpersistentservicePersonalizationManager->NotificationpersistentserviceSetPersonality(f_tMsg.cUsername))) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Personality not set in Personality Manager :: user : %s", f_tMsg.cUsername);
      } else {
        // Get the instance of persistence manager
        CPersistenceManager *l_pPersistenceManager = l_pnsnpp->m_pPersistenceManager;

        if (NULL != l_pPersistenceManager) {  // LCOV_EXCL_BR_LINE 6: l_pPersistenceManager can't be NULL
          l_pPersistenceManager->SetUserPersistentPath(f_tMsg.cUsername);

          // save the user persistent notification data
          if (eFrameworkunifiedStatusOK != l_pnsnpp->NotificationpersistentserviceLoadPersistentNotificationData(eFrameworkunifiedPersistedStateUserVar)) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error while loading user specific notification data.");
          }

          // Get instance of notification manager
          CNotificationManager *l_pNotificationManager = NULL;
          l_pNotificationManager = l_pnsnpp->m_pNotificationManager;

          if (NULL != l_pNotificationManager) {  // LCOV_EXCL_BR_LINE 6: l_pNotificationManager can't be NULL
            // publish the user change notification
            if (eFrameworkunifiedStatusOK != (l_estatus = l_pNotificationManager->NotificationpersistentserviceServiceOnPublishEvent(AppName, NTFY_NPPService_UserChange, (PVOID)&f_tMsg, sizeof(f_tMsg)))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
              AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
              FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error in publish user change event.");  // LCOV_EXCL_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
            }
          }
        }
      }
    } else {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "l_pNotificationpersistentservicePersonalizationManager is NULL");  // LCOV_EXCL_LINE 6: l_pNotificationpersistentservicePersonalizationManager can't be NULL  // NOLINT[whitespace/line_length]
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return l_estatus;
}

EFrameworkunifiedStatus CsNotificationpersistentserviceNPPStart:: FrameworkunifiedOnExit(CEventDataPtr f_peventdata) {  // LCOV_EXCL_START 200: cannot test code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  return eFrameworkunifiedStatusOK;
  // LCOV_EXCL_STOP
}

CsNotificationpersistentserviceShutdownDataSave::CsNotificationpersistentserviceShutdownDataSave(std::string f_strName): CFrameworkunifiedLeafState(f_strName) {
}

CsNotificationpersistentserviceShutdownDataSave::~CsNotificationpersistentserviceShutdownDataSave() {  // LCOV_EXCL_START 200: cannot test code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus CsNotificationpersistentserviceShutdownDataSave::FrameworkunifiedOnEntry(CEventDataPtr f_peventdata) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");

  HANDLE l_pHApp = FrameworkunifiedGetAppHandle();
  CHKNULL(l_pHApp);

  PCSTR l_cMsgSource = FrameworkunifiedGetMsgSrc(l_pHApp);
  m_cShutdownRequestor = l_cMsgSource;
  FRAMEWORKUNIFIEDLOG(ZONE_PRD_INFO2, __FUNCTION__, "NS_NPP received STOP message from %s.", l_cMsgSource);

  CFrameworkunifiedHSM *l_pNotificationpersistentserviceHSM = FrameworkunifiedGetStateMachine(l_pHApp);
  CNSNPP *l_pnsnpp = static_cast<CNSNPP *>(l_pNotificationpersistentserviceHSM);

  if (NULL != l_pnsnpp) {  // LCOV_EXCL_BR_LINE 6: can't be NULL
    CShutDownMsgData *l_pMsgData = dynamic_cast<CShutDownMsgData *>(f_peventdata.get());
    CHKNULL(l_pMsgData);

    FRAMEWORKUNIFIEDLOG(ZONE_PRD_INFO2, __FUNCTION__, "Shutdown Type=%d, Data received with shutdown message:: %x",
           l_pMsgData->m_eShutdownType, l_pMsgData->m_uiMsgData);

    // save the persistent data on shutdown
    if (eFrameworkunifiedStatusOK != l_pnsnpp->NotificationpersistentservicePersistAll(l_pMsgData->m_eShutdownType, l_pMsgData->m_uiMsgData)) {  // LCOV_EXCL_BR_LINE 6: always return ok   // NOLINT[whitespace/line_length]
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error in NotificationpersistentserviceOnSavePersistentData");  // LCOV_EXCL_LINE 6: always return ok
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus CsNotificationpersistentserviceShutdownDataSave:: FrameworkunifiedOnExit(CEventDataPtr f_peventdata) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  try {
    HANDLE l_pHApp = FrameworkunifiedGetAppHandle();
    CHKNULL(l_pHApp);

    CFrameworkunifiedHSM *l_pNotificationpersistentserviceHSM = FrameworkunifiedGetStateMachine(l_pHApp);
    CNSNPP *l_pnsnpp = static_cast<CNSNPP *>(l_pNotificationpersistentserviceHSM);
    CHKNULL(l_pnsnpp);

    CPersistenceManager *l_pPersistenceManager = l_pnsnpp->m_pPersistenceManager;
    CHKNULL(l_pPersistenceManager);

    l_pPersistenceManager->ResetPersistFlag();

    // before sending shutdown ack, synchronize all the modified block buffers by NPPService for writing
    l_pnsnpp->Syncfs();

    // Send shutdown ack response to requester
    FRAMEWORKUNIFIEDLOG(ZONE_PRD_INFO2, __FUNCTION__, "Sending shutdown complete message to requestor  %s.",
           m_cShutdownRequestor.c_str());
    HANDLE hMq = FrameworkunifiedMcOpenSender(l_pHApp, m_cShutdownRequestor.c_str());
    if (NULL != hMq) {   // LCOV_EXCL_BR_LINE 4: NSFW error case.
      if (eFrameworkunifiedStatusOK != FrameworkunifiedSendMsg(hMq, NPS_NPP_STOP_ACK, 0, NULL)) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
        // LCOV_EXCL_START 4: NSFW error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error sending shutdown complete message to requestor  %s.",
               m_cShutdownRequestor.c_str());
        // LCOV_EXCL_STOP
      }
      FrameworkunifiedMcClose(hMq);
    } else {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error in OpenSender for requestor %s.", m_cShutdownRequestor.c_str());  // LCOV_EXCL_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
    }
  } catch (std::exception &e) {  // LCOV_EXCL_START 5: exception error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eFrameworkunifiedStatusOK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceOnRegisterPersistentFile
/// The state machine executes this transition when event for registering persistent file tag is
/// received.
////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_INTERNALTRANSITION(NotificationpersistentserviceOnRegisterPersistentFile) {
  try {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");
    CHKNULL(f_pSourceState);
    NC_RegisterPersistentFileMsg l_tMsg = {};

    HANDLE l_pHApp = f_pSourceState->FrameworkunifiedGetAppHandle();
    CHKNULL(l_pHApp);
    PCSTR l_cMsgSource = FrameworkunifiedGetMsgSrc(l_pHApp);

    CFrameworkunifiedHSM *l_pNotificationpersistentserviceHSM = FrameworkunifiedGetStateMachine(l_pHApp);
    CNSNPP *l_pnsnpp = static_cast<CNSNPP *>(l_pNotificationpersistentserviceHSM);

    CPersistenceManager *l_pPersistenceManager = l_pnsnpp->m_pPersistenceManager;


    if (NULL != l_pPersistenceManager && NULL != l_cMsgSource) {  // LCOV_EXCL_BR_LINE 6: l_pPersistenceManager and l_cMsgSource can't be NULL  // NOLINT[whitespace/line_length]
      if (eFrameworkunifiedStatusOK == FrameworkunifiedGetMsgDataOfSize(l_pHApp, static_cast<PVOID>(&l_tMsg), sizeof(l_tMsg), eSMRRelease)) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
        std::string l_cName;
        l_cName = l_tMsg.cFileTag;

        if (eFrameworkunifiedStatusOK != l_pPersistenceManager->NotificationpersistentserviceRegister(l_cMsgSource,  // LCOV_EXCL_BR_LINE 6: always return ok
                                                               l_tMsg.cFileTag,
                                                               ENOTIFICATIONPERSISTENTSERVICEPERSISTFILE,
                                                               // l_tMsg.eRegisterType,
                                                               l_tMsg.bIsUserFile)) {
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error in Registering Persitence File :: %s", l_tMsg.cFileTag);  // LCOV_EXCL_LINE 6: always return ok  // NOLINT[whitespace/line_length]
        }

        FRAMEWORKUNIFIEDLOG(ZONE_PRD_INFO2, __FUNCTION__, "Registered File %s as TO BE PERSISTED for %s",
               l_tMsg.cFileTag, l_cMsgSource);
      } else {
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Can't register persistent file. Invalid message size received.");  // LCOV_EXCL_LINE 6: l_pPersistenceManager and l_cMsgSource can't be NULL  // NOLINT[whitespace/line_length]
      }
    } else {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Can't register persistent file. l_pPersistenceManager is NULL");  // LCOV_EXCL_LINE 6: l_pPersistenceManager and l_cMsgSource can't be NULL  // NOLINT[whitespace/line_length]
    }
  } catch (std::exception &e) {  // LCOV_EXCL_START 5: exception error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
    return NULL;
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return f_pSourceState;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceOnReleasePersistentFile
/// The state machine executes this transition when event for releasing persistent file tag is
/// received.
////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_INTERNALTRANSITION(NotificationpersistentserviceOnReleasePersistentFile) {
  try {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");

    // Check if  persistence has been disabled than return immediately
    if (TRUE == CPersistenceManager::m_bPersistenceDisabled) {  // LCOV_EXCL_BR_LINE 6: custom command line options
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      return f_pSourceState;  // LCOV_EXCL_LINE 6: custom command line options
    }

    CHKNULL(f_pSourceState);
    NC_ReleasePersistentFileMsg l_tMsg = {};

    HANDLE l_pHApp = f_pSourceState->FrameworkunifiedGetAppHandle();
    CHKNULL(l_pHApp);
    PCSTR l_cMsgSource = FrameworkunifiedGetMsgSrc(l_pHApp);

    CFrameworkunifiedHSM *l_pNotificationpersistentserviceHSM = FrameworkunifiedGetStateMachine(l_pHApp);
    CNSNPP *l_pnsnpp = static_cast<CNSNPP *>(l_pNotificationpersistentserviceHSM);

    // get the instance of persistence manager
    CPersistenceManager *l_pPersistenceManager = l_pnsnpp->m_pPersistenceManager;
    // get the instance of persistence manager
    CnotificationpersistentservicePersonalizationManager *l_pNotificationpersistentservicePersonalizationManager = l_pnsnpp->m_pPersonalizationManager;

    if (NULL != l_pPersistenceManager && NULL != l_cMsgSource && NULL != l_pNotificationpersistentservicePersonalizationManager) {  // LCOV_EXCL_BR_LINE 6: l_pPersistenceManager,  l_cMsgSource and l_pNotificationpersistentservicePersonalizationManager can't be NULL  // NOLINT[whitespace/line_length]
      if (eFrameworkunifiedStatusOK == FrameworkunifiedGetMsgDataOfSize(l_pHApp, static_cast<PVOID>(&l_tMsg), sizeof(l_tMsg), eSMRRelease)) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
        // Check if the filetag is registered as user specific persistence
        if (TRUE == l_pPersistenceManager->IsUserPersistence(l_tMsg.cFileTag, ENOTIFICATIONPERSISTENTSERVICEPERSISTFILE)) {
          if (!l_pNotificationpersistentservicePersonalizationManager->NotificationpersistentserviceIsValidPersonality(l_tMsg.cUsername)) {
            FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
            return f_pSourceState;
          }
        }

        if (eFrameworkunifiedStatusOK != l_pPersistenceManager->NotificationpersistentserviceRelease(l_cMsgSource,
                                                              l_tMsg.cFileTag,
                                                              l_tMsg.cFilePath,
                                                              l_tMsg.eFrameworkunifiedReleaseType, // NOLINT (readability/naming)
                                                              ENOTIFICATIONPERSISTENTSERVICEPERSISTFILE,
                                                              l_tMsg.cUsername)) {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error in releasing persitence file :: %s", l_tMsg.cFileTag);
        }

        FRAMEWORKUNIFIEDLOG(ZONE_PRD_INFO2, __FUNCTION__, "Release file %s to tag %s for source %s",
               l_tMsg.cFilePath, l_tMsg.cFileTag, l_cMsgSource);
      } else {
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Can't release persistent file. Invalid message size received");  // LCOV_EXCL_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
      }
    } else {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Can't release persistent file. l_pPersistenceManager is NULL");  // LCOV_EXCL_LINE 6: l_pPersistenceManager,  l_cMsgSource and l_pNotificationpersistentservicePersonalizationManager can't be NULL  // NOLINT[whitespace/line_length]
    }
  } catch (std::exception &e) {  // LCOV_EXCL_START 5: exception error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
    return NULL;
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return f_pSourceState;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceOnLoadPersistentFile
/// The state machine executes this transition when event for loading persistent file is received.
////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_INTERNALTRANSITION(NotificationpersistentserviceOnLoadPersistentFile) {
  try {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");
    CHKNULL(f_pSourceState);
    NC_LoadPersistedFileMsg l_tMsg = {};

    HANDLE l_pHApp = f_pSourceState->FrameworkunifiedGetAppHandle();
    CHKNULL(l_pHApp);
    PCSTR l_cMsgSource = FrameworkunifiedGetMsgSrc(l_pHApp);

    CFrameworkunifiedHSM *l_pNotificationpersistentserviceHSM = FrameworkunifiedGetStateMachine(l_pHApp);
    CNSNPP *l_pnsnpp = static_cast<CNSNPP *>(l_pNotificationpersistentserviceHSM);

    CnotificationpersistentservicePersonalizationManager *l_pNotificationpersistentservicePersonalizationManager = l_pnsnpp->m_pPersonalizationManager;
    // get the instance of persistence manager
    CPersistenceManager *l_pPersistenceManager = l_pnsnpp->m_pPersistenceManager;

    if (NULL != l_pPersistenceManager && NULL != l_cMsgSource && NULL != l_pNotificationpersistentservicePersonalizationManager) {  // LCOV_EXCL_BR_LINE 6: l_pPersistenceManager,  l_cMsgSource and l_pNotificationpersistentservicePersonalizationManager can't be NULL  // NOLINT[whitespace/line_length]
      if (eFrameworkunifiedStatusOK == FrameworkunifiedGetMsgDataOfSize(l_pHApp, static_cast<PVOID>(&l_tMsg), sizeof(l_tMsg), eSMRRelease)) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
        FRAMEWORKUNIFIEDLOG(ZONE_PRD_INFO2, __FUNCTION__, "Load file %s that was PERSISTED by %s with tag %s",
               l_tMsg.cFilePath, l_cMsgSource, l_tMsg.cFileTag);

        // Check if the filetag is registered as user specific persistence
        if (TRUE == l_pPersistenceManager->IsUserPersistence(l_tMsg.cFileTag, ENOTIFICATIONPERSISTENTSERVICEPERSISTFILE)) {
          if (!l_pNotificationpersistentservicePersonalizationManager->NotificationpersistentserviceIsValidPersonality(l_tMsg.cUsername)) {
            FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
            return f_pSourceState;
          }
        }

        EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
        NC_LoadPersistedAck l_tMsgAck = {};

        HANDLE l_hMq = FrameworkunifiedMcOpenSender(l_pHApp, l_cMsgSource);

        if (NULL != l_hMq) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
          //// Check if  persistence has been disabled than send positive acknowledgement to the application
          if (TRUE == CPersistenceManager::m_bPersistenceDisabled) {  // LCOV_EXCL_BR_LINE 6: custom command line options  // NOLINT[whitespace/line_length]
            // LCOV_EXCL_START 6: custom command line options
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            l_tMsgAck.eStatus = eFrameworkunifiedStatusOK;
#ifdef AGL_PosixBasedOS001LEGACY_USED
            strlcpy(l_tMsgAck.cTag, l_tMsg.cFileTag, sizeof(l_tMsgAck.cTag));
#endif

            // send ack to requester
            if (eFrameworkunifiedStatusOK != (l_estatus = FrameworkunifiedSendMsg(l_hMq, NPS_GET_PERS_FILE_ACK, sizeof(l_tMsgAck), &l_tMsgAck))) {
              FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: FrameworkunifiedSendMsg returned l_estatus=%d", l_estatus);
            }
            // LCOV_EXCL_STOP
          } else {
            if (eFrameworkunifiedStatusOK != (l_estatus = l_pPersistenceManager->NotificationpersistentserviceLoad(l_cMsgSource,
                                                                            l_tMsg.cFileTag,
                                                                            l_tMsg.cFilePath,
                                                                            ENOTIFICATIONPERSISTENTSERVICEPERSISTFILE,
                                                                            l_tMsg.cUsername))) {
              if (eFrameworkunifiedStatusFileLoadSuccess == l_estatus) {  // LCOV_EXCL_BR_LINE 6: l_estatus can't be eFrameworkunifiedStatusFileLoadSuccess  // NOLINT[whitespace/line_length]
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                l_tMsgAck.eStatus = eFrameworkunifiedStatusOK;  // LCOV_EXCL_LINE 6: l_estatus can't be eFrameworkunifiedStatusFileLoadSuccess
              } else {
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error in Loading Persitence File :: %s", l_tMsg.cFileTag);

                // return error to requester
                l_tMsgAck.eStatus = l_estatus;
              }

              std::strncpy(l_tMsgAck.cTag, l_tMsg.cFileTag, (MAX_PATH_LENGTH - 1));

              // send ack to requester
              if (eFrameworkunifiedStatusOK != (l_estatus = FrameworkunifiedSendMsg(l_hMq, NPS_GET_PERS_FILE_ACK, sizeof(l_tMsgAck), &l_tMsgAck))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: FrameworkunifiedSendMsg returned l_estatus=%d", l_estatus);  // LCOV_EXCL_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
              }
              FRAMEWORKUNIFIEDLOG(ZONE_PRD_INFO2, __FUNCTION__, "NPS_GET_PERS_FILE_ACK is sent to %s. File load status: %d, "
                     "message sent status: %d ", l_cMsgSource, l_tMsgAck.eStatus, l_estatus);
            }
          }

          FrameworkunifiedMcClose(l_hMq);
        } else {
          // LCOV_EXCL_START 4: NSFW error case
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          // catastrophic failure!
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "McOpenSender failed for %s", l_cMsgSource);
          // LCOV_EXCL_STOP
        }
      } else {
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Can't load persistent file. Invalid message size received.");  // LCOV_EXCL_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
      }
    } else {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Can't load persistent file. l_pNotificationManager is NULL");  // LCOV_EXCL_LINE 6: l_pPersistenceManager, l_cMsgSource and l_pNotificationpersistentservicePersonalizationManager can't be NULL  // NOLINT[whitespace/line_length]
    }
  } catch (std::exception &e) {  // LCOV_EXCL_START 5: exception error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
    return NULL;
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return f_pSourceState;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceOnRegisterPersistentFolder
/// The state machine executes this transition when event for registering persistent folder tag is
/// received.
////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_INTERNALTRANSITION(NotificationpersistentserviceOnRegisterPersistentFolder) {
  try {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");
    CHKNULL(f_pSourceState);
    NC_RegisterPersistentFolderMsg l_tMsg = {};

    HANDLE l_pHApp = f_pSourceState->FrameworkunifiedGetAppHandle();
    CHKNULL(l_pHApp);
    PCSTR l_cMsgSource = FrameworkunifiedGetMsgSrc(l_pHApp);

    CFrameworkunifiedHSM *l_pNotificationpersistentserviceHSM = FrameworkunifiedGetStateMachine(l_pHApp);
    CNSNPP *l_pnsnpp = static_cast<CNSNPP *>(l_pNotificationpersistentserviceHSM);

    CPersistenceManager *l_pPersistenceManager = l_pnsnpp->m_pPersistenceManager;

    if (NULL != l_pPersistenceManager && NULL != l_cMsgSource) {  // LCOV_EXCL_BR_LINE 6: l_pPersistenceManager and l_cMsgSource can't be NULL  // NOLINT[whitespace/line_length]
      if (eFrameworkunifiedStatusOK == FrameworkunifiedGetMsgDataOfSize(l_pHApp,  // LCOV_EXCL_BR_LINE 4: NSFW error case
                                              static_cast<PVOID>(&l_tMsg),
                                              sizeof(l_tMsg),
                                              eSMRRelease)) {
        std::string l_cName;
        l_cName = l_tMsg.cFolderTag;

        if (eFrameworkunifiedStatusOK != l_pPersistenceManager->NotificationpersistentserviceRegister(l_cMsgSource,  // LCOV_EXCL_BR_LINE 6: always return ok
                                                               l_tMsg.cFolderTag,
                                                               ENOTIFICATIONPERSISTENTSERVICEPERSISTFOLDER,
                                                               // l_tMsg.eRegisterType,
                                                               l_tMsg.bIsUserFolder)) {
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error in Registering Persitence Folder :: %s", l_tMsg.cFolderTag);  // LCOV_EXCL_LINE 6: always return ok  // NOLINT[whitespace/line_length]
        }

        FRAMEWORKUNIFIEDLOG(ZONE_PRD_INFO2, __FUNCTION__, "Registered Folder %s as TO BE PERSISTED for %s",
               l_tMsg.cFolderTag, l_cMsgSource);
      } else {
        // LCOV_EXCL_START 4: NSFW error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Can't register persistent folder. Invalid message size received.");
        // LCOV_EXCL_STOP
      }
    } else {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Can't register persistent folder.. l_pPersistenceManager is NULL");  // LCOV_EXCL_LINE 6: l_pPersistenceManager and l_cMsgSource can't be NULL  // NOLINT[whitespace/line_length]
    }
  } catch (std::exception &e) {  // LCOV_EXCL_START 5: exception error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
    return NULL;
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return f_pSourceState;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceOnLoadPersistentFolder
/// The state machine executes this transition when event for loading persistent folder is received.
////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_INTERNALTRANSITION(NotificationpersistentserviceOnLoadPersistentFolder) {
  try {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");
    CHKNULL(f_pSourceState);
    NC_LoadPersistedFolderMsg l_tMsg = {};

    HANDLE l_pHApp = f_pSourceState->FrameworkunifiedGetAppHandle();
    CHKNULL(l_pHApp);
    PCSTR l_cMsgSource = FrameworkunifiedGetMsgSrc(l_pHApp);

    CFrameworkunifiedHSM *l_pNotificationpersistentserviceHSM = FrameworkunifiedGetStateMachine(l_pHApp);
    CNSNPP *l_pnsnpp = static_cast<CNSNPP *>(l_pNotificationpersistentserviceHSM);

    CnotificationpersistentservicePersonalizationManager *l_pNotificationpersistentservicePersonalizationManager = l_pnsnpp->m_pPersonalizationManager;

    // get the instance of persistence manager
    CPersistenceManager *l_pPersistenceManager = l_pnsnpp->m_pPersistenceManager;

    if (NULL != l_pPersistenceManager && NULL != l_cMsgSource && NULL != l_pNotificationpersistentservicePersonalizationManager) {  // LCOV_EXCL_BR_LINE 6: l_pPersistenceManager,  l_cMsgSource and l_pNotificationpersistentservicePersonalizationManager can't be NULL  // NOLINT[whitespace/line_length]
      if (eFrameworkunifiedStatusOK == FrameworkunifiedGetMsgDataOfSize(l_pHApp, static_cast<PVOID>(&l_tMsg), sizeof(l_tMsg), eSMRRelease)) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
        FRAMEWORKUNIFIEDLOG(ZONE_PRD_INFO2, __FUNCTION__, "Load folder %s that was PERSISTED by %s with tag %s",
               l_tMsg.cFolderPath, l_cMsgSource, l_tMsg.cFolderTag);

        // Check if the foldertag is registered as user specific persistence
        if (TRUE == l_pPersistenceManager->IsUserPersistence(l_tMsg.cFolderTag, ENOTIFICATIONPERSISTENTSERVICEPERSISTFOLDER)) {
          if (!l_pNotificationpersistentservicePersonalizationManager->NotificationpersistentserviceIsValidPersonality(l_tMsg.cUsername)) {
            FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
            return f_pSourceState;
          }
        }

        EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
        NC_LoadPersistedAck l_tMsgAck = {};
        HANDLE l_hMq = NULL;
        l_hMq = FrameworkunifiedMcOpenSender(l_pHApp, l_cMsgSource);

        if (NULL != l_hMq) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
          //// Check if  persistence has been disabled than  send positive acknowledgement to the application
          if (TRUE == CPersistenceManager::m_bPersistenceDisabled) {  // LCOV_EXCL_BR_LINE 6: custom command line options  // NOLINT[whitespace/line_length]
            // LCOV_EXCL_START 6: custom command line options
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            l_tMsgAck.eStatus = eFrameworkunifiedStatusOK;
#ifdef AGL_PosixBasedOS001LEGACY_USED
            strlcpy(l_tMsgAck.cTag, l_tMsg.cFolderTag, sizeof(l_tMsgAck.cTag));
#endif

            // send ack to requester
            if (eFrameworkunifiedStatusOK != (l_estatus = FrameworkunifiedSendMsg(l_hMq,
                                                        NPS_GET_PERS_FOLDER_ACK,
                                                        sizeof(l_tMsgAck), &l_tMsgAck))) {
              FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: FrameworkunifiedSendMsg returned l_estatus=%d", l_estatus);
            }
            // LCOV_EXCL_STOP
          } else {
            if (eFrameworkunifiedStatusOK != (l_estatus = l_pPersistenceManager->NotificationpersistentserviceLoad(l_cMsgSource,
                                                                            l_tMsg.cFolderTag,
                                                                            l_tMsg.cFolderPath,
                                                                            ENOTIFICATIONPERSISTENTSERVICEPERSISTFOLDER,
                                                                            l_tMsg.cUsername))) {
              FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error in Loading Persitence Folder :: %s", l_tMsg.cFolderTag);

              // return error to requester
              l_tMsgAck.eStatus = l_estatus;
              std::strncpy(l_tMsgAck.cTag, l_tMsg.cFolderTag, (MAX_PATH_LENGTH - 1));

              // send ack to requester
              if (eFrameworkunifiedStatusOK != (l_estatus = FrameworkunifiedSendMsg(l_hMq,  // LCOV_EXCL_BR_LINE 4: NSFW error case
                                                          NPS_GET_PERS_FOLDER_ACK,
                                                          sizeof(l_tMsgAck), &l_tMsgAck))) {
                // LCOV_EXCL_START 4: NSFW error case
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: FrameworkunifiedSendMsg returned l_estatus=%d", l_estatus);
                // LCOV_EXCL_STOP
              }
            }
          }
          FrameworkunifiedMcClose(l_hMq);
        } else {
          // LCOV_EXCL_START 4: NSFW error case
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                 "Can't send ack NPS_GET_PERS_FOLDER_ACK. McOpenSender failed for %s ", l_cMsgSource);
          // LCOV_EXCL_STOP
        }
      } else {
        // LCOV_EXCL_START 4: NSFW error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Can't load persistent folder. Invalid message size received.");
        // LCOV_EXCL_STOP
      }
    } else {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Can't load persistent folder. l_pNotificationManager is NULL");  // LCOV_EXCL_LINE 6: l_pPersistenceManager,  l_cMsgSource and l_pNotificationpersistentservicePersonalizationManager can't be NULL  // NOLINT[whitespace/line_length]
    }
  } catch (std::exception &e) {  // LCOV_EXCL_START 5: exception error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
    return NULL;
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return f_pSourceState;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceOnReleasePersistentFolder
/// The state machine executes this transition when event for releasing persistent folder tag is
/// received.
////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_INTERNALTRANSITION(NotificationpersistentserviceOnReleasePersistentFolder) {
  try {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");
    CHKNULL(f_pSourceState);

    // Check if  persistence has been disabled than return immediately
    if (TRUE == CPersistenceManager::m_bPersistenceDisabled) {  // LCOV_EXCL_BR_LINE 6: custom command line options
      // LCOV_EXCL_START 6: custom command line options
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
      return f_pSourceState;
      // LCOV_EXCL_STOP
    }

    NC_ReleasePersistentFolderMsg l_tMsg = {};

    HANDLE l_pHApp = f_pSourceState->FrameworkunifiedGetAppHandle();
    CHKNULL(l_pHApp);
    PCSTR l_cMsgSource = FrameworkunifiedGetMsgSrc(l_pHApp);

    CFrameworkunifiedHSM *l_pNotificationpersistentserviceHSM = FrameworkunifiedGetStateMachine(l_pHApp);
    CNSNPP *l_pnsnpp = static_cast<CNSNPP *>(l_pNotificationpersistentserviceHSM);

    // get the instance of persistence manager
    CPersistenceManager *l_pPersistenceManager = l_pnsnpp->m_pPersistenceManager;

    CnotificationpersistentservicePersonalizationManager *l_pNotificationpersistentservicePersonalizationManager = l_pnsnpp->m_pPersonalizationManager;

    if (NULL != l_pPersistenceManager && NULL != l_cMsgSource && NULL != l_pNotificationpersistentservicePersonalizationManager) {  // LCOV_EXCL_BR_LINE 6: l_pPersistenceManager,  l_cMsgSource and l_pNotificationpersistentservicePersonalizationManager can't be NULL  // NOLINT[whitespace/line_length]
      if (eFrameworkunifiedStatusOK == FrameworkunifiedGetMsgDataOfSize(l_pHApp, static_cast<PVOID>(&l_tMsg), sizeof(l_tMsg), eSMRRelease)) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
        // Check if the foldertag is registered as user specific persistence
        if (TRUE == l_pPersistenceManager->IsUserPersistence(l_tMsg.cFolderTag, ENOTIFICATIONPERSISTENTSERVICEPERSISTFOLDER)) {
          if (!l_pNotificationpersistentservicePersonalizationManager->NotificationpersistentserviceIsValidPersonality(l_tMsg.cUsername)) {
            FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
            return f_pSourceState;
          }
        }
        if (eFrameworkunifiedStatusOK != l_pPersistenceManager->NotificationpersistentserviceRelease(l_cMsgSource,
                                                              l_tMsg.cFolderTag,
                                                              l_tMsg.cFolderPath,
                                                              l_tMsg.eFrameworkunifiedReleaseType, // NOLINT (readability/naming)
                                                              ENOTIFICATIONPERSISTENTSERVICEPERSISTFOLDER,
                                                              l_tMsg.cUsername)) {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error in releasing persitence folder :: %s", l_tMsg.cFolderTag);
        }

        FRAMEWORKUNIFIEDLOG(ZONE_PRD_INFO2, __FUNCTION__, "Release Folder %s to tag %s for source %s",
               l_tMsg.cFolderPath, l_tMsg.cFolderTag, l_cMsgSource);
      } else {
        // LCOV_EXCL_START 4: NSFW error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Can't release persistent folder. Invalid message size received.");
        // LCOV_EXCL_STOP
      }
    } else {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Can't release persistent folder. l_pPersistenceManager is NULL");  // LCOV_EXCL_LINE 6: l_pPersistenceManager,  l_cMsgSource and l_pNotificationpersistentservicePersonalizationManager can't be NULL  // NOLINT[whitespace/line_length]
    }
  } catch (std::exception &e) {  // LCOV_EXCL_START 5: exception error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
    return NULL;
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return f_pSourceState;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceOnNPGetReadyStatus
///
////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_INTERNALTRANSITION(NotificationpersistentserviceOnNPGetReadyStatus) {
  try {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");

    CHKNULL(f_pSourceState);

    HANDLE l_pHApp = f_pSourceState->FrameworkunifiedGetAppHandle();
    CHKNULL(l_pHApp);

    PCSTR l_cMsgSource = FrameworkunifiedGetMsgSrc(l_pHApp);
    CHKNULL(l_cMsgSource);

    HANDLE l_hMq = FrameworkunifiedMcOpenSender(l_pHApp, l_cMsgSource);
    if (NULL != l_hMq) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
      EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

      // send ack to requester
      if (eFrameworkunifiedStatusOK == (l_estatus = FrameworkunifiedSendMsg(l_hMq, NPS_GET_READYSTATUS_ACK, 0, NULL))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
        FRAMEWORKUNIFIEDLOG(ZONE_PRD_INFO2, __PRETTY_FUNCTION__, "NPP ready status sent to %s.", l_cMsgSource);
      } else {
        // LCOV_EXCL_START 4: NSFW error case.
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __PRETTY_FUNCTION__, "Error sending NPS_GET_READYSTATUS_ACK to %s, status: %d", l_cMsgSource,
               l_estatus);
        // LCOV_EXCL_STOP
      }

      FrameworkunifiedMcClose(l_hMq);
      l_hMq = NULL;
    } else {
      // LCOV_EXCL_START 4: NSFW error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      // catastrophic failure!
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "McOpenSender failed for %s ", l_cMsgSource);
      // LCOV_EXCL_STOP
    }
  } catch (std::exception &e) {  // LCOV_EXCL_START 5: exception error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
    return NULL;
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return f_pSourceState;
}

IMPLEMENT_INTERNALTRANSITION(NotificationpersistentserviceOnNPRegisterImmediateNotifications) {
  try {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");
    EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

    CHKNULL(f_pSourceState);
    NC_register_multiple_immediate_notif_msg *l_pMsg = NULL;

    HANDLE l_pHApp = f_pSourceState->FrameworkunifiedGetAppHandle();
    CHKNULL(l_pHApp);

    CFrameworkunifiedHSM *l_pNotificationpersistentserviceHSM = FrameworkunifiedGetStateMachine(l_pHApp);
    CNSNPP *l_pnsnpp = static_cast<CNSNPP *>(l_pNotificationpersistentserviceHSM);

    // get app name of message source
    PCSTR l_cMsgSource = FrameworkunifiedGetMsgSrc(l_pHApp);
    CHKNULL(l_cMsgSource);

    // get instance of notification manager
    CNotificationManager *l_pNotificationManager = l_pnsnpp->m_pNotificationManager;

    if (NULL != l_pNotificationManager) {  // LCOV_EXCL_BR_LINE 6: l_pNotificationManager can't be NULL
      UI_32 l_uiMsgLength = FrameworkunifiedGetMsgLength(l_pHApp);

      if (l_uiMsgLength <= 0) {  // LCOV_EXCL_BR_LINE 6: l_uiMsgLength must be greater than 0
        // LCOV_EXCL_START 6: l_uiMsgLength must be greater than 0
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Can't register immediate persistence notification from source %s. Invalid message length.",
               l_cMsgSource);
        // LCOV_EXCL_STOP
      } else {
        std::vector<CHAR> l_vData = std::vector<CHAR>(l_uiMsgLength);

        // get the received data
        if (eFrameworkunifiedStatusOK == (l_estatus = FrameworkunifiedGetMsgDataOfSize(l_pHApp, &l_vData[0], static_cast<UI_32>(l_vData.size()), eSMRRelease))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
          l_pMsg = reinterpret_cast<NC_register_multiple_immediate_notif_msg *>(&l_vData[0]);

          if (NULL != l_pMsg) {  // LCOV_EXCL_BR_LINE 5: reinterpret_cast's error case.
            NC_register_immediate_notif_msg *l_pEventInfo = NULL;
            UI_32 l_uiCount;
            // register all the notifications
            for (l_uiCount = 0 ; l_uiCount < l_pMsg->numNotifications; ++l_uiCount) {
              l_pEventInfo = &l_pMsg->notifierList[l_uiCount];

              if (NULL != l_pEventInfo) {  // LCOV_EXCL_BR_LINE 6: l_pEventInfo can't be NULL
                FRAMEWORKUNIFIEDLOG(ZONE_PRD_INFO2, __FUNCTION__, "Register immediate persistence notification: "
                       "NotificationName[%d] = %s, data length = %d ",
                       l_uiCount, l_pEventInfo->notificationName, l_pEventInfo->maxLength);

                if (eFrameworkunifiedStatusOK != l_pNotificationManager->NotificationpersistentserviceServiceOnRegisterEvents(l_cMsgSource,
                                                                                       l_pEventInfo->notificationName,
                                                                                       l_pEventInfo->maxLength,
                                                                                       l_pEventInfo->persType,
                                                                                       l_pEventInfo->delay)) {
                  FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                         "Error in NotificationpersistentserviceServiceOnRegisterEvents :: %s", l_pEventInfo->notificationName);
                }
              } else {
                // LCOV_EXCL_START 6: l_pEventInfo can't be NULL
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                       "Can't register immediate persistence notification from source %s. l_pEventInfo is NULL",
                       l_cMsgSource);
                // LCOV_EXCL_STOP
              }
            }
          } else {
            // LCOV_EXCL_START 5: reinterpret_cast's error case.
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                   "Can't register immediate persistence notification from source %s. Invalid src mq",
                   l_cMsgSource);
            // LCOV_EXCL_STOP
          }
        } else {
          // LCOV_EXCL_START 4: NSFW error case.
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                 "Can't register immediate persistence notification from source %s, "
                 "Error getting message data, status: 0x%x",
                 l_cMsgSource, l_estatus);
          // LCOV_EXCL_STOP
        }
      }
    } else {
      // LCOV_EXCL_START 6: l_pNotificationManager can't be NULL
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Can't register immediate persistence notification from source %s. "
             "l_pNotificationManager is NULL", l_cMsgSource);
      // LCOV_EXCL_STOP
    }
  } catch (std::exception &e) {  // LCOV_EXCL_START 5: exception error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
    return NULL;
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return f_pSourceState;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceOnNPClearPersistedData
/// Deletes NPS Persistent Data and sends the ack back to requester
////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_INTERNALTRANSITION(NotificationpersistentserviceOnNPClearPersistedData) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusFail;

  // requeter MsgQ handle for sending Ack
  HANDLE hRequesterAck = NULL;

  // Application Handle
  HANDLE l_pHApp = NULL;

  // Received request data
  NC_ClearPersistedDataReq l_tMsg = {};

  // Ack structure
  NC_ClearPersisteDatadAck l_tMsgAck = {};

  // requester name
  PCSTR pRequester = NULL;

  try {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");

    l_pHApp = f_pSourceState->FrameworkunifiedGetAppHandle();
    CHKNULL(l_pHApp);

    CFrameworkunifiedHSM *l_pNotificationpersistentserviceHSM = FrameworkunifiedGetStateMachine(l_pHApp);
    CNSNPP *l_pnsnpp = static_cast<CNSNPP *>(l_pNotificationpersistentserviceHSM);
    CHKNULL(l_pnsnpp);

    pRequester = FrameworkunifiedGetMsgSrc(l_pHApp);
    CHKNULL(pRequester);

    if (sizeof(NC_ClearPersistedDataReq) == FrameworkunifiedGetMsgLength(l_pHApp)) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
      if (eFrameworkunifiedStatusOK == (l_estatus = FrameworkunifiedGetMsgDataOfSize(l_pHApp, (PVOID)(&l_tMsg), sizeof(NC_ClearPersistedDataReq), eSMRRelease))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
        FRAMEWORKUNIFIEDLOG(ZONE_PRD_INFO2, __FUNCTION__, "Request for deleting the persistent data of type %d received from %s",
               l_tMsg.ePersistenceData, pRequester);

        CPersistenceManager *l_pPersistenceManager = l_pnsnpp->m_pPersistenceManager;
        CHKNULL(l_pPersistenceManager);

        l_estatus = l_pPersistenceManager->ClearPersistenceData(l_tMsg.ePersistenceData);
      } else {
        // LCOV_EXCL_START 4: NSFW error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedGetMsgDataOfSize Error");
        // LCOV_EXCL_STOP
      }
    } else {
      // LCOV_EXCL_START 4: NSFW error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Can't delete persistent data. Invalid message size received.");
      // LCOV_EXCL_STOP
    }
  } catch (std::exception &e) {  // LCOV_EXCL_START 5: exception error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    f_pSourceState = NULL;
    l_estatus = eFrameworkunifiedStatusNullPointer;
    // LCOV_EXCL_STOP
  }

  // sending acknowledgement to the requester

  // create the requestor handle to send the ack
  hRequesterAck = FrameworkunifiedMcOpenSender(l_pHApp,
                                  pRequester);

  if (NULL != hRequesterAck) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
    // set the status
    l_tMsgAck.eStatus = l_estatus;

    // send NC_ClearPersisteDatadAck to the requester
    if (eFrameworkunifiedStatusOK != FrameworkunifiedSendMsg(hRequesterAck,  // LCOV_EXCL_BR_LINE 4: NSFW error case
                                   NPS_DELETE_PERSISTED_DATA_ACK,
                                   sizeof(l_tMsgAck),
                                   &l_tMsgAck)) {
      // LCOV_EXCL_START 4: NSFW error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error sending NPS_GET_PERS_FILE_ACK message to %s", pRequester);
      // LCOV_EXCL_STOP
    }

    // close handle
    FrameworkunifiedMcClose(hRequesterAck);
  } else {
    // LCOV_EXCL_START 4: NSFW error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "FrameworkunifiedMcOpenSender failed for %s ", pRequester);
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return f_pSourceState;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceOnNPSetDefaultPersistentData
/// Sets the default value of the persistent notification
////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_INTERNALTRANSITION(NotificationpersistentserviceOnNPSetDefaultPersistentData) {
  try {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");
    EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

    CHKNULL(f_pSourceState);

    HANDLE l_pHApp = f_pSourceState->FrameworkunifiedGetAppHandle();
    CHKNULL(l_pHApp);

    CFrameworkunifiedHSM *l_pNotificationpersistentserviceHSM = FrameworkunifiedGetStateMachine(l_pHApp);
    CHKNULL(l_pNotificationpersistentserviceHSM);

    CNSNPP *l_pnsnpp = static_cast<CNSNPP *>(l_pNotificationpersistentserviceHSM);
    CHKNULL(l_pnsnpp);

    PCSTR l_cMsgSource = FrameworkunifiedGetMsgSrc(l_pHApp);
    CHKNULL(l_cMsgSource);

    // get instance of notification manager
    CNotificationManager *l_pNotificationManager = l_pnsnpp->m_pNotificationManager;

    if (NULL != l_pNotificationManager) {  // LCOV_EXCL_BR_LINE 6: l_pNotificationManager can't be NULL
      UI_32 l_uiMsgLength = FrameworkunifiedGetMsgLength(l_pHApp);

      CHAR l_cNotificationName[MAX_SYS_INFO_SIZE] = {};

      // retrieve notification name
      FrameworkunifiedGetSystemInfo(l_pHApp, l_cNotificationName);

      if (0 != std::strlen(l_cNotificationName)) {  // LCOV_EXCL_BR_LINE 6: double check, l_cNotificationName can't be empty  // NOLINT[whitespace/line_length]
        FRAMEWORKUNIFIEDLOG(ZONE_PRD_INFO2, __FUNCTION__,
               "Set default value request for Notification:: %s from :: %s", l_cNotificationName,
               l_cMsgSource);

        if (0 != l_uiMsgLength) {
          std::vector<CHAR> pMsgData = std::vector<CHAR>(l_uiMsgLength);

          // get the received data
          if (eFrameworkunifiedStatusOK == (l_estatus = FrameworkunifiedGetMsgDataOfSize(l_pHApp, &pMsgData[0], static_cast<UI_32>(pMsgData.size()), eSMRRelease))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
            // set the default notification data
            if (eFrameworkunifiedStatusOK != l_pNotificationManager->NotificationpersistentserviceSetDefaultPersistentNotificationData(l_cNotificationName,
                                                                                                (PVOID)&pMsgData[0],
                                                                                                l_uiMsgLength)) {
              FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                     "Error in NotificationpersistentserviceSetDefaultPersistentNotificationData :: %s", l_cNotificationName);
            }
          } else {
            // LCOV_EXCL_START 4: NSFW error case
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                   "Unable to set default persistent notification data for %s, "
                   "Error retrieving data from message, status: 0x%x",
                   l_cNotificationName, l_estatus);
            // LCOV_EXCL_STOP
          }
        } else {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                 "Not setting default persistent notification data for %s, Data length received is %d",
                 l_cNotificationName, l_uiMsgLength);
        }
      } else {
        // LCOV_EXCL_START 6: double check, l_cNotificationName can't be empty
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Unable to Set DefaultPersistentNotificationData from source %s, Notification name is blank",
               l_cMsgSource);
        // LCOV_EXCL_STOP
      }
    } else {
      // LCOV_EXCL_START 6: l_pNotificationManager can't be NULL
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Can't set default value for notification from source %s, l_pNotificationManager is NULL", l_cMsgSource);
      // LCOV_EXCL_STOP
    }
  } catch (std::exception &e) {  // LCOV_EXCL_START 5: exception error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
    return NULL;
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return f_pSourceState;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceOnNPSetNotfnPersistentType
/// Sets the persist type of notification
////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_INTERNALTRANSITION(NotificationpersistentserviceOnNPSetNotfnPersistentType) {
  try {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");
    CHKNULL(f_pSourceState);

    HANDLE l_pHApp = f_pSourceState->FrameworkunifiedGetAppHandle();
    CHKNULL(l_pHApp);

    CFrameworkunifiedHSM *l_pNotificationpersistentserviceHSM = FrameworkunifiedGetStateMachine(l_pHApp);
    CHKNULL(l_pNotificationpersistentserviceHSM);

    CNSNPP *l_pnsnpp = static_cast<CNSNPP *>(l_pNotificationpersistentserviceHSM);
    CHKNULL(l_pnsnpp);

    EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

    CHAR l_cNotificationName[MAX_SYS_INFO_SIZE] = {};

    // retrieve notification name
    FrameworkunifiedGetSystemInfo(l_pHApp,
                     l_cNotificationName);

    // get instance of notification manager
    CNotificationManager *l_pNotificationManager = l_pnsnpp->m_pNotificationManager;

    if (NULL != l_pNotificationManager && 0 != std::strlen(l_cNotificationName)) {  // LCOV_EXCL_BR_LINE 6: l_pNotificationManager can't be NULL, l_cNotificationName can't be empty  // NOLINT[whitespace/line_length]
      FRAMEWORKUNIFIEDLOG(ZONE_PRD_INFO2, __FUNCTION__, "Set persist type request for Notification:: %s", l_cNotificationName);

      EFrameworkunifiedPersistCategory ePersistCategory = eFrameworkunifiedUserData;

      // get the received data
      if (eFrameworkunifiedStatusOK == (l_estatus = FrameworkunifiedGetMsgDataOfSize(l_pHApp, &ePersistCategory, sizeof(EFrameworkunifiedPersistCategory), eSMRRelease))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
        // set the notification persist type
        if (eFrameworkunifiedStatusOK != (l_estatus = l_pNotificationManager->NotificationpersistentserviceSetPersistentCategory(l_cNotificationName,  // LCOV_EXCL_BR_LINE 6: always return ok  // NOLINT[whitespace/line_length]
                                                                                          ePersistCategory))) {
          // LCOV_EXCL_START 6: always return ok
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error in Setting PersistentType for notification :: %s, Status: 0x%x",
                 l_cNotificationName, l_estatus);
          // LCOV_EXCL_STOP
        }
      } else {
        // LCOV_EXCL_START 4: NSFW error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Error in getting the PersistentType message data for notification :: %s, Status: 0x%x",
               l_cNotificationName, l_estatus);
        // LCOV_EXCL_STOP
      }
    } else {
      // LCOV_EXCL_START 6: l_pNotificationManager can't be NULL, l_cNotificationName can't be empty
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Can't set persist type for notification :: %s", l_cNotificationName);
      // LCOV_EXCL_STOP
    }
  } catch (std::exception &e) {  // LCOV_EXCL_START 5: exception error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
    return NULL;
    // LCOV_EXCL_STOP
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return f_pSourceState;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceOnNPSetFilePersistentType
/// Sets the persist type of file
////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_INTERNALTRANSITION(NotificationpersistentserviceOnNPSetFilePersistentType) {
  try {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");
    CHKNULL(f_pSourceState);

    HANDLE l_pHApp = f_pSourceState->FrameworkunifiedGetAppHandle();
    CHKNULL(l_pHApp);

    CFrameworkunifiedHSM *l_pNotificationpersistentserviceHSM = FrameworkunifiedGetStateMachine(l_pHApp);
    CHKNULL(l_pNotificationpersistentserviceHSM);

    CNSNPP *l_pnsnpp = static_cast<CNSNPP *>(l_pNotificationpersistentserviceHSM);
    CHKNULL(l_pnsnpp);

    PCSTR l_cMsgSource = FrameworkunifiedGetMsgSrc(l_pHApp);
    CHKNULL(l_cMsgSource);

    // get instance of persistence manager
    CPersistenceManager *l_pPersistenceManager = l_pnsnpp->m_pPersistenceManager;

    if (NULL != l_pPersistenceManager) {  // LCOV_EXCL_BR_LINE 6: l_pPersistenceManager can't be NULL
      NC_SetFilePersistType l_tMsg = {};

      // get the received data
      if (eFrameworkunifiedStatusOK == FrameworkunifiedGetMsgDataOfSize(l_pHApp, static_cast<PVOID>(&l_tMsg), sizeof(l_tMsg), eSMRRelease)) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
        if (0 != std::strlen(l_tMsg.cTag)) {  // LCOV_EXCL_BR_LINE 6: double check, l_tMsg.cTag can't be empty
          FRAMEWORKUNIFIEDLOG(ZONE_PRD_INFO2, __FUNCTION__,
                 "Set persist type request for file tag:: %s from %s", l_tMsg.cTag, l_cMsgSource);

          // set the file persist type
          if (eFrameworkunifiedStatusOK != l_pPersistenceManager->NotificationpersistentserviceSetPersistentCategory(l_cMsgSource,
                                                                              l_tMsg.cTag,
                                                                              l_tMsg.ePersistType,
                                                                              ENOTIFICATIONPERSISTENTSERVICEPERSISTFILE)) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                   "Error in Setting FilePersistentType for tag:: %s for request from:: %s", l_tMsg.cTag,
                   l_cMsgSource);
          }
        } else {
          // LCOV_EXCL_START 6: double check, l_tMsg.cTag can't be empty
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                 "set file persistent type request for NULL file tag from source:: %s", l_cMsgSource);
          // LCOV_EXCL_STOP
        }
      }
    } else {
      // LCOV_EXCL_START 6: l_pPersistenceManager can't be NULL
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Can't process request for setting persist type for file from source: %s, l_pPersistenceManager is NULL",
             l_cMsgSource);
      // LCOV_EXCL_STOP
    }
  } catch (std::exception &e) {  // LCOV_EXCL_START 5: exception error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
    return NULL;
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return f_pSourceState;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceOnNPSetFolderPersistentType
/// Sets the persist type of folder
////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_INTERNALTRANSITION(NotificationpersistentserviceOnNPSetFolderPersistentType) {
  try {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");
    CHKNULL(f_pSourceState);

    HANDLE l_pHApp = f_pSourceState->FrameworkunifiedGetAppHandle();
    CHKNULL(l_pHApp);

    CFrameworkunifiedHSM *l_pNotificationpersistentserviceHSM = FrameworkunifiedGetStateMachine(l_pHApp);
    CHKNULL(l_pNotificationpersistentserviceHSM);

    CNSNPP *l_pnsnpp = static_cast<CNSNPP *>(l_pNotificationpersistentserviceHSM);
    CHKNULL(l_pnsnpp);

    PCSTR l_cMsgSource = FrameworkunifiedGetMsgSrc(l_pHApp);
    CHKNULL(l_cMsgSource);

    // get instance of persistence manager
    CPersistenceManager *l_pPersistenceManager = l_pnsnpp->m_pPersistenceManager;

    if (NULL != l_pPersistenceManager) {  // LCOV_EXCL_BR_LINE 6: l_pPersistenceManager can't be NULL
      NC_SetFolderPersistType l_tMsg = {};

      // get the received data
      if (eFrameworkunifiedStatusOK == FrameworkunifiedGetMsgDataOfSize(l_pHApp, static_cast<PVOID>(&l_tMsg), sizeof(l_tMsg), eSMRRelease)) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
        if (0 != std::strlen(l_tMsg.cTag)) {  // LCOV_EXCL_BR_LINE 6: double check, l_tMsg.cTag can't be empty
          FRAMEWORKUNIFIEDLOG(ZONE_PRD_INFO2, __FUNCTION__, "Set persist type request for folder tag:: %s", l_tMsg.cTag);

          // set the file persist type
          if (eFrameworkunifiedStatusOK != l_pPersistenceManager->NotificationpersistentserviceSetPersistentCategory(l_cMsgSource,
                                                                              l_tMsg.cTag,
                                                                              l_tMsg.ePersistType,
                                                                              ENOTIFICATIONPERSISTENTSERVICEPERSISTFOLDER)) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                   "Error in Setting FolderPersistentType for tag:: %s for request from:: %s", l_tMsg.cTag,
                   l_cMsgSource);
          }
        } else {
          // LCOV_EXCL_START 6: double check, l_tMsg.cTag can't be empty
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                 "set file persistent type request for NULL folder tag from source:: %s", l_cMsgSource);
          // LCOV_EXCL_STOP
        }
      }
    } else {
      // LCOV_EXCL_START 6: l_pPersistenceManager can't be NULL
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Can't process request for setting persist type for folder from source: %s, l_pPersistenceManager is NULL",
             l_cMsgSource);
      // LCOV_EXCL_STOP
    }
  } catch (std::exception &e) {  // LCOV_EXCL_START 5: exception error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
    return NULL;
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return f_pSourceState;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceOnNPShutdown
/// Internal transition when NPP receives shutdown message
////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_INTERNALTRANSITION(NotificationpersistentserviceOnNPShutdown) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");
  try {
    CHKNULL(f_pSourceState);

    HANDLE l_pHApp = f_pSourceState->FrameworkunifiedGetAppHandle();
    CHKNULL(l_pHApp);

    NC_StopMsgData l_tStopMsgData = {};

    PCSTR l_cMsgSource = FrameworkunifiedGetMsgSrc(l_pHApp);
    CHKNULL(l_cMsgSource);

    if (eFrameworkunifiedStatusOK == FrameworkunifiedGetMsgDataOfSize(l_pHApp, static_cast<PVOID>(&l_tStopMsgData), sizeof(l_tStopMsgData), eSMRRelease)) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
      // make a transition to sNotificationpersistentserviceShutdownDataSave state and pass the message data through event
      CShutDownMsgData *l_pMsgData = new(std::nothrow) CShutDownMsgData(EVENT(evNPShutdownDataSave),
                                                                        l_tStopMsgData.eShutdownType,
                                                                        l_tStopMsgData.uiStopMsgData);
      CHKNULL(l_pMsgData);

      CEventDataPtr l_pData(l_pMsgData);
      f_pSourceState->FrameworkunifiedPostEvent(l_pData);

      FRAMEWORKUNIFIEDLOG(ZONE_PRD_INFO2, __FUNCTION__, "Shutdown Message received from %s with data: %d", l_cMsgSource,
             l_tStopMsgData.uiStopMsgData);
    } else {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Can't process shutdown request from:: %s, Invalid message data", l_cMsgSource);  // LCOV_EXCL_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
    }
  } catch (std::exception &e) {  // LCOV_EXCL_START 5: exception error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    f_pSourceState = NULL;
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return f_pSourceState;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NotificationpersistentserviceOnNPNorDataSaveAck
/// Internal transition when NPP receives shutdown ack message nor worker thread
////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_INTERNALTRANSITION(NotificationpersistentserviceOnNPNorDataSaveAck) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");
  try {
    // data received with ack
    UI_32 l_uiPersistCategoryFlag = 0;

    CHKNULL(f_pSourceState);

    HANDLE l_pHApp = f_pSourceState->FrameworkunifiedGetAppHandle();
    CHKNULL(l_pHApp);

    CFrameworkunifiedHSM *l_pNotificationpersistentserviceHSM = FrameworkunifiedGetStateMachine(l_pHApp);
    CHKNULL(l_pNotificationpersistentserviceHSM);

    CNSNPP *l_pnsnpp = static_cast<CNSNPP *>(l_pNotificationpersistentserviceHSM);
    CHKNULL(l_pnsnpp);

    PCSTR l_cMsgSource = FrameworkunifiedGetMsgSrc(l_pHApp);
    CHKNULL(l_cMsgSource);

    // get instance of persistence manager
    CPersistenceManager *l_pPersistenceManager = l_pnsnpp->m_pPersistenceManager;
    CHKNULL(l_pPersistenceManager);

    l_pPersistenceManager->SetImmediateDataPersistedStatus(TRUE);

    if (eFrameworkunifiedStatusOK == FrameworkunifiedGetMsgDataOfSize(l_pHApp, static_cast<PVOID>(&l_uiPersistCategoryFlag), sizeof(l_uiPersistCategoryFlag), eSMRRelease)) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
      FRAMEWORKUNIFIEDLOG(ZONE_PRD_INFO2, __FUNCTION__, "ShutdownAck received from %s with data: %d", l_cMsgSource,
             l_uiPersistCategoryFlag);
    }

    // depending on the flag delete the persisted data from persistent memory
    // which are not to be persisted during shutdown
    l_pnsnpp->DeletePersistedDataFolder(l_uiPersistCategoryFlag);

    if (TRUE == l_pPersistenceManager->HaveAllReleaseRequestsPersisted()) {  // LCOV_EXCL_BR_LINE 6: always return ok
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "All release requests are processed.");
      f_pSourceState->FrameworkunifiedPostEvent(EVENT(evIdle));
    }
  } catch (std::exception &e) {  // LCOV_EXCL_START 5: exception error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    f_pSourceState = NULL;
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return f_pSourceState;
}

CsNotificationpersistentservicePersistenceReady::CsNotificationpersistentservicePersistenceReady(std::string f_strName): CFrameworkunifiedLeafState(f_strName) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
}

CsNotificationpersistentservicePersistenceReady::~CsNotificationpersistentservicePersistenceReady() {  // LCOV_EXCL_START 200: cannot test code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus CsNotificationpersistentservicePersistenceReady::FrameworkunifiedOnEntry(CEventDataPtr f_peventdata) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");

  HANDLE l_pHApp = FrameworkunifiedGetAppHandle();

  if (NULL != l_pHApp) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
    HANDLE l_hMq = FrameworkunifiedMcOpenSender(l_pHApp, g_csendreadyackto);

    if (NULL != l_hMq) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
      EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

      // send ack to requester
      if (eFrameworkunifiedStatusOK == (l_estatus = FrameworkunifiedSendMsg(l_hMq, NPS_NPP_READY_EVENT, 0, NULL))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
        FRAMEWORKUNIFIEDLOG(ZONE_PRD_INFO2, __PRETTY_FUNCTION__, "NPP ready status sent to %s.", g_csendreadyackto);
      } else {
        // LCOV_EXCL_START 4: NSFW error case.
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __PRETTY_FUNCTION__, "Error sending NPS_NPP_READY_EVENT to %s, status: %d", g_csendreadyackto,
               l_estatus);
        // LCOV_EXCL_STOP
      }
      FrameworkunifiedMcClose(l_hMq);
      l_hMq = NULL;
    } else {
      // catastrophic failure!
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __PRETTY_FUNCTION__, "McOpenSender failed for %s ", g_csendreadyackto);  // LCOV_EXCL_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus CsNotificationpersistentservicePersistenceReady::FrameworkunifiedOnExit(CEventDataPtr f_peventdata) {  // LCOV_EXCL_START 200: cannot test code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return eFrameworkunifiedStatusOK;
  // LCOV_EXCL_STOP
}

#ifdef NPP_PROFILEINFO_ENABLE

IMPLEMENT_INTERNALTRANSITION(NotificationpersistentserviceOnNPProfileNotifications) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");
  try {
    CHKNULL(f_pSourceState);

    // get the application handle
    HANDLE l_pHApp = f_pSourceState->FrameworkunifiedGetAppHandle();
    CHKNULL(l_pHApp);

    // get the statemachine pointer of application
    CFrameworkunifiedHSM *l_pNotificationpersistentserviceHSM = FrameworkunifiedGetStateMachine(l_pHApp);
    CNSNPP *l_pnsnpp = static_cast<CNSNPP *>(l_pNotificationpersistentserviceHSM);
    CHKNULL(l_pnsnpp);

    // get the message source name
    PCSTR l_cMsgSource = FrameworkunifiedGetMsgSrc(l_pHApp);
    CHKNULL(l_cMsgSource);

    // get the message queue handle for sending the response
    HANDLE hSrcMqHandle = FrameworkunifiedMcOpenSender(l_pHApp, l_cMsgSource);
    CHKNULL(hSrcMqHandle);

    // get instance of notification manager
    CNotificationManager *l_pNotificationManager = l_pnsnpp->m_pNotificationManager;
    CHKNULL(l_pNotificationManager);

    std::string l_cNotificationProfileInfo = "";

    // get the data from notification manager
    if (eFrameworkunifiedStatusOK == l_pNotificationManager->GetNotificationProfilingData(l_cNotificationProfileInfo)) {
      UI_32 l_uiLength = l_cNotificationProfileInfo.size();

      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "FrameworkunifiedSendMsg NPS_PROFILE_NOTIFICATION_RESP Msg Length: %d", l_uiLength);

      PCHAR l_pData =  new CHAR[l_uiLength];
      std::memset(l_pData, '0', l_uiLength);

      std::strncpy(l_pData, l_cNotificationProfileInfo.c_str(), l_cNotificationProfileInfo.size());

      if (eFrameworkunifiedStatusOK != FrameworkunifiedSendMsg(hSrcMqHandle,
                                     NPS_PROFILE_NOTIFICATION_RESP,
                                     l_uiLength,
                                     l_pData)) {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__,
               "FrameworkunifiedSendMsg NPS_PROFILE_NOTIFICATION_RESP for Notification Profiling failed.");
      }

      delete[] l_pData;
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Failed to get Notification profiling data from NSNPP.");
    }
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    f_pSourceState = NULL;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return f_pSourceState;
}

IMPLEMENT_INTERNALTRANSITION(NotificationpersistentserviceOnNPProfilePersistence) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");
  try {
    CHKNULL(f_pSourceState);

    // get the application handle
    HANDLE l_pHApp = f_pSourceState->FrameworkunifiedGetAppHandle();
    CHKNULL(l_pHApp);

    // get the statemachine pointer of application
    CFrameworkunifiedHSM *l_pNotificationpersistentserviceHSM = FrameworkunifiedGetStateMachine(l_pHApp);
    CNSNPP *l_pnsnpp = static_cast<CNSNPP *>(l_pNotificationpersistentserviceHSM);
    CHKNULL(l_pnsnpp);

    // get the message source name
    PCSTR l_cMsgSource = FrameworkunifiedGetMsgSrc(l_pHApp);
    CHKNULL(l_cMsgSource);

    // get the message queue handle for sending the response
    HANDLE hSrcMqHandle = FrameworkunifiedMcOpenSender(l_pHApp, l_cMsgSource);
    CHKNULL(hSrcMqHandle);

    // get instance of persistence manager
    CPersistenceManager *l_pPersistenceManager = l_pnsnpp->m_pPersistenceManager;

    CHKNULL(l_pPersistenceManager);

    std::string l_cPersistenceProfileInfo = "";

    if (eFrameworkunifiedStatusOK == l_pPersistenceManager->GetPersistenceProfilingData(l_cPersistenceProfileInfo)) {
      UI_32 l_uiLength = l_cPersistenceProfileInfo.size();

      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "FrameworkunifiedSendMsg NPS_PROFILE_PERSISTENCE_RESP Msg Length: %d", l_uiLength);

      PCHAR l_pData =  new CHAR[l_uiLength];
      std::memset(l_pData, '0', l_uiLength);

      std::strncpy(l_pData, l_cPersistenceProfileInfo.c_str(), l_cPersistenceProfileInfo.size());

      if (eFrameworkunifiedStatusOK != FrameworkunifiedSendMsg(hSrcMqHandle,
                                     NPS_PROFILE_PERSISTENCE_RESP,
                                     l_uiLength,
                                     l_pData)) {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "FrameworkunifiedSendMsg NPS_PROFILE_PERSISTENCE_RESP for Persistence Profiling failed.");
      }

      delete[] l_pData;
    }
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    f_pSourceState = NULL;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return f_pSourceState;
}

#endif
